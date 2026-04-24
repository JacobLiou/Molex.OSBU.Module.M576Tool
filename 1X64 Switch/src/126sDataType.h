#define  MAX_LINE    256

typedef struct tag126sTestInfo 
{
	double dblTLSWL;
	double dblTLSPower;
	DWORD  dwChannelIndex;
	
	TCHAR tszOperatorID[8];
	TCHAR tszSPEC[8];
	TCHAR tszSerialNO[20];

}st126sTestInfo,*P126sTestInfo;

//扫描数据输出
struct ScanPowerCh 
{
	int VoltageX;
	int VoltageY;
	double MaxPower;
};

//定义配置信息
typedef  struct  tagTestingConfigure
{	
    DWORD	dwHP8164AAddress; // HP8164 Address
	
	//	double	dblHP8169AAngle;
	
	DWORD	dwWaitTimeInms;   // wait time for ms unit
	
	BOOL	bDetectTECReference;
	DWORD   dwCOMBaudRate;    //baud rate value
	
	BOOL	bDoLoopPopupMessage;
	// set it to 32 bytes
	// reserverd for future use
	DWORD	dwReserved[2];
	
}stTestingConfigure,*PTestingConfigure;

//定义归零信息
typedef  struct ReferencePower 
{
	double dblReferencePower[12];
	bool   bHasReference;
}ReferencePower,*pReferencePowerDate;

//定义个通道的光学参数
struct ChannalOpticalMessage
{
	double dblHighTempIL;
	double dblLowTempIL;
	double dblIL;
	double dblWDL[2];
	double dblPDL;
	double dblTDL;
	double dblRL;
	double dblRepeat;
	double dblDif;
	double dblCT[32];
	double dblMinDIR;
	double dblISO;
	double dblDarkIL;
	double dblTotalWDL;
	int nChannel[32];
};
 

typedef struct tagPMCfg
{
	INT iType[3]; //0: OPLINK_PM, 1: N7744, 2 1830C
	INT iConnect[3];//0: NET, 1:GPIB, 2:COM
	unsigned int iAdderess[3];
	BOOL bPMOpen[3];
}stPMCfg,*pstPMCfg;

#define TYPE_OPLINK_PM            0
#define TYPE_N7744                1
#define TYPE_1830C                2

#define CONNECT_NET               0
#define CONNECT_GPIB              1
#define CONNECT_COM               2
