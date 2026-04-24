// op816x.h : main header file for the OP816X DLL
//

#if !defined(AFX_OP816X_H__3F92E0F9_F607_4E8D_BCD6_13A3A66195F3__INCLUDED_)
#define AFX_OP816X_H__3F92E0F9_F607_4E8D_BCD6_13A3A66195F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "math.h"
#include "headerDef.h"
//#include "datatype.h"
#include "OpHP816XTLS.h"
#include "HP816XPM.h"
#include "N7745PM.h"
#include "SantecTLS.h"
#include "OpTLS.h"
#include "PM1830C.h"
using namespace std;
#define AUTO_IDN


/////////////////////////////////////////////////////////////////////////////
// COp816xApp
// See op816x.cpp for the implementation of this class
//

class COp816xApp : public CWinApp
{
public:
	COp816xApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COp816xApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(COp816xApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class DLLEXPORT COp816X
{
public:
	COp816X();
	~COp816X();
public:
	//扫描参数结构体
	POp816XScanParam	m_pScanParam;	
public:
	BOOL Zero(long lSlotIndex);
	//设置是否输出原始数据，设置为true表示输出，设置false表示输出差值数据
	void SetOutPutOriginalData(bool bHasOutPut);
	//目前扫描默认设置两次，该函数设置是否保存两次扫描原始数据
	//保存数据文件格式为: 1_ScanData_0_09 _20_03.csv
	//                    通道号_ScanData_扫描次数_扫描时间.csv        
	void SetHasOriginalData(bool bHasData,char* lpstrFilePath);
	//初始化激光器结构体，这里的nDeviceCount为设备台数，这里默认为1，目前只能设置为1
	bool InitialTLS(pDevice_TLSInfo pSettingInfo,int nDeviceCount=1);
	//初始化功率计，这里的nDeviceCount为设备台数，这里默认为1，目前只能设置为1
	bool InitialPM(pDevice_PMInfo pSettingInfo,int nDeviceCount=1);
	//打开设备
	bool OpenDevice();
	//关闭设备
	bool CloseDevice();
	//设置扫描时多次扫描功率计Range递减变化量，默认值为20
	void SetScanPMClippingLimit(WORD wClippingLimit= 20);
	//执行扫描
	bool ExecuteScan(long lRawDataAddr);
	//清空设备
	bool ClearDevice();
	//获得消息错误函数，目前函数没有实现功能
	bool GetErrorMessage(long lMessageAddr);
	//初始化扫描参数
	bool PrepareScan(long lScanParam);
	//读取功率函数,如果激光器带有功率计模块则读取功率计需要
	//按照从激光器功率计模块通道开始算起，读取功率计需要加上
	//激光器的功率模块通道，
    //如果激光器不带有功率计模块，则按照正常功率计模块通道读取
	bool ReadPower(long lSlotIndex, long lChannelIndex, double* pdblPowerValue);
	//目前函数没有实现
	bool ZeroAll(void);
	//设置激光器功率
	bool SetTLSPower(double dblPower);
	//设置功率计Range模式
	bool SetPWMRangeMode(long lSlotIndex, long lChannelIndex, long bAtuo);
	//设置功率计单位wm或dBm
	bool SetPWMUnit(long lSlotIndex, long lChannelIndex, long lPWMUnit);
	//设置激光器波长
	bool SetTLSWavelength(double dblWavelength);
	//目前函数没有实现
	bool PrepareFastSweep(long lScanParam);
	//目前函数没有实现
	bool ExecuteFastSweep(long lScanData);
	//目前函数没有实现
	bool CloseFastSweep(void);
	//目前函数没有实现
	bool SetMaxOutput(double dblWavelength);
	//目前函数没有实现
	bool GetMaxMinPower(long lSlotIndex, long lChannelIndex, long lDuration, double* pdblMaxPower, double* pdblMinPower);
	//设置功率计的监控时间
	bool SetPWMAverageTime(long lSlotIndex, double dblAvgTime);
    //设置功率计的参数，包括设置Range 模式、波长、监控时间、功率Range
	bool SetPWMParameters(long lSlotIndex, long lChannelIndex, bool bAutoRange, double dblWavelength, double dblAvgTile, double dblPwrRange);
	//如果功率计有衰减器设置功率计衰减器
	bool SetTLSAttenuation(bool bAuto, double dblAtten);
	//设置激光器一致控制
	bool SetTLSCoherenceControl(bool bEnable);
	//设置功率计波长
	bool SetPWMWavelength(long lSlotIndex, long lChannelIndex, double dblWavelength);
	//设置激光器参数包括，使用高功率还是低功率、输出功率、波长
	bool SetTLSParameters(bool bLowSEE, bool bLaserOn, double dblPower, double dblWavelength);
protected:
	bool CompleteQuery(long lpGPIB);
	bool GetGPIBEventRigister(long lhandle);
	bool GetAverResult(double *pdbSrx,double* pdbSrcY,int nCount);
	double Sum(double* srcX,int nCount);
	double SumProduct(double* srcX,double *srcY,int nCount);
	bool GetDeviceType(pstDeviceSetInfo pstDevInfo,long m_Com);
	bool OpenTLS();
	bool OpenPMDevice();
	bool YieldToPeers();
	bool GetPMArray(double *pdbFirstArry,double* pdbSecondArry,int nRange,int nPointCount);
    bool CalculateScanData(long lpScanDataAddr,vector< vector<double> >vdbPMArry,vector<double>dbWLArry);
	bool LinearFitRaw(double *pdblSrcX, double *pdblSrcY, int nSrcLen, double *pdblTarX,double *pdblTarY);
protected:
	bool bGetOriginalData;
	WORD m_PMClippingLimit;       
	bool m_bTLSHasPM;             //判断TLS是否有功率模块
	bool m_bHasData;              //是否要保存原始数据
	int nTLSPMSlotNum;            //HP8164有几个功率计模块
	CString m_FilePath;           //扫描原始数据路径
	COplkGPIBDevice m_TLSGPIB;  //激光器GPIB类指针
    COplkGPIBDevice m_PMGPIB;   //功率计GPIB类指针
	COpComm  m_TLSSerial;       //激光器串口类指针
	COpComm  m_PMSerial;        //功率计串口类指针
	Device_TLSInfo  m_TLSInfo;   //激光器设备结构体
	Device_PMInfo  m_PMInfo;     //功率计设备结构体	
	COpHP816XTLS * m_HP816XTLS; //Aglient激光器
	COpTLS  *m_opTLs; //自制激光器
	CHP816XPM * m_HP816XPM;//Aglient功率计
	CN7745PM  * m_N7745PM;//N7745功率计
	CSantecTLS * m_SantecTLS;//santec激光器
	CPM1830C   *m_PM1830;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OP816X_H__3F92E0F9_F607_4E8D_BCD6_13A3A66195F3__INCLUDED_)
