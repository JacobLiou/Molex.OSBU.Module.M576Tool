#pragma once

#include "Z4671Command.h"

/// Mirrors legacy DataDownLoadZ4671: StartFWUpdate → 400-byte chunks → FWUpdateEnd.
BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err);
typedef void (__cdecl *McsFwProgressCb)(int current, int total, void* user);
BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user);
