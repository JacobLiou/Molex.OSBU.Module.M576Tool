// OpHP816XTLS.h: interface for the COpHP816XTLS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPHP816XTLS_H__7DBC273E_D6A6_405B_92C8_C72FE9BFE987__INCLUDED_)
#define AFX_OPHP816XTLS_H__7DBC273E_D6A6_405B_92C8_C72FE9BFE987__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "op816x.h"


class  COpHP816XTLS  
{
public:
	bool SetPMLogStop(int PWMChannel);
	bool SetPWMRange(long lSlotIndex, long lChannelIndex,double dblPwrRange);
	bool GetTLSHasPM();
	bool GetReadSlotAndChannel(int nCurChannel,int &nSlot,int& nChannel);
	bool GetPMRealChannelNum(int &nCount);
	bool GetDeviceSlotNumber(pstSlotInfo pSlotInfo,int& nSlotNum);
	bool CloseDevice();
	void SetHasPM(bool bPM);
	bool SetTrigIngnore(int nChannel);
	bool PrepareSenseScan(long lScanParam);
	bool SetPWMParameters(long lSlotIndex, long lChannelIndex, bool bAutoRange, double dblWavelength, double dblAvgTile, double dblPwrRange);
	bool SetPWMWavelength(long lSlotIndex, long lChannelIndex, double dblWavelength);
	bool SetPWMUnit(long lSlotIndex, long lChannelIndex, long lPWMUnit);
	bool SetPWMRangeMode(long lSlotIndex, long lChannelIndex, long bAtuo);
	bool SetPWMAverageTime(long lSlotIndex, double dblAvgTime);
	bool ReadPower(long lSlotIndex, long lChannelIndex, double* pdblPowerValue);
	void SetHasOriginalData(bool bHasData,char* lpstrFilePath);
	bool getLambdaScanPMResult(int PWMChannel,double* pdbPMArry,double *pdbWL,int nScanIndex,WORD wRangeDecrement);
	bool setInitialRangeParams(double dbInitalRange,WORD wRangeDecrement);
	bool SetPMScanBegin();
	bool GetLamdaScanWLResult(double *pdbWLAarry);
	bool SetScanStart();
	bool PrepareScan(long lScanParam);
	bool SetTLSParameters(bool bLowSEE, bool bLaserOn, double dblPower, double dblWavelength);
	bool SetTLSCoherenceControl(bool bEnable);
	bool SetTLSWavelength(double dblWavelength);
	bool SetTLSPower(double dbPower);
	bool GetTLSAttenuation(double& dbPower);
	bool SetTLSAttenuation(bool bAuto, double dblAtten);
	bool SetLaserOn(BOOL bLow=false);
	void SetTLSInfoAndCom( pDevice_TLSInfo pDevInfo,long lhandle);
	bool OpenDevice();
	COpHP816XTLS();
	virtual ~COpHP816XTLS();
private:
	int m_nSlotSum;
	int m_nChannelSum;
	stSlotInfo m_Slot[8];
	bool m_bHasPM;
	bool bHasSaveOriginalData;
	CString m_strFilePath;
//	int m_nTLSSampleCount;
	pDevice_TLSInfo  m_pTLSInfo;
	POp816XScanParam m_pScanParam;
	COplkGPIBDevice * m_DevHandle;
	COpComm  * m_DevComHandle;
	void YieldToPeers();
	bool GetGPIBEventRigister(long lhandle);
	bool CompleteQuery(long lhandle);


};

#endif // !defined(AFX_OPHP816XTLS_H__7DBC273E_D6A6_405B_92C8_C72FE9BFE987__INCLUDED_)
