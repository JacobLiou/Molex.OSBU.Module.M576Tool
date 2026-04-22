#include "stdafx.h"
#include "McsFwTransport.h"
#include "Board439fTransTunnel.h"
#include "LutBinWriter.h"
#include "CalibConstants.h"
#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace {

static int ChunkCountForRead(size_t total)
{
	return (int)((total + 254u) / 255u);
}

// Z4671 flash read: caller already opened trans to target.
static BOOL ReadLutBundleOnCurrentTunnel(Z4671Command& cmd, LPCTSTR szOutPath, CString& err,
	McsFwProgressCb cb, void* user, int progressBase, int progressTotal)
{
	err.Empty();
	cmd.TraceInfo(_T("FW"), _T("Read LUT bundle (tunnel): output=%s"), szOutPath);
	const size_t total = CLutBinWriter::FullBundleFileSize();
	if (total == 0)
	{
		err = _T("Invalid bundle size.");
		cmd.TraceError(_T("FW"), _T("Read LUT bundle aborted: invalid bundle size."));
		return FALSE;
	}
	std::vector<BYTE> buf;
	buf.resize(total);
	size_t offset = 0;
	const int typ = M576_FLASH_FILE_TYPE;
	const int nChunkTotal = ChunkCountForRead(total);
	int nChunkDone = 0;

	while (offset < total)
	{
		const int req = (int)((total - offset) > 255 ? 255 : (total - offset));
		const DWORD flashAddr = M576_FLASH_LUT_READ_BASE + (DWORD)offset;
		cmd.TraceInfo(_T("FW"), _T("Read flash chunk %d/%d: addr=0x%08lX req=%d"), nChunkDone + 1, nChunkTotal, flashAddr, req);
		BYTE* pPayload = NULL;
		int nLen = 0;
		if (!cmd.GetLogFileData(typ, req, flashAddr, &pPayload, &nLen))
		{
			err = cmd.m_strLogInfo.IsEmpty() ? _T("GetLogFileData failed") : cmd.m_strLogInfo;
			cmd.TraceError(_T("FW"), _T("Read flash chunk failed at addr=0x%08lX req=%d: %s"), flashAddr, req, err.GetString());
			return FALSE;
		}
		if (pPayload == NULL || nLen <= 0)
		{
			err = _T("Empty flash read payload.");
			cmd.TraceError(_T("FW"), _T("Empty flash payload at addr=0x%08lX req=%d"), flashAddr, req);
			return FALSE;
		}
		const int copyLen = (nLen < req) ? nLen : req;
		if ((size_t)copyLen > total - offset)
		{
			err = _T("Flash read length overflow.");
			cmd.TraceError(_T("FW"), _T("Flash read overflow at addr=0x%08lX len=%d"), flashAddr, copyLen);
			return FALSE;
		}
		memcpy(&buf[offset], pPayload, (size_t)copyLen);
		offset += (size_t)copyLen;
		nChunkDone++;
		cmd.TraceInfo(_T("FW"), _T("Read flash chunk %d/%d complete: received=%d total=%Iu/%Iu"), nChunkDone, nChunkTotal, copyLen, offset, total);
		if (cb)
			cb(progressBase + nChunkDone, progressTotal, user);
		if ((size_t)copyLen < (size_t)req)
		{
			err = _T("Short flash read; check type/base with firmware.");
			cmd.TraceError(_T("FW"), _T("Short flash read at addr=0x%08lX req=%d got=%d"), flashAddr, req, copyLen);
			return FALSE;
		}
		Sleep(20);
	}

	HANDLE h = CreateFile(szOutPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Cannot create %s"), szOutPath);
		cmd.TraceError(_T("FW"), _T("Create backup file failed: %s"), err.GetString());
		return FALSE;
	}
	DWORD nWritten = 0;
	const BOOL ok = WriteFile(h, buf.data(), (DWORD)total, &nWritten, NULL) && nWritten == (DWORD)total;
	CloseHandle(h);
	if (!ok)
	{
		err = _T("Write backup file failed.");
		cmd.TraceError(_T("FW"), _T("Write backup file failed: %s"), szOutPath);
		return FALSE;
	}
	cmd.TraceInfo(_T("FW"), _T("Read LUT bundle complete: bytes=%Iu path=%s"), total, szOutPath);
	return TRUE;
}

// Z4671 burn: caller already opened trans to target.
static BOOL UploadBinOnCurrentTunnel(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user,
	int progressBase, int progressTotal)
{
	err.Empty();
	cmd.TraceInfo(_T("FW"), _T("Flash start (tunnel): bin=%s"), szBinPath);
	if (!cmd.StartFWUpdate())
	{
		err = cmd.m_strLogInfo;
		cmd.TraceError(_T("FW"), _T("StartFWUpdate failed: %s"), err.GetString());
		return FALSE;
	}
	cmd.TraceInfo(_T("FW"), _T("StartFWUpdate acknowledged."));
	Sleep(100);

	HANDLE hBin = CreateFile(szBinPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hBin == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Open BIN failed: %s"), szBinPath);
		cmd.TraceError(_T("FW"), _T("Open BIN failed: %s"), szBinPath);
		(void)cmd.FWUpdateEnd();
		return FALSE;
	}
	DWORD dwCodeSizeLeft = GetFileSize(hBin, NULL);
	if (dwCodeSizeLeft == 0)
	{
		err = _T("BIN file is empty.");
		cmd.TraceError(_T("FW"), _T("BIN file is empty: %s"), szBinPath);
		CloseHandle(hBin);
		(void)cmd.FWUpdateEnd();
		return FALSE;
	}
	int iCnt = (int)(dwCodeSizeLeft / MAX_DATA_LENGTH);
	const int totalChunks = iCnt + 1;
	cmd.TraceInfo(_T("FW"), _T("Flash bin opened: bytes=%lu chunks=%d"), dwCodeSizeLeft, totalChunks);
	char pbBinData[MAX_DATA_LENGTH];
	DWORD dwBytesRead = 0;
	int iCount = 0;

	while (dwCodeSizeLeft)
	{
		iCount++;
		WORD wDownloadSize = MAX_DATA_LENGTH;
		if (iCount > iCnt)
			wDownloadSize = (WORD)(dwCodeSizeLeft % MAX_DATA_LENGTH);
		dwCodeSizeLeft -= wDownloadSize;

		ZeroMemory(pbBinData, sizeof(pbBinData));
		if (!ReadFile(hBin, pbBinData, wDownloadSize, &dwBytesRead, NULL) || dwBytesRead != wDownloadSize)
		{
			err = _T("Read BIN chunk failed.");
			cmd.TraceError(_T("FW"), _T("Read BIN chunk failed: chunk=%d/%d size=%u"), iCount, totalChunks, wDownloadSize);
			CloseHandle(hBin);
			(void)cmd.FWUpdateEnd();
			return FALSE;
		}
		cmd.TraceInfo(_T("FW"), _T("Flash chunk %d/%d: size=%u remaining=%lu"), iCount, totalChunks, wDownloadSize, dwCodeSizeLeft);
		if (cb)
			cb(progressBase + iCount, progressTotal, user);
		if (!cmd.FWTranSportFW((BYTE*)pbBinData, wDownloadSize, iCount, totalChunks))
		{
			err = cmd.m_strLogInfo;
			cmd.TraceError(_T("FW"), _T("FWTranSportFW failed: chunk=%d/%d size=%u err=%s"), iCount, totalChunks, wDownloadSize, err.GetString());
			CloseHandle(hBin);
			(void)cmd.FWUpdateEnd();
			return FALSE;
		}
		cmd.TraceInfo(_T("FW"), _T("Flash chunk %d/%d acknowledged."), iCount, totalChunks);
	}
	CloseHandle(hBin);

	Sleep(1000);
	if (!cmd.FWUpdateEnd())
	{
		err = cmd.m_strLogInfo.IsEmpty() ? _T("FWUpdateEnd failed") : cmd.m_strLogInfo;
		cmd.TraceError(_T("FW"), _T("FWUpdateEnd failed: %s"), err.GetString());
		return FALSE;
	}
	cmd.TraceInfo(_T("FW"), _T("FWUpdateEnd acknowledged. Waiting for firmware settle."));
	Sleep(5000);
	cmd.TraceInfo(_T("FW"), _T("Flash complete (tunnel): bin=%s"), szBinPath);
	return TRUE;
}

} // namespace

CString M576TransBackupPathFromBase(LPCTSTR szBasePath, int transChannel)
{
	CString base(szBasePath);
	base.Trim();
	CString suffix;
	suffix.Format(_T("_t%d"), transChannel);
	const int dot = base.ReverseFind(_T('.'));
	if (dot < 0)
		return base + suffix + _T(".bin");
	return base.Left(dot) + suffix + base.Mid(dot);
}

BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err)
{
	return McsFwUploadBinEx(cmd, szBinPath, err, NULL, NULL);
}

BOOL McsReadLutBundleFromDevice(Z4671Command& cmd, LPCTSTR szOutPathBase, CString& err, McsFwProgressCb cb, void* user)
{
	err.Empty();
	if (g_m576FlashReadTransChannelCount == 0)
	{
		err = _T("No read trans channels configured.");
		return FALSE;
	}
	CString discard;
	(void)Board439fTransTunnel::EndTrans(cmd, discard);

	const size_t bundleBytes = CLutBinWriter::FullBundleFileSize();
	if (bundleBytes == 0)
	{
		err = _T("Invalid bundle size.");
		return FALSE;
	}
	const int chunksPerBundle = ChunkCountForRead(bundleBytes);
	const int progressTotal = (int)(g_m576FlashReadTransChannelCount * (size_t)chunksPerBundle);

	for (std::size_t i = 0; i < g_m576FlashReadTransChannelCount; ++i)
	{
		const int ch = g_m576FlashReadTransChannels[i];
		CString path = M576TransBackupPathFromBase(szOutPathBase, ch);
		cmd.TraceInfo(_T("FW"), _T("Read LUT multi-channel: trans=%d file=%s"), ch, path.GetString());

		(void)Board439fTransTunnel::EndTrans(cmd, discard);
		if (!Board439fTransTunnel::BeginTrans(cmd, ch, err))
		{
			err.Format(_T("trans %d: %s"), ch, err.GetString());
			(void)Board439fTransTunnel::EndTrans(cmd, discard);
			return FALSE;
		}
		const int progressBase = (int)(i * (size_t)chunksPerBundle);
		if (!ReadLutBundleOnCurrentTunnel(cmd, path, err, cb, user, progressBase, progressTotal))
		{
			err.Format(_T("trans %d: %s"), ch, err.GetString());
			(void)Board439fTransTunnel::EndTrans(cmd, discard);
			return FALSE;
		}
		if (!Board439fTransTunnel::EndTrans(cmd, err))
		{
			err.Format(_T("trans %d end $$: %s"), ch, err.GetString());
			return FALSE;
		}
	}
	cmd.TraceInfo(_T("FW"), _T("All read-backup channels done (base path=%s)."), szOutPathBase);
	return TRUE;
}

BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user)
{
	err.Empty();
	if (g_m576FlashBurnTransChannelCount == 0)
	{
		err = _T("No burn trans channels configured.");
		return FALSE;
	}
	CString discard;
	(void)Board439fTransTunnel::EndTrans(cmd, discard);

	HANDLE hProbe = CreateFile(szBinPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hProbe == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Open BIN failed: %s"), szBinPath);
		return FALSE;
	}
	const DWORD binSize = GetFileSize(hProbe, NULL);
	CloseHandle(hProbe);
	if (binSize == 0)
	{
		err = _T("BIN file is empty.");
		return FALSE;
	}
	const int iCnt = (int)(binSize / MAX_DATA_LENGTH);
	const int chunksThisFile = iCnt + 1;
	const int progressTotal = (int)(g_m576FlashBurnTransChannelCount * (size_t)chunksThisFile);

	for (std::size_t i = 0; i < g_m576FlashBurnTransChannelCount; ++i)
	{
		const int ch = g_m576FlashBurnTransChannels[i];
		cmd.TraceInfo(_T("FW"), _T("Burn: trans=%d bin=%s"), ch, szBinPath);

		(void)Board439fTransTunnel::EndTrans(cmd, discard);
		if (!Board439fTransTunnel::BeginTrans(cmd, ch, err))
		{
			err.Format(_T("trans %d: %s"), ch, err.GetString());
			(void)Board439fTransTunnel::EndTrans(cmd, discard);
			return FALSE;
		}
		const int progressBase = (int)(i * (size_t)chunksThisFile);
		if (!UploadBinOnCurrentTunnel(cmd, szBinPath, err, cb, user, progressBase, progressTotal))
		{
			err.Format(_T("trans %d: %s"), ch, err.GetString());
			(void)Board439fTransTunnel::EndTrans(cmd, discard);
			return FALSE;
		}
		if (!Board439fTransTunnel::EndTrans(cmd, err))
		{
			err.Format(_T("trans %d end $$: %s"), ch, err.GetString());
			return FALSE;
		}
	}
	cmd.TraceInfo(_T("FW"), _T("Burn finished all configured trans channels."));
	return TRUE;
}
