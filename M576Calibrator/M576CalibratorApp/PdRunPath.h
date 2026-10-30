#pragma once
// PD (Z4744 0415 Command C) Run Path helpers — isolated from PM. Peak loop stays on Dlg for shared
// RunPath utils; this module owns mapping gate + MEMS mapped apply contracts.

#include "PathCsvDriver.h"
#include "Pd1x64Mapping.h"
#include "M576OneX64Coef.h"
#include "Z4767StructDefine.h"

namespace M576PdRunPath
{

/// fileSlot 3 (2#1x64): require pd_*Mapping.csv aligned with steps. Other slots: no-op success.
BOOL Prepare1x64MappingOrFail(
	int fileSlot,
	LPCTSTR pdCsvAbsPath,
	const CArray<SPathStepPd, SPathStepPd const&>& steps,
	CArray<SMems1x64PdMapRow, SMems1x64PdMapRow const&>& outMapRows,
	CString& errMsg);

/// Write peak into MCS LUT (fileSlot 0/1) or mapped 2#1x64 MEMS (fileSlot 3).
BOOL ApplyPeakToSession(
	const SPathStepPd& step,
	int fileSlot,
	int pathLine0Based,
	int occT3,
	int occT4,
	unsigned short dacX,
	unsigned short dacY,
	int mcsTempSlot,
	int memsCalibSlot,
	const CArray<SMems1x64PdMapRow, SMems1x64PdMapRow const&>& mapRows,
	stLutSettingZ4671 lutByTrans[4],
	stM576OneX64MemsSwCoef mems1x64[2][4],
	CString& errMsg);

} // namespace M576PdRunPath
