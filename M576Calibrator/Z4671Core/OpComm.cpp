// OpComm.cpp: implementation of the COpComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OpComm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpComm::COpComm()
{
	m_hCommPort = INVALID_HANDLE_VALUE;
}

COpComm::~COpComm()
{
//	ClosePort();
}

BOOL COpComm::OpenPort(LPTSTR strPortName, DWORD dwBaudRate, 
					   BYTE bByteSize, BYTE bParity, BYTE bStopBit)
{
	BOOL			bPortReady = FALSE;
	DCB				PortDCB;
	COMMTIMEOUTS	PortTimeOuts;

	//	If the port was previously open, close it first
	ClosePort();

	//	Open the port and do new settings
	m_hCommPort = CreateFile(strPortName, 
							 GENERIC_READ | GENERIC_WRITE, 
							 0, 
							 NULL,
							 OPEN_EXISTING, 
							 0, 
							 NULL);
	if(m_hCommPort == INVALID_HANDLE_VALUE)
		return bPortReady;

	bPortReady = GetCommState(m_hCommPort, &PortDCB);
	if(!bPortReady)
		return bPortReady;

	PortDCB.BaudRate = dwBaudRate;
	PortDCB.ByteSize = bByteSize;
//	PortDCB.fParity  = TRUE;   //enable parity checking 
	PortDCB.Parity = bParity;//bParity;
	PortDCB.StopBits = bStopBit;
	PortDCB.fAbortOnError = TRUE;

	//	Disable XON/XOFF on both input and output
	PortDCB.fInX = 0;
	PortDCB.fOutX = 0;

	bPortReady = SetCommState(m_hCommPort, &PortDCB);
	if(!bPortReady)
		return bPortReady;

	bPortReady = GetCommTimeouts(m_hCommPort, &PortTimeOuts);
	if(!bPortReady)
		return bPortReady;

	PortTimeOuts.ReadIntervalTimeout = 50;
	PortTimeOuts.ReadTotalTimeoutConstant = 50;
	PortTimeOuts.ReadTotalTimeoutMultiplier = 10;
	PortTimeOuts.WriteTotalTimeoutConstant = 50;
	PortTimeOuts.WriteTotalTimeoutMultiplier = 10;

	bPortReady = SetCommTimeouts(m_hCommPort, &PortTimeOuts);
	if(!bPortReady)
		return bPortReady;

	bPortReady = SetupComm(m_hCommPort, 1024, 1024);
	if(!bPortReady)
		return bPortReady;

	bPortReady = PurgeComm(m_hCommPort, PURGE_TXCLEAR | PURGE_RXCLEAR);

	return bPortReady;
}

VOID COpComm::ClosePort()
{
	if(m_hCommPort != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCommPort);
		m_hCommPort = INVALID_HANDLE_VALUE;
	}
}

BOOL COpComm::WriteBuffer(char* pOutBuffer, DWORD dwBufferSize)
{
	DWORD dwBytesWritten;

	if(m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!PurgeComm(m_hCommPort, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		return FALSE;
	}

	WriteFile(m_hCommPort, pOutBuffer, dwBufferSize, &dwBytesWritten, NULL);
	
	if(dwBytesWritten != dwBufferSize)
		return FALSE;

	return TRUE;
}

BOOL COpComm::WriteBufferNoPurge(char* pOutBuffer, DWORD dwBufferSize)
{
	DWORD dwBytesWritten;
	if (m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;
	if (!WriteFile(m_hCommPort, pOutBuffer, dwBufferSize, &dwBytesWritten, NULL))
		return FALSE;
	return dwBytesWritten == dwBufferSize;
}

BOOL COpComm::WriteBufferNoPurge(BYTE* pOutBuffer, DWORD dwBufferSize)
{
	return WriteBufferNoPurge((char*)pOutBuffer, dwBufferSize);
}

BOOL COpComm::ReadBuffer(char* pInBuffer, DWORD dwBufferSize,PDWORD pdwLength)
{
	DWORD dwBytesRead;

	if(m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;

	ReadFile(m_hCommPort, pInBuffer, dwBufferSize, &dwBytesRead, NULL);

	if(dwBytesRead == 0)
		return FALSE;

	if (pdwLength!=NULL)
	{
		*pdwLength = dwBytesRead;
	}
	
	// if received data length  is not equal with the want data length
//	if(dwBytesRead != dwBufferSize)
//		return FALSE;

	return TRUE;
}
BOOL COpComm::WriteBuffer(BYTE* pOutBuffer, DWORD dwBufferSize)
{
	DWORD dwBytesWritten;

	if (m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!PurgeComm(m_hCommPort, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		return FALSE;
	}

	WriteFile(m_hCommPort, pOutBuffer, dwBufferSize, &dwBytesWritten, NULL);

	if (dwBytesWritten != dwBufferSize)
		return FALSE;

	return TRUE;
}
BOOL COpComm::ReadBuffer(BYTE* pInBuffer, DWORD dwBufferSize, PDWORD pdwReadLength)
{

	DWORD dwBytesRead;

	if (m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;

	ReadFile(m_hCommPort, pInBuffer, dwBufferSize, &dwBytesRead, NULL);

	if (dwBytesRead == 0)
		return FALSE;

	//PostRxDisplayMsg(pInBuffer,dwBytesRead);
	if (pdwReadLength != NULL)
	{
		*pdwReadLength = dwBytesRead;
	}

	return TRUE;
}
BOOL COpComm::ReadBuffer(BYTE* pInBuffer, DWORD dwBufferSize)
{
	DWORD dwBytesRead;

	if (m_hCommPort == INVALID_HANDLE_VALUE)
		return FALSE;

	ReadFile(m_hCommPort, pInBuffer, dwBufferSize, &dwBytesRead, NULL);

	if (dwBytesRead == 0)
		return FALSE;

	// if received data length  is not equal with the want data length
	//	if(dwBytesRead != dwBufferSize)
	//	return FALSE;

	return TRUE;
}