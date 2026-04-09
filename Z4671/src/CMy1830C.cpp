// CMy1830C.cpp: implementation of the CMy1830C class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "ventura.h"
#include "CMy1830C.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMy1830C::CMy1830C()
{

}

CMy1830C::~CMy1830C()
{

}

BOOL CMy1830C::ReadPower(double &dblPower)
{
	CString	strSend;
	WORD wIndex = 0;
	char	chRet[256];
	BOOL bSuc = FALSE;
	
	strSend.Format("D?\r\n");

//	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
//		return FALSE;


	Sleep(450);
	for (int iLoop = 0; iLoop < 10; iLoop++) //Modify by hanfeiz 2010-3-10
	{
		if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		{
			continue;
		}

		ZeroMemory(chRet, 256);

		if (ReadBuffer(chRet, 256))
		{
			bSuc =TRUE;
			break;
		}
	}
	
	if (!bSuc) 
	{
		return FALSE;
	}

	dblPower = atof(chRet);

	return TRUE;
}

BOOL CMy1830C::SetWavelength(int dblWavelength)
{
	CString	strSend;
	strSend.Format("w%d\r\n", dblWavelength);

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;
	
	return TRUE;
}

BOOL CMy1830C::GetWavelength(double &dblWavelength)
{
	CString	strSend;
	strSend.Format("W?\r\n");

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;
	
	char chRet[256];
	ZeroMemory(chRet, 256 * sizeof(char));
	if (!ReadBuffer(chRet, 256))
		return FALSE;

	dblWavelength = atof(chRet);

	return TRUE;
}

BOOL CMy1830C::SetRange(int RangeMode)
{
	CString	strSend;
	strSend.Format("R%d\r\n", RangeMode);

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;
	
	return TRUE;
}

BOOL CMy1830C::GetRange(int &nRangeMode)
{
	CString	strSend;
	strSend.Format("R?\r\n");

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;
	
	char	chRet[256];
	ZeroMemory(chRet, 256 * sizeof(char));
	if (!ReadBuffer(chRet, 256))
		return FALSE;

	nRangeMode = atoi(chRet);

	return TRUE;
}

BOOL CMy1830C::SetUnit(int nUnitType)
{
	CString	strSend;
	strSend.Format("U%d\r\n", nUnitType);

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;
	
	return TRUE;
}

BOOL CMy1830C::GetUnit(int &nUnitType)
{
	CString	strSend;
	strSend.Format("U?\r\n");

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;
	
	char	chRet[256];
	ZeroMemory(chRet, 256 * sizeof(char));
	if (!ReadBuffer(chRet, 256))
		return FALSE;

	nUnitType = atoi(chRet);

	return TRUE;
}

BOOL CMy1830C::SetAverageTime(int nTime)
{
	CString	strSend;
	strSend.Format("F%d\r\n", nTime);

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;
	
	return TRUE;
}

BOOL CMy1830C::GetAverageTime(int &nTime)
{
	CString	strSend;
	strSend.Format("F?\r\n");

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;
	
	char	chRet[256];
	ZeroMemory(chRet, 256 * sizeof(char));
	if (!ReadBuffer(chRet, 256))
		return FALSE;

	nTime = atoi(chRet);

	return TRUE;
}

BOOL CMy1830C::SetEcho(BOOL bEchoOff)
{
	CString	strSend;
	if (bEchoOff)
		strSend = "E0\r\n";
	else
		strSend = "E1\r\n";

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;

	return TRUE;
}

BOOL CMy1830C::SetStoRef()
{
	if(!SetUnit(2))
	{
		return FALSE;
	}
	CString	strSend;
	strSend.Format("S\r\n");

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;

	return TRUE;
}

BOOL CMy1830C::SetZero()
{
	CString	strSend;
	strSend.Format("Z1\r\n");

	if (!WriteBuffer(strSend.GetBuffer(32), strSend.GetLength()))
		return FALSE;

	return TRUE;
}
