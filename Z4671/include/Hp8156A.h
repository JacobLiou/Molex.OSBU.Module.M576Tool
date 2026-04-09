// Hp8156A.h: interface for the CHp8156A class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HP8156A_H__B138F563_E908_4FCC_B41C_DFAE8CD06B0D__INCLUDED_)
#define AFX_HP8156A_H__B138F563_E908_4FCC_B41C_DFAE8CD06B0D__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "op816x.h"

class  CHp8156A
{
public:
	CHp8156A();
	virtual ~CHp8156A();
public:
	void SetVOADevInfo(pstDeviceSetInfo pVOAInfo,long m_DevCom);
	BOOL ResetDevice();
	void CompleteRequest();
	BOOL SetWavelength(double dblWL);
	BOOL GetWavelength(double* pdblWL);
	BOOL SetAttenuation(double dblAtten);
	BOOL GetAttenuation(double &dblAtten);
	BOOL OpenDevice();
	BOOL SetEnableOutput(BOOL bEnable);
	BOOL SetInputOffset(double dblOffset);
private:
	COplkGPIBDevice *m_DevHandle;
	COpComm *m_DevComHandle;
    pstDeviceSetInfo m_pDevInfo;
};

#endif // !defined(AFX_HP8156A_H__B138F563_E908_4FCC_B41C_DFAE8CD06B0D__INCLUDED_)
