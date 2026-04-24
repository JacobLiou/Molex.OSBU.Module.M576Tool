// HP816XPM.h: interface for the CHP816XPM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HP816XPM_H__1AB976B1_EE6E_4D07_BB0C_8EDFBA82C6F9__INCLUDED_)
#define AFX_HP816XPM_H__1AB976B1_EE6E_4D07_BB0C_8EDFBA82C6F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "op816x.h"

class  CHP816XPM  
{
public:
	bool GetScanState();
	bool SetPMLogStop(int PWMChannel);
	bool CompleteQuery(long lpGPIB);
	bool GetGPIBEventRigister(long lpGPIB);
	bool SetPWMRange(long lSlotIndex, long lChannelIndex,double dblPwrRange);
	bool GetReadSlotAndChannel(int nCurChannel,int &nSlot,int& nChannel);
	bool GetPMRealChannelNum(int &nCount);
	void TLSHasPM(bool bHasPM,int nSlotCount);
	bool CloseDevice();	
	bool SetTrigIngnore(int nChannel);
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
	bool PrepareScan(long lScanParam);
	bool GetDeviceSlotNumber(pstSlotInfo pSlotInfo,int nSlotNum);
	bool OpenDevice();
	void SetPMInfoAndCom( pDevice_PMInfo pDevInfo,long lhandle);
	CHP816XPM();
	virtual ~CHP816XPM();
private:
	int m_nChannelSum;
	bool m_bTLSHasPM;
	int m_TLSChannelCount;
	bool bHasSaveOriginalData;
	CString m_strFilePath;
	int m_nSlotSum;
	int m_nPMSampleCount;
	pDevice_PMInfo  m_pPMInfo;
	stSlotInfo m_Slot[64];
	POp816XScanParam m_pScanParam;
	COplkGPIBDevice * m_DevHandle;
	COpComm *m_DevComHandle;

};

#endif // !defined(AFX_HP816XPM_H__1AB976B1_EE6E_4D07_BB0C_8EDFBA82C6F9__INCLUDED_)
