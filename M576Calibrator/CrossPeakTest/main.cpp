/*
 * CrossPeakTest: RECAL 3 0 / 3 1 cross peak. col0 = DAC base, rest = powers.
 * Peak index k on powers; DAC_peak = dac_base + k * Step.
 *
 *   CrossPeakTest.exe --mock-sweeps <file.csv>
 *   CrossPeakTest.exe --export-peak-csv <in.csv> [more.csv ...] [-o <dir>] [--utf8-bom]
 */
#include "PeakFinder2D.h"
#include "Peak1DSweepRecenter.h"
#include "PmRangeValidation.h"
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
using M576::SweepProfile;
using M576::SweepTrend;
using M576::AnalyzeRecal1DSweepProfile;
using M576::IsRetryablePeakFailure;
using M576::SuggestSweepRecenterDeltaDac;
using M576::SweepRecenterFailureInfo;
using M576::SweepTrendName;

static bool ParseNumberLine(const std::string& line, std::vector<double>& out);

struct RecalSweepCmdFields
{
	bool ok = false;
	int recalKind = 0;
	int sweepMode = -1;
	int baseDac = 0;
	int offsetDac = 0;
	int stepDac = 0;
	int delayMs = 0;
};

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
	int k = 0, mode = 0, base = 0, off = 0, step = 0, delay = 0;
	if (std::sscanf(cmd.c_str(), "RECAL %d %d %d %d %d %d", &k, &mode, &base, &off, &step, &delay) != 6)
		return false;
	if ((k != 3 && k != 5) || (mode != 0 && mode != 1))
		return false;
	out.ok = true;
	out.recalKind = k;
	out.sweepMode = mode;
	out.baseDac = base;
	out.offsetDac = off;
	out.stepDac = step;
	out.delayMs = delay;
	return true;
}

static bool SplitSweepCsvRow(const std::string& line, std::string& cmd, std::vector<double>& nums)
{
	cmd.clear();
	nums.clear();
	const size_t comma = line.find(',');
	if (comma == std::string::npos)
		return false;
	cmd = line.substr(0, comma);
	TrimInPlace(cmd);
	return ParseNumberLine(line.substr(comma + 1), nums) && !cmd.empty();
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
		"line_no,cmd,sweep_mode,base_dac,offset_dac,step_dac,delay_ms,dac_base,n_powers,"
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
			if (lower.find("cmd") != std::string::npos)
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
			for (int k = 0; k < 17; ++k)
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
		AppendCsvField(row, cf.ok ? std::to_string(cf.baseDac) : "", first);
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
	// test_sweeps.csv line 3: plateau jitter at apex; mono wedge had only 3 points before fixed-half fallback.
	{
		static const double kPlateauJitterPowers[] = {
			-129449, -129398, -129364, -129343, -129322, -129274, -129263, -129233, -129243, -129228,
			-129234, -129232, -129228, -129257, -129267, -129313, -129337, -129393, -129448, -129499,
			-129582, -129636, -129718, -129816, -129908, -130041, -130156, -130290, -130444, -130586,
			-130760, -130952, -131148,
		};
		std::vector<double> plateau(kPlateauJitterPowers, kPlateauJitterPowers + _countof(kPlateauJitterPowers));
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

static int RunSweepRecenterSelfTests()
{
	int fail = 0;
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
	}

	{
		// comm.log L7622 flat row (col0=-31): span below MIN_SPAN -> no retry
		const double commFlat[] = {
			-122228, -122243, -122228, -122207, -122212, -122217, -122216, -122259, -122296, -122275,
			-122263, -122254, -122234, -122230, -122225, -122228, -122214, -122212, -122209, -122192,
			-122193, -122183, -122180, -122173, -122165, -122161, -122151, -122142, -122131, -122140,
			-122132, -122134, -122123
		};
		std::vector<double> flat(commFlat, commFlat + n);
		const SweepProfile p = AnalyzeRecal1DSweepProfile(flat);
		if (p.trend != SweepTrend::Flat
			|| IsRetryablePeakFailure(Peak1DValidateCode::NotEnoughValidSamples, p, n))
		{
			std::fprintf(stderr, "self-test: comm.log flat row must be Flat and not retryable\n");
			++fail;
		}
	}

	{
		std::vector<double> edge((size_t)n);
		for (int i = 0; i < n; ++i)
			edge[(size_t)i] = -100000.0 - (double)(i * i);
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
	if (RunPmRangeSelfTests() != 0)
		return 11;
	if (RunSweepRecenterSelfTests() != 0)
		return 10;

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
