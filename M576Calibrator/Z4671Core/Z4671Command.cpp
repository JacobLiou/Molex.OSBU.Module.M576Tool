// Z4671Command.cpp: implementation of the Z4671Command class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "126s_45v_switch_app.h"
#include "Z4671Command.h"
#include "CommRetry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Z4671Command::Z4671Command()
{
   m_strLogInfo ="No Error";
	m_traceSeq = 0;
	m_pendingTraceSeq = 0;
	m_pendingTick = 0;
	m_pendingCmdCode = 0;
}

Z4671Command::~Z4671Command()
{

}

CString Z4671Command::LookupCommandName(BYTE cmdCode) const
{
	switch (cmdCode)
	{
	case CMD_HW_START_FIRMWARE_UPDATE:
		return _T("FW_START");
	case CMD_HW_TRANSPORT_FIRMWARE:
		return _T("FW_CHUNK");
	case CMD_HW_FIRMWARE_UPDATE_END:
		return _T("FW_END");
	case CMD_HW_RD_FILE:
		return _T("GET_LOG_FILE");
	case CMD_HW_READ_FILE_LEN:
		return _T("READ_FILE_LEN");
	case CMD_HW_RESET_MODULE:
		return _T("RESET_MODULE");
	default:
		{
			CString name;
			name.Format(_T("CMD_0x%02X"), cmdCode);
			return name;
		}
	}
}

void Z4671Command::TraceInfo(LPCTSTR category, LPCTSTR format, ...)
{
	if (!m_logTarget.IsEnabled())
		return;
	va_list args;
	va_start(args, format);
	CString detail;
	detail.FormatV(format, args);
	va_end(args);
	CString line;
	line.Format(_T("[INFO] [%s] %s"), category, detail.GetString());
	m_logTarget.Emit(line);
}

void Z4671Command::TraceError(LPCTSTR category, LPCTSTR format, ...)
{
	if (!m_logTarget.IsEnabled())
		return;
	va_list args;
	va_start(args, format);
	CString detail;
	detail.FormatV(format, args);
	va_end(args);
	CString line;
	line.Format(_T("[ERROR] [%s] %s"), category, detail.GetString());
	m_logTarget.Emit(line);
}

void Z4671Command::TraceFrame(BOOL isSend, BYTE cmdCode, const BYTE* frame, int frameLen, const BYTE* wireData, int wireLen, DWORD elapsedMs)
{
	if (!m_logTarget.IsEnabled())
		return;
	const DWORD seq = isSend ? ++m_traceSeq : (m_pendingTraceSeq != 0 ? m_pendingTraceSeq : ++m_traceSeq);
	CString name = LookupCommandName(cmdCode);
	CString line;
	if (isSend)
	{
		m_pendingTraceSeq = seq;
		m_pendingTick = GetTickCount();
		m_pendingCmdCode = cmdCode;
		m_pendingCmdName = name;
		line.Format(_T("[Z4671] #%lu SEND %s | frame=%s | wire=%s"),
			seq,
			name.GetString(),
			M576HexDump(frame, frameLen).GetString(),
			M576HexDump(wireData, wireLen).GetString());
	}
	else
	{
		if (elapsedMs > 0)
			line.Format(_T("[Z4671] #%lu RECV %s | wire=%s | frame=%s | %lums"),
				seq,
				m_pendingCmdName.IsEmpty() ? name.GetString() : m_pendingCmdName.GetString(),
				M576HexDump(wireData, wireLen).GetString(),
				M576HexDump(frame, frameLen).GetString(),
				elapsedMs);
		else
			line.Format(_T("[Z4671] #%lu RECV %s | wire=%s | frame=%s"),
				seq,
				m_pendingCmdName.IsEmpty() ? name.GetString() : m_pendingCmdName.GetString(),
				M576HexDump(wireData, wireLen).GetString(),
				M576HexDump(frame, frameLen).GetString());
		m_pendingTraceSeq = 0;
		m_pendingTick = 0;
		m_pendingCmdCode = 0;
		m_pendingCmdName.Empty();
	}
	m_logTarget.Emit(line);
}

BOOL Z4671Command::MasterReset()
{
	if(!WriteBuffer("MRST\r\n",strlen("MRST\r\n")))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL Z4671Command::HardReset()
{
	if(!WriteBuffer("HRST\r\n",5))
	{
		return FALSE;
	}
    return TRUE;
}

BOOL Z4671Command::Error()
{
	if(!WriteBuffer("ERROR\r\n",6))
	{
		return FALSE;
	}
    return TRUE;
}

BOOL Z4671Command::WatchDog()
{
	if(!WriteBuffer("WDERR\r\n",6))
	{
		return FALSE;
	}
    return TRUE;
}

BOOL Z4671Command::ClearIRQ()
{
	if(!WriteBuffer("CIRQ\r\n",5))
	{
		return FALSE;
	}
    return TRUE;
}

BOOL Z4671Command::ReadIRQ()
{
	if(!WriteBuffer("IRQ\r\n",4))
	{
		return FALSE;
	}
    return TRUE;
}

//128
BOOL Z4671Command::GetModulePara()
{
	char  chSendBuf[256];
	char  chReadBuf[2048];
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[256];
	
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	CString strMsg;
	
	byData[0] = START_CMD;
	byData[1] = 0XB2;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X81; 
	BYTE byID = 0;
	for ( byID=0;byID<0X81;byID++)
	{
		byData[5+byID] = byID;
	}
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[5+byID] = nCheckSum-1;
	byData[5+byID+1] = END_CMD;
	WORD nCmdLength = 5+byID+1+1;
	PBYTE pbySendData=NULL;
	if(!CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength))
	{
		m_strLogInfo.Format("CmdSendExchange执行错误");
		return FALSE;
	}
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo.Format("Send GetModulePara command failed");
		return FALSE;
	}
	Sleep(50);
	BYTE  byGetData[2048];
	if (!ReadBuffer((char*)byGetData,2048,&nCmdLength))
	{
		m_strLogInfo.Format("Read GetModulePara response failed");
		return FALSE;
	}
	PBYTE pbyNewData=NULL;
	
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		m_strLogInfo.Format("CmdReadExchange执行错误(%s)",strReadData);
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		m_strLogInfo.Format("返回第2字节错误:%02X(%s)",pbyNewData[1],strReadData);
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		m_strLogInfo.Format("返回第3字节错误:%02X(%s)",pbyNewData[2],strReadData);
		return FALSE;
	}
	if ((pbyNewData[3]*256+pbyNewData[4])!=646) 
	{
		m_strLogInfo.Format("返回第4字节错误:%02X(%s)",pbyNewData[4],strReadData);
		return FALSE;
	}
	if (pbyNewData[5]!=0XB2) 
	{
		m_strLogInfo.Format("返回第6字节错误:%02X(%s)",pbyNewData[5],strReadData);
		return FALSE;
	}
	int nIndex = 0;
	for ( nIndex=0;nIndex<129;nIndex++)
	{
		m_dwPara[nIndex] = (pbyNewData[7+nIndex*5]<<24)+(pbyNewData[8+nIndex*5]<<16)+(pbyNewData[9+nIndex*5]<<8)+pbyNewData[10+nIndex*5];
	}
	
	//Sleep(100);
	return TRUE;
}

BOOL Z4671Command::CmdSendExchange(PBYTE pbyte, WORD wLength, PBYTE *pReBuff, PWORD pwReLength)
{
	WORD wIndex;
	WORD wNewIndex=0;
	//BYTE pNewData[MAX_COUNT];
	ZeroMemory(m_pNewData,sizeof(m_pNewData));
	if (wLength>MAX_COUNT)
	{
		//LogInfo("指令数量超长",(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	m_pNewData[0] = pbyte[0]; //START_CMD
	//除开开头和结尾
	for (wIndex=1;wIndex<(wLength-1);wIndex++)
	{
		wNewIndex++;
		if (pbyte[wIndex]==START_CMD) 
		{
			m_pNewData[wNewIndex] = START_CMD+1;
			wNewIndex++;
			m_pNewData[wNewIndex] = START_CMD+1;
		}
		else if (pbyte[wIndex]==START_CMD+1) 
		{
			m_pNewData[wNewIndex] = START_CMD+1;
			wNewIndex++;
			m_pNewData[wNewIndex] = START_CMD+2;
		}
		else if (pbyte[wIndex]==END_CMD)
		{
			m_pNewData[wNewIndex] = END_CMD+1;
			wNewIndex++;
			m_pNewData[wNewIndex] = END_CMD+1;
		}
		else if (pbyte[wIndex]==END_CMD+1)
		{
			m_pNewData[wNewIndex] = END_CMD+1;
			wNewIndex++;
			m_pNewData[wNewIndex] = END_CMD+2;
		}
		else
		{
			m_pNewData[wNewIndex] = pbyte[wIndex];
		}
	}
	wNewIndex++;
	m_pNewData[wNewIndex] = pbyte[wLength-1];
	wNewIndex++;
	*pwReLength = wNewIndex;
	*pReBuff = m_pNewData;
	TraceFrame(TRUE, (wLength > 1) ? pbyte[1] : 0, pbyte, wLength, m_pNewData, wNewIndex);
	return TRUE;

}

BOOL Z4671Command::CmdReadExchange(PBYTE pbyte, WORD wLength, PBYTE *pReBuff, PWORD pwReLength)
{
	CString strMsg;
	CString strTemp;
	WORD wIndex;
	WORD wNewIndex=0;
	int  i=0;
	//BYTE m_pNewData[MAX_COUNT];
	ZeroMemory(m_pNewData,sizeof(m_pNewData));
	if (wLength>MAX_COUNT)
	{
		strMsg.Format("指令数量为%d，超出范围！",wLength);
		//LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	m_pNewData[0] = pbyte[0]; //START_CMD
	//除开开头和结尾
	for (wIndex=1;wIndex<(wLength-1);wIndex++)
	{
		wNewIndex++;
		if (pbyte[wIndex]==START_CMD||pbyte[wIndex]==END_CMD) 
		{
			strMsg.Format("回读数据不能出现标志位START_CMD/END_CMD");
            //LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			strTemp = "";
			for (i=0;i<(wLength-1);i++)
			{
				strMsg.Format("%02X ",pbyte[i]);
				strTemp = strTemp + strMsg;
			}
			//LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
			
//			return FALSE;
		}
		else if (pbyte[wIndex]==START_CMD+1) 
		{
			if (pbyte[wIndex+1]==START_CMD+1) 
			{
				m_pNewData[wNewIndex] = START_CMD;
				wIndex++;
			}
			else if (pbyte[wIndex+1]==START_CMD+2)
			{
				m_pNewData[wNewIndex] = START_CMD+1;
				wIndex++;
			}
			else
			{
				for (i=0;i<(wLength-1);i++)
				{
					strMsg.Format("%02X ",pbyte[i]);
					strTemp = strTemp + strMsg;
				}
				//LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
				//LogInfo("回读数据不能出现标志位START_CMD+1",(BOOL)FALSE,COLOR_RED);
		    //	return FALSE;
			}
		}
		else if (pbyte[wIndex]==END_CMD+1)
		{
			if (pbyte[wIndex+1]==END_CMD+1)
			{
				m_pNewData[wNewIndex] = END_CMD;
				wIndex++;
			}
			else if (pbyte[wIndex+1]==END_CMD+2) 
			{
				m_pNewData[wNewIndex] = END_CMD+1;
				wIndex++;
			}
			else
			{
				for (i=0;i<(wLength-1);i++)
				{
					strMsg.Format("%02X ",pbyte[i]);
					strTemp = strTemp + strMsg;
				}
				//LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
				//LogInfo("回读数据不能出现标志位END_CMD+1",(BOOL)FALSE,COLOR_RED);
		    	//return FALSE;
			}
		}
		else
		{
			m_pNewData[wNewIndex] = pbyte[wIndex];
		}
	}
	wNewIndex++;
	m_pNewData[wNewIndex] = pbyte[wLength-1];
	wNewIndex++;
	if (pwReLength != NULL)
		*pwReLength = wNewIndex;
	*pReBuff = &m_pNewData[0];
	TraceFrame(FALSE, (wNewIndex > 1) ? m_pNewData[1] : m_pendingCmdCode, m_pNewData, wNewIndex, pbyte, wLength,
		(m_pendingTick != 0) ? (GetTickCount() - m_pendingTick) : 0);
	return TRUE;

}

BOOL Z4671Command::ReadModuleVoltage()
{
	char  chSendBuf[256];
	char  chReadBuf[256];
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE byData[256];
	BYTE byGetData[256];
	
	int   i;
	int   nLength=0;
	int   nCheckSum;
	CString strValue;
	CString strMsg;

	byData[0] = START_CMD;
	byData[1] = 0XC2;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X00; 
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[5] = nCheckSum-1;
	byData[6] = END_CMD;
	WORD nCmdLength = 7;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = "串口打开错误！";
		return FALSE;
	}
	Sleep(20);
	
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = "读取数据错误";
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	if (pbyNewData[5]!=0XC2) 
	{
		return FALSE;
	}
	int nIndex = 0;
	for ( nIndex=0;nIndex<5;nIndex++)
	{
		m_wVoltage[nIndex] = (pbyNewData[9+nIndex*5]<<8)+pbyNewData[10+nIndex*5];
	}
	return TRUE;

}

//0:AGC
//1:APC
BOOL Z4671Command::SetEDFAWorkMode(int nEDFA, int nMode, double dblPower)
{
	char  chSendBuf[256];
	char  chReadBuf[256];
	CString strCmd;
	CString strToken;
	LONG   lPower;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[256];
	
	int   i;
	int   nLength=0;
	int   nCheckSum;
	CString strValue;
	CString strMsg;

	byData[0] = START_CMD;
	byData[1] = 0XBA;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X06; 
	if (nEDFA<8)
	{
		byData[5] = nEDFA+1;
	}
	else if (nEDFA>=8&&nEDFA<16) 
	{
		byData[5] = nEDFA + 59;
	}
	else
	{
		return FALSE;
	}
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	byData[6] = nMode;
	if (nMode==0)
	{
		lPower = 0x7FFFFFFF;
	}
	else
	{
		lPower = (long)dblPower*10;
	}
	byData[7] = (BYTE)(lPower>>24);
	byData[8] = (BYTE)(lPower>>16);
	byData[9] = (BYTE)(lPower>>8);
	byData[10] = (BYTE)(lPower);
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[11] = nCheckSum-1;
	byData[12] = END_CMD;
	WORD nCmdLength = 13;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(20);
	BYTE  byGetData[256];
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("读取数据错误");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	if (pbyNewData[5]!=0XBA) 
	{
		return FALSE;
	}
	return TRUE;
}

BOOL Z4671Command::ReadAlarm(int nType)
{
	char  chSendBuf[256];
	char  chReadBuf[256];
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[256];
	
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	CString strMsg;

	byData[0] = START_CMD;
	byData[1] = 0XB5;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X02; 
	byData[5] = 0X00;
	byData[6] = nType;
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[7] = nCheckSum-1;
	byData[8] = END_CMD;
	WORD nCmdLength = 9;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(20);
	BYTE  byGetData[256];
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("发送数据错误");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	if (pbyNewData[5]!=0XB5) 
	{
		return FALSE;
	}
	int nIndex = 0;
	for ( nIndex=0;nIndex<50;nIndex++)
	{
		m_pbyAlarm[nIndex] = pbyNewData[8+nIndex];
	}
	//Sleep(50);
	return TRUE;

}

BOOL Z4671Command::GetVOAAtten(int nVOAIndex, double *pdblAtten, int nType)
{
	char  chSendBuf[256];
	char  chReadBuf[256];
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[256];
	
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	double dblValue;
	

	byData[0] = START_CMD;
	byData[1] = 0XBE;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X01; 
	if (nType==0)
	{
		byData[5] = nVOAIndex + 59;  //EDFA VOA
	}
	else
	{
		byData[5] = nVOAIndex + 17; //SWITCH VOA
	}
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[6] = nCheckSum-1;
	byData[7] = END_CMD;
	WORD nCmdLength = 8;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = "发送数据错误";
		return FALSE;
	}
	Sleep(20);
	BYTE  byGetData[256];
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = "发送数据错误";
		return FALSE;
	}

	CString strMsg;
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	if (pbyNewData[5]!=byData[1]) 
	{
		return FALSE;
	}

	short  sValue=0;
	sValue = (pbyNewData[6]<<8)+pbyNewData[7];
	dblValue = sValue/10.0;
	*pdblAtten = dblValue;
	return TRUE;

}

BOOL Z4671Command::GetLogFileData(int nType,int nDataLength,DWORD dwAddress,byte **pbyData,int *pReDataLength)
{
	BOOL ok = FALSE;
	(void)M576WithRetry(M576_COMM_RETRY_MAX_ATTEMPTS, (DWORD)M576_COMM_RETRY_DELAY_MS, [&]() -> bool {
		ok = GetLogFileDataNoRetry(nType, nDataLength, dwAddress, pbyData, pReDataLength);
		return ok;
	});
	return ok;
}

BOOL Z4671Command::GetLogFileDataNoRetry(int nType,int nDataLength,DWORD dwAddress,byte **pbyData,int *pReDataLength)
{
	char  chSendBuf[256];
	char  chReadBuf[MAX_COUNT];
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	PBYTE  byData = new BYTE[256];
	
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	TraceInfo(_T("GetLogFileData"), _T("request type=%d dataLen=%d addr=0x%08lX"), nType, nDataLength, dwAddress);
//	double dblValue;
	
	byData[0] = START_CMD;
	byData[1] = 0XC4;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X06; 
	byData[5] = nType;
	nLength = byData[3]*256+byData[4]+4;
	byData[6] = nDataLength;
	byData[7] = (BYTE)(dwAddress>>24);
	byData[8] = (BYTE)(dwAddress>>16);
	byData[9] = (BYTE)(dwAddress>>8);
	byData[10] = (BYTE)dwAddress;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}	
	byData[11] = nCheckSum-1;
	byData[12] = END_CMD;
	WORD nCmdLength = 13;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	TraceInfo(_T("GetLogFileData"), _T("sendTx len=%u data=%s"), nCmdLength, M576HexDump(pbySendData, nCmdLength).GetString());
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		TraceError(_T("GetLogFileData"), _T("WriteBuffer failed"));
		return FALSE;
	}
	Sleep(100);
	PBYTE  byGetData = new BYTE[MAX_COUNT];
	if (!ReadBuffer((char*)byGetData,MAX_COUNT,&nCmdLength))
	{
		//LogInfo("读取EDFA返回接收错误");
		TraceError(_T("GetLogFileData"), _T("ReadBuffer failed"));
		delete []byGetData;
		return FALSE;
	}
	TraceInfo(_T("GetLogFileData"), _T("rawRx len=%u data=%s"), nCmdLength, M576HexDump(byGetData, nCmdLength).GetString());
	PBYTE pbyNewData=NULL;
	WORD nDecodedLength = 0;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData,&nDecodedLength))
	{
		TraceError(_T("GetLogFileData"), _T("CmdReadExchange failed"));
		delete []byGetData;
		return FALSE;
	}
	TraceInfo(_T("GetLogFileData"), _T("decoded len=%u data=%s"), nDecodedLength, M576HexDump(pbyNewData, nDecodedLength).GetString());
	delete []byGetData;
	byGetData = NULL;
	if (pbyNewData[1]!=0X00)
	{
		TraceError(_T("GetLogFileData"), _T("status not success: 0x%02X"), pbyNewData[1]);
		delete []byData;
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		TraceError(_T("GetLogFileData"), _T("index mismatch rsp=0x%02X req=0x%02X"), pbyNewData[2], byData[2]);
		delete []byData;
		return FALSE;
	}
//	delete []byData;
	if (pbyNewData[5]!=byData[1]) 
	{
		TraceError(_T("GetLogFileData"), _T("cmd mismatch rsp=0x%02X req=0x%02X"), pbyNewData[5], byData[1]);
		delete []byData;
		return FALSE;
	}
	delete []byData;
	byData = NULL;

	*pReDataLength = pbyNewData[7];
	*pbyData = &pbyNewData[12];
	if (*pReDataLength > 0)
	{
		const int nPayloadDumpLen = (nDecodedLength > 12) ? ((*pReDataLength < (int)(nDecodedLength - 12)) ? *pReDataLength : (int)(nDecodedLength - 12)) : 0;
		TraceInfo(_T("GetLogFileData"), _T("payload len=%d data=%s"), *pReDataLength,
			(nPayloadDumpLen > 0) ? M576HexDump(*pbyData, nPayloadDumpLen).GetString() : _T("<empty-or-out-of-range>"));
	}
	else
	{
		TraceInfo(_T("GetLogFileData"), _T("payload len=0"));
	}
	//pbyData = pbyGetData;
	return TRUE;

}

BOOL Z4671Command::SendFWTranSportFW(BYTE *byTransData, int nDataLength, int nIndex, int nSum)
{
	BYTE byData[500];
	int  nCheckSum;
	int  nLength;
//	BYTE byGetData[256];
	int  i;
	char chData[20];
	CString strData;
	
	ZeroMemory(chData,sizeof(chData));
	if (nDataLength>403)
	{
		return FALSE;
	}
	
	nLength = nDataLength+12;
	byData[0] = START_CMD;
	byData[1] = 0XE1;
	byData[2] = nIndex; //Index
	byData[3] = (BYTE)(nLength>>8);
	byData[4] = (BYTE)nLength;
	byData[5] = (BYTE)(nSum>>24);  //总包数
	byData[6] = (BYTE)(nSum>>16); 
	byData[7] = (BYTE)(nSum>>8);  //总包数
	byData[8] = (BYTE)nSum; 
	byData[9] = (BYTE)(nIndex>>24);
	byData[10] = (BYTE)(nIndex>>16);  //当前包
	byData[11] = (BYTE)(nIndex>>8);
	byData[12] = (BYTE)nIndex;  //当前
	byData[13] = (BYTE)(nDataLength>>24);
	byData[14] = (BYTE)(nDataLength>>16);  //当前
	byData[15] = (BYTE)(nDataLength>>8);
	byData[16] = (BYTE)nDataLength;  //当前
	for (i=0;i<nDataLength;i++)
	{
		byData[17+i] = byTransData[i];
	}
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[17+nDataLength] = nCheckSum-1;
	byData[17+nDataLength+1] = END_CMD;
	WORD nCmdLength = 17+nDataLength+2;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	int nCount=0;
//	BOOL bBreak;
	{
		BOOL wOk = FALSE;
		for (int w = 0; w < M576_COMM_RETRY_MAX_ATTEMPTS; ++w)
		{
			if (WriteBuffer((char*)pbySendData, nCmdLength))
			{
				wOk = TRUE;
				break;
			}
			if (w + 1 < M576_COMM_RETRY_MAX_ATTEMPTS)
				Sleep((DWORD)M576_COMM_RETRY_DELAY_MS);
		}
		if (!wOk)
			return FALSE;
	}
	return TRUE;

}

BOOL Z4671Command::SendMaster()
{
	if(!WriteBuffer("Master 3000\r\n",12))
	{
		return FALSE;
	}
	return TRUE;

}
CString Z4671Command::GetErrorMsg()
{
	return m_strLogInfo;
}

///////////////////////////////////////////////
//函数说明：
//    FPGA SPI 通讯测试
//Add by wanxin
//Time:20160819
///////////////////////////////////////////////////
BOOL Z4671Command::FPGASPIConnect()
{
	BYTE byData[600];
	ZeroMemory(byData,600);
	//F0 14 00 10 4F 50 4C 4B 4D 46 47 E0 81 00 00 00 00 00 02 00 C2        
	byData[0] = 0x55;
	byData[1] = 0xF0;
	byData[2] = 0X14;
	byData[3] = 0X00; //Index
	byData[4] = 0x10;
	byData[5] = 0x4F;
	byData[6] = 0x50;  
	byData[7] = 0x4C; 
	byData[8] = 0x4B;  
	byData[9] = 0x4D; 
	byData[10] = 0x46;
	byData[11] = 0x47;  
	byData[12] = 0xE0;
	byData[13] = 0x81;  
	byData[14] = 0x00;
	byData[15] = 0x00;  
	byData[16] = 0x00;
	byData[17] = 0x00;  
	byData[18] = 0x00;  
	byData[19] = 0x02;  
	byData[20]= 0x00;  
	byData[21] = 0xC2; 
	byData[22] = 0xAA;

	if(!WriteBuffer((char*)byData,23))
	{
		m_strLogInfo ="Send FPGA SPI connect command failed";
		return FALSE;
	}

	return TRUE;	
}
//////////////////////////////////////////////////
//函数说明：
//    发送EDFA SRESET内容的第一条指令
//Add by wanxin
//Time:20160819
////////////////////////////////////////////////////////
BOOL Z4671Command::EDFA_SRESETONECMD()
{
	BYTE bBuffer[600];
	static const char kMaster3000[] = "MASTER 3000\r\n";
	const DWORD masterLen = (DWORD)strlen(kMaster3000);
	ZeroMemory(bBuffer,600);
	memcpy(bBuffer, kMaster3000, masterLen);

	/*
	ZeroMemory(bBuffer,600);
	bBuffer[0]='M';
	bBuffer[1]='A';
	bBuffer[2]='S';
	bBuffer[3]='T'; 
	bBuffer[4]='E';
	bBuffer[5]='R';
	bBuffer[6]=' ';
	bBuffer[7]='3';
	bBuffer[8]='0';
	bBuffer[9]='0';
	bBuffer[10]='0';
	bBuffer[11]='\r';
	dwLength = strlen((char*)bBuffer);
	*/
	if(!WriteBuffer("\r\n",1))
	{
		m_strLogInfo = "Send EDFA SRESET first command prefix failed";
		return FALSE;			
	}
	Sleep(100);
	if(!WriteBuffer((char*)bBuffer, masterLen))
	{
		m_strLogInfo = "Send EDFA SRESET first command failed";
		return FALSE;			
	}
	Sleep(100);
	ZeroMemory(bBuffer,600);
	if(!ReadBuffer((char*)bBuffer,600))
	{
		m_strLogInfo = "EDFA_SRESET测试中接收第一条指令返回内容错误";
		return FALSE;			
	}
	
    if(bBuffer[0]!=0x4F && bBuffer[0]!=0x4B && bBuffer[0]!=0x0D)
	{
		m_strLogInfo = "EDFA_SRESET测试中接收第一条指令返回内容与配置内容不符";
		return FALSE;		
	}
	return TRUE;
}

//////////////////////////////////////////////////
//函数说明：
//    发送EDFA SRESET内容的第二条指令
//Add by wanxin
//Time:20160819
////////////////////////////////////////////////////////
BOOL Z4671Command::EDFA_SRESETTWOCMD()
{
	BYTE bBuffer[600];
	DWORD dwLength;
	BYTE bResult[29]=
	{0x00, 0x16, 0x00, 0x18, 0xF0, 0x0D, 0x0A, 0x49,
	 0x44, 0x58, 0x5F, 0x48, 0x41, 0x52, 0x44, 0x5F, 
	 0x52, 0x53, 0x54, 0x0D, 0x0A, 0x4F, 0x4B, 0x0D, 
	 0x0A, 0x0D, 0x0A, 0x3E, 0xDA};
	
	ZeroMemory(bBuffer,600);
	//F0 16 00 12 4F 50 4C 4B 4D 46 47 F9 62 6F 6F 74 74 79 70 65 0D 0A 4F
	bBuffer[0]=0x55;
	bBuffer[1]=0xF0;
	bBuffer[2]=0x16;
	bBuffer[3]=0x00; 
	bBuffer[4]=0x12;
	bBuffer[5]=0x4F;
	bBuffer[6]=0x50;
	bBuffer[7]=0x4C;
	bBuffer[8]=0x4B;
	bBuffer[9]=0x4D;
	bBuffer[10]=0x46;
	bBuffer[11]=0x47;
	bBuffer[12]=0xF9;
	bBuffer[13]=0x62;
	bBuffer[14]=0x6F;
	bBuffer[15]=0x6F;
	bBuffer[16]=0x74;
	bBuffer[17]=0x74;
	bBuffer[18]=0x79;
	bBuffer[19]=0x70;
	bBuffer[20]=0x65;
	bBuffer[21]=0x0D;
	bBuffer[22]=0x0A;
	bBuffer[23]=0x4F;
	bBuffer[24]=0xAA;
	dwLength = 25;
	if(!WriteBuffer((char*)bBuffer,dwLength))
	{
		m_strLogInfo = "Send EDFA SRESET second command failed";
		return FALSE;			
	}
	Sleep(50);
	ZeroMemory(bBuffer,600);
	if(!ReadBuffer((char*)bBuffer,600))
	{
		m_strLogInfo = "EDFA_SRESET测试中接收第二条指令返回内容错误";
		return FALSE;			
	}
	for(int i=0;i<29;i++)
	{
		if(bBuffer[i+1] != bResult[i])
		{
			m_strLogInfo = "EDFA SRESET second response content mismatch";
			return FALSE;			   
		} 
	}

	return TRUE;
}

//////////////////////////////////////////////////
//函数说明：
//    发送IDX_HARD_RST指令
//Add by wanxin
//Time:20160819
////////////////////////////////////////////////////////
BOOL Z4671Command::IDX_HARD_RST()
{
	BYTE bBuffer[600];
	DWORD dwLength;
	//00 17 00 01 F0 E5
	BYTE bResult[6]={0x00, 0x17, 0x00, 0x01, 0xF0, 0xE5};
	
	ZeroMemory(bBuffer,600);
	//F0 17 00 08 4F 50 4C 4B 4D 46 47 FE 44 
	bBuffer[0]=0x55;
	bBuffer[1]=0xF0;
	bBuffer[2]=0x17;
	bBuffer[3]=0x00;
	bBuffer[4]=0x08; 
	bBuffer[5]=0x4F;
	bBuffer[6]=0x50;
	bBuffer[7]=0x4C;
	bBuffer[8]=0x4B;
	bBuffer[9]=0x4D;
	bBuffer[10]=0x46;
	bBuffer[11]=0x47;
	bBuffer[12]=0xFE;
	bBuffer[13]=0x44;
	bBuffer[14]=0xAA;
	dwLength = 15;
	if(!WriteBuffer((char*)bBuffer,dwLength))
	{
		m_strLogInfo = "Send IDX_HARD_RST command failed";
		return FALSE;			
	}
	Sleep(50);
	ZeroMemory(bBuffer,600);
	if(!ReadBuffer((char*)bBuffer,600))
	{
		m_strLogInfo = "IDX_HARD_RST测试中接收指令返回内容错误";
		return FALSE;			
	}
	for(int i=0;i<6;i++)
	{
		if(bBuffer[i+1] != bResult[i])
		{
			m_strLogInfo = "IDX_HARD_RST response content mismatch";
			return FALSE;			   
		} 
	}

	return TRUE;
}
//////////////////////////////////////////////////
//函数说明：
//    发送IDX_SOFT_RST指令
//Add by wanxin
//Time:20160819
////////////////////////////////////////////////////////
BOOL Z4671Command::IDX_SOFT_RST()
{
	BYTE bBuffer[600];
	DWORD dwLength;
	//00 18 00 18 F0 0D 0A 49 44 58 5F 53 4F 46 54 5F 52 53 54 0D 0A 4F 4B 0D 0A 0D 0A 3E C3
	BYTE bResult[29]=
	{0x00, 0x18, 0x00, 0x18, 0xF0, 0x0D, 0x0A, 0x49,
	0x44, 0x58, 0x5F, 0x53, 0x4F, 0x46, 0x54, 0x5F,
	0x52, 0x53, 0x54, 0x0D, 0x0A, 0x4F, 0x4B, 0x0D, 
	0x0A, 0x0D, 0x0A, 0x3E, 0xC3};
	
	ZeroMemory(bBuffer,600);
	//F0 18 00 12 4F 50 4C 4B 4D 46 47 F9 62 6F 6F 74 74 79 70 65 0D 0A 5D
	bBuffer[0]=0x55;
	bBuffer[1]=0xF0;
	bBuffer[2]=0x18;
	bBuffer[3]=0x00; 
	bBuffer[4]=0x12;
	bBuffer[5]=0x4F;
	bBuffer[6]=0x50;
	bBuffer[7]=0x4C;
	bBuffer[8]=0x4B;
	bBuffer[9]=0x4D;
	bBuffer[10]=0x46;
	bBuffer[11]=0x47;
	bBuffer[12]=0xF9;
	bBuffer[13]=0x62;
	bBuffer[14]=0x6F;
	bBuffer[15]=0x6F;
	bBuffer[16]=0x74;
	bBuffer[17]=0x74;
	bBuffer[18]=0x79;
	bBuffer[19]=0x70;
	bBuffer[20]=0x65;
	bBuffer[21]=0x0D;
	bBuffer[22]=0x0A;
	bBuffer[23]=0x5D;
	bBuffer[24]=0xAA;
	dwLength = 25;
	if(!WriteBuffer((char*)bBuffer,dwLength))
	{
		m_strLogInfo = "Send IDX_SOFT_RST first command failed";
		return FALSE;			
	}
	Sleep(50);
	ZeroMemory(bBuffer,600);
	if(!ReadBuffer((char*)bBuffer,600))
	{
		m_strLogInfo = "IDX_SOFT_RST测试中接收指令返回内容错误";
		return FALSE;			
	}
    Sleep(2000);
	//ONCE MORE
	ZeroMemory(bBuffer,600);
	//F0 18 00 12 4F 50 4C 4B 4D 46 47 F9 62 6F 6F 74 74 79 70 65 0D 0A 5D
	bBuffer[0]=0x55;
	bBuffer[1]=0xF0;
	bBuffer[2]=0x18;
	bBuffer[3]=0x00; 
	bBuffer[4]=0x12;
	bBuffer[5]=0x4F;
	bBuffer[6]=0x50;
	bBuffer[7]=0x4C;
	bBuffer[8]=0x4B;
	bBuffer[9]=0x4D;
	bBuffer[10]=0x46;
	bBuffer[11]=0x47;
	bBuffer[12]=0xF9;
	bBuffer[13]=0x62;
	bBuffer[14]=0x6F;
	bBuffer[15]=0x6F;
	bBuffer[16]=0x74;
	bBuffer[17]=0x74;
	bBuffer[18]=0x79;
	bBuffer[19]=0x70;
	bBuffer[20]=0x65;
	bBuffer[21]=0x0D;
	bBuffer[22]=0x0A;
	bBuffer[23]=0x5D;
	bBuffer[24]=0xAA;
	dwLength = 25;
	if(!WriteBuffer((char*)bBuffer,dwLength))
	{
		m_strLogInfo = "Send IDX_SOFT_RST second command failed";
		return FALSE;			
	}
	Sleep(50);
	ZeroMemory(bBuffer,600);
	if(!ReadBuffer((char*)bBuffer,600))
	{
		m_strLogInfo = "IDX_SOFT_RST测试中接收指令返回内容错误";
		return FALSE;			
	}
	for(int i=0;i<29;i++)
	{
		if(bBuffer[i+1] != bResult[i])
		{
			m_strLogInfo = "IDX_SOFT_RST response content mismatch";
			return FALSE;			   
		} 
	}

	return TRUE;
}

BOOL Z4671Command::GetDumpData(char **pReBuff,int *dwSize)
{
	BYTE bBuffer[600];
	DWORD dwLength;
	char chBuff[600];
	int nIndex=0;
	DWORD dwLeng;
	
	ZeroMemory(bBuffer,600);
	//F0 18 00 12 4F 50 4C 4B 4D 46 47 F9 62 6F 6F 74 74 79 70 65 0D 0A 5D
	bBuffer[nIndex++]=0x55;
	bBuffer[nIndex++]=0xF0;
	bBuffer[nIndex++]=0x40;
	bBuffer[nIndex++]=0x00; 
	bBuffer[nIndex++]=0x1B;
	bBuffer[nIndex++]=0x4F;
	bBuffer[nIndex++]=0x50;
	bBuffer[nIndex++]=0x4C;
	bBuffer[nIndex++]=0x4B;
	bBuffer[nIndex++]=0x4D;
	bBuffer[nIndex++]=0x46;
	bBuffer[nIndex++]=0x47;
	bBuffer[nIndex++]=0xF9;
	bBuffer[nIndex++]=0x64;
	bBuffer[nIndex++]=0x75;
	bBuffer[nIndex++]=0x6D;
	bBuffer[nIndex++]=0x70;
	bBuffer[nIndex++]=0x20;
	bBuffer[nIndex++]=0x30;
	bBuffer[nIndex++]=0x78;
	bBuffer[nIndex++]=0x34;
	bBuffer[nIndex++]=0x30;
	bBuffer[nIndex++]=0x30;
	bBuffer[nIndex++]=0x37;
	bBuffer[nIndex++]=0x66;
	bBuffer[nIndex++]=0x30;
	bBuffer[nIndex++]=0x30;
	bBuffer[nIndex++]=0x30;
	bBuffer[nIndex++]=0x20;
	bBuffer[nIndex++]=0x31;
	bBuffer[nIndex++]=0x0D;
	bBuffer[nIndex++]=0x0A;
	bBuffer[nIndex++]=0x20;
	bBuffer[nIndex++]=0xAA;
	dwLength = nIndex;
	if(!WriteBuffer((char*)bBuffer,dwLength))
	{
		m_strLogInfo = "Send dump data command failed";
		return FALSE;			
	}
	Sleep(50);
	ZeroMemory(chBuff,600);
	if(!ReadBuffer(chBuff,600,&dwLeng))
	{
		m_strLogInfo = "dump data接收指令返回内容错误";
		return FALSE;			
	}
	*pReBuff = &chBuff[0];
	*dwSize = dwLeng;
	return TRUE;
}

BOOL Z4671Command::GetModuleState(int *nReady)
{
	char  chSendBuf[256];
	char  chReadBuf[256];
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[256];
	
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
//	double dblValue;
	int   nCmdIndex=0;
	
	byData[nCmdIndex++] = START_CMD;
	byData[nCmdIndex++] = 0XC0;
	byData[nCmdIndex++] = 0X00; //Index
	byData[nCmdIndex++] = 0X00;
	byData[nCmdIndex++] = 0X00; 
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[nCmdIndex++] = nCheckSum-1;
	byData[nCmdIndex++] = END_CMD;
	WORD nCmdLength = nCmdIndex;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = "发送数据错误";
		return FALSE;
	}
	Sleep(10);
	BYTE  byGetData[256];
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = "发送数据错误";
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	CString strMsg;
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	if (pbyNewData[5]!=byData[1]) 
	{
		return FALSE;
	}
	*nReady = pbyNewData[6];
	return TRUE;

}

BOOL Z4671Command::SendScanDoubleTrig(stSWScanPara *stScanPara)
{
	CString strINIFile;
	CString strValue;
	CString strCommand;
	char    chCommand[256];
	char    chRead[20];
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));

	BYTE  byData[256];
	BYTE  byGetData[256];
	int   i;
	int   nLength;
	int   nCheckSum;
	char  chValue[10];
	int   nSwitchIndex[2];
	CString strMsg;
	int   nIndex=0;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	
	nIndex = 0;
	byData[0] = START_CMD;
	nIndex++;
	byData[1] = 0XF0;
	nIndex++;
	byData[2] = 0X00; //Index
	nIndex++;
	byData[3] = 0X00;
	nIndex++;
	byData[4] = 0X1E;
	nIndex++;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
		nIndex++;
	}
	byData[12] = 0XE3;
	nIndex++;

	int n = 0;
    for ( n=0;n<2;n++)
	{
		if (stScanPara[n].nBlock==0)
		{
			nSwitchIndex[n] = stScanPara[n].nSwitch+33;
		}
		else if (stScanPara[n].nBlock==1) 
		{
			nSwitchIndex[n] = stScanPara[n].nSwitch+17;
		}
		else if (stScanPara[n].nBlock==2) 
		{
			if (stScanPara[n].nSwitch==0)
			{
				nSwitchIndex[n] = 50;
			}
			else if (stScanPara[n].nSwitch==1)
			{
				nSwitchIndex[n] = 49;
			}
		}
		byData[13+n*11] = (BYTE)nSwitchIndex[n];
		nIndex++;
		byData[14+n*11] = (BYTE)(stScanPara[n].nStartX>>8);
		nIndex++;
		byData[15+n*11] = (BYTE)stScanPara[n].nStartX;
		nIndex++;
		byData[16+n*11] = (BYTE)(stScanPara[n].nEndX>>8);
		nIndex++;
		byData[17+n*11] = (BYTE)stScanPara[n].nEndX;
		nIndex++;
		byData[18+n*11] = (BYTE)(stScanPara[n].nStartY>>8);
		nIndex++;
		byData[19+n*11] = (BYTE)stScanPara[n].nStartY;
		nIndex++;
		byData[20+n*11] = (BYTE)(stScanPara[n].nEndY>>8);
		nIndex++;
		byData[21+n*11] = (BYTE)stScanPara[n].nEndY;
		nIndex++;
		byData[22+n*11] = (BYTE)stScanPara[n].nStep>>8;
		nIndex++;
		byData[23+n*11] = (BYTE)stScanPara[n].nStep;
		nIndex++;
    }
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[35] = nCheckSum-1;
	nIndex++;
	byData[36] = END_CMD;
	nIndex++;
	WORD nCmdLength = 37;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,30,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}		
	return TRUE;

}

BOOL Z4671Command::SendCmdToEDFA(const CStringA& strCmd, char *pchReStr)
{
	CStringA strValue;
	char    chCommand[256];
	char    chRead[1024];

	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));

	BYTE  byData[256];
	BYTE  byGetData[256];
	int   i;
	int   nLength;
	int   nCheckSum;
	char  chValue[1024];
	CString strMsg;
	int   nIndex=0;
	ZeroMemory(chValue,sizeof(chValue));
	strValue = "OPLKMFG";
	strncpy(chValue, strValue.GetString(), strValue.GetLength());
	
	byData[nIndex++] = START_CMD;
	byData[nIndex++] = 0XF0;
	byData[nIndex++] = 0X02; //Index
	byData[nIndex++] = 0X00;
	byData[nIndex++] = strCmd.GetLength()+10;
	for (i=5;i<12;i++)
	{
		byData[nIndex++] = chValue[i-5];
	}
	byData[nIndex++] = 0XF9;

	strncpy(chValue, strCmd.GetString(), strCmd.GetLength());

	int n = 0;
    for ( n=0;n<strCmd.GetLength();n++)
	{
		byData[nIndex++] = (BYTE)chValue[n];
    }
	byData[nIndex++] = 0X0D;
	byData[nIndex++] = 0X0A;
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[nIndex++] = nCheckSum-1;
	byData[nIndex++] = END_CMD;
	WORD nCmdLength = nIndex;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,1024,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	nLength = pbyNewData[3]*256+pbyNewData[4];
	memcpy(pchReStr,&pbyNewData[6],nLength);
	return TRUE;

}

BOOL Z4671Command::ReadFPGA(int nChannel, DWORD dwAddress, char *pchReStr)
{
	CStringA strCmd;
	strCmd.Format("rfpga %d 0x%X",nChannel+1,dwAddress);
	char   chRead[1024];
	ZeroMemory(chRead,sizeof(chRead));	
	if(!SendCmdToEDFA(strCmd,pchReStr))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Z4671Command::GetALLSwitch(int nBlock, int *pPos)
{
	CString strINIFile;
	CString strValue;
	CString strCommand;
	char    chCommand[256];
	char    chRead[20];
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));

	BYTE  byData[256];
	BYTE  byGetData[256];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strMsg;
	int   nIndex=0;

	byData[nIndex++] = START_CMD;
	byData[nIndex++] = 0XD5;
	byData[nIndex++] = 0X00; //Index
	byData[nIndex++] = 0X00;
	byData[nIndex++] = 0X01;
	byData[nIndex++] = nBlock; //0/1
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[nIndex++] = nCheckSum-1;
	byData[nIndex++] = END_CMD;
	WORD nCmdLength = nIndex;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(10);
	if (!ReadBuffer((char*)byGetData,30,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	int n = 0;
	for ( n=0;n<16;n++)
	{
		pPos[n] = pbyNewData[6+n];
	}
	return TRUE;

}

BOOL Z4671Command::GetMONSwitch(int nSwitch, int *pPos)
{
	CString strINIFile;
	CString strValue;
	CString strCommand;
	char    chCommand[256];
	char    chRead[20];
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));

	BYTE  byData[256];
	BYTE  byGetData[256];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strMsg;
	int   nIndex=0;

	byData[nIndex++] = START_CMD;
	byData[nIndex++] = CMD_HW_GET_MONT_MONR_SWITCH_POSITION;
	byData[nIndex++] = 0X00; //Index
	byData[nIndex++] = 0X00;
	byData[nIndex++] = 0X01;
	if (nSwitch==1)
	{
		byData[nIndex++] = 50; //49/50
	}
	else
	{
		byData[nIndex++] = 49; //49/50
	}
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[nIndex++] = nCheckSum-1;
	byData[nIndex++] = END_CMD;
	WORD nCmdLength = nIndex;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(10);
	if (!ReadBuffer((char*)byGetData,30,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	
	int nPos = pbyNewData[6];
	*pPos = nPos;
	return TRUE;

}

BOOL Z4671Command::VOAVerify(int nStartIndex, int nEndIdnex, int nDACRange, int nStep, int nSleepTime)
{
	CString strINIFile;
	CString strValue;
	CString strCommand;
	char    chCommand[256];
	char    chRead[20];
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));

	BYTE  byData[256];
//	BYTE  byGetData[256];
	int   i;
	int   nLength;
	int   nCheckSum;
	char  chValue[10];
	CString strMsg;
	int   nIndex=0;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	
	byData[nIndex++] = START_CMD;
	byData[nIndex++] = 0XF0;
	byData[nIndex++] = 0X00; //Index
	byData[nIndex++] = 0X00;
	byData[nIndex++] = 0X10;
	for (i=5;i<12;i++)
	{
		byData[nIndex++] = chValue[i-5];
	}
	byData[nIndex++] = 0XE5;
	byData[nIndex++] = nStartIndex+17;
	byData[nIndex++] = nEndIdnex+17;
	byData[nIndex++] = (nDACRange>>8);
	byData[nIndex++] = nDACRange;
	byData[nIndex++] = (nSleepTime>>8);
	byData[nIndex++] = nSleepTime;
    byData[nIndex++] = (nStep>>8);
	byData[nIndex++] = nStep;
    
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[nIndex++] = nCheckSum-1;
	byData[nIndex++] = END_CMD;
	WORD nCmdLength = nIndex;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	return TRUE;

}

BOOL Z4671Command::VOACalibrate(int nStartIndex, int nEndIdnex, int nDACRange, int nStep, int nSleepTime)
{
	CString strINIFile;
	CString strValue;
	CString strCommand;
	char    chCommand[256];
	char    chRead[20];
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));

	BYTE  byData[256];
	int   i;
	int   nLength;
	int   nCheckSum;
	char  chValue[10];
	CString strMsg;
	int   nIndex=0;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	
	byData[nIndex++] = START_CMD;
	byData[nIndex++] = 0XF0;
	byData[nIndex++] = 0X00; //Index
	byData[nIndex++] = 0X00;
	byData[nIndex++] = 0X10;
	for (i=5;i<12;i++)
	{
		byData[nIndex++] = chValue[i-5];
	}
	byData[nIndex++] = 0XE4;
	byData[nIndex++] = nStartIndex+17;
	byData[nIndex++] = nEndIdnex+17;
	byData[nIndex++] = (nDACRange>>8);
	byData[nIndex++] = nDACRange;
	byData[nIndex++] = (nSleepTime>>8);
	byData[nIndex++] = nSleepTime;
    byData[nIndex++] = (nStep>>8);
	byData[nIndex++] = nStep;
    
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[nIndex++] = nCheckSum-1;
	byData[nIndex++] = END_CMD;
	WORD nCmdLength = nIndex;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	return TRUE;
}

BOOL Z4671Command::GetMCSTemp(double *dblTemp)
{
	double Temp=0;
	CString strCommand;
	CString strMsg;
	CString strToken;
	BYTE   byData[30];
	int    nLength;
	int    nCheckSum;
	BYTE   byGetData[256];
	int    i;
	char  chValue[10];
	CString strValue;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	
	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X08;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XF7;
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[13] = nCheckSum-1;
	byData[14] = END_CMD;
	DWORD nCmdLength = 15;
	PBYTE pSend=NULL;

	CmdSendExchange(byData,nCmdLength,&pSend,(PWORD)&nCmdLength);
	
	if(!WriteBuffer((char*)pSend,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("读取温度接收错误");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewGetData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewGetData))
	{
		return FALSE;
	}
	if (pbyNewGetData[1]!=0)
	{
		//m_strLogInfo.Format("状态错误0X%02X",pbyNewGetData[1]);
		return FALSE;
	}
	if (pbyNewGetData[2]!=byData[2])
	{
		//m_strLogInfo.Format("序号错误0X%02X",pbyNewGetData[2]);
		return FALSE;
	}
	SHORT sTemp;
	BYTE* pch = (BYTE*)&sTemp;
	pch[0] = pbyNewGetData[7];
	pch[1] = pbyNewGetData[6];
	Temp = (double)sTemp/10.0;
	*dblTemp = Temp;
	
	return TRUE;

}

BOOL Z4671Command::GetEDFATemp(double *dblTemp)
{
	double dblPower = -99;
	char  chSendBuf[50];
	char  chReadBuf[256];
	//double dblPDPower;
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[50];
	BYTE  byGetData[50];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	CString strMsg;

	byData[0] = START_CMD;
	byData[1] = 0XB2;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X01;
	byData[5] = 0X00;

	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[6] = nCheckSum-1;
	byData[7] = END_CMD;
	WORD nCmdLength = 8;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,50,&nCmdLength))
	{
		m_strLogInfo = ("读取EDFA温度接收错误");
		return FALSE;
	}
	
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		//m_strLogInfo.Format("状态错误0X%02X",pbyNewData[1]);
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		//m_strLogInfo.Format("序号错误0X%02X",pbyNewData[2]);
		return FALSE;
	}
	
	DWORD dwCurrent;
	dwCurrent = (pbyNewData[7]<<24)+(pbyNewData[8]<<16)+(pbyNewData[9]<<8)+pbyNewData[10];
	SHORT sTemp;
	sTemp = (SHORT)dwCurrent;
	double dblCurrent = (double)sTemp/10.0;		
	*dblTemp = dblCurrent;
	return TRUE;

}

BOOL Z4671Command::GetProductSN(char *pchSN)
{
	CString strCommand;
	CString strMsg;
	CString strToken;

	BYTE  byData[10];
	BYTE  byGetData[256];
	int   nLength;
	int   nCheckSum;
	int   i;
	
	byData[0] = START_CMD;
	byData[1] = 0XA2;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X00;
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[5] = nCheckSum-1;
	byData[6] = END_CMD;
	WORD nCmdLength = 7;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(20);
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if (!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		//m_strLogInfo.Format("状态错误0X%02X",pbyNewData[1]);
		if (pbyNewData[1]==0X09)
		{
			strToken = "NULL"; //表示该产品SN号还未写入设备
			strcpy(pchSN,strToken);
			//*strSN = strToken;
			return TRUE;
		}
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		//m_strLogInfo.Format("序号错误0X%02X",pbyNewData[2]);
		return FALSE;
	}
	BYTE chASCII[32];
	ZeroMemory(chASCII,sizeof(chASCII));
	for (i=0;i<32;i++)
	{
		chASCII[i] = byGetData[i+6];
	}
	strToken = strtok((char*)chASCII," ");
	strcpy(pchSN,strToken);
	//*strSN = strToken;
	m_stModuleInfo.strSN = strToken;
	return TRUE;

}

BOOL Z4671Command::GetProductPN(char *pchPN)
{
	CString strCommand;
	CString strMsg;
	CString strToken;

	BYTE  byData[10];
	BYTE  byGetData[256];
	int   nLength;
	int   nCheckSum;
	int   i;
	
	byData[0] = START_CMD;
	byData[1] = 0XA5;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X00;
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[5] = nCheckSum-1;
	byData[6] = END_CMD;
	WORD nCmdLength = 7;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(20);
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if (!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		//m_strLogInfo.Format("状态错误0X%02X",pbyNewData[1]);
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		//m_strLogInfo.Format("序号错误0X%02X",pbyNewData[2]);
		return FALSE;
	}
	BYTE chASCII[32];
	ZeroMemory(chASCII,sizeof(chASCII));
	for (i=0;i<32;i++)
	{
		chASCII[i] = byGetData[i+6];
	}
	strToken = strtok((char*)chASCII," ");
	//*strPN = strToken;
	strcpy(pchPN,strToken);
	m_stModuleInfo.strPN = strToken;
	return TRUE;

}

BOOL Z4671Command::CloseSwitchMonitor(int nSwitchIndex)
{
	CString strTemp;
	char chData[20];
	char chReData[40];
	CString strValue;
	CString strInfo;
	BYTE  byData[20];
	BYTE  byGetData[10];
	int   i;
	int   nLength;
	int   nCheckSum;
	WORD nCmdLength = 8;
	PBYTE pbySendData=NULL;
	int n=1;
//	SHORT sValue;
	int nIndex;
	nIndex=17+nSwitchIndex;//(17~32)
	ZeroMemory(chData,sizeof(chData));
	ZeroMemory(chReData,sizeof(chReData));
	byData[0] = START_CMD;
	byData[1] = 0XCB;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X02;
	byData[5] = nIndex;
	//发送的数据是16bit整数格式 单位为0.1dB
//	sValue = dblAtten*10;
	byData[6] = 0x00;

	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[7] = nCheckSum-1;
	byData[8] = END_CMD;
	nCmdLength = 9;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(20);
	if (!ReadBuffer((char*)byGetData,10,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	CString strMsg;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		//LogInfo("Index序号不一样");
		return FALSE;
	}
	return TRUE;

}

BOOL Z4671Command::GetTesterInfo()
{
	static const char s_crlf[] = "\r\n";
	static const char s_info[] = "info\r\n";
	DWORD    nLength;
	char   chGetData[256];

	if(!WriteBuffer((char*)s_crlf,2))
	{
		m_strLogInfo = ("发送回车符错误");
		return FALSE;
	}
	Sleep(100);
	ZeroMemory(chGetData,sizeof(chGetData));
	//LogInfo("测试板信息:");
	if(!WriteBuffer((char*)s_info,(DWORD)strlen(s_info)))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(chGetData,256,&nLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	m_strLogInfo = (LPCTSTR)chGetData;
	return TRUE;

}

BOOL Z4671Command::GetVersion(char *pchVer)
{
	CString strCommand;
	CString strMsg;
	CString strToken;
	CString strConfig;
	BYTE   byData[256];
	int    nLength;
	int    nCheckSum;
	BYTE   byGetData[256];
	int    i;
	
	byData[0] = START_CMD;
	byData[1] = 0XA3;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X00;
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[5] = nCheckSum-1;
	byData[6] = END_CMD;
	WORD nCmdLength = 7;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(10);
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	pbySendData = NULL;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}

	char chHWASCII[8];
	char chFWASCII[8];
	CString strHW;
	CString strFW;
	ZeroMemory(chHWASCII,sizeof(chHWASCII));
	ZeroMemory(chFWASCII,sizeof(chFWASCII));
	for (i=6;i<14;i++)
	{
		if (pbyNewData[i]==0X20)
		{
			break;
		}
		chHWASCII[i-6] = pbyNewData[i];
	}
	strToken = strtok(chHWASCII," ");
	strHW.Format("HW Version:%s\n",strToken);
	m_stModuleInfo.strHWVer = strToken;

	for ( i=14;i<22;i++)
	{
		if (pbyNewData[i]==0X20)
		{
			break;
		}
		chFWASCII[i-14] = pbyNewData[i];
	}
	strToken = strtok(chFWASCII," ");
	m_stModuleInfo.strFWVer = strToken;
	strFW.Format("FW Version:%s",strToken);
	strHW = strHW + strFW;
	strcpy(pchVer,strHW);
	//*pstrVer = strHW;
	return TRUE;

}

BOOL Z4671Command::GetMCSVersion(char *pchMCSVer)
{
	BYTE  byData[100];
	BYTE  byGetData[100];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	CString strToken;
	CString strToken1;
	char  chValue[10];
	CString strMsg;
	WORD nCmdLength;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	
	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X08;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XF0;
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	//nCheckSum = byData[1];
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[13] = nCheckSum-1;
	byData[14] = END_CMD;
	nCmdLength = 15;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(20);
	if (!ReadBuffer((char*)byGetData,100,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	char chVersion[16];
	CString strVer;
	ZeroMemory(chVersion,sizeof(chVersion));
	memcpy(chVersion,&pbyNewData[6],16);
	strToken.Format("%s",chVersion);
	
	strToken1 = strToken;	
	strToken1 = strToken1.Left(8);
	strToken1.TrimLeft();
	strToken1.TrimRight();
	strVer.Format("MCS Main Version: %s\n",strToken1);
	m_stModuleInfo.strMainVer = strToken1;

	strToken1 = strToken.Left(16);
	strToken1 = strToken1.Right(8);
	strToken1.TrimLeft();
	strToken1.TrimRight();
	strMsg.Format("MCS Boot Version: %s",strToken1);
	m_stModuleInfo.strBootVer = strToken1;
	strVer = strVer + strMsg;
	//*pstrMCSVer = strVer;
	strcpy(pchMCSVer,strVer);
	
	return TRUE;

}

BOOL Z4671Command::GetEDFAInfo(char *pchEDFAVer)
{
	static const char kOplkHdr[] = "OPLKMFG";
	static const char kPrivInfo[] = "privinfo\r\n";
	const int nOplk = (int)strlen(kOplkHdr);
	const int nPriv = (int)strlen(kPrivInfo);

	BYTE  byData[100];
	BYTE  byGetData[4096];
	int   i;
	int   nLength;
	int   nCheckSum;
	char  chValue[4096];
	CString strMsg;
	WORD nCmdLength;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue, kOplkHdr, nOplk);
	
	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = (BYTE)(nPriv+8);
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XF9;
	for (i=0;i<nPriv;i++)
	{
		byData[13+i] = (BYTE)kPrivInfo[i];
	}

	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;

	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[13+nPriv] = nCheckSum-1;
	byData[13+nPriv+1] = END_CMD;
	nCmdLength = 15+nPriv;

	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,4096,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	//将字节转存到pbyNewData
    PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
  
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}

	char chEDFAInfo[4096];
	CString strToken;
	CString strConfig;
	int   nFlag=0;
	ZeroMemory(chEDFAInfo,sizeof(chEDFAInfo));
	memcpy(chEDFAInfo,&pbyNewData[6],4096);	
	strMsg.Format("EDFA Info:");
	//Module Name
	strToken = strtok(chEDFAInfo,"\r\n");
	m_stModuleInfo.strEDFAName = strToken;
    strMsg = strMsg+strToken;
	//FW Version
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFAFWVer = strToken;
	strMsg = strMsg+strToken;
	//HW Version
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFAHWVer = strToken;
	strMsg = strMsg+strToken;

	nFlag = strToken.Find(":");
	strToken.Delete(0,nFlag+2);

	//SN
	strToken = strtok(NULL,"\r\n");
	strMsg = strMsg+strToken;
	
	//Date
	strToken = strtok(NULL,"\r\n");
	strMsg = strMsg+strToken;
	
	//MFG Name
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFAMFGName = strToken;
	strMsg = strMsg+strToken;

	//PN
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFAPN = strToken;
	strMsg = strMsg+strToken;

	//Cal Date
	strToken = strtok(NULL,"\r\n");
	strMsg = strMsg+strToken;
	
	//PCB Version
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFAPCBVer = strToken;
	strMsg = strMsg+strToken;

	
	//PCBA Version
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFAPCBAVer = strToken;
	strMsg = strMsg+strToken;
	
	//Bundle Version
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFABundleVer = strToken;
	strMsg = strMsg+strToken;

	//Boot Version
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFABootVer = strToken;
	strMsg = strMsg+strToken;

	//Main Version
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFAMainVer = strToken;
	strMsg = strMsg+strToken;
	
	//FPGA Version
	strToken = strtok(NULL,"\r\n");
	m_stModuleInfo.strEDFAFPGAVer = strToken;
	strMsg = strMsg+strToken;
	strcpy(pchEDFAVer,strToken);
	//*pstrEDFAVer = strMsg;
	
	return TRUE;

}

BOOL Z4671Command::SwitchALLSwitch(int nDrop, int *nPort, int nCount)
{
	CString strTemp;
	char chData[20];
	char chReData[40];
	CString strValue;
	CString strInfo;
	int   i;
	int   nLength;
	int   nCheckSum;
	ZeroMemory(chData,sizeof(chData));
	ZeroMemory(chReData,sizeof(chReData));
	
	BYTE  byData[100];
	BYTE  byGetData[100];
	char  chValue[10];
	WORD   nCmdLength;
	PBYTE pbySendData=NULL;
	CString strMsg;
	ZeroMemory(chValue,sizeof(chValue));
	
	byData[0] = START_CMD;
	byData[1] = 0XD4;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X11;
	if (nDrop==1)
	{
		byData[5] = 1; //drop
	}
	else
	{
		byData[5] = 0;
	}
	for (i=6;i<22;i++)
	{
		byData[i] = nPort[i-6];
	}		
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[22] = nCheckSum-1;
	byData[23] = END_CMD;
	nCmdLength = 24;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
				
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(100);
	if (!ReadBuffer((char*)byGetData,100,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		//LogInfo("Index序号不一样");
		return FALSE;
	}
	return TRUE;
}

BOOL Z4671Command::SendCmd()
{
	/*
	BYTE  pbyData[MAX_COUNT];
	WORD   nCmdLength;
	int   nCmd = 0;
	int   nCheckSum = 0;

	pbyData[nCmd++] = START_CMD;
	pbyData[nCmd++] = m_stCmd.byCmd;
	pbyData[nCmd++] = m_stCmd.wCmdIndex;
	pbyData[nCmd++] = m_stCmd.wLength;
	for (int i=0;i<m_stCmd.wLength;i++)
	{
		pbyData[nCmd++] = m_stCmd.byData[i];
	}
	
	for (i=1;i<=m_stCmd.wLength;i++)
	{
		nCheckSum = nCheckSum^pbyData[i];
	}
	pbyData[nCmd++] = nCheckSum-1;
	pbyData[nCmd++] = END_CMD;
	PBYTE pbySendData=NULL;
//	pbySendData = &m_pbySendData[0];
	
	CmdSendExchange(pbyData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("发送数据错误");
		return FALSE;
	}
	m_stCmd.wCmdIndex++;

  */
	return TRUE;
}

//nBlock:1~3;
//nSwitch:1~16;
//nPort:1~8


BOOL Z4671Command::SwitchSingleSwitch(int nBlock, int nSwitch, int nPort)
{
	CString strTemp;
	char chData[20];
	char chReData[40];
	CString strValue;
	CString strInfo;
	BYTE  byData[20];
	BYTE  byGetData[10];
	int   i;
	int   nLength;
	int   nCheckSum;
	WORD nCmdLength = 8;
	DWORD dwCmd;
	PBYTE pbySendData=NULL;
	int n=1;
	CString strMsg;
	ZeroMemory(chData,sizeof(chData));
	ZeroMemory(chReData,sizeof(chReData));
	
	if (nBlock==3)
	{
		byData[0] = START_CMD;
		byData[1] = CMD_HW_SET_MONT_MONR_SWITCH_POSITION;
		byData[2] = 0X00; //Index
		byData[3] = 0X00;
		byData[4] = 0X02;
		if (nSwitch==2)  //mux
		{
			byData[5] = MONO;
		}
		else if(nSwitch==1)  //deMUX
		{
			byData[5] = MONI;
		}
		else
		{
			return FALSE;
		}
		byData[6] = nPort;

		nLength = byData[3]*256+byData[4]+4;
		nCheckSum = 0;
		for (i=1;i<=nLength;i++)
		{
			nCheckSum = nCheckSum^byData[i];
		}
		byData[7] = nCheckSum-1;
		byData[8] = END_CMD;
		nCmdLength = 9;
		n=1;
		CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	}
	else
	{
		byData[0] = START_CMD;
		byData[1] = CMD_HW_SET_SWITCH_POSITION;
		byData[2] = 0X00; //Index
		byData[3] = 0X00;
		byData[4] = 0X02;
		if (nBlock==2)  //AM
		{
			byData[5] = AM1+nSwitch-1;
		}
		else if(nBlock==1)  //DM
		{
			byData[5] = DM1+nSwitch-1;
		}
		byData[6] = nPort;

		nLength = byData[3]*256+byData[4]+4;
		nCheckSum = 0;
		for (i=1;i<=nLength;i++)
		{
			nCheckSum = nCheckSum^byData[i];
		}
		byData[7] = nCheckSum-1;
		byData[8] = END_CMD;
		nCmdLength = 9;
		CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	}
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,10,(PDWORD)&dwCmd))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<dwCmd;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pNewData=NULL;

	if(!CmdReadExchange(byGetData, dwCmd,&pNewData))
	{
		return FALSE;
	}
	if (pNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pNewData[2]!=byData[2])
	{
		return FALSE;
	}
	return TRUE;
}

BOOL Z4671Command::GetSingleSwitchState(int nBlock, int nSwitch, int *pPos)
{
	CString strINIFile;
	CString strValue;
	CString strCommand;
	char    chCommand[256];
	char    chRead[20];
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));

	BYTE  byData[256];
	BYTE  byGetData[256];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strMsg;
	int   nIndex=0;

	byData[nIndex++] = START_CMD;
	byData[nIndex++] = CMD_HW_GET_SWITCH_POSITION;
	byData[nIndex++] = 0X00; //Index
	byData[nIndex++] = 0X00;
	byData[nIndex++] = 0X01;
	if (nBlock==2) //DM
	{
		byData[nIndex++] = DM1+nSwitch-1; 
	}
	else if (nBlock==1) //AM
	{
		byData[nIndex++] = AM1+nSwitch-1; 
	}
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[nIndex++] = nCheckSum-1;
	byData[nIndex++] = END_CMD;
	WORD nCmdLength = nIndex;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(10);
	if (!ReadBuffer((char*)byGetData,30,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}

	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	
	int nPos = pbyNewData[6];
	*pPos = nPos;
	return TRUE;

}

BOOL Z4671Command::SendScanTrig(stSWScanPara stScanPara)
{
	BYTE  byData[30];
	BYTE  byGetData[30];
	int   i;
	int   nLength;
	int   nCheckSum;
//	CString strValue;
	char  chValue[10];
	int   nSwitchIndex;
	CString strMsg;
	CString strValue;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	int nBlock = stScanPara.nBlock;
	int nSwitch = stScanPara.nSwitch;
	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X13;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XF5;
	if (nBlock==0)
	{
		nSwitchIndex = nSwitch+33;
	}
	else if (nBlock==1) 
	{
		nSwitchIndex = nSwitch+17;
	}
	else if (nBlock==2) 
	{
		if (nSwitch==0)
		{
			nSwitchIndex = 50;
		}
		else if (nSwitch==1)
		{
			nSwitchIndex = 49;
		}
	}	
	byData[13] = (BYTE)nSwitchIndex;
	byData[14] = (BYTE)(stScanPara.nStartX>>8);
	byData[15] = (BYTE)stScanPara.nStartX;
	byData[16] = (BYTE)(stScanPara.nEndX>>8);
	byData[17] = (BYTE)stScanPara.nEndX;
	byData[18] = (BYTE)(stScanPara.nStartY>>8);
	byData[19] = (BYTE)stScanPara.nStartY;
	byData[20] = (BYTE)(stScanPara.nEndY>>8);
	byData[21] = (BYTE)stScanPara.nEndY;
	byData[22] = (BYTE)(stScanPara.nStep>>8);
	byData[23] = (BYTE)stScanPara.nStep;

	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[24] = nCheckSum-1;
	byData[25] = END_CMD;
	WORD nCmdLength = 26;
	PBYTE pbySendData=NULL;
	DWORD dwCmd=0;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,30,(PDWORD)&dwCmd))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<dwCmd;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData, dwCmd,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}		
	return TRUE;
}
//0为X
BOOL Z4671Command::SetSwitchDAC(int nBlock, int nSwitch, int nDac, int nXY)
{
	CString strCommand;
	BYTE  byData[20];
	BYTE  byGetData[30];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	char  chValue[10];
	int   nSwitchIndex;
	CString strMsg;
	ZeroMemory(chValue,sizeof(chValue));
	ZeroMemory(byData,sizeof(byData));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());	
	int error = 0;
	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X0C;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XF3;
	if (nBlock==1)
	{
		nSwitchIndex = nSwitch+32;	
	}
	else if (nBlock==2)
	{
		nSwitchIndex = nSwitch+16;
	}
	else if (nBlock==3)
	{
		if (nSwitch==1)
		{
			nSwitchIndex = MONI;
		}
		else
		{
			nSwitchIndex = MONO;
		}
	}		
	byData[13] = (BYTE)nSwitchIndex;
	byData[14] =  nXY; //X AXIS
	byData[15] = (BYTE)(nDac>>8); //X AXIS
	byData[16] = (BYTE)nDac; //X AXIS
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[17] = nCheckSum-1;
	byData[18] = END_CMD;
	WORD nCmdLength = 19;
	DWORD dwCmd=0;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	loop:	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(100);
	if (!ReadBuffer((char*)byGetData,30,(PDWORD)&dwCmd))
	{
		m_strLogInfo = ("接收错误！");
		error++;
		if (error > 10)
		{
			return FALSE;
		}
		goto loop;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<dwCmd;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
    if(!CmdReadExchange(byGetData, dwCmd,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}		
	return TRUE;
}

BOOL Z4671Command::SendHitlessTestCmd(int nBlock, int nSwitch, int nCHNum)
{
	CString strINIFile;
	CString strValue;
	CString strCommand;
	char    chCommand[256];
	char    chRead[200];
	
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chRead,sizeof(chRead));
	BYTE  byData[30];
	BYTE  byGetData[30];
	int   i;
	int   nLength;
	int   nCheckSum;
//	CString strValue;
	char  chValue[10];
	int   nSwitchIndex;
	CString strMsg;
	DWORD dwLength = 65000;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());

	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X0E;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XFB;
	if (nBlock==0)
	{
		nSwitchIndex = nSwitch+33;
	}
	else if (nBlock==1) 
	{
		nSwitchIndex = nSwitch+17;
	}
	else if (nBlock==2) 
	{
		if (nSwitch==0)
		{
			nSwitchIndex = 50;
		}
		else if (nSwitch==1)
		{
			nSwitchIndex = 49;
		}
	}	
	byData[13] = (BYTE)nSwitchIndex;
	byData[14] = (BYTE)nCHNum;
	byData[15] = (BYTE)(dwLength>>24);
	byData[16] = (BYTE)(dwLength>>16);
	byData[17] = (BYTE)(dwLength>>8);
	byData[18] = (BYTE)(dwLength);

	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
 	byData[19] = nCheckSum-1;
	byData[20] = END_CMD;
	WORD nCmdLength = 21;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,30,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (byGetData[1]!=0X00)
	{
		return FALSE;
	}
	if (byGetData[2]!=byData[2])
	{
		return FALSE;
	}		
	return TRUE;
}

BOOL Z4671Command::StartFWUpdate()
{
	BOOL ok = FALSE;
	(void)M576WithRetry(M576_COMM_RETRY_MAX_ATTEMPTS, (DWORD)M576_COMM_RETRY_DELAY_MS, [&]() -> bool {
		ok = StartFWUpdateNoRetry();
		return ok;
	});
	return ok;
}

BOOL Z4671Command::StartFWUpdateNoRetry()
{
	BYTE byData[256];
	int  nCheckSum;
	int  nLength;
	BYTE byGetData[256];
	int  i;
	char chData[20];

	ZeroMemory(chData,sizeof(chData));
	strcpy_s(chData, "STARTUPDATE");

	byData[0] = START_CMD;
	byData[1] = 0XE0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X0B;
	byData[5] = (BYTE)chData[0];
	byData[6] = (BYTE)chData[1];
	byData[7] = (BYTE)chData[2];
	byData[8] = (BYTE)chData[3];
	byData[9] = (BYTE)chData[4];
	byData[10] = (BYTE)chData[5];
	byData[11] = (BYTE)chData[6];
	byData[12] = (BYTE)chData[7];
	byData[13] = (BYTE)chData[8];
	byData[14] = (BYTE)chData[9];
	byData[15] = (BYTE)chData[10];
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[16] = nCheckSum-1;
	byData[17] = END_CMD;
	WORD nCmdLength = 18;
	PBYTE pbySendData=NULL;
	DWORD dwCmd;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,200,(PDWORD)&dwCmd))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	CString strMsg;
	strReadData = "";
	for (nCmd=0;nCmd<dwCmd;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	//CmdReadExchange(byGetData,nCmdLength,&pbyNewData);
	if(!CmdReadExchange(byGetData, dwCmd,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	return TRUE;
}

BOOL Z4671Command::FWTranSportFW(BYTE *byTransData, int nDataLength, int nIndex, int nSum)
{
	BOOL ok = FALSE;
	(void)M576WithRetry(M576_COMM_RETRY_MAX_ATTEMPTS, (DWORD)M576_COMM_RETRY_DELAY_MS, [&]() -> bool {
		ok = FWTranSportFWNoRetry(byTransData, nDataLength, nIndex, nSum);
		return ok;
	});
	return ok;
}

BOOL Z4671Command::FWTranSportFWNoRetry(BYTE *byTransData, int nDataLength, int nIndex, int nSum)
{
	BYTE byData[500];
	int  nCheckSum;
	int  nLength;
	BYTE byGetData[256];
	int  i;
	char chData[20];
	CString strData;
	
	ZeroMemory(chData,sizeof(chData));
	if (nDataLength>403)
	{
		m_strLogInfo = ("数据长度超长");
		return FALSE;
	}
	
	nLength = nDataLength+12;
	byData[0] = START_CMD;
	byData[1] = 0XE1;
	byData[2] = nIndex; //Index
	byData[3] = (BYTE)(nLength>>8);
	byData[4] = (BYTE)nLength;
	byData[5] = (BYTE)(nSum>>24);  //总包数
	byData[6] = (BYTE)(nSum>>16); 
	byData[7] = (BYTE)(nSum>>8);  //总包数
	byData[8] = (BYTE)nSum; 
	byData[9] = (BYTE)(nIndex>>24);
	byData[10] = (BYTE)(nIndex>>16);  //当前包
	byData[11] = (BYTE)(nIndex>>8);
	byData[12] = (BYTE)nIndex;  //当前
	byData[13] = (BYTE)(nDataLength>>24);
	byData[14] = (BYTE)(nDataLength>>16);  //当前
	byData[15] = (BYTE)(nDataLength>>8);
	byData[16] = (BYTE)nDataLength;  //当前
	for (i=0;i<nDataLength;i++)
	{
		byData[17+i] = byTransData[i];
	}
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[17+nDataLength] = nCheckSum-1;
	byData[17+nDataLength+1] = END_CMD;
	WORD nCmdLength = 17+nDataLength+2;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	int nCount=0;
	BOOL bBreak;
	DWORD dwCmd;
	int error = 0;
	while (1)
	{
		bBreak = TRUE;
loopl:	if(!WriteBuffer((char*)pbySendData,nCmdLength))
		{
			m_strLogInfo = ("串口打开错误！");
			return FALSE;
		}
 
		if (!ReadBuffer((char*)byGetData,20,(PDWORD)&dwCmd))
		{
			m_strLogInfo = ("接收错误！");
			error++;
			if (error > 50)
			{
				return FALSE;
			}
			goto loopl;
		
		}
		if (bBreak)
		{
			break;
		}
		Sleep(50);
		nCount++;
		if (nCount>2)
		{
			m_strLogInfo = "传送升级数据包失败";
			return FALSE;
		}
	}

	int   nCmd;
	CString strReadData;
	CString strMsg;
	strReadData = "";
	for (nCmd=0;nCmd<dwCmd;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData, dwCmd,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if(byData[2]==85&&pbyNewData[2]==86&&pbyNewData[3]==86)
	{

	}
	else
	{
		if (pbyNewData[2]!=byData[2])
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL Z4671Command::FWUpdateEnd()
{
	BOOL ok = FALSE;
	(void)M576WithRetry(M576_COMM_RETRY_MAX_ATTEMPTS, (DWORD)M576_COMM_RETRY_DELAY_MS, [&]() -> bool {
		ok = FWUpdateEndNoRetry();
		return ok;
	});
	return ok;
}

BOOL Z4671Command::FWUpdateEndNoRetry()
{
	BYTE byData[256];
	int  nCheckSum;
	int  nLength;
	BYTE byGetData[256];
	int  i;
	char chData[20];
	
	ZeroMemory(chData,sizeof(chData));
	strcpy_s(chData, "UPDATEEND");
	
	byData[0] = START_CMD;
	byData[1] = 0XE2;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X09;
	byData[5] = (BYTE)chData[0];
	byData[6] = (BYTE)chData[1];
	byData[7] = (BYTE)chData[2];
	byData[8] = (BYTE)chData[3];
	byData[9] = (BYTE)chData[4];
	byData[10] = (BYTE)chData[5];
	byData[11] = (BYTE)chData[6];
	byData[12] = (BYTE)chData[7];
	byData[13] = (BYTE)chData[8];

	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[14] = nCheckSum-1;
	byData[15] = END_CMD;
	WORD nCmdLength = 16;
	DWORD dwCmd;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(1000);
	if (!ReadBuffer((char*)byGetData,20,(PDWORD)&dwCmd))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	CString strMsg;
	strReadData = "";
	for (nCmd=0;nCmd<dwCmd;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData, dwCmd,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	return TRUE;

}

BOOL Z4671Command::GetProductID(char *pchID)
{
	CString strCommand;
	CString strMsg;
	CString strToken;
	BYTE   byData[10];
	int    nLength;
	int    nCheckSum;
	BYTE   byGetData[256];
	int    i;
	
	byData[0] = START_CMD;
	byData[1] = 0XA0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X00;
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[5] = nCheckSum-1;
	byData[6] = END_CMD;
	WORD nCmdLength = 7;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
    if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}	
	strToken.Format("0X%02X",pbyNewData[6]);
	//*strID = strToken;
	strcpy(pchID,strToken);
    m_stModuleInfo.strID = strToken;
	return TRUE;

}

BOOL Z4671Command::GetProductSupplier(char *pchSupplier)
{
	CString strCommand;
	CString strMsg;
	CString strToken;
	BYTE   byData[10];
	int    nLength;
	int    nCheckSum;
	BYTE   byGetData[256];
	int    i;
	
	byData[0] = START_CMD;
	byData[1] = 0XA1;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X00;
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[5] = nCheckSum-1;
	byData[6] = END_CMD;
	WORD nCmdLength = 7;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;

	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	BYTE chASCII[16];
	ZeroMemory(chASCII,sizeof(chASCII));
	for (i=0;i<16;i++)
	{
		chASCII[i] = pbyNewData[i+6];
	}
	strToken = strtok((char*)chASCII," ");
	//*strSupplier = strToken;
	strcpy(pchSupplier,strToken);
	m_stModuleInfo.strSupplier = strToken;
	return TRUE;

}

BOOL Z4671Command::GetMCSAlarm()
{
	BYTE  byData[100];
	BYTE  byGetData[100];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	char  chValue[10];
	CString strMsg;
	WORD nCmdLength;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	
	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X08;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XF8;
	
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[13] = nCheckSum-1;
	byData[14] = END_CMD;
	nCmdLength = 15;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,30,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	m_stModuleInfo.byAlarm = pbyNewData[6];
	return TRUE;

}
double Z4671Command::GetTemperature()
{
	double Temp = 0;
	CString strCommand;
	//	char  chCommand[30];
	//	char  chRead[30];
	//	DWORD dwLength;
	CString strMsg;
	CString strToken;
	BYTE   byData[30];
	int    nLength;
	int    nCheckSum;
	BYTE   byGetData[256];
	int    i;
	char  chValue[10];
	CString strValue;
	ZeroMemory(chValue, sizeof(chValue));
	strValue.Format("OPLKMFG");
	memcpy(chValue, strValue, strValue.GetLength());
	{
		byData[0] = START_CMD;
		byData[1] = 0XF0;
		byData[2] = 0X00; //Index
		byData[3] = 0X00;
		byData[4] = 0X08;
		for (i = 5; i<12; i++)
		{
			byData[i] = chValue[i - 5];
		}
		byData[12] = 0XF7;
		nLength = byData[3] * 256 + byData[4] + 4;
		nCheckSum = 0;
		//nCheckSum = byData[1];
		for (i = 1; i <= nLength; i++)
		{
			nCheckSum = nCheckSum^byData[i];
		}
		byData[13] = nCheckSum - 1;
		byData[14] = END_CMD;
		DWORD nCmdLength = 15;
		PBYTE pSend = NULL;

		CmdSendExchange(byData, nCmdLength, &pSend, (PWORD)&nCmdLength);

		if (!WriteBuffer((char*)pSend, nCmdLength))
		{
			return -99;
		}
		Sleep(50);
		if (!ReadBuffer((char*)byGetData, 256, &nCmdLength))
		{
			//strMsg.Format("%s",(CHAR*)&pSend);
			//	LogInfo(strMsg);
			return -99;
		}
		PBYTE pbyNewGetData = NULL;
		//CmdReadExchange(byGetData,nCmdLength,&pbyNewGetData);
		if (!CmdReadExchange(byGetData, nCmdLength, &pbyNewGetData))
		{
			return -99;
		}
		if (byGetData[2] != pbyNewGetData[2])
		{
			return -99;
		}
		SHORT sTemp;
		BYTE* pch = (BYTE*)&sTemp;
		pch[0] = pbyNewGetData[7];
		pch[1] = pbyNewGetData[6];
		Temp = (double)sTemp / 10.0;
	}

	return Temp;
}
//0~15
float Z4671Command::GetPDPower(int nPD)
{
	char  chSendBuf[10];
	char  chReadBuf[256];
	double dblPDPower;
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf, sizeof(chSendBuf));
	ZeroMemory(chReadBuf, sizeof(chReadBuf));
	BYTE  byData[30];
	BYTE  byGetData[30];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	char  chValue[10];
	//	int   nSwitchIndex;
	CString strMsg;
	ZeroMemory(chValue, sizeof(chValue));
	strValue.Format("OPLKMFG");
	memcpy(chValue, strValue, strValue.GetLength());

	byData[0] = START_CMD;
	byData[1] = 0XB2;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X01;
	byData[5] = nPD;
	nLength = byData[3] * 256 + byData[4] + 4;
	nCheckSum = 0;
	for (i = 1; i <= nLength; i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[6] = nCheckSum - 1;
	byData[7] = END_CMD;
	WORD nCmdLength = 8;
	PBYTE pbySendData = NULL;
	CmdSendExchange(byData, nCmdLength, &pbySendData, &nCmdLength);
	if (!WriteBuffer((char*)pbySendData, nCmdLength))
	{
		return -99;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData, 30, &nCmdLength))
	{
		return -99;
	}
	PBYTE pbyNewData = NULL;
	if (!CmdReadExchange(byGetData, nCmdLength, &pbyNewData))
	{
		return FALSE;
	}

	if (pbyNewData[2] != byData[2])
	{
		return -99;
	}
	long dwPower;
	dwPower = (pbyNewData[7] << 24) + (pbyNewData[8] << 16) + (pbyNewData[9] << 8) + pbyNewData[10];
	dblPDPower = (double)dwPower / 100.0;

	return dblPDPower;
}
BOOL Z4671Command::GetProductPDADC(int nPD, int *PDADC)
{
	BYTE  byData[30];
	BYTE  byGetData[30];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	char  chValue[10];
//	int   nSwitchIndex;
	CString strMsg;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());

	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X09;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XF6;
	byData[13] = (BYTE)nPD;
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	//nCheckSum = byData[1];
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[14] = nCheckSum-1;
	byData[15] = END_CMD;
	WORD nCmdLength = 16;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,30,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	*PDADC = pbyNewData[6]*256+pbyNewData[7];		
	
	return TRUE;

}

BOOL Z4671Command::GetProductPDPower(int nPDIndex, double *pdblPDPower)
{
	char  chSendBuf[10];
	char  chReadBuf[256];
	double dblPDPower;
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[30];
	BYTE  byGetData[30];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	char  chValue[10];
//	int   nSwitchIndex;
	CString strMsg;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	
	byData[0] = START_CMD;
	byData[1] = 0XB2;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X01;
	byData[5] = nPDIndex;
	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[6] = nCheckSum-1;
	byData[7] = END_CMD;
	WORD nCmdLength = 8;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,30,&nCmdLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	long dwPower;
	dwPower = (pbyNewData[7]<<24)+(pbyNewData[8]<<16)+(pbyNewData[9]<<8)+pbyNewData[10];
	dblPDPower = (double)dwPower/10.0;	
	*pdblPDPower = dblPDPower;
	return TRUE;
}

BOOL Z4671Command::GetCurrentDAC(int nBlock, int nSwitch, SHORT *psDACX, SHORT *psDACY)
{
	char  chSendBuf[10];
	char  chReadBuf[256];
	double dblPDPower;
	CString strCmd;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[256];
	BYTE  byGetData[256];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	char  chValue[10];
//	int   nSwitchIndex;
	CString strMsg;
	ZeroMemory(chValue,sizeof(chValue));
	strValue.Format("OPLKMFG");
	strncpy(chValue,strValue,strValue.GetLength());
	
	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X09;
	for (i=5;i<12;i++)
	{
		byData[i] = chValue[i-5];
	}
	byData[12] = 0XF4;
	if (nBlock==1) 
	{
		byData[13] = nSwitch+32;
	}
	else if (nBlock==2) 
	{
		byData[13] = nSwitch+16;
	}
	else if (nBlock==3) 
	{
		if (nSwitch==1) 
		{
			byData[13] = 50;
		}
		else if (nSwitch==2)
		{
			byData[13] = 49;
		}
	}

	nLength = byData[3]*256+byData[4]+4;
	nCheckSum = 0;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[14] = nCheckSum-1;
	byData[15] = END_CMD;
	WORD nCmdLength = 0x10;
	DWORD dwLength = 0x00;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData,256,(PDWORD)&dwLength))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<dwLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData, dwLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	SHORT sDACX;
	SHORT sDACY;
	sDACX = (SHORT)((pbyNewData[6]<<8)+pbyNewData[7]);
    *psDACX = sDACX;

	sDACY = (SHORT)((pbyNewData[8]<<8)+pbyNewData[9]);
	*psDACY = sDACY;
	return TRUE;

}

BOOL Z4671Command::SetVOAAtten(int nType, int nVOAIndex, double dblAtten)
{
	char  chSendBuf[256];
	char  chReadBuf[256];
	CString strCmd;
	CString strToken;
	LONG   lPower;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	BYTE  byData[256];
	
	int   i;
	int   nLength=0;
	int   nCheckSum=0;
	CString strValue;
	CString strMsg;
	SHORT sValue;

	byData[0] = START_CMD;
	byData[1] = 0XBD;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X03; 
	if (nType==0) //EDFA VOA
	{
		byData[5] = nVOAIndex + 59;
	}
	else  //SW VOA
	{
		byData[5] = nVOAIndex + 17;
	}
	sValue = dblAtten*10;
	byData[6] = (BYTE)(sValue<<8);
	byData[7] = (BYTE)(sValue);

	nLength = byData[3]*256+byData[4]+4;
	for (i=1;i<=nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[8] = nCheckSum-1;
	byData[9] = END_CMD;
	WORD nCmdLength = 10;
	PBYTE pbySendData=NULL;
	CmdSendExchange(byData,nCmdLength,&pbySendData,&nCmdLength);
	
	if(!WriteBuffer((char*)pbySendData,nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	BYTE  byGetData[256];
	if (!ReadBuffer((char*)byGetData,256,&nCmdLength))
	{
		m_strLogInfo = ("读取数据错误");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd=0;nCmd<nCmdLength;nCmd++)
	{
		strMsg.Format("%02X ",byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	
	PBYTE pbyNewData=NULL;
	if(!CmdReadExchange(byGetData,nCmdLength,&pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1]!=0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2]!=byData[2])
	{
		return FALSE;
	}
	if (pbyNewData[5]!=0XBD) 
	{
		return FALSE;
	}
	return TRUE;

}


bool Z4671Command::SetScanDelayTime(int nDelayus)
{
	CString strCommand;
	BYTE  byData[20];
	BYTE  byGetData[30];
	int   i;
	int   nLength;
	int   nCheckSum;
	CString strValue;
	char  chValue[10];
	int   nSwitchIndex;
	CString strMsg;
	ZeroMemory(chValue, sizeof(chValue));
	ZeroMemory(byData, sizeof(byData));
	strValue.Format("OPLKMFG");
	strncpy(chValue, strValue, strValue.GetLength());

	byData[0] = START_CMD;
	byData[1] = 0XF0;
	byData[2] = 0X00; //Index
	byData[3] = 0X00;
	byData[4] = 0X0C;
	for (i = 5;i<12;i++)
	{
		byData[i] = chValue[i - 5];
	}
	byData[12] = 0XFD;
	
	byData[13] = (BYTE)(nDelayus>>24);
	byData[14] = (BYTE)(nDelayus >> 16); //X AXIS
	byData[15] = (BYTE)(nDelayus >> 8); //X AXIS
	byData[16] = (BYTE)(nDelayus); //X AXIS
	nLength = byData[3] * 256 + byData[4] + 4;
	nCheckSum = 0;
	for (i = 1;i <= nLength;i++)
	{
		nCheckSum = nCheckSum^byData[i];
	}
	byData[17] = nCheckSum - 1;
	byData[18] = END_CMD;
	WORD nCmdLength = 19;
	DWORD dwCmd = 0;
	PBYTE pbySendData = NULL;
	CmdSendExchange(byData, nCmdLength, &pbySendData, &nCmdLength);
	if (!WriteBuffer((char*)pbySendData, nCmdLength))
	{
		m_strLogInfo = ("串口打开错误！");
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer((char*)byGetData, 30, (PDWORD)&dwCmd))
	{
		m_strLogInfo = ("接收错误！");
		return FALSE;
	}
	int   nCmd;
	CString strReadData;
	strReadData = "";
	for (nCmd = 0;nCmd<dwCmd;nCmd++)
	{
		strMsg.Format("%02X ", byGetData[nCmd]);
		strReadData = strReadData + strMsg;
	}
	m_strLogInfo = strReadData;
	PBYTE pbyNewData = NULL;
	if (!CmdReadExchange(byGetData, dwCmd, &pbyNewData))
	{
		return FALSE;
	}
	if (pbyNewData[1] != 0X00)
	{
		return FALSE;
	}
	if (pbyNewData[2] != byData[2])
	{
		return FALSE;
	}
	return TRUE;
}
