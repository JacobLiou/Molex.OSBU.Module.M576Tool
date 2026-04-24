
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OPLKGPIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OPLKGPIB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef DLLEXPORT
#define DLLEXPORT   __declspec( dllexport )
#endif
#ifndef DllImport
#define DllImport   __declspec( dllimport )
#endif

#include "decl-32.h"

#pragma once

#define	GPIB_SUCCEED(x)	(!((x) & (ERR | TIMO)))
#define	MAX_STRLEN		1024
#define EXTRA_BYTES		300



typedef	int		GPIB_HANDLE, *PGPIB_HANDLE;
typedef	WORD	GPIB_STATUS;

// This class is exported from the OplkGPIB.dll
class  DLLEXPORT COplkGPIBDevice 
{
public:
	COplkGPIBDevice();
	~COplkGPIBDevice();
	
public:
	BOOL ConfigGB(int option,int value);
	BOOL OpenDevice(DWORD dwGPIBAddress,int nBordIndex=0);
	BOOL CloseDevice();
	BOOL WriteGPIB(PVOID pWriteBuffer);
	BOOL ReadGPIB(PVOID pREadBuffer, DWORD dwReadLength);
	BOOL SerialPollGPIB(PCHAR pbSPByte);
	BOOL WaitForCompletion(WORD wMask);

	BOOL IsRequestCompleted();
	BOOL GetGPIBError(PSTR pszErrorMessage);
	BOOL ClearDevice();
protected:
	GPIB_HANDLE m_hGPIBDevice;
	// TODO: add your methods here.
};
