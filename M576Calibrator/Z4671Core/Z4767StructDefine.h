#ifndef  __MODULESTRUCT_HEADER__
#define  __MODULESTRUCT_HEADER__

typedef struct CheckInfo
{
	//MCS INFO
	CString strSN;
	CString strPN;
	CString strHWVer;
	CString strFWVer;
	CString strID;
	CString strSupplier;
	CString strMainVer;
	CString strBootVer;
	BYTE    byAlarm;

	//EDFA INFO
	CString strEDFAName;
	CString strEDFAFWVer;
	CString strEDFAHWVer;
	CString strEDFAMFGName;
	CString strEDFAPN;
	CString strEDFAPCBVer;
	CString strEDFAPCBAVer;
	CString strEDFABundleVer;
	CString strEDFABootVer;
	CString strEDFAMainVer;
	CString strEDFAFPGAVer;
	
}stCheckInfo;

//串口合集
typedef struct WorkStationConfig
{
	int nProductPort; //产品串口号
	int nDMPDBoxPort;
	int nAMPDBoxPort;
	int nSwitchBoxPort[4];
	int nSwitchBoxType[4];
	int nCirBoxPort; //循环箱串口号
	int nTLSPort;
	int nVOAPort;
	int nPMPort;
	double dblVOAAtten; //
}stWorkStationConfig;

//温度配置
//#define MAX_LINE 1024
#define MAX_DAC 4096
#define MAX_VOLTAGE 60000
#define TEMP_CALIB_NUM 4  //定义温度数 数据下载用
enum{ROOM_TEMP=0,LOW_TEMP,HIGH_TEMP,MID_TEMP,TEMP_COUNT};

enum{PD_CAL=0,PD_VERIFY,SW_CAL_DM,SW_CAL_AM,SW_CAL_MON,SW_SCAN_DM,SW_SCAN_AM,SW_SCAN_MON,
SW_IL_TEST,SW_WDL_TEST,TYPE_COUNT};

typedef struct TempConfigInfo
{
	double dblCirBoxSet[TEMP_COUNT]; //产品串口号
	double dblModuleTemp[TEMP_COUNT]; //模块温度允许+/-范围
	int    nModuleTargetTemp;
}stTempConfigInfo;

#define IN_PUT 0
#define OUT_PUT 1

#define PM_PART1 0
#define PM_PART2 1
#define PM_MONITOR 3

#define WL_OFFSET  0.01
#define TEST_PART 2
#define MAX_TEST_PORT 64
#define BLOCK_COUNT 3

#define PART_COUNT 3
typedef struct OpticalPathConfig
{
	int nPartCount; //
	int nInput[PART_COUNT];
	int nOutput[PART_COUNT];
}stOpticalPathConfig;

#define MAX_WL 10
typedef struct WLConfig
{
	double dblCalWL; 
	double nWLTestCount;
	double dblTestWL[MAX_WL];
}stWLConfig;

//定义个归零值的结构体，以方便调用
typedef struct tagDataRef
{
    double dblCenter[MAX_TEST_PORT][MAX_TEST_PORT];  //中心波长的归零值 1550 [前端开关序号][后端开关序号]
	double dblWDL[MAX_TEST_PORT][MAX_TEST_PORT][MAX_WL];  //WDL波长点对应的归零值
	double dblAfterSWBoxIL[MAX_TEST_PORT];
}stRefDaTa;

#define MAX_VOA_CAL_COUNT 100
#define MAX_VOA_PORT 8
#define VOA_CAL_RANGE  5 //VOA分段
typedef struct tagVOACal
{
	int nActualCalCount[MAX_VOA_PORT];
	int nDACRange; //定标范围
	int nStep;
	double pdblCalPower[MAX_VOA_PORT][MAX_VOA_CAL_COUNT];
	int pnCalDAC[MAX_VOA_PORT][MAX_VOA_CAL_COUNT];
	int pnCalDiffDAC[MAX_VOA_PORT][MAX_VOA_CAL_COUNT];
	double pdblCalIL[MAX_VOA_PORT][MAX_VOA_CAL_COUNT];
	double dblCalTemp; //定标温度
	int nSleepTime; //延迟时间

	int pnCOEF[MAX_VOA_PORT][VOA_CAL_RANGE][4];
	int nRangeCount; //分段
	double dblAttRange[VOA_CAL_RANGE];
}stVOACal;

#define MAX_VOA_CAL_COUNT 100
#define MAX_VOA_PORT 8
#define VOA_VER_RANGE  5 //分段判断
typedef struct tagVOAVerify
{
	int nActualVerCount;
	int nDACRange; //定标范围
	double pdblSetAtten[MAX_VOA_CAL_COUNT];
	double pdblActualAtten[MAX_VOA_PORT][MAX_VOA_CAL_COUNT];
    double pdblVerPower[MAX_VOA_PORT][MAX_VOA_CAL_COUNT];
	int pnDAC[MAX_VOA_PORT][MAX_VOA_CAL_COUNT];
	double dblVerTemp; //验证温度
	double dblMonitorPower;
	int    nSleepTime; //延迟时间
	double dblStep; //验证步进
	int    nVOARange; //VOA精度分段个数
	double dblAttRange[VOA_VER_RANGE];

	double pdblAccuracy[MAX_VOA_PORT][VOA_VER_RANGE];
	double dblMaxRange; //验证范围
	
}stVOAVerify;

typedef struct tagVOALimit
{
	double dblCalTemp[3]; //定标温度限制
	double dblVerTemp[3]; //验证温度限制
	double dblCALAtten[VOA_CAL_RANGE];
	double dblCOEF[MAX_VOA_PORT][VOA_CAL_RANGE][4]; //MAX_PORT

	double dblVerifyAtten[VOA_VER_RANGE];
	double dblAccuracy[MAX_VOA_PORT][VOA_VER_RANGE];
	
}stVOALimit;


//定义各个产品测试条件
#define MAX_PD_CAL_COUNT 30  //PD定标数量
#define PD_LUT_COEF 1000000
typedef struct tagPDCal
{
	double dblStartPower;
	double dblEndPower;
	double dblStep;
	int    nActualCalCount;
	int    nPDMaxValue;  //0dB衰减下ADC范围
	int    nPDMinValue;
	double pdblCalADC[MAX_PD_CAL_COUNT];
	double pdblCalPower[MAX_PD_CAL_COUNT];
	double dblTemperature;
	double dblK;
	double dblC;
	long   lK;
	long   lC;
}stPDCal;


//定义各个参数限制条件
#define PD_VER_RANGE  5 //分段判断
typedef struct tagPDLimit
{
	double dblTemperature[TEMP_COUNT]; 
	double dblSlope;
	double dblIntercept;
	double dblAccuracy[PD_VER_RANGE];
	double dblPower[PD_VER_RANGE];
}stPDLimit;

#define MAX_PD_VER_COUNT 50  //PD定标数量
typedef struct tagPDVer
{
	double dblStartPower;
	double dblEndPower;
	double dblStep;
	double dblTemperature;
	int    nActualVerCount;
	WORD   pwVerADC[MAX_PD_VER_COUNT];
	double pdblVerActualPower[MAX_PD_VER_COUNT];
	double pdblVerPDPower[MAX_PD_VER_COUNT];
	int    nPDRange; //PD精度分段个数
	double pdblAccuracy[PD_VER_RANGE];
}stPDVer;

typedef struct tagSwitchPara
{
	int nSwitchType;
	int nCHCount;//每个开关通道数
	int nSWCount[BLOCK_COUNT]; //DM,AM,MON开关数
	int nRow;   //定义开关图的行数
	int nLine;  //定义开关图的列数
	int nScanStep;//单点扫描步进
	int nDelayTime;
}stSWPara;

//定义扫描结构体
typedef struct tagScanPara
{
	int nBlock;
	int nSwitch;
	int nStartX;
	int nEndX;
	int nStartY;
	int nEndY;
	int nStep;
}stSWScanPara;

#define HITLESS_POINT_TOTAL 4096

#define MAX_SW_COUNT  34
#define PORT_MAX_COUNT 20 //最大通道数
#define MID_MAX_COUNT 12 //最大中转点数

typedef struct tagSwitchCal //定标数据
{
	int nPointAtten[BLOCK_COUNT];  //单点定标衰减值
	int nScanAtten[BLOCK_COUNT];   //扫描衰减值
	BOOL bHitless[BLOCK_COUNT][16]; //
}stSWCal;

#define HITLESS_MIN 500   //2016.11.3  ris
#define HITLESS_MAX 4000
#define HITLESS_TIME 55000

/*
//定义开关扫描结构体
typedef struct tagScanPara
{
	int nBlock;
	int nSwitch;
	int nStartX;
	int nEndX;
	int nStartY;
	int nEndY;
	int nStep;
}stSWScanPara;
*/

typedef struct tagHitlessPoint
{
	int dacX[HITLESS_POINT_TOTAL];
	int dacY[HITLESS_POINT_TOTAL];
}stHitlessPoint;

typedef struct tagSwitchXYPoint
{
	int dacX[PORT_MAX_COUNT];
	int dacY[PORT_MAX_COUNT];
	double dblIL[PORT_MAX_COUNT];
}stSwitchXYPoint;

typedef struct tagMidPoint
{
	int dacX[PORT_MAX_COUNT];
	int dacY[PORT_MAX_COUNT];
}stMidPoint;

typedef struct tagPortRange
{
	int nStartX;
	int nStopX;
	int nStartY;
	int nStopY;
}stPortRange;

typedef struct tagSWLimit
{
	int    nMaxVolatgeDiff;//与来料数据最大DAC差异值
	int    nSWCalADCRange; //开关定标时，PD ADC上报范围
	int    nMaxADCDiff;    //同一开关的ADC差异最大值
	////////////////////////
	double  dblDACX[PORT_MAX_COUNT];  //通道坐标范围
	double  dblDACY[PORT_MAX_COUNT];
	double  dblPDADC[PORT_MAX_COUNT];
	double  dblMidDACX[PORT_MAX_COUNT];  //通道坐标范围
	double  dblMidDACY[PORT_MAX_COUNT];
	double  dblCalTemp[TEMP_COUNT];  //定标温度要求
	double  dblHitless;
	double  dblMAPCH;

	/*
	double  dblWDL;
	double  dblTDL;
	double  dblIL;
	double  dblUniformity;
	*/
}stSWLimit;

typedef struct tagOpticalLimit
{
	double dblMaxIL[TEMP_COUNT][PORT_MAX_COUNT]; //WDL中的最大IL
	double dblIL[TEMP_COUNT][PORT_MAX_COUNT];
	double dblTDL[PORT_MAX_COUNT];
	double dblWDL[PORT_MAX_COUNT];
	double dblWDLIL[PORT_MAX_COUNT][MAX_WL];
	double dblUniIL;
}stOpticalLimit;

#define MAX_TEST_ITEM 1000

typedef struct tagTestItem
{
	int nIndex[MAX_TEST_ITEM]; //TaskIndex
	CString strItemName[MAX_TEST_ITEM];  //任务名称
}stTestItem;

#define AM_NUM 16
#define COEF_NUM 4
#define STAGE_NUM 2

//定义数据结构体
//定义bin文件的头
typedef struct
{
	//bBundleTag
	BYTE pBundleTag[8];
	//bProductType
	BYTE pProductType[8];
	//dwBundleHdrCRC32
    DWORD dwBundleHdrCRC32;
}stLutBundleHeader1;

typedef struct
{
	//dwBundleSize
	DWORD dwBundleSize;
	//wBundleHdrSize
	WORD  wBundleHdrSize;
	//wImageCount
	WORD wImageCount;
	DWORD dwReserved;
	//bBundleVersion
	BYTE pBundleVer[16];
	//PN
	BYTE pBundlePN[32];
	//SN
	BYTE pBundleSN[32];
	//The time at which this bundle file generated
    BYTE pBundleTime[16];	

}stLutBundleHeader2;

typedef struct  
{
	//LUT表的文件头
	//Tag
	BYTE  pLutTag[4];
    //ImageType
	BYTE  byImageType;
	BYTE  byHitless;
	BYTE  byStorageID;
	BYTE  byImageIndex;
    DWORD dwImageVersion;
	BYTE  pTimeStamp[4];
	DWORD dwBaseAddress;
	//reserved
	DWORD dwImageCRC32;
	DWORD dwImageSize;
	BYTE  bySectionCount;
	BYTE  byReserved[3];
}stImageHeader;

typedef struct  
{
	//Tag
	DWORD dwSectorTag;
	DWORD dwSectorAddress;
	DWORD dwSectorSize;

	WORD  wReserved;
	WORD  wStorageID;

    BYTE  byPartNumber[16];
}stSectorHeader;

typedef struct  
{
	DWORD dwSystemFlag;
	char  pchSN[32];				// Serial Number	"SNxxxxxxxx"  
	char  pchHwVer[16];				// Hardware version	"0.1"	
	char  pchMfgDate[16];			// Date of mfg		     "2013-01-16"
	char  pchPN[32];				// Product Number	" OMSSMCSB00TLB01"	or "OMSSMCSP00TLB01"	
	char  pchCalibDate[16];			// Date of Calibration	 "2013-01-16"		""	
	SHORT  wAlarmMaxTemperature;		// 
	SHORT  wAlarmMinTemperature;		// 
	// DAC setting for each optical channels
	WORD  wCalibPtrDAC[34][TEMP_CALIB_NUM][PORT_MAX_COUNT+MID_MAX_COUNT][2];   
	// Calibration temperature point for each switch
	SHORT  wTemperaturePoint[34][TEMP_CALIB_NUM];
	// The next target point for each channel.
	BYTE  wMidPointSelection[34][PORT_MAX_COUNT];
	// Setting value is 1-base, range from 1 to 12.
	BYTE  w1x8SWChannelMapping[34][PORT_MAX_COUNT];
	// Select one of channels or middle points for dark setting points. 1-base 1~(12+6)
	BYTE  wDarkMidPoint[34];
	// Channel number for each Switch
	WORD  wMaxChannelNUM;
	// time delay for each DAC setting point, total switching time will be (wSwitchDelayUs*50).
	WORD  wSwitchDelayUs;
	BYTE   bReserved1[2];
	long sPDSlope[AM_NUM][TEMP_CALIB_NUM];
	long sPDIntercept[AM_NUM][TEMP_CALIB_NUM];
	SHORT sTemperaturePoint[TEMP_CALIB_NUM];

	WORD   wSwitchIL[2][16][PORT_MAX_COUNT];
	BYTE   bReserved[1524];
	DWORD  dwCRC32;	
}stLutSettingZ4671;

typedef struct  
{
	LONG lAmVOACoef[AM_NUM][TEMP_CALIB_NUM][PORT_MAX_COUNT][COEF_NUM][STAGE_NUM]; 
	BYTE bReserved[2044];
	DWORD dwVOACRC32;
}stLutSettingZ4671_Add;

#define VOA_COEF 1048576
#define SWITCH_DELAY_TIME 250



#endif