// OSA_HP86142.h: interface for the COSA_HP86142 class.
//Reference: HP86142 User guide
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OSA_HP86142_H__6F08D52B_DF48_4ACA_9744_A879E3696C74__INCLUDED_)
#define AFX_OSA_HP86142_H__6F08D52B_DF48_4ACA_9744_A879E3696C74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "op816x.h"
#include "NFCalArithmetic.h"


class  COSA_HP86142  
{
public:
	COSA_HP86142();
	virtual ~COSA_HP86142();
    //设备设置指令
	BOOL    SetNFCalType(BOOL bIsAmp);
    void    SetOSADevInfo(pstDeviceSetInfo pVOAInfo,long m_DevCom);
	void	CompleteRequest();
	BOOL	SetOSALocalCtrl();
	//用以OSA校准使用
	BOOL    SetCalWL(double dbInputWL, double &dbGetCaWL);
	BOOL    SetCalPower(double dbInputWL,double dbInputPower,double &dbGetCaPower);
	// Aglient的各种模式{OSA:0,Filter:1,PowerMeter:2,PassiveComponent:3,WDM_Autoscan:4,Amp_ISS_Test:5 }
 	BOOL    SelectApp(int AppIndex);
    //放大模式指令
	BOOL	InitAmpTest(double dblStartWL, double dblStopWL, double dblBWSen);
    BOOL	SetOffset(BOOL bSource, double dblOffset);
	BOOL	MeasureSource();
	BOOL	MeasureAmplifier();	
	BOOL	GetEDFAResult(double *pdblSumSrcSigPwr, double *pdblSumAmpSigPwr, double *pdblSunGain);
	BOOL    GetAllEDFAResult(PAutoOSAData pOSAResult);	
	BOOL	ExitAmpMode();
	BOOL	GetSourceOffset(double * pdblSrcOffset);
	BOOL	GetAmpOffset(double * pdblAmpOffset);
	BOOL	GetChannelSourcePower(double* pdblSrcPwr, WORD* pwChCount);
	BOOL	GetChannelGain(double* pdblGain, WORD* pwChCount);
	BOOL	GetChannelNF(double* pdblNF, WORD* pwChCount);
	BOOL	GetChannelWL(double* pdblWL, WORD* pwChCount);
	BOOL	GetChannelCount(WORD *pwChCount);
    //普通Sweep模式
	BOOL	InitSingleScan(double dblStartWL, double dblStopWL, double dblRBW, double dblSens, int nScanPoint);	
	BOOL    GetSingleSweepResult(double *pdbWL,double *pdbPower);
	BOOL	SetSweepRange(double dblStartWL, double dblStopWL);	
	BOOL	SetScanOnce(int nScanPoint, double *pdblScanData);
	BOOL    SetResolutionBW(float ResolutionBW);
	BOOL    SetSensitivity(int Sensitivity);
 	BOOL    SetSpan(int SpanWL);
	BOOL    GetOSAScancfg();
	BOOL    DoSingleSweep();
	BOOL    GetMaxPoint(float *pPower);
 	BOOL    GetCenterWLandSMSRandBW(float *pcenterwl,float *pSMSR,float *pBW);
 	BOOL    MarkMaxPoint();
 	BOOL    SetCenterWL(float pcenterwl);
private:
	BOOL m_NFCalType;
	COplkGPIBDevice *m_DevHandle;
	pstDeviceSetInfo m_pDevInfo;
	CString		*m_pstrLogFileName;
	stScanSetting	m_stScanSetting;
	stEDFAResult	m_stEDFAResult;
	double	m_pdblScanData[2][MAX_SCAN_DATA];	
	CNFCalArithmetic  m_NFCal;
};

#endif // !defined(AFX_OSA_HP86142_H__6F08D52B_DF48_4ACA_9744_A879E3696C74__INCLUDED_)
