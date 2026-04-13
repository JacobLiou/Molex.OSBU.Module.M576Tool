#pragma once

/// Same as legacy Z4671 UI: low-temperature LUT slot index.
#ifndef IDX_TEMP_LOW
#define IDX_TEMP_LOW 0
#endif

/// RECAL 0: TLS source channel 1~8 (2x8 switch channel per PRD).
#ifndef M576_DEFAULT_TLS_SOURCE
#define M576_DEFAULT_TLS_SOURCE 1
#endif
/// RECAL 0: wavelength (nm), PRD 850~1650; 1310 calibrator default.
#ifndef M576_DEFAULT_WAVELENGTH_NM
#define M576_DEFAULT_WAVELENGTH_NM 1310.0
#endif

/// RECAL 0 extended: delay (ms), DAC half-range, step — align with 429F firmware / Z4744.
#ifndef M576_MIN_RECAL_DELAY_MS
#define M576_MIN_RECAL_DELAY_MS 20
#endif
#ifndef M576_MAX_RECAL_DELAY_MS
#define M576_MAX_RECAL_DELAY_MS 100
#endif
#ifndef M576_DEFAULT_RECAL_DELAY_MS
#define M576_DEFAULT_RECAL_DELAY_MS 40
#endif
#ifndef M576_MIN_DAC_RANGE
#define M576_MIN_DAC_RANGE 1
#endif
#ifndef M576_MAX_DAC_RANGE
#define M576_MAX_DAC_RANGE 200
#endif
#ifndef M576_DEFAULT_DAC_RANGE
#define M576_DEFAULT_DAC_RANGE 64
#endif
#ifndef M576_MIN_DAC_STEP
#define M576_MIN_DAC_STEP 1
#endif
#ifndef M576_MAX_DAC_STEP
#define M576_MAX_DAC_STEP 100
#endif
#ifndef M576_DEFAULT_DAC_STEP
#define M576_DEFAULT_DAC_STEP 5
#endif

/// Peak grid → DAC: symmetric around mid DAC; PRD often uses base ± range (e.g. ±64). Tune with firmware.
#ifndef M576_PEAK_GRID_DAC_BASE
#define M576_PEAK_GRID_DAC_BASE 2048
#endif
#ifndef M576_PEAK_GRID_DAC_RANGE
#define M576_PEAK_GRID_DAC_RANGE 64
#endif

/// 2# MCS rows: if firmware maps 32 MCS to LUT[0..31] for 1# and LUT[16..31] or second bank for 2#, adjust (default: same index as 1#, last write wins — verify on integration).
#ifndef M576_MCS2_SW_INDEX_OFFSET
#define M576_MCS2_SW_INDEX_OFFSET 0
#endif

/// Z4671 GetLogFileData (0xC4): flash file / region type; confirm with firmware.
#ifndef M576_FLASH_FILE_TYPE
#define M576_FLASH_FILE_TYPE 0
#endif
/// Byte offset in flash for LUT bundle read (0 = start of type region; or e.g. 0x65000 if image-only).
#ifndef M576_FLASH_LUT_READ_BASE
#define M576_FLASH_LUT_READ_BASE 0
#endif
