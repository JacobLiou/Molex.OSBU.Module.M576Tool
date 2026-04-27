#pragma once
// 1310 nm 定标结果与已有 LUT 的合并：仅覆写低温槽，其它温区/元数据以 base 为准。

#include "Z4767StructDefine.h"
#include "M576OneX64Coef.h"
#include "CalibConstants.h"

/// Start from `base` (typically loaded from existing BIN) and overwrite only IDX_TEMP_LOW DAC pairs
/// with values taken from `src1310` (same dimensions).
// 在 base 上，用 src1310 中仅 IDX_TEMP_LOW 的 DAC 对覆盖同维项（同结构体布局）。
void MergeLut1310LowTempSlot(stLutSettingZ4671& base, const stLutSettingZ4671& src1310);

// 1x64：每块 2K 上仅用 src 的低温档 stChnDAC[IDX_LOW] 覆盖 base
void MergeMems1310LowTempSlot(stM576OneX64MemsSwCoef base[4], const stM576OneX64MemsSwCoef src[4]);
