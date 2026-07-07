/*
 * CrossPeakTest: RECAL 3 0 / 3 1 cross peak. col0 = DAC base, rest = powers.
 * Peak index k on powers; DAC_peak = dac_base + k * Step.
 *
 *   CrossPeakTest.exe --mock-sweeps <file.csv>
 *   CrossPeakTest.exe --export-peak-csv <in.csv> [more.csv ...] [-o <dir>] [--utf8-bom]
 *
 * Legacy 5-param sweep CSV (e.g. comm_2026-05-25_recal_sweeps.csv): pre dual-base firmware;
 * ParseRecalSweepCmd maps single base to baseX/baseY best-effort. See *.LEGACY.txt beside file.
 */
#include "PeakFinder2D.h"
#include "Peak1DSweepRecenter.h"
#include "Peak1DSweepPipeline.h"
#include "PmRangeValidation.h"
#include "CalibPathOutcome.h"
#include "M576Peak1DConstants.h"
#include <windows.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>

using M576::Peak1DValidateCode;
using M576::SweepProfile;
using M576::SweepTrend;
using M576::AnalyzeRecal1DSweepProfile;
using M576::IsRetryablePeakFailure;
using M576::IsFlatSweepFailure;
using M576::IsMonotoneSweepFailure;
using M576::SuggestFlatRetryDacRange;
using M576::SuggestJumpMaxDacRange;
using M576::PlanRecalYCrossResweep;
using M576::AdjustProfileForMonoRecenter;
using M576::SuggestSweepRecenterDeltaDac;
using M576::SweepRecenterFailureInfo;
using M576::SweepTrendName;
using M576::SweepRecenterSessionState;
using M576::SweepRetryAction;
using M576::SweepRetryPlan;
using M576::InitSweepRecenterSessionState;
using M576::IsCoarsePeakHint;
using M576::IsCoarseExpandedInteriorPeak;
using M576::NeedsFineRefineAfterSuccess;
using M576::IsFineRefineSweepAttempt;
using M576::Peak1DFitPolicy;
using M576::Peak1DFitPolicyForCrossAxis;
using M576::Peak1DFitPolicyForSweepResult;
using M576::PlanNextRecal1DSweepAttempt;
using M576::PlanFineRefineAfterCoarseSuccess;
using M576::ParabolaVertexMax1D;
using M576::AdjustProfileForFlatAtMaxShift;
using M576::IsInteriorPeakHint;
using M576::SuggestFlatAtMaxShiftBase;
using M576::SuggestSweepRecenterNewBase;
using M576::DetectSweepRecenterOscillation;
using M576::ApplySweepRetryPlan;
using M576::FindUnimodalPeak1DIndex;
using M576::SweepRetryActionLogTag;
using M576::IsFineRefineSweepAttempt;
using M576::Recal1DSweepPipelineState;
using M576::Recal1DSweepCommand;
using M576::Recal1DPipelinePhase;
using M576::InitRecal1DSweepPipeline;
using M576::GetNextPipelineSweepCommand;
using M576::AdvanceRecal1DSweepPipeline;
using M576::StitchMovingBaseFromAnchor;
using M576::IsStitchLeft;
using M576::BuildPeakPipelineFailureReport;
using M576::PlanRecalYCrossResweepPipeline;

static bool ParseNumberLine(const std::string& line, std::vector<double>& out);

struct RecalSweepCmdFields
{
	bool ok = false;
	bool legacy5Param = false;
	int recalKind = 0;
	int sweepMode = -1;
	int baseX = 0;
	int baseY = 0;
	int offsetDac = 0;
	int stepDac = 0;
	int delayMs = 0;
};

static std::string FormatRecalSweepCmd(
	int recalKind, int sweepMode, int baseX, int baseY, int offsetDac, int stepDac, int delayMs)
{
	char buf[128];
	std::snprintf(buf, sizeof(buf), "RECAL %d %d %d %d %d %d %d",
		recalKind, sweepMode, baseX, baseY, offsetDac, stepDac, delayMs);
	return buf;
}

static const char* Peak1DCodeName(Peak1DValidateCode c)
{
	switch (c)
	{
	case Peak1DValidateCode::Ok: return "Ok";
	case Peak1DValidateCode::Empty: return "Empty";
	case Peak1DValidateCode::LowSpan: return "LowSpan";
	case Peak1DValidateCode::NotStrictLocal: return "NotStrictLocal";
	case Peak1DValidateCode::EdgeNotAllowed: return "EdgeNotAllowed";
	case Peak1DValidateCode::MultiLocalMax: return "MultiLocalMax";
	case Peak1DValidateCode::NotEnoughSamples: return "NotEnoughSamples";
	case Peak1DValidateCode::NotEnoughValidSamples: return "NotEnoughValidSamples";
	case Peak1DValidateCode::ParabolaNotDownward: return "ParabolaNotDownward";
	case Peak1DValidateCode::ParabolaFitSingular: return "ParabolaFitSingular";
	case Peak1DValidateCode::VertexOutOfRange: return "VertexOutOfRange";
	case Peak1DValidateCode::PmRangeMismatch: return "PmRangeMismatch";
	default: return "?";
	}
}

static Peak1DValidateCode Peak1DCodeFromCsvName(const std::string& name)
{
	if (name == "Ok") return Peak1DValidateCode::Ok;
	if (name == "Empty") return Peak1DValidateCode::Empty;
	if (name == "LowSpan") return Peak1DValidateCode::LowSpan;
	if (name == "NotStrictLocal") return Peak1DValidateCode::NotStrictLocal;
	if (name == "EdgeNotAllowed") return Peak1DValidateCode::EdgeNotAllowed;
	if (name == "MultiLocalMax") return Peak1DValidateCode::MultiLocalMax;
	if (name == "NotEnoughSamples") return Peak1DValidateCode::NotEnoughSamples;
	if (name == "NotEnoughValidSamples") return Peak1DValidateCode::NotEnoughValidSamples;
	if (name == "ParabolaNotDownward") return Peak1DValidateCode::ParabolaNotDownward;
	if (name == "ParabolaFitSingular") return Peak1DValidateCode::ParabolaFitSingular;
	if (name == "VertexOutOfRange") return Peak1DValidateCode::VertexOutOfRange;
	if (name == "PmRangeMismatch") return Peak1DValidateCode::PmRangeMismatch;
	return Peak1DValidateCode::Empty;
}

static std::string ExtractStepTagFromPathField(const std::string& pathField)
{
	const std::string key = "step=";
	const size_t p = pathField.find(key);
	if (p == std::string::npos)
		return pathField;
	const size_t start = p + key.size();
	const size_t end = pathField.find('|', start);
	if (end == std::string::npos)
		return pathField.substr(start);
	return pathField.substr(start, end - start);
}

static double RecalSweepCenterFromCol0Diag(double col0, int halfRange)
{
	return col0 + (double)halfRange;
}

/// Replay comm CSV row through fit + planner; explain why VertexOutOfRange did not retry.
static std::string ExplainVorNoRetryReason(
	Peak1DValidateCode code,
	const SweepProfile& profile,
	int sampleCount,
	bool afterExpand,
	const std::vector<double>& powSamples,
	const SweepRecenterFailureInfo& failure,
	const SweepRecenterSessionState& session,
	const SweepRetryPlan& plan,
	bool lastAttempt)
{
	std::ostringstream o;
	if (lastAttempt)
		o << "lastAttempt; ";
	if (profile.trend == SweepTrend::Flat)
	{
		o << "trend=Flat=>IsRetryable=false; ";
		if (code == Peak1DValidateCode::VertexOutOfRange && IsFlatSweepFailure(code, profile))
			o << "VOR+Flat IsFlatSweepFailure=>JumpFlatMax or fallback; ";
	}
	else if (code == Peak1DValidateCode::VertexOutOfRange)
	{
		const int n = sampleCount;
		const bool edgeArgmax = (profile.argmaxIndex == 0 || profile.argmaxIndex == (n - 1));
		if (edgeArgmax)
			o << "VOR edge_argmax=>baseRetryable; ";
		else if (failure.hasTPeak && std::isfinite(failure.tPeak) && failure.tPeak < 0.0)
		{
			if (profile.argmaxIndex <= n / 3)
				o << "VOR t*<0 left_flank argmax<=" << (n / 3) << "=>baseRetryable; ";
			else
				o << "VOR t*<0 interior_argmax=" << profile.argmaxIndex << "=>NOT baseRetryable; ";
		}
		else if (failure.hasTPeak && std::isfinite(failure.tPeak) && failure.tPeak > (double)(n - 1))
		{
			if (profile.argmaxIndex >= (n * 2) / 3 || profile.argmaxIndex <= n / 3)
				o << "VOR t*>n-1 flank argmax=>baseRetryable; ";
			else
				o << "VOR t*>n-1 interior_argmax=" << profile.argmaxIndex << "=>NOT baseRetryable; ";
		}
		else
			o << "VOR no outside t* interior_argmax=" << profile.argmaxIndex << "=>NOT baseRetryable; ";
		if (!afterExpand)
			o << "afterExpand=0=>no tail-mono fallback; ";
		else if (!IsRetryablePeakFailure(code, profile, n, afterExpand, &powSamples, &failure))
			o << "afterExpand=1 but tail/argmax fallback still false; ";
	}
	if (!IsFlatSweepFailure(code, profile) && !session.flatJumpedToMax)
		o << "not FlatJump; ";
	if (IsMonotoneSweepFailure(code, profile, sampleCount) && !session.inCoarsePhase)
		o << "monoFail eligible; ";
	if (IsCoarsePeakHint(code, profile, sampleCount, &failure) && !session.fineConsumed)
		o << "coarseHint eligible; ";
	if (plan.action == SweepRetryAction::GiveUp)
		o << "planner=GiveUp";
	else
		o << "planner=" << SweepRetryActionLogTag(plan.action) << " (INV-12 fallback if no specialized branch)";
	return o.str();
}

/// Assert planner never GiveUp before lastAttempt (except PmRangeMismatch).
static int RunMaxAttemptsFallbackSelfTests()
{
	int fail = 0;
	const int uiFine = 64;

	{
		// comm_2026-07-02 step260 att1: VOR @offset=64; unified 0.3 dB flat gate => NonMono (not Flat) => ShiftOnly, not JumpFlatMax.
		static const double step260Att1[] = {
			-247791, -247787, -247818, -247830, -247880, -247919, -247958, -248046, -248112, -248214,
			-248298, -248436, -248553, -248695, -248880, -249045, -249225, -249413, -249644, -249893,
			-250132, -250401, -250701, -251012, -251334, -251667, -252031, -252407, -252837, -253266,
			-253742, -254197, -254726,
		};
		const int n = (int)(sizeof(step260Att1) / sizeof(step260Att1[0]));
		std::vector<double> pow(step260Att1, step260Att1 + n);
		const SweepProfile prof = AnalyzeRecal1DSweepProfile(pow);
		SweepRecenterSessionState st = {};
		InitSweepRecenterSessionState(st, uiFine, 9999);
		SweepRecenterFailureInfo fi = {};
		fi.code = Peak1DValidateCode::VertexOutOfRange;
		fi.tPeak = -0.5;
		fi.hasTPeak = true;
		if (prof.trend == SweepTrend::Flat)
		{
			std::fprintf(stderr, "self-test: step260 att1 fixture must not be Flat under 0.3 dB gate\n");
			++fail;
		}
		if (IsFlatSweepFailure(Peak1DValidateCode::VertexOutOfRange, prof))
		{
			std::fprintf(stderr, "self-test: step260 att1 VOR must not be IsFlatSweepFailure (trend=%s)\n",
				SweepTrendName(prof.trend));
			++fail;
		}
		const SweepRetryPlan plan = PlanNextRecal1DSweepAttempt(
			st, Peak1DValidateCode::VertexOutOfRange, prof, pow, 305.0 + 64.0, 0, false, fi);
		if (plan.action == SweepRetryAction::GiveUp)
		{
			std::fprintf(stderr, "self-test: step260 att1 VOR+NonMono must not GiveUp before max attempts\n");
			++fail;
		}
		if (plan.action != SweepRetryAction::ShiftOnly || plan.nextRange != uiFine)
		{
			std::fprintf(stderr,
				"self-test: step260 att1 must ShiftOnly@%d (action=%d range=%d)\n",
				uiFine, (int)plan.action, plan.nextRange);
			++fail;
		}
	}

	{
		// comm_2026-07-02 step259 att5: VOR+Flat after fine refine must FlatAtMaxShift, not GiveUp.
		static const double step259Att5[] = {
			-255372, -255376, -255378, -255363, -255355, -255367, -255374, -255381, -255390, -255385,
			-255392, -255378, -255377, -255391, -255384, -255400, -255395, -255389, -255387, -255389,
			-255399, -255406, -255390, -255399, -255439, -255484, -255444, -255421, -255419, -255401,
			-255403, -255412, -255403,
		};
		const int n = (int)(sizeof(step259Att5) / sizeof(step259Att5[0]));
		std::vector<double> pow(step259Att5, step259Att5 + n);
		const SweepProfile prof = AnalyzeRecal1DSweepProfile(pow);
		SweepRecenterSessionState st = {};
		InitSweepRecenterSessionState(st, uiFine, -35);
		st.flatJumpedToMax = true;
		st.fineConsumed = true;
		st.attemptRange = uiFine;
		SweepRecenterFailureInfo fi = {};
		fi.code = Peak1DValidateCode::VertexOutOfRange;
		fi.tPeak = -0.8447;
		fi.hasTPeak = true;
		fi.hasPrevAttempt = true;
		const SweepRetryPlan plan = PlanNextRecal1DSweepAttempt(
			st, Peak1DValidateCode::VertexOutOfRange, prof, pow, -99.0 + 64.0, 4, false, fi);
		if (plan.action == SweepRetryAction::GiveUp)
		{
			std::fprintf(stderr, "self-test: step259 att5 VOR+Flat post-fine must not GiveUp\n");
			++fail;
		}
		if (plan.action != SweepRetryAction::FlatAtMaxShift)
		{
			std::fprintf(stderr,
				"self-test: step259 att5 post-fine fallback must FlatAtMaxShift (action=%d)\n",
				(int)plan.action);
			++fail;
		}
	}

	{
		std::vector<double> p((size_t)33, -130000.0);
		const SweepProfile prof = AnalyzeRecal1DSweepProfile(p);
		SweepRecenterSessionState st = {};
		InitSweepRecenterSessionState(st, uiFine, 9999);
		SweepRecenterFailureInfo fi = {};
		fi.code = Peak1DValidateCode::PmRangeMismatch;
		const SweepRetryPlan plan = PlanNextRecal1DSweepAttempt(
			st, Peak1DValidateCode::PmRangeMismatch, prof, p, 0.0, 0, false, fi);
		if (plan.action != SweepRetryAction::GiveUp)
		{
			std::fprintf(stderr, "self-test: PmRangeMismatch must GiveUp immediately\n");
			++fail;
		}
	}

	{
		std::vector<double> strictDec((size_t)33);
		for (int i = 0; i < 33; ++i)
			strictDec[(size_t)i] = -130000.0 - i * 3000.0;
		const SweepProfile p = AnalyzeRecal1DSweepProfile(strictDec);
		SweepRecenterSessionState st = {};
		InitSweepRecenterSessionState(st, uiFine, 9999);
		SweepRecenterFailureInfo fi = {};
		fi.code = Peak1DValidateCode::ParabolaNotDownward;
		const SweepRetryPlan plan = PlanNextRecal1DSweepAttempt(
			st, Peak1DValidateCode::ParabolaNotDownward, p, strictDec, 0.0, 11, true, fi);
		if (plan.action != SweepRetryAction::GiveUp)
		{
			std::fprintf(stderr, "self-test: lastAttempt must GiveUp\n");
			++fail;
		}
	}

	return fail;
}

static void PrintFitTraceStdout(const char* stageTag, const M576::Peak1DFitTrace& tr)
{
	if (tr.globalMaxIndex >= 0)
		std::printf("  %s globalMax: idx=%d P=%.12g\n", stageTag, tr.globalMaxIndex, tr.globalMaxY);
	const size_t nf = tr.fitIndex.size();
	if (nf == 0 || tr.fitY.size() != nf)
	{
		std::printf("  %s fitPoints: (none after preprocess)\n", stageTag);
		return;
	}
	const int head = (int)M576_PEAK1D_LOG_FIT_FIRST;
	const int tail = (int)M576_PEAK1D_LOG_FIT_LAST;
	std::printf("  %s fitPoints (%zu): ", stageTag, nf);
	if ((int)nf <= head + tail)
	{
		for (size_t k = 0; k < nf; ++k)
			std::printf("%s[%d]=%.8g", k ? ";" : "", tr.fitIndex[k], tr.fitY[k]);
	}
	else
	{
		for (int k = 0; k < head; ++k)
			std::printf("%s[%d]=%.8g", k ? ";" : "", tr.fitIndex[(size_t)k], tr.fitY[(size_t)k]);
		std::printf(" ... ");
		for (size_t k = nf - (size_t)tail; k < nf; ++k)
			std::printf(";[%d]=%.8g", tr.fitIndex[k], tr.fitY[k]);
	}
	std::printf("\n");
}

/// 每行: col0=dac_base, 其余为 P1..Pn（与 RECAL 扫频行一致）。跑 ParabolaVertexMax1D + Peak1DFitTrace，打印与上位机日志等价的摘要。
static int RunMockSweepLinesFile(const char* path)
{
	std::ifstream f(path, std::ios::binary);
	if (!f)
	{
		std::fprintf(stderr, "mock-sweeps: cannot open: %s\n", path);
		return 1;
	}
	std::string line;
	int lineNo = 0;
	int nFail = 0;
	while (std::getline(f, line))
	{
		++lineNo;
		while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ' || line.back() == '\t'))
			line.pop_back();
		if (line.empty())
			continue;
		std::vector<double> nums;
		if (!ParseNumberLine(line, nums) || nums.size() < 1 + (size_t)M576_PEAK1D_CUBIC_MIN_SAMPLES)
		{
			std::printf("--- line %d: skip (need dac + >=%d powers, got %zu)\n",
				lineNo, (int)M576_PEAK1D_CUBIC_MIN_SAMPLES, nums.size());
			continue;
		}
		const double dac0 = nums[0];
		std::vector<double> powers(nums.begin() + 1, nums.end());
		std::printf("=== line %d dac_base=%.17g n=%zu ===\n", lineNo, dac0, powers.size());
		M576::Peak1DFitTrace tr;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		const bool ok = M576::ParabolaVertexMax1D(powers, t, c, &tr);
		int idx = 0;
		if (ok)
			idx = (int)std::lround(t);
		std::printf("  ParabolaVertexMax1D: %s  t*=%.8g  lround(idx)=%d  code=%s\n",
			ok ? "OK" : "FAIL", t, idx, Peak1DCodeName(c));
		PrintFitTraceStdout("mock", tr);
		if (!ok)
			++nFail;
	}
	std::printf("mock-sweeps: processed file, lines with FAIL=%d\n", nFail);
	return nFail > 0 ? 1 : 0;
}

struct SweepRow
{
	double dac_base = 0.0;
	std::vector<double> powers;
};

static std::string PathToExeDirFile(const char* filename)
{
	char module[MAX_PATH];
	DWORD n = GetModuleFileNameA(nullptr, module, MAX_PATH);
	if (n == 0 || n >= MAX_PATH)
		return {};
	char* slash = strrchr(module, '\\');
	if (!slash)
		return {};
	*slash = '\0';
	std::string p(module);
	p += '\\';
	p += filename;
	return p;
}

static std::string Utf16LeBlobToUtf8(const unsigned char* data, size_t len)
{
	if (len < 2 || (len % 2) != 0)
		return {};
	size_t nch = len / 2;
	std::wstring w(nch, L'\0');
	std::memcpy(w.data(), data, len);
	int need = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
	if (need <= 0)
		return {};
	std::string out((size_t)need, '\0');
	WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), &out[0], need, nullptr, nullptr);
	return out;
}

static bool LooksLikeUtf16LeNoBom(const std::vector<char>& raw)
{
	if (raw.size() < 6)
		return false;
	if ((unsigned char)raw[0] == 0xEF && raw.size() >= 3 && (unsigned char)raw[1] == 0xBB && (unsigned char)raw[2] == 0xBF)
		return false;
	if ((unsigned char)raw[0] == 0xFF && (unsigned char)raw[1] == 0xFE)
		return false;
	if ((unsigned char)raw[0] == 0xFE && (unsigned char)raw[1] == 0xFF)
		return false;
	int asciiPairs = 0;
	for (size_t i = 0; i + 1 < raw.size() && i < 64; i += 2)
	{
		unsigned char c = (unsigned char)raw[i];
		unsigned char hi = (unsigned char)raw[i + 1];
		if (hi != 0)
			return false;
		if (c >= 0x20 && c <= 0x7E)
			asciiPairs++;
		else if (c == '\r' || c == '\n' || c == '\t' || c == ',' || c == '-' || c == '.' || (c >= '0' && c <= '9'))
			asciiPairs++;
		else
			return false;
	}
	return asciiPairs >= 2;
}

static bool ParseNumberLine(const std::string& line, std::vector<double>& out)
{
	out.clear();
	std::string t;
	t.reserve(line.size());
	for (char c : line)
	{
		if (c == ',' || c == ';' || c == '\t')
			t += ' ';
		else
			t += c;
	}
	std::istringstream iss(t);
	double v = 0.0;
	while (iss >> v)
		out.push_back(v);
	return !out.empty();
}

static bool ParseSweepRow(const std::vector<double>& nums, SweepRow& row, std::string& err)
{
	if (nums.size() < 2)
	{
		err = "each line needs DAC base + at least one power column";
		return false;
	}
	row.dac_base = nums[0];
	row.powers.assign(nums.begin() + 1, nums.end());
	return true;
}

static void TrimInPlace(std::string& s)
{
	while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n'))
		s.pop_back();
	size_t i = 0;
	while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
		++i;
	if (i > 0)
		s.erase(0, i);
}

static bool ReadTextFileUtf8(const char* path, std::string& text, std::string& err)
{
	std::vector<char> raw;
	{
		std::ifstream f(path, std::ios::binary);
		if (!f)
		{
			err = "cannot open: ";
			err += path;
			return false;
		}
		raw.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	}
	if (raw.size() >= 2 && (unsigned char)raw[0] == 0xFF && (unsigned char)raw[1] == 0xFE)
		text = Utf16LeBlobToUtf8((const unsigned char*)raw.data() + 2, raw.size() - 2);
	else if (LooksLikeUtf16LeNoBom(raw))
		text = Utf16LeBlobToUtf8((const unsigned char*)raw.data(), raw.size());
	else if (raw.size() >= 3 && (unsigned char)raw[0] == 0xEF && (unsigned char)raw[1] == 0xBB && (unsigned char)raw[2] == 0xBF)
		text.assign(raw.begin() + 3, raw.end());
	else
		text.assign(raw.begin(), raw.end());
	if (text.empty() && !raw.empty())
	{
		err = "UTF-16 read failed: ";
		err += path;
		return false;
	}
	return true;
}

static bool ParseRecalSweepCmd(const std::string& cmd, RecalSweepCmdFields& out)
{
	out = RecalSweepCmdFields();
	int k = 0, mode = 0, v3 = 0, v4 = 0, v5 = 0, v6 = 0, v7 = 0;
	if (std::sscanf(cmd.c_str(), "RECAL %d %d %d %d %d %d %d", &k, &mode, &v3, &v4, &v5, &v6, &v7) == 7)
	{
		if ((k != 3 && k != 5) || (mode != 0 && mode != 1))
			return false;
		out.ok = true;
		out.recalKind = k;
		out.sweepMode = mode;
		out.baseX = v3;
		out.baseY = v4;
		out.offsetDac = v5;
		out.stepDac = v6;
		out.delayMs = v7;
		return true;
	}
	// Legacy 5-param (single base): comm_2026-05-25_recal_sweeps.csv and older FW logs.
	if (std::sscanf(cmd.c_str(), "RECAL %d %d %d %d %d %d", &k, &mode, &v3, &v4, &v5, &v6) == 6)
	{
		if ((k != 3 && k != 5) || (mode != 0 && mode != 1))
			return false;
		out.ok = true;
		out.legacy5Param = true;
		out.recalKind = k;
		out.sweepMode = mode;
		out.offsetDac = v4;
		out.stepDac = v5;
		out.delayMs = v6;
		if (mode == 0)
		{
			out.baseX = 9999;
			out.baseY = v3;
		}
		else
		{
			out.baseY = v3;
			out.baseX = 9999;
		}
		return true;
	}
	return false;
}

static bool SplitCsvFieldsSimple(const std::string& line, std::vector<std::string>& fields)
{
	fields.clear();
	std::string cur;
	bool inQuote = false;
	for (size_t i = 0; i < line.size(); ++i)
	{
		const char c = line[i];
		if (inQuote)
		{
			if (c == '"')
			{
				if (i + 1 < line.size() && line[i + 1] == '"')
				{
					cur += '"';
					++i;
				}
				else
					inQuote = false;
			}
			else
				cur += c;
		}
		else if (c == '"')
			inQuote = true;
		else if (c == ',')
		{
			fields.push_back(cur);
			cur.clear();
		}
		else
			cur += c;
	}
	fields.push_back(cur);
	return !fields.empty();
}

static bool SplitSweepCsvRow(const std::string& line, std::string& cmd, std::vector<double>& nums)
{
	cmd.clear();
	nums.clear();
	std::vector<std::string> fields;
	if (!SplitCsvFieldsSimple(line, fields))
		return false;
	for (std::string& f : fields)
		TrimInPlace(f);
	if (fields.empty())
		return false;

	size_t cmdIdx = 0;
	const std::string& first = fields[0];
	if (first.find("|slot=") != std::string::npos
		|| (first.rfind("RECAL", 0) != 0 && fields.size() >= 2))
		cmdIdx = 1;
	if (cmdIdx >= fields.size())
		return false;
	cmd = fields[cmdIdx];
	TrimInPlace(cmd);
	if (cmd.empty())
		return false;

	size_t numStart = cmdIdx + 1;
	if (cmdIdx == 1 && fields.size() >= cmdIdx + 4)
		numStart = cmdIdx + 4;

	for (size_t i = numStart; i < fields.size(); ++i)
	{
		if (fields[i].empty())
			continue;
		try
		{
			nums.push_back(std::stod(fields[i]));
		}
		catch (...)
		{
			return false;
		}
	}
	return !nums.empty();
}

static std::string EscapeCsvField(const std::string& s)
{
	bool needQuote = false;
	for (char c : s)
	{
		if (c == ',' || c == '"' || c == '\r' || c == '\n')
		{
			needQuote = true;
			break;
		}
	}
	if (!needQuote)
		return s;
	std::string o = "\"";
	for (char c : s)
	{
		if (c == '"')
			o += "\"\"";
		else
			o += c;
	}
	o += '"';
	return o;
}

static void AppendCsvField(std::ostringstream& row, const std::string& field, bool& first)
{
	if (!first)
		row << ',';
	first = false;
	row << EscapeCsvField(field);
}

static std::string FormatFitPointsCsv(const M576::Peak1DFitTrace& tr)
{
	const size_t nf = tr.fitIndex.size();
	if (nf == 0 || tr.fitY.size() != nf)
		return {};
	std::ostringstream oss;
	for (size_t k = 0; k < nf; ++k)
	{
		if (k)
			oss << ';';
		oss << '[' << tr.fitIndex[k] << "]=" << tr.fitY[k];
	}
	return oss.str();
}

static std::string PathFileName(const std::string& path)
{
	const size_t s1 = path.find_last_of('\\');
	const size_t s2 = path.find_last_of('/');
	const size_t pos = (s1 == std::string::npos) ? s2 : (s2 == std::string::npos ? s1 : (std::max)(s1, s2));
	if (pos == std::string::npos)
		return path;
	return path.substr(pos + 1);
}

static std::string PathDirName(const std::string& path)
{
	const size_t s1 = path.find_last_of('\\');
	const size_t s2 = path.find_last_of('/');
	const size_t pos = (s1 == std::string::npos) ? s2 : (s2 == std::string::npos ? s1 : (std::max)(s1, s2));
	if (pos == std::string::npos)
		return {};
	return path.substr(0, pos + 1);
}

static std::string StemBeforeExtension(const std::string& filename)
{
	const size_t dot = filename.find_last_of('.');
	if (dot == std::string::npos || dot == 0)
		return filename;
	return filename.substr(0, dot);
}

static std::string MakePeakAnalysisOutPath(const char* inPath, const char* outDir)
{
	const std::string in(inPath);
	const std::string base = PathFileName(in);
	const std::string stem = StemBeforeExtension(base);
	const std::string outName = stem + "_peak_analysis.csv";
	if (outDir && outDir[0])
	{
		std::string d(outDir);
		if (d.back() != '\\' && d.back() != '/')
			d += '\\';
		return d + outName;
	}
	const std::string dir = PathDirName(in);
	return dir.empty() ? outName : dir + outName;
}

/// Returns negative on IO error; else count of rows with fit_ok=0.
static int RunExportPeakCsv(const char* inPath, const char* outPath, bool utf8Bom)
{
	std::string text, err;
	if (!ReadTextFileUtf8(inPath, text, err))
	{
		std::fprintf(stderr, "export-peak-csv: %s\n", err.c_str());
		return -1;
	}

	std::ofstream out(outPath, std::ios::binary);
	if (!out)
	{
		std::fprintf(stderr, "export-peak-csv: cannot write: %s\n", outPath);
		return -1;
	}
	if (utf8Bom)
	{
		const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
		out.write((const char*)bom, 3);
	}

	static const char* kHdr =
		"line_no,cmd,sweep_mode,base_x,base_y,legacy_5param,offset_dac,step_dac,delay_ms,dac_base,n_powers,"
		"fit_ok,validate_code,span,trend,global_max_idx,global_max_y,t_star,peak_idx,dac_peak,fit_n,fit_points\n";
	out << kHdr;

	int lineNo = 0;
	int nFail = 0;
	int nData = 0;
	std::istringstream iss(text);
	std::string line;
	while (std::getline(iss, line))
	{
		++lineNo;
		while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
			line.pop_back();
		if (line.empty())
			continue;
		if (lineNo == 1)
		{
			std::string lower = line;
			for (char& c : lower)
				if (c >= 'A' && c <= 'Z')
					c = (char)(c - 'A' + 'a');
			if (lower.find("cmd") != std::string::npos || lower.find("path") != std::string::npos)
				continue;
		}

		std::string cmd;
		std::vector<double> nums;
		if (!SplitSweepCsvRow(line, cmd, nums))
		{
			std::ostringstream row;
			bool first = true;
			AppendCsvField(row, std::to_string(lineNo), first);
			AppendCsvField(row, line, first);
			for (int k = 0; k < 19; ++k)
				AppendCsvField(row, "", first);
			AppendCsvField(row, "0", first);
			AppendCsvField(row, "Empty", first);
			for (int k = 0; k < 9; ++k)
				AppendCsvField(row, "", first);
			AppendCsvField(row, "0", first);
			AppendCsvField(row, "", first);
			out << row.str() << '\n';
			++nFail;
			continue;
		}

		RecalSweepCmdFields cf;
		ParseRecalSweepCmd(cmd, cf);

		double dacBase = 0;
		std::vector<double> powers;
		if (!nums.empty())
		{
			dacBase = nums[0];
			if (nums.size() > 1)
				powers.assign(nums.begin() + 1, nums.end());
		}

		M576::Peak1DFitTrace tr;
		double tStar = 0;
		Peak1DValidateCode code = Peak1DValidateCode::Ok;
		const bool fitOk = !powers.empty()
			&& M576::ParabolaVertexMax1D(powers, tStar, code, &tr);
		const SweepProfile prof = AnalyzeRecal1DSweepProfile(powers);

		int peakIdx = 0;
		if (fitOk)
			peakIdx = (int)std::lround(tStar);

		const double stepDac = cf.ok ? (double)cf.stepDac : 0.0;
		const double dacPeak = dacBase + tStar * stepDac;

		std::ostringstream row;
		bool first = true;
		AppendCsvField(row, std::to_string(lineNo), first);
		AppendCsvField(row, cmd, first);
		AppendCsvField(row, cf.ok ? std::to_string(cf.sweepMode) : "", first);
		AppendCsvField(row, cf.ok ? std::to_string(cf.baseX) : "", first);
		AppendCsvField(row, cf.ok ? std::to_string(cf.baseY) : "", first);
		AppendCsvField(row, cf.ok && cf.legacy5Param ? "1" : "0", first);
		AppendCsvField(row, cf.ok ? std::to_string(cf.offsetDac) : "", first);
		AppendCsvField(row, cf.ok ? std::to_string(cf.stepDac) : "", first);
		AppendCsvField(row, cf.ok ? std::to_string(cf.delayMs) : "", first);
		AppendCsvField(row, std::to_string(dacBase), first);
		AppendCsvField(row, std::to_string(powers.size()), first);
		AppendCsvField(row, fitOk ? "1" : "0", first);
		AppendCsvField(row, Peak1DCodeName(code), first);
		AppendCsvField(row, std::to_string(prof.span), first);
		AppendCsvField(row, SweepTrendName(prof.trend), first);
		AppendCsvField(row, tr.globalMaxIndex >= 0 ? std::to_string(tr.globalMaxIndex) : "", first);
		AppendCsvField(row, tr.globalMaxIndex >= 0 ? std::to_string(tr.globalMaxY) : "", first);
		AppendCsvField(row, fitOk ? std::to_string(tStar) : "", first);
		AppendCsvField(row, fitOk ? std::to_string(peakIdx) : "", first);
		AppendCsvField(row, fitOk ? std::to_string(dacPeak) : "", first);
		AppendCsvField(row, std::to_string(tr.fitIndex.size()), first);
		AppendCsvField(row, FormatFitPointsCsv(tr), first);
		out << row.str() << '\n';

		++nData;
		if (!fitOk)
			++nFail;
	}

	std::printf("export-peak-csv: %s -> %s  data_rows=%d  fit_fail=%d\n", inPath, outPath, nData, nFail);
	return nFail;
}

static int RunExportPeakCsvMain(int argc, char* argv[])
{
	bool utf8Bom = false;
	const char* outDir = nullptr;
	std::vector<const char*> inputs;
	for (int i = 2; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "--utf8-bom") == 0)
			utf8Bom = true;
		else if (std::strcmp(argv[i], "-o") == 0)
		{
			if (i + 1 >= argc)
			{
				std::fprintf(stderr, "export-peak-csv: -o requires directory\n");
				return 2;
			}
			outDir = argv[++i];
		}
		else
			inputs.push_back(argv[i]);
	}
	if (inputs.empty())
	{
		std::fprintf(stderr,
			"usage: CrossPeakTest.exe --export-peak-csv <in.csv> [more.csv] [-o <dir>] [--utf8-bom]\n");
		return 2;
	}

	int rc = 0;
	for (const char* in : inputs)
	{
		const std::string outPath = MakePeakAnalysisOutPath(in, outDir);
		const int nFail = RunExportPeakCsv(in, outPath.c_str(), utf8Bom);
		if (nFail < 0)
			rc = 2;
		else if (nFail > 0 && rc == 0)
			rc = 1;
	}
	return rc;
}

static bool LoadSweepsFile(const char* path, SweepRow& sweepY, SweepRow& sweepX, double& outStep, bool& stepFromFile, std::string& err)
{
	std::string text;
	std::vector<char> raw;
	{
		std::ifstream f(path, std::ios::binary);
		if (!f)
		{
			err = "cannot open file: ";
			err += path;
			return false;
		}
		raw.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	}

	if (raw.size() >= 2 && (unsigned char)raw[0] == 0xFF && (unsigned char)raw[1] == 0xFE)
	{
		text = Utf16LeBlobToUtf8((const unsigned char*)raw.data() + 2, raw.size() - 2);
	}
	else if (LooksLikeUtf16LeNoBom(raw))
	{
		text = Utf16LeBlobToUtf8((const unsigned char*)raw.data(), raw.size());
	}
	else if (raw.size() >= 3 && (unsigned char)raw[0] == 0xEF && (unsigned char)raw[1] == 0xBB && (unsigned char)raw[2] == 0xBF)
	{
		text.assign(raw.begin() + 3, raw.end());
	}
	else
	{
		text.assign(raw.begin(), raw.end());
	}

	if (text.empty())
	{
		err = "UTF-16 file read failed";
		return false;
	}

	std::vector<std::string> lines;
	{
		std::istringstream iss(text);
		std::string ln;
		while (std::getline(iss, ln))
		{
			while (!ln.empty() && (ln.back() == '\r' || ln.back() == '\n'))
				ln.pop_back();
			if (!ln.empty())
				lines.push_back(ln);
		}
	}

	if (lines.size() < 2)
	{
		err = "need line1 RECAL 3 0, line2 RECAL 3 1; optional line3 Step DAC";
		return false;
	}

	std::vector<double> n1, n2;
	if (!ParseNumberLine(lines[0], n1) || !ParseNumberLine(lines[1], n2))
	{
		err = "could not parse numbers on line 1 or 2";
		return false;
	}

	if (!ParseSweepRow(n1, sweepY, err))
		return false;
	if (!ParseSweepRow(n2, sweepX, err))
		return false;

	stepFromFile = false;
	if (lines.size() >= 3)
	{
		std::vector<double> n3;
		if (ParseNumberLine(lines[2], n3) && n3.size() == 1)
		{
			outStep = n3[0];
			stepFromFile = true;
		}
	}
	return true;
}

static double SyntheticRawPeakQuad(int i, int center, double base = -250000.0, double kPerSq = 2000.0)
{
	const double d = (double)(i - center);
	return base - kPerSq * d * d;
}

/// Scale valid samples about peak so span >= targetSpanRaw (comm fixtures often <2.5 dB before fix).
static void AmplifyRecalSweepSpanInPlace(std::vector<double>& powers, double targetSpanRaw = 30000.0)
{
	double peakY = -(1e300);
	double minY = 0;
	bool has = false;
	for (double v : powers)
	{
		if (M576::IsRecal1DPowerInvalidValue(v))
			continue;
		if (!has)
		{
			peakY = minY = v;
			has = true;
		}
		else
		{
			peakY = (std::max)(peakY, v);
			minY = (std::min)(minY, v);
		}
	}
	if (!has)
		return;
	const double span = peakY - minY;
	if (span < 1.0)
		return;
	const double scale = targetSpanRaw / span;
	for (double& v : powers)
	{
		if (M576::IsRecal1DPowerInvalidValue(v))
			continue;
		v = peakY - (peakY - v) * scale;
	}
}

static void DefaultDemoData(SweepRow& y, SweepRow& x)
{
	y.dac_base = 32000.0;
	y.powers.resize(9);
	for (int i = 0; i < 9; ++i)
		y.powers[(size_t)i] = SyntheticRawPeakQuad(i, 4);
	x.dac_base = 32000.0;
	x.powers.resize(9);
	for (int i = 0; i < 9; ++i)
		x.powers[(size_t)i] = SyntheticRawPeakQuad(i, 3);
}

/// Synthetic checks for span / strict-peak / cross; returns number of failures.
static int RunPeak1DSelfTests()
{
	int fail = 0;
	{
		std::vector<double> flat(7, -250000.0);
		int idx = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::FindUnimodalPeak1DIndex(flat, idx, c) || c != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: flat line should fail ParabolaNotDownward\n");
			++fail;
		}
	}
	{
		std::vector<double> one(9);
		for (int i = 0; i < 9; ++i)
			one[(size_t)i] = SyntheticRawPeakQuad(i, 4);
		int idx = 0;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::FindUnimodalPeak1DIndex(one, idx, c, &t) || std::abs(t - 4.0) > 0.05 || idx != 4
			|| c != Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: quadratic peak at t≈4, lround->4 (cubic fit)\n");
			++fail;
		}
	}
	{
		std::vector<double> y(9), x(9);
		for (int i = 0; i < 9; ++i)
		{
			y[(size_t)i] = SyntheticRawPeakQuad(i, 4);
			x[(size_t)i] = SyntheticRawPeakQuad(i, 3);
		}
		int br = 0, bc = 0;
		Peak1DValidateCode yc = Peak1DValidateCode::Ok, xc = Peak1DValidateCode::Ok;
		double tY = 0, tX = 0;
		if (!M576::PeakCrossFrom1DScans(y, x, br, bc, &yc, &xc, &tY, &tX, nullptr, nullptr) || br != 4 || bc != 3
			|| yc != Peak1DValidateCode::Ok || xc != Peak1DValidateCode::Ok
			|| std::abs(tY - 4.0) > 0.05 || std::abs(tX - 3.0) > 0.05)
		{
			std::fprintf(stderr, "self-test: cross cubic tY≈4 tX≈3, lround (4,3)\n");
			++fail;
		}
	}
	{
		std::vector<double> two(2, 1.0);
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(two, t, c) || c != Peak1DValidateCode::NotEnoughSamples)
		{
			std::fprintf(stderr, "self-test: n=2 should be NotEnoughSamples\n");
			++fail;
		}
	}
	{
		std::vector<double> three(3, 1.0);
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(three, t, c) || c != Peak1DValidateCode::NotEnoughSamples)
		{
			std::fprintf(stderr, "self-test: n=3 should be NotEnoughSamples (cubic needs n>=4)\n");
			++fail;
		}
	}
	{
		std::vector<double> up(5);
		for (int i = 0; i < 5; ++i)
			up[(size_t)i] = (double)(i * i) * 10.0; // span>>min; still upward-opening => ParabolaNotDownward
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		double t = 0;
		if (M576::ParabolaVertexMax1D(up, t, c) || c != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: monotone-up samples => ParabolaNotDownward\n");
			++fail;
		}
	}
	{
		std::vector<double> strictDec(33);
		for (int i = 0; i < 33; ++i)
			strictDec[(size_t)i] = -32785.0 - (double)i * 120.0;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(strictDec, t, c) || c != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: StrictDec sweep must not return Ok with t*=0 at dac_base\n");
			++fail;
		}
	}
	// Masked fit: one firmware sentinel removed; true vertex at i=3, still ~3.0
	{
		std::vector<double> seven(9);
		for (int i = 0; i < 9; ++i)
			seven[(size_t)i] = SyntheticRawPeakQuad(i, 4);
		seven[0] = -999999.0; // M576_RECAL_POW_INVALID_1
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(seven, t, c) || c != Peak1DValidateCode::Ok || std::abs(t - 4.0) > 0.2)
		{
			std::fprintf(stderr, "self-test: masked quadratic peak near 4.0 (one -999999)\n");
			++fail;
		}
	}
	{
		std::vector<double> p(5, -999900.0); // 3 valid sentinels + 2 real samples => n_valid=2 < 4
		p[0] = 0.0;
		p[1] = 1.0;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(p, t, c) || c != Peak1DValidateCode::NotEnoughValidSamples)
		{
			std::fprintf(stderr, "self-test: 2 valid / 3 sentinels => NotEnoughValidSamples\n");
			++fail;
		}
	}
	{
		std::vector<double> allInv(5, -999900.0);
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(allInv, t, c) || c != Peak1DValidateCode::Empty)
		{
			std::fprintf(stderr, "self-test: all sentinels => Empty\n");
			++fail;
		}
	}
	// RECAL 3 0 日志 P1..Pn 摘录（col0=2357 已剥离）；近平坦，主路径不因 span 早退 LowSpan
	{
		static const double kFlatPmRecal3Y[] = {
			-529251, -529250, -529251, -529251, -529250, -529250, -529250, -529250, -529250, -529250,
			-529250, -529251, -529250, -529250, -529250, -529250, -529250, -529250, -529250, -529249,
			-529249, -529249, -529249, -529249, -529250, -529249, -529249, -529250, -529251, -529249,
			-529249, -529250, -529250, -529250,
		};
		const size_t nFlat = sizeof(kFlatPmRecal3Y) / sizeof(kFlatPmRecal3Y[0]);
		std::vector<double> flatPm(kFlatPmRecal3Y, kFlatPmRecal3Y + nFlat);
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		const bool okPm = M576::ParabolaVertexMax1D(flatPm, t, c);
		if (c == Peak1DValidateCode::LowSpan)
		{
			std::fprintf(stderr, "self-test: flat PM sweep must not return LowSpan from ParabolaVertexMax1D\n");
			++fail;
		}
		int idx = 0;
		const bool okFind = M576::FindUnimodalPeak1DIndex(flatPm, idx, c, nullptr);
		if (c == Peak1DValidateCode::LowSpan)
		{
			std::fprintf(stderr, "self-test: FindUnimodal on flat PM must not return LowSpan\n");
			++fail;
		}
		if (okFind != okPm)
		{
			std::fprintf(stderr, "self-test: FindUnimodal vs ParabolaVertexMax1D mismatch on flat PM\n");
			++fail;
		}
	}
	// 不对称右尾 + 末端孤立尖峰：预处理后主峰仍应在 ~14 附近而非尖峰格点。
	{
		const int nk = 36;
		std::vector<double> tailSpike((size_t)nk);
		for (int i = 0; i < nk; ++i)
		{
			const double qi = ((double)i - 14.0) / 13.5;
			tailSpike[(size_t)i] = -300000.0 - 55000.0 * qi * qi;
		}
		for (int i = 24; i < nk - 2; ++i)
			tailSpike[(size_t)i] -= 1200.0 * (double)(i - 23);
		tailSpike[(size_t)(nk - 1)] = 0.0;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		int idx = 0;
		if (!M576::FindUnimodalPeak1DIndex(tailSpike, idx, c, &t) || c != Peak1DValidateCode::Ok
			|| std::abs(t - 14.0) > 1.25 || idx < 13 || idx > 15)
		{
			std::fprintf(stderr, "self-test: tail + end spike preprocess + cubic peak ~14\n");
			++fail;
		}
	}
	{
		std::vector<double> q(9);
		for (int i = 0; i < 9; ++i)
			q[(size_t)i] = SyntheticRawPeakQuad(i, 4);
		M576::Peak1DFitTrace tr;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(q, t, c, &tr) || c != Peak1DValidateCode::Ok || tr.globalMaxIndex != 4
			|| std::abs(tr.globalMaxY - (-250000.0)) > 1e-3 || tr.fitIndex.size() < (size_t)M576_PEAK1D_CUBIC_MIN_SAMPLES)
		{
			std::fprintf(stderr, "self-test: Peak1DFitTrace global max and fit count\n");
			++fail;
		}
	}
	// test_sweeps.csv line 3: plateau jitter at apex; mono wedge had only 3 points before fixed-half fallback.
	{
		static const double kPlateauJitterPowers[] = {
			-129449, -129398, -129364, -129343, -129322, -129274, -129263, -129233, -129243, -129228,
			-129234, -129232, -129228, -129257, -129267, -129313, -129337, -129393, -129448, -129499,
			-129582, -129636, -129718, -129816, -129908, -130041, -130156, -130290, -130444, -130586,
			-130760, -130952, -131148,
		};
		std::vector<double> plateau(kPlateauJitterPowers, kPlateauJitterPowers + _countof(kPlateauJitterPowers));
		{
			const double peakY = *std::max_element(plateau.begin(), plateau.end());
			for (double& v : plateau)
				v = peakY - (peakY - v) * 15.0; // shallow comm bell → >=0.3 dB shoulders at sweep ends
		}
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(plateau, t, c) || c != Peak1DValidateCode::Ok || t < 8.0 || t > 13.0)
		{
			std::fprintf(stderr, "self-test: plateau jitter sweep (test_sweeps line 3) t* in [8,13] (got %.4g code=%s)\n",
				t, Peak1DCodeName(c));
			++fail;
		}
	}
	// User case: one sentinel near apex should still fit around idx 16.
	{
		static const double kUserCasePowers[] = {
			-312383, -302967, -294154, -285926, -278262, -271169, -264760, -258847, -253550, -248888, -244814,
			-241367, -238560, -236333, -234793, -233866, -999999, -233920, -234894, -236541, -238853, -241854,
			-245498, -249833, -254876, -260669, -267150, -274382, -282334, -290989, -300460, -310711, -321720,
		};
		std::vector<double> p(kUserCasePowers, kUserCasePowers + _countof(kUserCasePowers));
		int idx = 0;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::FindUnimodalPeak1DIndex(p, idx, c, &t) || c != Peak1DValidateCode::Ok || idx != 16 || std::abs(t - 16.0) > 0.25)
		{
			std::fprintf(stderr, "self-test: user sample with one sentinel should fit around idx=16\n");
			++fail;
		}
	}
	// prominence (raw scale, INV-16): global micro-flat (span < 0.3 dB) => ParabolaNotDownward
	{
		std::vector<double> micro(7, -250000.0);
		micro[3] = -248500.0; // span 0.15 dB = 1500 raw
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(micro, t, c) || c != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: globalMicroFlat span=0.15 must be ParabolaNotDownward\n");
			++fail;
		}
	}
	// prominence (post-outlier): useOk span < 0.3 dB => ParabolaNotDownward
	{
		std::vector<double> weakLeft(9, -249900.0);
		weakLeft[4] = -250000.0; // span 0.01 dB = 100 raw
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(weakLeft, t, c) || c != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: preprocess span=0.01 must be ParabolaNotDownward\n");
			++fail;
		}
	}
	// post-outlier flat gate: isolated spike inflates pre-outlier span; useOk span >= 0.3 dB => Strict Ok
	{
		std::vector<double> withSpike(9);
		for (int i = 0; i < 9; ++i)
		{
			const double x = (double)(i - 4);
			withSpike[(size_t)i] = -250000.0 - 400.0 * x * x; // span 6400 raw = 0.64 dB
		}
		withSpike[1] = -240000.0;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(withSpike, t, c) || c != Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: postOutlierSpike useOk span>=0.3dB must Ok (code=%s)\n",
				Peak1DCodeName(c));
			++fail;
		}
	}
	// one-sided shoulder: global span >= 0.3 dB may Ok (shoulder trim not hard reject)
	{
		std::vector<double> asym(9);
		asym[4] = -250000.0;
		for (int i = 0; i < 4; ++i)
			asym[(size_t)i] = -250400.0;
		for (int i = 5; i < 9; ++i)
			asym[(size_t)i] = -250000.0 - 5000.0 * (double)(i - 5);
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(asym, t, c) || c != Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: oneSidedShoulder global span ok may Ok (code=%s)\n",
				Peak1DCodeName(c));
			++fail;
		}
	}
	// asymmetric long tail — global span large; preprocess gate passes
	{
		static const double kAsymLongTail[] = {
			-300000, -295000, -290000, -285000, -280000, -275000, -270000, -265000, -260000, -255000,
			-250000, -250100, -250150, -250200, -250250,
		};
		std::vector<double> asymTail(kAsymLongTail, kAsymLongTail + _countof(kAsymLongTail));
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(asymTail, t, c) || c != Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: asymmetricLongTail global span ok may Ok (code=%s)\n",
				Peak1DCodeName(c));
			++fail;
		}
	}
	// prominence: noise ripple (< 0.3 dB bump on flat)
	{
		std::vector<double> ripple(9, -250000.0);
		ripple[4] = -247500.0; // 0.25 dB = 2500 raw
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(ripple, t, c) || c != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: noiseRipple must be ParabolaNotDownward\n");
			++fail;
		}
	}
	// prominence: symmetric weak peak still Ok
	{
		std::vector<double> weak(9);
		for (int i = 0; i < 9; ++i)
			weak[(size_t)i] = SyntheticRawPeakQuad(i, 4);
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(weak, t, c) || c != Peak1DValidateCode::Ok || std::abs(t - 4.0) > 0.15)
		{
			std::fprintf(stderr, "self-test: symmetricWeakPeak must Ok near t=4\n");
			++fail;
		}
	}
	// prominence: symmetric fit window half-width around argmax
	{
		std::vector<double> q(9);
		for (int i = 0; i < 9; ++i)
			q[(size_t)i] = SyntheticRawPeakQuad(i, 4);
		M576::Peak1DFitTrace tr;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(q, t, c, &tr) || c != Peak1DValidateCode::Ok || tr.globalMaxIndex != 4)
		{
			std::fprintf(stderr, "self-test: symmetric fit window needs trace at idx 4\n");
			++fail;
		}
		else
		{
			int fmin = tr.fitIndex[0], fmax = tr.fitIndex[0];
			for (int fi : tr.fitIndex)
			{
				fmin = (std::min)(fmin, fi);
				fmax = (std::max)(fmax, fi);
			}
			const int halfL = tr.globalMaxIndex - fmin;
			const int halfR = fmax - tr.globalMaxIndex;
			if (halfL != halfR)
			{
				std::fprintf(stderr, "self-test: symmetric fit window must be symmetric (L=%d R=%d)\n", halfL, halfR);
				++fail;
			}
		}
	}
	// comm_2026-07-01 L523: step 241 attempt 5 — global span ~0.31 dB; Ok allowed under preprocess gate
	{
		static const double kStep241Attempt5[] = {
			-253541, -253247, -252979, -252752, -252510, -252281, -252101, -251929, -251755, -251581,
			-251440, -251312, -251189, -251093, -250975, -250892, -250808, -250734, -250660, -250606,
			-250571, -250543, -250499, -250486, -250471, -250472, -250482, -250490, -250506, -250522,
			-250548, -250593, -250618,
		};
		std::vector<double> p(kStep241Attempt5, kStep241Attempt5 + _countof(kStep241Attempt5));
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(p, t, c))
		{
			if (c == Peak1DValidateCode::Empty || c == Peak1DValidateCode::NotEnoughValidSamples
				|| c == Peak1DValidateCode::NotEnoughSamples)
			{
				std::fprintf(stderr, "self-test: comm step241 attempt5 unexpected empty (code=%s)\n",
					Peak1DCodeName(c));
				++fail;
			}
		}
	}
	// runtime prominence override
	{
		M576::Peak1DSetMinProminenceDb(1.0, false);
		std::vector<double> shallow(7);
		for (int i = 0; i < 7; ++i)
			shallow[(size_t)i] = -250000.0 - 1500.0 * std::abs((double)(i - 3));
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::ParabolaVertexMax1D(shallow, t, c) || c != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: prominence 1.0 should reject shallow peak\n");
			++fail;
		}
		M576::Peak1DResetMinProminenceDb();
	}
	return fail;
}

static int RunPmRangeSelfTests()
{
	int fail = 0;
	{
		double lo = 0, hi = 0;
		if (!M576::GetPmRangeDbmBounds(0, lo, hi) || lo != -14.0 || hi != 6.0)
		{
			std::fprintf(stderr, "pm-range: bounds 0\n");
			++fail;
		}
		if (M576::GetPmRangeDbmBounds(4, lo, hi))
		{
			std::fprintf(stderr, "pm-range: range 4 should not have bounds\n");
			++fail;
		}
	}
	{
		const double raw = -126102.0;
		const double dbm = M576::RecalRawToDbm(raw);
		if (std::abs(dbm - (-12.6102)) > 1e-6)
		{
			std::fprintf(stderr, "pm-range: RecalRawToDbm\n");
			++fail;
		}
		std::vector<double> p = { -200000.0, raw, -999999.0 };
		double pr = 0, pd = 0, lo = 0, hi = 0;
		int idx = -1;
		if (!M576::ValidatePeakPowerInPmRange(0, p, -1, pr, pd, lo, hi, idx) || idx != 1)
		{
			std::fprintf(stderr, "pm-range: range0 should accept -126102 at idx 1\n");
			++fail;
		}
		if (M576::ValidatePeakPowerInPmRange(1, p, -1, pr, pd, lo, hi, idx))
		{
			std::fprintf(stderr, "pm-range: range1 should reject -12.61 dBm peak\n");
			++fail;
		}
		if (!M576::ValidatePeakPowerInPmRange(4, p, -1, pr, pd, lo, hi, idx))
		{
			std::fprintf(stderr, "pm-range: auto should skip\n");
			++fail;
		}
	}
	{
		const double rawEdge = -140000.0; // -14.0 dBm
		std::vector<double> p = { rawEdge };
		double pr = 0, pd = 0, lo = 0, hi = 0;
		int idx = -1;
		if (!M576::ValidatePeakPowerInPmRange(0, p, 0, pr, pd, lo, hi, idx))
		{
			std::fprintf(stderr, "pm-range: -14 dBm should be in range 0\n");
			++fail;
		}
		if (M576::ValidatePeakPowerInPmRange(1, p, 0, pr, pd, lo, hi, idx))
		{
			std::fprintf(stderr, "pm-range: -14 dBm should not be in range 1\n");
			++fail;
		}
	}
	{
		std::vector<double> p((size_t)33);
		for (int i = 0; i < 33; ++i)
			p[(size_t)i] = -130000.0 - i * 3000.0;
		const SweepProfile prof = AnalyzeRecal1DSweepProfile(p);
		if (IsRetryablePeakFailure(Peak1DValidateCode::PmRangeMismatch, prof, 33))
		{
			std::fprintf(stderr, "pm-range: PmRangeMismatch must not retry\n");
			++fail;
		}
	}
	{
		if (M576::ParseOpmPmRangeReplyAscii("1") != 1
			|| M576::ParseOpmPmRangeReplyAscii("4") != 4
			|| M576::ParseOpmPmRangeReplyAscii(" 2 ") != 2
			|| M576::ParseOpmPmRangeReplyAscii("OK") != -1
			|| M576::ParseOpmPmRangeReplyAscii("") != -1)
		{
			std::fprintf(stderr, "pm-range: ParseOpmPmRangeReplyAscii\n");
			++fail;
		}
	}
	{
		std::string commPath = PathToExeDirFile("comm_2026-05-25_recal_sweeps.csv");
		if (commPath.empty())
			commPath = "comm_2026-05-25_recal_sweeps.csv";
		std::ifstream f(commPath);
		if (f)
		{
			std::string line;
			while (std::getline(f, line))
			{
				if (line.find("RECAL 3 0") == std::string::npos)
					continue;
				std::vector<double> nums;
				if (!ParseNumberLine(line, nums) || nums.size() < 3)
					break;
				std::vector<double> powers(nums.begin() + 1, nums.end());
				double pr = 0, pd = 0, lo = 0, hi = 0;
				int idx = -1;
				if (!M576::ValidatePeakPowerInPmRange(0, powers, -1, pr, pd, lo, hi, idx))
				{
					std::fprintf(stderr, "pm-range: comm line1 peak should pass range 0 (dbm=%.4f idx=%d)\n", pd, idx);
					++fail;
				}
				if (M576::ValidatePeakPowerInPmRange(1, powers, -1, pr, pd, lo, hi, idx))
				{
					std::fprintf(stderr, "pm-range: comm line1 peak should fail range 1 (dbm=%.4f)\n", pd);
					++fail;
				}
				break;
			}
		}
	}
	return fail;
}

static std::vector<double> MakeBellPow(int n, int peakIdx, double floorVal, double peakVal)
{
	std::vector<double> p((size_t)n, floorVal);
	for (int i = 0; i < n; ++i)
	{
		const double d = ((double)i - (double)peakIdx) / 4.0;
		p[(size_t)i] = peakVal - 8000.0 * d * d;
	}
	return p;
}

static std::vector<double> MakeMesaPow(int n, int riseEnd, int fallStart, double floorVal, double peakVal)
{
	std::vector<double> p((size_t)n, floorVal);
	if (n <= 0 || riseEnd < 1 || fallStart >= n || fallStart <= riseEnd)
		return p;
	for (int i = 0; i < riseEnd; ++i)
		p[(size_t)i] = floorVal + (peakVal - floorVal) * ((double)i / (double)riseEnd);
	for (int i = riseEnd; i < fallStart; ++i)
		p[(size_t)i] = peakVal;
	for (int i = fallStart; i < n; ++i)
	{
		const double t = (double)(i - fallStart) / (double)(n - 1 - fallStart);
		p[(size_t)i] = peakVal + (floorVal - peakVal) * t;
	}
	return p;
}

static double Col0FromMovingBase(int movingBase, int halfRange)
{
	return (double)movingBase - (double)halfRange;
}

static bool FeedPipelineSweepExact(
	Recal1DSweepPipelineState& st,
	const std::vector<double>& pow,
	double col0,
	int dacStep)
{
	Recal1DSweepCommand cmd = {};
	if (!GetNextPipelineSweepCommand(st, cmd))
		return false;
	st.movingBase = cmd.movingBase;
	int peakIdx = 0;
	double tPeak = 0.0;
	Peak1DValidateCode code = Peak1DValidateCode::Empty;
	(void)FindUnimodalPeak1DIndex(pow, peakIdx, code, &tPeak, nullptr, cmd.fitPolicy);
	const bool peakOk = (code == Peak1DValidateCode::Ok);
	return AdvanceRecal1DSweepPipeline(st, col0, pow, peakOk, code, tPeak, peakIdx, dacStep);
}

static int RunSweepPipelineSelfTests()
{
	int fail = 0;
	const int uiFine = 64;
	const int dacStep = 4;
	const int movingBaseKnown = 1000;
	const int col0AnchorKnown = (int)Col0FromMovingBase(movingBaseKnown, 200);

	if (StitchMovingBaseFromAnchor(-70, 1) != -270
		|| StitchMovingBaseFromAnchor(-70, 2) != 130)
	{
		std::fprintf(stderr, "self-test: StitchMovingBaseFromAnchor col0=-70 k1/k2 symmetric\n");
		++fail;
	}
	if (StitchMovingBaseFromAnchor(col0AnchorKnown, 1) != 600)
	{
		std::fprintf(stderr, "self-test: StitchMovingBaseFromAnchor col0=800 k=1 left\n");
		++fail;
	}
	if (StitchMovingBaseFromAnchor(col0AnchorKnown, 2) != col0AnchorKnown + 200)
	{
		std::fprintf(stderr, "self-test: StitchMovingBaseFromAnchor col0=800 k=2 right (+200)\n");
		++fail;
	}
	if (StitchMovingBaseFromAnchor(col0AnchorKnown, 3) != col0AnchorKnown - 400)
	{
		std::fprintf(stderr, "self-test: StitchMovingBaseFromAnchor col0=800 k=3 explore left (-400)\n");
		++fail;
	}
	if (StitchMovingBaseFromAnchor(col0AnchorKnown, 4) != col0AnchorKnown + 400)
	{
		std::fprintf(stderr, "self-test: StitchMovingBaseFromAnchor col0=800 k=4 explore right (+400)\n");
		++fail;
	}
	if (!IsStitchLeft(1) || IsStitchLeft(2))
	{
		std::fprintf(stderr, "self-test: IsStitchLeft k=1 left k=2 right\n");
		++fail;
	}

	{
		Recal1DSweepPipelineState st = {};
		InitRecal1DSweepPipeline(st, uiFine, 9999);
		const std::vector<double> bell = MakeBellPow(33, 16, -250000.0, -240000.0);
		FeedPipelineSweepExact(st, bell, Col0FromMovingBase(9999, uiFine), dacStep);
		if (st.phase != Recal1DPipelinePhase::Succeeded || st.sweepCount != 1)
		{
			std::fprintf(stderr, "self-test: pipeline Fine64 Ok -> 1 sweep (phase=%d count=%d)\n",
				(int)st.phase, st.sweepCount);
			++fail;
		}
	}

	{
		Recal1DSweepPipelineState st = {};
		InitRecal1DSweepPipeline(st, uiFine, 9999);
		static const double flat64[] = {
			-122228, -122243, -122228, -122207, -122212, -122217, -122216, -122259, -122296, -122275,
			-122263, -122254, -122234, -122230, -122225, -122228, -122214, -122212, -122209, -122192,
			-122193, -122183, -122180, -122173, -122165, -122161, -122151, -122142, -122131, -122140,
			-122132, -122134, -122123
		};
		std::vector<double> flat(flat64, flat64 + 33);
		FeedPipelineSweepExact(st, flat, Col0FromMovingBase(9999, uiFine), dacStep);
		const std::vector<double> bell200 = MakeBellPow(101, 50, -250000.0, -230000.0);
		FeedPipelineSweepExact(st, bell200, Col0FromMovingBase(9999, 200), dacStep);
		Recal1DSweepCommand fineCmd = {};
		if (!GetNextPipelineSweepCommand(st, fineCmd)
			|| fineCmd.halfRange != uiFine
			|| st.phase != Recal1DPipelinePhase::FineRefine)
		{
			std::fprintf(stderr, "self-test: pipeline 64 fail -> 200 ok -> fineRefine (range=%d phase=%d)\n",
				fineCmd.halfRange, (int)st.phase);
			++fail;
		}
		const std::vector<double> bellFine = MakeBellPow(33, 16, -250000.0, -240000.0);
		FeedPipelineSweepExact(st, bellFine, Col0FromMovingBase(fineCmd.movingBase, fineCmd.halfRange), dacStep);
		if (st.phase != Recal1DPipelinePhase::Succeeded || st.sweepCount != 3)
		{
			std::fprintf(stderr, "self-test: pipeline coarse+fine path sweeps=%d phase=%d\n",
				st.sweepCount, (int)st.phase);
			++fail;
		}
	}

	{
		Recal1DSweepPipelineState st = {};
		InitRecal1DSweepPipeline(st, uiFine, 9999);
		static const double flat64[] = {
			-122228, -122243, -122228, -122207, -122212, -122217, -122216, -122259, -122296, -122275,
			-122263, -122254, -122234, -122230, -122225, -122228, -122214, -122212, -122209, -122192,
			-122193, -122183, -122180, -122173, -122165, -122161, -122151, -122142, -122131, -122140,
			-122132, -122134, -122123
		};
		std::vector<double> flat(flat64, flat64 + 33);
		FeedPipelineSweepExact(st, flat, Col0FromMovingBase(9999, uiFine), dacStep);
		std::vector<double> flat200(101, -250000.0);
		FeedPipelineSweepExact(st, flat200, -70.0, dacStep);
		Recal1DSweepCommand stitchCmd = {};
		if (!GetNextPipelineSweepCommand(st, stitchCmd)
			|| stitchCmd.movingBase != -270
			|| stitchCmd.halfRange != 200
			|| st.phase != Recal1DPipelinePhase::Stitch
			|| st.anchorBase != -70)
		{
			std::fprintf(stderr,
				"self-test: 9999/col0=-70 coarse fail -> stitch k=1 base=%d anchor=%d phase=%d\n",
				stitchCmd.movingBase, st.anchorBase, (int)st.phase);
			++fail;
		}
	}

	{
		Recal1DSweepPipelineState st = {};
		InitRecal1DSweepPipeline(st, uiFine, movingBaseKnown);
		static const double flat64[] = {
			-122228, -122243, -122228, -122207, -122212, -122217, -122216, -122259, -122296, -122275,
			-122263, -122254, -122234, -122230, -122225, -122228, -122214, -122212, -122209, -122192,
			-122193, -122183, -122180, -122173, -122165, -122161, -122151, -122142, -122131, -122140,
			-122132, -122134, -122123
		};
		std::vector<double> flat(flat64, flat64 + 33);
		FeedPipelineSweepExact(st, flat, Col0FromMovingBase(movingBaseKnown, uiFine), dacStep);
		std::vector<double> flat200(101, -250000.0);
		FeedPipelineSweepExact(st, flat200, Col0FromMovingBase(movingBaseKnown, 200), dacStep);
		const int leftBase = StitchMovingBaseFromAnchor(col0AnchorKnown, 1);
		std::vector<double> stitchBell = MakeBellPow(101, 50, -250000.0, -230000.0);
		FeedPipelineSweepExact(st, stitchBell, Col0FromMovingBase(leftBase, 200), dacStep);
		if (st.stitchK != 2 || st.phase != Recal1DPipelinePhase::Stitch)
		{
			std::fprintf(stderr, "self-test: stitch k=1 collect only -> stitchK=%d phase=%d\n",
				st.stitchK, (int)st.phase);
			++fail;
		}
		const int rightBase = StitchMovingBaseFromAnchor(col0AnchorKnown, 2);
		FeedPipelineSweepExact(st, stitchBell, Col0FromMovingBase(rightBase, 200), dacStep);
		if (st.phase != Recal1DPipelinePhase::FineRefine || st.lastStitchK != 2)
		{
			std::fprintf(stderr, "self-test: stitch symmetric trio bell -> FineRefine (phase=%d lastStitchK=%d)\n",
				(int)st.phase, st.lastStitchK);
			++fail;
		}
	}

	{
		Recal1DSweepPipelineState st = {};
		InitRecal1DSweepPipeline(st, uiFine, movingBaseKnown);
		std::vector<double> flat33(33, -250000.0);
		std::vector<double> flat200(101, -250000.0);
		FeedPipelineSweepExact(st, flat33, Col0FromMovingBase(movingBaseKnown, uiFine), dacStep);
		FeedPipelineSweepExact(st, flat200, Col0FromMovingBase(movingBaseKnown, 200), dacStep);
		for (int k = 1; k <= 4; ++k)
		{
			const int baseK = StitchMovingBaseFromAnchor(col0AnchorKnown, k);
			FeedPipelineSweepExact(st, flat200, Col0FromMovingBase(baseK, 200), dacStep);
			if (k < 4 && st.phase == Recal1DPipelinePhase::Failed)
				break;
		}
		if (st.phase != Recal1DPipelinePhase::Failed || st.lastStitchK != 4)
		{
			std::fprintf(stderr, "self-test: stitch k1..4 all fail -> Failed (phase=%d lastStitchK=%d)\n",
				(int)st.phase, st.lastStitchK);
			++fail;
		}
		const M576::PeakPipelineFailureReport rep = BuildPeakPipelineFailureReport(st);
		if (std::strcmp(rep.failedPhase, "stitch_k4") != 0)
		{
			std::fprintf(stderr, "self-test: pipeline failure report phase=%s\n", rep.failedPhase);
			++fail;
		}
	}

	{
		Recal1DSweepPipelineState st = {};
		InitRecal1DSweepPipeline(st, uiFine, movingBaseKnown);
		std::vector<double> flat33(33, -250000.0);
		std::vector<double> flat200(101, -250000.0);
		FeedPipelineSweepExact(st, flat33, Col0FromMovingBase(movingBaseKnown, uiFine), dacStep);
		FeedPipelineSweepExact(st, flat200, Col0FromMovingBase(movingBaseKnown, 200), dacStep);
		for (int k = 1; k <= 2; ++k)
		{
			const int baseK = StitchMovingBaseFromAnchor(col0AnchorKnown, k);
			FeedPipelineSweepExact(st, flat200, Col0FromMovingBase(baseK, 200), dacStep);
		}
		if (st.phase != Recal1DPipelinePhase::Stitch || st.stitchK != 3)
		{
			std::fprintf(stderr, "self-test: symmetric trio flat fail -> explore k=3 (phase=%d stitchK=%d)\n",
				(int)st.phase, st.stitchK);
			++fail;
		}
		const int exploreBase = StitchMovingBaseFromAnchor(col0AnchorKnown, 3);
		std::vector<double> exploreBell = MakeBellPow(101, 50, -250000.0, -230000.0);
		FeedPipelineSweepExact(st, exploreBell, Col0FromMovingBase(exploreBase, 200), dacStep);
		if (st.phase != Recal1DPipelinePhase::FineRefine || st.lastStitchK != 3)
		{
			std::fprintf(stderr, "self-test: stitch explore k=3 bell -> FineRefine (phase=%d lastStitchK=%d)\n",
				(int)st.phase, st.lastStitchK);
			++fail;
		}
		if (st.hasLastMergePlateau)
		{
			std::fprintf(stderr, "self-test: explore merge must not use dual_knee\n");
			++fail;
		}
	}

	{
		const std::vector<double> mesa = MakeMesaPow(101, 20, 80, -260000.0, -240000.0);
		const std::vector<double> bell = MakeBellPow(101, 50, -260000.0, -240000.0);
		if (!M576::IsMergedMesaProfile(mesa))
		{
			std::fprintf(stderr, "self-test: IsMergedMesaProfile mesa must be true\n");
			++fail;
		}
		if (M576::IsMergedMesaProfile(bell))
		{
			std::fprintf(stderr, "self-test: IsMergedMesaProfile bell must be false\n");
			++fail;
		}
		double t = 0.0;
		Peak1DValidateCode c = Peak1DValidateCode::Empty;
		M576::Peak1DPlateauTrace tr = {};
		if (!M576::FindPlateauDualKneePeak1D(mesa, t, c, &tr) || c != Peak1DValidateCode::Ok
			|| t < 45.0 || t > 55.0 || !tr.usedDualKnee)
		{
			std::fprintf(stderr, "self-test: mesa dual_knee t* in [45,55] (got %.4g code=%s)\n",
				t, Peak1DCodeName(c));
			++fail;
		}
		std::vector<M576::Recal1DSweepSegment> segs;
		for (int k = 0; k <= 2; ++k)
		{
			M576::Recal1DSweepSegment seg = {};
			seg.col0 = Col0FromMovingBase(col0AnchorKnown, 200);
			seg.halfRange = 200;
			seg.stitchK = k;
			seg.pow = mesa;
			segs.push_back(seg);
		}
		double peakDac = 0.0;
		double tMerged = 0.0;
		int idxMerged = 0;
		Peak1DValidateCode mergeCode = Peak1DValidateCode::Empty;
		double spanRaw = 0.0;
		M576::Peak1DPlateauTrace mergeTr = {};
		if (!M576::FindPeakDacOnMerged(
				segs, dacStep, peakDac, tMerged, idxMerged, mergeCode, nullptr, spanRaw, &mergeTr)
			|| mergeCode != Peak1DValidateCode::Ok || tMerged < 45.0 || tMerged > 55.0
			|| !mergeTr.usedDualKnee)
		{
			std::fprintf(stderr, "self-test: FindPeakDacOnMerged mesa symmetric trio t* in [45,55] (got %.4g code=%s dual=%d)\n",
				tMerged, Peak1DCodeName(mergeCode), mergeTr.usedDualKnee ? 1 : 0);
			++fail;
		}
		std::vector<M576::Recal1DSweepSegment> bellSegs;
		for (int k = 0; k <= 2; ++k)
		{
			M576::Recal1DSweepSegment seg = {};
			seg.col0 = Col0FromMovingBase(col0AnchorKnown, 200);
			seg.halfRange = 200;
			seg.stitchK = k;
			seg.pow = bell;
			bellSegs.push_back(seg);
		}
		double bellT = 0.0;
		Peak1DValidateCode bellCode = Peak1DValidateCode::Empty;
		M576::Peak1DPlateauTrace bellTr = {};
		if (!M576::FindPeakDacOnMerged(
				bellSegs, dacStep, peakDac, bellT, idxMerged, bellCode, nullptr, spanRaw, &bellTr)
			|| bellCode != Peak1DValidateCode::Ok || bellTr.usedDualKnee)
		{
			std::fprintf(stderr, "self-test: FindPeakDacOnMerged bell symmetric trio relaxed (t=%.4g code=%s dual=%d)\n",
				bellT, Peak1DCodeName(bellCode), bellTr.usedDualKnee ? 1 : 0);
			++fail;
		}
	}

	return fail;
}

static int RunSweepRetryPlannerSelfTests()
{
	int fail = 0;
	const int uiFine = 64;
	const int n = 33;

	if (SuggestJumpMaxDacRange(64, 200) != 200 || SuggestJumpMaxDacRange(150, 200) != 200
		|| SuggestJumpMaxDacRange(200, 200) != 0)
	{
		std::fprintf(stderr, "self-test: SuggestJumpMaxDacRange 64/150->200 cap\n");
		++fail;
	}
	if (!NeedsFineRefineAfterSuccess(200, 64) || NeedsFineRefineAfterSuccess(64, 64))
	{
		std::fprintf(stderr, "self-test: NeedsFineRefineAfterSuccess coarse vs fine\n");
		++fail;
	}
	{
		SweepRecenterSessionState fineSt = {};
		fineSt.uiFineRange = 64;
		fineSt.attemptRange = 64;
		fineSt.fineConsumed = true;
		if (!IsFineRefineSweepAttempt(fineSt))
		{
			std::fprintf(stderr, "self-test: IsFineRefineSweepAttempt when fineConsumed\n");
			++fail;
		}
		fineSt.fineConsumed = false;
		if (IsFineRefineSweepAttempt(fineSt))
		{
			std::fprintf(stderr, "self-test: IsFineRefineSweepAttempt before fine sweep\n");
			++fail;
		}
		if (Peak1DFitPolicyForSweepResult(64, 64) != Peak1DFitPolicy::FineRefineRelaxed
			|| Peak1DFitPolicyForSweepResult(200, 64) != Peak1DFitPolicy::Strict)
		{
			std::fprintf(stderr, "self-test: Peak1DFitPolicyForSweepResult\n");
			++fail;
		}
		{
			SweepRecenterSessionState xSt = {};
			xSt.uiFineRange = 64;
			xSt.attemptRange = 64;
			xSt.fineConsumed = true;
			if (Peak1DFitPolicyForCrossAxis(xSt, 64, 64) != Peak1DFitPolicy::FineRefineRelaxed)
			{
				std::fprintf(stderr, "self-test: Peak1DFitPolicyForCrossAxis fine attempt\n");
				++fail;
			}
			xSt.fineConsumed = false;
			if (Peak1DFitPolicyForCrossAxis(xSt, 64, 64) != Peak1DFitPolicy::FineRefineRelaxed)
			{
				std::fprintf(stderr, "self-test: Peak1DFitPolicyForCrossAxis fine-range result\n");
				++fail;
			}
			if (Peak1DFitPolicyForCrossAxis(xSt, 200, 64) != Peak1DFitPolicy::Strict)
			{
				std::fprintf(stderr, "self-test: Peak1DFitPolicyForCrossAxis coarse range\n");
				++fail;
			}
		}
	}

	{
		std::vector<double> strictDec((size_t)n);
		for (int i = 0; i < n; ++i)
			strictDec[(size_t)i] = -130000.0 - i * 3000.0;
		const SweepProfile p = AnalyzeRecal1DSweepProfile(strictDec);
		SweepRecenterSessionState st = {};
		InitSweepRecenterSessionState(st, uiFine, 9999);
		SweepRecenterFailureInfo fi = {};
		fi.code = Peak1DValidateCode::ParabolaNotDownward;
		const double center = -100.0 + 64.0;
		const SweepRetryPlan plan = PlanNextRecal1DSweepAttempt(
			st, Peak1DValidateCode::ParabolaNotDownward, p, strictDec, center, 0, false, fi);
		if (plan.action != SweepRetryAction::MonoCoarseShift || plan.nextRange != 200
			|| plan.nextBase == 9999)
		{
			std::fprintf(stderr, "self-test: StrictDec planner must MonoCoarseShift @200 (action=%d range=%d base=%d)\n",
				(int)plan.action, plan.nextRange, plan.nextBase);
			++fail;
		}
	}

	{
		const double commFlat[] = {
			-122228, -122243, -122228, -122207, -122212, -122217, -122216, -122259, -122296, -122275,
			-122263, -122254, -122234, -122230, -122225, -122228, -122214, -122212, -122209, -122192,
			-122193, -122183, -122180, -122173, -122165, -122161, -122151, -122142, -122131, -122140,
			-122132, -122134, -122123
		};
		std::vector<double> flat(commFlat, commFlat + n);
		const SweepProfile pFlat = AnalyzeRecal1DSweepProfile(flat);
		SweepRecenterSessionState st = {};
		InitSweepRecenterSessionState(st, uiFine, 9999);
		SweepRecenterFailureInfo fi = {};
		fi.code = Peak1DValidateCode::ParabolaNotDownward;
		const SweepRetryPlan jump = PlanNextRecal1DSweepAttempt(
			st, Peak1DValidateCode::ParabolaNotDownward, pFlat, flat, -31.0 + 64.0, 0, false, fi);
		if (jump.action != SweepRetryAction::JumpFlatMax || jump.nextRange != 200 || jump.nextBase != 9999)
		{
			std::fprintf(stderr, "self-test: Flat planner must JumpFlatMax to 200\n");
			++fail;
		}
		SweepRecenterSessionState stMax = st;
		stMax.flatJumpedToMax = true;
		stMax.attemptRange = 200;
		const SweepRetryPlan shift = PlanNextRecal1DSweepAttempt(
			stMax, Peak1DValidateCode::ParabolaNotDownward, pFlat, flat, -31.0 + 200.0, 1, false, fi);
		if (shift.action != SweepRetryAction::FlatAtMaxShift || shift.nextRange != 200)
		{
			std::fprintf(stderr, "self-test: Flat@200 planner must FlatAtMaxShift (got action=%d)\n",
				(int)shift.action);
			++fail;
		}
	}

	{
		std::vector<double> midPeak((size_t)n, -130000.0);
		midPeak[(size_t)(n / 2)] = -120000.0;
		const SweepProfile pMid = AnalyzeRecal1DSweepProfile(midPeak);
		SweepRecenterFailureInfo fi = {};
		fi.code = Peak1DValidateCode::ParabolaNotDownward;
		fi.tPeak = (double)(n / 2);
		fi.hasTPeak = true;
		if (!IsCoarsePeakHint(Peak1DValidateCode::ParabolaNotDownward, pMid, n, &fi))
		{
			std::fprintf(stderr, "self-test: interior NonMono must be IsCoarsePeakHint\n");
			++fail;
		}
		SweepRecenterSessionState st = {};
		InitSweepRecenterSessionState(st, uiFine, 9999);
		st.attemptRange = 200;
		st.inCoarsePhase = true;
		const SweepRetryPlan fine = PlanNextRecal1DSweepAttempt(
			st, Peak1DValidateCode::ParabolaNotDownward, pMid, midPeak, 0.0 + 200.0, 2, false, fi);
		if (fine.action != SweepRetryAction::FineRefine || fine.nextRange != uiFine)
		{
			std::fprintf(stderr, "self-test: coarse hint must FineRefine to uiFine (action=%d range=%d)\n",
				(int)fine.action, fine.nextRange);
			++fail;
		}
	}

	{
		SweepRecenterSessionState st = {};
		InitSweepRecenterSessionState(st, uiFine, 9999);
		const SweepRetryPlan fineOk = PlanFineRefineAfterCoarseSuccess(
			st, -100.0, 16.0, n, 200);
		if (fineOk.action != SweepRetryAction::FineRefine || fineOk.nextRange != uiFine)
		{
			std::fprintf(stderr, "self-test: PlanFineRefineAfterCoarseSuccess\n");
			++fail;
		}
	}

	{
		// comm_2026-07-01 step 7 run2 attempt2 @200: left shoulder OK, right truncated, span ~0.92 dB.
		static const double step7Att2[] = {
			-257500, -256988, -256467, -256008, -255548, -255153, -254695, -254298, -253909, -253529,
			-253185, -252857, -252524, -252206, -251917, -251634, -251378, -251164, -250902, -250678,
			-250490, -250275, -250094, -249913, -249770, -249609, -249469, -249342, -249198, -249065,
			-248973, -248892, -248798, -248708, -248627, -248569, -248513, -248462, -248429, -248387,
			-248358, -248315, -248297, -248288, -248292, -248289, -248287, -248287, -248289, -248322,
			-248359, -248371, -248399, -248441, -248449, -248488, -248536, -248571, -248610, -248659,
			-248701, -248752, -248805, -248863, -248905, -248978, -249036, -249081, -249172, -249221,
			-249274, -249329, -249388, -249441, -249489, -249563, -249604, -249665, -249742, -249785,
			-249857, -249907, -249959, -250023, -250052, -250112, -250144, -250176, -250221, -250276,
			-250303, -250332, -250382, -250416, -250441, -250505, -250518, -250540, -250551, -250559,
			-250596
		};
		static const double step7Att3[] = {
			-250955, -250717, -250506, -250300, -250103, -249936, -249751, -249618, -249474, -249322,
			-249198, -249063, -248946, -248842, -248753, -248653, -248577, -248523, -248459, -248394,
			-248365, -248318, -248288, -248255, -248239, -248227, -248223, -248204, -248194, -248233,
			-248229, -248228, -248252, -248288, -248310, -248331, -248373, -248413, -248437, -248462,
			-248523, -248573, -248619, -248662, -248716, -248787, -248838, -248879, -248928, -248999,
			-249045, -249097, -249163, -249228, -249287, -249342, -249393, -249458, -249502, -249568,
			-249633, -249670, -249728, -249785, -249835, -249905, -249950, -249991, -250045, -250085,
			-250135, -250168, -250207, -250242, -250297, -250309, -250368, -250375, -250402, -250445,
			-250458, -250495, -250521, -250547, -250556, -250571, -250593, -250602, -250622, -250651,
			-250685, -250680, -250665, -250641, -250627, -250646, -250673, -250680, -250688, -250708,
			-250739
		};
		const int nStep7 = (int)(sizeof(step7Att2) / sizeof(step7Att2[0]));
		if (nStep7 != (int)(sizeof(step7Att3) / sizeof(step7Att3[0])))
		{
			std::fprintf(stderr, "self-test: step7 att2/att3 length mismatch\n");
			++fail;
		}
		else
		{
			std::vector<double> att2(step7Att2, step7Att2 + nStep7);
			std::vector<double> att3(step7Att3, step7Att3 + nStep7);
			const SweepProfile p2 = AnalyzeRecal1DSweepProfile(att2);
			const SweepProfile p3 = AnalyzeRecal1DSweepProfile(att3);
			SweepRecenterSessionState stMax = {};
			InitSweepRecenterSessionState(stMax, uiFine, 9999);
			stMax.flatJumpedToMax = true;
			stMax.attemptRange = 200;
			SweepRecenterFailureInfo fi = {};
			fi.code = Peak1DValidateCode::ParabolaNotDownward;
			const double centerAtt2 = -230.0;
			if (!IsCoarseExpandedInteriorPeak(stMax, p2, att2, nStep7))
			{
				std::fprintf(stderr, "self-test: step7 att2@200 must IsCoarseExpandedInteriorPeak\n");
				++fail;
			}
			if (IsCoarseExpandedInteriorPeak(stMax, p3, att3, nStep7))
			{
				std::fprintf(stderr, "self-test: step7 att3 plateau must not IsCoarseExpandedInteriorPeak\n");
				++fail;
			}
			const SweepRetryPlan fine2 = PlanNextRecal1DSweepAttempt(
				stMax, Peak1DValidateCode::ParabolaNotDownward, p2, att2, centerAtt2, 1, false, fi);
			if (fine2.action != SweepRetryAction::FineRefine || fine2.nextRange != uiFine)
			{
				std::fprintf(stderr,
					"self-test: step7 att2@200 must FineRefine (action=%d range=%d)\n",
					(int)fine2.action, fine2.nextRange);
				++fail;
			}
			const SweepRetryPlan plan3 = PlanNextRecal1DSweepAttempt(
				stMax, Peak1DValidateCode::ParabolaNotDownward, p3, att3, -160.0 + 200.0, 2, false, fi);
			if (plan3.action == SweepRetryAction::FineRefine)
			{
				std::fprintf(stderr, "self-test: step7 att3 must not FineRefine from coarse expanded\n");
				++fail;
			}
			double tStrict = 0.0;
			Peak1DValidateCode cStrict = Peak1DValidateCode::Ok;
			if (ParabolaVertexMax1D(att2, tStrict, cStrict, nullptr, Peak1DFitPolicy::Strict)
				&& cStrict == Peak1DValidateCode::Ok)
			{
				std::printf("self-test: step7 att2 Strict Ok under preprocess prominence (t=%.2f)\n", tStrict);
			}
			const SweepProfile flatShiftProf = AdjustProfileForFlatAtMaxShift(p2, nStep7);
			if (flatShiftProf.trend != SweepTrend::StrictDec)
			{
				std::fprintf(stderr, "self-test: step7 att2 FlatAtMaxShift profile must StrictDec (got %hs)\n",
					SweepTrendName(flatShiftProf.trend));
				++fail;
			}
			if (!IsInteriorPeakHint(p2, nStep7, fi))
			{
				std::fprintf(stderr, "self-test: step7 att2 must IsInteriorPeakHint for hint flat shift\n");
				++fail;
			}
			{
				const int hintBase = SuggestFlatAtMaxShiftBase(
					centerAtt2, p2, nStep7, 200, 1, fi);
				const int heuristicBase = SuggestSweepRecenterNewBase(
					centerAtt2, flatShiftProf, nStep7, 200, 1, fi);
				if (hintBase == heuristicBase)
				{
					std::fprintf(stderr,
						"self-test: step7 att2 hint flat shift must differ from heuristic (hint=%d heur=%d)\n",
						hintBase, heuristicBase);
					++fail;
				}
			}
			{
				SweepRecenterSessionState stOsc = stMax;
				stOsc.movingBase = -160;
				stOsc.flatShiftCount = 2;
				stOsc.oscillationDetected = true;
				stOsc.prevDeltaSign = -1;
				const SweepRetryPlan oscPlan = PlanNextRecal1DSweepAttempt(
					stOsc, Peak1DValidateCode::ParabolaNotDownward, p3, att3, -160.0 + 200.0, 3, false, fi);
				if (oscPlan.action != SweepRetryAction::FineRefine || oscPlan.nextRange != uiFine)
				{
					std::fprintf(stderr,
						"self-test: step7 ping-pong must escape to FineRefine (action=%d range=%d)\n",
						(int)oscPlan.action, oscPlan.nextRange);
					++fail;
				}
			}
			{
				SweepRecenterSessionState stSim = stMax;
				stSim.movingBase = -230;
				SweepRetryPlan sh1 = {};
				sh1.action = SweepRetryAction::FlatAtMaxShift;
				sh1.nextRange = 200;
				sh1.nextBase = -160;
				ApplySweepRetryPlan(stSim, sh1);
				SweepRetryPlan sh2 = {};
				sh2.action = SweepRetryAction::FlatAtMaxShift;
				sh2.nextRange = 200;
				sh2.nextBase = -230;
				ApplySweepRetryPlan(stSim, sh2);
				if (!DetectSweepRecenterOscillation(stSim))
				{
					std::fprintf(stderr, "self-test: alternating FlatAtMaxShift must detect oscillation\n");
					++fail;
				}
			}
		}
	}

	{
		// L1 frozen: non-ping-pong planner actions must stay unchanged.
		const int nFrozen = 33;
		const double commFlatFrozen[] = {
			-122228, -122243, -122228, -122207, -122212, -122217, -122216, -122259, -122296, -122275,
			-122263, -122254, -122234, -122230, -122225, -122228, -122214, -122212, -122209, -122192,
			-122193, -122183, -122180, -122173, -122165, -122161, -122151, -122142, -122131, -122140,
			-122132, -122134, -122123
		};
		std::vector<double> flatF(commFlatFrozen, commFlatFrozen + nFrozen);
		const SweepProfile pFlatF = AnalyzeRecal1DSweepProfile(flatF);
		SweepRecenterFailureInfo fiF = {};
		fiF.code = Peak1DValidateCode::ParabolaNotDownward;
		{
			std::vector<double> strictDecF((size_t)nFrozen);
			for (int i = 0; i < nFrozen; ++i)
				strictDecF[(size_t)i] = -130000.0 - i * 3000.0;
			const SweepProfile pDec = AnalyzeRecal1DSweepProfile(strictDecF);
			SweepRecenterSessionState st = {};
			InitSweepRecenterSessionState(st, uiFine, 9999);
			const SweepRetryPlan plan = PlanNextRecal1DSweepAttempt(
				st, Peak1DValidateCode::ParabolaNotDownward, pDec, strictDecF, -36.0 + 64.0, 0, false, fiF);
			if (plan.action != SweepRetryAction::MonoCoarseShift)
			{
				std::fprintf(stderr, "self-test frozen: StrictDec must MonoCoarseShift (got %d)\n", (int)plan.action);
				++fail;
			}
		}
		{
			SweepRecenterSessionState st = {};
			InitSweepRecenterSessionState(st, uiFine, 9999);
			const SweepRetryPlan jump = PlanNextRecal1DSweepAttempt(
				st, Peak1DValidateCode::ParabolaNotDownward, pFlatF, flatF, -31.0 + 64.0, 0, false, fiF);
			if (jump.action != SweepRetryAction::JumpFlatMax)
			{
				std::fprintf(stderr, "self-test frozen: Flat@64 must JumpFlatMax (got %d)\n", (int)jump.action);
				++fail;
			}
		}
		{
			SweepRecenterSessionState st = {};
			InitSweepRecenterSessionState(st, uiFine, 9999);
			st.flatJumpedToMax = true;
			st.attemptRange = 200;
			const SweepRetryPlan shift = PlanNextRecal1DSweepAttempt(
				st, Peak1DValidateCode::ParabolaNotDownward, pFlatF, flatF, -31.0 + 200.0, 1, false, fiF);
			if (shift.action != SweepRetryAction::FlatAtMaxShift)
			{
				std::fprintf(stderr, "self-test frozen: plateau Flat@200 must FlatAtMaxShift (got %d)\n", (int)shift.action);
				++fail;
			}
		}
	}

	return fail;
}

static int RunSweepRecenterSelfTests()
{
	int fail = RunSweepRetryPlannerSelfTests();
	fail += RunSweepPipelineSelfTests();
	const int dacRange = 64;
	const int n = 33;

	{
		std::vector<double> strictDec((size_t)n);
		for (int i = 0; i < n; ++i)
			strictDec[(size_t)i] = -130000.0 - i * 3000.0;
		const SweepProfile p = AnalyzeRecal1DSweepProfile(strictDec);
		const double delta0 = SuggestSweepRecenterDeltaDac(p, n, dacRange, 0);
		const double delta1 = SuggestSweepRecenterDeltaDac(p, n, dacRange, 1);
		if (p.trend != SweepTrend::StrictDec || delta0 >= 0.0 || std::abs(delta0) > 0.35 * dacRange + 1e-6)
		{
			std::fprintf(stderr, "self-test: StrictDec should yield negative bounded delta (got %.4g, %.4g)\n", delta0, delta1);
			++fail;
		}
		if (std::abs(delta1) + 1e-6 < std::abs(delta0))
		{
			std::fprintf(stderr, "self-test: StrictDec retry attempt should not shrink shift (got %.4g, %.4g)\n", delta0, delta1);
			++fail;
		}
		const double delta4 = SuggestSweepRecenterDeltaDac(p, n, dacRange, 4);
		if (std::abs(delta4) + 1e-6 < std::abs(delta0))
		{
			std::fprintf(stderr, "self-test: StrictDec attempt 4 should exceed attempt 0 (got %.4g, %.4g)\n", delta0, delta4);
			++fail;
		}
		if (!IsRetryablePeakFailure(Peak1DValidateCode::ParabolaNotDownward, p, n))
		{
			std::fprintf(stderr, "self-test: StrictDec ParabolaNotDownward should be retryable\n");
			++fail;
		}
		if (!IsMonotoneSweepFailure(Peak1DValidateCode::ParabolaNotDownward, p, n))
		{
			std::fprintf(stderr, "self-test: StrictDec must be IsMonotoneSweepFailure\n");
			++fail;
		}
		if (SuggestJumpMaxDacRange(dacRange, 200) != 200)
		{
			std::fprintf(stderr, "self-test: StrictDec path uses jump-max coarse range 200\n");
			++fail;
		}
	}

	{
		std::vector<double> strictInc((size_t)n);
		for (int i = 0; i < n; ++i)
			strictInc[(size_t)i] = -211300.0 + i * 3000.0;
		const SweepProfile p = AnalyzeRecal1DSweepProfile(strictInc);
		const double delta0 = SuggestSweepRecenterDeltaDac(p, n, dacRange, 0);
		if (p.trend != SweepTrend::StrictInc || delta0 <= 0.0)
		{
			std::fprintf(stderr, "self-test: StrictInc should yield positive delta (got %.4g)\n", delta0);
			++fail;
		}
		if (!IsMonotoneSweepFailure(Peak1DValidateCode::ParabolaNotDownward, p, n))
		{
			std::fprintf(stderr, "self-test: StrictInc must be IsMonotoneSweepFailure\n");
			++fail;
		}
	}

	{
		// Edge NonMono (argmax=0): small bumps break StrictDec but keep max at index 0.
		std::vector<double> edgeNonMono((size_t)n);
		edgeNonMono[0] = -120000.0;
		for (int i = 1; i < n; ++i)
			edgeNonMono[(size_t)i] = -130000.0 - i * 800.0 + ((i % 4 == 0) ? 120.0 : 0.0);
		const SweepProfile pEdge = AnalyzeRecal1DSweepProfile(edgeNonMono);
		if (pEdge.argmaxIndex > 1)
		{
			std::fprintf(stderr, "self-test: edge NonMono fixture argmax=%d trend=%s\n",
				pEdge.argmaxIndex, SweepTrendName(pEdge.trend));
			++fail;
		}
		if (pEdge.trend == SweepTrend::Flat)
		{
			std::fprintf(stderr, "self-test: edge NonMono fixture must not be Flat\n");
			++fail;
		}
		if (!IsMonotoneSweepFailure(Peak1DValidateCode::ParabolaNotDownward, pEdge, n))
		{
			std::fprintf(stderr, "self-test: edge NonMono must be IsMonotoneSweepFailure\n");
			++fail;
		}
		std::vector<double> midNonMono((size_t)n, -130000.0);
		midNonMono[(size_t)(n / 2)] = -120000.0;
		const SweepProfile pMid = AnalyzeRecal1DSweepProfile(midNonMono);
		if (IsMonotoneSweepFailure(Peak1DValidateCode::ParabolaNotDownward, pMid, n))
		{
			std::fprintf(stderr, "self-test: mid NonMono must not be IsMonotoneSweepFailure\n");
			++fail;
		}
	}

	{
		// PlanRecalYCrossResweep: pipeline recenter (no expand); ioDacRange stays ui fine.
		int baseYMono = -127;
		int offsetMono = 64;
		int prevArgMono = -1;
		bool usedExpandMono = false;
		bool monoExpandedCross = false;
		std::vector<double> strictDecCross((size_t)n);
		for (int i = 0; i < n; ++i)
			strictDecCross[(size_t)i] = -130000.0 - i * 3000.0;
		const SweepProfile pMonoCross = AnalyzeRecal1DSweepProfile(strictDecCross);
		if (pMonoCross.trend != SweepTrend::StrictDec)
		{
			std::fprintf(stderr, "self-test: strictDecCross fixture trend=%s\n", SweepTrendName(pMonoCross.trend));
			++fail;
		}
		if (!PlanRecalYCrossResweep(
				Peak1DValidateCode::ParabolaNotDownward,
				strictDecCross,
				-132.0,
				offsetMono,
				0,
				baseYMono,
				offsetMono,
				prevArgMono,
				0.0,
				false,
				usedExpandMono,
				monoExpandedCross)
			|| usedExpandMono || offsetMono != 64 || baseYMono == -127)
		{
			std::fprintf(stderr,
				"self-test: PlanRecalYCrossResweep StrictDec should recenter @64 (expand=%d offset=%d baseY=%d)\n",
				usedExpandMono ? 1 : 0, offsetMono, baseYMono);
			++fail;
		}
	}

	{
		// comm.log L7622 flat row (col0=-31): Flat profile; mono recenter N/A, flat expand applies.
		const double commFlat[] = {
			-122228, -122243, -122228, -122207, -122212, -122217, -122216, -122259, -122296, -122275,
			-122263, -122254, -122234, -122230, -122225, -122228, -122214, -122212, -122209, -122192,
			-122193, -122183, -122180, -122173, -122165, -122161, -122151, -122142, -122131, -122140,
			-122132, -122134, -122123
		};
		std::vector<double> flat(commFlat, commFlat + n);
		const SweepProfile p = AnalyzeRecal1DSweepProfile(flat);
		if (p.trend != SweepTrend::Flat)
		{
			std::fprintf(stderr, "self-test: comm.log flat row must be Flat\n");
			++fail;
		}
		if (!IsFlatSweepFailure(Peak1DValidateCode::ParabolaNotDownward, p))
		{
			std::fprintf(stderr, "self-test: comm.log flat row must be IsFlatSweepFailure\n");
			++fail;
		}
		if (IsRetryablePeakFailure(Peak1DValidateCode::ParabolaNotDownward, p, n))
		{
			std::fprintf(stderr, "self-test: Flat must not use mono IsRetryablePeakFailure\n");
			++fail;
		}
		if (SuggestFlatRetryDacRange(64, 200) != 128 || SuggestFlatRetryDacRange(128, 200) != 200
			|| SuggestFlatRetryDacRange(200, 200) != 0)
		{
			std::fprintf(stderr, "self-test: SuggestFlatRetryDacRange 64->128->200\n");
			++fail;
		}
	}

	{
		// Y pre passed but cross fit rejected: recenter base, keep ui offset for pipeline restart.
		int baseY = -127;
		int offset = 64;
		int prevArg = -1;
		bool usedExpand = false;
		bool monoRangeExpanded = false;
		static const double kBellY[] = {
			-246880, -246853, -246833, -246818, -246797, -246775, -246777, -246756, -246752, -246737,
			-246736, -246734, -246724, -246730, -246733, -246721, -246720, -246719, -246720, -246723,
			-246725, -246730, -246725, -246740, -246738, -246736, -246744, -246740, -246752, -246756,
			-246756, -246749, -246747,
		};
		const size_t nBell = sizeof(kBellY) / sizeof(kBellY[0]);
		std::vector<double> bellY(kBellY, kBellY + nBell);
		if (!PlanRecalYCrossResweep(
				Peak1DValidateCode::ParabolaNotDownward,
				bellY,
				-132.0,
				offset,
				0,
				baseY,
				offset,
				prevArg,
				15.0,
				true,
				usedExpand,
				monoRangeExpanded)
			|| usedExpand || offset != 64 || baseY == -127)
		{
			std::fprintf(stderr,
				"self-test: PlanRecalYCrossResweep shallow cross-fail should recenter (expand=%d offset=%d baseY=%d)\n",
				usedExpand ? 1 : 0, offset, baseY);
			++fail;
		}
	}

	{
		// Flat cross fail @200: still plans recenter for next pipeline round.
		int baseYFlat = -127;
		int offsetFlat = 64;
		int prevArgFlat = -1;
		bool usedExpandFlat = false;
		bool monoRangeExpandedFlat = false;
		static const double kFlatCross[] = {
			-122228, -122243, -122228, -122207, -122212, -122217, -122216, -122259, -122296, -122275,
			-122263, -122254, -122234, -122230, -122225, -122228, -122214, -122212, -122209, -122192,
			-122193, -122183, -122180, -122173, -122165, -122161, -122151, -122142, -122131, -122140,
			-122132, -122134, -122123
		};
		const size_t nFlatCross = sizeof(kFlatCross) / sizeof(kFlatCross[0]);
		std::vector<double> flatCross(kFlatCross, kFlatCross + nFlatCross);
		if (!PlanRecalYCrossResweep(
				Peak1DValidateCode::ParabolaNotDownward,
				flatCross,
				-231.0,
				200,
				2,
				baseYFlat,
				offsetFlat,
				prevArgFlat,
				0.0,
				false,
				usedExpandFlat,
				monoRangeExpandedFlat)
			|| offsetFlat != 64)
		{
			std::fprintf(stderr, "self-test: PlanRecalYCrossResweep flat@200 should recenter keep offset=64\n");
			++fail;
		}
	}

	{
		// User RECAL 3 0 flat Y sweep (2026-06-03, col0=-132 stripped).
		static const double kUserFlatY[] = {
			-246880, -246853, -246833, -246818, -246797, -246775, -246777, -246756, -246752, -246737,
			-246736, -246734, -246724, -246730, -246733, -246721, -246720, -246719, -246720, -246723,
			-246725, -246730, -246725, -246740, -246738, -246736, -246744, -246740, -246752, -246756,
			-246756, -246749, -246747,
		};
		const size_t nUser = sizeof(kUserFlatY) / sizeof(kUserFlatY[0]);
		std::vector<double> userFlat(kUserFlatY, kUserFlatY + nUser);
		const SweepProfile pu = AnalyzeRecal1DSweepProfile(userFlat);
		Peak1DValidateCode cUser = Peak1DValidateCode::Ok;
		double tUser = 0.0;
		(void)M576::ParabolaVertexMax1D(userFlat, tUser, cUser);
		if (pu.trend != SweepTrend::Flat || !IsFlatSweepFailure(cUser, pu))
		{
			std::fprintf(stderr, "self-test: user flat RECAL3 Y sweep must be Flat + IsFlatSweepFailure\n");
			++fail;
		}
	}

	{
		// comm 2026-06-03 14:05:35: offset=64 Flat -> expand; offset=128 plateau+StrictDec tail -> mono recenter @128.
		static const double kFlat64[] = {
			-259436, -259407, -259388, -259385, -259367, -259333, -259322, -259306, -259297, -259297,
			-259286, -259286, -259287, -259284, -259271, -259278, -259269, -259263, -259284, -259276,
			-259279, -259286, -259292, -259312, -259314, -259327, -259337, -259348, -259362, -259412,
			-259441, -259461, -259495,
		};
		static const double kExpand128[] = {
			-259075, -259081, -259074, -259081, -259066, -259078, -259077, -259081, -259084, -259073,
			-259066, -259078, -259078, -259087, -259085, -259071, -259082, -259080, -259088, -259108,
			-259091, -259094, -259093, -259103, -259085, -259095, -259087, -259110, -259112, -259112,
			-259115, -259114, -259124, -259138, -259136, -259133, -259144, -259161, -259170, -259184,
			-259200, -259228, -259234, -259257, -259279, -259297, -259323, -259349, -259368, -259410,
			-259442, -259481, -259519, -259563, -259606, -259650, -259719, -259773, -259830, -259882,
			-259947, -260020, -260106, -260176, -260270,
		};
		const size_t nFlat64 = sizeof(kFlat64) / sizeof(kFlat64[0]);
		const size_t nExp128 = sizeof(kExpand128) / sizeof(kExpand128[0]);
		std::vector<double> flat64(kFlat64, kFlat64 + nFlat64);
		std::vector<double> expand128(kExpand128, kExpand128 + nExp128);
		AmplifyRecalSweepSpanInPlace(expand128);
		const SweepProfile pFlat = AnalyzeRecal1DSweepProfile(flat64);
		Peak1DValidateCode cFlat = Peak1DValidateCode::Ok;
		double tFlat = 0.0;
		(void)M576::ParabolaVertexMax1D(flat64, tFlat, cFlat);
		if (pFlat.trend != SweepTrend::Flat || !IsFlatSweepFailure(cFlat, pFlat))
		{
			std::fprintf(stderr, "self-test: comm 2026-06-03 offset=64 must be Flat failure\n");
			++fail;
		}
		if (SuggestJumpMaxDacRange(64, 200) != 200)
		{
			std::fprintf(stderr, "self-test: flat64 should jump-max 64->200\n");
			++fail;
		}

		const SweepProfile pExp = AnalyzeRecal1DSweepProfile(expand128);
		Peak1DValidateCode cExp = Peak1DValidateCode::Ok;
		double tExp = 0.0;
		int idxExp = 0;
		(void)M576::FindUnimodalPeak1DIndex(expand128, idxExp, cExp, &tExp, nullptr);
		if (IsRetryablePeakFailure(cExp, pExp, (int)nExp128))
		{
			std::fprintf(stderr, "self-test: expand128 without flat flag must not base-retry (trend=%s argmax=%d)\n",
				SweepTrendName(pExp.trend), pExp.argmaxIndex);
			++fail;
		}
		if (cExp == Peak1DValidateCode::Ok)
		{
			// Preprocess spanAll gate: amplified plateau+StrictDec tail may Strict Ok; recenter not required.
		}
		else
		{
			if (!IsRetryablePeakFailure(cExp, pExp, (int)nExp128, true, &expand128))
			{
				std::fprintf(stderr, "self-test: expand128 after flat expand must allow mono recenter\n");
				++fail;
			}
			const SweepProfile adj = AdjustProfileForMonoRecenter(pExp, expand128, true);
			if (adj.trend != SweepTrend::StrictDec)
			{
				std::fprintf(stderr, "self-test: expand128 post-flat should map to StrictDec (got %s)\n",
					SweepTrendName(adj.trend));
				++fail;
			}
			const double delta128 = SuggestSweepRecenterDeltaDac(adj, (int)nExp128, 128, 0);
			if (delta128 >= 0.0)
			{
				std::fprintf(stderr, "self-test: expand128 StrictDec recenter should shift left (delta=%.4g)\n", delta128);
				++fail;
			}
		}
	}

	{
		// comm 2026-06-03 Step 206: plateau + -999999 + StrictDec tail, VertexOutOfRange t*<0 (col0=154).
		static const double kStep206[] = {
			-240714, -240713, -240716, -240715, -240719, -240724, -240738, -240747, -240765, -240777,
			-240809, -240832, -240862, -240901, -240940, -240998, -999999, -241122, -241180, -241252,
			-241340, -241423, -241519, -241621, -241727, -241850, -241980, -242117, -242259, -242415,
			-242574, -242754, -242943,
		};
		const size_t n206 = sizeof(kStep206) / sizeof(kStep206[0]);
		std::vector<double> step206(kStep206, kStep206 + n206);
		AmplifyRecalSweepSpanInPlace(step206);
		const SweepProfile p206 = AnalyzeRecal1DSweepProfile(step206);
		Peak1DValidateCode c206Fit = Peak1DValidateCode::Ok;
		double t206Fit = 0.0;
		int idx206 = 0;
		(void)M576::FindUnimodalPeak1DIndex(step206, idx206, c206Fit, &t206Fit, nullptr);
		// Runtime log: VertexOutOfRange + t*<0; static fit may differ — use log failure shape for gated path.
		const Peak1DValidateCode c206 = Peak1DValidateCode::VertexOutOfRange;
		const double t206 = -2.5;
		if (p206.argmaxIndex > 1 && p206.argmaxIndex <= (int)n206 / 3
			&& IsRetryablePeakFailure(c206, p206, (int)n206))
		{
			std::fprintf(stderr,
				"self-test: Step206 interior argmax must not retry without failure (argmax=%d)\n",
				p206.argmaxIndex);
			++fail;
		}
		SweepRecenterFailureInfo fail206 = {};
		fail206.code = c206;
		fail206.tPeak = t206;
		fail206.hasTPeak = true;
		if (!IsRetryablePeakFailure(c206, p206, (int)n206, false, &step206, &fail206))
		{
			std::fprintf(stderr, "self-test: Step206 plateau+left-outside must be retryable (fit=%d argmax=%d)\n",
				(int)c206Fit, p206.argmaxIndex);
			++fail;
		}
		const SweepProfile adj206 = AdjustProfileForMonoRecenter(p206, step206, false, &fail206);
		if (adj206.trend != SweepTrend::StrictDec || adj206.argmaxIndex != 0)
		{
			std::fprintf(stderr, "self-test: Step206 should map to StrictDec@0 (trend=%s argmax=%d)\n",
				SweepTrendName(adj206.trend), adj206.argmaxIndex);
			++fail;
		}
		const double delta206 = SuggestSweepRecenterDeltaDac(adj206, (int)n206, 64, 0, fail206);
		if (delta206 >= 0.0)
		{
			std::fprintf(stderr, "self-test: Step206 recenter should shift left (delta=%.4g)\n", delta206);
			++fail;
		}
	}

	{
		// comm 2026-06-03 Step 478: left-edge argmax + long StrictDec tail, VertexOutOfRange t*>n-1 (col0=110).
		static const double kStep478[] = {
			-258071, -258054, -258070, -258063, -258095, -258102, -258122, -258134, -258158, -258179,
			-258214, -258251, -258278, -258332, -258371, -258421, -258492, -258549, -258610, -258685,
			-258763, -258850, -258939, -259042, -259158, -259288, -259399, -259533, -259674, -259822,
			-259986, -260154, -260324,
		};
		const size_t n478 = sizeof(kStep478) / sizeof(kStep478[0]);
		std::vector<double> step478(kStep478, kStep478 + n478);
		AmplifyRecalSweepSpanInPlace(step478);
		const SweepProfile p478 = AnalyzeRecal1DSweepProfile(step478);
		Peak1DValidateCode c478Fit = Peak1DValidateCode::Ok;
		double t478Fit = 0.0;
		int idx478 = 0;
		(void)M576::FindUnimodalPeak1DIndex(step478, idx478, c478Fit, &t478Fit, nullptr);
		if (c478Fit == Peak1DValidateCode::ParabolaNotDownward)
		{
			// Raw prominence gate may reject before cubic vertex; use comm-log exterior t* for recenter chain.
			c478Fit = Peak1DValidateCode::VertexOutOfRange;
			t478Fit = (double)n478 + 5.0;
		}
		else if (c478Fit != Peak1DValidateCode::VertexOutOfRange)
		{
			std::fprintf(stderr, "self-test: Step478 fit should be VertexOutOfRange (got %d t=%.4g)\n",
				(int)c478Fit, t478Fit);
			++fail;
		}
		const bool leftOut478 = std::isfinite(t478Fit) && t478Fit < -0.01;
		const bool rightOut478 = std::isfinite(t478Fit) && t478Fit > (double)(n478 - 1) + 0.01;
		if (!leftOut478 && !rightOut478)
		{
			std::fprintf(stderr, "self-test: Step478 t* should be outside window (t=%.4g n=%zu)\n",
				t478Fit, n478);
			++fail;
		}
		if (p478.argmaxIndex > 1 && IsRetryablePeakFailure(c478Fit, p478, (int)n478))
		{
			std::fprintf(stderr,
				"self-test: Step478 interior argmax must not retry without failure (argmax=%d)\n",
				p478.argmaxIndex);
			++fail;
		}
		SweepRecenterFailureInfo fail478 = {};
		fail478.code = Peak1DValidateCode::VertexOutOfRange;
		fail478.tPeak = t478Fit;
		fail478.hasTPeak = true;
		if (!IsRetryablePeakFailure(c478Fit, p478, (int)n478, false, &step478, &fail478))
		{
			std::fprintf(stderr, "self-test: Step478 vertex-outside+left-argmax must retry (argmax=%d t=%.4g)\n",
				p478.argmaxIndex, t478Fit);
			++fail;
		}
		const SweepProfile adj478 = AdjustProfileForMonoRecenter(p478, step478, false, &fail478);
		const double delta478 = SuggestSweepRecenterDeltaDac(adj478, (int)n478, 64, 0, fail478);
		if (leftOut478)
		{
			if (adj478.trend != SweepTrend::StrictDec || adj478.argmaxIndex != 0)
			{
				std::fprintf(stderr, "self-test: Step478 left-outside -> StrictDec@0 (trend=%s argmax=%d)\n",
					SweepTrendName(adj478.trend), adj478.argmaxIndex);
				++fail;
			}
			if (delta478 >= 0.0)
			{
				std::fprintf(stderr, "self-test: Step478 left-outside should shift left (delta=%.4g)\n", delta478);
				++fail;
			}
		}
		else if (rightOut478)
		{
			if (adj478.trend != SweepTrend::StrictInc || adj478.argmaxIndex != (int)n478 - 1)
			{
				std::fprintf(stderr, "self-test: Step478 right-outside -> StrictInc@%d (trend=%s argmax=%d)\n",
					(int)n478 - 1, SweepTrendName(adj478.trend), adj478.argmaxIndex);
				++fail;
			}
			if (delta478 <= 0.0)
			{
				std::fprintf(stderr, "self-test: Step478 right-outside should shift right (delta=%.4g)\n", delta478);
				++fail;
			}
		}

		// Synthetic: t*>n-1 + left argmax (Step 478 class when fit reports right-outside vertex).
		SweepProfile pSyn = p478;
		pSyn.argmaxIndex = 1;
		SweepRecenterFailureInfo failSyn = {};
		failSyn.code = Peak1DValidateCode::VertexOutOfRange;
		failSyn.tPeak = (double)n478 + 5.0;
		failSyn.hasTPeak = true;
		if (!IsRetryablePeakFailure(Peak1DValidateCode::VertexOutOfRange, pSyn, (int)n478, false, &step478, &failSyn))
		{
			std::fprintf(stderr, "self-test: synthetic right-outside+left-argmax must retry\n");
			++fail;
		}
		if (IsRetryablePeakFailure(Peak1DValidateCode::VertexOutOfRange, pSyn, (int)n478, false, &step478, nullptr))
		{
			std::fprintf(stderr, "self-test: synthetic right-outside must not retry without failure t*\n");
			++fail;
		}
		const SweepProfile adjSyn = AdjustProfileForMonoRecenter(pSyn, step478, false, &failSyn);
		const double deltaSyn = SuggestSweepRecenterDeltaDac(adjSyn, (int)n478, 64, 0, failSyn);
		if (adjSyn.trend != SweepTrend::StrictInc || adjSyn.argmaxIndex != (int)n478 - 1 || deltaSyn <= 0.0)
		{
			std::fprintf(stderr,
				"self-test: synthetic right-outside -> StrictInc shift right (trend=%s delta=%.4g)\n",
				SweepTrendName(adjSyn.trend), deltaSyn);
			++fail;
		}
		// Interior argmax + right-outside t* must not retry (ambiguous direction).
		SweepProfile pMid = p478;
		pMid.argmaxIndex = (int)n478 / 2;
		if (IsRetryablePeakFailure(Peak1DValidateCode::VertexOutOfRange, pMid, (int)n478, false, &step478, &failSyn))
		{
			std::fprintf(stderr, "self-test: right-outside+mid argmax must not retry\n");
			++fail;
		}
	}

	{
		std::vector<double> edge((size_t)n);
		for (int i = 0; i < n; ++i)
			edge[(size_t)i] = -250000.0 - 30000.0 * (double)(i * i);
		const SweepProfile p = AnalyzeRecal1DSweepProfile(edge);
		const double delta = SuggestSweepRecenterDeltaDac(p, n, dacRange, 0);
		if (p.argmaxIndex != 0 || delta >= 0.0)
		{
			std::fprintf(stderr, "self-test: edge argmax=0 should shift left (delta=%.4g)\n", delta);
			++fail;
		}
		if (!IsRetryablePeakFailure(Peak1DValidateCode::EdgeNotAllowed, p, n))
		{
			std::fprintf(stderr, "self-test: edge argmax should be retryable\n");
			++fail;
		}
	}

	{
		std::vector<double> strictDec((size_t)n);
		for (int i = 0; i < n; ++i)
			strictDec[(size_t)i] = -130000.0 - i * 3000.0;
		const SweepProfile p = AnalyzeRecal1DSweepProfile(strictDec);
		SweepRecenterFailureInfo failInfo = {};
		failInfo.code = Peak1DValidateCode::ParabolaNotDownward;
		failInfo.hasPrevAttempt = true;
		failInfo.prevArgmaxIndex = 0;
		const double dPlain = SuggestSweepRecenterDeltaDac(p, n, dacRange, 1);
		const double dStag = SuggestSweepRecenterDeltaDac(p, n, dacRange, 1, failInfo);
		if (std::abs(dStag) + 1e-6 < std::abs(dPlain))
		{
			std::fprintf(stderr, "self-test: stagnation should amplify shift (%.4g vs %.4g)\n", dPlain, dStag);
			++fail;
		}
	}

	{
		const int dacNarrow = 16;
		std::vector<double> strictInc((size_t)n);
		for (int i = 0; i < n; ++i)
			strictInc[(size_t)i] = -211300.0 + i * 3000.0;
		const SweepProfile p = AnalyzeRecal1DSweepProfile(strictInc);
		SweepRecenterFailureInfo failInfo = {};
		failInfo.code = Peak1DValidateCode::VertexOutOfRange;
		failInfo.tPeak = 16.0;
		failInfo.hasTPeak = true;
		const double dHeur = SuggestSweepRecenterDeltaDac(p, n, dacNarrow, 0);
		const double dLearn = SuggestSweepRecenterDeltaDac(p, n, dacNarrow, 0, failInfo);
		if (std::abs(dLearn - dHeur) < 0.25)
		{
			std::fprintf(stderr, "self-test: t* hint should change shift vs heuristic (heur=%.4g learn=%.4g)\n", dHeur, dLearn);
			++fail;
		}
	}

	{
		// User log 2026-06-01: col0=-198, offset=64 -> center=-134; newBase must stay negative (~-156), not 0.
		const double col0 = -198.0;
		const double centerDac = col0 + (double)dacRange;
		SweepProfile p = {};
		p.trend = SweepTrend::NonMono;
		p.argmaxIndex = 1;
		p.span = 803.0;
		p.validCount = n;
		const int newBase = SuggestSweepRecenterNewBase(centerDac, p, n, dacRange, 0);
		if (newBase >= 0)
		{
			std::fprintf(stderr,
				"self-test: signed RECAL base from col0=-198 must be negative (got %d, expected ~-156)\n",
				newBase);
			++fail;
		}
		if (std::abs(newBase + 156) > 3)
		{
			std::fprintf(stderr,
				"self-test: signed RECAL base from col0=-198 expected ~-156 (got %d)\n",
				newBase);
			++fail;
		}
	}

	return fail;
}

/// Assert SplitSweepCsvRow accepts legacy cmd-first rows and new path,cmd,attempt,peak_ok,code rows.
static int RunSweepCsvSplitSelfTests()
{
	int fail = 0;
	auto check = [&](const char* label, const char* line, const char* expectCmdPrefix, size_t minNums)
	{
		std::string cmd;
		std::vector<double> nums;
		if (!SplitSweepCsvRow(line, cmd, nums))
		{
			std::fprintf(stderr, "self-test sweep-csv: %s parse failed\n", label);
			++fail;
			return;
		}
		if (cmd.rfind(expectCmdPrefix, 0) != 0 || nums.size() < minNums)
		{
			std::fprintf(stderr,
				"self-test sweep-csv: %s mismatch cmd=%s nums=%zu\n",
				label,
				cmd.c_str(),
				nums.size());
			++fail;
		}
	};

	check(
		"legacy",
		"RECAL 3 0 9999 64 4 80,-2322,-2311,-2300",
		"RECAL 3",
		3);
	check(
		"path-ext",
		"pm_pm_mcs1|slot=1|step=1/128|RECAL 1 4 13 1 1 45|tgt=4 c1=13,"
		"RECAL 3 0 9999 9999 64 4 80,2,0,ParabolaNotDownward,-2322,-2311,-2300",
		"RECAL 3",
		3);
	return fail;
}

/// Locate sweep CSV beside exe, CrossPeakTest source dir, or cwd.
static std::ifstream OpenCrossPeakTestDataFile(const char* filename, std::string& outOpenedPath)
{
	outOpenedPath.clear();
	std::vector<std::string> candidates;
	{
		const std::string besideExe = PathToExeDirFile(filename);
		if (!besideExe.empty())
			candidates.push_back(besideExe);
	}
	{
		char module[MAX_PATH];
		DWORD n = GetModuleFileNameA(nullptr, module, MAX_PATH);
		if (n > 0 && n < MAX_PATH)
		{
			char* slash = strrchr(module, '\\');
			if (slash)
			{
				*slash = '\0';
				char* slash2 = strrchr(module, '\\');
				if (slash2)
				{
					*(slash2 + 1) = '\0';
					std::string parentDir(module);
					parentDir += filename;
					candidates.push_back(parentDir);
				}
			}
		}
	}
	candidates.push_back(filename);
	candidates.push_back(std::string("..\\CrossPeakTest\\") + filename);
	for (const std::string& path : candidates)
	{
		std::ifstream f(path, std::ios::binary);
		if (f)
		{
			outOpenedPath = path;
			return f;
		}
	}
	return std::ifstream();
}

/// Batch replay comm_2026-07-01_recal_sweeps.csv: spot-check known good peaks; log fit_ok vs csv_peak_ok.
static int RunComm20260701SweepRegressionTests()
{
	int fail = 0;
	std::string csvPath;
	std::ifstream f = OpenCrossPeakTestDataFile("comm_2026-07-01_recal_sweeps.csv", csvPath);
	if (!f)
	{
		std::fprintf(stderr, "self-test sweeps-regression: cannot open comm_2026-07-01_recal_sweeps.csv\n");
		return 1;
	}

	M576::Peak1DResetMinProminenceDb();

	int lineNo = 0;
	int csvPeakOk = 0;
	int fitOk = 0;
	std::string line;
	while (std::getline(f, line))
	{
		++lineNo;
		while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
			line.pop_back();
		if (line.empty())
			continue;
		if (lineNo == 1)
		{
			std::string lower = line;
			for (char& c : lower)
				if (c >= 'A' && c <= 'Z')
					c = (char)(c - 'A' + 'a');
			if (lower.find("path") != std::string::npos)
				continue;
		}

		std::vector<std::string> fields;
		if (!SplitCsvFieldsSimple(line, fields))
			continue;
		for (std::string& fld : fields)
			TrimInPlace(fld);
		if (fields.size() < 6)
			continue;

		int csvAttempt = 0;
		int csvPeakOkFlag = 0;
		try
		{
			if (fields.size() > 2)
				csvAttempt = std::stoi(fields[2]);
			if (fields.size() > 3)
				csvPeakOkFlag = std::stoi(fields[3]);
		}
		catch (...)
		{
			continue;
		}
		if (csvPeakOkFlag == 1)
			++csvPeakOk;

		std::string cmd;
		std::vector<double> nums;
		if (!SplitSweepCsvRow(line, cmd, nums) || nums.size() < 2)
			continue;
		std::vector<double> powers(nums.begin() + 1, nums.end());

		double tStar = 0;
		Peak1DValidateCode code = Peak1DValidateCode::Ok;
		const bool ok = M576::ParabolaVertexMax1D(powers, tStar, code);
		if (ok)
			++fitOk;

		auto expectOk = [&](const char* label)
		{
			if (!ok || code != Peak1DValidateCode::Ok)
			{
				std::fprintf(stderr,
					"self-test sweeps-regression: %s must Ok (line=%d ok=%d code=%s)\n",
					label,
					lineNo,
					ok ? 1 : 0,
					Peak1DCodeName(code));
				++fail;
			}
		};
		if (lineNo == 3)
			expectOk("step1 attempt2");
		if (lineNo == 4)
			expectOk("step1 attempt3");
		if (lineNo == 1718)
			expectOk("step242 slot2 attempt1");
	}

	std::printf("sweeps-regression: lines=%d csv_peak_ok=%d fit_ok=%d (fit/csv=%.3f)\n",
		lineNo, csvPeakOk, fitOk, csvPeakOk > 0 ? (double)fitOk / (double)csvPeakOk : 0.0);
	return fail;
}

/// Diagnostic replay of comm_2026-07-02: per VertexOutOfRange row, report trend/argmax/retry gates (no algorithm change).
static int RunComm20260702VertexOutOfRangeDiagnostics()
{
	std::string csvPath;
	std::ifstream f = OpenCrossPeakTestDataFile("comm_2026-07-02_recal_sweeps.csv", csvPath);
	if (!f)
	{
		std::fprintf(stderr, "vor-diag: cannot open comm_2026-07-02_recal_sweeps.csv\n");
		return 1;
	}

	M576::Peak1DResetMinProminenceDb();

	std::printf("\n=== comm_2026-07-02 VertexOutOfRange diagnostics (%s) ===\n", csvPath.c_str());
	const double minSpanRaw = M576::Peak1DMinFlatSpanRaw();

	std::string curPath;
	SweepRecenterSessionState session = {};
	int movingBase = 0;
	int attemptDacRange = 64;
	const int uiFineRange = 64;
	bool sessionInited = false;

	int lineNo = 0;
	int vorRows = 0;
	int vorGiveUp = 0;
	int vorEarlyGiveUp = 0;
	std::string line;
	while (std::getline(f, line))
	{
		++lineNo;
		while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
			line.pop_back();
		if (line.empty())
			continue;
		if (lineNo == 1)
		{
			std::string lower = line;
			for (char& c : lower)
				if (c >= 'A' && c <= 'Z')
					c = (char)(c - 'A' + 'a');
			if (lower.find("path") != std::string::npos)
				continue;
		}

		std::vector<std::string> fields;
		if (!SplitCsvFieldsSimple(line, fields))
			continue;
		for (std::string& fld : fields)
			TrimInPlace(fld);
		if (fields.size() < 5)
			continue;

		const std::string& pathField = fields[0];
		if (pathField != curPath)
		{
			curPath = pathField;
			sessionInited = false;
		}

		std::string cmd;
		std::vector<double> nums;
		if (!SplitSweepCsvRow(line, cmd, nums) || nums.size() < 2)
			continue;

		RecalSweepCmdFields cf;
		ParseRecalSweepCmd(cmd, cf);
		if (!cf.ok || cf.sweepMode != 0)
			continue;

		int csvAttempt = 1;
		std::string csvCodeName;
		try
		{
			csvAttempt = std::stoi(fields[2]);
			csvCodeName = fields[4];
		}
		catch (...)
		{
			continue;
		}

		const double col0 = nums[0];
		std::vector<double> powers(nums.begin() + 1, nums.end());
		const int n = (int)powers.size();
		if (n <= 0)
			continue;

		if (!sessionInited)
		{
			InitSweepRecenterSessionState(session, uiFineRange, cf.baseY);
			movingBase = cf.baseY;
			attemptDacRange = cf.offsetDac;
			sessionInited = true;
		}

		movingBase = cf.baseY;
		attemptDacRange = cf.offsetDac;
		session.movingBase = movingBase;
		session.attemptRange = attemptDacRange;

		const M576::Peak1DFitPolicy fitPolicy = IsFineRefineSweepAttempt(session)
			? M576::Peak1DFitPolicy::FineRefineRelaxed
			: M576::Peak1DFitPolicy::Strict;

		double tStar = 0.0;
		int peakIdx = 0;
		Peak1DValidateCode fitCode = Peak1DValidateCode::Ok;
		const bool fitOk = FindUnimodalPeak1DIndex(powers, peakIdx, fitCode, &tStar, nullptr, fitPolicy);
		const SweepProfile prof = AnalyzeRecal1DSweepProfile(powers);
		const Peak1DValidateCode csvCode = Peak1DCodeFromCsvName(csvCodeName);
		const Peak1DValidateCode code = fitOk ? Peak1DValidateCode::Ok : fitCode;

		const bool isVorRow = (csvCode == Peak1DValidateCode::VertexOutOfRange
			|| code == Peak1DValidateCode::VertexOutOfRange);

		SweepRecenterFailureInfo failInfo = {};
		failInfo.code = code;
		failInfo.tPeak = tStar;
		failInfo.hasTPeak = std::isfinite(tStar);
		failInfo.prevArgmaxIndex = session.prevArgmax;
		failInfo.hasPrevAttempt = (csvAttempt > 1);

		const bool lastAttempt = (csvAttempt >= (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS);
		const bool afterExpand = session.flatJumpedToMax || session.inCoarsePhase
			|| session.attemptRange >= M576_MAX_DAC_RANGE;
		const bool retryableBase = IsRetryablePeakFailure(code, prof, n, false, nullptr, &failInfo);
		const bool retryableExpand = IsRetryablePeakFailure(code, prof, n, afterExpand, &powers, &failInfo);
		const double centerDac = RecalSweepCenterFromCol0Diag(col0, attemptDacRange);
		const SweepRetryPlan plan = PlanNextRecal1DSweepAttempt(
			session, code, prof, powers, centerDac, csvAttempt - 1, lastAttempt, failInfo);

		if (isVorRow)
		{
			++vorRows;
			if (plan.action == SweepRetryAction::GiveUp)
			{
				++vorGiveUp;
				if (!lastAttempt)
					++vorEarlyGiveUp;
			}
			const std::string reason = ExplainVorNoRetryReason(
				code, prof, n, afterExpand, powers, failInfo, session, plan, lastAttempt);
			std::printf(
				"line=%d step=%s att=%d offset=%d baseY=%d n=%d\n"
				"  csv_code=%s fit_code=%s fit_ok=%d t*=%.4g argmax=%d trend=%s span=%.4g (minSpanRaw=%.4g flat=%d)\n"
				"  session: flatJump=%d coarse=%d fineUsed=%d flatShift=%d osc=%d afterExpand=%d\n"
				"  retryable(base=%d expand=%d) isFlatFail=%d isMonoFail=%d coarseHint=%d\n"
				"  plan=%s nextOffset=%d nextBase=%d | %s\n",
				lineNo,
				ExtractStepTagFromPathField(pathField).c_str(),
				csvAttempt,
				attemptDacRange,
				movingBase,
				n,
				csvCodeName.c_str(),
				Peak1DCodeName(code),
				fitOk ? 1 : 0,
				tStar,
				prof.argmaxIndex,
				SweepTrendName(prof.trend),
				prof.span,
				minSpanRaw,
				(prof.trend == SweepTrend::Flat) ? 1 : 0,
				session.flatJumpedToMax ? 1 : 0,
				session.inCoarsePhase ? 1 : 0,
				session.fineConsumed ? 1 : 0,
				session.flatShiftCount,
				session.oscillationDetected ? 1 : 0,
				afterExpand ? 1 : 0,
				retryableBase ? 1 : 0,
				retryableExpand ? 1 : 0,
				IsFlatSweepFailure(code, prof) ? 1 : 0,
				IsMonotoneSweepFailure(code, prof, n) ? 1 : 0,
				IsCoarsePeakHint(code, prof, n, &failInfo) ? 1 : 0,
				(plan.action == SweepRetryAction::GiveUp) ? "GiveUp" : SweepRetryActionLogTag(plan.action),
				plan.nextRange,
				plan.nextBase,
				reason.c_str());
			if (csvCode != code && csvCode == Peak1DValidateCode::VertexOutOfRange)
				std::printf("  NOTE: csv VOR but replay fit_code=%s\n", Peak1DCodeName(code));
		}

		if (fitOk)
		{
			const SweepRetryPlan finePlan = PlanFineRefineAfterCoarseSuccess(
				session, col0, tStar, n, attemptDacRange);
			if (finePlan.action != SweepRetryAction::GiveUp)
				ApplySweepRetryPlan(session, finePlan);
			session.prevArgmax = prof.argmaxIndex;
			continue;
		}

		if (plan.action == SweepRetryAction::GiveUp)
		{
			session.prevArgmax = prof.argmaxIndex;
			continue;
		}

		ApplySweepRetryPlan(session, plan);
		movingBase = session.movingBase;
		attemptDacRange = session.attemptRange;
		session.prevArgmax = prof.argmaxIndex;
	}

	std::printf("vor-diag: VertexOutOfRange rows=%d planner_GiveUp=%d early_GiveUp=%d (opened %s)\n",
		vorRows, vorGiveUp, vorEarlyGiveUp, csvPath.c_str());
	if (vorEarlyGiveUp > 0)
	{
		std::fprintf(stderr,
			"vor-diag: FAIL %d VOR row(s) GiveUp before lastAttempt (INV-12 fallback expected)\n",
			vorEarlyGiveUp);
		return 1;
	}
	return 0;
}

static int RunRecalCmdFormatSelfTests()
{
	int fail = 0;
	auto check = [&](const char* label, const std::string& cmd, int k, int mode, int bx, int by, int off, int step, int delay)
	{
		RecalSweepCmdFields cf;
		if (!ParseRecalSweepCmd(cmd, cf) || cf.legacy5Param)
		{
			std::fprintf(stderr, "self-test recal-cmd: %s parse failed: %s\n", label, cmd.c_str());
			++fail;
			return;
		}
		if (cf.recalKind != k || cf.sweepMode != mode || cf.baseX != bx || cf.baseY != by
			|| cf.offsetDac != off || cf.stepDac != step || cf.delayMs != delay)
		{
			std::fprintf(stderr,
				"self-test recal-cmd: %s field mismatch for %s\n",
				label,
				cmd.c_str());
			++fail;
		}
	};

	const std::string mode0 = FormatRecalSweepCmd(3, 0, 9999, 9999, 64, 4, 80);
	check("mode0 PM", mode0, 3, 0, 9999, 9999, 64, 4, 80);
	if (mode0 != "RECAL 3 0 9999 9999 64 4 80")
	{
		std::fprintf(stderr, "self-test recal-cmd: mode0 string expected 'RECAL 3 0 9999 9999 64 4 80' got '%s'\n",
			mode0.c_str());
		++fail;
	}

	const std::string mode1 = FormatRecalSweepCmd(3, 1, -2663, 2186, 128, 4, 80);
	check("mode1 PM", mode1, 3, 1, -2663, 2186, 128, 4, 80);
	if (mode1 != "RECAL 3 1 -2663 2186 128 4 80")
	{
		std::fprintf(stderr, "self-test recal-cmd: mode1 string mismatch\n");
		++fail;
	}

	const std::string pd5 = FormatRecalSweepCmd(5, 0, 9999, 9999, 64, 4, 80);
	check("mode0 PD", pd5, 5, 0, 9999, 9999, 64, 4, 80);

	RecalSweepCmdFields legacy;
	if (!ParseRecalSweepCmd("RECAL 3 0 9999 64 4 80", legacy) || !legacy.legacy5Param
		|| legacy.baseX != 9999 || legacy.baseY != 9999)
	{
		std::fprintf(stderr, "self-test recal-cmd: legacy 5-param mode0 mapping failed\n");
		++fail;
	}
	if (!ParseRecalSweepCmd("RECAL 3 1 2189 64 4 80", legacy) || !legacy.legacy5Param
		|| legacy.baseY != 2189 || legacy.baseX != 9999)
	{
		std::fprintf(stderr, "self-test recal-cmd: legacy 5-param mode1 mapping failed\n");
		++fail;
	}

	return fail;
}

static int FineRefineRelaxedSelfTests()
{
	int fail = 0;
	// 2026-06-08 Step 64 fine RECAL 3 0 @64: span < MinProminenceDb under Strict, visible peak at ~index 14.
	static const double kFineJun8[] = {
		-220349, -220352, -220344, -220338, -220333, -220336, -220324, -220324, -220318, -220315,
		-220312, -220305, -220303, -220295, -220291, -220297, -220294, -220296, -220295, -220297,
		-220301, -220301, -220316, -220320, -220331, -220343, -220354, -220375, -220396, -220424,
		-220458, -220489, -220520,
	};
	const size_t nFine = sizeof(kFineJun8) / sizeof(kFineJun8[0]);
	std::vector<double> fineJun8(kFineJun8, kFineJun8 + nFine);
	{
		Peak1DValidateCode cStrict = Peak1DValidateCode::Ok;
		double tStrict = 0.0;
		if (M576::ParabolaVertexMax1D(fineJun8, tStrict, cStrict, nullptr, Peak1DFitPolicy::Strict))
		{
			std::fprintf(stderr, "self-test: Jun8 fine sweep must fail Strict (span < MinProminenceDb)\n");
			++fail;
		}
		if (cStrict == Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: Jun8 fine sweep Strict code unset\n");
			++fail;
		}
	}
	{
		Peak1DValidateCode cRel = Peak1DValidateCode::Empty;
		double tRel = 0.0;
		int idxRel = 0;
		M576::Peak1DFitTrace tr;
		if (!M576::FindUnimodalPeak1DIndex(
				fineJun8, idxRel, cRel, &tRel, &tr, Peak1DFitPolicy::FineRefineRelaxed)
			|| cRel != Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: Jun8 fine sweep must pass FineRefineRelaxed\n");
			++fail;
		}
		if (idxRel < 0 || idxRel >= (int)nFine)
		{
			std::fprintf(stderr, "self-test: Jun8 fine sweep idx out of range\n");
			++fail;
		}
	}
	{
		std::vector<double> flatLowSpan((size_t)nFine, -220000.0);
		flatLowSpan[(size_t)(nFine / 2)] = -219999.0;
		Peak1DValidateCode cFlat = Peak1DValidateCode::Ok;
		double tFlat = 0.0;
		if (M576::ParabolaVertexMax1D(flatLowSpan, tFlat, cFlat, nullptr, Peak1DFitPolicy::Strict))
		{
			std::fprintf(stderr, "self-test: Strict must reject preprocess span below MinProminenceDb\n");
			++fail;
		}
		if (cFlat != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: Strict micro-flat must be ParabolaNotDownward (got %d)\n", (int)cFlat);
			++fail;
		}
	}
	{
		std::vector<double> monoDec((size_t)nFine);
		for (size_t i = 0; i < nFine; ++i)
			monoDec[i] = -250000.0 - (double)i * 1000.0;
		Peak1DValidateCode cMono = Peak1DValidateCode::Ok;
		double tMono = 0.0;
		if (M576::ParabolaVertexMax1D(monoDec, tMono, cMono, nullptr, Peak1DFitPolicy::Strict))
		{
			std::fprintf(stderr, "self-test: strict monotone sweep must fail Strict\n");
			++fail;
		}
		M576::Peak1DFitTrace trMono;
		int idxMono = 0;
		if (!M576::FindUnimodalPeak1DIndex(
				monoDec, idxMono, cMono, &tMono, &trMono, Peak1DFitPolicy::FineRefineRelaxed)
			|| cMono != Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: FineRefineRelaxed must accept coarse-located monotone fine window\n");
			++fail;
		}
	}
	// 2026-06-09: Y pre @64 OK but cross Y failed Strict — cross must use FineRefineRelaxed when range==uiFine.
	{
		static const double kFineJun9Y[] = {
			-220441, -220436, -220430, -220425, -220425, -220414, -220412, -220411, -220412, -220403,
			-220396, -220394, -220387, -220386, -220384, -220380, -220384, -220381, -220385, -220385,
			-220393, -220399, -220408, -220416, -220428, -220444, -220458, -220478, -220500, -220531,
			-220563, -220591, -220629,
		};
		const size_t nCross = sizeof(kFineJun9Y) / sizeof(kFineJun9Y[0]);
		std::vector<double> powY(kFineJun9Y, kFineJun9Y + nCross);
		std::vector<double> powX(nCross);
		for (size_t i = 0; i < nCross; ++i)
		{
			const double t = (double)i - 16.0;
			powX[i] = -1000.0 - 10.0 * t * t;
		}
		SweepRecenterSessionState xSt = {};
		InitSweepRecenterSessionState(xSt, 64, 9999);
		xSt.attemptRange = 64;
		const Peak1DFitPolicy crossYOk = Peak1DFitPolicyForSweepResult(64, 64);
		const Peak1DFitPolicy crossXOk = Peak1DFitPolicyForCrossAxis(xSt, 64, 64);
		int br = 0, bc = 0;
		Peak1DValidateCode yCross = Peak1DValidateCode::Ok;
		Peak1DValidateCode xCross = Peak1DValidateCode::Ok;
		if (M576::PeakCrossFrom1DScans(
				powY, powX, br, bc, &yCross, &xCross, nullptr, nullptr, nullptr, nullptr,
				Peak1DFitPolicy::Strict, Peak1DFitPolicy::Strict))
		{
			std::fprintf(stderr, "self-test: Jun9 cross must fail Strict Y on fine-range powY\n");
			++fail;
		}
		if (yCross == Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: Jun9 cross Strict Y should set failure code\n");
			++fail;
		}
		yCross = Peak1DValidateCode::Ok;
		xCross = Peak1DValidateCode::Ok;
		br = bc = 0;
		if (!M576::PeakCrossFrom1DScans(
				powY, powX, br, bc, &yCross, &xCross, nullptr, nullptr, nullptr, nullptr,
				crossYOk, crossXOk)
			|| yCross != Peak1DValidateCode::Ok || xCross != Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: Jun9 cross must pass Relaxed Y (fine-range policy) y=%d x=%d\n",
				(int)yCross, (int)xCross);
			++fail;
		}
	}
	return fail;
}

static int RunPathSummarySelfTests()
{
	int fail = 0;
	std::vector<SCalibPathStepOutcome> rows;
	{
		SCalibPathStepOutcome o = {};
		o.result = CalibPathStepResult::Failed;
		o.failCategory = CalibPathFailCategory::PmRangeMismatch;
		o.peakDbm = -5.0;
		o.loDbm = -10.0;
		o.hiDbm = 0.0;
		rows.push_back(o);
	}
	{
		SCalibPathStepOutcome o = {};
		o.result = CalibPathStepResult::Failed;
		o.failCategory = CalibPathFailCategory::XCrossPeak;
		rows.push_back(o);
	}
	{
		SCalibPathStepOutcome o = {};
		o.result = CalibPathStepResult::Skipped;
		o.failCategory = CalibPathFailCategory::CsvValidation;
		rows.push_back(o);
	}
	const SRunPathSummary s = BuildRunPathSummary(rows, 42, true, false);
	if (s.successCount != 42 || s.failedCount != 2 || s.skippedCount != 1)
	{
		std::fprintf(stderr, "self-test: BuildRunPathSummary counts (ok=%d fail=%d skip=%d)\n",
			s.successCount, s.failedCount, s.skippedCount);
		++fail;
	}
	if (s.failByCategory.count(CalibPathFailCategory::PmRangeMismatch) != 1
		|| s.failByCategory.count(CalibPathFailCategory::XCrossPeak) != 1
		|| s.failByCategory.count(CalibPathFailCategory::CsvValidation) != 1)
	{
		std::fprintf(stderr, "self-test: BuildRunPathSummary category buckets\n");
		++fail;
	}
	const std::string hdr = FormatRunPathSummaryHeaderText(s);
	if (hdr.find("success 42") == std::string::npos || hdr.find("failed 2") == std::string::npos)
	{
		std::fprintf(stderr, "self-test: FormatRunPathSummaryHeaderText\n");
		++fail;
	}
	return fail;
}

int main(int argc, char* argv[])
{
	if (argc >= 3 && std::strcmp(argv[1], "--export-peak-csv") == 0)
		return RunExportPeakCsvMain(argc, argv);

	if (argc >= 3 && std::strcmp(argv[1], "--mock-sweeps") == 0)
	{
		(void)RunMockSweepLinesFile(argv[2]);
		return 0;
	}

	if (RunPeak1DSelfTests() != 0)
		return 9;
	if (RunRecalCmdFormatSelfTests() != 0)
		return 12;
	if (RunSweepCsvSplitSelfTests() != 0)
		return 15;
	if (RunComm20260701SweepRegressionTests() != 0)
		return 16;
	if (RunMaxAttemptsFallbackSelfTests() != 0)
		return 18;
	if (RunComm20260702VertexOutOfRangeDiagnostics() != 0)
		return 17;
	if (RunPmRangeSelfTests() != 0)
		return 11;
	if (RunSweepRecenterSelfTests() != 0)
		return 10;
	if (FineRefineRelaxedSelfTests() != 0)
		return 14;
	if (RunPathSummarySelfTests() != 0)
		return 13;

	// Bare exe: self-tests only (DoD exit 0). Pass test_sweeps.csv for interactive cross demo.
	if (argc < 2)
		return 0;

	SweepRow sweepY, sweepX;
	double step = 1.0;
	bool stepSet = false;
	bool stepFromFile = false;
	std::string err;

	if (argc >= 2)
	{
		if (!LoadSweepsFile(argv[1], sweepY, sweepX, step, stepFromFile, err))
		{
			std::fprintf(stderr, "CrossPeakTest: %s\n", err.c_str());
			return 1;
		}
		if (argc >= 3)
		{
			step = std::strtod(argv[2], nullptr);
			stepSet = true;
		}
		else if (!stepFromFile)
			std::fprintf(stderr, "CrossPeakTest: no Step on line 3 and no argv[2]; using Step=1.0 (set RECAL Step DAC)\n");
		else
			stepSet = true;
	}
	else
	{
		const std::string beside = PathToExeDirFile("test_sweeps.csv");
		if (!beside.empty() && LoadSweepsFile(beside.c_str(), sweepY, sweepX, step, stepFromFile, err))
		{
			std::printf("Loaded: %s\n", beside.c_str());
			if (stepFromFile)
				stepSet = true;
			else
				std::fprintf(stderr, "CrossPeakTest: add line 3 with Step DAC, or: CrossPeakTest.exe test_sweeps.csv <Step>\n");
		}
		else
		{
			DefaultDemoData(sweepY, sweepX);
			std::printf("No test_sweeps.csv: using demo (Step=1.0).\n");
			stepSet = true;
		}
	}

	if (!stepSet)
		step = 1.0;

	if (sweepY.powers.size() != sweepX.powers.size())
	{
		std::fprintf(stderr, "CrossPeakTest: power column count mismatch %zu vs %zu\n",
			sweepY.powers.size(), sweepX.powers.size());
		return 2;
	}
	if (sweepY.powers.empty())
	{
		std::fprintf(stderr, "CrossPeakTest: no power samples\n");
		return 3;
	}

	int br = 0, bc = 0;
	double tY = 0, tX = 0;
	if (!M576::PeakCrossFrom1DScans(sweepY.powers, sweepX.powers, br, bc, nullptr, nullptr, &tY, &tX, nullptr, nullptr))
	{
		std::fprintf(stderr, "CrossPeakTest: PeakCrossFrom1DScans failed\n");
		return 4;
	}

	// Same as M576CalibratorDlg: rawDacX = Y-line col0 + tY*step, rawDacY = X-line col0 + tX*step.
	const double rawDacX = sweepY.dac_base + tY * step;
	const double rawDacY = sweepX.dac_base + tX * step;

	std::printf("Step DAC = %.17g (same as RECAL 3 Step)\n", step);
	std::printf("Power samples per axis N = %zu (0-based; cubic-fit peak position t*)\n", sweepY.powers.size());
	std::printf("RECAL 3 0: dac_base = %.17g  => tY=%.8g  lround->%d\n", sweepY.dac_base, tY, br);
	std::printf("RECAL 3 1: dac_base = %.17g  => tX=%.8g  lround->%d\n", sweepX.dac_base, tX, bc);
	std::printf("  power@grid sample at lround: P_y[%d]=%.17g  P_x[%d]=%.17g\n", br, sweepY.powers[(size_t)br], bc, sweepX.powers[(size_t)bc]);
	std::printf("  host linear DAC: rawDacX = col0_y + tY*step = %.17g, rawDacY = col0_x + tX*step = %.17g\n", rawDacX, rawDacY);
	std::printf("Cross lround(tY) lround(tX) = (%d, %d)\n", br, bc);
	return 0;
}
