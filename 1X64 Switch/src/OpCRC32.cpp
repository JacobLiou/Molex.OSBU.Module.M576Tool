// OpCRC32.cpp: implementation of the COpCRC32 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OpCRC32.h"
 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpCRC32::COpCRC32()
{
	DWORD	dwElement;
    ///unsigned long 	dwElement;
    LONG	lElementIndex, lPolyIndex;

    for (lElementIndex = 0; lElementIndex < 256; lElementIndex ++)
    {
        dwElement = lElementIndex;
        for (lPolyIndex = 8; lPolyIndex > 0; lPolyIndex --)
        {
            if (dwElement & 1)
                dwElement = (dwElement >> 1) ^ POLYNOMIAL;
            else
                dwElement >>= 1;
        }
        m_pdwCRC32Table[lElementIndex] = dwElement;
    }


}

COpCRC32::~COpCRC32()
{
}

DWORD COpCRC32::GetThisCRC(BYTE bThisByte)
{
	DWORD	dwOldCRC32;
	DWORD	dwTemp1, dwTemp2, dwTemp3, dwTemp4;

	dwOldCRC32 = m_dwCRC32;

//    m_dwCRC32 = ((dwOldCRC32 >> 8) & 0x00FFFFFF) ^ m_pdwCRC32Table[(dwOldCRC32 ^bThisByte) & 0x000000FF];
	dwTemp1 = (dwOldCRC32 >> 8) & 0x00FFFFFF;
	dwTemp2 = (dwOldCRC32 ^ bThisByte) & 0x000000FF;
	dwTemp3 = m_pdwCRC32Table[dwTemp2];
	dwTemp4 = dwTemp1 ^ dwTemp3;
			
	m_dwCRC32 = dwTemp4;

    return m_dwCRC32;

}

DWORD COpCRC32::GetCRC(PBYTE pDataStream, DWORD dwByteCount)
{
	DWORD	dwByteIndex;
	BYTE	bThisByte;

	//	Set m_dwCRC32 = 0xFFFFFFFF;
	InitCRC32();

	dwByteIndex = 0;
   	while (dwByteIndex < dwByteCount)
	{
		bThisByte = *pDataStream;
		GetThisCRC(bThisByte);

		pDataStream ++;
	}

	return m_dwCRC32;
}

VOID COpCRC32::InitCRC32()
{
	m_dwCRC32 = 0xFFFFFFFF;
}
