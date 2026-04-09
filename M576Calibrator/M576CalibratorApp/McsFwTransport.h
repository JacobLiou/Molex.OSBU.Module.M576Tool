#pragma once

#include "Z4671Command.h"

/// LUT BIN transport: sends Z4671 binary frames on the given link.
/// In single-COM topology, `cmd` is bound to the 429F serial port; firmware forwards to MCS.
/// If 429F later requires ASCII wrapping instead of transparent binary, replace this module
/// with an adapter implementing the same entry points (or introduce IMcsFwTunnel).

/// Mirrors legacy DataDownLoadZ4671: StartFWUpdate → 400-byte chunks → FWUpdateEnd.
BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err);
typedef void (__cdecl *McsFwProgressCb)(int current, int total, void* user);
BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user);
