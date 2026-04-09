// 126S_45V_Switch_AppDlg.h : header file
//
//{{AFX_INCLUDES()
#include "126sDataType.h"
#include "DataDefine.h"
#include "Command.h"
#include "excel9.h"
//#include "op816x.h"
#include "ntgraph1.h"
#include "ColorListBox.h"
#include "OpCRC32.h"
#include "XMLControl.h"
#include "Calpolyn.h"
#include "RLTest.h"
#include "SrcBankSocket.h"
#include "afxwin.h"
#include "Z4671Command.h"
//}}AFX_INCLUDES
#include "PDBoxForMCS.h"
#include "CMDInstrument.h"
#include "Calpolyn.h"

#if !defined(AFX_126S_45V_SWITCH_APPDLG_H__5D7E34BA_30D3_427F_88DB_A8E9D4A69CB4__INCLUDED_)
#define AFX_126S_45V_SWITCH_APPDLG_H__5D7E34BA_30D3_427F_88DB_A8E9D4A69CB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMy126S_45V_Switch_AppDlg dialog

//#include "CMy1830C.h"

#define PI 3.1415926
#define  MAX_ROW   501
#define  MAX_COLUM  501
#define  MAX_LINE   256

#define COLOR_BLACK			RGB(0, 0, 0)
#define COLOR_BLUE			RGB(0, 0, 255)
#define COLOR_GREEN			RGB(0, 255, 0)
#define COLOR_RED			RGB(255, 0, 0)
#define COLOR_YELLOW		RGB(255, 255, 0)
#define COLOR_LIGHT_BLUE	RGB(0, 255, 255)
#define COLOR_PINK			RGB(255, 0, 255)
#define COLOR_WHITE			RGB(255, 255, 255)
#define COLOR_ORINGE		RGB(255, 128, 0)
#define COLOR_AMETHYST		RGB(138, 43, 226)
#define COLOR_GRAY		    RGB(128, 128, 128)

#define READ_INFO               0
#define SET_OPTICAL_ROUTE       1
#define GET_PORT_NUMBER         2
#define GDAC                    3
#define GET_ALARM               4
#define SET_RESET               5
#define DOWNLOAD_FW             6
#define OPLINK_VERSION          7
#define SETX_COMMAND            8
#define SETY_COMMAND            9
#define GET_TEMP                10
#define SHOW_VOL                11
#define SPRT_TESTER_BOX         12

//#define MAX_COUNT               2046

//===========
#define MAX_BUF_DATA    64
#define XMODEM_NUL                  0X00
#define XMODEM_SOH					0x01
#define XMODEM_STX					0x02
#define XMODEM_EOT					0x04
#define XMODEM_ACK					0x06
#define XMODEM_NAK					0x15
#define XMODEM_CAN					0x18
#define XMODEM_EOF                  0x1A
#define XMODEM_STA                  0xAA
#define XMODEM_CRCCHR               'C'

#define XMODEM_BLOCK_HEAD_INDEX     0
#define XMODEM_BLOCK_NO_INDEX       1
#define XMODEM_BLOCK_NON_INDEX      2
#define	XMODEM_BLOCK_DATA_INDEX		3
#define XMODEM_BLOCK_BODY_SIZE_128	128
#define XMODEM_BLOCK_BODY_SIZE_1K	1024

#define XMODEM_BLOCK_HEAD_SIZE      1
#define XMODEM_BLOCK_NO_SIZE        2
#define XMODEM_BLOCK_CHK_SIZE       1 

#define	XMODEM_COMMUNICATION_FAIL	0
#define	XMODEM_DOWNLOAD_FAIL		1
#define	XMODEM_DOWNLOAD_SUCCESS		2

#define MAXCMDNMB 13
#define MAXCMDBT 20

//#define MAX_DAC  4095
#define MAX_VOLTAGE 60000
//===========================

enum{VOL_DATA,VOL_DIFF_DATA,DARK_DATA,IL_ROOM_DATA,IL_LOW_DATA,
IL_HIGH_DATA,RE_IL_DATA,RL_DATA,ISO_DATA,CT_DATA,
MIN_CT_DATA,DIR_DATA,PDL_DATA,WDL_DATA,ADJUST_DATA,
TDL_DATA,SWITCH_TIME_DATA,END_DATA};

enum{PM_8163A,PM_1830C,PM_OPLINK,PM_N7744};

//CWinThread*		g_hRunTestThread;

typedef  struct  tagLimitPara  //获取参数无纸化的限制
{
	double  dblMaxVoltage;
	double  dblMinVoltage;
	double  dblMaxXVoltage[32];
	double  dblMinXVoltage[32];
	double  dblMaxYVoltage[32];
	double  dblMinYVoltage[32];

	double  dblMaxXVoltageDiff[32];
	double  dblMinXVoltageDiff[32];
	double  dblMaxYVoltageDiff[32];
	double  dblMinYVoltageDiff[32];
	double  dblMaxRoomIL[32];  
	double  dblMinRoomIL[32];
	double  dblMaxCT[32];
	double  dblMinCT[32]; 
	double  dblMaxRepeatIL[32];
	double  dblMinRepeatIL[32];
	double  dblMaxDarkIL[32];
	double  dblMinDarkIL[32];
	double  dblMaxISO[32];
	double  dblMinISO[32];
	double  dblMaxRL[32];
	double  dblMinRL[32];
	double  dblMaxPDL[32];
	double  dblMinPDL[32];
	double  dblMaxWDL[32];
	double  dblMinWDL[32];
	double  dblMaxTDL[32];
	double  dblMinTDL[32];
}stLimitPara,*pstLimitPara;

typedef  struct  tagOpticalPara  //获取对应通道的扫描配置参数
{
	double  dblRoomIL[32];  
	double  dblLowIL[32];
	double  dblHighIL[32];
	double  dblCT[32][32]; 
	double  dblTDL[32];
	double  dblRepeatIL[32];
	double  dblILDiff[32];
	double  dblDarkIL[32];
	double  dblSwitchTime;
	double  dblRL[32];
}stOpticalPara,*pstOpticalPara;

typedef struct tagReCHConfig
{
	int nCHCount;
	int nStartIndex;
	int nVolatgeX[32];
	int nVolatgeY[32];
	int nPMIndex; //指定用某一个PM通道探测
}stReCHConfig;

typedef struct tagWorkStation
{
	BOOL bTLSEnable;
	int  nTLSPort;
	int  nProductPort;
	CString strGoldenSampleName;

	int nPMPort;

	int nSW4THPort;
	//TLS config
	int nID;
	char pchServer[256];
	WORD wPort;
	char pWLPath[1024];
}stWorkStation;

typedef struct tagRefConfig
{
	int  nTimeLimit;
	double  dblDiffLimit;
}stRefConfig;

typedef struct tagSNList
{
	char    strSN[128];
	int     nCHCount;
	int     nCOMCount; //入光数量
}stSNList;

#define MAX_SW_COUNT 34

#define DOUBLE_RANGE 0.0000001
#define MAX_COM_COUNT 4

#define IDX_TEMP_LOW 0
#define IDX_TEMP_ROOM 1
#define IDX_TEMP_HIGH 2
#define IDX_TEMP_RESERVED 3
class CMy126S_45V_Switch_AppDlg : public CDialog
{
	// Construction
public:
	stLutSettingZ4671 m_pstLUTInfoZ4671;
	stLutBundleHeader1 pstBundleHeader1;
	stLutBundleHeader2 pstBundleHeader2;
	stImageHeader  pstImageHdr;  //LUT头
	stSectorHeader pstSectorHeader;

	CString m_strVoltagePath;
	int m_nSNIndex;
	stSNList m_stSNList[MAX_SW_COUNT];
	Z4671Command m_CmdModule;
	CPDBoxForMCS m_PDBoxCmd;
	CCMDInstrument	m_CmdInstrument;
	BOOL m_bDeviceOpen;
	//
	CSrcBankSocket *m_pSrcBankSocket;
	//BOOL m_bReadSN
	stReCHConfig m_stReCHConfig;

	CString m_strGetSN;
	stRefConfig m_stRefConfig;
	double m_dblBreakIL;

	int m_nCenterX[5][32];
	int m_nCenterY[5][32];
	CString m_strSpec;
	BOOL m_bReady;  //准备测试
	stWorkStation m_stWorkStation;
	BOOL GetGoldenSampleConfig();
	//
    int m_nPortVoltDiffX[32];
	int m_nPortVoltDiffY[32];
	//
	BOOL UpdateNewATMData();
	BOOL m_bVoltagePass;
	BOOL m_bShowResult;
	BOOL GetParaRangeFromATMS();
	UDLSERVERLib::IEngineMgrPtr g_pEngine;
	UDLSERVERLib::IITLSInterfacePtr g_pTLS;
    UDLSERVERLib::IIPMInterfacePtr g_pPM;

	double m_dblSystemRL;
	BOOL m_bRefRL;
	BOOL GetConfigFile();
	BOOL OpenPMDevice();
	TCHAR m_chComputerName[256];
	BOOL m_bConnect1830C;
	BOOL CreatePath();
	BOOL CreateFullPath(LPCSTR lpPath);
	CString m_strItemName;
	CString m_strLocalPath;
	CString m_strConfigPath;
	CString m_strWorkStationPath;

	BOOL SetVoltage(int nDAC);
	BOOL ConfigLight(int nChannel,int nTime);
	BOOL StartLight(int nChannel);
	CRLtest m_dlgRLTest;
	BOOL SaveDataToCSV(int nType, int nChannel = 32,BOOL bGetFile = FALSE, int nTemp = IDX_TEMP_ROOM );
	BOOL m_bGetPaperlessTemp;
	SYSTEMTIME m_stStartTime;
	BOOL UpdateATMData();
	BOOL UpdateATMData2X16();
	BOOL GetCSVData();
	BOOL GetPDPower(int nChannel,double* pdblPower);
	BOOL SETTesterY(int y);
	BOOL SETTesterX(int x);
	double m_dblPDRef;
	double ReadPDPower();
	BOOL GetCT(double *dblValue,int nCount);
	BOOL SPRTProductCH(int nCH);
	BOOL AutoScan4x3(int nChannel, CString strFileINI, BOOL bScanRange);
	BOOL m_bTesterPort;
	BOOL m_bTesterALL;
	BOOL SPRTTesterCH(int nCH);
	BOOL AutoScan4X4(int nChannel,CString strFileINI,BOOL bScanRange);
	BOOL SetFileByMSMQ(CString strData, CString &strErr);
	BOOL GetExcelData();
	BOOL GetATMTestTemplate();
	BOOL GetPowerFromOPCom(int nPMIndex, BYTE bSlot, BYTE bChannel, double *pdblPower);
	BOOL SetWLFromOPCom(int iPMIndex, byte bSlot, byte bChan, long lWL);
	COpComm  m_comPM[5];
	BOOL SPRT(int nChannel);
    static	UINT ThreadFunc(LPVOID pParam);
	BOOL UpdatePMData();
	int m_nSwitchComPortCount;
	BOOL m_bTestIL;
	int m_iTimerCount;
	int m_iLightCount;
	int m_nIndex;
	BOOL m_bScan;
	BOOL SCANRANGE();
    BOOL m_bReadPowerStop;
	double ReadWL(int nCH);
	
	CXMLControl  m_ctrXMLCtr; //无纸化
	stLimitPara m_stLimitPara[MAX_COM_COUNT]; //COM1/COM2
	stOpticalPara m_stOpticalPara[MAX_COM_COUNT];

	int  m_nAdjustCH[4]; //调节扫描对应的四个顶点通道坐标
	int  m_nReadPwrCH[32];
	COpCRC32  m_CRC32;
	BOOL WriteBinFile(CString strBinName, CString strSave, DWORD dwLengthWrite, PBYTE pByteBinData);
	BOOL CreateSwitchPointBin(CString strSave, int nFile);
	BOOL CheckTempLutFile(CString strSave);
	BOOL TDLTest();
	BOOL WDLTest();
	BOOL ILCTTest();
	BOOL CreateBinFileAndDownload();
	BOOL CreateLutFile();
	BOOL CalAutoScanTest();
	BOOL  m_bILPass[16];
	BOOL  m_bCTPass[16];
	BOOL  m_bTDLPass[16];
	BOOL  m_bWDLPass[16];
	BOOL  m_bPDLPass[16];
	void SendCommand(CString strCommand);
	WORD XmodemCRC16(char *pbStream, WORD wLength);
	BOOL XmodemloadData(char *pbBinData, WORD wDownloadSize, BOOL bFileDone);
	BOOL SendRevCommand(CString command, COpComm *pcommandtask, CString setvalue="");
	BOOL DataDownLoad(CString strFileName);
	VOID DownLoadFW();
	double GetModuleTemp();
	double m_dblTDLIL[16][200];
	double m_dblTDLTemp[200];
	int  m_nTDLCount;
	BOOL m_bTDLStop;
	BOOL CheckParaPassOrFail(int nParaType,double dblValue,int nPort=0);
	CString	m_strPN;
	double m_dblWDLRef[16][50];
	double m_dblWDL[16][50];
	BOOL SetPWMWL(double dblWL,int nChannel=32);
	BOOL ConnectIP(int nPMIndex);
	BOOL ConnectPM();
	DWORD  m_nPMAddress[16];
	int  m_nPMType[16];
	int  m_nPMCount;
	int  m_nCHCount;
	VOID GetVolDataFormFile(int InputPort);
	VOID CalCulateVol();
	BOOL GetPower(int iBox, int iChannel, double *dblPower);
	BOOL LoadPMCfg(pstPMCfg pstPMInfo);
	VOID CalculateAngle(int nVolX0,int nVolY0,int nVolX3,int nVolY3,int nVolX8,int nVolY8,int nVolX11,int nVolY11);
	CString m_strNetFile;
	BOOL FindMyDirectory(CString strSubdir0, CString strSubdir1,CString strSubdir2);
	double m_dblReferencePower[32];
	double m_dblRefPowerRe; //反向归零数据
	int    m_Xbase[32];
	int    m_Ybase[32];
	double m_dblZeroWDL[100];
	BOOL AutoPointScan(int nChannel);
	BOOL AutoScan(int nChannel,CString strFileINI,BOOL bScanRange=TRUE);
   // CRect m_rOpenDevice;
	CCalpolyn  m_Calpolyn; //计算斜率截距
	BOOL  m_bOpenRLport;
	void ReadZeroFile(double dblTLSPower);
	BOOL m_bStartExcel;
	void CreateExcelFile();
	void CloseExcelFile();
	BOOL SetPWMWL(int nChannel,double dblWL);
	BOOL GetPowerFromIP(BYTE bBox, BYTE bSlot, BYTE bChannel, double *pdblPower);
	BOOL SetWLFromIP(int socket_fd, BYTE bSlot, BYTE bChannel, LONG lWL);
	double ReadPWMPower(int nChannel);
	BOOL bCoverFile(int nPara); //是否覆盖光学参数； 
    BOOL m_bOpenPM;           //是否设置了R152光功率计
	BOOL m_bOpen8164;           //是否打开了8164激光器
	BOOL m_bChangeSwitch;       //是否随机切换了开关
	void CallScanFile();
	BOOL bAutoTestIL();
	double ReadR152Power(int nChannel); //读取R152光功率计功率
	BOOL SetR152WL(int nChannel,double dblWaveLength); //设置R152光功率计波长
//	VOID LogInfo(CString strFileName, LPCTSTR tszlogMsg, BOOL bAppend); //构造函数
	CString m_strTemplatePath;                      //模板路径
	VOID SetLocalTestingConfiguration();
	BOOL SWToChannel(int nChannel,bool bSwitchTime = FALSE);                 //切换通道
	double m_dblRLS;                              //系统回损

	BOOL SCHN(int nchannel,int x,int y);	
	CString m_strOpticalDataPath;                  //数据路径
    CString m_strScanDataPath;                     //扫描数据路径
	/*
	COp816X	m_HP816X;
	COp816X m_PM1830C[4];
	COp816X m_1830CRL;
	COp816X m_PMN7744[3];
	*/
	//	CMy1830C m_PM1830C[4];
//	CR152Set R152set;
	double dblRefIL;
	struct ChannalOpticalMessage ChannalMessage[32];//通道光学参数
	struct ScanPowerCh ScanPowerChannal[32];        //通道自动扫描参数
	BOOL m_bPDLStop;                                // PDL测试终止
    COpComm m_opCom;
	COpComm m_opTesterCom;
	//COpComm m_opLightCom;
	BOOL SETX(int x);
	BOOL SETY(int y);
	BOOL SendMsgToSwitch(int nTime,int nCount);
	BOOL CreateDir();
	BOOL FindFileExist(LPCTSTR);
	BOOL SaveCSVFileHeader(HANDLE hCSVFile,LONG* lFileSize=NULL);
	BOOL SaveCSVFileScanDate(DWORD dwPhaseCount);
	void SaveDataToExcel(int nType,int nChannel = 16); //存储单一通道进光的时的测试数据
//	void SaveDataToExcel1(int nType,int nChannel = 16); //存储第二通道进光的时的测试数据

/*	BOOL SaveCSVFileOpticalDate(HANDLE hCSVFile,LONG* lFileSize);*/

	VOID YieldToPeers();

	double m_dblPDLData[10000];
	int m_iReadPMCount;
	int m_iCountPDL;
	int m_nChannel;
	double m_dblSwitchTime;
	
	double TestIL(double Pref,double Pout);
	BOOL m_bHasRefForTest;  //是否归零
	BOOL m_bZeroWDL;        //做WDL归零
	BOOL m_bIfSaveToServe;  //是否保存到网络
	
	BOOL			m_bAppendMessage, m_bRefDone;
    double          m_pdblWavelength; //激光器波长
    
	BOOL m_bAutoScan;  //电压扫描粗扫
	BOOL m_bAdjustScan; //调节扫描
	DWORD m_dwSingle[16];                           //标记单个通道是否扫描

	stTestingConfigure     m_stTestingConfigure;

	//EXCEL相关操作
	_Application m_app;
	Workbooks    m_books;
	_Workbook    m_book;
	Worksheets   m_sheets;
	_Worksheet   m_sheet;
	Range        m_range;
	LPDISPATCH   m_lpDisp;
	COleVariant  m_covOptional;

	VOID LogInfo(LPCTSTR tszlogMsg,BOOL bAppend=FALSE,DWORD dwColor=COLOR_BLACK);
	CCommand m_Command;
	BOOL m_bOpenPort; //是否打开了串口
	CMy126S_45V_Switch_AppDlg(CWnd* pParent = NULL);	// standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CMy126S_45V_Switch_AppDlg)
	enum { IDD = IDD_MY126S_45V_SWITCH_APP_DIALOG };
	//CComboBox	m_ctrlLightCH;
	CEdit	m_editIL;
	CProgressCtrl	m_ctrlProgress;
	///CComboBox	m_cbxPN;
	CComboBox	m_cbComPort;
	CComboBox	m_cbSaveData;
	CComboBox	m_cbMessage;
	CComboBox	m_cbChannel;
	CComboBox   m_cbSetWL;
	CComboBox   m_cbSetPDBox;
	CColorListBox	m_ctrlListMsg;
	CString	m_strID;
	CString	m_strSN;
	BOOL m_bIsSaveServer;
	CString		m_strSaveServerPath;
	double  m_dblTLSPower;      
	CString	m_strChannel;	
	int		m_nCount;
	int		m_nPeriod;
	CNTGraph1	m_NTGraph;
	CString	m_strMessage;
	double	m_dblCHValue;
	double	m_dblCHValue2;
	int		m_nTesterPort;
	int		m_nLightCom;
	double	m_dblTLSSetWL;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy126S_45V_Switch_AppDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL
		
		// Implementation
	protected:
		BOOL m_bTestStop;      //停止测试
		HICON m_hIcon;
		
		// Generated message map functions
		//{{AFX_MSG(CMy126S_45V_Switch_AppDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		afx_msg void OnButtonStopTest();
		afx_msg void OnButtonIlctTest();
		afx_msg void OnButtonIlRef();
		afx_msg void OnButtonExit();
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnButtonClear();
		afx_msg void OnButtonRepeatTest();
		afx_msg void OnButtonSendMessage();
		afx_msg void OnSelchangeComboSendMessage();
		afx_msg void OnButtonRegulateScan();
		afx_msg void OnButtonRlTest();
		afx_msg void OnSelchangeComboPn();
		afx_msg void OnCloseupComboPn();
		afx_msg void OnButtonRepeatTest2();
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnButtonDarkTest();
		afx_msg void OnButtonSetVoltage();
		afx_msg void OnButtonOpenTesterPort();
		afx_msg void OnButtonSwitchAll();
		afx_msg void OnButtonSetTesterVoltage();
		afx_msg void OnButtonUp();
	afx_msg void OnButtonTest();
	afx_msg void OnButtonCheck();
	afx_msg void OnButtonRlRef();
	afx_msg void OnButtonScanCh();
	afx_msg void OnButtonSetWl();
	afx_msg void OnBUTTONDOWNLoad();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
	protected: //variable

		CString m_strLogFileName;
		
		st126sTestInfo m_st126sTestInfo;
	private:
		int         m_nCommandType;  //COMMAND
		CString     m_strDirectory;
		long        m_lBaudRate;
/*		CString     m_strConfigFile; //配置文件*/
		BOOL        ReadTestInfo();
		enum OpticalPara{IL,PDL,WDL,LOWIL,HIGHIL,REIL,CT};
//		enum ScanPara{MAXPOWER};
				
public:
	afx_msg void OnKillfocusEditSn();
	bool GetWorkStationConfig();
	bool GetPNFromAMTS();
//	afx_msg void OnEnChangeEditSn();
	bool GetConfigByPN();
	afx_msg void OnCbnSelchangeComboComport();
	bool GetRefConfig();
	bool GetReCHConfig(CString strStruct);
	bool ScanReCH(int nCH);
	afx_msg void OnBnClickedButtonIlctTest2();
	bool GetReVolatgeFromFile(int CHIndex, int* pnVolatgeX, int* pnVolatgeY);
	bool OpenTLSDevice();
	bool ConnectSocket();
	bool SetTLSWL(double dblWL);
	void CloseSrcBankSocket();
	afx_msg void OnEnChangeEditSn();
	bool GetSNListFromFile();
	CComboBox m_cbxSNList;
	afx_msg void OnBnClickedButtonSwCal();
	bool GetVoltageFile();
	bool GetVoltageFileforCal();
	BOOL CreateFullPath(CString strPath);
	bool GetCHCenter(int nCHIndex);
	bool ScanPointForOplinkPM(int nPort,int nSwitchIndex);	
	bool ScanZoneEXSigleCH(int nPort,int nSwitchIndex);
	bool StartMonitorPD(int nPort, int nScanPoint);
	bool GetMaxIndex(int* pnIndex, int nPort);
	bool DataDownLoadZ4671(CString strBinFileName);
	bool StartFWUpdate();
	BOOL SetTargetPower(double dbPower);
	bool FWTranSportFW(unsigned char* byTransData, int nDataLength, int nIndex, int nSum);
	bool FWUpdateEnd();
	bool CreateBinFileZ4671();
	bool OnButtonPdDownload();
	double GetTemperature();
	unsigned long SwapDWORD(unsigned long dwInData);
	unsigned int SwapWORD(unsigned int wInData);
	afx_msg void OnCbnSelchangeComboSnIndex();
	afx_msg void OnBnClickedButtonSetVoltage2();
	afx_msg void OnBnClickedButtonIlctTest4();
	afx_msg void OnBnClickedButtonIlctTest5();
	afx_msg void OnBnClickedButtonIlctTest6();

	afx_msg void OnCbnSelchangeComboSetWl();
	afx_msg void OnCbnSelchangeComboSetPdbox();
	afx_msg void OnCbnSelchangeComboChSet();
	afx_msg void OnBnClickedButtonSetVoa();
	afx_msg void OnBnClickedButtonSetVoltage3();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_126S_45V_SWITCH_APPDLG_H__5D7E34BA_30D3_427F_88DB_A8E9D4A69CB4__INCLUDED_)
