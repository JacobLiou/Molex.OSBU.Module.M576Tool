#include "stdafx.h"
#include "McsFwTransport.h"
#include "LutBinWriter.h"
#include "CalibConstants.h"
#include <vector>

BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err)
{
	return McsFwUploadBinEx(cmd, szBinPath, err, NULL, NULL);
}

static int ChunkCountForRead(size_t total)
{
	return (int)((total + 254u) / 255u);
}

BOOL McsReadLutBundleFromDevice(Z4671Command& cmd, LPCTSTR szOutPath, CString& err, McsFwProgressCb cb, void* user)
{
	err.Empty();
	cmd.TraceInfo(_T("FW"), _T("Read LUT bundle start: output=%s"), szOutPath);
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
			cb(nChunkDone, nChunkTotal, user);
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

BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user)
{
	err.Empty();
	cmd.TraceInfo(_T("FW"), _T("Flash start: bin=%s"), szBinPath);
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
		return FALSE;
	}
	DWORD dwCodeSizeLeft = GetFileSize(hBin, NULL);
	if (dwCodeSizeLeft == 0)
	{
		err = _T("BIN file is empty.");
		cmd.TraceError(_T("FW"), _T("BIN file is empty: %s"), szBinPath);
		CloseHandle(hBin);
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
			return FALSE;
		}
		cmd.TraceInfo(_T("FW"), _T("Flash chunk %d/%d: size=%u remaining=%lu"), iCount, totalChunks, wDownloadSize, dwCodeSizeLeft);
		if (cb)
			cb(iCount, totalChunks, user);
		if (!cmd.FWTranSportFW((BYTE*)pbBinData, wDownloadSize, iCount, totalChunks))
		{
			err = cmd.m_strLogInfo;
			cmd.TraceError(_T("FW"), _T("FWTranSportFW failed: chunk=%d/%d size=%u err=%s"), iCount, totalChunks, wDownloadSize, err.GetString());
			CloseHandle(hBin);
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
	cmd.TraceInfo(_T("FW"), _T("Flash complete: bin=%s"), szBinPath);
	return TRUE;
}
