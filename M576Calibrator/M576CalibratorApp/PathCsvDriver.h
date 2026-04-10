#pragma once

#include <afxtempl.h>

/// One row of path_0330-style CSV: target index + four (block, channel) pairs for RECAL 1.
/// Order matches PRD: [1#1x64][1#MCS][2#MCS][2#1x64] — channel ranges validated separately.
struct SPathStep
{
	int targetSwitchIndex;
	int p1b, p1c, p2b, p2c, p3b, p3c, p4b, p4c;
	SPathStep()
		: targetSwitchIndex(0)
		, p1b(0), p1c(0), p2b(0), p2c(0), p3b(0), p3c(0), p4b(0), p4c(0)
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

/// PRD: 1x64 channels 1..64, MCS 1..18 — uses segment index 1..4 (1,4 = 1x64; 2,3 = MCS).
BOOL ValidatePathStep(const SPathStep& s, CString& errMsg);

BOOL LoadPathCsvPd(LPCTSTR szPath, CArray<SPathStepPd, SPathStepPd const&>& steps, CString& errMsg);

/// PD: target 1..4; 2#1x64 ch 1..32 -> 1# MCS, 33..64 -> 2# MCS; target index vs stage/MCS consistency.
BOOL ValidatePathStepPd(const SPathStepPd& s, CString& errMsg);
