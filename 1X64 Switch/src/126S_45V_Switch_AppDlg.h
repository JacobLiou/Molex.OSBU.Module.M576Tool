// 126S_45V_Switch_AppDlg.h : header file
//
//{{AFX_INCLUDES()
#include "126sDataType.h"
#include "DataDefine.h"
#include "Command.h"
#include "excel9.h"
#include "R152Set.h"
#include "op816x.h"
#include "ntgraph1.h"
#include "ColorListBox.h"
#include "OpCRC32.h"
#include "XMLControl.h"
#include "SrcBankSocket.h"
#include "cFusionClass.h"
//}}AFX_INCLUDES


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
#define SET_RESET               0
#define DOWNLOAD_FW             1
#define OPLINK_VERSION          2
#define SETX_COMMAND            3
#define SETY_COMMAND            9
#define GET_TEMP                4
#define SWITCH_TIME                11
#define SWITCH_RANDOM           12
#define CHANNEL_SHOW           13

#define MAX_COUNT               2048
#define MAX_DAC                 4095
#define MAX_VOLTAGE             60000

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


#define I2CSENDSIZE	                32

#define XMODEM_BLOCK_HEAD_SIZE      1
#define XMODEM_BLOCK_NO_SIZE        2
#define XMODEM_BLOCK_CHK_SIZE       1 

#define	XMODEM_COMMUNICATION_FAIL	0
#define	XMODEM_DOWNLOAD_FAIL		1
#define	XMODEM_DOWNLOAD_SUCCESS		2

#define MAXCMDNMB 13
#define MAXCMDBT 20
//===========================

typedef  struct  tagLimitPara  //获取参数无纸化的限制
{
	double  dblMaxVoltage;
	double  dblMinVoltage;
	double  dblMaxRoomIL;  
	double  dblMinRoomIL;
	double  dblMaxHighIL;  
	double  dblMinHighIL;
	double  dblMaxLowIL;  
	double  dblMinLowIL;
	double  dblMaxCT;
	double  dblMinCT; 
	double  dblMaxRepeatIL;
	double  dblMinRepeatIL;
	double  dblMaxDarkIL;
	double  dblMinDarkIL;
	double  dblMaxRL;
	double  dblMinRL;
	double  dblMaxSwitchTime;
	double  dblMinSwitchTime;
	double  dblMaxTDL;
	double  dblMinTDL;
}stLimitPara,*pstLimitPara;

typedef struct tagWorkStation
{
	BOOL bTLSEnable;
	int  nTLSPort;
	int  nProductPort;
	CString strGoldenSampleName;

	//TLS config
	int nID;
	char pchServer[256];
	WORD wPort;
	char pWLPath[1024];
}stWorkStation;

enum{VOL_DATA,SWITCH_TIME_DATA,IL_ROOM_DATA,IL_LOW_DATA,IL_HIGH_DATA,WDL_DATA,TDL_DATA,PDL_DATA,RE_IL_DATA,RL_DATA,
    CT_DATA,ADJUST_DATA,ISO_DATA,DARK_DATA,DIRECTION_DATA,SELECT_CHANNEL_DATA,
    WDL_DATA_5,WDL_DATA_8,ATTEN_REPEAT,VOL_DATA_5,VOL_DATA_8,REPEAT_ATTEN_5,REPEAT_ATTEN_8,
	DIRECTIVITY_DATA,TESTDATA,FQC_DATA
};

enum{PM_8163A,PM_1830C,PM_OPLINK,PM_N7744,PM_PDARRAY};

enum{TEMPT_ROOM,TEMPT_LOW,TEMPT_HIGH};

class CMy126S_45V_Switch_AppDlg : public CDialog
{
//Construction
public: 
	int i_chansw3;
	int i_chansw2;
	BOOL m_bisok;
	BOOL m_blogin;
	BOOL m_biscreatbin;
	BOOL AutoPointScanRIght(int nChannel);
	BOOL m_bischange;
	BOOL m_bisinit;
	BOOL m_bisdownimg;
	CString m_lutRoomTep;
	CString m_lutLowTep;
	CString m_lutHighTep;
	CString m_strSN;
	cFusionClass m_fusion;
	tagXMLItem   m_XMLItem;
	char strStationID[MAX_PATH];

	
	CString m_strWO;
	CString m_strWorkStatus;
	CString m_strWorkprss;

	//mims msg show 
	int		m_nMsgCount;
	CString strMIMSMsg;
	CString strUserMsg;
	CString strLoginMode;
	CString strSoftID;
	CString strPNInfo;
	CString strSNInfo;
	CString strMESMode;
	CString strSecondCalMsg;
	CString strSecondCalPSW;
	CString strWorkID;//工位ID
	CString strWorkName;//工位名
	CString strWOInfo;
	CString strSPECInfo;
	CString strCurProcessInfo;
	CString strTestProcessInfo;
	CString strHoldStaInfo;
	CString strReworkStaInfo;
	void bIniXMLItem();
	BOOL SWMsgCheckShow();
	void ConvertGBKToUtf8(CString &strGBK);
	void OnBnClickedButtonIn();

	//CCommandForFUJ m_CmdFUJ;
	BOOL TxSTRToI2C(CString strAddress, CString StrSetValue);
	BOOL TxBYTEToI2C(CString strAddress,BYTE byData,BYTE byData1, BYTE byData2, BYTE byData3, BYTE byData4, BYTE byData5, BYTE byData6, BYTE byData7, BYTE byData8, BYTE byData9, BYTE byData10, BYTE byData11, BYTE byData12, BYTE byData13, BYTE byData14, BYTE byData15, BYTE byData16, BYTE byData17, BYTE byData18, BYTE byData19, BYTE byData20, BYTE byData21, BYTE byData22, BYTE byData23, BYTE byData24, BYTE byData25, BYTE byData26, BYTE byData27, BYTE byData28, BYTE byData29, BYTE byData30, BYTE byData31,  DWORD dwSetLength);
	void AsiicToHex(BYTE *str,BYTE *hex,int cnt,int *nGetCount);
	BOOL RxDataFromI2C(CString wAddress);
	CString m_strErrorMsg;
	BOOL TxDataToI2C(CString strAddress, BYTE *pbySetValue, DWORD dwSetLength);

	BOOL   SendProductScanZone(int nSWIndex, int nMinX, int nMaxX, int nMinY, int nMaxY, int nStep);
	double GetModuleTemp();
	int	   GetScanState(BYTE btSWIndex);
	BOOL   SetTesterXYDAC(int nSWIndex,int x, int y);
	BOOL   SetSWChannel(int nSWIndex,int nCHIndex);
	BOOL   GetMemoryData(DWORD dwAddress,char *pchData);
	int	   GetASWTHState();
	BOOL   CreateMfgInfoBin();
	int	   GetModuleGLUTALM();
	CString GetModuleInfo();
	CString GetFWVer();
	BOOL   SetRSET();

	double CalTDL(double dblRoom,double dblLow,double dblHigh);
	CString m_strSelectRule;
	BOOL GetTempConfig();
	BOOL GetPortInfo(CString strPN,CString strSpec,CString strTestItem);
	BOOL GetVOAData(int nPort);
	BOOL GetCT(double *pdblValue, int nCount);
	double m_dblTemperature[3];  //
	BOOL GetAttenPointFromATMS();
	BOOL GetAttenPoint(CString strTempt);
	int m_n5dBAttenX[32];
	int m_n5dBAttenY[32];
	int m_n8dBAttenY[32];
	int m_n8dBAttenX[32];

	double m_pdb5dBWDL[32];
	double m_pdb8dBWDL[32];

	double m_pdb5dBRepeatAtten[32];
	double m_pdb8dBRepeatAtten[32];

	COpComm m_opComITLA;
	BOOL SetITLAWL(double dblWL);
	BOOL Find8dBAtten(int nPort,CString strTempt);
//	int  m_nWLCount
//	int m_dblWL
	BOOL GetVoltageDataFromATMS(char *chSN,int nChannelNum,int *pnX,int *pnY);
	BOOL GetVolDataFromXml(int nPort);
	void DeleteOne(int* nNum,int nDeletNum,int nCount);
	void ReShapeLine(int *nNum,int nCount);
	BOOL ChoseLine(int *nChNum, int nTotalCount, int nGetCount, int *nGetNum);
	BOOL m_bRLPass[32];
	BOOL m_bRepeatILPass[32];
	BOOL m_bDarkILPass[32];
	BOOL m_bHighILPass[32];
	BOOL m_bLowILPass[32];
	BOOL m_bRoomILPass[32];
	BOOL SelectChannel(int nTargetCh,int nTotalCh,int *nChNume);
	int m_bChange;
	int m_bShort;
	BOOL b_issn1;
   	BOOL b_issn2;
	BOOL b_issn3;
	BOOL b_issn4;
	BOOL SetTLSWavelength(double dblWL);
	BOOL JudgeFileWhetherExist(CString strPath);
	BOOL GetDataFromCSV(int nType, int nChannel, double *pdblData);
	BOOL GetDataFromFile(CString, int nRow, int nLine,double*pdblData);
	CString m_strSpec;
	int    m_nWLCount;
	double m_dblWL[48]; 
	BOOL m_bGetSNInfo; //判断获取SN信息是否成功
	BOOL m_bEndRef;  //探头是否拔出标记
	BOOL GetFinalData();
	double m_dblTargetTempt;
	double m_dblCurTempt;
	int    m_nSleepTime;
	double GetCurrentTempt();
	BOOL SetTemptStatus(int nStatus);
	BOOL SetTempToPoint(int SetTemp);
	BOOL m_bGetPaperlessTemp;
	BOOL SaveDataToCSV(int nType, int nChannel=32,CString strTemperature = "Room");
	BOOL  m_bRefRL;
	double m_dblSystemRL;
	BOOL  m_bConnect1830C;
	BOOL  m_bConnectTemptControl;
	//CString m_strNetConfigPath;
	CString m_strPNConfigPath;
	CString m_strFWVer;
	int m_nRefTime;  //超时时间限制
	COleDateTime m_RefStartTime;//获取归零开始时间;
	int  m_nPreRefNum;  //保存上一次归零通道的值
	int  m_nRefCount[12][32];
	BOOL CopyIntialTestData();
	BOOL FinalTest();
	int  m_nCHSelect[32];
	BOOL AutoScanChannel(int nChannel);
	SYSTEMTIME  m_StartTime;
	BOOL GetVoltageDataFromATMS();
	BOOL SetFileByMSMQ(CString strData, CString &strErr);
	BOOL UpdateATMData(int nTestType,int nChannel=32);

	CXMLControl  m_ctrXMLCtr; //无纸化
	stLimitPara m_stLimitPara;
	UDLSERVERLib::IEngineMgrPtr g_pEngine;
	UDLSERVERLib::IITLSInterfacePtr g_pTLS;
	BOOL m_bRoomIL;
	BOOL m_bLowIL;
	BOOL m_bHighIL;

	BOOL GetATMTestTemplate();
	BOOL GetPowerFromOPCom(int nPMIndex, BYTE bSlot, BYTE bChannel, double *pdblPower);
	BOOL SetWLFromOPCom(int iPMIndex, byte bSlot, byte bChan, long lWL);
	COpComm m_comPM[5];
	BOOL OneKeyWDLTest();
	void GetChannelInfo(void);
	BOOL OneKeyScan();
	int m_iTimerCount;
	BOOL m_bTestIL;
	int  m_nSwitchComPortCount;
	COpCRC32  m_CRC32;
	BOOL WriteBinFile(CString strBinName, CString strSave, DWORD dwLengthWrite, PBYTE pByteBinData);
	BOOL CreateSwitchPointBin(CString strSave, int nFile);
	BOOL CreateSwitchMapBin(CString strSave, int nFile);
	BOOL CreateModuleMapBin(CString strSave, int nFile);
	BOOL CheckTempLutFile(CString strSave);
	BOOL TDLTest();
	BOOL WDLTest();
	BOOL ILCTTest();
	BOOL CreateBinFileAndDownload();
	BOOL CreateLutFile();
	BOOL CalAutoScanTest();
	BOOL  m_bILPass[32];
	BOOL  m_bCTPass[32];
	BOOL  m_bTDLPass[32];
	BOOL  m_bWDLPass[32];
	BOOL  m_bPDLPass[32];
	void SendCommand(CString strCommand);
	WORD XmodemCRC16(char *pbStream, WORD wLength);
	BOOL XmodemloadData(char *pbBinData, WORD wDownloadSize, BOOL bFileDone);
	BOOL SendRevCommand(CString command, COpComm *pcommandtask, CString setvalue="");
	BOOL DataDownLoad(CString strFileName);
	VOID DownLoadFW();
	//double GetModuleTemp();
	CString m_strItemName;
	double m_dblTDLIL[16][200];
	double m_dblTDLTemp[200];
	int  m_nTDLCount;
	BOOL m_bTDLStop;
	BOOL CheckParaPassOrFail(int nParaType,double dblValue);
	CString	m_strPN;
	CString m_strGetPN;  //从无纸化获取的实际的PN
	double m_dblWDLRef[20][100];
	double m_dblWDL[20][100];
	BOOL SetPWMWL(double dblWL,int nChannel=32);
	BOOL ConnectIP(int nPMIndex);
	BOOL ConnectPM();
	DWORD  m_nPMAddress[16];
	int  m_nPMType[16];
	int  m_nPMConnect[16];
	int  m_nPMCount;
	int  m_nCHCount;
	BOOL GetVolDataFormFile(int InputPort);
	VOID CalCulateVol();
	BOOL GetPower(int iBox, int iChannel, double *dblPower);
	BOOL LoadPMCfg(pstPMCfg pstPMInfo);
	VOID CalculateAngle(int nVolX0,int nVolY0,int nVolX3,int nVolY3,int nVolX8,int nVolY8,int nVolX11,int nVolY11);
	CString m_strNetFile;
	BOOL FindMyDirectory(CString strSubdir0, CString strSubdir1,CString strSubdir2);
	double m_dblReferencePower[32];
	double m_dblPreRefPower[32][48];//记录上次归零结果
	int    m_Xbase[32];
	int    m_Ybase[32];
	double m_dbInitialIL[32];  //记录初测的IL值
	double m_dblZeroWDL[100];
	BOOL AutoPointScan(int nChannel);
	BOOL AutoScan(int nChannel,CString strFileINI,BOOL bScanRange=TRUE);
    CRect m_rOpenDevice;
	BOOL  m_bOpenRLport;
	void ReadZeroFile(double dblTLSPower);
	BOOL m_bStartExcel;
	BOOL CreateExcelFile();
	void CloseExcelFile();
	BOOL SetPWMWL(int nChannel,double dblWL);
	BOOL GetPowerFromIP(BYTE bBox, BYTE bSlot, BYTE bChannel, double *pdblPower);
	BOOL SetWLFromIP(int socket_fd, BYTE bSlot, BYTE bChannel, LONG lWL);
	double ReadPWMPower(int nChannel);
	BOOL bCoverFile(int nPara); //是否覆盖光学参数；
//	BOOL bCoverFile(ScanPara nPara);    //是否覆盖扫描参数；    
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
	COp816X	m_HP816X1;
	COp816X	m_HP816X2;
	COp816X m_PM1830C[16];
	COp816X m_1830CRL;
	COp816X m_PMN7744[16];
	//	CMy1830C m_PM1830C[4];
	CR152Set R152set;

	double dblDir[32][32];
	struct ChannalOpticalMessage ChannalMessage[32];//通道光学参数
	struct ScanPowerCh ScanPowerChannel[32];        //通道自动扫描参数
	double db_sacnchIL[48];
	BOOL m_bPDLStop;                                // PDL测试终止
    COpComm m_opCom;
	COpComm m_opTempt;  //控制冷热盘的串口
	BOOL SETX(int x);
	BOOL SETY(int y);
	BOOL SendMsgToSwitch(int nTime,int nCount);
	BOOL CreateDir();
	BOOL FindFileExist(LPCTSTR);
	BOOL SaveCSVFileHeader(HANDLE hCSVFile,LONG* lFileSize=NULL);
	BOOL SaveCSVFileScanDate(DWORD dwPhaseCount);
	void SaveDataToExcel(int nType,int nChannel = 32); //存储单一通道进光的时的测试数据
//	void SaveDataToExcel1(int nType,int nChannel = 16); //存储第二通道进光的时的测试数据

/*	BOOL SaveCSVFileOpticalDate(HANDLE hCSVFile,LONG* lFileSize);*/

	VOID YieldToPeers();

	bool OpenTLSDevice();
	bool ConnectSocket();
	bool SetTLSWL(double dblWL);
	void CloseSrcBankSocket();
	bool GetWorkStationConfig();

	CSrcBankSocket *m_pSrcBankSocket;
	stWorkStation m_stWorkStation;

	TCHAR m_chComputerName[256];

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
	 double          m_pdblTestWavelength[3]; //激光器波长
    
	BOOL m_bAutoScan;  //电压扫描粗扫
	BOOL m_bAdjustScan; //调节扫描
	DWORD m_dwSingle[2][32];                           //标记单个通道是否扫描

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
	CComboBox	m_cbChannel2;
	CComboBox	m_cbTestCH;
	CComboBox	m_cbSetWL;
	CProgressCtrl	m_ctrlProgress;
	CComboBox	m_cbxPN;
	CComboBox	m_cbTestItem;
	CComboBox	m_cbComPort;
	CComboBox	m_cbSaveData;
	CComboBox	m_cbMessage;
	CComboBox	m_cbSetCommand;
	CComboBox	m_cbChannel;
	CColorListBox	m_ctrlListMsg;
	CComboBox	m_ctrlComboxTemperature;
	CString	m_strID;
	CString	m_strCom;
	BOOL m_bIsSaveServer;
	CString		m_strSaveServerPath;
	double  m_dblTLSPower;      
	CString	m_strChannel;	
	int		m_nCount;
	int		m_nPeriod;
	CNTGraph1	m_NTGraph;
	CString	m_strMessage;
	double	m_dblX;
	double	m_dblY;
	double	m_dblCHValue2;
	CString	m_strSN2;
	int		m_nPort;
	int		m_nSwicthChannel;
	int		m_nSwitchCh;
	int		m_nSwitchTimeCH;
	CString	m_strSN3;
	CString	m_strSN4;
	CString	m_strPortNumber;
	int		m_pmcha;
	CString	m_strSN1;
	int		m_startchange1;
	int		m_endchange1;
	int		m_startchange2;
	int		m_endchange2;
	CString	m_password;
	CString	m_strSN5;
	CString	m_ModuleSN;
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
		afx_msg void OnButtonIlRef();
		afx_msg void OnButtonOpenPort();
		afx_msg void OnButtonIlctTest();
		afx_msg void OnButtonScan();
		afx_msg void OnButtonTDLTest();
		afx_msg void OnButtonSetChannel();
		afx_msg void OnButtonPdlTest();
		afx_msg void OnButtonWdlTest();
		afx_msg void OnButtonRepeat();
		afx_msg void OnButtonExit();
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnButtonOpenDevice();
		afx_msg void OnButtonSetPM();
		afx_msg void OnButtonClear();
		afx_msg void OnButtonAutoTest();
		afx_msg void OnButtonReadpower();
		afx_msg void OnButtonRepeatTest();
		afx_msg void OnButtonSendMessage();
		afx_msg void OnSelchangeComboSendMessage();
		afx_msg void OnUpdateEditMessage();
		afx_msg void OnButtonTest();
		afx_msg void OnButtonSaveFile();
		afx_msg void OnButtonRegulateScan();
		afx_msg void OnButtonSingleScan();
		afx_msg void OnButtonSingleChannelScan();
		afx_msg void OnButtonRegulateScan2();
		afx_msg void OnButtonConnect1830();
	afx_msg void OnButtonZero();
	afx_msg void OnButtonSetChannelVol();
	afx_msg void OnButtonRlTest();
	afx_msg void OnButtonSwitchTime();
	afx_msg void OnChangeEditSn();
	afx_msg void OnButtonWdlRef();
	afx_msg void OnSelchangeComboPn();
	afx_msg void OnCloseupComboPn();
	afx_msg void OnSelchangeComboTestItem();
	afx_msg void OnButtonAutotest();
	afx_msg void OnButtonCreateLut();
	afx_msg void OnButtonDownloadBin();
	afx_msg void OnCloseupComboTestItem();
	afx_msg void OnButtonIsoTest();
	afx_msg void OnButtonSwitchTimeTest();
	afx_msg void OnButtonReIlTest();
	afx_msg void OnButtonRef();
	afx_msg void OnButtonOneKeyTest();
	afx_msg void OnButtonDarkTest();
	afx_msg void OnButtonCheckData();
	afx_msg void OnButtonOnekeyTest();
	afx_msg void OnButtonRlRef();
	afx_msg void OnButtonReadTempt();
	afx_msg void OnKillfocusEditSn();
	afx_msg void OnButtonUploadAtms();
	afx_msg void OnButtonWdl();
	afx_msg void OnSelchangeComboSetWl();
	afx_msg void OnCloseupComboSetWl();
	afx_msg void OnButtonDirection();
	afx_msg void OnButtonSelectChannel();
	afx_msg void OnButtonOpenFile();
	afx_msg void OnButtonWdl2();
	afx_msg void OnButtonWdlRepeat();
	afx_msg void OnButtonCreateWdl();
	afx_msg void OnButtonDirectivityTest();
	afx_msg void OnChangeEditSn3();
	afx_msg void OnChangeEditSn4();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnCancelMode();
	afx_msg void OnButton5();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnButtonOnekeyTest2();
	afx_msg void OnButton9();
	afx_msg void OnButton11();
	afx_msg void OnButton12();
	afx_msg void OnButton13();
	afx_msg void OnButton14();
	afx_msg void OnButton15();
	afx_msg void OnSelchangeComboTemperature();
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
				
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_126S_45V_SWITCH_APPDLG_H__5D7E34BA_30D3_427F_88DB_A8E9D4A69CB4__INCLUDED_)
