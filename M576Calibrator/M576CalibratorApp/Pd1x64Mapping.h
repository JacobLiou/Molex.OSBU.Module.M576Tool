#pragma once
// PD 2#1x64 firmware mapping: pd_1x64_2Mapping.csv -> SW_x + CH_y (PD only; do not use for PM).

#include "PathCsvDriver.h"

/// One mapping row: cols 1..3 match PD path; cols 4..5 = SW 1..4 and CH_y 1..17 (1-based in file).
struct SMems1x64PdMapRow
{
	int targetSwitchIndex{ 0 };
	int ch1{ 0 };
	int ch2{ 0 };
	int sw1to4{ 0 };
	int chY1based{ 0 };
};

/// `pd_1x64_2.csv` -> `pd_1x64_2Mapping.csv` (insert "Mapping" before ".csv").
BOOL Pd1x64ResolveMappingPath(LPCTSTR pdCsvPath, CString& outMappingPath);

BOOL LoadPd1x64MappingCsv(LPCTSTR szPath, CArray<SMems1x64PdMapRow, SMems1x64PdMapRow const&>& rows, CString& errMsg);

BOOL ValidatePdStepsAgainstMapping(
	const CArray<SPathStepPd, SPathStepPd const&>& steps,
	const CArray<SMems1x64PdMapRow, SMems1x64PdMapRow const&>& mapRows,
	CString& errMsg);
