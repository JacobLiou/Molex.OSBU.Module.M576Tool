// OpTLS.h: interface for the COpTLS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTLS_H__C8194155_5E5D_456A_A244_E1E17FC0BAF6__INCLUDED_)
#define AFX_OPTLS_H__C8194155_5E5D_456A_A244_E1E17FC0BAF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "op816x.h"


class  COpTLS  
{
public:
	bool CloseDevice();
	bool SetTLSSourcePower(double dblPower);
	bool SetTLSWavelength(long lFrequency);
	bool SetTLSWavelength(double dblWavelength);
	bool GetDeviceConfig();
	bool OpenDevice();
	void SetTLSInfoAndCom( pDevice_TLSInfo pDevInfo,long lhandle);
	COpTLS();
	virtual ~COpTLS();
protected:
	pDevice_TLSInfo  m_pTLSInfo;
	COpComm *m_OpCom;
	double m_dblMinPower;
	double m_dblMaxPower;
	long m_lMinChannelNum;
	long m_lMaxChannelNum;
	long m_lMinFrequency;
	long m_lMaxFrequency;
	long m_lFrequencyGrid;

};

#endif // !defined(AFX_OPTLS_H__C8194155_5E5D_456A_A244_E1E17FC0BAF6__INCLUDED_)
