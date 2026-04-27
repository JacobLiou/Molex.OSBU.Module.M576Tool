#pragma once
// 将单步定标目标（RECAL 1/2 的 target）映射到 439F 上 trans 槽 0~3，并判定该步是否属某分文件 CSV 槽位。

#include "PathCsvDriver.h"

/// Map RECAL path step to trans slot index 0..3 (trans 1..4 on wire).
// PM/PD 目标 → trans 槽 0~3（串口上为 trans 1~4）。用于按路选 LUT/CSV。
int TransSlotFromPmTarget(int targetSwitchIndex);
int TransSlotFromPdTarget(int targetSwitchIndex);

/// True if PM step belongs to the given CSV file slot (0=1#MCS, 1=2#MCS, 2=1#1x64, 3=2#1x64).
// 是否落在指定 CSV 分文件（0~3 同四路产线分束）。
BOOL PmStepMatchesFileSlot(const SPathStep& step, int fileSlot, CString* pWarnMsg);

/// True if PD step belongs to the given CSV file slot.
// 同上，PD 步。
BOOL PdStepMatchesFileSlot(const SPathStepPd& step, int fileSlot, CString* pWarnMsg);
