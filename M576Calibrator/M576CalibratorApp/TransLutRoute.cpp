#include "stdafx.h"
#include "TransLutRoute.h"

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
