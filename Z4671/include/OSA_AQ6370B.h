// OSA_AQ6370B.h: interface for the COSA_AQ6370B class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OSA_AQ6370B_H__5F5F704A_3AC4_4C55_A664_32D665D9E2B1__INCLUDED_)
#define AFX_OSA_AQ6370B_H__5F5F704A_3AC4_4C55_A664_32D665D9E2B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "op816x.h"
#include "NFCalArithmetic.h"

class COSA_AQ6370B 
{
public:
	//设备设置指令
	BOOL SetNFCalType(BOOL bIsAmp);
    void SetOSADevInfo(pstDeviceSetInfo pVOAInfo,long m_DevCom);
	BOOL CloseOSA();
	BOOL GetGPIBAress(int *dblGpibAddress);
	BOOL ResetOSA();//Executes a device reset to return the instrument to the known (default) status.
	BOOL SetOSALocalCtrl();//Switches to Local mode.
	void CompleteRequest();//Check if the OSA busy,if busy then wait.
	//校准OSA时指令,参数无用，只是与Aglient接口一致
	BOOL SetCalWL(double dbInputWL, double &dbGetCaWL);
	


	//放大模式指令
	BOOL InitAmpTest(double dblStartWL,double dblStopWL,double dblRBW);//Set the OSA to Amp test mode and init the WL,RBW,sense.	
	BOOL SetOffset(BOOL bSource, double dblOffset);
	BOOL SetAmpSrcSigOffset(double dblSrcSigOffset);//Set level offset values (signal light) for the NF analysis function.
	BOOL GetAmpSrcSigOffset(double* pdblSrcSigOffset);//Get level offset values (signal light) for the NF analysis function.
	BOOL SetAmpOutSigOffset(double dblOutSigOffset);//Set level offset values (output light) for the NF analysis function.
	BOOL GetAmpOutSigOffset(double* pdblOutSigOffset);//Get level offset values (output light) for the NF analysis function.
	BOOL MeasureSource();
	BOOL MeasureAmplifier();
	BOOL GetChannelCount(WORD* pdwChannelCount);//Get the number of channels of the WDM,EDFA-NF, WDM FIL-PK,or WDM FIL-BTM analysis results.
	BOOL GetChannelSNR(double* pdblSNR,WORD* pdwChannelCount);//Get the gain value of the EDFA-NF analysis results.
	BOOL GetChannelWL(double* pdblWavelens,WORD* pdwChannelCount);//Get the wavelength value of the WDM,EDFA-NF, WDM FIL-PK,or WDM FIL-BTM analysis results.
	BOOL GetChannelNF(double* pdblNF,WORD* pdwChannelCount);//Get the NF value of the EDFA-NF analysis results.
	BOOL GetChannelGain(double* pdblGain,WORD* pdwChannelCount);//Get the gain value of the EDFA-NF analysis results.
	BOOL GetChannelSourcePower(double* pdblSourcePow,WORD* pdwChannelCount);//Get the level value of the WDM, EDFANF,WDM FIL-PK, or WDM FIL-BTM analysis results.	
	BOOL GetEDFAResult(char* pszResult,USHORT uiLength);
	BOOL GetEDFAResult(double *pdblSumSrcSigPwr, double *pdblSumAmpSigPwr, double *pdblSunGain);
	BOOL GetAllEDFAResult(PAutoOSAData pOSAResult);
	BOOL StartAnalysis();
	BOOL SetAnalysisType(WORD wAnalysisType = 12);//Set and start the type of analysis mode.

	//普通扫描模式
	BOOL InitSingleScan(double dblStartWL, double dblStopWL, double dblRBW, WORD dwSweepPoint);//Init the OSA before performing single scan.	
    BOOL GetSingleSweepResult(double *pdbWL,double *pdbPower);
	BOOL SetScanOnce(int nScanPoint, double *pdblScanData);
	BOOL  GetOSAScancfg();
	BOOL SetSweepRange(double dblStartWL,double dblStopWL);//set the start wavelength and stop wavelength	
	BOOL SetRBW(double dblRBW);//set RBW on the OSA
	BOOL GetRBW(double* pdblRBW);//Get RBW on the OSA
	BOOL SetPowerOffset(double dblOSAPowOffset);//Set the offset value for the level.
	BOOL GetPowerOffset(double* pdblOSAPowOffset);//Get the offset value of the level.
	BOOL SetWavelengthOffset(double dblWLOffset);//Set the offset value for the level wavelength.
	BOOL GetWavelengthOffset(double* pdblWLOffset);//Get the offset value of the level wavelength.
	BOOL SetWLOffsetTable(USHORT usWavelength,double dblWLOffset);//Set the wavelength offset table.
	BOOL GetWLOffsetTable(USHORT usWavelength,double* pdblWLOffset);//Get the wavelength offset table.
	BOOL SetPowOffsetTable(USHORT usWavelength,double dblPowOffset);//Set the level offset table.
	BOOL GetPowOffsetTable(USHORT usWavelength,double* pdblPowOffset);//Get the level offset table.	
	BOOL RunSweep();//perform a sweep and get the level axis data of specified trace.
	BOOL SetTraceAct(char szSelTrace);//set which trace active
	BOOL SetTraceAttribute(char szSelTrace,USHORT uiAttribute);//set the attribute of the given trace
	COSA_AQ6370B();
	virtual ~COSA_AQ6370B();

protected:
	int	   m_iChCount;
    pstDeviceSetInfo m_pDevInfo;
	COplkGPIBDevice *m_devComHandle;
	BOOL m_NFCalType;
	stScanSetting	m_stScanSetting;
	stEDFAResult	m_stEDFAResult;
	double	m_pdblScanData[2][MAX_SCAN_DATA];	
	CNFCalArithmetic  m_NFCal;

};

#endif // !defined(AFX_OSA_AQ6370B_H__5F5F704A_3AC4_4C55_A664_32D665D9E2B1__INCLUDED_)
