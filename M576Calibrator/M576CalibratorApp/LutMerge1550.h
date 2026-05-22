#pragma once
// 1550 nm: merge room-temp slot from session, copy to high-temp slot, set 30 C metadata.

#include "Z4767StructDefine.h"
#include "M576OneX64Coef.h"

void MergeLut1550RoomHighSlots(stLutSettingZ4671& base, const stLutSettingZ4671& src1550);
void MergeMems1550RoomHighSlots(stM576OneX64MemsSwCoef base[4], const stM576OneX64MemsSwCoef src[4]);
