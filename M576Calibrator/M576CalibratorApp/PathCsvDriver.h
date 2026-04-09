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

BOOL LoadPathCsv(LPCTSTR szPath, CArray<SPathStep, SPathStep const&>& steps, CString& errMsg);

/// PRD: 1x64 channels 1..64, MCS 1..18 — uses segment index 1..4 (1,4 = 1x64; 2,3 = MCS).
BOOL ValidatePathStep(const SPathStep& s, CString& errMsg);
