#pragma once
// 439F main board direct burn: fwdl\r + XMODEM + RSET, no trans/$$ (independent from sub-board paths).

#include "McsFwTransport.h"

/// 439F main board firmware: ASCII `fwdl\r` then XMODEM-1K on the same COM (no passthrough).
BOOL M576BurnBoard439fFirmware(
	Z4671Command& cmd,
	LPCTSTR szBinPath,
	CString& err,
	McsFwProgressCb cb,
	void* user);

int M576Board439fXmodemChunkCountForFileSize(DWORD fileBytes);
