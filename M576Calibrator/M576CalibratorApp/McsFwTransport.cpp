#include "stdafx.h"
#include "McsFwTransport.h"

BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err)
{
	return McsFwUploadBinEx(cmd, szBinPath, err, NULL, NULL);
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
