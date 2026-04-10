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
	const size_t total = CLutBinWriter::FullBundleFileSize();
	if (total == 0)
	{
		err = _T("Invalid bundle size.");
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
		BYTE* pPayload = NULL;
		int nLen = 0;
		if (!cmd.GetLogFileData(typ, req, flashAddr, &pPayload, &nLen))
		{
			err = cmd.m_strLogInfo.IsEmpty() ? _T("GetLogFileData failed") : cmd.m_strLogInfo;
			return FALSE;
		}
		if (pPayload == NULL || nLen <= 0)
		{
			err = _T("Empty flash read payload.");
			return FALSE;
		}
		const int copyLen = (nLen < req) ? nLen : req;
		if ((size_t)copyLen > total - offset)
		{
			err = _T("Flash read length overflow.");
			return FALSE;
		}
		memcpy(&buf[offset], pPayload, (size_t)copyLen);
		offset += (size_t)copyLen;
		nChunkDone++;
		if (cb)
			cb(nChunkDone, nChunkTotal, user);
		if ((size_t)copyLen < (size_t)req)
		{
			err = _T("Short flash read; check type/base with firmware.");
			return FALSE;
		}
		Sleep(20);
	}

	HANDLE h = CreateFile(szOutPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Cannot create %s"), szOutPath);
		return FALSE;
	}
	DWORD nWritten = 0;
	const BOOL ok = WriteFile(h, buf.data(), (DWORD)total, &nWritten, NULL) && nWritten == (DWORD)total;
	CloseHandle(h);
	if (!ok)
	{
		err = _T("Write backup file failed.");
		return FALSE;
	}
	return TRUE;
}

BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user)
{
	err.Empty();
	if (!cmd.StartFWUpdate())
	{
		err = cmd.m_strLogInfo;
		return FALSE;
	}
	Sleep(100);

	HANDLE hBin = CreateFile(szBinPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hBin == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Open BIN failed: %s"), szBinPath);
		return FALSE;
	}
	DWORD dwCodeSizeLeft = GetFileSize(hBin, NULL);
	if (dwCodeSizeLeft == 0)
	{
		err = _T("BIN file is empty.");
		CloseHandle(hBin);
		return FALSE;
	}
	int iCnt = (int)(dwCodeSizeLeft / MAX_DATA_LENGTH);
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
			CloseHandle(hBin);
			return FALSE;
		}
		if (cb)
			cb(iCount, iCnt + 1, user);
		if (!cmd.FWTranSportFW((BYTE*)pbBinData, wDownloadSize, iCount, iCnt + 1))
		{
			err = cmd.m_strLogInfo;
			CloseHandle(hBin);
			return FALSE;
		}
	}
	CloseHandle(hBin);

	Sleep(1000);
	if (!cmd.FWUpdateEnd())
	{
		err = cmd.m_strLogInfo.IsEmpty() ? _T("FWUpdateEnd failed") : cmd.m_strLogInfo;
		return FALSE;
	}
	Sleep(5000);
	return TRUE;
}
