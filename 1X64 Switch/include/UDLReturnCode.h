#ifndef UDL_RETURN_CODE_HEADER
#define UDL_RETURN_CODE_HEADER

#define DATA_TYPE_TRACE "TRACE"
#define DATA_TYPE_ALLTRACE "ATRACE"
#define DATA_TYPE_GRAPHICS "GRAPHICS"
#define DATA_TYPE_DATA "DATA"

#define FILE_TYPE_BMP "BMP"
#define FILE_TYPE_CSV "CSV"

// Define a structure for bit-wise access
typedef struct tagDeviceType
{
    BYTE     bType;         /* Mainframe Type */
    BYTE     bNum;         /* Mainframe Serial Number*/
}stDevType, *pstDevType; 

typedef struct tagModule
{
    BYTE     bSlot;         /* Mainframe slot number */
    BYTE     bChan;         /* Slot channel */   
}stModule, *pstModule;

typedef struct tagDeviceInformation
{
  char pszName[256];
  char pszSerialNum[256];
  char pszFWVer[256];
  char pszHWVer[256];
  char pszMFDate[256];
}stDevInfo, *pstDevInfo; 

typedef union tagDevCfg
{
    DWORD          dwDeviceCfg;
    stDevType      Mainframe;
    stModule       Module;
}DEVCFG;

#define SUCCESS 0

#define UNEXPECT_OCCUR				0x000000FF
//Error code define
// 0x00000000
//	 ^^^^^^^^^^
//	 ABCCDDEE
//A: Device type
//B: Communication type
//C: Device address (LAN:port index)
//D: Operation type
//E: Operation content


//Device type
#define ERROR_OSA					0x10000000
#define ERROR_PM					0x20000000
#define ERROR_VOA					0x30000000
#define ERROR_TLS					0x40000000
#define ERROR_PDLD					0x50000000

//Communication type
#define ERROR_COMMUNICATION_INFO	0x01000000
#define ERROR_COMMUNICATION_GPIB	0x02000000
#define ERROR_COMMUNICATION_LAN		0x03000000
#define ERROR_COMMUNICATION_COM		0x04000000
#define ERROR_COMMUNICATION_USB		0x05000000

//Operation type
#define ERROR_WRITE					0x00000100
#define ERROR_READ					0x00000200
#define ERROR_OPEN					0x00000300
#define ERROR_CLOSE					0x00000400
#define ERROR_SERIALPOLL			0x00000500
#define ERROR_WAIT_FOR_COMPLETION	0x00000600
#define ERROR_CLEAR					0x00000700
#define ERROR_REQ_COMPLETION		0x00000800
#define ERROR_GET_ERRORINFO			0x00000900
#define ERROR_LOGIN					0x00000A00
#define ERROR_RUN_OVERTIME			0x00000B00

//Operation content
#define ERROR_WL					0x00000001
#define ERROR_AVERIAGE_TIME			0x00000002
#define ERROR_PW					0x00000003
#define ERROR_RBW					0x00000004
#define ERROR_RESET					0x00000005
#define ERROR_UNIT					0x00000006
#define ERROR_ATTEN					0x00000007
#define ERROR_ANALYSIS				0x00000008
#define ERROR_PW_OFFSET				0x00000009
#define ERROR_WL_COUNT				0x0000000A
#define ERROR_NF					0x0000000B
#define ERROR_GAIN					0x0000000C
#define ERROR_SNR					0x0000000D
#define ERROR_SRC_PW_OFFSET			0x0000000E
#define ERROR_AMP_PW_OFFSET			0x0000000F
#define ERROR_WL_OFFSET				0x00000010
#define ERROR_CH_WL_OFFSET			0x00000011
#define ERROR_CH_PW_OFFSET			0x00000012
#define ERROR_SENS					0x00000013
#define ERROR_SWEEP_POINT			0x00000014
#define ERROR_SWEEP_MODE			0x00000015
#define ERROR_INITIAL				0x00000016
#define ERROR_SWEEP_DATA			0x00000017
#define	ERROR_LOCAL_CONTROL			0x00000018
#define	ERROR_SWEEP					0x00000019
#define	ERROR_WROK_MODE				0x0000001A
#define	ERROR_ADDRESS				0x0000001B
#define	ERROR_TRACE					0x0000001C
#define	ERROR_TRACE_ATTRIBUTE		0x0000001D
#define ERROR_SAVE_DATA				0x0000001E
#define ERROR_RANGE_MODE			0x0000001F
#define ERROR_ENABLE				0x00000020
#define ERROR_DISENABLE				0x00000021
#define ERROR_ATTEN_OFFSET			0x00000022
#define ERROR_STATUS				0x00000023
#define ERROR_INFO					0x00000024
#define ERROR_PW_RANGE				0x00000025
#define ERROR_WL_RANGE				0x00000026
#define ERROR_SRC_POW				0x00000027
#define ERROR_COHERENCE_CONTROL		0x00000028
#define ERROR_SWITCH_CHANNEL		0x00000029
#define ERROR_NF_LIB_CALCULATE		0x0000002A

#endif