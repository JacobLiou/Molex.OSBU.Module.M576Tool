#include "stdafx.h"
#include "LutMerge1550.h"
#include "CalibConstants.h"
// LutMerge1550.cpp：1550nm 定标结果合并常温槽并拷贝到高温槽（MCS + 1x64 MEMS）。
#include <cstring>

void MergeMems1550RoomHighSlots(stM576OneX64MemsSwCoef base[4], const stM576OneX64MemsSwCoef src[4])
{
	for (int b = 0; b < 4; ++b)
	{
		base[b].stCalibDAC[1] = src[b].stCalibDAC[1];
		base[b].stCalibDAC[2] = base[b].stCalibDAC[1];
	}
}

void MergeLut1550RoomHighSlots(stLutSettingZ4671& base, const stLutSettingZ4671& src1550)
{
	const int chCount = PORT_MAX_COUNT + MID_MAX_COUNT;
	for (int sw = 0; sw < 34; ++sw)
	{
		for (int ch = 0; ch < chCount; ++ch)
		{
			base.wCalibPtrDAC[sw][IDX_TEMP_ROOM][ch][0] = src1550.wCalibPtrDAC[sw][IDX_TEMP_ROOM][ch][0];
			base.wCalibPtrDAC[sw][IDX_TEMP_ROOM][ch][1] = src1550.wCalibPtrDAC[sw][IDX_TEMP_ROOM][ch][1];
			base.wCalibPtrDAC[sw][IDX_TEMP_HIGH][ch][0] = base.wCalibPtrDAC[sw][IDX_TEMP_ROOM][ch][0];
			base.wCalibPtrDAC[sw][IDX_TEMP_HIGH][ch][1] = base.wCalibPtrDAC[sw][IDX_TEMP_ROOM][ch][1];
		}
		base.wTemperaturePoint[sw][IDX_TEMP_ROOM] = (SHORT)M576_MCS_WTEMP_POINT_30C;
		base.wTemperaturePoint[sw][IDX_TEMP_HIGH] = (SHORT)M576_MCS_WTEMP_POINT_30C;
	}
	memcpy(base.pchCalibDate, src1550.pchCalibDate, sizeof(base.pchCalibDate));
}
