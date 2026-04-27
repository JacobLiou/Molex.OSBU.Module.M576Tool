#include "stdafx.h"
#include "TransLutRoute.h"
// PM/PD 的 target 与 439F trans 槽 0~3 的固定映射，以及分 CSV 文件槽过滤（供 RunPath* 分文件跑）。

// PM：Z4744 target 1/2/3/4/5/6 → 四个 trans 槽位（2#1x64 两阶段、MCS 两路再映射到 1#1x64/2#1x64 等，见下 switch）。
int TransSlotFromPmTarget(int targetSwitchIndex)
{
	switch (targetSwitchIndex)
	{
	case 3: return 0;
	case 4: return 1;
	case 1:
	case 2: return 2;
	case 5:
	case 6: return 3;
	default: return -1;
	}
}

// PD：仅四个 target 语义，1/2 在 PD 中映射到 trans4（2#1x64），3/4 映射到槽 0/1（MCS）。
int TransSlotFromPdTarget(int targetSwitchIndex)
{
	switch (targetSwitchIndex)
	{
	case 3: return 0;
	case 4: return 1;
	case 1:
	case 2: return 3;
	default: return -1;
	}
}

// 判定 PM 行是否应出现在指定 fileSlot 的 pm_*.csv 中（错槽则报警告文案）。
BOOL PmStepMatchesFileSlot(const SPathStep& step, int fileSlot, CString* pWarnMsg)
{
	const int slot = TransSlotFromPmTarget(step.targetSwitchIndex);
	if (slot < 0)
	{
		if (pWarnMsg)
			pWarnMsg->Format(_T("PM: unknown target_index %d"), step.targetSwitchIndex);
		return FALSE;
	}
	if (slot != fileSlot)
	{
		if (pWarnMsg)
			pWarnMsg->Format(_T("PM: row target_index %d maps to trans slot %d, but CSV is for slot %d."),
				step.targetSwitchIndex, slot + 1, fileSlot + 1);
		return FALSE;
	}
	return TRUE;
}

// 同上，PD 行与 pd_*.csv 分槽。
BOOL PdStepMatchesFileSlot(const SPathStepPd& step, int fileSlot, CString* pWarnMsg)
{
	const int slot = TransSlotFromPdTarget(step.targetSwitchIndex);
	if (slot < 0)
	{
		if (pWarnMsg)
			pWarnMsg->Format(_T("PD: unknown target_index %d"), step.targetSwitchIndex);
		return FALSE;
	}
	if (slot != fileSlot)
	{
		if (pWarnMsg)
			pWarnMsg->Format(_T("PD: row target_index %d maps to trans slot %d, but CSV is for slot %d."),
				step.targetSwitchIndex, slot + 1, fileSlot + 1);
		return FALSE;
	}
	return TRUE;
}
