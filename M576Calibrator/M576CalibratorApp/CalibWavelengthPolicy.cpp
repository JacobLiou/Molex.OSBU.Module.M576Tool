#include "stdafx.h"
#include "CalibWavelengthPolicy.h"

M576CalibBinWritePolicy PolicyFromWavelengthNm(int wavelengthNm)
{
	if (wavelengthNm == 1550)
		return M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh;
	return M576CalibBinWritePolicy::Slot1310Low;
}

int McsPrimaryTempSlot(M576CalibBinWritePolicy policy)
{
	switch (policy)
	{
	case M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh:
		return (int)IDX_TEMP_ROOM;
	default:
		return (int)IDX_TEMP_LOW;
	}
}

int MemsPrimaryCalibSlot(M576CalibBinWritePolicy policy)
{
	switch (policy)
	{
	case M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh:
		return 1;
	default:
		return 0;
	}
}
