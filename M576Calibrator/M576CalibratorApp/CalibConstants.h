#pragma once

/// Same as legacy Z4671 UI: low-temperature LUT slot index.
#ifndef IDX_TEMP_LOW
#define IDX_TEMP_LOW 0
#endif

/// TLS source channel 1~8 (2x8 switch). Command A: `RECAL 0 <tls> <nm> <pm_range>`.
#ifndef M576_MIN_TLS_SOURCE
#define M576_MIN_TLS_SOURCE 1
#endif
#ifndef M576_MAX_TLS_SOURCE
#define M576_MAX_TLS_SOURCE 8
#endif
#ifndef M576_DEFAULT_TLS_SOURCE
#define M576_DEFAULT_TLS_SOURCE 4
#endif
/// RECAL 0: wavelength (nm, int), PRD 850~1650.
#ifndef M576_MIN_WAVELENGTH_NM
#define M576_MIN_WAVELENGTH_NM 850
#endif
#ifndef M576_MAX_WAVELENGTH_NM
#define M576_MAX_WAVELENGTH_NM 1650
#endif
#ifndef M576_DEFAULT_WAVELENGTH_NM
#define M576_DEFAULT_WAVELENGTH_NM 1310
#endif
/// Power meter range index for Command A (PRD: 0..4, 4 = auto).
#ifndef M576_MIN_PM_RANGE
#define M576_MIN_PM_RANGE 0
#endif
#ifndef M576_MAX_PM_RANGE
#define M576_MAX_PM_RANGE 4
#endif
#ifndef M576_DEFAULT_PM_RANGE
#define M576_DEFAULT_PM_RANGE 1
#endif

/// Per-sweep params ride on `RECAL 3 / RECAL 5` (not on RECAL 0): delay (ms), DAC half-range, step.
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

/// RECAL 3 / 5 Base DAC: 9999 = firmware uses current channel DAC as sweep center (PRD); any other value = host sets base.
#ifndef M576_RECAL_FW_READ_BASE_DAC
#define M576_RECAL_FW_READ_BASE_DAC 9999
#endif

/// `RECAL 3` / `RECAL 5` sweep line read: `min(n*delay + margin, max)` — one global cap for PM/PD (not per opcode).
#ifndef M576_RECAL_SWEEP_READ_MARGIN_MS
#define M576_RECAL_SWEEP_READ_MARGIN_MS 2000
#endif
#ifndef M576_MAX_RECAL_SWEEP_READ_MS
#define M576_MAX_RECAL_SWEEP_READ_MS 5000
#endif
#ifndef M576_MIN_RECAL_SWEEP_READ_MS
#define M576_MIN_RECAL_SWEEP_READ_MS 1500
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

/// 439F passthrough: ASCII command / drain timing (tune with firmware).
#ifndef M576_439F_TRANS_DRAIN_IDLE_MS
#define M576_439F_TRANS_DRAIN_IDLE_MS 80
#endif
#ifndef M576_439F_TRANS_DRAIN_MAX_MS
#define M576_439F_TRANS_DRAIN_MAX_MS 3000
#endif
#ifndef M576_439F_TRANS_END_DRAIN_MAX_MS
#define M576_439F_TRANS_END_DRAIN_MAX_MS 1500
#endif
#ifndef M576_439F_POST_TRANS_MS
#define M576_439F_POST_TRANS_MS 50
#endif

#if defined(__cplusplus)
#include <cstddef>
/// Flash read-backup: trans channel index order (1=MCS1#, ... 4=1x64 #2 per site doc).
inline constexpr int g_m576FlashReadTransChannels[] = { 1, 2, 3, 4 };
inline constexpr std::size_t g_m576FlashReadTransChannelCount =
	sizeof(g_m576FlashReadTransChannels) / sizeof(g_m576FlashReadTransChannels[0]);
/// Burn list: default same as read; shrink to {1,2} for MCS-only during bring-up.
inline constexpr int g_m576FlashBurnTransChannels[] = { 1, 2, 3, 4 };
inline constexpr std::size_t g_m576FlashBurnTransChannelCount =
	sizeof(g_m576FlashBurnTransChannels) / sizeof(g_m576FlashBurnTransChannels[0]);
#endif
