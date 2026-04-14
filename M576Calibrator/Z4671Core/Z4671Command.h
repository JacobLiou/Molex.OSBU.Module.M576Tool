// : interface for the Z4671Command class.
//
////////Z4671Command.h//////////////////////////////////////////////////////////////

#if !defined(AFX_Z4671COMMAND_H__DB703521_25D3_4D9A_84AB_DB788C2700D5__INCLUDED_)
#define AFX_Z4671COMMAND_H__DB703521_25D3_4D9A_84AB_DB788C2700D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpComm.h"
#include "CommLog.h"
#include "Z4767StructDefine.h"

#define MAX_COUNT 2048
#define START_CMD 0X55
#define END_CMD   0XAA
#define LOG_LENGTH 10
#define MAX_DATA_LENGTH 400


//	Command
#define CMD_HW_READ_SUPPLIER_ID			0xA0
#define CMD_HW_READ_MANUFACTURER_NAME	0xA1
#define CMD_HW_READ_SERIAL_NUMBER		0xA2
#define CMD_HW_READ_UNIT_RVS			0xA3
#define CMD_HW_READ_UNIT_MANU_DATE		0xA4
#define CMD_HW_READ_MODUAL_NAME			0xA5

#define CMD_HW_SET_LASER_STATUS			0xB0
#define CMD_HW_GET_LASER_STATUS			0xB1
#define CMD_HW_READ_PER					0xB2
#define CMD_HW_SET_ALARM_THR			0xB3
#define CMD_HW_READ_ALARM_THR			0xB4
#define CMD_HW_READ_ALARM				0xB5
#define CMD_HW_READ_STANDARD_GAIN		0xB6
#define CMD_HW_READ_REAL_GAIN			0xB7
#define CMD_HW_READ_STANDARD_GAIN_RANGE	0xB8
#define CMD_HW_SET_EDFA_WORK_MODE		0xBA
#define CMD_HW_READ_EDFA_WORK_MODE				0xBB
#define CMD_HW_READ_EDFA_NF						0xBC
#define CMD_HW_SET_VOA_ATTENUATION				0xBD
#define CMD_HW_READ_VOA_ATTENUATION				0xBE
#define CMD_HW_READ_VOA_ATTENUATION_RANGE		0xBF
#define CMD_HW_READ_SOFT_STAT					0xC0
#define CMD_HW_RESET_MODULE				0xC1
#define CMD_HW_READ_VOLTAGE_VALUE		0xC2
#define CMD_HW_READ_FILE_LEN			0xC3
#define CMD_HW_RD_FILE					0xC4
#define CMD_HW_READ_MODULE_REGISTER		0xC5
#define CMD_HW_SET_ALARM_SIMULATE		0xC6
#define CMD_HW_CLEAR_ALARM_SIMULATE		0xC7
#define CMD_HW_READ_ALARM_SIMULATE		0xC8
#define CMD_HW_SET_SWITCH_POSITION		0xC9
#define CMD_HW_GET_SWITCH_POSITION		0xCA
#define CMD_HW_SET_AUTO_BLOCK_ENABLE	0xCB
#define CMD_HW_READ_AUTO_BLOCK_ENABLE	0xCC
#define CMD_HW_SET_LOS_CONFIRM_TIME		0xCD
#define CMD_HW_READ_LOS_CONFIRM_TIME	0xCE
#define CMD_HW_READ_EMCA_PARA_TABLE		0xCF
#define CMD_HW_READ_INSERT_LOSS			0xD0
#define CMD_HW_SET_EXIT_LOS_CONFIRM_TIME 0xD1
#define CMD_HW_READ_EXIT_LOS_CONFIRM_TIME 0xD2
#define CMD_HW_READ_INT_SRC_REG         0xD3
#define CMD_HW_SET_ALL_SWITCH_POSITION  0xD4
#define CMD_HW_GET_ALL_SWITCH_POSITION  0xD5
#define CMD_HW_SET_MONT_MONR_SWITCH_POSITION 0xD6
#define CMD_HW_GET_MONT_MONR_SWITCH_POSITION 0xD7
#define CMD_HW_READ_OUT_POWER_LOCK_RANGE     0xD8
#define CMD_HW_READ_ALL_CHANNEL_INSERT_LOSS  0xD9
#define CMD_ONLY_VENDOR_USE            0xF0


#define CMD_HW_START_FIRMWARE_UPDATE	0xE0
#define CMD_HW_TRANSPORT_FIRMWARE		0xE1
#define CMD_HW_FIRMWARE_UPDATE_END		0xE2

//	
#define	MODULE_TEMP				0x00
#define	ADD_AM1_INPUT_POWER		0x01
#define	ADD_AM2_INPUT_POWER		0x02
#define	ADD_AM3_INPUT_POWER		0x03
#define	ADD_AM4_INPUT_POWER		0x04
#define	ADD_AM5_INPUT_POWER		0x05
#define	ADD_AM6_INPUT_POWER		0x06
#define	ADD_AM7_INPUT_POWER		0x07
#define	ADD_AM8_INPUT_POWER		0x08
#define	ADD_AM9_INPUT_POWER		0x09
#define	ADD_AM10_INPUT_POWER	0x0A
#define	ADD_AM11_INPUT_POWER	0x0B
#define	ADD_AM12_INPUT_POWER	0x0C
#define	ADD_AM13_INPUT_POWER	0x0D
#define	ADD_AM14_INPUT_POWER	0x0E
#define	ADD_AM15_INPUT_POWER	0x0F
#define	ADD_AM16_INPUT_POWER	0x10
#define	ADD_LSR1_INPUT_POWER	0x11
#define	ADD_LSR2_INPUT_POWER	0x12
#define	ADD_LSR3_INPUT_POWER	0x13
#define	ADD_LSR4_INPUT_POWER	0x14
#define	ADD_LSR5_INPUT_POWER	0x15
#define	ADD_LSR6_INPUT_POWER	0x16
#define	ADD_LSR7_INPUT_POWER	0x17
#define	ADD_LSR8_INPUT_POWER	0x18
#define	ADD_LSR1_OUTPUT_POWER	0x19
#define	ADD_LSR2_OUTPUT_POWER	0x1A
#define	ADD_LSR3_OUTPUT_POWER	0x1B
#define	ADD_LSR4_OUTPUT_POWER	0x1C
#define	ADD_LSR5_OUTPUT_POWER	0x1D
#define	ADD_LSR6_OUTPUT_POWER	0x1E
#define	ADD_LSR7_OUTPUT_POWER	0x1F
#define	ADD_LSR8_OUTPUT_POWER	0x20
#define	ADD_LSR1_REAL_GAIN		0x21
#define	ADD_LSR2_REAL_GAIN		0x22
#define	ADD_LSR3_REAL_GAIN		0x23
#define	ADD_LSR4_REAL_GAIN		0x24
#define	ADD_LSR5_REAL_GAIN		0x25
#define	ADD_LSR6_REAL_GAIN		0x26
#define	ADD_LSR7_REAL_GAIN		0x27
#define	ADD_LSR8_REAL_GAIN		0x28
#define	ADD_LSR1_TEMP			0x29
#define	ADD_LSR2_TEMP			0x2A
#define	ADD_LSR3_TEMP			0x2B
#define	ADD_LSR4_TEMP			0x2C
#define	ADD_LSR5_TEMP			0x2D
#define	ADD_LSR6_TEMP			0x2E
#define	ADD_LSR7_TEMP			0x2F
#define	ADD_LSR8_TEMP			0x30
#define	ADD_LSR1_WCV			0x31
#define	ADD_LSR2_WCV			0x32
#define	ADD_LSR3_WCV			0x33
#define	ADD_LSR4_WCV			0x34
#define	ADD_LSR5_WCV			0x35
#define	ADD_LSR6_WCV			0x36
#define	ADD_LSR7_WCV			0x37
#define	ADD_LSR8_WCV			0x38
#define	ADD_LSR1_CCV			0x39
#define	ADD_LSR2_CCV			0x3A
#define	ADD_LSR3_CCV			0x3B
#define	ADD_LSR4_CCV			0x3C
#define	ADD_LSR5_CCV			0x3D
#define	ADD_LSR6_CCV			0x3E
#define	ADD_LSR7_CCV			0x3F
#define	ADD_LSR8_CCV			0x40
#define	ADD_LSR1_BCV			0x41
#define	ADD_LSR2_BCV			0x42
#define	ADD_LSR3_BCV			0x43
#define	ADD_LSR4_BCV			0x44
#define	ADD_LSR5_BCV			0x45
#define	ADD_LSR6_BCV			0x46
#define	ADD_LSR7_BCV			0x47
#define	ADD_LSR8_BCV			0x48
#define	DROP_LSR1_INPUT_POWER	0x49
#define	DROP_LSR2_INPUT_POWER	0x4A
#define	DROP_LSR3_INPUT_POWER	0x4B
#define	DROP_LSR4_INPUT_POWER	0x4C
#define	DROP_LSR5_INPUT_POWER	0x4D
#define	DROP_LSR6_INPUT_POWER	0x4E
#define	DROP_LSR7_INPUT_POWER	0x4F
#define	DROP_LSR8_INPUT_POWER	0x50
#define	DROP_LSR1_OUTPUT_POWER	0x51
#define	DROP_LSR2_OUTPUT_POWER	0x52
#define	DROP_LSR3_OUTPUT_POWER	0x53
#define	DROP_LSR4_OUTPUT_POWER	0x54
#define	DROP_LSR5_OUTPUT_POWER	0x55
#define	DROP_LSR6_OUTPUT_POWER	0x56
#define	DROP_LSR7_OUTPUT_POWER	0x57
#define	DROP_LSR8_OUTPUT_POWER	0x58
#define	DROP_LSR1_REAL_GAIN		0x59
#define	DROP_LSR2_REAL_GAIN		0x5A
#define	DROP_LSR3_REAL_GAIN		0x5B
#define	DROP_LSR4_REAL_GAIN		0x5C
#define	DROP_LSR5_REAL_GAIN		0x5D
#define	DROP_LSR6_REAL_GAIN		0x5E
#define	DROP_LSR7_REAL_GAIN		0x5F
#define	DROP_LSR8_REAL_GAIN		0x60
#define	DROP_LSR1_TEMP			0x61
#define	DROP_LSR2_TEMP			0x62
#define	DROP_LSR3_TEMP			0x63
#define	DROP_LSR4_TEMP			0x64
#define	DROP_LSR5_TEMP			0x65
#define	DROP_LSR6_TEMP			0x66
#define	DROP_LSR7_TEMP			0x67
#define	DROP_LSR8_TEMP			0x68
#define	DROP_LSR1_WCV			0x69
#define	DROP_LSR2_WCV			0x6A
#define	DROP_LSR3_WCV			0x6B
#define	DROP_LSR4_WCV			0x6C
#define	DROP_LSR5_WCV			0x6D
#define	DROP_LSR6_WCV			0x6E
#define	DROP_LSR7_WCV			0x6F
#define	DROP_LSR8_WCV			0x70
#define	DROP_LSR1_CCV			0x71
#define	DROP_LSR2_CCV			0x72
#define	DROP_LSR3_CCV			0x73
#define	DROP_LSR4_CCV			0x74
#define	DROP_LSR5_CCV			0x75
#define	DROP_LSR6_CCV			0x76
#define	DROP_LSR7_CCV			0x77
#define	DROP_LSR8_CCV			0x78
#define	DROP_LSR1_BCV			0x79
#define	DROP_LSR2_BCV			0x7A
#define	DROP_LSR3_BCV			0x7B
#define	DROP_LSR4_BCV			0x7C
#define	DROP_LSR5_BCV			0x7D
#define	DROP_LSR6_BCV			0x7E
#define	DROP_LSR7_BCV			0x7F
#define	DROP_LSR8_BCV			0x80

#define CMD_SCUCCESS		0x00 //	执行成功
#define CMD_INVALID			0x01 //	不明命令
#define CMD_INVALID_FRAME	0x02 //	帧格式错误
#define CMD_INVLAID_PARA	0x03 //	参数不合法
#define CMD_CHKSUM_ERR		0x04 //	校验错误
#define CMD_BUSY			0x08 //	还在执行
#define CMD_NO_EXECUTE		0x09 //	命令没有执行
#define CMD_EXE_FAIL		0x0A //	命令执行失败

/*
struct stZ4671CmdInfo{
	char chCmdName[64];
	BYTE byCmd;
	WORD wCmdIndex;
	WORD wLength;
	BYTE byData[1024];
};
*/

//定义CMD
//定义指令格式
//INFO,CMD,DATA_length

//定义光口起始编号
#define OUT1 1
#define IN1  9
#define AM1  17
#define DM1  33
#define MONO 49
#define MONI 50
#define EDFA_VOA1 59
#define AM_VOA1   17

class Z4671Command : public COpComm  
{
public:
	BOOL SetVOAAtten(int nType,int nVOAIndex,double dblAtten);
	BOOL GetCurrentDAC(int nBlock,int nSwitch,SHORT *psDACX,SHORT *psDACY);
	BOOL GetProductPDPower(int nPDIndex, double *pdblPDPower);
	double GetTemperature();
	BOOL GetProductPDADC(int nPD,int *PDADC);
	float GetPDPower(int nPD);
	BOOL GetMCSAlarm();
	BOOL GetProductSupplier(char *pchSupplier);
	BOOL GetProductID(char *pchID);
	stCheckInfo m_stModuleInfo;
	BOOL FWUpdateEnd();
	BOOL FWTranSportFW(BYTE *byTransData,int nDataLength,int nIndex,int nSum);
	BOOL StartFWUpdate();
	BOOL SendHitlessTestCmd(int nBlock, int nSwitch, int nCHNum);
	BOOL SetSwitchDAC(int nBlock,int nSwitch,int nDac,int nXY);
	BOOL SendScanTrig(stSWScanPara stScanPara);
	BOOL GetSingleSwitchState(int nBlock,int nSwitch,int *pPos);
	BOOL SwitchSingleSwitch(int nBlock,int nSwitch,int nPort);
	BOOL  SendCmd();
//	stZ4671CmdInfo m_stCmd;
//	BYTE  m_pbySendData[1024];
	
	BYTE  m_pNewData[2048];
	LONG m_dwPara[129];
	BYTE  m_pbyAlarm[50];
	WORD m_wVoltage[5];

	BOOL SwitchALLSwitch(int nDrop, int *nPort, int nCount);
	BOOL GetEDFAInfo(char *pchEDFAVer);
	BOOL GetMCSVersion(char *pchMCSVer);
	BOOL GetVersion(char *pchVer);
	BOOL GetTesterInfo();
	//定义产品属性
	/*
    int  m_nBlockCount;
	int  m_nSwitchCount;
	int  m_nCHCount;
	*/
	//定义产品方法
	BOOL CloseSwitchMonitor(int nSwitchIndex);
	BOOL GetProductPN(char *pchPN);
	BOOL GetProductSN(char *pchSN);
	BOOL GetEDFATemp(double *dblTemp);
	BOOL GetMCSTemp(double *dblTemp);
	BOOL VOACalibrate(int nStartIndex,int nEndIdnex,int nDACRange,int nStep,int nSleepTime);
	BOOL VOAVerify(int nStartIndex,int nEndIdnex,int nDACRange,int nStep,int nSleepTime);
	BOOL GetMONSwitch(int nSwitch,int *pPos);
	BOOL GetALLSwitch(int nBlock,int *pPos);
	BOOL ReadFPGA(int nChannel,DWORD dwAddress,char *pchReStr);
	BOOL SendCmdToEDFA(CString strCmd,char *pchReStr);
	stSWScanPara m_stScanPara;
	CString m_strLogInfo;
	void SetCommLogTarget(const M576CommLogTarget& logTarget) { m_logTarget = logTarget; }
	void TraceInfo(LPCTSTR category, LPCTSTR format, ...);
	void TraceError(LPCTSTR category, LPCTSTR format, ...);
	BOOL SendScanDoubleTrig(stSWScanPara *stScanPara);
	
	BOOL GetModuleState(int *nReady);
	BOOL GetDumpData(char **pReBuff,int *dwSize);
	BOOL EDFA_SRESETTWOCMD();
	BOOL IDX_SOFT_RST();
	BOOL IDX_HARD_RST();
	BOOL EDFA_SRESETONECMD();
	BOOL FPGASPIConnect();
	CString GetErrorMsg();
	BOOL SendMaster();
	BOOL SendFWTranSportFW(BYTE *byTransData,int nDataLength,int nIndex,int nSum);
	BOOL GetLogFileData(int nType,int nDataLength,DWORD dwAddress,byte **pbyData,int *pReDataLength);
	BOOL GetVOAAtten(int nVOAIndex,double *pdblAtten,int nType=0);
	BOOL ReadAlarm(int nType);
	BOOL SetEDFAWorkMode(int nEDFA,int nMode,double dblPower=-99);
	BOOL ReadModuleVoltage();
	BOOL CmdSendExchange(PBYTE pbyte, WORD wLength, PBYTE *pReBuff, PWORD pwReLength);
	BOOL CmdReadExchange(PBYTE pbyte, WORD wLength, PBYTE *pReBuff, PWORD pwReLength=NULL);
	BOOL GetModulePara();
	BOOL ReadIRQ();
	BOOL ClearIRQ();
	BOOL WatchDog();
	BOOL Error();
	BOOL HardReset();
	BOOL MasterReset();
	
	Z4671Command();
	virtual ~Z4671Command();
private:
	M576CommLogTarget m_logTarget;
	DWORD m_traceSeq;
	DWORD m_pendingTraceSeq;
	DWORD m_pendingTick;
	BYTE m_pendingCmdCode;
	CString m_pendingCmdName;
	CString LookupCommandName(BYTE cmdCode) const;
	void TraceFrame(BOOL isSend, BYTE cmdCode, const BYTE* frame, int frameLen, const BYTE* wireData, int wireLen, DWORD elapsedMs = 0);

public:
	bool SetScanDelayTime(int nDelayus);
};

#endif // !defined(AFX_Z4671COMMAND_H__DB703521_25D3_4D9A_84AB_DB788C2700D5__INCLUDED_)
