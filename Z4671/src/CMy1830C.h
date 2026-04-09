// CMy1830C.h: interface for the CPM1830C class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMy1830C_H__662AC0E7_75A1_4AE2_ABD6_D7D2589B7BF9__INCLUDED_)
#define AFX_CMy1830C_H__662AC0E7_75A1_4AE2_ABD6_D7D2589B7BF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpComm.h"

class CMy1830C : public COpComm  
{
public:
	BOOL SetZero();
	BOOL SetStoRef();
	BOOL SetEcho(BOOL bEchoOff = TRUE);
	BOOL GetAverageTime(int &nTime);//1=Slow; 2=medium; 3=fast
	BOOL SetAverageTime(int nTime);//1=Slow; 2=medium; 3=fast
	BOOL GetUnit(int &nUnitType);//1=w; 2=dB; 3=dBm; 4=REL
	BOOL SetUnit(int nUnitType=3);//1=w; 2=dB; 3=dBm; 4=REL
	BOOL GetRange(int &nRangeMode);//
	BOOL SetRange(int RangeMode=0);//0 = auto, 1~8
	BOOL GetWavelength(double &dblWavelength);
	BOOL SetWavelength(int dblWavelength);
	BOOL ReadPower(double &dblPower);
	CMy1830C();
	virtual ~CMy1830C();
};

#endif // !defined(AFX_PM1830C_H__662AC0E7_75A1_4AE2_ABD6_D7D2589B7BF9__INCLUDED_)
