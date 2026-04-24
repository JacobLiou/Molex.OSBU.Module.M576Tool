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

/// Z4671 GetLogFileData (0xC4): 16-bit file type (default 0x007B per FW); flash base `M576_FLASH_LUT_READ_BASE`.
#ifndef M576_FLASH_FILE_TYPE
#define M576_FLASH_FILE_TYPE 0
#endif
/// Start of `stLutSettingZ4671` in device flash (32-bit, big-endian in 0xC4); not the bundle file base—
/// headers are assembled on PC. Use 0 unless FW says otherwise.
#ifndef M576_FLASH_LUT_READ_BASE
#define M576_FLASH_LUT_READ_BASE 0U
#endif
/// 0xC4 LUT read: fixed steps of at most `M576_FLASH_READ_CHUNK_MAX` (128) bytes; last
/// 0xC4 may be 1..128 (remainder when `total` is not a multiple of 128) or 128 (final full block when it is).
#ifndef M576_FLASH_READ_CHUNK_MAX
#define M576_FLASH_READ_CHUNK_MAX 128
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

/// 1x64 MemsSw: one switch coef block = 2K = 0x800 B (CRC@+0x7FC in block); not MCS `stLutSettingZ4671`.
#ifndef M576_1X64_MEMS_BIN_SIZE
#define M576_1X64_MEMS_BIN_SIZE 2048u
#endif
/// 14538: four switch coefficient regions per 1x64; contiguous 4×2K = 8KB from `ADDR_SWITCH1_COEF`.
#ifndef ADDR_SWITCH1_COEF
#define ADDR_SWITCH1_COEF 0x0E000u
#endif
#ifndef ADDR_SWITCH2_COEF
#define ADDR_SWITCH2_COEF 0x0E800u
#endif
#ifndef ADDR_SWITCH3_COEF
#define ADDR_SWITCH3_COEF 0x0F000u
#endif
#ifndef ADDR_SWITCH4_COEF
#define ADDR_SWITCH4_COEF 0x0F800u
#endif
/// Full `MEM` read for one 1x64: 4 × 2K = 8KB from `M576_1X64_FLASH_BASE_TRANS*`. XMODEM burn sends up to this size.
#ifndef M576_1X64_MEMS_BACKUP_TOTAL_SIZE
#define M576_1X64_MEMS_BACKUP_TOTAL_SIZE (4u * M576_1X64_MEMS_BIN_SIZE)
#endif
/// `MEM` read: address step and payload bytes per response (32 B from 64 hex chars; 64 steps * 32 = 2048 B).
#ifndef M576_1X64_MEM_ADDR_STEP
#define M576_1X64_MEM_ADDR_STEP 32u
#endif
#ifndef M576_1X64_MEM_PAYLOAD_BYTES
#define M576_1X64_MEM_PAYLOAD_BYTES 32u
#endif
/// `MEM` reply: wait up to this long (multi-chunk may arrive >100ms apart; see `MemDrainHexResponse`).
#ifndef M576_1X64_MEM_READ_MAX_MS
#define M576_1X64_MEM_READ_MAX_MS 5000u
#endif
/// After 64+ payload hex in buffer, end when RX idle this long (ms) so the full line/CR is flushed.
#ifndef M576_1X64_MEM_READ_IDLE_OK_MS
#define M576_1X64_MEM_READ_IDLE_OK_MS 60u
#endif
/// Extra delay after `MEM` TX before starting RX drain (firmware / 439F may need time before payload bytes).
#ifndef M576_1X64_MEM_AFTER_CMD_MS
#define M576_1X64_MEM_AFTER_CMD_MS 120u
#endif
/// Per-tunnel 1x64 local flash: read starts at first switch block (0x0E000..0x0F800 = 4×2K). trans3/trans4 are different devices via 439F `trans`.
#ifndef M576_1X64_FLASH_BASE_TRANS3
#define M576_1X64_FLASH_BASE_TRANS3 ADDR_SWITCH1_COEF
#endif
#ifndef M576_1X64_FLASH_BASE_TRANS4
#define M576_1X64_FLASH_BASE_TRANS4 ADDR_SWITCH1_COEF
#endif
/// After `fwdl\\r` before first XMODEM block (legacy `SendRevCommand` used long sleep).
#ifndef M576_1X64_FWDL_PRE_MS
#define M576_1X64_FWDL_PRE_MS 5000u
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

/// Suffix before ".bin" per 439F trans 1~4 (e.g. `x_mcs1.bin`), same order as `pm_*.csv` / `pd_*.csv` stems.
inline constexpr const TCHAR* const g_m576TransLutBinSuffix[4] = {
	_T("_mcs1"), _T("_mcs2"), _T("_1x64_1"), _T("_1x64_2")
};

/// Default path CSV per trans slot: 0=1#MCS, 1=2#MCS, 2=1#1x64, 3=2#1x64 (ASCII filenames).
inline constexpr const TCHAR* const g_m576DefaultPmCsvRel[4] = {
	_T("output\\pm_mcs1.csv"),
	_T("output\\pm_mcs2.csv"),
	_T("output\\pm_1x64_1.csv"),
	_T("output\\pm_1x64_2.csv"),
};
inline constexpr const TCHAR* const g_m576DefaultPdCsvRel[4] = {
	_T("output\\pd_mcs1.csv"),
	_T("output\\pd_mcs2.csv"),
	_T("output\\pd_1x64_1.csv"),
	_T("output\\pd_1x64_2.csv"),
};
#endif
