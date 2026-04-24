// Copyright @2005, Oplink communication,Inc
// The data type and the const define here
// Revision 1.0  05-17-2005

#ifndef  _LUCENT_INTERFACE_DATA_TYPE_H
#define  _LUCENT_INTERFACE_DATA_TYPE_H

#define    BEEP_FREQUENCY            280
#define    BEEP_DURATION             10

#define    FACTOR_TEC_VOLTAGE_FOR_HUAWEI           1000.0

#define    MINUTE_IN_MS                    6000


#define  SIZE_RESISTER_VOLTAGE_OF_VOA_THERMISTER    26
// delete it 2005-07-20 by yangjie
// replace  it with the stTestingConfigure.
//#define    ZERO_ATTEN_MAX_IL         1.0  // 7.0
//#define    ZERO_ATTEN_MIN_IL         0.5  //6.0

// 2005-07-06 modified 
//#define    ZERO_ATTEN_MIN_IL_CRITERIA  0.5 //5.5

#define    MAX_LIST_LINES            2000

#define    TEMPERATURE_WAIT_TIME     6.0E5 // 600000


//********************************************************************************************
// Begin of compile macro definition
#define    chSTR2(x)                #x
#define    chSTR(x)                 chSTR2(x)
#define    chMSG(desc)              message(__FILE__"("chSTR(__LINE__)"):" #desc)

// i.e.
// #pragma chMSG(Fix this later)
// it output like a line as follows:
// C:\file.h(82) : Fix this later

// End  of compile macro definition
//*******************************************************************************************


#define   DPRAM_MAX_ADDRESS     0x07FF

// note: the DPRAM address size is 1K word
#define   DPRAM_ADDRESS_MOD     0x03FF


//************************************************************************************************
// Begin of message of the Adapter RS232 to DPRAM definition
#define  TX_BUFFER_SIZE       256
#define  RX_BUFFER_SIZE       256

//  Set / Get Message of the RS232 to DPRAM Adapter
#define MSG_GET    0x00
#define MSG_SET    0x01

//  Message format Index of the RS232 to DPRAM Adapter
#define MSG_IDX_MSGID       0x00
#define MSG_IDX_LENGTH      0x01
#define MSG_IDX_STATUS      0x02
#define MSG_IDX_ACCESS      0x03
#define MSG_IDX_OBJECTID    0x04
#define MSG_IDX_INSTANCE    0x05
#define MSG_IDX_PAYLOAD     0x06

#define MSG_BASE_LENGTH     0x07

#define RX_QUEUE_IDX_SIZE   4095
#define RX_QUEUE_LENGTH     4096
#define TX_BUFFER_LENGTH    256

#define TIME_OUT            5000


//  Message Status Information of the RS232 to DPRAM Adapter
#define MSG_STATUS_CMD_SUCCESS      0x00
#define MSG_STATUS_IDLE             0x01
#define MSG_STATUS_INCOMPLETE       0x02
#define MSG_STATUS_MSG_COMPLETE     0x03
#define MSG_STATUS_MSG_VALID        0x04
#define MSG_STATUS_INVALID_LENGTH   0x05
#define MSG_STATUS_INVALID_ACCESS   0x06
#define MSG_STATUS_INVALID_OBJECTID 0x07
#define MSG_STATUS_INVLID_DATA      0x08
#define MSG_STATUS_INVALID_CHECKSUM 0x09
#define MSG_STATUS_CMD_BUSY         0x0A
#define MSG_STATUS_EXEC_FAILED      0x0B
#define MSG_STATUS_AUTONOMOUS_CMD   0xFF

//  ObjectID Index of the RS232 to DPRAM Adapter 
#define OBJID_SERIAL_NUMBER 		0x00
#define OBJID_HW_REVISION			0x08
#define OBJID_FW_REVISION			0x0A
#define OBJID_MFG_DATE				0x0C
#define OBJID_DPRAM_MSG             0x40
#define OBJID_PORT_H                0x80
#define OBJID_PORT_M                0x81
#define OBJID_PORT_P                0x82
#define OBJID_PORT_T                0x83
#define OBJID_END                   0xFF

// Instance Index of the RS232 to DPRAM Adapter
#define  INSTANCE_IDX_0             0x00
#define  INSTANCE_IDX_1             0x01
#define  INSTANCE_IDX_2             0x02
#define  INSTANCE_IDX_3             0x03
#define  INSTANCE_IDX_4             0x04
#define  INSTANCE_IDX_5             0x05
#define  INSTANCE_IDX_6             0x06
#define  INSTANCE_IDX_7             0x07

//	Bit definitions of 0 through 15
#define BIT_0	0x0001
#define BIT_1	0x0002
#define BIT_2	0x0004
#define BIT_3	0x0008
#define BIT_4	0x0010
#define BIT_5	0x0020
#define BIT_6	0x0040
#define BIT_7	0x0080
#define BIT_8	0x0100
#define BIT_9	0x0200
#define BIT_10	0x0400
#define BIT_11	0x0800
#define BIT_12	0x1000
#define BIT_13	0x2000
#define BIT_14	0x4000
#define BIT_15	0x8000
// End of the message of the RS232 to DPRAM definition
//************************************************************************************************


//************************************************************************************************
// command type for huawei
#define CMD_HUAWEI_VMUX_COMMUNICATION               0
#define CMD_HUAWEI_VMUX_GET_SUPPLIER_ID             1
#define CMD_HUAWEI_VMUX_GET_VERSION_NUMBER          2
#define CMD_HUAWEI_VMUX_GET_MANUFACTURE_NUMBER      3
#define CMD_HUAWEI_VMUX_FIRMWARE_VERSION            4
#define CMD_HUAWEI_VMUX_GET_STATUS                  5
#define CMD_HUAWEI_VMUX_GET_TEMPERATURE             6
#define CMD_HUAWEI_VMUX_GET_SYSTEM_TIMER            7
#define CMD_HUAWEI_VMUX_SET_ALL_VOA_ATTEN           8
#define CMD_HUAWEI_VMUX_CODE_CONFIRM_FLAG           9
#define CMD_HUAWEI_VMUX_GET_CODE0_VERSION           10
#define CMD_HUAWEI_VMUX_GET_CODE1_VERSION           11
#define CMD_HUAWEI_VMUX_GET_CODE2_VERSION           12
#define CMD_HUAWEI_VMUX_SET_VOA_ATTEN               13
#define CMD_HUAWEI_VMUX_GET_VOA_SINGLE_ATTEN        14
#define CMD_HUAWEI_VMUX_STORE_VOA_ATTEN             15
#define CMD_HUAWEI_VMUX_SET_TEMPERATURE             16
#define CMD_HUAWEI_VMUX_GET_VMUX_TEMPERATURE        17
#define CMD_HUAWEI_VMUX_SET_VOA_DAC                 18
#define CMD_HUAWEI_VMUX_GET_VOA_DAC                 19
#define CMD_HUAWEI_VMUX_GET_VOA_TEMPERATURE         20
#define CMD_HUAWEI_VMUX_GET_INSIDE_TEMPERATURE_ADC  21
#define CMD_HUAWEI_VMUX_GET_FIRMWARE_FINISH_DATE    22
#define CMD_HUAWEI_VMUX_SET_PRODUCT_SERIAL_NUMBER   23    
#define CMD_HUAWEI_VMUX_UPGRADE_FIRMWARE            24
#define CMD_HUAWEI_VMUX_ERASE_BLOCK_FIRMWARE        25
#define CMD_HUAWEI_VMUX_FIRMWARE_VERIFICATION       26
#define CMD_HUAWEI_VMUX_SWITCH_CODE_BLOCK           27
#define CMD_HUAWEI_VMUX_GET_MFG_INFO                28
#define CMD_VMUA_WATCH_MULTI_VALUE                  29
#define CMD_HUWEI_VMUX_AD5381_OFFSET_VALUE          30
#define CMD_HUAWEI_VMUX_AD5381_GAIN_VALUE           31
#define CMD_HUAWEI_VMUX_VOA_ALL_CHANNLE_DAC         32
#define CMD_VMUX_CURRENT_CALIBRATE_VOA                      33
#define CMD_VMUX_CURRENT_VERIFY_VOA                         34
#define CMD_HUAWEI_VMUX_DOWNLOAD_VOA_LUT                    35
#define CMD_HUAWEI_VMUX_DOWNLOAD_SINGLE_CHANNEL_LUT         36
#define CMD_VMUX_CURRENT_SCAN_REFERENCE                     37
#define CMD_VMUX_SET_DEFAULT_FACTORY_INFO                   38
//#define CMD_VMUX_CURRENT_SCAN_TESTING                       39
#define CMD_VMUX_CURRENT_SCAN_OPTICAL                       39
#define CMD_HUAWEI_VMUX_GET_VOA_all_ATTEN                   40
#define CMD_HUAWEI_VMUX_SET_TIME_CALIBRATE                   41
#define CMD_HUAWEI_VMUX_FIND_PHYSICAL_CH                     42
#define CMD_VMUX_MATERIAL_CALIBRATE_VOA                     43
#define CMD_HUAWEI_VMUX_SET_SUPPLIER_ID                     44
#define CMD_HUAWEI_VMUX_SET_VERSION_NUMBER                   45

#define CMD_HUAWEI_VMUX_CHECK_TEST_CHANNEL       60

#define CMD_HUAWEI_VMUX_SET_AWG_TYPE             61
#define CMD_HUAWEI_VMUX_READ_AWG_TYPE             62

// If you add new HW command, start from 100
//Get Vender ID
#define CMD_HUAWEI_VMUX_GET_VENDER_ID				100
//Find current actual physical channel
//#define CMD_HUAWEI_VMUX_FIND_PHYSICAL_CH			101
// ---------------------------------------------------------------
// Added by xiayongw on 2006-10-26
// Define some message
#define MY_MSG_INITIAL_DIAGRAM			(WM_USER + 106)
#define MY_MSG_SEND_PLOT_DATA			(WM_USER + 107)
#define MY_MSG_CALC_SLOPE_INTERCEPT		(WM_USER + 108)
#define MY_MSG_SEND_SLOPE_INTERCEPT		(WM_USER + 109)

//30 -- PD Calibration
#define CMD_HUAWEI_PD_CALIBRATION			32
//31 -- PD Verification
#define CMD_HUAWEI_PD_VERIFICATION			33

#define OBJECT_ID_GET_ADC_VALUE							0x00
#define OBJECT_ID_GET_POWER_FROM_MODULE					0x01
#define OBJECT_ID_GET_MODULE_TEMPERATURE				0x14
// End added by xiayongw on 2006-10-26
// ---------------------------------------------------------------
//************************************************************************************************

//************************************************************************************************
// Begin of the command of the Lucent VMUX const definition
//  command code definition
#define  CMD_VMUX_NOP                     1
#define  CMD_VMUX_SET_VOA                 2
#define  CMD_VMUX_SET_VENDOR_PARM         3   // TBD
#define  CMD_VMUX_SOFT_RESET              4
#define  CMD_VMUX_INIT_VOA                5
#define  CMD_VMUX_REPORT_TEMPERATURE      6
#define  CMD_VMUX_RESTORE_VOA_SETTING     7
#define  CMD_VMUX_DPRAM_TEST              8
#define  CMD_VMUX_ERROR_TEST              9
#define  CMD_VMUX_FIRMWARE_INITIALIZE     12
#define  CMD_VMUX_FIRMWARE_DOWNLOAD       13
#define  CMD_VMUX_FIRMWARE_INSTALL        14

// some manufacture command definition 
#define  CMD_VMUX_SET_VOA_DAC             20
#define  CMD_VMUX_GET_AWG_TEMPERATURE     21  // Get AWG temperature
#define  CMD_VMUX_GET_CASE_TEMPERATURE    22
#define  CMD_VMUX_GET_TEC_TEMPERATURE     23  // Get TEC temperature
#define  CMD_VMUX_GET_DAC_MONITOR_1       24
#define  CMD_VMUX_GET_DAC_MONITOR_2       25

// used for read and write DPRAM
#define  CMD_VMUX_GET_DPRAM_VALUE         26
#define  CMD_VMUX_SET_DPRAM_VALUE         27

// integrated command definition
#define  CMD_VMUX_CALIBRATE_VOA           30
#define  CMD_VMUX_VERIFY_VOA              31
#define  CMD_VMUX_CREATE_LUT              32
#define  CMD_VMUX_TUNE_LUT                33
#define  CMD_VMUX_UPGRADE_LUT             34
#define  CMD_VMUX_WATCH_TEMPERATURE       35
#define  CMD_VMUX_UPGRADE_FIRMWARE        36

// IO operation command definition
#define  CMD_VMUX_SET_START_IO            40
#define  CMD_VMUX_SET_RESET_IO            41
#define  CMD_VMUX_GET_DONE_IO             42
#define  CMD_VMUX_GET_ERROR_IO            43
#define  CMD_VMUX_GET_BUSY_IO             44

#define  CMD_VMUX_SET_TLS_PARAM				49
// TLS SCAN command definition
#define  CMD_VMUX_SCAN_REFERENCE          50
#define  CMD_VMUX_SCAN_TESTING            51

#define  CMD_VMUX_SET_AWG_ONOFF                 60     // set AWG  on off
#define  CMD_VMUX_SET_AWG_TEMPERATURE           61     // set AWG temperature
#define  CMD_VMUX_SET_TEC_ONOFF                 62     // set TEC  on off
#define  CMD_VMUX_SET_TEC_TEMPERATURE           63    // Set TEC temperature

#define  CMD_VMUX_SET_AWG_TMPT_THRESHOLD         65  // set AWG temperature high threshold
//#define  CMD_VMUX_SET_AWG_TMPT_LOW_THRESHOLD    66  // set AWG temperature low threshold

#define  CMD_VMUX_SET_TEC_TMPT_THRESHOLD        67  // set TEC temperature high threshold
//#define  CMD_VMUX_SET_TEC_TMPT_LOW_THRESHOLD    68  // set TEC temperature low threshold

#define  CMD_VMUX_GET_POWER_SUPPLY_ADC          69  // get power supply ADC value

#define  CMD_VMUX_ILLEGAL_COMMAND               90  // the illegal command

#define  CMD_VMUX_COMPARE_COMMAND               91  // compare the command 

#define  CMD_VMUX_RETRIEVE_PWM_VALUE            92  // retrieve the PWM value from the DPRAM

#define  CMD_VMUX_DO_ATTEN_REFERENCE            95  // do VOA attenuation reference

//#define  CMD_VMUA_WATCH_MULTI_VALUE             96  // watch multi chip value

#define  CMD_VMUX_UPGRADE_ONE_CHANNEL_LUT       97  // upgrade one channel VOA LUT

// If you add new HW command, start from 300

// End of the command of the Lucent VMUX definition
//*************************************************************************************************

//*************************************************************************************************
// begin of the temperature threshold definition
#define  THRESHOLD_AWG_HIGH_TEMPERATURE        85  // AWG high temperature threshold
#define  THRESHOLD_AWG_LOW_TEMPERATURE         65  // AWG low temperature threshold

#define  AWG_HIGH_TEMPERATURE        950  // AWG high temperature threshold
#define  AWG_LOW_TEMPERATURE         650  // AWG low temperature threshold


#define  THRESHOLD_TEC_HIGH_TEMPERATURE        65  // TEC high temperature threshold
#define  THRESHOLD_TEC_LOW_TEMPERATURE         55  // TEC low temperature threshold

// End of the temperature threshold definition
//*************************************************************************************************

//*************************************************************************************************
// begin of the chip type definition
#define  CHIP_VMUX_AWG_TEMPERATURE        0
#define  CHIP_VMUX_CASE_TEMPERATURE       1
#define  CHIP_VMUX_TEC_TEMPERATURE        2
#define  CHIP_VMUX_DAC_MONITOR_1          3
#define  CHIP_VMUX_DAC_MONITOR_2          4
// end of the chip type definition
//*************************************************************************************************


//*************************************************************************************************
// Begin of the address of the DPRAM mapping
// Dual port RAM mapping
#define  ADDR_DPRAM_SUPPLIER_ID          0x0000
#define  ADDR_DPRAM_VERSION_NUMBER       0x0001
#define  ADDR_DPRAM_BARCODE_NUMBER       0x0005  // from 0x0005 to 0x000b
#define  ADDR_DPRAM_BARCODE_END          0x000B  // barcode end address
#define  ADDR_DPRAM_BIT_MAPPED_CMD       0x0020  // Bit mapped command
#define  ADDR_DPRAM_COMMAND_CODE         0x0021
#define  ADDR_DPRAM_COMMAND_DATA         0x0022
#define  ADDR_DPRAM_BIT_MAPPED_STATUS    0x0023  // Bit mapped status,indicates command execution status: ready,error.
#define  ADDR_DPRAM_STATUS_CODE_2        0x0024  // status code(contains result of the executed command)
#define  ADDR_DPRAM_ERROR_CODE           0x0025
#define  ADDR_DPRAM_TEMPERATURE_LIMIT_HIGH     0x0026
#define  ADDR_DPRAM_TEMPERATURE_LIMIT_LOW      0x0027
#define  ADDR_DPRAM_CURRENT_TEMPERATURE        0x0028     // current device temperature
#define  ADDR_DPRAM_HARDWARE_FAILURE_CODE      0x0029     // hardware failure detected
#define  ADDR_DPRAM_DOWNLOAD_FILE_SIZE_HIGH_WORD  0x0030  // download binary file size high word
#define  ADDR_DPRAM_DOWNLOAD_FILE_SIZE_LOW_WORD   0x0031  // download binary file size low word
#define  ADDR_DPRAM_DOWNLOAD_BUFFER_OFFSER_ADDR   0x0032  // download buffer DPRAM address offset
#define  ADDR_DPRAM_DOWNLOAD_BUFFER_LENGTH        0x0033  // download buffer length in words
#define  ADDR_DPRAM_DIFFERENTIAL_ATTENUATION      0x0200  // attenuation increments to existing VOA settings
#define  ADDR_DPRAM_VOA_ATTENUATION_SETTING       0x0300  // Running sum of the differential attenuation data including the offset loaded by the initialize command
#define  ADDR_DPRAM_STORE_VOA_ATTENUATION         0x0600  // Every time "Store" command is issued,the 0x0300 values are also copied at 0x0600
#define  ADDR_DPRAM_FIRMWARE_DOWNLOAD_CODE        0x0780  // Available for firmware upgrade

// some manufacture command in the DPRAM mapping
#define  ADDR_DPRAM_VOA_DAC                       0x000E   // VOA DAC setting
#define  ADDR_DPRAM_AWG_TEMPERATURE               0x000C   // AWG Temperature
#define  ADDR_DPRAM_CASE_TEMPERATURE              0x0151   // case temperature
#define  ADDR_DPRAM_TEC_TEMPERATURE               0x0152   // TEC temperature ADC,
#define  ADDR_DPRAM_TEC_REFERENCE_ADC_HALF_VALUE  0x0153   // TEC reference ADC half value,
#define  ADDR_DPRAM_DAC_MONITOR_1                 0x0153   // DAC monitor 1
#define  ADDR_DPRAM_DAC_MONITOR_2                 0x0154   // DAC monitor 2

#define  ADDR_DPRAM_AWG_ONOFF                     0x0700   // AWG on off address
#define  ADDR_DPRAM_AWG_TEMPERATURE_SET_POINT     0x0701   // AWG Temperature setting value
#define  ADDR_DPRAM_TEC_ONOFF                     0x0702   // TEC on off address
#define  ADDR_DPRAM_TEC_TEMPERATURE_SET_POINT     0x0703   // TEC Temperature setting value
#define  ADDR_DPRAM_AWG_TEMPERATURE_THRESHOLD 0x0704  // AWG  temperture threshold 0x704,0x705
//#define  ADDR_DPRAM_AWG_LOW_TEMPERATURE_THRESHOLD 0x0705  // AWG low termperature threshold
#define  ADDR_DPRAM_TEC_TEMPERATURE_THRESHOLD 0x0706  // TEC temperature threshold 0x706,0x707
//#define  ADDR_DPRAM_TEC_LOW_TEMPERATURE_THRESHOLD 0x0707  // TEC low temperature threshold
#define  ADDR_DPRAM_POWER_SUPPLY_ADC               0x0708  // Power supply ADC

#define  ADDR_PWM_OUTPUT_NUMBER                    0x0710  // the address of the PWM output number 

#define  ADDR_PWM_OUTPUT_VALUE                     0x0800  // the address of the PWM output value 

#define  ADDR_TMPT_OUTPUT_VALUE                    0x0C00   // the Address of the temperature output value

//  status code 2(0x0024) definition
#define  STATUS_CODE_2_SUCCESS                     0x0000
#define  STAUTS_CODE_2_FAIL                        0xFFFF
#define  STATUS_CODE_2_CHECKSUM_FAIL               0xFFF0
#define  STATUS_CODE_2_INCOMPATIBLE_FIRMWARE       0xFFF1

// Barcode length
#define  BARCODE_LENGTH                            7
#define  YEAR_2000                                 2000

#define  PULSE_START_IO                           2  // -\_/- >200ns
#define  PULSE_RESET_IO                           2  // -\_/- >200ns

// End the address of the DPRAM mapping  definition
//***************************************************************************************************

//*************************************************************************************************
//Begin of  the firmware of the RS232 to DPRAM const definition
// note : we defined the VOA_COUNT const variable is not the really variables
#define VOA_COUNT                          80//64
//#define VOA_COUNT_HUAWEI_40_CHANNEL        40

#define PPAGEWinSize				0x4000L
#define PPAGEWinStart				0x8000L

#define	WORDS_PER_SECTOR	256
#define	BYTES_PER_SECTOR	512
//	64 more bytes to avoid overflow
#define FW_BUFFER_SIZE 		BYTES_PER_SECTOR + 64

//	Reset causes:
#define RESET_HW		0x01
#define RESET_SW		0x02
#define	RESET_COLD		0xFF

//	version block
#define VER_MAIN		0x0001
#define VER_BOOT		0xFFFF

#define  SIZE_ONE_SECTOR_IN_BYTE   512
//new protocal modify 1024 to 512
#define  SIZE_ONE_SECTOR_IN_BYTE_VOA_DATA 512 //1024
#define  SIZE_ONE_SECTOR_IN_BYTE_VOA_THREE_DATA 0x200

#define  TMPT_NUM 3

//	Address of Boot information
#define ADDR_BOOT_INFO			0x0400
//  Address of Version Number for testing
#define ADDR_VER_INFO 			0x0440
//	Address of register maps in EEPROM
#define ADDR_MEMORY_MAP 		0x0800

//	Address of ResetCause in EEPROM
#define ADDR_RESET_CAUSE		0x0990

#define  ADDR_VOA_LUT           0x308000 // 

//#define  ADDR_VOA_LUT_CONVERT_ADDR  0x0C0000 // the address is the 0x308000 convert address
#define  ADDR_VOA_LUT_CONVERT_ADDR  0x00380000 
//the address of voa lut begin the channel more than 33
#define  ADDR_VOA_LUT_CONVERT_ADDR_32 0x003EC000

//	Address of download buffer for Main code - Page 34
#define ADDR_MAIN_BAK			0x348000

//	Address of download buffer for seconday ISR of MAIN - Page 37
#define ADDR_2ND_ISR_MAIN_BAK	0x378000

//	Address of download buffer for library functions of MAIN - 0x378800
#define ADDR_LIBFUNC_MAIN_BAK	0x378800

//	Address of main code entry point - Page 38
#define ADDR_MAIN_START			0x388000
//	Address of boot loader entry point - Page 3C
#define ADDR_BOOT_START			0x3C8000
//	Address of download buffer for boot loader - Page 3D
#define ADDR_BOOT_BAK			0x3D8000

//	Address of Library functions of Main - 0x4000 - 0x57FF (6K)
#define	ADDR_LIBFUNC_MAIN		0x3E8000
//	Address of Library functions of Boot - 0x5800 - 0x6FFF (6K)
#define	ADDR_LIBFUNC_BOOT_BAK	0x3E9800

//	Address of secondary ISR Entries - 0x7000 - 0x77FF (2K)
#define ADDR_2ND_ISR_VECTOR		0x3EB000
//	Address of ISR Entries backup - 0x7800 - 0x7FFF (2K)
#define ADDR_2ND_ISR_BOOT_BAK	0x3EB800L

//	Address of Init code for Main/Boot - 0xC000 - 0xC7FF (2K)
#define ADDR_INIT				0x3F8000L
//	Address of Init code backup for Boot - 0xC800 - 0xCFFF (2K)
#define ADDR_INIT_BAK			0x3F8800L

//	Address of library functions for Boot - 0xD000 - 0xEFFF (8K)
#define ADDR_LIBFUNC_BOOT		0x3F9000L

//	Address of primary ISR entry - 0xF000 - 0xFFFF (4K)
#define ADDR_1ST_ISR			0x3FB000L
// End of the firmware RS232 to DPRAM address mapping definition
//***********************************************************************************************


//***********************************************************************************************
#define  LIGHT					2.99792458E8
//#define  VER_MFG                               TRUE
#define   FVA3100_INIT_ATTEN                  0
// define the const length
#define  SECTOR_LENGTH                         32
#define  MAXLINE				               256
// note : the two variables are same, you can select any one for use
#define  MAX_LINE                              256

// the TLS max output power is set to 3dBm
#define  TLS_MAX_POWER                         3

#define  MAX_STEP                              1024

// 1.6V/2.5V/0.8 * 4096 = 3276.8
//#define  DAC_MAX_VALUE                         3300
//#define  DAC_MAX_VALUE_FOR_HUAWEI              2600

// definition VOA max attenuation 15 dB
#define  VOA_MAX_ATTEN                         200 

#define  VOA_MAX_ATTEN_FOR_HUAWEI              220

#define  FLEX_POINT_INDEX                      250

// For testing only
#define  DUPON_LUT_ONLY_FOR_TEST               FALSE

// define the VOA lut header 
#define  IDX_VOA_LUT_RANGE                      0
#define  IDX_VOA_LUT_CHECKSUM                   1
#define  IDX_VOA_LUT_INDEX                      2
#define  IDX_VOA_LUT_VER                        3
#define  IDX_VOA_LUT_RESERVED                   4
#define  IDX_VOA_LUT_FLEXPOINT                  255

// define the VOA lut header value
#define  VOA_LUT_RANGE                        150  //15 dB
#define  VOA_LUT_VER                           1

#define  TARGET_CHIP_IS_BIG_ENDIAN             TRUE
// ---------------------------------------------------------------
// Added by xiayongw on 2006-10-26
#define MAX_PARAMETER_LENGTH					128

// ---------------------------------------------------------------
//***********************************************************************************************

//***********************************************************************************************
typedef struct tagLuxVMUXTestInfo
{
	//  Firmware version;Hardware Version
	DWORD	dwFMVerHigh, dwHWVerHigh;
	DWORD	dwFMVerLow, dwHWVerLow;
	TCHAR	tszWorkStation[8]; // work station name
	TCHAR	tszOperatorID[8];  // Operator ID
	TCHAR	tszTravelCardNo[20]; // serial number
	time_t	tmStart, tmEnd;    // test start time and stop time
	
	//  test case ID
    DWORD dwTestCaseID;
	//  test type : PD_calibrate/PD_verify/VOA calibrate / VOA verify
	DWORD dwTestType;

	//   test temperature
	double 	dblTestTemperature;

	//  TLS wavelength 
	double	dblTLSWL;
	// TLS output power
	double  dblTLSPower;

	DWORD   dwChannelIndex;  // channel index

	//  Power  offset : e.g. 1% coupler offset; output power offset.
	double  dblPowerOffset;
	
	//	Reserve some more space	make it up to 128 byte
	DWORD	pdwReserved[7];
	
} stLuxVMUXTestInfo, *PLuxVMUXTestInfo;

// define the testing process
#define  PLOT_PD_CALIBRATE                 0
#define  PLOT_PD_VERIFY                    1
#define  PLOT_VOA_CALIBRATE                2
#define  PLOT_VOA_VERIFY                   3
#define  PLOT_WATCH_TEMPERATURE            4
#define  PLOT_TLS_SCAN_REFERENCE           5
#define  PLOT_TLS_SCAN_TESTING             6
#define  PLOT_WATCH_PWM                    8
#define   PLOT_VOA_REFERENCE               9

#define  CREATE_LUT                        7

// graph axes setting value
typedef struct tagPlotAxesInfo
{
    //  PLOT_PD_CALIBRATE , PLOT_PD_VERIFY ,PLOT_VOA_CALIBRATE ,PLOT_VOA_VERIFY  ,PLOT_WATCH_TEMPERATURE  
	DWORD       dwPlotType;
	// sample count
	DWORD       dwSampleCount;
	// X axis setting value
	double      dblXMin;
	double      dblXMax;
	double      dblXStep;
	bool        bXIncrease;

	// Y0 axix setting value
	double      dblY0Min;
	double      dblY0Max;
	bool        bY0Inverted;

	// Y1 axix setting value
	double      dblY1Min;
	double      dblY1Max;
	bool        bY1Inverted;

	// until to here, all the data size is 84 bytes
	// set the data structure to 128 bytes
	// reserved for future used
	DWORD       dwReserved[13];
	
} stPlotAxesInfo, *PPlotAxesInfo;

typedef struct tagAutoTestInfo
{
	TCHAR				tszSN[MAXLINE];            // product's SN
	TCHAR				tszTesterID[MAXLINE];      // tester ID
	CTime				timeTest;                  // the time for test

	int					pnCmdFail[MAXLINE];  // others command
	int					pnVOAFail[MAXLINE];  // VOA command
	int					pnPDFail[MAXLINE];   // PD command
	int					pnLOSFail[MAXLINE];  // LOS command
	int                 pnSwitchFail[MAXLINE];  // switch command
	int                 pnIllegalFail[MAXLINE]; // illegal command

	DWORD				dwCmdTestNum;          // command test number
	DWORD				dwVOATestNum;          // VOA test number
	DWORD				dwPDTestNum;           // PD  test number
	DWORD				dwLOSTestNum;          // LOS test number
	DWORD               dwSwitchTestNum;       // switch test number
	DWORD               dwIllegalTestNum;      // illegal command test number

	DWORD               dwLoopCount;    // loop count for the auto testing count
///#pragma  chMSG(modify the Template structure later)
	//	stLuxVMUXTemplate	stTemplate;     // the template for test
} stAutoTestInfo, *PAutoTestInfo;


typedef struct tagDeviceConfigureInfo
{
	BOOL             bUseHP8164A;
	BOOL             bUseHP8169A;
	BOOL             bUse6024E;
	BOOL             bUseFVA3100;
	BOOL	         bMFGVer;

	DWORD            dwModuleType;
 
	// 2005-08-16
	DWORD           dwLanguageInfo;

	BOOL             bDebugVer;

	BOOL             bUseHP8166A;

	// set the data structrue to 36 bytes
	DWORD            dwReserved[5];

} stDeviceConfigureInfo, *PDeviceConfigureInfo;

//***********************************************************************************************
//

// temperary for debug
#define  MAX_TMPT_COUNT                4
#define  MAX_PORT_COUNT                4
#define  CHANNEL_12                    0x0002
#define  CHANNEL_13                    0x0004

#define  ALPHA_ERROR_VALUE              -1111
#define  TLS_MAX_SAMPLE_COUNT           12000

typedef struct tagPathTestInfo
{
	DWORD	dwSampleCount, dwTmptCfg;
	BOOL	bDoPDL;
	DWORD	dwReserved;
} stPathTestInfo, *PPathTestInfo;


typedef struct tagDUTTestInfo
{
	DWORD	dwVersionMajor, dwVersionMinor;
	TCHAR	tszWorkStation[8];
	TCHAR	tszOperatorID[8];
	TCHAR	tszDeviceModel[20];
	TCHAR	tszTravelCardNo[20];
	time_t	tmStart, tmEnd;

	DWORD	dwTmptCount;
	DWORD	pdwTemperatures[MAX_TMPT_COUNT];
	//	dblCentralWL is primarily used to pick up central WL
	//	from registration dialog box
	double	dblCentralWL;
	// TRUE:  test all ports of DUT at the same temperature, save time
	// FALSE: test all possible ports with the same fuse, 
	//		  then change temperature, high accurate
	BOOL	bTempFuse;
	DWORD	dwChannelCfg;

	stPathTestInfo	PathInfo[MAX_PORT_COUNT][MAX_PORT_COUNT];

	DWORD	dwProcessIndex;	// 0: SB, 1: SA, 2: PT, 3: PTC
	//	By here, structure size is 344 bytes
	//	Reserve some more space to make it up to 360 bytes
	DWORD	pdwReserved[3];
} stDUTTestInfo, *PDUTTestInfo;

// 2005-05-25 add for VOA verify accuray
/*typedef struct tagVOAAccurayData
{

	// this channel has reference
	BOOL       bHasReference;
	
	// the channel reference wavelength(nm)
	double     dblReferenceWavelength;

	// the channel reference power (dBm):this is optical power 
	double     dblReferencePower;

	// the zero attenuation  power
	double     dblZeroAttenPower;

	//  the forward low accuracy value (from 0 dB to 1.0dB)
 	double     dblForwardLowAccuracy;
	// judge the VOA forward low accuracy is OK or not
	BOOL       bVOAForwardLowOK;
	// the forward low max difference index value(from 0 to 1.0dB)
	double      dbForwardLowMaxAccurIndex;
	//the forward high accuracy value (from 1.0dB to 10dB)
	double     dblForwardHighAccuracy;
	// the forward high max difference index value(from 1.0dB to 10dB)
	double      dbForwardHighMaxAccurIndex;
	// judge the VOA forward high is OK or not
	BOOL       bVOAForwardHighOK;

	// the backward setting
	// the backward low accuracy value (from 0 dB to 1.0dB)
 	double     dblBackwardLowAccuracy;
	// judge the VOA forward low accuracy is OK or not
	BOOL       bVOABackwardLowOK;
	// the forward low max difference index value(from 0 to 1.0dB)
	double      dbBackwardLowMaxAccurIndex;
	//the forward high accuracy value (from 1.0dB to 10dB)
	double     dblBackwardHighAccuracy;
	// the forward high max difference index value(from 1.0dB to 10dB)
	double      dbBackwardHighMaxAccurIndex;
	// judge the VOA forward high is OK or not
	BOOL       bVOABackwardHighOK;

	// until to here, the structure size is 84 bytes
	// set the structure to 96bytes
	DWORD      dwReserved[3];

}stVOAAccurayData, *PVOAAccurayData;
*/
//Modify by hanfei on 2010-09-25
typedef struct tagVOAAccurayData
{

	// this channel has reference
	BOOL       bHasReference;
	
	// the channel reference wavelength(nm)
	double     dblReferenceWavelength;

	// the channel reference power (dBm):this is optical power 
	double     dblReferencePower;

	// the zero attenuation  power
	double     dblZeroAttenPower;

	//  the forward low accuracy value (from 0 dB to 10dB)
 	double     dblForwardLowAccuracy;
	// judge the VOA forward low accuracy is OK or not
	BOOL       bVOAForwardLowOK;
	// the forward low max difference index value(from 0 to 10dB)
	double      dbForwardLowMaxAccurIndex;
	//the forward middle accuracy value (from 10dB to 15dB)
	double     dblForwardMiddleAccuracy;
	// the forward high max difference index value(from 1.0dB to 10dB)
	double      dbForwardMiddleMaxAccurIndex;
	// judge the VOA forward middle is OK or not
	BOOL       bVOAForwardMiddleOK;
	//the forward high accuracy value (from 15dB to 20dB)
	double     dblForwardHighAccuracy;
	// the forward high max difference index value(from 15dB to 20dB)
	double      dbForwardHighMaxAccurIndex;
	// judge the VOA forward high is OK or not
	BOOL       bVOAForwardHighOK;

	// the backward setting
	// the backward low accuracy value (from 0 dB to 10dB)
 	double     dblBackwardLowAccuracy;
	// judge the VOA forward low accuracy is OK or not
	BOOL       bVOABackwardLowOK;
	// the forward low max difference index value(from 0 to 10dB)
	double      dbBackwardLowMaxAccurIndex;
	//the forward high accuracy value (from 10dB to 15dB)
	double     dblBackwardMiddleAccuracy;
	// the forward high max difference index value(from 10dB to 15dB)
	double      dbBackwardMiddleMaxAccurIndex;
	// judge the VOA forward high is OK or not
	BOOL       bVOABackwardMiddleOK;
	//the forward high accuracy value (from 15dB to 20dB)
	double     dblBackwardHighAccuracy;
	// the forward high max difference index value(from 15dB to 20dB)
	double      dbBackwardHighMaxAccurIndex;
	// judge the VOA forward high is OK or not
	BOOL       bVOABackwardHighOK;

	// until to here, the structure size is 84 bytes
	// set the structure to 96bytes
	DWORD      dwReserved[3];

}stVOAAccurayData, *PVOAAccurayData;

// 2005-05-25 add for VOA verify accuray
typedef struct tagVOATotalAccurayData
{
	// the VOA index
	DWORD               dwVOAIndex;
	
	stVOAAccurayData    pstSingleChanneVOAAccurData[VOA_COUNT];


}stVOATotalAccurayData, *PVOATotalAccurayData;

// define the TEC temperature look up table
typedef struct tagDuponVOAThermisterLUT
{
	int          nTemperatureIndex[32];  //
	
	double       dblResisterValue[32];   // unit(Ohm)

}stDuponVOAThermisterLUT, *PDuponVOAThermisterLUT;

// define the testing configure information
typedef  struct  tagTestingConfigure
{
	int		nProductType; // product type: huawei VMUX or Lucent VMUX

    DWORD	dwLanguageInfo;  // Language select

	int		nVOACount;  // the total VOA count

	DWORD	dwMaxDACValue; // the max VOA DAC value
    float	fZeroAttenMinIL; // 0dB Attenuation's IL (Min value)
	float	fZeroAttenMaxIL; // 0dB Attenuation's IL (max value)
    float	fZeroAttenStandardIL; // 0dB Attenuation's IL (max value)

	DWORD	dwCOMBaudRate;  // baud rate value

	DWORD	dwHP8164AAddress; // HP8164 Address

	DWORD	dwHP8166AAddress; // HP8166 Address

	DWORD	dwHP8169AAddress; // HP8169 Address

//	double	dblHP8169AAngle;

	DWORD	dwWaitTimeInms;   // wait time for ms unit

	BOOL	bDetectTECReference;

	BOOL	bDoLoopPopupMessage;
	// set it to 32 bytes
	// reserverd for future use
	DWORD	dwReserved[2];

}stTestingConfigure,*PTestingConfigure;

#endif
