// Copyright @2006, Oplink communication,Inc
// The data type and the const define here
// Revision 1.0  11-02-2006
// Author: xiaoyong wang
#include "headerDef.h"
#ifndef  _CONFIGURATION_DATA_TYPE_H
#define  _CONFIGURATION_DATA_TYPE_H


//***************************************************
//****** Basic Definition
//***************************************************

// Define some message
#define MSG_INITIALIZE_PROGRESS_CTRL	(WM_USER + 100)
#define MSG_UPDATE_PROGRESS_CTRL		(WM_USER + 101)
#define MSG_UPDATE_INTERFACE_VALUE		(WM_USER + 102)
#define MSG_SHOW_MESSAGE				(WM_USER + 103)

#define MSG_SHOW_DETAILED_CMD_INFO_DLG	(WM_USER + 300)
#define MSG_SHOW_SIMPLE_CMD_INFO_DLG	(WM_USER + 301)
#define MSG_DISPLAY_DETAILED_CMD_INFO	(WM_USER + 302)
#define MSG_DISPLAY_SIMPLE_CMD_INFO		(WM_USER + 303)

#define MSG_UPDATE_CMD_INFO				(WM_USER + 304)
#define CONTROL_LIST_CTRL_MSG			(WM_USER + 305)
#define MY_CLICK_MSG					(WM_USER + 306)

#define MESSAGE_FOR_READ				0
#define MESSAGE_FOR_WRITE				1

// common macro
// Macro for product
//----------------------------------------------
#define VMUX140C00ZAB01  					00
#define VMUX140H00ZAB02  					01
#define VMUX140L00ZAB03  					02
#define VMUX140Q00ZAB04  					03
#define VMUX140C00ZAB05  					04
#define VMUX140C00ZAB06  					05
#define VMUX14025CZAB07  					06
#define VMUX14025HZAB08  					07
#define VMUX14025HZAB09  					08
#define VMUX14025HZAB10  					09
#define VMUX148C00ZAB11  					10
#define VMUX148H00ZAB12  					11
#define VMUX148H25ZAB13  					12
#define VMUX148C25ZAB14  					13
#define VMUX140M20PDC01                     14
//----------------------------------------------
#define VMUX32XL20LUC01 					100
#define VMUX32XL20LUC02 					101
#define VMUX32XL20LUC03 					102
//----------------------------------------------
#define VMUX108000LUC01						130
#define VMUX108000LUC01_MUX					131
#define VMUX108000LUC01_DMUX				132
#define VMUX18SROALUC01_8023				133

#define AAWG144C17LUC01_MUX					150
#define AAWG144C17LUC01_DMUX				151

#define DWDM41WSSPZAB01						200
#define DWDM41WSSPZAB02						201
#define DWDM81WSSPZAB13						202
#define DWDM81WSSHZAB14						203

#define BLOCKER_FOR_LUCENT					230
#define BLOCK_FOR_HUAWEI					231

#define VMUX140YYYZCC01						250
#define VMUX148CXXZCC09						251

#define DWDMG105XXZHOXX						300
#define HW_OSU_1_8_SWITCH					301
#define DS1624_TEST							302

#define OADM108B25AON01						350
#define OADM108B25AON02_MUX					351
#define OADM108B25AON02_DMUX				352

#define	chSTR2(x)		#x
#define	chSTR(x)		chSTR2(x)
#define	chMSG(desc)		message(__FILE__"("chSTR(__LINE__)"):" #desc)


#define MAX_LENGTH						256
#define MAX_STRING_LENGTH				256
#define MAX_MSG_BUFFER					256
#define STR_MIN_BUFFER_SIZE				256
#define STR_MAX_BUFFER_SIZE				512
#define MAX_REF_SAMPLE					512
#define MAX_PD_CALIB_SAMPLE				512
#define MAX_PD_VERIFY_SAMPLE			512
#define MAX_VOA_CALIB_SAMPLE			1024
#define MAX_VOA_VERIFY_SAMPLE			1024
#define MAX_TEMPERATURE_TYPE			3
#define MAX_PD_NUMBER					16
#define MAX_VOA_NUMBER					64
#define SECTOR_SIZE_IN_BYTE				1024
#define MAX_COL_COUNT					32

#define MAX_MATRIX_ROW_COUNT			256
#define MAX_MATRIX_COL_COUNT			256

#define MAX_GAIN_VERIFY_SAMPLE			256

#define AMPLIFY_MULTIPLE_POWER			100
#define AMPLIFY_MULTIPLE_ATTEN			100
#define AMPLIFY_MULTIPLE_MIN_IL			100
#define AMPLIFY_MULTIPLE_PD_SLOPE		1024

#define MULTIPLE_PD_SLOPE_TEMPER		64 * 1024
#define MAX_DIMENSION_NUMBER			4
#define MAX_SELECTED_FILE_COUNT			64

#define AMPLIFY_MULTIPLE_PD_INTERCEPT	1024
#define AMPLIFY_MULTIPLE_VOA_SLOPE		1024

//Define language type
#define LANGUAGE_CHINESE				0
#define LANGUAGE_ENGLISH				1

// Macro for view data
#define VIEW_DATA_PD_CALIB						0
#define VIEW_DATA_PD_VERIFY						1
#define VIEW_DATA_OVERALL_TEMPER_SLOP_INTERPT	2

#define VIEW_DATA_VOA_CALIB						0
#define VIEW_DATA_VOA_VERIFY					1

// Macro for module setting
#define FREQ_DESCENDING					0
#define FREQ_ASCENDING					1

//Define communication macro
//Define communication type
#define SERIAL_PORT						0
#define SERIAL_PORT_I2C					1
#define SERIAL_PORT_ONE_WIRE			2
#define SERIAL_PORT_DPRAM				3
#define PARALLEL_PORT					4
#define PARALLEL_PORT_ONE_WIRE			5

#define MAX_DATA_PARAM_COUNT			8
#define MAX_PROT_PARAM_COUNT			64
#define MAX_FDBK_DATA_PARAM_COUNT		16

//Define data align type
#define DATA_LEFT_ALIGN					0
#define DATA_RIGHT_ALIGN				1

//Define command send type
#define CMD_SEND_TYPE_SEND				0
#define CMD_SEND_TYPE_READ_WRITE		1

#define WINDOW_MODE_MODAL				0
#define WINDOW_MODE_EMBEDDED			1

//Define temperature type
#define TEMPERATURE_LOW					0
#define TEMPERATURE_ROOM				1
#define TEMPERATURE_HIGH				2

// Define parameter type
#define ALL_PARA						0
#define DEVICE_PARA						1
#define MODULE_PARA						2
#define COMMAND_PARA					3
#define UPGRADE_PARA					4
#define PD_PARA							5
#define PM_PARA							6
#define REFERENCE_PARA					7
#define TLS_PARA						8
#define VOA_PARA						9
#define OPTICAL_PARA					10
#define REPORT_FORMS_PARA				11
#define EDFA_PARA						12
#define APPLICATION_PARA				13

#define CMD_INFO_PARA					14
#define CMD_MFG_PARA					15
#define CMD_MFG_INFO_PARA				16
#define CMD_DELIVERY_PARA				17
#define PROT_WRITE_PARA					18
#define PROT_READ_PARA					19
#define PROT_FDBK_PARA					20
#define PROCESS_STATUS_PARA				21

// Define command type
#define CMD_COMMON						0x01
#define CMD_MFG							0x02
#define CMD_MFG_INFO					0x04
#define CMD_DELIVERY					0x08
#define CMD_ALL							0x10

// Define command ID type
#define CMD_ID_TYPE_BINARY				0
#define CMD_ID_TYPE_ASCII				1

// Define access type
#define READ_ONLY						0
#define WRITE_ONLY						1
#define WRITE_READ						2

// Define protocol type
#define PROT_TYPE_READ					0
#define PROT_TYPE_WRITE					1
#define PROT_TYPE_FEEDBACK				2

// Define Param Alterability macro
#define PARAM_ALTERABLE					0
#define PARAM_FIXED						1

//Define data size type (DST)
#define DST_BYTE						0
#define DST_WORD						1
#define DST_ASCII_TO_WORD				2
#define DST_DWORD						3
#define DST_LONG						4

// Define data type
#define DATA_TYPE_BYTE					0
#define DATA_TYPE_WORD					1
#define DATA_TYPE_DWORD					2
#define DATA_TYPE_DOUBLE				3
#define DATA_TYPE_NUMBER				4
#define DATA_TYPE_ASCII					5
#define DATA_TYPE_VERSION				6
#define DATA_TYPE_DATE					7
#define DATA_TYPE_CHECKSUM				8
#define DATA_TYPE_DWORD_COMBINED		9
#define DATA_TYPE_FOUR_WORD_COMBINED	10
#define DATA_TYPE_SIX_WORD_COMBINED		11
#define DATA_TYPE_SIXTEEN_WORD_COMBINED	12

#if 0
// Define Action ID
#define ACTION_WRITE					0
#define ACTION_READ						1
#define ACTION_WRITE_READ				2
#endif

// Define command string type
#define CMD_STR_TYPE_WRITE					0
#define CMD_STR_TYPE_READ					1
#define CMD_STR_TYPE_FEEDBACK				2

// Define Checksum type
#define ORIGINAL_CODE					0
#define REVERSE_CODE					1
#define COMPLEMENT_CODE					2
#define REVERSE_ADD_1					3

// define array type
#define ARRAY_TYPE_ASCENDIND			0
#define ARRAY_TYPE_DESCENDIND			1
#define ARRAY_TYPE_OUT_OF_ORDER			2

#define CMD_INFO_DSP_MODE_DETAILED		0
#define CMD_INFO_DSP_MODE_SIMPLE		1

typedef struct tagApplicationConfig
{
	int			iCmdInfoDlgDspMode;

	DWORD		dwMaxMessageLine;

	char	*	pchCommandFileFullName[MAX_LENGTH];

	BOOL		bUseParallelPort;

} stApplicationConfig, *pstApplicationConfig;

typedef struct tagProcessStatus
{
	BOOL		bSuccess;
	int			iReturnCode;
	char	*	pchMsg[MAX_LENGTH];
	char	*	pchErrorMsg[MAX_LENGTH];
} stProcStatus, * pstProcStatus;

typedef struct tagMatrixAttributes
{
	int			iRowCount;
	int			iColCount;
	char	*	pchRowUnit[MAX_LENGTH / 8];
	char	*	pchColUnit[MAX_LENGTH / 8];
	char	*	pchColName[MAX_LENGTH];
	double	*	pdblFirstColData;
} stMatrix, * pstMatrix;

// Macro for optical scanning
#define MAX_SCAN_TASK_COUNT				32

// Macro for TLS
#define TLS_HIGH_PWR					0
#define TLS_LOW_SSE						1

#define TLS_LASER_OFF					0
#define TLS_LASER_ON					1

#define PWR_MODE_MANUAL					0
#define	PWR_MODE_AUTO					1

#define PWR_SLOT_1						1
#define PWR_SLOT_2						2
#define PWR_SLOT_3						3
#define PWR_SLOT_4						4

#define PWR_CH0							0
#define PWR_CH1							1

// Macro for debug
#define VER_MFG							1
#define VER_DEBUG						0

// Macro for upgrade
// upgrade file type (UFT)
#define UFT_NONE						0x00
#define UFT_BOOT_LOADER					0xBB
#define UFT_FIRMWARE_CODE				0xCC
#define UFT_DAC_ASE						0xDD
#define UFT_TO_EEPROM					0xEE

// define MCU Data Sequence Type (MDST)
#define MCU_DSQT_LITTLE_ENDIAN					0x00
#define MCU_DSQT_BIG_ENDIAN						0x01
#define MCU_DSQT_DW_LITTLE_W_BIG				0x02
#define MCU_DSQT_DW_BIG_W_LITTLE				0x03

// Trans protocol
#define X_MODEM							0

#define ADDR_VOA_MIN_IL_DAC				0xA40
#define ADDR_PD_SLOPE_INTERCEPT			0xA80

#define PPAGE_30						0x0C0000
#define PPAGE_31						0x0C4000
#define PPAGE_32						0x0C8000
#define PPAGE_33						0x0CC000
// LUT1: Att->DAC look-up-table
#define VOA_LUT1_SLOPE_BASE_ADDR				PPAGE_30
#define VOA_LUT1_INTERCEPT_BASE_ADDR			PPAGE_31
// LUT2: DAC->Att look-up-table
#define VOA_LUT2_SLOPE_BASE_ADDR				PPAGE_32
#define VOA_LUT2_INTERCEPT_BASE_ADDR			PPAGE_33

#define LUT_TYPE_ATTEN_DAC				0
#define LUT_TYPE_DAC_ATTEN				1

// calibration level
#define PRECISE_CALIB					0
#define COMMON_CALIB					1
#define ROUGH_CALIB						2

// Macro for PD
#define PD_CALIB_POS_OUTPUT_PORT		0
#define PD_CALIB_POS_INPUT_PORT			1

// Macro for CreatePDLUTMode
#define ORIGINAL_SINGLE_OVERALL_BIN		0
#define SINGLE_OVERALL_BIN				1
#define OVERALL_BIN						2

// enum for VOA
enum enumGetILDACMode{
	GET_LOW_TEMPER_IL_DAC,
	GET_ROOM_TEMPER_IL_DAC,
	GET_HIGH_TEMPER_IL_DAC,
	GET_MIN_IL_DAC,
	GET_MAX_IL_DAC
};

#define VOA_VERIFY_MODE_ATTEN		0
#define VOA_VERIFY_MODE_POWER		1

// enum for window
enum enumWindowIdentify{
	W_COMMUNICATION,
	W_CALIB_PD,
	W_CALIB_VOA,
	W_OPTICAL_SCAN,
	W_EDFA_TESTING,
	W_UPGRADE,
	W_DELIVER_PRODUCT,
	W_VIEW_TEST_DATA
};

// Macro for MFG
#define UPDATE_MFG_INFO_MODE_UPGRADING		0
#define UPDATE_MFG_INFO_MODE_DIRECTLY		1
#define UPDATE_MFG_INFO_MODE_MAPPING_FILE	2

// Define the data structure for reference(find laser error)
typedef struct tagReferenceData
{
	BOOL	bDoneReference;

	double	pdblRefWL[MAX_REF_SAMPLE];

	double	pdblRefPower[MAX_REF_SAMPLE];

	double	pdblTLSError[MAX_REF_SAMPLE];

} stReferenceData, * pstReferenceData;

// Define the data structure for PD calibration
typedef struct tagPDCalibData
{
	double	pdblActualPower[MAX_PD_CALIB_SAMPLE];

	double	pdblADC[MAX_PD_CALIB_SAMPLE];

	double	pdblCalculatedPower[MAX_PD_CALIB_SAMPLE];

	double	pdblPowerError[MAX_PD_CALIB_SAMPLE];

} stPDCalibData, * pstPDCalibData;

// Define the data structure for PDSlopeIntercept
typedef struct tagPDSlopeIntercept
{
	double		pdblCalibTemperature[MAX_TEMPERATURE_TYPE];
	double		pdblSlope[MAX_TEMPERATURE_TYPE];
	double		pdblIntercept[MAX_TEMPERATURE_TYPE];
	char	*	pchCalibTime[MAX_TEMPERATURE_TYPE][MAX_LENGTH / 8];

} stPDSlopeIntercept, * pstPDSlopeIntercept;

// Define the data structure for PDVerifyError
typedef struct tagPDVerifyError
{
	double		pdblVerifyTemperature[MAX_TEMPERATURE_TYPE];
	double		pdblVerifyError[MAX_TEMPERATURE_TYPE];
	char	*	pchVerifyTime[MAX_TEMPERATURE_TYPE][MAX_LENGTH / 8];

} stPDVerifyError, * pstPDVerifyError;

// Define the data structure for PD adjusting
typedef struct tagPDAdjusting
{
	double	dblCurModuleTemper;
	char	*	pchCurModuleTemperType[16];

	char	*	pchTheTemperTypeWillBeAdj[16];
	double	dblAdjOffsetSlope;
	double	dblAdjOffsetIntercept;
	int		iAdjType;						// 0: move up, 1: move down, 2: recalibrate, 3: not move
} stPDAdjusting, * pstPDAdjusting;

// Define the data structure for VOA calibration and verify
typedef struct tagVOACalibData
{
	double	pdblSettingAtten[MAX_VOA_CALIB_SAMPLE];

	double	pdblSettingPower[MAX_VOA_CALIB_SAMPLE];

	double	pdblPMPower[MAX_VOA_CALIB_SAMPLE];

	double	pdblMeasuredIL[MAX_VOA_CALIB_SAMPLE];

	double	pdblActualAtten[MAX_VOA_CALIB_SAMPLE];

	double	pdblModulePower[MAX_VOA_CALIB_SAMPLE];

	double	pdblDAC[MAX_VOA_CALIB_SAMPLE];

	double	pdblPowerError[MAX_VOA_CALIB_SAMPLE];

	double	pdblAttenError[MAX_VOA_CALIB_SAMPLE];

	double	pdblRegularAtten[MAX_VOA_CALIB_SAMPLE];

	double	pdblRegularDAC[MAX_VOA_CALIB_SAMPLE];

} stVOACalibData, * pstVOACalibData;

// Define the data structure for the AttenVSDAC
typedef struct tagVOAAttenVSDAC
{
	double	pdblCalibTemperature[MAX_TEMPERATURE_TYPE];
	double	pdblDAC[MAX_TEMPERATURE_TYPE];

	double	dblAtten;
	double	dblSlope;
	double	dblIntercept;

	int		iSampleQuantity;

} stAttenVSDAC, * pstAttenVSDAC;

// Define the data structure for the DACvsAtten
typedef struct tagVOADACvsAtten
{
	double	pdblCalibTemperature[MAX_TEMPERATURE_TYPE];
	double	pdblAtten[MAX_TEMPERATURE_TYPE];

	double	dblDAC;
	double	dblSlope;
	double	dblIntercept;

	int		iSampleQuantity;

} stDACvsAtten, * pstDACvsAtten;

// Define the data structure for the SingleVOAMinILDAC
typedef struct tagSingleVOAMinILDAC
{
	double		pdblCalibTemperature[MAX_TEMPERATURE_TYPE];
	double		pdblMinIL[MAX_TEMPERATURE_TYPE];
	double		pdblDACAtMinIL[MAX_TEMPERATURE_TYPE];
	char	*	pchCalibTime[MAX_TEMPERATURE_TYPE][MAX_LENGTH / 8];

} stSingleVOAMinILDAC, * pstSingleVOAMinILDAC;

// Define the data structure for VOAVerifyError
typedef struct tagVOAVerifyError
{
	double		pdblVerifyTemperature[MAX_TEMPERATURE_TYPE];
	double		pdblAttenModeError[MAX_TEMPERATURE_TYPE];
	double		pdblPowerModeError[MAX_TEMPERATURE_TYPE];
	char	*	pchVerifyTime[MAX_TEMPERATURE_TYPE][MAX_LENGTH / 8];

} stVOAVerifyError, * pstVOAVerifyError;

/*
typedef struct tagOp816XScanParam
{
	DWORD	dwSize;				// no use, default 0
	double	dblStartWL,
			dblStopWL,
			dblStepSize;
	double	dblTLSPower,
			dblPMPower;
	DWORD	dwScanTimes;		// specify the scan times of 816x, default 1
	DWORD	dwScannedChCount;	// how many channels for scan at the same time
	DWORD	dwScanHighChId,		// which channel(iChId) for scan in the range of 32~63, dwScanHighChId = (DWORD)pow(2, iChId % 31)
			dwScanLowChId;		// which channel(iChId) for scan in the range of 0~31, dwScanLowChId = (DWORD)pow(2, iChId)
	DWORD	dwSampleCount;		// return from 816x function
} stOp816XScanParam, *pstOp816XScanParam;
*/

//***************************************************
//****** Definition for configuration
//***************************************************

// Define the application's  parameters
typedef struct tagApplicationPara
{
	BOOL	bReadDataWhenPortOpening;	// read all data when opening port

	BOOL	bCalculateChecksumWhenWrite;// Calculate Checksum When Write data to module

	BOOL	bUseI2CBuffer;				// use I2C buffer

	BOOL	bEnablePopupMessage;

	BOOL	bEnableDebugMode;

	DWORD	dwMaxMessageLine;			// Max Message Line in the list control

	DWORD	dwCycleSendInterval;

	int		iLanguageType;				// language type: 0: Chinese, 1: English

	int		iCmdInfoDlgDspMode;

	char	*	pchWorkingDirectory[MAX_LENGTH];

	BOOL	bAutoCreateDirectory;

	char	*	pchDirForCreate[MAX_LENGTH];

} stApplicationPara, * pstApplicationPara;

// Define the device's  parameters
typedef struct tagDevicePara
{
	int		iComType;

	WORD	wCOMPortIndex;			// COM port index:  1 or 2 

	DWORD	dwCOMBaudRate;			// COM port baud rate value: 9600, 19200, 38400

	DWORD	dwI2CBaudRate;			// I2C Baud rate:            100k, 400k

	WORD	wI2CAdapterAddr;		// I2C address, i.e.: 0xa0

	WORD	wI2CDeviceAddr;			// I2C address, i.e.: 0xa0

	DWORD	iReadCheckTimes;

	DWORD	iCycleTimesInOneUS;

	BOOL	bUseHP8164A;
	BOOL	bUseHP8166A;
	BOOL	bUseHP8169A;

	BOOL	bUse6024E;
	BOOL	bUseFVA3100;

	DWORD	dwHP8164AAddress;    // HP 8164A Address
	DWORD	dwHP8166AAddress;    // HP 8166A Address
	DWORD	dwHP8169AAddress;    // HP 8169A address

	double	dblAlphaValue;

	BOOL	bUseParallelPort;

	double	dblAmbientTemperature;

	double	dblLowTemperLowerThreshold;
	double	dblLowTemperUpperThreshold;
	double	dblStandardLowTemper;

	double	dblRoomTemperLowerThreshold;
	double	dblRoomTemperUpperThreshold;
	double	dblStandardRoomTemper;

	double	dblHighTemperLowerThreshold;
	double	dblHighTemperUpperThreshold;
	double	dblStandardHighTemper;

} stDevicePara, *pstDevicePara;

// Define the module's  parameters
typedef struct tagModulePara
{
	int		iModuleID;				// no use now

	int		iModuleType;			// module type, such as: HW VMUX, LUCENT VMUX, EDFA, etc.

	char	*	pchModuleName[64];

	char	*	pchModuleSN[16];

	char	*	pchModuleSO[16];

	int		iDateType;

	int		iCPUByteSequence;

	DWORD	dwFreqSpacing;

// WB: working band
	double	dblStartFreqWB;
	double	dblEndFreqWB;
	double	dblCentralWL;
	int		iChCountWB;
	DWORD	dwStartChIDWB;
	DWORD	dwEndChIDWB;

	int		iChIdOffset;
// FB: full band
	double	dblStartFreqFB;
	double	dblEndFreqFB;
	int		iChCountFB;
	DWORD	dwStartChIDFB;
	DWORD	dwEndChIDFB;

} stModulePara, * pstModulePara;

// Define the Scanning Task's parameters
typedef struct tagScanningTaskPara
{
	int		iTaskID;

	BOOL	bScanningPDL;

	int		iILCurveType;

	char	*	pchScanningTaskName[MAX_LENGTH / 8];

	char	*	pchPreExecCmdFileFullName[MAX_LENGTH];

	char	*	pchTLSErrorFileFullName[MAX_LENGTH];

	char	*	pchScanningFilePath[MAX_LENGTH];

	stOp816XScanParam	_stOp816XScanParam;

} stScanningTaskPara, * pstScanningTaskPara;

// Define the optical scanning's parameters
typedef struct tagOpticalPara
{
	stScanningTaskPara	_stScanningTaskPara;

	char	*	pchLastScanTaskName[MAX_LENGTH / 8];

	int		iScanTaskCount;

	int		iScanFileCount;

	int		iTestingTemperatureType;

	double	dblPolarizerAngle;

} stOpticalPara, * pstOpticalPara;

// Define the PD's  parameters
typedef struct tagPDPara
{
	int		iPDChNumber;

	int		iPDIndex;

	double	dblPDMaxError;

	double	dblCentralWL;

	double	dblMaxIL;

	DWORD	dwMinADCValue;

	DWORD	dwMaxADCValue;

	double	dblMinPower;

	double	dblMaxPower;

	double	dblPowerStep;

	int		iPDCalibPosition;

	double	dblCalibTemperature;	

	int		iCurCalibTemperType;

	char	*	pchFlagCalibratedLow[16];

	char	*	pchFlagCalibratedRoom[16];

	char	*	pchFlagCalibratedHigh[16];

} stPDPara, * pstPDPara;

// Define the TLS's  parameters
typedef struct tagTLSPara
{
	double	dblTLSPower;

	BOOL	bTLSPowerMode;

	double	dblTLSWL;

	double	dblTLSAtten;

} stTLSPara, * pstTLSPara;

// Define the PowerMeter's  parameters
typedef struct tagPWMPara
{
	BOOL	bIsPMSynchroWithTLS;

	double	dblPMPower;

	double	dblPMWL;

	int		iPMRangeMode;

	double	dblPMAtten;

} stPMPara, * pstPMPara;

// Define the reference's  parameters
typedef struct tagReferencePara
{
	double	dblRefPower;

	double	dblRefStartWL;

	double	dblRefEndWL;

	double	dblRefWLStep;

	double	dblMinAlarmPower;

	char	*	pchReferenceFileName[MAX_LENGTH];

} stReferencePara, * pstReferencePara;

// Define the VOA's  parameters
typedef struct tagVOAPara
{
	int		iVOAChNumber;
	int		iVOAChIndex;
	int		iWLBand;
	double	dblCentralWL;
	DWORD	dwMinDACValue;
	DWORD	dwMaxDACValue;
	int		iCalibAccuracy;
	double	dblMinPower;
	double	dblMaxPower;
	double	dblAttenMinValue;
	double	dblAttenMaxValue;
	double	dblAttenStep;
	DWORD	dwRegressDACCrudeSample;
	DWORD	dwRegressDACExactSample;

	double	dblCalibTemperature;
	int		iCurCalibTemperType;
	char	*	pchFlagCalibratedLow[16];
	char	*	pchFlagCalibratedRoom[16];
	char	*	pchFlagCalibratedHigh[16];

	int		iVOAVerifyMode;
	double	dblVOAMaxError;
	double	dblZeroAttenMaxIL;
	double	dblZeroAttenMinIL;
	double	dblZeroAttenStandardIL;

	double	dblVOAMinILFound;
	DWORD	dwDACAtMinILFound;
	DWORD	dwMaxDACDuringCalib;
} stVOAPara, * pstVOAPara;

// Define the EDFA's parameters
typedef struct tagEDFAPara
{
	double		dblEDFAPDMinInpPower;
	double		dblEDFAPDMaxInpPower;
	double		dblEDFAPDMinOupPower;
	double		dblEDFAPDMaxOupPower;
	double		dblEDFAPDInpPwStep;
	double		dblEDFAPDOupPwStep;
	double		dblEDFAASEMinInpPower;
	double		dblEDFAASEMaxInpPower;
	double		dblEDFAASEMinOupPower;
	double		dblEDFAASEMaxOupPower;
	double		dblEDFAASEInpPwStep;
	double		dblEDFAASEOupPwStep;
	double		dblEDFAPUMPMinInpPower;
	double		dblEDFAPUMPMaxInpPower;
	double		dblEDFAPUMPMinOupPower;
	double		dblEDFAPUMPMaxOupPower;
	double		dblEDFAPUMPInpPwStep;
	double		dblEDFAPUMPOupPwStep;

	double		dblEDFAPUMPRatedCurrent;
	double		dblEDFAPUMPMaxCurrent;

	DWORD		dwPumpMinDAC;
	DWORD		dwPumpMaxDAC;
	DWORD		dwPDMinValidADCInputPort;
	DWORD		dwPDMaxValidADCInputPort;
	DWORD		dwPDMinValidADCOutputPort;
	DWORD		dwPDMaxValidADCOutputPort;

	char		*pchGainConfigFileName[MAX_STRING_LENGTH];

} stEDFAPara, * pstEDFAPara;

// Define the data structure for Gain configuration
typedef struct tagSingleGainConfigInfo
{
	double	dblGainValue;

	BOOL	bIsValid;

	char	*pchValidInputPowerSeq[MAX_STRING_LENGTH];

	double	dblGainError;

	double	dblNFError;

	double	dblFlatnessError;

} stSingleGainConfigInfo, * pstSingleGainConfigInfo;

// Define the data structure for Gain configuration
typedef struct tagAllGainConfig
{
	stSingleGainConfigInfo	_pstSingleGainConfigInfo[MAX_GAIN_VERIFY_SAMPLE];

	int						iGainCount;

} stAllGainConfig, * pstAllGainConfig;

// Define the parameters of LastConfigStatus
typedef struct tagLastConfigStatus
{
	int			iLastModuleType;

	DWORD		dwLastPageIndex;

	char	*	pchLastModuleIdentifier[MAX_LENGTH];

} stLastConfigStatus, * pstLastConfigStatus;

// Define the protocol param item struct
typedef struct tagProtocolParamItem
{
	int			iSN;
	int			iPosInCmdString;
	char	*	pchParamItemID[MAX_LENGTH / 4];
	char	*	pchParamItemName[MAX_LENGTH];
	char	*	pchParamItemValue[2 * MAX_LENGTH];	// maybe unknow
	int			iItemType;
	int			iItemLength;						// maybe unknow
	BOOL		bIsParamValueFixed;
	BOOL		bIsDataField;
	BOOL		bIsCheckSumAddField;
	BOOL		bIsCheckSumSaveField;
	int			iPosInCmdValue;

} stProtParamItem, * pstProtParamItem;

// Define the protocol param struct
typedef struct tagProtocolParameter
{
	stProtParamItem		_stMsgStartID;				// 消息起始符
	stProtParamItem		_stDeviceID;				// 设备地址(ID)
	stProtParamItem		_stDataLen;					// 数据长度
	stProtParamItem		_stMsgLen;					// 消息长度
	stProtParamItem		_stCmdCode;					// 命令代码
	stProtParamItem		_stWorkMode;				// 工作模式
	stProtParamItem		_stE2Addr;					// EEPROM地址
	stProtParamItem		_stChannelID;				// 通道号
	stProtParamItem		_stSendValue;				// 发送数据
	stProtParamItem		_stFeedbackValue;			// 反馈数据
	stProtParamItem		_stAccessType;				// 访问类型
	stProtParamItem		_stStatus;					// 状态字
	stProtParamItem		_stChecksum;				// 校验和
	stProtParamItem		_stReserve1;				// 保留1
	stProtParamItem		_stReserve2;				// 保留2
	stProtParamItem		_stSeparator1;				// 分隔符1
	stProtParamItem		_stSeparator2;				// 分隔符2
	stProtParamItem		_stSeparator3;				// 分隔符3
	stProtParamItem		_stSeparator4;				// 分隔符4
	stProtParamItem		_stMsgEndID;				// 消息结束符
} stProtParam, * pstProtParam;

// Define the protocol config struct
typedef struct tagProtocolConfig
{
	stProtParamItem		_pstProtParamItem[MAX_PROT_PARAM_COUNT];

	int					iProtParamCount;
	int					iFDBKDataCount;
	int					iCmdLength;
	BYTE				pchCmdStringTemplate[2 * MAX_LENGTH];

} stProtocolConfig, * pstProtocolConfig;

// Define the command param struct
typedef struct tagCommandPara
{
	char	*	pchCommandFileFullName[MAX_LENGTH];

	int			iChecksumType;

	char	*	pchSendProtType[MAX_LENGTH / 2];

	char	*	pchFeedbackProtType[MAX_LENGTH / 2];

	char	*	pchLastConfigProtType[MAX_LENGTH / 2];

} stCommandPara, * pstCommandPara;

// Define the command information struct
typedef struct tagCommandInformation
{
	// basic info
	int			iCmdType;						// 0: generic command, 1: MFG command, 2: MFG information command
	char	*	pchCmdCode[MAX_LENGTH / 4];		// Fill to command template
	char	*	pchCmdName[MAX_LENGTH];
	char	*	pchCMDValue[2 * MAX_LENGTH];	// Fill to command template
	char	*	pchCMDValueParam[MAX_DATA_PARAM_COUNT][MAX_LENGTH / 4];
	BOOL		bIsCmdValueFixed;
	BYTE		btFilledCharInValue;
	char	*	pchAlignWay[MAX_LENGTH / 2];
	int			iCmdCodeType;
	int			iDataType;
	int			iDataLength;
	BOOL		bIsDataLenFixed;
	int			iAccessType;
	int			iDataAmplification;
	char	*	pchDataUnit[MAX_LENGTH / 8];
	char	*	pchDescription[MAX_LENGTH];
//------------------------------------------------
	BOOL		bIsCommonCmd;
	BOOL		bIsMfgCmd;
	BOOL		bIsMfgInfoCmd;
	BOOL		bIsFactorySettingCmd;

	// Send info
	char	*	pchCmdSendWay[MAX_LENGTH / 2];
	char	*	pchSendProtType[MAX_LENGTH / 2];
	int			iSendCmdLength;
	BYTE		pchSendMsg[2 * MAX_LENGTH];
	char	*	pchInnerString[2 * MAX_LENGTH];

	// Feedback info
	char	*	pchFeedbackProtType[MAX_LENGTH / 2];
	BOOL		bHasFeedbackMsg;
	char	*	pchFDBKDataParamNameList[MAX_LENGTH];
	int			iFeedbackCmdLength;
	int			iKeyValuePos;
	BYTE		pchFeedbackMsg[4 * MAX_LENGTH];
	char	*	pchFDBKDataParamName[MAX_FDBK_DATA_PARAM_COUNT][MAX_LENGTH / 4];
	char	*	pchFDBKDataParamValue[MAX_FDBK_DATA_PARAM_COUNT][MAX_LENGTH / 4];
	char	*	pchDisplayString[MAX_LENGTH];

	// Other info
	int			iRowIndex;
	char	*	pchWorkMode[MAX_LENGTH / 4];
	DWORD		dwChannelID;
	DWORD		dwE2Addr;
} stCommandInfo, * pstCommandInfo;

// Description of insertion in command info structure
typedef struct tagListCtrlColumn
{
	int			iStartCol;
	int			iCount;
	char		chInsertionValue;

} stListCtrlCol, *pstListCtrlCol;

// Define the MFG command param struct
typedef struct tagMFGCommandPara
{
// Common command-------------------------------------
	char	*	pchCMDGetTemper[MAX_LENGTH];
// VMUX command---------------------------------------
	char	*	pchCMDGetADC[MAX_LENGTH];
	char	*	pchCMDGetPowerInputPort[MAX_LENGTH];
	char	*	pchCMDGetPowerOutputPort[MAX_LENGTH];
	char	*	pchCMDSetAtten[MAX_LENGTH];
	char	*	pchCMDSetPowerOutputPort[MAX_LENGTH];
	char	*	pchCMDSetVOADAC[MAX_LENGTH];
// EDFA command---------------------------------------
	char	*	pchCMDReadADCStage1[MAX_LENGTH];
	char	*	pchCMDReadADCStage2[MAX_LENGTH];
	char	*	pchCMDReadADCStage3[MAX_LENGTH];
	char	*	pchCMDReadADC_TIA[MAX_LENGTH];
	char	*	pchCMDReadADC_AD8304[MAX_LENGTH];
	char	*	pchCMDReadPumpPowerADC[MAX_LENGTH];
	char	*	pchCMDSetPumpCurrent[MAX_LENGTH];
	char	*	pchCMDSetPumpCurrentDAC[MAX_LENGTH];
	char	*	pchCMDGetPumpCurrentDAC[MAX_LENGTH];
	char	*	pchCMDSetAGC[MAX_LENGTH];
	char	*	pchCMDGetAGC[MAX_LENGTH];
	char	*	pchCMDSetAPCPower[MAX_LENGTH];
	char	*	pchCMDGetAPCPowerTotal[MAX_LENGTH];
// upgrade command------------------------------------
	char	*	pchCMDStartUpgradeFW[MAX_LENGTH];
	char	*	pchCMDCancelUpgradeFW[MAX_LENGTH];
	char	*	pchCMDFinishUpgradeFW[MAX_LENGTH];
	char	*	pchCMDResetFW[MAX_LENGTH];
} stMFGCmdPara, * pstMFGCmdPara;

// define MFG command ID
// Common command-------------------------------------
#define MFG_CMD_GetTemper						0
// VMUX command---------------------------------------
#define MFG_CMD_GetADC							1
#define MFG_CMD_GetPowerInputPort				2
#define MFG_CMD_GetPowerOutputPort				3
#define MFG_CMD_SetAtten						4
#define MFG_CMD_SetPowerOutputPort				5
#define MFG_CMD_SetDAC							6
// EDFA command---------------------------------------
#define MFG_CMD_ReadADCStage1					7
#define MFG_CMD_ReadADCStage2					8
#define MFG_CMD_ReadADCStage3					9
#define MFG_CMD_ReadADC_TIA						10
#define MFG_CMD_ReadADC_AD8304					11
#define MFG_CMD_ReadPumpPowerADC				12
#define MFG_CMD_SetPumpCurrent					13
#define MFG_CMD_SetPumpCurrentDAC				14
#define MFG_CMD_GetPumpCurrentDAC				15
#define MFG_CMD_SetAGC							16
#define MFG_CMD_GetAGC							17
#define MFG_CMD_SetAPCPower						18
#define MFG_CMD_GetAPCPowerTotal				19
// upgrade command------------------------------------
#define MFG_CMD_StartUpgradeFW					20
#define MFG_CMD_CancelUpgradeFW					21
#define MFG_CMD_FinishUpgradeFW					22
#define MFG_CMD_ResetFW							23

// Define the MFG information command param struct
typedef struct tagMFGInfoCommandPara
{
	char	*	pchCMDGetFWVer[MAX_LENGTH];

	char	*	pchCMDSetFWVer[MAX_LENGTH];

	char	*	pchCMDGetHWVer[MAX_LENGTH];

	char	*	pchCMDSetHWVer[MAX_LENGTH];

	char	*	pchCMDGetFWDate[MAX_LENGTH];

	char	*	pchCMDSetFWDate[MAX_LENGTH];

	char	*	pchCMDGetModuleName[MAX_LENGTH];

	char	*	pchCMDSetModuleName[MAX_LENGTH];

	char	*	pchCMDGetModuleSN[MAX_LENGTH];

	char	*	pchCMDSetModuleSN[MAX_LENGTH];

	char	*	pchCMDGetProductCode[MAX_LENGTH];

	char	*	pchCMDSetProductCode[MAX_LENGTH];

	char	*	pchCMDGetModuleType[MAX_LENGTH];

	char	*	pchCMDSetModuleType[MAX_LENGTH];

	char	*	pchCMDGetVenderID[MAX_LENGTH];

	char	*	pchCMDSetVenderID[MAX_LENGTH];

	char	*	pchCMDGetCalibDate[MAX_LENGTH];

	char	*	pchCMDSetCalibDate[MAX_LENGTH];

	char	*	pchCMDGetMFGDate[MAX_LENGTH];

	char	*	pchCMDSetMFGDate[MAX_LENGTH];

	char	*	pchCMDGetVenderInternalSN[MAX_LENGTH];

	char	*	pchCMDSetVenderInternalSN[MAX_LENGTH];

	char	*	pchCMDGetVenderInternalFWVer[MAX_LENGTH];

	char	*	pchCMDSetVenderInternalFWVer[MAX_LENGTH];

	char	*	pchCMDGetVenderInternalHWVer[MAX_LENGTH];

	char	*	pchCMDSetVenderInternalHWVer[MAX_LENGTH];

	char	*	pchCMDGetVenderInternalOpticsVer[MAX_LENGTH];

	char	*	pchCMDSetVenderInternalOpticsVer[MAX_LENGTH];

	char	*	pchCMDGetVenderInternalMachineVer[MAX_LENGTH];

	char	*	pchCMDSetVenderInternalMachineVer[MAX_LENGTH];

} stMFGInfoCmdPara, * pstMFGInfoCmdPara;

// Define the delivery command param struct
typedef struct tagDeliveryCmd
{
	char	*	pchCMDRestoreDefault[MAX_LENGTH];

	char	*	pchCMD_0 [MAX_LENGTH];
	char	*	pchCMD_1 [MAX_LENGTH];
	char	*	pchCMD_2 [MAX_LENGTH];
	char	*	pchCMD_3 [MAX_LENGTH];
	char	*	pchCMD_4 [MAX_LENGTH];
	char	*	pchCMD_5 [MAX_LENGTH];
	char	*	pchCMD_6 [MAX_LENGTH];
	char	*	pchCMD_7 [MAX_LENGTH];
	char	*	pchCMD_8 [MAX_LENGTH];
	char	*	pchCMD_9 [MAX_LENGTH];
	char	*	pchCMD_10[MAX_LENGTH];
	char	*	pchCMD_11[MAX_LENGTH];
	char	*	pchCMD_12[MAX_LENGTH];
	char	*	pchCMD_13[MAX_LENGTH];
	char	*	pchCMD_14[MAX_LENGTH];
	char	*	pchCMD_15[MAX_LENGTH];

} stDeliveryCmdPara, * pstDeliveryCmdPara;

// Define the parameters of delivery
typedef struct tagDeliveryPara
{
} stDeliveryPara, * pstDeliveryPara;

// Define the upgrading  parameters
typedef struct tagUpgradePara
{
	int		iUpgradeBinCountMode;

	char	*	pchUpgradeFilePathS19[MAX_LENGTH];

	char	*	pchUpgradeFilePathBin[MAX_LENGTH];

	int			iUpgradeProtType;

	int			iUpdateMFGInfoMode;

	BYTE		btUpgradeFileType;

	int			dwUpgradeAddress;

	int			iUpgradeDataType;

} stUpgradePara, * pstUpgradePara;


//***************************************************
//****** Definition for create report
//***************************************************

#define MAX_CHANNEL_COUNT			64
#define MAX_ATTEN_POINT_COUNT		8
#define BW_NUM						4
#define LIGHT						2.99792458E8

// Define IL curve type
#define IL_CURVE_TYPE_NP			0		// IL curve is narrow peak type
#define IL_CURVE_TYPE_BP			1		// IL curve is broad peak type
#define IL_CURVE_TYPE_VI			2		// IL curve is vibrated type
#define IL_CURVE_TYPE_SE			3		// IL curve is sectioned type
#define IL_CURVE_TYPE_REF			4		// common reference data
#define IL_CURVE_TYPE_RLREF			5		// RL reference data

#define UNIT_TYPE_NM				0
#define UNIT_TYPE_GHZ				1

#define CLEAR_BAND_WIDTH			0
#define NON_CLEAR_BAND_WIDTH		1

/*
typedef struct tagOp816XRawData
{
	double		*pdblWavelengthArray;
	PDWORD		pdwDataArrayAddr;
} stOp816XRawData, *pstOp816XRawData;
*/

typedef struct tagAutoScanParam
{
	BOOL	bReference;
	BOOL	bDoPDL;

	DWORD	dwSize;           // no use here
	DWORD	dwSlotIndex;      // no use here
	DWORD	dwChannelIndex;   // no use here

	double  dblAlphaValue;   // alpha search value

	stOp816XScanParam	_stOp816XScanParam;
} stAutoScanParam, *pstAutoScanParam;

// the auto raw data
typedef struct tagAutoRawData
{
	// the wavelength array
	// we only use the wavelength once
	PDWORD	pdwWavelengthArray;

	// the loss array,the max channel count is 64
	// the pnLossArray[channelIndex] probably
	// including 1*sampleCount(No PDL)
	//        or 5*SampleCount(Do PDL)
	PINT	pnLossArray[MAX_CHANNEL_COUNT];    

} stAutoRawData, *pstAutoRawData;

typedef struct tagChannelInfo
{
	DWORD   dwScannedChCount;
	DWORD   dwILSizeOfEachChannel;
	DWORD   dwSweepCount;
	DWORD*  pdwChannelILArray[MAX_CHANNEL_COUNT];
} stChannelInfo;

typedef struct tagFileInfo
{
	pstAutoScanParam	_pstAutoScanParam;
	DWORD			dwFileHeadLength;

	DWORD*			pdwWavelength;
	DWORD			dwTotalWLSize;

	stChannelInfo	_stChannelInfo;
} stFileInfo, * pstFileInfo;

// max or min il
typedef struct tagMaxMinILInfo
{	   
	DWORD	dwMaxIL;			// max IL value
	DWORD	dwMaxILIndex;		// max IL index
	DWORD	dwMinIL;			// min Il value
	DWORD	dwMinILIndex;		// min IL index

	DWORD	dwSearchRangeLeftIndex;
	DWORD	dwSearchRangeRightIndex;
	BOOL	bIsPassBand;		// is pass band or block band
} stMaxMinILInfo;

//  Center wavelength at 3dB down
typedef struct tagCentralWLInfo
{
	double	dblRightWL;				// righ wavelength (nm)
	double	dblRightFreq;
	DWORD	dwRightIndex;
	double	dblLeftWL;				// left wavelength
	double	dblLeftFreq;
	DWORD	dwLeftIndex;
	double	dblCentralWL;
	double	dblCentralFreq;
} stCentralWLInfo;

typedef struct tagITUInfo
{
	double	dblITUFreq;				// frequency
	double	dblITUWL;				// wavelength
	int	    iScannedChannelID;		// channel index
	BOOL    bITUChFound;			// indicate that the ITU channel found
	BOOL	bIsSubBand;				// is sub band or non-sub band
} stITUInfo;

typedef struct tagExcelData
{
	double	dblITUWL;				// The Wavelength of itu
	double	dblILAtITU;				// The Insertion Loss at ITU grid
	double	dblCentralWL;			// Wavelength
	double	dblWLAccuracy;			// WL Accuracy 
	double	dblMaxIL;				// MAX IL
	double	dblMinIL;				// Min IL
	double	dblWDL;					// WDL
	double	dblRipple;				// Ripple
	double	dblBWAt_0Dot5dB;		// Bandwidth (0.5db,1db,3db)
	double	dblBWAt_1dB;
	double	dblBWAt_3dB;
	double	dblBWAt_20dB;
	double	dblPDL;
//	double	dblPDLAt_0dB;			// PDL(0-5dB Att,5-10dB Att)
//	double	dblPDLAt_5dB;
//	double	dblPDLAt_10dB;
//	double	dblPDLAt_15dB;
//	double	dblPDLAt_20dB;

	double	dblISOTotal;
	double	dblISOAdj;				// Isolation(Adjacent channel ,Non-Adjacent channel)
	double	dblISONonAdj;
	double	dblISORightAdj;
	double	dblRightNonAdj;
	double	dblISOLeftAdj;
	double	dblISOLeftNonAdj;

	double	dblCrosstalkAdj;		// Crosstalk(Adjacent channel ,Non-Adjacent channel and total)
	double	dblCrosstalkNonAdj;
	double	dblCrosstalkTotal;
	double	pdblCrosstalkNonAdj[MAX_CHANNEL_COUNT + 2];

	double	dblMinRLDB;				// Min RL

} stExcelData;

// Define the structures related to report forms

//1. S/O structure
typedef struct tagSerialOrder
{
	BOOL		bOutput;
	int			iRowIndex;
	int			iColIndex;
} stSerialOrder, *pstSerialOrder;

//2. S/N structure
typedef struct tagSerialNumber
{
	BOOL		bOutput;
	int			iRowIndex;
	int			iColIndex;
} stSerialNumber, *pstSerialNumber;

//3. The structure of ILAtITU
typedef struct tagILAtITU
{
	BOOL		bOutput;
	int			iColIndex;
} stILAtITU, *pstILAtITU;

//4. CentralWL structure
typedef struct tagCentralWL
{
	BOOL		bOutput;
	int			iColIndex;
} stCentralWL, *pstCentralWL;

//5. CentralWL accuracy structure
typedef struct tagCWAccuracy
{
	BOOL		bOutput;
	int			iColIndex;
	int			iUnitType;			// Type: 0: nm, 1: GHz
} stCWAccuracy, *pstCWAccuracy;

//6. Max IL structure
typedef struct tagMaxIL
{
	BOOL		bOutput;
	int			iColIndex;
} stMaxIL, *pstMaxIL;

//7. Ripple structure
typedef struct tagRipple
{
	BOOL		bOutput;
	int			iColIndex;
} stRipple, *pstRipple;

//8. Bandwidth structure
typedef struct tagBandwidth
{
	BOOL		bOutput;
	int			iColIndex;
	int			iUnitType;			// Type: 0: nm, 1: GHz
	int			iILPointCount;
	int			piILPoint[MAX_ATTEN_POINT_COUNT];
} stBandwidth, *pstBandwidth;

//9. PDL structure
typedef struct tagPDL
{
	BOOL		bOutput;
	int			iColIndex;
	int			iAttenPointCount;
	int			piAttenPoint[MAX_ATTEN_POINT_COUNT];
} stPDL, *pstPDL;

//10. Isolation structure
typedef struct tagIsolation
{
	BOOL		bOutput;
	int			iColIndex;
	BOOL		bOutputAdj;
	BOOL		bOutputNonAdj;
	BOOL		bOutputRightAdj;
	BOOL		bOutputRightNonAdj;
	BOOL		bOutputLeftAdj;
	BOOL		bOutputLeftNonAdj;
} stIsolation, *pstIsolation;

//11. Crosstalk structure
typedef struct tagCrosstalk
{
	BOOL		bOutput;
	int			iColIndex;
	BOOL		bOutputAdj;
	BOOL		bOutputNonAdj;
	BOOL		bOutputTotal;
} stCrosstalk, *pstCrosstalk;

//12. ReturnLoss structure
typedef struct tagReturnLoss
{
	BOOL		bOutput;
	int			iColIndex;
} stReturnLoss, *pstReturnLoss;

// Report forms structure
typedef struct tagReportForms
{
	stSerialOrder		_stSerialOrder;
	stSerialNumber		_stSerialNumber;
	stILAtITU			_stILAtITU;
	stCentralWL			_stCentralWL;
	stCWAccuracy		_stCWAccuracy;
	stMaxIL				_stMaxIL;
	stRipple			_stRipple;
	stBandwidth			_stBandwidth;
	stPDL				_stPDL;
	stIsolation			_stIsolation;
	stCrosstalk			_stCrosstalk;
	stReturnLoss		_stReturnLoss;

	char			*	pchRawDataFilterString[MAX_LENGTH / 4];
	int					iTestingTemperatureType;

	int					iFirstRowID;
	char			*	pchScanningFilePath[MAX_LENGTH];
	char			*	pchTemplateFileFullName[MAX_LENGTH];
	char			*	pchFIRPath[MAX_LENGTH];

	BOOL				bUseDefaultScanningFilePath;
	BOOL				bUseDefaultTemplateFileFullName;
	BOOL				bUseDefaultFIRPath;

//--------------------------------------------------------
	int					iPassbandType;
	double				dblITUClearPassband;
	double				dblCrosstalkAdjPassband;
	double				dblCrosstalkNonAdjPassband;
} stReportFormsPara, *pstReportFormsPara;

//--------------------------------------------------------

// Define the configuration struct which include all configuration information
typedef struct tagConfiguration
{
	stApplicationPara		* _pstApplicationPara;
	stDevicePara			* _pstDevicePara;
	stModulePara			* _pstModulePara;
	stOpticalPara			* _pstOpticalPara;
	stPDPara				* _pstPDPara;
	stPMPara				* _pstPMPara;
	stReferencePara			* _pstReferencePara;
	stTLSPara				* _pstTLSPara;
	stVOAPara				* _pstVOAPara;
	stReportFormsPara		* _pstReportFormsPara;
	stDeliveryCmdPara		* _pstDeliveryCmdPara;
	stUpgradePara			* _pstUpgradePara;

	// command configuration
	stCommandPara			* _pstCommandPara;
	stMFGCmdPara			* _pstMFGCmdPara;
	stMFGInfoCmdPara		* _pstMFGInfoCmdPara;
	stCommandInfo			* _pstCommandInfo;
	stProtocolConfig		* _pstProtConfigWrite,
							* _pstProtConfigRead,
							* _pstProtConfigFeedback;

	// EDFA configuration
	stEDFAPara				* _pstEDFAPara;
	stAllGainConfig			* _pstAllGainConfig;

	// other configuration
	stLastConfigStatus		* _pstLastConfigStatus;
	stProcStatus			* _pstProcStatus;
} stConfigInfo, * pstConfigInfo;

//定义归零配置文件参数
typedef struct tagZeroRefference 
{
	int ZeroRefHour;
	double ZeroRefMax;
	double ZeroRefMin;
}stZeroRefferencr;


#endif
