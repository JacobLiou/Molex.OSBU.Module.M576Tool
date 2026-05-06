/*
 * CrossPeakTest: RECAL 3 0 / 3 1 cross peak. col0 = DAC base, rest = powers.
 * Peak index k on powers; DAC_peak = dac_base + k * Step.
 */
#include "PeakFinder2D.h"
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

using M576::Peak1DValidateCode;

static bool ParseNumberLine(const std::string& line, std::vector<double>& out);

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
	default: return "?";
	}
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
	return 0;
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

static void DefaultDemoData(SweepRow& y, SweepRow& x)
{
	// Exact quadratics p(i)=50-1.0*(i-iv)^2 (10x scale vs old demo): span>=M576_PEAK1D_MIN_SPAN_DB; LS vertex at iv unchanged.
	y.dac_base = 32000.0;
	y.powers.resize(7);
	for (int i = 0; i < 7; ++i)
		y.powers[(size_t)i] = 50.0 - 1.0 * (double)(i - 3) * (double)(i - 3);
	x.dac_base = 32000.0;
	x.powers.resize(7);
	for (int i = 0; i < 7; ++i)
		x.powers[(size_t)i] = 50.0 - 1.0 * (double)(i - 2) * (double)(i - 2);
}

/// Synthetic checks for span / strict-peak / cross; returns number of failures.
static int RunPeak1DSelfTests()
{
	int fail = 0;
	{
		std::vector<double> flat(7, -10.0);
		int idx = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (M576::FindUnimodalPeak1DIndex(flat, idx, c) || c != Peak1DValidateCode::ParabolaNotDownward)
		{
			std::fprintf(stderr, "self-test: flat line should fail ParabolaNotDownward\n");
			++fail;
		}
	}
	{
		std::vector<double> one(5);
		for (int i = 0; i < 5; ++i)
			one[(size_t)i] = 50.0 - 1.0 * (double)(i - 2) * (double)(i - 2);
		int idx = 0;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::FindUnimodalPeak1DIndex(one, idx, c, &t) || std::abs(t - 2.0) > 0.05 || idx != 2
			|| c != Peak1DValidateCode::Ok)
		{
			std::fprintf(stderr, "self-test: quadratic peak at t≈2, lround->2 (cubic fit)\n");
			++fail;
		}
	}
	{
		std::vector<double> y(7), x(7);
		for (int i = 0; i < 7; ++i)
		{
			y[(size_t)i] = 50.0 - 1.0 * (double)(i - 3) * (double)(i - 3);
			x[(size_t)i] = 50.0 - 1.0 * (double)(i - 2) * (double)(i - 2);
		}
		int br = 0, bc = 0;
		Peak1DValidateCode yc = Peak1DValidateCode::Ok, xc = Peak1DValidateCode::Ok;
		double tY = 0, tX = 0;
		if (!M576::PeakCrossFrom1DScans(y, x, br, bc, &yc, &xc, &tY, &tX, nullptr, nullptr) || br != 3 || bc != 2
			|| yc != Peak1DValidateCode::Ok || xc != Peak1DValidateCode::Ok
			|| std::abs(tY - 3.0) > 0.05 || std::abs(tX - 2.0) > 0.05)
		{
			std::fprintf(stderr, "self-test: cross cubic tY≈3 tX≈2, lround (3,2)\n");
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
	// Masked fit: one firmware sentinel removed; true vertex at i=3, still ~3.0
	{
		std::vector<double> seven(7);
		for (int i = 0; i < 7; ++i)
			seven[(size_t)i] = 50.0 - 1.0 * (double)(i - 3) * (double)(i - 3);
		seven[2] = -999999.0; // M576_RECAL_POW_INVALID_1
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(seven, t, c) || c != Peak1DValidateCode::Ok || std::abs(t - 3.0) > 0.2)
		{
			std::fprintf(stderr, "self-test: masked quadratic peak near 3.0 (one -999999)\n");
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
		std::vector<double> q(7);
		for (int i = 0; i < 7; ++i)
			q[(size_t)i] = 50.0 - 1.0 * (double)(i - 3) * (double)(i - 3);
		M576::Peak1DFitTrace tr;
		double t = 0;
		Peak1DValidateCode c = Peak1DValidateCode::Ok;
		if (!M576::ParabolaVertexMax1D(q, t, c, &tr) || c != Peak1DValidateCode::Ok || tr.globalMaxIndex != 3
			|| std::abs(tr.globalMaxY - 50.0) > 1e-6 || tr.fitIndex.size() < (size_t)M576_PEAK1D_CUBIC_MIN_SAMPLES)
		{
			std::fprintf(stderr, "self-test: Peak1DFitTrace global max and fit count\n");
			++fail;
		}
	}
	return fail;
}

int main(int argc, char* argv[])
{
	if (argc >= 3 && std::strcmp(argv[1], "--mock-sweeps") == 0)
	{
		(void)RunMockSweepLinesFile(argv[2]);
		return 0;
	}

	if (RunPeak1DSelfTests() != 0)
		return 9;

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
