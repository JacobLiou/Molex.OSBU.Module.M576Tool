// N7745PM.h: interface for the CN7745PM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N7745PM_H__8911F5BB_0BBA_42CC_9BC0_B4B8E3E2CA23__INCLUDED_)
#define AFX_N7745PM_H__8911F5BB_0BBA_42CC_9BC0_B4B8E3E2CA23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "op816x.h"

class  CN7745PM  
{
public:
	BOOL Zero(long lSlotIndex);
	bool SetPMLogStop(int PWMChannel);
	bool CompleteQuery(long lpGPIB);
	bool GetGPIBEventRigister(long lpGPIB);
	void TLSHasPM(bool bHasPM,int nSlotCount);
	bool setInitialRangeParams(double dbInitalRange,WORD wRangeDecrement);
	bool SetTrigIngnore(int nChannel);
	bool CloseDevice();
	bool SetPWMParameters(long lSlotIndex, long lChannelIndex, bool bAutoRange, double dblWavelength, double dblAvgTile, double dblPwrRange);
	bool SetPWMWavelength(long lSlotIndex, long lChannelIndex, double dblWavelength);
	bool SetPWMUnit(long lSlotIndex, long lChannelIndex, long lPWMUnit);
	bool SetPWMRangeMode(long lSlotIndex, long lChannelIndex, long bAtuo);
	bool SetPWMAverageTime(long lSlotIndex, double dblAvgTime);
	bool ReadPower(long lSlotIndex, long lChannelIndex, double* pdblPowerValue);
	void SetHasOriginalData(bool bHasData,char* lpstrFilePath);
	bool getLambdaScanPMResult(int PWMChannel,double* pdbPMArry,double *pdbWL,int nScanIndex,WORD wRangeDecrement);
	bool SetPMScanBegin();
	bool PrepareScan(long lScanParam);
	bool OpenDevice();
	void SetPMInfoAndCom( pDevice_PMInfo pDevInfo,long lhandle);
	CN7745PM();
	virtual ~CN7745PM();
private:
	bool m_bTLSHasPM ;
    int m_TLSChannelCount;
	bool bHasSaveOriginalData;
	CString m_strFilePath;
	int m_nPMSampleCount;
	pDevice_PMInfo m_pPMInfo;
	POp816XScanParam m_pScanParam;
	COplkGPIBDevice * m_DevHandle;
	void YieldToPeers();

};

#endif // !defined(AFX_N7745PM_H__8911F5BB_0BBA_42CC_9BC0_B4B8E3E2CA23__INCLUDED_)
