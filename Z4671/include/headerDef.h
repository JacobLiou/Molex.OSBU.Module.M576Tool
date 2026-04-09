#ifndef  __DEFHEADER__
#define  __DEFHEADER__


#ifndef DLLEXPORT
#define DLLEXPORT   __declspec( dllexport )
#endif
#ifndef DllImport
#define DllImport   __declspec( dllimport )
#endif

#include "OplkGPIB.h"
#include "OpComm.h"
#include "Decl-32.h"
#include <vector>
#include "math.h"
using namespace std;
#pragma once 

#define	OSA_ADDRESS					23
#define ATTEN_ADDRESS				28
#define	PM_ADDRESS					20       
//#define OSA_SCAN_BANDWIDTH          0.2
#define	SRC_SCAN							0
#define	AMP_SCAN							1
#define MAX_SCAN_DATA						5001
#define OSA_SCAN_MIN_WAVE                   1525
#define OSA_SCAN_MAX_WAVE                   1565
#define SCAN_BW_SEN                         0.2
#define MANUAL_OFFSET                       0.6
#define SCAN_SENSITIVITY                    -70
#define INCLUDE_SHOT_NOISE                  0   // 0: Not include shot noise// 1: Include shot noise
#define EXCLUDE_SRC_ASE_POWER               1   // 0: Don't care source(input power) ASE power// 1: Noise figure exclude source ASE power
#define RBW_CALC_MODE                       0   // 0: Average Measure RBW// 1: Actual Measure RB // 2: Scan Setting RBW
#define AUTO_MODE                           1   // Interpolation offset, auto or manual specific value
#define SOURCE_SIGNAL_OFFSET                0
#define AMP_SIGNAL_OFFSET                   0
#define SCAN_WL_STEP                        0.001


//初始化TLS扫描信息结构体
typedef struct tagOp816XScanParam
{
	//	ScanParameter members are in nm units.
	//	Oplk816x will transfer them into m (by 1e-9)
	DWORD	m_dwSize;                                   // 结构体大小
	double	m_dblStartWL, m_dblStopWL, m_dblStepSize;   //扫描启止波长和步长
	double	m_dblTLSPower, m_dblPWMPower; 		       //扫描TLS输出功率和功率计接收功率
	DWORD	m_dwNumberOfScan;                          //扫描次数
	DWORD	m_dwChannelNumber;                         //通道数
	DWORD	m_dwChannelCfgHigh, m_dwChannelCfgLow;     //通道是否启用
	DWORD	m_dwSampleCount;                           //扫描点数
} stOp816XScanParam, *POp816XScanParam;


//输出TLS扫描数据结构体
typedef struct tagOp816XRawData
{
	double		*m_pdblWavelengthArray;  //输出波长
	PDWORD		m_pdwDataArrayAddr; //扫描功率
} stOp816XRawData, *POp816XRawData;

typedef	struct tagAutoOSAData
{
	WORD   wTotalChannelCount;      //光源总数
	double dblChWL[128];               //通道波长
	double dblChSrcPwr[128];           //通道输入功率
	double dblChGain[128];             //通道增益
	double dblChOutPwr[128];           //通道输出功率
	double dblChAsePwr[128];           //通道ASE
	double dblChNF[128];              //通道NF
	double dblInputSingalPower;        //输入总功率
	double dblOutputSingalPower;      //输出总功率
	double dblGainRipple;             //增益Ripple
	double dblGainFlatness;           //增益平坦度
	double dblMaxNF;                //最大NF
	double dblMaxGain;              //最大增益
	double dblMinGain;              //最小增益
	double dblSumGain;             //总增益
} STAutoOSAData, *PAutoOSAData;

typedef enum{GPIB,SERIAL,USB,NET} enComType;
typedef enum{HP8164,OPTLS,SANTEC,HP8163,N7745PM,PM1830C,HP8164PM,HP8156A,OPVOA,HP86142B,AQ6370C,AQ6370CNET} DeviceCaption;
typedef enum{TLSDEVICE,PMDEVICE,VOADEVICE,OSADEVICE,SWITCHDEVICE} DeviceType;
//OPLINK自制VOA类型
typedef enum{JIXIE,MEMS,EVOA} opVoaType;

typedef struct tagDeviceInfo
{
   BOOL bHasDevice;          //设备是否启用
   DeviceType deType;        //设备类型
   DeviceCaption deCaption;  //设备名称
   BOOL bLaserLow;           //如果设备名称为HP8164,初始化时候需要设置该参数
   enComType  nComType;      //通讯方式
   int nAddress;             //地址,当使用网络通信时需要使用inet_addr()函数转换地址
   int nGPIBIndex;           //如果是GPIB通信设置GPIB地址索引号
   DWORD dwBaud;                //如果是串口通信设置波特率
   char szDescription[MAX_PATH];  //设备描述
}stDeviceSetInfo,*pstDeviceSetInfo;


typedef  stDeviceSetInfo Device_TLSInfo;
typedef  pstDeviceSetInfo pDevice_TLSInfo;
typedef  stDeviceSetInfo Device_PMInfo;
typedef  pstDeviceSetInfo pDevice_PMInfo;


#define SWEEPPOINT 5001

#define dbTomw(dbm) pow(10,double(dbm)/10)
#define mwTodb(mw) (((double)log10(mw))*10)

typedef enum {Ag81619A,Ag81618A,HP81533};

typedef struct tagSlotInfo
{
	int nSlot;            //槽位号
	int nType;           //探头型号
	int nChannelNum;    //通道号
	int nSlotChannel;    //槽位通道号
}stSlotInfo,*pstSlotInfo;

typedef struct tagPMInfo
{
	int nSensIndex;     //通道号
	int nChannelIndex;  //通道索引号 
} STPMInfo, *PPMInfo;

#endif


