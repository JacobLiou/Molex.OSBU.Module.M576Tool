 // Command.h: interface for the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMAND_H__CC60B09C_75AF_4620_B090_15FCE542EAD4__INCLUDED_)
#define AFX_COMMAND_H__CC60B09C_75AF_4620_B090_15FCE542EAD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpComm.h"
#include "headerDef.h"
class CCommandAtten  
{
public:
	CCommandAtten();
	virtual ~CCommandAtten();
	void SetVOADevInfo(pstDeviceSetInfo pVOAInfo,long lhanle);
	//…Ë÷√OPlink VOA¿‡–Õ
	void SetOpVOAType(opVoaType voatype);
	BOOL OpenDevice();
	BOOL VoaMoveStep(BYTE byVoaCH, BYTE byVoaMoveDirection, int nVoaMoveSteps);
	BOOL ReadVoaADC(BYTE byVoaCH, int *nVoaADC);
	BOOL SetVoaAtten(double dblVoaAtten);
	BOOL GetVoaAtten(double &dblVoaAtten);
private:
	pstDeviceSetInfo m_pDevInfo;
	opVoaType m_voatype;
	COpComm *m_OpCom;

};

#endif // !defined(AFX_COMMAND_H__CC60B09C_75AF_4620_B090_15FCE542EAD4__INCLUDED_)
