#pragma once
// 1310 nm 定标结果与已有 LUT 的合并：仅覆写低温槽，其它温区/元数据以 base 为准。

#include "Z4767StructDefine.h"
#include "CalibConstants.h"

/// Start from `base` (typically loaded from existing BIN) and overwrite only IDX_TEMP_LOW DAC pairs
/// with values taken from `src1310` (same dimensions).
// 在 base 上，用 src1310 中仅 IDX_TEMP_LOW 的 DAC 对覆盖同维项（同结构体布局）。
void MergeLut1310LowTempSlot(stLutSettingZ4671& base, const stLutSettingZ4671& src1310);
