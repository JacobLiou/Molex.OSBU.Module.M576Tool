// OpCRC32.h: interface for the COpCRC32 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCRC32_H__1E8A92AC_32AE_429C_9433_5F13B0C55F97__INCLUDED_)
#define AFX_OPCRC32_H__1E8A92AC_32AE_429C_9433_5F13B0C55F97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
#define	POLYNOMIAL	0xEDB88320

class COpCRC32  
{
public:
	VOID  InitCRC32();
	DWORD GetCRC(PBYTE pDataStream, DWORD dwByteCount);
	DWORD GetThisCRC(BYTE bThisByte);
	COpCRC32();
	virtual ~COpCRC32();

protected:
	DWORD m_dwCRC32;
	DWORD	m_pdwCRC32Table[256];
};

#endif // !defined(AFX_OPCRC32_H__1E8A92AC_32AE_429C_9433_5F13B0C55F97__INCLUDED_)
