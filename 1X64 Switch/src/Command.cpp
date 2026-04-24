// Command.cpp: implementation of the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "126S_45V_Switch_App.h"
#include "Command.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommand::CCommand()
{
	m_I2CAddress = "C0";
}

CCommand::~CCommand()
{

}

BOOL CCommand::SetCommand(WORD iCommandAddr, CString strTxData)
{
	TCHAR	pchRead[120] = {0}, pchWrite[120] = {0};
	CString strRead;
	CString strTemp;

	ZeroMemory(pchWrite, 120);
	sprintf(pchWrite, "seti2c %s %02x %s\r", m_I2CAddress, iCommandAddr, strTxData);

	strRead = pchWrite;

	if (!m_Com.WriteBuffer(pchWrite, strRead.GetLength())) 
	{
		strTemp.Format("发送命令到0x%x失败!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}	
	BYTE bIndex = 0;
	for(; bIndex < 200; bIndex ++)
	{
		ZeroMemory(pchRead, 120);
		
		if (!m_Com.ReadBuffer(pchRead, 120)) 
		{
			strTemp.Format("0x%x返回数据失败!", iCommandAddr);
			AfxMessageBox(strTemp, MB_ICONERROR);
			return FALSE;
		}

		strRead = pchRead;
		if ("OK" != strRead.Mid(0, 2))
		{
			continue;
		}
		else
		{
			break;
		}
	}
	if (bIndex > 199) 
	{
		strTemp.Format("发送命令到0x%x无效!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}
	
	return TRUE;
}


BOOL CCommand::UpgradeCommand(WORD iCommandAddr, BYTE byTxData)
{
	TCHAR	pchWrite[120];
	CString strTemp;

	ZeroMemory(pchWrite, 120);
	sprintf(pchWrite, "seti2c %s %02x %02x\r", m_I2CAddress, iCommandAddr, byTxData);

	if (!m_Com.WriteBuffer(pchWrite, 16)) 
	{
		strTemp.Format("发送命令到0x%x失败!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}	
	
	return TRUE;
}
BOOL CCommand::ByteSetCommand(WORD iCommandAddr, CString strTxData)
{
	TCHAR	pchRead[120] = {0}, pchWrite[120] = {0};
	CString strRead;
	CString strTemp;

	ZeroMemory(pchWrite, 120);
	sprintf(pchWrite, "SBI2C %s %02x %s\r", m_I2CAddress, iCommandAddr, strTxData);
// 	sprintf(pchWrite, "seti2c c0 %02x %s\r", iCommandAddr, strTxData);

	strRead = pchWrite;

	if (!m_Com.WriteBuffer(pchWrite, strRead.GetLength())) 
	{
		strTemp.Format("发送命令到0x%x失败!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}	
	BYTE bIndex = 0;
	for(; bIndex < 200; bIndex ++)
	{
		Sleep(1);
		ZeroMemory(pchRead, 120);
		
		if (!m_Com.ReadBuffer(pchRead, 120)) 
		{
			strTemp.Format("0x%x返回数据失败!", iCommandAddr);
			AfxMessageBox(strTemp, MB_ICONERROR);
			return FALSE;
		}

		strRead = pchRead;
		if ("OK" != strRead.Mid(0, 2))
		{
			continue;
		}
		else
		{
			break;
		}
	}
	if (bIndex > 199) 
	{
		strTemp.Format("发送命令到0x%x无效!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}
	
	return TRUE;
}

double CCommand::ReadPower(WORD iCommandAddr)
{
	CString strTxData;
	char chTemp[64];
	SHORT nPower = 0;
	double dblPower = 0;

	if(!GetCommand(iCommandAddr, 2, &strTxData))
		return 0;

	ZeroMemory(chTemp, 64);
	sprintf(chTemp, "%s", strTxData);
	sscanf(chTemp, "%x", &nPower);
	
	dblPower = nPower*0.01;

	return dblPower;
}

WORD CCommand::GetADC(WORD iCommandAddr)
{
	CString strTxData;
	char chTemp[64];
	WORD wADC = 0;

	if(!GetCommand(iCommandAddr, 2, &strTxData))
		return 0;

	ZeroMemory(chTemp, 64);
	sprintf(chTemp, "%s", strTxData);
	sscanf(chTemp, "%x", &wADC);

	return wADC;
}


WORD CCommand::GenerateCheckSum(BYTE pbyMsg[32])
{
	WORD wChksum  = 0x00;
	int   i = 0;
	for( i=0; i < 32; i++)
	{
		wChksum += pbyMsg[i];
	}

	return ~wChksum ;
}


BOOL CCommand::GetCommand(WORD iCommandAddr, WORD wRxDataLen, CString *strTxData)
{
	TCHAR	pchRead[120] = {0}, pchWrite[120] = {0};
	CString strRead;
	CString strTemp;
	
	ZeroMemory(pchWrite, 120);
	sprintf(pchWrite, "seti2c %s %02x\r", m_I2CAddress, iCommandAddr);

	strRead = pchWrite;

	if (!m_Com.WriteBuffer(pchWrite, strRead.GetLength())) 
	{
		strTemp.Format("发送命令到0x%x失败!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}
	BYTE bIndex = 0;
	for(; bIndex < 200; bIndex ++)
	{
		ZeroMemory(pchRead, 120);
		
		if (!m_Com.ReadBuffer(pchRead, 120)) 
		{
			strTemp.Format("0x%x返回数据失败!", iCommandAddr);
			AfxMessageBox(strTemp, MB_ICONERROR);
			return FALSE;
		}

		strRead = pchRead;
		if ("OK" != strRead.Mid(0, 2))
		{
			continue;
		}
		else
		{
			break;
		}
	}
	if (bIndex > 199) 
	{
		strTemp.Format("发送命令到0x%x无效!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}
	
	ZeroMemory(pchWrite, 120);
	sprintf(pchWrite, "geti2c %s %02x\r", m_I2CAddress, wRxDataLen);
	strRead = pchWrite;

	if (!m_Com.WriteBuffer(pchWrite, strRead.GetLength())) 
	{
		strTemp.Format("发送命令到0x%x失败!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}
	bIndex = 0;
	for(; bIndex < 200; bIndex ++)
	{
		ZeroMemory(pchRead, 120);
		
		if (!m_Com.ReadBuffer(pchRead, 120)) 
		{
			strTemp.Format("0x%x返回数据失败!", iCommandAddr);
			AfxMessageBox(strTemp, MB_ICONERROR);
			return FALSE;
		}

		strRead = pchRead;
		CString strLoop("");
		if (strRead.GetLength() == wRxDataLen*2+3)
		{
			for (int iLoop = 0; iLoop < wRxDataLen; iLoop++) 
			{
				strTemp.Format("%s", strRead.Mid(iLoop*2, 2));
				strLoop += strTemp;
			}
			*strTxData = strLoop;

			break;
		}
		else
		{
			continue;
		}
	}
	if (bIndex > 199) 
	{
		strTemp.Format("发送命令到0x%x无效!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}
	
	return TRUE;
}

CString CCommand::BinTranslateToHex(CString strData)
{
	int iLen = strData.GetLength();
	int idata = 0;
	for(int iLoop = 0; iLoop < iLen; iLoop ++)
	{  
		if(strData[iLoop] == '1')  
		{  
			int iTemp = 1;  
			iTemp = iTemp << (iLen - iLoop - 1);  
			idata |= iTemp; 
		}
	}

	strData.Format("%x", idata);

	return strData;
}

CString CCommand::HexTranslateToBin(CString strHex)
{
	int iDec = 0;
	int iYushu = 0, iShang = 0; 
    CString strBin = _T(""), strTemp; 
    TCHAR buf[2]; 
    BOOL bContinue = TRUE; 

	sscanf(strHex, "%x", &iDec);
    while (bContinue) 
    { 
        iYushu = iDec % 2;
        iShang = iDec / 2;
        sprintf(buf, "%d", iYushu); 
        strTemp = strBin; 
        strBin.Format("%s%s", buf, strTemp); 
        iDec = iShang; 
        if (iShang == 0) 
            bContinue = FALSE; 
    } 
    
	int iTemp = strBin.GetLength()%4;
	
    switch(iTemp) 
    { 
    case 1: 
        strTemp.Format(_T("000%s"), strBin); 
        strBin = strTemp; 
        break; 
    case 2: 
        strTemp.Format(_T("00%s"), strBin); 
        strBin = strTemp; 
        break; 
    case 3: 
        strTemp.Format(_T("0%s"), strBin); 
        strBin = strTemp; 
        break; 
    default: 
        break; 
    }

	return strBin;
}

BOOL CCommand::OpenComPort(CString strComPort, int iBaudRate)
{
	m_Com.ClosePort();

	char *chComPort = strComPort.GetBuffer(4);

	if(m_Com.OpenPort(chComPort, iBaudRate))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CCommand::CloseComPort()
{
	m_Com.ClosePort();
}

BOOL CCommand::Upgrard32ByteCommand(WORD iCommandAddr, CString strTxData)
{
	TCHAR	pchRead[120], pchWrite[120];
	CString strRead;
	CString strTemp;

	ZeroMemory(pchWrite, 120);

	sprintf(pchWrite, "sbi2c %s %02x %s\r", m_I2CAddress, iCommandAddr, strTxData);

	strRead = pchWrite;

	if (!m_Com.WriteBuffer(pchWrite, strRead.GetLength())) 
	{
		strTemp.Format("发送命令到0x%x失败!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}
	BYTE bIndex = 0;
	for(; bIndex < 200; bIndex ++)
	{
		ZeroMemory(pchRead, 120);
		
		if (!m_Com.ReadBuffer(pchRead, 120)) 
		{
			strTemp.Format("0x%x返回数据失败!", iCommandAddr);
			AfxMessageBox(strTemp, MB_ICONERROR);
			return FALSE;
		}

		strRead = pchRead;
		if ("OK" != strRead.Mid(0, 2))
		{
			continue;
		}
		else
		{
			break;
		}
	}
	if (bIndex > 199) 
	{
		strTemp.Format("发送命令到0x%x无效!", iCommandAddr);
		AfxMessageBox(strTemp, MB_ICONERROR);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CCommand::SetSoftReset()
{
	CString strTemp;
	BYTE	pbyData[32];

//	strTemp = "Seti2c C0 d9 01\r";
	strTemp.Format("Seti2c %s d9 01\r", m_I2CAddress);
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	return TRUE;
}


BOOL CCommand::SetWarmReset()
{
	CString strTemp;
	BYTE	pbyData[32];	
	BYTE	bDir;
	BYTE	pbyRx[32];
	BYTE	bIO;
	
	// 1. Get direction
	strTemp.Format("getsdir\r");
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);
	sscanf((char*)pbyRx, "%x", &bDir);

	//	Check output
	if(bDir & BIT_4)
	{
		
	}
	else
	{
		strTemp.Format("setsdir %x\r", bDir | BIT_4);
		memcpy(pbyData, strTemp, strTemp.GetLength());
		m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
		Sleep(10);
		ZeroMemory(pbyRx, 32);
		m_Com.ReadBuffer((char*)pbyRx, 32);
	}

	// 2. Get IO
	strTemp.Format("getsio\r");
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);
	sscanf((char*)pbyRx, "%x", &bIO);

	strTemp.Format("getsdir\r");
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);
	sscanf((char*)pbyRx, "%x", &bDir);

	// 3. Set IO To low level
	bIO &=~BIT_4;
	strTemp.Format("setsio %x\r", bIO);
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);

	//4. Set IO to high level
	bIO |= BIT_4;
	strTemp.Format("setsio %x\r", bIO);
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);	

	return TRUE;	
}

BOOL CCommand::SetColdReset()
{	
	SetGpioBit('s', BIT_6, 1);
	SetGpioBit('s', BIT_6, 0);
	SetGpioBit('s', BIT_6, 1);

	return TRUE;
}

BOOL CCommand::SetI2CData(BYTE bAddress, BYTE *pbData, BYTE bLen)
{
	CString strTemp;
	BYTE	pbyData[32];
	BYTE	pbyRx[32];
	BYTE	bIndex;
	CString strData;

//	strTemp.Format("SetI2C C0 %x", bAddress);
	strTemp.Format("SetI2C %s %x", m_I2CAddress, bAddress);

	for(bIndex = 0; bIndex < bLen; bIndex++)
	{
		strData.Format(" %x", pbData[bIndex]);
		strTemp += strData;
	}

	strTemp += "\r";

	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);	

	return TRUE;	
}

BOOL CCommand::SetI2CData(BYTE bAddress, long lData, BYTE bLen)
{
	CString strTemp;
	BYTE	pbyData[32];
	BYTE	pbyRx[32];
	BYTE	bIndex;
	CString strData;
	BYTE pbData[4];

//	strTemp.Format("SetI2C C0 %x", bAddress);
	strTemp.Format("SetI2C %s %x", m_I2CAddress, bAddress);

	for(bIndex = 0; bIndex < bLen; bIndex++)
	{
		pbData[bIndex] = (BYTE)(lData >> 8*bIndex);
//		strData.Format(" %x", pbData[bIndex]);
//		strTemp += strData;
	}

	for(bIndex = bLen-1; bIndex < bLen; bIndex--)
	{
//		pbData[bIndex] = (BYTE)(lData >> 8*bIndex);
		strData.Format(" %x", pbData[bIndex]);
		strTemp += strData;
	}

	strTemp += "\r";

	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);	

	return TRUE;	
}

BOOL CCommand::GetI2CData(BYTE bAddress, BYTE bLen, BYTE *pbOut)
{
	CString strTemp;
	BYTE	pbyData[32];
	BYTE	pbyRx[64];
	BYTE	bIndex;
	CString strData;

//	strTemp.Format("seti2c c0 %x\r", bAddress);
	strTemp.Format("seti2c %s %x\r", m_I2CAddress, bAddress);

	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);	

//	strTemp.Format("geti2c c0 %x\r", bLen);
	strTemp.Format("geti2c c0 %x\r", bLen);

	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 64);
	m_Com.ReadBuffer((char*)pbyRx, 64);	

	strTemp = (char*)pbyRx;
	for(bIndex = 0; bIndex < bLen; bIndex++)
	{	
		strData = strTemp.Left(2*(bIndex + 1));
		strData = strData.Right(2);
		sscanf(strData, "%x", pbOut + bIndex);
	}

	return TRUE;
}

BOOL CCommand::SetGpioBit(char chPort, BYTE bBit, BYTE bValue)
{
	CString strTemp;
	BYTE	pbyData[32];	
	BYTE	bDir;
	BYTE	pbyRx[32];
	BYTE	bIO;	
	// 1. Get direction
	strTemp.Format("get%cdir\r", chPort);
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);
	sscanf((char*)pbyRx, "%x", &bDir);

	//	Check output
	if(bDir & bBit)
	{
		
	}
	else
	{
		strTemp.Format("set%cdir %x\r", chPort, bDir | bBit);
		memcpy(pbyData, strTemp, strTemp.GetLength());
		m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
		Sleep(10);
		ZeroMemory(pbyRx, 32);
		m_Com.ReadBuffer((char*)pbyRx, 32);
	}

	// 2. Get IO
	strTemp.Format("get%cio\r", chPort);
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);
	sscanf((char*)pbyRx, "%x", &bIO);


	// 3. Set IO To low level
	
	if(bValue == 0)
		bIO &=~bBit;
	else
		bIO |= bBit;
	strTemp.Format("set%cio %x\r",chPort, bIO);
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);	

	return TRUE;
}

BOOL CCommand::GetGpioBit(char chPort, BYTE bBit, BYTE *pbValue)
{
	CString strTemp;
	BYTE	pbyData[32];	
	BYTE	bDir;
	BYTE	pbyRx[32];
	BYTE	bIO;	
	// 1. Get direction
	strTemp.Format("get%cdir\r", chPort);
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);
	m_Com.ReadBuffer((char*)pbyRx, 32);
	sscanf((char*)pbyRx, "%x", &bDir);

	//	If this bit is output, set it to input
	if(bDir & bBit)
	{
		strTemp.Format("set%cdir %x\r", chPort, bDir & ~bBit);
		memcpy(pbyData, strTemp, strTemp.GetLength());
		m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
		Sleep(10);
		ZeroMemory(pbyRx, 32);
		m_Com.ReadBuffer((char*)pbyRx, 32);		
	}

	// 2. Get IO
	strTemp.Format("get%cio\r", chPort);
	memcpy(pbyData, strTemp, strTemp.GetLength());
	m_Com.WriteBuffer((char*)pbyData, strTemp.GetLength());
	Sleep(10);
	ZeroMemory(pbyRx, 32);	
	m_Com.ReadBuffer((char*)pbyRx, 32);
	sscanf((char*)pbyRx, "%x", &bIO);
	
	*pbValue = (bIO & bBit);

	if(*pbValue > 0)
	{
		*pbValue  = 1;
	}

	return TRUE;
}

