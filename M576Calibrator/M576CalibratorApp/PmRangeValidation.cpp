#include "PmRangeValidation.h"
#include "PeakFinder2D.h"
// PmRangeValidation.cpp：峰功率 dBm 区间校验（INV-16）；ParseOpmPmRangeReplyAscii 仅解析固件 opm 应答格式。
#include <cmath>
#include <cstring>

namespace M576
{
	int ParseOpmPmRangeReplyAscii(const char* lineAscii)
	{
		if (!lineAscii)
			return -1;
		while (*lineAscii == ' ' || *lineAscii == '\t' || *lineAscii == '\r' || *lineAscii == '\n')
			++lineAscii;
		if (!*lineAscii)
			return -1;
		const size_t len = std::strlen(lineAscii);
		size_t end = len;
		while (end > 0)
		{
			const char c = lineAscii[end - 1];
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
				--end;
			else
				break;
		}
		if (end != 1)
			return -1;
		const char c = lineAscii[0];
		if (c >= '0' && c <= '4')
			return c - '0';
		return -1;
	}

	bool GetPmRangeDbmBounds(int pmRangeIndex, double& outLoDbm, double& outHiDbm)
	{
		switch (pmRangeIndex)
		{
		case 0:
			outLoDbm = -14.0;
			outHiDbm = 6.0;
			return true;
		case 1:
			outLoDbm = -34.0;
			outHiDbm = -14.0;
			return true;
		case 2:
			outLoDbm = -54.0;
			outHiDbm = -34.0;
			return true;
		case 3:
			outLoDbm = -75.0;
			outHiDbm = -54.0;
			return true;
		default:
			return false;
		}
	}

	const char* PmRangeDbmIntervalDesc(int pmRangeIndex)
	{
		switch (pmRangeIndex)
		{
		case 0:
			return "[-14,6]";
		case 1:
			return "[-34,-14)";
		case 2:
			return "[-54,-34)";
		case 3:
			return "[-75,-54)";
		default:
			return "(auto/skip)";
		}
	}

	static bool IsDbmInPmRange(int pmRangeIndex, double dbm)
	{
		double lo = 0.0, hi = 0.0;
		if (!GetPmRangeDbmBounds(pmRangeIndex, lo, hi))
			return true;
		switch (pmRangeIndex)
		{
		case 0:
			return dbm >= lo && dbm <= hi;
		case 1:
		case 2:
		case 3:
			return dbm >= lo && dbm < hi;
		default:
			return true;
		}
	}

	int FindGlobalMaxValidPowerIndex(const std::vector<double>& powers)
	{
		int best = -1;
		double bestY = 0.0;
		bool have = false;
		const int n = (int)powers.size();
		for (int i = 0; i < n; ++i)
		{
			const double v = powers[(size_t)i];
			if (IsRecal1DPowerInvalidValue(v))
				continue;
			if (!have || v > bestY)
			{
				have = true;
				bestY = v;
				best = i;
			}
		}
		return best;
	}

	bool ValidatePeakPowerInPmRange(
		int pmRangeIndex,
		const std::vector<double>& powers,
		int peakIdxHint,
		double& outPeakRaw,
		double& outPeakDbm,
		double& outLoDbm,
		double& outHiDbm,
		int& outPeakIdx)
	{
		outPeakRaw = 0.0;
		outPeakDbm = 0.0;
		outLoDbm = 0.0;
		outHiDbm = 0.0;
		outPeakIdx = -1;

		if (pmRangeIndex == 4)
			return true;
		if (!GetPmRangeDbmBounds(pmRangeIndex, outLoDbm, outHiDbm))
			return true;

		int idx = peakIdxHint;
		if (idx < 0 || idx >= (int)powers.size())
			idx = FindGlobalMaxValidPowerIndex(powers);
		outPeakIdx = idx;
		if (idx < 0)
			return false;

		outPeakRaw = powers[(size_t)idx];
		if (IsRecal1DPowerInvalidValue(outPeakRaw))
			return false;

		outPeakDbm = RecalRawToDbm(outPeakRaw);
		return IsDbmInPmRange(pmRangeIndex, outPeakDbm);
	}
}
