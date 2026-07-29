// CommandSFPVOA.h: interface for the CCommandSFPVOA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDSFPVOA_H__85D2A798_50BF_4191_A3C3_83C405324213__INCLUDED_)
#define AFX_COMMANDSFPVOA_H__85D2A798_50BF_4191_A3C3_83C405324213__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpComm.h"
#include "OpCRC32.h"

//========================================================================//
// 公用宏定义
#define	BIT(x)							(DWORD)(0x00000001 << x)

#define MAX_DATA						4096
#define	RS232_BUFFER_SIZE				128

#define MAX_CMD							256
#define	MAX_LINE						256
#define	SFPVOA_FW_UPG					0x01
#define	TESTER_FW_UPG					0x02
#define	ALL_VOA_UPGD					0xFF
//========================================================================//
// EEPROM地址定义
#define OBJID_SERIAL_ID_I2C_ADDR        0x50
#define OBJID_DIAGNOSTIC_I2C_ADDR       0x51

//========================================================================//
// 命令协议相关
#define	CMD_FLAG_IDX					0
#define	CMD_OBJID_IDX					1
#define	CMD_ACCESS_IDX					2
#define	CMD_DATA_LEN_IDX				3
#define	CMD_STATUS_IDX					4
#define	CMD_PLAYLOAD_DATA_IDX			5

#define	CMD_EXECUTE_SUCESS				0x00
#define	CMD_EXECUTE_FAIL				0x01
#define	CMD_START_FLAG					0xFE

#define CMD_WRITE						0x00
#define CMD_READ						0x01

#define	MSG_BASIC_LENGTH				0x07

//========================================================================//
// 测试板命令字定义
#define	OBJID_READ_ADAPTER_INFO			0x80
#define	OBJID_SET_ADAPTER_BAUD_RATE		0x81
#define	OBJID_SET_I2C_BUS_FREQ			0x82
#define	OBJID_SOFT_RESET_ADAPTER		0x83
#define	OBJID_READ_CONNECT_CHANNEL		0x84
#define	OBJID_SET_GET_CURRENT_CHANNEL	0x85
#define OBJID_SET_I2C_1X2SW             0x86
#define	OBJID_SET_I2C_1X8SW				0x87
#define	OBJID_SET_ADAPTER_DOWDLSWEEP	0x88
#define	OBJID_SET_ADAPTER_SWEEPNOTRIG	0x90
#define	OBJID_SET_ADAPTER_DODACSWEEP	0x91
#define	OBJID_SET_ADAPTER_DOATTSWEEP	0x92
#define	OBJID_GET_ADAPTER_SWEEPSTATUS	0x93
#define	OBJID_READ_VOA_DAC				0x94
#define	OBJID_READ_PD_POWER				0x40
#define	OBJID_READ_PDALL_POWER			0x41

#define	OBJID_READ_VOA_REG				0xA0
#define	OBJID_XMODEM_DOWNLOAD			0xC0

//========================================================================//
// EEPROM(Addr:0xA0)
#define REG_MODULE_TYPE                 0
#define REG_EXT_MODULE_TYPE             1
#define REG_CONNECTOR                   2
#define REG_VENDOR_NAME                 20
#define REG_VENDOR_OUI                  37
#define REG_VENDOR_PN                   40
#define REG_VENDOR_REV                  56
#define REG_CC_BASE                     63
#define REG_OPTIONS                     64
#define REG_VENDOR_SN                   68
#define REG_MANUF_DATE                  84
#define REG_SFF_COMPLIANCE              94
#define REG_CC_EXT                      95
#define REG_VENDOR_DATA                 96
#define REG_CALIB_DATE                  128
#define REG_HW_VERSION                  136
#define REG_FW_VERSION                  137
#define	REG_CUSTOMER_BOM_CODE			138
#define REG_IL_OPTICAL_PATH             142

#define	REG_VOA_CAL_WL_STD				60
#define	REG_VOA_CFG_STD					143
#define REG_MIN_WL_STD                  144
#define REG_MAX_WL_STD                  146
#define	REG_VOA_ATTEN_RNG_STD			148
#define	REG_MIN_OUTPUT_PD_RNG_STD		150
#define	REG_MAX_OUTPUT_PD_RNG_STD		152

#define REG_MIN_WL_HW					143
#define REG_MAX_WL_HW					145

//========================================================================//
// EEPROM(Addr:0xA2)
#define	REG_TEMP_HIGH_ALARM				0
#define	REG_TEMP_LOW_ALARM				2
#define REG_TEMP_HIGH_WARNING           4
#define REG_TEMP_LOW_WARNING            6
#define REG_PIN_STATUS_SET				8
#define REG_LUT_CRC32_CHECK				9
#define REG_LABEL_CRC32_CKS				10
#define REG_VOA_CRC32_CKS				14
#define REG_PD_CRC32_CKS				18
#define REG_MODULE_TEMP                 96


// A2不同类型VOA之间的寄存器差异
#define	REG_VOA_DRV_DAC_HW				60
#define	REG_VOA_DRV_DAC_STD				147

#define	REG_SET_VOA_ATT_HW				128
#define	REG_SET_VOA_ATT_STD				130

#define	REG_PD_ADC_HW					50
#define	REG_PD_ADC_STD					135

#define	REG_PD_IN_PWR_HW				104
#define	REG_PD_IN_PWR_STD				102
#define	REG_PD_OUT_PWR_STD				104

#define	REG_OPLK_FW_VERSION_HW			37
#define	REG_OPLK_FW_VERSION_STD			23

#define	REG_VOA_WORK_MODE_HW			248
#define	REG_VOA_WORK_MODE_STD			128

#define	REG_MODULE_TEMP_ADC_HW			58
#define	REG_MODULE_TEMP_ADC_STD			143

//========================================================================//
// XMODEM相关
#define XMODEM_SOH						0x01
#define XMODEM_STX						0x02
#define XMODEM_EOT						0x04
#define XMODEM_ACK						0x06
#define XMODEM_NAK						0x15
#define XMODEM_CAN						0x18
#define XMODEM_CRCCHR					'C'

#define XMODEM_BLOCK_HEAD_INDEX			0
#define XMODEM_BLOCK_NO_INDEX			1
#define XMODEM_BLOCK_NON_INDEX			2
#define	XMODEM_BLOCK_DATA_INDEX			3
#define XMODEM_BLOCK_BODY_SIZE_128		128
#define XMODEM_BLOCK_BODY_SIZE_1K		1024

#define	XMODEM_COMMUNICATION_FAIL		0
#define	XMODEM_DOWNLOAD_FAIL			1
#define	XMODEM_DOWNLOAD_SUCCESS			2

//========================================================================//
// 测试夹具相关
#define	ASE_INPUT_CAL_ATT				(15)

#define	SRC_SFP_1_153094				0x01
#define	SRC_SFP_2_153431				0x02
#define	SRC_SFP_3_154838				0x03
#define	SRC_SFP_4_155094				0x04
#define	SRC_SFP_5_155258				0x05
#define	SRC_SFP_6_155482				0x06
#define	SRC_SFP_7_155731				0x07
#define	SRC_SFP_8_Reserved				0x08
#define	SRC_ASE_SOURCE					0x09

#define	SW_1X2_CH1						0x00
#define	SW_1X2_CH2						0x01

#define	SW_1X8_CH1						0x01
#define	SW_1X8_CH8						0x08

//OSA工位号
#define	SRC_WS_REF				0x00
#define	SRC_WS_1				0x01
#define	SRC_WS_2				0x02
#define	SRC_WS_3				0x03
#define	SRC_WS_4				0x04
#define	SRC_WS_5				0x05
#define	SRC_WS_6				0x06
#define	SRC_WS_7				0x07
#define	SRC_WS_8				0x08
#define	SRC_ASE_LD_rev			0x09


class CCommandSFPVOA  
{
public:
	CCommandSFPVOA();

	virtual ~CCommandSFPVOA();
public:
	COpComm*	m_pRS232;
	
public:
	BOOL ReadDataFromI2CBus(BYTE bI2CAddr, WORD wMemoryOffset, PBYTE pbDataArray, BYTE bDatalength, CString* pszErrMsg = NULL);
	BOOL WriteDataToI2CBus(BYTE bI2CAddr, BYTE bMemoryOffset, PBYTE pbDataArray, BYTE bDatalength, CString* pszErrMsg = NULL);
	BOOL WriteDataToTester(BYTE bObjID, PBYTE pbDataBytes, BYTE bDatalength, BOOL bWrite = TRUE);
	BOOL ReadDataFromTester(BYTE bObjID, PBYTE pbDataBytes, BYTE bDatalength);
	BOOL SendCmdToAdapter(BYTE bObjID, BYTE  bAccess, PBYTE pbDataSource, BYTE  bDataBytes, CString* pszErrMsg = NULL);

	BOOL GetModuleSN(CString *pstrRespMsg);
	BOOL GetModulePN(CString *pstrRespMsg);
	BOOL GetMinIL(PBYTE pbMinIL, CString *pstrErrMsg);
	BOOL GetModuleTempt(double* pdblTempt, CString *pstrErrMsg);
	BOOL GetModuleTemptADC(WORD wTempADCReg, PWORD pwTempADC, CString *pstrErrMsg);

	BOOL SetWorkMode(WORD wModeReg, BYTE byMode, CString *pstrErrMsg);
	BOOL SetVOADAC(WORD wDACRegAddr, WORD wDACValue, CString *pstrErrMsg);
	BOOL SetVOAAtten(WORD wAttRegAddr, double dblAtten, CString *pstrErrMsg);

	BOOL SetVOAPINStatus(BOOL bLogicHigh);
	BOOL GetVOAPINStatus(PBYTE pbPinStatus);

	BOOL SetVOALutCRC32Check(BOOL bCheckPDLut);

	BOOL GetAllModuleTemp(BYTE bySetChannel, BYTE bPresent, WORD wADCReg, double* pdblTempt, PWORD pwTemptADC, CString *pstrErrMsg);
	BOOL GetAllModuleCRC32(BYTE bySetChannel, BYTE bPresent, PDWORD pdwLabelCRC32, 
						   PDWORD pdwVOACRC32, PDWORD pdwPDCRC32, CString *pstrErrMsg);
	BOOL GetAllModuleDAC(BYTE bySetChannel, BYTE bPresent, WORD wDACReg, PWORD pwVOADAC, CString *pstrErrMsg);

	BOOL GetAllPDADC(BYTE bySetChannel, BYTE bPresent, WORD wADCReg, PWORD pwPDADC, CString *pstrErrMsg);
	BOOL GetAllPDPower(BYTE bySetChannel, BYTE bPresent, WORD wPowerReg, double* pdblPDPower, CString *pstrErrMsg);


	// 通过测试盒完成的函数
	BOOL SelectChannel(BYTE byChannel);		//切换电路I2C
	BOOL SelectChannel1X8SW(BYTE byChannel);//切换1x2光路开关
	BOOL SelectChannel1X2SW(BYTE byChannel);//切换光路开关
	BOOL GetVoaChannelStatus(BYTE *bStatus);

	BOOL SelectLDSource(BYTE byLDSource);

	BOOL SetSFPVoaDACAttnotrigSweep(BYTE bySetChannel,WORD wyDACAtt,WORD wySetValue,CString *pstrErrMsg);//文档中没有给出DAC用那个值0x0000
	BOOL DoSFPVoaDACSweep(BYTE bySetChannel,WORD wyDACReg,WORD wyStartDAC,WORD wyStopDAC,BYTE wyStepDAC,WORD wyMaxOPMtime,CString *pstrErrMsg);//没有给出DAC
	BOOL DoSFPVoaAttSweep(BYTE bySetChannel,WORD wyAttReg,WORD wyStartAtt,WORD wyStopAtt,BYTE byStepAtt,WORD* wyData,WORD wyMaxOPMtime,CString *pstrErrMsg);
	BOOL WDLExecuteSweep(BYTE* pbyData, CString *pstrErrMsg);
	BOOL CheckSweepStatus(PBYTE pbSweepStatus);

	BOOL GetVOARegValue(BYTE bySetChannel, BYTE bI2CAddr, BYTE bPresent, WORD wyRegValue, PBYTE pbReadData, 
						BYTE bDataCount, CString *pstrErrMsg, BOOL bMSBFirst = TRUE);
	//LWDM读取小板的PD功率
	BOOL GetPDRegValue( BYTE bSIALLCh,BYTE bPDChan, PBYTE pbReadData, CString *pstrErrMsg, BOOL bMSBFirst);
	BOOL GetLWDMPDPower(BYTE bSIALLCh,BYTE bPDChan,double* pdblPDPower, CString *pstrErrMsg);
	BOOL SelectWorkStation(BYTE byWSindex);
	// XMODEM升级
	BOOL StartXmodem(BYTE bySFPType,BYTE bySFPChannel);
	BYTE XmodemloadData(PBYTE pbBinData, WORD wDownloadSize, BOOL bFileDone, CString* pstrErrMsg);
	BYTE GetXmodemUpgStatus(void);

protected:
	//RS232 Buffer
	BYTE		m_pbTxBuffer[RS232_BUFFER_SIZE];
	BYTE		m_pbRxBuffer[RS232_BUFFER_SIZE];
	BYTE		m_bVOAXmodemUpgStatus;
	WORD		m_wBytesRead;
};

#endif // !defined(AFX_COMMANDSFPVOA_H__85D2A798_50BF_4191_A3C3_83C405324213__INCLUDED_)
