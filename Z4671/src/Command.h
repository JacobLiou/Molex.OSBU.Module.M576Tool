// Command.h: interface for the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMAND_H__132C730A_1404_4B4D_B042_14CC165E6C9A__INCLUDED_)
#define AFX_COMMAND_H__132C730A_1404_4B4D_B042_14CC165E6C9A__INCLUDED_

#if _MSC_VER > 1000
#include "OpComm.h"
#pragma once
#endif // _MSC_VER > 1000

#include "DataDefine.h"

class CCommand  
{
public:
	BOOL UpgradeCommand(WORD iCommandAddr, BYTE byTxData);
	BOOL GetGpioBit(char chPort, BYTE bBit, BYTE* pbValue);
	BOOL SetGpioBit(char chPort, BYTE bBit, BYTE bValue);
	BOOL GetI2CData(BYTE bAddress, BYTE bLen, BYTE* pbOut);
	BOOL SetI2CData(BYTE bAddress, BYTE* pbData, BYTE bLen);
	BOOL SetI2CData(BYTE bAddress, long lData, BYTE bLen);
	
	BOOL ByteSetCommand(WORD iCommandAddr, CString strTxData);
	BOOL SetColdReset();
	BOOL SetWarmReset();
	BOOL SetSoftReset();
	void CloseComPort();
	BOOL Upgrard32ByteCommand(WORD iCommandAddr, CString strTxData);
	BOOL OpenComPort(CString strComPort, int iBaudRate);
	CString HexTranslateToBin(CString strHex);
	CString BinTranslateToHex(CString strData);
	BOOL GetCommand(WORD iCommandAddr,WORD wRxDataLen, CString *strTxData);
	WORD GenerateCheckSum(BYTE pbyMsg[64]);
	WORD GetADC(WORD iCommandAddr);
	
	double ReadPower(WORD iCommandAddr);
	BOOL SetCommand(WORD iCommandAddr, CString TxData);
	COpComm m_Com;
	CString	m_I2CAddress;
	CCommand();
	virtual ~CCommand();
	
};

#endif // !defined(AFX_COMMAND_H__132C730A_1404_4B4D_B042_14CC165E6C9A__INCLUDED_)
