#pragma once

#include "Z4671Command.h"

/// LUT BIN over 439F: ASCII "trans <n>" / "$$" then Z4671 binary on the same COM.
/// Channel tables: CalibConstants.h (g_m576FlashReadTransChannels, g_m576FlashBurnTransChannels).

/// StartFWUpdate -> 400-byte chunks -> FWUpdateEnd (per trans target).
BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err);
typedef void (__cdecl *McsFwProgressCb)(int current, int total, void* user);
BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user);

/// From base path "x.bin" builds "x_tN.bin" (multi-channel read output naming).
CString M576TransBackupPathFromBase(LPCTSTR szBasePath, int transChannel);

/// Read LUT from each trans channel; szOutPathBase is stem (e.g. out\backup.bin -> out\backup_t1.bin).
BOOL McsReadLutBundleFromDevice(Z4671Command& cmd, LPCTSTR szOutPathBase, CString& err, McsFwProgressCb cb, void* user);
