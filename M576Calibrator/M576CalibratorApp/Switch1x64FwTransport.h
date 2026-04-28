#pragma once
// 1# / 2# 1x64：走 MEM 协议读四段 Flash body（每段 2048 B）、组 2208 B bin（合成 BUNDLEHEADER）、XMODEM 烧回；与 MCS 0xC4 读 stLutSettingZ4671 是两条路径。

#include "McsFwTransport.h"

/// 1# 1x64 (trans 3) / 2# 1x64 (trans 4): MEM 回读 4×2048 B body，写出四份 2208 B bin（160 B 头 + body）；与 MCS 0xC4 LUT 不同。
/// `swSn[0..3]`：各 bin bundle 头 SN（与 Flash 0xD800 区每开关 SN 对应）。
BOOL M576Read1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szOutPathBase, int transChannel, DWORD flashBase, CString& err, McsFwProgressCb cb,
	void* user, int progressBase, int progressTotal, const CString swSn[4]);

/// 一次 fwdl + XMODEM 下发单个 2208 B MemsSw 文件。`bSendRset` 为 TRUE 时在本文件发送完成后 Sleep+RSET（四路中仅最后一次传 TRUE）。
BOOL M576Upload1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user, int progressBase, int progressTotal,
	BOOL bSendRset);

int M5761x64MemReadStepCount(); // MEM 分步读总步数
int M5761x64XmodemChunkCountForFileSize(DWORD fileBytes); // 给定文件长度需多少 XMODEM 块

/// Read 4 switches: one `MEM` at `M576_1X64_SNPN_BASE_ADDR` (64 B), 16B per sw (8 SN + 8 PN ASCII).
BOOL M576Read1x64SnPnAllOnCurrentTunnel(
	Z4671Command& cmd, CString outSn[4], CString outPn[4], CString& err);
