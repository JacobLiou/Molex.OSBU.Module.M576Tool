#pragma once
// 1310 vs 1550: which LUT/Mems temperature slot to fill during Run Path and merge on Write BIN.

#include "CalibConstants.h"

enum class M576CalibBinWritePolicy
{
	Slot1310Low,
	Slot1550RoomThenCopyHigh,
};

M576CalibBinWritePolicy PolicyFromWavelengthNm(int wavelengthNm);
int McsPrimaryTempSlot(M576CalibBinWritePolicy policy);
int MemsPrimaryCalibSlot(M576CalibBinWritePolicy policy);
