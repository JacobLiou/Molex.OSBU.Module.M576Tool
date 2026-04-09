#include "stdafx.h"
#include "LutMerge1310.h"

void MergeLut1310LowTempSlot(stLutSettingZ4671& base, const stLutSettingZ4671& src1310)
{
	for (int sw = 0; sw < 34; ++sw)
	{
		for (int ch = 0; ch < PORT_MAX_COUNT + MID_MAX_COUNT; ++ch)
		{
			base.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0] = src1310.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0];
			base.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][1] = src1310.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][1];
		}
		base.wTemperaturePoint[sw][IDX_TEMP_LOW] = src1310.wTemperaturePoint[sw][IDX_TEMP_LOW];
	}
	memcpy(base.pchCalibDate, src1310.pchCalibDate, sizeof(base.pchCalibDate));
}
