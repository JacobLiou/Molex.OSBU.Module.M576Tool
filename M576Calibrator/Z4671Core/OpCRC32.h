// OpCRC32.h: interface for the COpCRC32 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCRC32_H__INCLUDED_)
#define AFX_OPCRC32_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <Windows.h>

#define POLYNOMIAL 0xEDB88320L

class COpCRC32
{
public:
	COpCRC32();
	virtual ~COpCRC32();
	VOID InitCRC32();
	DWORD GetThisCRC(BYTE bThisByte);
	DWORD GetCRC(PBYTE pDataStream, DWORD dwByteCount);

private:
	DWORD m_dwCRC32;
	DWORD m_pdwCRC32Table[256];
};

#endif
