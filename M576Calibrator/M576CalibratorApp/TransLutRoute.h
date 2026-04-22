#pragma once

#include "PathCsvDriver.h"

/// Map RECAL path step to trans slot index 0..3 (trans 1..4 on wire).
int TransSlotFromPmTarget(int targetSwitchIndex);
int TransSlotFromPdTarget(int targetSwitchIndex);

/// True if PM step belongs to the given CSV file slot (0=1#MCS, 1=2#MCS, 2=1#1x64, 3=2#1x64).
BOOL PmStepMatchesFileSlot(const SPathStep& step, int fileSlot, CString* pWarnMsg);

/// True if PD step belongs to the given CSV file slot.
BOOL PdStepMatchesFileSlot(const SPathStepPd& step, int fileSlot, CString* pWarnMsg);
