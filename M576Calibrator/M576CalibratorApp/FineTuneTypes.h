#pragma once
// Shared FineTune address identity (no MFC). Used by FineTuneBinPatch and SmallRangeCalibSelection.

#include <cstdio>
#include <string>

enum class FineTuneDeviceKind
{
	Mcs1 = 0,
	Mcs2,
	OneX64_1,
	OneX64_2,
};

struct FineTuneAddress
{
	FineTuneDeviceKind device = FineTuneDeviceKind::Mcs1;
	/// MCS: block 1..32, channel 1..18.
	int mcsBlock1to32 = 1;
	int mcsCh1to18 = 1;
	/// 1x64 Mapping: SW 1..4, CH_y 1..17.
	int sw1to4 = 1;
	int chY1to17 = 1;
};

inline bool FineTuneIsMcsDevice(FineTuneDeviceKind d)
{
	return d == FineTuneDeviceKind::Mcs1 || d == FineTuneDeviceKind::Mcs2;
}

inline bool FineTuneAddressEqual(const FineTuneAddress& a, const FineTuneAddress& b)
{
	if (a.device != b.device)
		return false;
	if (FineTuneIsMcsDevice(a.device))
		return a.mcsBlock1to32 == b.mcsBlock1to32 && a.mcsCh1to18 == b.mcsCh1to18;
	return a.sw1to4 == b.sw1to4 && a.chY1to17 == b.chY1to17;
}

inline const char* FineTuneDeviceKindNameA(FineTuneDeviceKind d)
{
	switch (d)
	{
	case FineTuneDeviceKind::Mcs1: return "MCS1";
	case FineTuneDeviceKind::Mcs2: return "MCS2";
	case FineTuneDeviceKind::OneX64_1: return "1X64_1";
	case FineTuneDeviceKind::OneX64_2: return "1X64_2";
	default: return "Unknown";
	}
}

/// PM CSV fileSlot: 0=MCS1, 1=MCS2, 2=1X64_1, 3=1X64_2. Returns -1 if unknown.
inline int FineTuneAddressPmFileSlot(FineTuneDeviceKind d)
{
	switch (d)
	{
	case FineTuneDeviceKind::Mcs1: return 0;
	case FineTuneDeviceKind::Mcs2: return 1;
	case FineTuneDeviceKind::OneX64_1: return 2;
	case FineTuneDeviceKind::OneX64_2: return 3;
	default: return -1;
	}
}

/// MCS pm_mcs*.csv row index 0..575 for (block,ch). Returns -1 if out of range.
inline int FineTuneMcsPmStepIndex0(int mcsBlock1to32, int mcsCh1to18)
{
	if (mcsBlock1to32 < 1 || mcsBlock1to32 > 32 || mcsCh1to18 < 1 || mcsCh1to18 > 18)
		return -1;
	return (mcsBlock1to32 - 1) * 18 + (mcsCh1to18 - 1);
}

inline std::string FineTuneAddressFormatLabelA(const FineTuneAddress& addr)
{
	char buf[96] = {};
	if (FineTuneIsMcsDevice(addr.device))
	{
		std::snprintf(
			buf,
			sizeof(buf),
			"%s block=%d ch=%d",
			FineTuneDeviceKindNameA(addr.device),
			addr.mcsBlock1to32,
			addr.mcsCh1to18);
	}
	else
	{
		std::snprintf(
			buf,
			sizeof(buf),
			"%s SW=%d CH_y=%d",
			FineTuneDeviceKindNameA(addr.device),
			addr.sw1to4,
			addr.chY1to17);
	}
	return std::string(buf);
}
