#include "SmallRangeCalibSelection.h"

#include <cstdio>
#include <sstream>

bool SmallRangeAppendUnique(std::vector<FineTuneAddress>& channels, const FineTuneAddress& addr)
{
	for (const FineTuneAddress& existing : channels)
	{
		if (FineTuneAddressEqual(existing, addr))
			return false;
	}
	channels.push_back(addr);
	return true;
}

bool SmallRangeBuildMcsPmStep(const FineTuneAddress& addr, SmallRangePmStep& outStep)
{
	outStep = SmallRangePmStep{};
	if (!FineTuneIsMcsDevice(addr.device))
		return false;
	if (addr.mcsBlock1to32 < 1 || addr.mcsBlock1to32 > 32
		|| addr.mcsCh1to18 < 1 || addr.mcsCh1to18 > 18)
		return false;
	outStep.targetSwitchIndex = (addr.device == FineTuneDeviceKind::Mcs1) ? 3 : 4;
	outStep.c1 = addr.mcsBlock1to32;
	outStep.c2 = addr.mcsCh1to18;
	outStep.c3 = addr.mcsCh1to18;
	outStep.c4 = addr.mcsBlock1to32 + 32;
	return true;
}

bool SmallRangeMcsAddressMatchesPmStep(const FineTuneAddress& addr, const SmallRangePmStep& step)
{
	SmallRangePmStep expect{};
	if (!SmallRangeBuildMcsPmStep(addr, expect))
		return false;
	return step.targetSwitchIndex == expect.targetSwitchIndex
		&& step.c1 == expect.c1
		&& step.c2 == expect.c2
		&& step.c3 == expect.c3
		&& step.c4 == expect.c4;
}

bool SmallRangeOneX64AddressMatchesMapRow(const FineTuneAddress& addr, const SmallRangeMapRow& row)
{
	if (FineTuneIsMcsDevice(addr.device))
		return false;
	return row.sw1to4 == addr.sw1to4 && row.chY1based == addr.chY1to17;
}

static bool PmStepsEqual(const SmallRangePmStep& a, const SmallRangePmStep& b)
{
	return a.targetSwitchIndex == b.targetSwitchIndex
		&& a.c1 == b.c1 && a.c2 == b.c2 && a.c3 == b.c3 && a.c4 == b.c4;
}

static std::string FormatPmStepA(const SmallRangePmStep& s)
{
	char buf[96] = {};
	std::snprintf(
		buf,
		sizeof(buf),
		"tgt=%d c1=%d c2=%d c3=%d c4=%d",
		s.targetSwitchIndex,
		s.c1,
		s.c2,
		s.c3,
		s.c4);
	return std::string(buf);
}

bool SmallRangeResolvePmWhitelist(
	const std::vector<FineTuneAddress>& channels,
	const std::vector<SmallRangePmStep> pmStepsBySlot[4],
	const std::vector<SmallRangeMapRow> mapRowsBySlot[4],
	std::vector<SmallRangeWhitelistEntry>& outWhitelist,
	std::string& errMsg)
{
	outWhitelist.clear();
	errMsg.clear();
	if (pmStepsBySlot == nullptr || mapRowsBySlot == nullptr)
	{
		errMsg = "SmallRange: null PM/Mapping slot arrays.";
		return false;
	}

	std::vector<FineTuneAddress> unique;
	for (const FineTuneAddress& addr : channels)
		SmallRangeAppendUnique(unique, addr);

	if (unique.empty())
	{
		errMsg = "SmallRange: no FineTune channels recorded.";
		return false;
	}

	for (const FineTuneAddress& addr : unique)
	{
		const int fileSlot = FineTuneAddressPmFileSlot(addr.device);
		if (fileSlot < 0 || fileSlot > 3)
		{
			errMsg = "SmallRange: unknown FineTune device for " + FineTuneAddressFormatLabelA(addr);
			return false;
		}

		const std::vector<SmallRangePmStep>& steps = pmStepsBySlot[fileSlot];
		if (steps.empty())
		{
			std::ostringstream oss;
			oss << "SmallRange: PM CSV slot " << (fileSlot + 1)
				<< " empty/missing for " << FineTuneAddressFormatLabelA(addr);
			errMsg = oss.str();
			return false;
		}

		SmallRangeWhitelistEntry entry{};
		entry.fileSlot = fileSlot;
		entry.addr = addr;
		entry.label = FineTuneAddressFormatLabelA(addr);

		if (FineTuneIsMcsDevice(addr.device))
		{
			const int idx = FineTuneMcsPmStepIndex0(addr.mcsBlock1to32, addr.mcsCh1to18);
			if (idx < 0)
			{
				errMsg = "SmallRange: invalid MCS block/ch for " + entry.label;
				return false;
			}
			if (idx >= (int)steps.size())
			{
				std::ostringstream oss;
				oss << "SmallRange: MCS step index " << idx
					<< " out of range (CSV rows=" << steps.size()
					<< ") for " << entry.label;
				errMsg = oss.str();
				return false;
			}
			if (!SmallRangeMcsAddressMatchesPmStep(addr, steps[(size_t)idx]))
			{
				std::ostringstream oss;
				oss << "SmallRange: MCS CSV row " << (idx + 1)
					<< " mismatch for " << entry.label
					<< " (got " << FormatPmStepA(steps[(size_t)idx]) << ")";
				errMsg = oss.str();
				return false;
			}
			entry.stepIndex0 = idx;
			entry.step = steps[(size_t)idx];
		}
		else
		{
			const std::vector<SmallRangeMapRow>& maps = mapRowsBySlot[fileSlot];
			if (maps.empty())
			{
				errMsg = "SmallRange: Mapping.csv missing/empty for " + entry.label;
				return false;
			}

			int mapHits = 0;
			int mapIndex = -1;
			for (int i = 0; i < (int)maps.size(); ++i)
			{
				if (SmallRangeOneX64AddressMatchesMapRow(addr, maps[(size_t)i]))
				{
					++mapHits;
					mapIndex = i;
				}
			}
			if (mapHits == 0)
			{
				errMsg = "SmallRange: no Mapping.csv row for " + entry.label;
				return false;
			}
			if (mapHits > 1)
			{
				std::ostringstream oss;
				oss << "SmallRange: ambiguous Mapping.csv (" << mapHits
					<< " rows) for " << entry.label;
				errMsg = oss.str();
				return false;
			}

			const SmallRangeMapRow& mr = maps[(size_t)mapIndex];
			SmallRangePmStep want{};
			want.targetSwitchIndex = mr.targetSwitchIndex;
			want.c1 = mr.c1;
			want.c2 = mr.c2;
			want.c3 = mr.c3;
			want.c4 = mr.c4;

			int stepHits = 0;
			int stepIndex = -1;
			for (int i = 0; i < (int)steps.size(); ++i)
			{
				if (PmStepsEqual(steps[(size_t)i], want))
				{
					++stepHits;
					stepIndex = i;
				}
			}
			if (stepHits == 0)
			{
				std::ostringstream oss;
				oss << "SmallRange: PM CSV has no row matching Mapping "
					<< FormatPmStepA(want) << " for " << entry.label;
				errMsg = oss.str();
				return false;
			}
			if (stepHits > 1)
			{
				std::ostringstream oss;
				oss << "SmallRange: ambiguous PM CSV (" << stepHits
					<< " rows) for " << entry.label
					<< " step " << FormatPmStepA(want);
				errMsg = oss.str();
				return false;
			}
			entry.stepIndex0 = stepIndex;
			entry.step = steps[(size_t)stepIndex];
		}

		// Dedup by (fileSlot, stepIndex0) �� same physical path from two FineTune writes.
		bool already = false;
		for (const SmallRangeWhitelistEntry& e : outWhitelist)
		{
			if (e.fileSlot == entry.fileSlot && e.stepIndex0 == entry.stepIndex0)
			{
				already = true;
				break;
			}
		}
		if (!already)
			outWhitelist.push_back(entry);
	}

	if (outWhitelist.empty())
	{
		errMsg = "SmallRange: whitelist empty after resolve.";
		return false;
	}
	return true;
}

bool SmallRangeWhitelistContains(
	const std::vector<SmallRangeWhitelistEntry>& whitelist,
	int fileSlot,
	int stepIndex0)
{
	for (const SmallRangeWhitelistEntry& e : whitelist)
	{
		if (e.fileSlot == fileSlot && e.stepIndex0 == stepIndex0)
			return true;
	}
	return false;
}

int SmallRangeWhitelistCountForSlot(
	const std::vector<SmallRangeWhitelistEntry>& whitelist,
	int fileSlot)
{
	int n = 0;
	for (const SmallRangeWhitelistEntry& e : whitelist)
	{
		if (e.fileSlot == fileSlot)
			++n;
	}
	return n;
}
