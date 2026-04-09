#pragma once

#include "Z4767StructDefine.h"
#include "CalibConstants.h"

/// Start from `base` (typically loaded from existing BIN) and overwrite only IDX_TEMP_LOW DAC pairs
/// with values taken from `src1310` (same dimensions).
void MergeLut1310LowTempSlot(stLutSettingZ4671& base, const stLutSettingZ4671& src1310);
