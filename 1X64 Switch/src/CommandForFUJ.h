// CommandForFUJ.h: interface for the CCommandForFUJ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDFORFUJ_H__057DB837_4936_4AFC_8FE4_0F90FB5C3213__INCLUDED_)
#define AFX_COMMANDFORFUJ_H__057DB837_4936_4AFC_8FE4_0F90FB5C3213__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpComm.h"
#include "OpCRC32.h"

enum{CMD_MANUFACTURER_ID,
CMD_HW_VER,
CMD_FW_VER,
CMD_SN,
CMD_PN,
CMD_MFG_DATE,
CMD_CALI_DATE,
CMD_SW_TYPE,
CMD_3V3_VOLTAGE,
CMD_MODULE_TEMP,
CMD_MODULE_TEMP_ADC,
CMD_INNER_FW_VER,
CMD_SET_MEMORY_ADDRESS,
CMD_GET_MEMORY_ADDRESS,
CMD_GET_MEMORY_DATA,
CMD_GET_SW_DAC_ADD5504,
CMD_GET_SW_DAC,
CMD_SET_SW_DAC,
CMD_READ_REGISTER,
CMD_READ_CURRENT_REGISTER,
CMD_SET_REGISTER,
CMD_SW_SCAN_STATUS,
CMD_SW_SCAN_SETTINGS,
CMD_SW_SCAN_START,
CMD_SW_HITLESS_STATUS,
CMD_SW_HITLESS_SETTINGS,
CMD_SW_HITLESS_TEST,
CMD_LUT_VERIFY_STATUS,
CMD_SW_RELIABLE_TEST_COUNTS,
CMD_SW_RELIABLE_TEST_STAUTS,
CMD_SW_RELIABLE_TEST_START,
CMD_GET_CHANNEL,
CMD_SET_CHANNEL,
CMD_SOFTWARE_RESET,
CMD_START_FW_UPGRADE,
CMD_ARM_FW_UPGRADE,
CMD_FW_UPGRADE_DATA,
}; 

//Tester
enum
{TESTER_VER,
TESTER_MEM,
TESTER_SPRT,
TESTER_GPRT,
TESTER_INFO,
TESTER_GALM,
TESTER_SETX,
TESTER_SETY,
TESTER_SETXY,
TESTER_SETXY_READ_0,
TESTER_SETXY_READ_1,
TESTER_SETXY_READ,
TESTER_GADC_0,
TESTER_GDAC_1,
TESTER_GDAC,
TESTER_GTEMP,
TESTER_GTEMPADC,
TESTER_GVOLADC,
TESTER_OPLK,
TESTER_GAD5504REG,
TESTER_SCAN_SET,
TESTER_SCAN_READ,
TESTER_HLTS_SET,
TESTER_HLTS_READ,
TESTER_RSET,
TESTER_RESET,
TESTER_RSTI2C,
TESTER_FWDL,
TESTER_SWCH,
TESTER_STROBE,
TESTER_STROBE_READ,
};
//Command define
#define I2C_SET 1
#define I2C_GET 0

#define MAX_BUFFER 256

typedef struct{
	BYTE byMsgID;
	BYTE byCmdLength;
	BYTE byStatus;
    BYTE byAccess;
	BYTE byInstance;
	BYTE byObjectID;
	BYTE byPara;
}stCmdStruct;

typedef struct{
	char pchCmdName[1024];
	stCmdStruct stCmd;
	BYTE byDataSize;
	BYTE pbyData[1024];
	BYTE byCheckSum;
}stCmdInfo;

typedef struct
{
	char pchCmdName[1024];
	CString strCmd;
	BYTE bRead;
	BYTE byParaCount;
	int  nGetDataSize;
}stTesterCmd;

typedef struct
{
	CString strSN;
	CString strPN;
	WORD    wID;
	CString strFWVer;
	CString strHWVer;
	CString strMFGDate;
	CString strCalDate;
	CString strSwitchType;
}stProductInfo;

class CCommandForFUJ : public COpComm  
{
public:
	BOOL GetWorkMode(int *pMode);
	BOOL SetWorkMode(int nMode);
	char  m_pReadData[1024];
	BOOL SendTesterCmd(int nCmdType,int *pnData=NULL);
	int   m_nTesterCmdCount;
	stTesterCmd m_stTesterCmd[256];
	BOOL GetTesterCmdInfo();
	stProductInfo m_stModuleInfo;
	BOOL GetProductInfo();
	BOOL SoftwareReset();
	BOOL ArmFWUpgrade();
	BOOL UpgradeFWData(BYTE *pbyData);
	BOOL FWStart();
	BOOL FWUpgrade(CString strBinFile);
	BOOL GetPort(int *pnCH);
	BOOL SetPort(int nCH);
	BOOL HitlessScan(int nCH,int nTime);
	BOOL SetScanStart(int nStartX,int nEndX,int nStartY,int nEndY,int nStep);
	BOOL GetXYDAC(int *pXDAC,int *pYDAC);
	BOOL SetXYDAC(SHORT nXDAC,SHORT nYDAC);
	int   m_nCmdCount;
	TCHAR m_tszAppFolder[256];
    COpCRC32 m_CRC32;
	BYTE m_pbyReadData[MAX_BUFFER];
	BYTE m_pbySendData[MAX_BUFFER];
	BOOL CheckResponseCmd(PBYTE pbyReadData,int nDataLength,BYTE byIDIndex);
	CString m_strError;
	BOOL GetCheckSum(BYTE *pbyCmd,int nLength,BYTE *pbyCheckSum);
	stCmdInfo m_stCMDInfo[1024]; //
	BOOL SetCmdToSlave(BYTE byIDIndex,BYTE *pbyData=NULL);
	BOOL GetCmdInfo();

	BOOL   SendProductScanZone(int nSWIndex, int nMinX, int nMaxX, int nMinY, int nMaxY, int nStep);
	double GetModuleTemp();
	int	   GetScanState(BYTE btSWIndex);
	BOOL   SetTesterXYDAC(int nSWIndex,int x, int y);
	BOOL   SetSWChannel(int nSWIndex,int nCHIndex);
	BOOL   GetMemoryData(DWORD dwAddress,char *pchData);

	CCommandForFUJ();
	virtual ~CCommandForFUJ();

};

#endif // !defined(AFX_COMMANDFORFUJ_H__057DB837_4936_4AFC_8FE4_0F90FB5C3213__INCLUDED_)
