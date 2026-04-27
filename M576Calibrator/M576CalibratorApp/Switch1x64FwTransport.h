#pragma once
// 1# / 2# 1x64：走 MEM 协议读四段 2K 系数、XMODEM 烧回；与 MCS 0xC4 读 stLutSettingZ4671 是两条路径。

#include "McsFwTransport.h"

/// 1# 1x64 (trans 3) / 2# 1x64 (trans 4): MEM dump 4×2K (`M576_1X64_MEMS_BACKUP_TOTAL_SIZE`) + XMODEM (not MCS 0xC4 LUT).
// 当前 trans 隧道上从指定 Flash 基址 MEM 回读 8KB Mems 系数到文件（进度区间由调用方传入）。
BOOL M576Read1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szOutPath, DWORD flashBase, CString& err, McsFwProgressCb cb, void* user,
	int progressBase, int progressTotal, const CString& strBundleSn);

/// XMODEM (fwdl) burn for 1x64 MemsSw bin (not StartFWUpdate / FWTranSportFW).
// fwdl 后 XMODEM 下发 1x64 系数库（非 MCS 上载流程）。
BOOL M576Upload1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user, int progressBase, int progressTotal);

int M5761x64MemReadStepCount(); // MEM 分步读总步数
int M5761x64XmodemChunkCountForFileSize(DWORD fileBytes); // 给定文件长度需多少 XMODEM 块

/// Read ASCII SN: one `MEM` at `M576_1X64_SN_MEM_ADDR` (32 B), same line format as coef MEM dump (per site).
// 在已 `BeginTrans` 的 1x64 隧道上：单地址 MEM 回 32B，按可打印串解析（与 MCS 0xA2 不同）。
BOOL M576Read1x64SnStringOnCurrentTunnel(
	Z4671Command& cmd, DWORD memAddr, CString& outSn, CString& err);
