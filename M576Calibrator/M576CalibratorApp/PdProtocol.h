#pragma once
// Z4744 0415 Command C (PD) pure protocol helpers — no MFC, safe for CrossPeakTest.

#include <cstdio>
#include <cstring>

namespace M576PdProtocol
{

/// Wire: RECAL 2 <target 1..4> <2#1x64 ch 1..64> <MCS ch 1..18>
inline bool FormatRecal2Cmd(int target, int ch1, int ch2, char* buf, size_t bufChars)
{
	if (buf == nullptr || bufChars < 32)
		return false;
	std::snprintf(buf, bufChars, "RECAL 2 %d %d %d", target, ch1, ch2);
	return true;
}

/// Wire: RECAL 5 <mode> <baseX> <baseY> <offset> <step> <delay> (same 6-param layout as RECAL 3).
inline bool FormatRecal5Cmd(
	int sweepMode,
	int baseX,
	int baseY,
	int offsetDac,
	int stepDac,
	int delayMs,
	char* buf,
	size_t bufChars)
{
	if (buf == nullptr || bufChars < 64)
		return false;
	std::snprintf(
		buf,
		bufChars,
		"RECAL 5 %d %d %d %d %d %d",
		sweepMode,
		baseX,
		baseY,
		offsetDac,
		stepDac,
		delayMs);
	return true;
}

/// Validate one PD path step (0415 Command C ranges + half-bank routing).
inline bool ValidatePathStep(
	int targetSwitchIndex,
	int ch1,
	int ch2,
	char* errBuf,
	size_t errChars)
{
	auto setErr = [&](const char* msg) {
		if (errBuf != nullptr && errChars > 0)
		{
			std::snprintf(errBuf, errChars, "%s", msg);
		}
	};
	if (targetSwitchIndex < 1 || targetSwitchIndex > 4)
	{
		setErr("PD target_index out of 1..4");
		return false;
	}
	if (ch1 < 1 || ch1 > 64)
	{
		setErr("PD ch1 (2#1x64 ch) out of 1..64");
		return false;
	}
	if (ch2 < 1 || ch2 > 18)
	{
		setErr("PD ch2 (MCS ch) out of 1..18");
		return false;
	}
	if (targetSwitchIndex == 3 && ch1 > 32)
	{
		setErr("PD target 3 (1# MCS) requires ch1 in 1..32");
		return false;
	}
	if (targetSwitchIndex == 4 && ch1 < 33)
	{
		setErr("PD target 4 (2# MCS) requires ch1 in 33..64");
		return false;
	}
	if (errBuf != nullptr && errChars > 0)
		errBuf[0] = '\0';
	return true;
}

/// PD target -> trans slot 0..3. 1/2 -> slot3 (2#1x64); 3->0 (1#MCS); 4->1 (2#MCS). No 1#1x64.
inline int TransSlotFromTarget(int targetSwitchIndex)
{
	switch (targetSwitchIndex)
	{
	case 3:
		return 0;
	case 4:
		return 1;
	case 1:
	case 2:
		return 3;
	default:
		return -1;
	}
}

/// MCS LUT write address for PD target 3/4. Returns false for 1x64 targets.
struct SMcsLutAddr
{
	int swIdx0{ 0 };
	int chIdx0{ 0 };
};

/// block1to32 -> LUT sw (same as M576McsBlock1To32ToLutSwIdx0).
inline int McsBlock1To32ToLutSwIdx0(int block1to32)
{
	if (block1to32 < 1 || block1to32 > 32)
		return 0;
	const int ch0 = block1to32 - 1;
	return (ch0 < 16) ? (ch0 + 16) : (ch0 - 16);
}

inline bool ResolveMcsLutAddr(int targetSwitchIndex, int ch1, int ch2, SMcsLutAddr& out)
{
	if (ch2 < 1 || ch2 > 18)
		return false;
	if (targetSwitchIndex == 3)
	{
		if (ch1 < 1 || ch1 > 32)
			return false;
		out.swIdx0 = McsBlock1To32ToLutSwIdx0(ch1);
		out.chIdx0 = ch2 - 1;
		return true;
	}
	if (targetSwitchIndex == 4)
	{
		if (ch1 < 33 || ch1 > 64)
			return false;
		const int block = ch1 - 32;
		out.swIdx0 = McsBlock1To32ToLutSwIdx0(block);
		out.chIdx0 = ch2 - 1;
		return true;
	}
	return false;
}

/// 0415 path budget: 64 Stage1 + 3 Stage2 + 576 + 576.
inline constexpr int kPathBudgetRows = 64 + 3 + 576 + 576;

} // namespace M576PdProtocol
