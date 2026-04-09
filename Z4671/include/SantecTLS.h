// SantecTLS.h: interface for the CSantecTLS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SANTECTLS_H__6C16B3CB_51D0_4744_BA47_E7EE6A9BCD0A__INCLUDED_)
#define AFX_SANTECTLS_H__6C16B3CB_51D0_4744_BA47_E7EE6A9BCD0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "op816x.h"


class  CSantecTLS  
{
public:
	bool CloseDevice();
	bool GetLamdaScanWLResult(double *pdbWLAarry);
	bool SetScanStart();
	bool PrepareScan(long lScanParam);
	bool SetTLSParameters(bool bLowSEE, bool bLaserOn, double dblPower, double dblWavelength);
	bool SetTLSCoherenceControl(bool bEnable);
	bool SetTLSWavelength(double dblWavelength);
	bool SetTLSPower(double dbPower);
	bool GetTLSAttenuation(double& dbPower);
	bool SetTLSAttenuation(bool bAuto, double dblAtten);
	bool SetLaserOn();
	void SetTLSInfoAndCom( pDevice_TLSInfo pDevInfo,long lhandle);
	bool OpenDevice();
	CSantecTLS();
	virtual ~CSantecTLS();
	
private:
	pDevice_TLSInfo  m_pTLSInfo;
	POp816XScanParam m_pScanParam;
	COplkGPIBDevice * m_DevHandle;
	void YieldToPeers();
	bool GetGPIBEventRigister(long lpGPIB);
	bool CompleteQuery(long lpGPIB);
};

#endif // !defined(AFX_SANTECTLS_H__6C16B3CB_51D0_4744_BA47_E7EE6A9BCD0A__INCLUDED_)
