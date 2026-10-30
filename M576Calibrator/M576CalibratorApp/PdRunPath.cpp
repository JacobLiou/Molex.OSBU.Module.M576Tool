#include "stdafx.h"
#include "PdRunPath.h"
#include "LutPeakApply.h"
#include "PdProtocol.h"
// PdRunPath.cpp：0415 Command C 编排辅助（Mapping 门禁 + 会话落盘）；不触碰 PM。

BOOL M576PdRunPath::Prepare1x64MappingOrFail(
	int fileSlot,
	LPCTSTR pdCsvAbsPath,
	const CArray<SPathStepPd, SPathStepPd const&>& steps,
	CArray<SMems1x64PdMapRow, SMems1x64PdMapRow const&>& outMapRows,
	CString& errMsg)
{
	outMapRows.RemoveAll();
	errMsg.Empty();
	/// PD only calibrates 2#1x64 (trans slot 3). Slot 2 (1#1x64) must stay empty / unused.
	if (fileSlot != 3)
		return TRUE;
	if (steps.GetSize() == 0)
		return TRUE;
	CString mapPath;
	if (pdCsvAbsPath == NULL || pdCsvAbsPath[0] == 0 || !Pd1x64ResolveMappingPath(pdCsvAbsPath, mapPath)
		|| !LoadPd1x64MappingCsv(mapPath, outMapRows, errMsg)
		|| !ValidatePdStepsAgainstMapping(steps, outMapRows, errMsg))
	{
		if (errMsg.IsEmpty())
			errMsg = _T("PD 1x64: mapping CSV missing or invalid.");
		return FALSE;
	}
	return TRUE;
}

BOOL M576PdRunPath::ApplyPeakToSession(
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
	CString& errMsg)
{
	errMsg.Empty();
	(void)occT3;
	(void)occT4;
	if (fileSlot < 0 || fileSlot > 3)
	{
		errMsg = _T("PD ApplyPeak: bad fileSlot");
		return FALSE;
	}
	if (fileSlot < 2)
	{
		M576PdProtocol::SMcsLutAddr addr;
		if (!M576PdProtocol::ResolveMcsLutAddr(step.targetSwitchIndex, step.ch1, step.ch2, addr))
		{
			errMsg = _T("PD ApplyPeak: MCS address resolve failed");
			return FALSE;
		}
		ApplyRecalPeakToLutPd(step, occT3, occT4, dacX, dacY, mcsTempSlot, lutByTrans[fileSlot]);
		return TRUE;
	}
	if (fileSlot != 3)
	{
		errMsg = _T("PD ApplyPeak: 1#1x64 (slot2) is not calibrated in PD mode");
		return FALSE;
	}
	if (pathLine0Based < 0 || pathLine0Based >= mapRows.GetSize())
	{
		errMsg.Format(_T("PD ApplyPeak: mapping row missing for path line %d"), pathLine0Based + 1);
		return FALSE;
	}
	const SMems1x64PdMapRow& m = mapRows[pathLine0Based];
	ApplyRecalPeakToMems1x64PdMapped(
		step, m.sw1to4, m.chY1based, dacX, dacY, memsCalibSlot, mems1x64[1]);
	return TRUE;
}
