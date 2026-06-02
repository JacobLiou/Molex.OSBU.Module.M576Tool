#pragma once
// 1310 nm 定标结果与已有 LUT 的合并：仅覆写低温槽，其它温区/元数据以 base 为准。

#include "Z4767StructDefine.h"
#include "M576OneX64Coef.h"
#include "CalibConstants.h"

/// `base` + session `src1310`: IDX_TEMP_LOW DAC for sw 0..31, ch 0..17 only (path CH1..18); ch 18..31 and sw 32/33 unchanged in `base`.
// 在 base 上合并 session 低温 DAC：仅 ch 0..17；CH19+ 低温槽保留 backup。
void MergeLut1310LowTempSlot(stLutSettingZ4671& base, const stLutSettingZ4671& src1310);

// 1x64：低温档仅合并 PM 映射的 stChnDAC[0..16]（CH_y 1..17）；第 18 路 [17] 及以后、stMidDAC 保留 base。
void MergeMems1310LowTempSlot(stM576OneX64MemsSwCoef base[4], const stM576OneX64MemsSwCoef src[4]);
