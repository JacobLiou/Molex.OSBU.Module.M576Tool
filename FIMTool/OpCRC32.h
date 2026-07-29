// OpCRC32.h: interface for the COpCRC32 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPCRC32_H__1E8A92AC_32AE_429C_9433_5F13B0C55F97__INCLUDED_)
#define AFX_OPCRC32_H__1E8A92AC_32AE_429C_9433_5F13B0C55F97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
//WH CRC Cal 2014-7-14  RIS
#define	POLYNOMIAL	0xEDB88320
#define CRC_TABLE_LEN      256
#define CRC_CYCLE_VALUE    8
#define CRC_CALC_VALUE     0xEDB88320
#define CRC_START_VALUE    0xFFFFFFFF
#define MAX_BYTE_VALUE     0xFF
#define BIT_8_SHIFT         8


class COpCRC32  
{
public:
	VOID  InitCRC32();
	DWORD GetCRC(PBYTE pDataStream, DWORD dwByteCount);
	DWORD GetThisCRC(BYTE bThisByte);
	COpCRC32();
	virtual ~COpCRC32();

	void  Util_CRC32InitTable();
	INT   Util_CRC32Calculate(UCHAR *pucSource, UINT uSize, UINT *puCrc);
	UINT  m_auCRC32Table[CRC_TABLE_LEN];


protected:
	DWORD m_dwCRC32;
	DWORD	m_pdwCRC32Table[256];
};

#endif // !defined(AFX_OPCRC32_H__1E8A92AC_32AE_429C_9433_5F13B0C55F97__INCLUDED_)
