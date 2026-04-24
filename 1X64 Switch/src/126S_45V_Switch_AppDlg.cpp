// 126S_45V_Switch_AppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "126S_45V_Switch_App.h"
#include "126S_45V_Switch_AppDlg.h"
#include <math.h>
#include "COMUTIL.H"
#include "Command.h"
#include "R152Set.h"
#include "opnet.h"
#include "opslot.h"
#include "SelectCH.h"
#include "TestRL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


stPMCfg g_stPMCfg;
TCHAR g_tszAppFolder[MAX_PATH];
stMemsSwCoef g_stMemsSwCoef;
CString m_strNetConfigPath;

const st64PortMap c_st64PortMap  =
{
	{0}, //BUNDLEHEADER
    {
    {0,0,0,0}, // ch0: all switches in block state
    {1,0,0,0}, // ch1: SW1->ch1, other switches in block state
    {2,0,0,0}, // ch2: SW1->ch2, ~
    {3,0,0,0}, // ch3: SW1->ch3, ~
    {4,0,0,0}, // ch4: SW1->ch4, ~
    {5,0,0,0}, // ch5: SW1->ch5, ~
    {6,0,0,0}, // ch6: SW1->ch6, ~
    {7,0,0,0}, // ch7: SW1->ch7, ~
    {11,0,0,0}, // ch8: SW1->ch11, ~
    {12,0,0,0}, // ch9: SW1->ch12, ~
    {13,0,0,0}, // ch10: SW1->ch13, ~
    {14,0,0,0}, // ch11: SW1->ch14, ~
    {15,0,0,0}, // ch12: SW1->ch15, ~
    {16,0,0,0}, // ch13: SW1->ch16, ~
    {17,0,0,0}, // ch14: SW1->ch17, ~
    {8,1,0,0}, // ch15: SW1->ch8, SW2->ch1, other switches in block state
    {8,2,0,0}, // ch16: SW1->ch8, SW2->ch2, ~
    {8,3,0,0}, // ch17: SW1->ch8, SW2->ch3, ~
    {8,4,0,0}, // ch18: SW1->ch8, SW2->ch4, ~
    {8,5,0,0}, // ch19: SW1->ch8, SW2->ch5, ~
    {8,6,0,0}, // ch20: SW1->ch8, SW2->ch6, ~
    {8,7,0,0}, // ch21: SW1->ch8, SW2->ch7, ~
    {8,8,0,0}, // ch22: SW1->ch8, SW2->ch8, ~
    {8,9,0,0}, // ch23: SW1->ch8, SW2->ch9, ~
    {8,10,0,0}, // ch24: SW1->ch8, SW2->ch10, ~
    {8,11,0,0}, // ch25: SW1->ch8, SW2->ch11, ~
    {8,12,0,0}, // ch26: SW1->ch8, SW2->ch12, ~
    {8,13,0,0}, // ch27: SW1->ch8, SW2->ch13, ~
    {8,14,0,0}, // ch28: SW1->ch8, SW2->ch14, ~
    {8,15,0,0}, // ch29: SW1->ch8, SW2->ch15, ~
    {8,16,0,0}, // ch30: SW1->ch8, SW2->ch16, ~
    {8,17,0,0}, // ch31: SW1->ch8, SW2->ch17, ~
    {9,0,1,0}, // ch32: SW1->ch9, SW3->ch1, other switches in block state
    {9,0,2,0}, // ch33: SW1->ch9, SW3->ch2, ~
    {9,0,3,0}, // ch34: SW1->ch9, SW3->ch3, ~
    {9,0,4,0}, // ch35: SW1->ch9, SW3->ch4, ~
    {9,0,5,0}, // ch36: SW1->ch9, SW3->ch5, ~
    {9,0,6,0}, // ch37: SW1->ch9, SW3->ch6, ~
    {9,0,7,0}, // ch38: SW1->ch9, SW3->ch7, ~
    {9,0,8,0}, // ch39: SW1->ch9, SW3->ch8, ~
    {9,0,9,0}, // ch40: SW1->ch9, SW3->ch9, ~
    {9,0,10,0}, // ch41: SW1->ch9, SW3->ch10, ~
    {9,0,11,0}, // ch42: SW1->ch9, SW3->ch11, ~
    {9,0,12,0}, // ch43: SW1->ch9, SW3->ch12, ~
    {9,0,13,0}, // ch44: SW1->ch9, SW3->ch13, ~
    {9,0,14,0}, // ch45: SW1->ch9, SW3->ch14, ~
    {9,0,15,0}, // ch46: SW1->ch9, SW3->ch15, ~
    {9,0,16,0}, // ch47: SW1->ch9, SW3->ch16, ~
    {9,0,17,0}, // ch48: SW1->ch9, SW3->ch17, ~
    {10,0,0,2}, // ch49: SW1->ch10, SW4->ch2, other switches in block state
    {10,0,0,3}, // ch50: SW1->ch10, SW4->ch3, ~
    {10,0,0,4}, // ch51: SW1->ch10, SW4->ch4, ~
    {10,0,0,5}, // ch52: SW1->ch10, SW4->ch5, ~
    {10,0,0,6}, // ch53: SW1->ch10, SW4->ch6, ~
    {10,0,0,7}, // ch54: SW1->ch10, SW4->ch7, ~
    {10,0,0,8}, // ch55: SW1->ch10, SW4->ch8, ~
    {10,0,0,9}, // ch56: SW1->ch10, SW4->ch9, ~
    {10,0,0,10}, // ch57: SW1->ch10, SW4->ch10, ~
    {10,0,0,11}, // ch58: SW1->ch10, SW4->ch11, ~
    {10,0,0,12}, // ch59: SW1->ch10, SW4->ch12, ~
    {10,0,0,13}, // ch60: SW1->ch10, SW4->ch13, ~
    {10,0,0,14}, // ch61: SW1->ch10, SW4->ch14, ~
    {10,0,0,15}, // ch62: SW1->ch10, SW4->ch15, ~
    {10,0,0,16}, // ch63: SW1->ch10, SW4->ch16, ~
    {10,0,0,17} // ch64: SW1->ch10, SW4->ch17, ~
    },
    {0}, // Reserved
    0 // CRC32
};
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for a pp About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy126S_45V_Switch_AppDlg dialog

CMy126S_45V_Switch_AppDlg::CMy126S_45V_Switch_AppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy126S_45V_Switch_AppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMy126S_45V_Switch_AppDlg)
	m_strID = _T("");
	m_strCom = _T("");
	m_strChannel = _T("");
	m_strMessage = _T("");
	m_dblX = 0.0;
	m_dblY = 0.0;
	m_dblCHValue2 = 0.0;
	m_strSN2 = _T("");
	m_nPort = 1;
	m_nSwicthChannel = 0;
	m_nSwitchCh = 0;
	m_nSwitchTimeCH = 12;
	m_strSN3 = _T("");
	m_strSN4 = _T("");
	m_strPortNumber = _T("");
	m_pmcha = 1;
	m_strSN1 = _T("");
	m_pdblTestWavelength[3] = { 1310,1550,1625 };
	m_startchange1 = 0;
	m_endchange1 = 0;
	m_startchange2 = 0;
	m_endchange2 = 0;
	m_password = _T("");
	m_strSN5 = _T("");
	m_strPN = "8X1";
	m_strItemName = "初测";
	m_nCHCount = 12;
	m_bShort=-1;
	m_ModuleSN = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bTestStop = FALSE;
	m_bOpenPort = FALSE;
	m_bPDLStop = FALSE;
	m_bHasRefForTest = FALSE;
    m_bOpenPM = FALSE;
	m_bOpen8164 = FALSE;
	m_bChangeSwitch = FALSE;
	m_bStartExcel = FALSE;
	m_bZeroWDL = FALSE;
	m_bAutoScan = FALSE; //
	m_bOpenRLport = FALSE;
	m_bTDLStop = FALSE;
	m_bAdjustScan = FALSE;
	m_bTestIL = FALSE;

	m_bRoomIL = FALSE;
	m_bLowIL = FALSE;
	m_bHighIL = FALSE;
	m_bRefRL = FALSE;
	m_bConnect1830C = FALSE;

	m_bGetPaperlessTemp = FALSE;
	m_bConnectTemptControl = FALSE;
	m_bEndRef = TRUE;
	m_bGetSNInfo = FALSE;
	//初始化
	m_dblTLSPower = 0; //激光器功率初始值
    m_pdblWavelength = m_pdblTestWavelength[1]; //激光器波长初始值
	m_iReadPMCount = 0;

	m_nPreRefNum = 99;

	m_strFWVer = "";

	for (int i=0; i<16; i++)
	{
		m_bILPass[i] = TRUE;
		m_bCTPass[i] = TRUE;
		m_bTDLPass[i] = TRUE;
		m_bWDLPass[i] = TRUE;
		m_bPDLPass[i] = TRUE;
	}
	ZeroMemory(m_dblZeroWDL,sizeof(double)*100);
	ZeroMemory(m_dwSingle,sizeof(m_dwSingle));

	//数组初始化
	for (int i=0; i<16; i++)
	{
		  m_Xbase[i] = 0;
		  m_Ybase[i] = 0;
		  ScanPowerChannel[i].MaxPower = -100;
		  m_dblReferencePower[i] = 100;
	 }

/*    m_pReferencePower->dblReferencePower=0;//初始化归零光功率*/
}

void CMy126S_45V_Switch_AppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy126S_45V_Switch_AppDlg)
	DDX_Control(pDX, IDC_COMBO_CH_SET2, m_cbChannel2);
	DDX_Control(pDX, IDC_COMBO_TEST_CH, m_cbTestCH);
	DDX_Control(pDX, IDC_COMBO_SET_WL, m_cbSetWL);
	DDX_Control(pDX, IDC_PROGRESS2, m_ctrlProgress);
	DDX_Control(pDX, IDC_COMBO_PN, m_cbxPN);
	DDX_Control(pDX, IDC_COMBO_TEST_ITEM, m_cbTestItem);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_cbComPort);
	DDX_Control(pDX, IDC_COMBO_SaveData, m_cbSaveData);
	DDX_Control(pDX, IDC_COMBO_SEND_MESSAGE, m_cbMessage);
	DDX_Control(pDX, IDC_COMBO_CH_SET, m_cbChannel);
	DDX_Control(pDX, IDC_LIST_INFO, m_ctrlListMsg);
	DDX_Control(pDX, IDC_COMBO_TEMPERATURE, m_ctrlComboxTemperature);
	DDX_Text(pDX, IDC_EDIT_ID, m_strID);
	DDX_CBString(pDX, IDC_COMBO_CH_SET, m_strChannel);
	DDX_Control(pDX, IDC_NTGRAPHCTRL1, m_NTGraph);
	DDX_Text(pDX, IDC_EDIT_MESSAGE, m_strMessage);
	DDX_Text(pDX, IDC_EDIT_X, m_dblX);
	DDX_Text(pDX, IDC_EDIT_Y, m_dblY);
	DDX_Text(pDX, IDC_EDIT_CHANNEL2, m_dblCHValue2);
	DDX_Text(pDX, IDC_EDIT_SN2, m_strSN2);
	DDX_Text(pDX, IDC_EDIT_COM_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_SWITCH_CHANNEL, m_nSwicthChannel);
	DDX_Text(pDX, IDC_EDIT_CHANNEL, m_nSwitchCh);
	DDX_Text(pDX, IDC_EDIT_SWITCH_TIME_CH, m_nSwitchTimeCH);
	DDX_Text(pDX, IDC_EDIT_SN3, m_strSN3);
	DDX_Text(pDX, IDC_EDIT_SN4, m_strSN4);
	DDX_Text(pDX, IDC_EDIT_PN, m_strPortNumber);
	DDX_Text(pDX, IDC_EDIT_CHN, m_pmcha);
	DDV_MinMaxInt(pDX, m_pmcha, 0, 20);
	DDX_Text(pDX, IDC_EDIT_SN, m_strSN1);
	DDX_Text(pDX, IDC_EDIT1, m_startchange1);
	DDV_MinMaxInt(pDX, m_startchange1, 0, 19);
	DDX_Text(pDX, IDC_EDIT2, m_endchange1);
	DDV_MinMaxInt(pDX, m_endchange1, 0, 19);
	DDX_Text(pDX, IDC_EDIT3, m_startchange2);
	DDV_MinMaxInt(pDX, m_startchange2, 0, 19);
	DDX_Text(pDX, IDC_EDIT4, m_endchange2);
	DDV_MinMaxInt(pDX, m_endchange2, 0, 19);
	DDX_Text(pDX, IDC_EDIT_ID2, m_password);
	DDX_Text(pDX, IDC_EDIT_SN5, m_strSN5);
	DDX_Text(pDX, IDC_EDIT5, m_ModuleSN);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMy126S_45V_Switch_AppDlg, CDialog)
	//{{AFX_MSG_MAP(CMy126S_45V_Switch_AppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_STOP_TEST, OnButtonStopTest)
	ON_BN_CLICKED(IDC_BUTTON_IL_REF, OnButtonIlRef)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_PORT, OnButtonOpenPort)
	ON_BN_CLICKED(IDC_BUTTON_ILCT_TEST, OnButtonIlctTest)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, OnButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_TDL_TEST, OnButtonTDLTest)
	ON_BN_CLICKED(IDC_BUTTON_SET_CHANNAL, OnButtonSetChannel)
	ON_BN_CLICKED(IDC_BUTTON_PDL_TEST, OnButtonPdlTest)
	ON_BN_CLICKED(IDC_BUTTON_WDL_TEST, OnButtonWdlTest)
	ON_BN_CLICKED(IDC_BUTTON_REPEAT, OnButtonRepeat)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DEVICE, OnButtonOpenDevice)
	ON_BN_CLICKED(IDC_BUTTON_SET_PM, OnButtonSetPM)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_TEST, OnButtonAutoTest)
	ON_BN_CLICKED(IDC_BUTTON_READPOWER, OnButtonReadpower)
	ON_BN_CLICKED(IDC_BUTTON_REPEAT_TEST, OnButtonRepeatTest)
	ON_BN_CLICKED(IDC_BUTTON_SEND_MESSAGE, OnButtonSendMessage)
	ON_CBN_SELCHANGE(IDC_COMBO_SEND_MESSAGE, OnSelchangeComboSendMessage)
	ON_EN_UPDATE(IDC_EDIT_MESSAGE, OnUpdateEditMessage)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_FILE, OnButtonSaveFile)
	ON_BN_CLICKED(IDC_BUTTON_REGULATE_SCAN, OnButtonRegulateScan)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_SCAN, OnButtonSingleScan)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_CHANNEL_SCAN, OnButtonSingleChannelScan)
	ON_BN_CLICKED(IDC_BUTTON_REGULATE_SCAN2, OnButtonRegulateScan2)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT1830, OnButtonConnect1830)
	ON_BN_CLICKED(IDC_BUTTON_ZERO, OnButtonZero)
	ON_BN_CLICKED(IDC_BUTTON_SET_CHANNEL_VOL, OnButtonSetChannelVol)
	ON_BN_CLICKED(IDC_BUTTON_RL_TEST, OnButtonRlTest)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_TIME, OnButtonSwitchTime)
	ON_EN_CHANGE(IDC_EDIT_SN, OnChangeEditSn)
	ON_BN_CLICKED(IDC_BUTTON_WDL_REF, OnButtonWdlRef)
	ON_CBN_SELCHANGE(IDC_COMBO_PN, OnSelchangeComboPn)
	ON_CBN_CLOSEUP(IDC_COMBO_PN, OnCloseupComboPn)
	ON_CBN_SELCHANGE(IDC_COMBO_TEST_ITEM, OnSelchangeComboTestItem)
	ON_BN_CLICKED(IDC_BUTTON_AUTOTEST, OnButtonAutotest)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_LUT, OnButtonCreateLut)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_BIN, OnButtonDownloadBin)
	ON_CBN_CLOSEUP(IDC_COMBO_TEST_ITEM, OnCloseupComboTestItem)
	ON_BN_CLICKED(IDC_BUTTON_ISO_TEST, OnButtonIsoTest)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_TIME_TEST, OnButtonSwitchTimeTest)
	ON_BN_CLICKED(IDC_BUTTON_RE_IL, OnButtonReIlTest)
	ON_BN_CLICKED(IDC_BUTTON_WDL_REF2, OnButtonRef)
	ON_BN_CLICKED(IDC_BUTTON_ONE_KEY_TEST, OnButtonOneKeyTest)
	ON_BN_CLICKED(IDC_BUTTON_DARK_TEST, OnButtonDarkTest)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_DATA, OnButtonCheckData)
	ON_BN_CLICKED(IDC_BUTTON_ONEKEY_TEST, OnButtonOnekeyTest)
	ON_BN_CLICKED(IDC_BUTTON_RL_REF, OnButtonRlRef)
	ON_BN_CLICKED(IDC_BUTTON_READ_TEMPT, OnButtonReadTempt)
	ON_EN_KILLFOCUS(IDC_EDIT_SN, OnKillfocusEditSn)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_ATMS, OnButtonUploadAtms)
	ON_BN_CLICKED(IDC_BUTTON_WDL, OnButtonWdl)
	ON_CBN_SELCHANGE(IDC_COMBO_SET_WL, OnSelchangeComboSetWl)
	ON_CBN_CLOSEUP(IDC_COMBO_SET_WL, OnCloseupComboSetWl)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTION, OnButtonDirection)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CHANNEL, OnButtonSelectChannel)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FILE, OnButtonOpenFile)
	ON_BN_CLICKED(IDC_BUTTON_WDL2, OnButtonWdl2)
	ON_BN_CLICKED(IDC_BUTTON_WDL_REPEAT, OnButtonWdlRepeat)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_WDL, OnButtonCreateWdl)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTIVITY_TEST, OnButtonDirectivityTest)
	ON_EN_CHANGE(IDC_EDIT_SN3, OnChangeEditSn3)
	ON_EN_CHANGE(IDC_EDIT_SN4, OnChangeEditSn4)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_BN_CLICKED(IDC_BUTTON_ONEKEY_TEST2, OnButtonOnekeyTest2)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON11, OnButton11)
	ON_BN_CLICKED(IDC_BUTTON12, OnButton12)
	ON_BN_CLICKED(IDC_BUTTON13, OnButton13)
	ON_BN_CLICKED(IDC_BUTTON14, OnButton14)
	ON_BN_CLICKED(IDC_BUTTON15, OnButton15)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_RL_TEST2, OnButtonRlTest)
	ON_CBN_SELCHANGE(IDC_COMBO_TEMPERATURE, OnSelchangeComboTemperature)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy126S_45V_Switch_AppDlg message handlers

BOOL CMy126S_45V_Switch_AppDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	AfxSocketInit(NULL);

    //设置默认信息
	m_ctrlComboxTemperature.SetCurSel(0);
	m_cbChannel.SetCurSel(0);
	m_cbChannel2.SetCurSel(0);
	m_cbSaveData.SetCurSel(0);
	m_cbComPort.SetCurSel(0);
	m_bischange = FALSE;
	m_bisinit = FALSE;
	m_bisdownimg = FALSE;
	m_blogin = FALSE;
	m_cbTestItem.SetCurSel(1);
	m_cbTestCH.SetCurSel(1);  //默认测试9~12CH
    
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//得到当前目录的路径
	GetCurrentDirectory(MAX_PATH,g_tszAppFolder);
	//换肤
	CString strname1,strname2,strname3;
	strname1.Format("%s\\skinh.she",g_tszAppFolder);
	strname2.Format("%s\\skinh\\chinesered.she",g_tszAppFolder);
	strname3.Format("%s\\chinesered.she",g_tszAppFolder);
	
	CopyFile(strname2,strname3,FALSE);
	DeleteFile(strname1);
	rename(strname3,strname1);
	SkinH_Attach();
	//////////
	GetDlgItem(IDC_STATIC_CHSELECT)->ShowWindow(FALSE);
	GetDlgItem(IDC_COMBO_TEST_CH)->ShowWindow(FALSE);
    ////////////
	GetDlgItem(IDC_STATIC_CHANNEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_CHANNEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_CHANNEL2)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_X)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_X)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Y)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_Y)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_UNIT1)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_UNIT2)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_SWITCH_CHANNEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_SWITCH_CHANNEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_WDL_REF)->ShowWindow(FALSE);
	
	GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_WDL_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PDL_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_WL)->EnableWindow(FALSE);


		CFont * m_font;
		m_font = new CFont;
		m_font->CreateFont(30, // nHeight
		10, // nWidth
		0, // nEscapement
		0, // nOrientation
		FW_BOLD, // nWeight
		FALSE, // bItalic
		FALSE, // bUnderline
		0, // cStrikeOut
		ANSI_CHARSET, // nCharSet
		OUT_DEFAULT_PRECIS, // nOutPrecision
		CLIP_DEFAULT_PRECIS, // nClipPrecision
		DEFAULT_QUALITY, // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("Arial")); // lpszFac 

	     GetDlgItem(IDC_STATIC_POW)->SetFont(m_font);

	// TODO: Add extra initialization here
	//获取配置文件路径
	m_strLogFileName.Format("%s\\TestInformation.txt", m_strNetConfigPath);


	//初始化归零次数
	
	ZeroMemory(m_nRefCount,sizeof(int)*12*32);

	CString strFileName;
	CString strValue;
	CString strKey;
	DWORD dwLength = 256;

	//获取配置信息
	//获得网络数据存储路径
	strFileName.Format("%s\\config\\DataSave.ini",g_tszAppFolder);
	int iLen = GetPrivateProfileString("ServeAddress","Address",NULL,m_strNetFile.GetBuffer(256),256,strFileName);
    if (iLen <= 0)
    {
		MessageBox("读取配置文件DataSave.ini失败");
		return FALSE;
    }
	char m_pchComputerName[256];
	CString strSave;
	ZeroMemory(m_pchComputerName, sizeof(m_pchComputerName));
	GetComputerName(m_pchComputerName, &dwLength);
	GetPrivateProfileString("Config","Address","error",m_strNetConfigPath.GetBuffer(256),256,strFileName);
	if (m_strNetConfigPath=="error")
	{
		MessageBox("读取配置文件DataSave.ini失败！");
		return FALSE;
	}
	strSave = m_strNetConfigPath;
	m_strNetConfigPath.Format("%s\\%s", strSave,m_pchComputerName);

	GetPrivateProfileString("Config", "PNConfig", "error", m_strPNConfigPath.GetBuffer(256), 256, strFileName);
	if (m_strPNConfigPath == "error")
	{
		MessageBox("读取配置文件DataSave.ini失败！");
		return FALSE;
	}
	
	//是否上传到网络路径
	
    iLen = GetPrivateProfileString("Save","Save",NULL,strSave.GetBuffer(128),128,strFileName);
    if (iLen <= 0)
    {
		MessageBox("读取配置文件DataSave.ini失败");
		return FALSE;
    }
	m_bIfSaveToServe = atoi(strSave);

	GetPrivateProfileString("FirmwareVer","Value","error",m_strFWVer.GetBuffer(256),256,strFileName);
	if (m_strFWVer=="error")
	{
		MessageBox("读取配置文件DataSave.ini失败！");
		return FALSE;
	}
	
	/*
	CString strFile;
	if (m_bIfSaveToServe)
	{
		strFile.Format("%s\\config\\%s\\PortInfo.ini",m_strNetConfigPath,m_strPN);
	}
	else
	{
		strFile.Format("%s\\config\\%s\\PortInfo.ini",g_tszAppFolder,m_strPN);
	}
	
	GetPrivateProfileString("CHCount","Value","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		MessageBox("读取配置文件PortInfo.ini错误CHCount");
		return FALSE;
	}
	m_nCHCount = atoi(strValue);
	*/
		
	if (m_bIfSaveToServe==1)
	{
		MessageBox("使用网路路径！");
	}
	else
	{
		MessageBox("使用本地路径！");
	}
    SetDefID(IDC_BUTTON15);
	SWMsgCheckShow();
	char* pcharOutString = NULL;
	if (!m_fusion.m_pFucSetEmployeeAccount(m_strID, &pcharOutString))
	{
		AfxMessageBox(pcharOutString);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy126S_45V_Switch_AppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == 61536)
	{
		OnButtonExit();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy126S_45V_Switch_AppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy126S_45V_Switch_AppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMy126S_45V_Switch_AppDlg::OnButtonStopTest() 
{
	// TODO: Add your control notification handler code here
		m_bTestStop = TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonIlRef() 
{
	// TODO: Add your control notification handler code here
	double     dblReadPwr;
	CString strReferenceFileName(""),strMsg(""),strRemarkFlag("");
	CString strMessage,hCSVFile;
	int nCH;
	CString strInfo;
	LPTSTR pStr;
	char   sep[] = ",\t\n";
	char   *token = NULL;
	char   lineBuf[256];
	CString strTemp,strDataBeginFlag;
	CStdioFile fileReferenceFile;
	int iSel;
	int nCount = 0;

	if (m_bOpenPM == FALSE)
	{
		MessageBox("请先打开光功率计！");
		return;			
	}
	
//	if (m_bOpenPM)
	{	
		strReferenceFileName.Format("%s\\data\\RefZeroData.txt",g_tszAppFolder);
		
		m_ctrlListMsg.ResetContent();
		strRemarkFlag = ";";		
		if (GetFileAttributes(strReferenceFileName) != -1)
		{
			strMsg.Format("最近一次归零数据存放在%s\n"
				"点击<是>\t调用该数据\n"
				"点击<否>\t重新归零\n",strReferenceFileName);
			iSel = MessageBox(strMsg,"确认",MB_YESNO |MB_ICONQUESTION);
		}
		else
		iSel = IDNO;
				
		if (IDYES == iSel)
		{
			if (!(fileReferenceFile.Open(strReferenceFileName,CFile::modeRead|CFile::typeText,NULL)))
			{
			  MessageBox("打开文件失败！");
			  return;
			}
			else
			{
			  pStr = fileReferenceFile.ReadString(lineBuf,256);
			  m_bHasRefForTest = TRUE;

			  while (NULL != pStr)
			  {
				  strTemp.Format("%s",lineBuf);
				  strTemp.Replace("\t"," ");
				  strTemp.TrimLeft();
				  strTemp.TrimRight();
				  strRemarkFlag = strTemp.Left(1);
				  strTemp.TrimRight();
				  if (!strcmp(";",strRemarkFlag)||strTemp.IsEmpty())
				  {
					  pStr = fileReferenceFile.ReadString(lineBuf,256);
					  continue;
				  }
				  //获取数据
				  token = strtok(lineBuf,sep);
				  strTemp.Format("%s",token);
				  strTemp.TrimLeft();
				  nCH = strtol(strTemp,NULL,10) - 1;

				  token = strtok(NULL,sep);
				  strTemp.Format("%s",token);
				  strTemp.TrimLeft();
				  m_dblReferencePower[nCH] = strtod(strTemp,NULL);
				  pStr = fileReferenceFile.ReadString(lineBuf,256);
			  }
			  fileReferenceFile.Close();

			  //显示
			  strMsg.Format("通道    归零功率(dBm)");
			  LogInfo(strMsg,FALSE);
			  for (int i = 0; i < m_nCHCount; i++)
			  {
				  strMsg.Format("%02d      %.2f",i+1,m_dblReferencePower[i]);
				  if (100 == m_dblReferencePower[i])
				  {
					  m_bHasRefForTest = FALSE;
					  nCount++;
				  }
				  LogInfo(strMsg,FALSE);
			  }
			  if (m_bHasRefForTest == FALSE)
			  {
				  strMsg.Format("还有%d个通道的归零未完成！",nCount);
				  LogInfo(strMsg);
			  }
			  else
				  LogInfo("所有通道的归零已完成！");
			}
		}
		//重新归零
		else if (IDNO == iSel)
		{
			if(!fileReferenceFile.Open(strReferenceFileName,CFile::modeCreate|CFile::modeReadWrite,NULL))
			{
				MessageBox("创建文件失败！");
				return;
			}
			strMsg.Format("channel  RefPower(dBm)\n");
			strMsg = strRemarkFlag + strMsg;
			fileReferenceFile.WriteString(strMsg);

			UpdateData();
			nCH = m_cbChannel.GetCurSel();
			strMsg.Format("请连接光源线到通道%d\n<连接好后点击确定>",nCH);
			MessageBox(strMsg);
			dblReadPwr = ReadPWMPower(nCH-1);
			strInfo.Format("通道%d的归零光功率为：%.2fdBm",nCH,dblReadPwr);
			LogInfo(strInfo,FALSE);
			m_dblReferencePower[nCH-1] = dblReadPwr;
			m_bHasRefForTest = TRUE;

			for (int i = 0; i < 12; i ++)
			{
				if (m_dblReferencePower[nCH] != 100)
				{
					strMsg.Format("%d,          %.3f\n",i+1,m_dblReferencePower[i]);
					fileReferenceFile.WriteString(strMsg);
				}
				else
				{
					strMsg.Format(" \n");
					fileReferenceFile.WriteString(strMsg);
					m_bHasRefForTest = FALSE;
				}
				
			}
			fileReferenceFile.Close();    		
		}   	
	}
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonOpenPort() 
{
	// TODO: Add your control notification handler code here
	TCHAR tszCOM[15];
    CString strCOM;
	CString strError,strMsg;

	CString strFileName;

	UpdateData(TRUE);
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!ReadTestInfo())
	{

		return;
	}
	if (m_nPort>9)
	{
		strCOM.Format("\\\\.\\COM%d",m_nPort);
	}
	else
	{
		strCOM.Format("COM%d",m_nPort);
	}
//    strCOM.Format("COM%d",m_nPort);
    strcpy(tszCOM,strCOM);

	if (m_bOpenPort)
	{
		m_opCom.ClosePort();
		SetDlgItemText(IDC_BUTTON_OPEN_PORT,"打开产品串口");
		m_bOpenPort = FALSE;
		LogInfo("关闭串口成功！");
	}
	else
	{
		if (!m_opCom.OpenPort(tszCOM,115200))
		{
			strMsg.Format("串口打开错误！");
			//MessageBox(strMsg,"Initial Error",MB_OK|MB_ICONERROR);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);

			return;
		}
		else
		{
			LogInfo("串口打开成功！");
			m_bOpenPort = TRUE;
			SetDlgItemText(IDC_BUTTON_OPEN_PORT,"关闭串口");

			CString strMfgInfo =  GetModuleInfo();
			CString strFWVer = strMfgInfo.Mid(8,3);
			strMsg.Format("FW版本%s,FQC版本%s",strFWVer,m_strFWVer);
			LogInfo(strMsg);
			if (strcmp(strFWVer,m_strFWVer)==0)
			{
				LogInfo("验证FW版本信息成功",BOOL(FALSE),COLOR_GREEN);
			}
			else 
			{
				m_opCom.ClosePort();
				SetDlgItemText(IDC_BUTTON_OPEN_PORT,"打开产品串口");
				m_bOpenPort = FALSE;
				LogInfo("关闭串口成功！");
				LogInfo("验证FW版本信息失败",BOOL(FALSE),COLOR_RED);
				return ;
			}

//			m_CmdFUJ.m_hCommPort = m_opCom.m_hCommPort;
			//if (!GetATMTestTemplate())
			//{
			//	MessageBox("获取无纸化模板失败！");
			//	return;
			//}
		}
	} 
	UpdateWindow();


}

BOOL CMy126S_45V_Switch_AppDlg::ReadTestInfo()
{
   UpdateData(TRUE);
   
   if (m_strID.IsEmpty())
   {
	   MessageBox("请输入工号！",MB_OK);
	   return FALSE;
   }
  // if (m_strSN.IsEmpty())
  // {
//	   MessageBox("请输入SN号！",MB_OK);
//	   return FALSE;
 //  }

   return TRUE;
}

VOID CMy126S_45V_Switch_AppDlg::LogInfo(LPCTSTR tszlogMsg, BOOL bAppend,DWORD dwColor)
{
	int				dx  = 0;
	CDC*			pDC;
	DWORD			dwLineIndex=0;
	FILE*			pLogFile	= NULL;
	CString         strMsg;
	CSize			sz;
	CString         strToken;
	CString         strTime;
    CString         strValue;
	SYSTEMTIME      st;
	CString         strNetFile;
	CString         strFile;

	pDC			= m_ctrlListMsg.GetDC();
	dwLineIndex = m_ctrlListMsg.GetCount();
	
	// if the lines is more than 256
	// clear the message here

	if(dwLineIndex > MAX_LIST_LINES)
	{
		m_ctrlListMsg.ResetContent();
		dwLineIndex = 0;
	}

	FindMyDirectory(m_strItemName,m_strPN,m_strSN);

	char chComputerName[256];
	DWORD dwLength=256;
	ZeroMemory(chComputerName,sizeof(chComputerName));
	GetComputerName(chComputerName,&dwLength);
	
	GetLocalTime(&st);  
	int swnum;
	
	if (b_issn1) 
	{
		swnum=1;
	}
	else if (b_issn2)
	{
	    swnum=2;
	}
	else if(b_issn3)
	{
		swnum=3;
	}
	strTime.Format("(%s)%02d:%02d:%02d SW:%d",&chComputerName,st.wHour,st.wMinute,st.wSecond,swnum);
	strValue.Format("%s%s",strTime,tszlogMsg);
	strNetFile.Format("%s\\data\\%s\\%s\\%s\\Log-%04d-%02d-%02d.txt",m_strNetFile,m_strItemName,m_strPN,m_strSN,st.wYear,st.wMonth,st.wDay);
	strFile.Format("%s\\data\\%s\\%s\\%s\\Log-%04d-%02d-%02d.txt",g_tszAppFolder,m_strItemName,m_strPN,m_strSN,st.wYear,st.wMonth,st.wDay);
	if (m_bIfSaveToServe)
	{
		pLogFile = fopen(strNetFile,"a+");
	}
	else
	{
		pLogFile = fopen(strFile,"a+");
	}
	if(pLogFile!=NULL)
	{
		fprintf(pLogFile, "%s\n", strValue);
		fclose(pLogFile);
		if (m_bIfSaveToServe)
		{
			CopyFile(strNetFile,strFile,FALSE);
		}
	}
	else
	{

	}
	//如果bAppend为真，则自动换行
//	if (bAppend)
//	{
//		strToken = strtok((char *)tszlogMsg,"\r");
//		strToken.Replace("\t","    ");
//		m_ctrlListMsg.AddString(strToken,COLOR_BLUE);
//		while (strToken != "")
//		{
//			strToken = strtok(NULL,"\r");
//			strToken.Replace("\t","     ");
//			m_ctrlListMsg.AddString(strToken);
//		}
//	}
//	else
       m_ctrlListMsg.AddString(tszlogMsg,dwColor);

	m_ctrlListMsg.SetCurSel(dwLineIndex);
	
	if (dwLineIndex > 9)
	{
		m_ctrlListMsg.SetTopIndex(dwLineIndex - 9);
	}

	for (DWORD i = 0; i < dwLineIndex + 1; i++)
	{
		m_ctrlListMsg.GetText(i, strMsg);
		sz = pDC->GetTextExtent(strMsg);
		
		if (sz.cx > dx)
			dx = sz.cx;
	}
	m_ctrlListMsg.ReleaseDC(pDC);
	
	// Set the horizontal extent so every character of all strings 
	// can be scrolled to.
	m_ctrlListMsg.SetHorizontalExtent(dx + 10);
// 	是否有LOG文件夹	
// 		if(bAppend)
// 		{
// 			pLogFile = fopen(m_strLogFileName, "at");
// 			
// 			if(!pLogFile)
// 			{
// 				return;
// 			}
// 			
// 			fprintf(pLogFile, "%s\n", tszlogMsg);
// 			
// 			fclose(pLogFile);
// 		}
	UpdateWindow();

}

void CMy126S_45V_Switch_AppDlg::OnButtonIlctTest() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	CString strTemp1;
	CString strChannel;
	CString strMsg;
	double  dblPref;
	double  dblPout;
	double  dblPower;
    CString	m_strchan,dwBaseAddress;
    CString	strCommand;
	char    strCT[32];
	char    strIL[32];
	int nchannel;
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
//	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;
//	int j;
	int i;
	BOOL   bFlag = TRUE;
	m_bTestStop = FALSE;

	ZeroMemory(strCT,sizeof(strCT));
	ZeroMemory(strIL,sizeof(strIL));

	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}
	int nWL = m_cbSetWL.GetCurSel()-1;
	if (nWL>=0)
	{
		SetPWMWL(m_dblWL[nWL]);
		m_bChange=TRUE;
	}
	else
	{
	}
	//获取当前时间
	SYSTEMTIME st;
	COleDateTime    tCurTime;
	COleDateTimeSpan tDifHour; 
	int        nDifHour;
	GetLocalTime(&st);
	tCurTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	tDifHour = tCurTime-m_RefStartTime; //计算时间插值
	nDifHour = tDifHour.GetTotalHours();
	if (nDifHour>m_nRefTime)  //比较归零时间差异
	{
		//TEST
		strMsg.Format("当前归零时间为%d小时，已超过归零限制时间（%d小时），请重新归零！",nDifHour,m_nRefTime);
		MessageBox(strMsg);
		m_bHasRefForTest = FALSE;
		return;
	}
	UpdateData();
	FindMyDirectory(m_strItemName,m_strPN,m_strSN);
	m_ctrlListMsg.ResetContent();
	double dblTargetTempt;
	int    nTemptStatus; //设置温度状态
	double dblCurTempt; //当前温度
	int nTemp = m_ctrlComboxTemperature.GetCurSel();
	if (nTemp==0)
	{
		strTemp.Format("开始测试IL及CT常温测试！");
		LogInfo(strTemp,FALSE);
		strTemp.Format("通道");
		for ( i=0;i<m_nCHCount;i++)
		{
			strTemp1.Format("  CT%d",i+1);
			strTemp = strTemp+ strTemp1;
		}
		LogInfo(strTemp);
		nTemptStatus = 0;
	}
	else if (nTemp==1)
	{
		strTemp.Format("开始测试IL低温测试！");
		LogInfo(strTemp);
		nTemptStatus = 2;
	}
	else 
	{
		strTemp.Format("开始测试IL高温测试！");
		LogInfo(strTemp);
		nTemptStatus = 1;
	}	
	dblTargetTempt = m_dblTemperature[nTemp];
	//判断当前温度是否达到要求；
	if (m_bConnectTemptControl)
	{
		int nIndex=0;
		while(TRUE)
		{
			dblCurTempt = GetCurrentTempt(); //获取当前冷热盘温度
			if (fabs(dblTargetTempt-dblCurTempt)>2)
			{
				strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
				LogInfo(strMsg,BOOL(FALSE),COLOR_BLACK);
				Sleep(1000);
			}
			else
			{
				break;
			}
			nIndex++;
			if(nIndex>200)
			{
				strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
				LogInfo(strMsg,BOOL(FALSE),COLOR_RED);
				SetTemptStatus(nTemptStatus);
				return;
			}
		}
	}
	/*if (m_strItemName=="终测") //终测直接调用数据
	{
		strMsg.Format("是否调用已下载的数据？\n",
			"点击<是>:已重新扫描，直接测试\n",
			"点击<否>:调用初测数据进行测试\n");
	//	int iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);
		iSel = IDNO;
		if (iSel==IDNO)
		{
			//若是终测，则直接使用调用的电压值测试IL，并copy对应通道的其他参数
			MessageBox("请将对应通道按顺序接入光功率计中");
			if(!FinalTest())
			{
				LogInfo("调用电压数据出错！");
				return;
			}
			LogInfo("开始下载新的数据");
			OnButtonDownloadBin();
	    	Sleep(1000);
		}
	}*/
	double pdblCT[32];
//	if (b_issn1)
//	{
//		m_nCHCount = 18;
//	}
//	else
//	{
//		m_nCHCount = 16;
//
//	}
	for (nchannel = 0; nchannel < m_nCHCount; nchannel++)
	{
		YieldToPeers();
		if (m_bTestStop)
		{
			MessageBox("测试中止！");
			return;
		}
		//开关切换
//		if(!SWToChannel(nchannel+1))
//		{
//			strMsg.Format("切换到通道%d错误",nchannel+1);
//			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
//			return;
//		}    

			/*TxDataToI2C("00D6",(BYTE*)"MFGCMD",6);
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}

			Sleep(100);
			RxDataFromI2C("00DC");
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}
            
 
		Sleep(100);*/

		if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return ;
		}

		
		if (nchannel>14)
		{
			m_strchan.Format("%x",nchannel+1);
		}
		else
		{
			m_strchan.Format("0%x",nchannel+1);

		}

		if(!SetSWChannel(atoi(dwBaseAddress),nchannel+1))
		{
			LogInfo("设置SWChannel失败!",FALSE,COLOR_RED);
			return ;
		}
		
        
		/*strCommand.Format("%s%s",dwBaseAddress,m_strchan);
		
		TxSTRToI2C("0143",strCommand); //Set Single MemsSwitch Channel
        
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return ;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return ;
		}*/
		
		Sleep(800);
		
		bFlag = TRUE;
		strTemp.Format("%02d",nchannel+1);
		if (m_ctrlComboxTemperature.GetCurSel()==0)
		{
			if (m_nPMType[0]==PM_PDARRAY)
			{
				if (!GetCT(pdblCT,m_nCHCount)) 
				{
					return;
				}
			}
			for(int i = 0; i < m_nCHCount; i++)
			{
				//调用归零数据
				if (nWL>=0)
				{
					dblPref = m_dblWDLRef[i][nWL];
				}
				else
				{
					dblPref = m_dblReferencePower[i];
				}
				if (m_nPMType[0]==PM_PDARRAY)
				{
					dblPower = pdblCT[i];
				}
				else
				{
					dblPower = ReadPWMPower(i);
				}
				
				strMsg.Format("%.2f",dblPower);
			
				dblPout = dblPower;
				ChannalMessage[nchannel].dblCT[i] = TestIL(dblPref,dblPout);
				if (nchannel == i)
				{
					ChannalMessage[nchannel].dblIL = ChannalMessage[nchannel].dblCT[i];
					if (nWL>=0)
					{
						m_dblWDL[nchannel][nWL] = ChannalMessage[nchannel].dblCT[i];
					}
					if (!CheckParaPassOrFail(IL_ROOM_DATA,ChannalMessage[nchannel].dblIL))
					{
						bFlag = FALSE;
					}			
				}
				else
				{
					if (!CheckParaPassOrFail(CT_DATA,ChannalMessage[nchannel].dblCT[i]))
					{
						bFlag = FALSE;
					}
				}
				strTemp1.Format("  %.2f",ChannalMessage[nchannel].dblCT[i]);
				strTemp = strTemp+strTemp1;
			}
		}
		else if (m_ctrlComboxTemperature.GetCurSel()==1) 	
		{
			if (nWL>=0)
			{
				dblPref = m_dblWDLRef[nchannel][nWL];
			}
			else
			{
				dblPref = m_dblReferencePower[nchannel];
			}
			dblPower = ReadPWMPower(nchannel);
			ChannalMessage[nchannel].dblLowTempIL = TestIL(dblPref,dblPower);
			strTemp.Format("Channel:%d; IL:%.2f",nchannel+1,ChannalMessage[nchannel].dblLowTempIL);	
			if (!CheckParaPassOrFail(IL_LOW_DATA,ChannalMessage[nchannel].dblLowTempIL))
			{
				bFlag = FALSE;
			}
		}
		else if (m_ctrlComboxTemperature.GetCurSel()==2) 	
		{
			if (nWL>=0)
			{
				dblPref = m_dblWDLRef[nchannel][nWL];
			}
			else
			{
				dblPref = m_dblReferencePower[nchannel];
			}
			dblPower = ReadPWMPower(nchannel);
			ChannalMessage[nchannel].dblHighTempIL = TestIL(dblPref,dblPower);
			strTemp.Format("Channel:%d; IL:%.2f",nchannel+1,ChannalMessage[nchannel].dblHighTempIL);
			if (!CheckParaPassOrFail(IL_HIGH_DATA,ChannalMessage[nchannel].dblHighTempIL))
			{
				bFlag = FALSE;
			}
		}		
		if (!bFlag)
		{
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			LogInfo(strTemp);
		}	
	}
	if (m_ctrlComboxTemperature.GetCurSel()==0)
	{
		//SaveDataToExcel(IL_ROOM_DATA);
		SaveDataToCSV(IL_ROOM_DATA);
		//UpdateATMData(IL_ROOM_DATA);
		m_bRoomIL = TRUE;
		Sleep(200);
		//SaveDataToExcel(CT_DATA);
		SaveDataToCSV(CT_DATA);
		//UpdateATMData(CT_DATA);
    	LogInfo("IL/CT常温测试完毕!");
		m_bTestIL = TRUE;
	}
	else if (m_ctrlComboxTemperature.GetCurSel()==1)
	{
		//SaveDataToExcel(IL_LOW_DATA);
		SaveDataToCSV(IL_LOW_DATA);
		//UpdateATMData(IL_LOW_DATA);
		m_bLowIL = TRUE;
		LogInfo("IL低温测试完毕!");
	}
	else if (m_ctrlComboxTemperature.GetCurSel()==2)
	{
		//SaveDataToExcel(IL_HIGH_DATA);
		SaveDataToCSV(IL_HIGH_DATA);
		//UpdateATMData(IL_HIGH_DATA);
		m_bHighIL = TRUE;
		LogInfo("IL高温测试完毕!");
	}
	if (m_bRoomIL&&m_bLowIL&&m_bHighIL)
	{
		double dblMaxIL;
		double dblMinIL;
		//计算TDL
		for(int nCH=0;nCH<m_nCHCount;nCH++)
		{
			dblMinIL=100;
			dblMaxIL=-99;
			bFlag = TRUE;
			if (ChannalMessage[nCH].dblLowTempIL>dblMaxIL)
			{
				dblMaxIL = ChannalMessage[nCH].dblLowTempIL;
			}
		    if (ChannalMessage[nCH].dblHighTempIL>dblMaxIL)
			{
				dblMaxIL = ChannalMessage[nCH].dblHighTempIL;
			}
			if (ChannalMessage[nCH].dblIL>dblMaxIL)
			{
				dblMaxIL = ChannalMessage[nCH].dblIL;
			}

			if (ChannalMessage[nCH].dblLowTempIL<dblMinIL)
			{
				dblMinIL = ChannalMessage[nCH].dblLowTempIL;
			}
			if (ChannalMessage[nCH].dblHighTempIL<dblMinIL)
			{
				dblMinIL = ChannalMessage[nCH].dblHighTempIL;
			}
			if (ChannalMessage[nCH].dblIL<dblMinIL)
			{
				dblMinIL = ChannalMessage[nCH].dblIL;
			}
			ChannalMessage[nCH].dblTDL = dblMaxIL-dblMinIL;

			strTemp.Format("Channel:%d; TDL:%.2f",nCH+1,ChannalMessage[nCH].dblTDL);
			if (!CheckParaPassOrFail(TDL_DATA,ChannalMessage[nCH].dblTDL))
			{
				bFlag = FALSE;
			}	
			if (!bFlag)
			{
				LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
			}
			else
			{
				LogInfo(strTemp);
			}	
		}
		SaveDataToCSV(TDL_DATA);
		//SaveDataToExcel(TDL_DATA);
		//UpdateATMData(TDL_DATA);
		LogInfo("计算TDL完毕!");
	}
	if (m_strItemName=="终测"&&m_nCHCount<12) //若是终测，则COPY初测对应通道的数据
	{
	//	if (iSel==IDNO)
		{
			CopyIntialTestData();
			LogInfo("复制初测数据结束");
		//	OnButtonCheckData();
        	OnButtonUploadAtms();
		}
	}

		if(!SetSWChannel(atoi(dwBaseAddress),0))
		{
			LogInfo("设置SWChannel失败!",FALSE,COLOR_RED);
			return ;
		}

		/*TxDataToI2C("00D6",(BYTE*)"MFGCMD",6);
		if (m_strErrorMsg != "")
		{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
		}

		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
		}
            
 
		Sleep(100);
		 if (b_issn1) 
		{
			dwBaseAddress="00";
		}
		else if (b_issn2)
		{
			dwBaseAddress="01";
		}
		else if(b_issn3)
		{
			dwBaseAddress="02";
		}
		else
		{
			return ;
		}

		
        
		strCommand.Format("%s00",dwBaseAddress);
		
		TxSTRToI2C("0143",strCommand);
        
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return ;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return ;
		}*/
		
	
}
//=============
//计算IL
//=============
double CMy126S_45V_Switch_AppDlg::TestIL(double Pref,double Pout)
{
	double IL;
	IL = Pref - Pout;
	return IL;

}

//EXCEL保存
void CMy126S_45V_Switch_AppDlg::OnButtonScan() 
{
	// TODO: Add your control notification handler code here
	CString strFileName,strMsg;
	int nChannel = 0;
	CString strFile;
	CString strValue;
	int iSel= IDYES;
	int nCHNumber=0;
	int nCHCount=0;
	//保存本地文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	//		WIN32_FIND_DATA win32Find;
	CString strNetPath;
	CString strTempt;

	try
	{
		if(!m_blogin)
		{
			MessageBox("请登陆无纸化账号!");
			return;
		}
		if (!m_bGetSNInfo)
		{
			MessageBox("请先确认SN号的信息！");
			return;
		}
		
		if (!m_bOpenPM)
		{
			MessageBox("请先连接光功率计！");
			return;
		}
		if (!m_bOpenPort)
		{
			MessageBox("请先打开串口！");
			return;
		}
		if (!m_bHasRefForTest)
		{
			MessageBox("请先归零！");
			return ;
		}

		if(m_pdblWavelength<=1361)
		{
			//切换光源
			if(!SetTLSWavelength(m_pdblWavelength))
			{
				MessageBox("设置激光器波长失败！");
			}
		}
		if(m_bChange)
		{
			if (!SetPWMWL(m_pdblWavelength))
			{
				LogInfo("设置波长错误！");
				return;
			}
			m_bChange=FALSE;
		}
		
		//设置路径
		UpdateData();
    	FindMyDirectory(m_strItemName,m_strPN,m_strSN);

		LogInfo("开始驱动开关，开始扫描...",FALSE);

        //初始化图形控件
		m_NTGraph.ClearGraph();
		m_NTGraph.SetXLabel("X(mv)");
		m_NTGraph.SetYLabel("Y(mv)");
		m_NTGraph.SetRange(-60000,60000,-60000,60000);
		m_NTGraph.SetXGridNumber(10);
		m_NTGraph.SetYGridNumber(10);

		CString strFileINI;
		int nComPort = m_cbComPort.GetCurSel();

		if (nComPort==0)
		{
			strFileINI = "ScanRange.ini";
		}
		else if (nComPort==1)
		{
			strFileINI = "ScanRange-COM1.ini";
		}
		else
		{

		}

		/*
		if (m_strItemName=="终测")
		{
			nCHCount = m_nCHCount;
		}
		else if (m_strItemName=="初测")
		{
			nCHCount = 12;
		}
		*/
		nCHCount = m_nCHCount;

		nChannel = m_cbChannel.GetCurSel()-1;
		LogInfo("开始调用电压数据！");
		int nPort=0;
		if(1) //(!GetVoltageDataFromATMS())
		{
			LogInfo("调用不到无纸化数据，开始调用文件数据！");
			
			for (int i=0;i<nCHCount;i++)
			{
				if (m_cbChannel.GetCurSel()==0)
				{
					nPort = i;
				}
				else
				{
					nCHCount = 1;
					nPort = m_cbChannel.GetCurSel()-1;
				}


				if(!GetVolDataFormFile(nPort))
				{
					LogInfo("找不到可调用的电压文件！",FALSE,COLOR_RED);
					return;
				}
			}			
		}
		/*
		if (m_strItemName=="终测")
		{
			nCHCount = m_nCHCount;
		}
		else if (m_strItemName=="初测")
		{
			nCHCount = 12;
			if(m_strPN=="18X1")
			{
				nCHCount=18;
			}
		}
		*/

		for (int iIndex=0;iIndex<nCHCount;iIndex++)
		{
			YieldToPeers();
			//细扫
			if (m_cbChannel.GetCurSel()==0)
			{
				nPort = iIndex;
			}
			else
			{
				nCHCount=1; //只扫描一次
				nPort = m_cbChannel.GetCurSel()-1;
			}
			
			//TxDataToI2C("00D6",(BYTE*)"MFGCMD",6);  //MFG command enable or disable

			/*if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}

			//Sleep(100);
			//RxDataFromI2C("00DC"); //Command Status

			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}*/
            
 
			//Sleep(100);
			//RxDataFromI2C("0161");//temperature

			double dbModuleTemp = GetModuleTemp();
		    CString msg;
		    long tempint;

		    msg.Format("温度: %.2f",dbModuleTemp);
			LogInfo(msg);

			   int nTemp = m_ctrlComboxTemperature.GetCurSel();
			   if (nTemp==0)
			   {
				   //m_lutRoomTep = dbModuleTemp * 100;
				   m_lutRoomTep.Format("%d",(int)(dbModuleTemp * 100));
				  tempint = atoi(m_lutRoomTep);//strtoul(m_lutRoomTep,NULL,16); 
				  if (tempint<2200 || tempint>2800)
				  {
					  msg = "温度选择不对，请在常温下定标";
					  LogInfo(msg,FALSE,COLOR_RED);
			    	  return;
				
				  }
			   }
			   if (nTemp==1)
			   {

				   m_lutLowTep.Format("%d",(int)(dbModuleTemp * 100));
				   //WORD w_lowtep;
				   short db_lowtep;
				   db_lowtep = atoi(m_lutLowTep);//strtoul(m_lutLowTep,NULL,16);  // m_lutLowTep 
				   //db_lowtep = w_lowtep;
				   if (db_lowtep > -200 || db_lowtep< -800)
				  {
					  msg = "温度选择不对，请在低温下定标";
					  LogInfo(msg,FALSE,COLOR_RED);
			    	  return;
				
				  }
			   }
			   if (nTemp==2)
			   {
				   m_lutHighTep.Format("%d",(int)(dbModuleTemp * 100));
				   tempint = atoi(m_lutHighTep);//strtoul(m_lutHighTep,NULL,16);  //m_lutHighTep

				   if (tempint<6700 || tempint>7300)
				  {
					  msg = "温度选择不对，请在高温下定标";
					  LogInfo(msg,FALSE,COLOR_RED);
			    	  return;
				
				  }
			   }
			if(!AutoPointScan(nPort))
			{
				strMsg.Format("通道%d扫描出错,是否进行粗扫描？",nPort+1);
				iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);
				if (iSel==IDNO)
				{
					//continue;
					return;

				}
				if(!AutoScanChannel(nPort))
				{
					strMsg.Format("通道%d粗描出错",nPort+1);
					LogInfo(strMsg);
					//continue;
					return;
				}
				if(!AutoPointScanRIght(nPort))
				{
					strMsg.Format("通道%d细描出错",nPort+1);
					LogInfo(strMsg);
					//continue;
					return;
				}
			}
			
			if (m_cbComPort.GetCurSel()==1)
			{
				if(m_ctrlComboxTemperature.GetCurSel()==0)
				{
					strTempt = "Room";
					strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
					strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				}
				else if (m_ctrlComboxTemperature.GetCurSel()==1)
				{
					strTempt = "Low";
					strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1-Low.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
					strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1-Low.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				}
				else if (m_ctrlComboxTemperature.GetCurSel()==2)
				{
					strTempt = "High";
					strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1-High.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
					strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1-High.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				}
			}
			else
			{
				if(m_ctrlComboxTemperature.GetCurSel()==0)
				{
					strTempt = "Room";
					strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
					strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				}
				else if (m_ctrlComboxTemperature.GetCurSel()==1)
				{
					strTempt = "Low";
					strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-Low.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
					strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-Low.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				}
				else if (m_ctrlComboxTemperature.GetCurSel()==2)
				{
					strTempt = "High";
					strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-High.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
					strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-High.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				}
			}

			if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
			{
				MessageBox("创建数据文件失败！");
				return;
			}
			stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");

			{
				strContent.Format("%d,%d,%d,%.2f\n",nPort+1,ScanPowerChannel[nPort].VoltageX,ScanPowerChannel[nPort].VoltageY,ScanPowerChannel[nPort].MaxPower);
				stdCSVFile.WriteString(strContent);
			}
			stdCSVFile.Close();

			if (m_strItemName=="终测") //终测需判断与初测IL数据的差异
			{
				CString strFile;
				CString strValue;
				double  dbMaxValue=0;
//				if (m_bIfSaveToServe)
//				{
//					strFile.Format("%s\\config\\%s-%s\\ParaDemand.ini",m_strNetConfigPath,m_strSpec,m_strPN);
//				}
//				else
//				{
					strFile.Format("%s\\%s-%s\\ParaDemand.ini", m_strPNConfigPath,m_strSpec,m_strPN);
//				}
				
				GetPrivateProfileString("DIFF_IL","Value","error",strValue.GetBuffer(128),128,strFile);
				if (strcmp(strValue,"error")==0)
				{
					strMsg.Format("获取配置%s失败！(DIFF_IL)",strFile);
					LogInfo(strMsg,FALSE,COLOR_RED);
					return ;
				}
				dbMaxValue = atof(strValue);
				if (fabs(m_dbInitialIL[nPort]-ScanPowerChannel[nPort].MaxPower)>dbMaxValue)
				{
					strMsg.Format("通道%d的IL与初测IL的差异超过了%.2f dB,请检查接线!",nPort+1,dbMaxValue);
					LogInfo(strMsg,COLOR_RED);				
				} 
			}

			if (m_bIfSaveToServe)
			{
				CopyFile(strCSVFile,strNetPath,FALSE);
			}
		
			//保存电压数据到EXCEL
			SaveDataToExcel(VOL_DATA,nPort);	

			if (stricmp(m_strSpec,"Z4767")==0)
			{
				if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0)
				{
					if (m_cbTestCH.GetCurSel()==1)
					{
						if(!Find8dBAtten(nPort,strTempt))
						{
							LogInfo("定标8dB衰减点位置错误！");
						}
					}
					else
					{
						m_n5dBAttenX[nPort] = 0;
						m_n5dBAttenY[nPort] = 0;
						m_n8dBAttenX[nPort] = 0;
						m_n8dBAttenY[nPort] = 0;
					}
				}
			}	
			YieldToPeers();
		}
		SaveDataToCSV(VOL_DATA);
		//UpdateATMData(VOL_DATA);
		Sleep(100);
		
	}
	catch(TCHAR* ptszErrorMsg)
	{
		MessageBox(ptszErrorMsg);
		return;
	}
	catch(...)
	{
		MessageBox("其他错误！扫描失败");
		return;
	}

	LogInfo("保存扫描信息完毕！",TRUE);  
}

void CMy126S_45V_Switch_AppDlg::OnButtonTDLTest() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int nChannel = m_cbChannel.GetCurSel();

	if(nChannel <= 0)
	{
		LogInfo("请选择单个通道!",FALSE,COLOR_RED);
		return ;
	}

	CString	dwBaseAddress, strTemp;
	if (b_issn1) 
	{
		dwBaseAddress="01";
	}
	else if (b_issn2)
	{
		dwBaseAddress="02";
	}
	else if(b_issn3)
	{
		dwBaseAddress="03";
	}
	else if(b_issn4)
	{
		dwBaseAddress="04";
	}
	else
	{
		return ;
	}
	if(!SetSWChannel(atoi(dwBaseAddress),nChannel))
	{
		LogInfo("设置SWChannel失败!",FALSE,COLOR_RED);
		return ;
	}
    double dblPower = ReadPWMPower(nChannel-1);
	double dblIL = m_dblReferencePower[nChannel-1] - dblPower;
	strTemp.Format("通道%d IL: %.2f",nChannel,dblIL);
	LogInfo(strTemp);
}


BOOL CMy126S_45V_Switch_AppDlg::SaveCSVFileScanDate(DWORD dwPhaseCount)
{
   BOOL bFunctionOK = FALSE;
   DWORD dwBytesWrite,dwBytesReturned;
   CString strFileName,strTmptValue,strSourcePath;
   HANDLE hCSVFile = INVALID_HANDLE_VALUE;
  
   //设置文件
   strFileName.Format("%s\\data\\Scan_Data.csv",g_tszAppFolder);
   try
   {
	   //创建文件
/*	   hCSVFile = CreateFile(strFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);*/

// 	   if (INVALID_HANDLE_VALUE == hCSVFile)
// 	   {
// 		   throw"创建扫描文件失败！";
// 	   }
// 	   //保存CSV文件头
// 	   if (!SaveCSVFileHeader(hCSVFile))
// 	   {
// 		   throw"创建扫描文件头失败";
// 	   }
	   hCSVFile = CreateFile(strFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	   int nchannel;
	   nchannel = m_cbChannel.GetCurSel();
	   for (int j=0;j<12;j++)
	   {
		  if(nchannel == j)
		   {	  
			   strTmptValue.Format("%d,%.2f,%.2f,%0.2f\n",j+1,ScanPowerChannel[j].VoltageX,
				   ScanPowerChannel[j].VoltageY,ScanPowerChannel[j].MaxPower);
			   dwBytesWrite = strTmptValue.GetLength() * sizeof(char);
		       WriteFile(hCSVFile, strTmptValue, dwBytesWrite, &dwBytesReturned, NULL);
			 if(dwBytesWrite != dwBytesReturned)
				 {
					   throw "Function, 写入扫描数据失败！";
				 }
		   }
		   bFunctionOK = TRUE;
	   }
	   
   }
   catch(TCHAR * ptszErrorMsg)
   {
	   MessageBox(ptszErrorMsg,"保存文件失败",MB_OK|MB_ICONERROR);
	   bFunctionOK = FALSE;
   }

   //关闭文件句柄
   if (hCSVFile != INVALID_HANDLE_VALUE)
   {
	   CloseHandle(hCSVFile);
	   hCSVFile = INVALID_HANDLE_VALUE;
   }
//    if (m_bIsSaveServer)
//    {
	   strSourcePath.Format("%s\\data\\Scan_Power.csv",
		   g_tszAppFolder);

/*   }*/
   return bFunctionOK;
}

BOOL CMy126S_45V_Switch_AppDlg::SaveCSVFileHeader(HANDLE hCSVFile,LONG* lFileSize)
{
       BOOL bFunctionOK=FALSE;
       DWORD dwBytesWrite,dwBytesReturned;
	   CString strFileName,strTmptValue,strAxisX,strAxisY;
/*	   WORD wGetTemp;*/
	   LONG lDistanceToMove=0;

	   try
	   {
	       //写入SN(ROW 1)
		   strTmptValue.Format("SN,%s\n",m_st126sTestInfo.tszSerialNO);
		   dwBytesWrite = strTmptValue.GetLength() *sizeof(char);
		   WriteFile(hCSVFile,strTmptValue,dwBytesWrite,&dwBytesReturned,NULL);
		   if (dwBytesWrite!=dwBytesReturned)
		   {
			   throw"写入SN错误,保存CSV文件头错误！";
		   }
		   lDistanceToMove += dwBytesReturned;

		   //写入SPEC(ROW 2)
		   strTmptValue.Format("SPEC,%s\n",m_st126sTestInfo.tszSPEC);
		   dwBytesWrite = strTmptValue.GetLength() *sizeof(char);
		   WriteFile(hCSVFile,strTmptValue,dwBytesWrite,&dwBytesReturned,NULL);
		   if (dwBytesWrite!=dwBytesReturned)
		   {
			   throw"写入spec错误，保存CSV文件头错误！";
		   }
		   lDistanceToMove += dwBytesReturned;

		   //写入ID(ROW 3)
		   strTmptValue.Format("ID,%s\n",m_st126sTestInfo.tszOperatorID);
		   dwBytesWrite = strTmptValue.GetLength() *sizeof(char);
		   WriteFile(hCSVFile,strTmptValue,dwBytesWrite,&dwBytesReturned,NULL);
		   if (dwBytesWrite!=dwBytesReturned)
		   {
			   throw"写入ID错误，保存CSV文件头错误！";
		   }
		   lDistanceToMove += dwBytesReturned;

		   //其他行的内容
		   //5~10行保留
		   for (int i=5;i<10;i++)
		   {
			   strTmptValue=_T("Reserved\n");
			   dwBytesWrite=strTmptValue.GetLength() * sizeof(char);
			   WriteFile(hCSVFile,strTmptValue,dwBytesWrite,&dwBytesReturned,NULL);
			   if (dwBytesWrite!=dwBytesReturned)
			   {
				   throw"写入CSV错误！";
			   }
			   lDistanceToMove += dwBytesReturned;
		   }
		   
		   //写入数据(ROW 11)
		   strTmptValue.Format("Channel,X_axis,Y_axis,MAX Power\n");
		   dwBytesWrite=strTmptValue.GetLength() * sizeof(char);
		   WriteFile(hCSVFile,strTmptValue,dwBytesWrite,&dwBytesReturned,NULL);
		   if (dwBytesWrite != dwBytesReturned)
		   {
			   throw"Function,写入csv失败";
			   lDistanceToMove += dwBytesReturned;
		   }
            
		   //设置文件大小
		   if (lFileSize!=NULL)
		   {
			   *lFileSize = lDistanceToMove;
		   }
		   
		   bFunctionOK = TRUE;

	   }
	   catch(TCHAR* ptszErrorMsg)
	   {
	       MessageBox(ptszErrorMsg,"Write File Error",MB_OK|MB_ICONERROR);
		   return bFunctionOK;
	   }

	   return bFunctionOK;
}

BOOL CMy126S_45V_Switch_AppDlg::FindFileExist(LPCTSTR lpstrFileFullName)
{
   CString strErrMsg;
   HANDLE hResultFile = INVALID_HANDLE_VALUE;
   WIN32_FIND_DATA winFindFileData;
   
   hResultFile =  FindFirstFile(lpstrFileFullName,&winFindFileData);

   //find the exist file
   if (hResultFile!= INVALID_HANDLE_VALUE)
   {
	   FindClose(hResultFile);
	   return TRUE;	  
   }
   else
   { 
	   FindClose(hResultFile);
       return FALSE;
   }
}

void CMy126S_45V_Switch_AppDlg::OnButtonSetChannel() 
{
	// TODO: Add your control notification handler code here

	int      nChannel;
	CString  strMsg;
	
	nChannel = m_cbChannel.GetCurSel() + 1;
	//切换开关
	if(!SWToChannel(nChannel))
	{
	   strMsg.Format("切换到通道%d失败！",nChannel);
	   LogInfo(strMsg,FALSE);
	}
	else
	{
		strMsg.Format("切换到通道%d成功！",nChannel);
	    LogInfo(strMsg,TRUE);
	}
 
}

void CMy126S_45V_Switch_AppDlg::OnButtonPdlTest() 
{
	// TODO: Add your control notification handler code here
	CString strChannal,strMsg,strButton;
	int i=0;
	double MaxdblPower;
	double MindblPower;
	char   strPDL[10] ;
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
//	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;
	m_bTestStop = FALSE;

	ZeroMemory(strPDL,sizeof(char)*10);

	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	} 
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}

	//设置路径
	UpdateData();
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);

	m_nChannel = m_cbChannel.GetCurSel();
	if (m_nChannel==0)
	{
		MessageBox("通道选择错误！");
		return;
	}

	//按钮变为“停止监控”
    if (!m_bPDLStop)
    {
		if(!SWToChannel(m_nChannel))
		{
			strMsg.Format("开关切换到通道%d错误！",m_nChannel);
			MessageBox(strMsg);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}
		Sleep(300);
    	strMsg.Format("开始通道%d的PDL测试！",m_nChannel);
		MessageBox(strMsg);

		m_NTGraph.ClearGraph();
		m_NTGraph.SetXLabel("次数");
		m_NTGraph.SetYLabel("Power(dBm)");
		m_NTGraph.SetRange(0,200,-90,10);
		m_NTGraph.SetXGridNumber(20);
	    m_NTGraph.SetYGridNumber(10);

		SetDlgItemText(IDC_BUTTON_PDL_TEST,"停止测试");
		m_bPDLStop=TRUE;
        UpdateWindow();
		SetTimer(1, 300, NULL);
		return;
    }
	else 
	{
		m_iCountPDL = m_iReadPMCount;
		m_iReadPMCount = 0;
		SetDlgItemText(IDC_BUTTON_PDL_TEST,"PDL测试");
		m_bPDLStop=FALSE;
		LogInfo("测试完毕！");
		KillTimer(1);
	}

	//计算PDL
	MaxdblPower = m_dblPDLData[0];
	MindblPower = m_dblPDLData[0];
	for (int j = 1; j < m_iCountPDL; j++)
	{
		if (m_dblPDLData[j] >= MaxdblPower)
		{
			MaxdblPower = m_dblPDLData[j];
		}
		if (m_dblPDLData[j] <= MindblPower)
		{
			MindblPower = m_dblPDLData[j];
		}
	}

	ChannalMessage[m_nChannel-1].dblPDL = MaxdblPower - MindblPower;
	strMsg.Format("通道%d的PDL值为：%.2f dB",m_nChannel,ChannalMessage[m_nChannel-1].dblPDL);
	if (!CheckParaPassOrFail(PDL_DATA,ChannalMessage[m_nChannel-1].dblPDL))
	{
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
	}
	else
	{
		LogInfo(strMsg);
	}

	SaveDataToCSV(PDL_DATA);
	UpdateATMData(PDL_DATA);
}

void CMy126S_45V_Switch_AppDlg::OnButtonWdlTest() 
{
	// TODO: Add your control notification handler code here
	double dblTLSPower=0.0;
	double dblStartWL1=0;
	double dblENDWL1=0;
	double dblPower=0.0;
	double step1=0;
	double dblStartWL2=0;
	double dblENDWL2=0;
	double step2=0;
	int count,count1;
    double dblWDLPower[1000];
    double dblMIN;
	double dblMAX;
	int  nChannel;
	CString strMsg; 
	double dbl;
	CString strZeroFile;
	CStdioFile stdZeroFile;
//	LPTSTR    lpStr;
//	char    ReadValue[256];
	CString  strToken;
    char     sep[] = ",\t\n";
    m_bTestStop = FALSE;

	CString strIniFile;
	CString strValue;
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
//	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;

    ZeroMemory(dblWDLPower,sizeof(dblWDLPower));

	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先设置光功率计参数！");
		return;
	}
	//设置路径
	UpdateData();
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);

	if (!m_bOpen8164)
	{
		MessageBox("请先打开设备！");
		return;
	}

	strIniFile.Format("%s\\%s-%s\\WDLSET.INI", m_strPNConfigPath,m_strSpec,m_strPN);
	DWORD nLen1 = GetPrivateProfileString("WDL1","STARTWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen1 <= 0)
		MessageBox("读取配置文件错误！");
    dblStartWL1 = atof(strValue);
	
	DWORD nLen2 = GetPrivateProfileString("WDL1","STOPWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen2 <= 0)
		MessageBox("读取配置文件错误！");
    dblENDWL1 = atof(strValue);
	
	DWORD nLen3 = GetPrivateProfileString("WDL1","STEP",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen2 <= 0)
		MessageBox("读取配置文件错误！");
    step1 = atof(strValue);

	DWORD nLen4 = GetPrivateProfileString("WDL2","STARTWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen4 <= 0)
		MessageBox("读取配置文件错误！");
    dblStartWL2 = atof(strValue);
	
	DWORD nLen5 = GetPrivateProfileString("WDL2","STOPWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen5 <= 0)
		MessageBox("读取配置文件错误！");
    dblENDWL2 = atof(strValue);
	
	DWORD nLen6 = GetPrivateProfileString("WDL2","STEP",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen6 <= 0)
		MessageBox("读取配置文件错误！");
    step2 = atof(strValue);

	//初始化图形界面
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("波长(nm)");
	m_NTGraph.SetYLabel("功率值(dBm)");
	m_NTGraph.SetRange(dblStartWL1,dblENDWL2,-2,2);
	m_NTGraph.SetXGridNumber(10);
	m_NTGraph.SetYGridNumber(8);

	try
	{		
	    UpdateData();	
		nChannel = m_cbChannel.GetCurSel();
		if (nChannel==0)
		{
			MessageBox("通道选择错误！");
			return;
		}
		if(!SWToChannel(nChannel))
		{
			MessageBox("开关切换失败！");
			return;
		}
		Sleep(300);
		strMsg.Format("开始通道%d的WDL测试！",nChannel);
	    MessageBox(strMsg);		
		count = 0;
		for (dbl = dblStartWL1; dbl <= dblENDWL1; dbl += step1)
		{
			YieldToPeers();
            if (m_bTestStop)
			{
				MessageBox("测试中止！");
				//设置回中心波长
				if(!SetPWMWL(m_pdblWavelength,nChannel-1))
				{
					LogInfo("设置波长失败！");
				}
	        	SetTLSWavelength(m_pdblWavelength);
				return;
			}
			SetTLSWavelength(dbl);
			//设置光功率计波长
			if(!SetPWMWL(dbl,nChannel-1))
			{
				MessageBox("设置光功率计波长失败！");
				//设置回中心波长
				if(!SetPWMWL(m_pdblWavelength,nChannel-1))
				{
					LogInfo("设置波长失败！");
				}
		        SetTLSWavelength(m_pdblWavelength);
				return;
			}
			Sleep(300);
	//		for (nChannel=0;nChannel<12;nChannel++)
	//		{				
				dblPower = ReadPWMPower(nChannel-1);
				m_dblWDL[nChannel-1][count] = m_dblWDLRef[nChannel-1][count]-dblPower;
				strMsg.Format("通道%d:　波长:%.2f nm , IL:%.2f dB",nChannel,dbl,m_dblWDL[nChannel-1][count]);
				LogInfo(strMsg,TRUE);
		        count++;
	//		}						
		} 
		count1=count;
		for (dbl = dblStartWL2; dbl <= dblENDWL2; dbl += step2)
		{
			YieldToPeers();
            if (m_bTestStop)
			{
				MessageBox("测试中止！");
				//设置回中心波长
				if(!SetPWMWL(m_pdblWavelength,nChannel-1))
				{
					LogInfo("设置波长失败！");
				}
	        	SetTLSWavelength(m_pdblWavelength);
				return;
			}
			SetTLSWavelength(dbl);
			//设置光功率计波长
			if(!SetPWMWL(dbl,nChannel-1))
			{
				MessageBox("设置光功率计波长失败！");
				//设置回中心波长
				if(!SetPWMWL(m_pdblWavelength,nChannel-1))
				{
					LogInfo("设置波长失败！");
				}
		        SetTLSWavelength(m_pdblWavelength);
				return;
			}
			Sleep(300);			
			dblPower = ReadPWMPower(nChannel-1);
			m_dblWDL[nChannel-1][count] = m_dblWDLRef[nChannel-1][count]-dblPower;
			strMsg.Format("通道%d:　波长:%.2f nm , IL:%.2f dB",nChannel,dbl,m_dblWDL[nChannel-1][count]);
			LogInfo(strMsg,TRUE);
		    count++;					
		} 

		//设置回1550nm波长
        if(!SetPWMWL(m_pdblWavelength,nChannel-1))
		{
			LogInfo("设置波长失败！");
		}
		SetTLSWavelength(m_pdblWavelength);
		
		//计算WDL	
		dblMIN = m_dblWDL[nChannel-1][0];
		dblMAX = m_dblWDL[nChannel-1][0];
		for(int i=1;i<(count1-1);i++)
		{
			if (m_dblWDL[nChannel-1][i] <= dblMIN)
			{
				dblMIN = m_dblWDL[nChannel-1][i];
			}
			if (m_dblWDL[nChannel-1][i] >= dblMAX)
			{
				dblMAX = m_dblWDL[nChannel-1][i];
			}
		}
		ChannalMessage[nChannel-1].dblWDL[0] = dblMAX - dblMIN;//计算W
		strMsg.Format("短波通道%d的WDL为:%.2f dB",nChannel,ChannalMessage[nChannel-1].dblWDL[0]);
		LogInfo(strMsg);
		dblMIN = m_dblWDL[nChannel-1][count1];
		dblMAX = m_dblWDL[nChannel-1][count1];
		for(int i=count1;i<(count-1);i++)
		{
			if (m_dblWDL[nChannel-1][i] <= dblMIN)
			{
				dblMIN = m_dblWDL[nChannel-1][i];
			}
			if (m_dblWDL[nChannel-1][i] >= dblMAX)
			{
				dblMAX = m_dblWDL[nChannel-1][i];
			}
		}
		ChannalMessage[nChannel-1].dblWDL[1] = dblMAX - dblMIN;
		strMsg.Format("长波通道%d的WDL为:%.2f dB",nChannel,ChannalMessage[nChannel-1].dblWDL[1]);
		LogInfo(strMsg);
		//计算WDL	
		dblMIN = m_dblWDL[nChannel-1][0];
		dblMAX = m_dblWDL[nChannel-1][0];
		for(int i=1;i<(count-1);i++)
		{
			if (m_dblWDL[nChannel-1][i] <= dblMIN)
			{
				dblMIN = m_dblWDL[nChannel-1][i];
			}
			if (m_dblWDL[nChannel-1][i] >= dblMAX)
			{
				dblMAX = m_dblWDL[nChannel-1][i];
			}
		}
		ChannalMessage[nChannel-1].dblTotalWDL = dblMAX - dblMIN;//计zong算WDL	
		strMsg.Format("通道%d的WDL为:%.2f dB",nChannel,ChannalMessage[nChannel-1].dblTotalWDL);
		if(!CheckParaPassOrFail(WDL_DATA,ChannalMessage[nChannel-1].dblTotalWDL))
		{
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			LogInfo(strMsg);
		}	
		
		SaveDataToCSV(WDL_DATA,nChannel-1);
		UpdateATMData(WDL_DATA,nChannel-1); 				
	}
	catch (...)
	{
		MessageBox("请保存文件", "Error", MB_OK|MB_ICONERROR);
	}
}

void CMy126S_45V_Switch_AppDlg::OnButtonRepeat() 
{
	// TODO: Add your control notification handler code here
	/*
	if(!CreateSwitchMapBin("",0))
	{
		LogInfo("下载SwitchMapBin数据错误！",(BOOL)FALSE,COLOR_RED);
						
		return;
	}

	if(!CreateModuleMapBin("",0))
	{
		LogInfo("下载ModuleMapBin数据错误！",(BOOL)FALSE,COLOR_RED);
		return;
	}
	*/
}

BOOL CMy126S_45V_Switch_AppDlg::CreateDir()
{
	CString			 strMsg;
	HANDLE			 hDirectory = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	 winFindFileData;
	CString			 strDeviceDirectory;
	CString			 strLogFileName;
	
	// create the data directory
	strDeviceDirectory.Format("%s\\Data", m_strDirectory);
	hDirectory = FindFirstFile(strDeviceDirectory, &winFindFileData);
	
	if (INVALID_HANDLE_VALUE == hDirectory)
	{
		if (!CreateDirectory(strDeviceDirectory, NULL))
		{
			strMsg.Format ("无法建立DATA目录");
			AfxMessageBox(strMsg);			
			FindClose(hDirectory);
			return FALSE;
		}
	}
	else
	{
		FindClose(hDirectory);
	}
	
	// create the device SN directory
	strDeviceDirectory.Format("%s\\Data\\%s",  m_strDirectory, m_strSN);
	hDirectory = FindFirstFile(strDeviceDirectory, &winFindFileData);
	
	if (INVALID_HANDLE_VALUE == hDirectory)
	{
		if (!CreateDirectory(strDeviceDirectory, NULL))
		{
			strMsg.Format ("无法建立SN目录");
			AfxMessageBox(strMsg);		
			FindClose(hDirectory);	
			return FALSE;
		}
	}
	else
	{
		FindClose(hDirectory);
	}
	
	// create the ScanFile directory
	strDeviceDirectory.Format("%s\\Data\\%s\\ScanFile", m_strDirectory, m_strSN);
	hDirectory = FindFirstFile(strDeviceDirectory, &winFindFileData);
	
	if (INVALID_HANDLE_VALUE == hDirectory)
	{
		if (!CreateDirectory(strDeviceDirectory, NULL))
		{
			strMsg = _T("无法建立SN ScanFile目录");
			AfxMessageBox(strMsg);			
			FindClose(hDirectory);
			return FALSE;
		}
	}
	else
	{
		FindClose(hDirectory);
	}
	
	// create the OpticalData directory
	strDeviceDirectory.Format("%s\\Data\\%s\\OpticalData",  m_strDirectory,m_strSN);
	hDirectory = FindFirstFile(strDeviceDirectory, &winFindFileData);
	
	if (INVALID_HANDLE_VALUE == hDirectory)
	{
		if (!CreateDirectory(strDeviceDirectory, NULL))
		{
			strMsg = _T("无法建立SN OptialData目录");
			AfxMessageBox(strMsg);
			FindClose(hDirectory);
			return FALSE;
		}
	}
	else
	{
		FindClose(hDirectory);
	}
	
	// create log file
	strDeviceDirectory.Format("%s\\Data\\%s\\Log",  m_strDirectory, m_strSN);
	hDirectory = FindFirstFile(strDeviceDirectory, &winFindFileData);
	if (INVALID_HANDLE_VALUE == hDirectory)
	{
		if (!CreateDirectory(strDeviceDirectory, NULL))
		{
			strMsg = _T("无法建立SN Log目录");
			AfxMessageBox(strMsg);
			FindClose(hDirectory);
			return FALSE;
		}
	}
	else
	{
		FindClose(hDirectory);
	}
	
	return TRUE;
}

/*BOOL CMy126S_45V_Switch_AppDlg::SendMsgToSwitch(int nTime,int nCount)
{
	CString strTemp;
	char pbyData[20];
// 	BYTE bDir;
    char chReData[64];
	ZeroMemory(pbyData,sizeof(char)*20);
	ZeroMemory(chReData,sizeof(chReData));

	if(m_nCHCount==16)
	{
		strTemp.Format("ASWTH %d %d 32\r",nTime,nCount);
		if (m_nPMType[0] == PM_PDARRAY) 
		{
			strTemp.Format("ASWTH 1 %d %d 32\r",nTime,nCount);
		}
	}
	else
	{
		strTemp.Format("ASWTH %d %d %d\r",nTime,nCount,m_nCHCount);
		if (m_nPMType[0] == PM_PDARRAY) 
		{
			strTemp.Format("ASWTH 1 %d %d %d\r",nTime,nCount,m_nCHCount);
		}
	}
	
	memcpy(pbyData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		MessageBox("串口没有打开！");
		return FALSE;
	}
	Sleep(50);
    return TRUE;
}*/

/*BOOL CMy126S_45V_Switch_AppDlg::SETX(int x)
{
  CString strTemp;
  char pbyData[10];
  char pbyRx[20]; 
  CString dwBaseAddress;

  ZeroMemory(pbyData,sizeof(char)*10);
  ZeroMemory(pbyRx,sizeof(char)*20);

  x = x*MAX_DAC/60000.0;
   if (b_issn1) 
	{
		dwBaseAddress="00";
	}
	else if (b_issn2)
	{
		dwBaseAddress="01";
	}
	else if(b_issn3)
	{
		dwBaseAddress="02";
	}
	else
	{
		return FALSE;
	}
  //strTemp.Format("setx %d\r",x);
  if (m_nPMType[0] == PM_PDARRAY) 
  {
	  //strTemp.Format("setx 1 %d\r",x);
	  
  }
  memcpy(pbyData,strTemp,strTemp.GetLength());
  if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
  { 
     MessageBox("串口没打开，设置x电压错误！");
     return FALSE;
  }

  return TRUE;
    
}

BOOL CMy126S_45V_Switch_AppDlg::SETY(int y)
{
	CString strTemp;
	char pbyData[10];
	char pbyRx[20];  
	
	ZeroMemory(pbyData,sizeof(char)*10);
	ZeroMemory(pbyRx,sizeof(char)*20);
	y = y*MAX_DAC/60000.0;

	strTemp.Format("sety %d\r",y);
	if (m_nPMType[0] == PM_PDARRAY) 
	{
	  strTemp.Format("sety 1 %d\r",y);
	}
	memcpy(pbyData,strTemp,strTemp.GetLength());
	if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{ 
		MessageBox("串口没打开，设置y电压错误！");
		return FALSE;
	}

    return TRUE;
}

    

BOOL CMy126S_45V_Switch_AppDlg::SCHN(int nchannel,int x,int y)
{
	CString strTemp;
	char pbyData[30];
	char chReData[20];
	ZeroMemory(chReData,sizeof(char)*30);
	ZeroMemory(pbyData,sizeof(char)*20);

	strTemp.Format("schn %d %d %d\r",nchannel,x,y);
	memcpy(pbyData,strTemp,strTemp.GetLength());

    if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		MessageBox("串口未打开，通道设置错误！");
		return FALSE;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		MessageBox("通道设置错误，接收错误！");
		return  FALSE;
	}
	strTemp = chReData;
	if (strTemp != "\rI: Command OK!\r>")
	{
		MessageBox("通道设置错误，回读信息错误！");
		return FALSE;
	}
    return TRUE;
}*/

BOOL CMy126S_45V_Switch_AppDlg::SWToChannel(int nChannel,bool bSwitchTime)
{
	CString strTemp;
	char chData[10];
	char chReData[40];
	CString strValue;
	CString strInfo;
	int     nValue;
	UpdateData();

	if (m_cbComPort.GetCurSel()==1)
	{
		nChannel = nChannel + m_nCHCount; 
	}
//	double  dblValue;
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*40);
	
	strTemp.Format("sprt %d\r",nChannel);
	if (m_nPMType[0]==PM_PDARRAY)
	{
		strTemp.Format("sprt 1 %d\r",nChannel);
	}
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		MessageBox("串口打开错误，切换开关错误！");
    	return FALSE;
	}
	Sleep(100);
	if (!m_opCom.ReadBuffer(chReData,40))
	{
		MessageBox("开关切换错误，接收错误！");
		return FALSE;
	}
	if (bSwitchTime)
    {
		strValue = strtok(chReData,"\n\t\r");
		nValue = atoi(strValue);
		m_dblSwitchTime = (double)nValue/1000.0;
		strInfo.Format("通道切换时间为：%f ms",m_dblSwitchTime);
		LogInfo(strInfo);
    }
	return TRUE;
}


void CMy126S_45V_Switch_AppDlg::OnButtonExit() 
{
	// TODO: Add your control notification handler code here
	m_bTestStop = FALSE;
	CDialog::OnCancel();
}

/*
VOID CMy126S_45V_Switch_AppDlg::LogInfo(CString strFileName, LPCTSTR tszlogMsg, BOOL bAppend)
{
    FILE *pLogFile= NULL;
	CString strTemp;
	TCHAR tszAppFolder[MAX_PATH];

	if (""==strFileName)
	{
		GetCurrentDirectory(MAX_PATH,tszAppFolder);
		strTemp.Format("%s\\data\\TestInformation.txt",tszAppFolder);
		strFileName = strTemp;
	}
	if (bAppend)
         pLogFile = fopen(strFileName,"at");
	else
		 pLogFile = fopen(strFileName,"wt");

	if (!pLogFile)
	{
		return;
	}
    fprintf(pLogFile,"%s\n",tszlogMsg);
	fclose(pLogFile);
}
*/
void CMy126S_45V_Switch_AppDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default 
	
	CString strMsg;
	if (1 == nIDEvent)
	{
		double dblPDLIL;
		int nChannel;
		nChannel = m_cbChannel.GetCurSel();							
		dblPDLIL = ReadPWMPower(nChannel-1);											
		m_dblPDLData[m_iReadPMCount] = dblPDLIL;
		m_iReadPMCount++;

		m_NTGraph.PlotXY(m_iReadPMCount,dblPDLIL,0);
		
		if (m_iReadPMCount > 200||m_bTestStop)
		{
			m_iCountPDL = m_iReadPMCount;
			m_iReadPMCount = 0;
			SetDlgItemText(IDC_BUTTON_PDL_TEST,"PDL测试");
			m_bPDLStop=FALSE;
			LogInfo("测试完毕！");
			KillTimer(1);
		}
	}
	else if (2 == nIDEvent)
	{
		double dblPower;
		dblPower =  ReadPWMPower(m_nPreRefNum-1);
		if (dblPower<-20)  //此时判断为无光
		{
			m_bEndRef = TRUE;
			KillTimer(2);
		}
	}
	else if (3==nIDEvent)  //测试低温数据
	{
		double dblCurTempt;
		//测试高低温时需等待
		m_nSleepTime++;
		
		//
		SetTemptStatus(2); //设置低温
	    m_dblTargetTempt = -5;
		m_dblTargetTempt = m_dblTemperature[1];
		dblCurTempt = GetCurrentTempt();
		strMsg.Format("当前温度为：%.1f℃",dblCurTempt);
		LogInfo(strMsg);

		if (fabs(dblCurTempt-m_dblTargetTempt)<2&&m_nSleepTime>30)
		{
			strMsg.Format("当前温度为：%.1f℃，开始测试低温参数",dblCurTempt);
	    	LogInfo(strMsg);
			m_ctrlComboxTemperature.SetCurSel(1);
	     	UpdateWindow();
			KillTimer(3);
			OnButtonIlctTest();	
			m_nSleepTime = 0;
			m_NTGraph.ClearGraph();
			SetTimer(4,10000,NULL);		
		}
		m_NTGraph.PlotXY(m_nSleepTime,dblCurTempt,0);
	}
	else if (4==nIDEvent)  //测试低温数据
	{
		double dblCurTempt;
		//测试高低温时需等待
		m_nSleepTime++;		
		//
		SetTemptStatus(1); //设置高温
		m_dblTargetTempt = 70;
		if (stricmp(m_strSpec,"Z4767")==0)
		{
			m_dblTargetTempt = 85;
		}
		m_dblTargetTempt = m_dblTemperature[2];
		dblCurTempt = GetCurrentTempt();
		strMsg.Format("当前温度为：%.1f℃",dblCurTempt);
		LogInfo(strMsg);
		if (fabs(dblCurTempt-m_dblTargetTempt)<2&&m_nSleepTime>48)
		{
			strMsg.Format("当前温度为：%.1f℃，开始测试低温参数",dblCurTempt);
	    	LogInfo(strMsg);
			m_ctrlComboxTemperature.SetCurSel(2);
			UpdateWindow();
			KillTimer(4);
			OnButtonIlctTest();	
			SetTemptStatus(0); //设置回常温
		}
		m_NTGraph.PlotXY(m_nSleepTime,dblCurTempt,0);
	}
	 if (8==nIDEvent)
	 {
		 double dblReadPwr;
		 
		 CString strpow,strpowarry[20];

		 strpow="";
		 

		 if(m_pmcha == 0)
		 {
			for(int k =0 ;k<20;k++)
			{

				dblReadPwr = ReadPWMPower(k);
				strpowarry[k].Format("%.2f",dblReadPwr);
				Sleep(100);

			}
			strpow.Format("%s  %s  %s  %s  %s\n%s  %s  %s  %s  %s\n%s  %s  %s  %s  %s\n%s  %s  %s  %s  %s",strpowarry[15],strpowarry[16],strpowarry[17],strpowarry[18],strpowarry[19],strpowarry[10],strpowarry[11],strpowarry[12],strpowarry[13],strpowarry[14],strpowarry[5],strpowarry[6],strpowarry[7],strpowarry[8],strpowarry[9],strpowarry[0],strpowarry[1],strpowarry[2],strpowarry[3],strpowarry[4]);
			GetDlgItem(IDC_STATIC_POW)->SetWindowText(strpow);

		 }
		 else
		 {
			 dblReadPwr = ReadPWMPower(m_pmcha-1);

			 dblReadPwr = m_dblReferencePower[m_pmcha-1] - dblReadPwr;

			 strpow.Format("CH%d: %.2f",m_pmcha,dblReadPwr);

			 GetDlgItem(IDC_STATIC_POW)->SetWindowText(strpow);

		 }
		 

		 
	 }
	
	CDialog::OnTimer(nIDEvent);
}

VOID CMy126S_45V_Switch_AppDlg::YieldToPeers()
{
	MSG	msg;
	while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if(!AfxGetThread()->PumpMessage())
			break;
	}
}

void CMy126S_45V_Switch_AppDlg::OnButtonOpenDevice() 
{
	// TODO: Add your control notification handler code here
	//初始化设备
    DWORD    dwGPIBAddress1;   
	DWORD    dwGPIBAddress2;
	CString  strFile;
	CString  strWL;
	CString  strPower;
	CString  strValue;

	strFile.Format("%s\\%s\\device-14538.ini", m_strNetConfigPath,m_strPN);
	
	DWORD nLen1 = GetPrivateProfileString("TLS","WaveLength",
		NULL,strWL.GetBuffer(128),128,strFile);
	if (nLen1 <= 0)
			throw("读取配置信息失败！");
	m_pdblWavelength = atof(strWL);

	DWORD nLen2 = GetPrivateProfileString("TLS","OutPutPower",
		NULL,strPower.GetBuffer(128),128,strFile);
	if (nLen2 <= 0)
		throw("读取配置信息失败！");
	m_dblTLSPower = atof(strPower);

	DWORD nLen3 = GetPrivateProfileString("TLS","GPIB1",
		NULL,strValue.GetBuffer(128),128,strFile);
	if (nLen3 <= 0)
		throw("读取配置信息失败！");
	dwGPIBAddress1 = atof(strValue);

	DWORD nLen4 = GetPrivateProfileString("TLS","GPIB2",
		NULL,strValue.GetBuffer(128),128,strFile);
	if (nLen4 <= 0)
		throw("读取配置信息失败！");
	dwGPIBAddress2= atof(strValue);

	LogInfo("正在初始化设备...",TRUE);
	stDeviceSetInfo stTLSInfo1;
    stTLSInfo1.bHasDevice = TRUE;
	stTLSInfo1.nComType = GPIB;
    stTLSInfo1.deCaption = HP8164;
	stTLSInfo1.bLaserLow = 0;
	stTLSInfo1.deType = TLSDEVICE;
	stTLSInfo1.nAddress = dwGPIBAddress1;
	stTLSInfo1.nGPIBIndex = 0;
	m_HP816X1.InitialTLS(&stTLSInfo1);

	stDeviceSetInfo stTLSInfo2;
    stTLSInfo2.bHasDevice = TRUE;
	stTLSInfo2.nComType = GPIB;
    stTLSInfo2.deCaption = HP8164;
	stTLSInfo2.bLaserLow = 0;
	stTLSInfo2.deType = TLSDEVICE;
	stTLSInfo2.nAddress = dwGPIBAddress1;
	stTLSInfo2.nGPIBIndex = 0;
	m_HP816X2.InitialTLS(&stTLSInfo2);

	if(!m_HP816X1.OpenDevice())
	{
		MessageBox("打开激光器异常","设备连接错误！");
		return;
	}

	if(!m_HP816X1.SetTLSParameters(0,1,m_dblTLSPower,m_pdblWavelength))
	{
		 MessageBox("设置激光器参数失败！","error",MB_OK|MB_ICONERROR);
		 return ;
	}

	if(!m_HP816X2.OpenDevice())
	{
		MessageBox("打开激光器异常","设备连接错误！");
		return;
	}

	if(!m_HP816X2.SetTLSParameters(0,1,m_dblTLSPower,m_pdblWavelength))
	{
		 MessageBox("设置激光器参数失败！","error",MB_OK|MB_ICONERROR);
		 return ;
	}

    m_bOpen8164 = TRUE;
	LogInfo("打开TLS设备成功!");
}


void CMy126S_45V_Switch_AppDlg::OnButtonSetPM() 
{
	// TODO: Add your control notification handler code here
	int      nPMCount=0;
	CString  strFile;
	CString  strValue;
	CString  strSection;
	DWORD    dwIPAdress[4];
//	DWORD    dwGPIBAddress1,dwGPIBAddress2;   
	CString  strWL1,strWL2;
	CString  strPower;
	int      nTLSEnable;
	int nTLSCount;
	CString  strMsg;
	CString strTemp;
	
	strFile.Format("%s\\device-14538.ini", m_strNetConfigPath);
	strMsg.Format("开始获取配置文件%s",strFile);
	LogInfo(strMsg);
	
	DWORD nLen2 = GetPrivateProfileString("PM","PMCount",NULL,strValue.GetBuffer(128),128,strFile);
	if (nLen2 <= 0)
	{
		MessageBox("读取配置信息[PM]-PMCount失败！");
		return;
	}
	m_nPMCount = atoi(strValue);

	GetPrivateProfileString("TLS","enable","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		LogInfo("读取配置信息[TLS]-enable失败！");
		return;
	}
	nTLSEnable = atoi(strValue);

	GetPrivateProfileString("TLS","TLSCOUNT","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		LogInfo("读取配置信息[TLS]-TLSCOUNT失败！");
		return;
	}
	nTLSCount = atoi(strValue);

	DWORD dwLength=256;
	GetComputerName(m_chComputerName,&dwLength);

	if (!GetWorkStationConfig())
	{
		LogInfo("读取配置信息GetWorkStationConfig失败！");
		return;
	}

	if(OpenTLSDevice())
	{
		LogInfo("连接集成光源成功！");
	}
	else
	{
		LogInfo("连接集成光源错误！");
		return;
	}

	if (nTLSEnable!=0)
	{
	/*	char szBuffer[MAX_PATH];
		CString strTemp;
		GetCurrentDirectory(MAX_PATH,szBuffer);
		CString strConfigFile;
		strConfigFile.Format("%s\\config\\UDL.Config_%d.xml",g_tszAppFolder,nTLSCount);
		LogInfo(strConfigFile);

		if(g_pEngine!=NULL)
		{
			g_pTLS->CloseTLSDevice(0);
		}
		LogInfo("开始启动组件");

		::CoInitialize(NULL);
		HRESULT hr = g_pEngine.CreateInstance(__uuidof(EngineMgr));
		ASSERT( SUCCEEDED( hr ) );

		hr 	=g_pTLS.CreateInstance(__uuidof(ITLSInterface));
		ASSERT( SUCCEEDED( hr ) );//启动组件

		LogInfo("开始加载配置");
		if(g_pEngine->LoadConfiguration((_bstr_t)strConfigFile))
		{
			AfxMessageBox(g_pEngine->GetGetLastMessage());
			return;
		}
		
		LogInfo("开始打开设备");
		if(g_pEngine->OpenEngine())
		{
			AfxMessageBox(g_pEngine->GetGetLastMessage());
			return;
		}

		LogInfo("开始设置波长");
		if(!SetTLSWavelength(m_pdblWavelength))
		{
			strMsg.Format("设置激光器波长为%.2fnm失败！",m_pdblWavelength);
			LogInfo(strMsg,FALSE,COLOR_RED);
			return;
		}
		strMsg.Format("设置激光器波长为%.2fnm！",m_pdblWavelength);
		LogInfo(strMsg);
		*/
		
		LogInfo("连接TLS成功！");
	}
	//按通道分
	for (int i=0;i<m_nPMCount;i++)
	{
		strSection.Format("PM%d",i);
		GetPrivateProfileString(strSection,"PMType","error",strValue.GetBuffer(128),128,strFile);
		if (strValue == "error")
		{
			strMsg.Format("读取配置信息[%s]-PMType失败！",strSection);
			LogInfo(strMsg,FALSE,COLOR_RED);
	    	return;
		}
		m_nPMType[i] = atoi(strValue);

		GetPrivateProfileString(strSection,"CONNECT","error",strValue.GetBuffer(128),128,strFile);
		if (strValue == "error")
		{
			strMsg.Format("读取配置信息[%s]-CONNECT失败！",strSection);
			LogInfo(strMsg,FALSE,COLOR_RED);
			return;
		}
		m_nPMConnect[i] = atoi(strValue);
		
		GetPrivateProfileString(strSection,"PMAddress","error",strValue.GetBuffer(128),128,strFile);
		if (strValue == "error")
		{
			strMsg.Format("读取配置信息[%s]-PMAddress失败！",strSection);
			LogInfo(strMsg,FALSE,COLOR_RED);
			return;
		}

		if (m_nPMConnect[i]==0) //网口
		{
			sscanf(strValue,"%d.%d.%d.%d",&dwIPAdress[0],&dwIPAdress[1],&dwIPAdress[2],&dwIPAdress[3]);
	        m_nPMAddress[i] = dwIPAdress[0]*65536*256 + dwIPAdress[1]*65536 + dwIPAdress[2]*256+dwIPAdress[3];
			strMsg.Format("功率计%d,波长为：%.1fnm,类型为：%d,网口地址为：%d.%d.%d.%d",i+1,m_pdblWavelength,m_nPMType[i],
				dwIPAdress[0],dwIPAdress[1],dwIPAdress[2],dwIPAdress[3]);
		}
		else if (m_nPMConnect[i]==1)
		{
			m_nPMAddress[i] = atoi(strValue);
			strMsg.Format("功率计%d,波长为：%.1fnm,类型为：%d,串口地址为%d",i+1,m_pdblWavelength,m_nPMType[i],m_nPMAddress[i]);
		}
		LogInfo(strMsg);
	}
	//连接光功率计
	if (!ConnectPM())
	{
		LogInfo("连接PM错误！");
		return;
	}
 //   if(m_bChange)
//	{
		if (!SetPWMWL(m_pdblWavelength))
		{
			LogInfo("设置波长错误！");
			return;
		}
		m_bChange=FALSE;
//	}
	m_bOpenPM = TRUE;

    //连接1830C，用于测试RL
	int n1830CEnable;
	GetPrivateProfileString("1830C","enable","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		LogInfo("获取配置[1830C]-enable错误！");
		return;
	}
	n1830CEnable = atoi(strValue);

	int n1830CPort;
	if (n1830CEnable != 0)
	{
		GetPrivateProfileString("1830C","SerialPort","error",strValue.GetBuffer(128),128,strFile);
		if (strValue=="error")
		{
			LogInfo("获取device.ini配置1830C串口号错误！");
			return;
		}
		n1830CPort = atoi(strValue);
		LogInfo("启动1830C......");
		stDeviceSetInfo stPMInfo;
		stPMInfo.bHasDevice = TRUE;
		stPMInfo.nComType = SERIAL;
		stPMInfo.deCaption = PM1830C;
		stPMInfo.deType = PMDEVICE;
		stPMInfo.dwBaud = 9600;
		stPMInfo.nAddress = n1830CPort;
		m_1830CRL.InitialPM(&stPMInfo);
		if(!m_1830CRL.OpenDevice())
		{
			LogInfo("打开1830C异常");
			return;
		}
		
		if(!m_1830CRL.SetPWMParameters(0,1,1,m_pdblWavelength,0.01,0))
		{
			LogInfo("设置1830C参数失败！");
			return;
		}
		m_1830CRL.SetPWMUnit(0,1,0);
		strMsg.Format("1830C波长设置为：%.2fnm,串口号为：%d",m_pdblWavelength,n1830CPort);
		LogInfo(strMsg);
		m_bConnect1830C = TRUE;
	}

	//连接冷热盘，用于控制温度
	int nTemptEnable;
	int nTemptPort;
	CString strSerialPort;
	TCHAR tchSerialPort[20];
	ZeroMemory(tchSerialPort,sizeof(tchSerialPort));
	GetPrivateProfileString("TemptControl","enable","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		LogInfo("获取配置[TemptControl]-enable错误！");
		return;
	}
	nTemptEnable = atoi(strValue);
	 
	if (nTemptEnable!=0)
	{
		GetPrivateProfileString("TemptControl","SerialPort","error",strValue.GetBuffer(128),128,strFile);
		if (strValue=="error")
		{
			LogInfo("获取配置[TemptControl]-SerialPort错误！");
			return;
		}
		nTemptPort = atoi(strValue);
		if (nTemptPort>9)
		{
			strSerialPort.Format("\\\\.\\COM%d",nTemptPort);
		}
		else
		{
			strSerialPort.Format("COM%d",nTemptPort);
		}
		memcpy(tchSerialPort,strSerialPort,strSerialPort.GetLength());
		if(!m_opTempt.OpenPort(tchSerialPort,19200))
		{			
			strMsg.Format("打开串口%d错误！连接冷热盘失败",nTemptPort);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return ;
		}
		LogInfo("开始连接冷热盘，并初始化温度");
		//初始化冷热盘设置
		if(!SetTempToPoint(m_dblTemperature[1])) //设置低温温度
		{
			return;
		}
		strMsg.Format("设置冷热盘低温为%.1f℃成功！",m_dblTemperature[1]);
		LogInfo(strMsg);
		//LogInfo("设置冷热盘低温为-5℃成功！");
		//初始化冷热盘设置
		double dblTargetTemp=70;
		if (stricmp(m_strSpec,"Z4767")==0)
		{
			dblTargetTemp = 85;
		}
		dblTargetTemp = m_dblTemperature[2];
		if(!SetTempToPoint(dblTargetTemp)) //设置高温温度
		{
			return;
		}
		strMsg.Format("设置冷热盘高温为%.0f℃成功！",dblTargetTemp);
		LogInfo(strMsg);
		m_bConnectTemptControl = TRUE;
	}
	else
	{
		LogInfo("不设置冷热盘！");
	}
	LogInfo("连接设备成功！");
	MessageBox("连接设备成功！");

}

BOOL CMy126S_45V_Switch_AppDlg::SetR152WL(int nChannel,double dblWaveLength)
{
	BOOL bFunctionOK;
	unsigned char slot_id;
	unsigned char chan_id;
    int n;
	try
	{
		n = nChannel%4;
		slot_id = n/2;
		chan_id = n%2;
		if (nChannel >= 0 && nChannel <4)
		{
			//set_wl1(slot_id, chan_id, (int)(dblWaveLength*10000) );
		    do 
			{
			  set_wl1(slot_id, chan_id, (int)(dblWaveLength*10000) );
			} 
			while (slot_array[slot_id].module.pm.chan[chan_id].wl_value1 != dblWaveLength*10000);         
		}
		if (nChannel >= 4 && nChannel <8)
		{
			do 
			{
				set_wl2(slot_id, chan_id, (int)(dblWaveLength*10000) );
			} 
		   while (slot_array[slot_id].module.pm.chan[chan_id].wl_value2 != dblWaveLength*10000);
		}
		if (nChannel >= 8 && nChannel <12)
		{
			do 
			{
				set_wl3(slot_id, chan_id, (int)(dblWaveLength*10000) );
			} 
		   while (slot_array[slot_id].module.pm.chan[chan_id].wl_value3 != dblWaveLength*10000);
		}

		bFunctionOK = TRUE;
    }    
    catch(...)
	{
		MessageBox("设置光功率计波长失败！","error",MB_OK|MB_ICONERROR);
		bFunctionOK = FALSE;
	}

	return bFunctionOK;
}

double CMy126S_45V_Switch_AppDlg::ReadR152Power(int nChannel)
{
	unsigned char slot_id;
	unsigned char chan_id;
    int n;
	double dblR152Power;
	
	n = nChannel%4;
	slot_id = n/2;
	chan_id = n%2;
	
	if (nChannel >=0 && nChannel <= 3)
	{
        //WaitForSingleObject(hThread1,1000);
		dblR152Power = slot_array[slot_id].module.pm.chan[chan_id].pm_value1/10000.0;
		if ( slot_array[slot_id].module.pm.chan[chan_id].data_flag1 == DATA_OK )
		{
			return dblR152Power;
		}
		else
		{
			//MessageBox("Read PM ERROR!");
		}
		return dblR152Power;
	}
	
	if (nChannel >=4 && nChannel <= 7)
	{
		//WaitForSingleObject(hThread2,1000);
		dblR152Power = slot_array[slot_id].module.pm.chan[chan_id].pm_value2/10000.0;
		if ( slot_array[slot_id].module.pm.chan[chan_id].data_flag2 != DATA_OK )
		{
			//MessageBox("Read PM ERROR!");
		}
		else
		{
			return dblR152Power;
		}
	}
	
	if (nChannel >=8 && nChannel <= 11)
	{
		//WaitForSingleObject(hThread3,1000);
		dblR152Power = slot_array[slot_id].module.pm.chan[chan_id].pm_value3/10000.0;
		if ( slot_array[slot_id].module.pm.chan[chan_id].data_flag3 != DATA_OK )
		{
			//MessageBox("Read PM ERROR!");
		}
		else
		{
			return dblR152Power;
		}
	} 
	
	return -999;
}

void CMy126S_45V_Switch_AppDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_ctrlListMsg.ResetContent();
	int x=0;
	int y=0;
	//GetVoltageDataFromATMS("B5319606",12,&x,&y);
}

void CMy126S_45V_Switch_AppDlg::OnButtonAutoTest() 
{
	//================================
	//1、自动扫描
	//================================	

	//=========================================
	//2、测试IL及CT
	//=========================================
    if (!bAutoTestIL())
    {
		LogInfo("测试IL失败！",FALSE);
    }

	//=========================================
	//3、测试
	//=========================================

}


//=========================================
//测试IL及CT
//=========================================
BOOL CMy126S_45V_Switch_AppDlg::bAutoTestIL()
{
	CString strTemp;
	CString strChannel;
	double  dblPref=0;
	double  dblPout;
	char    strCT[10]={'\0'};
	char    strIL[10]={'\0'};
	int nchannel;
	
	//得到sheet2
//	m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t((short)2)));

	for (nchannel = 0; nchannel <12 ; nchannel++)
	{
		strTemp.Format("开始测试通道%d的IL及CT",nchannel+1);
		LogInfo(strTemp,TRUE);
		dblPref = m_dblReferencePower[nchannel];
				
		for (int i=0;i<12;i++)
		{
			SETX(ScanPowerChannel[i].VoltageX);
			SETY(ScanPowerChannel[i].VoltageY);
			dblPout = ReadPWMPower(i);
// 			if(!m_HP816X.ReadPower(PM_SLOT,PM_CHANNEL,&dblPower))
// 			{
// 				MessageBox("读取光功率错误!");
// 				return FALSE;
// 			}

			ChannalMessage[nchannel].dblCT[i] = TestIL(dblPref,dblPout);
			if (nchannel == i)
			{
				ChannalMessage[nchannel].dblIL=ChannalMessage[nchannel].dblCT[i];
				strTemp.Format("通道%d的IL为:%.2f dB",nchannel+1,ChannalMessage[nchannel].dblIL);
				LogInfo(strTemp,TRUE);
			}
			else
			{
				strTemp.Format("通道%d的CT为:%.2f dB",i+1,ChannalMessage[nchannel].dblCT[i]);
				LogInfo(strTemp,TRUE);
			}
			//保存CT
// 			_gcvt(ChannalMessage[nchannel].dblCT[i],10,strCT);
// 			m_range.AttachDispatch(m_sheet.GetCells(),true);
// 			m_range.SetItem(_variant_t((long)(nchannel+5)),_variant_t((long)(i+10)),_variant_t(strCT));
		}
		//转换为字符串
    
		_gcvt(ChannalMessage[nchannel].dblIL,10,strIL);
		//保存IL
// 		m_range.AttachDispatch(m_sheet.GetCells(),true);
// 		m_range.SetItem(_variant_t((long)(nchannel+5)),_variant_t((long)2),_variant_t(strIL));

	}
// 	m_book.Save();
// 	m_app.SetDisplayAlerts(false); //不弹出对话框询问是否保存
	LogInfo("IL/CT测试完毕!",TRUE);
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::CallScanFile()
{
	//得到sheet1
	CString strMsg;
// 	m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t((short)1)));
// 	for (int i = 0; i < 12; i++)
// 	{
// 		CString str;
// 		int VolueX,VolueY;
// 
// 		//获得X;
// 		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(i+5)),_variant_t((long)1)).pdispVal);
// 		m_covOptional = m_range.GetValue();
// 		//找到字符串
// 		if (m_covOptional.vt == VT_BSTR)
// 		{
// 			str = m_covOptional.bstrVal;
// 			VolueX = atoi(str);
// 		}
//         ScanPowerChannel[i].VoltageX =VolueX;
// 
//         //获得Y
// 		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(i+5)),_variant_t((long)2)).pdispVal);
// 		m_covOptional = m_range.GetValue();
// 		//找到字符串
// 		if (m_covOptional.vt == VT_BSTR)
// 		{
// 			str = m_covOptional.bstrVal;
// 			VolueY = atoi(str);
// 		}
// 	    ScanPowerChannel[i].VoltageY = VolueY;
// 		
// 		SCHN(i,VolueX,VolueY);
//         strMsg.Format("调用扫描数据，设置通道%d的电压坐标为(%d,%d)",i+1,VolueX,VolueY);
// 	}
}

void CMy126S_45V_Switch_AppDlg::OnButtonReadpower() 
{
	// TODO: Add your control notification handler code here
	
	int nCH;
	CR152Set R152Set;
	CString strInfo;
	nCH = m_cbChannel.GetCurSel();
	CString  strMsg;
	m_bTestStop = FALSE;

//power
	double   Power = 1;
	int      count = 0;

	while(1)
	{
		for (int i=0;i<12;i++)
		{
			YieldToPeers();
			if (m_bTestStop)
			{
				MessageBox("测试中止！");
				return;
			}
//			SWToChannel(i);
			//  SetPWMWL(0,1510);
			Power = ReadPWMPower(i);
			strMsg.Format("%.3f",Power);
			LogInfo(strMsg);
			Sleep(100);
		}
		count ++;
		if (count > 50)
		{
			break;
		}
	}

}

void CMy126S_45V_Switch_AppDlg::OnButtonRepeatTest() 
{
	// TODO: Add your control notification handler code here
	double dblPref;
	double dblPout;
	char   strRepeat[10];
	char   strDif[10];
	int nChannel;
	CString strMsg,strChannel;

    ZeroMemory(strRepeat,sizeof(char)*10);
	ZeroMemory(strDif,sizeof(char)*10);
	
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return;			
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先设置光功率计！");
		return;
	}
	if(!m_bHasRefForTest)
	{
        MessageBox("请先归零！");
		return;
	}
	OnButtonRepeat();
	if (!m_bChangeSwitch)
	{
		MessageBox("请先随机切换开关！");
		return;
	}
	//设置路径
	UpdateData();
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);

    m_ctrlListMsg.ResetContent();
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("通道");
	m_NTGraph.SetYLabel("重复性");
	m_NTGraph.SetRange(0,16,-5,5);
	m_NTGraph.SetXGridNumber(16);
	m_NTGraph.SetYGridNumber(10);

	//保存文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;

	if (m_cbComPort.GetCurSel()==0)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_Repeat_%s.csv", m_strNetFile,m_strPN,m_strSN,m_strSN,m_strItemName);
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_Repeat_%s-COM1.csv", m_strNetFile,m_strPN,m_strSN,m_strSN,m_strItemName);
	}
	else
	{

	}
	
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle!=INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s已经存在，是否覆盖数据？",strCSVFile);
		iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);		
		if (iSel==IDNO)
		{
			return;
		}
	}
	
	if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建数据文件失败！");
		return;
	}
	stdCSVFile.WriteString("Channel,IL,IL_Repeat,Diff\n");

    for (nChannel = 0; nChannel < m_nCHCount; nChannel ++)
    {
		SWToChannel(nChannel);
		Sleep(400);
		dblPref = m_dblReferencePower[nChannel];
		dblPout = ReadPWMPower(nChannel);
		
		ChannalMessage[nChannel].dblRepeat = TestIL(dblPref,dblPout);
		ChannalMessage[nChannel].dblDif = ChannalMessage[nChannel].dblIL-ChannalMessage[nChannel].dblRepeat;

		strMsg.Format("切换开关后,通道%d的IL为:%.2f,重复性为:%.2f",nChannel+1,ChannalMessage[nChannel].dblRepeat,ChannalMessage[nChannel].dblDif);	
		if (!CheckParaPassOrFail(RE_IL_DATA,ChannalMessage[nChannel].dblDif))
		{
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			LogInfo(strMsg,FALSE);
		}
		
		m_NTGraph.PlotXY(nChannel+1,ChannalMessage[nChannel].dblDif,0);

		strContent.Format("%d,%.3f,%.3f,%.3f\n",nChannel+1,ChannalMessage[nChannel].dblIL,ChannalMessage[nChannel].dblRepeat,ChannalMessage[nChannel].dblDif);
		stdCSVFile.WriteString(strContent);
		//保存重复性数据
    }	
	stdCSVFile.Close();

	SaveDataToExcel(RE_IL_DATA);
}


BOOL CMy126S_45V_Switch_AppDlg::bCoverFile(int nPara)
{
// 	int nCH;
// 	UpdateData();
// 	nCH = m_cbChannel.GetCurSel();
// 	COleVariant   vResult;
// 	m_range.AttachDispatch(m_sheet.GetCells(),true);
// 
//      switch (nPara)
//      {
//      case IL:
// 		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(nCH+5)),_variant_t((long)2)).pdispVal);
//        	vResult = m_range.GetValue();
// 		if (vResult.vt != VT_EMPTY)
// 		{
// 			return FALSE;
// 		}
//      	break;
// 	 case PDL:
//         m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(nCH+5)),_variant_t((long)3)).pdispVal);
// 		vResult = m_range.GetValue();
// 		if (vResult.vt != VT_EMPTY)
// 		{
// 			return FALSE;
// 		}
//      	break;
// 	 case WDL:
// 		 m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(nCH+5)),_variant_t((long)4)).pdispVal);
// 		 vResult = m_range.GetValue();
// 		 if (vResult.vt != VT_EMPTY)
// 		 {
// 			 return FALSE;
// 		 }
//      	break;
// 	 case LOWIL:
// 		 m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(nCH+5)),_variant_t((long)5)).pdispVal);
// 		 vResult = m_range.GetValue();
// 		 if (vResult.vt != VT_EMPTY)
// 		 {
// 			 return FALSE;
// 		 }
//      	break;
// 	 case HIGHIL:
// 		 m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(nCH+5)),_variant_t((long)6)).pdispVal);
// 		 vResult = m_range.GetValue();
// 		 if (vResult.vt != VT_EMPTY)
// 		 {
// 			 return FALSE;
// 		 }
//      	break;
// 	 case REIL:
// 		 m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(nCH+5)),_variant_t((long)8)).pdispVal);
// 		 vResult = m_range.GetValue();
// 		 if (vResult.vt != VT_EMPTY)
// 		 {
// 			 return FALSE;
// 		 }
//      	break;
// 	 case CT:
// 		 m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(nCH+5)),_variant_t((long)10)).pdispVal);
// 		 vResult = m_range.GetValue();
// 		 if (vResult.vt != VT_EMPTY)
// 		 {
// 			 return FALSE;
// 		 }
//      	break;
// 	 default:
// 		 MessageBox("未知错误！");
// 		 return FALSE;
// 		 break;
//      }
	 return TRUE;
}

//nChannel:0~15
double CMy126S_45V_Switch_AppDlg::ReadPWMPower(int nChannel)
{
	double	dblPower;
	int     i1830CCount=0;
	int     i8163ACount=0;
	int     iOplinkPMCount=0;
	int     iN7744Count=0;
	int     i;
	int     iIndexOplink = 0;
	int     iIndexN7744 = 0;
	char    chSendBuf[256];
	char    chReadBuf[256];
	CString strSend;
	CString strToken;
	
	for (i=0;i<m_nPMCount;i++)
	{
		if (m_nPMType[i]==PM_8163A) 
		{
			i8163ACount++;
		}
		else if (m_nPMType[i]==PM_1830C)
		{
			i1830CCount++;
		}
		else if (m_nPMType[i]==PM_OPLINK)
		{
			iOplinkPMCount++;
		}
		else if (m_nPMType[i]==PM_N7744)
		{
			iN7744Count++;
		}
		else if (m_nPMType[i]==PM_PDARRAY)
		{
			ZeroMemory(chSendBuf,sizeof(chSendBuf));
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			strSend.Format("get power %d\r",nChannel+1);
			memcpy(chSendBuf,strSend,strSend.GetLength());

			if (!m_comPM[i].WriteBuffer(chSendBuf,strSend.GetLength()))
			{
				LogInfo("发送Get power错误！");
				dblPower=-99;
				return dblPower;
			}
			Sleep(50);
			if (!m_comPM[i].ReadBuffer(chReadBuf,256))
			{
				LogInfo("接收Get power错误！");
				dblPower=-99;
				return dblPower;
			}
			dblPower = atof(chReadBuf);
			dblPower = dblPower/100.0;
			return dblPower;			
		}
		else
		{
		}
	}
	
	if (nChannel<i8163ACount)
	{
		m_HP816X1.ReadPower(nChannel/2+1,nChannel%2,&dblPower);
	}
	else if (nChannel<(i1830CCount+i8163ACount))
	{
		m_PM1830C[nChannel].ReadPower(1,0,&dblPower);
	}
	else if (nChannel<(i1830CCount+i8163ACount+iOplinkPMCount*4))
	{
		iIndexOplink = (nChannel-(i1830CCount+i8163ACount))/4+(i1830CCount+i8163ACount);
		if ((nChannel-(i1830CCount+i8163ACount))/4==0)
		{
			if (m_nPMConnect[iIndexOplink]==0)
			{
				GetPowerFromIP(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/2,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
			else if (m_nPMConnect[iIndexOplink]==1) 
			{
				GetPowerFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/2,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
		}
		else if ((nChannel-(i1830CCount+i8163ACount))/4==1)
		{
			if (m_nPMConnect[iIndexOplink]==0)
			{
				GetPowerFromIP(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/6,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
			else if (m_nPMConnect[iIndexOplink]==1) 
			{
				GetPowerFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/6,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
		}
		else if ((nChannel-(i1830CCount+i8163ACount))/4==2)
		{
			if (m_nPMConnect[iIndexOplink]==0)
			{
				GetPowerFromIP(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/10,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
			else if (m_nPMConnect[iIndexOplink]==1) 
			{
				GetPowerFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/10,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
		}
		else if ((nChannel-(i1830CCount+i8163ACount))/4==3)
		{
			if (m_nPMConnect[iIndexOplink]==0)
			{
				GetPowerFromIP(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/14,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
			else if (m_nPMConnect[iIndexOplink]==1) 
			{
				GetPowerFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/14,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
		}
		else if ((nChannel-(i1830CCount+i8163ACount))/4==4)
		{
			if (m_nPMConnect[iIndexOplink]==0)
			{
				GetPowerFromIP(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/18,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
			else if (m_nPMConnect[iIndexOplink]==1) 
			{
				GetPowerFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/18,(nChannel-(i1830CCount+i8163ACount))%2,&dblPower);
			}
		}
		else
		{
			dblPower = -80;
		}	
	}
	else // N7744 
	{
		iIndexN7744 = (nChannel - (i1830CCount*1+i8163ACount*1+iOplinkPMCount*4))/4+iOplinkPMCount;
		int iCH = (nChannel - (i1830CCount*1+i8163ACount*1+iOplinkPMCount*4))%4;//0~3
        m_PMN7744[iIndexN7744].ReadPower(iCH+1,iCH+1,&dblPower);
		if (dblPower == -200)
		{
			m_PMN7744[iIndexN7744].ReadPower(iCH+1,iCH+1,&dblPower);
			Sleep(100);
		}
		if (dblPower == -200)
		{
			dblPower = -80;
		}
	}
	
	if (dblPower<-80)
	{
		dblPower = -80;
	}
	return dblPower;

}
BOOL CMy126S_45V_Switch_AppDlg::SetWLFromIP(int socket_fd, BYTE bSlot, BYTE bChannel, LONG lWL)
{

	char pchCmd[256];
	char recvBuf[256];
	WORD wSendLen;
	long lTempWL;

	UpdateData();
	ZeroMemory(pchCmd,256);
	sprintf(pchCmd,"SENS%d,CHAN%d:POW:WAV %dnm",bSlot,bChannel,lWL);

	wSendLen = strlen(pchCmd);
	send(socket_fd,pchCmd,wSendLen,0);

	Sleep(100);

	ZeroMemory(pchCmd,256);
	sprintf(pchCmd,"SENS%d,CHAN%d,POW:WAV?",bSlot,bChannel);

	wSendLen = strlen(pchCmd);
	send(socket_fd,pchCmd,wSendLen,0);

	ZeroMemory(recvBuf,256);
	recv(socket_fd,recvBuf,256,0);

	lTempWL = atol(recvBuf);
	lTempWL = (long)(lTempWL + 0.5);

	if (lTempWL != lWL)
	{
		return FALSE;
	}

	return TRUE;


}

BOOL CMy126S_45V_Switch_AppDlg::GetPowerFromIP(BYTE bBox,BYTE bSlot, BYTE bChannel, double *pdblPower)
{
	char pchCmd[256];
	char recvBuf[256];
	WORD wSendLen;
	int  count = 0;
	CString strMsg;
	int iSocket;
	
	UpdateData();

// 	if(bBox==0)
// 	{
// 		iSocket=socket_fd1;
// 	}
// 	else if(bBox==1)
// 	{
// 		iSocket=socket_fd2;
// 	}
// 	else
// 	{
// 		iSocket=socket_fd3;
// 	}
	iSocket = g_socket_fd[bBox];

	//设置超时等待3秒
	int TimeOut = 3000;
	if (::setsockopt(iSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&TimeOut,sizeof(TimeOut)) == SOCKET_ERROR)
	{
		LogInfo("发送超时！");
		return FALSE;
	}
	if (::setsockopt(iSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&TimeOut,sizeof(TimeOut)) == SOCKET_ERROR)
	{
		LogInfo("接收超时！");
		return FALSE;
	}

	
	ZeroMemory(pchCmd, 256);
	sprintf(pchCmd, "SENS%d,CHAN%d:POW:READ?", bSlot, bChannel);
	
	wSendLen = strlen(pchCmd);
	send(iSocket,pchCmd,wSendLen,0);
	
	ZeroMemory(recvBuf, 256);
	recv(iSocket,recvBuf,256,0);
	//	LogInfo(recvBuf);

	*pdblPower = atof(recvBuf);
	
	return TRUE;
}

//nChannel
BOOL CMy126S_45V_Switch_AppDlg::SetPWMWL(int nChannel,double dblWL)
{
	BYTE bSlot;
	BYTE bChannel;
	long lWL;
	
	lWL = (long)(dblWL + 0.5);
	
    if (nChannel >= 0 && nChannel < 4)
    {
		if(g_stPMCfg.iType[0]==TYPE_OPLINK_PM)
		{
			bSlot = nChannel/2;
			bChannel = nChannel%2;
			if(!SetWLFromIP(socket_fd1,bSlot,bChannel,lWL))
			{
				LogInfo("设置波长错误！");
				return FALSE;
			}
		}
		else if(g_stPMCfg.iType[0]==TYPE_N7744)
		{
			if(!m_PMN7744[0].SetPWMWavelength(nChannel+1,nChannel+1,dblWL))
			{
				LogInfo("设置N7744波长错误！");
				return FALSE;
			}
		}
		else
		{
			if(!m_PM1830C[nChannel].SetPWMWavelength(1,0,dblWL))
			{
				LogInfo("设置1830C波长错误！");
				return FALSE;
			}
		}
    }
	else if (nChannel >= 4 && nChannel < 8)
	{
		if(g_stPMCfg.iType[1]==TYPE_OPLINK_PM)
		{
			nChannel = nChannel%4;
			bSlot = nChannel/2;
			bChannel = nChannel%2;
			if(!SetWLFromIP(socket_fd2,bSlot,bChannel,lWL))
			{
				LogInfo("设置波长错误！");
				return FALSE;
			}
		}
		else if(g_stPMCfg.iType[1]==TYPE_N7744)
		{
			if(!m_PMN7744[1].SetPWMWavelength(nChannel-3,nChannel-3,dblWL))
			{
				LogInfo("设置N7744波长错误！");
				return FALSE;
			}
		}
		else
		{
			if(!m_PM1830C[nChannel].SetPWMWavelength(1,0,dblWL))
			{
				LogInfo("设置1830C波长错误！");
				return FALSE;
			}
		}

	}
	else if (nChannel >= 8 && nChannel < 12)
	{
		if(g_stPMCfg.iType[2]==TYPE_OPLINK_PM)
		{
			nChannel = nChannel%4;
			bSlot = nChannel/2;
			bChannel = nChannel%2;
			if(!SetWLFromIP(socket_fd3,bSlot,bChannel,lWL))
			{
				LogInfo("设置波长错误！");
				return FALSE;
			}
		}
		else if(g_stPMCfg.iType[2]==TYPE_N7744)
		{
			if(!m_PMN7744[2].SetPWMWavelength(nChannel-7,nChannel-7,dblWL))
			{
				LogInfo("设置N7744波长错误！");
				return FALSE;
			}
		}
		else
		{
			if(!m_PM1830C[nChannel].SetPWMWavelength(1,0,dblWL))
			{
				LogInfo("设置1830C波长错误！");
				return FALSE;
			}
		}
	} 
    return TRUE;
}

// void CMy126S_45V_Switch_AppDlg::CreateExcelFile()
// {
// 
// // 	启动EXCEL服务器
// // 		COleVariant  m_covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR);
// // 		if (!m_app.CreateDispatch("EXCEL.Application",NULL))
// // 		{
// // 			AfxMessageBox("无法创建EXCEL应用！");
// // 			bStartExcel = FALSE;
// // 			return ;
// // 		}
// // 		
// // 		//允许其他用户控制EXCEL
// // 		m_app.SetUserControl(TRUE);
// // 		UpdateData();
// // 		//打开EXCEL文件	
// // 		//copy template
// // 		m_strOpticalDataPath.Format("%s\\data\\%s\\%s\\%s_Test_Data.xls",g_tszAppFolder,m_strPN,m_strSN,m_strSN);
// // 		m_strTemplatePath.Format("%s\\template\\Optical_Test_Data.xls",g_tszAppFolder);
// // 	
// // 	    HANDLE             hDictory = INVALID_HANDLE_VALUE;
// // 		WIN32_FIND_DATA    win32FindData;
// // 		CString            strMsg;
// // 		int                iSel;
// // 	
// // 		hDictory = FindFirstFile(m_strOpticalDataPath,&win32FindData);
// // 		if (hDictory != INVALID_HANDLE_VALUE)
// // 		{
// // 			strMsg.Format("查询到文件%s已经存在\n是否删除原来的数据？\n",m_strOpticalDataPath);
// // 			iSel = MessageBox(strMsg,"确认",MB_YESNO|MB_ICONQUESTION);
// // 		}
// // 	
// // 		if (iSel == IDYES)
// // 		{
// // 			DeleteFile(m_strOpticalDataPath);
// // 		}
// // 		else if (iSel == IDNO)
// // 		{
// // 			//打开EXCEL
// // 			m_books.AttachDispatch(m_app.GetWorkbooks(),true);
// // 			m_lpDisp=m_books.Open(m_strOpticalDataPath,m_covOptional,m_covOptional,m_covOptional,m_covOptional,
// // 				m_covOptional,m_covOptional,m_covOptional,m_covOptional,m_covOptional,m_covOptional,
// // 				m_covOptional,m_covOptional);
// // 			ASSERT(m_lpDisp);
// // 			m_book.AttachDispatch(m_lpDisp);
// // 			m_sheets.AttachDispatch(m_book.GetWorksheets(),true);
// // 			
// // 			m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t((short)(1))));
// // 		    m_range.AttachDispatch(m_sheet.GetCells(),true);
// // 	
// // 			bStartExcel = TRUE;
// // 			return;
// // 		}
// // 	
// // 		CopyFile(m_strTemplatePath,m_strOpticalDataPath,FALSE);
// // 		
// // 		//打开EXCEL
// // 		m_books.AttachDispatch(m_app.GetWorkbooks(),true);
// // 		m_lpDisp=m_books.Open(m_strOpticalDataPath,m_covOptional,m_covOptional,m_covOptional,m_covOptional,
// // 			m_covOptional,m_covOptional,m_covOptional,m_covOptional,m_covOptional,m_covOptional,
// // 			m_covOptional,m_covOptional);
// // 		ASSERT(m_lpDisp);
// // 		m_book.AttachDispatch(m_lpDisp);
// // 		m_sheets.AttachDispatch(m_book.GetWorksheets(),true);
// // 	
// // 		//填入基本信息到表1
// // 		m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t((short)(1))));
// // 		m_range.AttachDispatch(m_sheet.GetCells(),true);
// // 		
// // 		//保存产品信息
// // 		GetDlgItem(IDC_EDIT_ID)->GetWindowText(m_strID);
// // 		m_range.AttachDispatch(m_sheet.GetRange(_variant_t("B3"),_variant_t("B3")),true);
// // 		m_range.SetValue2(_variant_t(m_strID));
// // 		
// // 		GetDlgItem(IDC_EDIT_SN)->GetWindowText(m_strSN);
// // 		m_range.AttachDispatch(m_sheet.GetRange(_variant_t("B1"),_variant_t("B1")),true);
// // 		m_range.SetValue2(_variant_t(m_strSN));
// // 		
// // 		GetDlgItem(IDC_EDIT_PN)->GetWindowText(m_strPN);
// // 		m_range.AttachDispatch(m_sheet.GetRange(_variant_t("B2"),_variant_t("B2")),true);
// // 		m_range.SetValue2(_variant_t(m_strPN));
// // 		
// // 		//填入基本信息到表2
// // 		m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t((short)(2))));
// // 		m_range.AttachDispatch(m_sheet.GetCells(),true);
// // 		
// // 		//保存产品信息
// // 		GetDlgItem(IDC_EDIT_ID)->GetWindowText(m_strID);
// // 		m_range.AttachDispatch(m_sheet.GetRange(_variant_t("B3"),_variant_t("B3")),true);
// // 		m_range.SetValue2(_variant_t(m_strID));
// // 		
// // 		GetDlgItem(IDC_EDIT_SN)->GetWindowText(m_strSN);
// // 		m_range.AttachDispatch(m_sheet.GetRange(_variant_t("B1"),_variant_t("B1")),true);
// // 		m_range.SetValue2(_variant_t(m_strSN));
// // 		
// // 		GetDlgItem(IDC_EDIT_PN)->GetWindowText(m_strPN);
// // 		m_range.AttachDispatch(m_sheet.GetRange(_variant_t("B2"),_variant_t("B2")),true);
// // 		m_range.SetValue2(_variant_t(m_strPN));
// // 		m_book.Save();
// // 		m_app.SetDisplayAlerts(false);
// // 			
// // 		LogInfo("保存产品信息成功！",TRUE); 
// // 		
// 		bStartExcel = TRUE;
// }

void CMy126S_45V_Switch_AppDlg::OnButtonSendMessage() 
{
	// TODO: Add your control notification handler code here
	int   nCommand = 0;
	DWORD dwFeedBackLength;
	BYTE  ptCommand[MAX_COUNT];
	//	DWORD dwFeedBackLength;
	DWORD dwWaitTime = 100;
	DWORD dwLength = 0;
	CFileDialog dlgFilePath(TRUE);
	CString     strFile,strBinFileName;
	CString     strMsg;
	HANDLE      hBinFile = INVALID_HANDLE_VALUE;
	CString     strCommand;
	char        chSetCommand[MAX_LINE];
	char        chReadCommand[MAX_COUNT];
	BYTE    bytValue1 = 0;
	BYTE    bytValue2 = 0;
	CString strToken;
	CString m_strchan;
	char    sep[] = "\r\n";
	int     nCount=0;
	int x;
	int xaz;
		int  y;
	//	int     nSwitch;
	int     nPort;
	BOOL bSet;
//	int i;
	double dblTemp;
	BYTE     pbySendCommand[MAX_LINE];
	CString dwBaseAddress,strdacx,strdacy;
	
	ZeroMemory(chSetCommand,sizeof(char)*MAX_LINE);
	ZeroMemory(chReadCommand,sizeof(char)*MAX_COUNT);
	ZeroMemory(ptCommand,sizeof(BYTE)*MAX_COUNT);
	ZeroMemory(pbySendCommand,sizeof(pbySendCommand));
	
	UpdateData();
	if (!m_bOpenPort)
	{
		MessageBox("请先打开对应的串口！");
		return;
	}
    
	nCommand = m_cbMessage.GetCurSel();
	//默认值
	switch (nCommand)
	{
	case SET_RESET:
		bSet = SetRSET();
		if(!bSet)
		{
			LogInfo("软复位失败！",TRUE);
		}
		return;
	case OPLINK_VERSION:
		/*strCommand.Format("OPLK\r");
		if (m_nPMType[0] == PM_PDARRAY) 
		{
			strCommand.Format("OPLK\r");
		}*/
		strMsg = GetFWVer();
		LogInfo(strMsg,TRUE);
		return;
	case SETX_COMMAND:

		if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return;
		}

		m_dblX=(int)(m_dblX*MAX_DAC/60000.0);
        m_dblY=(int)(m_dblY*MAX_DAC/60000.0);
		x = m_dblX;
		y = m_dblY;
		if( !SetTesterXYDAC(atoi(dwBaseAddress),x, y) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return;
		}
		//m_strErrorMsg = "设置DAC成功";
		LogInfo("设置DAC成功!");
		return;
	case DOWNLOAD_FW:	
		DownLoadFW();
		return;
	case GET_TEMP:	
		dblTemp = GetModuleTemp();
		strMsg.Format("%.1f",dblTemp);
		LogInfo(strMsg,TRUE);
		return;
	case SWITCH_RANDOM:
		if(m_nCHCount==16)
		{
			 strCommand.Format("ASWTH %d %d 32\r",(int)m_dblX,(int)m_dblY);
		}
		else
		{
			 strCommand.Format("ASWTH %d %d %d\r",(int)m_dblX,(int)m_dblY,m_nCHCount);
		}
		break;
	case CHANNEL_SHOW:
		GetChannelInfo();
		return;
	default:
		return;
	}
	YieldToPeers();
	//发送命令
	memcpy(chSetCommand,strCommand,strCommand.GetLength());
	
	//if (!m_opCom.WriteBuffer(chSetCommand,strCommand.GetLength()))
//	{
//		LogInfo("串口通讯错误！",(BOOL)FALSE,COLOR_RED);
//		return;
//	}
//	Sleep(dwWaitTime);	

	LogInfo("命令发送成功",TRUE);

#if 1
//	if (!m_opCom.ReadBuffer(chReadCommand,MAX_COUNT))
//	{
//		LogInfo("读取数据错误！",(BOOL)FALSE,COLOR_RED);
//		return;
//	}
//	LogInfo(chReadCommand,TRUE,COLOR_BLUE);
#else
	DWORD dwPreTimeStamp;
	DWORD dwCurTimeStamp;
	dwCurTimeStamp = GetTickCount();
	dwPreTimeStamp = dwCurTimeStamp;
	while((dwCurTimeStamp - dwPreTimeStamp) < 200)
	{
		dwCurTimeStamp = GetTickCount();
		YieldToPeers();
		if (!m_opCom.ReadBuffer(chReadCommand,64))
		{
			continue;
		}
		LogInfo(chReadCommand,TRUE);
		dwPreTimeStamp = dwCurTimeStamp;
	}
#endif
}

void CMy126S_45V_Switch_AppDlg::OnSelchangeComboSendMessage() 
{
	// TODO: Add your control notification handler code here
 	UpdateData();
 	int nChannel; 
 	nChannel = m_cbMessage.GetCurSel();
// 
 	GetDlgItem(IDC_STATIC_CHANNEL)->ShowWindow(FALSE);
 	GetDlgItem(IDC_EDIT_CHANNEL)->ShowWindow(FALSE);
 	GetDlgItem(IDC_EDIT_CHANNEL2)->ShowWindow(FALSE);
 	GetDlgItem(IDC_STATIC_X)->ShowWindow(FALSE);
 	GetDlgItem(IDC_EDIT_X)->ShowWindow(FALSE);
 	GetDlgItem(IDC_STATIC_Y)->ShowWindow(FALSE);
 	GetDlgItem(IDC_EDIT_Y)->ShowWindow(FALSE);
 	GetDlgItem(IDC_STATIC_UNIT1)->SetWindowText("mv");
 	GetDlgItem(IDC_STATIC_UNIT1)->ShowWindow(FALSE);
 	GetDlgItem(IDC_STATIC_UNIT2)->ShowWindow(FALSE);

// 
	switch (nChannel)
	{
	case SWITCH_TIME:
	case SET_OPTICAL_ROUTE:
		GetDlgItem(IDC_STATIC_CHANNEL)->ShowWindow(TRUE);
 	    GetDlgItem(IDC_EDIT_CHANNEL)->ShowWindow(TRUE);
	    break;
	case GET_ALARM:
		GetDlgItem(IDC_STATIC_CHANNEL)->ShowWindow(TRUE);
 	    GetDlgItem(IDC_EDIT_CHANNEL)->ShowWindow(TRUE);
	    break;
	case SETX_COMMAND:
		SetDlgItemText(IDC_STATIC_X,"X");
		GetDlgItem(IDC_STATIC_X)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_X)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_UNIT1)->SetWindowText("mv");
		GetDlgItem(IDC_STATIC_UNIT1)->ShowWindow(TRUE);
		SetDlgItemText(IDC_STATIC_Y,"Y");
		GetDlgItem(IDC_STATIC_Y)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Y)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_UNIT2)->ShowWindow(TRUE);
		break;
	case SETY_COMMAND:
		GetDlgItem(IDC_STATIC_CHANNEL)->ShowWindow(TRUE);
 	    GetDlgItem(IDC_EDIT_CHANNEL)->ShowWindow(TRUE);
		break;
	case SWITCH_RANDOM:
		SetDlgItemText(IDC_STATIC_X,"周期");
		GetDlgItem(IDC_STATIC_X)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_X)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_UNIT1)->SetWindowText("ms");
		GetDlgItem(IDC_STATIC_UNIT1)->ShowWindow(TRUE);

		SetDlgItemText(IDC_STATIC_Y,"次数");
		GetDlgItem(IDC_STATIC_Y)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Y)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_UNIT2)->SetWindowText("次");
		GetDlgItem(IDC_STATIC_UNIT2)->ShowWindow(TRUE);

	}

}

 
//读取扫描文件
void CMy126S_45V_Switch_AppDlg::ReadZeroFile(double dblTLSPower)
{
	CString strFileName;
	CString strMsg;
	CString strToken;
	char    sep[] = ",\t\n";
	LPTSTR   lpstr = NULL;
	char    chLine[MAX_LINE];
	double  dblTLS = 0;
	double  dblZero = 0;
	int     nchannel;

	ZeroMemory(chLine,sizeof(char) * MAX_LINE);

	for (nchannel = 0;nchannel < 12;nchannel++)
	{
		strFileName.Format("%s\\data\\ZeroFile_%d.csv",g_tszAppFolder,nchannel+1);
		HANDLE hHandle = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA win32FindData;
		hHandle = FindFirstFile(strFileName,&win32FindData);
		if (hHandle == INVALID_HANDLE_VALUE)
		{
			strMsg.Format("发现文件%s不存在，请检查",strFileName);
			MessageBox(strMsg,"确认",MB_YESNO|MB_ICONQUESTION);
			return;
		}
		
		CStdioFile  stdFile;
		if(!stdFile.Open(strFileName,CFile::modeRead,NULL))
		{
			strMsg.Format("文件%s打开失败！",strFileName);
			MessageBox(strMsg);
			return;
		}
		
		while(1)
		{
			lpstr = stdFile.ReadString(chLine,MAX_LINE);
			if(lpstr == NULL)
			{
				MessageBox("读取值为空，请查看文件！");
				return;
			}
			strToken = strtok(chLine,sep);
			dblTLS = atof(strToken);
			strToken = strtok(NULL,sep);
			dblZero = atof(strToken);
			
			if (dblTLS == dblTLSPower)
			{
				m_dblReferencePower[nchannel] = dblZero;
				break;
			}
		}
		
	}
	 
}

void CMy126S_45V_Switch_AppDlg::OnUpdateEditMessage() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
}

//实现自动扫描
//输入：nchannel：0~11
//输出：扫描功率点
//0:x:-40~-30;y:25~45
//1:x:-25~-15;y:25~45
//2:x:15~25;y:25~45
//3:x:30~40:y:25~45
//4:x:30~40:y:-15~15
//5:x:15~25:y:-15~15
//6:x:-25~-15:y:-15~15
//7:x:-40~-30:y:-15~15
//8:x:-40~-30:y:-45~-30
//9:x:-25~-15:y:-45~-30
//10:x:15~25:y:-45~-30
//11:x:30~40:y:-45~-30
BOOL CMy126S_45V_Switch_AppDlg::AutoScan(int nChannel,CString strFileINI,BOOL bScanRange)
{
	int x;
	int y;
    int x_step ;
	int y_step ;
	int count = 0;
	int x_count = 0;
	int nCount = 0;
	int iCount = 0;
	double  dblPower = 0; 
	BOOL    bFlag1 = FALSE;
	BOOL    bFlag2 = FALSE;
	BOOL    bFlag3 = FALSE;
	BOOL    bFlag4 = FALSE;
	CString strMsg;
	int x_start = 0;
	int x_stop = 0;
	int y_start = 0;
	int y_stop = 0;
	double dblPrePower = 100;
	double dblPrePowerX = 100;
	double dblPrePowerY = 100;
    char    chValue[32];
	ZeroMemory(chValue,sizeof(char)*32);

	CString strINIFile;
	CString strValue;
	CString strStartX;
	CString strEndX;
	CString strStartY;
	CString strEndY;
	BOOL    bFindPoint = FALSE;
	m_bTestStop = FALSE;
	CString strSection;

//	if (m_bIfSaveToServe)
//	{
//		strINIFile.Format("%s\\config\\%s-%s\\%s",m_strNetConfigPath,m_strSpec,m_strPN,strFileINI);	
//	}
//	else
//	{
		strINIFile.Format("%s\\%s-%s\\%s", m_strPNConfigPath,m_strSpec,m_strPN,strFileINI);
//	}

//	if (m_cbComPort.GetCurSel()==1)
	{
		strSection.Format("CH%d",nChannel+1);
		DWORD nLen1 = GetPrivateProfileString(strSection,"x_start",
			NULL,strValue.GetBuffer(128),128,strINIFile);
		if (nLen1 <= 0)
		{
			MessageBox("读取配置信息失败！");
			return FALSE;
		}
		x_start = atoi(strValue);

		DWORD nLen2 = GetPrivateProfileString(strSection,"x_end",
			NULL,strValue.GetBuffer(128),128,strINIFile);
		if (nLen2 <= 0)
		{
			MessageBox("读取配置信息失败！");
	        return FALSE;	
		}
		x_stop = atoi(strValue);

		DWORD nLen3 = GetPrivateProfileString(strSection,"y_start",
			NULL,strValue.GetBuffer(128),128,strINIFile);
		if (nLen3 <= 0)
		{
			MessageBox("读取配置信息失败！");
			return FALSE;
		}
		y_start = atoi(strValue);
		
		DWORD nLen4 = GetPrivateProfileString(strSection,"y_end",
			NULL,strValue.GetBuffer(128),128,strINIFile);
		if (nLen4 <= 0)
		{
			MessageBox("读取配置信息失败！");
			return FALSE;
		}
		y_stop = atoi(strValue);

		DWORD nLen5 = GetPrivateProfileString("step","x_step",
			NULL,strValue.GetBuffer(128),128,strINIFile);
		if (nLen5 <= 0)
		{
			MessageBox("读取配置信息失败！");
			return FALSE;
		}
		x_step = atoi(strValue);
		
		DWORD nLen6 = GetPrivateProfileString("step","y_step",
			NULL,strValue.GetBuffer(128),128,strINIFile);
		if (nLen6 <= 0)
		{
			MessageBox("读取配置信息失败！");
			return FALSE;
		}
		y_step = atoi(strValue);
	}

	for (x = x_start;x<=x_stop;x=x+x_step)
	{
		for (y=y_start;y<=y_stop;y=y+y_step)
		{
			YieldToPeers();
			if (m_bTestStop)
			{
				LogInfo("测试中止!",(BOOL)FALSE,COLOR_RED);
				return FALSE;
			}
			
			if(!SETY(y))
			{
				LogInfo("设置Y电压错误！",(BOOL)FALSE,COLOR_RED);
				return FALSE;
			}
			Sleep(50);
			
			if(!SETX(x))
			{
				LogInfo("设置X电压错误！",(BOOL)FALSE,COLOR_RED);
				return FALSE;
			}
			Sleep(300);
			
			dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
            count = 0;
 			if (x == x_start)
			{
				//读取到x的第一个值时，需多次判断，已避免光功率计没有准确上报
				while (dblPower < 15 && nCount <3)
				{
					Sleep(400);
					dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
					nCount++;
				}
				if (dblPower == m_dblReferencePower[nChannel])
				{
					LogInfo("与光功率计通讯错误!",(BOOL)FALSE,COLOR_RED);
				    return FALSE;
				}

			}					
			count = 0;
			while (dblPower <= 0 && count < 3)
			{
				Sleep(200);
				dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
				count++;
			}
			if (dblPower <= 0)
			{
				LogInfo("光功率值过大！将取上一次的值！",(BOOL)FALSE,COLOR_RED);
				dblPower = dblPrePowerX;
			}
			dblPrePowerX = dblPower;

			m_NTGraph.PlotXY(x,y,0);
			strMsg.Format("%d,%d,IL,%.2f\n",x,y,dblPower);
			LogInfo(strMsg);

			if (dblPower<dblPrePower)
			{
				dblPrePower = dblPower;
				m_Xbase[nChannel] = x;
				m_Ybase[nChannel] = y;
			}
			if (dblPrePower < 20) 
			{
				return	TRUE;
			}
		}
	}
	strMsg.Format("通道%d没有扫描到IL小于20dB的点，请重新配置该通道扫描范围！",nChannel+1);
	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
	return FALSE;
}

//算法2：寻找基准点附近的点；
BOOL CMy126S_45V_Switch_AppDlg::AutoPointScan(int nChannel)
{
	int x;
	int y;
    int x_step = 2000;
	int x_step1 = 2000;
	int x_step2 = -2000;
	int y_step = 2000;
	int y_step1 = 2000;
	int y_step2 = -2000;
	CString strCommand;
	int XADC,YADC;
	m_bTestStop = FALSE;
	int count = 0;
	int x_count = 0;
	double  dblPower = 0; 
	BOOL    bFlag1 = FALSE;
	BOOL    bFlagX = FALSE;
	BOOL    bFlagY = FALSE;
	int iCount = 0;
	CString strMsg;
	int x_base = 0;
	int y_base = 0;
	int x_bbackup = 0;
	int y_bbackup = 0;
	double dblBasePower = 100;
	double dblBasePower_X;
	double dblBasePower_Y;
	double dblPrePowerX = 100;
	double dblPrePowerY = 100;
    char    chValue[10];
	CString strSendBuf;
	char chSendBuf[50];
	char chReadBuf[256];
	int xmin,xmax,ymin,ymax;
	CString m_cmd,dwBaseAddress,strdacx,strdacy,strdacxmin,strdacxmax,strdacymin,strdacymax;
	ZeroMemory(chValue,sizeof(char)*10);

	x_base = m_Xbase[nChannel];
	y_base = m_Ybase[nChannel];
	x_bbackup = x_base;
    y_bbackup = y_base;

	//导入扫描配置
	CString strINI;
	CString strValue;
	CString strName;
	CString strToken;
	double  Point1[6];
	int     step1[6];
	for (int i=0;i<6;i++)
	{
		strName.Format("Point%d",i+1);
		strINI.Format("%s\\%s-%s\\ScanConfig.ini", m_strPNConfigPath,m_strSpec,m_strPN);
		GetPrivateProfileString("PointScan",strName,"error",strValue.GetBuffer(128),128,strINI);
		if (strValue == "error")
		{
			LogInfo("读取配置ScanConfig错误！");
			return FALSE;
		}
		Point1[i] = atof(strValue);
		
		strName.Format("step%d",i+1);
		strINI.Format("%s\\%s-%s\\ScanConfig.ini", m_strPNConfigPath,m_strSpec,m_strPN);
		GetPrivateProfileString("PointScan",strName,"error",strValue.GetBuffer(128),128,strINI);
		if (strValue == "error")
		{
			LogInfo("读取配置ScanConfig错误！");
			return FALSE;
		}
		step1[i] = atoi(strValue);
	}

	double dblSetIL;
	GetPrivateProfileString("MIN_IL","IL","error",strValue.GetBuffer(128),128,strINI);
	if (strValue == "error")
	{
		dblSetIL=0.8;
	}
	dblSetIL= atof(strValue);
////////////////////////////////////////////////////////
	int  wScanMaxIndex;
	SHORT sDACx;
	SHORT sDACy;
	SHORT sScanMinVx;
	SHORT sScanMaxVx;
	SHORT sScanMinVy;
	SHORT sScanMaxVy;
	WORD  wStep = 16;
	WORD  wRowIndex;
	WORD  wColumnIndex;
	BOOL  bAdjust = FALSE;

	CString strConfigFile;
	int nBlock = 0;
	int nSwitch = 0;
	int nScanPoint;
	char pchRead[128];
	int nReadCount;
	ZeroMemory(pchRead, sizeof(pchRead));
//////////////////////////////////////////////////////////////
	if (m_nPMType[0]==PM_PDARRAY) //PD扫描盒
	{
		 if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}
//先细扫
		XADC=(x_base*MAX_DAC/60000.0);
		YADC=(y_base*MAX_DAC/60000.0);

		sDACx = XADC;
		sDACy = YADC;
		while (wStep)
		{
			sScanMinVx = sDACx - wStep * 15;
			sScanMaxVx = sDACx + wStep * 16;
			sScanMinVy = sDACy - wStep * 15;
			sScanMaxVy = sDACy + wStep * 16;
			if (sScanMinVx < -4095)
			{
				sScanMinVx = -4095;
				sScanMaxVx = sScanMinVx + wStep * 31;
			}
			else if (sScanMaxVx > 4095)
			{
				sScanMinVx = 4095 - wStep * 31;
				sScanMaxVx = 4095;
			}
			else
			{

			}
			if (sScanMinVy < -4095)
			{
				sScanMinVy = -4095;
				sScanMaxVy = sScanMinVy + wStep * 31;
			}
			else if (sScanMaxVy > 4095)
			{
				sScanMinVy = 4095 - wStep * 31;
				sScanMaxVy = 4095;
			}
			else
			{

			}
			nScanPoint = ((sScanMaxVx - sScanMinVx) / wStep + 1)*((sScanMaxVy - sScanMinVy) / wStep + 1);
	
			strMsg.Format("Point:%d", nScanPoint);
			LogInfo(strMsg);
			/*
			if (!m_OplinkPMFor20CH.StartMonitorPD(nPort + 1, nScanPoint))
			{
				strMsg = m_OplinkPMFor20CH.m_strErrorLog;
				LogInfo(strMsg, LOG_ERROR);
				return FALSE;
			}
			*/
			ZeroMemory(chSendBuf,sizeof(chSendBuf));
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			strSendBuf.Format("SCAN STARTADC %d %d\r",nChannel+1,nScanPoint);
			memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
			if(!m_comPM[0].WriteBuffer(chSendBuf,strSendBuf.GetLength()))
			{
				strMsg.Format("发送指令%s失败!",strSendBuf);
				LogInfo(strMsg);
				return FALSE;
			}
			Sleep(50);
			if (!m_comPM[0].ReadBuffer(chReadBuf,256))
			{
				strMsg.Format("接收指令%s失败!",strSendBuf);
				LogInfo(strMsg);
				return FALSE;
			}
			strMsg.Format("Scan:%d,%d,%d,%d,%d", sScanMinVx, sScanMaxVx, sScanMinVy, sScanMaxVy, wStep);
			LogInfo(strMsg);
			
			if(!SendProductScanZone(atoi(dwBaseAddress),sScanMinVx,sScanMaxVx,sScanMinVy,sScanMaxVy,wStep))
			{
				LogInfo("发送扫描指令失败!",FALSE,COLOR_RED);
				return FALSE;
			}

			for (int xloy = 1; xloy < 200 ;xloy ++)
			{
				Sleep(100);

				int nstate = GetScanState(atoi(dwBaseAddress));
				//RxDataFromI2C("0140"); //MemsSwitch Scan status
				if (nstate == 0)
				{
					break;
				}
				if (xloy>198)
				{
					LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
					return FALSE;

				}
			}
			
			ZeroMemory(chSendBuf,sizeof(chSendBuf));
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			strSendBuf.Format("GET MAXIDX %d\r",nChannel+1);

			memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
			if(!m_comPM[0].WriteBuffer(chSendBuf,strSendBuf.GetLength()))
			{
				strMsg.Format("发送指令%s失败!",strSendBuf);
				LogInfo(strMsg);
				return FALSE;
			}
			Sleep(50);
			if (!m_comPM[0].ReadBuffer(chReadBuf,256))
			{
				strMsg.Format("接收指令%s失败!",strSendBuf);
				LogInfo(strMsg);
				return FALSE;
			}
			LogInfo(chReadBuf);
			wScanMaxIndex = atoi(chReadBuf);
			
			if (wScanMaxIndex == 0)
			{
				strMsg.Format("没有找到区域(%d,%d,%d,%d,%d)的IL最小点。", sScanMinVx, sScanMaxVx, sScanMinVy, sScanMaxVy, wStep);
				LogInfo(strMsg);
				return FALSE;
			}
			//求取该序号对应的坐标
			wRowIndex = (wScanMaxIndex >> 5) & 0x1F;
			wColumnIndex = wScanMaxIndex & 0x1F;
			if (wRowIndex & 0x01)
			{
				wColumnIndex = 0x1F - wColumnIndex;
			}
			sDACx = sScanMinVx + wColumnIndex * wStep;
			sDACy = sScanMinVy + wRowIndex * wStep;
			strMsg.Format("序号：%d，最小点坐标：(%d,%d)", wScanMaxIndex, sDACx, sDACy);
			LogInfo(strMsg);
			wStep = (wStep >> 2);
		}
		m_Xbase[nChannel] = sDACx;
		m_Ybase[nChannel] = sDACy;
//////////////////////////////////////////////////

/*
        xmin = XADC - 30;
		xmax = XADC + 30;
		ymin = YADC - 30;
		ymax = YADC + 30;

		if(xmin < 0 )
        {
			
			strdacxmin.Format("%x",xmin);
			strdacxmin= strdacxmin.Right(4);
			
		
		}
		if (xmax < 0)
		{
			strdacxmax.Format("%x",xmax);
			strdacxmax= strdacxmax.Right(4);
		}
		if (ymax > 0 )
		{
           
			if (ymax>255 )
			{
				strdacymax.Format("0%x",ymax);
			}
			else if (ymax<16)
			{
				strdacymax.Format("000%x",ymax);
			}
			else
			{
				strdacymax.Format("00%x",ymax);
			}
		
		}
		if ( ymin > 0)
		{
			if (ymin>255)
			{
				strdacymin.Format("0%x",ymin);
			}
			else if (ymin<16)
			{
				strdacymin.Format("000%x",ymin);
			}
			else
			{
				strdacymin.Format("00%x",ymin);

			}
		}
		if(ymin < 0 )
        {
			
			strdacymin.Format("%x",ymin);
			strdacymin = strdacymin.Right(4);
			
		
		}
		if (ymax < 0)
		{
	    	strdacymax.Format("%x",ymax);
		    strdacymax= strdacymax.Right(4);
		}
			
	
		if(xmax > 0 )
		{
		
			if (xmax >255 )
			{	
				strdacxmax.Format("0%x",xmax);
			}
			else if (xmax<16)
			{
				strdacxmax.Format("000%x",xmax);
			}
			else
			{
			    strdacxmax.Format("00%x",xmax);
			}
			
		}
		if ( xmin>0)
		{
			if (xmin>255)
			{
				strdacxmin.Format("0%x",xmin);
			}
			else if (xmin<16)
			{
				strdacxmin.Format("000%x",xmin);
			}
			else 
			{
				strdacxmin.Format("00%x",xmin);

			}
		}
        
		ZeroMemory(chSendBuf,sizeof(chSendBuf));
		ZeroMemory(chReadBuf,sizeof(chReadBuf));
		strSendBuf.Format("SCAN STARTADC %d 3721\r",nChannel+1);
//		strMsg.Format("CH%d 中心电压 %d %d\r",nChannel+1,x_base,y_base);
//		LogInfo(strMsg);
		memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
		if(!m_comPM[0].WriteBuffer(chSendBuf,strSendBuf.GetLength()))
		{
			strMsg.Format("发送指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		Sleep(50);
		if (!m_comPM[0].ReadBuffer(chReadBuf,256))
		{
			strMsg.Format("接收指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}


		//m_cmd.Format("%s%s%s%s%s0001",dwBaseAddress,strdacxmin,strdacxmax,strdacymin,strdacymax);

	    //LogInfo(m_cmd);
		
		//TxSTRToI2C("014A",m_cmd); //Single MemsSwitch Scan 

		if(!SendProductScanZone(atoi(dwBaseAddress),xmin,xmax,ymin,ymax,1))
		{
			LogInfo("发送扫描指令失败!",FALSE,COLOR_RED);
			return FALSE;
		}

		
		/*if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);


        for (int xloy = 1; xloy < 200 ;xloy ++)
		{
				Sleep(100);

				int nstate = GetScanState(atoi(dwBaseAddress));
				//RxDataFromI2C("0140"); //MemsSwitch Scan status
				if (nstate == 0)
				{
					break;
				}
				if (xloy>198)
				{
				    LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
					return FALSE;

				}
        }
		
		ZeroMemory(chSendBuf,sizeof(chSendBuf));
		ZeroMemory(chReadBuf,sizeof(chReadBuf));
		strSendBuf.Format("GET MAXIDX %d\r",nChannel+1);

		memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
		if(!m_comPM[0].WriteBuffer(chSendBuf,strSendBuf.GetLength()))
		{
			strMsg.Format("发送指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		Sleep(50);
		if (!m_comPM[0].ReadBuffer(chReadBuf,256))
		{
			strMsg.Format("接收指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}

		LogInfo(chReadBuf);
		int maxidall=0,maxidax=0,maxiday=0;
		float maxf,fsum;
		int maxiyrow;

			maxidall = atoi(chReadBuf);

		if (maxidall < 1 || maxidall> 3721)
		{
			strMsg = "测试板返回最大功率点出错。";
			LogInfo(strMsg);
			return FALSE;
		}
        
		maxiday= maxidall / 61;
		maxiyrow = maxidall / 61;

		maxf=  maxidall / 61.0 ;
		maxidax = maxidall/61;

		fsum = maxf-maxidax;

		maxidax = 61 * fsum -1;
		if (maxiyrow % 2==0)
		{
			xmin = xmin + (maxidax);
			
		}
		else
		{
		
			xmin = xmax - (maxidax);
			
		}

		ymin = ymin + maxiday;

			ScanPowerChannel[nChannel].VoltageX = xmin*60000.0/MAX_DAC;
		ScanPowerChannel[nChannel].VoltageY = ymin*60000.0/MAX_DAC;

		m_Xbase[nChannel]=xmin;
        m_Ybase[nChannel]=ymin;
      
		 if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}
		if(m_Xbase[nChannel] < 0)
        {
			strdacx.Format("%x",m_Xbase[nChannel]);
			strdacx= strdacx.Right(4);
			
		}
		if(m_Ybase[nChannel]>0)
		{
			if (m_Ybase[nChannel]>255)
			{
				strdacy.Format("0%x",m_Ybase[nChannel]);
			}
			else if (m_Ybase[nChannel]<16)
			{
				strdacy.Format("000%x",m_Ybase[nChannel]);
			}
			else
			{
				strdacy.Format("00%x",m_Ybase[nChannel]);
			}
		}
		if(m_Ybase[nChannel] < 0)
		{
			strdacy.Format("%x",m_Ybase[nChannel]);
			strdacy = strdacy.Right(4);
		}
		if(m_Xbase[nChannel] > 0)
		{
			if (m_Xbase[nChannel] >255)
			{
				strdacx.Format("0%x",m_Xbase[nChannel]);
			}
			else if (m_Xbase[nChannel]<16)
			{
				strdacx.Format("000%x",m_Xbase[nChannel]);
			}
			else
			{
				strdacx.Format("00%x",m_Xbase[nChannel]);
			}
		}
		*/
		
		/*m_cmd.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",m_cmd); //Set Single MemsSwitch DAC

		
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC"); //Command Status
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),m_Xbase[nChannel], m_Ybase[nChannel]) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		
		Sleep(100);
		//读取该通道的功率值
		dblPower = ReadPWMPower(nChannel);
		/*
		if(!GetPDPower(m_nReadPwrCH[nChannel]+1,&dblPower))
		{
			throw"读取功率失败！";
		}
		
		//读取到的功率需要减去归零功率
		ScanPowerChannel[nChannel].MaxPower = m_dblReferencePower[nChannel]-dblPower;
		
		strMsg.Format("通道%d的电压点为：%d,%d,对应的功率为%.2fdB!",nChannel+1,ScanPowerChannel[nChannel].VoltageX,
			ScanPowerChannel[nChannel].VoltageY,ScanPowerChannel[nChannel].MaxPower);
		LogInfo(strMsg);
		if(ScanPowerChannel[nChannel].MaxPower>dblSetIL)
		{
			strMsg.Format("通道%d没有找到IL小于%.2fdB的点！",nChannel+1,dblSetIL);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			goto cab;
		}
		else if(ScanPowerChannel[nChannel].MaxPower < dblSetIL && ScanPowerChannel[nChannel].MaxPower > 0.69)
		{
			int iSel;
			strMsg.Format("参数超过0.7，是否启用细扫查找最小点？");
			iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);

			if (iSel==IDYES)
			{
				x_base = ScanPowerChannel[nChannel].VoltageX;
				y_base = ScanPowerChannel[nChannel].VoltageY;
				
				goto hht;
			}
			else
			{
		    	return TRUE;
			}
		}
		else
		{
			 return TRUE;

		}

cab:	XADC=(x_base*MAX_DAC/60000.0);
		YADC=(y_base*MAX_DAC/60000.0);


		xmin = XADC - 300;
		xmax = XADC + 300;
		ymin = YADC - 300;
		ymax = YADC + 300;
		

		if(xmin < 0 )
        {
			
			strdacxmin.Format("%x",xmin);
			strdacxmin= strdacxmin.Right(4);
			
		
		}
		if (xmax < 0)
		{
			strdacxmax.Format("%x",xmax);
			strdacxmax= strdacxmax.Right(4);
		}
		if (ymax > 0 )
		{
           
			if (ymax>255 )
			{
				strdacymax.Format("0%x",ymax);
			}
			else if (ymax<16)
			{
				strdacymax.Format("000%x",ymax);
			}
			else
			{
				strdacymax.Format("00%x",ymax);
			}
		
		}
		if ( ymin > 0)
		{
			if (ymin>255)
			{
				strdacymin.Format("0%x",ymin);
			}
			else if (ymin<16)
			{
				strdacymin.Format("000%x",ymin);
			}
			else
			{
				strdacymin.Format("00%x",ymin);

			}
		}
		if(ymin < 0 )
        {
			
			strdacymin.Format("%x",ymin);
			strdacymin = strdacymin.Right(4);
			
		
		}
		if (ymax < 0)
		{
	    	strdacymax.Format("%x",ymax);
		    strdacymax= strdacymax.Right(4);
		}
			
	
		if(xmax > 0 )
		{
		
			if (xmax >255 )
			{	
				strdacxmax.Format("0%x",xmax);
			}
			else if (xmax<16)
			{
				strdacxmax.Format("000%x",xmax);
			}
			else
			{
			    strdacxmax.Format("00%x",xmax);
			}
			
		}
		if ( xmin>0)
		{
			if (xmin>255)
			{
				strdacxmin.Format("0%x",xmin);
			}
			else if (xmin<16)
			{
				strdacxmin.Format("000%x",xmin);
			}
			else 
			{
				strdacxmin.Format("00%x",xmin);

			}
		}



		ZeroMemory(chSendBuf,sizeof(chSendBuf));
		ZeroMemory(chReadBuf,sizeof(chReadBuf));
		strSendBuf.Format("SCAN STARTADC %d 14400\r",nChannel+1);
//		strMsg.Format("CH%d 中心电压 %d %d\r",nChannel+1,x_base,y_base);
	    LogInfo(strSendBuf);
		memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
		if(!m_comPM[0].WriteBuffer(chSendBuf,strSendBuf.GetLength()))
		{
			strMsg.Format("发送指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		Sleep(50);
		if (!m_comPM[0].ReadBuffer(chReadBuf,256))
		{
			strMsg.Format("接收指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
        LogInfo(chReadBuf);

		/*m_cmd.Format("%s%s%s%s%s0005",dwBaseAddress,strdacxmin,strdacxmax,strdacymin,strdacymax);
		LogInfo(m_cmd);
		TxSTRToI2C("014A",m_cmd); //Single MemsSwitch Scan
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);*/

/*
		if(!SendProductScanZone(atoi(dwBaseAddress),xmin,xmax,ymin,ymax,5))
		{
			LogInfo("发送扫描指令失败!",FALSE,COLOR_RED);
			return FALSE;
		}

        for ( xloy = 1; xloy < 200 ;xloy ++)
		{
				Sleep(100);

				int nstate = GetScanState(atoi(dwBaseAddress));
				//RxDataFromI2C("0140"); //MemsSwitch Scan status
				if (nstate == 0)
				{
					break;
				}

				if (xloy>198)
				{
				    LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
					return FALSE;

				}
        }
		
		ZeroMemory(chSendBuf,sizeof(chSendBuf));
		ZeroMemory(chReadBuf,sizeof(chReadBuf));
		strSendBuf.Format("GET MAXIDX %d\r",nChannel+1);
       	LogInfo(strSendBuf);
		memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
		if(!m_comPM[0].WriteBuffer(chSendBuf,strSendBuf.GetLength()))
		{
			strMsg.Format("发送指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		
		Sleep(50);
		if (!m_comPM[0].ReadBuffer(chReadBuf,256))
		{
			strMsg.Format("接收指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}

		LogInfo(chReadBuf);
		
		maxidall = atoi(chReadBuf);

		if (maxidall < 1 || maxidall> 14400)
		{
			strMsg = "测试板返回最大功率点出错。";
			LogInfo(strMsg);
			return FALSE;
		}

        

		maxiday= maxidall / 121 * 5 ;
        maxiyrow = maxidall / 121 ;
		maxf=  maxidall / 121.0 ;
		maxidax = maxidall/121;

		fsum = maxf-maxidax;

		maxidax = 121 * fsum -1;
		if (maxiyrow % 2==0)
		{

			xmin = xmin + (maxidax*5);		
		}
		else
		{
		
			xmin = xmax - (maxidax*5);
			
		}
		
		ymin = ymin + maxiday;


        xmax = xmin + 30;
    	xmin = xmin - 30;
		
		
		ymax = ymin + 30;
		ymin = ymin - 30;
		
		

		if(xmin < 0 )
        {
			
			strdacxmin.Format("%x",xmin);
			strdacxmin= strdacxmin.Right(4);
			
		
		}
		if (xmax < 0)
		{
			strdacxmax.Format("%x",xmax);
			strdacxmax= strdacxmax.Right(4);
		}
		if (ymax > 0 )
		{
           
			if (ymax>255 )
			{
				strdacymax.Format("0%x",ymax);
			}
			else if (ymax<16)
			{
				strdacymax.Format("000%x",ymax);
			}
			else
			{
				strdacymax.Format("00%x",ymax);
			}
		
		}
		if ( ymin > 0)
		{
			if (ymin>255)
			{
				strdacymin.Format("0%x",ymin);
			}
			else if (ymin<16)
			{
				strdacymin.Format("000%x",ymin);
			}
			else
			{
				strdacymin.Format("00%x",ymin);

			}
		}
		if(ymin < 0 )
        {
			
			strdacymin.Format("%x",ymin);
			strdacymin = strdacymin.Right(4);
			
		
		}
		if (ymax < 0)
		{
	    	strdacymax.Format("%x",ymax);
		    strdacymax= strdacymax.Right(4);
		}
			
	
		if(xmax > 0 )
		{
		
			if (xmax >255 )
			{	
				strdacxmax.Format("0%x",xmax);
			}
			else if (xmax<16)
			{
				strdacxmax.Format("000%x",xmax);
			}
			else
			{
			    strdacxmax.Format("00%x",xmax);
			}
			
		}
		if ( xmin>0)
		{
			if (xmin>255)
			{
				strdacxmin.Format("0%x",xmin);
			}
			else if (xmin<16)
			{
				strdacxmin.Format("000%x",xmin);
			}
			else
			{
				strdacxmin.Format("00%x",xmin);

			}
		}



		ZeroMemory(chSendBuf,sizeof(chSendBuf));
		ZeroMemory(chReadBuf,sizeof(chReadBuf));
		strSendBuf.Format("SCAN STARTADC %d 3721\r",nChannel+1);
//		strMsg.Format("CH%d 中心电压 %d %d\r",nChannel+1,x_base,y_base);
//		LogInfo(strMsg);
		memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
		if(!m_comPM[0].WriteBuffer(chSendBuf,strSendBuf.GetLength()))
		{
			strMsg.Format("发送指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		Sleep(50);
		if (!m_comPM[0].ReadBuffer(chReadBuf,256))
		{
			strMsg.Format("接收指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}


		/*m_cmd.Format("%s%s%s%s%s0001",dwBaseAddress,strdacxmin,strdacxmax,strdacymin,strdacymax);

	    LogInfo(m_cmd);
		
		TxSTRToI2C("014A",m_cmd); //Single MemsSwitch Scan Setting

		
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);

        for ( xloy = 1; xloy < 200 ;xloy ++)
		{
				Sleep(100);
				RxDataFromI2C("0140");
				if (m_strErrorMsg == "")
				{
					break;
				}
				if (xloy>198)
				{
				    LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
					return FALSE;

				}
        }*/
/*
		if(!SendProductScanZone(atoi(dwBaseAddress),xmin,xmax,ymin,ymax,1))
		{
			LogInfo("发送扫描指令失败!",FALSE,COLOR_RED);
			return FALSE;
		}

        for (xloy = 1; xloy < 200 ;xloy ++)
		{
				Sleep(100);

				int nstate = GetScanState(atoi(dwBaseAddress));
				//RxDataFromI2C("0140"); //MemsSwitch Scan status
				if (nstate == 0)
				{
					break;
				}
				if (xloy>198)
				{
				    LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
					return FALSE;

				}
        }
		
		ZeroMemory(chSendBuf,sizeof(chSendBuf));
		ZeroMemory(chReadBuf,sizeof(chReadBuf));
		strSendBuf.Format("GET MAXIDX %d\r",nChannel+1);

		memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
		if(!m_comPM[0].WriteBuffer(chSendBuf,strSendBuf.GetLength()))
		{
			strMsg.Format("发送指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		Sleep(50);
		if (!m_comPM[0].ReadBuffer(chReadBuf,256))
		{
			strMsg.Format("接收指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}

		LogInfo(chReadBuf);

			maxidall = atoi(chReadBuf);

		if (maxidall < 1 || maxidall> 3721)
		{
			strMsg = "测试板返回最大功率点出错。";
			LogInfo(strMsg);
			return FALSE;
		}
        
		maxiday= maxidall / 61;
		maxiyrow = maxidall / 61;

		maxf=  maxidall / 61.0 ;
		maxidax = maxidall/61;

		fsum = maxf-maxidax;

		maxidax = 61 * fsum -1;
		if (maxiyrow % 2==0)
		{
			xmin = xmin + (maxidax);
			
		}
		else
		{
		
			xmin = xmax - (maxidax);
			
		}

		ymin = ymin + maxiday;




	


		ScanPowerChannel[nChannel].VoltageX = xmin*60000.0/MAX_DAC;
		ScanPowerChannel[nChannel].VoltageY = ymin*60000.0/MAX_DAC;

		m_Xbase[nChannel]=xmin;
        m_Ybase[nChannel]=ymin;
      
		 if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}
		if(m_Xbase[nChannel] < 0)
        {
			strdacx.Format("%x",m_Xbase[nChannel]);
			strdacx= strdacx.Right(4);
			
		}
		if(m_Ybase[nChannel]>0)
		{
			if (m_Ybase[nChannel]>255)
			{
				strdacy.Format("0%x",m_Ybase[nChannel]);
			}
			else if (m_Ybase[nChannel]<16)
			{
				strdacy.Format("000%x",m_Ybase[nChannel]);
			}
			else
			{
				strdacy.Format("00%x",m_Ybase[nChannel]);
			}
		}
		if(m_Ybase[nChannel] < 0)
		{
			strdacy.Format("%x",m_Ybase[nChannel]);
			strdacy = strdacy.Right(4);
		}
		if(m_Xbase[nChannel] > 0)
		{
			if (m_Xbase[nChannel] >255)
			{
				strdacx.Format("0%x",m_Xbase[nChannel]);
			}
			else if (m_Xbase[nChannel]<16)
			{
				strdacx.Format("000%x",m_Xbase[nChannel]);
			}
			else
			{
				strdacx.Format("00%x",m_Xbase[nChannel]);
			}
		}
		*/
		/*m_cmd.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		TxSTRToI2C("0145",m_cmd); //Set Single MemsSwitch DAC
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/

        ScanPowerChannel[nChannel].VoltageX = m_Xbase[nChannel]*60000.0/MAX_DAC;
		ScanPowerChannel[nChannel].VoltageY = m_Ybase[nChannel]*60000.0/MAX_DAC;
  
		if( !SetTesterXYDAC(atoi(dwBaseAddress),m_Xbase[nChannel], m_Ybase[nChannel]) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
	
		Sleep(100);
		//读取该通道的功率值
		dblPower = ReadPWMPower(nChannel);
		/*
		if(!GetPDPower(m_nReadPwrCH[nChannel]+1,&dblPower))
		{
			throw"读取功率失败！";
		}
		*/
		//读取到的功率需要减去归零功率
		ScanPowerChannel[nChannel].MaxPower = m_dblReferencePower[nChannel]-dblPower;
		
		strMsg.Format("通道%d的电压点为：%d,%d,对应的功率为%.2fdB!",nChannel+1,ScanPowerChannel[nChannel].VoltageX,
			ScanPowerChannel[nChannel].VoltageY,ScanPowerChannel[nChannel].MaxPower);
		LogInfo(strMsg);
		if(ScanPowerChannel[nChannel].MaxPower>dblSetIL)
		{
			strMsg.Format("通道%d没有找到IL小于%.2fdB的点！",nChannel+1,dblSetIL);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		int iSel;
		if(ScanPowerChannel[nChannel].MaxPower> 0.69)
		{
		
			strMsg.Format("参数超过0.7，是否启用细扫查找最小点？");
			iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);

			if (iSel==IDYES)
			{
				x_base = ScanPowerChannel[nChannel].VoltageX;
				y_base = ScanPowerChannel[nChannel].VoltageY;
				if (ScanPowerChannel[nChannel].MaxPower > 30)
				{
					x_base = x_bbackup;
				    y_base = y_bbackup;
				}
				goto hht;
			}
			else
			{
				return TRUE;

			}
		

		}
		return TRUE;
	}

//获取基准点功率
//	Sleep(1000);
//	SETX(x_base);
//	Sleep(300);
//	SETY(y_base);
//	Sleep(400);

    
		
     
hht:	XADC=(x_base*MAX_DAC/60000.0);
		YADC=(y_base*MAX_DAC/60000.0);
	   if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}
		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		
		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand); //Set Single MemsSwitch DAC

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}


	dblBasePower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);

	//扫描第一个点的光功率值，若异常，则重复读光功率计5次
	while (dblBasePower > 20 && x_count < 5)
	{
		Sleep(400);
		dblBasePower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
		x_count++;
	}
	while (dblBasePower == 0 && x_count < 5)
	{
		Sleep(400);
		dblBasePower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
		x_count++;
	}
	if (dblBasePower == 0)
	{
		LogInfo("扫描出错！",(BOOL)FALSE,COLOR_RED);
		return  FALSE;
	}

	strMsg.Format("坐标：(%d,%d), Power:%.3f dBm\n",x_base,y_base,dblBasePower);
	LogInfo(strMsg);
	
	x = x_base;
	y = y_base;
	dblBasePower_X = dblBasePower;
	dblBasePower_Y = dblBasePower;

	XADC=(x*MAX_DAC/60000.0);
	YADC=(y*MAX_DAC/60000.0);


	int nCount = 0;
//	CString m_cmd;
	while (1)
	{
//y     
	//	SETX(x);

		if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}

		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		
		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand); //Set Single MemsSwitch DAC

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/


		dblBasePower_Y = dblBasePower_X;
		while(1)
		{
			YieldToPeers();
			if(m_bTestStop)
			{
				return FALSE;
			}		
			//y_step1 = 100;
			//y_step2 = -100;
			y_step1 = 1000;
			y_step2 = -1000;
			//设置步数
			//设置步数
			if (dblBasePower_Y < Point1[0])
			{
				y_step1 = step1[0];
				y_step2 = -step1[0];
			}
			if (dblBasePower_Y < Point1[1])
			{
				y_step1 = step1[1];
				y_step2 = -step1[1];
			}
			if (dblBasePower_Y < Point1[2])
			{
				y_step1 = step1[2];
				y_step2 = -step1[2];
			}
			if (dblBasePower_Y < Point1[3])
			{
				y_step1 = step1[3];
				y_step2 = -step1[3];
			}
			if (dblBasePower_Y < Point1[4])
			{
				y_step1 = step1[4];
				y_step2 = -step1[4];
			}
			if (dblBasePower_Y < Point1[5])
			{
				y_step1 = step1[5];
				y_step2 = -step1[5];
			}

			y_step = y_step1;
			if (bFlagY)
			{
				y_step = y_step2;			
			}
			y = y + y_step;
			//SETY(y);

           
		XADC=(x*MAX_DAC/60000.0);
		YADC=(y*MAX_DAC/60000.0);

       	/*if (b_issn1) 
		{
			dwBaseAddress="00";
		}
		else if (b_issn2)
		{
			dwBaseAddress="01";
		}
		else if(b_issn3)
		{
			dwBaseAddress="02";
		}
		else
		{
			return FALSE;
		}*/
		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}

		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand); //Set Single MemsSwitch DAC

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/






			Sleep(200);
			dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
			iCount = 0;
			while (dblPower <= 0&&iCount<5)
			{
				Sleep(200);
				dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
				iCount++;
			}

			if (dblPower <= 0) 
			{
				LogInfo("与光功率计通讯错误,读取的光功率值过大,将取上一次的值",(BOOL)FALSE,COLOR_RED);
				dblPower = dblPrePowerY;
			}
			dblPrePowerY = dblPower;
			
			m_NTGraph.PlotXY(x,y,0);
			strMsg.Format("坐标:(%d,%d)，IL：%.3f dB\n",x,y,dblPower);
			LogInfo(strMsg);

			//比较大小，存入小的值	
			if (dblPower < dblBasePower_Y)
			{
				dblBasePower_Y = dblPower;
				bFlag1 = TRUE;
				y_base = y;
				count = 0;
			}
			else
			{
				count++;
			}
			//连续三次减小，且有新的值存入 则跳出
			if (count == 4 && bFlag1 == TRUE)
			{
				bFlag1 = FALSE;
				count = 0;
				break;
			}
			//反向寻找时，连续三次未找到变大的值
			if (count==4 && bFlag1==FALSE && bFlagY==TRUE)
			{
				count = 0;
				break;
			}
			//连续三次减小，且无新的值存入，则反向查找
			if (count == 4 && bFlag1 == FALSE)
			{
				bFlagY = TRUE;
				y = y_base;
				count = 0;
			}			
		}
		bFlagY = FALSE;
		y = y_base;
		dblBasePower_X = dblBasePower_Y;
		//x
		//SETY(y);
      	XADC=(x*MAX_DAC/60000.0);
	   YADC=(y*MAX_DAC/60000.0);


      	if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}

		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		
		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand); //Set Single MemsSwitch DAC

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/
	
		while(1)
		{
			YieldToPeers();
			if(m_bTestStop)
			{
				return FALSE;
			}

			//x_step1 = 100;
			//x_step2 = -100;
			x_step1 = 1000;
			x_step2 = -1000;

			if (dblBasePower_X < Point1[0])
			{
				x_step1 = step1[0];
				x_step2 = -step1[0];
			}
			if (dblBasePower_X < Point1[1])
			{
				x_step1 = step1[1];
				x_step2 = -step1[1];
			}
			if (dblBasePower_X < Point1[2])
			{
				x_step1 = step1[2];
				x_step2 = -step1[2];
			}
			if (dblBasePower_X < Point1[3])
			{
				x_step1 = step1[3];
				x_step2 = -step1[3];
			}
			if (dblBasePower_X < Point1[4])
			{
				x_step1 = step1[4];
				x_step2 = -step1[4];
			}
			if (dblBasePower_X < Point1[5])
			{
				x_step1 = step1[5];
				x_step2 = -step1[5];
			}

			x_step = x_step1;
			if (bFlagX)
			{
				x_step = x_step2;		
			}
			x = x + x_step;
			//SETX(x);

		XADC=(x*MAX_DAC/60000.0);
	   YADC=(y*MAX_DAC/60000.0);


		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		
		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand); //Set Single MemsSwitch DAC

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/






			Sleep(100);
			dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
			iCount = 0;
			while (dblPower <= 0 && iCount<5)
			{
				Sleep(50);
				dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
				iCount++;
			}

			if (dblPower <= 0) 
			{
				LogInfo("与光功率计通讯错误,读取的光功率值过大!将取上一次的值",(BOOL)FALSE,COLOR_RED);
				dblPower = dblPrePowerX;
			}
			dblPrePowerX = dblPower;

			m_NTGraph.PlotXY(x,y,0);
			strMsg.Format("坐标:(%d,%d)，IL：%.3f dB\n",x,y,dblPower);
			LogInfo(strMsg);
			
			if (dblPower < dblBasePower_X)
			{
				dblBasePower_X = dblPower;
				bFlag1 = TRUE;
				x_base = x;
				count = 0;
			}
			else
			{
				count++;
			}
			if (count == 4 && bFlag1 == TRUE)
			{
				bFlag1 = FALSE;
				count = 0;
				break;
			}
			if (bFlagX==TRUE&&bFlag1==FALSE&&count==4) 
			{
				count = 0;
				break;
			}
			if (count == 4 && bFlag1 == FALSE)
			{
				bFlagX = TRUE;
				x = x_base;
				count = 0;
			}			
		}
		bFlagX = FALSE;
		x = x_base;
		if (fabs(dblBasePower_X - dblBasePower_Y)<=0.01 && dblBasePower_X < dblSetIL)
		{
			break;
		}
		nCount++;
		if (nCount>=5)
		{
			strMsg.Format("通道%d没有找到IL小于%.2fdB的点！",nChannel+1,dblSetIL);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return FALSE;				
		}
	}

	ScanPowerChannel[nChannel].VoltageX = x_base;
	ScanPowerChannel[nChannel].VoltageY = y_base;
	ScanPowerChannel[nChannel].MaxPower = dblBasePower_X;

	strMsg.Format("通道%d的最大插损为：%.2fdBm，对应的电压坐标为：(%d,%d)",nChannel+1,ScanPowerChannel[nChannel].MaxPower,
		ScanPowerChannel[nChannel].VoltageX,ScanPowerChannel[nChannel].VoltageY);
	LogInfo(strMsg,FALSE);
	
	return TRUE;
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonTest() 
{
	// TODO: Add your control notification handler code here
	double Power;
	CString strMsg;
	int n=0;

	if (!m_bOpenPM)
	{
		MessageBox("请先连接光功率计！");
		return;
	}

	for ( n=0;n<20;n++)
	{
		Power = ReadPWMPower(n);
		strMsg.Format("%.2f",Power);
		LogInfo(strMsg);
	}	
}

void CMy126S_45V_Switch_AppDlg::OnButtonSaveFile() 
{
	// TODO: Add your control notification handler code here
	CString strCSVFile;
	CStdioFile stdFile;
	CString strMsg;
	CString strValue;
	CString strContent;
	HANDLE hHandle;
	WIN32_FIND_DATA win32Find;
	LPTSTR  lpStr;
	CString strToken;
	char    chSep[]=",";
	char    chLine[MAX_LINE];
	int nCH=0;
	int i = 0;

	m_bAutoScan = FALSE;
	m_bTestStop = FALSE;
	ZeroMemory(m_dwSingle,sizeof(m_dwSingle));
	ZeroMemory(chLine,sizeof(MAX_LINE));

	UpdateData();
	//查看数据文件是否齐全
	//电压坐标文件
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage.csv", m_strNetFile,m_strPN,m_strSN,m_strSN);
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle==INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s不存在，无法保存数据！",strCSVFile);
		LogInfo(strMsg);
		return;	
	}
	//调用文件数据
	if (!stdFile.Open(strCSVFile,CFile::modeRead))
	{
		MessageBox("打开文件失败！");
		return;
	}
	lpStr = stdFile.ReadString(chLine,MAX_LINE);
	for (nCH=0;nCH<12;nCH++)
	{
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		strToken = strtok(chLine,chSep);
		strToken = strtok(NULL,chSep);
		ScanPowerChannel[nCH].VoltageX = atoi(strToken);
		strToken = strtok(NULL,chSep);
		ScanPowerChannel[nCH].VoltageY = atoi(strToken);
	}
	stdFile.Close();

	//CT
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_CT.csv", m_strNetFile,m_strPN,m_strSN,m_strSN);
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle==INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s不存在，无法保存数据！",strCSVFile);
		LogInfo(strMsg);
		return;	
	}
	//调用文件数据
	if (!stdFile.Open(strCSVFile,CFile::modeRead))
	{
		MessageBox("打开文件失败！");
		return;
	}
	lpStr = stdFile.ReadString(chLine,MAX_LINE);
	for (nCH=0;nCH<12;nCH++)
	{
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		strToken = strtok(chLine,chSep);
		for (i=0;i<12;i++)
		{		
	    	strToken = strtok(NULL,chSep);
			ChannalMessage[nCH].dblCT[i] = atof(strToken);
		}
	}
	stdFile.Close();
	//PDL
	for (nCH=0;nCH<12;nCH++)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_PDL_%d.csv", m_strNetFile,m_strPN,m_strSN,m_strSN,nCH+1);
		hHandle = FindFirstFile(strCSVFile,&win32Find);
		if (hHandle==INVALID_HANDLE_VALUE)
		{
			strMsg.Format("文件%s不存在，无法保存数据！",strCSVFile);
			LogInfo(strMsg);
			return;	
		}
		//调用文件数据
		if (!stdFile.Open(strCSVFile,CFile::modeRead))
		{
			MessageBox("打开文件失败！");
			return;
		}
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		strToken = strtok(chLine,chSep);
		strToken = strtok(NULL,chSep);
		ChannalMessage[nCH].dblPDL = atof(strToken);
    	stdFile.Close();
	}
		
	//REP
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_Repeat.csv", m_strNetFile,m_strPN,m_strSN,m_strSN);
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle==INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s不存在，无法保存数据！",strCSVFile);
		LogInfo(strMsg);
		return;	
	}
	//调用文件数据
	if (!stdFile.Open(strCSVFile,CFile::modeRead))
	{
		MessageBox("打开文件失败！");
		return;
	}
	lpStr = stdFile.ReadString(chLine,MAX_LINE);
	for (nCH=0;nCH<12;nCH++)
	{
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		strToken = strtok(chLine,chSep);
		strToken = strtok(NULL,chSep);
		strToken = strtok(NULL,chSep);
		strToken = strtok(NULL,chSep);
		ChannalMessage[nCH].dblDif = atof(strToken);
	}
	stdFile.Close();

	//LOW_IL
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_Low.csv", m_strNetFile,m_strPN,m_strSN,m_strSN);
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle==INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s不存在，无法保存数据！",strCSVFile);
		LogInfo(strMsg);
		return;	
	}
	//调用文件数据
	if (!stdFile.Open(strCSVFile,CFile::modeRead))
	{
		MessageBox("打开文件失败！");
		return;
	}
	lpStr = stdFile.ReadString(chLine,MAX_LINE);
	for (nCH=0;nCH<12;nCH++)
	{
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		strToken = strtok(chLine,chSep);
		strToken = strtok(NULL,chSep);
		ChannalMessage[nCH].dblLowTempIL = atof(strToken);
	}
	stdFile.Close();
	
	//HIGH_IL
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_High.csv", m_strNetFile,m_strPN,m_strSN,m_strSN);
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle==INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s不存在，无法保存数据！",strCSVFile);
		LogInfo(strMsg);
		return;
	}
	//调用文件数据
	if (!stdFile.Open(strCSVFile,CFile::modeRead))
	{
		MessageBox("打开文件失败！");
		return;
	}
	lpStr = stdFile.ReadString(chLine,MAX_LINE);
	for (nCH=0;nCH<12;nCH++)
	{
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		strToken = strtok(chLine,chSep);
		strToken = strtok(NULL,chSep);
		ChannalMessage[nCH].dblHighTempIL = atof(strToken);
	}
	stdFile.Close();

	//Room_IL
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_Room.csv", m_strNetFile,m_strPN,m_strSN,m_strSN);
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle==INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s不存在，无法保存数据！",strCSVFile);
		LogInfo(strMsg);
		return;
	}
	//调用文件数据
	if (!stdFile.Open(strCSVFile,CFile::modeRead))
	{
		MessageBox("打开文件失败！");
		return;
	}
	lpStr = stdFile.ReadString(chLine,MAX_LINE);
	for (nCH=0;nCH<12;nCH++)
	{
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		strToken = strtok(chLine,chSep);
		strToken = strtok(NULL,chSep);
		ChannalMessage[nCH].dblIL = atof(strToken);
	}
	stdFile.Close();

	//TDL
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_TDL.csv", m_strNetFile,m_strPN,m_strSN,m_strSN);
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle==INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s不存在，无法保存数据！",strCSVFile);
		LogInfo(strMsg);
		return;	
	}
	//调用文件数据
	if (!stdFile.Open(strCSVFile,CFile::modeRead))
	{
		MessageBox("打开文件失败！");
		return;
	}
	lpStr = stdFile.ReadString(chLine,MAX_LINE);
	for (nCH=0;nCH<12;nCH++)
	{
		lpStr = stdFile.ReadString(chLine,MAX_LINE);
		strToken = strtok(chLine,chSep);
		strToken = strtok(NULL,chSep);
		ChannalMessage[nCH].dblTDL = atof(strToken);
	}
	stdFile.Close();

	//数据调用完毕，写入CSV文件
	int iSel = IDYES;
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_Data.csv", m_strNetFile,m_strPN,m_strSN,m_strSN);
    hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle!=INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s已存在，是否覆盖？",strCSVFile);
		iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);
		if (iSel==IDNO)
		{
			LogInfo("文件未保存！");
			return;
		}
	}
	if (!stdFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建数据文件失败！");
		return;
	}
	strContent = "通道";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%d",i+1);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	strContent = "电压X";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%d",ScanPowerChannel[i].VoltageX);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	strContent = "电压Y";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%d",ScanPowerChannel[i].VoltageY);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	strContent = "IL";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%.2f",ChannalMessage[i].dblIL);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	for(nCH=0;nCH<12;nCH++)
	{
		strContent.Format("CH%d/CT",nCH+1);
		for (i=0;i<12;i++)
		{
			strValue.Format(",%.2f",ChannalMessage[nCH].dblCT[i]);
			strContent = strContent+strValue;
		}
		strContent = strContent+"\n";
	    stdFile.WriteString(strContent);
	}
	strContent = "PDL";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%.2f",ChannalMessage[i].dblPDL);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	strContent = "REP";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%.4f",ChannalMessage[i].dblDif);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	strContent = "LOW_IL";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%.2f",ChannalMessage[i].dblLowTempIL);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	strContent = "HIGH_IL";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%.2f",ChannalMessage[i].dblHighTempIL);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	strContent = "TDL";
	for (i=0;i<12;i++)
	{
		strValue.Format(",%.2f",ChannalMessage[i].dblTDL);
		strContent = strContent+strValue;
	}
	strContent = strContent+"\n";
	stdFile.WriteString(strContent);
	
	stdFile.Close();
	LogInfo("保存文件完毕！");

}

void CMy126S_45V_Switch_AppDlg::OnButtonRegulateScan() 
{
	// TODO: Add your control notification handler code here
	//粗扫
	if (!m_bOpenPM)
	{
		MessageBox("请先打开1830C！");
		return;
	}
	if (!m_bOpenPort)
	{
		throw("请先打开串口！");
		return;
	}
	
	CString strMsg;
	strMsg.Format("请连接好光路\n请将通道按顺序接入光功率计！");
	MessageBox(strMsg);
	//设置路径
	UpdateData();
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);
	
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("X(mv)");
	m_NTGraph.SetYLabel("Y(mv)");
	m_NTGraph.SetRange(-60000,60000,-60000,60000);
	m_NTGraph.SetXGridNumber(10);
	m_NTGraph.SetYGridNumber(10);
	
	int iSel = IDYES;
	
	CString strFileINI;
	if (m_cbComPort.GetCurSel()==0)
	{
		strFileINI = "ScanAdjustRange.ini";
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strFileINI = "ScanAdjustRange-com1.ini";
	}
	
	if (m_bAdjustScan)
	{
		strMsg.Format("已存在粗扫数据，是否重新粗扫？");
		iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);
	}
	if (iSel==IDYES)
	{
		if(!AutoScan(0,strFileINI,FALSE))
		{
			m_bAdjustScan = FALSE;
			return;
		}
		if(!AutoScan(1,strFileINI,FALSE))
		{
			m_bAdjustScan = FALSE;
			return;
		}
		if(!AutoScan(2,strFileINI,FALSE))
		{
			m_bAdjustScan = FALSE;
			return;
		}
		if(!AutoScan(3,strFileINI,FALSE))
		{
			m_bAdjustScan = FALSE;
			return;
		}
		m_bAdjustScan = TRUE;
		
	}
	
	CalculateAngle(m_Xbase[0],m_Ybase[0],m_Xbase[1],m_Ybase[1],m_Xbase[2],m_Ybase[2],m_Xbase[3],m_Ybase[3]);
    
	m_NTGraph.ClearGraph();
	
	if(!AutoPointScan(0))
	{
		MessageBox("扫描通道1出错！");
		return;
	}
	if(!AutoPointScan(1))
	{
		MessageBox("扫描通道2出错！");
		return;
	}
	if(!AutoPointScan(2))
	{
		MessageBox("扫描通道3出错！");
		return;
	}
	if(!AutoPointScan(3))
	{
		MessageBox("扫描通道4出错！");
		return;
	}
	//扫描结束，计算出转角
	CalculateAngle(ScanPowerChannel[0].VoltageX,ScanPowerChannel[0].VoltageY,ScanPowerChannel[1].VoltageX,ScanPowerChannel[1].VoltageY,
		ScanPowerChannel[2].VoltageX,ScanPowerChannel[2].VoltageY,ScanPowerChannel[3].VoltageX,ScanPowerChannel[3].VoltageY);
	//新加算法，计算X,Y电压建议值
	CalCulateVol();

	//保存调节数据
	SaveDataToExcel(ADJUST_DATA);
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonSingleScan() 
{
	// TODO: Add your control notification handler code here
	int x;
	int y;
	int step;
	int count = 0;
	int x_count = 0;
	int nCount = 0;
	double  dblPower = 0; 
	double  dblMaxPower = -100;
	BOOL    bFlag1 = FALSE;
	BOOL    bFlag2 = FALSE;
	BOOL    bFlag3 = FALSE;
	BOOL    bFlag4 = FALSE;
	CString strMsg;
	int x_start = 0;
	int x_stop = 0;
	int x_end = 0;
	int y_end = 0;
	int y_start = 0;
	int y_stop = 0;
	int ich = 0;
	double dblPrePower = 100;
    char    chValue[10];
	int i,j;
	ZeroMemory(chValue,sizeof(char)*10);

	CString strINIFile;
	CString strValue;
	CString strStartX;
	CString strEndX;
	CString strStartY;
	CString strEndY;
	CString strX;
	CString strY;
	CString strPower;
	CString strFile;
	CStdioFile stdFile;

	FILE *fp;

	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}
	m_bTestStop = FALSE;

	//设置路径
	UpdateData();
	
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);
    ///////////////////////////////////////////扫描算法

	//获取配置文件
	strINIFile.Format("%s\\Hitless.ini", m_strNetConfigPath);
	
	//获取配置文件里面的值
	DWORD nLen1 = GetPrivateProfileString("XRange","X_Start",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
		x_start = atoi(strValue);

	DWORD nLen2 = GetPrivateProfileString("XRange","X_End",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	x_end = atoi(strValue);

	DWORD nLen3 = GetPrivateProfileString("YRange","Y_Start",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	y_start = atoi(strValue);
	
	DWORD nLen4 = GetPrivateProfileString("YRange","Y_End",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	y_end = atoi(strValue);

	DWORD nLen5 = GetPrivateProfileString("Step","step",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	step = atoi(strValue);

	//初始化图形控件
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("X(mv)");
	m_NTGraph.SetYLabel("Y(mv)");
	m_NTGraph.SetRange(-45000,45000,-45000,45000);
	m_NTGraph.SetXGridNumber(10);
	m_NTGraph.SetYGridNumber(10);

	for (ich = 0;ich <12;ich++)
	{
		strFile.Format("%s\\data\\%s_HitlessNeg_%d.csv", m_strNetFile,m_strSN,ich+1);
		
		if(!stdFile.Open(strFile,CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox("创建文件失败！");
			return;
		}
		//写第一行的X坐标	
		for (x=x_start;x<=0;x=x+step)
		{
			if (x==0)
			{
				strValue.Format(",%d\n",x);
				stdFile.WriteString(strValue);
			}
			else
			{
				strValue.Format(",%d",x);
				stdFile.WriteString(strValue);
			}		
		}
		stdFile.Close();
	}
	for (ich = 0;ich <12;ich++)
	{
		strFile.Format("%s\\data\\%s_HitlessPos_%d.csv", m_strNetFile,m_strSN,ich+1);
		
		if(!stdFile.Open(strFile,CFile::modeCreate|CFile::modeWrite))
		{
			MessageBox("创建文件失败！");
			return;
		}
		//写第一行的X坐标	
		for (x=0;x<=x_end;x=x+step)
		{
			if (x==x_end)
			{
				strValue.Format(",%d\n",x);
				stdFile.WriteString(strValue);
			}
			else
			{
				strValue.Format(",%d",x);
				stdFile.WriteString(strValue);
			}		
		}
		stdFile.Close();
	}
	
	i=0;
	j=0;	
	//扫描出所有x负方向的值
	for (y=y_start;y<=y_end;y=y+step,j++)
	{
		for (x=x_start;x<=0;x=x+step,i++)
		{
			YieldToPeers();
			if (m_bTestStop)
			{
				MessageBox("测试中止!");
				return ;
			}
			SETX(x);
			Sleep(50);
			SETY(y);
			Sleep(250);
			m_NTGraph.PlotXY(x,y,0);
			//跳变时，等待
			if (x==x_start)
			{
				Sleep(200);
			}
			for (ich = 0;ich <12;ich++)
			{
				strFile.Format("%s\\data\\%s_HitlessNeg_%d.csv", m_strNetFile,m_strSN,ich+1);
				fp = fopen(strFile,"a+");
				if (fp == NULL)
				{
					return;
				}
				if (x==x_start)
				{
					strValue.Format("%d,",y);
                	fwrite(strValue,1,strValue.GetLength(),fp);
				}
				dblPower = ReadPWMPower(ich) -m_dblReferencePower[ich];
				if (x==0)
				{
					strValue.Format("%.2f\n",dblPower);
					fwrite(strValue,1,strValue.GetLength(),fp);
				}
				else
				{
					strValue.Format("%.2f,",dblPower);
			    	fwrite(strValue,1,strValue.GetLength(),fp);
				}				
				//g_HitlessPower[ich].ScanPwr[i][j] = dblPower;
				fclose(fp);

			}
			strMsg.Format("%d,%d,IL,%.2f\n",x,y,dblPower);
			LogInfo(strMsg);
		}
	}
     //扫描出所有x正方向的值
	for (y=y_start;y<=y_end;y=y+step,j++)
	{
		for (x=0;x<=x_end;x=x+step,i++)
		{
			YieldToPeers();
			if (m_bTestStop)
			{
				MessageBox("测试中止!");
				return ;
			}
			SETX(x);
			Sleep(50);
			SETY(y);
			Sleep(250);
			m_NTGraph.PlotXY(x,y,0);
			//跳变时，等待
			if (x==0)
			{
				Sleep(200);
			}
			for (ich = 0;ich <12;ich++)
			{
				strFile.Format("%s\\data\\%s_HitlessPos_%d.csv", m_strNetFile,m_strSN,ich+1);
				fp = fopen(strFile,"a+");
				if (fp == NULL)
				{
					return;
				}
				if (x==0)
				{
					strValue.Format("%d,",y);
					fwrite(strValue,1,strValue.GetLength(),fp);
				}
				dblPower = ReadPWMPower(ich) -m_dblReferencePower[ich];
				if (x==x_end)
				{
					strValue.Format("%.2f\n",dblPower);
					fwrite(strValue,1,strValue.GetLength(),fp);
				}
				else
				{
					strValue.Format("%.2f,",dblPower);
					fwrite(strValue,1,strValue.GetLength(),fp);
				}				
				//g_HitlessPower[ich].ScanPwr[i][j] = dblPower;
				fclose(fp);
				
			}
			strMsg.Format("%d,%d,IL,%.2f\n",x,y,dblPower);
			LogInfo(strMsg);
		}
	}
	MessageBox("扫描完毕！");
	LogInfo("扫描完毕！");

}

BOOL CMy126S_45V_Switch_AppDlg::CreateExcelFile()
{
	//启动EXCEL服务器
	CString strMsg;
	COleVariant  m_covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR);
	if (!m_app.CreateDispatch("EXCEL.Application",NULL))
	{
		AfxMessageBox("无法创建EXCEL应用！");
		m_bStartExcel = FALSE;
		return FALSE;
	}
	
	//允许其他用户控制EXCEL
	m_app.SetUserControl(TRUE);
	//打开EXCEL文件
	//设置路径
	UpdateData();
	m_strTemplatePath.Format("%s\\template\\%s\\%s 测试单.xls",g_tszAppFolder,m_strItemName,m_strPN);

	if(GetFileAttributes(m_strTemplatePath) == -1)
	{
		strMsg.Format("没有找到模板%s",m_strTemplatePath);
		MessageBox(strMsg);
		return FALSE;
	}

	if (m_bIfSaveToServe)
	{
		m_strOpticalDataPath.Format("%s\\data\\%s\\%s\\%s\\%s_Optical_Test_Data.xls",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	else
	{	
		m_strOpticalDataPath.Format("%s\\data\\%s\\%s\\%s\\%s_Optical_Test_Data.xls",g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);	
	}	
    HANDLE             hDictory = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA    win32FindData;

	hDictory = FindFirstFile(m_strOpticalDataPath,&win32FindData);
	if (hDictory == INVALID_HANDLE_VALUE)
	{
// 		strMsg.Format("查询到文件%s已经存在\n是否覆盖？\n",m_strOpticalDataPath);
// 		iSel = MessageBox(strMsg,"确认",MB_YESNO|MB_ICONQUESTION);
		//若文件不存在，则复制模板过去
        CopyFile(m_strTemplatePath,m_strOpticalDataPath,FALSE);
	}
	//打开EXCEL
	m_books.AttachDispatch(m_app.GetWorkbooks(),true);
	m_lpDisp=m_books.Open(m_strOpticalDataPath,m_covOptional,m_covOptional,m_covOptional,m_covOptional,
		m_covOptional,m_covOptional,m_covOptional,m_covOptional,m_covOptional,m_covOptional,
		m_covOptional,m_covOptional);
	ASSERT(m_lpDisp);
	m_book.AttachDispatch(m_lpDisp);
	m_sheets.AttachDispatch(m_book.GetWorksheets(),true);

	m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t((short)(1))));
	m_range.AttachDispatch(m_sheet.GetCells(),true);

	m_bStartExcel = TRUE;
	return TRUE;
	
}

void CMy126S_45V_Switch_AppDlg::CloseExcelFile()
{
	if (m_bStartExcel)
	{
		m_book.Save();
		m_app.SetDisplayAlerts(false); //不弹出对话框询问是否保存
		
		m_app.Quit();                  //退出		
		m_range.ReleaseDispatch();
		m_sheet.ReleaseDispatch();
		m_sheets.ReleaseDispatch();
		m_book.ReleaseDispatch();
		m_books.ReleaseDispatch();
    	m_app.ReleaseDispatch();
	}
}


BOOL CMy126S_45V_Switch_AppDlg::FindMyDirectory(CString strSubdir0, CString strSubdir1,CString strSubdir2)
{
    CString		strFileName;
	CString		strTmptValue;	
	CString     strmsg;
	HANDLE	hDirectory = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	winFindFileData;
	CString     strSNFile;

	//是否保存到网络路径
// 	CString strNetFileName;
// 	CString strSave;
// 	strNetFileName.Format("%s\\config\\DataSave.ini",g_tszAppFolder);
// 	int iLen = GetPrivateProfileString("Save","Save",NULL,strSave.GetBuffer(128),128,strNetFileName);
//     if (iLen <= 0)
//     {
// 		MessageBox("读取配置文件DataSave.ini失败");
// 		return FALSE;
//     }
	//保存到网络路径
	if (m_bIfSaveToServe == 1) //网络路径
	{
		CString strNetFilePath;
		strNetFilePath.Format("%s\\data",m_strNetFile);
        hDirectory = FindFirstFile(strNetFilePath,&winFindFileData);

		if (hDirectory == INVALID_HANDLE_VALUE)
		{
			if (!CreateDirectory(strNetFilePath,NULL))
			{
				strTmptValue.Format("不能创建data子目录%s",strNetFilePath);
				MessageBox(strTmptValue, "目录错误", MB_OK | MB_ICONERROR);
				return FALSE;
			}
		}
		else
			FindClose(hDirectory);

//		strNetFilePath.Format("%s\\",m_strNetFile);
		strNetFilePath += "\\";
		strNetFilePath += strSubdir0;
        hDirectory = FindFirstFile(strNetFilePath,&winFindFileData);

		if (hDirectory == INVALID_HANDLE_VALUE)
		{
			if (!CreateDirectory(strNetFilePath,NULL))
			{
				strTmptValue = "不能创建'" + strSubdir0 + "'子目录";
				MessageBox(strTmptValue, "目录错误", MB_OK | MB_ICONERROR);
				return FALSE;
			}
		}
		else
			FindClose(hDirectory);

		strNetFilePath += "\\" + strSubdir1;

		hDirectory = FindFirstFile(strNetFilePath, &winFindFileData);
		if (INVALID_HANDLE_VALUE == hDirectory)					
		{		
			if (!CreateDirectory(strNetFilePath, NULL))
			{
		//		strmsg.Format("不能创建文件%s", strNetFilePath);
		//		MessageBox(strmsg, "文件错误", MB_OK | MB_ICONERROR);
		//		return FALSE;		// create subdir fail
			}
		}
		else
		   FindClose(hDirectory);

		strNetFilePath += "\\" + strSubdir2;
		
		hDirectory = FindFirstFile(strNetFilePath, &winFindFileData);
		if (INVALID_HANDLE_VALUE == hDirectory)					
		{

			if (!CreateDirectory(strNetFilePath, NULL))
			{
			//	strmsg.Format("不能创建文件%s", strNetFilePath);
			//	MessageBox(strmsg, "文件错误", MB_OK | MB_ICONERROR);
			//	return FALSE;		// create subdir fail
			}
		}
		else
		   FindClose(hDirectory);
		
        CString strVolFile;
		strVolFile = strNetFilePath + "\\BIN";
	
		hDirectory = FindFirstFile(strVolFile, &winFindFileData);
		if (INVALID_HANDLE_VALUE == hDirectory)					
		{
			if (!CreateDirectory(strVolFile, NULL))
			{
				strmsg.Format("不能创建文件%s", strVolFile);
				MessageBox(strmsg, "文件错误", MB_OK | MB_ICONERROR);
				return FALSE;		// create subdir fail
			}
		}
		else
			FindClose(hDirectory);

	}

//本地路径
	if (m_strSN.IsEmpty())
	{
		MessageBox("请输入产品SN号!", "SN号输入", MB_OK | MB_ICONERROR);
		return  FALSE;
	}

	strFileName.Format("%s\\data", g_tszAppFolder);
	hDirectory = FindFirstFile(strFileName, &winFindFileData);
	
	if (INVALID_HANDLE_VALUE == hDirectory)
	{
		if (!CreateDirectory(strFileName, NULL))
		{
			strTmptValue = "不能创建data子目录";
			MessageBox(strTmptValue, "目录错误", MB_OK | MB_ICONERROR);			
			return FALSE;		// create subdir fail
		}
	}
	else
		FindClose(hDirectory);

	strFileName += "\\";
	strFileName += strSubdir0;

	hDirectory = FindFirstFile(strFileName, &winFindFileData);
	
	if (INVALID_HANDLE_VALUE == hDirectory)
	{
		if (!CreateDirectory(strFileName, NULL))
		{
			strTmptValue = "不能创建'" + strSubdir0 + "'子目录";
			MessageBox(strTmptValue, "目录错误", MB_OK | MB_ICONERROR);			
			return FALSE;		// create subdir fail
		}
	}
	else
		FindClose(hDirectory);

	strFileName += "\\" + strSubdir1;

	hDirectory = FindFirstFile(strFileName, &winFindFileData);
	if (INVALID_HANDLE_VALUE == hDirectory)					
	{
		if (!CreateDirectory(strFileName, NULL))
		{
			strmsg.Format("不能创建文件%s", strFileName);
			MessageBox(strmsg, "文件错误", MB_OK | MB_ICONERROR);
			return FALSE;		// create subdir fail
		}
	}
	else
		FindClose(hDirectory);

	strFileName += "\\" + strSubdir2;
	
	hDirectory = FindFirstFile(strFileName, &winFindFileData);
	if (INVALID_HANDLE_VALUE == hDirectory)					
	{
		if (!CreateDirectory(strFileName, NULL))
		{
			strmsg.Format("不能创建文件%s", strFileName);
			MessageBox(strmsg, "文件错误", MB_OK | MB_ICONERROR);
			return FALSE;		// create subdir fail
		}
	}
	else
		FindClose(hDirectory);

	//创建BIN目录
	CString strVolFile;

	strVolFile = strFileName + "\\BIN";

	hDirectory = FindFirstFile(strVolFile, &winFindFileData);
	if (INVALID_HANDLE_VALUE == hDirectory)					
	{
		if (!CreateDirectory(strVolFile, NULL))
		{
			MessageBox("不能创建bin目录文件!", "文件错误", MB_OK | MB_ICONERROR);
			return FALSE;		// create subdir fail
		}
	}
	else
		FindClose(hDirectory);

    return TRUE;
}

//依据最小点坐标计算角度值
VOID CMy126S_45V_Switch_AppDlg::CalculateAngle(int nVolX0,int nVolY0,int nVolX3,int nVolY3,int nVolX8,int nVolY8,int nVolX11,int nVolY11)
{
	int nXVolatage0;
	int nYVolatage0;
	int nXVolatage3;
	int nYVolatage3;
	int nXVolatage8;
	int nYVolatage8;
	int nXVolatage11;
	int nYVolatage11;
	int nAverageX1;
	int nAverageY1;
	int nAverageX2;
	int nAverageY2;
	int nAverageX;
	int nAverageY;
	CString strMsg1;
	CString strMsg2;
	CString strMsg3;
	double dblXVolatage0;
	double dblYVolatage0;
	double dblXVolatage3;
	double dblYVolatage3;
	double dblXVolatage8;
	double dblYVolatage8;
	double dblXVolatage11;
	double dblYVolatage11;
	
	nXVolatage0 = nVolX0;
	nYVolatage0 = nVolY0;
	nXVolatage3 = nVolX3;
	nYVolatage3 = nVolY3;
	nXVolatage8 = nVolX8;
	nYVolatage8 = nVolY8;
	nXVolatage11 = nVolX11;
	nYVolatage11 = nVolY11;

	dblXVolatage0 = nXVolatage0/1000;
	dblYVolatage0 = nYVolatage0/1000;
	dblXVolatage3 = nXVolatage3/1000;
	dblYVolatage3 = nYVolatage3/1000;
	dblXVolatage8 = nXVolatage8/1000;
	dblYVolatage8 = nYVolatage8/1000;
	dblXVolatage11 = nXVolatage11/1000;
	dblYVolatage11 = nYVolatage11/1000;

    nAverageX1 = sqrt((dblXVolatage0*dblXVolatage0+dblXVolatage3*dblXVolatage3)/2)*1000; //计算平均值
	nAverageY1 = sqrt((dblYVolatage0*dblYVolatage0+dblYVolatage8*dblYVolatage8)/2)*1000;

	nAverageX2 = sqrt((dblXVolatage8*dblXVolatage8+dblXVolatage11*dblXVolatage11)/2)*1000; //计算平均值
	nAverageY2 = sqrt((dblYVolatage3*dblYVolatage3+dblYVolatage11*dblYVolatage11)/2)*1000;
    
    nAverageX = (nAverageX1+nAverageX2)/2;
	nAverageY = (nAverageY1+nAverageY2)/2;

    m_ctrlListMsg.ResetContent();
	
	strMsg1.Format("电压值选取位置：(%d,%d)",nAverageX,nAverageY);

	//开始计算角度
	//计算比例系数
	nAverageY1 = nAverageY1/1000;
	nAverageX1 = nAverageX1/1000;
	nAverageY2 = nAverageY1/1000;
	nAverageX2 = nAverageX1/1000;
	double dblScale;
    dblScale = (nAverageY1*nAverageY1+nAverageY2*nAverageY2)/2*1.5;
	dblScale = dblScale/((nAverageX1*nAverageX1+nAverageX2*nAverageX2)/2);

	//计算转角
	double dblXAngle0;
	double dblXAngle3;
	double dblXAngle8;
	double dblXAngle11;
	double dblYAngle0;
	double dblYAngle3;
	double dblYAngle8;
	double dblYAngle11;
	
	dblXAngle0 = -nXVolatage0*nXVolatage0*dblScale;
	dblXAngle3 = nXVolatage3*nXVolatage3*dblScale;
	dblXAngle8 = -nXVolatage8*nXVolatage8*dblScale;
	dblXAngle11 = nXVolatage11*nXVolatage11*dblScale;

    dblYAngle0 = nYVolatage0*nYVolatage0;
	dblYAngle3 = nYVolatage3*nYVolatage3;
	dblYAngle8 = -nYVolatage8*nYVolatage8;
	dblYAngle11 = -nYVolatage11*nYVolatage11;
	
	//计算斜率
	double dblKX1;
	double dblKY1;
	double dblKX2;
	double dblKY2;
	double dblAngleX1;
	double dblAngleX2;
	double dblAngleY1;
	double dblAngleY2;

    dblKX1 = (dblYAngle3-dblYAngle0)/(dblXAngle3-dblXAngle0);
	dblKX2 = (dblYAngle11-dblYAngle8)/(dblXAngle11-dblXAngle8);

	dblAngleX1 = atan(dblKX1)/PI*180;
	dblAngleX2 = atan(dblKX2)/PI*180;

	strMsg2.Format("X需要转动的角度分别为：%.4f,%.4f",dblAngleX1,dblAngleX2);

	if (dblXAngle8 == dblXAngle0)
	{
		dblAngleY1 = 90;
	}
	else
	{
		dblKY1 = (dblYAngle8-dblYAngle0)/(dblXAngle8-dblXAngle0);
		dblAngleY1 = atan(dblKY1)/PI*180;
	}

	if (dblXAngle11 == dblXAngle3)
	{
		dblAngleY2 = 90;
	}
	else
	{
		dblKY2 = (dblYAngle11-dblYAngle3)/(dblXAngle11-dblXAngle3);
		dblAngleY2 = atan(dblKY2)/PI*180;
	}

	if (dblAngleY1 != 0)
	{
		dblAngleY1 = (90-fabs(dblAngleY1))*(-dblAngleY1/fabs(dblAngleY1));
	}
	if (dblAngleY2 != 0)
	{
		dblAngleY2 = (90-fabs(dblAngleY2))*(-dblAngleY2/fabs(dblAngleY2));
	}
	
	strMsg3.Format("Y需要转动的角度分别为：%.4f,%.4f",dblAngleY1,dblAngleY2);
	LogInfo(strMsg1);
	LogInfo(strMsg2);
	LogInfo(strMsg3);
	MessageBox(strMsg1);
	strMsg2 = strMsg2+"\n"+strMsg3;
	MessageBox(strMsg2);
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonSingleChannelScan() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	int x;
	int y;
	int step;
	int count = 0;
	int x_count = 0;
	int nCount = 0;
	double  dblPower = 0; 
	double  dblMaxPower = -100;
	BOOL    bFlag1 = FALSE;
	BOOL    bFlag2 = FALSE;
	BOOL    bFlag3 = FALSE;
	BOOL    bFlag4 = FALSE;
	CString strMsg;
	int x_start = 0;
	int x_stop = 0;
	int x_end = 0;
	int y_end = 0;
	int y_start = 0;
	int y_stop = 0;
	int ich ;
	double dblPrePower = 100;
    char    chValue[10];
	ZeroMemory(chValue,sizeof(char)*10);

	CString strINIFile;
	CString strValue;
	CString strStartX;
	CString strEndX;
	CString strStartY;
	CString strEndY;
	CString strX;
	CString strY;
	CString strPower;
	CString strFile;
	CStdioFile stdFile;	

	UpdateData();
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}

	ich = m_cbChannel.GetCurSel();

	if (ich==-1)
	{
		MessageBox("请先选择通道！");
		return;
	}

	m_bTestStop = FALSE;

	//设置路径
	UpdateData();
	
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);
    ///////////////////////////////////////////扫描算法

	//获取配置文件
	strINIFile.Format("%s\\SingleChannel.ini", m_strNetConfigPath);
	
	//获取配置文件里面的值
	DWORD nLen1 = GetPrivateProfileString("XRange","X_Start",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
		x_start = atoi(strValue);

	DWORD nLen2 = GetPrivateProfileString("XRange","X_End",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	x_end = atoi(strValue);

	DWORD nLen3 = GetPrivateProfileString("YRange","Y_Start",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	y_start = atoi(strValue);
	
	DWORD nLen4 = GetPrivateProfileString("YRange","Y_End",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	y_end = atoi(strValue);

	DWORD nLen5 = GetPrivateProfileString("Step","step",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	step = atoi(strValue);

	//初始化图形控件
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("X(mv)");
	m_NTGraph.SetYLabel("Y(mv)");
	m_NTGraph.SetRange(-45000,45000,-45000,45000);
	m_NTGraph.SetXGridNumber(10);
	m_NTGraph.SetYGridNumber(10);

	strFile.Format("%s\\data\\%s_SingleChannel_%d.csv",g_tszAppFolder,m_strSN,ich+1);
		
	if(!stdFile.Open(strFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建文件失败！");
		return;
	}
	int nValue = -40000;
	int i=0;

	stdFile.WriteString(",");
	nValue = x_start;
	while(nValue<=x_end)
	{
		i++;
		strValue.Format("%d,",nValue);
	    stdFile.WriteString(strValue);
		nValue = x_start+i*step;		
	}

	stdFile.WriteString(strValue);

	//扫描出所有的值
	for (y=y_start;y<=y_end;y=y+step)
	{
		for (x=x_start;x<=x_end;x=x+step)
		{
			YieldToPeers();
			if (m_bTestStop)
			{
				MessageBox("测试中止!");
				stdFile.Close();
				return ;
			}
			SETX(x);
			Sleep(50);
			SETY(y);
			Sleep(250);
			m_NTGraph.PlotXY(x,y,0);

			dblPower = ReadPWMPower(ich) -m_dblReferencePower[ich];
			//跳变时，等待
			if (x==x_start)
			{
				Sleep(200);
				strValue.Format("\n%d,%.2f,",y,dblPower);
				stdFile.WriteString(strValue);
			}
			else
			{
				strValue.Format("%.2f,",dblPower);
			   stdFile.WriteString(strValue);

			}
	
			strMsg.Format("(%d,%d)；IL,%.2f\n",x,y,dblPower);
			LogInfo(strMsg);
		}
	}
	stdFile.Close();
    
	MessageBox("扫描完毕！");
	LogInfo("扫描完毕！");
}

void CMy126S_45V_Switch_AppDlg::OnButtonRegulateScan2() 
{
	// TODO: Add your control notification handler code here

	if (!m_bOpenPM)
	{
		MessageBox("请先打开1830C！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}
	
	CString strMsg;
	strMsg.Format("请连接好光路\n请将通道1，4，9，12按顺序接入光功率计！");
	MessageBox(strMsg);
	//设置路径
	UpdateData();
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);
	
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("X(mv)");
	m_NTGraph.SetYLabel("Y(mv)");
	m_NTGraph.SetRange(-45000,45000,-45000,45000);
	m_NTGraph.SetXGridNumber(10);
	m_NTGraph.SetYGridNumber(10);
	
	int iSel = IDYES;
	
	CString strFileINI;
	strFileINI = "ScanAdjustRange.ini";
	if (m_bAdjustScan)
	{
		strMsg.Format("已存在粗扫数据，是否重新粗扫？");
		iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);
	}
	if (iSel==IDYES)
	{
		if(!AutoScan(0,strFileINI,FALSE))
		{
			m_bAdjustScan = FALSE;
			return;
		}
		if(!AutoScan(1,strFileINI,FALSE))
		{
			m_bAdjustScan = FALSE;
			return;
		}
		if(!AutoScan(2,strFileINI,FALSE))
		{
			m_bAdjustScan = FALSE;
			return;
		}
		if(!AutoScan(3,strFileINI,FALSE))
		{
			m_bAdjustScan = FALSE;
			return;
		}
		m_bAdjustScan = TRUE;
		
	}
	
	CalculateAngle(m_Xbase[0],m_Ybase[0],m_Xbase[1],m_Ybase[1],m_Xbase[2],m_Ybase[2],m_Xbase[3],m_Ybase[3]);
    
	m_NTGraph.ClearGraph();
	
	if(!AutoPointScan(0))
	{
		MessageBox("扫描通道1出错！");
		return;
	}
	if(!AutoPointScan(1))
	{
		MessageBox("扫描通道4出错！");
		return;
	}
	if(!AutoPointScan(2))
	{
		MessageBox("扫描通道9出错！");
		return;
	}
	if(!AutoPointScan(3))
	{
		MessageBox("扫描通道12出错！");
		return;
	}
	//扫描结束，计算出转角
	CalculateAngle(ScanPowerChannel[0].VoltageX,ScanPowerChannel[0].VoltageY,ScanPowerChannel[1].VoltageX,ScanPowerChannel[1].VoltageY,
		ScanPowerChannel[2].VoltageX,ScanPowerChannel[2].VoltageY,ScanPowerChannel[3].VoltageX,ScanPowerChannel[3].VoltageY);
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonConnect1830() 
{
	// TODO: Add your control notification handler code here
	//初始化设备

	CString  strFile;
	CString  strWL;
	CString  strPower;
	CString  strPMcomport;
	CString  strMsg;
	CString  strKey;
	int      nPMcomport[4];
	DWORD     nLen;
	int i=0;

	ZeroMemory(nPMcomport,sizeof(nPMcomport));

	//1.获取1830C的通道配置
	strFile.Format("%s\\1830ComPort.INI", m_strNetConfigPath);
	for (i=0;i<4;i++)
	{
		strKey.Format("PM%d",i+1);
	    nLen = GetPrivateProfileString("1830C",strKey,
			NULL,strPMcomport.GetBuffer(128),128,strFile);
		if (nLen <= 0)
			throw("读取配置信息失败！");
    	nPMcomport[i] = atoi(strPMcomport);

	}
    //2.连接1830C
	stDeviceSetInfo stPM1830CInfo;
    stPM1830CInfo.bHasDevice = TRUE;
	stPM1830CInfo.nComType = SERIAL;
    stPM1830CInfo.deCaption = PM1830C;
	stPM1830CInfo.deType = PMDEVICE;
	stPM1830CInfo.dwBaud = 9600;

	for(i=0;i<4;i++)
	{
		stPM1830CInfo.nAddress = nPMcomport[i];
		m_PM1830C[i].InitialPM(&stPM1830CInfo);

		strMsg.Format("正在初始化第%d台光功率计1830C...",i+1);
		LogInfo(strMsg);
		
		if (!m_PM1830C[i].OpenDevice())
		{
			strMsg.Format("打开光功率计%d失败!",i+1);
			MessageBox(strMsg);
			return;
		}
			
//		m_PM1830C[i].SetPWMRangeMode(1,0,0);   
		Sleep(50);//AUTO
		m_PM1830C[i].SetPWMWavelength(1,0,m_pdblWavelength);
		Sleep(100);
//		m_PM1830C[i].SetPWMUnit(1,0,0);         //dBm
//		m_PM1830C[i].SetPWMParameters(1,0,0,1550,2,0);   //Medium
        strMsg.Format("打开光功率计%d成功！",i+1);
		LogInfo(strMsg);
		g_stPMCfg.iConnect[i]=3;
	}
	m_bOpenPM = TRUE;
	
}

BOOL CMy126S_45V_Switch_AppDlg::LoadPMCfg(pstPMCfg pstPMInfo)
{
	CString strFileName;
	CString strSection;
	CString strKey;
	CHAR    chTemp[128];
	strFileName.Format("%s\\%s-%s\\device-14538.ini", m_strPNConfigPath,m_strSpec,m_strPN);
	for(int i=0; i<3; i++)
	{
		strSection.Format("PM%d",i);
		strKey.Format("TYPE");
		ZeroMemory(chTemp,128);
		GetPrivateProfileString(strSection,strKey,"0",chTemp,128,strFileName);
		pstPMInfo->iType[i]=atoi(chTemp);

		strKey.Format("CONNECT");
		ZeroMemory(chTemp,128);
		GetPrivateProfileString(strSection,strKey,"0",chTemp,128,strFileName);
		pstPMInfo->iConnect[i]=atoi(chTemp);

		strKey.Format("ADDRESS");
		ZeroMemory(chTemp,128);
		GetPrivateProfileString(strSection,strKey,"0",chTemp,128,strFileName);
		pstPMInfo->iAdderess[i]=(unsigned int)atoi(chTemp);
	}

	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::GetPower(int iBox, int iChannel,double *dblPower)
{
	int iSlot;
	
	if(g_stPMCfg.iConnect[iBox]==CONNECT_NET)//R152//N7744
	{
		iSlot=iChannel/2;
		iChannel=iChannel%2;

		if(g_stPMCfg.iType[iBox]==TYPE_OPLINK_PM)
		{
			Sleep(100);
			if(!GetPowerFromIP(iBox,iSlot,iChannel,dblPower))
			{
				LogInfo("连接超时！");
				return FALSE;
			}
		}
	}
	else if(g_stPMCfg.iConnect[iBox]==CONNECT_GPIB)//N7744
	{
		Sleep(250);
		if(!m_PMN7744[iBox].ReadPower(iChannel+1,iChannel+1,dblPower))
		{
			LogInfo("从GPIB端口读取功率错误！");
			return FALSE;
		}
	}
	else
	{

		//1830C
		if (!m_PM1830C[iChannel].ReadPower(1,0,dblPower))
		{
			return FALSE;
		}
	}	
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnButtonZero() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;

	for(int i=0; i<3; i++)
	{
		if(g_stPMCfg.iType[i]==TYPE_N7744)
		{
			if(g_stPMCfg.bPMOpen[i])
			{
				for(int j=0;j<4;j++)
				{
					strMsg.Format("是否要对第%d个功率计的第%d通道进行清零？,如果是，请对该通道遮光。",i,j);

					if(IDYES==MessageBox(strMsg,"确认",MB_YESNO |MB_ICONQUESTION))
					{
						if(!m_PMN7744[i].Zero(j))
						{
							strMsg.Format("清零第%d个功率计的第%d通道失败！",i,j);
							LogInfo(strMsg);
						}
					}
				}
			}
			else
			{
				strMsg.Format("第%d个功率计没有连接",i);
				LogInfo(strMsg);
			}
		}
	}

	LogInfo("清零完毕！");
	
}

void CMy126S_45V_Switch_AppDlg::SaveDataToExcel(int nType,int nChannel)
{
	CString strValue;
	CString strTemp;
	int nIndex=0;
	int nIndexCH=0;

	CString strIniFile;
	int     nSaveExcel;
	return;
	strIniFile.Format("%s\\config\\DataSave.ini",g_tszAppFolder);
	GetPrivateProfileString("SaveExcel","Value","ERROR",strValue.GetBuffer(128),128,strIniFile);
	if (strValue=="ERROR")
	{
		nSaveExcel = 0;
	}
	else
	{
		nSaveExcel= atoi(strValue);
	}
	if (nSaveExcel!=1)
	{
		LogInfo("数据不保存到EXCEL");
		return;
	}
	//启动EXCEL
	if(!CreateExcelFile())
	{
		return;
	}
	//选择表单
	m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("测试单")));
	m_range.AttachDispatch(m_sheet.GetCells(),true);
	if (m_cbComPort.GetCurSel()==1)
	{
		m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("测试单-COM1")));
	    m_range.AttachDispatch(m_sheet.GetCells(),true);
	}
	//保存的数据类型
	UpdateData();
	switch (nType)
	{
	case VOL_DATA:
		strTemp.Format("电压数据");
		if (nChannel==32)
		{
			 for (nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				strValue.Format("%d",ScanPowerChannel[nIndex].VoltageX);
				m_range.SetItem(_variant_t((long)(10)),_variant_t((long)(4+nIndex)),_variant_t(strValue));

				strValue.Format("%d",ScanPowerChannel[nIndex].VoltageY);
				m_range.SetItem(_variant_t((long)(12)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
			}
		}
		else
		{
			strValue.Format("%d",ScanPowerChannel[nChannel].VoltageX);
			m_range.SetItem(_variant_t((long)(10)),_variant_t((long)(4+nChannel)),_variant_t(strValue));

			strValue.Format("%d",ScanPowerChannel[nChannel].VoltageY);
			m_range.SetItem(_variant_t((long)(12)),_variant_t((long)(4+nChannel)),_variant_t(strValue));
		}
		break;
	case SWITCH_TIME_DATA: 
		strTemp.Format("开关切换时间数据");
    	strValue.Format("%.2f",m_dblSwitchTime);
		if (m_strItemName=="初测")
		{
			m_range.SetItem(_variant_t((long)(13)),_variant_t((long)(4+m_nSwitchTimeCH-1)),_variant_t(strValue));  
		}
		else if (m_strItemName=="终测")
		{
			m_range.SetItem(_variant_t((long)(13)),_variant_t((long)(4)),_variant_t(strValue));  
		}		    
		break;
	case IL_ROOM_DATA:
		strTemp.Format("常温IL数据");
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblIL);
			m_range.SetItem(_variant_t((long)(14)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
	case IL_LOW_DATA:
		strTemp.Format("低温IL数据");
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblLowTempIL);
			m_range.SetItem(_variant_t((long)(15)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
	case IL_HIGH_DATA:
		strTemp.Format("高温IL数据");
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblHighTempIL);
			m_range.SetItem(_variant_t((long)(16)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
	case WDL_DATA:
		strTemp.Format("WDL数据");
		if (m_ctrlComboxTemperature.GetCurSel()==0)
		{
			nIndexCH = 17;
		}
		else if (m_ctrlComboxTemperature.GetCurSel()==1)
		{
			nIndexCH = 18;
		}
		else
		{
			nIndexCH = 19;
		}
		if (nChannel==32)
		{
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				strValue.Format("%.2f",ChannalMessage[nIndex].dblWDL);
				m_range.SetItem(_variant_t((long)(nIndexCH)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
			}
		}
		else
		{
			strValue.Format("%.2f",ChannalMessage[nChannel].dblWDL);
			m_range.SetItem(_variant_t((long)(nIndexCH)),_variant_t((long)(4+nChannel)),_variant_t(strValue));
		}
		
		break;
	case TDL_DATA:
		strTemp.Format("TDL数据");
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblTDL);
			m_range.SetItem(_variant_t((long)(20)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
	case PDL_DATA:
		strTemp.Format("PDL数据");
		if (nChannel==32)
		{
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				strValue.Format("%.2f",ChannalMessage[nIndex].dblPDL);
				m_range.SetItem(_variant_t((long)(21)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
			}
		}
		else
		{
			strValue.Format("%.2f",ChannalMessage[nChannel].dblPDL);
			m_range.SetItem(_variant_t((long)(21)),_variant_t((long)(4+nChannel)),_variant_t(strValue));
		}
		
		break;
	case RE_IL_DATA:
		strTemp.Format("重复性数据");
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblDif);
			m_range.SetItem(_variant_t((long)(22)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
	case RL_DATA:
		strTemp.Format("RL数据");
		if (nChannel==32) 
		{
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				strValue.Format("%.2f",ChannalMessage[nIndex].dblRL);
				m_range.SetItem(_variant_t((long)(23)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
			}
		}
		else
		{
			strValue.Format("%.2f",ChannalMessage[nChannel].dblRL);
			m_range.SetItem(_variant_t((long)(23)),_variant_t((long)(4+nChannel)),_variant_t(strValue));
		}
		break;
	case CT_DATA:
		strTemp.Format("CT数据");
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			for(nIndexCH=0;nIndexCH<m_nCHCount;nIndexCH++)
			{
				strValue.Format("%.2f",ChannalMessage[nIndex].dblCT[nIndexCH]);
			    m_range.SetItem(_variant_t((long)(24+nIndex)),_variant_t((long)(4+nIndexCH)),_variant_t(strValue));
			}		
		}
		break;
	case ADJUST_DATA:
		strTemp.Format("电压数据");
		//选择表单
		m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("调节测试单")));
    	m_range.AttachDispatch(m_sheet.GetCells(),true);
		if (m_cbSaveData.GetCurSel()==0)
		{
			for (nIndex=0;nIndex<4;nIndex++)
			{
				strValue.Format("%.2f",ScanPowerChannel[nIndex].MaxPower);
				m_range.SetItem(_variant_t((long)(9+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
				strValue.Format("%d",ScanPowerChannel[nIndex].VoltageX);
				m_range.SetItem(_variant_t((long)(10+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
				strValue.Format("%d",ScanPowerChannel[nIndex].VoltageY);
				m_range.SetItem(_variant_t((long)(11+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
			}
		}
		else if(m_cbSaveData.GetCurSel()==1)
		{
			for (nIndex=0;nIndex<4;nIndex++)
			{
				strValue.Format("%.2f",ScanPowerChannel[nIndex].MaxPower);
				m_range.SetItem(_variant_t((long)(23+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
				strValue.Format("%d",ScanPowerChannel[nIndex].VoltageX);
				m_range.SetItem(_variant_t((long)(24+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
				strValue.Format("%d",ScanPowerChannel[nIndex].VoltageY);
				m_range.SetItem(_variant_t((long)(25+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
			}
		}
		break;
	case ISO_DATA:
		strTemp.Format("ISO数据");
		for (nIndex = 0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblISO);
			m_range.SetItem(_variant_t((long)(40)),_variant_t((long)(nIndex+4)),_variant_t(strValue));	
		}
		break;
	case DARK_DATA:
		strTemp.Format("DARK值数据");
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			if (m_nCHCount<=12)
			{
				strValue.Format("%.2f",ChannalMessage[nIndex].dblDarkIL);
				m_range.SetItem(_variant_t((long)(36)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
			}
			else
			{
				strValue.Format("%.2f",ChannalMessage[nIndex].dblDarkIL);
				m_range.SetItem(_variant_t((long)(24+m_nCHCount)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
			}
		}
	default:
		break;
		
	}
	//退出EXCEL
	CloseExcelFile();

    //拷贝到本地
	CString strLocalExcel;
	CString strMsg;
	strLocalExcel.Format("%s\\data\\%s\\%s\\%s\\%s_Optical_Test_Data.xls",g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);
	if (m_bIfSaveToServe)
	{
		CopyFile(m_strOpticalDataPath,strLocalExcel,FALSE);
	}	
	strMsg.Format("保存%s到Excel成功！",strTemp);
	LogInfo(strMsg);

}

VOID CMy126S_45V_Switch_AppDlg::CalCulateVol()
{
	double dblCoefVx;
	double dblCoefVy;
	double Vx[4];
	double Vy[4];
	int  RefVx[4];
	int  RefVy[4];
	CString strMsg;

	for (int i=0;i<4;i++)
	{
		Vx[i] = (double)ScanPowerChannel[i].VoltageX;
		Vy[i] = (double)ScanPowerChannel[i].VoltageY;
	}

	//计算Vx,Vy比例系数	
//	dblCoefVx = (Vx[2]*Vx[2]-Vx[0]*Vx[0])/(Vx[1]*Vx[1]-Vx[3]*Vx[3]);
//	dblCoefVy = (Vy[1]*Vy[1]-Vy[0]*Vy[0])/(Vy[2]*Vy[2]-Vy[3]*Vy[3]);
	dblCoefVx = 1;
	dblCoefVy = 1;
	
	//X,Y电压建议值
    RefVx[0] = sqrt((Vx[0]*Vx[0]+dblCoefVx*Vx[3]*Vx[3])/2);
	RefVx[2] = sqrt((Vx[2]*Vx[2]+dblCoefVx*Vx[1]*Vx[1])/2);
	RefVx[1] = sqrt(RefVx[2]*RefVx[2]/dblCoefVx);
	RefVx[3] = sqrt(RefVx[0]*RefVx[0]/dblCoefVx);

	RefVy[0] = sqrt((Vy[0]*Vy[0]+dblCoefVy*Vy[3]*Vy[3])/2);
	RefVy[1] = sqrt((Vy[1]*Vy[1]+dblCoefVy*Vy[2]*Vy[2])/2);
	RefVy[2] = sqrt(RefVy[1]*RefVy[1]/dblCoefVy);
	RefVy[3] = sqrt(RefVy[0]*RefVy[0]/dblCoefVy);

	for (int i=0;i<4;i++)
	{
		strMsg.Format("建议电压值%d为：(%d,%d)",i+1,RefVx[i],RefVy[i]);
		LogInfo(strMsg);
	}
    
}

void CMy126S_45V_Switch_AppDlg::OnButtonSetChannelVol() 
{
	// TODO: Add your control notification handler code here
	CString strFilePath;
	CString strMsg;
	CString strFile;
	CStdioFile stdFile;
	LPTSTR  lpStr;
	CString strToken;
	char    sep[] = ",";
	char    chReadBuffer[MAX_LINE];
	int     nVolX[12];
	int     nVolY[12];
	CString strCommand;

	ZeroMemory(chReadBuffer,sizeof(chReadBuffer));
	ZeroMemory(nVolX,sizeof(nVolX));
	ZeroMemory(nVolY,sizeof(nVolY));

	UpdateData();
	if (m_strSN2.IsEmpty()==1)
	{
		MessageBox("请先填入调用文件名的SN号！");
		return;
	}
	strFilePath.Format("%s\\data\\%s\\%s\\%s_ScanVoltage.csv", m_strNetFile,m_strPN,m_strSN2,m_strSN2);
	if (!stdFile.Open(strFilePath,CFile::modeRead))
	{
		strMsg.Format("打开文件%s失败！",strFilePath);
		MessageBox(strMsg);
		return;
	}
	lpStr = stdFile.ReadString(chReadBuffer,MAX_LINE);
	for(int i=0;i<12;i++)
	{
		lpStr = stdFile.ReadString(chReadBuffer,MAX_LINE);
	    strToken = strtok(chReadBuffer,sep);
		strToken = strtok(NULL,sep);
		nVolX[i] = atoi(strToken);
		strToken = strtok(NULL,sep);
		nVolY[i] = atoi(strToken);
	}
	stdFile.Close();

    //设置电压值
	for (int i=0;i<12;i++)
	{
		if (!SCHN(i+1,nVolX[i],nVolY[i]))
		{
			MessageBox("设置电压坐标出错！");
			return;
		}
		strMsg.Format("设置通道%d,坐标(%d,%d)",i+1,nVolX[i],nVolY[i]);
		LogInfo(strMsg);
	}

}

void CMy126S_45V_Switch_AppDlg::OnButtonRlTest() 
{
	// TODO: Add your control notification handler code here
	int nChannel = 0;
	int i=0;
	UpdateData();
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bRefRL)
	{
		MessageBox("请先做RL系统归零！");
		return;
	}
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	//
	int  nWL = m_cbSetWL.GetCurSel()-1;
	//if (m_bConnect1830C)
	//{
		if (nWL>=0)
		{
		//	if(!m_1830CRL.SetPWMParameters(0,1,1,m_dblWL[nWL],0.01,0))
		//	{
		//		LogInfo("设置1830C参数失败！");
		//		return;
		//	}
			if(!SetPWMWL(m_dblWL[nWL],19))
				{
					LogInfo("设置波长失败！");
				}
		}
		else
		{
			//if(!m_1830CRL.SetPWMParameters(0,1,1,m_pdblWavelength,0.01,0))
			//{
			//	LogInfo("设置1830C参数失败！");
			//	return;
			//}
				if(!SetPWMWL(m_pdblWavelength,19))
				{
					LogInfo("设置波长失败！");
				}
		}
		
	//}
	double dblPower;
	double dblMaxPower = -99;
	double dblRLSD;
	double dblRLS;
	CString strMsg;
	CString dwBaseAddress,m_strchan,strCommand;
	LogInfo("开始测试产品RL,请等待......");
	MessageBox("请绕死产品所有的出光端，并将回损头接入20通道");
//	if (b_issn1)
//	{
//		m_nCHCount = 18;
//	}
//	else
//	{
//		m_nCHCount = 16;
//
//	}
	    	/*TxDataToI2C("00D6",(BYTE*)"MFGCMD",6);
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}

			Sleep(100);
			RxDataFromI2C("00DC"); //Command Status
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}*/
            

	for (nChannel = 0;nChannel<m_nCHCount;nChannel++)
	{
		dblMaxPower=-100;
		//SWToChannel(nChannel+1);
		 if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return ;
		}

		if (nChannel+1>15)
		{
			 m_strchan.Format("%x",nChannel+1);

		}
		else
		{
			m_strchan.Format("0%x",nChannel+1);
		}
        
		if(!SetSWChannel(atoi(dwBaseAddress),nChannel+1))
		{
			LogInfo("设置SWChannel失败!",FALSE,COLOR_RED);
			return ;
		}
		
		/*strCommand.Format("%s%s",dwBaseAddress,m_strchan); 
		
		TxSTRToI2C("0143",strCommand); //Set Single MemsSwitch Channel
        
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return ;
		}
		Sleep(100);
		RxDataFromI2C("00DC"); //Command Status
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return ;
		}*/
		
		Sleep(100);
		for (i=0;i<5;i++)  //读取5次，取RL最差值
		{
			YieldToPeers();
			//m_1830CRL.ReadPower(0,0,&dblPower);
			dblPower = ReadPWMPower(19);
			Sleep(100);
			if (dblPower>dblMaxPower)
			{
				dblMaxPower = dblPower;
			}
		}
		//计算RL
		if (nWL>=0)
		{
			dblRLSD = m_dblWDLRef[nChannel][nWL] - dblMaxPower;
		}
		else
		{
			dblRLSD = m_dblReferencePower[nChannel] - dblMaxPower;
		}
	
		dblRLS = pow(10,-m_dblSystemRL/10.0);
		dblRLSD = pow(10,-dblRLSD/10.0);
		if (dblRLSD<dblRLS)
		{
			ChannalMessage[nChannel].dblRL= 58;
		}
		else if (dblRLSD==dblRLS)
		{
			ChannalMessage[nChannel].dblRL= 62;

		}
		else
		{
			ChannalMessage[nChannel].dblRL = -3.01-10.0*log10(dblRLSD-dblRLS);
		}
		
		if(!CheckParaPassOrFail(RL_DATA,ChannalMessage[nChannel].dblRL))
		{
			strMsg.Format("通道%d的RL为：%.1f dB,参数不合格！",nChannel+1,ChannalMessage[nChannel].dblRL);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("通道%d的RL为：%.1f dB",nChannel+1,ChannalMessage[nChannel].dblRL);
	    	LogInfo(strMsg);
		}			
	}
	LogInfo("RL测试完毕！");
//	SaveDataToExcel(RL_DATA);
	SaveDataToCSV(RL_DATA);
	//UpdateATMData(RL_DATA);
}

void CMy126S_45V_Switch_AppDlg::OnButtonSwitchTime() 
{
	// TODO: Add your control notification handler code here
	int nChannel;
	nChannel = 0;
	SWToChannel(nChannel);
	nChannel = 11;
	SWToChannel(nChannel,TRUE);
	
	//获取数据
	CString strCommand = "GMON\r";
	char    chWrite[10];
	WORD    wRead[2048];
	CString strValue;
//	int     nValue;
//	double  dblValue;
	CString strInfo;
//	int     nRead[100000];
	
	ZeroMemory(chWrite,sizeof(chWrite));
	ZeroMemory(wRead,sizeof(wRead));
    memcpy(chWrite,strCommand,strCommand.GetLength());
	
	if(!m_opCom.WriteBuffer(chWrite,strCommand.GetLength()))
	{
		MessageBox("串口打开错误！");
		return;
	}
	Sleep(100);
	if (!m_opCom.ReadBuffer((char*)wRead,2048*sizeof(WORD)))
	{
		MessageBox("接收数据错误！");
		return;
	}
	m_NTGraph.ClearGraph();
	int nMAX=0;
	for (int j=1;j<wRead[0];j++)
	{
		if(nMAX<wRead[j])
		{
			nMAX = wRead[j];
		}
	}
	m_NTGraph.SetRange(-10,1024,-10,nMAX+10);
	for (int i=0;i<wRead[0];i++)
	{
		m_NTGraph.PlotXY(i,wRead[i+1],0);
	}
	SaveDataToExcel(SWITCH_TIME_DATA);
}

//com14进光-12
//还是com15进光-9
BOOL CMy126S_45V_Switch_AppDlg::GetVolDataFormFile(int InputPort)
{
	CString strFile;
	CStdioFile stdFile;
	char chBuffer[256];
	CString strValue;
	CString strMsg;
	CString strPath;

	CString strCHSelINI;
	CStdioFile stdINIFile;
	CSelectCH  dlgSelect;
	CString    strKey;

	if (m_bIfSaveToServe)  
	{
		strPath = m_strNetFile;  //调用网路数据
	}
	else
	{
		strPath = g_tszAppFolder;
	}


	CString strVoltPathFile;
	strVoltPathFile.Format("%s\\config\\DataSave.ini",g_tszAppFolder);
	GetPrivateProfileString("VoltData","Path","ERROR",strValue.GetBuffer(128),128,strVoltPathFile);
	if (strValue == "ERROR")
	{
		strMsg.Format("打开文件%s失败！",strVoltPathFile);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	if (m_cbComPort.GetCurSel()==0)
	{
		strFile.Format("%s\\%s\\%s_ScanVoltage_%d.csv",strValue,m_strSN,m_strSN,InputPort+1);
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strFile.Format("%s\\%s\\%s_ScanVoltage_%d-COM1.csv",strValue,m_strSN,m_strSN,InputPort+1);
	}		
	if(!stdFile.Open(strFile,CFile::modeRead))
	{
		strMsg.Format("打开文件%s失败！",strFile);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	stdFile.ReadString(chBuffer,256);
//	for(int nIndex=0;nIndex<1;nIndex++)
	{
		stdFile.ReadString(chBuffer,256);
		strValue = strtok(chBuffer,",");
		strValue = strtok(NULL,",");
		m_Xbase[InputPort] = atoi(strValue);
		strValue = strtok(NULL,",");
		m_Ybase[InputPort] = atoi(strValue);
		strValue = strtok(NULL,",");
		m_dbInitialIL[InputPort] = atof(strValue);  //添加初测的IL值，以便和终测数据对比
	}
	stdFile.Close();
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnChangeEditSn() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	for (int n=0;n<2;n++) 
	{
		for (int i=0;i<16;i++)
		{
			m_dwSingle[n][i] = 0;
		}
	}
	
	
}

BOOL CMy126S_45V_Switch_AppDlg::ConnectPM()
{
	CString strInfo;
	CString strCom;
	CString strTemp;
	TCHAR   tchCom[15];
	
	for(int i=0;i<m_nPMCount;i++)
	{
		if (m_nPMType[i] == PM_8163A)
		{
			if(!m_HP816X1.SetPWMUnit(i,i%2+1,0))
			{
				 LogInfo("设置激光器参数失败！",FALSE,COLOR_RED);
				 return FALSE;
			}	
		}
		else if (m_nPMType[i] == PM_1830C)
		{
			stDeviceSetInfo stPM1830CInfo;
			stPM1830CInfo.bHasDevice = TRUE;
			stPM1830CInfo.nComType = SERIAL;
			stPM1830CInfo.deCaption = PM1830C;
			stPM1830CInfo.deType = PMDEVICE;
			stPM1830CInfo.dwBaud = 9600;
			
			for(i=0;i<m_nPMCount;i++)
			{
				stPM1830CInfo.nAddress = m_nPMAddress[i];
				m_PM1830C[i].InitialPM(&stPM1830CInfo);
				
				strInfo.Format("正在初始化第%d台光功率计1830C...",i+1);
				LogInfo(strInfo);
				
				if (!m_PM1830C[i].OpenDevice())
				{
					strInfo.Format("打开光功率计%d失败!",i+1);
					MessageBox(strInfo);
					return FALSE;
				}				
				m_PM1830C[i].SetPWMWavelength(1,0,m_pdblWavelength);
				Sleep(50);
				strInfo.Format("打开光功率计%d成功！",i+1);
				LogInfo(strInfo);
			}
			
		}
		else if (m_nPMType[i] == PM_OPLINK)
		{
			if (m_nPMConnect[i]==1)
			{
				ZeroMemory(tchCom,sizeof(tchCom));
				if (m_nPMAddress[i]>9)
				{
					strCom.Format("\\\\.\\COM%d",m_nPMAddress[i]);
				}
				else
				{
					strCom.Format("COM%d",m_nPMAddress[i]);
				}
				memcpy(tchCom,strCom,strCom.GetLength());
				if(!m_comPM[i].OpenPort(tchCom,115200))
				{
					strInfo.Format("打开串口%d失败！",m_nPMAddress[i]);
					LogInfo(strInfo);
					return FALSE;
				}
			}
			else if (m_nPMConnect[i]==0)
			{
				ConnectIP(i);
			}
			
		}
		else if (m_nPMType[i] == PM_N7744)
		{
			stDeviceSetInfo stPMInfo;
			if (m_nPMConnect[i]==0)
			{
				stPMInfo.bHasDevice = TRUE;
				stPMInfo.nComType = NET;
				stPMInfo.nGPIBIndex =0;
				strTemp.Format("%d.%d.%d.%d", (BYTE)(m_nPMAddress[i] >> 24), (BYTE)(m_nPMAddress[i]  >> 16), (BYTE)(m_nPMAddress[i] >> 8),(BYTE)(m_nPMAddress[i]));
				stPMInfo.nAddress = inet_addr(strTemp);
				stPMInfo.deCaption = N7745PM;
			    stPMInfo.deType = PMDEVICE;
			}
			else
			{
				stPMInfo.bHasDevice = TRUE;
				stPMInfo.nComType = GPIB;
				stPMInfo.nGPIBIndex = 0;
				if (i==2)
				{
					stPMInfo.nGPIBIndex = 1;
				}
				stPMInfo.nAddress = m_nPMAddress[i];
				//stPMInfo.dwBaud =9600;
				stPMInfo.deCaption = N7745PM;
			    stPMInfo.deType = PMDEVICE;
			}
						
			m_PMN7744[i].InitialPM(&stPMInfo);
			
			strInfo.Format("正在初始化第%d台光功率计N7744...",i+1);
			LogInfo(strInfo);
			
			if (!m_PMN7744[i].OpenDevice())
			{
				strInfo.Format("打开光功率计%d失败!",i+1);
				MessageBox(strInfo);
				return FALSE;
			}
			
			
			for(int j=1;j<=4;j++)
			{
				if(!m_PMN7744[i].SetPWMUnit(j,j,0))
				{
					strInfo.Format("设置单位到dBm失败！");
					LogInfo(strInfo,(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				if (!m_PMN7744[i].SetPWMAverageTime(j,0.01)) //平均时间：
				{
					LogInfo("设置平均读取速率失败！",(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				if (!m_PMN7744[i].SetPWMRangeMode(j,j,1)) 
				{
					LogInfo("设置AUTO RANGE失败！",(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
			}	
			strInfo.Format("打开N7744光功率计%d成功！",i+1);
			LogInfo(strInfo);
		}
		else if (m_nPMType[i] == PM_PDARRAY)
		{
			
			ZeroMemory(tchCom,sizeof(tchCom));
			if (m_nPMAddress[i]>9)
			{
				strCom.Format("\\\\.\\COM%d",m_nPMAddress[i]);
			}
			else
			{
				strCom.Format("COM%d",m_nPMAddress[i]);
			}
			memcpy(tchCom,strCom,strCom.GetLength());
			if(!m_comPM[i].OpenPort(tchCom,115200))
			{
				strInfo.Format("打开串口%d失败！",m_nPMAddress[i]);
				LogInfo(strInfo);
				return FALSE;
			}
			
		//	m_comPM[i].m_hCommPort = m_opCom.m_hCommPort;
		}
		
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::ConnectIP(int nPMIndex)
{
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	CString	strTemp;
	SOCKADDR_IN addrSrv;
	BYTE	bResult;
	DWORD	dwAddress;
	
	UpdateData();		
	wVersionRequested = MAKEWORD( 1, 1 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		return FALSE;
	}
	
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 )
	{
		WSACleanup();
		return FALSE;
	}
	g_socket_fd[nPMIndex] =socket(AF_INET,SOCK_DGRAM,0);
	
	dwAddress = m_nPMAddress[nPMIndex];
	strTemp.Format("%d.%d.%d.%d", (BYTE)(dwAddress >> 24), (BYTE)(dwAddress >> 16), (BYTE)(dwAddress >> 8),(BYTE)(dwAddress));
	addrSrv.sin_addr.S_un.S_addr = inet_addr(strTemp);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(8686);
	
	int iMode = 1;
	//	ioctlsocket(socket_fd1, FIONBIO,(u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
	//允许或禁止套接口的非阻塞模式  	
	bResult = connect(g_socket_fd[nPMIndex],(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	if (bResult == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CMy126S_45V_Switch_AppDlg::SetPWMWL(double dblWL,int nChannel)
{
	BYTE bSlot;
	BYTE bChannel;
	long lWL;
	int  i8163ACount = 0;
	int  i1830CCount = 0;
	int  iOplinkPMCount = 0;
	int  iN7744Count = 0;
	
	if (nChannel==32)  //设置所有通道波长
	{
		for(int i=0;i<m_nPMCount;i++)
		{
			if (m_nPMType[i] == PM_8163A)
			{
				if(!m_HP816X1.SetPWMWavelength(i/2+1,i%2,dblWL))
				{
					LogInfo("设置PM波长错误！",(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				m_HP816X1.SetPWMUnit(i/2+1,i%2,0);
				
			}
			else if (m_nPMType[i] == PM_1830C)
			{
				if(!m_PM1830C[i].SetPWMWavelength(1,0,dblWL))
				{
					LogInfo("设置1830C波长错误！",(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
			}
			else if (m_nPMType[i] == PM_OPLINK)
			{
				lWL = (long)(dblWL + 0.5);
				for (nChannel=0;nChannel<4;nChannel++)
				{
					bSlot = nChannel/2;
					bChannel = nChannel%2;		
					if (m_nPMConnect[i]==0)
					{
						if(!SetWLFromIP(g_socket_fd[i],bSlot,bChannel,lWL))
						{
							LogInfo("设置自制光功率计波长错误！",(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
					}
					else if (m_nPMConnect[i]==1)
					{
						if(!SetWLFromOPCom(i,bSlot,bChannel,lWL))
						{
							LogInfo("设置自制光功率计波长错误！",(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
					}
									
				}	
			}
			else if (m_nPMType[i] == PM_N7744)
			{
				for (nChannel=0;nChannel<4;nChannel++)
				{
					if(!m_PMN7744[i].SetPWMWavelength(nChannel+1,nChannel+1,dblWL))
					{
						LogInfo("设置N7744波长错误！",(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
				}	
			}
			else if (m_nPMType[i] == PM_PDARRAY) 
			{
				LogInfo("无需设置波长");
				return TRUE;
			}
		}	
	}
	else //设置单一通道波长
	{
		int i = 0;
		for (;i<m_nPMCount;i++)
		{
			if (m_nPMType[i]==PM_8163A) 
			{
				i8163ACount++;
			}
			else if (m_nPMType[i]==PM_1830C)
			{
				i1830CCount++;
			}
			else if (m_nPMType[i]==PM_OPLINK)
			{
				iOplinkPMCount++;
			}
			else if (m_nPMType[i]==PM_N7744)
			{
				iN7744Count++;
			}
			else if (m_nPMType[i] == PM_PDARRAY) 
			{
				LogInfo("无需设置波长");
				return TRUE;
			}
			else
			{
			}
		}
		if (nChannel < i8163ACount) 
		{
			if(!m_HP816X1.SetPWMWavelength(i/2+1,i%2,dblWL))
			{
				LogInfo("设置PM波长错误！",(BOOL)FALSE,COLOR_RED);
				return FALSE;
			}
		}
		else if (nChannel < (i8163ACount+i1830CCount))
		{
			if(!m_PM1830C[i].SetPWMWavelength(1,0,dblWL))
			{
				LogInfo("设置1830C波长错误！",(BOOL)FALSE,COLOR_RED);
				return FALSE;
			}
		}
		else if(nChannel < (i8163ACount+i1830CCount+iOplinkPMCount*4))
		{
			lWL = (long)(dblWL + 0.5);

			int iIndexOplink = (nChannel-(i1830CCount+i8163ACount))/4+(i1830CCount+i8163ACount);
			if ((nChannel-(i1830CCount+i8163ACount))/4==0)
			{
				if (m_nPMConnect[iIndexOplink]==0)
				{
					SetWLFromIP(g_socket_fd[iIndexOplink],(nChannel-(i1830CCount+i8163ACount))/2,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
				else if (m_nPMConnect[iIndexOplink]==1) 
				{
					SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/2,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
			}
			else if ((nChannel-(i1830CCount+i8163ACount))/4==1)
			{
				if (m_nPMConnect[iIndexOplink]==0)
				{
					SetWLFromIP(g_socket_fd[iIndexOplink],(nChannel-(i1830CCount+i8163ACount))/6,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
				else if (m_nPMConnect[iIndexOplink]==1) 
				{
					SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/6,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
			}
			else if ((nChannel-(i1830CCount+i8163ACount))/4==2)
			{
				if (m_nPMConnect[iIndexOplink]==0)
				{
					SetWLFromIP(g_socket_fd[iIndexOplink],(nChannel-(i1830CCount+i8163ACount))/10,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
				else if (m_nPMConnect[iIndexOplink]==1) 
				{
					SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/10,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
			}
			else if ((nChannel-(i1830CCount+i8163ACount))/4==3)
			{
				if (m_nPMConnect[iIndexOplink]==0)
				{
					SetWLFromIP(g_socket_fd[iIndexOplink],(nChannel-(i1830CCount+i8163ACount))/14,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
				else if (m_nPMConnect[iIndexOplink]==1) 
				{
					SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/14,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
			}
			else if ((nChannel-(i1830CCount+i8163ACount))/4==4)
			{
				if (m_nPMConnect[iIndexOplink]==0)
				{
					SetWLFromIP(g_socket_fd[iIndexOplink],(nChannel-(i1830CCount+i8163ACount))/18,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
				else if (m_nPMConnect[iIndexOplink]==1) 
				{
					SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/18,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
				}
			}

		}
		else if (nChannel < (i8163ACount+i1830CCount+iOplinkPMCount*4+iN7744Count*4))
		{
			if(!m_PMN7744[i].SetPWMWavelength(nChannel+1,nChannel+1,dblWL))
			{
				LogInfo("设置N7744波长错误！",(BOOL)FALSE,COLOR_RED);
				return FALSE;
			}
			int iIndexN7744 = (nChannel - (i1830CCount*1+i8163ACount*1+iOplinkPMCount*4))/4;
	    	int iCH = (nChannel - (i1830CCount*1+i8163ACount*1+iOplinkPMCount*4))%4;//0~3
			if(!m_PMN7744[iIndexN7744].SetPWMWavelength(iCH+1,iCH+1,dblWL))
			{
				LogInfo("设置N7744波长错误！",(BOOL)FALSE,COLOR_RED);
				return FALSE;
			}

		}
	}
	
    return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonWdlRef() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	double dblStartWL;
	double dblENDWL;
	double step;
	CString strValue;
	m_bTestStop = FALSE;

	CString strIniFile;
	strIniFile.Format("%s\\%s-%s\\WDLSET.INI", m_strPNConfigPath,m_strSpec,m_strPN);
	DWORD nLen1 = GetPrivateProfileString("WDL","STARTWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen1 <= 0)
		MessageBox("读取配置文件错误！");
    dblStartWL = atof(strValue);
	
	DWORD nLen2 = GetPrivateProfileString("WDL","STOPWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen2 <= 0)
		MessageBox("读取配置文件错误！");
    dblENDWL = atof(strValue);
	
	DWORD nLen3 = GetPrivateProfileString("WDL","STEP",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen2 <= 0)
		MessageBox("读取配置文件错误！");
    step = atof(strValue);

	int nWLCount = 0;
	int nWL = 0;

	double     dblReadPwr;
	CString strReferenceFileName(""),strMsg(""),strRemarkFlag("");
	CString strMessage,hCSVFile;
	int nCH;
	CString strInfo;
	LPTSTR pStr;
	char   sep[] = ",\t\n";
	char   *token = NULL;
	char   lineBuf[256];
	CString strTemp,strDataBeginFlag;
	CStdioFile fileReferenceFile;
	int iSel = IDYES;
	int nCount = 0;
	double  dblWL;
	if (!m_bOpen8164)
	{
		MessageBox("请打开激光器");
		return;			
	}

//	WORD wAmount;
//	WORD wTestCH[16];
//	GetTestChannelInfo(&wAmount,wTestCH);

	for (int i=0;i<m_nCHCount;i++)
	{
		strReferenceFileName.Format("%s\\data\\RefZero\\ReferenceZeroData-CH%d.txt",g_tszAppFolder,i+1);
		if (GetFileAttributes(strReferenceFileName) == -1)
		{
			iSel = IDNO;
			break;
		}
	}
	if (iSel==IDYES)
	{
		strMsg.Format("最近一次归零数据已存在\n"
			"点击<是>\t调用该数据\n"
			"点击<否>\t重新归零\n");
		iSel = MessageBox(strMsg,"确认",MB_YESNO |MB_ICONQUESTION);
	}

	m_ctrlListMsg.ResetContent();
	strRemarkFlag = ";";					
	if (IDYES == iSel)
	{
		for (int i=0;i<m_nCHCount;i++)
		{
			strReferenceFileName.Format("%s\\data\\RefZero\\ReferenceZeroData-CH%d.txt",g_tszAppFolder,i+1);
			if (!(fileReferenceFile.Open(strReferenceFileName,CFile::modeRead|CFile::typeText,NULL)))
			{
				MessageBox("打开文件失败！");
				return;
			}
			pStr = fileReferenceFile.ReadString(lineBuf,256);
			nCH = 0;
			nWL = 0;
			while (NULL != pStr)
			{
				strTemp.Format("%s",lineBuf);
				strTemp.Replace("\t"," ");
				strTemp.TrimLeft();
				strTemp.TrimRight();
				strRemarkFlag = strTemp.Left(1);
				strTemp.TrimRight();
				if (!strcmp(";",strRemarkFlag)||strTemp.IsEmpty())
				{
					pStr = fileReferenceFile.ReadString(lineBuf,256);
					continue;
				}
				//获取数据
				token = strtok(lineBuf,sep);
				strTemp.Format("%s",token);
				strTemp.TrimLeft();
				// nCH = strtol(strTemp,NULL,10) - 1;

				token = strtok(NULL,sep);
				strTemp.Format("%s",token);
				strTemp.TrimLeft();
				dblWL = atof(strTemp);

				token = strtok(NULL,sep);
				strTemp.Format("%s",token);
				strTemp.TrimLeft();

				if (fabs(dblWL-m_pdblWavelength)<=0.5)
				{
					m_dblReferencePower[i] = atof(strTemp);	
				}
				m_dblWDLRef[i][nWL] = atof(strTemp);
				
				//m_dblNewRef[wTestCH[nCH]] = m_dblReferencePower[nCH];
				pStr = fileReferenceFile.ReadString(lineBuf,256);
				strMsg.Format("通道%d:波长：%.1f nm,归零值为:%.2f",i+1,dblWL,m_dblWDLRef[i][nWL]);
				LogInfo(strMsg,FALSE);
				nWL++;
			}
		   fileReferenceFile.Close();

		}		
		 LogInfo("所有通道的归零已完成！");			
	}
	//重新归零
	else if (IDNO == iSel)
	{
		UpdateData();
		int i = m_cbChannel.GetCurSel();
		if (i==0)
		{
			MessageBox("请选择正确的通道！");
			return;
		}

		{
			YieldToPeers();
			{
				if (m_bTestStop)
				{
					return;
				}
			}
			strMsg.Format("请连接光源线接光功率计%d\n<连接好后点击确定>",i);
		    MessageBox(strMsg);
			strReferenceFileName.Format("%s\\data\\RefZero\\ReferenceZeroData-CH%d.txt",g_tszAppFolder,i);
			if(!fileReferenceFile.Open(strReferenceFileName,CFile::modeCreate|CFile::modeReadWrite,NULL))
			{
				MessageBox("创建文件失败！");
				return;
			}
			strMsg.Format("channel  WL  RefPower(dBm)\n");
			strMsg = strRemarkFlag + strMsg;
		    fileReferenceFile.WriteString(strMsg);
			nWL = 0;
			for( dblWL=dblStartWL;dblWL<=dblENDWL;dblWL = dblWL + step)
			{
				YieldToPeers();
				SetTLSWavelength(dblWL);
				SetPWMWL(dblWL,i-1);
				Sleep(100);
				dblReadPwr = ReadPWMPower(i-1);
				strInfo.Format("通道%d:波长：%.1f nm，归零光功率为：%.2fdBm",i,dblWL,dblReadPwr);
				LogInfo(strInfo,FALSE);
				m_dblWDLRef[i-1][nWL++] = dblReadPwr;
				if (fabs(dblWL-m_pdblWavelength)<=0.5)
				{
					m_dblReferencePower[i-1] = dblReadPwr;
				}

				//m_dblNewRef[wTestCH[i]] = m_dblReferencePower[i];	
				strMsg.Format("%d, %.1f ,  %.3f\n",i,dblWL,dblReadPwr);
		    	fileReferenceFile.WriteString(strMsg);
			}
			fileReferenceFile.Close();			
		}
		SetTLSWavelength(m_pdblWavelength);
	    if(m_bChange)
		{
			if (!SetPWMWL(m_pdblWavelength))
			{
				LogInfo("设置波长错误！");
				return;
			}
			m_bChange=FALSE;
		}		   		
	} 
	else 
	{
		m_bHasRefForTest = FALSE;
	}
	m_bHasRefForTest = TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnSelchangeComboPn() 
{
	// TODO: Add your control notification handler code here
	
}

void CMy126S_45V_Switch_AppDlg::OnCloseupComboPn() 
{
	// TODO: Add your control notification handler code here
	/*
	GetDlgItemText(IDC_COMBO_PN,m_strPN);
	//获取配置信息
	CString strFile;
	CString strValue;
	if (m_bIfSaveToServe)
	{
		strFile.Format("%s\\config\\%s\\PortInfo.ini",m_strNetConfigPath,m_strPN);
	}
	else
	{
		strFile.Format("%s\\config\\%s\\PortInfo.ini",g_tszAppFolder,m_strPN);
	}

	GetPrivateProfileString("CHCount","Value","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		MessageBox("读取配置文件PortInfo.ini错误CHCount");
		return;
	}
	m_nCHCount = atoi(strValue);

	GetPrivateProfileString("ComPort","Value","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		MessageBox("读取配置文件PortInfo.ini错误ComPort");
		return;
	}
	m_nSwitchComPortCount = atoi(strValue);
	*/

}

BOOL CMy126S_45V_Switch_AppDlg::CheckParaPassOrFail(int nParaType,double dblValue)
{
	//获取参数范围要求
	CString strIniFile;
	CString strSection;
	CString strValue;
	CString strMsg;
	double  dblMaxValue;
	double  dblMinValue;

//	if (m_bIfSaveToServe)
//	{
 //       strIniFile.Format("%s\\config\\%s-%s\\ParaDemand.ini",m_strNetConfigPath,m_strSpec,m_strPN);
//	}
//	else
//	{
	    strIniFile.Format("%s\\%s-%s\\ParaDemand.ini", m_strPNConfigPath,m_strSpec,m_strPN);
//	}
	switch (nParaType)
	{
	case IL_HIGH_DATA:
	case IL_LOW_DATA:
	case IL_ROOM_DATA:
		strSection = "IL";
		break;
	case WDL_DATA:
		strSection = "WDL";
		break;
	case ATTEN_REPEAT:
		strSection = "ATTEN_REPEAT";
		break;
	case WDL_DATA_5:
		strSection = "WDL_5";
		break;
	case WDL_DATA_8:
		strSection = "WDL_8";
		break;
	case TDL_DATA:
		strSection = "TDL";
		break;
	case PDL_DATA:
		strSection = "PDL";
		break;
	case RE_IL_DATA:
		strSection = "REP_IL";
		break;
	case RL_DATA:
		strSection = "RL";
		break;
	case CT_DATA:
		strSection = "CT";
		break;
	case DARK_DATA:
		strSection = "DARK";
		break;
	case SWITCH_TIME_DATA:
		strSection = "Switch Time";
		break;
	case DIRECTIVITY_DATA:
		strSection = "DIRECTIVITY";
		break;
	default:
		return FALSE;
	}

	if((m_strSN.Compare(m_strSN1) == 0) && 
		(nParaType == IL_HIGH_DATA || nParaType == IL_LOW_DATA || nParaType == IL_ROOM_DATA))
	{
		GetPrivateProfileString(strSection,"MaxValue1","error",strValue.GetBuffer(128),128,strIniFile);
		if (strValue == "error")
		{
			strMsg.Format("获取配置文件%s失败(%s)",strIniFile,strSection);
			LogInfo(strMsg,FALSE,COLOR_RED);
			return FALSE;
		}
		dblMaxValue = atof(strValue);

		GetPrivateProfileString(strSection,"MinValue1","error",strValue.GetBuffer(128),128,strIniFile);
		if (strValue == "error")
		{
			strMsg.Format("获取配置文件%s失败(%s)",strIniFile,strSection);
			LogInfo(strMsg,FALSE,COLOR_RED);
			return FALSE;
		}
		dblMinValue = atof(strValue);
	}
	else
	{
		GetPrivateProfileString(strSection,"MaxValue","error",strValue.GetBuffer(128),128,strIniFile);
		if (strValue == "error")
		{
			strMsg.Format("获取配置文件%s失败(%s)",strIniFile,strSection);
			LogInfo(strMsg,FALSE,COLOR_RED);
			return FALSE;
		}
		dblMaxValue = atof(strValue);

		GetPrivateProfileString(strSection,"MinValue","error",strValue.GetBuffer(128),128,strIniFile);
		if (strValue == "error")
		{
			strMsg.Format("获取配置文件%s失败(%s)",strIniFile,strSection);
			LogInfo(strMsg,FALSE,COLOR_RED);
			return FALSE;
		}
		dblMinValue = atof(strValue);
	}

	if (dblValue>=dblMinValue&&dblValue<=dblMaxValue)
	{
		return TRUE;
	}
	return FALSE;

}

void CMy126S_45V_Switch_AppDlg::OnSelchangeComboTestItem() 
{
	// TODO: Add your control notification handler code here
	/*
	UpdateData();
	GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(TRUE);
	
	if (m_cbTestItem.GetCurSel()==0)
	{
		GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(FALSE);
    	GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(FALSE);
	}
	*/
}

/*double CMy126S_45V_Switch_AppDlg::GetModuleTemp()
{
	double dblTemp = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("GTMP\r");
	if (m_nPMType[0] == PM_PDARRAY)
	{
		strTemp.Format("GTMP 1\r");
	}
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("串口打开错误！",(BOOL)FALSE,COLOR_RED);
		return -99;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！",(BOOL)FALSE,COLOR_RED);
		return -99;
	}
    dblTemp = atof(chReData);	
	dblTemp = dblTemp/10.0;
	return dblTemp;

}*/

VOID CMy126S_45V_Switch_AppDlg::DownLoadFW()
{
	CString  strFileName;
	CFileDialog dlgDataFile(TRUE);
	dlgDataFile.m_ofn.lpstrFilter = "原始数据文件(*.bin)\0*.bin\0\0";
	
	if (dlgDataFile.DoModal() == IDCANCEL)
	{
		return;
	}
	strFileName = dlgDataFile.GetPathName();
	if (!strFileName.IsEmpty())
	{
		if (!DataDownLoad(strFileName))
        {
			MessageBox("下载"+strFileName+"失败！");
			return;
        }
		else
		{			
			MessageBox("下载"+strFileName+"成功！");
			return;
		}
	}
	else
	{
		LogInfo("请选择bin文件！");
	}

}

/*BOOL CMy126S_45V_Switch_AppDlg::DataDownLoad(CString strFileName)
{
	BOOL		bFileDone   = FALSE;
	BOOL		bFunctionOK = FALSE;
	BYTE		bDownloadOK;
	int         bByteIndex;
	//char		pbBinData[XMODEM_BLOCK_BODY_SIZE_1K + 32];
	BYTE		pbBinData[2208];
	WORD        wAddress;
	DWORD		dwBytesRead;
	DWORD		dwCodeSizeLeft;
	DWORD		dwTotalDataCount;
	DWORD		dwLoadDataSize;
	WORD		wDownloadSize;
	WORD		wCRC16;
	CString     strMsg;
	BYTE		bPacketIndex;
	HANDLE		hBinFile = INVALID_HANDLE_VALUE;
	int			iCnt=0,iCount=0;

	try
	{
		// 1. 发命令去启动Xmodem
//		strMsg.Format("FWDL\r");
//		if (m_nPMType[0] == PM_PDARRAY)
//		{
//			strMsg.Format("FWDL 1\r");
//		}
//		if(!SendRevCommand(strMsg,&m_opCom))
//			throw "启动Xmodem失败";


		
		TxSTRToI2C("00DD","01");

		if (m_strErrorMsg != "")
		{
			throw m_strErrorMsg;
		}

		Sleep(100);

			RxDataFromI2C("00FE");
			if (m_strErrorMsg != "")
			{
			     throw m_strErrorMsg;
			}



		// 打开bin文件
		hBinFile = CreateFile(strFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hBinFile == INVALID_HANDLE_VALUE)
			throw "Error: Open Bin file fail";

		//Get Download.bin Filesize
		dwCodeSizeLeft = GetFileSize(hBinFile, NULL);
		if(dwCodeSizeLeft == 0)
			throw "Error: Bin file is empty";

		// 2. 开始下载文件 
		bPacketIndex     = 1; 
		dwLoadDataSize   = 0;
		dwTotalDataCount = dwCodeSizeLeft;

		//iCnt = dwCodeSizeLeft/XMODEM_BLOCK_BODY_SIZE_1K;
		//iCnt += (dwCodeSizeLeft%XMODEM_BLOCK_BODY_SIZE_1K)/128;
	
		iCnt = dwCodeSizeLeft/I2CSENDSIZE;
		iCnt += (dwCodeSizeLeft%I2CSENDSIZE)/32;
		m_ctrlProgress.SetRange(0,iCnt);
        
		if (m_bisdownimg)
		{
			wDownloadSize = 416;
		}
		else
		{
			wDownloadSize =2208;

		}
		//wDownloadSize =2208;

		dwCodeSizeLeft -= wDownloadSize;
		dwLoadDataSize += wDownloadSize;

		ReadFile(hBinFile, pbBinData, wDownloadSize, &dwBytesRead, NULL);
			if(dwBytesRead != wDownloadSize)
				throw "Error: Read data from bin file error";
        
       	for(bByteIndex = 0; bByteIndex<wDownloadSize; )
		{
			wAddress = (WORD)bByteIndex;

            //TxDataToI2C("00DE",(BYTE*)pbBinData[bByteIndex],32);

		
			
		     TxBYTEToI2C("00DE",pbBinData[bByteIndex],pbBinData[bByteIndex+1],pbBinData[bByteIndex+2],pbBinData[bByteIndex+3],pbBinData[bByteIndex+4],pbBinData[bByteIndex+5],pbBinData[bByteIndex+6],pbBinData[bByteIndex+7],pbBinData[bByteIndex+8],pbBinData[bByteIndex+9],pbBinData[bByteIndex+10],pbBinData[bByteIndex+11],pbBinData[bByteIndex+12],pbBinData[bByteIndex+13],pbBinData[bByteIndex+14],pbBinData[bByteIndex+15],pbBinData[bByteIndex+16],pbBinData[bByteIndex+17],pbBinData[bByteIndex+18],pbBinData[bByteIndex+19],pbBinData[bByteIndex+20],pbBinData[bByteIndex+21],pbBinData[bByteIndex+22],pbBinData[bByteIndex+23],pbBinData[bByteIndex+24],pbBinData[bByteIndex+25],pbBinData[bByteIndex+26],pbBinData[bByteIndex+27],pbBinData[bByteIndex+28],pbBinData[bByteIndex+29],pbBinData[bByteIndex+30],pbBinData[bByteIndex+31],32);

			

			bByteIndex = bByteIndex +32;

			Sleep(100);

		//	if (bByteIndex > 2150)
		//	{
		//		Sleep(100);
		//	}

			if (m_strErrorMsg != "")
			{
				throw m_strErrorMsg;
			}

			RxDataFromI2C("00FE");
			if (m_strErrorMsg != "")
			{
				throw m_strErrorMsg;
			}
		    YieldToPeers();	

			iCount++;
			m_ctrlProgress.SetPos(iCount);

		}

	    TxSTRToI2C("00FF","01");

		if (m_strErrorMsg != "")
		{
			throw m_strErrorMsg;
		}

		Sleep(100);

		RxDataFromI2C("00FE");
		if (m_strErrorMsg != "")
		{
		   throw m_strErrorMsg;
		}


		bFunctionOK = TRUE;
		strMsg.Format("等待4s。。。。。。");
		LogInfo(strMsg);
		//Sleep(4000);
		strMsg.Format("下载文件%s成功！",strFileName);
        LogInfo(strMsg);
		//发送复位信号
		//strMsg.Format("RSET %d\r",m_nSwitchIndex+1);
		//strMsg.Format("RSET\r");
		//SendCommand(strMsg);
		LogInfo("发送复位信号！");
	}
	
	catch(TCHAR* pszErrMsg)
	{
		bFunctionOK = FALSE;
		LogInfo(pszErrMsg,TRUE);
		MessageBox(pszErrMsg, "Error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		bFunctionOK = FALSE;
		MessageBox("other error!");
	}

	if(hBinFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hBinFile);
	}

	return	bFunctionOK;

}

BOOL CMy126S_45V_Switch_AppDlg::SendRevCommand(CString command, COpComm *pcommandtask, CString setvalue)
{
	CString strStr;
	unsigned long RevCommLen=0;
	int iCnt=0;
	BOOL bFlag=0;
	char    SendBuffer[MAXCMDBT];
    char    RevBuffer[MAXCMDBT*7];
	
	memset(SendBuffer,0,sizeof(SendBuffer));
	memset(RevBuffer,0,sizeof(RevBuffer));
	
	strStr = command;
	strncpy((char *)SendBuffer,strStr.GetBuffer(strStr.GetLength()),strStr.GetLength());
	
	if(!pcommandtask->WriteBuffer(SendBuffer,strStr.GetLength()))
	{
		MessageBox("命令发送失败！");
		return FALSE;
	}
	
	memset(RevBuffer,0,sizeof(RevBuffer));
	
	Sleep(5000);
	//	if(!pcommandtask->ReadBuffer(RevBuf,MAXCMDBT*7,&dwReRead))
	if(!pcommandtask->ReadBuffer(RevBuffer,MAXCMDBT*7))
	{
		MessageBox("命令接收错误！");
		return FALSE;
	}
	
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::XmodemloadData(char *pbBinData, WORD wDownloadSize, BOOL bFileDone)
{
	char	byTempBuf[MAX_BUF_DATA];
	int		nTimeOut;
	int		nRetry = 0;
	DWORD	dwReadLength=0;
	
	// Send Packet data
	if(!m_opCom.WriteBuffer(pbBinData, wDownloadSize))
	{
		MessageBox("RS232发送数据错误!", "提示", MB_OK|MB_ICONERROR);
		return	XMODEM_COMMUNICATION_FAIL;
	}
	
	// Check Response
    while (1)
    {
        Sleep(50);
		nTimeOut = 0;
	 	if(m_opCom.ReadBuffer(byTempBuf, MAX_BUF_DATA,&dwReadLength))
        {
            // Affirmative Answer
            if(byTempBuf[dwReadLength - 1] == XMODEM_ACK)
			{
                break;
			}
            // Negative Answer
            else if(byTempBuf[dwReadLength - 1] == XMODEM_NAK)
            {
				// Send Packet data again
				if(!m_opCom.WriteBuffer(pbBinData, wDownloadSize))
				{
					MessageBox("RS232发送数据错误!", "提示", MB_OK|MB_ICONERROR);
					return XMODEM_COMMUNICATION_FAIL;
				}

                nRetry++;
				if(nRetry > 10)
				{
					// Cancel Transfer
					byTempBuf[0] = XMODEM_CAN;
					byTempBuf[1] = XMODEM_CAN;
					byTempBuf[2] = XMODEM_CAN;

					if(!m_opCom.WriteBuffer(byTempBuf, 3))
					{
						MessageBox("发送3个CAN取消Xmodem下载失败!", "提示", MB_OK|MB_ICONERROR);
					}
					else
					{
						MessageBox("发送3个CAN取消Xmodem下载，请重新再下载一次!", "提示", MB_OK|MB_ICONQUESTION);
					}
					
					return	XMODEM_COMMUNICATION_FAIL;
				}
            }
			else if(byTempBuf[dwReadLength - 1] == XMODEM_CAN)
			{
				MessageBox("模块发生CAN字符上来取消Xmodem下载，请重新下载一次!", "提示", MB_OK|MB_ICONERROR);
				return	XMODEM_COMMUNICATION_FAIL;
			}
			else
			{
				MessageBox("模块发送过来的回应信息不正常，请重启模块后再下载一次!", "提示", MB_OK|MB_ICONERROR);
				return	XMODEM_COMMUNICATION_FAIL;
			}
		}

		nTimeOut ++;
		if(nTimeOut >= 210)
		{
			MessageBox("等待Xmodem回应超过10s，请复位模块，重新下载一次！！", "提示", MB_OK|MB_ICONERROR);
			return XMODEM_COMMUNICATION_FAIL;
		}
    }

    // Transfer Done
	if(bFileDone)
	{
		byTempBuf[0] = XMODEM_EOT;
		byTempBuf[1] = XMODEM_EOT;
		byTempBuf[2] = XMODEM_EOT;
		
		if(!m_opCom.WriteBuffer(byTempBuf, 3))
		{
			MessageBox("发送3个EOT完成Xmodem下载失败", "提示", MB_OK|MB_ICONERROR);
			return	XMODEM_COMMUNICATION_FAIL;
		}
		
		while (1)
		{
			Sleep(50);
			nTimeOut = 0;
			ZeroMemory(byTempBuf, MAX_BUF_DATA);
			if(m_opCom.ReadBuffer(byTempBuf, MAX_BUF_DATA,&dwReadLength))
			{
				// Affirmative Answer
				if(byTempBuf[0] == XMODEM_ACK)
				{
					if(strcmp(byTempBuf + 1, "Successful") == 0)
						return	XMODEM_DOWNLOAD_SUCCESS;
					else
						return	XMODEM_DOWNLOAD_FAIL;
				}
				else if(byTempBuf[0] == XMODEM_NAK)
				{
					return	XMODEM_DOWNLOAD_FAIL;
				}
			}

			nTimeOut ++;
			if(nTimeOut >= 210)
			{
				MessageBox("等待Xmodem回应超过10s，请复位模块，重新下载一次！！", "提示", MB_OK|MB_ICONERROR);
				return XMODEM_COMMUNICATION_FAIL;
			}
		}
	}

    return XMODEM_DOWNLOAD_SUCCESS;

}

WORD CMy126S_45V_Switch_AppDlg::XmodemCRC16(char *pbStream, WORD wLength)
{
	WORD    wCRCValue;
    WORD    wIndex;
	
    wCRCValue = 0;
	
    while(wLength > 0)
    {
        wCRCValue = wCRCValue ^ (WORD)(*pbStream ++) << 8;
		
        for(wIndex = 0; wIndex < 8; wIndex ++)
        {
            if(wCRCValue & 0x8000)    
				wCRCValue = (wCRCValue << 1) ^ 0x1021;
            else
				wCRCValue = wCRCValue << 1;
        }
		
        wLength --;
    }
	
    return wCRCValue;

}

void CMy126S_45V_Switch_AppDlg::SendCommand(CString strCommand)
{
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	memcpy(chData,strCommand,strCommand.GetLength());
    if(!m_opCom.WriteBuffer(chData,strCommand.GetLength()))
    {
		MessageBox("串口打开错误！");
		return ;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		MessageBox("读取错误，接收错误！");
		return ;
	}

}*/

void CMy126S_45V_Switch_AppDlg::OnButtonAutotest() 
{
	// TODO: Add your control notification handler code here
	BOOL bILTest      = TRUE;
	BOOL bWDLTest     = TRUE;
	BOOL bTDLTest     = TRUE;
	CString strMsg;

	if (!m_bOpenPort)
	{
		MessageBox("请先打开产品串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}
// 	if (!m_bOpenCirBoxPort)
// 	{
// 		MessageBox("请先打开产品串口！");
// 		return;
// 	}
	
	UpdateData();
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);

	//自动定标通道
	if (!CalAutoScanTest())
	{
		LogInfo("自动扫描定标出错！",(BOOL)FALSE,COLOR_RED);
		return;
	}
	if (!CreateLutFile())
	{
		LogInfo("创建LUT表出错！",(BOOL)FALSE,COLOR_RED);
		return;
	}
	if (!CreateBinFileAndDownload())
	{
		LogInfo("下载数据错误！",(BOOL)FALSE,COLOR_RED);
		return;
	}

	//开始测试光学参数
// 	if (!TDLTest())
// 	{
// 		bTDLTest = FALSE;
// 	}
	if (!ILCTTest())
	{
		bILTest = FALSE;
	}
	if (!WDLTest())
	{
		bWDLTest = FALSE;
	}

	m_ctrlListMsg.ResetContent();
// 	if (!bTDLTest)
// 	{
// 		LogInfo("TDL测试失败！",(BOOL)FALSE,COLOR_RED);
// 	}
//     else
// 	{
// 		LogInfo("TDL测试正常！");
// 	}
	if (!bILTest)
	{
		LogInfo("IL/CT测试失败！",(BOOL)FALSE,COLOR_RED);
	}
	else
	{
		LogInfo("IL/CT测试正常！");
	}
	if (!bWDLTest)
	{
		LogInfo("WDL测试失败！",(BOOL)FALSE,COLOR_RED);
	}
	else
	{
		LogInfo("WDL测试正常！");
	}

	for (int i=0;i<4;i++)
	{
		if (!m_bILPass[i])
		{
			strMsg.Format("通道%d IL参数不合格！",i+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		if (!m_bCTPass[i])
		{
			strMsg.Format("通道%d CT参数不合格！",i+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		if (!m_bTDLPass[i])
		{
			strMsg.Format("通道%d TDL参数不合格！",i+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		if (!m_bWDLPass[i])
		{
			strMsg.Format("通道%d WDL参数不合格！",i+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
	}
	LogInfo("测试完毕！");
}

BOOL CMy126S_45V_Switch_AppDlg::CalAutoScanTest()
{
	CString strFileName,strMsg;
	CString strFile;
	CString strValue;
	CString strTemp;
	CString strLutFile;
	CStdioFile stdFile;
	CString    strInfo;
		
		//设置路径
	UpdateData();
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);

	double dblRoomTemp = GetModuleTemp();
	
	strTemp = "Room";
	LogInfo("开始常温测试，等待模块温度。。");

	strLutFile.Format("%s\\data\\%s\\%s\\LUT\\TempData_%s.csv",g_tszAppFolder,m_strPN,m_strSN,strTemp);
	if(!stdFile.Open(strLutFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建文件失败！");
		return FALSE;
	}
	strValue.Format("%.1f",dblRoomTemp);
	stdFile.WriteString(strValue);
	stdFile.Close();
	
    //初始化图形控件
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("X(mv)");
	m_NTGraph.SetYLabel("Y(mv)");
	m_NTGraph.SetRange(-60000,60000,-60000,60000);
	m_NTGraph.SetXGridNumber(10);
	m_NTGraph.SetYGridNumber(10);
	
	CString strFileINI;
	if (m_cbComPort.GetCurSel()==0)
	{
		strFileINI = "ScanRange.ini";
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strFileINI = "ScanRange.ini-COM1";
	}
	else
	{

	}
		
    for (int nChannel=0;nChannel<=m_nCHCount;nChannel++)
	{
		strInfo.Format("开始扫描通道%d",nChannel+1);
		LogInfo(strInfo);
		if(!AutoScan(nChannel,strFileINI))
		{
			strMsg.Format("粗扫通道%d出错！",nChannel+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}	
		m_NTGraph.ClearGraph();
		
		if(!AutoPointScan(nChannel))
		{
			strMsg.Format("细扫通道%d出错！",nChannel+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		
		//保存文件
		CString strCSVFile;
		CStdioFile stdCSVFile;
		CString strContent;
		int    iSel=IDYES;

		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\VOL\\%s_ScanVoltage_%s_%d_%s.csv",
			g_tszAppFolder,m_strPN,m_strSN,m_strSN,strTemp,nChannel+1,m_strItemName);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\VOL\\%s_ScanVoltage_%s_%d_%s-COM1.csv",
			g_tszAppFolder,m_strPN,m_strSN,m_strSN,strTemp,nChannel+1,m_strItemName);
		}
		
		if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
		{
			LogInfo("创建数据文件失败！",FALSE,COLOR_RED);
			return FALSE;
		}
		strContent.Format("%d,%d,%d,%.2f\n",nChannel+1,ScanPowerChannel[nChannel].VoltageX,
			ScanPowerChannel[nChannel].VoltageY,ScanPowerChannel[nChannel].MaxPower);
		stdCSVFile.WriteString(strContent);		
		stdCSVFile.Close();	
	}
	SaveDataToExcel(VOL_DATA);
	Sleep(200);
	SaveDataToExcel(IL_ROOM_DATA);
	LogInfo("保存常温扫描数据成功！"); 	
	LogInfo("定标完毕！");
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::CreateLutFile()
{
	CString strFileLut;
	CStdioFile stdFileLut;
	CString strFile;
	CStdioFile stdFile;
	CString  strVendorName;
	CString  strPN;
	CString  strHWVersion;
	CString  strFWVersion;
	CString  strSN;
	CString  strTemp;
	CString  strTime;
	CString  strValue;
	CString  strToken;
	char     chSep[] = ",\t";
	char     chReadBuff[MAX_LINE];
	SYSTEMTIME  st;
	CString strNetFile;
	
	UpdateData();	
	LogInfo("开始创建产品信息LUT表");
    ZeroMemory(chReadBuff,sizeof(chReadBuff));
	GetSystemTime(&st);
	strFileLut.Format("%s\\data\\%s\\%s\\%s\\BIN\\ProductInfo_LUT.csv",g_tszAppFolder,m_strItemName,m_strPN,m_strSN);
	strNetFile.Format("%s\\data\\%s\\%s\\%s\\BIN\\ProductInfo_LUT.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN);
	if (!stdFileLut.Open(strFileLut,CFile::modeCreate|CFile::modeWrite))
	{
		LogInfo("创建产品信息LUT失败！",FALSE,COLOR_RED);
		return FALSE;
	}
	strVendorName = "Oplink\n";
	stdFileLut.WriteString(strVendorName);
	strSN = m_strSN+"\n";
	stdFileLut.WriteString(strSN);
	strPN = "UOFS104U00IFA01G\n";
	stdFileLut.WriteString(strPN);
	strHWVersion = "HW 0.1\n";
	stdFileLut.WriteString(strHWVersion);
	strTime.Format("%04d-%02d-%02d",st.wYear,st.wMonth,st.wDay);
	stdFileLut.WriteString(strTime);
	stdFileLut.Close();
	LogInfo("生成产品信息LUT表完成！");
	if (m_bIfSaveToServe)
	{
		CopyFile(strFileLut,strNetFile,FALSE);
	}
	return TRUE;


}

BOOL CMy126S_45V_Switch_AppDlg::CreateBinFileAndDownload()
{
	CString strSave;
	CString strINIFile;
	CString strBinFile;
	CString strValue;
	int     nFile;
	CString strFile;
	
	UpdateData();
	//寻找三个温度的数据是否齐全
	//数据不全，返回0；
	//只有常温数据，返回1；
	//常温和低温数据，返回3；
	//常温和高温数据，返回4；
    //三个温度都有，返回6；
	//四个温度都有，返回10；

	if (!CreateLutFile())
	{
		LogInfo("创建LUT表出错！",(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	if (!m_bisinit)
	{
		nFile = CheckTempLutFile(strSave);
	}
	else
	{
		nFile = 5;

	}
	
	if (nFile==0)
	{
		MessageBox("数据不全，请查看！");
		return FALSE;
	}
	else if (nFile ==1)
	{
		//MessageBox("检测到只有常温数据！");
		LogInfo("检测到只有常温数据！");
	}
	else if (nFile ==2)
	{
		//MessageBox("检测到只有常温数据！");
		LogInfo("检测到只有低温数据！");
	}
	else if (nFile ==3)
	{
		//MessageBox("检测到只有常温数据！");
		LogInfo("检测到只有高温数据！");
	}
	else if (nFile ==4)
	{
		//MessageBox("检测到有三个温度数据！");
		LogInfo("检测到有常温和低温数据！");
	}
	else if (nFile ==5)
	{
		//MessageBox("检测到有四个温度数据！");
		LogInfo("检测到有三个温度数据！");
	}

	if(!CreateSwitchPointBin(strSave,nFile))
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::ILCTTest()
{
	CString strTemp;
	CString strTemp1;
	CString strChannel;
	CString strMsg;
	double  dblPref;
	double  dblPout;
	double  dblPower;
	char    strCT[10];
	char    strIL[10];
	int nchannel;
	m_bTestStop = FALSE;

	ZeroMemory(strCT,sizeof(char)*10);
	ZeroMemory(strIL,sizeof(char)*10);

    strTemp.Format("开始IL/CT测试！");
    LogInfo(strTemp);
	
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;

	ZeroMemory(ChannalMessage,sizeof(ChannalMessage));

	strTemp.Format("通道");
	for (int i=0;i<m_nCHCount;i++)
	{
		strTemp1.Format("  CT%d",i+1);
		strTemp = strTemp+strTemp1;
	}	
	LogInfo(strTemp);

	for (nchannel = 0; nchannel < m_nCHCount; nchannel ++)
	{
		YieldToPeers();
		if (m_bTestStop)
		{
			LogInfo("测试中止！",FALSE,COLOR_RED);
			break;
		}
		//开关切换
        if(!SWToChannel(nchannel))
		{
			LogInfo("开关切换错误！",FALSE,COLOR_RED);
			return FALSE;
		}
		Sleep(300);

		for(int i = 0; i < m_nCHCount; i++)
		{
			//启用归零数据
	        dblPref = m_dblReferencePower[i];
		    dblPower = ReadPWMPower(i);
			strMsg.Format("%.2f",dblPower);
		
		    dblPout = dblPower;
		    ChannalMessage[nchannel].dblCT[i] = TestIL(dblPref,dblPout);
		    if (nchannel == i)
			{
			    ChannalMessage[nchannel].dblIL = ChannalMessage[nchannel].dblCT[i];
				if (!CheckParaPassOrFail(IL_ROOM_DATA,ChannalMessage[nchannel].dblIL))
				{
					m_bILPass[nchannel] = FALSE;
				}
			}
	        else
			{
				if (!CheckParaPassOrFail(CT_DATA,ChannalMessage[nchannel].dblCT[i]))
				{
					m_bCTPass[nchannel] = FALSE;
				}
			}
		}
		strTemp.Format("%02d",nchannel+1);
		for (int i=0;i<m_nCHCount;i++)
		{
			strTemp1.Format("  %.2f",ChannalMessage[nchannel].dblCT[i]);
			strTemp = strTemp + strTemp1;
		}
		if (m_bILPass[nchannel]==FALSE||m_bCTPass[nchannel]==FALSE)
		{
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			LogInfo(strTemp);
		}		
	}

	//保存到CSV文件
	if (m_cbComPort.GetCurSel()==0)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_CT_%s.csv", m_strNetFile,m_strPN,m_strSN,m_strSN,m_strItemName);
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_CT_%s-COM1.csv", m_strNetFile,m_strPN,m_strSN,m_strSN,m_strItemName);
	}
	
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle!=INVALID_HANDLE_VALUE)
	{
		strMsg.Format("文件%s已经存在，覆盖数据",strCSVFile);
		LogInfo(strMsg);
	}
	
	if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
	{
		LogInfo("创建CT数据文件失败！",FALSE,COLOR_RED);
		return FALSE;
	}

	strTemp = "Channel";
	for (int i=0;i<m_nCHCount;i++)
	{
		strTemp1.Format(",%d",i+1);
		strTemp = strTemp + strTemp1;
	}
	strTemp = strTemp + "\n";
	stdCSVFile.WriteString(strTemp);
    
	for (int i=0;i<m_nCHCount;i++)
	{
		strTemp.Format("CH%d/CT",i+1);
		for (int j=0;j<m_nCHCount;j++)
		{
			strTemp1.Format(",%.2f",ChannalMessage[i].dblCT[j]);
			strTemp = strTemp + strTemp1;
		}
		strTemp = strTemp + "\n";
		stdCSVFile.WriteString(strTemp);
	}
	stdCSVFile.Close();

	//保存到EXCEL文件中
	SaveDataToExcel(CT_DATA);
	Sleep(100);
	SaveDataToExcel(IL_ROOM_DATA);
	LogInfo("IL/CT测试完毕!");
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::WDLTest()
{
	double dblTLSPower=0.0;
	double dblStartWL=0;
	double dblENDWL=0;
	double dblPower=0.0;
	double step=0;
	int count;
    double dblWDLPower[1000];
    double dblMIN;
	double dblMAX;
	int  nChannel;
	CString strMsg; 
	double dbl;
	CString strZeroFile;
	CStdioFile stdZeroFile;
//	LPTSTR    lpStr;
//	char    ReadValue[256];
	CString  strToken;
    char     sep[] = ",\t\n";
    m_bTestStop = FALSE;

	CString strIniFile;
	CString strValue;
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;

    ZeroMemory(dblWDLPower,sizeof(double)*100);

	strIniFile.Format("%s\\%s-%s\\WDLSET.INI", m_strPNConfigPath,m_strSpec,m_strPN);
	DWORD nLen1 = GetPrivateProfileString("WDL","STARTWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen1 <= 0)
	{
		LogInfo("读取配置文件错误！",(BOOL)FALSE,COLOR_RED);
    	return FALSE;
	}	
    dblStartWL = atof(strValue);
	
	DWORD nLen2 = GetPrivateProfileString("WDL","STOPWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen2 <= 0)
	{
		LogInfo("读取配置文件错误！",(BOOL)FALSE,COLOR_RED);
    	return FALSE;
	}
    dblENDWL = atof(strValue);
	
	DWORD nLen3 = GetPrivateProfileString("WDL","STEP",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen2 <= 0)
	{
		LogInfo("读取配置文件错误！",(BOOL)FALSE,COLOR_RED);
    	return FALSE;
	}
    step = atof(strValue);

	//初始化图形界面
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("波长(nm)");
	m_NTGraph.SetYLabel("功率值(dBm)");
	m_NTGraph.SetRange(dblStartWL,dblENDWL,-2,2);
	m_NTGraph.SetXGridNumber(10);
	m_NTGraph.SetYGridNumber(8);
		
	UpdateData();	
	for (nChannel=0;nChannel<4;nChannel++)
	{
		if(!SWToChannel(nChannel))
		{
			LogInfo("开关切换失败！",(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		Sleep(300);
		strMsg.Format("开始通道%d的WDL测试！",nChannel+1);
	    LogInfo(strMsg);		
		count = 0;
		for (dbl = dblStartWL; dbl <= dblENDWL; dbl += step)
		{
			YieldToPeers();
            if (m_bTestStop)
			{
				LogInfo("测试中止！",(BOOL)FALSE,COLOR_RED);
				if(!SetPWMWL(m_pdblWavelength))
				{
					LogInfo("设置波长失败！",(BOOL)FALSE,COLOR_RED);
				}
				m_bChange=FALSE;
	        	SetTLSWavelength(m_pdblWavelength);
				return FALSE;
			}
			SetTLSWavelength(dbl);
			Sleep(50);
			//设置光功率计波长
			if(!SetPWMWL(dbl))
			{
				LogInfo("设置光功率计波长失败！",(BOOL)FALSE,COLOR_RED);	
				return FALSE;
			}
			m_bChange=TRUE;
			Sleep(300);
					
			dblPower = ReadPWMPower(nChannel);
			m_dblWDL[nChannel][count] = m_dblWDLRef[nChannel][count]-dblPower;
			strMsg.Format("通道%d:　波长:%.2f nm , IL:%.2f dB",nChannel,dbl,m_dblWDL[nChannel][count]);
			LogInfo(strMsg);
		    count++;							
		} 
		//设置回1550nm波长
        if(m_bChange)
		{
			if (!SetPWMWL(m_pdblWavelength))
			{
				LogInfo("设置波长错误！");
				return FALSE;
			}
			m_bChange=FALSE;
		}
		SetTLSWavelength(m_pdblWavelength);
		//计算WDL
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_WDL_%d_%s.csv", m_strNetFile,m_strPN,m_strSN,m_strSN,nChannel+1,m_strItemName);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_WDL_%d_%s-COM1.csv", m_strNetFile,m_strPN,m_strSN,m_strSN,nChannel+1,m_strItemName);
		}
		else
		{

		}	
		hHandle = FindFirstFile(strCSVFile,&win32Find);
		if (hHandle!=INVALID_HANDLE_VALUE)
		{
			strMsg.Format("文件%s已经存在，覆盖数据",strCSVFile);
			LogInfo(strMsg);
		}
		
		if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
		{
			LogInfo("创建WDL数据文件失败！",(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		strContent.Format("Channel,WDL\n");
	    stdCSVFile.WriteString(strContent);

		dblMIN = m_dblWDL[nChannel][0];
		dblMAX = m_dblWDL[nChannel][0];
		for(int i=1;i<(count-1);i++)
		{
			if (m_dblWDL[nChannel][i] <= dblMIN)
			{
				dblMIN = m_dblWDL[nChannel][i];
			}
			if (m_dblWDL[nChannel][i] >= dblMAX)
			{
				dblMAX = m_dblWDL[nChannel][i];
			}
		}
		ChannalMessage[nChannel].dblWDL[0] = dblMAX - dblMIN;
		strMsg.Format("通道%d的WDL为:%.2f dB",nChannel+1,ChannalMessage[nChannel].dblWDL[0]);
		if (!CheckParaPassOrFail(WDL_DATA,ChannalMessage[nChannel].dblWDL[0]))
		{
			m_bWDLPass[nChannel] = FALSE;
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			LogInfo(strMsg);
		}
		strContent.Format("%d,%.2f\n",nChannel,ChannalMessage[nChannel].dblWDL);
		stdCSVFile.WriteString(strContent);
		stdCSVFile.Close();	
		SaveDataToExcel(WDL_DATA,nChannel);    				
	}
	LogInfo("WDL测试完毕！");
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::TDLTest()
{
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::CheckTempLutFile(CString strSave)
{
	CString strLUTFile;
	CString strPath;
	CString strMsg;
	CString strTemp;
	int     nFile = 0;
	BOOL    bFlagLow = TRUE;
	BOOL    bFlagMid = TRUE;
	BOOL    bFlagRoom = TRUE;
	BOOL    bFlagHigh = TRUE;

	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	else
	{
		strPath = g_tszAppFolder;
	}

	strLUTFile.Format("%s\\data\\%s\\%s\\%s\\BIN\\ProductInfo_LUT.csv",strPath,m_strItemName,m_strPN,m_strSN);
	if(GetFileAttributes(strLUTFile) == -1)
	{
        strMsg.Format("文件%s不存在！",strLUTFile);
		MessageBox(strMsg);
		nFile = 0;
		return nFile;
	}

	//获取需下载的通道数和通道名
	CString strIniFile;
	CString strKeyName;
	CString strValue;

	strTemp="Room";
	for(int i=0;i<m_nCHCount;i++)
	{		
		if (m_nSwitchComPortCount==2)
		{
			strLUTFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
			strPath,m_strItemName,m_strPN,m_strSN,m_strSN,i+1);
			if (GetFileAttributes(strLUTFile) == -1)
			{
				nFile = 0;
				bFlagRoom = FALSE;
				strMsg.Format("数据文件%s不存在！",strLUTFile);
				LogInfo(strMsg);
				//return nFile;
				break;
			}
		}
		strLUTFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",
			strPath,m_strItemName,m_strPN,m_strSN,m_strSN,i+1);
		
		if (GetFileAttributes(strLUTFile) == -1)
		{
			nFile = 0;
			bFlagRoom = FALSE;
			strMsg.Format("数据文件%s不存在！",strLUTFile);
			LogInfo(strMsg);
			//return nFile;
			break;
		}	
	}		
	strTemp="Low";
	for(int i=0;i<m_nCHCount;i++)
	{		
		if (m_nSwitchComPortCount==2)
		{
			strLUTFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1-Low.csv",
				strPath,m_strItemName,m_strPN,m_strSN,m_strSN,i+1);
			if (GetFileAttributes(strLUTFile) == -1)
			{
				bFlagLow = FALSE;
				strMsg.Format("数据文件%s不存在！",strLUTFile);
			}
		}
		strLUTFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-Low.csv",
			strPath,m_strItemName,m_strPN,m_strSN,m_strSN,i+1);
		if (GetFileAttributes(strLUTFile) == -1)
		{
			if ( i>1 )
			{
				nFile = 0;
				bFlagLow = FALSE;
				//return nFile;
				break;
			}
			bFlagLow = FALSE;
			strMsg.Format("数据文件%s不存在！",strLUTFile);
			break;
			LogInfo(strMsg);
		}	
	}
	strTemp="High";
	for(int i=0;i<m_nCHCount;i++)
	{		
		if (m_nSwitchComPortCount==2)
		{
			strLUTFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1-High.csv",
				strPath,m_strItemName,m_strPN,m_strSN,m_strSN,i+1);
			if (GetFileAttributes(strLUTFile) == -1)
			{
			
				bFlagHigh = FALSE;
				strMsg.Format("数据文件%s不存在！",strLUTFile);
			}
		}
		strLUTFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-High.csv",
			strPath,m_strItemName,m_strPN,m_strSN,m_strSN,i+1);
		if (GetFileAttributes(strLUTFile) == -1)
		{
			if ( i>1 )
			{
				nFile = 0;
				bFlagHigh = FALSE;
				//return nFile;
				break;
			}
			bFlagHigh = FALSE;
			strMsg.Format("数据文件%s不存在！",strLUTFile);
			break;
			LogInfo(strMsg);
		}	
	}
	
	if (bFlagRoom)
	{
		nFile = 1;
	}
	if (bFlagLow)
	{
		nFile = 2;
	}
	if (bFlagHigh)
	{
		nFile = 3;
	}
	if (bFlagLow && bFlagRoom)
	{
		nFile = 4;
	}
	if (bFlagHigh && bFlagRoom && bFlagLow)
	{
		nFile = 5;
	}
	return nFile;

}

BOOL CMy126S_45V_Switch_AppDlg::CreateModuleMapBin(CString strSave, int nFile)
{
	CString   strMsg;
	CString		strPN;
	int			count = 0;
	char		chValue[MAX_LINE];
	PBYTE 		pbswInfo;
	st64PortMap m_st64PortMap;

	pbswInfo = (PBYTE)&m_st64PortMap;
	memset(&m_st64PortMap, 0x00, sizeof(m_st64PortMap));

		//bBundleTag
	count = 0;
    CString strBundle = "OPLINK";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strBundle,strBundle.GetLength());
    for (int i=0;i<strBundle.GetLength();i++)
	{
		//pByteBinData[count++] = (BYTE)(chValue[i]);
		m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(chValue[i]);
    }	
	//bProductType
	count = 8;
	CString strProductType = "SWITCH";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strProductType,strProductType.GetLength());
    for (int i=0;i<strProductType.GetLength();i++)
	{
		//pByteBinData[count++] = (BYTE)(chValue[i]);
		m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(chValue[i]);
    }
	//dwBundleHdrCRC32(Reserved)
	DWORD dwHdrCRC32 = 0;
	count = 16;
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>24);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>16);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>8);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>24);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>16);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>8);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32);

	DWORD dwBundleSize = 0x04A0;
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>24);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>16);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>8);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize);
	//wBundleHdrSize
	WORD	wBundleHdrSize = 128;
	//pByteBinData[count++] = (BYTE)(wBundleHdrSize>>8);
	//pByteBinData[count++] = (BYTE)(wBundleHdrSize);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(wBundleHdrSize>>8);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(wBundleHdrSize);
	//wImageCount
	WORD wImageCount = 1;
//	pByteBinData[count++] = (BYTE)(wImageCount>>8);
//	pByteBinData[count++] = (BYTE)(wImageCount);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(wImageCount>>8);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(wImageCount);
	//
	//bBundleVersion
	count = 32;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (int i=0;i<m_strSN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_st64PortMap.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	//bPartNumber
	count = 48;
    //strPN = m_strPortNumber;
	strPN = "14538_1x64_MEMSW";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strPN,strPN.GetLength());
    for (int i=0;i<strPN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_st64PortMap.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	// Serial Number of this product,
    count = 80;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (int i=0;i<m_strSN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_st64PortMap.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	// The time at which this bundle file generated
	SYSTEMTIME st;
	GetSystemTime(&st);
	CString strTime;
	strTime.Format("%04d.%02d.%02d.%02d.%02d",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	count = 112;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strTime,strTime.GetLength());
    for (int i=0;i<strTime.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_st64PortMap.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }


		m_st64PortMap.BUNDLEHEADER[count++] = 0x49;
		m_st64PortMap.BUNDLEHEADER[count++] = 0x42;
		m_st64PortMap.BUNDLEHEADER[count++] = 0x46;
		m_st64PortMap.BUNDLEHEADER[count++] = 0x48;
	//ImageType
    	m_st64PortMap.BUNDLEHEADER[count++] = 0x87;
	//ImageHitless
		m_st64PortMap.BUNDLEHEADER[count++] = 0x00;
	//StorageID
		m_st64PortMap.BUNDLEHEADER[count++] = 0x01;
	//ImageIndex
		m_st64PortMap.BUNDLEHEADER[count++] = 0x01;
	//dwImageVersion
		m_st64PortMap.BUNDLEHEADER[count++] = 0x00;
		m_st64PortMap.BUNDLEHEADER[count++] = 0x00;
		m_st64PortMap.BUNDLEHEADER[count++] = 0x00;
		m_st64PortMap.BUNDLEHEADER[count++] = 0x00;
	//dwTimeStamp
		m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(st.wMonth);
		m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(st.wDay);
		m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(st.wHour);
		m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(st.wMinute);
    
	//dwBaseAddress
	DWORD dwBaseAddress=0x1F000;

	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>24);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>16);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>8);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress);
	//dwImageCRC32

	count = 152;
  //  DWORD dwImageSize = dwBinsize-160; //总长度减去文件头长度
	DWORD dwImageSize = 1024; //总长度减去文件头长度
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>24);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>16);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>8);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwImageSize);

	m_st64PortMap.BUNDLEHEADER[count++] = 0x01;
	m_st64PortMap.BUNDLEHEADER[count++] = 0x00;
	m_st64PortMap.BUNDLEHEADER[count++] = 0x00;
	m_st64PortMap.BUNDLEHEADER[count++] = 0x00;

	for(int nCHIndex = 0; nCHIndex < MAX_SW_PORT_NUM+1; nCHIndex++)
	{
		for(int nSWIndex = 0; nSWIndex < SWITCH_NUM; nSWIndex++)
		{
			m_st64PortMap.bPortVsSwCh[nCHIndex][nSWIndex] = c_st64PortMap.bPortVsSwCh[nCHIndex][nSWIndex];
		}
	}
	DWORD dwImageCRC32 = 0;
	m_CRC32.InitCRC32();
	for (int i = 160;i < 1180; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
	dwImageCRC32 = ~dwImageCRC32;
	m_st64PortMap.dwCRC32 = dwImageCRC32;

	m_CRC32.InitCRC32();
 	for (int i = 160;i < 1184; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
 	dwImageCRC32 = ~dwImageCRC32;
	count = 148;

	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>24);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>16);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>8);
    m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32);

	//计算header的CRC
	m_CRC32.InitCRC32();
 	for (int i = 20;i < 128; i++)
 	{
 		dwHdrCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
 	dwHdrCRC32 = ~dwHdrCRC32;
	count = 16;
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>24);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>16);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>8);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>24);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>16);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>8);
	m_st64PortMap.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32);

	strMsg.Format("%s_ModuleMapBin.bin",m_strSN);
	if(!WriteBinFile(strMsg,strSave,1184,pbswInfo))
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::CreateSwitchMapBin(CString strSave, int nFile)
{
	CString   strMsg;
	CString   strPN;
	int       count;
	char      chValue[MAX_LINE];
	PBYTE 	 pbswInfo;
	stSwChInfo m_stSwChInfo;

	pbswInfo = (PBYTE)&m_stSwChInfo;
	memset(&m_stSwChInfo, 0x00, sizeof(m_stSwChInfo));

		//bBundleTag
	count = 0;
    CString strBundle = "OPLINK";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strBundle,strBundle.GetLength());
    for (int i=0;i<strBundle.GetLength();i++)
	{
		//pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(chValue[i]);
    }	
	//bProductType
	count = 8;
	CString strProductType = "SWITCH";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strProductType,strProductType.GetLength());
    for (int i=0;i<strProductType.GetLength();i++)
	{
		//pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(chValue[i]);
    }
	//dwBundleHdrCRC32(Reserved)
	DWORD dwHdrCRC32 = 0;
	count = 16;
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>24);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>16);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>8);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>24);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>16);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>8);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32);

	DWORD dwBundleSize = 0x04A0;
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>24);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>16);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>8);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize);
	//wBundleHdrSize
	WORD	wBundleHdrSize = 128;
	//pByteBinData[count++] = (BYTE)(wBundleHdrSize>>8);
	//pByteBinData[count++] = (BYTE)(wBundleHdrSize);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(wBundleHdrSize>>8);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(wBundleHdrSize);
	//wImageCount
	WORD wImageCount = 1;
//	pByteBinData[count++] = (BYTE)(wImageCount>>8);
//	pByteBinData[count++] = (BYTE)(wImageCount);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(wImageCount>>8);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(wImageCount);
	//
	//bBundleVersion
	count = 32;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (int i=0;i<m_strSN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stSwChInfo.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	//bPartNumber
	count = 48;
    //strPN = m_strPortNumber;
	strPN = "14538_1x64_MEMSW";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strPN,strPN.GetLength());
    for (int i=0;i<strPN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stSwChInfo.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	// Serial Number of this product,
    count = 80;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (int i=0;i<m_strSN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stSwChInfo.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	// The time at which this bundle file generated
	SYSTEMTIME st;
	GetSystemTime(&st);
	CString strTime;
	strTime.Format("%04d.%02d.%02d.%02d.%02d",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	count = 112;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strTime,strTime.GetLength());
    for (int i=0;i<strTime.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stSwChInfo.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }


		m_stSwChInfo.BUNDLEHEADER[count++] = 0x49;
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x42;
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x46;
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x48;
	//ImageType
    	m_stSwChInfo.BUNDLEHEADER[count++] = 0x86;
	//ImageHitless
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x00;
	//StorageID
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x01;
	//ImageIndex
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x01;
	//dwImageVersion
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x00;
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x00;
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x00;
		m_stSwChInfo.BUNDLEHEADER[count++] = 0x00;
	//dwTimeStamp
		m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(st.wMonth);
		m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(st.wDay);
		m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(st.wHour);
		m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(st.wMinute);
    
	//dwBaseAddress
	DWORD dwBaseAddress=0x1F800;

	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>24);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>16);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>8);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress);

	count = 152;
  //  DWORD dwImageSize = dwBinsize-160; //总长度减去文件头长度
	DWORD dwImageSize = 1024; //总长度减去文件头长度
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>24);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>16);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>8);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageSize);

	m_stSwChInfo.BUNDLEHEADER[count++] = 0x01;
	m_stSwChInfo.BUNDLEHEADER[count++] = 0x00;
	m_stSwChInfo.BUNDLEHEADER[count++] = 0x00;
	m_stSwChInfo.BUNDLEHEADER[count++] = 0x00;


	if(m_bischange)
	{
		int i_checkswnum=0;

		if (b_issn1) 
		{
			i_checkswnum = 1;
		}
		else if (b_issn2)
		{
			i_checkswnum = 2;
		}
		else if(b_issn3)
		{
			i_checkswnum = 3;
		}
		else if(b_issn4)
		{
			i_checkswnum = 4;
		}
		else
		{
			return FALSE;
		}

		/////////////////////////////////////////////////////////////////////
		DWORD dwAddreesMap= 0x1F800;
		char chData[64];
		CString strchan;
		int i_chan;

		for(int swReadIndex=0; swReadIndex < SWITCH_NUM; swReadIndex++)
		{
			//dwAddreesMap = 0x1F800 + swReadIndex*32;
			dwAddreesMap = 0x0D900 + swReadIndex * 32;
			ZeroMemory(chData,sizeof(char)*64);
			if(!GetMemoryData(dwAddreesMap,chData))
			{
				LogInfo("GetMemoryData失败!",FALSE,COLOR_RED);
				return FALSE;
			}
			for(int nCHIndex=0; nCHIndex<32; nCHIndex++)
			{
				strchan.Format("%c%c",chData[nCHIndex*2],chData[nCHIndex*2+1]);
				i_chan  = strtol(strchan,NULL,16);
				m_stSwChInfo.bChMapping[swReadIndex][nCHIndex] = i_chan;
			}
		}
		//////////////////////////////////////////////////////////////////////

		m_stSwChInfo.bChMapping[i_checkswnum-1][m_startchange1-1] = m_startchange2;
		m_stSwChInfo.bChMapping[i_checkswnum-1][m_startchange2-1] = m_startchange1;
		
	}
	else
	{
		for(int nSWIndex = 0; nSWIndex < SWITCH_NUM; nSWIndex++)
		{
			for(int nCHIndex = 0; nCHIndex < 32; nCHIndex++)
			{
				m_stSwChInfo.bChMapping[nSWIndex][nCHIndex] = nCHIndex+1;
			}
		}
	}


	DWORD dwImageCRC32 = 0;
	m_CRC32.InitCRC32();
	for (int i = 160;i < 1180; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
	dwImageCRC32 = ~dwImageCRC32;
	m_stSwChInfo.dwCRC32 = dwImageCRC32;

	m_CRC32.InitCRC32();
 	for (int i = 160;i < 1184; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
 	dwImageCRC32 = ~dwImageCRC32;
	count = 148;

	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>24);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>16);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>8);
    m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32);

	//计算header的CRC
	m_CRC32.InitCRC32();
 	for (int i = 20;i < 128; i++)
 	{
 		dwHdrCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
 	dwHdrCRC32 = ~dwHdrCRC32;
	count = 16;
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>24);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>16);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>8);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>24);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>16);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>8);
	m_stSwChInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32);

	strMsg.Format("%s_SingSwitchMapBin.bin",m_strSN);
	if(!WriteBinFile(strMsg,strSave,1184,pbswInfo))
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::CreateSwitchPointBin(CString strSave, int nFile)
{
	DWORD     dwBinsize;
    CString   strPath;
	CString   strMsg;
	CString   strFWversion;
	int       count;
	CString   strPN;
	DWORD     dwHeader = 64;
	char      chValue[MAX_LINE];
	int  i;
	BYTE      pByteBinData[100000];
	CString   strToken;
	char      sep[]=",\t\n";
	char   chBuffer[MAX_LINE];
	LPCTSTR  lpStr1;
	CString  strToken1;
	int ch;
	CString strTemperature;
	CString strLutFile;
	int nTemp;
	double dblTemperature;
	DWORD  dwTemperature;
	CStdioFile stdPDFile;

	PBYTE 	 pbswInfo;

    ZeroMemory(pByteBinData,sizeof(BYTE)*100000);
	ZeroMemory(chValue,sizeof(char)*MAX_LINE);

	memset(&g_stMemsSwCoef, 0x00, sizeof(g_stMemsSwCoef));


	g_stMemsSwCoef.bSWTypeChanNum   = 0x13;
	if (m_strPN == "77501369A11")
	{
		g_stMemsSwCoef.bSWTypeChanNum   = 0x12;
	}
    g_stMemsSwCoef.bReserved0 = 0x00;
	g_stMemsSwCoef.bDarkPoint = 0x01;
	g_stMemsSwCoef.bReserved1 = 0x00;
	g_stMemsSwCoef.wSwitchDelayUs = 0xC8;
	g_stMemsSwCoef.bReserved2[0] = 0x00;
	g_stMemsSwCoef.bReserved2[1] = 0x00;
	//g_stMemsSwCoef.bMidPoint[];
	//g_stMemsSwCoef.wSwitchIL[];
	g_stMemsSwCoef.pstMidPointsMatrix.bMidPointNum   = 0x00;
	//g_stMemsSwCoef.pstMidPointsMatrix
	//g_stMemsSwCoef.bReserved6[];



	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	else
	{
		strPath = g_tszAppFolder;
	}
    
	//bBundleTag
	count = 0;
    CString strBundle = "OPLINK";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strBundle,strBundle.GetLength());
    for (i=0;i<strBundle.GetLength();i++)
	{
		//pByteBinData[count++] = (BYTE)(chValue[i]);
		g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(chValue[i]);
    }	
	//bProductType
	count = 8;
	CString strProductType = "SWITCH";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strProductType,strProductType.GetLength());
    for (i=0;i<strProductType.GetLength();i++)
	{
		//pByteBinData[count++] = (BYTE)(chValue[i]);
		g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(chValue[i]);
    }
	//dwBundleHdrCRC32(Reserved)
	DWORD dwHdrCRC32 = 0;
	count = 16;
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>24);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>16);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>8);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>24);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>16);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>8);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32);
	//dwBundleSize
//	DWORD dwBundleSize = 0x02A0;
//	pByteBinData[count++] = (BYTE)(dwBundleSize>>24);
//	pByteBinData[count++] = (BYTE)(dwBundleSize>>16);
//	pByteBinData[count++] = (BYTE)(dwBundleSize>>8);
//	pByteBinData[count++] = (BYTE)(dwBundleSize);
	DWORD dwBundleSize = 0x08A0;
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>24);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>16);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>8);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize);
	//wBundleHdrSize
	WORD	wBundleHdrSize = 128;
	//pByteBinData[count++] = (BYTE)(wBundleHdrSize>>8);
	//pByteBinData[count++] = (BYTE)(wBundleHdrSize);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(wBundleHdrSize>>8);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(wBundleHdrSize);
	//wImageCount
	WORD wImageCount = 1;
//	pByteBinData[count++] = (BYTE)(wImageCount>>8);
//	pByteBinData[count++] = (BYTE)(wImageCount);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(wImageCount>>8);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(wImageCount);
	//
	//bBundleVersion
	count = 32;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (i=0;i<m_strSN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		g_stMemsSwCoef.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	//bPartNumber
	count = 48;
    //strPN = m_strPortNumber;
	//strPN = "14538_1x64_MEMSW";
	strPN = "14538_1x64MemsSw";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strPN,strPN.GetLength());
    for (i=0;i<strPN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		g_stMemsSwCoef.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	// Serial Number of this product,
    count = 80;
	CString strtempsn;
	
	strtempsn = "SUPERSN";
	ZeroMemory(chValue,sizeof(chValue));
	//memcpy(chValue,m_strSN,m_strSN.GetLength());
	memcpy(chValue, strtempsn, strtempsn.GetLength());
    for (i=0;i<strtempsn.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		g_stMemsSwCoef.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	// The time at which this bundle file generated
	SYSTEMTIME st;
	GetSystemTime(&st);
	CString strTime;
	strTime.Format("%04d.%02d.%02d.%02d.%02d",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	count = 112;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strTime,strTime.GetLength());
    for (i=0;i<strTime.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		g_stMemsSwCoef.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }

		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x49;
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x42;
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x46;
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x48;
	//ImageType
    	g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x85;
	//ImageHitless
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x00;
	//StorageID
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x01;
	//ImageIndex
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x01;
	//dwImageVersion
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x00;
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x00;
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x00;
		g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x00;
	//dwTimeStamp
		g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(st.wMonth);
		g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(st.wDay);
		g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(st.wHour);
		g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(st.wMinute);
    
	//dwBaseAddress
	DWORD dwBaseAddress=0x0E000;
	if (b_issn1) 
	{
		dwBaseAddress=0x0E000;
	}
	else if (b_issn2)
	{
		dwBaseAddress=0x0E800;
	}
	else if(b_issn3)
	{
		dwBaseAddress=0x0F000;
	}
	else if(b_issn4)
	{
		dwBaseAddress=0x0F800;
	}
	else
	{
		strMsg="数据地址识别失败";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
		
	}
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>24);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>16);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>8);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress);
	//dwImageCRC32
	count = 148;
    DWORD dwImageCRC32 = 0;

	LPCTSTR  lpStr;
	CString strLUTFile;
	CStdioFile stdFile;
	char chLineBuff[MAX_LINE];
	//1.dwSystemFlag 0x00 length:4
	//2.写入产品信息
	//system tag
	/*count = 160;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	
	strLUTFile.Format("%s\\data\\%s\\%s\\%s\\BIN\\ProductInfo_LUT.csv",strPath,m_strItemName,m_strPN,m_strSN);
    if(!stdFile.Open(strLUTFile,CFile::modeRead|CFile::typeText))
	{
		strMsg.Format("打开文件%s失败！",strLUTFile);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	count = 164;
	//Vendor,16
	lpStr = stdFile.ReadString(chLineBuff,MAX_LINE);
	strToken = strtok(chLineBuff,sep);
	ZeroMemory(chValue,sizeof(char)*MAX_LINE);
	memcpy(chValue,strToken,strToken.GetLength());
	for (i=0;i<strToken.GetLength();i++)
	{
		pByteBinData[count++] = (BYTE)(chValue[i]);
	}
	//SN,16
	count = 164+16;
    lpStr = stdFile.ReadString(chLineBuff,MAX_LINE);
	strToken = strtok(chLineBuff,sep);
	ZeroMemory(chValue,sizeof(char)*MAX_LINE);
	memcpy(chValue,strToken,strToken.GetLength());
	for (i=0;i<strToken.GetLength();i++)
	{
		pByteBinData[count++] = (BYTE)chValue[i];
	}
	count = 164+16*2;
	//PN,32bytes	
	lpStr = stdFile.ReadString(chLineBuff,MAX_LINE);
	strToken = strtok(chLineBuff,sep);
	ZeroMemory(chValue,sizeof(char)*MAX_LINE);
	memcpy(chValue,strToken,strToken.GetLength());
	for (i=0;i<strToken.GetLength();i++)
	{
		pByteBinData[count++] = (BYTE)chValue[i];
	}
	count = 164+16*4;
	//HW Version，16
    lpStr = stdFile.ReadString(chLineBuff,MAX_LINE);
	strToken = strtok(chLineBuff,sep);
	ZeroMemory(chValue,sizeof(char)*MAX_LINE);
	memcpy(chValue,strToken,strToken.GetLength());
	for (i=0;i<strToken.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]>>8);
		pByteBinData[count++] = (BYTE)chValue[i];
	}
	count = 164+16*5;
	//MFG Date，16
    lpStr = stdFile.ReadString(chLineBuff,MAX_LINE);
	strToken = strtok(chLineBuff,sep);
	ZeroMemory(chValue,sizeof(char)*MAX_LINE);
	memcpy(chValue,strToken,strToken.GetLength());
	for (i=0;i<strToken.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]>>8);
		pByteBinData[count++] = (BYTE)chValue[i];
	}
	count = 164+16*6;
////////////////////////////
	//3.报警温度
    int AlarmMaxTemp = 850;  //告警温度改为85
	int AlarmMinTemp =  -100;
    pByteBinData[count++] = (BYTE)(AlarmMaxTemp>>8);
	pByteBinData[count++] = (BYTE)AlarmMaxTemp;
	pByteBinData[count++] = (BYTE)(AlarmMinTemp>>8);
	pByteBinData[count++] = (BYTE)AlarmMinTemp;

	//4.定标温度
	int lTempSlope2 = 0;
	int lTempSlope1 = 40000;
	int lTempOffset = -32768000;
    int dwTempShift = 16;
	pByteBinData[count++] = (BYTE)(lTempSlope2>>24);
	pByteBinData[count++] = (BYTE)(lTempSlope2>>16);
	pByteBinData[count++] = (BYTE)(lTempSlope2>>8);
	pByteBinData[count++] = (BYTE)(lTempSlope2);
	
	pByteBinData[count++] = (BYTE)(lTempSlope1>>24);
	pByteBinData[count++] = (BYTE)(lTempSlope1>>16);
	pByteBinData[count++] = (BYTE)(lTempSlope1>>8);
	pByteBinData[count++] = (BYTE)(lTempSlope1);

	pByteBinData[count++] = (BYTE)(lTempOffset>>24);
	pByteBinData[count++] = (BYTE)(lTempOffset>>16);
	pByteBinData[count++] = (BYTE)(lTempOffset>>8);
	pByteBinData[count++] = (BYTE)(lTempOffset);

	pByteBinData[count++] = (BYTE)(dwTempShift>>24);
	pByteBinData[count++] = (BYTE)(dwTempShift>>16);
	pByteBinData[count++] = (BYTE)(dwTempShift>>8);
	pByteBinData[count++] = (BYTE)(dwTempShift);*/

	//获取需下载的通道数和通道名
	
	//5.通道定标数据
	//写入开关通道数据
	int  dwPointX[32];
	int  dwPointY[32];
	float  dwPointIL[32];
	CString strpointx,strpointy,strpointil;
	ZeroMemory(dwPointX,sizeof(dwPointX));
	ZeroMemory(dwPointY,sizeof(dwPointY));



	if (!m_bisinit)
	{
	
	
	
	for (nTemp=0;nTemp<3;nTemp++)
	{
		if (nFile==1)
		{
			strTemperature="Room";
		}
		else if (nFile==2)
		{
			strTemperature="Low";
		}
		else if (nFile==3)
		{
			strTemperature="High";
		}
		else if (nFile==4)
		{
			if (nTemp==0)
			{
				strTemperature="Low";
			}
			else if (nTemp==1)
			{
				strTemperature="Room";
			}
			else if (nTemp==2)
			{
				strTemperature="Room";
			}
		}
		else if (nFile==5)
		{
			if (nTemp==0)
			{
				strTemperature="Low";
			}
			else if (nTemp==1)
			{
				strTemperature="Room";
			}
			else if (nTemp==2)
			{
				strTemperature="High";
			}
		
		}
	
	

		for (ch=0;ch<m_nCHCount;ch++)
		{
			if (strTemperature=="High")
			{
				strLutFile.Format("%s\\Data\\%s\\%s\\%s\\%s_ScanVoltage_%d-High.csv",
					strPath,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
			}
			else if (strTemperature=="Low")
			{
				strLutFile.Format("%s\\Data\\%s\\%s\\%s\\%s_ScanVoltage_%d-Low.csv",
					strPath,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
			}
			else
			{
				strLutFile.Format("%s\\Data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",
					strPath,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
			}
		
			if(!stdPDFile.Open(strLutFile,CFile::modeRead|CFile::typeText))
			{
				strMsg.Format("打开文件%s失败！",strLutFile);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				return FALSE;
			}
			ZeroMemory(chBuffer,sizeof(char)*MAX_LINE);
			lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
			if (lpStr1 == NULL)
			{
				LogInfo("读取LUT表错误，数据缺失!",FALSE,COLOR_RED);
				stdPDFile.Close();
				return FALSE;
			}
			lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
			strToken1 = strtok(chBuffer,sep);
			strToken1 = strtok(NULL,sep);
			dwPointX[ch] = atoi(strToken1);
			dwPointX[ch] = dwPointX[ch]*MAX_DAC/60000;
			strToken1 = strtok(NULL,sep);
			dwPointY[ch] = atoi(strToken1);
			dwPointY[ch] = dwPointY[ch]*MAX_DAC/60000;	
			stdPDFile.Close();
			strToken1 = strtok(NULL,sep);
			dwPointIL[ch] = atof(strToken1);

			if(fabs(dwPointIL[ch]) > 1.0)
			{
				strMsg.Format("通道%d%s温IL大于1.0，请检查！",ch+1,strTemperature);
				LogInfo(strMsg,FALSE,COLOR_RED);
				stdPDFile.Close();
				MessageBox(strMsg);
				return FALSE;
			}
			dwPointIL[ch] = dwPointIL[ch]*100;	
		
              
			if (strTemperature=="High")
			{
				//TEST
				m_lutHighTep = "1B20";
				g_stMemsSwCoef.stCalibDAC[2].wValid = 0x01 ; 
				g_stMemsSwCoef.stCalibDAC[2].sTemperature = strtol(m_lutHighTep,NULL,16);
			
                if (dwPointX[ch]>0)
				{
					if (dwPointX[ch] >255)
					{
						strpointx.Format("0%x",dwPointX[ch]);
					}
					else if (dwPointX[ch]<16)
					{
						strpointx.Format("000%x",dwPointX[ch]);
					}
					else
					{
						strpointx.Format("00%x",dwPointX[ch]);
					}
                }
				if (dwPointX[ch]<0)
				{
						strpointx.Format("%x",dwPointX[ch]);
			            strpointx= strpointx.Right(4);

				}
				
				if (dwPointY[ch]>0)
				{
					if (dwPointY[ch] >255)
					{
						strpointy.Format("0%x",dwPointY[ch]);
					}
					else if (dwPointY[ch]<16)
					{
						strpointy.Format("000%x",dwPointY[ch]);
					}
					else
					{
						strpointy.Format("00%x",dwPointY[ch]);
					}
                }
				if (dwPointY[ch]<0)
				{
						strpointy.Format("%x",dwPointY[ch]);
			            strpointy= strpointy.Right(4);

				}
				
				g_stMemsSwCoef.stCalibDAC[2].stChnDAC[ch].sDACx = strtol(strpointx,NULL,16);
             	//g_stMemsSwCoef.stCalibDAC[2].stChnDAC[ch].sDACx[0] = strtol(strpointx.Right(2),NULL,16);
				g_stMemsSwCoef.stCalibDAC[2].stChnDAC[ch].sDACy= strtol(strpointy,NULL,16);
             	//g_stMemsSwCoef.stCalibDAC[2].stChnDAC[ch].sDACy[0] = strtol(strpointy.Right(2),NULL,16);
				
				
			}
			else if  (strTemperature=="Low" )
			{
				m_lutLowTep = "FE3E";
				g_stMemsSwCoef.stCalibDAC[0].wValid = 0x01 ; 
				g_stMemsSwCoef.stCalibDAC[0].sTemperature = strtol(m_lutLowTep,NULL,16);

			    if (dwPointX[ch]>0)
				{
					if (dwPointX[ch] >255)
					{
						strpointx.Format("0%x",dwPointX[ch]);
					}
					else if (dwPointX[ch]<16)
					{
						strpointx.Format("000%x",dwPointX[ch]);
					}
					else
					{
						strpointx.Format("00%x",dwPointX[ch]);
					}
                }
				if (dwPointX[ch]<0)
				{
						strpointx.Format("%x",dwPointX[ch]);
			            strpointx= strpointx.Right(4);

				}
				
				if (dwPointY[ch]>0)
				{
					if (dwPointY[ch] >255)
					{
						strpointy.Format("0%x",dwPointY[ch]);
					}
					else if (dwPointY[ch]<16)
					{
						strpointy.Format("000%x",dwPointY[ch]);
					}
					else
					{
						strpointy.Format("00%x",dwPointY[ch]);
					}
                }
				if (dwPointY[ch]<0)
				{
						strpointy.Format("%x",dwPointY[ch]);
			            strpointy= strpointy.Right(4);

				}
				
				g_stMemsSwCoef.stCalibDAC[0].stChnDAC[ch].sDACx = strtol(strpointx,NULL,16);
				//g_stMemsSwCoef.stCalibDAC[0].stDAC[ch].sDACx[1] = strtol(strpointx.Left(2),NULL,16);
             	//g_stMemsSwCoef.stCalibDAC[0].stDAC[ch].sDACx[0] = strtol(strpointx.Right(2),NULL,16);
				g_stMemsSwCoef.stCalibDAC[0].stChnDAC[ch].sDACy = strtol(strpointy,NULL,16);
             	//g_stMemsSwCoef.stCalibDAC[0].stDAC[ch].sDACy[0] = strtol(strpointy.Right(2),NULL,16);

				if (nFile == 2)
				{
					m_lutRoomTep = "0997";
					g_stMemsSwCoef.stCalibDAC[1].wValid = 0x01 ; 
					//g_stMemsSwCoef.stCalibDAC[1].wValid[1] = 0x00 ; 
					g_stMemsSwCoef.stCalibDAC[1].sTemperature = strtol(m_lutRoomTep,NULL,16);
					//g_stMemsSwCoef.stCalibDAC[1].sTemperature[1] = strtol(m_lutRoomTep.Left(2),NULL,16);
					g_stMemsSwCoef.stCalibDAC[1].stChnDAC[ch].sDACx = strtol(strpointx,NULL,16);
             		//g_stMemsSwCoef.stCalibDAC[1].stChnDAC[ch].sDACx[0] = strtol(strpointx.Right(2),NULL,16);
					g_stMemsSwCoef.stCalibDAC[1].stChnDAC[ch].sDACy = strtol(strpointy,NULL,16);
             		//g_stMemsSwCoef.stCalibDAC[1].stChnDAC[ch].sDACy[0] = strtol(strpointy.Right(2),NULL,16);
                    
				}
			
				
			}
			else
			{
				m_lutRoomTep = "0997";
				g_stMemsSwCoef.stCalibDAC[1].wValid = 0x01 ; 
				//g_stMemsSwCoef.stCalibDAC[1].wValid[1] = 0x00 ; 
				g_stMemsSwCoef.stCalibDAC[1].sTemperature = strtol(m_lutRoomTep,NULL,16);
				//g_stMemsSwCoef.stCalibDAC[1].sTemperature[1] = strtol(m_lutRoomTep.Left(2),NULL,16);
			    if (dwPointX[ch]>0)
				{
					if (dwPointX[ch] >255)
					{
						strpointx.Format("0%x",dwPointX[ch]);
					}
					else if (dwPointX[ch]<16)
					{
						strpointx.Format("000%x",dwPointX[ch]);
					}
					else
					{
						strpointx.Format("00%x",dwPointX[ch]);
					}
                }
				if (dwPointX[ch]<0)
				{
						strpointx.Format("%x",dwPointX[ch]);
			            strpointx= strpointx.Right(4);

				}
				
				if (dwPointY[ch]>0)
				{
					if (dwPointY[ch] >255)
					{
						strpointy.Format("0%x",dwPointY[ch]);
					}
					else if (dwPointY[ch]<16)
					{
						strpointy.Format("000%x",dwPointY[ch]);
					}
					else
					{
						strpointy.Format("00%x",dwPointY[ch]);
					}
                }
				if (dwPointY[ch]<0)
				{
						strpointy.Format("%x",dwPointY[ch]);
			            strpointy= strpointy.Right(4);

				}

				int trshex;

				trshex=dwPointIL[ch];
				
				if (dwPointIL[ch] >255)
				{
					strpointil.Format("0%x",trshex);
				}
				else if (dwPointIL[ch]<16)
				{
					strpointil.Format("000%x",trshex);
				}
				else
				{
					strpointil.Format("00%x",trshex);
				}
				
				g_stMemsSwCoef.stCalibDAC[1].stChnDAC[ch].sDACx = strtol(strpointx,NULL,16);
             	//g_stMemsSwCoef.stCalibDAC[1].stChnDAC[ch].sDACx[0] = strtol(strpointx.Right(2),NULL,16);
				g_stMemsSwCoef.stCalibDAC[1].stChnDAC[ch].sDACy = strtol(strpointy,NULL,16);
             	//g_stMemsSwCoef.stCalibDAC[1].stChnDAC[ch].sDACy[0] = strtol(strpointy.Right(2),NULL,16);
				g_stMemsSwCoef.wSwitchIL[ch] = strtol(strpointil,NULL,16);
				//g_stMemsSwCoef.wSwitchIL[ch][0] = strtol(strpointil.Right(2),NULL,16);
				
			}
		}
		if (m_nSwitchComPortCount==2)
		{
			for (ch=0;ch<m_nCHCount;ch++)
			{
				if (strTemperature=="High")
				{
					strLutFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1-High.csv",
						strPath,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
				}
				else if (strTemperature=="Low")
				{
					strLutFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1-Low.csv",
						strPath,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
				}
				else
				{
					strLutFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
						strPath,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
				}
			
				if(!stdPDFile.Open(strLutFile,CFile::modeRead|CFile::typeText))
				{
					strMsg.Format("打开文件%s失败！",strLutFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				ZeroMemory(chBuffer,sizeof(char)*MAX_LINE);
				lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
				if (lpStr1 == NULL)
				{
					LogInfo("读取LUT表错误，数据缺失!",FALSE,COLOR_RED);
					stdPDFile.Close();
					return FALSE;
				}
				lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
				strToken1 = strtok(chBuffer,sep);
				strToken1 = strtok(NULL,sep);
				dwPointX[ch+m_nCHCount] = atoi(strToken1);
				dwPointX[ch+m_nCHCount] = dwPointX[ch+m_nCHCount]*MAX_DAC/60000;
				strToken1 = strtok(NULL,sep);
				dwPointY[ch+m_nCHCount] = atoi(strToken1);
				dwPointY[ch+m_nCHCount] = dwPointY[ch+m_nCHCount]*MAX_DAC/60000;	
				stdPDFile.Close();
			}
		}

		}
		
		//查找最优IL
		float dwminIL1=50,dwminIL2=50;
		int indexminIL1=0,indexminIL2=0;

			/*g_stMemsSwCoef.bMaxChannelNUM = 0x13;

			if (m_strPN == "77501369A11")
			{
					g_stMemsSwCoef.bMaxChannelNUM = 0x12;
			}
			if (m_strPN == "77501369A11")
			{
				for(int xx=1;xx < 19;xx++)
				{
					g_stMemsSwCoef.bSWChnMapping[xx-1] = 0x00+xx;
				}
			}
			else
			{
				for(int xx=1;xx < 20;xx++)
			{
			}

				g_stMemsSwCoef.bSWChnMapping[xx-1] = 0x00+xx;
					
			}*/

		

			if (m_bischange)
			{
				  if (!b_issn1 && m_strID!="4710" && m_strID!="2953" && m_strID!="5360"&&m_strID!="271")
				  {
					  MessageBox("只允许第一个SW挑选通道");
					  m_bischange = FALSE;
					  return FALSE;
					  
				  }
				  else
				  {
					  CString m_strda;
					  m_strda.Format("%s启用工程师权限手动切换通道%d和通道%d。",m_strID,m_startchange1,m_startchange2);
                      LogInfo(m_strda);
					  
				  }
				   CString m_strchan1,m_strchan2;

                    /*g_stMemsSwCoef.bSWChnMapping[m_startchange1-1] = 0x12;
					g_stMemsSwCoef.bSWChnMapping[m_startchange2-1] = 0x13;
					if (m_strPN == "77501369A11")
					{
						g_stMemsSwCoef.bSWChnMapping[m_startchange1-1] = 0x11;
						g_stMemsSwCoef.bSWChnMapping[m_startchange2-1] = 0x12;

					}*/
					if (m_startchange1 > 0 && m_startchange2 > 0 )
					{
						if (m_startchange1>15)
						{
							m_strchan1.Format("%x",m_startchange1);
						}
						else
						{
							m_strchan1.Format("0%x",m_startchange1);

						}
						if (m_startchange2>15)
						{
							m_strchan2.Format("%x",m_startchange2);
						}
						else
						{
							m_strchan2.Format("0%x",m_startchange2);

						}
						/*if (m_strPN == "77501369A11")
						{
						   g_stMemsSwCoef.bSWChnMapping[16] = strtol(m_strchan1,NULL,16);
						   g_stMemsSwCoef.bSWChnMapping[17] = strtol(m_strchan2,NULL,16);
						   //g_stMemsSwCoef.bSWChnMapping[8] = 0x12;
						}
						else
						{
						   g_stMemsSwCoef.bSWChnMapping[17] = strtol(m_strchan1,NULL,16);
						   g_stMemsSwCoef.bSWChnMapping[18] = strtol(m_strchan2,NULL,16);
						}
						*/
					}
					
					
			}
		
			

	}

		//memset(g_stMemsSwCoef.bMidPoint, 0x01, 36);
     
	
		
		for (ch=0;ch<50;ch++)   //写开关通道坐标+18MID POIINT
		{
			
			if (m_nSwitchComPortCount==2)
			{
				if (ch<m_nCHCount*2)  //填写数据
				{
					pByteBinData[count++] = (BYTE)(dwPointX[ch]>>8);
					pByteBinData[count++] = (BYTE)(dwPointX[ch]);
					pByteBinData[count++] = (BYTE)(dwPointY[ch]>>8);
		    		pByteBinData[count++] = (BYTE)(dwPointY[ch]);
				}
				else            //其他位置写0
				{
					pByteBinData[count++] = 0x00;
					pByteBinData[count++] = 0x00;
					pByteBinData[count++] = 0x00;
					pByteBinData[count++] = 0x00;
				}
			}
			else
			{
				if (ch<m_nCHCount)  //填写数据
				{
					pByteBinData[count++] = (BYTE)(dwPointX[ch]>>8);
					pByteBinData[count++] = (BYTE)(dwPointX[ch]);
					pByteBinData[count++] = (BYTE)(dwPointY[ch]>>8);
		    		pByteBinData[count++] = (BYTE)(dwPointY[ch]);
				}
				else            //其他位置写0
				{
					pByteBinData[count++] = 0x00;
					pByteBinData[count++] = 0x00;
					pByteBinData[count++] = 0x00;
					pByteBinData[count++] = 0x00;
				}
			}	
		}

	

	//6.写入切换路径映射表数据.
    for (ch=0;ch<32;ch++)
	{
		pByteBinData[count++] = 0x00;
		pByteBinData[count++] = 0x00;
	}

	//7.DarkPoint
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x20;

	//8.通道数
	if (m_nSwitchComPortCount==2)
	{
		pByteBinData[count++] = (BYTE)((m_nCHCount*2)>>8);
	    pByteBinData[count++] = (BYTE)(m_nCHCount*2);
	}
	else
	{
		pByteBinData[count++] = (BYTE)(m_nCHCount>>8);
    	pByteBinData[count++] = (BYTE)m_nCHCount;
	}

	//9.写入温度数据	
	for (nTemp=0;nTemp<3;nTemp++)
	{
	    if (nFile==1)
		{
			strTemperature="Room";
		}
		else if (nFile==2)
		{
			strTemperature="Low";
		}
		else if (nFile==3)
		{
			strTemperature="High";
		}
		else if (nFile==4)
		{
			if (nTemp==0)
			{
				strTemperature="Low";
			}
			else if (nTemp==1)
			{
				strTemperature="Room";
			}
			else if (nTemp==2)
			{
				strTemperature="Room";
			}
		}
		else if (nFile==5)
		{
			if (nTemp==0)
			{
				strTemperature="Low";
			}
			else if (nTemp==1)
			{
				strTemperature="Room";
			}
			else if (nTemp==2)
			{
				strTemperature="High";
			}
		
		}
//		strLutFile.Format("%s\\data\\%s\\%s\\LUT\\TempData_%s.csv",g_tszAppFolder,m_strPN,m_strSN,strTemperature);
//		
//		if(!stdPDFile.Open(strLutFile,CFile::modeRead|CFile::typeText))
//		{
//			strMsg.Format("打开文件%s失败！",strLutFile);
//			MessageBox(strMsg);
//			return FALSE;
//		}
//		lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
//		strToken1 = strtok(chBuffer,sep);
//		dblTemperature = atof(strToken1);
		if (strTemperature=="Low")
		{
			dblTemperature = m_dblTemperature[1];
		}
		else if (strTemperature=="Room")
		{
			dblTemperature = m_dblTemperature[0];
		}
		else if (strTemperature=="High")
		{
			dblTemperature = m_dblTemperature[2];
		}
		else
		{
			dblTemperature = m_dblTemperature[0];
		}
		
		dwTemperature = (DWORD)(dblTemperature*10);
		pByteBinData[count++] = (BYTE)(dwTemperature>>8);
		pByteBinData[count++] = (BYTE)(dwTemperature);
//		stdPDFile.Close();
	}

	//开关切换时间10ms = 200us*500
	WORD wSwitchTime = 200;
	pByteBinData[count++] = (BYTE)(wSwitchTime>>8);
	pByteBinData[count++] = (BYTE)(wSwitchTime);

	g_stMemsSwCoef.wSwitchDelayUs = 0xC8;
	//g_stMemsSwCoef.wSwitchDelayUs[0] = 0x90;



	//10.Reserved
    for (i=0;i<534;i++)
	{
		pByteBinData[count++] = 0xFF;
    }
//	 for (i=0;i<524;i++)
//	{
//		g_stMemsSwCoef.bReserved[i++] = 0xFF;
//    }

    //memset(g_stMemsSwCoef.bReserved, 0xFF, 524);

	// Serial Number of this product,
    count = 0;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (i=0;i<m_strSN.GetLength();i++)
	{
		g_stMemsSwCoef.bReserved7[count++]= (BYTE)(chValue[i]);
    }


	// The time at which this bundle file generated
	count = 16;
	strTime.Format("%04d%02d%02d%02d%02d",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strTime,strTime.GetLength());
    for (i=0;i<strTime.GetLength();i++)
	{
		g_stMemsSwCoef.bReserved7[count++]= (BYTE)(chValue[i]);
    }


	count = 152;
  //  DWORD dwImageSize = dwBinsize-160; //总长度减去文件头长度
	DWORD dwImageSize = 2048; //总长度减去文件头长度
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>24);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>16);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwImageSize>>8);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwImageSize);

	g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x01;
	g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x00;
	g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x00;
	g_stMemsSwCoef.BUNDLEHEADER[count++] = 0x00;

	
	pbswInfo = (PBYTE)&g_stMemsSwCoef;
	m_CRC32.InitCRC32();

	for (i = 160;i < 2204; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
	dwImageCRC32 = ~dwImageCRC32;
	g_stMemsSwCoef.dwCRC32 = dwImageCRC32;
	//g_stMemsSwCoef.dwCRC32[2] = (BYTE)(dwImageCRC32>>16);
	//g_stMemsSwCoef.dwCRC32[1] = (BYTE)(dwImageCRC32>>8);
	//g_stMemsSwCoef.dwCRC32[0] = (BYTE)(dwImageCRC32);





//	m_CRC32.InitCRC32();
// 	for (i = 160;i < count; i++)
// 	{
// 		dwImageCRC32 = m_CRC32.GetThisCRC(pByteBinData[i]);
// 	}
// 	dwImageCRC32 = ~dwImageCRC32;
//	pByteBinData[count++] = (BYTE)(dwImageCRC32>>24);
//	pByteBinData[count++] = (BYTE)(dwImageCRC32>>16);
//	pByteBinData[count++] = (BYTE)(dwImageCRC32>>8);
//	pByteBinData[count++] = (BYTE)(dwImageCRC32);

	//得出文件长度
	dwBinsize = count;
	//dwImageCRC32
//    dwImageCRC32 = 0;
	m_CRC32.InitCRC32();
 	for (i = 160;i < 2208; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
 	dwImageCRC32 = ~dwImageCRC32;
	count = 148;
//	pByteBinData[count++] = (BYTE)(dwImageCRC32>>24);
//	pByteBinData[count++] = (BYTE)(dwImageCRC32>>16);
//	pByteBinData[count++] = (BYTE)(dwImageCRC32>>8);
//	pByteBinData[count++] = (BYTE)(dwImageCRC32);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>24);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>16);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>8);
    g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32);




	//计算header的CRC
	m_CRC32.InitCRC32();
 	for (i = 20;i < 128; i++)
 	{
 		dwHdrCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
 	dwHdrCRC32 = ~dwHdrCRC32;
	count = 16;
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>24);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>16);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>8);
//	pByteBinData[count++] = (BYTE)(dwHdrCRC32);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>24);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>16);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>8);
	g_stMemsSwCoef.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32);

	strMsg.Format("%s_DataBin.bin",m_strSN);
	if(!WriteBinFile(strMsg,strSave,2208,pbswInfo))
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::WriteBinFile(CString strBinName, CString strSave, DWORD dwLengthWrite, PBYTE pByteBinData)
{
	CString   strBinFileName;
	DWORD     dwByteWritten;
	HANDLE    hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA win32FindData;
	CString   strMsg;
	int       iSel = IDYES;
	CString   strNetFile; 
	HANDLE    hBinFile;
	
	strBinFileName.Format("%s\\data\\%s\\%s\\%s\\BIN\\%s", m_strNetFile,m_strItemName,m_strPN,m_strSN,strBinName);
	strNetFile.Format("%s\\data\\%s\\%s\\%s\\BIN\\%s",m_strNetFile,m_strItemName,m_strPN,m_strSN,strBinName);
	//strBinFileName.Format("%s\\data\\%s\\BIN\\%s",g_tszAppFolder,m_strSN,strBinName);
	//strNetFile.Format("%s\\data\\%s\\BIN\\%s",m_strNetFile,m_strSN,strBinName);


	
	hFindFile = FindFirstFile(strBinFileName,&win32FindData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		strMsg.Format("发现文件%s已经存在,直接覆盖数据",strBinFileName);
		//	iSel = MessageBox(strMsg,"确认",MB_YESNO|MB_ICONQUESTION);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_BLUE);
		iSel = IDYES;
		if (m_bIfSaveToServe)
		{
			//CopyFile(strBinFileName,strNetFile,FALSE);
		}
	}
	else
	{
		iSel = IDYES;
	}

	
		if (iSel == IDYES)
		{
			hBinFile = CreateFile(strBinFileName,GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, 0, NULL);
			if (hBinFile == INVALID_HANDLE_VALUE)
			{
				LogInfo("创建bin文件失败!",FALSE,COLOR_RED);
				return FALSE;
			}
			
			WriteFile(hBinFile,pByteBinData,dwLengthWrite,&dwByteWritten,NULL);
			if (dwLengthWrite != dwByteWritten)
			{
				LogInfo("写bin文件错误！",FALSE,COLOR_RED);
				return FALSE;
			}
			
			if (hBinFile != INVALID_HANDLE_VALUE )
			{
				CloseHandle(hBinFile);
				hBinFile = INVALID_HANDLE_VALUE;
			}
		}
		else if (iSel == IDNO)
		{
			LogInfo("数据未保存！",FALSE,COLOR_RED);
			return FALSE;
		}
	
	
	HANDLE	hDirectory = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	winFindFileData;
	
	if (m_bIfSaveToServe)
	{
		/*
		hDirectory = FindFirstFile(strNetFile,&winFindFileData);
		if (hDirectory != INVALID_HANDLE_VALUE)
		{	
			DeleteFile(strNetFile);	
		}
		
		if (!CopyFile(strBinFileName,strNetFile,FALSE)) 
		{
			LogInfo("保存数据至网络失败!",FALSE,COLOR_RED);
			//LogInfo(strBinFileName, FALSE, COLOR_RED);
			//LogInfo(strNetFile, FALSE, COLOR_RED);
			return FALSE;
		}
		*/
		strMsg.Format("开始下载文件%s",strNetFile);
		LogInfo(strMsg);
		if(!DataDownLoad(strNetFile))
		{
			return FALSE;
		}
	}
	else
	{
		strMsg.Format("开始下载文件%s",strBinFileName);
		LogInfo(strMsg);
        if(!DataDownLoad(strBinFileName))
		{
			return FALSE;
		}
	}
	
    return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnButtonCreateLut() 
{
	// TODO: Add your control notification handler code here
	if (!CreateLutFile())
	{
		LogInfo("创建LUT表出错！",(BOOL)FALSE,COLOR_RED);
		return;
	}
}

void CMy126S_45V_Switch_AppDlg::OnButtonDownloadBin() 
{
	// TODO: Add your control notification handler code here
	if(!m_blogin)
	{
		MessageBox("请登陆无纸化账号!");
		return;
	}
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bisinit)
	{
		    CString dwBaseAddress;
			int i_checkswnum;

		     if (b_issn1) 
			{
				//dwBaseAddress="17FE0";
				 dwBaseAddress = "0E7E0";
				
				i_checkswnum = 1;
			}
			else if (b_issn2)
			{
				//dwBaseAddress="187E0";
				dwBaseAddress = "0EFE0";
				
				i_checkswnum = 2;
			}
			else if(b_issn3)
			{
				//dwBaseAddress="18FE0";
				dwBaseAddress="0F7E0";
				
				i_checkswnum = 3;
			}
			else if(b_issn4)
			{
				//dwBaseAddress="197E0";
				dwBaseAddress = "0FFE0";
				
				i_checkswnum = 4;
			}
			else
			{
				return ;
			}

			/*TxDataToI2C("00D6",(BYTE*)"MFGCMD",6);
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}

			Sleep(100);
			RxDataFromI2C("00DC");
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}
            
 
		    Sleep(100);

			TxSTRToI2C("0166",dwBaseAddress); //设定寄存器地址
        
			if (m_strErrorMsg != "")
				{
				     MessageBox("请注意，初始化数据失败");
					LogInfo("读取内容失败。确认产品数据失败，请重试",BOOL(FALSE),COLOR_RED);
					return ;
				}
			Sleep(100);

			m_bisinit = TRUE;

			RxDataFromI2C("016A"); //读寄存器*/

			DWORD nAddress;
			sscanf(dwBaseAddress, "%x", &nAddress);

			char chData[64];
			ZeroMemory(chData,sizeof(char)*64);
			if(!GetMemoryData(nAddress,chData))
			{
				LogInfo("GetMemoryData失败!",FALSE,COLOR_RED);
				return ;
			}

			m_strErrorMsg.Format("%s",chData);

			LogInfo("开始确认产品信息",BOOL(FALSE),COLOR_GREEN);
			

			m_bisinit = FALSE;

			CString  strchan1,strchan2,strchan3,strchan4,checksn;
			int i_chan1,i_chan2,i_chan3,i_chan4;
			
			/*strchan1 = m_strErrorMsg.Left(2);
			strchan2 = m_strErrorMsg.Left(4);
			strchan2 = strchan2.Right(2);
			strchan3 = m_strErrorMsg.Left(6);
			strchan3 = strchan3.Right(2);
			strchan4 = m_strErrorMsg.Left(8);
			strchan4 = strchan4.Right(2);

			strchan1.Format("%c%c",chData[18],chData[19]);
			strchan2.Format("%c%c",chData[20],chData[21]);
			strchan3.Format("%c%c",chData[22],chData[23]);*/

			checksn = "";
			for(int nIndex=0; nIndex<24; )
			{
				strchan1.Format("%c%c",chData[nIndex],chData[nIndex+1]);
				i_chan1  = strtol(strchan1,NULL,16);
				strchan1.Format("%c",i_chan1);
				checksn += strchan1;
				nIndex = nIndex + 2;
			}
			
			/*i_chan1  = strtol(strchan1,NULL,16);
			i_chan2  = strtol(strchan2,NULL,16);
			i_chan3  = strtol(strchan3,NULL,16);
			//i_chan4  = strtol(strchan4,NULL,16);
			checksn.Format("%c%c%c",i_chan1,i_chan2,i_chan3);*/

            LogInfo(checksn,BOOL(FALSE),COLOR_GREEN);
			LogInfo(m_strSN,BOOL(FALSE),COLOR_GREEN);
			if (checksn.Find(m_strSN) != -1) 
			{
                LogInfo("SN号信息比对正确",BOOL(FALSE),COLOR_GREEN);
			}
			else
			{  
				LogInfo("SN信息对比错误，请确认是否已初始化产品信息！",(BOOL)FALSE,COLOR_RED);
				return;
			}
			/*if (i_chan4 == i_checkswnum)
			{
				LogInfo("开关位置信息比对正确",BOOL(FALSE),COLOR_GREEN);
			}
			else
			{  
				LogInfo("开关位置信息对比错误，请仔细检查开关填入位置信息",(BOOL)FALSE,COLOR_RED);
				return;
			}*/
			
			LogInfo("所有信息比对正确，开始下载开关数据",BOOL(FALSE),COLOR_GREEN);
			


	}
	

	if (!CreateBinFileAndDownload())
	{
		LogInfo("下载数据错误！",(BOOL)FALSE,COLOR_RED);
		m_biscreatbin = FALSE;
		return;
	}
	   m_biscreatbin = TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnCloseupComboTestItem() 
{
	// TODO: Add your control notification handler code here
	/*
	UpdateData();
	if (m_cbTestItem.GetCurSel()==0)
	{
		m_strItemName = "终测";
	}
	else if (m_cbTestItem.GetCurSel()==1)
	{
		m_strItemName = "初测";
	}
	*/
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonIsoTest() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	double  dblPower;
	m_bTestStop = FALSE;
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bOpenPort) 
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (m_cbComPort.GetCurSel()==0)
	{
		strMsg.Format("开始隔离度的测试!请将COM18接进光，COM19接光功率计通道1");
	}
	else if(m_cbComPort.GetCurSel()==1)
	{
		strMsg.Format("开始隔离度的测试!请将COM19接进光，COM18接光功率计通道1");
	}
	MessageBox(strMsg);
	for (int nCH=0;nCH<m_nCHCount;nCH++)
	{
		YieldToPeers();
		if (m_bTestStop)
		{
			LogInfo("测试终止");
			return;
		}
		if(!SWToChannel(nCH+1))
		{
			MessageBox("开关切换失败！");
			return;
		}
		Sleep(200);
		dblPower = ReadPWMPower(0);
		dblPower = m_dblReferencePower[nCH]-dblPower;
		ChannalMessage[nCH].dblISO = dblPower;	
		if(!CheckParaPassOrFail(ISO_DATA,ChannalMessage[nCH].dblISO))
		{
			strMsg.Format("Channel:%d; ISO:%.2f,不合格",nCH+1,dblPower);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("Channel:%d; ISO:%.2f",nCH+1,dblPower);
	     	LogInfo(strMsg);
		}
       
	}
	SaveDataToExcel(ISO_DATA);
	SaveDataToCSV(ISO_DATA);
	UpdateATMData(ISO_DATA);
	Sleep(100);
	LogInfo("测试完毕！");
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonSwitchTimeTest() 
{
	// TODO: Add your control notification handler code here
	//获取数据
	CString strCommand;
	char    chWrite[10];
	WORD    wRead[2048];
	char    chRead[256];
	CString strValue;
	CString strInfo;
	int     nRevPort;

	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	int nWL = m_cbSetWL.GetCurSel()-1;
	if (nWL>=0)
	{
		SetPWMWL(m_dblWL[nWL]);
		m_bChange=TRUE;
	}
	if (m_bConnect1830C)
	{
		if (nWL>=0)
		{
			if(!m_1830CRL.SetPWMParameters(0,1,0,m_dblWL[nWL],0.01,7))
			{
				LogInfo("设置1830C参数失败！");
				return;
			}
		}
		else
		{
			if(!m_1830CRL.SetPWMParameters(0,1,0,m_pdblWavelength,0.01,7))
			{
				LogInfo("设置1830C参数失败！");
				return;
			}
		}
		
	}
	//开关切换到1通道
	SWToChannel(1);
	Sleep(200);
	
	//获取当前通道位置
	strCommand.Format("GPRT\r");

	ZeroMemory(chWrite,sizeof(chWrite));
	ZeroMemory(chRead,sizeof(chRead));
    memcpy(chWrite,strCommand,strCommand.GetLength());
	
	if(!m_opCom.WriteBuffer(chWrite,strCommand.GetLength()))
	{
		MessageBox("串口打开错误！");
		return;
	}
	Sleep(100);
	if (!m_opCom.ReadBuffer(chRead,256))
	{
		MessageBox("接收数据错误！");
		return;
	}
	int nPrePort = atoi(chRead);

	UpdateData();
	int nComPort = m_cbComPort.GetCurSel();
//	if (m_nCHCount<12)
//	{
//		nRevPort = 12;
//	}
//	else
//	{
		nRevPort = m_nCHCount;
//	}	
	m_nSwitchTimeCH = nRevPort;
	strInfo.Format("请将通道%d接入1830C中",nRevPort);
	MessageBox(strInfo);
	strCommand.Format("SWTHS %d\r",nRevPort+nComPort*16);
	ZeroMemory(chWrite,sizeof(chWrite));
	ZeroMemory(wRead,sizeof(wRead));
    memcpy(chWrite,strCommand,strCommand.GetLength());

	if(!m_opCom.WriteBuffer(chWrite,strCommand.GetLength()))
	{
		MessageBox("串口打开错误！");
	    return;
	}
	Sleep(200);
	if (!m_opCom.ReadBuffer((char*)wRead,2048*sizeof(WORD)))
	{
		MessageBox("接收数据错误！");
		return;
	}
	m_NTGraph.ClearGraph();
	int nMAX=0;
	for (int j=1;j<wRead[0];j++)
	{
		if(nMAX<wRead[j])
		{
			nMAX = wRead[j];
		}
	}
	m_NTGraph.SetRange(-10,1024,-10,nMAX+100);
	for (int i=0;i<wRead[0];i++)
	{
		m_NTGraph.PlotXY(i,wRead[i+1],0);
	}

	//计算切换时间
	//计算终止点位置
	double dblFinalPoint = nMAX*0.9;
	int n = 1;
	for (;n<wRead[0];n++)
	{
		if (dblFinalPoint<=wRead[n])
		{
			break;
		}
	}
	double dblSwitch = n*50;
	CString strMsg;
	m_dblSwitchTime = dblSwitch/1000.0;
	if(!CheckParaPassOrFail(SWITCH_TIME_DATA,m_dblSwitchTime))
	{
		strMsg.Format("通道%d到通道%d开关切换时间：%.1f ms,不合格！",nPrePort,m_nSwitchTimeCH,m_dblSwitchTime);
    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
	}
	else
	{
		strMsg.Format("通道%d到通道%d开关切换时间：%.1f ms。",nPrePort,m_nSwitchTimeCH,m_dblSwitchTime);
    	LogInfo(strMsg);
	}
	
//	SaveDataToExcel(SWITCH_TIME_DATA,m_nSwitchTimeCH-1);
	SaveDataToCSV(SWITCH_TIME_DATA,nPrePort);
	UpdateATMData(SWITCH_TIME_DATA);

}

void CMy126S_45V_Switch_AppDlg::OnButtonReIlTest() 
{
	// TODO: Add your control notification handler code here
	CString strChannel;
	//开始随机切换
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bTestIL)
	{
		MessageBox("请先测试IL！");
		return;
	}
	int nWL = m_cbSetWL.GetCurSel()-1;
	if (nWL>=0)
	{
		SetPWMWL(m_dblWL[nWL]);
		m_bChange=TRUE;
	}

	//获取当前时间
	CString strMsg;
	SYSTEMTIME st;
	COleDateTime    tCurTime;
	COleDateTimeSpan tDifHour; 
	int        nDifHour;
	GetLocalTime(&st);
	tCurTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	tDifHour = tCurTime-m_RefStartTime; //计算时间插值
	nDifHour = tDifHour.GetTotalHours();
	if (nDifHour>m_nRefTime)  //比较归零时间差异
	{
		strMsg.Format("当前归零时间为%d小时，已超过归零限制时间（%d小时），请重新归零！",nDifHour,m_nRefTime);
		MessageBox(strMsg);
		m_bHasRefForTest = FALSE;
		return;
	}

	UpdateData();
	double dblTargetTempt;
	int    nTemptStatus; //设置温度状态
	double dblCurTempt; //当前温度
	CString strTemp;
	int nTemp = m_ctrlComboxTemperature.GetCurSel();
	if (nTemp==0)
	{
		strTemp.Format("开始重复性常温测试！");
		LogInfo(strTemp,FALSE);
		dblTargetTempt = 26;
		nTemptStatus = 0;
	}
	else if (nTemp==1)
	{
		strTemp.Format("开始重复性低温测试！");
		LogInfo(strTemp);
		dblTargetTempt = -5;
		nTemptStatus = 2;
	}
	else 
	{
		strTemp.Format("开始重复性高温测试！");
		LogInfo(strTemp);
		dblTargetTempt = 70;
		if (stricmp(m_strSpec,"Z4767")==0)
		{
			dblTargetTempt = 85;
		}
		nTemptStatus = 1;
	}	
	dblTargetTempt = m_dblTemperature[nTemp];
	//判断当前温度是否达到要求；
	if (m_bConnectTemptControl)
	{
		dblCurTempt = GetCurrentTempt(); //获取当前冷热盘温度
		if (fabs(dblTargetTempt-dblCurTempt)>2)
		{
			strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
			LogInfo(strMsg,BOOL(FALSE),COLOR_RED);
			SetTemptStatus(nTemptStatus);
			return;
		}
	}

	FindMyDirectory(m_strItemName,m_strPN,m_strSN);
	LogInfo("开始随机切换开关3800次...",TRUE);
	LogInfo("请等待60s......！");
	
	
	//随即平均切换100次
	if (!SendMsgToSwitch(0,3800)) //?
	{
		LogInfo("随机切换错误！");
		return;
	}

	for (int xloy = 1; xloy < 100 ;xloy ++)
	{
			Sleep(1000);
			int nstate = GetASWTHState();
			//RxDataFromI2C("0140"); //MemsSwitch Scan status
			if (nstate == 0)
			{
				break;
			}
			if (xloy>98)
			{
				LogInfo("重复性测试超时！",FALSE,COLOR_RED);
				return ;

			}
			strMsg.Format("时间：%d s",xloy);
			LogInfo(strMsg);
    }

	/*for (int i=40;i>0;i--)
	{
		strMsg.Format("剩余时间：%d s",i);
		LogInfo(strMsg);
		Sleep(1000);
	}*/
	LogInfo("开关切换完毕！");
	//记录IL数据！
	m_ctrlListMsg.ResetContent();
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("通道");
	m_NTGraph.SetYLabel("重复性");
	m_NTGraph.SetRange(0,m_nCHCount,-5,5);
	m_NTGraph.SetXGridNumber(m_nCHCount);
	m_NTGraph.SetYGridNumber(10);
	
	//保存文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;
	double dblPref;
	double dblPout;

	int nComPort = m_cbComPort.GetCurSel();
	
	strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_IL_Repeat-COM%d.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nComPort);
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle!=INVALID_HANDLE_VALUE)
	{
	}
	
	if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建数据文件失败！");
		return;
	}
	stdCSVFile.WriteString("Channel,IL,IL_Repeat,Diff\n");
	CString dwBaseAddress,m_strchan,strCommand;
	for (int nChannel = 0; nChannel < m_nCHCount; nChannel ++)
	{
		//SWToChannel(nChannel+1);


		
			/*TxDataToI2C("00D6",(BYTE*)"MFGCMD",6);
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}

			Sleep(100);
			RxDataFromI2C("00DC");
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}
            
 
		Sleep(100);*/

		 if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return ;
		}

		
		if (nChannel>14)
		{
			m_strchan.Format("%x",nChannel+1);
		}
		else
		{
			m_strchan.Format("0%x",nChannel+1);

		}

		if(!SetSWChannel(atoi(dwBaseAddress),nChannel+1))
		{
			LogInfo("设置SWChannel失败!",FALSE,COLOR_RED);
			return ;
		}		
        
		/*strCommand.Format("%s%s",dwBaseAddress,m_strchan);
		
		TxSTRToI2C("0143",strCommand);
        
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return ;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return ;
		}*/

		Sleep(800);
		if (nWL>=0)
		{
			dblPref = m_dblWDLRef[nChannel][nWL];
		}
		else
		{
			dblPref = m_dblReferencePower[nChannel];
		}
		
		dblPout = ReadPWMPower(nChannel);
		
		ChannalMessage[nChannel].dblRepeat = TestIL(dblPref,dblPout);
		ChannalMessage[nChannel].dblDif = ChannalMessage[nChannel].dblIL-ChannalMessage[nChannel].dblRepeat;
		
		if (!CheckParaPassOrFail(RE_IL_DATA,ChannalMessage[nChannel].dblDif))
		{
			strMsg.Format("切换开关后,通道%d的IL为:%.2f,重复性为:%.2f,参数不合格",nChannel+1,
				ChannalMessage[nChannel].dblRepeat,ChannalMessage[nChannel].dblDif);
	     	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);

		}
		else
		{
			strMsg.Format("切换开关后,通道%d的IL为:%.2f,重复性为:%.2f",nChannel+1,
				ChannalMessage[nChannel].dblRepeat,ChannalMessage[nChannel].dblDif);
     		LogInfo(strMsg,FALSE);
		}
		
		m_NTGraph.PlotXY(nChannel+1,ChannalMessage[nChannel].dblDif,0);
		
		strContent.Format("%d,%.3f,%.3f,%.3f\n",nChannel+1,ChannalMessage[nChannel].dblIL,ChannalMessage[nChannel].dblRepeat,ChannalMessage[nChannel].dblDif);
		stdCSVFile.WriteString(strContent);
		//保存重复性数据
	}	
	stdCSVFile.Close();	
	
//	SaveDataToExcel(RE_IL_DATA);
	SaveDataToCSV(RE_IL_DATA);
	//UpdateATMData(RE_IL_DATA);
	LogInfo("保存重复性数据结束!");

}

void CMy126S_45V_Switch_AppDlg::OnButtonRef() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CString strValue;
	m_bTestStop = FALSE;

	double     dblReadPwr;
	CString strReferenceFileName(""),strMsg(""),strRemarkFlag("");
	CString strMessage,hCSVFile;
//	int nCH;
	CString strInfo;
	LPTSTR pStr;
	char   sep[] = ",\t\n";
	char   *token = NULL;
	char   lineBuf[256];
	CString strTemp,strDataBeginFlag;
	CStdioFile fileReferenceFile;
	int iSel = IDYES;
	int nCount = 0;
	double  dblWL;
	SYSTEMTIME st;
	CString  strFile;
	int      nYear;
	int      nMonth;
	int      nDay;
	int      nHour;
	int      nMinute;
	int      nSecond;
	CString  strToken;
	COleDateTime     tStartTime;
	COleDateTime     tEndTime;
	COleDateTimeSpan tHour;
	int      nHourDiff;
	int      nCurCH;
	double dblTempPower;
	int iWL = 0;

	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先连接光功率计！");
		return;
	}
	if (!m_bEndRef)
	{
		MessageBox("请先拔出光功率计探头！");
		return;
	}
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}

	//获取归零配置：
	//需归零的次数
	strFile.Format("%s\\RefConfig.ini", m_strNetConfigPath);
	GetPrivateProfileString("RefCount","Value","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		MessageBox("读取RefConfig.ini配置RefCount错误！");
		return;
	}
	int nRefCount = atoi(strValue);
    strMsg.Format("需按通道顺序重复归零%d次，比较2次差异！",nRefCount);
	MessageBox(strMsg);

	GetPrivateProfileString("RefTime","Value","error",strValue.GetBuffer(128),128,strFile);
	if (strValue=="error")
	{
		MessageBox("读取RefConfig.ini配置RefTime错误！");
		return;
	}
	m_nRefTime = atoi(strValue);

	FindMyDirectory(m_strItemName,m_strPN,m_strSN);

	int nWL = m_cbSetWL.GetCurSel()-1;
	for (int i=0;i<m_nCHCount;i++)
	{
		if (nWL>=0)
		{
			strReferenceFileName.Format("%s\\data\\RefZero\\RefZeroData-%.0f-CH%d.txt",g_tszAppFolder,m_dblWL[nWL],i+1);
		}
		else
		{
			nWL=-1;
			strReferenceFileName.Format("%s\\data\\RefZero\\RefZeroData-CH%d.txt",g_tszAppFolder,i+1);
		}		
		if (GetFileAttributes(strReferenceFileName) == -1)
		{
			iSel = IDNO;
			break;
		}
	}
	if (iSel==IDYES)
	{
		strMsg.Format("最近一次归零数据已存在\n"
			"点击<是>\t调用该数据\n"
			"点击<否>\t重新归零\n");
		iSel = MessageBox(strMsg,"确认",MB_YESNO |MB_ICONQUESTION);
	}

	m_ctrlListMsg.ResetContent();
	strRemarkFlag = ";";					
	if (IDYES == iSel)
	{
		GetLocalTime(&st);
		for (int i=0;i<m_nCHCount;i++)
		{
			if (nWL>=0)
			{
				strReferenceFileName.Format("%s\\data\\RefZero\\RefZeroData-%.0f-CH%d.txt",g_tszAppFolder,m_dblWL[nWL],i+1);
			}
			else
			{
				strReferenceFileName.Format("%s\\data\\RefZero\\RefZeroData-CH%d.txt",g_tszAppFolder,i+1);
			}
		
			if (!(fileReferenceFile.Open(strReferenceFileName,CFile::modeRead|CFile::typeText,NULL)))
			{
				strMsg.Format("打开文件%s失败！请检查该归零文件是否存在",strReferenceFileName);
				LogInfo(strMsg,FALSE,COLOR_RED);
				return;
			}

			//获取时间
			pStr = fileReferenceFile.ReadString(lineBuf,256);
			strToken = strtok(lineBuf,"-");
			nYear = atoi(strToken);
			strToken = strtok(NULL,"-");
			nMonth = atoi(strToken);
			strToken = strtok(NULL,"-");
			nDay = atoi(strToken);
			strToken = strtok(NULL,"-");
			nHour = atoi(strToken);
			strToken = strtok(NULL,"-");
			nMinute = atoi(strToken);
			strToken = strtok(NULL,"-");
			nSecond = atoi(strToken);
			tStartTime.SetDateTime(nYear,nMonth,nDay,nHour,nMinute,nSecond);
			tEndTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

			tHour = tEndTime-tStartTime;
			nHourDiff = tHour.GetTotalHours(); //获取时间差异
		
			if (nHourDiff>m_nRefTime)
			{
				//TEST
				strMsg.Format("归零时间差异%d小时，已超过限制时间(%d小时)",nHourDiff,m_nRefTime);
				LogInfo(strMsg,FALSE,COLOR_RED);
				fileReferenceFile.Close();
				m_bHasRefForTest = FALSE;
				return;
			}
			pStr = fileReferenceFile.ReadString(lineBuf,256);
			for (iWL=0;iWL<m_nWLCount;iWL++)
			{
				pStr = fileReferenceFile.ReadString(lineBuf,256);
				//获取数据
				token = strtok(lineBuf,sep);
				strTemp.Format("%s",token);
				strTemp.TrimLeft();

				token = strtok(NULL,sep);
				strTemp.Format("%s",token);
				strTemp.TrimLeft();
				dblWL = atof(strTemp);

				token = strtok(NULL,sep);
				strTemp.Format("%s",token);
				strTemp.TrimLeft();
		
				if (nWL>=0)
				{
					m_dblWDLRef[i][nWL] = atof(strTemp);
					strMsg.Format("通道%d:波长：%.2f nm,归零值为:%.2fdBm",i+1,dblWL,m_dblWDLRef[i][nWL]);
				}
				else
				{
					m_dblWDLRef[i][iWL] = atof(strTemp);
					if (fabs(m_dblWL[iWL] - m_pdblWavelength)<0.2) //Center WL
					{
						m_dblReferencePower[i] = atof(strTemp);	
					}
					strMsg.Format("通道%d:波长：%.2f nm,归零值为:%.2fdBm",i+1,m_dblWL[iWL],m_dblWDLRef[i][iWL]);
				}				
				LogInfo(strMsg,FALSE);
			}
		    fileReferenceFile.Close();
		}
		m_RefStartTime = tStartTime;
		m_bHasRefForTest = TRUE;
		LogInfo("所有通道的归零已完成！");			
	}
	//重新归零
	else if (IDNO == iSel)
	{
		UpdateData();
		nCurCH = m_cbChannel.GetCurSel();
		if (nCurCH==0)
		{
			MessageBox("请选择正确的通道！");
			return;
		}		
		strMsg.Format("请连接光源线接光功率计%d\n<连接好后点击确定>",nCurCH);
		MessageBox(strMsg);
		if (nWL>=0)
		{
			strReferenceFileName.Format("%s\\data\\RefZero\\RefZeroData-%.0f-CH%d.txt",g_tszAppFolder,m_dblWL[nWL],nCurCH);
		}
		else
		{
			strReferenceFileName.Format("%s\\data\\RefZero\\RefZeroData-CH%d.txt",g_tszAppFolder,nCurCH);
		}
		
		if(!fileReferenceFile.Open(strReferenceFileName,CFile::modeCreate|CFile::modeReadWrite))
		{
			MessageBox("创建文件失败！");
			return;
		}
		GetLocalTime(&st);
		strMsg.Format("%d-%d-%d-%d-%d-%d\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		fileReferenceFile.WriteString(strMsg);
		m_RefStartTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);//获取归零开始时间。

		strMsg.Format("channel  WL  RefPower(dBm)\n");
		fileReferenceFile.WriteString(strMsg);

		if (nWL<0)
		{
			if(!SetPWMWL(m_pdblWavelength,nCurCH-1))
			{
				LogInfo("设置功率计波长失败！");
				return;
			}
			strTemp.Format("设置通道%d波长为%.2f成功",nCurCH,m_pdblWavelength);
			LogInfo(strTemp);
			m_bChange=TRUE;
			Sleep(300);
			dblReadPwr = ReadPWMPower(nCurCH-1);
			if (dblReadPwr<-20)
			{
				strInfo.Format("当前归零光功率为：%.2fdBm,请确认探头是否插入",dblReadPwr);
	     		LogInfo(strInfo,(BOOL)FALSE,COLOR_RED);
				fileReferenceFile.Close();
				return;
			}
			strInfo.Format("通道%d:波长：%.2f nm，归零光功率为：%.2fdBm",nCurCH,m_pdblWavelength,dblReadPwr);
			LogInfo(strInfo,FALSE);			
	    	m_dblWDLRef[nCurCH-1][0] = dblReadPwr;
			strMsg.Format("%d, %.1f ,  %.3f\n",nCurCH,m_pdblWavelength,dblReadPwr);
			fileReferenceFile.WriteString(strMsg);
		}
		else
		{
			for (iWL=0;iWL<m_nWLCount;iWL++)
			{
				g_pTLS->SetTLSWavelength(0,m_dblWL[iWL]);
				Sleep(100);
				if(!SetPWMWL(m_dblWL[iWL],nCurCH-1))
				{
					LogInfo("设置功率计波长失败！");
					return;
				}
				Sleep(100);
				strTemp.Format("设置通道%d波长为%.2f成功",nCurCH,m_dblWL[iWL]);
		    	LogInfo(strTemp);
				dblReadPwr = ReadPWMPower(nCurCH-1);
				m_dblWDLRef[nCurCH-1][iWL] = dblReadPwr;
				strMsg.Format("%d, %.2f ,  %.2f\n",nCurCH,m_dblWL[iWL],dblReadPwr);
				fileReferenceFile.WriteString(strMsg);
				strInfo.Format("通道%d:波长：%.2f nm，归零光功率为：%.2fdBm",nCurCH,m_dblWL[iWL],dblReadPwr);
				LogInfo(strInfo,FALSE);	
				if (fabs(m_dblWL[iWL] - m_pdblWavelength)<0.2)
				{
					m_dblReferencePower[nCurCH-1] = dblReadPwr;
				}
			}
		}
		fileReferenceFile.Close();	
		
		m_nRefCount[nWL+1][nCurCH-1]++; //当前通道归零次数

		m_bHasRefForTest = TRUE;
		for (int iIndex=0;iIndex<m_nCHCount;iIndex++)
		{
			if (m_nRefCount[nWL+1][iIndex]<nRefCount) //判断次数是否足够
			{
	//			strTemp.Format("归零%d 次数为%d",nWL+1,m_nRefCount[nWL+1][iIndex]);
	//			LogInfo(strTemp);
				m_bHasRefForTest = FALSE;
			}
			else
			{
				if (nRefCount>1) //归零次数大于一次时，比较最近2次归零的结果差异
				{
					if (nWL>=0)
					{
						dblTempPower = m_dblWDLRef[iIndex][nWL];
						if (fabs(m_dblPreRefPower[iIndex][0]-dblTempPower)>0.02) //差异过大时，报错
						{
							strMsg.Format("通道%d上次归零值为%.3f,本次归零值为%.3f,差异大于0.02，请重新归零！",iIndex+1,m_dblPreRefPower[nCurCH-1][0],dblTempPower);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							m_bHasRefForTest = FALSE;
						}
						else
						{
							strMsg.Format("通道%d已归零完毕！",iIndex+1);
							LogInfo(strMsg);
						//	m_bEndRef = TRUE;
						}
					}
					else
					{
						for (iWL=0;iWL<m_nWLCount;iWL++)
						{
							dblTempPower = m_dblWDLRef[iIndex][iWL];
						}
						if (fabs(m_dblPreRefPower[iIndex][iWL]-dblTempPower)>0.02) //差异过大时，报错
						{
							strMsg.Format("通道%d上次归零值为%.3f,本次归零值为%.3f,差异大于0.02，请重新归零！",iIndex+1,m_dblPreRefPower[nCurCH-1][iWL],dblTempPower);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							m_bHasRefForTest = FALSE;
						}
						else
						{
							strMsg.Format("通道%d已归零完毕！",iIndex+1);
							LogInfo(strMsg);
						//	m_bEndRef = TRUE;
						}
					}
				
					
				}
			}
			
		}
		if (nWL>=0)
		{
			m_dblPreRefPower[nCurCH-1][0] = m_dblWDLRef[nCurCH-1][nWL];
		}
		else
		{
			for (iWL=0;iWL<m_nWLCount;iWL++)
			{
				m_dblPreRefPower[nCurCH-1][iWL] = m_dblWDLRef[nCurCH-1][iWL];
			}
		}

		//保留当前通道序号
		m_nPreRefNum = nCurCH; 
		//开始监控当前归零通道，看其是否插拔，若未拔出，则做出标记
		m_bEndRef = FALSE;
		SetTimer(2,1000,NULL);
	} 
	else 
	{
		m_bHasRefForTest = FALSE;
	}
	if (m_bHasRefForTest)
	{
		MessageBox("归零已完成！");
	}
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonOneKeyTest() 
{
	// TODO: Add your control notification handler code here
	if (!m_bOpenPM)
	{
		MessageBox("请先连接光功率计！");
		return;
	}
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}

    //扫描
	if(!OneKeyScan())
	{
		MessageBox("扫描出错！");
		return;
	}

	//下载数据
	if (!CreateLutFile())
	{
		LogInfo("创建LUT表出错！",(BOOL)FALSE,COLOR_RED);
		return;
	}
	
	if (!CreateBinFileAndDownload())
	{
		LogInfo("下载数据错误！",(BOOL)FALSE,COLOR_RED);
		return;
	}
	//IL/CT测试
	Sleep(1000);
	OnButtonIlctTest();
// 		//如果是终测，则添加WDL测试功能
// 	if(m_cbTestItem.GetCurSel()==0)
// 	{
// 		if(!OneKeyWDLTest())
// 		{
// 			LogInfo("WDL测试失败！");
// 		}
// 	}

	//重复性测试
	OnButtonReIlTest();
	OnButtonDarkTest();
	
}

BOOL CMy126S_45V_Switch_AppDlg::OneKeyScan()
{
	CString strFileName,strMsg;
	int nChannel = 0;
	CString strFile;
	CString strValue;
	int iSel= IDYES;
	int nCHNumber=0;

	try{
		if (!m_bOpenPM)
		{
			MessageBox("请先连接光功率计！");
			return FALSE;
		}
		if (!m_bOpenPort)
		{
			MessageBox("请先打开串口！");
			return FALSE;
		}
		if (!m_bHasRefForTest)
		{
			MessageBox("请先归零！");
			return FALSE;
		}
		//设置路径
		UpdateData();
    	FindMyDirectory(m_strItemName,m_strPN,m_strSN);

		LogInfo("开始驱动开关，开始扫描...",FALSE);

        //初始化图形控件
		m_NTGraph.ClearGraph();
		m_NTGraph.SetXLabel("X(mv)");
		m_NTGraph.SetYLabel("Y(mv)");
		m_NTGraph.SetRange(-60000,60000,-60000,60000);
		m_NTGraph.SetXGridNumber(10);
		m_NTGraph.SetYGridNumber(10);

		CString strFileINI;
		int nComPort = m_cbComPort.GetCurSel();
		if (nComPort==0)
		{
			strFileINI = "ScanRange.ini";
		}
		else if (nComPort==1)
		{
			strFileINI = "ScanRange-COM1.ini";
		}
		else
		{

		}
		

		//判断是不是终测，终测直接调用文件
		//if (m_cbTestItem.GetCurSel()==0)  //终测
		{
			LogInfo("开始调用初测电压数据！");
			for(nChannel=0;nChannel<m_nCHCount;nChannel++ )
			{
				LogInfo("开始调用电压数据！");
				if(!GetVolDataFormFile(nChannel))
				{
					LogInfo("找不到可调用的电压文件！",FALSE,COLOR_RED);
					return FALSE;
				}
			}		
		}
		for(nChannel=0;nChannel<m_nCHCount;nChannel++ )
		{
			//细扫		
			if(!AutoPointScan(nChannel))
			{
				strMsg.Format("通道%d扫描出错！",nChannel+1);
				MessageBox(strMsg);	
				return FALSE;
			}
			//保存文件
			CString strCSVFile;
			CStdioFile stdCSVFile;
			CString strNetFile;
			CString strContent;
			HANDLE  hHandle = INVALID_HANDLE_VALUE;
//			WIN32_FIND_DATA win32Find;
			int    iSel=IDYES;

			if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nChannel+1);
				strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nChannel+1);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN,nChannel+1);
				strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nChannel+1);
			}
			
//			hHandle = FindFirstFile(strCSVFile,&win32Find);
//			if (hHandle!=INVALID_HANDLE_VALUE)
//			{
//				strMsg.Format("文件%s已经存在，是否覆盖数据？",strCSVFile);
//				iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);		
//				if (iSel==IDNO)
//				{
//					return FALSE;
//				}
// 			}

			if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
			{
				MessageBox("创建数据文件失败！");
				return FALSE;
			}
			stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");
		//	for (int i=0;i<m_nCHCount;i++)
			{
				strContent.Format("%d,%d,%d,%.2f\n",nChannel+1,ScanPowerChannel[nChannel].VoltageX,ScanPowerChannel[nChannel].VoltageY,ScanPowerChannel[nChannel].MaxPower);
				stdCSVFile.WriteString(strContent);
			}
			stdCSVFile.Close();

			if (m_strItemName=="终测") //终测需判断与初测IL数据的差异
			{
				CString strFile;
				CString strValue;
				double  dbMaxValue=0;
	//			if (m_bIfSaveToServe)
	//			{
	//				strFile.Format("%s\\config\\%s-%s\\ParaDemand.ini",m_strNetConfigPath,m_strSpec,m_strPN);
	//			}
	//			else
	//			{
					strFile.Format("%s\\%s-%s\\ParaDemand.ini", m_strPNConfigPath,m_strSpec,m_strPN);
	//			}
				GetPrivateProfileString("DIFF_IL","Value","error",strValue.GetBuffer(128),128,strFile);
				if (strcmp(strValue,"error")==0)
				{
					strMsg.Format("获取配置%s失败！(DIFF_IL)",strFile);
					LogInfo(strMsg,FALSE,COLOR_RED);
					return FALSE;
				}
                dbMaxValue = atof(strValue);
				if (fabs(m_dbInitialIL[nChannel]-ScanPowerChannel[nChannel].MaxPower)>dbMaxValue)
				{
					strMsg.Format("通道%d的IL与初测IL差异超过了%.2f dB,是否继续?",nChannel+1,dbMaxValue);
					//LogInfo(strMsg);
					int iSelect = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);
					if (iSelect==IDNO)
					{
						return FALSE;
					}
				} 
			}
			//ADDED BY RIS，数据保存到网络
			if (m_bIfSaveToServe)
			{
				CopyFile(strCSVFile,strNetFile,FALSE);
			}
			//保存电压数据到EXCEL
			SaveDataToExcel(VOL_DATA,nChannel);
		}
		
								
	}
	catch(TCHAR* ptszErrorMsg)
	{
		MessageBox(ptszErrorMsg);
		return FALSE;
	}
	catch(...)
	{
		LogInfo("其他错误！扫描失败");
		return FALSE;
	}

	LogInfo("保存扫描信息完毕！",TRUE);  
	return TRUE;

}

void CMy126S_45V_Switch_AppDlg::GetChannelInfo()
{
	int nAddress = (0x6E00 + 120);
	int i,j, nIndex;

	char buf[128];
	SHORT sChannelInfo[32][2];
	char* pch = (char*)&sChannelInfo[0][0];
	DWORD dwLength;

	memset(pch,0, 128);
	nIndex = 0;


	for(i=0; i<4; i++)
	{
		sprintf(buf, "MEM %d\r", nAddress);
		nAddress += 32;

			
		if (!m_opCom.WriteBuffer(buf,strlen(buf)))
		{
			LogInfo("串口通讯错误！",(BOOL)FALSE,COLOR_RED);
			return;
		}
		Sleep(200);	

		memset(buf, 0, 128);

		if (!m_opCom.ReadBuffer(buf,128, &dwLength))
		{
			LogInfo("读取数据错误！",(BOOL)FALSE,COLOR_RED);
			return;
		}

		if((buf[0] != 0x55) || (buf[1]!=(32+4)))
		{
			break;
		}

		
		for(j=5; j<dwLength; j++)
		{
			if(nIndex >= 128)
				break;
			if(buf[j]==0x56)
			{
				j++;
				pch[nIndex] = buf[j] - 1;
			}
			else
			{
				pch[nIndex] = buf[j];
			}

			nIndex++;
		}
	}


	pch = (char*)&sChannelInfo[0][0];
	for(i=0; i<64; i++)
	{
		pch[0] ^= pch[1];
		pch[1] ^= pch[0];
		pch[0] ^= pch[1];

		pch += 2;
	}

	Sleep(0);
	CString strMsg;
	for (int iCH = 0;iCH<32/*m_nCHCount*/;iCH++)
	{
		strMsg.Format("通道%d:%d,%d;", iCH+1,sChannelInfo[iCH][0]*60000/MAX_DAC, sChannelInfo[iCH][1]*60000/MAX_DAC);
		LogInfo(strMsg);
	}
	
}

BOOL CMy126S_45V_Switch_AppDlg::OneKeyWDLTest()
{
	double dblTLSPower=0.0;
	double dblStartWL=0;
	double dblENDWL=0;
	double dblPower=0.0;
	double step=0;
	int count;
    double dblWDLPower[1000];
    double dblMIN;
	double dblMAX;
	int  nChannel;
	CString strMsg; 
	double dbl;
	CString strZeroFile;
	CStdioFile stdZeroFile;
//	LPTSTR    lpStr;
//	char    ReadValue[256];
	CString  strToken;
    char     sep[] = ",\t\n";
    m_bTestStop = FALSE;

	CString strIniFile;
	CString strValue;
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
//	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;

    ZeroMemory(dblWDLPower,sizeof(dblWDLPower));

	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return FALSE;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先设置光功率计参数！");
		return FALSE;
	}
	//设置路径
	UpdateData();
    FindMyDirectory(m_strItemName,m_strPN,m_strSN);

	if (!m_bOpen8164)
	{
		MessageBox("请先打开设备！");
		return FALSE;
	}

	strIniFile.Format("%s\\%s-%s\\WDLSET.INI", m_strPNConfigPath,m_strSpec,m_strPN);
	DWORD nLen1 = GetPrivateProfileString("WDL","STARTWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen1 <= 0)
		MessageBox("读取配置文件错误！");
    dblStartWL = atof(strValue);
	
	DWORD nLen2 = GetPrivateProfileString("WDL","STOPWL",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen2 <= 0)
		MessageBox("读取配置文件错误！");
    dblENDWL = atof(strValue);
	
	DWORD nLen3 = GetPrivateProfileString("WDL","STEP",
		NULL,strValue.GetBuffer(128),128,strIniFile);
	if (nLen2 <= 0)
		MessageBox("读取配置文件错误！");
    step = atof(strValue);

	//初始化图形界面
	m_NTGraph.ClearGraph();
	m_NTGraph.SetXLabel("波长(nm)");
	m_NTGraph.SetYLabel("功率值(dBm)");
	m_NTGraph.SetRange(dblStartWL,dblENDWL,-2,2);
	m_NTGraph.SetXGridNumber(10);
	m_NTGraph.SetYGridNumber(8);

	m_bChange=TRUE;
	try
	{		
	    UpdateData();	

		for (nChannel=1;nChannel<=m_nCHCount;nChannel++)
		{
			if(!SWToChannel(nChannel))
			{
				MessageBox("开关切换失败！");
				return FALSE;
			}
			Sleep(300);
			strMsg.Format("开始通道%d的WDL测试！",nChannel);
			LogInfo(strMsg);		
			count = 0;
			for (dbl = dblStartWL; dbl <= dblENDWL; dbl += step)
			{
				YieldToPeers();
				if (m_bTestStop)
				{
					MessageBox("测试中止！");
					//设置回中心波长
					if(!SetPWMWL(m_pdblWavelength,nChannel-1))
					{
						LogInfo("设置波长失败！");
					}
	        		SetTLSWavelength(m_pdblWavelength);
					return FALSE;
				}
				SetTLSWavelength(dbl);
				//设置光功率计波长
				if(!SetPWMWL(dbl,nChannel-1))
				{
					MessageBox("设置光功率计波长失败！");
					//设置回中心波长
					if(!SetPWMWL(m_pdblWavelength,nChannel-1))
					{
						LogInfo("设置波长失败！");
					}
					SetTLSWavelength(m_pdblWavelength);
					return FALSE;
				}
				Sleep(300);
			//	for (nChannel=0;nChannel<4;nChannel++)
				{				
					dblPower = ReadPWMPower(nChannel-1);
					m_dblWDL[nChannel-1][count] = m_dblWDLRef[nChannel-1][count]-dblPower;
					strMsg.Format("通道%d:　波长:%.2f nm , IL:%.2f dB",nChannel,dbl,m_dblWDL[nChannel-1][count]);
					LogInfo(strMsg,TRUE);
					count++;
				}						
			} 
			//设置回1550nm波长
			if(!SetPWMWL(m_pdblWavelength,nChannel-1))
			{
				LogInfo("设置波长失败！");
			}
			SetTLSWavelength(m_pdblWavelength);
			
			//计算WDL	

			dblMIN = m_dblWDL[nChannel-1][0];
			dblMAX = m_dblWDL[nChannel-1][0];
			for(int i=1;i<(count-1);i++)
			{
				if (m_dblWDL[nChannel-1][i] <= dblMIN)
				{
					dblMIN = m_dblWDL[nChannel-1][i];
				}
				if (m_dblWDL[nChannel-1][i] >= dblMAX)
				{
					dblMAX = m_dblWDL[nChannel-1][i];
				}
			}
			ChannalMessage[nChannel-1].dblWDL[0] = dblMAX - dblMIN;
			strMsg.Format("通道%d的WDL为:%.2f dB",nChannel,ChannalMessage[nChannel-1].dblWDL[0]);
			if(!CheckParaPassOrFail(WDL_DATA,ChannalMessage[nChannel-1].dblWDL[0]))
			{
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			}
			else
			{
				LogInfo(strMsg);
			}	   			
			SaveDataToExcel(WDL_DATA,nChannel-1);
		}	
	    				
	}
	catch (...)
	{
		MessageBox("请保存文件", "Error", MB_OK|MB_ICONERROR);
	}
	return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonDarkTest() 
{
	// TODO: Add your control notification handler code here
	double dblPref;
	double dblPower;
	CString strTemp;
	BOOL bFlag = TRUE;
	CString strCSVFile;
//	HANDLE hHandle;
	CStdioFile stdCSVFile;
	CString strContent;
//	WIN32_FIND_DATA win32Find;
	
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}
	int nWL = m_cbSetWL.GetCurSel()-1;
	if (nWL>=0)
	{
		SetPWMWL(m_dblWL[nWL]);
		m_bChange=TRUE;
	}

	CString dwBaseAddress,strCommand;

		/*TxDataToI2C("00D6",(BYTE*)"MFGCMD",6);
		if (m_strErrorMsg != "")
		{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
		}

		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
		}*/
            
 
		Sleep(100);
		 if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return ;
		}

		if(!SetSWChannel(atoi(dwBaseAddress),0))
		{
			LogInfo("设置SWChannel失败!",FALSE,COLOR_RED);
			return ;
		}
        
		/*strCommand.Format("%s00",dwBaseAddress);
		
		TxSTRToI2C("0143",strCommand); //Set Single MemsSwitch Channel
        
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return ;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return ;
		}*/
		

	//获取当前时间
	SYSTEMTIME st;
	CString    strMsg;
	COleDateTime    tCurTime;
	COleDateTimeSpan tDifHour; 
	int        nDifHour;
	GetLocalTime(&st);
	tCurTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	tDifHour = tCurTime-m_RefStartTime; //计算时间插值
	nDifHour = tDifHour.GetTotalHours();
	if (nDifHour>m_nRefTime)  //比较归零时间差异
	{
		strMsg.Format("当前归零时间为%d小时，已超过归零限制时间（%d小时），请重新归零！",nDifHour,m_nRefTime);
		MessageBox(strMsg);
		m_bHasRefForTest = FALSE;
		return;
	}
	
	UpdateData();
	double dblTargetTempt;
	int    nTemptStatus; //设置温度状态
	double dblCurTempt; //当前温度
	int nTemp = m_ctrlComboxTemperature.GetCurSel();
	if (nTemp==0)
	{
		strTemp.Format("开始Dark IL常温测试！");
		LogInfo(strTemp,FALSE);
		dblTargetTempt = 25;
		nTemptStatus = 0;
	}
	else if (nTemp==1)
	{
		strTemp.Format("开始Dark IL低温测试！");
		LogInfo(strTemp);
		dblTargetTempt = -5;
		nTemptStatus = 2;
	}
	else 
	{
		strTemp.Format("开始Dark IL高温测试！");
		LogInfo(strTemp);
		dblTargetTempt = 70;
		if (stricmp(m_strSpec,"Z4767")==0)
		{
			dblTargetTempt = 85;
		}
		nTemptStatus = 1;
	}
	dblTargetTempt = m_dblTemperature[nTemp];
	
	//判断当前温度是否达到要求；
	if (m_bConnectTemptControl)
	{
		dblCurTempt = GetCurrentTempt(); //获取当前冷热盘温度
		if (fabs(dblTargetTempt-dblCurTempt)>2)
		{
			strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
			LogInfo(strMsg,BOOL(FALSE),COLOR_RED);
			SetTemptStatus(nTemptStatus);
			return;
		}
	}

	GetLocalTime(&m_StartTime);
	
	SETX(0);
	Sleep(100);
	SETY(0);
	Sleep(200);
	
	for (int nchannel = 0; nchannel < m_nCHCount; nchannel++)
	{	
		//调用归零数据
		if (nWL>=0)
		{
			dblPref = m_dblWDLRef[nchannel][nWL];
		}
		else
		{
			dblPref = m_dblReferencePower[nchannel];
		}
		
		dblPower = ReadPWMPower(nchannel);
		Sleep(200);
		ChannalMessage[nchannel].dblDarkIL= TestIL(dblPref,dblPower);
		
		if (!CheckParaPassOrFail(DARK_DATA,ChannalMessage[nchannel].dblDarkIL))
		{
			bFlag = FALSE;
		}
		
		strTemp.Format("CH%d,DARK IL:%.2f",nchannel+1,ChannalMessage[nchannel].dblDarkIL);
		
		if (!bFlag)
		{
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			LogInfo(strTemp);
		}	
		
	}

	/*
	//保存到CSV文件
	if (m_cbComPort.GetCurSel()==0)
	{		
		strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_Dark.csv",g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);	
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_Dark-COM1.csv",g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	hHandle = FindFirstFile(strCSVFile,&win32Find);
	if (hHandle!=INVALID_HANDLE_VALUE)
	{
		//		strMsg.Format("文件%s已经存在，是否覆盖数据？",strCSVFile);
		//		iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);		
		//		if (iSel==IDNO)
		//		{
		//			return;
		// 		}
	}
	
	if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建DARK数据文件失败！");
		LogInfo("创建DARK数据文件失败！",FALSE,COLOR_RED);
		return;
	}
	strContent.Format("Channel,Dark IL\n");
	stdCSVFile.WriteString(strContent);
	
	for (int i=0;i<m_nCHCount;i++)
	{
		strContent.Format("%d,%.2f\n",i+1,ChannalMessage[i].dblDarkIL);
		stdCSVFile.WriteString(strContent);
	}
	stdCSVFile.Close();
	*/
	
	//保存到EXCEL文件中
//	SaveDataToExcel(DARK_DATA);

	
	
		/* if (b_issn1) 
		{
			dwBaseAddress="00";
		}
		else if (b_issn2)
		{
			dwBaseAddress="01";
		}
		else if(b_issn3)
		{
			dwBaseAddress="02";
		}
		else
		{
			return ;
		}

		
        
		strCommand.Format("%s00",dwBaseAddress);
		
		TxSTRToI2C("0143",strCommand);
        
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return ;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return ;
		}*/
		
	SaveDataToCSV(DARK_DATA);
	//UpdateATMData(DARK_DATA);
	LogInfo("DARK值测试完毕!");
	
}

BOOL CMy126S_45V_Switch_AppDlg::SetWLFromOPCom(int iPMIndex, byte bSlot, byte bChan, long lWL)
{
	char pchCmd[256];
	char recvBuf[256];
	WORD wSendLen;
	long lTempWL;
	
	UpdateData();
	ZeroMemory(pchCmd,256);
	sprintf(pchCmd,"SENS%d,CHAN%d:POW:WAV %dnm\r",bSlot,bChan,lWL);
	wSendLen = strlen(pchCmd);
	
	if (!m_comPM[iPMIndex].WriteBuffer(pchCmd,wSendLen))
	{
		return FALSE;
	}
	Sleep(100);
	
	ZeroMemory(pchCmd,256);
	sprintf(pchCmd,"SENS%d,CHAN%d,POW:WAV?\r",bSlot,bChan);
	
	wSendLen = strlen(pchCmd);
	if (!m_comPM[iPMIndex].WriteBuffer(pchCmd,wSendLen))
	{
		return FALSE;
	}
	Sleep(100);
	
	ZeroMemory(recvBuf,256);
	if (!m_comPM[iPMIndex].ReadBuffer(recvBuf,256))
	{
		return FALSE;
	}
	Sleep(100);
	
	lTempWL = atol(recvBuf);
	lTempWL = (long)(lTempWL + 0.5);
	
	if (lTempWL != lWL)
	{
		return FALSE;
	}
	
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::GetPowerFromOPCom(int nPMIndex, BYTE bSlot, BYTE bChannel, double *pdblPower)
{
	char pchCmd[256];
	char recvBuf[256];
	WORD wSendLen;
	int  count = 0;
	CString strMsg;
	
	UpdateData();
	ZeroMemory(pchCmd, 256);
	sprintf(pchCmd, "SENS%d,CHAN%d:POW:READ?\r", bSlot, bChannel);
	wSendLen = strlen(pchCmd);
	if (!m_comPM[nPMIndex].WriteBuffer(pchCmd,wSendLen))
	{
		return FALSE;
	}
	Sleep(50);
	
	ZeroMemory(recvBuf, 256);
	if (!m_comPM[nPMIndex].ReadBuffer(recvBuf,256))
	{
		return FALSE;
	}
	*pdblPower = atof(recvBuf);
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::GetATMTestTemplate()
{
	CString strMsg;
	CString strURL;
	int     iRandNo=rand();
	DWORD   dwBuf[256];
	DWORD   dwTmptBuf[256];
	DWORD   dwWDMTemp;
	CString strErrMsg;
	int     nNodeCount;
	CString strTemperature;
	CString strTemp;
	CString strMax;
	CString strMin;
	CString strProcess;
	CString strLocalFile;

	if (m_strItemName=="终测")  //终测
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&test=1",m_strSN); //获取测试模板
	}
	else if (m_strItemName=="初测")//初测
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&pretest=1",m_strSN); //获取测试模板
	}
	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	//核对PN信息是否对应
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
 		return FALSE;
 	}
	dwWDMTemp = dwBuf[0];
	ZeroMemory(dwTmptBuf, 256 * sizeof(DWORD));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwTmptBuf, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	for (int nTempIndex = 0; nTempIndex < 3; nTempIndex++)
	{
		dwWDMTemp = dwTmptBuf[nTempIndex];
		m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
		if (strTemp=="Room") //常温
		{
			//电压
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Voltage", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxVoltage = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
			m_stLimitPara.dblMinVoltage = atof(strTemp);			
			//IL
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "IL", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxRoomIL = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinRoomIL = atof(strTemp);
			//CT
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "CT1", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxCT = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinCT = atof(strTemp);
			//RepeatIL
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "RepeatIL", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxRepeatIL = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinRepeatIL = atof(strTemp);
			//DarkIL
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "DarkIL", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxDarkIL = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinDarkIL = atof(strTemp);
			//SwitchTime
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SwitchTime", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxSwitchTime = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinSwitchTime = atof(strTemp);
			//TDL
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "TDL", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxTDL = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinTDL = atof(strTemp);
			//RL
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "RL", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxRL = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinRL = atof(strTemp);		
		}
		else if (strTemp=="Low") //低温
		{
			//IL
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "IL", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxLowIL = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinLowIL = atof(strTemp);
		}
		else if (strTemp=="High")//高温
		{
			//IL
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "IL", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxHighIL = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinHighIL = atof(strTemp);
		}	
	}

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::UpdateATMData(int nTestType,int nChannel)
{
	//上传数据
	CString strSaveData=_T(""),strSaveHead=_T("");
	CString strTemp=_T(""),strSN=_T("");
	CString	strErrMsg= _T("");
	CString strSNData= _T("");
	char    szComputerName[256];
    DWORD   dw=256;
	DWORD	dwBytesWrite = 0;
	DWORD   dwBytesReturned = 0;
	HANDLE  hTXTFile = INVALID_HANDLE_VALUE;
	SYSTEMTIME st;
	int     nCH = 0;
	int     nPort = 0;
	DWORD   dwBuf[256];
	DWORD   dwBufCH[256];
	DWORD   dwBufTemp[256];
	CString strMsg;
	int     nNodeCount=0;
	CString strXMLFile;
	DWORD   dwWDMTemp;
	CString strValue;
	int nTempIndex = 0;
	CString strTestItem;
	CString strCHSelect;
	CString  strLocalFile;
	CString  strURL;
	CString  strNode;
	CString  strItem;
	CString strName, strmsg;
	CString strErrorMsg;
	BOOL m_bfusion = TRUE;

	if(m_strPN=="2X16")
	{
		if (m_cbComPort.GetCurSel()==1)
		{
			strName="-COM1";
		}
		else if (m_cbComPort.GetCurSel()==0)
		{
			strName="-COM0";
		}
	}
	else
	{
		strName="";
	}

	GetLocalTime(&st);
	strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", m_strNetFile, m_strItemName, m_strPN, m_ModuleSN, m_ModuleSN);
	//strXMLFile.Format("%s\\Data\\%s\\%s\\%s_XML.xml", m_strNetFile, m_strItemName, m_strPN, m_ModuleSN);
	//strXMLFile = "\\\\zh-mfs-srv\\Public\\WLMTestProgram\\ZH_Ruijiang\\1X64Fusion\\data\\XML.xml";
	//strXMLFile.Format("%s\\TEST.xml", g_tszAppFolder);
	if (m_bfusion)
	{
		if (!m_fusion.bLoadXMLTemplet(strXMLFile, &strErrorMsg))
		{
			AfxMessageBox(strErrorMsg);
			return FALSE;
		}
	}
	else
	{
		m_ctrXMLCtr.LoadXmlFile(strXMLFile);
		//增加上传测试信息
		ZeroMemory(dwBuf, 256 * sizeof(DWORD));
		nNodeCount = 0;
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "USER", dwBuf, nNodeCount);
		if (nNodeCount == 0)
		{
			strMsg = "XML文件结点不完整,测试的用户名不符合!";
			LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
			return FALSE;
		}
		m_ctrXMLCtr.ModifyNode("NodeValue", m_strID, dwBuf[0]);

		//测试类型
		ZeroMemory(dwBuf, 256 * sizeof(DWORD));
		nNodeCount = 0;
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SOFTWARE", dwBuf, nNodeCount);
		if (nNodeCount == 0)
		{
			strMsg = "XML文件结点不完整,SOFTWARE不符合!";
			LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
			return FALSE;
		}
	}

	
	
	if (m_strItemName=="终测")  //终测
	{
		strTemp = "1830"; 
	}
	else if (m_strItemName=="初测")//初测
	{
		strTemp = "1830-P"; 
	}

	if (stricmp(m_strSpec,"Z4767")==0)
	{
		if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0)
		{
			if (m_strItemName=="初测")
			{
				strTemp = "1830";  //存入终测位置
			}
		}
	}

	
	if (stricmp(m_strSpec,"Z4839")==0)
	{
		
			if (m_strItemName=="初测")
			{
				strTemp = "1830";  //存入终测位置
			}
		
	}

	int nSNIndex=0;
	if (stricmp(m_strSN,m_strSN1)==0)
	{
		nSNIndex=0;
		m_XMLItem.strObjectKey = "MEMS SW1";
	}
	else if (stricmp(m_strSN,m_strSN3)==0)
	{
		nSNIndex=1;
		m_XMLItem.strObjectKey = "MEMS SW2";
	}
	else if (stricmp(m_strSN,m_strSN4)==0)
	{
		nSNIndex=2;
		m_XMLItem.strObjectKey = "MEMS SW3";
	}
	else if (stricmp(m_strSN,m_strSN5)==0)
	{
		nSNIndex=3;
		m_XMLItem.strObjectKey = "MEMS SW4";
	}
	else
	{
		strMsg = "器件SN不正确，请检查!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	
	/*
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);

	//增加测试开始时间和结束时间
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "START", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	strTemp.Format("%d-%02d-%02d %02d:%02d:%02d",m_StartTime.wYear,m_StartTime.wMonth,
		m_StartTime.wDay,m_StartTime.wHour,m_StartTime.wMinute,m_StartTime.wSecond);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "DATE", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	GetLocalTime(&st);
	CTime curTime = CTime::GetCurrentTime();
	strTemp.Format("%d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);//add end

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "COMPUTER", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的工位不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	GetComputerName(szComputerName,&dw);
	m_ctrXMLCtr.ModifyNode("NodeValue",szComputerName,dwBuf[0]);	
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
 		return FALSE;
 	}
	dwWDMTemp = dwBuf[0];

	ZeroMemory(dwBufTemp, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwBufTemp, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	*/
	int nWL = m_cbSetWL.GetCurSel()-1;
	 
	switch (nTestType)
	{
	case VOL_DATA:
		for (nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			if (nTempIndex == 0)
			{
				m_XMLItem.strTenvKey = "RT";
				strTemp = "Room";
			}
			else if (nTempIndex == 1)
			{
				m_XMLItem.strTenvKey = "LT";
				strTemp = "Low";
			}
			else if (nTempIndex == 2)
			{
				m_XMLItem.strTenvKey = "HT";
				strTemp = "High";
			}
			if (strTemp=="Room") //常温
			{
				CString strLutFile,strToken1;
				CStdioFile stdPDFile;
				char chBuffer[256];
				char   sep[] = ",\t\n";
				LPCTSTR  lpStr1;

				for (int ch=0;ch<18;ch++)
				{
				
					//strLutFile.Format("%s\\Data\\%s\\%s\\%s\\%s_ScanVoltage_%d-High.csv",
					//	strPath,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
					
					
					
				   strLutFile.Format("%s\\Data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",
					   m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);

					LogInfo(strLutFile);
					
				
					if(!stdPDFile.Open(strLutFile,CFile::modeRead|CFile::typeText))
					{
						strMsg.Format("打开文件%s失败！",strLutFile);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
						//return FALSE;
					}
					ZeroMemory(chBuffer,sizeof(char)*MAX_LINE);
					lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
					if (lpStr1 == NULL)
					{
						LogInfo("读取LUT表错误，数据缺失!",FALSE,COLOR_RED);
						stdPDFile.Close();
						return FALSE;
					}
					lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
					strToken1 = strtok(chBuffer,sep);
					strToken1 = strtok(NULL,sep);
					ScanPowerChannel[ch].VoltageX = atoi(strToken1);
					strToken1 = strtok(NULL,sep);
					ScanPowerChannel[ch].VoltageY = atoi(strToken1);
					stdPDFile.Close();
				}

				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "Volt_X";
					strmsg.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strmsg;
					strValue.Format("%d", ScanPowerChannel[nCH].VoltageX);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
									

					m_XMLItem.strParamName = "Volt_Y";
					strmsg.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strmsg;
					strValue.Format("%d", ScanPowerChannel[nCH].VoltageY);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}

					/*
					strItem.Format("VOLTAGE_X_DIFF");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						//LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						//return FALSE;
					}
					else
					{
						if(nCH < 17)
						{
							strValue.Format("%d",ScanPowerChannel[nCH+1].VoltageX - ScanPowerChannel[nCH].VoltageX);
						}
						else
						{
							strValue = "0";
						}
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
					
					strItem.Format("VOLTAGE_Y_DIFF");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						//LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						//return FALSE;
					}
					else
					{
						if(nCH < 17)
						{
							strValue.Format("%d",ScanPowerChannel[nCH+1].VoltageY - ScanPowerChannel[nCH].VoltageY);
						}
						else
						{
							strValue = "0";
						}
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
					*/
				}
			}
			if (strTemp=="Low") 
			{
				CString strLutFile,strToken1;
				CStdioFile stdPDFile;
				char chBuffer[256];
				char   sep[] = ",\t\n";
				LPCTSTR  lpStr1;
				int by1x18;
				if (m_strPN == "77501369A11")
				{
					by1x18 = 18;
				}
				else
				{
					by1x18 = 19;

				}
				int ch = 0;
				for (;ch<by1x18;ch++)
				{
				
					//strLutFile.Format("%s\\Data\\%s\\%s\\%s\\%s_ScanVoltage_%d-High.csv",
					//	strPath,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
					
					
					
				   strLutFile.Format("%s\\Data\\%s\\%s\\%s\\%s_ScanVoltage_%d-Low.csv",
					   m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
					
				
					if(!stdPDFile.Open(strLutFile,CFile::modeRead|CFile::typeText))
					{
						ScanPowerChannel[ch].VoltageX = 0;
						strMsg.Format("打开文件%s失败！",strLutFile);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
						//return FALSE;
					}
					ZeroMemory(chBuffer,sizeof(char)*MAX_LINE);
					lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
					if (lpStr1 == NULL)
					{
						LogInfo("读取LUT表错误，数据缺失!",FALSE,COLOR_RED);
						stdPDFile.Close();
						return FALSE;
					}
					lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
					strToken1 = strtok(chBuffer,sep);
					strToken1 = strtok(NULL,sep);
					ScanPowerChannel[ch].VoltageX = atoi(strToken1);
					strToken1 = strtok(NULL,sep);
					ScanPowerChannel[ch].VoltageY = atoi(strToken1);
					stdPDFile.Close();
				}


				for(int nCH = 0;nCH<m_nCHCount;nCH++)
				{

					if(ScanPowerChannel[ch].VoltageX == 0)
					{
						break;
					}

					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "Volt_X";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%d", ScanPowerChannel[nCH].VoltageX);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
									
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "Volt_Y";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%d", ScanPowerChannel[nCH].VoltageY);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}

					/*
					strItem.Format("VOLTAGE_X_DIFF");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						//LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						//return FALSE;
					}
					else
					{
						if(nCH < 17)
						{
							strValue.Format("%d",ScanPowerChannel[nCH+1].VoltageX - ScanPowerChannel[nCH].VoltageX);
						}
						else
						{
							strValue = "0";
						}
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
					
					strItem.Format("VOLTAGE_Y_DIFF");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						//LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						//return FALSE;
					}
					else
					{
						if(nCH < 17)
						{
							strValue.Format("%d",ScanPowerChannel[nCH+1].VoltageY - ScanPowerChannel[nCH].VoltageY);
						}
						else
						{
							strValue = "0";
						}
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
					*/
				}
			}
			if (strTemp=="High") //常温
			{

				CString strLutFile,strToken1;
				CStdioFile stdPDFile;
				char chBuffer[256];
				char   sep[] = ",\t\n";
				LPCTSTR  lpStr1;
				int by1x18;
				if (m_strPN == "77501369A11")
				{
					by1x18 = 18;
				}
				else
				{
					by1x18 = 19;

				}
				int ch = 0;
				for (;ch<by1x18;ch++)
				{
				
					strLutFile.Format("%s\\Data\\%s\\%s\\%s\\%s_ScanVoltage_%d-High.csv",
						m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,ch+1);
					
				
					
				
					if(!stdPDFile.Open(strLutFile,CFile::modeRead|CFile::typeText))
					{
						ScanPowerChannel[ch].VoltageX = 0;
						strMsg.Format("打开文件%s失败！",strLutFile);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
						//return FALSE;
					}
					ZeroMemory(chBuffer,sizeof(char)*MAX_LINE);
					lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
					if (lpStr1 == NULL)
					{
						LogInfo("读取LUT表错误，数据缺失!",FALSE,COLOR_RED);
						stdPDFile.Close();
						return FALSE;
					}
					lpStr1 = stdPDFile.ReadString(chBuffer,MAX_LINE);
					strToken1 = strtok(chBuffer,sep);
					strToken1 = strtok(NULL,sep);
					ScanPowerChannel[ch].VoltageX = atoi(strToken1);
					strToken1 = strtok(NULL,sep);
					ScanPowerChannel[ch].VoltageY = atoi(strToken1);
					stdPDFile.Close();
				}

				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					if(ScanPowerChannel[ch].VoltageX == 0)
					{
						break;
					}

					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "Volt_X";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%d", ScanPowerChannel[nCH].VoltageX);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}

					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "Volt_Y";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%d", ScanPowerChannel[nCH].VoltageY);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}

				
					/*
					strItem.Format("VOLTAGE_X_DIFF");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						//LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						//return FALSE;
					}
					else
					{
						if(nCH < 17)
						{
							strValue.Format("%d",ScanPowerChannel[nCH+1].VoltageX - ScanPowerChannel[nCH].VoltageX);
						}
						else
						{
							strValue = "0";
						}
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
					
					strItem.Format("VOLTAGE_Y_DIFF");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						//LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						//return FALSE;
					}
					else
					{
						if(nCH < 17)
						{
							strValue.Format("%d",ScanPowerChannel[nCH+1].VoltageY - ScanPowerChannel[nCH].VoltageY);
						}
						else
						{
							strValue = "0";
						}
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
					*/
				}
				
			}
		}
		break;
		//5dB Voltage
	case VOL_DATA_5:
			for (nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				strTemp="";
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNode.Format("Voltage_5dB");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
					for(nCH = 0;nCH<8;nCH++)
					{
//						if (nCH>=4)
//						{
//							nPort = nCH+4;
//						}
//						else
//						{
//							nPort = nCH;
//						}
						nPort = nCH;
						strItem.Format("CH%d_X",nPort+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%d",m_n5dBAttenX[nCH]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
										
						strItem.Format("CH%d_Y",nPort+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%d",m_n5dBAttenY[nCH]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
			}
		break;
		//8dB VoltageD
	case VOL_DATA_8:
			for (nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				strTemp="";
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNode.Format("Voltage_8dB");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
					for(nCH = 0;nCH<8;nCH++)
					{
// 						if (nCH>=4)
//						{
//							nPort = nCH+4;
//						}
//						else
//						{
//							nPort = nCH;
// 						}
						nPort = nCH;
						strItem.Format("CH%d_X",nPort+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%d",m_n8dBAttenX[nCH]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
										
						strItem.Format("CH%d_Y",nPort+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%d",m_n8dBAttenY[nCH]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
			}
		break;
		//5dB Repeat Atten
	case REPEAT_ATTEN_5:
			for (nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				strTemp="";
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNode.Format("VOAREP_5dB");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
					for(nCH = 0;nCH<8;nCH++)
					{
// 						if (nCH>=4)
//						{
//							nPort = nCH+4;
//						}
//						else
//						{
//							nPort = nCH;
// // 						}
						nPort = nCH;
						strItem.Format("CH%d",nPort+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%.2f",m_pdb5dBRepeatAtten[nCH]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
										
					}
				}
			}
		break;
			//5dB Repeat Atten
	case REPEAT_ATTEN_8:
			for (nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				strTemp="";
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNode.Format("VOAREP_8dB");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
					for(nCH = 0;nCH<8;nCH++)
					{
//						if (nCH>=4)
//						{
//							nPort = nCH+4;
//						}
//						else
//						{
//							nPort = nCH;
//						}
						nPort = nCH;
						strItem.Format("CH%d",nPort+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%.2f",m_pdb8dBRepeatAtten[nCH]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
										
					}
				}
			}
		break;
	case IL_ROOM_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			if (nTempIndex == 0)
			{
				m_XMLItem.strTenvKey = "RT";
				strTemp = "Room";
			}
			else if (nTempIndex == 1)
			{
				m_XMLItem.strTenvKey = "LT";
				strTemp = "Low";
			}
			else if (nTempIndex == 2)
			{
				m_XMLItem.strTenvKey = "HT";
				strTemp = "High";
			}
		
			if (strTemp=="Room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				
				if ((m_strPN=="18X1")||(m_strPN=="1X12-2"))
				{
					if(nWL<0)
					{
						strNode.Format("IL-%.0f",m_pdblWavelength);
					}
					else
					{
						strNode.Format("IL-%.0f",m_dblWL[nWL]);
					}
				}
				else
				{
					strNode.Format("IL%s",strName);
				}
				if (m_strSpec=="13998") 
				{
					strNode.Format("IL%s",strName);
				}
				
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "IL";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%.2f", ChannalMessage[nCH].dblIL);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
					

				}
			}
		}
		break;
	case IL_HIGH_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			if (nTempIndex == 0)
			{
				m_XMLItem.strTenvKey = "RT";
				strTemp = "Room";
			}
			else if (nTempIndex == 1)
			{
				m_XMLItem.strTenvKey = "LT";
				strTemp = "Low";
			}
			else if (nTempIndex == 2)
			{
				m_XMLItem.strTenvKey = "HT";
				strTemp = "High";
			}
			if (strTemp=="High") //常温
			{
				//ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				//strNode.Format("IL%s",strName);
				//m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "IL";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%.2f", ChannalMessage[nCH].dblHighTempIL);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
				}
			}
		}
		break;
	case IL_LOW_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			if (nTempIndex == 0)
			{
				m_XMLItem.strTenvKey = "RT";
				strTemp = "Room";
			}
			else if (nTempIndex == 1)
			{
				m_XMLItem.strTenvKey = "LT";
				strTemp = "Low";
			}
			else if (nTempIndex == 2)
			{
				m_XMLItem.strTenvKey = "HT";
				strTemp = "High";
			}
			if (strTemp=="Low") //常温
			{
				//ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				//strNode.Format("IL%s",strName);
				//m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "IL";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%.2f", ChannalMessage[nCH].dblLowTempIL);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
					
				}
			}
		}
		break;
	case WDL_DATA_5: //5dB WDL
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				strNode.Format("WDL_5dB");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				if (nChannel!=32)
				{
					if (nChannel>=8)
					{
						nCH = nChannel-4;
					}
					else
					{
						nCH = nChannel;
					}
					strItem.Format("CH%d",nChannel+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%.2f",m_pdb5dBWDL[nCH]);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
				}
				else
				{
					for(nCH = 0;nCH<8;nCH++)
					{
//						if (nCH>=4)
//						{
//							nPort = nCH+4;
//						}
//						else
//						{
//							nPort = nCH;
//						}
						nPort = nCH;
						strItem.Format("CH%d",nPort+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%.2f",m_pdb5dBWDL[nCH]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
				
			}
		}
		break;
    case WDL_DATA_8: //8dB WDL
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				strNode.Format("WDL_8dB");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				if (nChannel!=32)
				{
					if (nChannel>=8)
					{
						nCH = nChannel-4;
					}
					else
					{
						nCH = nChannel;
					}
					strItem.Format("CH%d",nChannel+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%.2f",m_pdb8dBWDL[nCH]);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
				}
				else
				{
					for(nCH = 0;nCH<8;nCH++)
					{
//						if (nCH>=4)
//						{
//							nPort = nCH+4;
//						}
//						else
//						{
//							nPort = nCH;
//						}
						nPort = nCH;
						strItem.Format("CH%d",nPort+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%.2f",m_pdb8dBWDL[nCH]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
				
			}
		}
		break;
	case TDL_DATA:
		m_XMLItem.strTenvKey = "AT";
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			//dwWDMTemp = dwBufTemp[nTempIndex]; 
			//m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (nTempIndex == 0) //常温
			{
				
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "TDL";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%.2f", ChannalMessage[nCH].dblTDL); //ChannalMessage[nCH].dblTDL
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
					
					//if(ChannalMessage[nCH].dblTDL > -98)
					//	m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
				}
			}
		}
		break;
	case PDL_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="Room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				strNode.Format("PDL%s",strName);
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					strItem.Format("CH%d",nCH+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%.2f",ChannalMessage[nCH].dblPDL);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
				}
			}
		}
		break;
	case CT_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{

			if (nTempIndex == 0)
			{
				m_XMLItem.strTenvKey = "RT";
				strTemp = "Room";
			}
			else if (nTempIndex == 1)
			{
				m_XMLItem.strTenvKey = "LT";
				strTemp = "Low";
			}
			else if (nTempIndex == 2)
			{
				m_XMLItem.strTenvKey = "HT";
				strTemp = "High";
			}
			if (strTemp=="Room") //常温
			{
				for (nCH = 0;nCH<m_nCHCount;nCH++)
				{	
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "MinCT";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%.2f", ChannalMessage[nCH].dblCT[31]);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
				//	strValue.Format("%.2f",ChannalMessage[nCH].dblCT[31]);
				//	m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);

					/*ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					if ((m_strPN=="18X1")||(m_strPN=="1X12-2"))
					{
						if(nWL<0)
						{
							strNode.Format("CT%d-%.0f",nCH+1,m_pdblWavelength);
						}
						else
						{
							strNode.Format("CT%d-%.0f",nCH+1,m_dblWL[nWL]);
						}
					}
					else
					{
						strNode.Format("CT%d%s",nCH+1,strName);
					}

					if (m_strSpec=="13998") 
					{
						strNode.Format("CT%d%s",nCH+1,strName);
					}
			    	//m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
					for(int nPort = 0;nPort<m_nCHCount;nPort++)
					{
						if (nPort == nCH)
						{
							continue;
						}
						//strItem.Format("CH%d",nPort+1);
						ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
						strNode.Format("SWITCH_OPTICAL-PORT%d_WL1640",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);

						strItem.Format("MIN_CT");

						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%.2f",ChannalMessage[nCH].dblCT[nPort]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}*/
				}			
			}
		}		
		break;
	case DIRECTIVITY_DATA: //只保存最小值
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="room") //常温
			{			
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				strNode.Format("DIR%s",strName);

				if (m_strSpec=="13998") 
				{
					strNode.Format("DIR%s",strName);
				}
			    m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
			
				double dblMinValue = ChannalMessage[nChannel].dblMinDIR;
				strItem.Format("CH%d",nChannel+1);
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
				if (nNodeCount == 0)
				{
					strMsg.Format("%s-%s结点不存在",strNode,strItem);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				strValue.Format("%.2f",dblMinValue);
				m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
				
				
			}
		}		
		break;
	case RE_IL_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			if (nTempIndex == 0)
			{
				m_XMLItem.strTenvKey = "RT";
				strTemp = "Room";
			}
			else if (nTempIndex == 1)
			{
				m_XMLItem.strTenvKey = "LT";
				strTemp = "Low";
			}
			else if (nTempIndex == 2)
			{
				m_XMLItem.strTenvKey = "HT";
				strTemp = "High";
			}
			if (strTemp=="Room") //常温
			{
				//ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				if ((m_strPN=="18X1")||(m_strPN=="1X12-2"))
				{
				//	if(nWL<0)
				//		strNode.Format("RepeatIL-%.0f",m_pdblWavelength);
				//	else
				//		strNode.Format("RepeatIL-%.0f",m_dblWL[nWL]);
				}
				else
				{
				//	strNode.Format("RepeatIL%s",strName);
				}

				if (m_strSpec=="13998") 
				{
				//	strNode.Format("RepeatIL%s",strName);
				}
				//m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "Repeatability";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%.2f", ChannalMessage[nCH].dblDif);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
					//strValue.Format("%.2f",ChannalMessage[nCH].dblDif);
					//m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
				}
			}
		}
		break;
	case DARK_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="Room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				if ((m_strPN=="18X1")||(m_strPN=="1X12-2"))
				{
					if(nWL<0)
						strNode.Format("DarkIL-%.0f",m_pdblWavelength);
					else
						strNode.Format("DarkIL-%.0f",m_dblWL[nWL]);
				}
				else
				{
					strNode.Format("DarkIL%s",strName);
				}

				if (m_strSpec=="13998") 
				{
					strNode.Format("DarkIL%s",strName);
				}
				//m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNode.Format("SWITCH_OPTICAL-PORT%d_WL1640",nCH+1 + 18*nSNIndex);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);

					strItem.Format("DARK_IL");
					
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%.2f",ChannalMessage[nCH].dblDarkIL);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
				}
			}
		}
		break;

	case RL_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			if (nTempIndex == 0)
			{
				m_XMLItem.strTenvKey = "RT";
				strTemp = "Room";
			}
			else if (nTempIndex == 1)
			{
				m_XMLItem.strTenvKey = "LT";
				strTemp = "Low";
			}
			else if (nTempIndex == 2)
			{
				m_XMLItem.strTenvKey = "HT";
				strTemp = "High";
			}
			if (strTemp=="Room") //常温
			{
				//ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				if ((m_strPN=="18X1")||(m_strPN=="1X12-2"))
				{
					//if(nWL<0)
					//	strNode.Format("RL-%.0f",m_pdblWavelength);
					//else
					//	strNode.Format("RL-%.0f",m_dblWL[nWL]);
				}
				else
				{
				//	strNode.Format("RL%s",strName);
				}

				if (m_strSpec=="13998") 
				{
				//	strNode.Format("RL%s",strName);
				}

				//m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					m_XMLItem.strcondKey = "C000001";
					m_XMLItem.strParamName = "RL";
					strValue.Format("1-%d", nCH + 1);
					m_XMLItem.strPortKey = strValue;
					strValue.Format("%.2f", ChannalMessage[nCH].dblRL);
					m_XMLItem.strParamValue = strValue;

					if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
					{
						AfxMessageBox(strErrorMsg);
						return FALSE;
					}
				//	strValue.Format("%.2f",ChannalMessage[nCH].dblRL);
				//	m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
				}
			}
		}
		break;
	case ISO_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				if ((m_strPN=="18X1")||(m_strPN=="1X12-2"))
				{
					if(nWL<0)
					{
						strNode.Format("ISO-%.0f",m_pdblWavelength);
					}
					else
					{
						strNode.Format("ISO-%.0f",m_dblWL[nWL]);
					}
					
				}
				else
				{
					strNode.Format("ISO%s",strName);
				}

				if (m_strSpec=="13998") 
				{
					strNode.Format("ISO%s",strName);
				}
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(nCH = 0;nCH<m_nCHCount;nCH++)
				{
					strItem.Format("CH%d",nCH+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%.2f",ChannalMessage[nCH].dblISO);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
				}
			}
		}
		break;
	case SWITCH_TIME_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="Room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				strNode.Format("SwitchTime%s",strName);
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);	
				if (nNodeCount == 0)
				{
					strMsg.Format("%s-%s结点不存在",strNode,strItem);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				strItem.Format("Switch_Time");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
				strValue.Format("%.2f",m_dblSwitchTime);
				m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
				
			}
		}
		break;
	case WDL_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="Room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				
				strNode.Format("WDL-1260");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);	
				if(nChannel<32)
				{
					strItem.Format("CH%d",nChannel+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%.2f",ChannalMessage[nChannel].dblWDL[0]);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
				}
				else
				{
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strItem.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%.2f",ChannalMessage[nCH].dblWDL[0]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
					}
				}
				
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				strNode.Format("WDL-1580");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);		
				if(nChannel<32)
				{
					strItem.Format("CH%d",nChannel+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%.2f",ChannalMessage[nChannel].dblWDL[1]);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
				}
				else
				{
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strItem.Format("CH%d",nCH+1);
						
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%.2f",ChannalMessage[nCH].dblWDL[1]);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
					}
				}
				

				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				strNode.Format("WDL");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);	
				if(nChannel<32)
				{
					strItem.Format("CH%d",nChannel+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%.2f",ChannalMessage[nChannel].dblTotalWDL);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
				}
				else
				{
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strItem.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
						if (nNodeCount == 0)
						{
							strMsg.Format("%s-%s结点不存在",strNode,strItem);
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							return FALSE;
						}
						strValue.Format("%.2f",ChannalMessage[nCH].dblTotalWDL);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
					}
				}
			}
		}
		break;
	case SELECT_CHANNEL_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="Room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				if ((m_strPN=="18X1")||(m_strPN=="1X12-2"))
				{
					strNode.Format("CHSelect-%.0f",m_dblWL[nWL]);
				}
				else
				{
					strNode.Format("CHSelect%s",strName);
				}

				//if (m_strSpec=="13998") 
				//{
					strNode ="CHSelect";
				//}
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(nCH = 1;nCH<3;nCH++)
				{
					strItem.Format("SW%d",nCH+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}
					strValue.Format("%d",m_startchange1);
					if (nCH==2)
					{
						strValue.Format("%d",m_startchange2);
					}
					if (m_bischange && b_issn1) 
					{
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
					}
					else if (!b_issn1 && m_bischange)
					{
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
					}
					else 
					{
						strValue = "0";
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
					
				
				}
			}
		}
		break;
	case FQC_DATA:
		for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			m_XMLItem.strTenvKey = "RT";
			m_XMLItem.strObjectKey = "RESULT";
			m_XMLItem.strPortKey = "NONE";
			m_XMLItem.strcondKey = "C000000";
			m_XMLItem.strParamName = "FQC";
			m_XMLItem.strParamValue = "99";
			strTemp = "Room";
			if (strTemp=="Room") //常温
			{
				
				if (nChannel == 0)
				{
					m_XMLItem.strParamValue = "0";
				}
				else
				{
					m_XMLItem.strParamValue = "1";
				}

				if (!m_fusion.bUpdateXMLTemplet(m_XMLItem, &strErrorMsg))
				{
					AfxMessageBox(strErrorMsg);
					return FALSE;
				}
			}
		}
		break;
	}
	//if (!m_ctrXMLCtr.CloseXMLFile())
	//{
	//	strMsg = "XML关闭失败,请通知相关工程师!";
	//	LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
	//	return FALSE;
	//}
	m_fusion.atmsTest.SetMiscSoftwareInfo("SOFTWARE3352_MSW_1X64_FTS", "V1.0.0.0", "12/20/2021", "Ruijiang Nie");

	m_fusion.atmsTest.SetMfgRecordByKey("Test_Station", strStationID);

	m_fusion.atmsTest.SetMfgRecordByKey("Operator", m_strID.GetBuffer());

	m_fusion.atmsTest.SetMfgRecordByKey("Test_Type", TTR_TESTTYPE_TESTDATA);

	m_fusion.atmsTest.SetMfgRecordByKey("Perms_Level", TTR_TESTMODE_MFG);
	Sleep(20);


	if (!m_fusion.bSaveXMLTemplet(strXMLFile, &strErrorMsg))
	{
		AfxMessageBox(strErrorMsg);
		return FALSE;
	}
	if (m_bIfSaveToServe)
	{
		CopyFile(strXMLFile,strLocalFile,FALSE);  //复制到本地
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::SetFileByMSMQ(CString strData, CString &strErr)
{
	CStdioFile file(strData, CFile::modeRead|CFile::typeText);
	CString strRead, strSend;
	strSend.Empty();
	do
	{
		file.ReadString(strRead);
		DWORD	dwCodeLen = WideCharToMultiByte(CP_UTF8, NULL, strRead.AllocSysString(), -1, NULL, NULL, 0, FALSE);
		TCHAR	*ptStr = new TCHAR[dwCodeLen];
		WideCharToMultiByte(CP_UTF8, NULL, strRead.AllocSysString(), -1, ptStr, dwCodeLen, NULL, FALSE);
		strSend += ptStr;
		delete [] ptStr;
	} while(!strRead.IsEmpty());
	
	if (!UploadXMLMessage((LPSTR)(LPCTSTR)strSend, (LPSTR)(LPCTSTR)strData))
		return FALSE;
	else
		return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::GetVoltageDataFromATMS()
{
	CString strMsg;
	CString strURL;
	CString strLocalFile;
	int     iRandNo=rand();
	DWORD   dwBuf[256];
	DWORD   dwTmptBuf[256];
	DWORD   dwBufCH[256];
	DWORD   dwWDMTemp;
	DWORD   dwWDMTempItem;
	CString strErrMsg;
	int     nNodeCount;
	CString strTemperature;
	CString strTemp;
	CString strMax;
	CString strMin;
	CString strKey;
	CString strProcess;
	
	if (m_strItemName=="终测") //终测
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&randno=%d&showdata=1&Pretest=1",m_strSN,iRandNo);
	}
	else if(m_strItemName=="初测")
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&randno=%d&showdata=1&adjust=1",m_strSN,iRandNo);
	}
	strLocalFile.Format("%s\\template\\Test_temp.xml", g_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	//核对PN信息是否对应
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
 		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
 		return FALSE;
 	}
	dwWDMTemp = dwBuf[0];
	ZeroMemory(dwTmptBuf, 256 * sizeof(DWORD));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwTmptBuf, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	dwWDMTemp = dwTmptBuf[0];
	m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值

	//根据PN判断通道组合
	if (strTemp=="room") //常温
	{
		//电压
		ZeroMemory(dwBufCH,sizeof(dwBufCH));
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Voltage", dwBufCH, nNodeCount,dwWDMTemp);
		dwWDMTempItem = dwBufCH[0];

		for (int nPort=0;nPort<m_nCHCount;nPort++)
		{
			strTemp="";
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			strKey.Format("CH%d_X",nPort+1);

			if (m_strItemName=="初测") //2016-6-2
			{
				if (stricmp(m_strSpec,"Z4767")==0)
				{
					if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //初测选取1~4,9~12通道数据
					{
						if (m_cbTestCH.GetCurSel()==1) //9~12
						{
							if (nPort>=4)
							{
								strKey.Format("CH%d_X",nPort+5);
							}
						}
						
					}
				}
				
			}
	
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
			m_Xbase[nPort] = atoi(strTemp);
			
			strKey.Format("CH%d_Y",nPort+1);
			if (m_strItemName=="初测") //2016-6-2
			{
				if (stricmp(m_strSpec,"Z4767")==0)
				{
					if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //初测选取1~4,9~12通道数据
					{
						if (m_cbTestCH.GetCurSel()==1) //9~12
						{
							if (nPort>=4)
							{
								strKey.Format("CH%d_Y",nPort+5);
							}
						}
						
					}
				}
				
			}
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwWDMTempItem);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
	    	m_Ybase[nPort] = atoi(strTemp);
		}	
		//差损
		ZeroMemory(dwBufCH,sizeof(dwBufCH));
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "IL", dwBufCH, nNodeCount,dwWDMTemp);
		dwWDMTempItem = dwBufCH[0];

		for (int nPort=0;nPort<m_nCHCount;nPort++)
		{
			strTemp="";
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			strKey.Format("CH%d",nPort+1);
			if (m_strItemName=="初测") //2016-6-2
			{
				if (stricmp(m_strSpec,"Z4767")==0)
				{
					if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //初测选取1~4,9~12通道数据
					{
						if (m_cbTestCH.GetCurSel()==1) //9~12
						{
							if (nPort>=4)
							{
								strKey.Format("CH%d",nPort+5);
							}
						}
					}
				}
				
			}
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
			m_dbInitialIL[nPort] = atof(strTemp);
		}	

	}

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "XML关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::AutoScanChannel(int nChannel)
{
	int x;
	int y;
    int x_step ;
	int y_step ;
	int count = 0;
	int x_count = 0;
	int nCount = 0;
	int iCount = 0;
	double  dblPower = 0; 
	BOOL    bFlag1 = FALSE;
	BOOL    bFlag2 = FALSE;
	BOOL    bFlag3 = FALSE;
	BOOL    bFlag4 = FALSE;
	CString strMsg;
	int x_start = 0;
	int x_stop = 0;
	int y_start = 0;
	int y_stop = 0;
	double dblPrePower = 100;
	double dblPrePowerX = 100;
	double dblPrePowerY = 100;
    char    chValue[20];
	CString m_cmd,dwBaseAddress,strdacx,strdacy;
	m_bTestStop = FALSE;
	ZeroMemory(chValue,sizeof(char)*20);

	CString strINIFile;
	CString strValue;
	CString strStartX;
	CString strEndX;
	CString strStartY;
	CString strEndY;
	BOOL    bFindPoint = FALSE;
	m_bTestStop = FALSE;
	CString strSection;
	CString strKey;
	double dblSetIL=0;
//	if (m_bIfSaveToServe)
//	{
//		strINIFile.Format("%s\\config\\%s-%s\\ScanRange.ini",m_strNetConfigPath,m_strSpec,m_strPN);
//	}
//	else
//	{
		strINIFile.Format("%s\\%s-%s\\ScanRange.ini", m_strPNConfigPath,m_strSpec,m_strPN);
//	}

	
	strKey.Format("CH%d",nChannel+1);

	DWORD nLen1 = GetPrivateProfileString(strKey,"x_start",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置信息失败！");
	x_start = atoi(strValue);

	DWORD nLen2 = GetPrivateProfileString(strKey,"x_end",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen2 <= 0)
		MessageBox("读取配置信息失败！");
	x_stop = atoi(strValue);

	DWORD nLen3 = GetPrivateProfileString(strKey,"y_start",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen3 <= 0)
		MessageBox("读取配置信息失败！");
	y_start = atoi(strValue);
	
	DWORD nLen4 = GetPrivateProfileString(strKey,"y_end",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen4 <= 0)
		MessageBox("读取配置信息失败！");
	y_stop = atoi(strValue);

	DWORD nLen5 = GetPrivateProfileString("step","x_step",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen5 <= 0)
		MessageBox("读取配置信息失败！");
	x_step = atoi(strValue);
	
	DWORD nLen6 = GetPrivateProfileString("step","y_step",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen6 <= 0)
		MessageBox("读取配置信息失败！");
	y_step = atoi(strValue);

	nLen6 = GetPrivateProfileString("MIN_IL","IL",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen6 <= 0)
		dblSetIL=0.8;
	dblSetIL = atof(strValue);


	count = 0;
	int XCount=1;
	int YCount=1;
	int nFlag = 1;
	int n=0;
	int nXflag=0;
	int nYflag=0;
	BOOL bCenterPoint1 = FALSE;
	BOOL bCenterPoint2 = FALSE;

	CString strVolt;
	x = x_start;
	y = y_start;

	//导入扫描配置
	CString strINI;
//	CString strValue;
	int     Point1;
	int     Point2;
	int     Point3;
//	if (m_bIfSaveToServe)
//	{
//		strINI.Format("%s\\config\\%s-%s\\ScanConfig.ini",m_strNetConfigPath,m_strSpec,m_strPN);
//	}
//	else
//	{
		strINI.Format("%s\\%s-%s\\ScanConfig.ini", m_strPNConfigPath,m_strSpec,m_strPN);
//	}
	
	GetPrivateProfileString("RangeScan","Point1","error",strValue.GetBuffer(128),128,strINI);
	if (strValue == "error")
	{
		LogInfo("读取配置ScanConfig错误！");
		return FALSE;
	}
	Point1 = atoi(strValue);
	GetPrivateProfileString("RangeScan","Point2","error",strValue.GetBuffer(128),128,strINI);
	if (strValue == "error")
	{
		LogInfo("读取配置ScanConfig错误！");
		return FALSE;
	}
	Point2 = atoi(strValue);
	GetPrivateProfileString("RangeScan","Point3","error",strValue.GetBuffer(128),128,strINI);
	if (strValue == "error")
	{
		LogInfo("读取配置ScanConfig错误！");
		return FALSE;
	}
	Point3 = atoi(strValue);

	while (1)
	{
		YieldToPeers();
		if (m_bTestStop)
		{
			return FALSE;
		}
			
//		if(!SETY(y))
//		{
//			LogInfo("设置Y电压错误！",(BOOL)FALSE,COLOR_RED);
//			return FALSE;
//		}
//		Sleep(100);
//		if(!SETX(x))
//		{
//			LogInfo("设置X电压错误！",(BOOL)FALSE,COLOR_RED);
//			return FALSE;
//		}
//		Sleep(400);

		if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}

		int XADC,YADC;
		
     
		XADC=(x*MAX_DAC/60000.0);
		YADC=(y*MAX_DAC/60000.0);
		
		if(XADC < 0)
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
		
		}
		if(YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);
		}
		if(XADC>0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}

		/*m_cmd.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",m_cmd); //Set Single MemsSwitch DAC

		
		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/

		Sleep(400);

		m_NTGraph.PlotXY(x,y,0);
		
		dblPower =  m_dblReferencePower[nChannel]-ReadPWMPower(nChannel);
		if (dblPower<dblPrePower)
		{
			dblPrePower = dblPower;
			m_Xbase[nChannel] = x;
			m_Ybase[nChannel] = y;
		}
		strMsg.Format("X:%d mv,Y:%d mv,IL:%.2f dB",x,y,dblPower);
		LogInfo(strMsg);

		if (bCenterPoint1==FALSE&&dblPrePower<Point1)  //小于45dB时，重新取中心点开始扫描
		{
			count = 0;
			XCount=1;
			YCount=1;
		    nFlag = 1;
			n=0;
			nXflag=0;
        	nYflag=0;
			bCenterPoint1 = TRUE;
		}
		if (bCenterPoint2==FALSE&&dblPrePower<Point2)  //小于30dB时，重新取中心点开始扫描
		{
			count = 0;
			XCount=1;
			YCount=1;
			nFlag = 1;
			n=0;
			nXflag=0;
			nYflag=0;
			bCenterPoint2 = TRUE;
		}
		if (dblPrePower < Point3)  //跳出条件
		{
			strMsg.Format("通道%d已经扫描到！",nChannel+1);
			LogInfo(strMsg);
			return	TRUE;
		}
		count++;
//		if (count==nFlag) //换step符号
//		{
//			n++;
//			nFlag = nFlag+2*n;
//			x_step = -x_step;
//			y_step = -y_step;
//			XCount++;
//			YCount++;
//			nXflag=0;
//			nYflag=0;
//		}



		
	//	y_step = -y_step;
	//	if (nYflag<YCount)
	//	{
		    if (x>0)
			{
			     x = x + x_step;
			}
			if (x<0)
			{
			     x = x - x_step;
			}
			 if (y>0)
			{
			     y = y + y_step;
			}
			if (x<0)
			{
			     y = y - y_step;
			}
		
	//		nYflag++;
	//	}
	//	else if (nXflag<XCount)
	//	{
		
	//		nXflag++;
	//	}
	//	else
	//	{
			//reserved
	//	}
		
//		if (x<x_start)
//		{
//			x = x_start;
//		}
//		if (y<y_start)
//		{
//			y = y_start;
//		}
//		if(x>x_stop)
//		{
//			x = x_stop;
//		}
//		if (y>y_stop)
//		{
//			y=y_stop;
//		}
		
		if(count>=400)
		{
			break;
		}			
	}
	
	if(dblPrePower<=dblSetIL)
	{
		strMsg.Format("通道%d没有扫描到IL小于%d dB的点，最小点为%.2f,将会继续测试。",nChannel+1,Point3,dblPrePower);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_GREEN);
		return TRUE;
	}
	else
	{
		strMsg.Format("通道%d没有扫描到IL小于%d dB的点，请重新配置该通道扫描范围！",nChannel+1,Point3);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
	}
	return FALSE;
}

void CMy126S_45V_Switch_AppDlg::OnButtonCheckData() 
{
	// TODO: Add your control notification handler code here
	CString strInitialFile;
	CString strPath;
	int     nPort;
	double dblMin=100;
	double dblMax=-100;
	CString strMsg;
	CString strTemp;

	BOOL bOK=TRUE;
	BOOL bFlagPass=TRUE;
	int nChannel=0;
	double dblValue;
	int nTemp;

	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	else
	{
		strPath = g_tszAppFolder;
	}

	//获取初测数据
	for (nPort=0;nPort<m_nCHCount;nPort++)
	{
		nTemp=nPort;
		if(m_strItemName == "")
		{
			//从初测数据里面获取通道标签
			if(!GetDataFromCSV(SELECT_CHANNEL_DATA,nPort+1,&dblValue))
			{
				strTemp.Format("获取通道编号失败！");
				LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
				return;
			}
			nChannel=(int)dblValue-1;
			nTemp=nChannel;
		}

		m_bDarkILPass[nPort]=TRUE;
		m_bRoomILPass[nPort]=TRUE;
		m_bLowILPass[nPort]=TRUE;
		m_bHighILPass[nPort]=TRUE;
		m_bRLPass[nPort]=TRUE;
		m_bCTPass[nPort]=TRUE;
		m_bTDLPass[nPort]=TRUE;
		m_bRepeatILPass[nPort]=TRUE; //判断哪些通道是合格的

		strMsg.Format("通道%d ",nPort+1);
		
		dblMin=100;
		dblMax=-100;
		double pdblValue[2]={0};
		//Vol数据
		if(!GetDataFromCSV(VOL_DATA,nPort+1,pdblValue))
		{
			ScanPowerChannel[nPort].VoltageX=-99;
			ScanPowerChannel[nPort].VoltageX=-99;
			strTemp=strMsg+"没有获取到Voltagex值数据";
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			ScanPowerChannel[nPort].VoltageX=pdblValue[0];
			ScanPowerChannel[nPort].VoltageY=pdblValue[1];
		}

		if (!GetVOAData(nPort))
		{
			
		}
		//CT数据
		if(!GetDataFromCSV(CT_DATA,nPort+1,&ChannalMessage[nPort].dblCT[0]))
		{
			ChannalMessage[nPort].dblIL=-99;
			strTemp=strMsg+"没有获取到CT值数据";
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		}

		double dblMinCT=1000;
		for(int i=0;i<m_nCHCount;i++)
		{
			if(i==nPort)
			{
				continue;
			}

			if(ChannalMessage[nPort].dblCT[i] < dblMinCT)
				dblMinCT = ChannalMessage[nPort].dblCT[i];
			
			if (!CheckParaPassOrFail(CT_DATA,ChannalMessage[nPort].dblCT[i]))
			{
				strMsg.Format("CH%d CT在通道%d不合格",nPort+1,i+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
				m_bCTPass[nPort]=FALSE;
			}
		}
		ChannalMessage[nPort].dblCT[31] = dblMinCT;
		//常温数据
		if(!GetDataFromCSV(IL_ROOM_DATA,nPort+1,&ChannalMessage[nPort].dblIL))
		{
			ChannalMessage[nPort].dblIL=-99;
			strTemp=strMsg+"没有获取到常温IL值数据";
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		}
		else 
		{
			m_bRoomILPass[nPort]=CheckParaPassOrFail(IL_ROOM_DATA,ChannalMessage[nPort].dblIL);
			if(!m_bRoomILPass[nPort])
			{
				strMsg.Format("CH%d常温IL不合格",nPort+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
	

		if(ChannalMessage[nPort].dblIL>dblMax)
			dblMax=ChannalMessage[nPort].dblIL;
		if(ChannalMessage[nPort].dblIL<dblMin)
			dblMin=ChannalMessage[nPort].dblIL;
		
		//低温数据
		if(!GetDataFromCSV(IL_LOW_DATA,nTemp+1,&ChannalMessage[nPort].dblLowTempIL))
		{
			ChannalMessage[nPort].dblLowTempIL=-99;
			strTemp=strMsg+"没有获取到低温IL值数据";
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
			bOK=FALSE;
		}
		else
		{
			m_bLowILPass[nPort]=CheckParaPassOrFail(IL_LOW_DATA,ChannalMessage[nPort].dblLowTempIL);
			if (!m_bLowILPass[nPort])
			{
				strMsg.Format("CH%d低温IL不合格",nPort+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		
		if(ChannalMessage[nPort].dblLowTempIL>dblMax)
			dblMax=ChannalMessage[nPort].dblLowTempIL;
		if(ChannalMessage[nPort].dblLowTempIL<dblMin)
			dblMin=ChannalMessage[nPort].dblLowTempIL;
		//高温数据
		if(!GetDataFromCSV(IL_HIGH_DATA,nTemp+1,&ChannalMessage[nPort].dblHighTempIL))
		{
			ChannalMessage[nPort].dblHighTempIL=-99;
			strTemp=strMsg+"没有获取到高温IL值数据";
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
			bOK=FALSE;
		}
		else 
		{
			m_bHighILPass[nPort]=CheckParaPassOrFail(IL_HIGH_DATA,ChannalMessage[nPort].dblHighTempIL);
			if (!m_bHighILPass[nPort])
			{
				strMsg.Format("CH%d高温IL不合格",nPort+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}

		if(ChannalMessage[nPort].dblHighTempIL>dblMax)
			dblMax=ChannalMessage[nPort].dblHighTempIL;
		if(ChannalMessage[nPort].dblHighTempIL<dblMin)
			dblMin=ChannalMessage[nPort].dblHighTempIL;

		//TDL
		ChannalMessage[nPort].dblTDL=dblMax-dblMin;
		SaveDataToCSV(TDL_DATA);
		//tdl数据
		if(!GetDataFromCSV(TDL_DATA,nTemp+1,&ChannalMessage[nPort].dblTDL))
		{
			ChannalMessage[nPort].dblTDL=-99;
			//ChannalMessage[nPort].dblTDL = CalTDL(ChannalMessage[nPort].dblIL,ChannalMessage[nPort].dblLowTempIL,ChannalMessage[nPort].dblHighTempIL);
			bOK=FALSE;
		}
	
		m_bTDLPass[nPort]=CheckParaPassOrFail(TDL_DATA,ChannalMessage[nPort].dblTDL);
		if (!CheckParaPassOrFail(TDL_DATA,ChannalMessage[nPort].dblTDL))
		{
		//	strMsg.Format("CH%dTDL不合格",nPort+1);
		//	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		
		//REP数据
		if(!GetDataFromCSV(RE_IL_DATA,nTemp+1,&ChannalMessage[nPort].dblDif))
		{
			ChannalMessage[nPort].dblDif=-99;
		//	strTemp=strMsg+"没有获取到RE值数据";
		//	LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		}
		else 
		{
			m_bRepeatILPass[nPort]=CheckParaPassOrFail(RE_IL_DATA,ChannalMessage[nPort].dblDif);
			if(!m_bRepeatILPass[nPort])
			{
		//		strMsg.Format("CH%d重复性不合格",nPort+1);
		//		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		//RL数据
		if(!GetDataFromCSV(RL_DATA,nTemp+1,&ChannalMessage[nPort].dblRL))
		{
			ChannalMessage[nPort].dblRL=-99;
			strTemp=strMsg+"没有获取到RL值数据";
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
			bOK=FALSE;
		}
		else 
		{
			m_bRLPass[nPort]= CheckParaPassOrFail(RL_DATA,ChannalMessage[nPort].dblRL);
			if(!m_bRLPass[nPort])
			{
				strMsg.Format("CH%dRL不合格",nPort+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		//SW TIME
		if(!GetDataFromCSV(SWITCH_TIME_DATA,nTemp+1,&m_dblSwitchTime))
		{
			m_dblSwitchTime = -99;
		//	strTemp=strMsg+"没有获取到开关切换时间数据";
		//	LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
			bOK=FALSE;
		}
		else
		{
			if (!CheckParaPassOrFail(SWITCH_TIME_DATA,m_dblSwitchTime))
			{
		//		strMsg.Format("CH%d开关切换时间不合格",nPort+1);
		//		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		//DARK数据
		if(!GetDataFromCSV(DARK_DATA,nTemp+1,&ChannalMessage[nPort].dblDarkIL))
		{
			ChannalMessage[nPort].dblDarkIL = -99;
			strTemp=strMsg+"没有获取到Dark值数据";
			LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
			bOK=FALSE;
		}
		else
		{
			m_bDarkILPass[nPort]=CheckParaPassOrFail(DARK_DATA,ChannalMessage[nPort].dblDarkIL);
			if (!m_bDarkILPass[nPort])
			{
				strMsg.Format("CH%dDARK不合格",nPort+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		//ISO
		if(!GetDataFromCSV(ISO_DATA,nTemp+1,&ChannalMessage[nPort].dblISO))
		{
			ChannalMessage[nPort].dblISO = -99;
		//	strTemp=strMsg+"没有获取到ISO值数据";
		//	LogInfo(strTemp,(BOOL)FALSE);
			bOK=FALSE;
		}
		//DIR DIRECTIVITY_DATA
		if(!GetDataFromCSV(DIRECTIVITY_DATA,nTemp,&ChannalMessage[nPort].dblMinDIR))
		{
			ChannalMessage[nPort].dblMinDIR=-99;
		//	strTemp=strMsg+"没有获取到方向性值的数据";
		//	LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		}
	}
	if(bOK)
	{
//		LogInfo("参数完整！");
	}
	else
	{
//		LogInfo("有缺少的参数！",(BOOL)FALSE,COLOR_RED);
	}

	if(!bFlagPass)
	{
//		LogInfo("有不合格的数据！",(BOOL)FALSE,COLOR_RED);
	}
	else if(bOK)
	{
//		LogInfo("参数都合格！");
	}
}

BOOL CMy126S_45V_Switch_AppDlg::FinalTest()
{
	CString strFile;
	CStdioFile stdFile;
	char chBuffer[256];
	CString strValue;
	CString strMsg;
	CString strPath;
	
	CString strCHSelINI;
	CStdioFile stdINIFile;
	CSelectCH  dlgSelect;
	CString    strKey;
//	int      nSelectCH;
	int  nPort=0;
	//保存本地文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	//		WIN32_FIND_DATA win32Find;
	CString strNetPath;
	
	if (m_bIfSaveToServe)  
	{
		strPath = m_strNetFile;  //调用网路数据
	}
	else
	{
		strPath = g_tszAppFolder;
	}
	
	if (m_cbTestItem.GetCurSel()==0)  //终测
	{
		if (1) //1X12
		{
			strCHSelINI.Format("%s\\data\\终测\\%s\\%s\\%s_SelectCH.ini",strPath,m_strPN,m_strSN,m_strSN);
			if (GetFileAttributes(strCHSelINI)==-1)
			{
				dlgSelect.m_strPath = strPath;
				dlgSelect.m_strPN = m_strPN;
				dlgSelect.m_strSN = m_strSN;
				dlgSelect.m_nCHCount = m_nCHCount;
				dlgSelect.DoModal();
			}
			for (nPort=0;nPort<m_nCHCount;nPort++)
			{
				strKey.Format("CH%d",nPort+1);
				GetPrivateProfileString("SelectCH",strKey,"ERROR",strValue.GetBuffer(128),128,strCHSelINI);
				if (strValue=="ERROR")
				{
					MessageBox("读取通道对应关系表错误！");
					return FALSE;
				}
		        m_nCHSelect[nPort] = atoi(strValue)-1;
			}
		}
		else
		{
			for (nPort=0;nPort<m_nCHCount;nPort++)
			{
				m_nCHSelect[nPort] = nPort;
			}
		}
		
		for (nPort=0;nPort<m_nCHCount;nPort++)
		{
			ZeroMemory(chBuffer,sizeof(chBuffer));
			if (m_cbComPort.GetCurSel()==0)
			{	
				strFile.Format("%s\\data\\初测\\%s\\%s\\%s_ScanVoltage_%d.csv",strPath,m_strPN,m_strSN,m_strSN,m_nCHSelect[nPort]+1);
				if(!stdFile.Open(strFile,CFile::modeRead))
				{
					strMsg.Format("打开文件%s失败！",strFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				stdFile.ReadString(chBuffer,256);		
				stdFile.ReadString(chBuffer,256);
				strValue = strtok(chBuffer,",");
				strValue = strtok(NULL,",");
				m_Xbase[nPort] = atoi(strValue);
				ScanPowerChannel[nPort].VoltageX = m_Xbase[nPort];
				strValue = strtok(NULL,",");
				m_Ybase[nPort] = atoi(strValue);
				ScanPowerChannel[nPort].VoltageY = m_Ybase[nPort];
				strValue = strtok(NULL,",");
				m_dbInitialIL[nPort] = atof(strValue);  //添加初测的IL值，以便和终测数据对比
				stdFile.Close();	

				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
                if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
				{
					MessageBox("创建数据文件失败！");
					return FALSE;
				}
				stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");
				//	for (int i=0;i<m_nCHCount;i++)
				{
					strContent.Format("%d,%d,%d,%.2f\n",nPort+1,m_Xbase[nPort],m_Ybase[nPort],m_dbInitialIL[nPort]);
					stdCSVFile.WriteString(strContent);
				}
	            stdCSVFile.Close();
				if (m_bIfSaveToServe)
				{
					CopyFile(strCSVFile,strNetPath,FALSE);
				}
				//保存电压数据到EXCEL
                SaveDataToExcel(VOL_DATA,nPort);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strFile.Format("%s\\data\\初测\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",strPath,m_strPN,m_strSN,m_strSN,m_nCHSelect[nPort]+1);
				if(!stdFile.Open(strFile,CFile::modeRead))
				{
					strMsg.Format("打开文件%s失败！",strFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				stdFile.ReadString(chBuffer,256);		
				stdFile.ReadString(chBuffer,256);
				strValue = strtok(chBuffer,",");
				strValue = strtok(NULL,",");
				m_Xbase[nPort] = atoi(strValue);
				ScanPowerChannel[nPort].VoltageX = m_Xbase[nPort];
				strValue = strtok(NULL,",");
				m_Ybase[nPort] = atoi(strValue);
				ScanPowerChannel[nPort].VoltageY = m_Ybase[nPort];
				strValue = strtok(NULL,",");
				m_dbInitialIL[nPort] = atof(strValue);
				stdFile.Close();	

				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
				if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
				{
					MessageBox("创建数据文件失败！");
					return FALSE;
				}
				stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");
				//	for (int i=0;i<m_nCHCount;i++)
				{
					strContent.Format("%d,%d,%d,%.2f\n",nPort+1,m_Xbase[nPort],m_Ybase[nPort],m_dbInitialIL[nPort]);
					stdCSVFile.WriteString(strContent);
				}
	            stdCSVFile.Close();
				if (m_bIfSaveToServe)
				{
					CopyFile(strCSVFile,strNetPath,FALSE);
				}
				//保存电压数据到EXCEL
                SaveDataToExcel(VOL_DATA,nPort);
			}
		}
		SaveDataToCSV(VOL_DATA);
		UpdateATMData(VOL_DATA);
	}
	LogInfo("调用初测电压数据结束");
	//复制初测的数据;

	return TRUE;
}


//这里需要把所有需要从excel里获取的数据从csv文件里读取。**********************************
//2015-2-25*******************************************************************************
//****************************************************************************************
BOOL CMy126S_45V_Switch_AppDlg::CopyIntialTestData()
{
	CString strInitialFile;
	CString strPath;
	int     nPort;
	double dblMin=100;
	double dblMax=-100;


	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	else
	{
		strPath = g_tszAppFolder;
	}
	//获取初测数据

	for (nPort=0;nPort<m_nCHCount;nPort++)
	{
		dblMin=100;
		dblMax=-100;
		//常温数据
		if(!GetDataFromCSV(IL_ROOM_DATA,nPort+1,&ChannalMessage[nPort].dblIL))
		{
			ChannalMessage[nPort].dblIL=-99;
			LogInfo("没有获取到常温IL值数据");
		}

		if(ChannalMessage[nPort].dblIL>dblMax)
			dblMax=ChannalMessage[nPort].dblIL;
		if(ChannalMessage[nPort].dblIL<dblMin)
			dblMin=ChannalMessage[nPort].dblIL;
		
		//低温数据
		if(!GetDataFromCSV(IL_LOW_DATA,nPort+1,&ChannalMessage[nPort].dblLowTempIL))
		{
			ChannalMessage[nPort].dblLowTempIL=-99;
			LogInfo("没有获取到低温IL值数据");
		}
		if(ChannalMessage[nPort].dblLowTempIL>dblMax)
			dblMax=ChannalMessage[nPort].dblLowTempIL;
		if(ChannalMessage[nPort].dblLowTempIL<dblMin)
			dblMin=ChannalMessage[nPort].dblLowTempIL;
		//高温数据
		if(!GetDataFromCSV(IL_HIGH_DATA,nPort+1,&ChannalMessage[nPort].dblHighTempIL))
		{
			ChannalMessage[nPort].dblHighTempIL=-99;
			LogInfo("没有获取到高温IL值数据");
		}
		if(ChannalMessage[nPort].dblHighTempIL>dblMax)
			dblMax=ChannalMessage[nPort].dblHighTempIL;
		if(ChannalMessage[nPort].dblHighTempIL<dblMin)
			dblMin=ChannalMessage[nPort].dblHighTempIL;
		//tdl数据
		if(!GetDataFromCSV(TDL_DATA,nPort+1,&ChannalMessage[nPort].dblTDL))
		{
			ChannalMessage[nPort].dblTDL=-99;
			LogInfo("没有获取到TDL值数据");
		}
	
		//REP数据
		if(!GetDataFromCSV(RE_IL_DATA,nPort+1,&ChannalMessage[nPort].dblDif))
		{
			ChannalMessage[nPort].dblDif=-99;
			LogInfo("没有获取到RE值数据");
		}
		//RL数据
		if(!GetDataFromCSV(RL_DATA,nPort+1,&ChannalMessage[nPort].dblRL))
		{
			ChannalMessage[nPort].dblRL=-99;
			LogInfo("没有获取到RL值数据");
		}
		//SW TIME
		if(!GetDataFromCSV(SWITCH_TIME_DATA,nPort+1,&m_dblSwitchTime))
		{
			m_dblSwitchTime = -99;
			LogInfo("没有获取到开关切换时间数据");
		}
		//DARK数据
		if(!GetDataFromCSV(DARK_DATA,nPort+1,&ChannalMessage[nPort].dblDarkIL))
		{
			ChannalMessage[nPort].dblDarkIL = -99;
			LogInfo("没有获取到Dark值数据");
		}
		//DARK数据
		if(!GetDataFromCSV(DIRECTIVITY_DATA,nPort,&ChannalMessage[nPort].dblMinDIR))
		{
			ChannalMessage[nPort].dblMinDIR = -99;
			LogInfo("没有获取到方向性数据");
		}
		//TDL
	//	ChannalMessage[nPort].dblTDL=dblMax-dblMin;
	}
	m_nSwitchTimeCH = m_nCHCount;
	//写入新的Excel数据
	//SaveDataToExcel(IL_ROOM_DATA);
//	UpdateATMData(IL_ROOM_DATA);

	UpdateATMData(IL_LOW_DATA);
	UpdateATMData(IL_HIGH_DATA);
	UpdateATMData(TDL_DATA);
	UpdateATMData(RE_IL_DATA);
	UpdateATMData(RL_DATA);
	UpdateATMData(DARK_DATA);
	UpdateATMData(SWITCH_TIME_DATA);
	for (int nCh=0;nCh<m_nCHCount;nCh++)
	{
		UpdateATMData(DIRECTIVITY_DATA,nCh);
	}
	//UpdateATMData(DIRECTIVITY_DATA);
	LogInfo("保存数据结束！");

	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnButtonOnekeyTest() 
{
//	// TODO: Add your control notification handler code here
//	double dblCurTempt;
//	double dblTargetTempt=m_dblTemperature[0];
//	CString strMsg;
//	int     nTemptStatus = 0;
//	int i;
//	m_bTestStop = FALSE;
//	UpdateData();
//	m_NTGraph.ClearGraph();
//	if (!m_bOpenPM)
//	{
//		MessageBox("请先打开光功率计！");
//		return;
//	}
//	if (!m_bOpenPort)
//	{
//		MessageBox("请先打开串口！");
//		return ;
//	}
//	if (!m_bGetSNInfo)
//	{
//		MessageBox("请先确认SN号的信息！");
//		return;
//	}
//	if (!m_bHasRefForTest)
//	{
//		MessageBox("请先归零！");
//		return ;
//	}
//	//获取当前时间
//	SYSTEMTIME st;
//	COleDateTime    tCurTime;
//	COleDateTimeSpan tDifHour; 
//	int        nDifHour;
//	GetLocalTime(&st);
//	tCurTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
//	tDifHour = tCurTime-m_RefStartTime; //计算时间插值
//	nDifHour = tDifHour.GetTotalHours();
//	if (nDifHour>m_nRefTime)  //比较归零时间差异
//	{
//		strMsg.Format("当前归零时间为%d小时，已超过归零限制时间（%d小时），请重新归零！",nDifHour,m_nRefTime);
//		MessageBox(strMsg);
//		m_bHasRefForTest = FALSE;
//		return;
//	}
//	//设置常温
//    m_ctrlComboxTemperature.SetCurSel(0);
//	UpdateWindow();
//	//判断当前温度是否达到要求；
//	if (m_bConnectTemptControl)
//	{
//		dblCurTempt = GetCurrentTempt(); //获取当前冷热盘温度
//		if (fabs(dblTargetTempt-dblCurTempt)>2)
//		{
//			strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
//			LogInfo(strMsg,BOOL(FALSE),COLOR_RED);
//			SetTemptStatus(0);
//			return;
//		}
//	}
//
//	OnButtonIlctTest();
//	if (m_bTestStop)
//	{
//		LogInfo("测试停止");
//		return;
//	}
//	//OnButtonWdl2();
//	OnButtonReIlTest();
//	if (m_bTestStop)
//	{
//		LogInfo("测试停止");
//		return;
//	}
//	OnButtonDarkTest();
//
////	OnButtonSwitchTimeTest();
//
//	if (m_bTestStop)
//	{
//		LogInfo("测试停止");
//		return;
//	}
//	if (m_nSwitchComPortCount==2)
//	{
//		OnButtonIsoTest();
//	}
//
//	if ((m_strPN=="18X1")||(m_strPN=="1X12-2"))
//	{
//		if (m_strSpec=="13998")
//		{
//
//		}
//		else
//		{
//			return;
//		}
//		
//	}
//	//开始测试低温数据；
//	//dblTargetTempt = -5;
//	dblTargetTempt = m_dblTemperature[1];
//    m_ctrlComboxTemperature.SetCurSel(1);
//	SetTemptStatus(2);
//	UpdateWindow();
//	while (1)
//	{
//		YieldToPeers();
//		if (m_bTestStop) 
//		{
//			LogInfo("测试终止");
//			return;
//		}
//		SetTemptStatus(2);
//
//		dblCurTempt = GetCurrentTempt();
//		strMsg.Format("当前温度为：%.1f℃",dblCurTempt);
//		LogInfo(strMsg);
//		if (fabs(dblCurTempt-dblTargetTempt)<2)
//		{
//			break;
//		}
//		Sleep(1000);
//		
//	}
//	for (i=300;i>0;i--)
//	{
//		YieldToPeers();
//		if (m_bTestStop) 
//		{
//			LogInfo("测试终止");
//			return;
//		}
//		strMsg.Format("剩余时间：%d s",i);
//		LogInfo(strMsg);
//		Sleep(1000);
//	}
//	if (m_bTestStop)
//	{
//		LogInfo("测试停止");
//		return;
//	}
//	OnButtonIlctTest();
//	//OnButtonWdl2();
//	if (stricmp(m_strSpec,"Z4767")==0)
//	{
//		if (stricmp(m_strGetPN,"77501363A2")==0||stricmp(m_strGetPN,"77501354A2")==0)
//		{
//			if (m_cbTestCH.GetCurSel()==1)
//			{
//				OnButtonWdl2();//低温添加5dB/8dBWDL测试
//        		OnButtonWdlRepeat();//低温添加WDL测试
//			}
//			else
//			{
//				for (int nPort=0;nPort<8;nPort++)
//				{
//					m_pdb5dBWDL[nPort] = 0.1;
//					m_pdb8dBWDL[nPort] = 0.1;
//					m_pdb5dBRepeatAtten[nPort] = 0;
//					m_pdb8dBRepeatAtten[nPort] = 0;
//				}
//				
//			}
//		}
//	}
//	
//	Sleep(1000);
//
//	//开始测试高温数据；
//	//dblTargetTempt = 70;	
//	if (stricmp(m_strSpec,"Z4767")==0)
//	{
//		dblTargetTempt = 85;
//	}
//	dblTargetTempt = m_dblTemperature[2];
//    m_ctrlComboxTemperature.SetCurSel(2);
//	SetTemptStatus(1);
//
//	UpdateWindow();
//	while (1)
//	{
//		SetTemptStatus(1);
//		YieldToPeers();
//		if (m_bTestStop) 
//		{
//			LogInfo("测试终止");
//			return;
//		}
//		dblCurTempt = GetCurrentTempt();
//		strMsg.Format("当前温度为：%.1f℃",dblCurTempt);
//		LogInfo(strMsg);
//		if (fabs(dblCurTempt-dblTargetTempt)<2)
//		{
//			break;
//		}
//		Sleep(1000);	
//	}
//	for (i=480;i>0;i--)
//	{
//		YieldToPeers();
//		if (m_bTestStop) 
//		{
//			LogInfo("测试终止");
//			return;
//		}
//		strMsg.Format("剩余时间：%d s",i);
//		LogInfo(strMsg);
//		Sleep(1000);
//	}
//	if (m_bTestStop)
//	{
//		LogInfo("测试停止");
//		return;
//	}
//	OnButtonIlctTest();
//	//OnButtonWdl2();
//	LogInfo("测试完毕！");
//	OnButtonSelectChannel();
//
//	SetTemptStatus(0);
//	OnButtonUploadAtms();
//	m_ctrlComboxTemperature.SetCurSel(0);
   OnButtonRlTest();
   OnButtonIlctTest();
   OnButtonDarkTest();
   OnButtonReIlTest();

    int nTemp = m_ctrlComboxTemperature.GetCurSel();
	if (nTemp==2)
	{

		OnButtonUploadAtms();
	}


	
}

void CMy126S_45V_Switch_AppDlg::OnButtonRlRef() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;

//	if (!m_bConnect1830C)
//	{
//		MessageBox("请先连接1830C");
//		return;
//	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	CString strTemp;
	int nWL = m_cbSetWL.GetCurSel()-1;
	if (nWL>=0)
	{
		//if(!m_1830CRL.SetPWMParameters(0,1,1,m_dblWL[nWL],0.01,0))
		//{
		//	LogInfo("设置1830C参数失败！");
		//	return;
		//}
		    if(!SetPWMWL(m_dblWL[nWL],19))
			{
				LogInfo("设置功率计波长失败！");
				return;
			}
			strTemp.Format("设置通道20波长为%.2f成功",m_dblWL[nWL]);
			LogInfo(strTemp);
	}
	else
	{
	//	if(!m_1830CRL.SetPWMParameters(0,1,1,m_pdblWavelength,0.01,0))
	//	{
	//		LogInfo("设置1830C参数失败！");
	//		return;
	//	}
			if(!SetPWMWL(m_pdblWavelength,19))
			{
				LogInfo("设置功率计波长失败！");
				return;
			}
			strTemp.Format("设置通道20波长为%.2f成功",m_pdblWavelength);
			LogInfo(strTemp);
			
	}
	
	UpdateData();
	double dblReadPower=0;
	MessageBox("请绕死光源出光端，并将RL头接入1830C光功率计！");
	LogInfo("开始记录系统回损,请等待...");

	//连续读取5次，记录绝对功率最大的值（RL最差的值）
	double dblMaxPower=-99;
	for (int i=0;i<3;i++)
	{
	//	if(!m_1830CRL.ReadPower(0,1,&dblReadPower))
	//	{
	//		MessageBox("读取1830C光功率计错误！");
	//		return;
	//	}
		dblReadPower = ReadPWMPower(19);

		Sleep(100);
		if (dblReadPower>dblMaxPower) //
		{
			dblMaxPower = dblReadPower;
		}
	}
	if (nWL>=0)
	{
		m_dblSystemRL = m_dblWDLRef[0][nWL] - dblMaxPower;
	}
	else
	{
		m_dblSystemRL = m_dblReferencePower[0] - dblMaxPower;
		//m_dblSystemRL = abf(dblMaxPower);
	}
	
	strMsg.Format("系统归零RL为%.2fdBm",m_dblSystemRL);
	LogInfo(strMsg);
	m_bRefRL = TRUE;
	MessageBox("RL系统归零完毕");
}

BOOL CMy126S_45V_Switch_AppDlg::SaveDataToCSV(int nType, int nChannel,CString strTemperature)
{
	CString strValue;
	CString strTemp;
	int nIndex=0;
	int nIndexCH=0;
	CString strCSVFile;
	CString strNetFile;
	FILE *fp = NULL;

	UpdateData();
	FindMyDirectory(m_strItemName,m_strPN,m_strSN);
	//启动EXCEL
	//保存的数据类型
	int nWL = m_cbSetWL.GetCurSel()-1;

	int nTemp = m_ctrlComboxTemperature.GetCurSel();
	CString str_tmperatuer;
	if (nTemp==0)
	{
		str_tmperatuer = "Voltage-ROOM";
	}
	else if (nTemp==1)
	{
		str_tmperatuer = "Voltage-LOW";
	
	}
	else 
	{
		str_tmperatuer = "Voltage-HIGH";	
	}	
	switch (nType)
	{
	case VOL_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,str_tmperatuer);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,str_tmperatuer);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,str_tmperatuer);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,str_tmperatuer);
		}
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,X-Voltage,Y-Voltage\n");
		fprintf(fp,strValue);
	
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%d,%d\n",nIndex+1,ScanPowerChannel[nIndex].VoltageX,ScanPowerChannel[nIndex].VoltageY);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;
		
	case IL_ROOM_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
			
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,IL\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblIL);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;
	case IL_LOW_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Low-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Low-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Low.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Low.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Low-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Low-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
		
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,IL\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblLowTempIL);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;
	case IL_HIGH_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_High-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_High-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_High.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_High.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_High-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_High-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,IL\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblHighTempIL);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;
	case TDL_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\TDL-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\TDL-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\TDL.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\TDL.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\TDL-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\TDL-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,TDL\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblTDL);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;
	case CT_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
		fp = fopen(strCSVFile,"wt");
		strTemp = "CH";
		for (nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format(",CT%d",nIndex+1);
			strTemp = strTemp + strValue;
		}
		strTemp = strTemp + "\n";
		fprintf(fp,strTemp);
			
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,",nIndex+1);
			fprintf(fp,strValue);
			for(nIndexCH=0;nIndexCH<m_nCHCount;nIndexCH++)
			{
				strValue.Format("%.2f,",ChannalMessage[nIndex].dblCT[nIndexCH]);
				fprintf(fp,strValue);
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
		break; 
	case  DIRECTIVITY_DATA:
        if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature);
		}
		fp = fopen(strCSVFile,"wt");
		strTemp = "CH,Directivity\n";
		fprintf(fp,strTemp);
		strTemp.Format("%d,%.2f\n",nChannel+1,ChannalMessage[nChannel].dblMinDIR);
		fprintf(fp,strTemp);
		fclose(fp);
		break;
	case DARK_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\DarkIL-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
		        strNetFile.Format("%s\\data\\%s\\%s\\%s\\DarkIL-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\DarkIL.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		        strNetFile.Format("%s\\data\\%s\\%s\\%s\\DarkIL.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\DarkIL-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\%s\\%s\\%s\\DarkIL-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}	
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,DarkIL\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblDarkIL);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;	

	case RE_IL_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\RepeatIL-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
		        strNetFile.Format("%s\\data\\%s\\%s\\%s\\RepeatIL-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\RepeatIL.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		        strNetFile.Format("%s\\data\\%s\\%s\\%s\\RepeatIL.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}

		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\RepeatIL-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\%s\\%s\\%s\\RepeatIL-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,RepeatIL\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblDif);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;

	case ISO_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\ISO.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\%s\\%s\\%s\\ISO.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\ISO-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\%s\\%s\\%s\\ISO-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}			
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,ISO\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblISO);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;
	case RL_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\RL-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\RL-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\RL.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\RL.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);

			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\RL-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\RL-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}			
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,RL\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblRL);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;
	case SWITCH_TIME_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\SwitchTime.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\SwitchTime.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\SwitchTime-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\SwitchTime-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}			
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,SwitchTime\n");
		fprintf(fp,strValue);
		strValue.Format("CH%d-CH%d,%.2f\n",nChannel,m_nSwitchTimeCH,m_dblSwitchTime);
		fprintf(fp,strValue);
		fclose(fp);
		break;
	case DIRECTION_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\CH%02d_Direction.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel);
		    strNetFile.Format("%s\\data\\%s\\%s\\%s\\CH%02d_Direction.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\CH%02d_Direction-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel);
		    strNetFile.Format("%s\\data\\%s\\%s\\%s\\CH%02d_Direction-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel);
		}	
		fp = fopen(strCSVFile,"wt");
		if (m_nCHCount==18)
		{
			strValue.Format("COM,CT1,CT2,CT3,CT4,CT5,CT6,CT7,CT8,CT9,CT10,CT11,CT12,CT13,CT14,CT15,CT16,CT17,CT18\n");
		    fprintf(fp,strValue);

			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				strValue.Format("%d,",nIndex+1);
				fprintf(fp,strValue);
				for(nIndexCH=0;nIndexCH<m_nCHCount;nIndexCH++)
				{
					strValue.Format("%.2f,",dblDir[nIndex][nIndexCH]);
					fprintf(fp,strValue);
				}
				fprintf(fp,"\n");
			}
			fclose(fp);
		}
		break;
	case PDL_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\PDL-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\PDL-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\PDL.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\PDL.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);

			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\PDL-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\PDL-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}			
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,PDL\n");
		fprintf(fp,strValue);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblPDL);
			fprintf(fp,strValue);
		}
		fclose(fp);
		break;
	case WDL_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\WDL_CH%02d.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\WDL_CH%02d.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1);			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\WDL_CH%02d-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\WDL_CH%02d-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1);
		}			
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,WDL\n");
		fprintf(fp,strValue);
		strValue.Format("%d,%.2f\n",nChannel+1,ChannalMessage[nChannel].dblWDL[0]);
		fprintf(fp,strValue);
		fclose(fp);
		break;
	case TESTDATA:
		if (m_cbComPort.GetCurSel()==0)
		{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\TEST_CH.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\TEST_CH.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\TEST_CH-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\TEST_CH-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}			
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,IL\n");
		fprintf(fp,strValue);
		for (nIndex=0;nIndex<20;nIndex++)
		{
			strValue.Format("%d,%.2f\n",m_nSwitchCh+nIndex,db_sacnchIL[nIndex]);
			fprintf(fp,strValue);
		}
		
		fclose(fp);
		break;
	}
	if (m_bIfSaveToServe)
	{
		CopyFile(strCSVFile,strNetFile,FALSE);
	}	
	LogInfo("保存数据到CSV结束！");
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SetTempToPoint(int SetTemp)
{
	BYTE bTxContent[15];
	char chTxContent[15];
	CString strTemp;
	ZeroMemory(bTxContent,sizeof(bTxContent));
	ZeroMemory(chTxContent,sizeof(chTxContent));
	
	BYTE bCheckSum = 0X00;
	
	bTxContent[0] = 0X2A; //START
	bTxContent[1] = 0X30; //Flag
	bTxContent[2] = 0X30; //Flag
	bTxContent[3] = 0X30; //
	bTxContent[4] = 0X32; //功能:02H
	if (SetTemp>=50)
	{
		bTxContent[5] = 0X31;  //加热：1 
	}
	else if (SetTemp<=0)
	{
		bTxContent[5] = 0X32;  //制冷：2
	}
	else
	{
		bTxContent[5] = 0X30;  //回到常温
	}
	bTxContent[6] = (BYTE)(fabs(SetTemp)/10);   //温度
	bTxContent[7] = (BYTE)(fabs(SetTemp%10));   //温度
	bTxContent[8] = 0x30;   //温度
	bTxContent[9] = 0x30;    //保持时间
	bTxContent[10] = 0x30;   //保持时间
	bTxContent[11] = 0x30;   //保持时间
	bTxContent[12] = 0x30;   //保持时间
	//checksum
	for (int i=1;i<13;i++)
	{
		bCheckSum += bTxContent[i];
	}
	bTxContent[13] = bCheckSum;
	bTxContent[14] = 0X0D;
	
	memcpy(chTxContent,bTxContent,15);
	if(!m_opTempt.WriteBuffer(chTxContent,15))
	{
		strTemp = "发送设置温度指令失败!";
		LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	Sleep(1000); //温度等待时间
	
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::SetTemptStatus(int nStatus)
{
	BYTE bTxContent[15];
	char chTxContent[15];
	CString strTemp;
	ZeroMemory(bTxContent,sizeof(bTxContent));
	ZeroMemory(chTxContent,sizeof(chTxContent));
	
	BYTE bCheckSum = 0X00;
	
	bTxContent[0] = 0X2A; //START
	bTxContent[1] = 0X30; //Flag
	bTxContent[2] = 0X30; //Flag
	bTxContent[3] = 0X30; //
	bTxContent[4] = 0X34; //功能:04H
	bTxContent[5] = 0X30; 
	bTxContent[6] = 0X30+(BYTE)nStatus;  
	
	bTxContent[7] = 0x30;   //温度
	bTxContent[8] = 0x30;   //温度
	bTxContent[9] = 0x30;    //保持时间
	bTxContent[10] = 0x30;   //保持时间
	bTxContent[11] = 0x30;   //保持时间
	bTxContent[12] = 0x30;   //保持时间
	//checksum
	for (int i=1;i<13;i++)
	{
		bCheckSum += bTxContent[i];
	}
	bTxContent[13] = bCheckSum;
	bTxContent[14] = 0X0D;
	
	memcpy(chTxContent,bTxContent,15);
	if(!m_opTempt.WriteBuffer(chTxContent,15))
	{
		strTemp = "发送设置温度指令失败!";
		LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	Sleep(50);
	if(!m_opTempt.ReadBuffer(chTxContent,15))
	{
		strTemp = "发送设置温度指令失败!";
		LogInfo(strTemp,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	return TRUE;

}

double CMy126S_45V_Switch_AppDlg::GetCurrentTempt()
{
	BYTE bTxContent[15];
	char chTxContent[15];
	char chRxContent[256];
	CString strTemp;
	double  dblTempt1=-99;
	double  dblTempt2=-99;
	double  dblTempt=-99;
	
	ZeroMemory(bTxContent,sizeof(bTxContent));
	ZeroMemory(chTxContent,sizeof(chTxContent));
	ZeroMemory(chRxContent,sizeof(chRxContent));
	
	BYTE bCheckSum = 0X00;
	
	bTxContent[0] = 0X2A; //START
	bTxContent[1] = 0X30; //Flag
	bTxContent[2] = 0X30; //Flag
	bTxContent[3] = 0X30; //
	bTxContent[4] = 0X35; //功能:05H
	bTxContent[5] = 0X30;  //
	
	bTxContent[6] = 0X30;   //温度
	bTxContent[7] = 0X30;   //温度
	bTxContent[8] = 0x30;   //温度
	bTxContent[9] = 0x30;    //保持时间
	bTxContent[10] = 0x30;   //保持时间
	bTxContent[11] = 0x30;   //保持时间
	bTxContent[12] = 0x30;   //保持时间
	//checksum
	for (int i=1;i<13;i++)
	{
		bCheckSum += bTxContent[i];
	}
	bTxContent[13] = bCheckSum;
	bTxContent[14] = 0X0D;
	
	memcpy(chTxContent,bTxContent,15);
	if(!m_opTempt.WriteBuffer(chTxContent,15))
	{
		strTemp = "发送读取温度指令失败!";
		LogInfo(strTemp,TRUE,COLOR_RED);
		return dblTempt;
	}
	Sleep(50);
	if (!m_opTempt.ReadBuffer(chRxContent,256))
	{
		strTemp = "接收读取温度指令失败!";
		LogInfo(strTemp,TRUE,COLOR_RED);
		return dblTempt;
	}
	
	dblTempt1 = ((BYTE)chRxContent[10]-0X30)*10+((BYTE)chRxContent[11]-0X30)+((BYTE)chRxContent[12]-0X30)*0.1;
	if((BYTE)chRxContent[9]==0X31) //温度为正
	{
		dblTempt1 =dblTempt1;
	}
	else if ((BYTE)chRxContent[9]==0X32) //温度为负
	{
		dblTempt1 = -dblTempt1;
	}
	dblTempt2 = ((BYTE)chRxContent[25]-0X30)*10+((BYTE)chRxContent[26]-0X30)+((BYTE)chRxContent[27]-0X30)*0.1;
	if((BYTE)chRxContent[24]==0X31) //温度为正
	{
		dblTempt2 =dblTempt2;
	}
	else if ((BYTE)chRxContent[24]==0X32) //温度为负
	{
		dblTempt2 = -dblTempt2;
	}
	dblTempt = (dblTempt1+dblTempt2)/2.0;
	
	return dblTempt;

}

BOOL CMy126S_45V_Switch_AppDlg::GetFinalData()
{
	CString strInitialFile;
	CString strPath;
	CString strMsg;
	int     nPort;
	BOOL    bFlagPass = TRUE;

	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	else
	{
		strPath = g_tszAppFolder;
	}
	UpdateData();

	//获取初测数据
	strInitialFile.Format("%s\\data\\%s\\%s\\%s\\%s_Optical_Test_Data.xls",strPath,m_strItemName,m_strPN,m_strSN,m_strSN);
	if (GetFileAttributes(strInitialFile)==-1)
	{
		strMsg.Format("%s不存在！",strInitialFile);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	COleVariant  m_covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR);
	if (!m_app.CreateDispatch("EXCEL.Application",NULL))
	{
		AfxMessageBox("无法创建EXCEL应用！");
		return FALSE;
	}
	m_bStartExcel = TRUE;
	//允许其他用户控制EXCEL
	m_app.SetUserControl(TRUE);
	//打开EXCEL
	m_books.AttachDispatch(m_app.GetWorkbooks(),true);
	m_lpDisp=m_books.Open(strInitialFile,m_covOptional,m_covOptional,m_covOptional,m_covOptional,
		m_covOptional,m_covOptional,m_covOptional,m_covOptional,m_covOptional,m_covOptional,
		m_covOptional,m_covOptional);
	ASSERT(m_lpDisp);
	m_book.AttachDispatch(m_lpDisp);
	m_sheets.AttachDispatch(m_book.GetWorksheets(),true);
	
	m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t((short)1)));
	m_range.AttachDispatch(m_sheet.GetCells(),true);
    COleVariant   vResult;
	for (int nCH=0;nCH<m_nCHCount;nCH++)
	{
		nPort = m_nCHSelect[nCH];
		//电压X数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(10)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ScanPowerChannel[nCH].VoltageX = vResult.dblVal;
	
		}
		else
		{
			ScanPowerChannel[nCH].VoltageX = -99;
			strMsg.Format("CH%d没有获取到常温X电压",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//电压Y数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(12)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ScanPowerChannel[nCH].VoltageY = vResult.dblVal;
			
		}
		else
		{
			ScanPowerChannel[nCH].VoltageY = -99;
			strMsg.Format("CH%d没有获取到常温Y电压",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}

		//常温IL数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(14)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ChannalMessage[nCH].dblIL = vResult.dblVal;
			if (!CheckParaPassOrFail(IL_ROOM_DATA,ChannalMessage[nCH].dblIL))
			{
				strMsg.Format("CH%d常温IL不合格",nCH+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			ChannalMessage[nCH].dblIL = -99;
			strMsg.Format("CH%d没有获取到常温IL",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//低温数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(15)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ChannalMessage[nCH].dblLowTempIL = vResult.dblVal;
			if (!CheckParaPassOrFail(IL_LOW_DATA,ChannalMessage[nCH].dblLowTempIL))
			{
				strMsg.Format("CH%d低温IL不合格",nCH+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			ChannalMessage[nCH].dblLowTempIL = -99;
			strMsg.Format("CH%d没有获取到低温IL",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//高温数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(16)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ChannalMessage[nCH].dblHighTempIL = vResult.dblVal;
			if (!CheckParaPassOrFail(IL_HIGH_DATA,ChannalMessage[nCH].dblHighTempIL))
			{
				strMsg.Format("CH%d高温IL不合格",nCH+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			ChannalMessage[nCH].dblHighTempIL = -99;
			strMsg.Format("CH%d没有获取到高温IL",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//tdl数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(20)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ChannalMessage[nCH].dblTDL = vResult.dblVal;
			if (!CheckParaPassOrFail(TDL_DATA,ChannalMessage[nCH].dblTDL))
			{
				strMsg.Format("CH%dTDL不合格",nCH+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			ChannalMessage[nCH].dblTDL = -99;
			strMsg.Format("CH%d没有获取到TDL",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//REP数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(22)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ChannalMessage[nCH].dblDif = vResult.dblVal;
			if (!CheckParaPassOrFail(RE_IL_DATA,ChannalMessage[nCH].dblDif))
			{
				strMsg.Format("CH%d重复性不合格",nCH+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			ChannalMessage[nCH].dblDif = -99;
			strMsg.Format("CH%d没有获取到重复性数据",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//RL数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(23)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ChannalMessage[nCH].dblRL = vResult.dblVal;
			if (!CheckParaPassOrFail(RL_DATA,ChannalMessage[nCH].dblRL))
			{
				strMsg.Format("CH%dRL不合格",nCH+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			ChannalMessage[nCH].dblRL = -99;
			strMsg.Format("CH%d没有获取到RL数据",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//开关切换时间
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(13)),_variant_t((long)(4+m_nCHCount-1))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			m_dblSwitchTime = vResult.dblVal;
			if (!CheckParaPassOrFail(SWITCH_TIME_DATA,m_dblSwitchTime))
			{
				strMsg.Format("开关切换时间不合格");
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			m_dblSwitchTime = -99;
			strMsg.Format("没有获取到开关切换时间数据");
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//RL数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(23)),_variant_t((long)(4+nPort))).pdispVal);
        vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ChannalMessage[nCH].dblRL = vResult.dblVal;
			if (!CheckParaPassOrFail(RL_DATA,ChannalMessage[nCH].dblRL))
			{
				strMsg.Format("CH%dRL不合格",nCH+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			ChannalMessage[nCH].dblRL = -99;
			strMsg.Format("CH%d没有获取到RL数据",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
		//DARK数据
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		if (m_nCHCount<=12)
		{
			m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(36)),_variant_t((long)(4+nPort))).pdispVal);
		}
		else
		{
			m_range.AttachDispatch(m_range.GetItem(_variant_t((long)(43)),_variant_t((long)(4+nPort))).pdispVal);
		}
		vResult = m_range.GetValue();
		if (vResult.vt == VT_R8)
		{
			ChannalMessage[nCH].dblDarkIL = vResult.dblVal;
			if (!CheckParaPassOrFail(DARK_DATA,ChannalMessage[nCH].dblDarkIL))
			{
				strMsg.Format("CH%dDARK不合格",nCH+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				bFlagPass = FALSE;
			}
		}
		else
		{
			ChannalMessage[nCH].dblDarkIL = -99;
			strMsg.Format("CH%d没有获取到Dark值数据",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			bFlagPass = FALSE;
		}
	}
	CloseExcelFile();
	if (bFlagPass)
	{
		LogInfo("所有参数都合格！");
	}
	strMsg.Format("获取%s数据成功！",strInitialFile);
	LogInfo(strMsg);
	return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonReadTempt() 
{
	// TODO: Add your control notification handler code here
	m_bTestStop = FALSE;
	double dbCurTempt;
	CString strMsg;
	if (m_bConnectTemptControl)
	{
		while (1)
		{
			YieldToPeers();
			if (m_bTestStop)
			{
				return;
			}
			dbCurTempt = GetCurrentTempt();
			strMsg.Format("当前温度为：%.2f ℃",dbCurTempt);
			LogInfo(strMsg);
			Sleep(1000);
		}
	}
	else
	{
		LogInfo("请先打开冷热盘！");
	}
		
	
}

void CMy126S_45V_Switch_AppDlg::OnKillfocusEditSn() 
{
	// TODO: Add your control notification handler code here
//	UpdateData();
//	//获取无纸化PN
//	CString strFileName;
//	CString strURL,strTemp;
//	CString strLocalFile;
//	CString strMsg;
//	//获取配置信息
//	CString strFile;
//	CString strValue;
//	CString strKey;
//	int     nNodeCount;
//	CString strErrMsg;
//	DWORD   dwBuf[256];
//	CString strWDLFile;
//	int     nRand = rand();
//	m_bGetSNInfo = FALSE;
//	GetLocalTime(&m_StartTime);
//	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&randno=%d",m_strSN,nRand);	
//	//获得XML模板文件
//	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
//	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
//	{
//		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
//	}
//	else
//	{
//		strMsg = "模板下载错误!";
//		MessageBox(strMsg);
//		return;
//	}
//
//	/*
//	//查看是否有错误
//	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
//	nNodeCount = 0;
//	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Error", dwBuf, nNodeCount);
//	if (nNodeCount == 0)
//	{
//		strMsg = "XML文件结点不完整,没有找到无纸化工序信息!";
//		MessageBox(strMsg);
//		return;
//	}
//	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
//	*/
//
//	//设置初测还是终测，获取当前工序
//	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
//	nNodeCount = 0;
//	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PROCESS_TYPE", dwBuf, nNodeCount);
//	if (nNodeCount == 0)
//	{
//		strMsg = "XML文件结点不完整,没有找到无纸化工序信息!";
//		MessageBox(strMsg);
//		return;
//	}
//	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);	
//	if (stricmp(strTemp,"EBOX_MEMSSWITCH07")==0)
//	{
//		m_strItemName = "终测";
//		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测2");
//		m_cbTestItem.SetCurSel(0);
//	}
//	else if (stricmp(strTemp,"EBOX_MEMSSWITCH19")==0)
//	{
//		m_strItemName = "初测";
//		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
//		m_cbTestItem.SetCurSel(1);
//	}
//	else
//	{
//		m_strItemName = strTemp;
//		strMsg.Format("工序代号%s不在该软件测试范围内，请确认！",strTemp);
//		LogInfo(strMsg,FALSE,COLOR_RED);
//		return;
//	}
//	
//	strMsg.Format("从无纸化获取到的当前工序为:%s(%s)",m_strItemName,strTemp);
//	LogInfo(strMsg);
//
//	if (!m_ctrXMLCtr.CloseXMLFile())
//	{
//		strMsg = "无纸化关闭失败,请通知相关工程师!";
//		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
//		return;
//	}
//	nRand = rand();
//	//重新获取模板
//	CString strTestItem;
//	if (m_strItemName=="初测")
//	{
//		strTestItem = "Initial";
//		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&pretest=1&randno=%d",m_strSN,nRand);
//	}
//	else if (m_strItemName=="终测")
//	{
//		strTestItem = "Final";
//		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&test=1&randno=%d",m_strSN,nRand);
//	}		
//	//获得XML模板文件
//	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
//	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
//	{
//		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
//	}
//	else
//	{
//		strMsg = "模板下载错误!";
//		MessageBox(strMsg);
//		return;
//	}
//	CString strPN;
//	//PN信息
//	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
//	nNodeCount = 0;
//	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
//	if (nNodeCount == 0)
//	{
//		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
//		MessageBox(strMsg);
//		return;
//	}
//	m_ctrXMLCtr.GetNodeAttr("NodeValue", strPN, dwBuf[0]);
//	m_strGetPN = strPN;
//	//SPEC信息
//	CString strSpec;
//	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
//	nNodeCount = 0;
//	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SPEC", dwBuf, nNodeCount);
//	if (nNodeCount == 0)
//	{
//		strMsg = "XML文件结点不完整,没有找到无纸化SPEC信息!";
//		MessageBox(strMsg);
//		return;
//	}
//	m_ctrXMLCtr.GetNodeAttr("NodeValue", strSpec, dwBuf[0]);
//	m_strSpec = strSpec;
//	strMsg.Format("从无纸化获取到的Spec:%s,PN:%s",strSpec,strPN);
//	LogInfo(strMsg);
//	SetDlgItemText(IDC_EDIT_SPEC,strSpec);
//
//	m_strPN = m_strGetPN;
//	if(!GetPortInfo(m_strPN,m_strSpec,strTestItem))
//	{
//		return;
//	}
//	strMsg.Format("入光通道数为：%d,出光通道数为：%d",m_nSwitchComPortCount,m_nCHCount);
//	LogInfo(strMsg);
//	if (!m_strSelectRule.IsEmpty())
//	{
//		if (m_strItemName=="初测")
//		{
//			strMsg.Format("通道挑选规则：%s",m_strSelectRule);
//			LogInfo(strMsg);
//		}
//	}
//	//加载通道配置
//	m_cbChannel.ResetContent();
//	for (int nIndex=0;nIndex<=m_nCHCount;nIndex++)
//	{
//		strValue.Format("%d",nIndex);
//		m_cbChannel.AddString(strValue);
//	}
//	m_cbChannel.SetCurSel(0);
//	
//	SetDlgItemText(IDC_COMBO_COMPORT,"COM0");
//	m_cbComPort.SetCurSel(0);
//	UpdateWindow();
//
//	if (m_nWLCount>1)
//	{
//		//加载波长配置
//		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
//			
//		//加载WL波长
//		m_cbSetWL.ResetContent();
//		strValue.Format("%.0f",m_pdblWavelength);
//		m_cbSetWL.AddString(strValue);
//		for (int i=0;i<m_nWLCount;i++)
//		{
//		//	m_dblWL[i] = atof(strValue);
//			strValue.Format("%.2f",m_dblWL[i]);
//			m_cbSetWL.AddString(strValue);
//		}
//		m_cbSetWL.SetCurSel(0);
//
//		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_WDL_TEST)->EnableWindow(TRUE);
//	}
//
//	if (stricmp(m_strSpec,"Z4767")==0)
//	{
//		if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //77501363A2 for Z4671
//		{
//			m_strItemName="初测"; //按初测处理
//			GetDlgItem(IDC_STATIC_CHSELECT)->ShowWindow(TRUE);
//    		GetDlgItem(IDC_COMBO_TEST_CH)->ShowWindow(TRUE);
//		}
//	}
//	
//	/*
//	if (stricmp(m_strGetPN,"77501351A2")==0)
//	{
//		m_strPN = "1X12";
//		if (m_strSpec=="12829")
//		{
//			m_strPN = "8X1";
//			if (m_strItemName=="终测")
//			{
//				m_nCHCount=8;
//			}
//		}
//		if (m_strSpec=="12425"||m_strSpec=="10046")
//		{
//			m_strPN = "4X1";
//			if (m_strItemName=="终测")
//			{
//				m_nCHCount=4;
//			}
//		}
//		if (m_strSpec=="13235")
//		{
//			m_strPN = "2X1";
//			if (m_strItemName=="终测")
//			{
//				m_nCHCount=2;
//			}
//		}
//	}
//	else if (stricmp(m_strGetPN,"77501353A0")==0)
//	{
//		m_strPN = "2X16";
//		m_nSwitchComPortCount = 2;
//		m_cbComPort.SetCurSel(1);
//		if (m_strItemName=="终测")
//		{
//			m_nCHCount=16;
//		}
//		else if (m_strItemName=="初测")
//		{
//			m_nCHCount=16;
//		}
//	}
//	else if(stricmp(m_strGetPN,"77501354A2_L")==0)
//	{
//		m_strPN = "8X1";
//		if (m_strItemName=="终测")
//		{
//			m_nCHCount=8;
//		}
//	}
//	else if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //77501363A2 for Z4671
//	{
//		m_strPN = "8X1";
//		if (m_strItemName=="终测")
//		{
//			m_nCHCount=8;
//		}
//		else if (m_strItemName=="初测")  //2016-6-2，初测只测试8个通道
//		{
//			m_nCHCount=8;
//		}
//		m_strItemName="初测"; //按初测处理
//		GetDlgItem(IDC_STATIC_CHSELECT)->ShowWindow(TRUE);
//    	GetDlgItem(IDC_COMBO_TEST_CH)->ShowWindow(TRUE);
//	}
//	else if (stricmp(m_strGetPN,"77501355A2")==0)
//	{
//		m_strPN = "1X9";
//		if (m_strItemName=="终测")
//		{
//			m_nCHCount=9;
//		}
//	}
//	else if ((stricmp(m_strGetPN,"77501356A2")==0)||(stricmp(m_strGetPN,"UOFS102U00IFA01G")==0))
//	{
//		m_strPN = "2X1";
//		if (m_strItemName=="终测")
//		{
//			m_nCHCount=2;
//		}
//	}
//	else if (stricmp(m_strGetPN,"77501357A2")==0)
//	{
//		m_strPN = "9X1";
//		if (m_strItemName=="终测")
//		{
//			m_nCHCount=9;
//		}
//	}
//	else if (stricmp(m_strGetPN,"77501358A2")==0)
//	{
//		m_strPN = "4X1";
//		if (m_strItemName=="终测")
//		{
//			m_nCHCount=4;
//		}
//	}
//	else if(m_strGetPN=="UOFS104E00ZCC01G")
//	{
//		m_strPN = "4X1";
//		if (m_strItemName=="终测")
//		{
//			m_strPN = "4X1";
//			m_nCHCount=4;
//		}
//	}
//	else if (stricmp(m_strGetPN,"77501359A2")==0)
//	{
//		m_strPN = "1X19";
//		m_nCHCount = 19;
//		if (m_strSpec=="S0379")
//		{
//			m_strPN = "1X16";
//			if (m_strItemName=="终测")
//			{
//				m_nCHCount=16;
//			}
//		}
//	}
//	else if (stricmp(m_strGetPN,"77501360A11")==0)  //添加18X1开关
//	{
//		m_strPN = "18X1";
//		m_nCHCount = 18;
//		if (m_strSpec=="13998")
//		{
//			m_pdblWavelength = 1550;
//		}
//		else
//		{
//			m_pdblWavelength=1360;
//			GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
//			GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
//			GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
//			
//			//加载WL波长
//			m_cbSetWL.ResetContent();
//
//			strValue.Format("%.0f",m_pdblWavelength);
//			m_cbSetWL.AddString(strValue);
//			strWDLFile.Format("%s\\config\\%s\\WDLSET.INI",g_tszAppFolder,m_strPN);
//			GetPrivateProfileString("WDL","COUNT","error",strValue.GetBuffer(128),128,strWDLFile);
//			if (strValue=="error")
//			{
//				strMsg.Format("读取%s文件错误！",strWDLFile);
//				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
//				return;
//			}
//			m_nWLCount = atoi(strValue);
//
//			for (int i=0;i<m_nWLCount;i++)
//			{
//				strKey.Format("WL%d",i+1);
//				GetPrivateProfileString("WDL",strKey,"error",strValue.GetBuffer(128),128,strWDLFile);
//				if (strValue=="error")
//				{
//					strMsg.Format("读取%s文件错误！",strWDLFile);
//					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
//					return;
//				}
//				m_dblWL[i] = atof(strValue);
//				m_cbSetWL.AddString(strValue);
//			}
//			m_cbSetWL.SetCurSel(0);
//		}
//		
//	}
//	else if(m_strPN=="UOFS112U00LSR01G")
//	{
//		m_strPN = "1X12-2";
//		m_nCHCount = 12;
//		m_pdblWavelength=1550;
//		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_WDL_TEST)->EnableWindow(TRUE);
//		GetDlgItem(IDC_BUTTON_PDL_TEST)->EnableWindow(TRUE);
//		//加载WL波长
//		m_cbSetWL.ResetContent();
//
//		strValue.Format("%.0f",m_pdblWavelength);
//		m_cbSetWL.AddString(strValue);
//		strWDLFile.Format("%s\\config\\%s\\WDLSET.INI",g_tszAppFolder,m_strPN);
//		GetPrivateProfileString("WDL","COUNT","error",strValue.GetBuffer(128),128,strWDLFile);
//		if (strValue=="error")
//		{
//			strMsg.Format("读取%s文件错误！",strWDLFile);
//			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
//			return;
//		}
//		m_nWLCount = atoi(strValue);
//
//		for (int i=0;i<m_nWLCount;i++)
//		{
//			strKey.Format("WL%d",i+1);
//			GetPrivateProfileString("WDL",strKey,"error",strValue.GetBuffer(128),128,strWDLFile);
//			if (strValue=="error")
//			{
//				strMsg.Format("读取%s文件错误！",strWDLFile);
//				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
//				return;
//			}
//			m_dblWL[i] = atof(strValue);
//			m_cbSetWL.AddString(strValue);
//		}
//		m_cbSetWL.SetCurSel(0);
//
//	}
//	else if (stricmp(m_strGetPN,"UOFS108L00ATL01G")==0)  //add PN:UOFS108L00ATL01G  2016-7-28
//	{
//		m_strPN = "8X1-12829";
//		if (m_strItemName=="终测")
//		{
//			m_nCHCount=8;
//		}
//	}
//	else
//	{
//		strMsg.Format("PN%s不在该软件识别范围内，请确认！",m_strGetPN);
//		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
//		return;
//	}
//	*/
//	GetTempConfig();
//	SetDlgItemText(IDC_EDIT_PN,m_strPN);
//	UpdateWindow();
//
//	if (m_nSwitchComPortCount == 2)
//	{
//		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(TRUE);
//	}
//	else
//	{
//		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(FALSE);
//	}
//
//	if (!m_ctrXMLCtr.CloseXMLFile())
//	{
//		strMsg = "无纸化关闭失败,请通知相关工程师!";
//		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
//		return;
//	}
//
//	GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(TRUE);
//	if (m_strItemName=="终测")
//	{
//		GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(FALSE);
//	}
//	m_bGetSNInfo = TRUE;	
//	CString strXMLFile;
//	if (m_bIfSaveToServe)
//	{
//		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN);
//	}
//	else 
//	{
//		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);
//	}
//	CopyFile(strLocalFile,strXMLFile,FALSE); //COPY 模板
//	strMsg.Format("基本信息获取完毕！",FALSE,COLOR_BLUE);
//	LogInfo(strMsg);
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonUploadAtms() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CString strXMLFile;
	CString strErrMsg;

		CString strMsg;
	CString strURL;
	int     iRandNo=rand();
	DWORD   dwBuf[256];
	DWORD   dwTmptBuf[256];
	DWORD   dwWDMTemp;
	//CString strErrMsg;
	int     nNodeCount;
	CString strTemperature;
	CString strTemp;
	CString strMax;
	CString strMin;
	CString strProcess, strErrorMsg;
	CString strLocalFile;

	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&test=1",m_ModuleSN); //获取测试模板

	

	
	strXMLFile.Format("%s\\Data\\%s\\%s\\%s_XML.xml", m_strNetFile, m_strItemName, m_strPN, m_ModuleSN);
	
	CString strNetFilePath;
	CString		strTmptValue;
	CString     strmsg;
	HANDLE	hDirectory = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	winFindFileData;
	strNetFilePath.Format("%s\\Data\\%s\\%s\\%s", m_strNetFile, m_strItemName, m_strPN, m_ModuleSN);
	hDirectory = FindFirstFile(strNetFilePath, &winFindFileData);

	if (hDirectory == INVALID_HANDLE_VALUE)
	{
		if (!CreateDirectory(strNetFilePath, NULL))
		{
			strTmptValue.Format("不能创建data子目录%s", strNetFilePath);
			MessageBox(strTmptValue, "目录错误", MB_OK | MB_ICONERROR);
			return ;
		}
	}
	else
		FindClose(hDirectory);

	strXMLFile.Format("%s\\%s_XML.xml", strNetFilePath, m_ModuleSN);
	//strXMLFile = "\\\\zh-mfs-srv\\Public\\WLMTestProgram\\ZH_Ruijiang\\1X64Fusion\\data\\XML.xml";
	//	strXMLFile.Format("%s\\XML.xml", m_tszCaliDataFolder);
	if (!m_fusion.bDownLoadItemInfo(m_ModuleSN, "MEMS-SWITCH-Module测试", "", "", FALSE, strXMLFile))
	{
		return;
	}



	bIniXMLItem();

	OnButtonSelectChannel();

	if ( (stricmp(m_strSpec,"Z4839")==0) || (stricmp(m_strSpec,"14538")==0) )
	{
		
			UpdateATMData(VOL_DATA);
			//UpdateATMData(ISO_DATA);
			LogInfo("VOLOK");
			UpdateATMData(IL_LOW_DATA);
			UpdateATMData(IL_HIGH_DATA);
			UpdateATMData(IL_ROOM_DATA);
			LogInfo("ILOK");
			UpdateATMData(CT_DATA);
			UpdateATMData(TDL_DATA);
			LogInfo("TDLOK");
			UpdateATMData(RE_IL_DATA);
			LogInfo("REOK");
			UpdateATMData(RL_DATA);
			//UpdateATMData(DARK_DATA);
		
	}
	
	
	if (stricmp(m_strSpec,"Z4767")==0)
	{
		if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //初测选取1~8
		{
			UpdateATMData(VOL_DATA_5);
			UpdateATMData(VOL_DATA_8);
			UpdateATMData(REPEAT_ATTEN_5);
			UpdateATMData(REPEAT_ATTEN_8);
			UpdateATMData(WDL_DATA_5);
			UpdateATMData(WDL_DATA_8);
		}
	}
	

	//if (!SetFileByMSMQ(strXMLFile, strErrMsg))
	//{
	//	LogInfo("无纸化数据上传失败！",(BOOL)FALSE,COLOR_RED);
	//	LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
	//	return;
	//}
	if (!m_fusion.bUploadXMLData(strXMLFile, &strErrorMsg))
	{
		AfxMessageBox(strErrorMsg);
		return;
	}

	LogInfo("无纸化数据上传成功！");
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonWdl() 
{
	// TODO: Add your control notification handler code here
	CString strILFile;
	CString strWDLFile;
	CString strPath;
	CString strMsg;
	CString strToken;
	CString strTemp;
	CString strValue;
	double dblMax;
	double dblMin;
//	char    chReadBuf[256];
	int     nCH=0;
	int     nWL=0;
	FILE    *fp=NULL;
	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	else
	{
		strPath = g_tszAppFolder;
	}

	for (nWL=0;nWL<m_nWLCount;nWL++)
	{
		strILFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-%.0f.CSV",strPath,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
		
		for (nCH=0;nCH<m_nCHCount;nCH++)
		{
			if(!GetDataFromFile(strILFile,nCH+2,2,&m_dblWDL[nCH][nWL]))
			{
				return;
			}	
		}
	}
	//计算WDL
	double dblMaxS;
	double dblMinS;
	double dblMaxL;
	double dblMinL;
	strWDLFile.Format("%s\\data\\%s\\%s\\%s\\WDL_Room.CSV",strPath,m_strItemName,m_strPN,m_strSN);
    fp = fopen(strWDLFile,"wt");
	strValue.Format("CH");
	for (nWL=0;nWL<m_nWLCount;nWL++)
	{
		strTemp.Format(",%.0f",m_dblWL[nWL]);
		strValue = strValue + strTemp;
	}
	strValue = strValue + ",WDL_S,WDL_L,WDL\n";
	fprintf(fp,strValue);
	


	for (nCH=0;nCH<m_nCHCount;nCH++)
	{
		dblMaxS = 0;
		dblMinS = 99;
		dblMaxL = 0;
		dblMinL = 99;
		dblMax=0;
		dblMin=99;
		strValue.Format("%d",nCH+1);
		for (nWL=0;nWL<3;nWL++)
		{
			strTemp.Format(",%2f",m_dblWDL[nCH][nWL]);
			strValue = strValue + strTemp;
			if (m_dblWDL[nCH][nWL]>dblMaxS)
			{
				dblMaxS = m_dblWDL[nCH][nWL];
			}
			if (m_dblWDL[nCH][nWL]<dblMinS)
			{
				dblMinS = m_dblWDL[nCH][nWL];
			}
		}
		for (nWL=3;nWL<m_nWLCount;nWL++)
		{
			strTemp.Format(",%2f",m_dblWDL[nCH][nWL]);
			strValue = strValue + strTemp;
			if (m_dblWDL[nCH][nWL]>dblMaxL)
			{
				dblMaxL = m_dblWDL[nCH][nWL];
			}
			if (m_dblWDL[nCH][nWL]<dblMinL)
			{
				dblMinL = m_dblWDL[nCH][nWL];
			}
		}
		dblMax=dblMaxS;
		dblMin=dblMinS;
		if(dblMaxL>dblMax)
			dblMax=dblMaxL;
		if(dblMinL<dblMin)
		{
			dblMin=dblMinL;
		}
		ChannalMessage[nCH].dblWDL[0] = dblMaxS - dblMinS;
		strTemp.Format(",%.2f",ChannalMessage[nCH].dblWDL[0]);
		strValue = strValue + strTemp;
		ChannalMessage[nCH].dblWDL[1] = dblMaxL - dblMinL;
		strTemp.Format(",%.2f\n",ChannalMessage[nCH].dblWDL[1]);
		strValue = strValue + strTemp;
		ChannalMessage[nCH].dblTotalWDL=dblMax-dblMin;
		strTemp.Format(",%.2f\n",ChannalMessage[nCH].dblTotalWDL);
		strValue = strValue + strTemp;
    	fprintf(fp,strValue);
		strMsg.Format("通道%d的WDL为:%.2f dB,%.2f dB",nCH+1,ChannalMessage[nCH].dblWDL[0],ChannalMessage[nCH].dblWDL[1]);
		LogInfo(strMsg);
		
		
	}
	fclose(fp);
	UpdateATMData(WDL_DATA);
	LogInfo("计算WDL结束！");	
}

BOOL CMy126S_45V_Switch_AppDlg::JudgeFileWhetherExist(CString strPath)
{
	DWORD lPath = INVALID_FILE_SIZE;
	lPath = GetFileAttributes(strPath);
	
	if (lPath  == INVALID_FILE_SIZE) 
		return FALSE;	

	return TRUE;
}
//Row-行 Line-列,都从1开始
BOOL CMy126S_45V_Switch_AppDlg::GetDataFromFile(CString pszFileName, int nRow, int nLine,double*pdblData)
{
	//先将csv文件打开，并开始读取数据
	FILE* pFile = NULL;
	TCHAR tszMark[] = ",\r\n";
	CHAR* pszToken;
	CHAR pszLineData[1024];
	BYTE byIndex = 0;
	int nLineIndex=1;
    BOOL	bFunctionOK=FALSE;
	CString strTemp;
	ZeroMemory(pszLineData, sizeof(CHAR) * 1024);
	try
	{
		if(!JudgeFileWhetherExist(pszFileName))
		{
			return FALSE;
		}

		pFile = fopen(pszFileName, "r");
		if(pFile == NULL)
		{
			throw"打开文件失败";
		}
		else
		{
			strTemp.Format("打开文件%s成功",pszFileName);
		//	LogInfo(strTemp);
		}
		if((nRow<1)||(nLine<1))
		{
			throw"行标签和列标签必须大于0";
		}
		while(TRUE)
		{
			if(NULL == fgets(pszLineData, 1024, pFile))
			{
				throw"读取数据失败！";
			}
			if(nLineIndex<nRow)
			{
				nLineIndex++;
				continue;
			}
			else
			{
				pszToken = strtok(pszLineData, tszMark);
				if(pszToken == NULL)
				{
					throw"读取数据失败！";
				}
			
				for(int nIndex=1;nIndex<nLine;nIndex++)
				{
					pszToken = strtok(NULL,tszMark);
					if(pszToken == NULL)
					{
						throw"读取数据失败！";
					}
				}
				*pdblData=atof(pszToken);
				break;
			}
		}

		bFunctionOK= TRUE;

	}
	catch(TCHAR* ptszErrorMsg)
	{
		LogInfo(ptszErrorMsg);
	}
	catch(...)
	{
		LogInfo("其他错误！读取csv数据失败");
	}

	fclose(pFile);
	return bFunctionOK;
}

BOOL CMy126S_45V_Switch_AppDlg::GetDataFromCSV(int nType, int nChannel, double *pdblData)
{
	CString strCSVFile;
	CString strNetFile;
	CString strPath;
	int nIndex;
	int nIndexCH;
	int nWL = m_cbSetWL.GetCurSel()-1;
//	int nWL=-1;
	CString strValue;
	CString strTemperature = "Room";

	
	switch (nType)
	{
	case VOL_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nChannel);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nChannel);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nChannel);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nChannel);
		}

		if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}

		

		if(!GetDataFromFile(strPath,2,2,pdblData))
		{
			return FALSE;
		}

		if(!GetDataFromFile(strPath,2,3,pdblData+1))
		{
			return FALSE;
		}
		
		break;
		
	case IL_ROOM_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\IL_Room-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}
		if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}

			
		
		if(!GetDataFromFile(strPath,1+nChannel,2,pdblData))
		{
			return FALSE;
		}
		
		break;
	case IL_LOW_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\IL_Low-%.0f.CSV", m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\终测\\%s\\%s\\IL_Low-%.0f.CSV",m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\IL_Low.CSV", m_strNetFile,m_strPN,m_strSN);
		    	strNetFile.Format("%s\\data\\终测\\%s\\%s\\IL_Low.CSV",m_strNetFile,m_strPN,m_strSN);
			}			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\IL_Low-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\终测\\%s\\%s\\IL_Low-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}
		if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}
		
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			if(!GetDataFromFile(strPath,nChannel+1,2,pdblData))
			{
				return FALSE;
			}
		}
	
		break;
	case IL_HIGH_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\IL_High-%.0f.CSV", m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\终测\\%s\\%s\\IL_High-%.0f.CSV",m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\IL_High.CSV", m_strNetFile,m_strPN,m_strSN);
		    	strNetFile.Format("%s\\data\\终测\\%s\\%s\\IL_High.CSV",m_strNetFile,m_strPN,m_strSN);
			}			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\IL_High-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\终测\\%s\\%s\\IL_High-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}
			
		if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}

		if(!GetDataFromFile(strPath,nChannel+1,2,pdblData))
		{
			return FALSE;
		}		
		break;
	case TDL_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\TDL-%.0f.CSV", m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\终测\\%s\\%s\\TDL-%.0f.CSV",m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\TDL.CSV", m_strNetFile,m_strPN,m_strSN);
		    	strNetFile.Format("%s\\data\\终测\\%s\\%s\\TDL.CSV",m_strNetFile,m_strPN,m_strSN);
			}			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\TDL-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\终测\\%s\\%s\\TDL-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}

		if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}


		if(!GetDataFromFile(strPath,nChannel+1,2,pdblData))
		{
			return FALSE;
		}	
		break;
	case CT_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);//,m_dblWL[nWL]
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\CT_Roomf.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\CT_Room-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);
		}

		if (m_nCHCount==19)
		{
			strValue.Format("CH,CT1,CT2,CT3,CT4,CT5,CT6,CT7,CT8,CT9,CT10,CT11,CT12,CT13,CT14,CT15,CT16,CT17,CT18,CT19\n");
		}
		else if (m_nCHCount==16)
		{
			strValue.Format("CH,CT1,CT2,CT3,CT4,CT5,CT6,CT7,CT8,CT9,CT10,CT11,CT12,CT13,CT14,CT15,CT16\n");
		}
		else
		{
			strValue.Format("CH,CT1,CT2,CT3,CT4,CT5,CT6,CT7,CT8,CT9,CT10,CT11,CT12\n");
		}
		
		if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}
		
	
		for( nIndexCH=0;nIndexCH<m_nCHCount;nIndexCH++)
		{
			if(!GetDataFromFile(strPath,nChannel+1,nIndexCH+2,&pdblData[nIndexCH]))
			{
				return FALSE;
			}
		}	
		
		break;
	case DIRECTIVITY_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s-%.0f.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature,m_dblWL[nWL]);
		    	strNetFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s-%.0f.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature);
			    strNetFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s-COM1.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature);
			strNetFile.Format("%s\\data\\%s\\%s\\%s\\Directivity_CH%d_%s-COM1.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN,nChannel+1,strTemperature);
		}
	
		if(!GetDataFromFile(strCSVFile,2,2,pdblData))
		{
			return FALSE;
		}
					
		break;

	case DARK_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\DarkIL-%.0f.CSV", m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
		        strNetFile.Format("%s\\data\\终测\\%s\\%s\\DarkIL-%.0f.CSV",m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\DarkIL.CSV", m_strNetFile,m_strPN,m_strSN);
		        strNetFile.Format("%s\\data\\终测\\%s\\%s\\DarkIL.CSV",m_strNetFile,m_strPN,m_strSN);
			}
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\DarkIL-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\终测\\%s\\%s\\DarkIL-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}	
	     
		if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}
		
		if(!GetDataFromFile(strPath,nChannel+1,2,pdblData))
		{
			return FALSE;
		}
		
		break;	

	case RE_IL_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\RepeatIL-%.0f.CSV", m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
		        strNetFile.Format("%s\\data\\终测\\%s\\%s\\RepeatIL-%.0f.CSV",m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\RepeatIL.CSV", m_strNetFile,m_strPN,m_strSN);
		        strNetFile.Format("%s\\data\\终测\\%s\\%s\\RepeatIL.CSV",m_strNetFile,m_strPN,m_strSN);
			}

		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\RepeatIL-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\终测\\%s\\%s\\RepeatIL-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}

        if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}
		
	
		if(!GetDataFromFile(strPath,nChannel+1,2,pdblData))
		{
			return FALSE;
		}
		
		break;

	case ISO_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\ISO.CSV", m_strNetFile,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\终测\\%s\\%s\\ISO.CSV",m_strNetFile,m_strPN,m_strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\ISO-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
		    strNetFile.Format("%s\\data\\终测\\%s\\%s\\ISO-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}	
		 if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}
		
		if(!GetDataFromFile(strPath,nChannel+1,2,pdblData))
		{
			return FALSE;
		}
		
		break;
	case RL_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			if (nWL>=0)
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\RL-%.0f.CSV", m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
			    strNetFile.Format("%s\\data\\终测\\%s\\%s\\RL-%.0f.CSV",m_strNetFile,m_strPN,m_strSN,m_dblWL[nWL]);
			}
			else
			{
				strCSVFile.Format("%s\\data\\终测\\%s\\%s\\RL.CSV", m_strNetFile,m_strPN,m_strSN);
			    strNetFile.Format("%s\\data\\终测\\%s\\%s\\RL.CSV",m_strNetFile,m_strPN,m_strSN);

			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\RL-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\终测\\%s\\%s\\RL-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}			
         if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}
		
		if(!GetDataFromFile(strPath,nChannel+1,2,pdblData))
		{
			return FALSE;
		}
		
		break;
	case SWITCH_TIME_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\SwitchTime.CSV", m_strNetFile,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\终测\\%s\\%s\\SwitchTime.CSV",m_strNetFile,m_strPN,m_strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\SwitchTime-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\终测\\%s\\%s\\SwitchTime-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}	
		 if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}
		if(!GetDataFromFile(strPath,2,2,pdblData))
		{
			return FALSE;
		}
		break;	
	case SELECT_CHANNEL_DATA:
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\SelectCh.CSV", m_strNetFile,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\终测\\%s\\%s\\SelectCh.CSV",m_strNetFile,m_strPN,m_strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\终测\\%s\\%s\\SelectCh-COM1.CSV", m_strNetFile,m_strPN,m_strSN);
			strNetFile.Format("%s\\data\\终测\\%s\\%s\\SelectCh-COM1.CSV",m_strNetFile,m_strPN,m_strSN);
		}
		 if (m_bIfSaveToServe)
		{
			strPath = strNetFile;
		}
		else
		{
			strPath = strCSVFile;
		}
		if(!GetDataFromFile(strPath,nChannel+1,2,pdblData))
		{
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SetTLSWavelength(double dblWL)
{

	if(dblWL<=1360)
	{
		if(m_bShort!=1)
		{
			MessageBox("请手动切换到短波开关！");
		}
		if(S_FALSE==g_pTLS->SetTLSWavelength(1,dblWL))
		{
			return FALSE;
		}
		m_bShort=1;
	}
	else
	{
		if(m_bShort!=0)
		{
			MessageBox("请手动切换到长波开关！");
		}
		if(S_FALSE==g_pTLS->SetTLSWavelength(0,dblWL))
		{
			return FALSE;
		}
		m_bShort=0;
	}
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnSelchangeComboSetWl() 
{
	// TODO: Add your control notification handler code here
}

void CMy126S_45V_Switch_AppDlg::OnCloseupComboSetWl() 
{
	// TODO: Add your control notification handler code here
	int nWL = m_cbSetWL.GetCurSel()-1;
	CString strTemp;

	if(nWL>=0)
	{
		if(g_pTLS==NULL)
		{
			LogInfo("没有连接激光器",FALSE);
			return;
		}
		if(!SetTLSWavelength(m_dblWL[nWL]))
		{
			MessageBox("设置波长失败！","错误");
		}
	}
	else
	{
		if(!SetTLSWavelength(m_pdblWavelength))
		{
			MessageBox("设置波长失败！","错误");
		}
	}

//	for(int nIndex=0; nIndex<m_nCHCount;nIndex++)
//	{
//		if (nWL>=0)
//		{
//			if(!SetPWMWL(m_dblWL[nWL],nIndex))
//			{
//				LogInfo("设置功率计波长失败！");
//				return;
//			}
//			strTemp.Format("设置通道%d波长为%.3f成功",nIndex+1,m_dblWL[nWL]);
//			LogInfo(strTemp);
//		}
//		else
//		{
//			if(!SetPWMWL(m_pdblWavelength,nIndex))
//			{
//				LogInfo("设置功率计波长失败！");
//				return;
//			}
//			strTemp.Format("设置通道%d波长为%.3f成功",nIndex+1,m_pdblWavelength);
//			LogInfo(strTemp);
//		}
//	}

}

void CMy126S_45V_Switch_AppDlg::OnButtonDirection() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	double dblDataArray[32][32]={0};
	CString strRawData;
	CString strSection;
	CString strKey;
	CString strTemp1;
	CString strMsg;
	double dblPref;
	double dblPower;
	int iSel;
	CString strFileName;
	CHAR chTemp[MAX_PATH]={0};
	try
	{
		int nWL = m_cbSetWL.GetCurSel()-1;
		if (nWL>=0)
		{
			SetPWMWL(m_dblWL[nWL]);
			m_bChange=TRUE;
		}
		else
		{
			SetPWMWL(m_pdblWavelength);
			m_bChange=FALSE;
		}

		strRawData.Format("%s\\data\\%s\\%s\\%s\\DirectoryRaw.ini",g_tszAppFolder,m_strItemName,m_strPN,m_strSN);
		//开始测试方向性
		for(int nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			ZeroMemory(dblDataArray,sizeof(dblDataArray));
			if(m_bTestStop)
			{
				throw"人为终止";
			}

			strTemp.Format("请将通道%d接入光，将COM端接通道%d的功率计，然后点YES，如果要跳过该通道请点NO",nIndex+1,nIndex+1);

			iSel=MessageBox((LPCSTR)strTemp,"提示！",MB_YESNO|MB_ICONQUESTION);

		    if(iSel==IDYES)
			{
				for(int nChannal=0;nChannal<m_nCHCount;nChannal++)
				{
					strSection.Format("COM%02d_CH%02d",nIndex+1,nChannal+1);
					//将开关切过去
					if(!SWToChannel(nChannal+1))
					{
						strMsg.Format("切换到通道%d错误",nChannal+1);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						return;
					}   
					Sleep(800);
					strTemp.Format("CH%d",nChannal+1);
					for(int i = 0; i < m_nCHCount; i++)
					{
						//调用归零数据
						dblPref = m_dblReferencePower[i];
						dblPower = ReadPWMPower(i);
						dblDataArray[nChannal][i] = TestIL(dblPref,dblPower);
						strTemp1.Format("  %.2f",dblDataArray[nChannal][i]);
						strTemp = strTemp+strTemp1;
						strKey.Format("IL%02d",i+1);
						strMsg.Format("%.2f",dblDataArray[nChannal][i]);
						WritePrivateProfileString(strSection,strKey,strMsg,strRawData);
					}
					LogInfo(strTemp);
				}
			}
		}
		//存储数据
		for(int nChannel=0;nChannel<m_nCHCount;nChannel++)//开关切换的通道
		{
			for(int nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				strSection.Format("COM%02d_CH%02d",nIndex+1,nChannel+1);
				for(int i=0;i<m_nCHCount;i++)
				{
					strKey.Format("IL%02d",i+1);
					ZeroMemory(chTemp,sizeof(chTemp));
					GetPrivateProfileString(strSection,strKey,"ERROR",chTemp,MAX_PATH,strRawData);
					strTemp=chTemp;
					if(strTemp=="ERROR")
					{
						strTemp.Format("CH%02d通道方向性数据不完整！");
						throw (char*)(LPCSTR)strTemp;
					}
					else
					{
						dblDir[nIndex][i]=atof(strTemp);
					}
				}
			}
			///各开关切换端口保存到相同的文件中
			if(!SaveDataToCSV(DIRECTION_DATA,nChannel+1))
			{
				throw"保存方向性数据失败！";
			}	
		}

	}
	catch(TCHAR* ptszErrorMsg)
	{
		MessageBox(ptszErrorMsg);
		return;
	}
	catch(...)
	{
		MessageBox("其他错误！扫描失败");
		return;
	}
    LogInfo("方向性测试完成！");
}

void CMy126S_45V_Switch_AppDlg::OnButtonSelectChannel() 
{
	int nChNume[31]={0};
	OnButtonCheckData();

	/*if(m_strItemName == "初测")
	{
		//
		if(stricmp(m_strSelectRule,"2X1") == 0)
		{
			if(!SelectChannel(2,m_nCHCount,nChNume))
			{
				LogInfo("挑选2X1通道失败！",FALSE,COLOR_RED);
			}
		}
		else if(stricmp(m_strSelectRule,"4X1") == 0)
		{
			if(!SelectChannel(4,m_nCHCount,nChNume))
			{
				LogInfo("挑选4X1通道失败！",FALSE,COLOR_RED);
			}
		}
		else if(stricmp(m_strSelectRule,"8X1") == 0)
		{
			if(!SelectChannel(8,m_nCHCount,nChNume))
			{
				LogInfo("挑选8X1通道失败！",FALSE,COLOR_RED);
			}

		}
		else if(stricmp(m_strSelectRule,"9X1") == 0)
		{
			if(!SelectChannel(9,m_nCHCount,nChNume))
			{
				LogInfo("挑选9X1通道失败！",FALSE,COLOR_RED);
			}
		}
		else if(stricmp(m_strSelectRule,"1X9") == 0)
		{
			if(!SelectChannel(9,m_nCHCount,nChNume))
			{
				LogInfo("挑选1X9通道失败！",FALSE,COLOR_RED);
			}
		}
		else if(stricmp(m_strSelectRule,"1X16") == 0)
		{
			if(!SelectChannel(16,m_nCHCount,nChNume))
			{
				LogInfo("挑选1X16通道失败！",FALSE,COLOR_RED);
			}
		}
		else 
		{
//			LogInfo("该PN不需要自动挑选通道！",FALSE,COLOR_RED);
		}
	}*/
}

BOOL CMy126S_45V_Switch_AppDlg::SelectChannel(int nTargetCh,int nTotalCh,int *nChNume)
{
	CString strCHSetINI;
	CString strValue;
	CString strMsg;
	CString strKey;
	CString strSection;
	CStdioFile stdFile;
	int     nCHSelect[32];
	int     nCH = 0;
	int     nSwitchIndex=0;
	int     nSumCH = 0;
	double  dbMaxCT=-99;
	int     nTemp;
	int     i=0;
	int     j=0;
	int   nCount1=0;
	int   nCount2=0;
	BOOL  bFlag;
	int   m_nCHSelect[32]={-1};
	double m_dbTDL[32]={0};
	CString strTemp;
	CString strCSVFile;
	CString strNewFile;
	int nGetNum[32]={0};
	FILE *fp = NULL;
	//所有参数是否合格
	bFlag  = FALSE;
	nSumCH = 0;

	UpdateData();
	FindMyDirectory(m_strItemName,m_strPN,m_strSN);
	strCSVFile.Format("%s\\data\\%s\\%s\\%s\\SelectCh.CSV", m_strNetFile,m_strItemName,m_strPN,m_strSN);
	strNewFile.Format("%s\\data\\%s\\%s\\%s\\SelectCh.CSV",m_strNetFile,m_strItemName,m_strPN,m_strSN);

	if (m_strItemName=="初测") //2016-6-2
	{
		if (stricmp(m_strSpec,"Z4767")==0)
		{
			if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //初测选取1~8
			{
				strTemp.Format("挑选通道成功。选出的通道为:");
				LogInfo(strTemp,(BOOL)FALSE,COLOR_BLUE);
				//将结果保存在文件中
				fp = fopen(strCSVFile,"wt");
				strValue.Format("Num,     CH,     TDL\n");
				LogInfo(strValue,(BOOL)FALSE,COLOR_BLUE);
				fprintf(fp,strValue);
		
				for(int nIndex=0;nIndex<8;nIndex++)
				{
					nGetNum[nIndex] = nIndex;
					/*
					if (m_cbTestCH.GetCurSel()==1)
					{
						if (nIndex>=4) 
						{
							nGetNum[nIndex] = nIndex+4;
						}
					}
					*/

					m_dbTDL[nIndex] = ChannalMessage[nIndex].dblTDL;
					ChannalMessage[0].nChannel[nIndex]=nGetNum[nIndex]+1;
					strValue.Format("%d,    %d,    %.2f\n",nIndex+1,nGetNum[nIndex]+1,m_dbTDL[nGetNum[nIndex]]);
					LogInfo(strValue,(BOOL)FALSE,COLOR_BLUE);
					fprintf(fp,strValue);

				}
				fclose(fp);

				CopyFile(strCSVFile,strNewFile,FALSE);
				//UpdateATMData(SELECT_CHANNEL_DATA,8);
				return TRUE;
				//写入无纸化
				//UpdateATMData(SELECT_CHANNEL_DATA,8);
			}
			else
			{
			}
		}

		
	}
	

	for (nCH=0;nCH<nTotalCh;nCH++)
	{
		if(m_bDarkILPass[nCH]&&m_bRoomILPass[nCH]&&m_bLowILPass[nCH]&&m_bHighILPass[nCH]
			&&m_bRLPass[nCH]&&m_bCTPass[nCH]&&m_bTDLPass[nCH]&&m_bRepeatILPass[nCH]) //判断哪些通道是合格的
		{
			nCHSelect[nSumCH]=nCH;  //按通道顺序选取合格通道
			nSumCH++;
		}
	
		m_dbTDL[nCH] = ChannalMessage[nCH].dblTDL;
	}
		
	//按照每个通道TDL最小值从小到大排序
	for (i=0;i<nSumCH;i++)
	{
		for (j=(nSumCH-1);j>i;j--)
		{
			if (m_dbTDL[nCHSelect[j]]<m_dbTDL[nCHSelect[j-1]])
			{
				nTemp = nCHSelect[j-1];
				nCHSelect[j-1] = nCHSelect[j];
				nCHSelect[j] = nTemp;
			}
		}
	}

	if(nSumCH<nTargetCh)
	{
		strTemp.Format("所有参数都合格的通道个数为%d,不满足%d的要求",nSumCH,nTargetCh);
		MessageBox(strTemp);
	}
	else
	{
		if(!ChoseLine(nCHSelect,nSumCH, nTargetCh,nGetNum))
		{
			for(int nIndex=0;nIndex<nTargetCh;nIndex++)
			{
				ChannalMessage[0].nChannel[nIndex]=-1;
			}
			UpdateATMData(SELECT_CHANNEL_DATA,nTargetCh);
			LogInfo("选通道失败！",(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		else
		{
			strTemp.Format("挑选通道成功。选出的通道为:");
			LogInfo(strTemp,(BOOL)FALSE,COLOR_BLUE);
			//将结果保存在文件中
			fp = fopen(strCSVFile,"wt");
			strValue.Format("Num,     CH,     TDL\n");
			LogInfo(strValue,(BOOL)FALSE,COLOR_BLUE);
			fprintf(fp,strValue);
	
			if(nTargetCh==1)
				nTargetCh=9;
			for(int nIndex=0;nIndex<nTargetCh;nIndex++)
			{
				ChannalMessage[0].nChannel[nIndex]=nGetNum[nIndex]+1;
				strValue.Format("%d,    %d,    %.2f\n",nIndex+1,nGetNum[nIndex]+1,m_dbTDL[nGetNum[nIndex]]);
				LogInfo(strValue,(BOOL)FALSE,COLOR_BLUE);
				fprintf(fp,strValue);

			}
			fclose(fp);

			CopyFile(strCSVFile,strNewFile,FALSE);
			//写入无纸化
			UpdateATMData(SELECT_CHANNEL_DATA,nTargetCh);
		}
	}

	return TRUE;
}

//挑选同排
BOOL CMy126S_45V_Switch_AppDlg::ChoseLine(int *nChNum, int nTotalCount, int nGetCount, int *nGetNum)
{
	int Row[8][5]={0};//最多8排
	int nExp[3]={0};
	BOOL bOK=FALSE;

	//先分为排。Row[n][0]为这一排的有效个数
	for(int nIndex=0;nIndex<nTotalCount;nIndex++)
	{
		Row[nChNum[nIndex]/4][0]++;
		Row[nChNum[nIndex]/4][Row[nChNum[nIndex]/4][0]]=nChNum[nIndex];
	}

	//如果需要挑选的通道数为2
	if(nGetCount==2)
	{
		//先看第一个备选通道所在的排是否有足够的通道数
		for(int nLine=0; nLine<8;nLine++)
		{
			if(Row[nChNum[0]/4][nLine]>1)
			{
				//如果有，那么赋值
				nGetNum[0]=Row[nChNum[nLine]/4][1];
				nGetNum[1]=Row[nChNum[nLine]/4][2];
				bOK=TRUE;
				break;
			}
		}
		ReShapeLine(nGetNum,2);
	}
	else if(nGetCount==4)
	{
		//先判断nChNum[0]是否在第二排
		int iTemp=0;
		while(iTemp<nTotalCount)
		{
			if((nChNum[iTemp]/4==0)&&(Row[0][0]>3))
			{
				memcpy(nGetNum,Row[0]+1,4*sizeof(int));
				bOK=TRUE;
				break;
			}
			else if((nChNum[iTemp]/4==2)&&(Row[2][0]>3))
			{	
				memcpy(nGetNum,Row[2]+1,4*sizeof(int));
				bOK=TRUE;
				break;
			}
			else
			{
				iTemp++;
			}
		}
		//优先选1~4，9~12
		if((!bOK)&&(Row[1][0]>3))
		{
			memcpy(nGetNum,Row[1]+1,4*sizeof(int));
			bOK=TRUE;
		}
		ReShapeLine(nGetNum,4);
	}
	else if(nGetCount==8)
	{
		if(Row[0][0]==4)
		{
			memcpy(nGetNum,Row[0]+1,sizeof(int)*4);
			if(Row[2][0]==4)
			{
				memcpy(nGetNum+4,Row[2]+1,sizeof(int)*4);
				bOK=TRUE;
			}
			else if(Row[1][0]==4)
			{
				memcpy(nGetNum+4,Row[1]+1,sizeof(int)*4);
				bOK=TRUE;
			}
		}

		ReShapeLine(nGetNum,8);
	}
	else if(nGetCount==1)//在9~12选一个
	{
		if((Row[0][0]==4)&&(Row[1][0]==4)&&(Row[2][0]>0))
		{
			memcpy(nGetNum,Row[0]+1,4*sizeof(int));
			memcpy(nGetNum+4,Row[1]+1,4*sizeof(int));
			memcpy(nGetNum+8,Row[2]+1,1*sizeof(int));
			bOK=TRUE;
		}
		ReShapeLine(nGetNum,9);
	}
	else if(nGetCount==16)
	{
		memcpy(nGetNum,nChNum,sizeof(int)*nTotalCount);
		ReShapeLine(nGetNum,nTotalCount);
		int iExp=0;
		//19个通道，先排除8,13通道，再排除除8,13，通道之外TDL最差的通道
		for(int nIndex=0;nIndex<nTotalCount;nIndex++)
		{
			if((nChNum[nIndex]!=7)&&(nChNum[nIndex]!=12))
			{
				nExp[2]=nChNum[nIndex];
			}
			else
			{
				nExp[iExp]=nChNum[nIndex];
				iExp++;
			}
		}
	
		if(nTotalCount==17)//需要排掉一个
		{
			if(iExp>0)
			{
				//排掉nExp[0]
				DeleteOne(nGetNum,nExp[iExp-1],17);
			}
			else
			{
				//排掉nChNum[nTotalCount-1]
				DeleteOne(nGetNum,nExp[2],17);
			}	
		}
		else if(nTotalCount==18)
		{
			if(iExp>1)
			{
				//排掉nExp[0],nExp[1]
				DeleteOne(nGetNum,nExp[0],18);
				DeleteOne(nGetNum,nExp[1],17);
			}
			else if(iExp>0)
			{
				//排掉nExp[1]
				DeleteOne(nGetNum,nExp[iExp-1],18);
				DeleteOne(nGetNum,nExp[2],17);
			}
		}
		else if(nTotalCount==19)
		{
			//排掉nExp[0],nExp[1],nExp[2]
			DeleteOne(nGetNum,nExp[0],19);
			DeleteOne(nGetNum,nExp[1],18);
			DeleteOne(nGetNum,nExp[2],17);
		}

		bOK=TRUE;
	}
	else if(nGetCount==9)
	{
		memcpy(nGetNum,nChNum,sizeof(int)*nTotalCount);
		ReShapeLine(nGetNum,nTotalCount);
		int iExp=0;
		//12个通道，先排除6,7通道，再排除除6,7，通道之外TDL最差的通道
		for(int nIndex=0;nIndex<nTotalCount;nIndex++)
		{
			if((nChNum[nIndex]!=5)&&(nChNum[nIndex]!=6))
			{
				nExp[2]=nChNum[nIndex];
			}
			else
			{
				nExp[iExp]=nChNum[nIndex];
				iExp++;
			}
		}
	
		if(nTotalCount==10)//需要排掉一个
		{
			if(iExp>0)
			{
				//排掉nExp[0]
				DeleteOne(nGetNum,nExp[iExp-1],10);
			}
			else
			{
				//排掉nChNum[nTotalCount-1]
				DeleteOne(nGetNum,nExp[2],10);
			}	
		}
		else if(nTotalCount==11)
		{
			if(iExp>1)
			{
				//排掉nExp[0],nExp[1]
				DeleteOne(nGetNum,nExp[0],11);
				DeleteOne(nGetNum,nExp[1],10);
			}
			else if(iExp>0)
			{
				//排掉nExp[1]
				DeleteOne(nGetNum,nExp[iExp-1],11);
				DeleteOne(nGetNum,nExp[2],10);
			}
		}
		else if(nTotalCount==12)
		{
			//排掉nExp[0],nExp[1],nExp[2]
			DeleteOne(nGetNum,nExp[0],12);
			DeleteOne(nGetNum,nExp[1],11);
			DeleteOne(nGetNum,nExp[2],10);
		}
		bOK=TRUE;
	}

	return bOK;
}
//排序
void CMy126S_45V_Switch_AppDlg::ReShapeLine(int *nNum, int nCount)
{
	int nTemp;
	for(int i=0;i<nCount-1;i++)
	{
		for(int j=0;j<nCount-1;j++)
		{
			if(nNum[j]>nNum[j+1])
			{
				nTemp=nNum[j];
				nNum[j]=nNum[j+1];
				nNum[j+1]=nTemp;
			}
		}
	}
}

void CMy126S_45V_Switch_AppDlg::DeleteOne(int *nNum, int nDeletNum, int nCount)
{
	int nTag=0;
	for(int nIndex=0;nIndex<nCount;nIndex++)
	{
		if(nNum[nIndex]==nDeletNum)
		{
			//后面的都开始错位
			nTag=nIndex;
			break;
		}
	}
	for(int nIndex=nTag;nIndex<nCount-1;nIndex++)
	{
		nNum[nIndex]=nNum[nIndex+1];
	}
}

void CMy126S_45V_Switch_AppDlg::OnButtonOpenFile() 
{
	// TODO: Add your control notification handler code here
	CString strDataFileDirectory;
	int nReturnCode = 0;
	if (m_bIfSaveToServe)
	{
		strDataFileDirectory.Format("%s\\Data\\%s\\%s\\%s", m_strNetFile,m_strItemName,m_strPN,m_strSN);
	}
	else 
	{
		strDataFileDirectory.Format("%s\\Data\\%s\\%s\\%s", m_strNetFile,m_strItemName,m_strPN,m_strSN);
	}
	HANDLE handle = ShellExecute(NULL,"open",NULL,NULL,strDataFileDirectory,SW_SHOW);
	if(handle<(HANDLE)32)
	{
		MessageBox("打开数据文件夹错误!", NULL, MB_OK);
	}
}

BOOL CMy126S_45V_Switch_AppDlg::GetVoltageDataFromATMS(char *chSN, int nChannelNum, int *pnX, int *pnY)
{
	//获取无纸化PN
	CString strFileName;
	CString strURL,strTemp;
	CString strLocalFile;
	CString strMsg;
	//获取配置信息
	CString strFile;
	CString strValue;
	CString strKey;
	int     nNodeCount;
	CString strErrMsg;
	DWORD   dwBuf[256];
	DWORD   dwWDMTempItem;
	DWORD   dwBufCH[256];
	DWORD   dwTmptBuf[256];
	DWORD   dwWDMTemp;
	CString strWDLFile;
	int     nRand = rand();
	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=B5319606&randno=41");	
	//获得XML模板文件
	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);

	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		MessageBox(strMsg);
		return FALSE;
	}
	//核对PN信息是否对应
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
 		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
 		return FALSE;
 	}
	dwWDMTemp = dwBuf[0];
	ZeroMemory(dwTmptBuf, 256 * sizeof(DWORD));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwTmptBuf, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	dwWDMTemp = dwTmptBuf[0];
	m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值

	//根据PN判断通道组合
	if (strTemp=="room") //常温
	{
		//电压
		ZeroMemory(dwBufCH,sizeof(dwBufCH));
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Voltage", dwBufCH, nNodeCount,dwWDMTemp);
		dwWDMTempItem = dwBufCH[0];

		for (int nPort=0;nPort<nChannelNum;nPort++)
		{
			strTemp="";
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
//			strKey.Format("CH%d_X",m_nCHConfig[nPort]+1);
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
			pnX[nPort] = atoi(strTemp)*5095/60000;
			
			strKey.Format("CH%d_Y",nPort+1);
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwWDMTempItem);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
	    	pnY[nPort] = atoi(strTemp)*MAX_DAC/MAX_VOLTAGE;
		}	
		//差损
	}
	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "XML关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::Find8dBAtten(int nPort,CString strTempt) 
{
	double  dblBasePower;
	double  dblAtten=0;
	int     nStep;
	int     nStartDiff;
	int     nSetVoltage;
	double  dblIL;
	CString strMsg;
	CString strDirFile;
	CString strKey;
	CString strValue;
	int     nDir=0;
	CString strDir;
	int     nCH=0;

	//1.get dir
	strKey.Format("CH%d",nPort+1);
	strDir.Format("%s\\%s-%s\\VOA.ini", m_strPNConfigPath,m_strSpec,m_strPN);
	GetPrivateProfileString("Dir",strKey,"ERROR",strValue.GetBuffer(128),128,strDir);
	if (strValue=="ERROR")
	{
		LogInfo("读取VOA.ini配置错误！");
		nDir = 0;	
	}
	nDir = atoi(strValue);
	if (nDir==0) 
	{
		LogInfo("不定标VOA");
		return TRUE;
	}
	GetPrivateProfileString("Start","Value","ERROR",strValue.GetBuffer(128),128,strDir);
	if (strValue=="ERROR")
	{
		LogInfo("读取VOA.ini配置错误！");
		//nDir = 0;	
		nStartDiff = 200;
	}
	else
	{
		nStartDiff = atoi(strValue);
	}
	//2.set 0 dB
	SETX(ScanPowerChannel[nPort].VoltageX);
	Sleep(200);
	SETY(ScanPowerChannel[nPort].VoltageY);
	Sleep(400);
	dblBasePower = m_dblReferencePower[nPort] - ReadPWMPower(nPort);
	strMsg.Format("X:%d,Y:%d,IL:%.2fdB",ScanPowerChannel[nPort].VoltageX,ScanPowerChannel[nPort].VoltageY,dblBasePower);
	LogInfo(strMsg);
	if (nDir==3) 
	{
		nStep = 30;
	//	nStartDiff = 1000;
	}
	else if (nDir==4)
	{
		nStep = -30;
		nStartDiff = -nStartDiff;
	}
	else
	{
	}
    
	CString strCSVFile;
    CString strNetPath;
	FILE    *fp=NULL;
	strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_VOAVoltage_%d.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
	strNetPath.Format("%s\\data\\%s\\%s\\%s\\%s_VOAVoltage_%d.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
    
	fp = fopen(strCSVFile,"wt");
	if (fp==NULL)
	{
		strMsg.Format("创建文件%s失败",strCSVFile);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	strMsg.Format("X,Y,Atten\n");
	fprintf(fp,strMsg);
	
	int nCount=0;
	BOOL b5dBFlag = FALSE;
	if (nPort>=8)
	{
		nCH = nPort-4;
	}
	else
	{
		nCH = nPort;
	}
	nSetVoltage = ScanPowerChannel[nPort].VoltageY + nStartDiff;
	while (1)
	{
		YieldToPeers();
		if (m_bTestStop)
		{
			fclose(fp);
			return FALSE;
		}

		SETY(nSetVoltage);
		Sleep(100);
		dblIL = m_dblReferencePower[nPort] - ReadPWMPower(nPort);
		strMsg.Format("X:%d,Y:%d,Atten:%.2fdB",ScanPowerChannel[nPort].VoltageX,nSetVoltage,dblIL-dblBasePower);
		LogInfo(strMsg);
		strMsg.Format("%d,%d,%.2f\n",ScanPowerChannel[nPort].VoltageX*4096/60000,nSetVoltage*4096/60000,dblIL-dblBasePower);
    	fprintf(fp,strMsg);
		if (!b5dBFlag)
		{
			if ((dblIL - dblBasePower)>=5)
			{
				m_n5dBAttenX[nCH] = ScanPowerChannel[nPort].VoltageX;
				m_n5dBAttenY[nCH] = nSetVoltage;
				b5dBFlag = TRUE;
			}
			
		}
		//if ((dblIL - dblBasePower)>=5)
		//{
		//	break;
		//}
		if ((dblIL - dblBasePower)>=8)
		{
			m_n8dBAttenX[nCH] = ScanPowerChannel[nPort].VoltageX;
			m_n8dBAttenY[nCH] = nSetVoltage;
			break;
		}
		nSetVoltage += nStep;
		nCount++;
		if (nCount>=200)
		{
			fclose(fp);
			return FALSE;
		}	
	}
	//
	fclose(fp);
	//
	strMsg.Format("通道%dVOA定标完毕！",nPort+1);
	LogInfo(strMsg);
	return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonWdl2() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	CString strTemp1;
	CString strChannel;
	CString strMsg;
	double  dblPref;
	double  dblPower;
	int nchannel;
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	CString strTempt;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	int    iSel=IDYES;
	int    nPort;
	BOOL   bFlag = TRUE;
	m_bTestStop = FALSE;

	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}

	//获取当前时间
	SYSTEMTIME st;
	COleDateTime    tCurTime;
	COleDateTimeSpan tDifHour; 
	int        nDifHour;
	GetLocalTime(&st);
	tCurTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	tDifHour = tCurTime-m_RefStartTime; //计算时间插值
	nDifHour = tDifHour.GetTotalHours();
	if (nDifHour>m_nRefTime)  //比较归零时间差异
	{
		strMsg.Format("当前归零时间为%d小时，已超过归零限制时间（%d小时），请重新归零！",nDifHour,m_nRefTime);
		MessageBox(strMsg);
		m_bHasRefForTest = FALSE;
		return;
	}
	UpdateData();
	FindMyDirectory(m_strItemName,m_strPN,m_strSN);
	m_ctrlListMsg.ResetContent();
	double dblTargetTempt;
	int    nTemptStatus; //设置温度状态
	double dblCurTempt; //当前温度
	int nTemp = m_ctrlComboxTemperature.GetCurSel();
	if (nTemp==0)
	{
		strTemp.Format("开始测试WDL常温测试！");
		LogInfo(strTemp,FALSE);
		dblTargetTempt = 26;
		nTemptStatus = 0;
		strTempt = "Room";
	}
	else if (nTemp==1)
	{
		strTemp.Format("开始测试WDL低温测试！");
		LogInfo(strTemp);
		dblTargetTempt = -5;
		nTemptStatus = 2;
		strTempt = "Low";
	}
	else 
	{
		strTemp.Format("开始测试WDL高温测试！");
		LogInfo(strTemp);
		dblTargetTempt = 70;
		if (stricmp(m_strSpec,"Z4767")==0)
		{
			dblTargetTempt = 85;
		}
		nTemptStatus = 1;
		strTempt = "High";
	}	
	dblTargetTempt = m_dblTemperature[nTemp];
	//判断当前温度是否达到要求；
	if (m_bConnectTemptControl)
	{
		int nIndex=0;
		while(TRUE)
		{
			dblCurTempt = GetCurrentTempt(); //获取当前冷热盘温度
			if (fabs(dblTargetTempt-dblCurTempt)>2)
			{
				strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
				LogInfo(strMsg,BOOL(FALSE),COLOR_BLACK);
				Sleep(1000);
			}
			else
			{
				break;
			}
			nIndex++;
			if(nIndex>200)
			{
				strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
				LogInfo(strMsg,BOOL(FALSE),COLOR_RED);
				SetTemptStatus(nTemptStatus);
				return;
			}
		}
	}

	CString strVOAFile;
	char    chRead[256];
	ZeroMemory(chRead,sizeof(chRead));
	FILE    *fp = NULL;
	CString strToken;
	int     nX;
	int     nY;
	int     n5dBX;
	int     n5dBY;
	int     n8dBX;
	int     n8dBY;
	CString strFile;
	CString strNetFile;
	int    iWL=0;
	double dblWDLIL[100];
	double dblMax;
	double dblMin;
	double dblWDL;
	CString strDirFile;
	CString strKey;
	CString strValue;
	int     nDir=0;
	CString strDir;
	CStdioFile stdVOAFile;
	LPCTSTR lpStr=NULL;
	double dblAtten;
//	CString strNetFile;

	if (!GetAttenPointFromATMS()) //优先调用无纸化数据
	{
		LogInfo("调用无纸化数据失败，开始调用本地数据");
		//调用本地文件
		for (nPort=0;nPort<8;nPort++)
		{
//			if (nPort>=4)
//			{
//				nchannel = nPort+4;
//			}
//			else
//			{
//				nchannel = nPort;
//			}
			nchannel = nPort;
			if (m_bIfSaveToServe) 
			{
				strVOAFile.Format("%s\\data\\%s\\%s\\%s\\%s_VOAVoltage_%d.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nchannel+1);
			}
			else
			{	
				strVOAFile.Format("%s\\data\\%s\\%s\\%s\\%s_VOAVoltage_%d.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nchannel+1);
			}
			if (!stdVOAFile.Open(strVOAFile,CFile::modeRead)) 
			{
				strMsg.Format("打开文件%s失败！",strVOAFile);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				return;
			}
			lpStr = stdVOAFile.ReadString(chRead,256);
			while (lpStr!=NULL)
			{
				strToken = strtok(chRead,",");
				nX = atoi(strToken);
				strToken = strtok(NULL,",");
				nY = atoi(strToken);
				strToken = strtok(NULL,",");
				dblAtten = atof(strToken);
				if (dblAtten>=5)
				{
					n5dBX = nX;
					n5dBY = nY;
					break;
				}
				lpStr = stdVOAFile.ReadString(chRead,256);
			}
			lpStr = stdVOAFile.ReadString(chRead,256);
			while (lpStr!=NULL)
			{
				strToken = strtok(chRead,",");
				nX = atoi(strToken);
				strToken = strtok(NULL,",");
				nY = atoi(strToken);
				strToken = strtok(NULL,",");
				dblAtten = atof(strToken);
				if (dblAtten>=8)
				{
					n8dBX = nX;
					n8dBY = nY;
					break;
				}
				lpStr = stdVOAFile.ReadString(chRead,256);
			}
			stdVOAFile.Close();
			
			//5dB Test
			n5dBX = n5dBX*MAX_VOLTAGE/MAX_DAC;
			n5dBY = n5dBY*MAX_VOLTAGE/MAX_DAC;
			n8dBX = n8dBX*MAX_VOLTAGE/MAX_DAC;
			n8dBY = n8dBY*MAX_VOLTAGE/MAX_DAC;
			m_n5dBAttenX[nPort] = n5dBX;
			m_n5dBAttenY[nPort] = n5dBY;
			m_n8dBAttenX[nPort] = n8dBX;
			m_n8dBAttenY[nPort] = n8dBY;
		}
	}
	for (nchannel = 0; nchannel < m_nCHCount; nchannel++)
	{
		if(m_cbChannel.GetCurSel()!=0)
		{
			if (m_cbChannel.GetCurSel() != (nchannel+1) )
			{
				continue;
			}
		}
		
//		if (nchannel>=4) 
//		{
//			nPort = nchannel+4;
//		}
//		else
//		{
//			nPort = nchannel;
//		}
		
		nPort = nchannel;
		YieldToPeers();
		if (m_bTestStop) 
		{
			LogInfo("测试终止！");
			return ;
		}
		//1.get dir
		strKey.Format("CH%d",nPort+1);
		strDir.Format("%s\\%s-%s\\VOA.ini", m_strPNConfigPath,m_strSpec,m_strPN);
		GetPrivateProfileString("Dir",strKey,"ERROR",strValue.GetBuffer(128),128,strDir);
		if (strValue=="ERROR")
		{
			LogInfo("读取VOA.ini配置错误！");
			nDir = 0;
		}
		nDir = atoi(strValue);
		if (nDir==0) 
		{
			strMsg.Format("通道%d不测试VOA",nPort+1);
			LogInfo(strMsg);
			continue;
		}
		YieldToPeers();
		if (m_bTestStop)
		{
			MessageBox("测试中止！");
			return;
		}
		
		strMsg.Format("设置5dB电压值为：%d,%d",m_n5dBAttenX[nchannel],m_n5dBAttenY[nchannel]);
		LogInfo(strMsg);

		SETX(m_n5dBAttenX[nchannel]);
		Sleep(100);
		SETY(m_n5dBAttenY[nchannel]);
		Sleep(100);

		//Get VOA Data
		strFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att5dB_%d-%s.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1,strTempt);
        strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att5dB_%d-%s.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1,strTempt);
		fp = fopen(strFile,"wt");
		if (fp==NULL)
		{
			strMsg.Format("创建文件%s失败！",strFile);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}
		dblMax = -1;
		dblMin = 100;
		strTemp.Format("WL,IL\n");
		fprintf(fp,strTemp);
		for (iWL=0;iWL<m_nWLCount;iWL++)
		{
			YieldToPeers();
			if (m_bTestStop) 
			{
				LogInfo("测试终止！");
				return ;
			}
			g_pTLS->SetTLSWavelength(0,m_dblWL[iWL]);
			Sleep(100);
			if(!SetPWMWL(m_dblWL[iWL],nchannel))
			{
				LogInfo("设置功率计波长失败！");
				return;
			}
			Sleep(600);
			//strTemp.Format("设置通道%d波长为%.3f成功",nchannel+1,m_dblWL[iWL]);
		    //LogInfo(strTemp);

			dblPref = m_dblWDLRef[nchannel][iWL];
	      	dblPower = ReadPWMPower(nchannel);
			dblWDLIL[iWL] = dblPref - dblPower;
			strTemp.Format("%2f,%.2f\n",m_dblWL[iWL],dblWDLIL[iWL]);
			fprintf(fp,strTemp);
			strMsg.Format("CH%d,WL:%.2fnm,IL:%.2fdB",nchannel+1,m_dblWL[iWL],dblWDLIL[iWL]);
			LogInfo(strMsg);
			if (dblWDLIL[iWL]>dblMax)
			{
				dblMax = dblWDLIL[iWL];
			}
			if (dblWDLIL[iWL]<dblMin) 
			{
				dblMin = dblWDLIL[iWL];
			}
		}
		//Cal WDL
		dblWDL = dblMax - dblMin;
		strTemp.Format("WDL,%.2f\n",dblWDL);
		fprintf(fp,strTemp);
		fclose(fp);
		m_pdb5dBWDL[nchannel] = dblWDL;
		if (!CheckParaPassOrFail(WDL_DATA_5,dblWDL))
		{
			strMsg.Format("通道%d,WDL:%.2fdB,不合格",nchannel+1,dblWDL);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("通道%d,WDL:%.2fdB",nchannel+1,dblWDL);
	    	LogInfo(strMsg);
		}
		
		if (m_bIfSaveToServe)
		{
			CopyFile(strFile,strNetFile,FALSE);
		}
		UpdateATMData(WDL_DATA_5,nchannel);

		//8dB Test
		strMsg.Format("设置8dB电压值为：%d,%d",m_n8dBAttenX[nchannel],m_n8dBAttenY[nchannel]);
		LogInfo(strMsg);

		SETX(m_n8dBAttenX[nchannel]);
		Sleep(100);
		SETY(m_n8dBAttenY[nchannel]);
		Sleep(100);

		//Get VOA Data
		strFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att8dB_%d-%s.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1,strTempt);
        strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att8dB_%d-%s.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1,strTempt);
		fp = fopen(strFile,"wt");
		if (fp==NULL)
		{
			strMsg.Format("创建文件%s失败！",strFile);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}
		dblMax = -1;
		dblMin = 100;
		strTemp.Format("WL,IL\n");
		fprintf(fp,strTemp);
		for (iWL=0;iWL<m_nWLCount;iWL++)
		{
			YieldToPeers();
			if (m_bTestStop) 
			{
				LogInfo("测试终止！");
				return ;
			}
			g_pTLS->SetTLSWavelength(0,m_dblWL[iWL]);
			Sleep(100);
			if(!SetPWMWL(m_dblWL[iWL],nchannel))
			{
				LogInfo("设置功率计波长失败！");
				return;
			}
			Sleep(600);
			//strTemp.Format("设置通道%d波长为%.3f成功",nchannel+1,m_dblWL[iWL]);
		    //LogInfo(strTemp);

			dblPref = m_dblWDLRef[nchannel][iWL];
	      	dblPower = ReadPWMPower(nchannel);
			dblWDLIL[iWL] = dblPref - dblPower;
			strTemp.Format("%2f,%.2f\n",m_dblWL[iWL],dblWDLIL[iWL]);
			fprintf(fp,strTemp);
			strMsg.Format("CH%d,WL:%.2fnm,IL:%.2fdB",nchannel+1,m_dblWL[iWL],dblWDLIL[iWL]);
			LogInfo(strMsg);
			if (dblWDLIL[iWL]>dblMax)
			{
				dblMax = dblWDLIL[iWL];
			}
			if (dblWDLIL[iWL]<dblMin) 
			{
				dblMin = dblWDLIL[iWL];
			}
		}
		//Cal WDL
		dblWDL = dblMax - dblMin;
		strTemp.Format("WDL,%.2f\n",dblWDL);
		fprintf(fp,strTemp);
		fclose(fp);
		
		m_pdb8dBWDL[nchannel] = dblWDL;
		if (!CheckParaPassOrFail(WDL_DATA_8,dblWDL))
		{
			strMsg.Format("通道%d,WDL:%.2fdB,不合格",nchannel+1,dblWDL);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("通道%d,WDL:%.2fdB",nchannel+1,dblWDL);
	    	LogInfo(strMsg);
		}
		
		if (m_bIfSaveToServe)
		{
			CopyFile(strFile,strNetFile,FALSE);
		}
		
    	UpdateATMData(WDL_DATA_8,nchannel);
	}
///	OnButtonCreateWdl();
	LogInfo("测试完毕！");
}

BOOL CMy126S_45V_Switch_AppDlg::SetITLAWL(double dblWL)
{
	CString strValue;
	char    chBuf[256];
	ZeroMemory(chBuf,sizeof(chBuf));	
	strValue.Format("SWL 1 %.2f\r",dblWL);
	memcpy(chBuf,strValue,strValue.GetLength());
	if(!m_opComITLA.WriteBuffer(chBuf,strValue.GetLength()))
	{
		return FALSE;
	}
	return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonWdlRepeat() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	double dbl5dB[8][20];
	double dbl8dB[8][20];
	double dblMax[2][8]={-100};
	double dblMin[2][8]={100};
	double dblPower;
	double dblRefPower;
//	int nTempt;
	CString strNetFile;
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	int nNum[8]={0};
	CString strMsg;
	CString strTempt;
//	CString strTemp;
	//将VOA设置到0，然后设置到5，再设置到0，再设置到8，反复20次，计算5和8的平均值

	int  nCHCount=8;
	for (int nPort=0;nPort<nCHCount;nPort++)
	{
		for(int nIndex=0;nIndex<2;nIndex++)
		{
			dblMax[nIndex][nPort]=-100;
			dblMin[nIndex][nPort]=100;
		}
	}
	m_bTestStop = FALSE;

	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}

	//获取当前时间
	SYSTEMTIME st;
	COleDateTime    tCurTime;
	COleDateTimeSpan tDifHour; 
	int        nDifHour;
	GetLocalTime(&st);
	tCurTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	tDifHour = tCurTime-m_RefStartTime; //计算时间插值
	nDifHour = tDifHour.GetTotalHours();
	if (nDifHour>m_nRefTime)  //比较归零时间差异
	{
		strMsg.Format("当前归零时间为%d小时，已超过归零限制时间（%d小时），请重新归零！",nDifHour,m_nRefTime);
		MessageBox(strMsg);
		m_bHasRefForTest = FALSE;
		return;
	}
	UpdateData();
	FindMyDirectory(m_strItemName,m_strPN,m_strSN);
	m_ctrlListMsg.ResetContent();
	double dblTargetTempt;
	int    nTemptStatus; //设置温度状态
	double dblCurTempt; //当前温度
	int    nTemp = m_ctrlComboxTemperature.GetCurSel();
	if (nTemp==0)
	{
		strTemp.Format("开始测试衰减重复性常温测试！");
		LogInfo(strTemp,FALSE);
		dblTargetTempt = 26;
		nTemptStatus = 0;
		strTempt = "Room";
	}
	else if (nTemp==1)
	{
		strTemp.Format("开始测试衰减重复性低温测试！");
		LogInfo(strTemp);
		dblTargetTempt = -5;
		nTemptStatus = 2;
		strTempt = "Low";
	}
	else 
	{
		strTemp.Format("开始测试衰减重复性高温测试！");
		LogInfo(strTemp);
		dblTargetTempt = 70;
		if (stricmp(m_strSpec,"Z4767")==0)
		{
			dblTargetTempt = 85;
		}
		nTemptStatus = 1;
		strTempt = "High";
	}
	dblTargetTempt = m_dblTemperature[nTemp];
	//判断当前温度是否达到要求；
	if (m_bConnectTemptControl)
	{
		int nIndex=0;
		while(TRUE)
		{
			dblCurTempt = GetCurrentTempt(); //获取当前冷热盘温度
			if (fabs(dblTargetTempt-dblCurTempt)>2)
			{
				strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
				LogInfo(strMsg,BOOL(FALSE),COLOR_BLACK);
				Sleep(1000);
			}
			else
			{
				break;
			}
			nIndex++;
			if(nIndex>200)
			{
				strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
				LogInfo(strMsg,BOOL(FALSE),COLOR_RED);
				SetTemptStatus(nTemptStatus);
				return;
			}
		}
	}
	if(!GetAttenPoint(strTempt))
	{
		return;
	}

	int nActualPort;
	int nActualTemp;
	while(TRUE)
	{
		YieldToPeers();
		if(m_bTestStop)
		{
			LogInfo("测试停止！",BOOL(FALSE),COLOR_RED);
			return;
		}
		int nPort=rand()%8;
		/*
		if (nPort>=4)
		{
			nActualPort = nPort+4;
		}
		else
		{
			nActualPort = nPort;
		}
		*/
		nActualPort = nPort;
	
		Sleep(100);
		/*
		if(!SWToChannel(nActualPort+1))
		{
			strMsg.Format("切换到通道%d错误",nActualPort+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}    
		*/
		dblRefPower = m_dblReferencePower[nActualPort];
	
		SETX(m_n5dBAttenX[nPort]);
		Sleep(100);
		SETY(m_n5dBAttenY[nPort]);
		Sleep(100);
		strMsg.Format("5dBPort%d,X:%d,Y:%d",nActualPort+1,m_n5dBAttenX[nPort],m_n5dBAttenY[nPort]);
		LogInfo(strMsg);

	    dblPower = ReadPWMPower(nActualPort);
		strMsg.Format("PM%d,Power:%.2fdBm",nActualPort+1,dblPower);
		LogInfo(strMsg);
		if (dblPower<-40) 
		{
			Sleep(100);
			dblPower = ReadPWMPower(nActualPort);
			strMsg.Format("PM%d,Power:%.2fdBm",nActualPort+1,dblPower);
			LogInfo(strMsg);
			return;
		}
			
		//获取通道的IL值
		dbl5dB[nPort][nNum[nPort]]=dblRefPower-dblPower;
		if(dbl5dB[nPort][nNum[nPort]]<dblMin[0][nPort])
			dblMin[0][nPort]=dbl5dB[nPort][nNum[nPort]];
		if(dbl5dB[nPort][nNum[nPort]]>dblMax[0][nPort])
			dblMax[0][nPort]=dbl5dB[nPort][nNum[nPort]];

		strTemp.Format("通道%d 5dB第%d个IL 为%.2f",nActualPort+1,nNum[nPort]+1,dbl5dB[nPort][nNum[nPort]]);
		LogInfo(strTemp);

		//int iTemp=rand()%9;
		int iTemp=rand()%8;
		/*
		if (iTemp>=4)
		{
			nActualTemp = iTemp+4;
		}
		else
		{
			nActualTemp = iTemp;
		}
		*/
		nActualTemp = iTemp;
			
		SETX(m_n8dBAttenX[iTemp]);
		Sleep(100);
		SETY(m_n8dBAttenY[iTemp]);
		Sleep(100);
		strMsg.Format("8dBPort%d,X:%d,Y:%d",nActualTemp+1,m_n8dBAttenX[iTemp],m_n8dBAttenY[iTemp]);
		LogInfo(strMsg);

		/*
		if(!SWToChannel(nActualTemp+1))
		{
			strMsg.Format("切换到通道%d错误",nActualTemp+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}  
		Sleep(100);
		if(!SWToChannel(nActualPort+1))
		{
			strMsg.Format("切换到通道%d错误",nActualPort+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}  
		*/
		SETX(m_n8dBAttenX[nPort]);
		Sleep(100);
		SETY(m_n8dBAttenY[nPort]);
		Sleep(100);
		strMsg.Format("8dBPort%d,X:%d,Y:%d",nActualPort+1,m_n8dBAttenX[nPort],m_n8dBAttenY[nPort]);
		LogInfo(strMsg);
		dblPower = ReadPWMPower(nActualPort);
		strMsg.Format("PM%d,Power:%.2fdBm",nActualPort+1,dblPower);
		LogInfo(strMsg);
		if (dblPower<-40) 
		{
			Sleep(100);
			dblPower = ReadPWMPower(nActualPort);
			strMsg.Format("PM%d,Power:%.2fdBm",nActualPort+1,dblPower);
			LogInfo(strMsg);
			return;
		}
		dbl8dB[nPort][nNum[nPort]]=dblRefPower-dblPower;	
		if(dbl8dB[nPort][nNum[nPort]]<dblMin[1][nPort])
			dblMin[1][nPort]=dbl8dB[nPort][nNum[nPort]];
		if(dbl8dB[nPort][nNum[nPort]]>dblMax[1][nPort])
			dblMax[1][nPort]=dbl8dB[nPort][nNum[nPort]];
		if(nNum[nPort]>15)//只要有一个到20就停止切换
			break;

		strTemp.Format("通道%d 8dB第%d个IL 为%.2f",nActualPort+1,nNum[nPort]+1,dbl8dB[nPort][nNum[nPort]]);
		LogInfo(strTemp);
		nNum[nPort]++;
	}

	//double dbl5dBRepeat[12];
	//double dbl8dBRepeat[12];
	for(int nPort=0;nPort<8;nPort++)
	{
		/*
		if (nPort>=4)
		{
			nActualPort = nPort+4;
		}
		else
		{
			nActualPort = nPort;
		}
		*/
		nActualPort = nPort;

	//	strTemp.Format("Port%d 5dB重复性为%.2f",nActualPort+1,dblMax[0][nPort]-dblMin[0][nPort]);
	//	LogInfo(strTemp);
		m_pdb5dBRepeatAtten[nPort]=dblMax[0][nPort]-dblMin[0][nPort];

		if (!CheckParaPassOrFail(ATTEN_REPEAT,m_pdb5dBRepeatAtten[nPort]))
		{
			strMsg.Format("通道%d,5dB衰减重复性:%.2fdB,不合格",nActualPort+1,m_pdb5dBRepeatAtten[nPort]);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("通道%d,5dB衰减重复性:%.2fdB",nActualPort+1,m_pdb5dBRepeatAtten[nPort]);
	    	LogInfo(strMsg);
		}
		
		//strTemp.Format("Port%d 8dB重复性为%.2f",nActualPort+1,dblMax[1][nPort]-dblMin[1][nPort]);
		//LogInfo(strTemp);
		m_pdb8dBRepeatAtten[nPort]=dblMax[1][nPort]-dblMin[1][nPort];
		if (!CheckParaPassOrFail(ATTEN_REPEAT,m_pdb8dBRepeatAtten[nPort]))
		{
			strMsg.Format("通道%d,8dB衰减重复性:%.2fdB,不合格",nActualPort+1,m_pdb8dBRepeatAtten[nPort]);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("通道%d,8dB衰减重复性:%.2fdB",nActualPort+1,m_pdb8dBRepeatAtten[nPort]);
	    	LogInfo(strMsg);
		}
	
		strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s_VOA%d_Repeat_%s.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nActualPort+1,strTempt);
		strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_VOA%d_Repeat_%s.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nActualPort+1,strTempt);
	
		if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
		{
			strMsg.Format("创建数据文件%s失败！",strCSVFile);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}
		strContent.Format("Index,5dBIL,8dBIL\n");
		stdCSVFile.WriteString(strContent);
		double dbl5dBMax=0;
		double dbl8dBMax=0;
		for(int nIndex=0;nIndex<nNum[nPort];nIndex++)
		{
			strContent.Format("%d,%.2f,%.2f\n",nIndex+1,dbl5dB[nPort][nIndex],dbl8dB[nPort][nIndex]);
			if (fabs(dbl5dB[nPort][nIndex])>fabs(dbl5dBMax))
			{
				dbl5dBMax = dbl5dB[nPort][nIndex];
			}
			if (fabs(dbl8dB[nPort][nIndex])>fabs(dbl8dBMax))
			{
				dbl8dBMax = dbl8dB[nPort][nIndex];
			}
			stdCSVFile.WriteString(strContent);
		}
		//m_pdb5dBRepeatAtten[nPort] = dbl5dBMax;
		//m_pdb8dBRepeatAtten[nPort] = dbl8dBMax;
		stdCSVFile.Close();
		if (m_bIfSaveToServe)
		{
			CopyFile(strCSVFile,strNetFile,FALSE);
		}
	} 
	
	strNetFile.Format("%s\\data\\%s\\%s\\%s\\%s_ATT_Repeat_VOA_%s.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,strTempt);
	strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ATT_Repeat_VOA_%s.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,strTempt);
    FILE *fp=NULL;
	fp=fopen(strCSVFile,"wt");
	if (fp==NULL)
	{
		strMsg.Format("创建文件%s失败！",strCSVFile);
		LogInfo(strMsg);
		return;
	}
	strContent.Format("CH,5dB_Repeat,8dB_Repeat\n");
	fprintf(fp,strContent);
	for (int nCH=0;nCH<8;nCH++)
	{
		strContent.Format("%d,%.2f,%.2f\n",nCH+1,m_pdb5dBRepeatAtten[nCH],m_pdb8dBRepeatAtten[nCH]);
    	fprintf(fp,strContent);
	}
	fclose(fp);
	if (m_bIfSaveToServe)
	{
		CopyFile(strCSVFile,strNetFile,FALSE);
	}
	Sleep(100);
	UpdateATMData(REPEAT_ATTEN_5);
	UpdateATMData(REPEAT_ATTEN_8);
	LogInfo("测试完毕！");
}

BOOL CMy126S_45V_Switch_AppDlg::GetAttenPoint(CString strTempt)
{
	CString strVOAFile;
	char    chRead[256];
	ZeroMemory(chRead,sizeof(chRead));
	FILE    *fp = NULL;
	CString strToken;
	int     nX;
	int     nY;
	int     n5dBX;
	int     n5dBY;
	int     n8dBX;
	int     n8dBY;
	CString strFile;
	CString strNetFile;
	int    iWL=0;
//	double dblWDLIL[100];
//	double dblMax;
//	double dblMin;
//	double dblWDL;
	CString strDirFile;
	CString strKey;
	CString strValue;
	int     nDir=0;
	CString strDir;
	CStdioFile stdVOAFile;
	LPCTSTR lpStr=NULL;
	double dblAtten;
	int    nPort;
	CString strMsg;
	for (int nchannel=0;nchannel<8;nchannel++)
	{
// 		if (nchannel>=4)
//		{
//			nPort = nchannel+4;//不选5~8；
//		}
//		else
//		{
//			nPort = nchannel;
// 		}
		nPort = nchannel;
		if (m_bIfSaveToServe) 
		{
			strVOAFile.Format("%s\\data\\%s\\%s\\%s\\%s_VOAVoltage_%d.csv",m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
		}
		else
		{	
			strVOAFile.Format("%s\\data\\%s\\%s\\%s\\%s_VOAVoltage_%d.csv", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
		}
		if (!stdVOAFile.Open(strVOAFile,CFile::modeRead)) 
		{
			strMsg.Format("打开文件%s失败！",strVOAFile);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		lpStr = stdVOAFile.ReadString(chRead,256);
		while (lpStr!=NULL)
		{
			strToken = strtok(chRead,",");
			nX = atoi(strToken);
			strToken = strtok(NULL,",");
			nY = atoi(strToken);
			strToken = strtok(NULL,",");
			dblAtten = atof(strToken);
			if (dblAtten>=5)
			{
				n5dBX = nX;
				n5dBY = nY;
				break;
			}
			lpStr = stdVOAFile.ReadString(chRead,256);
		}
		lpStr = stdVOAFile.ReadString(chRead,256);
		while (lpStr!=NULL)
		{
			strToken = strtok(chRead,",");
			nX = atoi(strToken);
			strToken = strtok(NULL,",");
			nY = atoi(strToken);
			strToken = strtok(NULL,",");
			dblAtten = atof(strToken);
			if (dblAtten>=8)
			{
				n8dBX = nX;
				n8dBY = nY;
				break;
			}
			lpStr = stdVOAFile.ReadString(chRead,256);
		}
		stdVOAFile.Close();
		n5dBX = n5dBX*MAX_VOLTAGE/MAX_DAC;
		n5dBY = n5dBY*MAX_VOLTAGE/MAX_DAC;
		n8dBX = n8dBX*MAX_VOLTAGE/MAX_DAC;
		n8dBY = n8dBY*MAX_VOLTAGE/MAX_DAC;
		m_n5dBAttenX[nchannel] = n5dBX;
		m_n5dBAttenY[nchannel] = n5dBY;
		m_n8dBAttenX[nchannel] = n8dBX;
		m_n8dBAttenY[nchannel] = n8dBY;
		strMsg.Format("5dBPort%d,%d,%d",nchannel+1,m_n5dBAttenX[nchannel],m_n5dBAttenY[nchannel]);
		LogInfo(strMsg);
		strMsg.Format("8dBPort%d,%d,%d",nchannel+1,m_n8dBAttenX[nchannel],m_n8dBAttenY[nchannel]);
		LogInfo(strMsg);
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::GetAttenPointFromATMS()
{
	CString strMsg;
	CString strURL;
	CString strLocalFile;
	int     iRandNo=rand();
	DWORD   dwBuf[256];
	DWORD   dwTmptBuf[256];
	DWORD   dwBufCH[256];
	DWORD   dwWDMTemp;
	DWORD   dwWDMTempItem;
	CString strErrMsg;
	int     nNodeCount;
	CString strTemperature;
	CString strTemp;
	CString strMax;
	CString strMin;
	CString strKey;
	CString strProcess;
	
	if (m_strItemName=="终测") //终测
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&randno=%d&showdata=1&Pretest=1",m_strSN,iRandNo);
	}
	else if(m_strItemName=="初测")
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&randno=%d&showdata=1&adjust=1",m_strSN,iRandNo);
	}
	strLocalFile.Format("%s\\template\\Test_temp.xml", g_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	//核对PN信息是否对应
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
 		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
 		return FALSE;
 	}
	dwWDMTemp = dwBuf[0];
	ZeroMemory(dwTmptBuf, 256 * sizeof(DWORD));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwTmptBuf, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	dwWDMTemp = dwTmptBuf[0];
	m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值

	int nCH=0;
	int nPort=0;
	//根据PN判断通道组合
	if (strTemp=="room") //常温
	{
		//5dB电压
		ZeroMemory(dwBufCH,sizeof(dwBufCH));
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Voltage_5dB", dwBufCH, nNodeCount,dwWDMTemp);
		dwWDMTempItem = dwBufCH[0];

		for (nPort=0;nPort<8;nPort++)
		{
			if (nPort>=4)
			{
				nCH = nPort+4;
			}
			else
			{
				nCH = nPort;
			}
			strTemp="";
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			strKey.Format("CH%d_X",nCH+1);
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
			m_n5dBAttenX[nPort] = atoi(strTemp);
			
			strKey.Format("CH%d_Y",nCH+1);
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwWDMTempItem);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
	    	m_n5dBAttenY[nPort] = atoi(strTemp);
		}
		//8dB电压
		ZeroMemory(dwBufCH,sizeof(dwBufCH));
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Voltage_8dB", dwBufCH, nNodeCount,dwWDMTemp);
		dwWDMTempItem = dwBufCH[0];

		for (nPort=0;nPort<8;nPort++)
		{
			if (nPort>=4)
			{
				nCH = nPort+4;
			}
			else
			{
				nCH = nPort;
			}
			strTemp="";
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			strKey.Format("CH%d_X",nCH+1);
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
			m_n8dBAttenX[nPort] = atoi(strTemp);
			
			strKey.Format("CH%d_Y",nCH+1);
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwWDMTempItem);
			m_ctrXMLCtr.GetNodeAttr("RESULT",strTemp,dwBuf[0]);
			if (strTemp.IsEmpty())
			{
				LogInfo("没有找到对应的电压值！");
				return FALSE;
			}
	    	m_n8dBAttenY[nPort] = atoi(strTemp);
		}	
	}

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "XML关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnButtonCreateWdl() 
{
	// TODO: Add your control notification handler code here
	CString strILFile;
	CString strWDLFile;
	CString strPath;
	CString strMsg;
	CString strToken;
	CString strTemp;
	CString strValue;
//	double dblMax;
//	double dblMin;
	int     nCH=0;
	int     nPort=0;
	int     nWL=0;
	char    chBUf[256];
	FILE    *fp=NULL;
	LPCTSTR  lpStr = NULL;
	CString strTempt;
	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	else
	{
		strPath = g_tszAppFolder;
	}
	if (m_ctrlComboxTemperature.GetCurSel()==0)
	{
		strTemp.Format("开始计算WDL常温参数！");
		LogInfo(strTemp,FALSE);
//		dblTargetTempt = 26;
//		nTemptStatus = 0;
		strTempt = "Room";
	}
	else if (m_ctrlComboxTemperature.GetCurSel()==1)
	{
		strTemp.Format("开始计算WDL低温参数！");
		LogInfo(strTemp);
	//	dblTargetTempt = -5;
	//	nTemptStatus = 2;
		strTempt = "Low";
	}
	else 
	{
		strTemp.Format("开始计算WDL高温参数！");
		LogInfo(strTemp);
	//	dblTargetTempt = 70;
	//	nTemptStatus = 1;
		strTempt = "High";
	}
	double dbl5dBWDL[8];
	double dbl8dBWDL[8];
	//5dB
	for (nCH=0;nCH<8;nCH++)
	{
		if (nCH>=4)
		{
			nPort = nCH+4;
		}
		else
		{
			nPort = nCH;
		}
		nPort = nCH;
		strILFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att5dB_%d-%s.CSV",strPath,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1,strTempt);
		fp = fopen(strILFile,"r");
		if (fp==NULL) 
		{
			strMsg.Format("打开文件%s失败！",strILFile);
	//		LogInfo(strMsg);
			return;
		}
		ZeroMemory(chBUf,sizeof(chBUf));
		while (1)
		{
			lpStr = fgets(chBUf,256,fp);
			if (lpStr==NULL)
			{
				break;
			}
			strToken = strtok(chBUf,",");
			strToken = strtok(NULL,",");
		}
		fclose(fp);
		dbl5dBWDL[nCH] = atof(strToken);
		
		//8dB
		strILFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att8dB_%d-%s.CSV",strPath,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1,strTempt);
		fp = fopen(strILFile,"r");
		if (fp==NULL) 
		{
			strMsg.Format("打开文件%s失败！",strILFile);
			LogInfo(strMsg);
			return;
		}
		ZeroMemory(chBUf,sizeof(chBUf));
		while (1)
		{
			lpStr = fgets(chBUf,256,fp);
			if (lpStr==NULL)
			{
				break;
			}
			strToken = strtok(chBUf,",");
			strToken = strtok(NULL,",");
		}
		fclose(fp);
		dbl8dBWDL[nCH] = atof(strToken);
	}
	//保存WDL数据
//	CString strWDLFile;
	strWDLFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att-%s.CSV",strPath,m_strItemName,m_strPN,m_strSN,m_strSN,strTempt);
	fp = fopen(strWDLFile,"wt");
	if (fp==NULL)
	{
		strMsg.Format("打开文件%s失败！",strWDLFile);
		LogInfo(strMsg);
		return;
	}
	strValue.Format("CH,5dBWDL,8dBWDL\n");
	fprintf(fp,strValue);
	for (nCH=0;nCH<8;nCH++)
	{
		if (!CheckParaPassOrFail(WDL_DATA_5,dbl5dBWDL[nCH]))
		{
			strMsg.Format("通道%d,5dB_WDL:%.2fdB,不合格",nCH+1,dbl5dBWDL[nCH]);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("通道%d,5dB_WDL:%.2fdB",nCH+1,dbl5dBWDL[nCH]);
	    	LogInfo(strMsg);
		}
		if (!CheckParaPassOrFail(WDL_DATA_8,dbl8dBWDL[nCH]))
		{
			strMsg.Format("通道%d,8dB_WDL:%.2fdB,不合格",nCH+1,dbl8dBWDL[nCH]);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("通道%d,8dB_WDL:%.2fdB",nCH+1,dbl8dBWDL[nCH]);
	    	LogInfo(strMsg);
		}
		strValue.Format("%d,%.2f,%.2f\n",nCH+1,dbl5dBWDL[nCH],dbl8dBWDL[nCH]);
    	fprintf(fp,strValue);
	}
	fclose(fp);
	LogInfo("计算WDL结束！");
}

BOOL CMy126S_45V_Switch_AppDlg::GetCT(double *pdblValue, int nCount)
{
	CString strTemp;
	CString strToken;
	char pbyData[30];
	char chData[256];
	ZeroMemory(pbyData,sizeof(pbyData));
	ZeroMemory(chData,sizeof(chData));
	
	strTemp.Format("get power 0 %d\r",nCount);
	memcpy(pbyData,strTemp,strTemp.GetLength());	
    if(!m_comPM[0].WriteBuffer(pbyData,strTemp.GetLength()))
	{
		LogInfo("串口未打开，发送Get POWER 0 CH错误！",FALSE,COLOR_RED);
		return FALSE;
	}
	Sleep(50);
	if(!m_comPM[0].ReadBuffer(chData,256))
	{
		LogInfo("串口未打开，接收Get POWER 0 CH错误！",FALSE,COLOR_RED);
		return FALSE;
	}
	strToken = strtok(chData,"\r");
	for(int i=0;i<nCount;i++)
	{
		pdblValue[i] = atof(strToken)/100.0;
		strToken = strtok(NULL,"\r");
	}
    return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::GetVOAData(int nPort)
{
	CString strCSVFile;
	CString strPath;
	CString strToken;
	CString strMsg;
	FILE    *fp=NULL;
	char    chRead[256];
	int     nX;
	int     nY;
	double dblValue;
	BOOL   bFlag=FALSE;
	strPath = g_tszAppFolder;
	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	//Voltage
	strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_VOAVoltage_%d.csv",strPath,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);

    fp = fopen(strCSVFile,"r");
	if (fp==NULL)
	{
		strMsg.Format("文件%s不存在！直接赋值",strCSVFile);
	//	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_n5dBAttenX[nPort] = 0;
		m_n5dBAttenY[nPort] = 0;
	}
	else
	{
		fgets(chRead,256,fp);
		do 
		{
			if(NULL==fgets(chRead,256,fp))
			{
				strMsg.Format("读取文件%s失败！",strCSVFile);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				fclose(fp);
				return FALSE;
			}
			strToken = strtok(chRead,",");
			nX = atoi(strToken);
			strToken = strtok(NULL,",");
			nY = atoi(strToken);
			strToken = strtok(NULL,",");
			dblValue = atof(strToken);
			if (!bFlag)
			{
				if (dblValue>=5)
				{
					nX = nX*MAX_VOLTAGE/MAX_DAC;
					nY = nY*MAX_VOLTAGE/MAX_DAC;
					m_n5dBAttenX[nPort] = nX;
					m_n5dBAttenY[nPort] = nY;
					bFlag = TRUE;
				}
			}
			if (dblValue>=8)
			{
				nX = nX*MAX_VOLTAGE/MAX_DAC;
				nY = nY*MAX_VOLTAGE/MAX_DAC;
				m_n8dBAttenX[nPort] = nX;
				m_n8dBAttenY[nPort] = nY;
				break;
			}
		} while(1);
		fclose(fp);
	}
	
	int nCount=0;
	//WDL-5dB
	strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att5dB_%d-Low.csv",strPath,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);

    fp = fopen(strCSVFile,"r");
	if (fp==NULL)
	{
		strMsg.Format("文件%s不存在！直接赋值0.1",strCSVFile);
	//	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_pdb5dBWDL[nPort] = 0.1;
	}
	else
	{
		do 
		{
			if(NULL==fgets(chRead,256,fp))
			{
				strMsg.Format("读取文件%s失败！",strCSVFile);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				fclose(fp);
				return FALSE;
			}
			nCount++;
			if (nCount>=13)
			{
				strToken = strtok(chRead,",");
	    		strToken = strtok(NULL,",");
	    		m_pdb5dBWDL[nPort] = atof(strToken);
				break;
			}

		} while(1);
		fclose(fp);
	}
	
	//WDL-8dB
	strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL-Att8dB_%d-Low.csv",strPath,m_strItemName,m_strPN,m_strSN,m_strSN,nPort+1);
    nCount = 0;
    fp = fopen(strCSVFile,"r");
	if (fp==NULL)
	{
		strMsg.Format("文件%s不存在！",strCSVFile);
	//	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_pdb8dBWDL[nPort] = 0.1;
	}
	else
	{
		do 
		{
			if(NULL==fgets(chRead,256,fp))
			{
				strMsg.Format("读取文件%s失败！",strCSVFile);
			//	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				fclose(fp);
				return FALSE;
			}
			nCount++;
			if (nCount>=13)
			{
				strToken = strtok(chRead,",");
	    		strToken = strtok(NULL,",");
	    		m_pdb8dBWDL[nPort] = atof(strToken);
				break;
			}

		} while(1);
		fclose(fp);
	}
	
	//Repeat-5dB
	strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ATT_Repeat_VOA_Low.csv",strPath,m_strItemName,m_strPN,m_strSN,m_strSN);
    nCount = 0;
    fp = fopen(strCSVFile,"r");
	if (fp==NULL)
	{
		strMsg.Format("文件%s不存在！直接赋值",strCSVFile);
	//	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_pdb5dBRepeatAtten[nPort] = 0;
		m_pdb8dBRepeatAtten[nPort] = 0;
	}
	else
	{
		do 
		{
			if(NULL==fgets(chRead,256,fp))
			{
				strMsg.Format("读取文件%s失败！",strCSVFile);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				fclose(fp);
				return FALSE;
			}
			
			if (nCount==(nPort+1))
			{
				strToken = strtok(chRead,",");
	    		strToken = strtok(NULL,",");
	    		m_pdb5dBRepeatAtten[nPort] = atof(strToken);
				strToken = strtok(NULL,",");
	    		m_pdb8dBRepeatAtten[nPort] = atof(strToken);
				break;
			}
			nCount++;
		} while(1);
		fclose(fp);
	}
	
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::GetPortInfo(CString strPN, CString strSpec,CString strTestItem)
{
	//获取PN通道信息
	CString strConfigFile;
	CString strValue;
	CString strMsg;
	CString strWDLFile;
	strConfigFile.Format("%s\\%s-%s\\PortInfo.ini", m_strPNConfigPath,strSpec,strPN);
	if (GetFileAttributes(strConfigFile)==-1) 
	{
		strMsg.Format("文件不存在:%s",strConfigFile);
		LogInfo(strMsg,FALSE,COLOR_RED);
		return FALSE;
	}
	
	GetPrivateProfileString("CHCount",strTestItem,"ERROR",strValue.GetBuffer(128),128,strConfigFile);
	if (strcmp(strValue,"ERROR")==0)
	{
		strMsg.Format("配置项CHCount-%s不存在",strTestItem);
		LogInfo(strMsg,FALSE,COLOR_RED);
		return FALSE;
	}
	m_nCHCount = atoi(strValue);

	GetPrivateProfileString("ComPort","Value","ERROR",strValue.GetBuffer(128),128,strConfigFile);
	if (strcmp(strValue,"ERROR")==0)
	{
		strMsg.Format("配置项ComPort-Value不存在");
		LogInfo(strMsg,FALSE,COLOR_RED);
		return FALSE;
	}
	m_nSwitchComPortCount = atoi(strValue);

	GetPrivateProfileString("CHSelect","Value","ERROR",strValue.GetBuffer(128),128,strConfigFile);
	if (strcmp(strValue,"ERROR")==0)
	{
		strMsg.Format("配置项CHSelect-Value不存在");
		LogInfo(strMsg);
		m_strSelectRule = "";
	}
	m_strSelectRule = strValue;

	//加载WL波长
	strWDLFile.Format("%s\\%s-%s\\WDLSET.ini", m_strPNConfigPath,strSpec,strPN);
	GetPrivateProfileString("WL","WLCount","error",strValue.GetBuffer(128),128,strWDLFile);
	if (strValue=="error")
	{
		strMsg.Format("读取%s文件错误！(WDL-WLCount)",strWDLFile);
	//	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_pdblWavelength = 1550;
		m_nWLCount = 1;
		m_dblWL[0] = m_pdblWavelength;
		LogInfo("默认配置中心波长为单波：1550nm");
		return TRUE;
	}
	m_nWLCount = atoi(strValue);

	CString strKey;
	for (int i=0;i<m_nWLCount;i++)
	{
		strKey.Format("WL%d",i+1);
		GetPrivateProfileString("WL",strKey,"error",strValue.GetBuffer(128),128,strWDLFile);
		if (strValue=="error")
		{
			strMsg.Format("读取%s文件错误！(WDL-%s)",strWDLFile,strKey);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		if (i>=48)
		{
			LogInfo("波长数量超过限制，自动忽略后续波长！",FALSE,COLOR_RED);
			break;
		}
		m_dblWL[i] = atof(strValue);
		strMsg.Format("测试波长为：%.2fnm",m_dblWL[i]);
		LogInfo(strMsg);
	}

	//
	GetPrivateProfileString("CalWL","Value","error",strValue.GetBuffer(128),128,strWDLFile);
	if (strValue=="error")
	{
		strMsg.Format("读取%s文件错误！(CalWL-Value),获取定标波长错误",strWDLFile);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	m_pdblWavelength = atof(strValue);
	strMsg.Format("定标波长为：%.2fnm",m_pdblWavelength);
	LogInfo(strMsg);
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::GetTempConfig()
{
	//获取测试温度值 added by ris 2016-5-11
	CString strTempFile;
	CString strAppName;
	double dblTemp;
	CString strValue;
	CString strMsg;
	strTempFile.Format("%s\\%s-%s\\Temperature.ini", m_strPNConfigPath,m_strSpec,m_strPN);
	for (int nTemp=0;nTemp<3;nTemp++)
	{
		if (nTemp==0)
		{
			strAppName.Format("Room");
			dblTemp = 25;
		}
		else if (nTemp==1)
		{
			strAppName.Format("Low");
			dblTemp = -5;
		}
		else if (nTemp==2)
		{
			strAppName.Format("High");
			dblTemp = 70;
		}
		
		GetPrivateProfileString(strAppName,"Value","ERROR",strValue.GetBuffer(128),128,strTempFile);
		if (strcmp(strValue,"ERROR")==0)
		{
			m_dblTemperature[nTemp] = dblTemp;
		}
		else
		{
			m_dblTemperature[nTemp] = atof(strValue);
		}
		strMsg.Format("%s设置温度:%.1f℃",strAppName,m_dblTemperature[nTemp]);
		LogInfo(strMsg);
	}
	return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonDirectivityTest() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	double dblPref;
	double dblPower;
	CString strTemp;
	BOOL bFlag = TRUE;
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;	
	if (!m_bGetSNInfo)
	{
		MessageBox("请先确认SN号的信息！");
		return;
	}
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bOpenPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}
	int nWL = m_cbSetWL.GetCurSel()-1;
	if (nWL>=0)
	{
		SetPWMWL(m_dblWL[nWL]);
		m_bChange=TRUE;
	}

	//获取当前时间
	SYSTEMTIME st;
	CString    strMsg;
	COleDateTime    tCurTime;
	COleDateTimeSpan tDifHour; 
	int        nDifHour;
	GetLocalTime(&st);
	tCurTime.SetDateTime(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	tDifHour = tCurTime-m_RefStartTime; //计算时间插值
	nDifHour = tDifHour.GetTotalHours();
	if (nDifHour>m_nRefTime)  //比较归零时间差异
	{
		strMsg.Format("当前归零时间为%d小时，已超过归零限制时间（%d小时），请重新归零！",nDifHour,m_nRefTime);
		MessageBox(strMsg);
		m_bHasRefForTest = FALSE;
		return;
	}
	
	UpdateData();
	double dblTargetTempt;
	int    nTemptStatus; //设置温度状态
	double dblCurTempt; //当前温度
	CString strTemperature;
	int    nPort = m_cbChannel.GetCurSel()-1;
	if (nPort<0)
	{
		MessageBox("请选择正确的通道！");
		return;
	}
	int nTemp = m_ctrlComboxTemperature.GetCurSel();
	if (nTemp==0)
	{
		strTemp.Format("请将通道%d接入光，开始常温下通道%d方向性测试！",nPort+1,nPort+1);
		MessageBox(strTemp);
		dblTargetTempt = 25;
		nTemptStatus = 0;
		strTemperature = "Room";
	}
	else if (nTemp==1)
	{
		strTemp.Format("请将通道%d接入光，开始低温下通道%d方向性测试！",nPort+1,nPort+1);
		MessageBox(strTemp);
		dblTargetTempt = -5;
		nTemptStatus = 2;
		strTemperature = "Low";
	}
	else 
	{
		strTemp.Format("请将通道%d接入光，开始高温下通道%d方向性测试！",nPort+1,nPort+1);
		MessageBox(strTemp);
		dblTargetTempt = 70;
		if (stricmp(m_strSpec,"Z4767")==0)
		{
			dblTargetTempt = 85;
		}
		nTemptStatus = 1;
		strTemperature = "High";
	}
	dblTargetTempt = m_dblTemperature[nTemp];
	
	//判断当前温度是否达到要求；
	if (m_bConnectTemptControl)
	{
		dblCurTempt = GetCurrentTempt(); //获取当前冷热盘温度
		if (fabs(dblTargetTempt-dblCurTempt)>2)
		{
			strMsg.Format("当前温度为：%.1f,目标温度为：%.1f，差异超过2℃不能开始测试，请等待温度！",dblCurTempt,dblTargetTempt);
			LogInfo(strMsg,BOOL(FALSE),COLOR_RED);
			SetTemptStatus(nTemptStatus);
			return;
		}
	}

	GetLocalTime(&m_StartTime);
	CString strValue;

	strValue.Format("CH%02d-DIR",nPort+1);
	LogInfo(strValue,FALSE,COLOR_BLUE);

	strValue.Format("Port");
    for (int nchannel = 0; nchannel < m_nCHCount; nchannel++)
	{
		if (nPort==nchannel)
		{
			continue;
		}
		strTemp.Format(",PM%02d",nchannel+1);
		strValue = strValue + strTemp;
	}
	LogInfo(strValue,FALSE,COLOR_BLUE);
	double dblDIR;
	double dblMinDIR = 100;

	for (int nCH=0;nCH<m_nCHCount;nCH++)
	{
		YieldToPeers();
		if (m_bTestStop)
		{
			LogInfo("测试终止",FALSE,COLOR_RED);
			return;
		}
		if(!SWToChannel(nCH+1))
		{
			strMsg.Format("切换到通道%d错误",nCH+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}    
		strMsg.Format("切换到通道%d成功",nCH+1);
		LogInfo(strMsg,(BOOL)FALSE);
		Sleep(800);

		strValue.Format("%02d",nCH+1);
		bFlag = TRUE;
		for (int nchannel = 0; nchannel < m_nCHCount; nchannel++)
		{	
			//调用归零数据
			if (nWL>=0)
			{
				dblPref = m_dblWDLRef[nchannel][nWL];
			}
			else
			{
				dblPref = m_dblReferencePower[nchannel];
				dblPref = 0;
			}
			if (nPort==nchannel)
			{
				continue;
			}
				
			dblPower = ReadPWMPower(nchannel);
			Sleep(200);
			dblDIR= TestIL(dblPref,dblPower);
			if (dblDIR<dblMinDIR) 
			{
				dblMinDIR = dblDIR;
			}
			if (!CheckParaPassOrFail(DIRECTIVITY_DATA,dblDIR))
			{
				bFlag = FALSE;
			}
			strTemp.Format(",%.2f",dblDIR);
			strValue = strValue+strTemp;
				
		}
		if (!bFlag)
		{
			LogInfo(strValue,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			LogInfo(strValue);
		}
	}
	ChannalMessage[nPort].dblMinDIR = dblMinDIR;
	
	SaveDataToCSV(DIRECTIVITY_DATA,nPort,strTemperature);
	UpdateATMData(DIRECTIVITY_DATA,nPort);
	strTemp.Format("通道%d方向性测试完毕",nPort+1);
	LogInfo(strTemp);
}

double CMy126S_45V_Switch_AppDlg::CalTDL(double dblRoom, double dblLow, double dblHigh)
{
	double dblMinValue = 100;
	double dblMaxValue = -100;
	double dblTDL;
	dblMinValue = dblRoom;
	dblMaxValue = dblRoom;
	if (dblLow>dblMaxValue) 
	{
		dblMaxValue = dblLow;
	}
	if (dblLow<dblMinValue)
	{
		dblMinValue = dblLow;
	}

	if (dblHigh>dblMaxValue) 
	{
		dblMaxValue = dblHigh;
	}
	if (dblHigh<dblMinValue)
	{
		dblMinValue = dblHigh;
	}

	dblTDL = dblMaxValue - dblMinValue;
	return dblTDL;
}

void CMy126S_45V_Switch_AppDlg::OnChangeEditSn3() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	for (int n=0;n<2;n++) 
	{
		for (int i=0;i<16;i++)
		{
			m_dwSingle[n][i] = 0;
		}
	}
	
}

void CMy126S_45V_Switch_AppDlg::OnChangeEditSn4() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	for (int n=0;n<2;n++) 
	{
		for (int i=0;i<16;i++)
		{
			m_dwSingle[n][i] = 0;
		}
	}
	
	
}

void CMy126S_45V_Switch_AppDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_strItemName = "终测";
	m_strPN = "1831782693";
	m_strSpec = "Z6193";
	CString strTestItem = "Initial";
	//获取无纸化PN
	m_strSN = m_strSN1;
	CString strFileName;
	CString strURL,strTemp;
	CString strLocalFile;
	CString strMsg;
	//获取配置信息
	CString strFile;
	CString strValue;
	CString strKey;
	int     nNodeCount;
	CString strErrMsg;
	DWORD   dwBuf[256];
	CString strWDLFile;
	int     nRand = rand();
	m_bGetSNInfo = FALSE;
	GetLocalTime(&m_StartTime);
	SetDlgItemText(IDC_EDIT_SPEC,m_strSpec);
	/*
	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&showdata=1&randno=%d",m_strSN,nRand);	
	//获得XML模板文件
	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		MessageBox(strMsg);
		m_bisok = FALSE;
		return;
	}


	//设置初测还是终测，获取当前工序
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PROCESS_TYPE", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化工序信息!";
		MessageBox(strMsg);
		m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);	
	if (stricmp(strTemp,"EBOX_MEMSSWITCH07")==0)
	{
		m_strItemName = "初测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
		m_cbTestItem.SetCurSel(0);
	}
	else if (stricmp(strTemp,"EBOX_MEMSSWITCH19")==0 || stricmp(strTemp,"")==0)
	{
		m_strItemName = "初测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
		m_cbTestItem.SetCurSel(1);
	}
	else if (stricmp(strTemp,"EBOX_MEMSSWITCH06")==0)
	{
		m_strItemName = "终测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测2");
		m_cbTestItem.SetCurSel(0);
	
	}
	else
	{
		m_strItemName = strTemp;
		strMsg.Format("工序代号%s不在该软件测试范围内，请确认！",strTemp);
		LogInfo(strMsg,FALSE,COLOR_RED);
		m_bisok = FALSE;
		return;
	}
	
	strMsg.Format("从无纸化获取到的当前工序为:%s(%s)",m_strItemName,strTemp);
	LogInfo(strMsg);

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
		return;
	}
	nRand = rand();
	//重新获取模板
	CString strTestItem;
	if (m_strItemName=="初测")
	{
		strTestItem = "Initial";
//		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&pretest=1&showdata=1&randno=%d",m_strSN,nRand);
	}
	else if (m_strItemName=="终测")
	{
//		strTestItem = "Final";
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&test=1&showdata=1&randno=%d",m_strSN,nRand);
	}		
	//获得XML模板文件
//	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
//	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
//	{
//		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
//	}
//	else
//	{
//		strMsg = "模板下载错误!";
//		MessageBox(strMsg);
//		m_bisok = FALSE;
//		return;
//	}
	CString strPN;
	//PN信息
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strPN, dwBuf[0]);
	m_strGetPN = strPN;
	//SPEC信息
	CString strSpec;
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SPEC", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化SPEC信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strSpec, dwBuf[0]);
	m_strSpec = strSpec;
	strMsg.Format("从无纸化获取到的Spec:%s,PN:%s",strSpec,strPN);
	LogInfo(strMsg);
	SetDlgItemText(IDC_EDIT_SPEC,strSpec);

	m_strPN = m_strGetPN;


    DWORD   dwWDMTemp;
	DWORD   dwBufTemp[256];
	DWORD   dwBufCH[256];
	CString  strNode;
	CString  strItem;
	CString strchani;
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
 		return ;
 	}
	dwWDMTemp = dwBuf[0];

	ZeroMemory(dwBufTemp, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwBufTemp, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
		return ;
	}
    int nTempIndex = 0;
	for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				
				strNode ="CHSelect";
				
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(int nCH = 1;nCH<3;nCH++)
				{
					strItem.Format("SW%d",nCH+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						m_bisok = FALSE;
						return ;
					}
					m_ctrXMLCtr.GetNodeAttr("RESULT", strchani, dwBuf[0]);

					if (strchani != "")
					{
						if (nCH==1)
						{
							i_chansw2 =  atoi(strchani);	
						}
						if (nCH==2) 
						{
							i_chansw3 =  atoi(strchani);					
						}
					}
	       
				
				}
			}
		}*/
	if(!GetPortInfo(m_strPN,m_strSpec,strTestItem))
	{
			m_bisok = FALSE;
		return;
	}
	strMsg.Format("入光通道数为：%d,出光通道数为：%d",m_nSwitchComPortCount,m_nCHCount);
	LogInfo(strMsg);
	if (!m_strSelectRule.IsEmpty())
	{
		if (m_strItemName=="初测")
		{
			strMsg.Format("通道挑选规则：%s",m_strSelectRule);
			LogInfo(strMsg);
		}
	}
	//加载通道配置
	m_cbChannel.ResetContent();
	for (int nIndex=0;nIndex<=m_nCHCount;nIndex++)
	{
		strValue.Format("%d",nIndex);
		m_cbChannel.AddString(strValue);
	}
	m_cbChannel.SetCurSel(0);
	
	SetDlgItemText(IDC_COMBO_COMPORT,"COM0");
	m_cbComPort.SetCurSel(0);
	UpdateWindow();

	if (m_nWLCount>1)
	{
		//加载波长配置
		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
			
		//加载WL波长
		m_cbSetWL.ResetContent();
		strValue.Format("%.0f",m_pdblWavelength);
		m_cbSetWL.AddString(strValue);
		for (int i=0;i<m_nWLCount;i++)
		{
		//	m_dblWL[i] = atof(strValue);
			strValue.Format("%.2f",m_dblWL[i]);
			m_cbSetWL.AddString(strValue);
		}
		m_cbSetWL.SetCurSel(0);

		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL_TEST)->EnableWindow(TRUE);
	}

	if (stricmp(m_strSpec,"Z4767")==0)
	{
		if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //77501363A2 for Z4671
		{
			m_strItemName="初测"; //按初测处理
			GetDlgItem(IDC_STATIC_CHSELECT)->ShowWindow(TRUE);
    		GetDlgItem(IDC_COMBO_TEST_CH)->ShowWindow(TRUE);
		}
	}
	
	GetTempConfig();
	SetDlgItemText(IDC_EDIT_PN,m_strPN);
	UpdateWindow();

	if (m_nSwitchComPortCount == 2)
	{
		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(FALSE);
	}

	/*if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}*/

	GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(TRUE);
	if (m_strItemName=="")
	{
		GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(FALSE);
	}
	m_bGetSNInfo = TRUE;	

	CString strXMLFile;
	if (m_bIfSaveToServe)
	{
		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	else 
	{
		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	CopyFile(strLocalFile,strXMLFile,FALSE); //COPY 模板
	strMsg.Format("基本信息获取完毕！",FALSE,COLOR_BLUE);
	LogInfo(strMsg);

	GetDlgItem(IDC_EDIT_SN3)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SN4)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN5)->EnableWindow(false);
	b_issn1 = TRUE;
	b_issn2 = FALSE;
	b_issn3 = FALSE;
	b_issn4 = FALSE;
	
}

void CMy126S_45V_Switch_AppDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here

	UpdateData();
	m_strSN  =  m_strSN3;

	m_strItemName = "终测";
	m_strPN = "1831782693";
	m_strSpec = "Z6193";
	CString strTestItem = "Initial";

	//获取无纸化PN
	CString strFileName;
	CString strURL,strTemp;
	CString strLocalFile;
	CString strMsg;
	//获取配置信息
	CString strFile;
	CString strValue;
	CString strKey;
	int     nNodeCount;
	CString strErrMsg;
	DWORD   dwBuf[256];
	CString strWDLFile;
	int     nRand = rand();
	m_bGetSNInfo = FALSE;
	GetLocalTime(&m_StartTime);
	SetDlgItemText(IDC_EDIT_SPEC,m_strSpec);
	/*
	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&showdata=1&randno=%d",m_strSN,nRand);	
	//获得XML模板文件
	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}


	//设置初测还是终测，获取当前工序
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PROCESS_TYPE", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化工序信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);	
	if (stricmp(strTemp,"EBOX_MEMSSWITCH07")==0)
	{
	      m_strItemName = "初测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
		m_cbTestItem.SetCurSel(1);
	}
	else if (stricmp(strTemp,"EBOX_MEMSSWITCH19")==0 || stricmp(strTemp,"")==0)
	{
		m_strItemName = "初测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
		m_cbTestItem.SetCurSel(1);
	}
	else if (stricmp(strTemp,"EBOX_MEMSSWITCH06")==0)
	{
		m_strItemName = "终测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测2");
		m_cbTestItem.SetCurSel(0);
		
	}
	else
	{
		m_strItemName = strTemp;
		strMsg.Format("工序代号%s不在该软件测试范围内，请确认！",strTemp);
		LogInfo(strMsg,FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}
	
	strMsg.Format("从无纸化获取到的当前工序为:%s(%s)",m_strItemName,strTemp);
	LogInfo(strMsg);

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}
	nRand = rand();
	//重新获取模板
	CString strTestItem;
	if (m_strItemName=="初测")
	{
		strTestItem = "Initial";
//		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&pretest=1&showdata=1&randno=%d",m_strSN,nRand);
	}
	else if (m_strItemName=="终测")
	{
		strTestItem = "Final";
//		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&test=1&showdata=1&randno=%d",m_strSN,nRand);
	}		
	//获得XML模板文件
//	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
//	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
//	{
//		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
//	}
//	else
//	{
//		strMsg = "模板下载错误!";
//		MessageBox(strMsg);
//			m_bisok = FALSE;
//		return;
//	}
	CString strPN;
	//PN信息
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strPN, dwBuf[0]);
	m_strGetPN = strPN;
	//m_strGetPN = "77501369A11";
	//strPN = "77501369A11";
	//SPEC信息
	CString strSpec;
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SPEC", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化SPEC信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strSpec, dwBuf[0]);
	m_strSpec = strSpec;
	strMsg.Format("从无纸化获取到的Spec:%s,PN:%s",strSpec,strPN);
	LogInfo(strMsg);
	SetDlgItemText(IDC_EDIT_SPEC,strSpec);

	m_strPN = m_strGetPN;

	DWORD   dwWDMTemp;
	DWORD   dwBufTemp[256];
	DWORD   dwBufCH[256];
	CString  strNode;
	CString  strItem;
	CString strchani;
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
 		return ;
 	}
	dwWDMTemp = dwBuf[0];

	ZeroMemory(dwBufTemp, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwBufTemp, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
		return ;
	}
    int nTempIndex = 0;
	for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				
				strNode ="CHSelect";
				
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(int nCH = 1;nCH<3;nCH++)
				{
					strItem.Format("SW%d",nCH+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						m_bisok = FALSE;
						return ;
					}
					m_ctrXMLCtr.GetNodeAttr("RESULT", strchani, dwBuf[0]);

					if (strchani != "")
					{
						if (nCH==1)
						{
							i_chansw2 =  atoi(strchani);	
						}
						if (nCH==2) 
						{
							i_chansw3 =  atoi(strchani);					
						}
					}
	       
				
				}
			}
		}*/
	
	if(!GetPortInfo(m_strPN,m_strSpec,strTestItem))
	{
			m_bisok = FALSE;
		return;
	}
	strMsg.Format("入光通道数为：%d,出光通道数为：%d",m_nSwitchComPortCount,m_nCHCount);
	LogInfo(strMsg);
	if (!m_strSelectRule.IsEmpty())
	{
		if (m_strItemName=="初测")
		{
			strMsg.Format("通道挑选规则：%s",m_strSelectRule);
			LogInfo(strMsg);
		}
	}
	//加载通道配置
	m_cbChannel.ResetContent();
	for (int nIndex=0;nIndex<=m_nCHCount;nIndex++)
	{
		strValue.Format("%d",nIndex);
		m_cbChannel.AddString(strValue);
	}
	m_cbChannel.SetCurSel(0);
	
	SetDlgItemText(IDC_COMBO_COMPORT,"COM0");
	m_cbComPort.SetCurSel(0);
	UpdateWindow();

	if (m_nWLCount>1)
	{
		//加载波长配置
		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
			
		//加载WL波长
		m_cbSetWL.ResetContent();
		strValue.Format("%.0f",m_pdblWavelength);
		m_cbSetWL.AddString(strValue);
		for (int i=0;i<m_nWLCount;i++)
		{
		//	m_dblWL[i] = atof(strValue);
			strValue.Format("%.2f",m_dblWL[i]);
			m_cbSetWL.AddString(strValue);
		}
		m_cbSetWL.SetCurSel(0);

		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL_TEST)->EnableWindow(TRUE);
	}

	if (stricmp(m_strSpec,"Z4767")==0)
	{
		if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //77501363A2 for Z4671
		{
			m_strItemName="初测"; //按初测处理
			GetDlgItem(IDC_STATIC_CHSELECT)->ShowWindow(TRUE);
    		GetDlgItem(IDC_COMBO_TEST_CH)->ShowWindow(TRUE);
		}
	}
	
	/*
	if (stricmp(m_strGetPN,"77501351A2")==0)
	{
		m_strPN = "1X12";
		if (m_strSpec=="12829")
		{
			m_strPN = "8X1";
			if (m_strItemName=="终测")
			{
				m_nCHCount=8;
			}
		}
		if (m_strSpec=="12425"||m_strSpec=="10046")
		{
			m_strPN = "4X1";
			if (m_strItemName=="终测")
			{
				m_nCHCount=4;
			}
		}
		if (m_strSpec=="13235")
		{
			m_strPN = "2X1";
			if (m_strItemName=="终测")
			{
				m_nCHCount=2;
			}
		}
	}
	else if (stricmp(m_strGetPN,"77501353A0")==0)
	{
		m_strPN = "2X16";
		m_nSwitchComPortCount = 2;
		m_cbComPort.SetCurSel(1);
		if (m_strItemName=="终测")
		{
			m_nCHCount=16;
		}
		else if (m_strItemName=="初测")
		{
			m_nCHCount=16;
		}
	}
	else if(stricmp(m_strGetPN,"77501354A2_L")==0)
	{
		m_strPN = "8X1";
		if (m_strItemName=="终测")
		{
			m_nCHCount=8;
		}
	}
	else if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //77501363A2 for Z4671
	{
		m_strPN = "8X1";
		if (m_strItemName=="终测")
		{
			m_nCHCount=8;
		}
		else if (m_strItemName=="初测")  //2016-6-2，初测只测试8个通道
		{
			m_nCHCount=8;
		}
		m_strItemName="初测"; //按初测处理
		GetDlgItem(IDC_STATIC_CHSELECT)->ShowWindow(TRUE);
    	GetDlgItem(IDC_COMBO_TEST_CH)->ShowWindow(TRUE);
	}
	else if (stricmp(m_strGetPN,"77501355A2")==0)
	{
		m_strPN = "1X9";
		if (m_strItemName=="终测")
		{
			m_nCHCount=9;
		}
	}
	else if ((stricmp(m_strGetPN,"77501356A2")==0)||(stricmp(m_strGetPN,"UOFS102U00IFA01G")==0))
	{
		m_strPN = "2X1";
		if (m_strItemName=="终测")
		{
			m_nCHCount=2;
		}
	}
	else if (stricmp(m_strGetPN,"77501357A2")==0)
	{
		m_strPN = "9X1";
		if (m_strItemName=="终测")
		{
			m_nCHCount=9;
		}
	}
	else if (stricmp(m_strGetPN,"77501358A2")==0)
	{
		m_strPN = "4X1";
		if (m_strItemName=="终测")
		{
			m_nCHCount=4;
		}
	}
	else if(m_strGetPN=="UOFS104E00ZCC01G")
	{
		m_strPN = "4X1";
		if (m_strItemName=="终测")
		{
			m_strPN = "4X1";
			m_nCHCount=4;
		}
	}
	else if (stricmp(m_strGetPN,"77501359A2")==0)
	{
		m_strPN = "1X19";
		m_nCHCount = 19;
		if (m_strSpec=="S0379")
		{
			m_strPN = "1X16";
			if (m_strItemName=="终测")
			{
				m_nCHCount=16;
			}
		}
	}
	else if (stricmp(m_strGetPN,"77501360A11")==0)  //添加18X1开关
	{
		m_strPN = "18X1";
		m_nCHCount = 18;
		if (m_strSpec=="13998")
		{
			m_pdblWavelength = 1550;
		}
		else
		{
			m_pdblWavelength=1360;
			GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
			
			//加载WL波长
			m_cbSetWL.ResetContent();

			strValue.Format("%.0f",m_pdblWavelength);
			m_cbSetWL.AddString(strValue);
			strWDLFile.Format("%s\\config\\%s\\WDLSET.INI",g_tszAppFolder,m_strPN);
			GetPrivateProfileString("WDL","COUNT","error",strValue.GetBuffer(128),128,strWDLFile);
			if (strValue=="error")
			{
				strMsg.Format("读取%s文件错误！",strWDLFile);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				return;
			}
			m_nWLCount = atoi(strValue);

			for (int i=0;i<m_nWLCount;i++)
			{
				strKey.Format("WL%d",i+1);
				GetPrivateProfileString("WDL",strKey,"error",strValue.GetBuffer(128),128,strWDLFile);
				if (strValue=="error")
				{
					strMsg.Format("读取%s文件错误！",strWDLFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					return;
				}
				m_dblWL[i] = atof(strValue);
				m_cbSetWL.AddString(strValue);
			}
			m_cbSetWL.SetCurSel(0);
		}
		
	}
	else if(m_strPN=="UOFS112U00LSR01G")
	{
		m_strPN = "1X12-2";
		m_nCHCount = 12;
		m_pdblWavelength=1550;
		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL_TEST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PDL_TEST)->EnableWindow(TRUE);
		//加载WL波长
		m_cbSetWL.ResetContent();

		strValue.Format("%.0f",m_pdblWavelength);
		m_cbSetWL.AddString(strValue);
		strWDLFile.Format("%s\\config\\%s\\WDLSET.INI",g_tszAppFolder,m_strPN);
		GetPrivateProfileString("WDL","COUNT","error",strValue.GetBuffer(128),128,strWDLFile);
		if (strValue=="error")
		{
			strMsg.Format("读取%s文件错误！",strWDLFile);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}
		m_nWLCount = atoi(strValue);

		for (int i=0;i<m_nWLCount;i++)
		{
			strKey.Format("WL%d",i+1);
			GetPrivateProfileString("WDL",strKey,"error",strValue.GetBuffer(128),128,strWDLFile);
			if (strValue=="error")
			{
				strMsg.Format("读取%s文件错误！",strWDLFile);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				return;
			}
			m_dblWL[i] = atof(strValue);
			m_cbSetWL.AddString(strValue);
		}
		m_cbSetWL.SetCurSel(0);

	}
	else if (stricmp(m_strGetPN,"UOFS108L00ATL01G")==0)  //add PN:UOFS108L00ATL01G  2016-7-28
	{
		m_strPN = "8X1-12829";
		if (m_strItemName=="终测")
		{
			m_nCHCount=8;
		}
	}
	else
	{
		strMsg.Format("PN%s不在该软件识别范围内，请确认！",m_strGetPN);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return;
	}
	*/
	GetTempConfig();
	SetDlgItemText(IDC_EDIT_PN,m_strPN);
	UpdateWindow();

	if (m_nSwitchComPortCount == 2)
	{
		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(FALSE);
	}

	/*if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}*/

	GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(TRUE);
	if (m_strItemName=="")
	{
		GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(FALSE);
	}
	m_bGetSNInfo = TRUE;	
	CString strXMLFile;
	if (m_bIfSaveToServe)
	{
		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	else 
	{
		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	CopyFile(strLocalFile,strXMLFile,FALSE); //COPY 模板
	strMsg.Format("基本信息获取完毕！",FALSE,COLOR_BLUE);
	LogInfo(strMsg);

	GetDlgItem(IDC_EDIT_SN)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN3)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SN4)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN5)->EnableWindow(false);
	b_issn1 = FALSE;
	b_issn2 = TRUE;
	b_issn3 = FALSE;
	b_issn4 = FALSE;
	
}

void CMy126S_45V_Switch_AppDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
    m_strSN =  m_strSN4;
	m_strItemName = "终测";
	m_strPN = "1831782693";
	m_strSpec = "Z6193";
	CString strTestItem = "Initial";
	//获取无纸化PN
	CString strFileName;
	CString strURL,strTemp;
	CString strLocalFile;
	CString strMsg;
	//获取配置信息
	CString strFile;
	CString strValue;
	CString strKey;
	int     nNodeCount;
	CString strErrMsg;
	DWORD   dwBuf[256];
	CString strWDLFile;
	int     nRand = rand();
	m_bGetSNInfo = FALSE;
	GetLocalTime(&m_StartTime);
	SetDlgItemText(IDC_EDIT_SPEC,m_strSpec);
	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&showdata=1&randno=%d",m_strSN,nRand);	
	//获得XML模板文件
	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}

	/*
	//设置初测还是终测，获取当前工序
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PROCESS_TYPE", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化工序信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);	
	if (stricmp(strTemp,"EBOX_MEMSSWITCH06")==0)
	{
		m_strItemName = "终测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测2");
		m_cbTestItem.SetCurSel(0);
	}
	else if (stricmp(strTemp,"EBOX_MEMSSWITCH19")==0  || stricmp(strTemp,"")==0)
	{
		m_strItemName = "初测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
		m_cbTestItem.SetCurSel(1);
	}
	else if (stricmp(strTemp,"EBOX_MEMSSWITCH07")==0)
	{
		m_strItemName = "初测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
		m_cbTestItem.SetCurSel(1);
	}
	else
	{
		m_strItemName = strTemp;
		strMsg.Format("工序代号%s不在该软件测试范围内，请确认！",strTemp);
		LogInfo(strMsg,FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}
	
	strMsg.Format("从无纸化获取到的当前工序为:%s(%s)",m_strItemName,strTemp);
	LogInfo(strMsg);

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}
	nRand = rand();
	//重新获取模板
	CString strTestItem;
	if (m_strItemName=="初测")
	{
		strTestItem = "Initial";
	//	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&pretest=1&showdata=1&randno=%d",m_strSN,nRand);
	}
	else if (m_strItemName=="终测")
	{
		strTestItem = "Final";
	//	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&test=1&showdata=1&randno=%d",m_strSN,nRand);
	}		
	//获得XML模板文件
//	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
//	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
//	{
//		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
//	}
//	else
//	{
//		strMsg = "模板下载错误!";
//		MessageBox(strMsg);
//			m_bisok = FALSE;
//		return;
//	}
	CString strPN;
	//PN信息
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strPN, dwBuf[0]);
	m_strGetPN = strPN;
	//SPEC信息
	CString strSpec;
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SPEC", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化SPEC信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strSpec, dwBuf[0]);
	m_strSpec = strSpec;
	strMsg.Format("从无纸化获取到的Spec:%s,PN:%s",strSpec,strPN);
	LogInfo(strMsg);
	SetDlgItemText(IDC_EDIT_SPEC,strSpec);

	m_strPN = m_strGetPN;

	    DWORD   dwWDMTemp;
	DWORD   dwBufTemp[256];
	DWORD   dwBufCH[256];
	CString  strNode;
	CString  strItem;
	CString strchani;
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
 		return ;
 	}
	dwWDMTemp = dwBuf[0];

	ZeroMemory(dwBufTemp, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwBufTemp, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
		return ;
	}
    int nTempIndex = 0;
	for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				
				strNode ="CHSelect";
				
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(int nCH = 1;nCH<3;nCH++)
				{
					strItem.Format("SW%d",nCH+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						m_bisok = FALSE;
						return ;
					}
					m_ctrXMLCtr.GetNodeAttr("RESULT", strchani, dwBuf[0]);

					if (strchani != "")
					{
						if (nCH==1)
						{
							i_chansw2 =  atoi(strchani);	
						}
						if (nCH==2) 
						{
							i_chansw3 =  atoi(strchani);					
						}
					}
	       
				
				}
			}
		}*/

	if(!GetPortInfo(m_strPN,m_strSpec,strTestItem))
	{
			m_bisok = FALSE;
		return;
	}
	strMsg.Format("入光通道数为：%d,出光通道数为：%d",m_nSwitchComPortCount,m_nCHCount);
	LogInfo(strMsg);
	if (!m_strSelectRule.IsEmpty())
	{
		if (m_strItemName=="初测")
		{
			strMsg.Format("通道挑选规则：%s",m_strSelectRule);
			LogInfo(strMsg);
		}
	}
	//加载通道配置
	m_cbChannel.ResetContent();
	for (int nIndex=0;nIndex<=m_nCHCount;nIndex++)
	{
		strValue.Format("%d",nIndex);
		m_cbChannel.AddString(strValue);
	}
	m_cbChannel.SetCurSel(0);
	
	SetDlgItemText(IDC_COMBO_COMPORT,"COM0");
	m_cbComPort.SetCurSel(0);
	UpdateWindow();

	if (m_nWLCount>1)
	{
		//加载波长配置
		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
			
		//加载WL波长
		m_cbSetWL.ResetContent();
		strValue.Format("%.0f",m_pdblWavelength);
		m_cbSetWL.AddString(strValue);
		for (int i=0;i<m_nWLCount;i++)
		{
		//	m_dblWL[i] = atof(strValue);
			strValue.Format("%.2f",m_dblWL[i]);
			m_cbSetWL.AddString(strValue);
		}
		m_cbSetWL.SetCurSel(0);

		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL_TEST)->EnableWindow(TRUE);
	}

	if (stricmp(m_strSpec,"Z4767")==0)
	{
		if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //77501363A2 for Z4671
		{
			m_strItemName="初测"; //按初测处理
			GetDlgItem(IDC_STATIC_CHSELECT)->ShowWindow(TRUE);
    		GetDlgItem(IDC_COMBO_TEST_CH)->ShowWindow(TRUE);
		}
	}
	
	GetTempConfig();
	SetDlgItemText(IDC_EDIT_PN,m_strPN);
	UpdateWindow();

	if (m_nSwitchComPortCount == 2)
	{
		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(FALSE);
	}

	/*if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}*/

	GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(TRUE);
	if (m_strItemName=="")
	{
		GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(FALSE);
	}
	m_bGetSNInfo = TRUE;	
	CString strXMLFile;
	if (m_bIfSaveToServe)
	{
		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	else 
	{
		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	CopyFile(strLocalFile,strXMLFile,FALSE); //COPY 模板
	strMsg.Format("基本信息获取完毕！",FALSE,COLOR_BLUE);
	LogInfo(strMsg);

	GetDlgItem(IDC_EDIT_SN3)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN4)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SN)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN5)->EnableWindow(false);
	b_issn1 = FALSE;
	b_issn2 = FALSE;
	b_issn3 = TRUE;
	b_issn4 = FALSE;
}

BOOL CMy126S_45V_Switch_AppDlg::TxDataToI2C(CString strAddress, BYTE *pbySetValue, DWORD dwSetLength)
{
	DWORD dwFeedBackLength;
	DWORD dwWaitTime = 100;
	BYTE bytSlaveAddr;
	CString strCommand;
	CString strTemp="";
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
//	BYTE byHiAddr = HIBYTE(wAddress);
//	BYTE byLoAddr = LOBYTE(wAddress);
//	BYTE byHindex = HIBYTE(wdyIndex);
//	BYTE byLoindex = LOBYTE(wdyIndex);
	
	bytSlaveAddr =  0xF8;
	//Seti2c f8 a5 3c 00 42 1 
	//Geti2c f8 00 42 0f
	strCommand.Format("SetI2C 0 %02x %s",bytSlaveAddr,strAddress);
	for (DWORD n=0;n<dwSetLength;n++) 
	{
		strTemp.Format("%02x",pbySetValue[n]);
		strCommand = strCommand +  strTemp;
	}
	strCommand = strCommand + "\r";
	ZeroMemory(chSetCommand,sizeof(char)*MAX_COUNT);
	memcpy(chSetCommand,strCommand,strCommand.GetLength());
	
	if(!m_opCom.WriteBuffer(chSetCommand,strCommand.GetLength()))
	{
		m_strErrorMsg = "发送指令错误！";
		return FALSE;
	}
	Sleep(dwWaitTime);
	ZeroMemory(chReadCommand,sizeof(char)*MAX_COUNT);
	if (!m_opCom.ReadBuffer(chReadCommand,MAX_COUNT,&dwFeedBackLength))
	{
		m_strErrorMsg = "读取I2C数据错误！";
		return FALSE;
	}

	strTemp = chReadCommand;

	if(strTemp.Find("E: Invalid Command") !=-1)
	{
		m_strErrorMsg ="无效的指令";
		return FALSE;
	}
    else if(strTemp.Find("E: Invalid para") !=-1)
	{
		m_strErrorMsg ="无效的参数";
		return FALSE;
	}
	else if(strTemp.Find("E: Invalid data") !=-1)
	{
		m_strErrorMsg ="无效的数据";
		return FALSE;
	}
	else if(strTemp.Find("fail") !=-1)
	{
		m_strErrorMsg ="指令执行错误";
		return FALSE;
	}
	
	m_strErrorMsg = "";
    return TRUE; 
	

}

BOOL CMy126S_45V_Switch_AppDlg::RxDataFromI2C(CString wAddress)
{
	DWORD dwFeedBackLength;
	DWORD dwWaitTime = 100;
	BYTE bytSlaveAddr;
	CString strCommand;
	CString strTemp="";
	BYTE bSendBuffer;
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
	//bytSlaveAddr = SLAVE_WRITE;
//	BYTE byHaddex = HIBYTE(wAddress);
//	BYTE byLoaddex = LOBYTE(wAddress);
	ZeroMemory(chSetCommand,sizeof(char)*MAX_COUNT);
	bytSlaveAddr = 0xF8;//Read

	//这里需要先调用SetI2C才能调用GetI2C
	bSendBuffer =0;
 if(!TxSTRToI2C(wAddress,""))
	{
		return FALSE;
	}
    Sleep(20);
	if (wAddress == "0161" )
	{
		sprintf(chSetCommand,"GetI2C 0 %02x 02\r",bytSlaveAddr);
		
	}
	else if (wAddress == "0035")
	{
		sprintf(chSetCommand,"GetI2C 0 %02x 08\r",bytSlaveAddr);
	}
	else if (wAddress == "016A" && m_bisinit)
	{
		sprintf(chSetCommand,"GetI2C 0 %02x 0004\r",bytSlaveAddr);
	}
	else if (wAddress == "016A")
	{
		sprintf(chSetCommand,"GetI2C 0 %02x 0002\r",bytSlaveAddr);
	}
	else
	{
		sprintf(chSetCommand,"GetI2C 0 %02x 01\r",bytSlaveAddr);
	}
	

	if (!m_opCom.WriteBuffer(chSetCommand,strlen(chSetCommand)))
	{
		m_strErrorMsg = "发送指令错误！";
		return FALSE;
	} 
	Sleep(dwWaitTime);
	ZeroMemory(chReadCommand,sizeof(char)*MAX_COUNT);
	if (!m_opCom.ReadBuffer(chReadCommand,MAX_COUNT,&dwFeedBackLength))
	{
		m_strErrorMsg = "读取I2C数据错误";
		return FALSE;
	}

	strTemp = chReadCommand;

	if(strTemp.Find("E: Invalid Command") !=-1)
	{
		m_strErrorMsg ="无效的指令";
		return FALSE;
	}
    else if(strTemp.Find("E: Invalid para") !=-1)
	{
		m_strErrorMsg ="无效的参数";
		return FALSE;
	}
	else if(strTemp.Find("E: Invalid data") !=-1)
	{
		m_strErrorMsg ="无效的数据";
		return FALSE;
	}
	else if(strTemp.Find("E: Execute Fail") !=-1)
	{
		m_strErrorMsg ="指令执行错误";
		return FALSE;
	}	
	else
	{
		if (wAddress == "0161")
		{
			   int nTemp = m_ctrlComboxTemperature.GetCurSel();
			   if (nTemp==0)
			   {
				   m_lutRoomTep = strTemp;
				   m_lutRoomTep = m_lutRoomTep.Left(4);
			   }
			   if (nTemp==1)
			   {
				   m_lutLowTep = strTemp;
				   m_lutLowTep = m_lutLowTep.Left(4);
			   }
			   if (nTemp==2)
			   {
				    m_lutHighTep = strTemp;
					m_lutHighTep = m_lutHighTep.Left(4);
			   }
			   
		}
		else if (wAddress == "0035")
		{
			   CString strShowMsg,strmsg;
			   int ntemp;
			   strShowMsg = "软件版本：";
			   ntemp =strtol(strTemp.Left(2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;
			   ntemp =strtol(strTemp.Mid(2,2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;
			   ntemp =strtol(strTemp.Mid(4,2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;
			   ntemp =strtol(strTemp.Mid(6,2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;
			   ntemp =strtol(strTemp.Mid(8,2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;
			   ntemp =strtol(strTemp.Mid(10,2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;
			   ntemp =strtol(strTemp.Mid(12,2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;
			   ntemp =strtol(strTemp.Mid(14,2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;
			   ntemp =strtol(strTemp.Mid(16,2),NULL,16);
			   strmsg.Format("%d", ntemp);
			   strShowMsg = strShowMsg + strmsg;


			   LogInfo(strShowMsg);


			   


		}
		else if (wAddress == "016A")
		{
			m_strErrorMsg = strTemp;
			LogInfo(m_strErrorMsg);
			return TRUE;
		}
		else if(strTemp.Find("00") ==-1)
		{
			m_strErrorMsg ="反馈命令错误";
			return FALSE;
		}

	}
//	int nGetLength;
//    AsiicToHex((BYTE*)chReadCommand,pbySetValue,dwFeedBackLength-1,&nGetLength);
//	*pdwSetLength = nGetLength;
	
	
		m_strErrorMsg = "";

	
	return TRUE;	

}

void CMy126S_45V_Switch_AppDlg::AsiicToHex(BYTE *str, BYTE *hex, int cnt, int *nGetCount)
{
	int i,j;
	BYTE bTemp[3];
	CString strTemp;
	j=0;
	for(i = 0; i < cnt-1; i+=2)
	{
        ZeroMemory(bTemp,3);
		memcpy(bTemp,str+i,2);
		hex[j] = (BYTE)strtol((char*)bTemp,NULL,16);
        j++;  
	}

	*nGetCount = j;

}

BOOL CMy126S_45V_Switch_AppDlg::TxBYTEToI2C(CString strAddress, BYTE byData, BYTE byData1, BYTE byData2, BYTE byData3, BYTE byData4, BYTE byData5, BYTE byData6, BYTE byData7, BYTE byData8, BYTE byData9, BYTE byData10, BYTE byData11, BYTE byData12, BYTE byData13, BYTE byData14, BYTE byData15, BYTE byData16, BYTE byData17, BYTE byData18, BYTE byData19, BYTE byData20, BYTE byData21, BYTE byData22, BYTE byData23, BYTE byData24, BYTE byData25, BYTE byData26, BYTE byData27, BYTE byData28, BYTE byData29, BYTE byData30, BYTE byData31, DWORD dwSetLength)
{
	DWORD dwFeedBackLength;
	DWORD dwWaitTime = 100;
	BYTE bytSlaveAddr;
	CString strCommand;
	CString strTemp="";
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
//	BYTE byHiAddr = HIBYTE(wAddress);
//	BYTE byLoAddr = LOBYTE(wAddress);
//	BYTE byHindex = HIBYTE(wdyIndex);
//	BYTE byLoindex = LOBYTE(wdyIndex);
	
	bytSlaveAddr =  0xF8;
	//Seti2c f8 a5 3c 00 42 1 
	//Geti2c f8 00 42 0f
	strCommand.Format("SetI2C 0 %02x %s",bytSlaveAddr,strAddress);
	//for (DWORD n=0;n<dwSetLength;n++) 
	//{
	if(dwSetLength > 30)
	{
		//strTemp.Format("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",byData,byData1,byData2,byData3,byData4,byData5,byData6,byData7,byData8,byData9,byData10,byData11,byData12,byData13,byData14,byData15,byData16,byData17,byData18,byData19,byData20,byData21,byData22,byData23,byData24,byData25,byData26,byData27,byData28,byData29,byData30,byData31);
	    strTemp.Format("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",byData,byData1,byData2,byData3,byData4,byData5,byData6,byData7,byData8,byData9,byData10,byData11,byData12,byData13,byData14,byData15,byData16,byData17,byData18,byData19,byData20,byData21,byData22,byData23,byData24,byData25,byData26,byData27,byData28,byData29,byData30,byData31);
		//strTemp.Format("%02x",pbySetValue[n]);
	}
	else if(dwSetLength  == 1)
	{
		strTemp.Format("%x",byData);
	}

   
	strCommand = strCommand +  strTemp;

	strCommand = strCommand + "\r";
	ZeroMemory(chSetCommand,sizeof(char)*MAX_COUNT);
	memcpy(chSetCommand,strCommand,strCommand.GetLength());
	
	if(!m_opCom.WriteBuffer(chSetCommand,strCommand.GetLength()))
	{
		m_strErrorMsg = "发送指令错误！";
		return FALSE;
	}
	Sleep(dwWaitTime);
	ZeroMemory(chReadCommand,sizeof(char)*MAX_COUNT);
	if (!m_opCom.ReadBuffer(chReadCommand,MAX_COUNT,&dwFeedBackLength))
	{
		m_strErrorMsg = "读取I2C数据错误！";
		return FALSE;
	}

	strTemp = chReadCommand;

	if(strTemp.Find("E: Invalid Command") !=-1)
	{
		m_strErrorMsg ="无效的指令";
		return FALSE;
	}
    else if(strTemp.Find("E:Invalid para") !=-1)
	{
		m_strErrorMsg ="无效的参数";
		return FALSE;
	}
	else if(strTemp.Find("E: Invalid data") !=-1)
	{
		m_strErrorMsg ="无效的数据";
		return FALSE;
	}
	else if(strTemp.Find("E: Execute Fail") !=-1)
	{
		m_strErrorMsg ="指令执行错误";
		return FALSE;
	}
	
	m_strErrorMsg = "";
    return TRUE; 


}

void CMy126S_45V_Switch_AppDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_SN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SN3)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SN4)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SN5)->EnableWindow(TRUE);
	b_issn1 = FALSE;
	b_issn2 = FALSE;
	b_issn3 = FALSE;
	b_issn4 = FALSE;
}

BOOL CMy126S_45V_Switch_AppDlg::TxSTRToI2C(CString strAddress, CString StrSetValue)
{

	DWORD dwFeedBackLength;
	DWORD dwWaitTime = 100;
	BYTE bytSlaveAddr;
	CString strCommand;
	CString strTemp="";
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
//	BYTE byHiAddr = HIBYTE(wAddress);
//	BYTE byLoAddr = LOBYTE(wAddress);
//	BYTE byHindex = HIBYTE(wdyIndex);
//	BYTE byLoindex = LOBYTE(wdyIndex);
	
	bytSlaveAddr =  0xF8;
	//Seti2c f8 a5 3c 00 42 1 
	//Geti2c f8 00 42 0f
	if(StrSetValue== "")
	{
		strCommand.Format("SetI2C 0 %02x %s",bytSlaveAddr,strAddress);
	}
	else
	{
		strCommand.Format("SetI2C 0 %02x %s%s",bytSlaveAddr,strAddress,StrSetValue);
	}
	
	
	strCommand = strCommand + "\r";
	ZeroMemory(chSetCommand,sizeof(char)*MAX_COUNT);
	memcpy(chSetCommand,strCommand,strCommand.GetLength());
	
	if(!m_opCom.WriteBuffer(chSetCommand,strCommand.GetLength()))
	{
		m_strErrorMsg = "发送指令错误！";
		return FALSE;
	}
	Sleep(dwWaitTime);
	ZeroMemory(chReadCommand,sizeof(char)*MAX_COUNT);
	if (!m_opCom.ReadBuffer(chReadCommand,MAX_COUNT,&dwFeedBackLength))
	{
		m_strErrorMsg = "读取I2C数据错误！";
		return FALSE;
	}

	strTemp = chReadCommand;

	if(strTemp.Find("E: Invalid Command") !=-1)
	{
		m_strErrorMsg ="无效的指令";
		return FALSE;
	}
    else if(strTemp.Find("E: Invalid para") !=-1)
	{
		m_strErrorMsg ="无效的参数";
		return FALSE;
	}
	else if(strTemp.Find("E: Invalid data") !=-1)
	{
		m_strErrorMsg ="无效的数据";
		return FALSE;
	}
	else if(strTemp.Find("E: Execute Fail") !=-1)
	{
		m_strErrorMsg ="指令执行错误";
		return FALSE;
	}
	
	m_strErrorMsg = "";
    return TRUE; 
	
}

void CMy126S_45V_Switch_AppDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CMy126S_45V_Switch_AppDlg::OnButton5() 
{
	// TODO: Add your control notification handler code here

	UpdateData();

	
	SetTimer(8,1000,NULL);
	
}

void CMy126S_45V_Switch_AppDlg::OnButton7() 
{
	// TODO: Add your control notification handler code here

	KillTimer(8);
	
}

void CMy126S_45V_Switch_AppDlg::OnButton8() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_bischange = TRUE;
	if (!CreateMfgInfoBin())
	{
		MessageBox("下载Map表失败！");
		return;
	}
	
	/*
	if(!CreateSwitchMapBin("",0))
	{
		MessageBox("下载Map表失败！");
		return;
	}
	*/
	m_bischange = FALSE;
	
}

BOOL CMy126S_45V_Switch_AppDlg::AutoPointScanRIght(int nChannel)
{
		int x;
	int y;
    int x_step = 2000;
	int x_step1 = 2000;
	int x_step2 = -2000;
	int y_step = 2000;
	int y_step1 = 2000;
	int y_step2 = -2000;
	CString strCommand;
	int XADC,YADC;
	m_bTestStop = FALSE;
	int count = 0;
	int x_count = 0;
	double  dblPower = 0; 
	BOOL    bFlag1 = FALSE;
	BOOL    bFlagX = FALSE;
	BOOL    bFlagY = FALSE;
	int iCount = 0;
	CString strMsg;
	int x_base = 0;
	int y_base = 0;
	double dblBasePower = 100;
	double dblBasePower_X;
	double dblBasePower_Y;
	double dblPrePowerX = 100;
	double dblPrePowerY = 100;
    char    chValue[10];
	CString strSendBuf;
	char chSendBuf[50];
	char chReadBuf[256];
	int xmin,xmax,ymin,ymax;
	CString m_cmd,dwBaseAddress,strdacx,strdacy,strdacxmin,strdacxmax,strdacymin,strdacymax;
	ZeroMemory(chValue,sizeof(char)*10);

	x_base = m_Xbase[nChannel];
	y_base = m_Ybase[nChannel];

	//导入扫描配置
	CString strINI;
	CString strValue;
	CString strName;
	CString strToken;
	double  Point1[6];
	int     step1[6];
	for (int i=0;i<6;i++)
	{
		strName.Format("Point%d",i+1);
		strINI.Format("%s\\%s-%s\\ScanConfig.ini", m_strPNConfigPath,m_strSpec,m_strPN);
		GetPrivateProfileString("PointScan",strName,"error",strValue.GetBuffer(128),128,strINI);
		if (strValue == "error")
		{
			LogInfo("读取配置ScanConfig错误！");
			return FALSE;
		}
		Point1[i] = atof(strValue);
		
		strName.Format("step%d",i+1);
		strINI.Format("%s\\%s-%s\\ScanConfig.ini", m_strPNConfigPath,m_strSpec,m_strPN);
		GetPrivateProfileString("PointScan",strName,"error",strValue.GetBuffer(128),128,strINI);
		if (strValue == "error")
		{
			LogInfo("读取配置ScanConfig错误！");
			return FALSE;
		}
		step1[i] = atoi(strValue);
	}

	double dblSetIL;
	GetPrivateProfileString("MIN_IL","IL","error",strValue.GetBuffer(128),128,strINI);
	if (strValue == "error")
	{
		dblSetIL=0.8;
	}
	dblSetIL= atof(strValue);

	
    
		
     
		XADC=(x_base*MAX_DAC/60000.0);
		YADC=(y_base*MAX_DAC/60000.0);

		if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}

		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		
		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand); //Set Single MemsSwitch DAC

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/


	dblBasePower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);

	//扫描第一个点的光功率值，若异常，则重复读光功率计5次
	while (dblBasePower > 20 && x_count < 5)
	{
		Sleep(400);
		dblBasePower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
		x_count++;
	}
	while (dblBasePower == 0 && x_count < 5)
	{
		Sleep(400);
		dblBasePower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
		x_count++;
	}
	if (dblBasePower == 0)
	{
		LogInfo("扫描出错！",(BOOL)FALSE,COLOR_RED);
		return  FALSE;
	}

	strMsg.Format("坐标：(%d,%d), Power:%.3f dBm\n",x_base,y_base,dblBasePower);
	LogInfo(strMsg);
	
	x = x_base;
	y = y_base;
	dblBasePower_X = dblBasePower;
	dblBasePower_Y = dblBasePower;

	XADC=(x*MAX_DAC/60000.0);
	YADC=(y*MAX_DAC/60000.0);


	int nCount = 0;
//	CString m_cmd;
	while (1)
	{
//y     
	//	SETX(x);

		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}

		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand);

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/


		dblBasePower_Y = dblBasePower_X;
		while(1)
		{
			YieldToPeers();
			if(m_bTestStop)
			{
				return FALSE;
			}		
			//y_step1 = 100;
			//y_step2 = -100;
			y_step1 = 1000;
			y_step2 = -1000;
			//设置步数
			//设置步数
			if (dblBasePower_Y < Point1[0])
			{
				y_step1 = step1[0];
				y_step2 = -step1[0];
			}
			if (dblBasePower_Y < Point1[1])
			{
				y_step1 = step1[1];
				y_step2 = -step1[1];
			}
			if (dblBasePower_Y < Point1[2])
			{
				y_step1 = step1[2];
				y_step2 = -step1[2];
			}
			if (dblBasePower_Y < Point1[3])
			{
				y_step1 = step1[3];
				y_step2 = -step1[3];
			}
			if (dblBasePower_Y < Point1[4])
			{
				y_step1 = step1[4];
				y_step2 = -step1[4];
			}
			if (dblBasePower_Y < Point1[5])
			{
				y_step1 = step1[5];
				y_step2 = -step1[5];
			}

			y_step = y_step1;
			if (bFlagY)
			{
				y_step = y_step2;			
			}
			y = y + y_step;
			//SETY(y);

           
		XADC=(x*MAX_DAC/60000.0);
		YADC=(y*MAX_DAC/60000.0);

       	if (b_issn1) 
		{
			dwBaseAddress="01";
		}
		else if (b_issn2)
		{
			dwBaseAddress="02";
		}
		else if(b_issn3)
		{
			dwBaseAddress="03";
		}
		else if(b_issn4)
		{
			dwBaseAddress="04";
		}
		else
		{
			return FALSE;
		}

		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		
		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand);

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/






			Sleep(200);
			dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
			iCount = 0;
			while (dblPower <= 0&&iCount<5)
			{
				Sleep(200);
				dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
				iCount++;
			}

			if (dblPower <= 0) 
			{
				LogInfo("与光功率计通讯错误,读取的光功率值过大,将取上一次的值",(BOOL)FALSE,COLOR_RED);
				dblPower = dblPrePowerY;
			}
			dblPrePowerY = dblPower;
			
			m_NTGraph.PlotXY(x,y,0);
			strMsg.Format("坐标:(%d,%d)，IL：%.3f dB\n",x,y,dblPower);
			LogInfo(strMsg);

			//比较大小，存入小的值	
			if (dblPower < dblBasePower_Y)
			{
				dblBasePower_Y = dblPower;
				bFlag1 = TRUE;
				y_base = y;
				count = 0;
			}
			else
			{
				count++;
			}
			//连续三次减小，且有新的值存入 则跳出
			if (count == 4 && bFlag1 == TRUE)
			{
				bFlag1 = FALSE;
				count = 0;
				break;
			}
			//反向寻找时，连续三次未找到变大的值
			if (count==4 && bFlag1==FALSE && bFlagY==TRUE)
			{
				count = 0;
				break;
			}
			//连续三次减小，且无新的值存入，则反向查找
			if (count == 4 && bFlag1 == FALSE)
			{
				bFlagY = TRUE;
				y = y_base;
				count = 0;
			}			
		}
		bFlagY = FALSE;
		y = y_base;
		dblBasePower_X = dblBasePower_Y;
		//x
		//SETY(y);
      	XADC=(x*MAX_DAC/60000.0);
	   YADC=(y*MAX_DAC/60000.0);


      	/*if (b_issn1) 
		{
			dwBaseAddress="00";
		}
		else if (b_issn2)
		{
			dwBaseAddress="01";
		}
		else if(b_issn3)
		{
			dwBaseAddress="02";
		}
		else
		{
			return FALSE;
		}*/

		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		
		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand);

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/





	
		while(1)
		{
			YieldToPeers();
			if(m_bTestStop)
			{
				return FALSE;
			}

			//x_step1 = 100;
			//x_step2 = -100;
			x_step1 = 1000;
			x_step2 = -1000;

			if (dblBasePower_X < Point1[0])
			{
				x_step1 = step1[0];
				x_step2 = -step1[0];
			}
			if (dblBasePower_X < Point1[1])
			{
				x_step1 = step1[1];
				x_step2 = -step1[1];
			}
			if (dblBasePower_X < Point1[2])
			{
				x_step1 = step1[2];
				x_step2 = -step1[2];
			}
			if (dblBasePower_X < Point1[3])
			{
				x_step1 = step1[3];
				x_step2 = -step1[3];
			}
			if (dblBasePower_X < Point1[4])
			{
				x_step1 = step1[4];
				x_step2 = -step1[4];
			}
			if (dblBasePower_X < Point1[5])
			{
				x_step1 = step1[5];
				x_step2 = -step1[5];
			}

			x_step = x_step1;
			if (bFlagX)
			{
				x_step = x_step2;		
			}
			x = x + x_step;
			//SETX(x);

		XADC=(x*MAX_DAC/60000.0);
	   YADC=(y*MAX_DAC/60000.0);

			
        /*if (b_issn1) 
		{
			dwBaseAddress="00";
		}
		else if (b_issn2)
		{
			dwBaseAddress="01";
		}
		else if(b_issn3)
		{
			dwBaseAddress="02";
		}
		else
		{
			return FALSE;
		}*/

		if(XADC < 0 )
        {
			strdacx.Format("%x",XADC);
			strdacx= strdacx.Right(4);
			
		}
		if (YADC > 0)
		{
			if (YADC>255)
			{
				strdacy.Format("0%x",YADC);
			}
			else if (YADC<16)
			{
				strdacy.Format("000%x",YADC);
			}
			else
			{
				strdacy.Format("00%x",YADC);
			}
		}
		if(YADC < 0)
		{
			strdacy.Format("%x",YADC);
			strdacy = strdacy.Right(4);

		}
		if(XADC > 0)
		{
			if (XADC >255)
			{
				strdacx.Format("0%x",XADC);
			}
			else if (XADC<16)
			{
				strdacx.Format("000%x",XADC);
			}
			else
			{
				strdacx.Format("00%x",XADC);
			}
		}

		if( !SetTesterXYDAC(atoi(dwBaseAddress),XADC, YADC) )
		{
			LogInfo("设置DAC失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		
		/*strCommand.Format("%s%s%s",dwBaseAddress,strdacx,strdacy);
		
		TxSTRToI2C("0145",strCommand);

		if (m_strErrorMsg != "")
		{
		    LogInfo(m_strErrorMsg);
			return FALSE;
		}
		Sleep(100);
		RxDataFromI2C("00DC");
		if (m_strErrorMsg != "")
		{
	        LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			 return FALSE;
		}*/






			Sleep(100);
			dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
			iCount = 0;
			while (dblPower <= 0 && iCount<5)
			{
				Sleep(50);
				dblPower = m_dblReferencePower[nChannel] - ReadPWMPower(nChannel);
				iCount++;
			}

			if (dblPower <= 0) 
			{
				LogInfo("与光功率计通讯错误,读取的光功率值过大!将取上一次的值",(BOOL)FALSE,COLOR_RED);
				dblPower = dblPrePowerX;
			}
			dblPrePowerX = dblPower;

			m_NTGraph.PlotXY(x,y,0);
			strMsg.Format("坐标:(%d,%d)，IL：%.3f dB\n",x,y,dblPower);
			LogInfo(strMsg);
			
			if (dblPower < dblBasePower_X)
			{
				dblBasePower_X = dblPower;
				bFlag1 = TRUE;
				x_base = x;
				count = 0;
			}
			else
			{
				count++;
			}
			if (count == 4 && bFlag1 == TRUE)
			{
				bFlag1 = FALSE;
				count = 0;
				break;
			}
			if (bFlagX==TRUE&&bFlag1==FALSE&&count==4) 
			{
				count = 0;
				break;
			}
			if (count == 4 && bFlag1 == FALSE)
			{
				bFlagX = TRUE;
				x = x_base;
				count = 0;
			}			
		}
		bFlagX = FALSE;
		x = x_base;
		if (fabs(dblBasePower_X - dblBasePower_Y)<=0.01 && dblBasePower_X < dblSetIL)
		{
			break;
		}
		nCount++;
		if (nCount>=5)
		{
			strMsg.Format("通道%d没有找到IL小于%.2fdB的点！",nChannel+1,dblSetIL);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return FALSE;				
		}
	}

	ScanPowerChannel[nChannel].VoltageX = x_base;
	ScanPowerChannel[nChannel].VoltageY = y_base;
	ScanPowerChannel[nChannel].MaxPower = dblBasePower_X;

	strMsg.Format("通道%d的最大插损为：%.2fdBm，对应的电压坐标为：(%d,%d)",nChannel+1,ScanPowerChannel[nChannel].MaxPower,
		ScanPowerChannel[nChannel].VoltageX,ScanPowerChannel[nChannel].VoltageY);
	LogInfo(strMsg,FALSE);
	
	return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonOnekeyTest2() 
{
	// TODO: Add your control notification handler code here

	MessageBox("功能开发中。。。      ：）");
	return;

	
	
}

void CMy126S_45V_Switch_AppDlg::OnButton9() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	TCHAR        m_tszAppFolder[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, m_tszAppFolder);
	CString	strFileName = "";
	strFileName.Format("%s\\data\\log.xml", m_tszAppFolder);
	DWORD   dwBuf[256];
	int     nNodeCount = 0;
	CString	strURL= _T("");
	CString	strLocalFile= _T("");
	CString	strErrMsg= _T("");
	CString strTemp = _T("");
	int     iRandNo=rand();
	CString	strPortNameBuff[42];
//	DWORD   dwWDMTemp;
	CString strTestResult="";
//	DWORD   dwWDMTestItem;
	CString m_strusername;
	CString m_strworkhouse;
	CString m_strposname;

	strURL.Format("http://172.18.1.101/amts/Sys_CheckLoginInfo.aspx?user=%s&pwd=%s&computer=""&login=Oplink\ws7-operator",m_strID,m_password);

	//strLocalFile.Format("%s\\Test.xml", m_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strFileName))
	{
		m_ctrXMLCtr.LoadXmlFile(strFileName);
	}
	else
	{
		AfxMessageBox("登录失败!");
		return;
	}
	try
	{

		//核对SN 信息是否对应
		ZeroMemory(dwBuf, 256 * sizeof(DWORD));
		nNodeCount = 0;
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "STATUS", dwBuf, nNodeCount);
		if (nNodeCount == 0)
		{
			throw "获取登录状态失败";
		}
        m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
		if (strTemp !="1")
		{
			throw "用户名或密码错误";
		}
		ZeroMemory(dwBuf, 256 * sizeof(DWORD));
		nNodeCount = 0;
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "USER_NAME", dwBuf, nNodeCount);
		if (nNodeCount == 0)
		{
			throw "获取登录状态失败";
		}
        m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
		m_strusername=strTemp;
		ZeroMemory(dwBuf, 256 * sizeof(DWORD));
		nNodeCount = 0;
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "DEP_NAME", dwBuf, nNodeCount);
		if (nNodeCount == 0)
		{
			throw "获取登录状态失败";
		}
        m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
		m_strworkhouse=strTemp;
		ZeroMemory(dwBuf, 256 * sizeof(DWORD));
		nNodeCount = 0;
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "POS_NAME", dwBuf, nNodeCount);
		if (nNodeCount == 0)
		{
			throw "获取登录状态失败";
		}
        m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
		m_strposname=strTemp;
		
		
	}
	catch (LPTSTR lpErr)
	{
		AfxMessageBox(lpErr);
		return;
	}
	m_blogin=true;
    CString m_stm1;
	m_stm1.Format("用户:%s\n部门:%s\n职位:%s\n",m_strusername,m_strworkhouse,m_strposname);
	SetDlgItemText(IDC_STATIC_LOGIN,m_stm1);

	
}

void CMy126S_45V_Switch_AppDlg::OnButton11() 
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_EDIT_ID,"");
	SetDlgItemText(IDC_EDIT_ID2,"");
	SetDlgItemText(IDC_STATIC_LOGIN,"");
	m_blogin = false;	
}

void CMy126S_45V_Switch_AppDlg::OnButton12() 
{
	CString strTemp="";
	UpdateData();

	if(!m_blogin)
	{
	    MessageBox("请登陆无纸化账号!");
		return;
	}
	//CString strcmdaddr[4] = {"17FE0","187E0","18FE0","197E0"};
	CString strcmdaddr[4] = { "0E7E0","0EFE0","0F7E0","0FFE0" };
    m_bisok = TRUE;
	if (m_strSN1 == "")
	{
		LogInfo("SN1为空，初始化请一次填入三个产品SN。",FALSE,COLOR_RED);
		return;
	}
	if (m_strSN3 == "")
	{
		LogInfo("SN2为空，初始化请一次填入三个产品SN。",FALSE,COLOR_RED);
		return;
	}
	if (m_strSN4 == "")
	{
		LogInfo("SN3为空，初始化请一次填入三个产品SN。",FALSE,COLOR_RED);
		return;
	}
	if (m_strSN5 == "")
	{
		LogInfo("SN4为空，初始化请一次填入三个产品SN。",FALSE,COLOR_RED);
		return;
	}
    m_strSN1.MakeUpper();
	m_strSN2.MakeUpper();
	m_strSN3.MakeUpper();
	m_strSN4.MakeUpper();
	m_strSN5.MakeUpper();

	FindMyDirectory(m_strItemName,m_strPN,m_strSN);

		for (int i =0 ; i<4 ; i++)
		{
			if (i==0)
			{
				OnButton1();
				if (!m_bisok)
				{
					LogInfo("无纸化数据出错，初始化失败",FALSE,COLOR_RED);
					return;
				}
			}
			if (i==1)
			{
				OnButton2();
				if (!m_bisok)
				{
					LogInfo("无纸化数据出错，初始化失败",FALSE,COLOR_RED);
					return;
				}
			}
			if (i==2) 
			{
				OnButton3();
				if (!m_bisok)
				{
					LogInfo("无纸化数据出错，初始化失败",FALSE,COLOR_RED);
					return;
				}
			}
			if (i==3) 
			{
				OnButton14();
				if (!m_bisok)
				{
					LogInfo("无纸化数据出错，初始化失败",FALSE,COLOR_RED);
					return;
				}
			}

			/*TxDataToI2C("00D6",(BYTE*)"MFGCMD",6);
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}

			Sleep(100);
			RxDataFromI2C("00DC");
			if (m_strErrorMsg != "")
			{
			      LogInfo(m_strErrorMsg,FALSE,COLOR_RED);
			    	return;
			}
            
 
		    Sleep(100);

/*			TxSTRToI2C("0166",strcmdaddr[i]); //Set memory address
        
			if (m_strErrorMsg != "")
				{
				     MessageBox("请注意，初始化数据失败");
					LogInfo("读取BIN失败。初始化数据失败，请重新下载",BOOL(FALSE),COLOR_RED);
					return ;
				}
			Sleep(100);

			RxDataFromI2C("016A"); //Get memory data*/

			DWORD nAddress;
			sscanf(strcmdaddr[i], "%x", &nAddress);

			char chData[64];
			ZeroMemory(chData,sizeof(char)*64);
			if(!GetMemoryData(nAddress,chData))
			{
				LogInfo("GetMemoryData失败!",FALSE,COLOR_RED);
				return ;
			}

			strTemp.Format("%s",chData);

			LogInfo("开始初始化产品信息",BOOL(FALSE),COLOR_GREEN);
             
			CString  strchan1,strchan2;
			
			strchan2 = strTemp.Left(4);
 
			if ((strchan2 != "FFFF" && strchan2 != "0000") && (m_strID=="22815" || m_strID=="5360" || m_strID=="17771" || m_strID=="811"))
			{	
				   int  iSel;
				   strchan1.Format("你是否确定要修改SW%d开关内部SN号为%s？",i+1,m_strSN);
				    iSel=MessageBox(strchan1,"提示！",MB_YESNO|MB_ICONQUESTION);

					if(iSel==IDYES)
					{
						m_bisinit = TRUE;

						 if (!CreateBinFileAndDownload())
							{
								LogInfo("下载数据错误！",(BOOL)FALSE,COLOR_RED);
								m_bisinit = FALSE;
								return;
							}
					    CString m_strda;
					    m_strda.Format("%s启用工程师权限将此SN初始化为 SW: %d ",m_strID,i+1);
                        LogInfo(m_strda);
						m_bisinit = FALSE;
						//return ;
					}
				 
			}
			else if (strchan2 != "FFFF" && strchan2 != "0000" )
			{
				   MessageBox("请注意，内存数据中存在数据，不能做初始化");
			       LogInfo("请注意，内存数据中存在数据，不能做初始化",BOOL(FALSE),COLOR_RED);
				   return ;
				  
			}
			else
			{
				m_bisinit = TRUE;
				if (!CreateBinFileAndDownload())
					{
						LogInfo("下载数据错误！",(BOOL)FALSE,COLOR_RED);
						m_bisinit = FALSE;
						return;
					}

				if (i==0)
				{
					OnButtonRepeat();
				}

				strchan1.Format("初始化成为SW: %d 成功",i+1);
				LogInfo(strchan1,BOOL(FALSE),COLOR_GREEN);
				m_bisinit = FALSE;
				//return ;

			}
				    
			
		}

       return ;

			

	
}


void CMy126S_45V_Switch_AppDlg::OnButton13() 
{
	// TODO: Add your control notification handler code here
	BYTE byCHCount[4] = {14,17,17,16};
	BYTE byChannel64[4][18]={1,2,3,4,5,6,7,11,12,13,14,15,16,17,0,0,0,0,
							1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,0,
							1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,0,
							2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,0,0};
	double dbTDLValue[4][18] = {0.0};
	CString strTemp="";
	int res = 1;

	UpdateData();

	if(!m_blogin)
	{
	    MessageBox("请登陆无纸化账号!");
		return;
	}
	
    m_bisok = TRUE;
	if (m_strSN1 == "")
	{
		LogInfo("SN1为空，FQC请一次填入三个产品SN。",FALSE,COLOR_RED);
		return;
	}
	if (m_strSN3 == "")
	{
		LogInfo("SN2为空，FQC请一次填入三个产品SN。",FALSE,COLOR_RED);
		return;
	}
	if (m_strSN4 == "")
	{
		LogInfo("SN3为空，FQC请一次填入三个产品SN。",FALSE,COLOR_RED);
		return;
	}
	if (m_strSN5 == "")
	{
		LogInfo("SN4为空，FQC请一次填入三个产品SN。",FALSE,COLOR_RED);
		return;
	}
    m_strSN1.MakeUpper();
	m_strSN2.MakeUpper();
	m_strSN3.MakeUpper();
	m_strSN4.MakeUpper();
	m_strSN5.MakeUpper();

	if (!CreateMfgInfoBin())
	{
		MessageBox("下载Map表失败！");
		return;
	}

	for(int nSNIndex = 0; nSNIndex < 4; nSNIndex++)
	{
		if(nSNIndex == 0)
			m_strSN = m_strSN1;
		else if(nSNIndex == 1)
			m_strSN = m_strSN3;
		else if(nSNIndex == 2)
			m_strSN = m_strSN4;
		else if(nSNIndex == 3)
			m_strSN = m_strSN5;

		for(int nPort=0; nPort<18; nPort++)
		{
			if(!GetDataFromCSV(TDL_DATA,nPort+1,&dbTDLValue[nSNIndex][nPort]))
			{
					LogInfo("读取TDL数据出错",FALSE,COLOR_RED);
					res = 0;
					return;
			}
		}
	}

	for(int nSNIndex = 0; nSNIndex < 4; nSNIndex++)
	{
		for(int nPort=0; nPort<byCHCount[nSNIndex]; nPort++)
		{
			if(nSNIndex == 0)
			{
				if(dbTDLValue[nSNIndex][byChannel64[nSNIndex][nPort]] > 0.4)
				{
					strTemp.Format("通道%d TDL大于0.4，请检查！",byChannel64[nSNIndex][nPort]);
					LogInfo(strTemp,FALSE,COLOR_RED);
					res = 0;
					//return;
				}
			}
			else if(nSNIndex == 1)
			{
				if((dbTDLValue[nSNIndex][byChannel64[nSNIndex][nPort]] + dbTDLValue[0][7]) > 0.4)
				{
					strTemp.Format("第一级通道8+第二级通道%d TDL大于0.4，请检查！",byChannel64[nSNIndex][nPort]);
					LogInfo(strTemp,FALSE,COLOR_RED);
					res = 0;
					//return;
				}
			}
			else if(nSNIndex == 2)
			{
				if((dbTDLValue[nSNIndex][byChannel64[nSNIndex][nPort]] + dbTDLValue[0][8]) > 0.4)
				{
					strTemp.Format("第一级通道9+第二级通道%d TDL大于0.4，请检查！",byChannel64[nSNIndex][nPort]);
					LogInfo(strTemp,FALSE,COLOR_RED);
					res = 0;
					//return;
				}
			}
			else if(nSNIndex == 3)
			{
				if((dbTDLValue[nSNIndex][byChannel64[nSNIndex][nPort]] + dbTDLValue[0][9]) > 0.4)
				{
					strTemp.Format("第一级通道10+第二级通道%d TDL大于0.4，请检查！",byChannel64[nSNIndex][nPort]);
					LogInfo(strTemp,FALSE,COLOR_RED);
					res = 0;
					//return;
				}
			}
			
		}
	}




	 CString dwBaseAddress;
	 int i_checkswnum;

	 for (int i = 0; i < 4; i++)
	 {
		 if (i == 0)
		 {
			 OnButton1();
			 //dwBaseAddress="17FE0";
			 dwBaseAddress = "0E7E0";

			 i_checkswnum = 1;
			 if (!m_bisok)
			 {
				 LogInfo("无纸化数据出错，初始化失败", FALSE, COLOR_RED);
				 res = 0;
				 //return;
			 }
		 }
		 if (i == 1)
		 {
			 OnButton2();
			 //dwBaseAddress="187E0";
			 dwBaseAddress = "0EFE0";

			 i_checkswnum = 2;
			 if (!m_bisok)
			 {
				 LogInfo("无纸化数据出错，初始化失败", FALSE, COLOR_RED);
				 res = 0;
				 //return;
			 }
		 }
		 if (i == 2)
		 {
			 OnButton3();
			 //dwBaseAddress="18FE0";
			 dwBaseAddress = "0F7E0";

			 i_checkswnum = 3;
			 if (!m_bisok)
			 {
				 LogInfo("无纸化数据出错，初始化失败", FALSE, COLOR_RED);
				 res = 0;
				 //return;
			 }
		 }
		 if (i == 3)
		 {
			 OnButton14();
			 //dwBaseAddress="197E0";
			 dwBaseAddress = "0FFE0";

			 i_checkswnum = 4;
			 if (!m_bisok)
			 {
				 LogInfo("无纸化数据出错，初始化失败", FALSE, COLOR_RED);
				 res = 0;
				 //return;
			 }
		 }

		 DWORD nAddress;
		 sscanf(dwBaseAddress, "%x", &nAddress);

		 char chData[64];
		 ZeroMemory(chData, sizeof(char) * 64);
		 if (!GetMemoryData(nAddress, chData))
		 {
			 LogInfo("GetMemoryData失败!", FALSE, COLOR_RED);
			 res = 0;
			 //return ;
		 }

		 m_strErrorMsg.Format("%s", chData);

		 LogInfo("开始确认产品信息", BOOL(FALSE), COLOR_GREEN);


		 m_bisinit = FALSE;

		 CString  strchan1, strchan2, strchan3, strchan4, checksn;
		 int i_chan1, i_chan2, i_chan3, i_chan4;

		 /*	strchan1.Format("%c%c",chData[18],chData[19]);
			 strchan2.Format("%c%c",chData[20],chData[21]);
			 strchan3.Format("%c%c",chData[22],chData[23]);

			 i_chan1  = strtol(strchan1,NULL,16);
			 i_chan2  = strtol(strchan2,NULL,16);
			 i_chan3  = strtol(strchan3,NULL,16);

			 checksn.Format("%c%c%c",i_chan1,i_chan2,i_chan3);*/

			 /////
		 checksn = "";
		 for (int nIndex = 0; nIndex < 24; )
		 {
			 strchan1.Format("%c%c", chData[nIndex], chData[nIndex + 1]);
			 i_chan1 = strtol(strchan1, NULL, 16);
			 strchan1.Format("%c", i_chan1);
			 checksn += strchan1;
			 nIndex = nIndex + 2;
		 }

		 LogInfo(checksn, BOOL(FALSE), COLOR_GREEN);
		 LogInfo(m_strSN.Right(3), BOOL(FALSE), COLOR_GREEN);
		 if (checksn.Find(m_strSN) != -1)  //if (checksn == m_strSN.Right(3)) 
		 {
			 LogInfo("SN号信息比对正确", BOOL(FALSE), COLOR_GREEN);
		 }
		 else
		 {
			 LogInfo("SN信息对比错误，请仔细检查填入信息", (BOOL)FALSE, COLOR_RED);
			 res = 0;
			 //return;
		 }

		 CString strCommand;
		 for (int t1 = 0;t1 < 3;t1++)
		 {
			 if (b_issn1)
			 {

				 // strCommand = "17DF0";	
				 // strCommand = "0E7E0";
				 if (t1 == 0)
				 {
					 strCommand = "0E404";
				 }
				 if (t1 == 1)
				 {
					 strCommand = "0E4FC";
				 }
				 if (t1 == 2)
				 {
					 strCommand = "0E5F4";
				 }



			 }
			 else if (b_issn2)
			 {
				 // strCommand = "185F0";	
				 // strCommand = "0EFE0";
				 if (t1 == 0)
				 {
					 strCommand = "0EC04";
				 }
				 if (t1 == 1)
				 {
					 strCommand = "0ECFC";
				 }
				 if (t1 == 2)
				 {
					 strCommand = "0EDF4";
				 }


			 }
			 else if (b_issn3)
			 {
				 //strCommand = "18DF0";	
				 //strCommand = "0F7E0";
				 if (t1 == 0)
				 {
					 strCommand = "0F404";
				 }
				 if (t1 == 1)
				 {
					 strCommand = "0F4FC";
				 }
				 if (t1 == 2)
				 {
					 strCommand = "0F5F4";
				 }

			 }
			 else if (b_issn4)
			 {
				 //strCommand = "195F0";		
				//strCommand = "0FFE0";
				 if (t1 == 0)
				 {
					 strCommand = "0FC04";
				 }
				 if (t1 == 1)
				 {
					 strCommand = "0FCFC";
				 }
				 if (t1 == 2)
				 {
					 strCommand = "0FDF4";
				 }

			 }
			 else
			 {
				 LogInfo("验证定标数据信息失败", BOOL(FALSE), COLOR_RED);
				 res = 0;
				 //return ;
			 }

		 sscanf(strCommand, "%x", &nAddress);

		 ZeroMemory(chData, sizeof(char) * 64);
		 if (!GetMemoryData(nAddress, chData))
		 {
			 LogInfo("GetMemoryData失败!", FALSE, COLOR_RED);
			 res = 0;
			 //return ;
		 }

		 strchan1.Format("%c%c", chData[0], chData[1]);
		 strchan2.Format("%c%c", chData[2], chData[3]);

		 LogInfo(strchan1, BOOL(FALSE), COLOR_GREEN);
		 LogInfo(strchan2, BOOL(FALSE), COLOR_GREEN);

		 if (strchan1 != "00" || strchan2 != "00")
		 {
			 LogInfo("验证定标数据信息成功", BOOL(FALSE), COLOR_GREEN);

		 }
		 else
		 {
			 LogInfo("验证定标数据信息失败", BOOL(FALSE), COLOR_RED);
			 res = 0;
			 //return ;
		 }
	 }
				    
			
		}

		CString strMfgInfo =  GetModuleInfo();
		CString strFWVer = strMfgInfo.Mid(8,3);
		if (strcmp(strFWVer,m_strFWVer)==0)
		{
			LogInfo("验证FW版本信息成功",BOOL(FALSE),COLOR_GREEN);
		}
		else 
		{
			LogInfo("验证FW版本信息失败",BOOL(FALSE),COLOR_RED);
			res = 0;
			//return ;
		}

		int nState = GetModuleGLUTALM();
		strTemp.Format("回读Lut Table 值为%d",nState);
		LogInfo(strTemp);
		if(nState != 0)
		{
			LogInfo("验证Lut Table信息失败",BOOL(FALSE),COLOR_RED);
			res = 0;
			//return ;
		}
		else
		{
			LogInfo("验证Lut Table信息成功",BOOL(FALSE),COLOR_GREEN);
		}

	
		LogInfo("所有信息验证成功。",BOOL(FALSE),COLOR_GREEN);

	////////////////////////////上传FQC结果
	CString strXMLFile;
	CString strErrMsg;
	CString strMsg;
	CString strURL;
	int     iRandNo=rand();
	DWORD   dwBuf[256];
	CString strProcess;
	CString strLocalFile;

	CString strErrorMsg;

	//strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&test=1",m_ModuleSN); //获取测试模板
	

	char* pcharOutString = NULL;
	char strComputerName[MAX_PATH];
	char strStationName[MAX_PATH];
	 ZeroMemory(strComputerName, MAX_PATH * sizeof(char));
	ZeroMemory(strStationName, MAX_PATH * sizeof(char));
	//ZeroMemory(strStationID, MAX_PATH * sizeof(char));
	DWORD dwLength = 256;
	GetComputerName(strComputerName, &dwLength);

	//strXMLFile.Format("%s\\Data\\%s\\%s\\%s_XML.xml", m_strNetFile, m_strItemName, m_strPN, m_ModuleSN);

	strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", m_strNetFile, m_strItemName, m_strPN, m_ModuleSN, m_ModuleSN);

	if (!m_fusion.bDownLoadItemInfo(m_ModuleSN, "MEMS-SWITCH-Module测试", "", "", FALSE, strXMLFile))
	{
		return;
	}

	//if (m_ctrXMLCtr.GetHtmlSource(strURL, strXMLFile))
	//{
	//	m_ctrXMLCtr.LoadXmlFile(strXMLFile);
	//}
	//else
	//{
	//	strMsg = "模板下载错误!";
	//	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
	//	return ;
	//}

	UpdateATMData(FQC_DATA, res);

	if (!m_fusion.bUploadXMLData(strXMLFile, &strErrorMsg))
	{
		AfxMessageBox(strErrorMsg);
		return;
	}


	if (!m_fusion.bUpdateTestResult(m_ModuleSN, &strErrorMsg))
	{
		AfxMessageBox(strErrorMsg);
		return;
	}
	LogInfo("OPC无纸化数据上传完毕！");

	if (!m_fusion.m_pFucGetStationName(strComputerName, strStationID, MAX_PATH, strStationName, MAX_PATH, &pcharOutString))
	{
		AfxMessageBox(pcharOutString);
		AfxMessageBox("获取工位ID出错");
		return;
	}
	if (!m_fusion.m_pFucTriggerProcessMoveOut(m_ModuleSN, strStationID, &pcharOutString))
	{
		AfxMessageBox(pcharOutString);
		strErrorMsg.Format("move out 出错，请检查OPC配置 - SN %s SWID %s", m_ModuleSN, strStationID);
		AfxMessageBox(strErrorMsg);
		return;
	}

	//if (!SetFileByMSMQ(strXMLFile, strErrMsg))
	//{
	//	LogInfo("FQC无纸化数据上传失败！",(BOOL)FALSE,COLOR_RED);
	//	LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
	//	return;
	//}
	LogInfo("FQC无纸化数据上传完毕！");

    return ;


	
}

BOOL CMy126S_45V_Switch_AppDlg::SendMsgToSwitch(int nTime,int nCount)
{
	CString strTemp,strCommand;
	char pbyData[20];
// 	BYTE bDir;
    char chReData[64];
	ZeroMemory(pbyData,sizeof(char)*20);
	ZeroMemory(chReData,sizeof(chReData));

	int i_checkswnum=0;

	if (b_issn1) 
	{
		i_checkswnum = 1;
	}
	else if (b_issn2)
	{
		i_checkswnum = 2;
	}
	else if(b_issn3)
	{
		i_checkswnum = 3;
	}
	else if(b_issn4)
	{
		i_checkswnum = 4;
	}
	else
	{
		return FALSE;
	}

	
	strTemp.Format("ASWTH %d %d %d %d\r",i_checkswnum,nTime,nCount,m_nCHCount);
	
	memcpy(pbyData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		MessageBox("串口没有打开！");
		return FALSE;
	}
	Sleep(50);

	if (!m_opCom.ReadBuffer(chReData,64))
	{
		LogInfo("读取错误，接收错误！");
		return FALSE;
	}
	strCommand = chReData;
	if(strCommand.Find("OK") == -1)
	{
		return FALSE;
	}

    return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SETX(int x)
{
  CString strTemp;
  char pbyData[10];
  char pbyRx[20];  

  ZeroMemory(pbyData,sizeof(char)*10);
  ZeroMemory(pbyRx,sizeof(char)*20);

  x = x*MAX_DAC/60000.0;
  strTemp.Format("setx %d\r",x);
  memcpy(pbyData,strTemp,strTemp.GetLength());
  if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
  { 
     MessageBox("串口没打开，设置x电压错误！");
     return FALSE;
  }

  return TRUE;
    
}

BOOL CMy126S_45V_Switch_AppDlg::SETY(int y)
{
	CString strTemp;
	char pbyData[10];
	char pbyRx[20];  
	
	ZeroMemory(pbyData,sizeof(char)*10);
	ZeroMemory(pbyRx,sizeof(char)*20);
	y = y*MAX_DAC/60000.0;

	strTemp.Format("sety %d\r",y);
	memcpy(pbyData,strTemp,strTemp.GetLength());
	if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{ 
		MessageBox("串口没打开，设置y电压错误！");
		return FALSE;
	}

    return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SCHN(int nchannel,int x,int y)
{
	CString strTemp;
	char pbyData[30];
	char chReData[20];
	ZeroMemory(chReData,sizeof(char)*30);
	ZeroMemory(pbyData,sizeof(char)*20);

	strTemp.Format("schn %d %d %d\r",nchannel,x,y);
	memcpy(pbyData,strTemp,strTemp.GetLength());

    if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		MessageBox("串口未打开，通道设置错误！");
		return FALSE;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		MessageBox("通道设置错误，接收错误！");
		return  FALSE;
	}
	strTemp = chReData;
	if (strTemp != "\rI: Command OK!\r>")
	{
		MessageBox("通道设置错误，回读信息错误！");
		return FALSE;
	}
    return TRUE;
}

/*BOOL CMy126S_45V_Switch_AppDlg::SWToChannel(int nChannel,int nMode,bool bSwitchTime)
{
	CString strTemp;
	char chData[10];
	char chReData[40];
	CString strValue;
	CString strInfo;
	int     nValue;
	UpdateData();

	if (m_cbComPort.GetCurSel()==1)
	{
		nChannel = nChannel + m_nCHCount; 
	}
//	double  dblValue;
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*40);
	
	strTemp.Format("SWCH %d %d\r",nMode,nChannel-1);
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		MessageBox("串口打开错误，切换开关错误！");
    	return FALSE;
	}
	Sleep(100);
	if (!m_opCom.ReadBuffer(chReData,40))
	{
		MessageBox("开关切换错误，接收错误！");
		return FALSE;
	}
	if (bSwitchTime)
    {
		Sleep(700);
		strValue = strtok(chReData,"\n\t\r");
		nValue = atoi(strValue);
		m_dblSwitchTime = (double)nValue/1000.0;
		strInfo.Format("通道切换时间为：%f ms",m_dblSwitchTime);
		LogInfo(strInfo);
    }
	return TRUE;
}*/

/*double CMy126S_45V_Switch_AppDlg::GetModuleTemp()
{
	double dblTemp = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("GTMP\r");
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("串口打开错误！",(BOOL)FALSE,COLOR_RED);
		return -99;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！",(BOOL)FALSE,COLOR_RED);
		return -99;
	}
    dblTemp = atof(chReData);	
	dblTemp = dblTemp/100.0;
	return dblTemp;

}*/

BOOL CMy126S_45V_Switch_AppDlg::DataDownLoad(CString strFileName)
{
	BOOL		bFileDone   = FALSE;
	BOOL		bFunctionOK = FALSE;
	BYTE		bDownloadOK;
	char		pbBinData[XMODEM_BLOCK_BODY_SIZE_1K + 32];
	DWORD		dwBytesRead;
	DWORD		dwCodeSizeLeft;
	DWORD		dwTotalDataCount;
	DWORD		dwLoadDataSize;
	WORD		wDownloadSize;
	WORD		wCRC16;
	CString     strMsg;
	BYTE		bPacketIndex;
	HANDLE		hBinFile = INVALID_HANDLE_VALUE;
	int			iCnt=0,iCount=0;

	try
	{
		// 1. 发命令去启动Xmodem
		//if(!m_Command.StartXmodem())
		if(!SendRevCommand("fwdl\r",&m_opCom))
			throw "启动Xmodem失败";

		// 打开bin文件
		hBinFile = CreateFile(strFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hBinFile == INVALID_HANDLE_VALUE)
			throw "Error: Open Bin file fail";

		//Get Download.bin Filesize
		dwCodeSizeLeft = GetFileSize(hBinFile, NULL);
		if(dwCodeSizeLeft == 0)
			throw "Error: Bin file is empty";

		// 2. 开始下载文件 
		bPacketIndex     = 1; 
		dwLoadDataSize   = 0;
		dwTotalDataCount = dwCodeSizeLeft;
//		PostUpdateProgressMsg(dwLoadDataSize, dwTotalDataCount);
		iCnt = dwCodeSizeLeft/XMODEM_BLOCK_BODY_SIZE_1K;
		iCnt += (dwCodeSizeLeft%XMODEM_BLOCK_BODY_SIZE_1K)/128;
		m_ctrlProgress.SetRange(0,iCnt);

		while(dwCodeSizeLeft)
		{
			iCount++;
			m_ctrlProgress.SetPos(iCount);

			//if(g_bStop)
				//throw "Warning: Stop firmware upgrade!";

			ZeroMemory(pbBinData, XMODEM_BLOCK_BODY_SIZE_1K + 32);
			
			// 以1K Xmodem下载数据，不足1K的数据要求按照128为一个block下载数据
			if(dwCodeSizeLeft >= XMODEM_BLOCK_BODY_SIZE_1K)
				wDownloadSize = XMODEM_BLOCK_BODY_SIZE_1K;
			else if(dwCodeSizeLeft >= XMODEM_BLOCK_BODY_SIZE_128)
				wDownloadSize = XMODEM_BLOCK_BODY_SIZE_128;
			else
				wDownloadSize = (short)dwCodeSizeLeft;

			dwCodeSizeLeft -= wDownloadSize;
			dwLoadDataSize += wDownloadSize;

			// File done, the last block data
			if(dwCodeSizeLeft == 0)
			{
				bFileDone = TRUE;
			}
			
			// 从指定Bin文件中读取数据
			ReadFile(hBinFile, pbBinData + XMODEM_BLOCK_DATA_INDEX, wDownloadSize, &dwBytesRead, NULL);
			if(dwBytesRead != wDownloadSize)
			{
				CloseHandle(hBinFile);
				throw "Error: Read data from bin file error";
			}
			
			if(wDownloadSize < XMODEM_BLOCK_BODY_SIZE_128)
			{
				wDownloadSize = XMODEM_BLOCK_BODY_SIZE_128;
			}

			// 封装Xmodem协议的数据block
			wCRC16 = XmodemCRC16(pbBinData + XMODEM_BLOCK_DATA_INDEX, wDownloadSize);
			if(wDownloadSize == XMODEM_BLOCK_BODY_SIZE_1K)
				pbBinData[XMODEM_BLOCK_HEAD_INDEX] = XMODEM_STX;
			else
				pbBinData[XMODEM_BLOCK_HEAD_INDEX] = XMODEM_SOH;

			pbBinData[XMODEM_BLOCK_NO_INDEX]   = bPacketIndex;
			pbBinData[XMODEM_BLOCK_NON_INDEX]  = 0xFF - bPacketIndex;
			
			pbBinData[XMODEM_BLOCK_DATA_INDEX + wDownloadSize]     = (BYTE)(wCRC16 >> 8);
			pbBinData[XMODEM_BLOCK_DATA_INDEX + wDownloadSize + 1] = (BYTE)wCRC16;			

			// 将Xmodem block数据下载进模块
			bDownloadOK = XmodemloadData(pbBinData, wDownloadSize + 5, bFileDone);
			if(XMODEM_DOWNLOAD_SUCCESS != bDownloadOK)
			{
				bFunctionOK = FALSE;
				CloseHandle(hBinFile);
				hBinFile = INVALID_HANDLE_VALUE;
			//	return bFunctionOK;
			}

			bPacketIndex ++;			
			// post the update Progress message
//			PostUpdateProgressMsg(dwLoadDataSize, dwTotalDataCount);
			YieldToPeers();	
			
		}
		bFunctionOK = TRUE;
		strMsg.Format("等待4s。。。。。。");
		LogInfo(strMsg);
		Sleep(4000);
		strMsg.Format("下载文件%s成功！",strFileName);
        LogInfo(strMsg);
		//发送复位信号
		SendCommand("RSET\r");
		LogInfo("发送复位信号！");
	}
	
	catch(TCHAR* pszErrMsg)
	{
		bFunctionOK = FALSE;
		LogInfo(pszErrMsg,TRUE);
		MessageBox(pszErrMsg, "Error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		bFunctionOK = FALSE;
		MessageBox("other error!");
	}

	if(hBinFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hBinFile);
	}

	return	bFunctionOK;

}

BOOL CMy126S_45V_Switch_AppDlg::SendRevCommand(CString command, COpComm *pcommandtask, CString setvalue)
{
	CString strStr;
	unsigned long RevCommLen=0;
	int iCnt=0;
	BOOL bFlag=0;
	char    SendBuffer[MAXCMDBT];
    char    RevBuffer[MAXCMDBT*7];
	
	memset(SendBuffer,0,sizeof(SendBuffer));
	memset(RevBuffer,0,sizeof(RevBuffer));
	
	strStr = command;
	strncpy((char *)SendBuffer,strStr.GetBuffer(strStr.GetLength()),strStr.GetLength());
	
	if(!pcommandtask->WriteBuffer(SendBuffer,strStr.GetLength()))
	{
		MessageBox("命令发送失败！");
		return FALSE;
	}
	
	memset(RevBuffer,0,sizeof(RevBuffer));
	
	Sleep(5000);
	//	if(!pcommandtask->ReadBuffer(RevBuf,MAXCMDBT*7,&dwReRead))
	if(!pcommandtask->ReadBuffer(RevBuffer,MAXCMDBT*7))
	{
		MessageBox("命令接收错误！");
		return FALSE;
	}
	
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::XmodemloadData(char *pbBinData, WORD wDownloadSize, BOOL bFileDone)
{
	char	byTempBuf[MAX_BUF_DATA];
	int		nTimeOut;
	int		nRetry = 0;
	DWORD	dwReadLength=0;
	
	// Send Packet data
	if(!m_opCom.WriteBuffer(pbBinData, wDownloadSize))
	{
		MessageBox("RS232发送数据错误!", "提示", MB_OK|MB_ICONERROR);
		return	XMODEM_COMMUNICATION_FAIL;
	}
	
	// Check Response
    while (1)
    {
        Sleep(50);
		nTimeOut = 0;
	 	if(m_opCom.ReadBuffer(byTempBuf, MAX_BUF_DATA,&dwReadLength))
        {
            // Affirmative Answer
            if(byTempBuf[dwReadLength - 1] == XMODEM_ACK)
			{
                break;
			}
            // Negative Answer
            else if(byTempBuf[dwReadLength - 1] == XMODEM_NAK)
            {
				// Send Packet data again
				if(!m_opCom.WriteBuffer(pbBinData, wDownloadSize))
				{
					MessageBox("RS232发送数据错误!", "提示", MB_OK|MB_ICONERROR);
					return XMODEM_COMMUNICATION_FAIL;
				}

                nRetry++;
				if(nRetry > 10)
				{
					// Cancel Transfer
					byTempBuf[0] = XMODEM_CAN;
					byTempBuf[1] = XMODEM_CAN;
					byTempBuf[2] = XMODEM_CAN;

					if(!m_opCom.WriteBuffer(byTempBuf, 3))
					{
						MessageBox("发送3个CAN取消Xmodem下载失败!", "提示", MB_OK|MB_ICONERROR);
					}
					else
					{
						MessageBox("发送3个CAN取消Xmodem下载，请重新再下载一次!", "提示", MB_OK|MB_ICONQUESTION);
					}
					
					return	XMODEM_COMMUNICATION_FAIL;
				}
            }
			else if(byTempBuf[dwReadLength - 1] == XMODEM_CAN)
			{
				MessageBox("模块发生CAN字符上来取消Xmodem下载，请重新下载一次!", "提示", MB_OK|MB_ICONERROR);
				return	XMODEM_COMMUNICATION_FAIL;
			}
			else
			{
				MessageBox("模块发送过来的回应信息不正常，请重启模块后再下载一次!", "提示", MB_OK|MB_ICONERROR);
				return	XMODEM_COMMUNICATION_FAIL;
			}
		}

		nTimeOut ++;
		if(nTimeOut >= 210)
		{
			MessageBox("等待Xmodem回应超过10s，请复位模块，重新下载一次！！", "提示", MB_OK|MB_ICONERROR);
			return XMODEM_COMMUNICATION_FAIL;
		}
    }

    // Transfer Done
	if(bFileDone)
	{
		byTempBuf[0] = XMODEM_EOT;
		byTempBuf[1] = XMODEM_EOT;
		byTempBuf[2] = XMODEM_EOT;
		
		if(!m_opCom.WriteBuffer(byTempBuf, 3))
		{
			MessageBox("发送3个EOT完成Xmodem下载失败", "提示", MB_OK|MB_ICONERROR);
			return	XMODEM_COMMUNICATION_FAIL;
		}
		
		while (1)
		{
			Sleep(50);
			nTimeOut = 0;
			ZeroMemory(byTempBuf, MAX_BUF_DATA);
			if(m_opCom.ReadBuffer(byTempBuf, MAX_BUF_DATA,&dwReadLength))
			{
				// Affirmative Answer
				if(byTempBuf[0] == XMODEM_ACK)
				{
					if(strcmp(byTempBuf + 1, "Successful") == 0)
						return	XMODEM_DOWNLOAD_SUCCESS;
					else
						return	XMODEM_DOWNLOAD_FAIL;
				}
				else if(byTempBuf[0] == XMODEM_NAK)
				{
					return	XMODEM_DOWNLOAD_FAIL;
				}
			}

			nTimeOut ++;
			if(nTimeOut >= 210)
			{
				MessageBox("等待Xmodem回应超过10s，请复位模块，重新下载一次！！", "提示", MB_OK|MB_ICONERROR);
				return XMODEM_COMMUNICATION_FAIL;
			}
		}
	}

    return XMODEM_DOWNLOAD_SUCCESS;

}

WORD CMy126S_45V_Switch_AppDlg::XmodemCRC16(char *pbStream, WORD wLength)
{
	WORD    wCRCValue;
    WORD    wIndex;
	
    wCRCValue = 0;
	
    while(wLength > 0)
    {
        wCRCValue = wCRCValue ^ (WORD)(*pbStream ++) << 8;
		
        for(wIndex = 0; wIndex < 8; wIndex ++)
        {
            if(wCRCValue & 0x8000)    
				wCRCValue = (wCRCValue << 1) ^ 0x1021;
            else
				wCRCValue = wCRCValue << 1;
        }
		
        wLength --;
    }
	
    return wCRCValue;

}

void CMy126S_45V_Switch_AppDlg::SendCommand(CString strCommand)
{
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	memcpy(chData,strCommand,strCommand.GetLength());
    if(!m_opCom.WriteBuffer(chData,strCommand.GetLength()))
    {
		MessageBox("串口打开错误！");
		return ;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		MessageBox("读取错误，接收错误！");
		return ;
	}

}

/*BOOL CMy126S_45V_Switch_AppDlg::SendProductScanZone(int nMinX, int nMaxX, int nMinY, int nMaxY, int nStep)
{
	CString strCommand;
	int     nIndex=0;
	char    chCommand[256];
	ZeroMemory(chCommand,sizeof(chCommand));
	//发送扫描命令
	strCommand.Format("SCAN 1 %d %d %d %d %d\r",nMinX,nMaxX,nMinY,nMaxY,nStep);
	//LogInfo(strCommand);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!m_opCom.WriteBuffer(chCommand,strCommand.GetLength()))
	{
		LogInfo("发送扫描命令失败！");
		return FALSE;
	}
	Sleep(100);
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SETXDAC(int dacX)
{
	CString strCommand;
    char    chCommand[50];
	ZeroMemory(chCommand,sizeof(chCommand));
	CString strValue;
	char  chValue[10];
	CString strMsg;
	ZeroMemory(chValue,sizeof(chValue));
	strCommand.Format("setx %d\r",dacX);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!m_opCom.WriteBuffer(chCommand,strCommand.GetLength()))
	{
		LogInfo("setx发送失败！");
		return FALSE;
	}
	
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::SETYDAC(int dacY)
{
	CString strCommand;
    char    chCommand[50];
	ZeroMemory(chCommand,sizeof(chCommand));
	CString strValue;
	char  chValue[10];
	CString strMsg;
	ZeroMemory(chValue,sizeof(chValue));
	
	strCommand.Format("sety %d\r",dacY);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!m_opCom.WriteBuffer(chCommand,strCommand.GetLength()))
	{
		LogInfo("sety发送失败！");
		return FALSE;
	}
	
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::SetMode(BYTE byMode)
{
	CString strTemp;
    char pbyData[10];
    char pbyRx[128];  

	ZeroMemory(pbyData,sizeof(char)*10);
	ZeroMemory(pbyRx,sizeof(char)*128);

	//TTL需求
	if(m_nInterfaceType!=0)
	{
		strTemp.Format("smod %d\n",byMode);
		memcpy(pbyData,strTemp,strTemp.GetLength());
		if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
		{ 
	       LogInfo("串口没打开，设置模式错误！",(BOOL)FALSE,COLOR_RED);
		   return FALSE;
		}
		Sleep(200);

		if (!m_opCom.ReadBuffer(pbyRx,128))
		{
  			LogInfo("读取数据错误！",(BOOL)FALSE,COLOR_RED);
    		return FALSE;
		}
		LogInfo(pbyRx,TRUE);
		m_bMode=byMode;
	}

    return TRUE;
}*/

void CMy126S_45V_Switch_AppDlg::OnButton14() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
    m_strSN =  m_strSN5;

	m_strItemName = "终测";
	m_strPN = "1831782693";
	m_strSpec = "Z6193";
	CString strTestItem = "Initial";

	//获取无纸化PN
	CString strFileName;
	CString strURL,strTemp;
	CString strLocalFile;
	CString strMsg;
	//获取配置信息
	CString strFile;
	CString strValue;
	CString strKey;
	int     nNodeCount;
	CString strErrMsg;
	DWORD   dwBuf[256];
	CString strWDLFile;
	int     nRand = rand();
	m_bGetSNInfo = FALSE;
	GetLocalTime(&m_StartTime);
	SetDlgItemText(IDC_EDIT_SPEC,m_strSpec);

	/*
	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&showdata=1&randno=%d",m_strSN,nRand);	
	//获得XML模板文件
	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}


	//设置初测还是终测，获取当前工序
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PROCESS_TYPE", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化工序信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);	
	if (stricmp(strTemp,"EBOX_MEMSSWITCH06")==0)
	{
		m_strItemName = "终测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测2");
		m_cbTestItem.SetCurSel(0);
	}
	else if (stricmp(strTemp,"EBOX_MEMSSWITCH19")==0  || stricmp(strTemp,"")==0)
	{
		m_strItemName = "初测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
		m_cbTestItem.SetCurSel(1);
	}
	else if (stricmp(strTemp,"EBOX_MEMSSWITCH07")==0)
	{
		m_strItemName = "初测";
		SetDlgItemText(IDC_COMBO_TEST_ITEM,"终测1");
		m_cbTestItem.SetCurSel(1);
	}
	else
	{
		m_strItemName = strTemp;
		strMsg.Format("工序代号%s不在该软件测试范围内，请确认！",strTemp);
		LogInfo(strMsg,FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}
	
	strMsg.Format("从无纸化获取到的当前工序为:%s(%s)",m_strItemName,strTemp);
	LogInfo(strMsg);

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}
	nRand = rand();
	//重新获取模板
	CString strTestItem;
	if (m_strItemName=="初测")
	{
		strTestItem = "Initial";
	//	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&pretest=1&showdata=1&randno=%d",m_strSN,nRand);
	}
	else if (m_strItemName=="终测")
	{
		strTestItem = "Final";
	//	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&test=1&showdata=1&randno=%d",m_strSN,nRand);
	}		
	//获得XML模板文件
//	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
//	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
//	{
//		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
//	}
//	else
//	{
//		strMsg = "模板下载错误!";
//		MessageBox(strMsg);
//			m_bisok = FALSE;
//		return;
//	}
	CString strPN;
	//PN信息
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化PN信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strPN, dwBuf[0]);
	m_strGetPN = strPN;
	//SPEC信息
	CString strSpec;
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SPEC", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化SPEC信息!";
		MessageBox(strMsg);
			m_bisok = FALSE;
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strSpec, dwBuf[0]);
	m_strSpec = strSpec;
	strMsg.Format("从无纸化获取到的Spec:%s,PN:%s",strSpec,strPN);
	LogInfo(strMsg);
	SetDlgItemText(IDC_EDIT_SPEC,strSpec);

	m_strPN = m_strGetPN;

	    DWORD   dwWDMTemp;
	DWORD   dwBufTemp[256];
	DWORD   dwBufCH[256];
	CString  strNode;
	CString  strItem;
	CString strchani;
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
 		return ;
 	}
	dwWDMTemp = dwBuf[0];

	ZeroMemory(dwBufTemp, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwBufTemp, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		m_bisok = FALSE;
		return ;
	}
    int nTempIndex = 0;
	for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
		{
			strTemp="";
			dwWDMTemp = dwBufTemp[nTempIndex]; 
			m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
			if (strTemp=="room") //常温
			{
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				
				strNode ="CHSelect";
				
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBufCH, nNodeCount,dwWDMTemp);
				
				for(int nCH = 1;nCH<3;nCH++)
				{
					strItem.Format("SW%d",nCH+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strItem, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount == 0)
					{
						strMsg.Format("%s-%s结点不存在",strNode,strItem);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						m_bisok = FALSE;
						return ;
					}
					m_ctrXMLCtr.GetNodeAttr("RESULT", strchani, dwBuf[0]);

					if (strchani != "")
					{
						if (nCH==1)
						{
							i_chansw2 =  atoi(strchani);	
						}
						if (nCH==2) 
						{
							i_chansw3 =  atoi(strchani);					
						}
					}
	       
				
				}
			}
		}*/

	if(!GetPortInfo(m_strPN,m_strSpec,strTestItem))
	{
			m_bisok = FALSE;
		return;
	}
	strMsg.Format("入光通道数为：%d,出光通道数为：%d",m_nSwitchComPortCount,m_nCHCount);
	LogInfo(strMsg);
	if (!m_strSelectRule.IsEmpty())
	{
		if (m_strItemName=="初测")
		{
			strMsg.Format("通道挑选规则：%s",m_strSelectRule);
			LogInfo(strMsg);
		}
	}
	//加载通道配置
	m_cbChannel.ResetContent();
	for (int nIndex=0;nIndex<=m_nCHCount;nIndex++)
	{
		strValue.Format("%d",nIndex);
		m_cbChannel.AddString(strValue);
	}
	m_cbChannel.SetCurSel(0);
	
	SetDlgItemText(IDC_COMBO_COMPORT,"COM0");
	m_cbComPort.SetCurSel(0);
	UpdateWindow();

	if (m_nWLCount>1)
	{
		//加载波长配置
		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
			
		//加载WL波长
		m_cbSetWL.ResetContent();
		strValue.Format("%.0f",m_pdblWavelength);
		m_cbSetWL.AddString(strValue);
		for (int i=0;i<m_nWLCount;i++)
		{
		//	m_dblWL[i] = atof(strValue);
			strValue.Format("%.2f",m_dblWL[i]);
			m_cbSetWL.AddString(strValue);
		}
		m_cbSetWL.SetCurSel(0);

		GetDlgItem(IDC_STATIC_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SET_WL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WDL_TEST)->EnableWindow(TRUE);
	}

	if (stricmp(m_strSpec,"Z4767")==0)
	{
		if (stricmp(m_strGetPN,"77501354A2")==0||stricmp(m_strGetPN,"77501363A2")==0) //77501363A2 for Z4671
		{
			m_strItemName="初测"; //按初测处理
			GetDlgItem(IDC_STATIC_CHSELECT)->ShowWindow(TRUE);
    		GetDlgItem(IDC_COMBO_TEST_CH)->ShowWindow(TRUE);
		}
	}
	
	GetTempConfig();
	SetDlgItemText(IDC_EDIT_PN,m_strPN);
	UpdateWindow();

	if (m_nSwitchComPortCount == 2)
	{
		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(FALSE);
	}

	/*if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
			m_bisok = FALSE;
		return;
	}*/

	GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(TRUE);
//	GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(TRUE);
	if (m_strItemName=="")
	{
		GetDlgItem(IDC_BUTTON_RL_TEST2)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RE_IL)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DARK_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_ISO_TEST)->ShowWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_SWITCH_TIME_TEST)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SWITCH_TIME_CH)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SCAN)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWNLOAD_BIN)->ShowWindow(FALSE);
	}
	m_bGetSNInfo = TRUE;	
	CString strXMLFile;
	if (m_bIfSaveToServe)
	{
		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", m_strNetFile,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	else 
	{
		strXMLFile.Format("%s\\Data\\%s\\%s\\%s\\%s_XML.xml", g_tszAppFolder,m_strItemName,m_strPN,m_strSN,m_strSN);
	}
	CopyFile(strLocalFile,strXMLFile,FALSE); //COPY 模板
	strMsg.Format("基本信息获取完毕！",FALSE,COLOR_BLUE);
	LogInfo(strMsg);

	GetDlgItem(IDC_EDIT_SN3)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN4)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_SN5)->EnableWindow(TRUE);
	b_issn1 = FALSE;
	b_issn2 = FALSE;
	b_issn3 = FALSE;
	b_issn4 = TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SendProductScanZone(int nSWIndex, int nMinX, int nMaxX, int nMinY, int nMaxY, int nStep)
{
	CString strCommand;
	int     nIndex=0;
	char    chCommand[256];
	char chReData[20];
	
	ZeroMemory(chReData,sizeof(char)*20);
	ZeroMemory(chCommand,sizeof(chCommand));
	//发送扫描命令
	strCommand.Format("SCAN %d %d %d %d %d %d\r",nSWIndex,nMinX,nMaxX,nMinY,nMaxY,nStep);
	//LogInfo(strCommand);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!m_opCom.WriteBuffer(chCommand,strCommand.GetLength()))
	{
		LogInfo("发送扫描命令失败！");
		return FALSE;
	}
	Sleep(100);

	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！");
		return FALSE;
	}
	strCommand = chReData;
	if(strCommand.Find("OK") == -1)
	{
		return FALSE;
	}

	return TRUE;
}

double CMy126S_45V_Switch_AppDlg::GetModuleTemp()
{
	double dblTemp = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("GTMP\r");
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("发送命令失败！");
		return -99;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！");
		return -99;
	}
    dblTemp = atof(chReData);	
	dblTemp = dblTemp/100.0;
	return dblTemp;

}

CString CMy126S_45V_Switch_AppDlg::GetFWVer()
{
	double dblTemp = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("OPLK\r");
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("发送命令失败！");
		return "-99";
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！");
		return "-99";
	}
    strTemp = chReData;	
	return strTemp;

}

int CMy126S_45V_Switch_AppDlg::GetScanState(BYTE btSWIndex)
{
	int nState = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("SCAN %d\r",btSWIndex);
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("发送命令失败！");
		return -1;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！");
		return -1;
	}
    nState = atoi(chReData);	
	return nState;

}

int CMy126S_45V_Switch_AppDlg::GetASWTHState()
{
	int nState = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("ASWTH\r");
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("发送命令失败！");
		return -1;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！");
		return -1;
	}
    nState = atoi(chReData);	
	return nState;

}

BOOL CMy126S_45V_Switch_AppDlg::SetTesterXYDAC(int nSWIndex,int x, int y)
{
	CString strCommand;
	int     nIndex=0;
	char    chCommand[256];
	char chReData[20];
	
	ZeroMemory(chReData,sizeof(char)*20);
	ZeroMemory(chCommand,sizeof(chCommand));
	strCommand.Format("SETXY %d %d %d\r",nSWIndex,x,y);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!m_opCom.WriteBuffer(chCommand,strCommand.GetLength()))
	{
		LogInfo("发送扫描命令失败！");
		return FALSE;
	}
	Sleep(100);

	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！");
		return FALSE;
	}
	strCommand = chReData;
	if(strCommand.Find("OK") == -1)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SetRSET()
{
	CString strCommand;
	int     nIndex=0;
	char    chCommand[256];
	char chReData[20];
	
	ZeroMemory(chReData,sizeof(char)*20);
	ZeroMemory(chCommand,sizeof(chCommand));
	strCommand.Format("RSET\r");
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!m_opCom.WriteBuffer(chCommand,strCommand.GetLength()))
	{
		LogInfo("发送命令失败！");
		return FALSE;
	}
	Sleep(100);

	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！");
		return FALSE;
	}
	strCommand = chReData;
	if(strCommand.Find("OK") == -1)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SetSWChannel(int nSWIndex,int nCHIndex)
{
	CString strCommand;
	int     nIndex=0;
	char	chReData[40];
	char    chCommand[256];
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chReData,sizeof(char)*40);
	strCommand.Format("SWCH %d %d\r",nSWIndex,nCHIndex);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!m_opCom.WriteBuffer(chCommand,strCommand.GetLength()))
	{
		LogInfo("发送扫描命令失败！");
		return FALSE;
	}
	Sleep(100);
	if (!m_opCom.ReadBuffer(chReData,40))
	{
		LogInfo("开关切换错误，接收错误！");
		return FALSE;
	}

	strCommand = chReData;
	if(strCommand.Find("OK") == -1)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::GetMemoryData(DWORD dwAddress,char *pchData)
{
	int nState = 1;
	CString strTemp;
	char chData[64];
	char chReData[64];
	
	ZeroMemory(chData,sizeof(char)*64);
	ZeroMemory(chReData,sizeof(char)*64);
	
	strTemp.Format("MEM %d\r",dwAddress);
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("发送扫描命令失败！");
		return 0;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,64))
	{
		LogInfo("接收错误！");
		return 0;
	}
    memcpy(pchData,chReData,64);	
	return nState;

}

BOOL CMy126S_45V_Switch_AppDlg::CreateMfgInfoBin()
{
	CString   strMsg;
	CString   strPN;
	int       count;
	char      chValue[MAX_LINE];
	PBYTE 	  pbswInfo;
	stMfgInfo m_stMfgInfo;

	pbswInfo = (PBYTE)&m_stMfgInfo;
	memset(&m_stMfgInfo, 0x00, sizeof(m_stMfgInfo));

		//bBundleTag
	count = 0;
    CString strBundle = "OPLINK";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strBundle,strBundle.GetLength());
    for (int i=0;i<strBundle.GetLength();i++)
	{
		m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(chValue[i]);
    }	
	//bProductType
	count = 8;
	CString strProductType = "SWITCH";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strProductType,strProductType.GetLength());
    for (int i=0;i<strProductType.GetLength();i++)
	{
		m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(chValue[i]);
    }
	
	DWORD dwHdrCRC32 = 0;
	count = 16;
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>24);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>16);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>8);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32);

	DWORD dwBundleSize = 0x04A0;
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>24);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>16);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize>>8);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwBundleSize);
	//wBundleHdrSize
	WORD	wBundleHdrSize = 128;
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(wBundleHdrSize>>8);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(wBundleHdrSize);
	//wImageCount
	WORD wImageCount = 1;
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(wImageCount>>8);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(wImageCount);
	//
	//bBundleVersion
	count = 32;
	ZeroMemory(chValue,sizeof(chValue));
//	CString strtmpsn ="SUPERSN";
	memcpy(chValue,m_strSN,m_strSN.GetLength());
//	memcpy(chValue, strtmpsn, strtmpsn.GetLength());
    for (int i=0;i<m_strSN.GetLength();i++)
	{
		m_stMfgInfo.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	//bPartNumber
	count = 48;
	//strPN = "14538_1x64_MEMSW";
	strPN = "14538_1x64MemsSw";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strPN,strPN.GetLength());
    for (int i=0;i<strPN.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stMfgInfo.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	// Serial Number of this product,
    count = 80;
	ZeroMemory(chValue,sizeof(chValue));
	CString strtmpsn ="SUPERSN";
	//memcpy(chValue, m_strSN, m_strSN.GetLength());
	memcpy(chValue, strtmpsn, strtmpsn.GetLength());
	//memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (int i=0;i<strtmpsn.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stMfgInfo.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }
	// The time at which this bundle file generated
	SYSTEMTIME st;
	GetSystemTime(&st);
	CString strTime;
	strTime.Format("%04d.%02d.%02d.%02d.%02d",
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	count = 112;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strTime,strTime.GetLength());
    for (int i=0;i<strTime.GetLength();i++)
	{
	//	pByteBinData[count++] = (BYTE)(chValue[i]);
		m_stMfgInfo.BUNDLEHEADER[count++]= (BYTE)(chValue[i]);
    }


		m_stMfgInfo.BUNDLEHEADER[count++] = 0x49;
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x42;
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x46;
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x48;
	//ImageType
    	m_stMfgInfo.BUNDLEHEADER[count++] = 0x86;
	//ImageHitless
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x00;
	//StorageID
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x01;
	//ImageIndex
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x01;
	//dwImageVersion
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x00;
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x00;
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x00;
		m_stMfgInfo.BUNDLEHEADER[count++] = 0x00;
	//dwTimeStamp
		m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(st.wMonth);
		m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(st.wDay);
		m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(st.wHour);
		m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(st.wMinute);
    
	//dwBaseAddress
	DWORD dwBaseAddress=0x0D800;

	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>24);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>16);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress>>8);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwBaseAddress);


	count = 152;
	//  DWORD dwImageSize = dwBinsize-160; //总长度减去文件头长度
	DWORD dwImageSize = 1024; //总长度减去文件头长度
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageSize >> 24);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageSize >> 16);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageSize >> 8);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageSize);

	m_stMfgInfo.BUNDLEHEADER[count++] = 0x01;
	m_stMfgInfo.BUNDLEHEADER[count++] = 0x00;
	m_stMfgInfo.BUNDLEHEADER[count++] = 0x00;
	m_stMfgInfo.BUNDLEHEADER[count++] = 0x00;

	count = 0;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (int i=0;i<m_strSN.GetLength();i++)
	{
		m_stMfgInfo.pchSN[count++]= (BYTE)(chValue[i]);
    }

	count = 0;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strPN,m_strPN.GetLength());
    for (int i=0;i<m_strPN.GetLength();i++)
	{
		m_stMfgInfo.pchPN[count++]= (BYTE)(chValue[i]);
    }

	m_stMfgInfo.wMfgID = 0x01;
	m_stMfgInfo.wFwVer = 0x0100;
	m_stMfgInfo.wPCBAVer= 0x0100;
	m_stMfgInfo.wReserved0 = 0x00;

	m_stMfgInfo.dwMfgDate = (st.wYear << 16) + (st.wMonth << 8) + st.wDay;
	m_stMfgInfo.dwCalDate = (st.wYear << 16) + (st.wMonth << 8) + st.wDay;
	m_stMfgInfo.wSwitchType = 0x40;
	strcpy(m_stMfgInfo.pchGUIVer, "1.0.0.0");

	//得到Switch SN
	CString  strchan1;
	int		 i_chan1;
	CString  strReadSN="";
	char chData[64];

	//SN1
	ZeroMemory(chData,sizeof(char)*64);
	
	if(!GetMemoryData(0x0E7E0,chData))
	{
		LogInfo("GetMemoryData失败!",FALSE,COLOR_RED);
		return FALSE;
	}
	strReadSN="";
	for(int nIndex=0; nIndex<24; )
	{
		strchan1.Format("%c%c",chData[nIndex],chData[nIndex+1]);
		i_chan1  = strtol(strchan1,NULL,16);
		strchan1.Format("%c",i_chan1);
		strReadSN += strchan1;
		nIndex = nIndex + 2;
	}
	count = 0;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strReadSN,strReadSN.GetLength());
    for (int i=0;i<strReadSN.GetLength();i++)
	{
		m_stMfgInfo.pchDevice1SN[count++]= (BYTE)(chValue[i]);
    }

	//SN2
	ZeroMemory(chData,sizeof(char)*64);
	if(!GetMemoryData(0x0EFE0,chData))
	{
		LogInfo("GetMemoryData失败!",FALSE,COLOR_RED);
		return FALSE;
	}
	strReadSN="";
	for(int nIndex=0; nIndex<24; )
	{
		strchan1.Format("%c%c",chData[nIndex],chData[nIndex+1]);
		i_chan1  = strtol(strchan1,NULL,16);
		strchan1.Format("%c",i_chan1);
		strReadSN += strchan1;
		nIndex = nIndex + 2;
	}
	count = 0;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strReadSN,strReadSN.GetLength());
    for (int i=0;i<strReadSN.GetLength();i++)
	{
		m_stMfgInfo.pchDevice2SN[count++]= (BYTE)(chValue[i]);
    }
	
	//SN3
	ZeroMemory(chData,sizeof(char)*64);
	if(!GetMemoryData(0x0F7E0,chData))
	{
		LogInfo("GetMemoryData失败!",FALSE,COLOR_RED);
		return FALSE;
	}
	strReadSN="";
	for(int nIndex=0; nIndex<24; )
	{
		strchan1.Format("%c%c",chData[nIndex],chData[nIndex+1]);
		i_chan1  = strtol(strchan1,NULL,16);
		strchan1.Format("%c",i_chan1);
		strReadSN += strchan1;
		nIndex = nIndex + 2;
	}
	count = 0;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strReadSN,strReadSN.GetLength());
    for (int i=0;i<strReadSN.GetLength();i++)
	{
		m_stMfgInfo.pchDevice3SN[count++]= (BYTE)(chValue[i]);
    }

	//SN4
	ZeroMemory(chData,sizeof(char)*64);
	if(!GetMemoryData(0x0FFE0,chData))
	{
		LogInfo("GetMemoryData失败!",FALSE,COLOR_RED);
		return FALSE;
	}
	strReadSN="";
	for(int nIndex=0; nIndex<24; )
	{
		strchan1.Format("%c%c",chData[nIndex],chData[nIndex+1]);
		i_chan1  = strtol(strchan1,NULL,16);
		strchan1.Format("%c",i_chan1);
		strReadSN += strchan1;
		nIndex = nIndex + 2;
	}
	count = 0;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strReadSN,strReadSN.GetLength());
    for (int i=0;i<strReadSN.GetLength();i++)
	{
		m_stMfgInfo.pchDevice4SN[count++]= (BYTE)(chValue[i]);
    }

	if (m_bischange)
	{
		int i_checkswnum = 0;

		if (b_issn1)
		{
			i_checkswnum = 1;
		}
		else if (b_issn2)
		{
			i_checkswnum = 2;
		}
		else if (b_issn3)
		{
			i_checkswnum = 3;
		}
		else if (b_issn4)
		{
			i_checkswnum = 4;
		}
		else
		{
			return FALSE;
		}

		/////////////////////////////////////////////////////////////////////
		DWORD dwAddreesMap = 0x0D900;
		char chData[64];
		CString strchan;
		int i_chan;

		for (int swReadIndex = 0; swReadIndex < SWITCH_NUM; swReadIndex++)
		{
			//dwAddreesMap = 0x1F800 + swReadIndex * 32;
			dwAddreesMap = 0x0D900 + swReadIndex * 32;
			ZeroMemory(chData, sizeof(char) * 64);
			if (!GetMemoryData(dwAddreesMap, chData))
			{
				LogInfo("GetMemoryData失败!", FALSE, COLOR_RED);
				return FALSE;
			}
			for (int nCHIndex = 0; nCHIndex<32; nCHIndex++)
			{
				strchan.Format("%c%c", chData[nCHIndex * 2], chData[nCHIndex * 2 + 1]);
				i_chan = strtol(strchan, NULL, 16);
				m_stMfgInfo.bChMapping[swReadIndex][nCHIndex] = i_chan;
			}
		}
		//////////////////////////////////////////////////////////////////////

		m_stMfgInfo.bChMapping[i_checkswnum - 1][m_startchange1 - 1] = m_startchange2;
		m_stMfgInfo.bChMapping[i_checkswnum - 1][m_startchange2 - 1] = m_startchange1;

	}
	else
	{
		for (int nSWIndex = 0; nSWIndex < SWITCH_NUM; nSWIndex++)
		{
			for (int nCHIndex = 0; nCHIndex < 32; nCHIndex++)
			{
				m_stMfgInfo.bChMapping[nSWIndex][nCHIndex] = nCHIndex + 1;
			}
		}
	}


	for (int nCHIndex = 0; nCHIndex < MAX_SW_PORT_NUM + 1; nCHIndex++)
	{
		for (int nSWIndex = 0; nSWIndex < SWITCH_NUM; nSWIndex++)
		{
			m_stMfgInfo.bPortVsSwCh[nCHIndex][nSWIndex] = c_st64PortMap.bPortVsSwCh[nCHIndex][nSWIndex];
		}
	}

	DWORD dwImageCRC32 = 0;
	m_CRC32.InitCRC32();
	for (int i = 160;i < 1180; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
	dwImageCRC32 = ~dwImageCRC32;
	m_stMfgInfo.dwCRC32 = dwImageCRC32;

	m_CRC32.InitCRC32();
 	for (int i = 160;i < 1184; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
 	dwImageCRC32 = ~dwImageCRC32;
	count = 148;

	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>24);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>16);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32>>8);
    m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwImageCRC32);

	//计算header的CRC
	m_CRC32.InitCRC32();
 	for (int i = 20;i < 128; i++)
 	{
 		dwHdrCRC32 = m_CRC32.GetThisCRC(pbswInfo[i]);
 	}
 	dwHdrCRC32 = ~dwHdrCRC32;
	count = 16;

	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>24);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>16);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32>>8);
	m_stMfgInfo.BUNDLEHEADER[count++] = (BYTE)(dwHdrCRC32);

	strMsg.Format("%s_MfgInfoBin.bin",m_strSN);
	if(!WriteBinFile(strMsg,"",1184,pbswInfo))
	{
		return FALSE;
	}
	
	return TRUE;
}

int CMy126S_45V_Switch_AppDlg::GetModuleGLUTALM()
{
	int nState = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("GLUTALM\r");
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("发送命令失败！");
		return -99;
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！");
		return -99;
	}	
	strTemp = chReData;
	strTemp = (strTemp.Left(6)).Right(4);
	sscanf(strTemp, "%x", &nState);
	return nState;

}

CString CMy126S_45V_Switch_AppDlg::GetModuleInfo()
{
	double dblTemp = 0;
	CString strTemp;
	char chData[10];
	char chReData[512];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*512);
	
	strTemp.Format("INFO\r");
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("发送命令失败！");
		return "-99";
	}
	Sleep(50);
	if (!m_opCom.ReadBuffer(chReData,512))
	{
		LogInfo("读取错误，接收错误！");
		return "-99";
	}
    strTemp = chReData;	
	return strTemp;

}

void CMy126S_45V_Switch_AppDlg::OnButton15()
{
	CString strMsg;
	CString strURL;
	CString strLocalFile;
	int     iRandNo = rand();
	DWORD   dwBuf[256];
	DWORD   dwTmptBuf[256];
	DWORD   dwWDMTemp;
	BOOL   b_fusion = TRUE;
	CString strErrMsg;
	int     nNodeCount;
	CString strTemperature;
	CString strTemp;
	CString strMax;
	CString strMin;
	CString strKey;
	CString strProcess;
	CString strPcname;
	CString strPcID;
	BOOL b_res;
	char* pcharOutString = NULL;

	CString strPN = "UOFS164U00ADV01G-2";
	TCHAR tchrSubSNList[1024];
	TCHAR tchrStationID[1024];
	TCHAR tchrStationname[1024];
	ZeroMemory(tchrSubSNList, 1024 * sizeof(TCHAR));
	ZeroMemory(tchrStationID, 1024 * sizeof(TCHAR));
	ZeroMemory(tchrStationname, 1024 * sizeof(TCHAR));
	char strComputerName[MAX_PATH];
	char strStationName[MAX_PATH];
    ZeroMemory(strComputerName, MAX_PATH * sizeof(char));
	ZeroMemory(strStationName, MAX_PATH * sizeof(char));
	ZeroMemory(strStationID, MAX_PATH * sizeof(char));
	DWORD dwLength = 256;
	GetComputerName(strComputerName, &dwLength);
	//strMsg.Format("%s", strComputerName);
	//LogInfo(strMsg);

	UpdateData();
	//m_strSN = m_ModuleSN;
	if (b_fusion)
	{
	//	typedef bool(*GetWOHeaderList)(const char* pchrSN, const char* pchrDataCollectionDef,
	//		const char* pchrPostion, char* pchrOutValueList, int iOutBufLen, char** ppchrErrorMsg);
	//	typedef bool(*TriggerProcessMoveIn)(const char* pchrSN, const char* pchrMESProcess, const char* pchrStationID, char** ppchrOutErrorMsg);

		if (!m_fusion.bGetProductProcess(m_ModuleSN, &m_strWO, &m_strPN, &m_strSpec, &m_strWorkStatus, &m_strWorkprss, &strErrMsg))//获取表头信息
		{
			AfxMessageBox("获取OPC信息失败");
			AfxMessageBox(strErrMsg);
			return;
		}


		if (!m_fusion.m_pFucGetStationName(strComputerName, strStationID, MAX_PATH, strStationName, MAX_PATH, &pcharOutString))
		{
			if (!m_fusion.m_pFucGetStationName(strComputerName, strStationID, MAX_PATH, strStationName, MAX_PATH, &pcharOutString))
			{
				if (!m_fusion.m_pFucGetStationName(strComputerName, strStationID, MAX_PATH, strStationName, MAX_PATH, &pcharOutString))
				{
					AfxMessageBox(pcharOutString);
					strErrMsg.Format("工位信息获取失败 - 电脑名:%s", strComputerName);
					AfxMessageBox(strErrMsg);
					return;
				}
			}
		}

		if (!m_fusion.m_pFucTriggerProcessMoveIn(m_ModuleSN, m_strWorkprss, strStationID, &pcharOutString))
		{
			AfxMessageBox(pcharOutString);
			return;
		}

	
		b_res = m_fusion.m_pFucGetWOHeaderList(m_ModuleSN, "MEMS-SWITCH-组装14538", "MEMSSWITCH-1-SN,MEMSSWITCH-2-SN,MEMSSWITCH-3-SN,MEMSSWITCH-4-SN", tchrSubSNList, 1024, &pcharOutString);
		if (!b_res)
		{
			strErrMsg.Format("%s", pcharOutString);
			AfxMessageBox(strErrMsg);
			return;
		}
		else
		{
			int ica, iallca;
			strErrMsg.Format("%s", tchrSubSNList);
			ica = strErrMsg.Find(",");
			iallca = strErrMsg.GetLength();

			m_strSN1 = strErrMsg.Left(ica);
			strErrMsg = strErrMsg.Right(iallca - ica - 1);

			ica = strErrMsg.Find(",");
			iallca = strErrMsg.GetLength();
			m_strSN3 = strErrMsg.Left(ica);
			strErrMsg = strErrMsg.Right(iallca - ica - 1);

			ica = strErrMsg.Find(",");
			iallca = strErrMsg.GetLength();
			m_strSN4 = strErrMsg.Left(ica);
			strErrMsg = strErrMsg.Right(iallca - ica - 1);

			//ica = strErrorMsg.Find(",");
			//iallca = strErrorMsg.GetLength();
			m_strSN5 = strErrMsg;
			UpdateData(FALSE);
		}
	}
	else
	{
	
	iRandNo = iRandNo % 256;
	//获取工单表头的SN号
	strURL.Format("http://zh-amtsdb-srv/AMTS/Atd_SerialNoInfo.aspx?serialno=%s&processType=HEAD&randno=%d", m_ModuleSN, iRandNo);
	strLocalFile.Format("%s\\template\\Test_temp.xml", g_tszAppFolder);

	if (m_ctrXMLCtr.GetHtmlSource(strURL, strLocalFile))
	{
		m_ctrXMLCtr.LoadXmlFile(strLocalFile);
	}
	else
	{
		strMsg = "模板下载错误!";
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	//核对PN信息是否对应
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化信息!";
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		if (!m_ctrXMLCtr.CloseXMLFile())
		{
			strMsg = "XML关闭失败,请通知相关工程师!";
			LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
			return;
		}
		return;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	if (strcmp(strTemp, strPN) != 0)
	{
		strMsg.Format("获取的PN为:%s!与产品PN不一致", strTemp);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		if (!m_ctrXMLCtr.CloseXMLFile())
		{
			strMsg = "XML关闭失败,请通知相关工程师!";
			LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
			return;
		}
		return;
	}

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "HEAD", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,没有找到无纸化信息!";
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		if (!m_ctrXMLCtr.CloseXMLFile())
		{
			strMsg = "XML关闭失败,请通知相关工程师!";
			LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
			return;
		}
		return;
	}
	dwWDMTemp = dwBuf[0];

	strKey.Format("H758");
	ZeroMemory(dwTmptBuf, 256 * sizeof(DWORD));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwTmptBuf, nNodeCount, dwWDMTemp);
	if (nNodeCount == 0)
	{
		strMsg.Format("XML文件结点不完整,无纸化中无法找到%s!", strKey);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	dwWDMTemp = dwTmptBuf[0];
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwWDMTemp); //获取SN号
	if (strTemp.IsEmpty())
	{
		LogInfo("获取的SN号为空", (BOOL)FALSE, COLOR_RED);
		return;
	}
	m_strSN1 = strTemp;

	dwWDMTemp = dwBuf[0];
	strKey.Format("H759");
	ZeroMemory(dwTmptBuf, 256 * sizeof(DWORD));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwTmptBuf, nNodeCount, dwWDMTemp);
	if (nNodeCount == 0)
	{
		strMsg.Format("XML文件结点不完整,无纸化中无法找到%s!", strKey);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	dwWDMTemp = dwTmptBuf[0];
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwWDMTemp); //获取SN号
	if (strTemp.IsEmpty())
	{
		LogInfo("获取的SN号为空", (BOOL)FALSE, COLOR_RED);
		return;
	}
	m_strSN3 = strTemp;

	dwWDMTemp = dwBuf[0];
	strKey.Format("H760");
	ZeroMemory(dwTmptBuf, 256 * sizeof(DWORD));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwTmptBuf, nNodeCount, dwWDMTemp);
	if (nNodeCount == 0)
	{
		strMsg.Format("XML文件结点不完整,无纸化中无法找到%s!", strKey);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	dwWDMTemp = dwTmptBuf[0];
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwWDMTemp); //获取SN号
	if (strTemp.IsEmpty())
	{
		LogInfo("获取的SN号为空", (BOOL)FALSE, COLOR_RED);
		return;
	}
	m_strSN4 = strTemp;

	dwWDMTemp = dwBuf[0];
	strKey.Format("H761");
	ZeroMemory(dwTmptBuf, 256 * sizeof(DWORD));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwTmptBuf, nNodeCount, dwWDMTemp);
	if (nNodeCount == 0)
	{
		strMsg.Format("XML文件结点不完整,无纸化中无法找到%s!", strKey);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	dwWDMTemp = dwTmptBuf[0];
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwWDMTemp); //获取SN号
	if (strTemp.IsEmpty())
	{
		LogInfo("获取的SN号为空", (BOOL)FALSE, COLOR_RED);
		return;
	}
	m_strSN5 = strTemp;

	UpdateData(FALSE);

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "XML关闭失败,请通知相关工程师!";
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
 }
		
}

bool CMy126S_45V_Switch_AppDlg::OpenTLSDevice()
{
	CString strMsg;
	if (!ConnectSocket())
	{
		return FALSE;
	}
	if (!SetTLSWL(1550))
	{
		return FALSE;
	}
	CloseSrcBankSocket();
	strMsg.Format("设置激光器波长为%.2fnm", m_pdblWavelength);
	LogInfo(strMsg, FALSE);
	return TRUE;
}

bool CMy126S_45V_Switch_AppDlg::ConnectSocket()
{
	CString strFileName, strSection, strKeyName;
	int		nSize = 256;
	char	tszValue[256];
	CString strMsg;
	char	szClientID[10] = "";
	CString strTitle = "";

	m_pSrcBankSocket = new CSrcBankSocket;

	if (!m_pSrcBankSocket->Create())
	{
		AfxMessageBox("集成光源Socket 创建失败!", MB_ICONERROR);
		return FALSE;
	}

	if (!m_pSrcBankSocket->Connect(m_stWorkStation.pchServer, m_stWorkStation.wPort))
	{
		TCHAR	szBuf[1024] = "";
		LPVOID	lpMsgBuf;
		DWORD	dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		wsprintf(szBuf,
			_T("%s 出错信息 (出错码=%d): %s\n\nServer: %s, 端口: %d"),
			_T("CreateDirectory"), dw, lpMsgBuf, &m_stWorkStation.pchServer, m_stWorkStation.wPort);

		LocalFree(lpMsgBuf);

		MessageBox(szBuf, "连接集成光源服务器失败!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	Sleep(200);

	sprintf(szClientID, "%02d", m_stWorkStation.nID);

	if (!m_pSrcBankSocket->Send(szClientID, strlen(szClientID)))
	{
		AfxMessageBox("向集成光源服务器发送ClientID失败!", MB_ICONERROR);
		return FALSE;
	}

	m_pSrcBankSocket->hWnd = this->m_hWnd;
	m_pSrcBankSocket->m_dwSocketIndex = m_stWorkStation.nID;

	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);

	//关闭scoket后端口便能正常释放
	m_pSrcBankSocket->SetSockOpt(SO_REUSEADDR, (char *)&bOptVal, bOptLen, SOL_SOCKET);
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::SetTLSWL(double dblWL)
{
	BYTE		bClientID;
	CString		SendStr;
	CString     strMsg;
	char		strTmp[256] = "";
	int			nResponse;
	CTime		tmNow = CTime::GetCurrentTime();
	CString		strTMP = tmNow.Format("%H%M%S");
	int			nSocketSN = atoi(strTMP);
	if (fabs(dblWL - 1550) < 1)
	{
		dblWL = 1550;
	}
	m_pSrcBankSocket->m_nSocketSN = nSocketSN;
	strMsg.Format("设置激光器波长:%.2fnm",dblWL);
	LogInfo(strMsg);
	//theApp.m_bSetWLOK = FALSE;

	bClientID = (BYTE)m_stWorkStation.nID;
	sprintf(strTmp, "*%02d%7.3f,%06d#", bClientID, dblWL, nSocketSN);

	try
	{
		nResponse = m_pSrcBankSocket->Send(strTmp, strlen(strTmp));

		if ((SOCKET_ERROR == nResponse) || ((int)strlen(strTmp) != nResponse))
		{
			CloseSrcBankSocket();

			if (!ConnectSocket())
			{
				return FALSE;
			}

			nResponse = m_pSrcBankSocket->Send(strTmp, strlen(strTmp));

			if ((SOCKET_ERROR == nResponse) || ((int)strlen(strTmp) != nResponse))
			{
				MessageBox("无法和 集成光源 服务器通信, 需要重新打开本程序", "集成光源 Socket 出错",
					MB_OK | MB_ICONERROR);
			}
			return FALSE;
		}
	}
	catch (...)
	{
		AfxMessageBox("向集成光源服务器发送请求失败!");
		return FALSE;
	}

	//接收消息
	m_pSrcBankSocket->OnReceive(0);
	if (!m_pSrcBankSocket->m_bFunctionOK)
	{
		LogInfo(m_pSrcBankSocket->m_strError, (BOOL)FALSE, COLOR_RED);
		return FALSE;
	}

	/*
	if (!theApp.m_bSetWLOK)
	{
		CloseSrcBankSocket();

		if (!ConnectSocket())
		{
			return FALSE;
		}
	}
	*/
	return TRUE;
}


void CMy126S_45V_Switch_AppDlg::CloseSrcBankSocket()
{
	if (m_pSrcBankSocket)
	{
		m_pSrcBankSocket->Close();

		delete m_pSrcBankSocket;
		m_pSrcBankSocket = NULL;
	}
}

bool CMy126S_45V_Switch_AppDlg::GetWorkStationConfig()
{
	CString strFile;
	CString strApp;
	CString strKey;
	CString strValue;
	CString strMsg;
	int     nValue;
	strFile.Format("%s\\ClientID.ini", m_strNetConfigPath);

	strApp.Format("ID Set");
	strKey.Format("ID Address");
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(256), 256, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_stWorkStation.nID = atoi(strValue);
	strMsg.Format("对应的ID为：%d", m_stWorkStation.nID);
	LogInfo(strMsg);
	if (m_stWorkStation.nID < 0 || m_stWorkStation.nID>99)
	{
		strMsg.Format("对应的ID不在0~99范围内。");
		LogInfo(strMsg);
		return FALSE;
	}

	strApp.Format("ID Set");
	strKey.Format("Server");
	GetPrivateProfileString(strApp, strKey, "ERROR", m_stWorkStation.pchServer, 256, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	strMsg.Format("对应的Server地址为：%s", &m_stWorkStation.pchServer);
	LogInfo(strMsg);

	strApp.Format("ID Set");
	strKey.Format("Port_Socket");
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(256), 256, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_stWorkStation.wPort = atoi(strValue);
	strMsg.Format("对应的端口地址为：%d", m_stWorkStation.wPort);
	LogInfo(strMsg);

	strApp.Format("ID Set");
	strKey.Format("Path_WLList");
	GetPrivateProfileString(strApp, strKey, "ERROR", m_stWorkStation.pWLPath, 256, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	strMsg.Format("对应的波长文件地址为：%s", &m_stWorkStation.pWLPath);
	LogInfo(strMsg);
	
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnSelchangeComboTemperature() 
{
	int nCommand = m_ctrlComboxTemperature.GetCurSel();
	if(nCommand > 0)
	{
		GetDlgItem(IDC_BUTTON_RL_TEST2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DARK_TEST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RE_IL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_RL_TEST2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DARK_TEST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RE_IL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ONEKEY_TEST)->EnableWindow(TRUE);
	}	
}
BOOL CMy126S_45V_Switch_AppDlg::SWMsgCheckShow()
{
	CString strTemp;
	CString strTitle;
	CString strProgInfo;
	BOOL    m_bDebug = TRUE;
	CString strErrMsg;
	DWORD   dwBuf[256];
	int     nNodeCount;
	CString strSWName = "SW2055_MSW_1X64_FTS_V3.4.0.0        2022-11-22";
	CString strPath = "        工程路径:NA/NA/NA/NA/NA";
	//第一步获取MIMS传过来信息
	TCHAR tszCurrentFolder[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, tszCurrentFolder);
	strProgInfo.Format("%s\\template\\ProgInfo.xml", tszCurrentFolder);

	strMIMSMsg = AfxGetApp()->m_lpCmdLine;
	//AfxMessageBox(strMIMSMsg);
	ConvertGBKToUtf8(strMIMSMsg);
	if (strMIMSMsg.GetLength() <= 0)
	{
		::MessageBox(NULL, "程序缺失启动必须参数，请确认是否正常UUI启动", "提示", MB_OK);
		//return FALSE;
	}
	CFile TemFile;
	while (!TemFile.Open(strProgInfo, CFile::modeWrite | CFile::modeCreate))
	{
		return false;
	}
	TemFile.Write(strMIMSMsg, strMIMSMsg.GetLength());
	TemFile.Close();
	//解析对应的MIMS信息
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocumentPtr m_pDoc;
	HRESULT hr = m_pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (!SUCCEEDED(hr))
	{
		return FALSE;
	}
	_variant_t var = (LPCTSTR)strProgInfo;  //强制类型转换

	if (VARIANT_FALSE == m_pDoc->load(strProgInfo.AllocSysString()))
	{
		return FALSE;
	}
	MSXML2::IXMLDOMNodePtr PtrNode = NULL;

	PtrNode = m_pDoc->selectSingleNode("//User");
	if (PtrNode != NULL)
	{
		strUserMsg = (const char*)PtrNode->text;
	}
	PtrNode = m_pDoc->selectSingleNode("//PN");
	if (PtrNode != NULL)
	{
		strPNInfo = (const char*)PtrNode->text;
	}
	PtrNode = m_pDoc->selectSingleNode("//SN");
	if (PtrNode != NULL)
	{
		strSNInfo = (const char*)PtrNode->text;
	}
	PtrNode = m_pDoc->selectSingleNode("//Process");
//	if (PtrNode != NULL)
//	{
//		strSNInfo = (const char*)PtrNode->text;
//	}
	PtrNode = m_pDoc->selectSingleNode("//SoftwareID");
	if (PtrNode != NULL)
	{
		strSoftID = (const char*)PtrNode->text;
	}
	if (strSoftID.Find("2055") == -1)
	{
		::MessageBox(NULL, "软件调用不正确，请联系对应工程师", "提示", MB_OK);
		m_bDebug = FALSE;

	}
	PtrNode = m_pDoc->selectSingleNode("//LoginMode");
	if (PtrNode != NULL)
	{
		strLoginMode = (const char*)PtrNode->text;
	}

	PtrNode = m_pDoc->selectSingleNode("//CheckUser");
	if (PtrNode != NULL)
	{
		strSecondCalMsg = (const char*)PtrNode->text;
	}

	PtrNode = m_pDoc->selectSingleNode("//CheckPwd");
	if (PtrNode != NULL)
	{
		strSecondCalPSW = (const char*)PtrNode->text;
	}

	if (strLoginMode == "RDMode")
	{
		//::MessageBox(NULL, "研发模式,请注意", "提示", MB_OK);
	}

	PtrNode = m_pDoc->selectSingleNode("//MesMode");
	if (PtrNode != NULL)
	{
		strMESMode = (const char*)PtrNode->text;
	}


	if (strMESMode != "MESOnline")
	{
		::MessageBox(NULL, "切换到离线模式,请注意", "提示", MB_OK);
		//	OnBnClickedButtonIn();
		m_bDebug = FALSE;
	}

	//第二步，下载对应模板进行相关信息比对和显示
	//C接口获取
	if (m_fusion.m_pFucGetProductKeyInfo) //获取模板信息
	{
		char pchrPNBuffer[64];
		ZeroMemory(pchrPNBuffer, 64 * sizeof(char));
		char pchrSpecBuffer[64];
		ZeroMemory(pchrSpecBuffer, 64 * sizeof(char));
		char pchrWOBuffer[64];
		ZeroMemory(pchrWOBuffer, 64 * sizeof(char));
		char pchrProcessBuffer[64];
		ZeroMemory(pchrProcessBuffer, 64 * sizeof(char));
		char pchrStatusBuffer[64];
		ZeroMemory(pchrStatusBuffer, 64 * sizeof(char));
		char pchrInterProBuffer[64];
		ZeroMemory(pchrInterProBuffer, 64 * sizeof(char));
		char pchrTemplate[1024];
		ZeroMemory(pchrTemplate, 1024 * sizeof(char));
		char* pcharOutString = NULL;
		bool bRunFlag = m_fusion.m_pFucGetProductKeyInfo(strSNInfo.GetBuffer(0), pchrPNBuffer, 64,
			pchrSpecBuffer, 64, pchrWOBuffer, 64, pchrProcessBuffer, 64,
			pchrStatusBuffer, 64, &pcharOutString);
		CString strMsg;
		if (bRunFlag)
		{
			strMsg.Format("		%s@%s@%s@%s.xml",
				pchrPNBuffer, pchrProcessBuffer, pchrWOBuffer, pchrSpecBuffer, pchrWOBuffer);
			strSWName = strSWName + strMsg;

			strPNInfo.Format("%s", pchrPNBuffer);
			strSPECInfo.Format("%s", pchrSpecBuffer);
			strCurProcessInfo.Format("%s", pchrProcessBuffer);
			strWOInfo.Format("%s", pchrWOBuffer);
			//if 转换成内部工序
			m_fusion.m_pFucGetTestProcessCode(pchrPNBuffer, pchrProcessBuffer, pchrInterProBuffer, 64, &pcharOutString);
			strCurProcessInfo.Format("%s", pchrInterProBuffer);
			if (strCurProcessInfo != strTestProcessInfo)
			{
			//	strMsg.Format("工序不一致,当前工序:%s,软件测试工序：%s", strCurProcessInfo, strTestProcessInfo);
			//	AfxMessageBox(strMsg);
			//	return FALSE;
			}
			//获取对应模板存储成文件
			bool m_bshow = 0;
			CString strFileName;
			TCHAR tszCurrentFolder[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, tszCurrentFolder);
			strCurProcessInfo.Format("%s", pchrProcessBuffer);
			strFileName.Format("%s\\Template\\%s.xml", tszCurrentFolder, strSNInfo);
			bool bTempFlag = m_fusion.m_pFucGetProdTestTemplate(strSNInfo, strCurProcessInfo, strFileName,
				m_bshow, pchrTemplate, 1024, &pcharOutString);
			if (!bTempFlag)
			{
				strMsg.Format("%s", pcharOutString);
				AfxMessageBox(strMsg);
			}
			strMsg.Format("%s", pchrTemplate);
			if (strMsg.Find("\\zh-mfs-srv") == -1)
			{
				AfxMessageBox("非正常启动路径");
				m_bDebug = FALSE;
				strPath = "工程路径:TP/TT/NA/TD/CF";
			}

		}
		else
		{
			strMsg.Format("获取产品信息%s", pcharOutString);
			AfxMessageBox(strMsg);
		}

	}
	else
	{
		AfxMessageBox("获取模板信息数据错误");
		return false;
	}

	//第三步，相关信息显示
	//TP:test program
	//TT:test template
	//TD:test Data:
	//CF:configutation File
	//显示标题栏信息
	if (!m_bDebug)
	{
		strPath = "        工程路径:TP/TT/NA/TD/CF";//debug路径
	}
	strSWName = strSWName + strPath;
	SetWindowText(strSWName);
	//显示用户名和登陆模式
	//SetDlgItemText(IDC_ID, strUserMsg);
	m_strID = strUserMsg;
	strTemp.Format("工号：%s/r模式：%s/rMES :%s", m_strID, strLoginMode, strMESMode);
	m_blogin = TRUE;
	SetDlgItemText(IDC_STATIC_LOGIN, strTemp);
	UpdateData(FALSE);

}
void CMy126S_45V_Switch_AppDlg::ConvertGBKToUtf8(CString &strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL, 0);
	wchar_t * wszUtf8 = new wchar_t[len];
	memset(wszUtf8, 0, len);
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, wszUtf8, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL, NULL);
	strGBK = szUtf8;
	delete[] szUtf8;
	delete[] wszUtf8;
}
void CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIn()
{
	CString strMsg;
	char* pcharOutString = NULL;
	AfxMessageBox(strSecondCalMsg);
	AfxMessageBox(strSecondCalPSW);
	strSecondCalMsg = "9353";
	strSecondCalPSW = "00A1F187721C63501356BF791E69382C";
	bool bRFlag = m_fusion.m_pFucSetSystemToSpecialMode(strSecondCalMsg.GetBuffer(0), strSecondCalPSW.GetBuffer(0), "MESLESS", &pcharOutString);
	if (bRFlag<0)
	{
		strMsg.Format("切换到离线模式失败，%s", pcharOutString);
		AfxMessageBox(strMsg);
	}
	AfxMessageBox("切换到离线模式ok");

	char pchrProcessBuffer[64];
	ZeroMemory(pchrProcessBuffer, 64 * sizeof(char));
	bRFlag = m_fusion.m_pFucGetSystemCurrentMode(pchrProcessBuffer, 64, &pcharOutString);
	if (bRFlag<0)
	{
		strMsg.Format("切换到离线模式失败，%s", pcharOutString);
		AfxMessageBox(strMsg);
	}
	else
	{

	}
}

void CMy126S_45V_Switch_AppDlg::bIniXMLItem()
{
	m_XMLItem.strTenvKey = "RT";
	m_XMLItem.strObjectKey = "MEMS SW1";
	m_XMLItem.strPortKey = "1-1";
	m_XMLItem.strcondKey = "C000001";
	m_XMLItem.strParamName = "Volt_X";
	m_XMLItem.strParamValue = "99";
	m_XMLItem.strPassfail = "";
	m_XMLItem.strFile = "NULL";
	m_XMLItem.strDate = "NULL";
}