#pragma once
// 经 439F 的 trans/$$ 隧道在单 COM 上跑 Z4671 二进制：MCS LUT 分 trans 上载、分文件命名读回/备份等。

#include "Z4671Command.h"

/// Per-trans SN from devices: MCS trans1–2 (GetProductSN); 1x64 trans3–4 (four switches × SN per MEM read).
// UI / 写 bin 头用：MCS 各一路 SN；1x64 每片四开关 SN（ADDR_SWITCHn_COEF + 0x7E0）。
struct M576TransSnPnInfo
{
	CString mcsSn[2];
	CString oneX64Sn[2][4];
};

/// LUT BIN over 439F: ASCII "trans <n>" / "$$" then Z4671 binary on the same COM.
/// Channel tables: CalibConstants.h (g_m576FlashReadTransChannels, g_m576FlashBurnTransChannels).
// 先 ASCII 进 trans 通道、再 Z4671；读写通道表见 CalibConstants。

/// StartFWUpdate -> 400-byte chunks -> FWUpdateEnd (per trans target).
// 按 trans 上载 MCS 侧 LUT 固件包（400B 分块到结束包）。
BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err);
typedef void (__cdecl *McsFwProgressCb)(int current, int total, void* user); // 上载/读回进度，__cdecl
BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user);

/// From base "x.bin" -> "x_mcs1.bin" / "x_mcs2.bin" / "x_1x64_1.bin" + per-switch "x_1x64_1_swN.bin" (trans 1~4; see g_m576TransLutBinSuffix + M576TransBinPathForSwitch).
// 由用户选的“基名”生成分 trans 备份/输出文件名（后缀 g_m576TransLutBinSuffix）。
CString M576TransBackupPathFromBase(LPCTSTR szBasePath, int transChannel);
/// Load/burn: prefer that path; if missing, use legacy "x_tN.bin" when present.
// 读/烧时优先新命名，缺失则回退老命名 *_tN.bin（若存在）。
CString M576TransBinPathForRead(LPCTSTR szBasePath, int transChannel);

/// trans 1~2: same as M576TransBackupPathFromBase. trans 3~4: per-switch 2K file, e.g. `x_1x64_1_sw2.bin` (swIdx 0..3).
CString M576TransBinPathForSwitch(LPCTSTR szBasePath, int transChannel, int swIdx);

/// Read LUT from each trans channel; szOutPathBase is base (e.g. out\backup.bin -> out\backup_mcs1.bin …).
// 从各 trans 将设备 LUT 读回为多个分文件（如 out\foo.bin → out\foo_mcs1.bin …）。
/// `snInfo`：MCS 用 mcsSn[i] 写 LUT bundle SN；1x64 用 oneX64Sn[dev][sw] 写各 2K 头（dev 0=t3, 1=t4）。
BOOL McsReadLutBundleFromDevice(
	Z4671Command& cmd, LPCTSTR szOutPathBase, CString& err, McsFwProgressCb cb, void* user, const M576TransSnPnInfo& snInfo);

/// trans 1~2: GetProductSN (0xA2). trans 3~4: 4× `mem ADDR_SWITCH{1..4}_COEF+0x7E0` → 16 B SN ASCII each.
BOOL McsReadAllTransProductSn(Z4671Command& cmd, M576TransSnPnInfo& out, CString& err);
