// Copyright @2005, Oplink Communication,inc
// OpComm.h: interface for the COpComm class.
// Revision 1.0 05-17-2005
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_)
#define AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DLLEXPORT
#define DLLEXPORT   __declspec( dllexport )
#endif
#ifndef DllImport
#define DllImport   __declspec( dllimport )
#endif


class  DLLEXPORT COpComm  
{
public:

	BOOL ReadBuffer(char* pInBuffer, DWORD dwBufferSize, PDWORD pdwReadLength = NULL);
	// Read the data from the receive buffer of the COM port 
	//BOOL ReadBuffer(char* pInBuffer, DWORD dwBufferSize);
	
	// write the data to the transmit buffer of the COM port
	/// Serial TX is raw bytes (ASCII text and/or binary frames). Never send wchar_t / UTF-16.
	BOOL WriteBuffer(char* pOutBuffer, DWORD dwBufferSize);
	BOOL WriteBuffer(BYTE* pOutBuffer, DWORD dwBufferSize);
	/// Same as WriteBuffer but does not purge RX (required for ASCII request/response).
	BOOL WriteBufferNoPurge(char* pOutBuffer, DWORD dwBufferSize);
	BOOL WriteBufferNoPurge(BYTE* pOutBuffer, DWORD dwBufferSize);

	HANDLE GetPortHandle() const { return m_hCommPort; }

	BOOL ReadBuffer(BYTE* pInBuffer, DWORD dwBufferSize);
	BOOL ReadBuffer(BYTE* pInBuffer, DWORD dwBufferSize, PDWORD pdwReadLength);

	/// Driver RX queue length (for draining long RECAL 3 ASCII lines efficiently).
	DWORD RxBytesWaiting() const;

	// Close the COM port
	void ClosePort();

	// Configure the COM Port upon the input parameter,
	// Open the COM Port.
	BOOL OpenPort(LPTSTR strPortName,           // COM port index
		          DWORD dwBaudRate = 9600,    // Baud Rate:115200
				  BYTE bByteSize = 8,           // 8 data bits
				  BYTE bParity =  NOPARITY,     // no parity
				  BYTE bStopBits = ONESTOPBIT); // one stop bit
	COpComm();

	virtual ~COpComm();

protected:
	HANDLE	m_hCommPort; // COM port handler

};

#endif // !defined(AFX_OPCOMM_H__82138A00_2098_44B0_8F5F_C96B6BDB8880__INCLUDED_)
