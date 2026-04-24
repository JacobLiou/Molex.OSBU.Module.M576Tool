// OpOSAControl.h: interface for the COpOSAControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPOSACONTROL_H__2AB30215_EB73_4ACA_AA9B_1A8B0A8F606A__INCLUDED_)
#define AFX_OPOSACONTROL_H__2AB30215_EB73_4ACA_AA9B_1A8B0A8F606A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "headerDef.h"
#include "OSA_AQ6370B.h"
#include "OSA_HP86142.h"
#include "OSAAQ63XXCtrl.h"

class DLLEXPORT COpOSAControl  
{
public:

	//设备控制函数
	//打开设备
	BOOL OpenDevice();
	//设置OSA设备结构体
	void SetOSAInitial(pstDeviceSetInfo pOSAInfo);
    //关闭设备指令
	void CloseDevice();
    //进行WL校准
	BOOL SetCalWL(double dbInputWL, double &dbGetCaWL);
	//进行POWER校准
	BOOL SetCalPower(double dbInputWL,double dbInputPower,double &dbGetCaPower);

	//OSA放大功能函数
	//初始化方法
    BOOL InitAmpTest(double dblStartWL,double dblStopWL,double dblBWSen);
	//扫放大
	BOOL MeasureAmplifier();
	//获得EDFA所有扫描结果
	BOOL GetAllEDFAResult(PAutoOSAData pOSAResult);
	//设置NF算法
	BOOL SetNFClaType(BOOL bNFCal=FALSE);
	//获得SWeep的范围
	BOOL SetSweepRange(double dblStartWL,double dblStopWL);
	//获得通道的总数
	BOOL GetChannelCount(WORD* pwChCount);
	//获得通道波长
	BOOL GetChannelWL(double *pdblWL,WORD* pwChCount);
	//获得通道NF
	BOOL GetChannelNF(double *pdblNF,WORD* pwChCount);
	//获得通道增益
	BOOL GetChannelGain(double *pdbGain,WORD*pwChCount);
	//获得通道源功率
	BOOL GetChannelSourecePower(double*pdblSrcPwr,WORD*pwChCount);
	//获得放大的Offset
	BOOL GetAmpOffset(double &pdblAmpOffset);
	//获得源的Offset
	BOOL GetSourceOffset(double &pdblSrcOffset);
	//扫源
	BOOL MeasureSource();
	//获得EDFA结果包括总输入、总输出、总增益
	BOOL GetEDFAResult(double *pdblSumSrcSigPwr,double *pdblSumAmpSigPwr,double *pdblSunGain);
	//设置Offset
	BOOL SetOffset(BOOL bSource,double dblOffset);

    //OSA普通扫描模式
	BOOL  InitSingleScan(double dblStartWL, double dblStopWL, double dblRBW, double dblSens, int nScanPoint);	
	BOOL  GetSingleSweepResult(double *pdbWL,double *pdbPower);
    //获得最大功率点
	BOOL    GetMaxPoint(float *pPower);
	//获得中心波长和SMSR、RBW
	BOOL    GetCenterWLandSMSRandBW(float *pcenterwl,float *pSMSR,float *pBW);
	//标记最大点
	BOOL    MarkMaxPoint();
	//设置中心波长
	BOOL    SetCenterWL(float pcenterwl);
	//退出App
	BOOL    ExitApp();
	//Sweep一次
	BOOL    DoSingleSweep();
	//选择应用模式
	BOOL    SelectApp(int AppIndex);
    //设置RBW
	BOOL    SetResolutionBW(float ResolutionBW);
	//设置灵敏度
	BOOL    SetSensitivity(int Sensitivity);
	//设置带宽
 	BOOL    SetSpan(int SpanWL);

	COpOSAControl();
	virtual ~COpOSAControl();
private:
	BOOL GetDeviceType(pstDeviceSetInfo pstDevInfo,long m_Com);
	COSA_HP86142 * m_OSAHP81642;
	COSA_AQ6370B * m_OSAAQ6370B;
	COSAAQ63XXCtrl *m_OSAAQ6370BNET;

	pstDeviceSetInfo m_pDevInfo;
	COplkGPIBDevice * m_OSAGPIB;
};

#endif // !defined(AFX_OPOSACONTROL_H__2AB30215_EB73_4ACA_AA9B_1A8B0A8F606A__INCLUDED_)
