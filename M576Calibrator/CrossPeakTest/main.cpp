/*
 * CrossPeakTest: RECAL 3 0 / 3 1 cross peak. col0 = DAC base, rest = powers.
 * Peak index k on powers; DAC_peak = dac_base + k * Step.
 */
#include "PeakFinder2D.h"
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>


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
	y.dac_base = 32000.0;
	y.powers = { 0.1, 0.2, 0.4, 0.9, 0.3, 0.1, 0.0 };
	x.dac_base = 32000.0;
	x.powers = { 0.0, 0.1, 0.2, 0.15, 0.2, 0.95, 0.1 };
}

int main(int argc, char* argv[])
{
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
	if (!M576::PeakCrossFrom1DScans(sweepY.powers, sweepX.powers, br, bc))
	{
		std::fprintf(stderr, "CrossPeakTest: PeakCrossFrom1DScans failed\n");
		return 4;
	}

	const double dacY_peak = sweepY.dac_base + (double)br * step;
	const double dacX_peak = sweepX.dac_base + (double)bc * step;

	std::printf("Step DAC = %.17g (same as RECAL 3 Step)\n", step);
	std::printf("Power samples per axis N = %zu (peak index k among powers, 0-based)\n", sweepY.powers.size());
	std::printf("RECAL 3 0: dac_base = %.17g\n", sweepY.dac_base);
	std::printf("  k_y = %d  power = %.17g  => DAC_Y_peak = base + k_y*Step = %.17g\n",
		br, sweepY.powers[(size_t)br], dacY_peak);
	std::printf("RECAL 3 1: dac_base = %.17g\n", sweepX.dac_base);
	std::printf("  k_x = %d  power = %.17g  => DAC_X_peak = base + k_x*Step = %.17g\n",
		bc, sweepX.powers[(size_t)bc], dacX_peak);
	std::printf("Cross peak indices (k_y, k_x) = (%d, %d)\n", br, bc);
	std::printf("Cross peak DAC (Y, X) = (%.17g, %.17g)\n", dacY_peak, dacX_peak);
	return 0;
}
