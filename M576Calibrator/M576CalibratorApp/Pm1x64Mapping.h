#pragma once
// Firmware row-aligned mapping: pm_1x64_NMapping.csv -> SW_x + CH_y for MEMS low-temp DAC slot (PM only).

#include "PathCsvDriver.h"

/// One mapping row: cols 1?C5 match PM path; cols 6?C7 = SW 1..4 and CH_y 1..17 (1-based in file).
struct SMems1x64PmMapRow
{
	int targetSwitchIndex{ 0 };
	int c1{ 0 };
	int c2{ 0 };
	int c3{ 0 };
	int c4{ 0 };
	int sw1to4{ 0 };
	int chY1based{ 0 };
};

/// `pm_1x64_1.csv` -> `pm_1x64_1Mapping.csv` (insert "Mapping" before ".csv").
BOOL Pm1x64ResolveMappingPath(LPCTSTR pmCsvPath, CString& outMappingPath);

BOOL LoadPm1x64MappingCsv(LPCTSTR szPath, CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&>& rows, CString& errMsg);

BOOL ValidatePmStepsAgainstMapping(
	const CArray<SPathStep, SPathStep const&>& steps,
	const CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&>& mapRows,
	CString& errMsg);
