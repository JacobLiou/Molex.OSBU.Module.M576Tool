// PDBoxForMCS.cpp: implementation of the CPDBoxForMCS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PDBoxForMCS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPDBoxForMCS::CPDBoxForMCS()
{

}

CPDBoxForMCS::~CPDBoxForMCS()
{

}

BOOL CPDBoxForMCS::GetAllPDADC(int *pnADC)
{
	CString strCommand;
	char    chCommand[10];
	char    chRead[100];
	BYTE    bytData[40];
    DWORD   dwLength;
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));
	strCommand = "GetADC\r";
	memcpy(chCommand,strCommand,strCommand.GetLength());

	if (!WriteBuffer(chCommand,strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕGETADC√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(chRead,100,&dwLength))
	{
		m_strErrorLog = ("Ω” ’GETADC√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	
	int n = 0;
	int i = 0;
	for ( i=0;i<dwLength;i++)
	{
		bytData[i] = chRead[i];
		if (i%2==1)
		{
			pnADC[n++] = bytData[i-1]*256+bytData[i]; 
		}
	}
	return TRUE;	
}
bool CPDBoxForMCS::NewGetMaxPdCout(int chan, int npoint, double checkdb, WORD *maxindex)
{
	CString strCommand;
	char    chCommand[50];
	char    chRead[50];
	int cout, allcout;
	DWORD dwCRCValue = 0;
	ZeroMemory(chCommand, sizeof(char) * 50);
	ZeroMemory(chRead, sizeof(char) * 50);
	strCommand.Format("scan %d maxidx %d %.2f\r", chan + 1, npoint, checkdb);
	memcpy(chCommand, strCommand, strCommand.GetLength());

	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕCHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);

	if (!ReadBuffer(chRead, 50))
	{
		m_strErrorLog = ("Ω” ’CHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	m_strErrorLog.Format("%s", &chRead);
	if (m_strErrorLog.Find("CenterIndex:", 0) == -1)
	{
		return FALSE;
	}
	else
	{
		allcout = m_strErrorLog.GetLength();
		cout = m_strErrorLog.Find("CenterIndex:");

		m_strErrorLog = m_strErrorLog.Right(allcout - cout - 12);

		*maxindex = atoi(m_strErrorLog);
	}

	return TRUE;
}
BOOL CPDBoxForMCS::NewGetMaxPdCout(int chan, int npoint, int *maxid, double *maxpow, double checkdb)
{
	int errcout;
	errcout = 0;
GETMAX:	CString strCommand;
	char    chCommand[50];
	char    chRead[2048];
	int cout, allcout;
	DWORD dwCRCValue = 0;
	ZeroMemory(chCommand, sizeof(char) * 50);
	ZeroMemory(chRead, sizeof(char) * 2048);
	//strCommand.Format("scan %d maxidx %d %.2f\r", chan + 1, npoint, checkdb);
	strCommand.Format("scan 0 adcmax\r");
	memcpy(chCommand, strCommand, strCommand.GetLength());

	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕCHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(550);

	if (!ReadBuffer(chRead, 2048))
	{
		m_strErrorLog = ("Ω” ’CHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	m_strErrorLog.Format("%s", &chRead);


	//strErrorLog = "ch 0 MaxPwr: -65.89, IDX:252ch 1 MaxPwr: -66.18, IDX:768ch 2 MaxPwr: -64.73, IDX:425ch 3 MaxPwr: -66.07, IDX:1012ch 4 MaxPwr: -65.84, IDX:23ch 5 MaxPwr: -66.26, IDX:113ch 6 MaxPwr: -66.37, IDX:547ch 7 MaxPwr: -66.22, IDX:65ch 8 MaxPwr: -65.97, IDX:23ch 9 MaxPwr: -66.38, IDX:32ch 10 MaxPwr: -66.55, IDX:27ch 11 MaxPwr: -65.93, IDX:46ch 12 MaxPwr: -66.10, IDX:196ch 13 MaxPwr: -65.98, IDX:164ch 14 MaxPwr: -65.89, IDX:9ch 15 MaxPwr";
	int start = 0;
	int end = m_strErrorLog.Find(_T("\r\n"), start);
	int ncc = 0;
	while (end != -1) {
		CString line = m_strErrorLog.Mid(start, end - start);
		int maxPwrPos = line.Find(_T("MaxPwr:"));
		int idxPos = line.Find(_T("IDX:"));

		if (maxPwrPos != -1 && idxPos != -1) {
			CString maxPwrStr = line.Mid(maxPwrPos + 7, idxPos - maxPwrPos - 8);
			CString idxStr = line.Mid(idxPos + 4);

			double maxPwr;
			int idx;

			_stscanf_s(maxPwrStr, _T("%lf"), &maxPwr);
			_stscanf_s(idxStr, _T("%d"), &idx);

			// ¥Ê¥¢÷µµΩ∂‘”¶µƒ ˝◊È
			maxpow[ncc] = maxPwr;
			maxid[ncc] = idx;
			ncc++;
		}

		start = end + 2;
		end = m_strErrorLog.Find(_T("\r\n"), start);
	}

	for (int xa = 0; xa < 64; xa++)
	{
		if (maxid[xa] > 1024 || maxid[xa] == 0)
		{
			if (errcout < 5)
			{
				errcout = errcout + 1;
				goto GETMAX;
			}
			else
			{
				return FALSE;
			}

		}
	}

	return TRUE;
}
BOOL CPDBoxForMCS::NewGetMaxADC(int chan, int npoint, double checkdb, WORD *maxindex)
{
	CString strCommand;
	char    chCommand[50];
	char    chRead[50];
	int cout, allcout;
	DWORD dwCRCValue = 0;
	ZeroMemory(chCommand, sizeof(char) * 50);
	ZeroMemory(chRead, sizeof(char) * 50);
	strCommand.Format("scan %d adcmax %d\r", chan + 1, npoint);
	memcpy(chCommand, strCommand, strCommand.GetLength());

	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕCHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);

	if (!ReadBuffer(chRead, 50))
	{
		m_strErrorLog = ("Ω” ’CHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	m_strErrorLog.Format("%s", &chRead);
	if (m_strErrorLog.Find("CenterIndex:", 0) == -1)
	{
		return FALSE;
	}
	else
	{
		allcout = m_strErrorLog.GetLength();
		cout = m_strErrorLog.Find("CenterIndex:");

		m_strErrorLog = m_strErrorLog.Right(allcout - cout - 12);

		*maxindex = atoi(m_strErrorLog);
	}


	return TRUE;
}
BOOL CPDBoxForMCS::NewGetPDTriCout(int chan)
{
	CString strCommand;
	char    chCommand[50];
	char    chRead[50];
	DWORD dwCRCValue = 0;
	ZeroMemory(chCommand, sizeof(char) * 50);
	ZeroMemory(chRead, sizeof(char) * 50);
	strCommand.Format("scan 1 trigcnt\r");
	memcpy(chCommand, strCommand, strCommand.GetLength());

	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕCHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);

	if (!ReadBuffer(chRead, 50))
	{
		m_strErrorLog = ("Ω” ’CHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}

	return TRUE;
}
BOOL CPDBoxForMCS::NewSendPDMon(int chan, int npoint)
{
	CString strCommand;
	char    chCommand[50];
	char    chRead[50];
	DWORD dwCRCValue = 0;
	ZeroMemory(chCommand, sizeof(char) * 50);
	ZeroMemory(chRead, sizeof(char) * 50);
	//strCommand.Format("scan %d adcmax %d\r", chan + 1, npoint);//¥”1ø™ º
	strCommand.Format("scan 0 adcmax 1024\r");
	memcpy(chCommand, strCommand, strCommand.GetLength());

	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕCHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);

	if (!ReadBuffer(chRead, 20))
	{
		m_strErrorLog.Format("%s", &chRead);
		if (m_strErrorLog.Find("OK", 0) == -1)
		{
			return FALSE;
		}
	}

	return TRUE;

}
BOOL CPDBoxForMCS::GetPDPower(double *dbreadpow)
{
	CString strCommand, strValue, strGetValue;
	char    chCommand[50];
	char    chRead[2048];
	DWORD dwCRCValue = 0;
	double db_data;
	int cout, allcout;
	ZeroMemory(chCommand, sizeof(char) * 50);
	ZeroMemory(chRead, sizeof(char) * 2048);
	strCommand.Format("pd 0 pwr\r");
	memcpy(chCommand, strCommand, strCommand.GetLength());

	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕGet Power√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(100);

	if (!ReadBuffer(chRead, 2048))
	{
		m_strErrorLog.Format("%s", &chRead);
		if (m_strErrorLog.Find("OK", 0) == -1)
		{
			return FALSE;
		}
	}

	m_strErrorLog.Format("%s", &chRead);
	for (int i = 0; i < 64; i++)
	{
		strValue.Empty();
		strGetValue.Empty();
		AfxExtractSubString(strGetValue, m_strErrorLog, i + 1, ':');
		AfxExtractSubString(strValue, strGetValue, 1, ' ');
		db_data = atof(strValue);
		dbreadpow[i] = db_data;
	}

	return TRUE;

}
BOOL CPDBoxForMCS::ScanTest(int nMinADC, int nMaxADC)
{
	CString strCommand;
	char    chCommand[20];
	char    chRead[100];
    DWORD   dwLength;
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));
	strCommand.Format("scantest %d %d\r",nMinADC,nMaxADC);
	memcpy(chCommand,strCommand,strCommand.GetLength());

	if (!WriteBuffer(chCommand,strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕscantest√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(chRead,100,&dwLength))
	{
		m_strErrorLog = ("Ω” ’scantest√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	m_strErrorLog.Format("%s",&chRead);
	
	return TRUE;

}

BOOL CPDBoxForMCS::GetLengthAndCRC(DWORD *dwLength, DWORD *dwCRC)
{
	CString strCommand;
	char    chCommand[20];
	char    chRead[20];
	DWORD   dwLen = 0;
	ZeroMemory(chCommand,sizeof(char)*20);
	ZeroMemory(chRead,sizeof(char)*20);
	strCommand = "SCIDATA\r";
	memcpy(chCommand,strCommand,strCommand.GetLength());
	
	if (!WriteBuffer(chCommand,strCommand.GetLength())) 
	{
		m_strErrorLog = ("∑¢ÀÕscidata√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(chRead,20,&dwLen))
	{
		m_strErrorLog = ("ªÒ»°ºÏ—È¬Î∫Õ ˝æ›≥§∂»–≈œ¢ ß∞‹£°");
		return FALSE;
	}
	
	BYTE bytDataSize[8];
	int i = 0;
	for ( i=0;i<8;i++)
	{
		bytDataSize[i] = chRead[i];
	}
	
	DWORD dwDataLength;
	dwDataLength =  (bytDataSize[0]<<24);
	dwDataLength += (bytDataSize[1]<<16);
	dwDataLength += (bytDataSize[2]<<8);
	dwDataLength += bytDataSize[3];
	*dwLength = dwDataLength;
	
	DWORD dwGetCRC;
	dwGetCRC =  (bytDataSize[4]<<24);
	dwGetCRC += (bytDataSize[5]<<16);
	dwGetCRC += (bytDataSize[6]<<8);
	dwGetCRC += bytDataSize[7];
	*dwCRC = dwGetCRC;
	return TRUE;

}

BOOL CPDBoxForMCS::StopScan()
{
	CString strCommand;
	char    chCommand[20];
	char    chRead[20];
	DWORD dwCRCValue = 0;
	DWORD dwLength;
	ZeroMemory(chCommand,sizeof(char)*20);
	ZeroMemory(chRead,sizeof(char)*20);
	strCommand = "STOP\r";
	memcpy(chCommand,strCommand,strCommand.GetLength());
	
	if (!WriteBuffer(chCommand,strCommand.GetLength())) 
	{
		m_strErrorLog = ("∑¢ÀÕSTOP√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	if (!ReadBuffer(chRead, 20, &dwLength))
	{
		m_strErrorLog = ("Ω” ’STOP√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	m_strErrorLog.Format("%s", &chRead);
	if (m_strErrorLog.Find("Stop", 0) == -1)
	{
		return FALSE;
	}
	
	return TRUE;

}
BOOL CPDBoxForMCS::GetScanData()
{
	CString strCommand;
	char    chCommand[20];
	char    chRead[10240];
	DWORD dwCRCValue = 0;
	DWORD dwLength;
	ZeroMemory(chCommand, sizeof(char) * 20);
	ZeroMemory(chRead, sizeof(char) * 10240);
	strCommand = "SCAN 1 1KDATA\r";
	memcpy(chCommand, strCommand, strCommand.GetLength());

	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕGETDATA√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	if (!ReadBuffer(chRead, 10240, &dwLength))
	{
		m_strErrorLog = ("Ω” ’GETDATA√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	//m_strErrorLog.Format("%s", &chRead);
	
	//CString cstr;
	m_strErrorLog = "";
	int zeroCount = 0;
	CString hex;
	for (int i = 0; i < sizeof(chRead); i++)
	{
		if (chRead[i] == 0x00)
		{
			zeroCount++;
			if (zeroCount == 2)
			{
				break;
			}
		}
		else
		{
			zeroCount = 0;
		}
		hex.Format(_T("%02X"), (unsigned char)chRead[i]);
		m_strErrorLog += hex;
	}

	return TRUE;

}

BOOL CPDBoxForMCS::StartMonitor(int nPort)
{
	//CHMon
	CString strCommand;
	char    chCommand[20];
	char    chRead[20];
	DWORD dwCRCValue = 0;
	ZeroMemory(chCommand,sizeof(char)*20);
	ZeroMemory(chRead,sizeof(char)*20);
	strCommand.Format("CHMon %d\r",nPort);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	
	if (!WriteBuffer(chCommand,strCommand.GetLength())) 
	{
		m_strErrorLog = ("∑¢ÀÕCHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);
	/*
	if (!ReadBuffer(chRead,20))
	{
		m_strErrorLog = ("Ω” ’CHMon√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	*/
	return TRUE;

}

BOOL CPDBoxForMCS::HitlessTest(int nMinADC, int nSWTime)
{
	CString strCommand;
	char    chCommand[100];
	char    chRead[100];
    DWORD   dwLength;
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));
	strCommand.Format("hitlesstest %d %d\r",nMinADC,nSWTime);
	if (nSWTime == 0)
	{
		strCommand.Format("hitlesstest %d\r", nMinADC);
	}
	memcpy(chCommand,strCommand,strCommand.GetLength());

	if (!WriteBuffer(chCommand,strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕhitlesstest√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(chRead,100,&dwLength))
	{
		m_strErrorLog = ("Ω” ’hitlesstest√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	m_strErrorLog.Format("%s",&chRead);
	if (m_strErrorLog.Find("OK", 0) == -1)
	{
		return FALSE;
	}
	
	return TRUE;

}

bool CPDBoxForMCS::GetHitless(WORD* wValue)
{
	CString strCommand;
	char    chCommand[100];
	char    chRead[10];
	DWORD   dwLength;
	WORD    wRead;

	ZeroMemory(chCommand, sizeof(chCommand));
	ZeroMemory(chRead, sizeof(chRead));
	strCommand.Format("gethitless\r");
	memcpy(chCommand, strCommand, strCommand.GetLength());
	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕgethitless√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(chRead, 10, &dwLength))
	{
		m_strErrorLog = ("Ω” ’gethitless√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	if (dwLength == 0)
	{
		return FALSE;
	}
	wRead = *((unsigned char*)&chRead[0]);
	wRead *= 256;
	wRead += *((unsigned char*)&chRead[1]);
	*wValue = wRead;
	return TRUE;
}
BOOL CPDBoxForMCS::GetSiglePower(int nSigle, double *dbreadpow)
{
	CString strCommand, strValue, strGetValue;
	char    chCommand[50];
	char    chRead[2048];
	DWORD dwCRCValue = 0;
	double db_data;
	int cout, allcout;
	ZeroMemory(chCommand, sizeof(char) * 50);
	ZeroMemory(chRead, sizeof(char) * 2048);
	strCommand.Format("pd %d pwr\r", nSigle);
	if (nSigle < 0 || nSigle > 64)
	{
		m_strErrorLog = ("Channel Out of range£°");
		return FALSE;
	}
	memcpy(chCommand, strCommand, strCommand.GetLength());

	if (!WriteBuffer(chCommand, strCommand.GetLength()))
	{
		m_strErrorLog = ("∑¢ÀÕGet Power√¸¡Ó ß∞‹£°");
		return FALSE;
	}
	Sleep(100);

	if (!ReadBuffer(chRead, 2048))
	{
		m_strErrorLog.Format("%s", &chRead);
		if (m_strErrorLog.Find("OK", 0) == -1)
		{
			return FALSE;
		}
	}

	m_strErrorLog.Format("%s", &chRead);
	for (int i = 0; i < 1; i++)
	{
		strValue.Empty();
		strGetValue.Empty();
		AfxExtractSubString(strGetValue, m_strErrorLog, i + 1, ':');
		AfxExtractSubString(strValue, strGetValue, 1, ' ');
		db_data = atof(strValue);
		dbreadpow[i] = db_data;
	}

	return TRUE;

}