#pragma once

#include "McsFwTransport.h"

/// 1# 1x64 (trans 3) / 2# 1x64 (trans 4): MEM dump 4×2K (`M576_1X64_MEMS_BACKUP_TOTAL_SIZE`) + XMODEM (not MCS 0xC4 LUT).
BOOL M576Read1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szOutPath, DWORD flashBase, CString& err, McsFwProgressCb cb, void* user,
	int progressBase, int progressTotal);

/// XMODEM (fwdl) burn for 1x64 MemsSw bin (not StartFWUpdate / FWTranSportFW).
BOOL M576Upload1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user, int progressBase, int progressTotal);

int M5761x64MemReadStepCount();
int M5761x64XmodemChunkCountForFileSize(DWORD fileBytes);
