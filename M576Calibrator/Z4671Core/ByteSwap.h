#pragma once
#include <windows.h>

inline unsigned long SwapDWORD(unsigned long dwInData)
{
	BYTE* pch = (BYTE*)&dwInData;
	pch[0] ^= pch[3];
	pch[3] ^= pch[0];
	pch[0] ^= pch[3];
	pch[1] ^= pch[2];
	pch[2] ^= pch[1];
	pch[1] ^= pch[2];
	return dwInData;
}

inline unsigned int SwapWORD(unsigned int wInData)
{
	BYTE* pch = (BYTE*)&wInData;
	pch[0] ^= pch[1];
	pch[1] ^= pch[0];
	pch[0] ^= pch[1];
	return wInData;
}
