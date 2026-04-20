#pragma once

#include <afxtempl.h>

/// Power meter (RECAL 1 / Command B): target + four channel numbers per Z4744 — no per-segment block.
/// Order: [1#1x64 ch] [1#MCS ch] [2#MCS ch] [2#1x64 ch] -> c1..c4.
struct SPathStep
{
	int targetSwitchIndex;
	int c1, c2, c3, c4;
	SPathStep()
		: targetSwitchIndex(0)
		, c1(0), c2(0), c3(0), c4(0)
	{
	}
};

/// PD path (Command C / RECAL 2): per PRD Command C the wire form is
///   RECAL 2 <target> <2#1x64 ch> <MCS ch>
/// i.e. target + EXACTLY two channel numbers. Stage (Stage_1 / Stage_2) is implied by `target`
/// (1 -> Stage_1, 2 -> Stage_2); MCS bank (1# / 2#) is implied by the 2#1x64 channel half
/// (ch 1..32 -> 1# MCS, ch 33..64 -> 2# MCS).
struct SPathStepPd
{
	int targetSwitchIndex;	///< 1=2#1x64 Stage_1, 2=2#1x64 Stage_2, 3=1# MCS, 4=2# MCS
	int ch1;				///< [2#1x64 ch]  1..64
	int ch2;				///< [MCS ch]     1..18 (1#MCS when ch1<=32, 2#MCS when ch1>=33)
	SPathStepPd()
		: targetSwitchIndex(0)
		, ch1(0), ch2(0)
	{
	}
};

BOOL LoadPathCsv(LPCTSTR szPath, CArray<SPathStep, SPathStep const&>& steps, CString& errMsg);

/// Z4744 ranges: c1,c4 (1x64) 1..64; c2,c3 (MCS) 1..18; routing 1#1x64 / 2#1x64 <-> MCS sides.
BOOL ValidatePathStep(const SPathStep& s, CString& errMsg);

BOOL LoadPathCsvPd(LPCTSTR szPath, CArray<SPathStepPd, SPathStepPd const&>& steps, CString& errMsg);

/// PD: target 1..4; ch1 1..64; ch2 1..18; target=3 requires ch1<=32 (1# MCS), target=4 requires ch1>=33 (2# MCS).
BOOL ValidatePathStepPd(const SPathStepPd& s, CString& errMsg);
