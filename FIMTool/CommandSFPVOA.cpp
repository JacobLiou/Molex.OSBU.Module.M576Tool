// CommandSFPVOA.cpp: implementation of the CCommandSFPVOA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "testprocess.h"
#include "CommandSFPVOA.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandSFPVOA::CCommandSFPVOA()
{

}

CCommandSFPVOA::~CCommandSFPVOA()
{

}

BOOL CCommandSFPVOA::ReadDataFromI2CBus(BYTE bI2CAddr, WORD wMemoryOffset, PBYTE pbDataBytes, BYTE bDatalength, CString* pszErrMsg)
{
	BYTE	byAddress[2];
	BYTE    byThisByte;

	try
	{
		memset(byAddress, 0x00, 2);
		byAddress[0] = (BYTE)wMemoryOffset;

		if(!SendCmdToAdapter(bI2CAddr, CMD_WRITE, byAddress, 1))
			throw "Error: Write data to VOA I2C bus fail!";
	
		byThisByte = bDatalength;
		Sleep(100);
		if(!SendCmdToAdapter(bI2CAddr, CMD_READ, &byThisByte, 1))
			throw "Error: Read data from VOA I2C bus fail!";
	
		memcpy(pbDataBytes, m_pbRxBuffer, bDatalength);
	}
	catch(TCHAR* ptszErrorMsg)
	{
		if(NULL != pszErrMsg)
			pszErrMsg->Format("%s", ptszErrorMsg);

		return	FALSE;
	}
	
	return	TRUE;
}

BOOL CCommandSFPVOA::WriteDataToI2CBus(BYTE bI2CAddr, BYTE bMemoryOffset, PBYTE pbDataBytes, BYTE bDatalength, CString* pszErrMsg)
{
	BYTE i;
	BYTE	pbyTxBuf[MAX_PATH];

	try
	{		
		ZeroMemory(pbyTxBuf, MAX_PATH);
		
		pbyTxBuf[0] = bMemoryOffset;
		
		for(i = 0; i < bDatalength; i++)
		{
			pbyTxBuf[1 + i] = pbDataBytes[i];
		}
		
		if(!SendCmdToAdapter(bI2CAddr, CMD_WRITE, pbyTxBuf, 1 + bDatalength))
		{
			throw "Error: Write data to VOA I2C bus fail!";
		}

	}
	catch(TCHAR* ptszErrorMsg)
	{
		if(NULL != pszErrMsg)
			pszErrMsg->Format("%s", ptszErrorMsg);

		return	FALSE;
	}
	
	return	TRUE;
}

BOOL CCommandSFPVOA::WriteDataToTester(BYTE bObjID, PBYTE pbDataBytes, BYTE bDatalength, BOOL bWrite)
{
	if(bWrite)
	{
		if(!SendCmdToAdapter(bObjID, CMD_WRITE, pbDataBytes, bDatalength))
			return	FALSE;
	}
	else
	{
		if(!SendCmdToAdapter(bObjID, CMD_READ, pbDataBytes, bDatalength))
			return	FALSE;

		if(bObjID ==0x94 || bObjID == 0xA0)
			return	TRUE;

		memcpy(pbDataBytes, m_pbRxBuffer, bDatalength);
	}

	return	TRUE;
}

BOOL CCommandSFPVOA::ReadDataFromTester(BYTE bObjID, PBYTE pbDataBytes, BYTE bDatalength)
{
	BYTE	pbSendData[4] = {0};

	pbSendData[0] = bDatalength;
	if(!SendCmdToAdapter(bObjID, CMD_READ, pbSendData, 1))
		return	FALSE;

	memcpy(pbDataBytes, pbSendData, bDatalength);//

	return	TRUE;
}

BOOL CCommandSFPVOA::SendCmdToAdapter(BYTE bObjID, BYTE  bAccess, PBYTE pbDataSource, BYTE  bDataBytes, CString* pszErrMsg)
{
	CString	strErrMsg;
    BYTE	bChecksum = 0;
	WORD	wWriteLength;
	WORD	wReadLength;
	DWORD	dwBytesRead;

	try
	{
		// General the Tx message array.
		memset(m_pbTxBuffer, 0x00, RS232_BUFFER_SIZE);
		memset(m_pbRxBuffer, 0x00, RS232_BUFFER_SIZE);

		m_pbTxBuffer[CMD_FLAG_IDX]     = CMD_START_FLAG;
		m_pbTxBuffer[CMD_OBJID_IDX]    = bObjID;
		m_pbTxBuffer[CMD_ACCESS_IDX]   = bAccess;
		m_pbTxBuffer[CMD_DATA_LEN_IDX] = bDataBytes;
		m_pbTxBuffer[CMD_STATUS_IDX]   = CMD_EXECUTE_SUCESS;
		memcpy(m_pbTxBuffer + CMD_PLAYLOAD_DATA_IDX, pbDataSource, bDataBytes);
		
		wWriteLength = bDataBytes + MSG_BASIC_LENGTH - 1;

		// Calculate the checksm.
		bChecksum = 0;
		for(int i =0; i < wWriteLength - 1; i ++)
		{
			bChecksum += m_pbTxBuffer[i];
		}
		m_pbTxBuffer[i] = ~bChecksum;
		
		if(!m_pRS232->WriteBuffer((char*)m_pbTxBuffer, wWriteLength))
			throw "Error: Write serial port Buffer fail!";

				// Soft reset, nothing repsonse.
		if((bObjID == OBJID_SOFT_RESET_ADAPTER) || (bObjID == OBJID_SET_ADAPTER_BAUD_RATE) || (bObjID == 0xC0))
			return	TRUE;
		
		Sleep(100);
		if((bObjID==OBJID_READ_VOA_REG) || (bObjID==OBJID_SET_ADAPTER_SWEEPNOTRIG))
		{
			Sleep(100);
		}

		if(bAccess == CMD_WRITE)
			wReadLength = MSG_BASIC_LENGTH;
		else
			wReadLength = pbDataSource[0] + MSG_BASIC_LENGTH - 1;

		if((bObjID==0x94) || (bObjID == 0xA0))
		{
			wReadLength = 255;
		}
		//ZeroMemory(m_pbRxBuffer, sizeof(BYTE)*RS232_BUFFER_SIZE);
		if(!m_pRS232->ReadBuffer((char*)m_pbRxBuffer, wReadLength, &dwBytesRead))
			throw "Error: Read serial port Buffer fail!";

		if((bObjID==0x94) || (bObjID == 0xA0) || (bObjID == 0x50))
		{
			wReadLength = (WORD)dwBytesRead;
		}

		// Check receive message
		if(m_pbTxBuffer[CMD_FLAG_IDX] != m_pbRxBuffer[CMD_FLAG_IDX])
			throw "Error: Read message start flag is not matched!";
		else if(m_pbTxBuffer[CMD_OBJID_IDX] != m_pbRxBuffer[CMD_OBJID_IDX])
			throw "Error: Read message objID is not matched!";
		else if(m_pbTxBuffer[CMD_ACCESS_IDX] != m_pbRxBuffer[CMD_ACCESS_IDX])
			throw "Error: Read message Access is not matched!";

		if(((bObjID == OBJID_READ_CONNECT_CHANNEL) ||(bObjID == OBJID_SET_GET_CURRENT_CHANNEL)) && (bAccess == CMD_READ))
		{
			pbDataSource[0] = m_pbRxBuffer[CMD_PLAYLOAD_DATA_IDX];//如果是读，只返回一个字节长度吗？
		
			return	TRUE;
		}
	
		// Read Connect Channel and Read Current Connect channel send back one byte data.

		if(m_pbRxBuffer[CMD_DATA_LEN_IDX] != wReadLength - (MSG_BASIC_LENGTH - 1))
			throw "Error: Read message playload data length is wrong";

		if(m_pbRxBuffer[CMD_STATUS_IDX] != CMD_EXECUTE_SUCESS)
			throw "Error: Command executed fail!";

		bChecksum = 0;
		for(i =0; i < wReadLength - 1; i ++)
		{
			bChecksum += m_pbRxBuffer[i];
		}
		bChecksum = ~bChecksum;	
		
		if(bChecksum != m_pbRxBuffer[wReadLength - 1])
			throw "Error: Read message checksum is wrong";

		memcpy(m_pbRxBuffer, m_pbRxBuffer + CMD_PLAYLOAD_DATA_IDX, wReadLength - (MSG_BASIC_LENGTH - 1));

		m_wBytesRead = wReadLength - (MSG_BASIC_LENGTH - 1); 

	}
	catch(TCHAR* ptszErrorMsg)
	{
		if(NULL != pszErrMsg)
			pszErrMsg->Format("%s", ptszErrorMsg);

		strErrMsg.Format("%s", ptszErrorMsg);

		return	FALSE;
	}

	return	TRUE;
}

BOOL CCommandSFPVOA::GetModuleSN(CString *pstrRespMsg)
{
	CString	strErrMsg;
	char	pbyRxData[16] = {0};

	if(!ReadDataFromI2CBus(OBJID_SERIAL_ID_I2C_ADDR, REG_VENDOR_SN, (PBYTE)pbyRxData, 10, &strErrMsg))
	{
		pstrRespMsg->Format("%s", strErrMsg);
		return	FALSE;
	}

	pstrRespMsg->Format("%s", pbyRxData);

	return	TRUE;
}

BOOL CCommandSFPVOA::GetModulePN(CString *pstrRespMsg)
{
	CString	strErrMsg;
	char	pbyRxData[16] = {0};
	//ZeroMemory(pbyRxData,sizeof(char)*16);
	if(!ReadDataFromI2CBus(OBJID_SERIAL_ID_I2C_ADDR, REG_VENDOR_PN, (PBYTE)pbyRxData, 16, &strErrMsg))
	{
		pstrRespMsg->Format("%s", strErrMsg);
		return	FALSE;
	}

	pstrRespMsg->Format("%s", pbyRxData);

	return	TRUE;
}

BOOL CCommandSFPVOA::GetMinIL(PBYTE pbMinIL, CString *pstrErrMsg)
{	
	BYTE	pbyRxData[2] = {0};

	if(!ReadDataFromI2CBus(OBJID_SERIAL_ID_I2C_ADDR, REG_IL_OPTICAL_PATH, pbyRxData, 1, pstrErrMsg))
		return	FALSE;
	
	*pbMinIL = pbyRxData[0];

	return TRUE;
}

BOOL CCommandSFPVOA::GetModuleTempt(double* pdblTempt, CString *pstrErrMsg)
{
	BYTE	pbyRxData[4] = {0};

	if(!ReadDataFromI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, REG_MODULE_TEMP, pbyRxData, 2, pstrErrMsg))
		return	FALSE;

	*pdblTempt  = ((SHORT)(((WORD)pbyRxData[0]<<8) + pbyRxData[1]))/10.0;//读取SFP内部温度

	return	TRUE;
}

BOOL CCommandSFPVOA::GetModuleTemptADC(WORD wTempADCReg, PWORD pwTempADC, CString *pstrErrMsg)
{
	BYTE pbDataBytes[4] = {0};
	
	if(!ReadDataFromI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, wTempADCReg, pbDataBytes, 2, pstrErrMsg))
		return	FALSE;

	*pwTempADC = ((WORD)pbDataBytes[0] << 8) + pbDataBytes[1]; 

	return TRUE;
}

BOOL CCommandSFPVOA::SetWorkMode(WORD wModeReg, BYTE byMode, CString *pstrErrMsg)
{
	BYTE	pbDataBytes[2];

	for(int i = 0; i < 3; i ++)
	{
		if(!WriteDataToI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, (BYTE)wModeReg, &byMode, 1, pstrErrMsg))
			return	FALSE;

		if(!ReadDataFromI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, (BYTE)wModeReg, (PBYTE)pbDataBytes, 1, pstrErrMsg))
			return	FALSE;

		if(pbDataBytes[0] == byMode)
			break;
	}

	if(pbDataBytes[0] != byMode)
		return	FALSE;

	return TRUE;	
}

BOOL CCommandSFPVOA::SetVOADAC(WORD wDACRegAddr, WORD wDACValue, CString *pstrErrMsg)
{
	BYTE	pbDataBytes[4] = {0};

	for(int i = 0; i < 3; i ++)
	{
		pbDataBytes[0] = (BYTE)(wDACValue >> 8);
		pbDataBytes[1] = (BYTE)(wDACValue);
		if(!WriteDataToI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, (BYTE)wDACRegAddr, pbDataBytes, 2, pstrErrMsg))
			return	FALSE;

		if(!ReadDataFromI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, (BYTE)wDACRegAddr, (PBYTE)&pbDataBytes[2], 2, pstrErrMsg))
			return	FALSE;

		if((pbDataBytes[0] == pbDataBytes[2]) && (pbDataBytes[1] == pbDataBytes[3]))
			break;
	}

	if((pbDataBytes[0] != pbDataBytes[2]) || (pbDataBytes[1] != pbDataBytes[3]))
		return	FALSE;

	return TRUE;	
}

BOOL CCommandSFPVOA::SetVOAAtten(WORD wAttRegAddr, double dblAtten, CString *pstrErrMsg)
{
	WORD	wAttenValue;
	BYTE	pbDataBytes[4] = {0};

	wAttenValue = (WORD)(dblAtten * 10.0);

	for(int i = 0; i < 3; i ++)
	{
		pbDataBytes[0] = (BYTE)(wAttenValue >> 8);
		pbDataBytes[1] = (BYTE)(wAttenValue);
		if(!WriteDataToI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, (BYTE)wAttRegAddr, pbDataBytes, 2, pstrErrMsg))
			return	FALSE;

		if(!ReadDataFromI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, (BYTE)wAttRegAddr, (PBYTE)&pbDataBytes[2], 2, pstrErrMsg))
			return	FALSE;
	
		if((pbDataBytes[0] == pbDataBytes[2]) && (pbDataBytes[1] == pbDataBytes[3]))
			break;
	}

	if((pbDataBytes[0] != pbDataBytes[2]) || (pbDataBytes[1] != pbDataBytes[3]))
		return	FALSE;

	return TRUE;	
}
BOOL CCommandSFPVOA::SetVOAPINStatus(BOOL bLogicHigh)
{
	BYTE	bCheckByte;

	if(bLogicHigh)
		bCheckByte = 0xAA;	// 写0xAA设置所有管脚为高
	else
		bCheckByte = 0x55;	// 写0x55设置所有管脚为低

	if(!WriteDataToI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, REG_PIN_STATUS_SET, &bCheckByte, 1, NULL))
		return	FALSE;
	
	return TRUE;	
}

BOOL CCommandSFPVOA::GetVOAPINStatus(PBYTE pbPinStatus)
{
	BYTE	pbDataBytes[4] = {0};

	if(!ReadDataFromTester(0x84, pbDataBytes, 1))
		return	FALSE;

	pbPinStatus[0] = pbDataBytes[0];

	return	TRUE;
			
	/*
	// Present pin status
	if(pbDataSrc[0] & BIT0)
		LogInfo("VOA Present pin is high", FALSE);
	else
		LogInfo("VOA Present pin is low", FALSE);

	// Loss alarm pin
	if(pbDataSrc[0] & BIT1)
		LogInfo("VOA Output LOSS Alarm pin is high", FALSE);
	else
		LogInfo("VOA Output LOSS Alarm pin is low", FALSE);

	// Tx Fault pin
	if(pbDataSrc[0] & BIT2)
		LogInfo("VOA Tx Fault pin is high", FALSE);
	else
		LogInfo("VOA Tx Fault pin is low", FALSE);

	// Disable pin
	if(pbDataSrc[0] & BIT3)
		LogInfo("VOA Disable pin is high", FALSE);
	else
		LogInfo("VOA Disable pin is low", FALSE);
	*/
}

BOOL CCommandSFPVOA::SetVOALutCRC32Check(BOOL bCheckPDLut)
{
	BYTE bCheckByte;

	if(bCheckPDLut)
		bCheckByte = 0x07;	// bit0: 电子标签，bit1: VOA, bit2: PD
	else
		bCheckByte = 0x03;	// bit0: 电子标签，bit1: VOA, bit2: PD
	
	if(!WriteDataToI2CBus(OBJID_DIAGNOSTIC_I2C_ADDR, REG_LUT_CRC32_CHECK, &bCheckByte, 1, NULL))
		return	FALSE;
	
	return TRUE;
}

BOOL CCommandSFPVOA::GetAllModuleTemp(BYTE bySetChannel, BYTE bPresent, WORD wADCReg, double* pdblTempt, PWORD pwTemptADC, CString *pstrErrMsg)
{
	BYTE	i,bChanStaus, bVOAChan;
	SHORT	pnReadData[16] = {0};

	/**************************************
	Bit 4-7: Reserved
	Bit 3: 0b: One byte Addr
			  1b: Two bytes Addr
	Bit1-0: 00b: Current Channel
				 01b: Ch1 - Ch4
				 10b: Ch5 - Ch8
				 11b: Ch1 - Ch8
	**************************************/
	bChanStaus = bySetChannel & 0x03;
	if(bChanStaus == 0x00)
		bVOAChan = 1;
	else if(bChanStaus == 0x03)
		bVOAChan = 8;
	else
		bVOAChan = 4;

	// 读取模块温度: HUAWEI: 96 ~ 97, Standard: 96 ~ 97
	if(NULL != pdblTempt)
	{
		if(!GetVOARegValue(bySetChannel, OBJID_DIAGNOSTIC_I2C_ADDR, bPresent, REG_MODULE_TEMP, (PBYTE)pnReadData, 2, pstrErrMsg, TRUE))
			return	FALSE;
		for(i = 0; i < bVOAChan; i ++)
		{
			pdblTempt[i] = (double)pnReadData[i] / 10.0;
		}
	}

	if(NULL != pwTemptADC)
	{
		// 读取模块温度ADC: HUAWEI: 58 ~ 59, Standard: 145 ~ 146
		if(!GetVOARegValue(bySetChannel, OBJID_DIAGNOSTIC_I2C_ADDR, bPresent, wADCReg, (PBYTE)pnReadData, 2, pstrErrMsg, TRUE))
			return	FALSE;
		for(i = 0; i < bVOAChan; i ++)
		{
			pwTemptADC[i] = (WORD)pnReadData[i];
		}
	}

	return	TRUE;
}

BOOL CCommandSFPVOA::GetAllModuleCRC32(BYTE bySetChannel, BYTE bPresent, PDWORD pdwLabelCRC32, 
									   PDWORD pdwVOACRC32, PDWORD pdwPDCRC32, CString *pstrErrMsg)
{
	BYTE	pbDataBuffer[128] = {0};	// 12 * 8 = 96
	BYTE	i,bChanStaus, bVOAChan;
	PBYTE	pbDataRead = pbDataBuffer;

	/**************************************
	Bit 4-7: Reserved
	Bit 3: 0b: One byte Addr
			  1b: Two bytes Addr
	Bit1-0: 00b: Current Channel
				 01b: Ch1 - Ch4
				 10b: Ch5 - Ch8
				 11b: Ch1 - Ch8
	**************************************/
	bChanStaus = bySetChannel & 0x03;
	if(bChanStaus == 0x00)
		bVOAChan = 1;
	else if(bChanStaus == 0x03)
		bVOAChan = 8;
	else
		bVOAChan = 4;

	if(!GetVOARegValue(bySetChannel, OBJID_DIAGNOSTIC_I2C_ADDR, bPresent, REG_LABEL_CRC32_CKS, pbDataBuffer, 12, pstrErrMsg, FALSE))
		return	FALSE;

	for(i = 0; i < bVOAChan; i ++)
	{
		pdwLabelCRC32[i]  = (DWORD)pbDataRead[0] << 24;
		pdwLabelCRC32[i] += (DWORD)pbDataRead[1] << 16;
		pdwLabelCRC32[i] += (DWORD)pbDataRead[2] << 8;
		pdwLabelCRC32[i] += (DWORD)pbDataRead[3];

		pdwVOACRC32[i]  = (DWORD)pbDataRead[4] << 24;
		pdwVOACRC32[i] += (DWORD)pbDataRead[5] << 16;
		pdwVOACRC32[i] += (DWORD)pbDataRead[6] << 8;
		pdwVOACRC32[i] += (DWORD)pbDataRead[7];

		pdwPDCRC32[i]  = (DWORD)pbDataRead[8] << 24;
		pdwPDCRC32[i] += (DWORD)pbDataRead[9] << 16;
		pdwPDCRC32[i] += (DWORD)pbDataRead[10] << 8;
		pdwPDCRC32[i] += (DWORD)pbDataRead[11];

		pbDataRead += 12;
	}

	return	TRUE;
}

BOOL CCommandSFPVOA::GetAllModuleDAC(BYTE bySetChannel, BYTE bPresent, WORD wDACReg, PWORD pwVOADAC, CString *pstrErrMsg)
{
	BYTE	i,bChanStaus, bVOAChan;
	SHORT	pnReadData[16] = {0};

	/**************************************
	Bit 4-7: Reserved
	Bit 3: 0b: One byte Addr
			  1b: Two bytes Addr
	Bit1-0: 00b: Current Channel
				 01b: Ch1 - Ch4
				 10b: Ch5 - Ch8
				 11b: Ch1 - Ch8
	**************************************/
	bChanStaus = bySetChannel & 0x03;
	if(bChanStaus == 0x00)
		bVOAChan = 1;
	else if(bChanStaus == 0x03)
		bVOAChan = 8;
	else
		bVOAChan = 4;

	if(NULL != pwVOADAC)
	{
		// 读取模块温度ADC: HUAWEI: 58 ~ 59, Standard: 145 ~ 146
		if(!GetVOARegValue(bySetChannel, OBJID_DIAGNOSTIC_I2C_ADDR, bPresent, wDACReg, (PBYTE)pnReadData, 2, pstrErrMsg, TRUE))
			return	FALSE;

		for(i = 0; i < bVOAChan; i ++)
		{
			pwVOADAC[i] = (WORD)pnReadData[i];
		}
	}

	return	TRUE;
}

BOOL CCommandSFPVOA::GetAllPDADC(BYTE bySetChannel, BYTE bPresent, WORD wADCReg, PWORD pwPDADC, CString *pstrErrMsg)
{
	BYTE	i,bChanStaus, bVOAChan;
	SHORT	pnReadData[64] = {0};	// 4 * 8 = 32

	/**************************************
	Bit 4-7: Reserved
	Bit 3: 0b: One byte Addr
			  1b: Two bytes Addr
	Bit1-0: 00b: Current Channel
				 01b: Ch1 - Ch4
				 10b: Ch5 - Ch8
				 11b: Ch1 - Ch8
	**************************************/
	bChanStaus = bySetChannel & 0x03;
	if(bChanStaus == 0x00)
		bVOAChan = 1;
	else if(bChanStaus == 0x03)
		bVOAChan = 8;
	else
		bVOAChan = 4;

	if(NULL != pwPDADC)
	{
		// 读取模块温度ADC: HUAWEI: 58 ~ 59, Standard: 145 ~ 146
		if(!GetVOARegValue(bySetChannel, OBJID_DIAGNOSTIC_I2C_ADDR, bPresent, wADCReg, (PBYTE)pnReadData, 8, pstrErrMsg, TRUE))
			return	FALSE;

		// 4级ADC
		for(i = 0; i < bVOAChan; i ++)
		{
			pwPDADC[4*i]   = (WORD)pnReadData[4*i];
			pwPDADC[4*i+1] = (WORD)pnReadData[4*i+1];
			pwPDADC[4*i+2] = (WORD)pnReadData[4*i+2];
			pwPDADC[4*i+3] = (WORD)pnReadData[4*i+3];
		}
	}

	return	TRUE;
}


BOOL CCommandSFPVOA::GetLWDMPDPower(BYTE bSIALLCh,BYTE bPDChan,double* pdblPDPower, CString *pstrErrMsg)
{
	SHORT	pnReadData[8] = {0};

	/**************************************
	Header + ObjID + R/W + Length + Status + data + checksum
	write channel:
	FE 40 00 08 00 CD 08 00 00 01 80 00 A9 BA
	FE 40 00 08 00 CE 08 00 00 02 80 00 A7 BA
	FE 40 00 08 00 CF 08 00 00 03 80 00 A5 BA
	FE 40 00 08 00 D0 08 00 00 04 80 00 A3 BA
	FE 40 00 08 00 D1 08 00 00 05 80 00 A1 BA
	FE 40 00 08 00 D2 08 00 00 06 80 00 9F BA
	FE 40 00 08 00 D3 08 00 00 07 80 00 9D BA
	FE 40 00 08 00 D4 08 00 00 08 80 00 9B BA
	read power:
	Cmd:FE 40 01 01 00 0A B5
	Rsp:FE 40 01 0A 00 CD 0A 00 00 01 80 00 EE 44 75 B7
	value:0xEE44,-4540
	**************************************/

	if(NULL != pdblPDPower)
	{	
		if(!GetPDRegValue(bSIALLCh,bPDChan, (PBYTE)pnReadData, pstrErrMsg, TRUE))
			return	FALSE;
			if (bSIALLCh) 
			{
				for (int k=0;k<8;k++) //回读过来的信息4E FA D2 EE C4 EE C0 EE B7 EE 04 EF C0 EE CA
				{			
					pdblPDPower[k] = (double)pnReadData[k] / 100.0;
					if (pdblPDPower[k] < -50 || pdblPDPower[k] > 0.0)
					{
						//return FALSE;
					}
				}
			}
			else
			{			
				pdblPDPower[0] = (double)pnReadData[0] / 100.0;
				if (pdblPDPower[0] < -50 || pdblPDPower[0] > 0.0)
				{
					return FALSE;
				}
			}
	}

	return	TRUE;
}
BOOL CCommandSFPVOA::GetVoaChannelStatus(BYTE *bStatus)
{
	BYTE	bConnectStatus;
	for(int i = 0; i < 3; i ++)//都读三遍，测试盒有不明原因。
	{
		if(!ReadDataFromTester(OBJID_READ_CONNECT_CHANNEL, &bConnectStatus, 1))
			return FALSE;
	}
	*bStatus = bConnectStatus;

	return TRUE;
}
BOOL CCommandSFPVOA::SelectChannel1X2SW(BYTE byChannel)
{
	BYTE	bSwitchChan;

	bSwitchChan = byChannel;
	if (!WriteDataToTester(OBJID_SET_I2C_1X2SW, &bSwitchChan, 1))
		return FALSE;

	return TRUE;
}

BOOL CCommandSFPVOA::SelectChannel1X8SW(BYTE byChannel)
{
	BYTE	bSwitchChan;

	bSwitchChan = byChannel;
	if (!WriteDataToTester(OBJID_SET_I2C_1X8SW, &bSwitchChan, 1))
		return FALSE;

	return TRUE;
}

BOOL CCommandSFPVOA::SelectLDSource(BYTE byLDSource)
{
	BYTE	bSwitchPos;

	if((byLDSource > SRC_ASE_SOURCE) || (byLDSource < SRC_SFP_1_153094))
		return	FALSE;

	bSwitchPos = byLDSource - SRC_SFP_1_153094;
	if(bSwitchPos < 2)	// 0/1
	{
		if(!SelectChannel1X2SW(bSwitchPos))
			return FALSE;
		
		if(!SelectChannel1X8SW(SW_1X8_CH1))
			return	FALSE;
	}
	else				// 2-8
	{
		if(!SelectChannel1X8SW(bSwitchPos))
			return	FALSE;
	}

	return	TRUE;
}

BOOL CCommandSFPVOA::SelectWorkStation(BYTE byWSindex)
{
	BYTE	bSwitchPos;

	if((byWSindex > SRC_WS_8) || (byWSindex < SRC_WS_REF))
		return	FALSE;

	bSwitchPos = byWSindex - SRC_WS_REF;
	if(bSwitchPos < 1)	//0
	{
		if(!SelectChannel1X2SW(bSwitchPos))
			return FALSE;
		
		if(!SelectChannel1X8SW(SW_1X8_CH1))
			return	FALSE;
	}
	else				// 1-8
	{
		if (bSwitchPos < 2)//1
		{
			if(!SelectChannel1X2SW(bSwitchPos))
				return FALSE;
		}
		if(!SelectChannel1X8SW(bSwitchPos))
			return	FALSE;
	}

	return	TRUE;
}


BOOL CCommandSFPVOA::SelectChannel(BYTE byChannel)
{
	BYTE	bAnalogChan;

	bAnalogChan = byChannel;
	if (!WriteDataToTester(OBJID_SET_GET_CURRENT_CHANNEL, &bAnalogChan, 1))
		return FALSE;

	return TRUE;
}

BOOL CCommandSFPVOA::WDLExecuteSweep(BYTE* pbyStatus, CString *pstrErrMsg)
{	
	BYTE	pbyGetData[16] = {0};

	/*
	Command Play-load:
	0x69: Execute SFP transmitter sweep and trigger out Sync for WDL measurement
	Others: do nothing	*/
	pbyGetData[0] = 0x69;
	if(!WriteDataToTester(OBJID_SET_ADAPTER_DOWDLSWEEP, pbyGetData, 1))
		return	FALSE;

	return TRUE;	
}

BOOL CCommandSFPVOA::SetSFPVoaDACAttnotrigSweep(BYTE bySetChannel,WORD wyDACReg, WORD wDACValue,CString *pstrErrMsg)
{	
	BYTE pbyGetData[16] = {0};

	pbyGetData[0]= bySetChannel;

	pbyGetData[1]= (BYTE)(wyDACReg>>8);
	pbyGetData[2]= (BYTE)(wyDACReg);

	pbyGetData[3]= (BYTE)(wDACValue>>8);
	pbyGetData[4]= (BYTE)(wDACValue);

	if(!WriteDataToTester(OBJID_SET_ADAPTER_SWEEPNOTRIG, pbyGetData, 5))
	{
		Sleep(200);
		if(!WriteDataToTester(OBJID_SET_ADAPTER_SWEEPNOTRIG, pbyGetData, 5))
		{
			pstrErrMsg->Format("设置寄存器: %d到数值为: %d失败", wyDACReg, wDACValue);
			return	FALSE;
		}
	}

	return TRUE;	
}

//opm与ADAPTER同步扫描 VOA定标
BOOL CCommandSFPVOA::DoSFPVoaDACSweep(BYTE bySetChannel,WORD wyDACReg,WORD wyStartDAC,WORD wyStopDAC,BYTE byStepDAC,WORD wyMaxOPMtime,CString *pstrErrMsg)
{	
	BYTE	pbyGetData[16] = {0};

	pbyGetData[1]= (BYTE)(wyDACReg>>8);
	pbyGetData[2]= (BYTE)(wyDACReg);
	
	pbyGetData[3] = (BYTE)(wyStartDAC>>8);
	pbyGetData[4] = (BYTE)(wyStartDAC);

	pbyGetData[5] = (BYTE)(wyStopDAC>>8);
	pbyGetData[6] = (BYTE)(wyStopDAC);

	pbyGetData[7] = byStepDAC;

	pbyGetData[8] = (BYTE)(wyMaxOPMtime>>8);
	pbyGetData[9] = (BYTE)(wyMaxOPMtime);

	if(!WriteDataToTester(OBJID_SET_ADAPTER_DODACSWEEP, pbyGetData, 10))
		return	FALSE;

	return TRUE;	
}

//opm与ADAPTER同步扫描 VOA att定标
BOOL CCommandSFPVOA::DoSFPVoaAttSweep(BYTE bySetChannel,WORD wyAttReg,WORD wyStartAtt,WORD wyStopAtt,BYTE byStepAtt,WORD* wyData,WORD wyMaxOPMtime,CString *pstrErrMsg)
{	
	BYTE		pbyGetData[16] = {0};

	pbyGetData[0]= bySetChannel;

	pbyGetData[1]= (BYTE)(wyAttReg>>8);
	pbyGetData[2]= (BYTE)(wyAttReg);
	
	pbyGetData[3] = (BYTE)(wyStartAtt>>8);
	pbyGetData[4] = (BYTE)(wyStartAtt);

	pbyGetData[5] = (BYTE)(wyStopAtt>>8);
	pbyGetData[6] = (BYTE)(wyStopAtt);

	pbyGetData[7] = byStepAtt;

	pbyGetData[8] = (BYTE)(wyMaxOPMtime>>8);
	pbyGetData[9] = (BYTE)(wyMaxOPMtime);

	if(!WriteDataToTester(OBJID_SET_ADAPTER_DOATTSWEEP, pbyGetData, 10))
		return	FALSE;

	return TRUE;	
}

BOOL CCommandSFPVOA::CheckSweepStatus(PBYTE pbSweepStatus)
{
	BYTE	bSweepStatus;

	if(!ReadDataFromTester(OBJID_GET_ADAPTER_SWEEPSTATUS, &bSweepStatus, 1))
		return	FALSE;

	*pbSweepStatus = bSweepStatus;

	return	TRUE;
}
BOOL CCommandSFPVOA::GetPDRegValue(BYTE bSIALLCh,BYTE bPDChan, PBYTE pbReadData, CString *pstrErrMsg, BOOL bMSBFirst)
{
	BYTE		pbyTxBuf[16] = {0};
	PBYTE		pbRecvData = NULL;
	CString		strTemp;
	int bySetChannel=bPDChan;
	BYTE wyRegValue =0xCC;
	BYTE wyIICValue =0xAB;
	ZeroMemory(pbyTxBuf,sizeof(BYTE)*16);
	// data 
	//0   1  2  3  4 5  6  7
	//CD 08 00 00 01 80 00 A9
	//CE 08 00 00 02 80 00 A7
	if (!bSIALLCh)
	{
		pbyTxBuf[0] = wyRegValue+bPDChan;
		pbyTxBuf[1] = 0x08;
		pbyTxBuf[4] = bPDChan;
		pbyTxBuf[5] = 0x80;
		pbyTxBuf[7] = wyIICValue-(bPDChan*2);

		//CD 08 00 00 01 80 00 A9
		if(!SendCmdToAdapter(OBJID_READ_PD_POWER, CMD_WRITE, pbyTxBuf, 8, pstrErrMsg))
		{
			Sleep(200);
			if(!SendCmdToAdapter(OBJID_READ_PD_POWER, CMD_WRITE, pbyTxBuf, 8, pstrErrMsg))
				return FALSE;
		}
	}
	else
	{
		pbyTxBuf[1] = 0x08;
		pbyTxBuf[4] = 0x01;
		pbyTxBuf[5] = 0x83;
		pbyTxBuf[7] = 0x73;
						//0 1  2  3   4  5  6  7  	
		//FE 40 00 08 00 00 08 00 00 01 83 00 73 BA
		if(!SendCmdToAdapter(OBJID_READ_PD_POWER, CMD_WRITE, pbyTxBuf, 8, pstrErrMsg))
		{
			Sleep(200);
			if(!SendCmdToAdapter(OBJID_READ_PD_POWER, CMD_WRITE, pbyTxBuf, 8, pstrErrMsg))
				return FALSE;
		}
	}
	//Header + ObjID + R/W + Length + Status + data + checksum
	/*read power:
	Cmd:FE 40 01 01 00 0A B5
	Rsp:FE 40 01 0A 00 CD 0A 00 00 01 80 00 EE 44 75 B7
	*/
	if (bSIALLCh)
	{//Comd:FE 40 01 01 00 18 A7
		ZeroMemory(pbyTxBuf,sizeof(BYTE)*16);
		pbyTxBuf[0] = 0x18;
		if(!SendCmdToAdapter(OBJID_READ_PD_POWER, CMD_READ, pbyTxBuf, 1, pstrErrMsg))
		{
			Sleep(200);
			if(!SendCmdToAdapter(OBJID_READ_PD_POWER, CMD_READ, pbyTxBuf, 1, pstrErrMsg))
				return FALSE;
		}
	}
	else
	{
		ZeroMemory(pbyTxBuf,sizeof(BYTE)*16);
		pbyTxBuf[0] = 0x0A;	
		if(!SendCmdToAdapter(OBJID_READ_PD_POWER, CMD_READ, pbyTxBuf, 1, pstrErrMsg))
		{
			Sleep(200);
			if(!SendCmdToAdapter(OBJID_READ_PD_POWER, CMD_READ, pbyTxBuf, 1, pstrErrMsg))
				return FALSE;
		}
	}

	pbRecvData = m_pbRxBuffer;
	//pbRecvData+=7;
	if (bSIALLCh)
	{
	
		for (int i=0;i<8;i++)
		{
			*pbReadData ++ = pbRecvData[8+i*2];
			*pbReadData ++ = pbRecvData[7+i*2];
		}
	}
	else
	{	

		*pbReadData ++ = pbRecvData[8];
		*pbReadData ++ = pbRecvData[7];

	}



	return TRUE;	
}

BOOL CCommandSFPVOA::GetVOARegValue(BYTE bySetChannel, BYTE bI2CAddr, BYTE bPresent, WORD wyRegValue, PBYTE pbReadData, 
									BYTE bDataCount, CString *pstrErrMsg, BOOL bMSBFirst)
{
	BYTE		i, bVOAChan;
	BYTE		bCheckCount;
	WORD		wDataCount;
	BYTE		pbyTxBuf[16] = {0};
	PBYTE		pbRecvData = NULL;
	CString		strTemp;
	
	/**************************************
	bySetChannel:
	Bit 4-7: Reserved
	Bit 3: 0b: One byte Addr
			  1b: Two bytes Addr
	Bit1-0: 00b: Current Channel
				 01b: Ch1 - Ch4
				 10b: Ch5 - Ch8
				 11b: Ch1 - Ch8
	**************************************/

	// 同时读取4个或者8个产品的寄存器数据
	pbyTxBuf[0] = bySetChannel;
	if(bySetChannel & BIT(3))
	{
		pbyTxBuf[1] = (BYTE)(wyRegValue>>8);
		pbyTxBuf[2] = (BYTE)(wyRegValue);
		pbyTxBuf[3] = 0;
		pbyTxBuf[4] = bDataCount;
	}
	else
	{
		pbyTxBuf[1] = bI2CAddr;
		pbyTxBuf[2] = (BYTE)(wyRegValue);
		pbyTxBuf[3] = 0;
		pbyTxBuf[4] = bDataCount;
	}

	if(!SendCmdToAdapter(OBJID_READ_VOA_REG, CMD_READ, pbyTxBuf, 5, pstrErrMsg))
	{
		Sleep(200);
		if(!SendCmdToAdapter(OBJID_READ_VOA_REG, CMD_READ, pbyTxBuf, 5, pstrErrMsg))
			return FALSE;
	}
	bySetChannel &= 0x03;

	if(bySetChannel == 0x00)
		bVOAChan = 1;
	else if(bySetChannel == 0x03)
		bVOAChan = 8;
	else
		bVOAChan = 4;

	wDataCount = 0;
	pbRecvData = m_pbRxBuffer;
	if((bMSBFirst) && (!(bDataCount % 2)))
	{
		// DAC/ADC需要转换高低字节
		for(i = 0; i < bVOAChan; i ++)
		{
			bCheckCount = bDataCount;
			while(bCheckCount)
			{
				if(bPresent & BIT(i))
				{
					*pbReadData ++ = pbRecvData[1];
					*pbReadData ++ = pbRecvData[0];

					pbRecvData  += 2;
					wDataCount  += 2;
				}
				else
				{
					*pbReadData ++ = 0;
					*pbReadData ++ = 0;
				}

				bCheckCount -= 2;
			}
		}
	}
	else
	{
		for(i = 0; i < bVOAChan; i ++)
		{
			bCheckCount = bDataCount;
			while(bCheckCount)
			{
				if(bPresent & BIT(i))
				{
					*pbReadData ++ = pbRecvData[0];
					wDataCount += 1;
					pbRecvData ++;
				}
				else
				{
					*pbReadData ++ = 0;
				}

				bCheckCount --;
			}
		}
	}

	// 数值应该相同
	if(m_wBytesRead != wDataCount)
	 	return	FALSE;

	return TRUE;	
}


BOOL CCommandSFPVOA::StartXmodem(BYTE bySFPType, BYTE bySFPChannel)
{
	BYTE	pbyRxData[16] = {0};
	BYTE	pbDataSource[2];

	pbDataSource[0] = bySFPType;
	pbDataSource[1] = bySFPChannel;

	m_bVOAXmodemUpgStatus = 0xFF;

	if(!SendCmdToAdapter(OBJID_XMODEM_DOWNLOAD, CMD_WRITE, pbDataSource, 2))
		return	FALSE;

	// Check Response
	int nTimeOut = 0;
    while(1)
    {
        Sleep(50);
		if(m_pRS232->ReadBuffer((char*)pbyRxData, 10))
		{
			if(pbyRxData[0]  == XMODEM_CRCCHR || pbyRxData[1]  == XMODEM_CRCCHR)
				break;
		}
		nTimeOut ++;
		if(nTimeOut >= 210)
		{
		//	MessageBox(NULL, "等待Xmodem请求回应超过10s，请复位模块，重新下载一次！！", "提示", MB_OK|MB_ICONERROR);
			return FALSE;
		}
	}

	return TRUE;
}

BYTE CCommandSFPVOA::XmodemloadData(PBYTE pbBinData, WORD wDownloadSize, BOOL bFileDone, CString* pstrErrMsg)
{
	char	byTempBuf[MAX_CMD];
	int		nTimeOut;
	int		nRetry = 0;
	DWORD	dwReadLength;
	
	// Send Packet data
	if(!m_pRS232->WriteBuffer((char*)pbBinData, wDownloadSize))
	{
		*pstrErrMsg = "RS232发送数据错误!";
		return	XMODEM_COMMUNICATION_FAIL;
	}
	
	nTimeOut = 0;
	nRetry   = 0;
    while (1)
    {
        Sleep(20);

		if(m_pRS232->ReadBuffer(byTempBuf, MAX_CMD,&dwReadLength))
        {
           
			// Affirmative Answer
            if(byTempBuf[dwReadLength - 1] == XMODEM_ACK)//dwReadLength - 1
			{
                break;
			}
            // Negative Answer
            else if(byTempBuf[dwReadLength - 1] == XMODEM_NAK)//dwReadLength - 1
            {
				// Send Packet data again
				if(!m_pRS232->WriteBuffer((char*)pbBinData, wDownloadSize))
				{
					*pstrErrMsg = "RS232发送数据错误!";
					return XMODEM_COMMUNICATION_FAIL;
				}

                nRetry++;
				if(nRetry > 10)
				{
					// Cancel Transfer
					byTempBuf[0] = XMODEM_CAN;
					byTempBuf[1] = XMODEM_CAN;
					byTempBuf[2] = XMODEM_CAN;

					if(!m_pRS232->WriteBuffer((char*)byTempBuf, 3))
						*pstrErrMsg = "发送3个CAN取消Xmodem下载失败!";
					else
						*pstrErrMsg = "发送3个CAN取消Xmodem下载，请重新再下载一次!";
					
					return	XMODEM_COMMUNICATION_FAIL;
				}
            }
			else if(byTempBuf[dwReadLength - 1] == XMODEM_CAN)
			{
				*pstrErrMsg = "模块发生CAN字符上来取消Xmodem下载，请重新下载一次!";
				return	XMODEM_COMMUNICATION_FAIL;
			}
			else if(byTempBuf[dwReadLength - 1] == XMODEM_CRCCHR)
			{
				continue;
			}
			else
			{
				*pstrErrMsg = "模块发送过来的回应信息不正常，请重启模块后再下载一次!";
				return	XMODEM_COMMUNICATION_FAIL;
			}
		}

		nTimeOut ++;
		if(nTimeOut >= 500)
		{
			*pstrErrMsg = "等待Xmodem回应超过10s，请复位模块，重新下载一次！！";
			return XMODEM_COMMUNICATION_FAIL;
		}
    }

    // Transfer Done
	if(bFileDone)
	{
		byTempBuf[0] = XMODEM_EOT;
		byTempBuf[1] = XMODEM_EOT;
		byTempBuf[2] = XMODEM_EOT;
		
		if(!m_pRS232->WriteBuffer((char*)byTempBuf, 3))
		{
			*pstrErrMsg = "发送3个EOT完成Xmodem下载失败";
			return	XMODEM_COMMUNICATION_FAIL;
		}

		nTimeOut = 0;		
		while (1)
		{
			Sleep(500);
			ZeroMemory(byTempBuf, MAX_CMD);
			if(m_pRS232->ReadBuffer(byTempBuf, MAX_CMD,&dwReadLength))
			{
				// Three bytes: NAK, ACK, Status
				if(dwReadLength < 2)
					return	XMODEM_DOWNLOAD_FAIL;

				// Affirmative Answer
				if(byTempBuf[dwReadLength-2] == XMODEM_ACK)
				{
					// response 0x00 means all available sfp voa FW upgrade success
					if(byTempBuf[dwReadLength-1] == 0x00)
					{
						return	XMODEM_DOWNLOAD_SUCCESS;
					}

					// it any bit set to 1b means this SFP VOA FW upgrade fail
					m_bVOAXmodemUpgStatus = byTempBuf[1];
					/*
					for(i = 0; i < 8; i ++)
					{
						if(byTempBuf[1] & (1<<i))
						{
							strTemp.Format("Slot %d SFP VOA FW Upgrade Fail", i+1);
							LogInfo(strTemp, FALSE);
						}
					}
					*/
					return	XMODEM_DOWNLOAD_FAIL;
				}
				else if(byTempBuf[0] == XMODEM_NAK)
				{
					return	XMODEM_DOWNLOAD_FAIL;
				}
			}

			nTimeOut ++;
			if(nTimeOut >= 210)
			{
				*pstrErrMsg = "等待Xmodem回应超过10s，请复位模块，重新下载一次！！";
				return XMODEM_COMMUNICATION_FAIL;
			}
		}
	}

    return XMODEM_DOWNLOAD_SUCCESS;

}

BYTE CCommandSFPVOA::GetXmodemUpgStatus(void)
{
	return	m_bVOAXmodemUpgStatus;
}





