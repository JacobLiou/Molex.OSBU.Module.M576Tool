// OpComm.h: interface for the COpComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_)
#define AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COpComm  
{
public:
	HANDLE	m_hCommPort;
	BOOL ReadBuffer(char* pInBuffer, DWORD dwBufferSize,PDWORD pdwLength = NULL);
	BOOL WriteBuffer(char* pOutBuffer, DWORD dwBufferSize);
	VOID ClosePort();
	BOOL OpenPort(LPTSTR strPortName,
		DWORD dwBaudRate = 34800, 
		BYTE bByteSize = 8,
		BYTE bParity = NOPARITY,//EVENPARITY,
		BYTE bStopBits = ONESTOPBIT);
	COpComm();
	virtual ~COpComm();
	
protected:
	
	
};

#endif // !defined(AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_)
