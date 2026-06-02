#include "stdafx.h"
#include "LutMerge1310.h"
// 将 1310nm 定标源 LUT 的低温档（IDX_TEMP_LOW）写回基 LUT，并同步校准日期字段。

void MergeMems1310LowTempSlot(stM576OneX64MemsSwCoef base[4], const stM576OneX64MemsSwCoef src[4])
{
	for (int b = 0; b < 4; ++b)
	{
		stM576OneX64ChnDAC& dst = base[b].stCalibDAC[0];
		const stM576OneX64ChnDAC& srcSlot = src[b].stCalibDAC[0];
		for (unsigned ci = 0; ci < M576_1X64_PM_MERGE_CHN_COUNT; ++ci)
			dst.stChnDAC[ci] = srcSlot.stChnDAC[ci];
		if (srcSlot.wValid != 0)
			dst.wValid = srcSlot.wValid;
		// stChnDAC[17..35], stMidDAC: keep backup in base (PM maps CH_y 1..17 only).
	}
}

void MergeLut1310LowTempSlot(stLutSettingZ4671& base, const stLutSettingZ4671& src1310)
{
	for (int sw = 0; sw < M576_MCS_LUT_SW_MERGE_COUNT; ++sw)
	{
		for (unsigned ch = 0; ch < M576_MCS_LUT_MERGE_CHN_COUNT; ++ch)
		{
			base.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0] = src1310.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0];
			base.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][1] = src1310.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][1];
		}
		// ch 18..31 (CH19..CH32): keep backup in base.
		base.wTemperaturePoint[sw][IDX_TEMP_LOW] = src1310.wTemperaturePoint[sw][IDX_TEMP_LOW];
	}
	memcpy(base.pchCalibDate, src1310.pchCalibDate, sizeof(base.pchCalibDate));
}
