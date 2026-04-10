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

/// PD path (Command C / RECAL 2): target 1..4 + two segments — 2#1x64 (block, ch) + MCS (block, ch).
/// Z4744: optical path [2#1x64 ch][1#MCS ch] or [2#1x64 ch][2#MCS ch]; blocks 1/2 select stage or MCS bank.
struct SPathStepPd
{
	int targetSwitchIndex;
	int p2b, p2c, p3b, p3c;
	SPathStepPd()
		: targetSwitchIndex(0)
		, p2b(0), p2c(0), p3b(0), p3c(0)
	{
	}
};

BOOL LoadPathCsv(LPCTSTR szPath, CArray<SPathStep, SPathStep const&>& steps, CString& errMsg);

/// Z4744 ranges: c1,c4 (1x64) 1..64; c2,c3 (MCS) 1..18; routing 1#1x64 / 2#1x64 <-> MCS sides.
BOOL ValidatePathStep(const SPathStep& s, CString& errMsg);

BOOL LoadPathCsvPd(LPCTSTR szPath, CArray<SPathStepPd, SPathStepPd const&>& steps, CString& errMsg);

/// PD: target 1..4; 2#1x64 ch 1..32 -> 1# MCS, 33..64 -> 2# MCS; target index vs stage/MCS consistency.
BOOL ValidatePathStepPd(const SPathStepPd& s, CString& errMsg);
