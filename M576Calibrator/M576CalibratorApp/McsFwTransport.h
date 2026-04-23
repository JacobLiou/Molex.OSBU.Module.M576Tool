#pragma once

#include "Z4671Command.h"

/// LUT BIN over 439F: ASCII "trans <n>" / "$$" then Z4671 binary on the same COM.
/// Channel tables: CalibConstants.h (g_m576FlashReadTransChannels, g_m576FlashBurnTransChannels).

/// StartFWUpdate -> 400-byte chunks -> FWUpdateEnd (per trans target).
BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err);
typedef void (__cdecl *McsFwProgressCb)(int current, int total, void* user);
BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user);

/// From base "x.bin" -> "x_mcs1.bin" / "x_mcs2.bin" / "x_1x64_1.bin" / "x_1x64_2.bin" (trans 1~4; see g_m576TransLutBinSuffix).
CString M576TransBackupPathFromBase(LPCTSTR szBasePath, int transChannel);
/// Load/burn: prefer that path; if missing, use legacy "x_tN.bin" when present.
CString M576TransBinPathForRead(LPCTSTR szBasePath, int transChannel);

/// Read LUT from each trans channel; szOutPathBase is base (e.g. out\backup.bin -> out\backup_mcs1.bin …).
BOOL McsReadLutBundleFromDevice(Z4671Command& cmd, LPCTSTR szOutPathBase, CString& err, McsFwProgressCb cb, void* user);
