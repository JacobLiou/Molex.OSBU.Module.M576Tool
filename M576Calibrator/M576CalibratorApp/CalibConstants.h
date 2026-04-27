#pragma once
// 本头文件：M576 上位机与 439F/固件 约定的定标、通信、Flash/1x64 等常量。勿改宏值以免与设备不一致。

/// Same as legacy Z4671 UI: low-temperature LUT slot index.
// 与旧 Z4671 界面相同：低温档 LUT 温度槽下标（IDX_TEMP_LOW）。
#ifndef IDX_TEMP_LOW
#define IDX_TEMP_LOW 0
#endif

/// TLS source channel 1~8 (2x8 switch). Command A: `RECAL 0 <tls> <nm> <pm_range>`.
// TLS 光源路选通 1~8（2×8 开关）。对应命令 A：RECAL 0。
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
// RECAL 0 波长（nm 整数，PRD 约 850~1650）。
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
// 命令 A 的功率计档位索引（0~4，4 常为自动档）。
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
// 单条扫频参数挂在 RECAL 3/5 上（不在 RECAL 0）：延迟(ms)、DAC 半幅、步进。
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
#define M576_DEFAULT_DAC_STEP 4
#endif

/// RECAL 3 / 5 Base DAC: 9999 = firmware uses current channel DAC as sweep center (PRD); any other value = host sets base.
// RECAL 3/5 基准 DAC：9999 表示由固件读当前通道 DAC 为扫频中心，其余值由上位机写基准。
#ifndef M576_RECAL_FW_READ_BASE_DAC
#define M576_RECAL_FW_READ_BASE_DAC 9999
#endif

/// `RECAL 3` / `RECAL 5` sweep line read: `min(n*delay + margin, max)` — one global cap for PM/PD (not per opcode).
// 读 RECAL 3/5 扫频行总超时上界（PM/PD 共用，非按指令分别配）。
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
// 二维寻峰格点对应 DAC 区间：中点与半宽，与固件联调。
#ifndef M576_PEAK_GRID_DAC_BASE
#define M576_PEAK_GRID_DAC_BASE 2048
#endif
#ifndef M576_PEAK_GRID_DAC_RANGE
#define M576_PEAK_GRID_DAC_RANGE 64
#endif

/// 2# MCS rows: if firmware maps 32 MCS to LUT[0..31] for 1# and LUT[16..31] or second bank for 2#, adjust (default: same index as 1#, last write wins — verify on integration).
// 2# MCS 行在 LUT 中的下标偏移（与 1# 对齐或分 bank 时由集成态确认；默认 0 表示与 1# 同下标，后写覆盖需验证）。
#ifndef M576_MCS2_SW_INDEX_OFFSET
#define M576_MCS2_SW_INDEX_OFFSET 0
#endif

/// Z4671 GetLogFileData (0xC4): 16-bit file type (default 0x007B per FW); flash base `M576_FLASH_LUT_READ_BASE`.
// Z4671 0xC4 读文件：16 位文件类型；LUT 在 Flash 中的 32 位基址由宏给出（大端在协议中）。
#ifndef M576_FLASH_FILE_TYPE
#define M576_FLASH_FILE_TYPE 0
#endif
/// Start of `stLutSettingZ4671` in device flash (32-bit, big-endian in 0xC4); not the bundle file base—
/// headers are assembled on PC. Use 0 unless FW says otherwise.
// 设备 Flash 中 stLutSettingZ4671 起点（0xC4 协议中 32 位大端），非整包文件偏移；无特殊说明时可为 0。
#ifndef M576_FLASH_LUT_READ_BASE
#define M576_FLASH_LUT_READ_BASE 0U
#endif
/// 0xC4 LUT read: fixed steps of at most `M576_FLASH_READ_CHUNK_MAX` (128) bytes; last
/// 0xC4 may be 1..128 (remainder when `total` is not a multiple of 128) or 128 (final full block when it is).
// 0xC4 按块读 LUT：每步最多 128 字节，末包可为残块或整 128 字节块。
#ifndef M576_FLASH_READ_CHUNK_MAX
#define M576_FLASH_READ_CHUNK_MAX 128
#endif

/// Polly-style comm retry: failures after the first try (3) => 4 total attempts (1 initial + 3 on-failure).
// 类 Polly 的通信重试：失败后额外尝试次数，总尝试次数 = 1 + 该值（见 M576_COMM_RETRY_MAX_ATTEMPTS）。
#ifndef M576_COMM_RETRY_ON_FAILURE
#define M576_COMM_RETRY_ON_FAILURE 2
#endif
#ifndef M576_COMM_RETRY_MAX_ATTEMPTS
#define M576_COMM_RETRY_MAX_ATTEMPTS (1 + M576_COMM_RETRY_ON_FAILURE)
#endif
/// Fixed delay before re-attempting a full send/response or binary exchange.
// 整次收发或二进制交换失败后的固定间隔再重试（毫秒）。
#ifndef M576_COMM_RETRY_DELAY_MS
#define M576_COMM_RETRY_DELAY_MS 100U
#endif
/// Low-level `WriteBuffer` / `WriteBufferNoPurge` short retries (same cap as full exchange by default).
// 底层写缓冲的短重试次数上限，默认与整次交换最大尝试数一致。
#ifndef M576_COMM_WRITE_RETRY_MAX
#define M576_COMM_WRITE_RETRY_MAX M576_COMM_RETRY_MAX_ATTEMPTS
#endif

/// 439F passthrough: ASCII command / drain timing (tune with firmware).
// 439F 透传 trans：ASCII 下发后排空接收、结束 trans 的等待上界/空闲判据，与固件调参。
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
// 1x64 侧单路 Mems 系数块 2KB（含块尾 CRC），与 MCS 0xC4 读写的 LUT 结构体不同。
#ifndef M576_1X64_MEMS_BIN_SIZE
#define M576_1X64_MEMS_BIN_SIZE 2048u
#endif
/// 14538: four switch coefficient regions per 1x64; contiguous 4×2K = 8KB from `ADDR_SWITCH1_COEF`.
// 每片 1x64 四路开关系数在 Flash 上的起址步进（0x0E000 起每路 0x800）。
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
// 单次备份一片 1x64：4×2K 共 8KB；XMODEM 烧录时对应最大长度亦为此。
#ifndef M576_1X64_MEMS_BACKUP_TOTAL_SIZE
#define M576_1X64_MEMS_BACKUP_TOTAL_SIZE (4u * M576_1X64_MEMS_BIN_SIZE)
#endif
/// `MEM` read: address step and payload bytes per response (32 B from 64 hex chars; 64 steps * 32 = 2048 B).
// MEM 回读：地址步进与每回应负载字节数（与十六进制行格式配套）。
#ifndef M576_1X64_MEM_ADDR_STEP
#define M576_1X64_MEM_ADDR_STEP 32u
#endif
#ifndef M576_1X64_MEM_PAYLOAD_BYTES
#define M576_1X64_MEM_PAYLOAD_BYTES 32u
#endif
/// `MEM` reply: wait up to this long (multi-chunk may arrive >100ms apart; see `MemDrainHexResponse`).
// MEM 回包整体最长等待；多块到达间隔可能较大。
#ifndef M576_1X64_MEM_READ_MAX_MS
#define M576_1X64_MEM_READ_MAX_MS 5000u
#endif
/// After 64+ payload hex in buffer, end when RX idle this long (ms) so the full line/CR is flushed.
// 缓冲内已收到足够 payload 十六进制后，以接收空闲时间判定一帧结束、避免截断行。
#ifndef M576_1X64_MEM_READ_IDLE_OK_MS
#define M576_1X64_MEM_READ_IDLE_OK_MS 60u
#endif
/// Extra delay after `MEM` TX before starting RX drain (firmware / 439F may need time before payload bytes).
// 发出 MEM 命令后额外延时再开始收，给下游固件/439F 准备数据的时间。
#ifndef M576_1X64_MEM_AFTER_CMD_MS
#define M576_1X64_MEM_AFTER_CMD_MS 120u
#endif
/// 1x64: SN string stored in flash; read via one `MEM` line at this address, 32 B payload, interpret as printable ASCII.
// 1x64 序列号区：与 MCS `GetProductSN`（0xA2）不同，走 MEM 十六进制行读 32 字节再当字符串用。
#ifndef M576_1X64_SN_MEM_ADDR
#define M576_1X64_SN_MEM_ADDR 0x0000D800u
#endif
/// Per-tunnel 1x64 local flash: read starts at first switch block (0x0E000..0x0F800 = 4×2K). trans3/trans4 are different devices via 439F `trans`.
// 经 439F 不同 trans 隧道连到不同 1x64 设备时，其本地 Flash 读起址（与 ADDR_SWITCH* 区一致）。
#ifndef M576_1X64_FLASH_BASE_TRANS3
#define M576_1X64_FLASH_BASE_TRANS3 ADDR_SWITCH1_COEF
#endif
#ifndef M576_1X64_FLASH_BASE_TRANS4
#define M576_1X64_FLASH_BASE_TRANS4 ADDR_SWITCH1_COEF
#endif
/// After `fwdl\\r` before first XMODEM block (legacy `SendRevCommand` used long sleep).
// 发 fwdl 后、首包 XMODEM 前的空窗（兼容旧长延时行为）。
#ifndef M576_1X64_FWDL_PRE_MS
#define M576_1X64_FWDL_PRE_MS 5000u
#endif

#if defined(__cplusplus)
#include <cstddef>
/// Flash read-backup: trans channel index order (1=MCS1#, ... 4=1x64 #2 per site doc).
// 读 Flash/备份时按 trans 槽顺序：1#MCS, 2#MCS, 1#1x64, 2#1x64（与现网文档一致）。
inline constexpr int g_m576FlashReadTransChannels[] = { 1, 2, 3, 4 };
inline constexpr std::size_t g_m576FlashReadTransChannelCount =
	sizeof(g_m576FlashReadTransChannels) / sizeof(g_m576FlashReadTransChannels[0]);
/// Burn list: default same as read; shrink to {1,2} for MCS-only during bring-up.
// 烧录列表，默认与读序相同；样机期可仅烧 {1,2} 做 MCS 联调。
inline constexpr int g_m576FlashBurnTransChannels[] = { 1, 2, 3, 4 };
inline constexpr std::size_t g_m576FlashBurnTransChannelCount =
	sizeof(g_m576FlashBurnTransChannels) / sizeof(g_m576FlashBurnTransChannels[0]);

/// Suffix before ".bin" per 439F trans 1~4 (e.g. `x_mcs1.bin`), same order as `pm_*.csv` / `pd_*.csv` stems.
// 分 trans 备份/生成时文件名后缀，与四路 pm_/pd_ CSV 茎一一对应。
inline constexpr const TCHAR* const g_m576TransLutBinSuffix[4] = {
	_T("_mcs1"), _T("_mcs2"), _T("_1x64_1"), _T("_1x64_2")
};

/// Default path CSV per trans slot: 0=1#MCS, 1=2#MCS, 2=1#1x64, 3=2#1x64 (ASCII filenames).
// 默认定标路径 CSV 相对名：槽 0~3 对应 1#MCS、2#MCS、1#1x64、2#1x64。
inline constexpr const TCHAR* const g_m576DefaultPmCsvRel[4] = {
	_T("output\\pm_mcs1.csv"),
	_T("output\\pm_mcs2.csv"),
	_T("output\\pm_1x64_1.csv"),
	_T("output\\pm_1x64_2.csv"),
};
// PD 定标用默认 CSV 相对名（与上表四路 trans 一一对应）。
inline constexpr const TCHAR* const g_m576DefaultPdCsvRel[4] = {
	_T("output\\pd_mcs1.csv"),
	_T("output\\pd_mcs2.csv"),
	_T("output\\pd_1x64_1.csv"),
	_T("output\\pd_1x64_2.csv"),
};
#endif
