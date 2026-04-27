#pragma once
// 1# / 2# 1x64：走 MEM 协议读四段 2K 系数、XMODEM 烧回；与 MCS 0xC4 读 stLutSettingZ4671 是两条路径。

#include "McsFwTransport.h"

/// 1# 1x64 (trans 3) / 2# 1x64 (trans 4): MEM 回读 4×2K 写出四份 2K bin；与 MCS 0xC4 LUT 不同。
BOOL M576Read1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szOutPathBase, int transChannel, DWORD flashBase, CString& err, McsFwProgressCb cb,
	void* user, int progressBase, int progressTotal, const CString& strBundleSn);

/// 一次 fwdl + XMODEM 下发单个 2K MemsSw 文件。`bSendRset` 为 TRUE 时在本文件发送完成后 Sleep+RSET（四路中仅最后一次传 TRUE）。
BOOL M576Upload1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user, int progressBase, int progressTotal,
	BOOL bSendRset);

int M5761x64MemReadStepCount(); // MEM 分步读总步数
int M5761x64XmodemChunkCountForFileSize(DWORD fileBytes); // 给定文件长度需多少 XMODEM 块

/// Read ASCII SN: one `MEM` at `M576_1X64_SN_MEM_ADDR` (32 B), same line format as coef MEM dump (per site).
BOOL M576Read1x64SnStringOnCurrentTunnel(
	Z4671Command& cmd, DWORD memAddr, CString& outSn, CString& err);
