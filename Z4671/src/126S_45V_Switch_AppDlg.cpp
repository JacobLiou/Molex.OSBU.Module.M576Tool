// 126S_45V_Switch_AppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "126S_45V_Switch_App.h"
#include "126S_45V_Switch_AppDlg.h"
#include <math.h>
#include "COMUTIL.H"
#include "Command.h"
//#include "R152Set.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

stPMCfg g_stPMCfg;
TCHAR g_tszAppFolder[MAX_PATH];
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
	m_nIndex=0;
	m_iLightCount=0;
	m_bTesterPort = FALSE;
	m_bTesterALL = FALSE;
	//{{AFX_DATA_INIT(CMy126S_45V_Switch_AppDlg)
	m_strID = _T("");
	m_strSN= _T("");
	m_strChannel = _T("");
	m_strMessage = _T("");
	m_dblCHValue = 0.0;
	m_dblCHValue2 = 0.0;
	m_nTesterPort = 1;
	m_nLightCom = 0;
	m_strItemName = "调节";
	m_dblTLSSetWL = 0.0;
	dblRefIL = 1.5;
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
	m_bScan = FALSE;
	m_bTestIL = FALSE;
	//初始化
	m_dblTLSPower = 0; //激光器功率初始值
	m_pdblWavelength =  1550; //激光器波长初始值
	m_iReadPMCount = 0;
	
	int i = 0;
	for ( i=0; i<16; i++)
	{
		m_bILPass[i] = TRUE;
		m_bCTPass[i] = TRUE;
		m_bTDLPass[i] = TRUE;
		m_bWDLPass[i] = TRUE;
		m_bPDLPass[i] = TRUE;
	}
	ZeroMemory(m_dblZeroWDL,sizeof(double)*100);
	ZeroMemory(m_dwSingle,sizeof(DWORD)*16);

	//数组初始化
	for (i=0; i<16; i++)
	{
		  m_Xbase[i] = 0;
		  m_Ybase[i] = 0;
		  ScanPowerChannal[i].MaxPower = -100;
		  m_dblReferencePower[i] = 0;
	 }

/*    m_pReferencePower->dblReferencePower=0;//初始化归零光功率*/
}

void CMy126S_45V_Switch_AppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy126S_45V_Switch_AppDlg)
	//DDX_Control(pDX, IDC_COMBO_LIGHT_CH, m_ctrlLightCH);
	DDX_Control(pDX, IDC_EDIT_PM_IL1, m_editIL);
	DDX_Control(pDX, IDC_PROGRESS2, m_ctrlProgress);
	//DDX_Control(pDX, IDC_COMBO_PN, m_cbxPN);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_cbComPort);
	DDX_Control(pDX, IDC_COMBO_SaveData, m_cbSaveData);
	DDX_Control(pDX, IDC_COMBO_SEND_MESSAGE, m_cbMessage);
	DDX_Control(pDX, IDC_COMBO_CH_SET, m_cbChannel);
	DDX_Control(pDX, IDC_COMBO_SET_WL, m_cbSetWL);
	DDX_Control(pDX, IDC_COMBO_SET_PDBOX, m_cbSetPDBox);
	DDX_Control(pDX, IDC_LIST_INFO, m_ctrlListMsg);
	DDX_Text(pDX, IDC_EDIT_ID, m_strID);
	DDX_Text(pDX, IDC_EDIT_SN, m_strSN);
	DDX_CBString(pDX, IDC_COMBO_CH_SET, m_strChannel);
	DDX_Control(pDX, IDC_NTGRAPHCTRL1, m_NTGraph);
	DDX_Text(pDX, IDC_EDIT_COM_TESTER_PORT, m_nTesterPort);
	//DDX_Text(pDX, IDC_EDIT_LIGHT_COM, m_nLightCom);
	DDX_Text(pDX, IDC_EDIT_TLSWL, m_dblTLSSetWL);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_SN_INDEX, m_cbxSNList);
}

BEGIN_MESSAGE_MAP(CMy126S_45V_Switch_AppDlg, CDialog)
	//{{AFX_MSG_MAP(CMy126S_45V_Switch_AppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_STOP_TEST, OnButtonStopTest)
	ON_BN_CLICKED(IDC_BUTTON_ILCT_TEST, OnButtonIlctTest)
	ON_BN_CLICKED(IDC_BUTTON_IL_REF, OnButtonIlRef)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEND_MESSAGE, OnButtonSendMessage)
	ON_CBN_SELCHANGE(IDC_COMBO_SEND_MESSAGE, OnSelchangeComboSendMessage)
	ON_BN_CLICKED(IDC_BUTTON_REGULATE_SCAN, OnButtonRegulateScan)
	ON_BN_CLICKED(IDC_BUTTON_RL_TEST, OnButtonRlTest)
	ON_CBN_SELCHANGE(IDC_COMBO_PN, OnSelchangeComboPn)
	ON_CBN_CLOSEUP(IDC_COMBO_PN, OnCloseupComboPn)
	ON_BN_CLICKED(IDC_BUTTON_REPEAT_TEST2, OnButtonRepeatTest2)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_DARK_TEST, OnButtonDarkTest)
	ON_BN_CLICKED(IDC_BUTTON_SET_VOLTAGE, OnButtonSetVoltage)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_TESTER_PORT, OnButtonOpenTesterPort)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_ALL, OnButtonSwitchAll)
	ON_BN_CLICKED(IDC_BUTTON_SET_TESTER_VOLTAGE, OnButtonSetTesterVoltage)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_CHECK, OnButtonCheck)
	ON_BN_CLICKED(IDC_BUTTON_RL_REF, OnButtonRlRef)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_CH, OnButtonScanCh)
	ON_BN_CLICKED(IDC_BUTTON_SET_WL, OnButtonSetWl)
	ON_WM_MOUSEMOVE()
	ON_EN_KILLFOCUS(IDC_EDIT_SN, OnKillfocusEditSn)
    ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, OnCbnSelchangeComboComport)
	ON_BN_CLICKED(IDC_BUTTON_ILCT_TEST2, OnBnClickedButtonIlctTest2)
	ON_EN_CHANGE(IDC_EDIT_SN, OnEnChangeEditSn)
	ON_BN_CLICKED(IDC_BUTTON_SW_CAL, OnBnClickedButtonSwCal)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLoad, OnBUTTONDOWNLoad)
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDC_BUTTON_ILCT_TEST2, &CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIlctTest2)
	
	
	ON_CBN_SELCHANGE(IDC_COMBO_SN_INDEX, &CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboSnIndex)
	ON_BN_CLICKED(IDC_BUTTON_SET_VOLTAGE2, &CMy126S_45V_Switch_AppDlg::OnBnClickedButtonSetVoltage2)
	ON_BN_CLICKED(IDC_BUTTON_ILCT_TEST4, &CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIlctTest4)
	ON_BN_CLICKED(IDC_BUTTON_ILCT_TEST5, &CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIlctTest5)
	ON_BN_CLICKED(IDC_BUTTON_ILCT_TEST6, &CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIlctTest6)
	ON_CBN_SELCHANGE(IDC_COMBO_SET_WL, &CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboSetWl)
	ON_CBN_SELCHANGE(IDC_COMBO_SET_PDBOX, &CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboSetPdbox)
	ON_CBN_SELCHANGE(IDC_COMBO_CH_SET, &CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboChSet)
	ON_BN_CLICKED(IDC_BUTTON_SET_VOLTAGE3, &CMy126S_45V_Switch_AppDlg::OnBnClickedButtonSetVoltage3)
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
    //设置默认信息

//	m_ctrlComboxTemperature.SetCurSel(0);
	GetLocalTime(&m_stStartTime);
	m_cbSaveData.SetCurSel(0);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//得到当前目录的路径
	GetCurrentDirectory(MAX_PATH,g_tszAppFolder);

	DWORD dwLength=256;
	GetComputerName(m_chComputerName,&dwLength);
    m_bConnect1830C = FALSE;
	//m_bDeviceOpen = FALSE;

	CFont *font;
	font = new CFont;
	font->CreateFont(50,20,0,0,50,FALSE,FALSE,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		FF_SWISS,"幼圆");
	CEdit *m_Edit=(CEdit*)GetDlgItem(IDC_EDIT_PM_IL1);
	m_Edit->SetFont(font,FALSE);

	CString strFileName;
	CString strValue;
	CString strKey;
	WORD    wPNCount;

	m_cbComPort.SetCurSel(0);
	m_cbMessage.SetCurSel(0);
	m_cbChannel.SetCurSel(0);
	m_cbSetWL.SetCurSel(-1);
	m_cbSetPDBox.SetCurSel(-1);
	//获得网络数据存储路径
 	strFileName.Format("%s\\config\\DataSave.ini",g_tszAppFolder);
	int iLen = GetPrivateProfileString("ServeAddress","Address",NULL,m_strNetFile.GetBuffer(256),256,strFileName);
    if (iLen <= 0)
    {
		MessageBox("读取配置文件DataSave.ini失败");
		return FALSE;
    }
	//是否上传到网络路径
	CString strSave;
    iLen = GetPrivateProfileString("Save","Save",NULL,strSave.GetBuffer(256),256,strFileName);
    if (iLen <= 0)
    {
		MessageBox("读取配置文件DataSave.ini失败");
		return FALSE;
    }
	m_bIfSaveToServe = atoi(strSave);

	GetConfigFile();
	m_nCHCount = 32; //32
	m_strPN = "SWBox";

    //SetDefID(IDC_BUTTON_SEND_MESSAGE);
	m_bReady = FALSE;

	UpdateData(FALSE);

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
		CRect rectClient;		
		GetClientRect(&rectClient);		
		CDC dcMem;		
		CBitmap bmpMem;
				
		dcMem.CreateCompatibleDC(&dc);
		bmpMem.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height());		
//		dcMem.SelectObject(&bmp);	
		
		// 此处将绘制内容输出到dcMem上	
		// dcMem.FillRect(rectClient, &brush);
		dc.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcMem, 0, 0, SRCCOPY);		
		bmpMem.DeleteObject();		
		dcMem.DeleteDC();  

		/*
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
		*/
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
    m_bReadPowerStop = TRUE;
	KillTimer(3);
	//
	CString strTemp;
	CString strMsg;
	//strTemp.Format("%02d",nchannel+1);
	int i = 0;
	for( i = 0; i < 28; i++)
	{
		strMsg.Format("CT%d ",i+1);
		strTemp += strMsg;
	}
	LogInfo(strTemp);
}

void CMy126S_45V_Switch_AppDlg::OnButtonIlRef() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_bTestStop = FALSE;
	double     dblReadPwr;
	double     dblValue[64];
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
	CString strToken;
	SYSTEMTIME st;
	SYSTEMTIME stGet;
	COleDateTime ctTimeStart;
	COleDateTime ctTimeEnd;
	COleDateTimeSpan  timeSpan;
	int nHourDiff;
	CComboBox *pComb = (CComboBox*)GetDlgItem(IDC_COMBO_SET_WL);
	pComb->GetWindowText(strTemp);
	m_pdblWavelength = atof(strTemp);
	int inPM = m_cbSetPDBox.GetCurSel();
	m_nCHCount = 18;
	if (inPM == 0)
	{
		m_nPMCount = 18;
	}
	if (inPM == 1)
	{
		m_nPMCount = 36;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	int inWLTH = m_cbSetWL.GetCurSel();
	if (inWLTH < 0)
	{
		MessageBox("请选择波长！");
		return;
	}
	int i = 0;
	for ( i=0;i<m_nPMCount;i++)
	{
		strReferenceFileName.Format("%s\\data\\Ref\\%s\\%.0f",m_strLocalPath,m_chComputerName, m_pdblWavelength);
		CreateFullPath(strReferenceFileName);
		strReferenceFileName.Format("%s\\data\\Ref\\%s\\%.0f\\ReferenceZeroData-CH%d.txt",m_strLocalPath,m_chComputerName, m_pdblWavelength,i+1);
		if (GetFileAttributes(strReferenceFileName) == -1)
		{
			strMsg.Format("%s归零缺少", strReferenceFileName);
			LogInfo(strMsg, FALSE, COLOR_RED);
			iSel &= IDNO;
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
		GetLocalTime(&st); //
		ctTimeEnd = COleDateTime(st);
		//获取正向归零
		for (i=0;i<m_nPMCount;i++)
		{
			strReferenceFileName.Format("%s\\data\\Ref\\%s\\%.0f\\ReferenceZeroData-CH%d.txt",m_strLocalPath, m_chComputerName, m_pdblWavelength,i+1);
			if (!(fileReferenceFile.Open(strReferenceFileName,CFile::modeRead|CFile::typeText,NULL)))
			{
				strMsg.Format("打开文件%s失败！", strReferenceFileName);
				MessageBox(strMsg);
				return;
			}
			//获取归零时间
			pStr = fileReferenceFile.ReadString(lineBuf,256);
			strToken = strtok(lineBuf, ";-");
            stGet.wYear = atoi(strToken);
			strToken = strtok(NULL,"-");
            stGet.wMonth = atoi(strToken);
			strToken = strtok(NULL,"-");
            stGet.wDay = atoi(strToken);
			strToken = strtok(NULL,"-");
            stGet.wHour = atoi(strToken);
			strToken = strtok(NULL,"-");
            stGet.wMinute = atoi(strToken);
			strToken = strtok(NULL,"-");
            stGet.wSecond = atoi(strToken);
			ctTimeStart = COleDateTime(stGet);
            timeSpan = ctTimeEnd - ctTimeStart;
			nHourDiff = timeSpan.GetTotalHours();
			strMsg.Format("通道%d的归零时间距现在%d小时",i+1,nHourDiff);
			LogInfo(strMsg);
			if(nHourDiff>=m_stRefConfig.nTimeLimit)
			{
				fileReferenceFile.Close();
				strMsg.Format("归零时间已超过%d小时，需重新归零该通道",m_stRefConfig.nTimeLimit);
		    	LogInfo(strMsg,FALSE,COLOR_RED);
				return;
			}
			nCH = 0;
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
				if(fabs(dblWL-m_pdblWavelength)>0.01)
				{
					strMsg.Format("归零文件波长为%.2fnm,与要求波长(%.2fnm)不一致,需重新归零！",dblWL,m_pdblWavelength);
					LogInfo(strMsg,FALSE,COLOR_RED);
					fileReferenceFile.Close();
					return;
				}

				token = strtok(NULL,sep);
				strTemp.Format("%s",token);
				strTemp.TrimLeft();

				m_dblReferencePower[i] = atof(strTemp);	

				pStr = fileReferenceFile.ReadString(lineBuf,256);
				strMsg.Format("通道%d:波长:%.2f nm,归零值为:%.2fdBm",i+1,dblWL,m_dblReferencePower[i]);
				LogInfo(strMsg,FALSE);
			}
		   fileReferenceFile.Close();
		}	
		//获取反向归零数据
		m_stReCHConfig.nCHCount = 0;
		if (m_stReCHConfig.nCHCount != 0) //存在反向测试通道
		{
			strReferenceFileName.Format("%s\\data\\Ref\\%s\\%.0f\\ReferenceZeroData-Re.txt", m_strLocalPath, m_chComputerName, m_pdblWavelength, i + 1);
			if (!(fileReferenceFile.Open(strReferenceFileName, CFile::modeRead | CFile::typeText, NULL)))
			{
				strMsg.Format("打开文件%s失败！", strReferenceFileName);
				MessageBox(strMsg);
				return;
			}
			//获取归零时间
			pStr = fileReferenceFile.ReadString(lineBuf, 256);
			strToken = strtok(lineBuf, ";-");
			stGet.wYear = atoi(strToken);
			strToken = strtok(NULL, "-");
			stGet.wMonth = atoi(strToken);
			strToken = strtok(NULL, "-");
			stGet.wDay = atoi(strToken);
			strToken = strtok(NULL, "-");
			stGet.wHour = atoi(strToken);
			strToken = strtok(NULL, "-");
			stGet.wMinute = atoi(strToken);
			strToken = strtok(NULL, "-");
			stGet.wSecond = atoi(strToken);
			ctTimeStart = COleDateTime(stGet);
			timeSpan = ctTimeEnd - ctTimeStart;
			nHourDiff = timeSpan.GetTotalHours();
			strMsg.Format("通道%d的归零时间距现在%d小时", i + 1, nHourDiff);
			LogInfo(strMsg);
			if (nHourDiff >= m_stRefConfig.nTimeLimit)
			{
				fileReferenceFile.Close();
				strMsg.Format("归零时间已超过%d小时，需重新归零该通道", m_stRefConfig.nTimeLimit);
				LogInfo(strMsg, FALSE, COLOR_RED);
				return;
			}
			nCH = 0;
			while (NULL != pStr)
			{
				strTemp.Format("%s", lineBuf);
				strTemp.Replace("\t", " ");
				strTemp.TrimLeft();
				strTemp.TrimRight();
				strRemarkFlag = strTemp.Left(1);
				strTemp.TrimRight();
				if (!strcmp(";", strRemarkFlag) || strTemp.IsEmpty())
				{
					pStr = fileReferenceFile.ReadString(lineBuf, 256);
					continue;
				}
				//获取数据
				token = strtok(lineBuf, sep);
				strTemp.Format("%s", token);
				strTemp.TrimLeft();
				// nCH = strtol(strTemp,NULL,10) - 1;

				token = strtok(NULL, sep);
				strTemp.Format("%s", token);
				strTemp.TrimLeft();
				dblWL = atof(strTemp);

				if (fabs(dblWL - m_pdblWavelength)>0.01)
				{
					strMsg.Format("归零文件波长为%.2fnm,与要求波长(%.2fnm)不一致,需重新归零！", dblWL, m_pdblWavelength);
					LogInfo(strMsg, FALSE, COLOR_RED);
					fileReferenceFile.Close();
					return;
				}

				token = strtok(NULL, sep);
				strTemp.Format("%s", token);
				strTemp.TrimLeft();

				m_dblRefPowerRe = atof(strTemp);

				pStr = fileReferenceFile.ReadString(lineBuf, 256);
				strMsg.Format("通道%d:波长:%.2f nm,归零值为:%.2fdBm", i + 1, dblWL, m_dblRefPowerRe);
				LogInfo(strMsg, FALSE);
			}
			fileReferenceFile.Close();
		}
		LogInfo("所有通道的归零已完成！");			
	}
	//重新归零
	else if (IDNO == iSel)
	{
		UpdateData();
		i = m_cbChannel.GetCurSel();
		if (i<=0)
		{
			MessageBox("请选择正确的通道！");
			return;
		}
		if (i <= m_nPMCount) //正向通道
		{
			strMsg.Format("请连接光源线接光功率计%d\n<连接好后点击确定>", i);
			MessageBox(strMsg);
			GetLocalTime(&st);
			strReferenceFileName.Format("%s\\data\\Ref\\%s\\%.0f\\ReferenceZeroData-CH%d.txt", m_strLocalPath, m_chComputerName, m_pdblWavelength, i);
			if (!fileReferenceFile.Open(strReferenceFileName, CFile::modeCreate | CFile::modeReadWrite, NULL))
			{
				MessageBox("创建文件失败！");
				return;
			}
			//写入归零时间
			strMsg.Format("%04d-%02d-%02d-%02d-%02d-%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			strMsg = strRemarkFlag + strMsg;
			fileReferenceFile.WriteString(strMsg);

			strMsg.Format("channel  WL  RefPower(dBm)\n");
			strMsg = strRemarkFlag + strMsg;
			fileReferenceFile.WriteString(strMsg);

			if (inPM == 0)
			{
				dblReadPwr = ReadPWMPower(i - 1);
			}
			else if (inPM == 1)
			{
				if (i > 18)
				{
					if (!m_CmdInstrument.SetSwitchChannel(2, 2))
					{
						strMsg = "EDFATestBox切换IN1失败";
						LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);
						return;
					}
				}
				else
				{
					if (!m_CmdInstrument.SetSwitchChannel(2, 1))
					{
						strMsg = "EDFATestBox切换IN2失败";
						LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);
						return;
					}
				}
				m_PDBoxCmd.GetSiglePower(i,&dblReadPwr);
			}
			else
			{
				MessageBox("没有选择PM！");
				return;
			}
			strInfo.Format("通道%d:波长：%.2f nm，归零光功率为：%.2fdBm", i, m_pdblWavelength, dblReadPwr);
			LogInfo(strInfo, FALSE);
			m_dblReferencePower[i - 1] = dblReadPwr;

			strMsg.Format("%d, %.2f ,  %.2f\n", i, m_pdblWavelength, dblReadPwr);
			fileReferenceFile.WriteString(strMsg);
			fileReferenceFile.Close();
		}
		else //反向通道
		{ 
			nCH = i - m_nPMCount;
			strMsg.Format("请连接光源线接通道%d，回损头接通道%d\n<连接好后点击确定>", nCH + m_stReCHConfig.nStartIndex-1, m_stReCHConfig.nPMIndex);
			MessageBox(strMsg);
			GetLocalTime(&st);
			strReferenceFileName.Format("%s\\data\\Ref\\%s\\%.0f\\ReferenceZeroData-Re.txt", m_strLocalPath, m_chComputerName, m_pdblWavelength);
			if (!fileReferenceFile.Open(strReferenceFileName, CFile::modeCreate | CFile::modeReadWrite, NULL))
			{
				MessageBox("创建文件失败！");
				return;
			}
			//写入归零时间
			strMsg.Format("%04d-%02d-%02d-%02d-%02d-%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			strMsg = strRemarkFlag + strMsg;
			fileReferenceFile.WriteString(strMsg);

			strMsg.Format("channel  WL  RefPower(dBm)\n");
			strMsg = strRemarkFlag + strMsg;
			fileReferenceFile.WriteString(strMsg);

			dblReadPwr = ReadPWMPower(m_stReCHConfig.nPMIndex - 1);
			strInfo.Format("反向通道%d:波长：%.2f nm，归零光功率为：%.2fdBm", nCH + m_stReCHConfig.nStartIndex - 1, m_pdblWavelength, dblReadPwr);
			LogInfo(strInfo, FALSE);

			m_dblRefPowerRe = dblReadPwr;

			strMsg.Format("%d, %.2f ,  %.2f\n", nCH + m_stReCHConfig.nStartIndex - 1, m_pdblWavelength, dblReadPwr);
			fileReferenceFile.WriteString(strMsg);
			fileReferenceFile.Close();
		}
						   		
	} 
	else 
	{
		m_bHasRefForTest = FALSE;
	}
	m_bHasRefForTest = TRUE;
	
}



BOOL CMy126S_45V_Switch_AppDlg::ReadTestInfo()
{
   UpdateData(TRUE);
   
   if (m_strID.IsEmpty())
   {
	   MessageBox("请输入工号！",MB_OK);
	   return FALSE;
   }
   if (m_strSN.IsEmpty())
   {
	   MessageBox("请输入SN号！",MB_OK);
	   return FALSE;
   }

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
    
	char chComputerName[256];
	DWORD dwLength=256;
	ZeroMemory(chComputerName,sizeof(chComputerName));
	GetComputerName(chComputerName,&dwLength);
	
	GetLocalTime(&st);                             
	strTime.Format("(%s)%02d:%02d:%02d :",&chComputerName,st.wHour,st.wMinute,st.wSecond);
	strValue.Format("%s%s",strTime,tszlogMsg);
	strNetFile.Format("%s\\data\\%s\\%s\\Log-%04d-%02d-%02d.txt",m_strNetFile,m_strPN,m_strSN,st.wYear,st.wMonth,st.wDay);
	strFile.Format("%s\\data\\%s\\%s\\Log-%04d-%02d-%02d.txt",m_strLocalPath, m_strPN,m_strSN,st.wYear,st.wMonth,st.wDay);
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
	if (bAppend)
	{
		strToken = strtok((char *)tszlogMsg,"\r\n");
		strToken.Replace("\t","    ");
		m_ctrlListMsg.AddString(strToken,COLOR_BLUE);
		while (strToken != "")
		{
			strToken = strtok(NULL,"\r\n");
			strToken.Replace("\t","     ");
			m_ctrlListMsg.AddString(strToken);
		}
	}
	else
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
	char    strCT[32];
	char    strIL[32];
	int nchannel;
	m_bTestStop = FALSE;
	m_bReadPowerStop = TRUE;
	UpdateData();
	int     nCH;
	m_nSNIndex = m_cbxSNList.GetCurSel();
	strMsg.Format("当前测试SN为：%s", m_stSNList[m_nSNIndex].strSN);
	LogInfo(strMsg);

	ZeroMemory(strCT,sizeof(strCT));
	ZeroMemory(strIL,sizeof(strIL));

	if (!m_bReady)
	{
		MessageBox("请先输入SN！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("没有归零，请先归零");
		return;
	}
	if (m_stSNList[m_nSNIndex].nCHCount == 2 && m_stSNList[m_nSNIndex].nCOMCount == 2)
	{
		strMsg.Format("产品青色(CH2)端接进光，产品棕色(CH3)，紫色(CH4)依次接光功率计1,2"); 
		if(m_cbComPort.GetCurSel()==1)
		{
			strMsg.Format("产品红色(CH1)端接进光，产品棕色(CH3)接光功率计1"); 
		}
	}
	else
	{
		strMsg.Format("产品COM端接进光，产品出光依次接光功率计！");
	}
	MessageBox(strMsg);
	LogInfo(strMsg);


	if(!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}

	UpdateData();
	GetLocalTime(&m_stStartTime);
    LogInfo(strTemp,FALSE);

    m_ctrlListMsg.ResetContent();
    strTemp.Format("开始IL测试！");
    LogInfo(strTemp,FALSE);
	
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA win32Find;
	int    iSel=IDYES;
	int j;
	int i;
	BOOL   bFlag = TRUE;
	double  dblCT[32];
	BOOL pbFail[20]={0};
	BOOL pbyArray[20][20]={0};
	int nCHCount;
	nCHCount = m_stSNList[m_nSNIndex].nCHCount;
	if(m_cbComPort.GetCurSel()==1)
	{
		nCHCount = 1;
	}

	for (nchannel = 0; nchannel < nCHCount; nchannel++)
	{
		YieldToPeers();
		if (m_bTestStop)
		{
			MessageBox("测试中止！");
			return;
		}
		/*
		if(m_cbComPort.GetCurSel()==1)
		{
			//开关切换
			if(!SPRTProductCH(nchannel+1+ m_stSNList[m_nSNIndex].nCHCount))
			{
				strMsg.Format("切换到通道%d错误",nchannel+1+m_nCHCount);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				return;
			}
			
		}
		else
		*/
		{
			//开关切换
			if(m_stSNList[m_nSNIndex].nCHCount == 2 && m_stSNList[m_nSNIndex].nCOMCount == 2&&m_cbComPort.GetCurSel()==1)
			{
				if(!SPRTProductCH(2))
				{
					strMsg.Format("切换到通道%d错误",2);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					return;
				}
			}
			else
			{
				if(!SPRTProductCH(nchannel+1))
				{
					strMsg.Format("切换到通道%d错误",nchannel+1);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					return;
				}
			}
		
		}
		Sleep(100);
		int inPM = m_cbSetPDBox.GetCurSel();
		if (inPM == 0)
		{
			dblPout = ReadPWMPower(nchannel);
		}
		if (inPM == 1)
		{
			m_PDBoxCmd.GetSiglePower(nchannel+1, &dblPout);
		}
		dblPref = m_dblReferencePower[nchannel];
		ChannalMessage[nchannel].dblIL = TestIL(dblPref, dblPout);
		strMsg.Format("通道%d,IL:%.2fdB,Power:%.2fdBm,Ref:%.2fdBm",
			nchannel+1,ChannalMessage[nchannel].dblIL,dblPout,dblPref);
	    LogInfo(strMsg);
	}
	SaveDataToCSV(IL_ROOM_DATA,m_nSNIndex);
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

BOOL CMy126S_45V_Switch_AppDlg::SaveCSVFileScanDate(DWORD dwPhaseCount)
{
   BOOL bFunctionOK = FALSE;
   DWORD dwBytesWrite,dwBytesReturned;
   CString strFileName,strTmptValue,strSourcePath;
   HANDLE hCSVFile = INVALID_HANDLE_VALUE;
  
   //设置文件
   strFileName.Format("%s\\data\\Scan_Data.csv",m_strLocalPath);
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
	   int j = 0;
	   nchannel = m_cbChannel.GetCurSel();
	   for ( j=0;j<12;j++)
	   {
		  if(nchannel == j)
		   {	  
			   strTmptValue.Format("%d,%.2f,%.2f,%0.2f\n",j+1,ScanPowerChannal[j].VoltageX,
				   ScanPowerChannal[j].VoltageY,ScanPowerChannal[j].MaxPower);
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
		   m_strLocalPath);

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
		   int i = 5;
		   for ( i=5;i<10;i++)
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

BOOL CMy126S_45V_Switch_AppDlg::SendMsgToSwitch(int nTime,int nCount)
{
	CString strTemp;
	char pbyData[30]={0};
// 	BYTE bDir;
    char chReData[64];
	ZeroMemory(pbyData,sizeof(char)*20);
	ZeroMemory(chReData,sizeof(chReData));

	if(m_nCHCount==16)
	{
		strTemp.Format("SWITCH RAND 1 %d %d 32\r\n",nTime,nCount);
	}
	else
	{
		strTemp.Format("SWITCH RAND 1 %d %d %d\r\n",nTime,nCount,m_nCHCount);
	}
	memcpy(pbyData,strTemp,strTemp.GetLength());
    if(!m_opTesterCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		MessageBox("串口没有打开！");
		return FALSE;
	}
	Sleep(50);

    return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SETX(int x)
{
  CString strTemp;
  char pbyData[20];
  char pbyRx[20];  

  ZeroMemory(pbyData,sizeof(char)*20);
  ZeroMemory(pbyRx,sizeof(char)*20);

  CString strMsg;

  strTemp.Format("setx 1 %d\r\n",x);
  memcpy(pbyData,strTemp,strTemp.GetLength());
  if(!m_opTesterCom.WriteBuffer(pbyData,strTemp.GetLength()))
  { 
     MessageBox("串口没打开，设置x电压错误！");
     return FALSE;
  }
  Sleep(100);
  if(!m_opTesterCom.ReadBuffer(pbyRx,20))
  { 
     MessageBox("串口没打开，设置x电压错误！");
     return FALSE;
  }
  strMsg=pbyRx;
  if(strMsg.Find("OK")==-1)
  {
	  return FALSE;
  }

  return TRUE;
    
}

BOOL CMy126S_45V_Switch_AppDlg::SETY(int y)
{
	CString strTemp;
	char pbyData[20];
	char pbyRx[20];  
	
	ZeroMemory(pbyData,sizeof(char)*20);
	ZeroMemory(pbyRx,sizeof(char)*20);
	CString strMsg;
	strTemp.Format("sety 1 %d\r\n",y);
	memcpy(pbyData,strTemp,strTemp.GetLength());
	if(!m_opTesterCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{ 
		MessageBox("串口没打开，设置y电压错误！");
		return FALSE;
	}

	Sleep(100);
	if(!m_opTesterCom.ReadBuffer(pbyRx,20))
	{ 
		 MessageBox("串口没打开，设置x电压错误！");
		 return FALSE;
	}
	strMsg=pbyRx;
	if(strMsg.Find("OK")==-1)
	{
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

	strTemp.Format("schn %d %d %d\r\n",nchannel,x,y);
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

BOOL CMy126S_45V_Switch_AppDlg::SPRT(int nChannel)
{
	CString strTemp;
	char pbyData[30];
	ZeroMemory(pbyData,sizeof(char)*20);

	strTemp.Format("SPRT %d\r\n",nChannel+1);
	memcpy(pbyData,strTemp,strTemp.GetLength());

    if(!m_opCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		MessageBox("串口未打开，通道设置错误！");
		return FALSE;
	}
	Sleep(50);
    return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::SWToChannel(int nChannel,bool bSwitchTime)
{
	if (!SETX(ScanPowerChannal[nChannel].VoltageX))
	{
		MessageBox("设置电压错误！");
		return FALSE;
	}
	Sleep(200);
	if (!SETY(ScanPowerChannal[nChannel].VoltageY))
	{
		MessageBox("设置电压错误！");
		return FALSE;
	}
	Sleep(200);
	return TRUE;
}


void CMy126S_45V_Switch_AppDlg::OnButtonExit() 
{
	// TODO: Add your control notification handler code here
	m_bTestStop = TRUE;
	m_bReadPowerStop = TRUE;
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
     
	if (1 == nIDEvent)
	{
		double dblPDLIL;
		int nChannel;
		nChannel = m_cbChannel.GetCurSel();							
		dblPDLIL = ReadPWMPower(nChannel);											
		m_dblPDLData[m_iReadPMCount] = dblPDLIL;
		m_iReadPMCount++;

		m_NTGraph.PlotXY(m_iReadPMCount,dblPDLIL,0);
		
		if (m_iReadPMCount > 200||m_bTestStop)
		{
			m_iCountPDL = m_iReadPMCount;
			m_iReadPMCount = 0;
	//		SetDlgItemText(IDC_BUTTON_PDL_TEST,"PDL测试");
			m_bPDLStop=FALSE;
			LogInfo("测试完毕！");
			KillTimer(1);
		}
	}
	else if (2 == nIDEvent)
	{
		m_iTimerCount++;
		CString strMsg;
		strMsg.Format("剩余时间：%d s",40-m_iTimerCount);
		LogInfo(strMsg);

		if (m_iTimerCount>=40)
		{
			
			KillTimer(2);
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
			CString strNetFile;
			CStdioFile stdCSVFile;
			CString strContent;
			HANDLE  hHandle = INVALID_HANDLE_VALUE;
			WIN32_FIND_DATA win32Find;
			int    iSel=IDYES;
			CString strMsg;
			double dblPref;
			double dblPout;
			int nChannel = 0;

			int nComPort = m_cbComPort.GetCurSel();
			
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_Repeat-COM%d.csv",
				m_strLocalPath,m_strItemName,m_strSN,m_strSN,nComPort);
			strNetFile.Format("%s\\data\\%s\\%s\\%s_IL_Repeat-COM%d.csv",
				m_strNetFile,m_strItemName,m_strSN,m_strSN,nComPort);
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
			Sleep(200);  //add by ris 2014-10-11
			for ( nChannel = 0; nChannel < m_nCHCount; nChannel ++)
			{
				if(m_cbComPort.GetCurSel()==1)
					SPRTProductCH(nChannel+1+m_nCHCount);
				else
					SPRTProductCH(nChannel+1);
				Sleep(100);
				dblPref = m_dblReferencePower[nChannel];
				dblPout = ReadPWMPower(nChannel);
				
				ChannalMessage[nChannel].dblRepeat = TestIL(dblPref,dblPout);
				ChannalMessage[nChannel].dblDif = ChannalMessage[nChannel].dblIL-ChannalMessage[nChannel].dblRepeat;
				if (!CheckParaPassOrFail(RE_IL_DATA,ChannalMessage[nChannel].dblDif,nChannel))
				{
					strMsg.Format("切换开关后,通道%d的IL为:%.2f,重复性为:%.2f,参数不合格",nChannel+1,
					ChannalMessage[nChannel].dblRepeat,ChannalMessage[nChannel].dblDif);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				}
				else
				{
					strMsg.Format("切换开关后,通道%d的IL为:%.2f,重复性为:%.2f",nChannel+1,
					ChannalMessage[nChannel].dblRepeat,ChannalMessage[nChannel].dblDif);
					LogInfo(strMsg);
				}
				
				LogInfo(strMsg,FALSE);
		//		m_NTGraph.PlotXY(nChannel+1,ChannalMessage[nChannel].dblDif,0);
				
				strContent.Format("%d,%.3f,%.3f,%.3f\n",nChannel+1,ChannalMessage[nChannel].dblIL,ChannalMessage[nChannel].dblRepeat,ChannalMessage[nChannel].dblDif);
				stdCSVFile.WriteString(strContent);
				//保存重复性数据
			}	
			stdCSVFile.Close();	
			if (m_bIfSaveToServe)
			{
				CopyFile(strCSVFile,strNetFile,FALSE);
			}
	        SaveDataToCSV(RE_IL_DATA);
			LogInfo("保存重复性数据结束!");
		}
		
	}
	else if (3 == nIDEvent)
	{
		YieldToPeers();
		UpdatePMData();
		if(m_bReadPowerStop)
		{
			KillTimer(3);
		}

	}
	else if(4 == nIDEvent)
	{
		
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
	return ;
}

BOOL CMy126S_45V_Switch_AppDlg::SetR152WL(int nChannel,double dblWaveLength)
{
	
	BOOL bFunctionOK;
	/*
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
	*/

	return bFunctionOK;
}

double CMy126S_45V_Switch_AppDlg::ReadR152Power(int nChannel)
{
	/*
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
	*/
	return -999;
}

void CMy126S_45V_Switch_AppDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	m_ctrlListMsg.ResetContent();

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
	int i = 0;
	for (nchannel = 0; nchannel <12 ; nchannel++)
	{
		strTemp.Format("开始测试通道%d的IL及CT",nchannel+1);
		LogInfo(strTemp,TRUE);
		dblPref = m_dblReferencePower[nchannel];
				
		for (i=0;i<12;i++)
		{
			SETX(ScanPowerChannal[i].VoltageX);
			SETY(ScanPowerChannal[i].VoltageY);
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
//         ScanPowerChannal[i].VoltageX =VolueX;
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
// 	    ScanPowerChannal[i].VoltageY = VolueY;
// 		
// 		SCHN(i,VolueX,VolueY);
//         strMsg.Format("调用扫描数据，设置通道%d的电压坐标为(%d,%d)",i+1,VolueX,VolueY);
// 	}
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
	if (!m_bChangeSwitch)
	{
		MessageBox("请先随机切换开关！");
		return;
	}
	//设置路径
	UpdateData();
    FindMyDirectory("data",m_strPN,m_strSN);

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
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_Repeat.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN);
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_IL_Repeat-COM1.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN);
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

//nChannel:0~20
double CMy126S_45V_Switch_AppDlg::ReadPWMPower(int nChannel)
{
	double	dblPower;
	char chSendBuf[256];
	char chReadBuf[256];
	CString strSend;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));

//	SPRTTesterCH(nChannel+1);
//	Sleep(50);

	strSend.Format("get power %d\r\n",nChannel+1);
	memcpy(chSendBuf,strSend,strSend.GetLength());
	
	if (!m_opTesterCom.WriteBuffer(chSendBuf,strSend.GetLength()))
	{
		LogInfo("发送Get power错误！");
		dblPower=-99;
		return dblPower;
	}
	Sleep(50);
	if (!m_opTesterCom.ReadBuffer(chReadBuf,256))
	{
		LogInfo("接收Get power错误！");
		dblPower=-99;
		return dblPower;
	}
	dblPower = atof(chReadBuf);
	dblPower = dblPower/100.0;
	return dblPower;
}
BOOL CMy126S_45V_Switch_AppDlg::SetPWMWL(int nChannel, double dblWL)
{
	double	dblPower;
	char chSendBuf[256];
	char chReadBuf[256];
	CString strSend;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));

//	SPRTTesterCH(nChannel+1);
//	Sleep(50);

	strSend.Format("set wl %d %.2f \r\n",nChannel+1, dblWL);
	memcpy(chSendBuf,strSend,strSend.GetLength());
	
	if (!m_opTesterCom.WriteBuffer(chSendBuf,strSend.GetLength()))
	{
		LogInfo("发送Get power错误！");
		dblPower=-99;
		return dblPower;
	}
	Sleep(50);
	if (!m_opTesterCom.ReadBuffer(chReadBuf,256))
	{
		LogInfo("接收Get power错误！");
		dblPower=-99;
		return dblPower;
	}
	dblPower = atof(chReadBuf);
	dblPower = dblPower/100.0;
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

	Sleep(500);

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
	
//	UpdateData();

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
	//iSocket = g_socket_fd[bBox];

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

/*
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
*/
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
	char    sep[] = "\r\n";
	int     nCount=0;
	//	int     nSwitch;
	int     nPort;
	int i;
	double dblTemp;
	BYTE     pbySendCommand[MAX_LINE];
	int   nVoltX;
	int   nVoltY;
	CString strVolt;
	
	DownLoadFW();
	return;
	ZeroMemory(chSetCommand,sizeof(char)*MAX_LINE);
	ZeroMemory(chReadCommand,sizeof(char)*MAX_COUNT);
	ZeroMemory(ptCommand,sizeof(BYTE)*MAX_COUNT);
	ZeroMemory(pbySendCommand,sizeof(pbySendCommand));
	
	UpdateData();
	if (!m_bTesterPort)
	{
		MessageBox("请先打开对应的串口！");
		return;
	}
  //  m_ctrlListMsg.ResetContent();
	nCommand = m_cbMessage.GetCurSel();
	nPort = m_cbChannel.GetCurSel();
	//默认值
	switch (nCommand)
	{
	case READ_INFO:
		strCommand.Format("INFO 1\r\n");
		break;
	case SET_OPTICAL_ROUTE:	
		if(m_cbComPort.GetCurSel()==1)
		{
			nPort=nPort+m_nCHCount;
		}
		
		if(!SPRTProductCH(nPort))
		{
			LogInfo("发送指令失败！");
		}
		else
		{
			LogInfo("发送指令成功！");
		}
	//	SWToChannel(nPort-1);
//		strCommand.Format("SPRT %d\r\n",nPort);
		return;
	case GET_PORT_NUMBER:	
		strCommand.Format("GPRT 1\r\n");
		break;
	case GDAC:
		strCommand.Format("GDAC 1\r\n");
		break;
	case GET_ALARM:
		strCommand.Format("GALM 1\r\n");
		break;
	case SET_RESET:
		strCommand.Format("RSET 1\r\n");
		break;
	case OPLINK_VERSION:
		strCommand.Format("OPLK 1\r\n");
		break;
	case SETX_COMMAND:
		GetDlgItemText(IDC_EDIT_X,strVolt);
		nVoltX = atoi(strVolt);
		if(SETX(nVoltX))
		{
			strMsg.Format("OK");
			LogInfo(strMsg);
		}
		return;
	case SETY_COMMAND:	
		GetDlgItemText(IDC_EDIT_Y,strVolt);
		nVoltY = atoi(strVolt);
		if(SETY(nVoltY))
		{
			strMsg.Format("OK");
			LogInfo(strMsg);
		}
		return;
	case DOWNLOAD_FW:	
		DownLoadFW();
		return;
	case GET_TEMP:	
		dblTemp = GetModuleTemp();
		strMsg.Format("%.1f",dblTemp);
		LogInfo(strMsg,TRUE);
		return;
	case SHOW_VOL:
		for ( i=0;i<m_nCHCount;i++)
		{
			strMsg.Format("通道%d的电压坐标为:%d,%d; IL:%.2f",i+1,ScanPowerChannal[i].VoltageX,ScanPowerChannal[i].VoltageY,ScanPowerChannal[i].MaxPower);
			LogInfo(strMsg);
		}
		return;
	case SPRT_TESTER_BOX:
		if(!SPRTTesterCH(nPort))
		{
			LogInfo("发送指令失败！");
		}
		else
		{
			LogInfo("发送指令成功！");
		}
		return;

	default:
		;
	}
//	YieldToPeers();
	//发送命令
	memcpy(chSetCommand,strCommand,strCommand.GetLength());
	
	if (!m_opTesterCom.WriteBuffer(chSetCommand,strCommand.GetLength()))
	{
		LogInfo("串口通讯错误！",(BOOL)FALSE,COLOR_RED);
		return;
	}
	Sleep(dwWaitTime);	
	
	if (!m_opTesterCom.ReadBuffer(chReadCommand,MAX_COUNT))
	{
		LogInfo("读取数据错误！",(BOOL)FALSE,COLOR_RED);
		return;
	}
	LogInfo(chReadCommand,TRUE);

}

void CMy126S_45V_Switch_AppDlg::OnSelchangeComboSendMessage() 
{
	// TODO: Add your control notification handler code here
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
		strFileName.Format("%s\\data\\ZeroFile_%d.csv",m_strLocalPath,nchannel+1);
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
/*
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
    char    chValue[20];
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
	strINIFile.Format("%s\\config\\%s\\%s",m_strConfigPath,m_strPN,strFileINI);
	strKey.Format("CH%d",m_nReadPwrCH[nChannel]+1);

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

	//获取中心点信息：
	int nXSpace1;
	int nXSpace2;
	int nXSpace3;
	int nXSpace4;
	int nYSpace1;
	int nYSpace2;
	int nYSpace3;
	CString strCHFile;
	strCHFile.Format("%s\\config\\%s\\ChannelCenter.ini",m_strConfigPath,m_strPN);
	//strKey.Format("CH%d",m_nReadPwrCH[nChannel]+1);
	GetPrivateProfileString("XSpace1","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nXSpace1 = atoi(strValue);
	
	GetPrivateProfileString("XSpace2","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nXSpace2 = atoi(strValue);
	
	GetPrivateProfileString("XSpace3","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nXSpace3 = atoi(strValue);
	
	GetPrivateProfileString("XSpace4","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nXSpace4 = atoi(strValue);

	GetPrivateProfileString("YSpace1","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nYSpace1 = atoi(strValue);
	
	GetPrivateProfileString("YSpace2","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nYSpace2 = atoi(strValue);
	GetPrivateProfileString("YSpace3","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nYSpace3 = atoi(strValue);

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

	switch (m_nReadPwrCH[nChannel])
	{
	case 0:
		GetDlgItemText(IDC_EDIT_X,strVolt);
		x = atoi(strVolt);
		
		GetDlgItemText(IDC_EDIT_Y,strVolt);
		y = atoi(strVolt);
		break;
	case 1:
		x = m_Xbase[0] + nXSpace1;
		y = m_Ybase[0];
		break;
	case 2:
		x = m_Xbase[1] + nXSpace2;
		y = m_Ybase[1];
		break;
	case 3:
		x = m_Xbase[2] + nXSpace3;
		y = m_Ybase[2];	
		break;
	case 4:
		x = m_Xbase[3] + nXSpace4;
		y = m_Ybase[3];
		if (!bScanRange)
		{
			x = m_Xbase[0] + nXSpace1+nXSpace2+nXSpace3+nXSpace4;
			y = m_Ybase[0];
		}
		break;
	case 5:
		x = m_Xbase[4];
		y = m_Ybase[4]-nYSpace1;
		break;
	case 6:
		x = m_Xbase[5]-nXSpace4;
		y = m_Ybase[5];
		break;
	case 7:
		x = m_Xbase[6]-nXSpace3;
		y = m_Ybase[6];
		break;
	case 8:
		x = m_Xbase[7]-nXSpace2;
		y = m_Ybase[7];	
		break;
	case 9:
		x = m_Xbase[8]-nXSpace1;
		y = m_Ybase[8];
		break;
	case 10:
		x = m_Xbase[9];
		y = m_Ybase[9]-nYSpace2;
		break;
	case 11:
		x = m_Xbase[10]+nXSpace1;
		y = m_Ybase[10];
		break;
	case 12:
		x = m_Xbase[11]+nXSpace2;
		y = m_Ybase[11];
		break;
	case 13:
		x = m_Xbase[12]+nXSpace3;
		y = m_Ybase[12];
		break;
	case 14:
		x = m_Xbase[13]+nXSpace4;
		y = m_Ybase[13];
		break;
	case 15:
		x = m_Xbase[14];
		y = m_Ybase[14]-nYSpace3;
		if (!bScanRange)
		{
			x = m_Xbase[4];
			y = m_Ybase[4] - nYSpace1-nYSpace2-nYSpace3;
		}
		break;
	case 16:
		x = m_Xbase[15] - nXSpace4;
		y = m_Ybase[15];
		break;
	case 17:
		x = m_Xbase[16]-nXSpace3-nXSpace2;
		y = m_Ybase[16];
		break;
	case 18:
		x = m_Xbase[17]-nXSpace1;
		y = m_Ybase[17];
		if (!bScanRange)
		{
			x = m_Xbase[0];
			y = m_Ybase[15];
		}
		break;
	}

	//开始扫描通道
	int nDACX = x*MAX_DAC/MAX_VOLTAGE;
	int nDACY = y*MAX_DAC/MAX_VOLTAGE;
	CString strSendBuf;
	char chSendBuf[50];
	char chReadBuf[256];
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	strSendBuf.Format("SCAN CHANNEL 1 %d %d %d\r\n",m_nReadPwrCH[nChannel]+1,nDACX,nDACY);
	LogInfo(strSendBuf);
	memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
	if(!m_opTesterCom.WriteBuffer(chSendBuf,strSendBuf.GetLength()))
	{
		strMsg.Format("发送指令%s失败!",strSendBuf);
		LogInfo(strMsg);
		return FALSE;
	}
	Sleep(50);
	if (!m_opTesterCom.ReadBuffer(chReadBuf,256))
	{
		strMsg.Format("接收指令%s失败!",strSendBuf);
		LogInfo(strMsg);
		return FALSE;
	}
	strToken = strtok(chReadBuf,"\r:");
	strToken = strtok(NULL,"\r:");
	ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX = atoi(strToken)*MAX_VOLTAGE/MAX_DAC;
	m_Xbase[m_nReadPwrCH[nChannel]] = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX;
	strToken = strtok(NULL,"\r:");
	strToken = strtok(NULL,"\r:");
	ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY = atoi(strToken)*MAX_VOLTAGE/MAX_DAC;
	m_Ybase[m_nReadPwrCH[nChannel]] = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY;
	strToken = strtok(NULL,"\r:");
	strToken = strtok(NULL,"\r:");
	ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower = atoi(strToken);

	strMsg.Format("通道%d的电压点为：%d,%d,对应的ADC为%.0f！",nChannel+1,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX,
		ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY,ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower);
	LogInfo(strMsg);
	if(ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower<200)
	{
		return FALSE;
	}

	//保存文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	int    iSel=IDYES;

	if (m_cbComPort.GetCurSel()==1)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
	}
	else
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
	}
	
	if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建数据文件失败！");
		return FALSE;
	}
	stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");
//	for (int i=0;i<m_nCHCount;i++)
	{
		strContent.Format("%d,%d,%d,%.2f\n",m_nReadPwrCH[nChannel]+1,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY,ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower);
		stdCSVFile.WriteString(strContent);
	}
	stdCSVFile.Close();	
	return TRUE;

	
	while (1)
	{
		YieldToPeers();
		if (m_bTestStop)
		{
			return FALSE;
		}
			
		if(!SETY(y))
		{
			LogInfo("设置Y电压错误！",(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		Sleep(100);
		if(!SETX(x))
		{
			LogInfo("设置X电压错误！",(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		Sleep(400);

		m_NTGraph.PlotXY(x,y,0);
		
		dblPower =  m_dblReferencePower[m_nReadPwrCH[nChannel]]-ReadPWMPower(m_nReadPwrCH[nChannel]);
		if (dblPower<dblPrePower)
		{
			dblPrePower = dblPower;
			m_Xbase[m_nReadPwrCH[nChannel]] = x;
			m_Ybase[m_nReadPwrCH[nChannel]] = y;
		}
		strMsg.Format("X:%d mv,Y:%d mv,IL:%.2f dB",x,y,dblPower);
		LogInfo(strMsg);

		if (bCenterPoint1==FALSE&&dblPrePower<45)  //小于45dB时，重新取中心点开始扫描
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
		if (bCenterPoint2==FALSE&&dblPrePower<30)  //小于30dB时，重新取中心点开始扫描
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
		if (dblPrePower < 20)  //跳出条件
		{
			strMsg.Format("通道%d已经扫描到！",nChannel+1);
			LogInfo(strMsg);
			return	TRUE;
		}
		count++;
		if (count==nFlag) //换step符号
		{
			n++;
			nFlag = nFlag+2*n;
			x_step = -x_step;
			y_step = -y_step;
			XCount++;
			YCount++;
			nXflag=0;
			nYflag=0;
		}
		
		if (nYflag<YCount)
		{
			y = y + y_step;
			nYflag++;
		}
		else if (nXflag<XCount)
		{
			x = x + x_step;
			nXflag++;
		}
		else
		{
			//reserved
		}

		if (x<x_start)
		{
			x = x_start;
		}
		if (y<y_start)
		{
			y = y_start;
		}
		if(x>x_stop)
		{
			x = x_stop;
		}
		if (y>y_stop)
		{
			y=y_stop;
		}
		
		if(count>=400)
		{
			break;
		}			
	}
	
	strMsg.Format("通道%d没有扫描到IL小于20dB的点，请重新配置该通道扫描范围！",nChannel+1);
	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
	return FALSE;
	
}
*/
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

    char    chValue[20];
	ZeroMemory(chValue,sizeof(char)*20);

	x_base = m_Xbase[m_nReadPwrCH[nChannel]];
	y_base = m_Ybase[m_nReadPwrCH[nChannel]];

//获取基准点功率
	Sleep(100);
	SETX(x_base);
	Sleep(50);
	SETY(y_base);
	Sleep(300);
	dblBasePower =  m_dblReferencePower[m_nReadPwrCH[nChannel]]-ReadPWMPower(m_nReadPwrCH[nChannel]);

	//扫描第一个点的光功率值，若异常，则重复读光功率计5次
	while (dblBasePower > 30 && x_count < 5)
	{
		Sleep(100);
		dblBasePower =  m_dblReferencePower[m_nReadPwrCH[nChannel]]-ReadPWMPower(m_nReadPwrCH[nChannel]);
		x_count++;
	}
	while (dblBasePower == 0 && x_count < 5)
	{
		Sleep(100);
		dblBasePower = m_dblReferencePower[m_nReadPwrCH[nChannel]]-ReadPWMPower(m_nReadPwrCH[nChannel]);
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

	int nCount = 0;
	while (1)
	{
//y     
		SETX(x);
		Sleep(50);
		dblBasePower_Y = dblBasePower_X;
		while(1)
		{
			YieldToPeers();
			if(m_bTestStop)
			{
				return FALSE;
			}		
			y_step1 = 500;
			y_step2 = -500;
			//设置步数
			if (dblBasePower_Y < 15)
			{
				y_step1 = 300;
				y_step2 = -300;
			}
			if (dblBasePower_Y < 10)
			{
				y_step1 = 150;
				y_step2 = -150;
			}
			if (dblBasePower_Y < 5)
			{
				y_step1 = 80;
				y_step2 = -80;
			}
			if (dblBasePower_Y < 1.5)
			{
				y_step1 = 30;
				y_step2 = -30;
			}
			if (dblBasePower_Y < 0.8)
			{
				y_step1 = 7;
				y_step2 = -7;
			}
			if (m_strPN != "1X19")
			{
				if (nChannel>=4&&nChannel<8)
				{
					y_step1 = 50;
			    	y_step2 = -50;
				}
			}

			y_step = y_step1;
			if (bFlagY)
			{
				y_step = y_step2;			
			}
			y = y + y_step;
			SETY(y);
			Sleep(100);
			dblPower =  m_dblReferencePower[m_nReadPwrCH[nChannel]]-ReadPWMPower(m_nReadPwrCH[nChannel]);
			iCount = 0;
			while (dblPower <= 0&&iCount<5)
			{
				Sleep(100);
				dblPower =  m_dblReferencePower[m_nReadPwrCH[nChannel]]-ReadPWMPower(m_nReadPwrCH[nChannel]);
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
		SETY(y);
		Sleep(50);
		while(1)
		{
			YieldToPeers();
			if(m_bTestStop)
			{
				return FALSE;
			}
			x_step1 = 500;
			x_step2 = -500;

			if (dblBasePower_X < 15)
			{
				x_step1 = 300;
				x_step2 = -300;
			}
			if (dblBasePower_X < 10)
			{
				x_step1 = 150;
				x_step2 = -150;
			}
			if (dblBasePower_X < 5)
			{
				x_step1 = 80;
				x_step2 = -80;
			}
			if (dblBasePower_X < 1.5)
			{
				x_step1 = 30;
				x_step2 = -30;
			}
			if (dblBasePower_X < 0.8)
			{
				x_step1 = 8;
				x_step2 = -8;
			}
			if(m_strPN=="1X19")
			{
				if (nChannel==2||nChannel==7||nChannel==12||nChannel==17) 
				{
					x_step1 = 10;
					x_step2 = -10;
				}
			}		
			x_step = x_step1;
			if (bFlagX)
			{
				x_step = x_step2;		
			}
			x = x + x_step;
			SETX(x);
			Sleep(100);
			dblPower = m_dblReferencePower[m_nReadPwrCH[nChannel]]-ReadPWMPower(m_nReadPwrCH[nChannel]);
			iCount = 0;
			while (dblPower <= 0 && iCount<5)
			{
				Sleep(100);
				dblPower = m_dblReferencePower[m_nReadPwrCH[nChannel]]-ReadPWMPower(m_nReadPwrCH[nChannel]);
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
		if (fabs(dblBasePower_X - dblBasePower_Y)<=0.01 && dblBasePower_X < 0.8)
		{
			break;
		}
		nCount++;
		if (nCount>=10)
		{
			strMsg.Format("通道%d没有找到IL小于0.8dB的点！",m_nReadPwrCH[nChannel]+1);
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			break;				
		}
	}

	ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX = x_base;
	ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY = y_base;
	ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower = dblBasePower_X;

	strMsg.Format("通道%d的最大插损为：%.2fdBm，对应的电压坐标为：(%d,%d)",m_nReadPwrCH[nChannel]+1,ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower,
		ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY);
	LogInfo(strMsg,FALSE);

	//保存文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	int    iSel=IDYES;

	if (m_cbComPort.GetCurSel()==1)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
	}
	else
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_ScanVoltage_%d.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
	}
	
	if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建数据文件失败！");
		return FALSE;
	}
	stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");
//	for (int i=0;i<m_nCHCount;i++)
	{
		strContent.Format("%d,%d,%d,%.2f\n",m_nReadPwrCH[nChannel]+1,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY,ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower);
		stdCSVFile.WriteString(strContent);
	}
	stdCSVFile.Close();	
	return TRUE;
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonRegulateScan() 
{
	// TODO: Add your control notification handler code here
	//粗扫
	if(!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	m_bTestStop = FALSE;
	m_bReadPowerStop = TRUE;
	
	CString strMsg;
	strMsg.Format("请连接好光路\n请将通道按顺序接入光功率计！");
	MessageBox(strMsg);
	//设置路径
	UpdateData();
   // FindMyDirectory("data",m_strPN,m_strSN);
	
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

	CString strSection;
	CString strKey;
	CString strINIFile;
	CString strValue;
	strINIFile.Format("%s\\config\\%s\\%s",m_strConfigPath,m_strPN,strFileINI);
	strSection.Format("ScanChannel");
	int i = 0;
	/*
	for ( i=0;i<4;i++)
	{
		strKey.Format("CH%d",i+1);
		GetPrivateProfileString(strSection,strKey,"ERROR",strValue.GetBuffer(128),128,strINIFile);
		if (strValue=="ERROR")
		{
			strMsg.Format("读取配置文件%s的关键字ScanChannel失败！",strFileINI);
			MessageBox(strMsg);
			return;
		}
	    m_nReadPwrCH[i] = atoi(strValue)-1;
 	}
	*/
	
	if (m_bAdjustScan)
	{
		strMsg.Format("已存在粗扫数据，是否重新粗扫？");
		iSel = MessageBox(strMsg,"提示",MB_YESNO|MB_ICONQUESTION);
	}
	for (i = 0;i<4;i++)
	{
		m_nReadPwrCH[i] = m_nAdjustCH[i];
	}
	if (iSel==IDYES)
	{
		int i = 0;
		for( i=0;i<4;i++)
		{
			if(!AutoScan4x3(i,strFileINI,FALSE))
			{			
				m_bAdjustScan = FALSE;
				MessageBox("粗扫出错！");
				return;
			}
		}	
		m_bAdjustScan = TRUE;		
	}
	
	//扫描结束，计算出转角
	CalculateAngle(ScanPowerChannal[m_nReadPwrCH[0]].VoltageX,ScanPowerChannal[m_nReadPwrCH[0]].VoltageY,ScanPowerChannal[m_nReadPwrCH[1]].VoltageX,ScanPowerChannal[m_nReadPwrCH[1]].VoltageY,
		ScanPowerChannal[m_nReadPwrCH[2]].VoltageX,ScanPowerChannal[m_nReadPwrCH[2]].VoltageY,ScanPowerChannal[m_nReadPwrCH[3]].VoltageX,ScanPowerChannal[m_nReadPwrCH[3]].VoltageY);
	//新加算法，计算X,Y电压建议值
	CalCulateVol();

	//保存调节数据
	SaveDataToCSV(ADJUST_DATA);
	
}



void CMy126S_45V_Switch_AppDlg::CreateExcelFile()
{
	//启动EXCEL服务器
	COleVariant  m_covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR);
	if (!m_app.CreateDispatch("EXCEL.Application",NULL))
	{
		AfxMessageBox("无法创建EXCEL应用！");
		m_bStartExcel = FALSE;
		return;
	}
	
	//允许其他用户控制EXCEL
	m_app.SetUserControl(TRUE);
	//打开EXCEL文件
	//设置路径
	UpdateData();
	//copy template
	m_strTemplatePath.Format("%s\\template\\%s 测试单.xls",m_strLocalPath,m_strPN);
//	m_strTemplatePath.Format("%s\\template\\77501351A2 WRS7-1130-A0测试单.xls",g_tszAppFolder);
	if (m_bIfSaveToServe)
	{
		if (m_cbComPort.GetCurSel()==0)
		{
			m_strOpticalDataPath.Format("%s\\%s\\%s_Optical_Test_Data.xls",m_strNetFile,m_strSN,m_strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			m_strOpticalDataPath.Format("%s\\%s\\%s_Optical_Test_Data-COM1.xls",m_strNetFile,m_strSN,m_strSN);
		}
		else
		{

		}	
	}
	else
	{
		if (m_cbComPort.GetCurSel()==0)
		{
			m_strOpticalDataPath.Format("%s\\data\\%s\\%s_Optical_Test_Data.xls",m_strLocalPath,m_strSN,m_strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			m_strOpticalDataPath.Format("%s\\data\\%s\\%s_Optical_Test_Data-COM1.xls",m_strLocalPath,m_strSN,m_strSN);
		}
		else
		{

		}	
	}	
    HANDLE             hDictory = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA    win32FindData;
	CString            strMsg;

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
	return TRUE;
    CString		strFileName;
	CString		strTmptValue;	
	HANDLE	hDirectory = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	winFindFileData;
	CString     strSNFile;

	CString strNewFile;
	//是否保存到网络路径
	CString strNetFileName;
	CString strSave;
	CString strTempFile;
	strNetFileName.Format("%s\\config\\DataSave.ini",g_tszAppFolder);
	int iLen = GetPrivateProfileString("Save","Save",NULL,strSave.GetBuffer(128),128,strNetFileName);
    if (iLen <= 0)
    {
		MessageBox("读取配置文件DataSave.ini失败");
		return FALSE;
    }
	//保存到网络路径
	if (strSave == "1")
	{
		CString strNetFilePath;
		strNetFilePath.Format("%s\\",m_strNetFile);
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

		strTempFile = strNetFilePath+"\\Ref";
        hDirectory = FindFirstFile(strTempFile,&winFindFileData);

		if (hDirectory == INVALID_HANDLE_VALUE)
		{
			if (!CreateDirectory(strTempFile,NULL))
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
				MessageBox("不能创建目录文件!", "文件错误", MB_OK | MB_ICONERROR);
				return FALSE;		// create subdir fail
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
				MessageBox("不能创建目录文件!", "文件错误", MB_OK | MB_ICONERROR);
				return FALSE;		// create subdir fail
			}
		}
		else
		   FindClose(hDirectory);
		int i = 0;
        for ( i=0 ; i<2;i++)
		{
			if (i == 0)
			{
				strSNFile = strNetFilePath + "\\" + "DetailedScan";		   
			}
			else if (i == 1)
			{
				strSNFile = strNetFilePath + "\\" + "GlancingScan";
			}
			
			hDirectory = FindFirstFile(strSNFile, &winFindFileData);
			if (INVALID_HANDLE_VALUE == hDirectory)					
			{
				if (!CreateDirectory(strSNFile, NULL))
				{
					MessageBox("不能创建目录文件!", "文件错误", MB_OK | MB_ICONERROR);
					return FALSE;		// create subdir fail
				}
			}
			else
	        	FindClose(hDirectory);
		}

	}

//本地路径
	if (m_strSN.IsEmpty())
	{
		MessageBox("请输入产品SN号!", "SN号输入", MB_OK | MB_ICONERROR);
		return  FALSE;
	}

	strFileName.Format("%s\\", g_tszAppFolder);
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

	strNewFile=strFileName+"\\Ref";
	hDirectory = FindFirstFile(strNewFile, &winFindFileData);
	
	if (INVALID_HANDLE_VALUE == hDirectory)
	{
		if (!CreateDirectory(strNewFile, NULL))
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
			MessageBox("不能创建目录文件!", "文件错误", MB_OK | MB_ICONERROR);
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
			MessageBox("不能创建目录文件!", "文件错误", MB_OK | MB_ICONERROR);
			return FALSE;		// create subdir fail
		}
	}
	else
		FindClose(hDirectory);
	int i = 0;
    for ( i=0 ; i<2;i++)
	{
	 
	   if (i == 0)
		{
			strSNFile = strFileName + "\\" + "DetailedScan";		   
		}
		else if (i == 1)
		{
			strSNFile = strFileName + "\\" + "GlancingScan";
		}
		
		hDirectory = FindFirstFile(strSNFile, &winFindFileData);
		if (INVALID_HANDLE_VALUE == hDirectory)					
		{
			if (!CreateDirectory(strSNFile, NULL))
			{
			   MessageBox("不能创建目录文件!", "文件错误", MB_OK | MB_ICONERROR);
			   return FALSE;		// create subdir fail
			}
	   }
	   else
		FindClose(hDirectory);
   }
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

	dblXVolatage0 = nXVolatage0*MAX_VOLTAGE/MAX_DAC/1000;
	dblYVolatage0 = nYVolatage0*MAX_VOLTAGE/MAX_DAC/1000;
	dblXVolatage3 = nXVolatage3*MAX_VOLTAGE/MAX_DAC/1000;
	dblYVolatage3 = nYVolatage3*MAX_VOLTAGE/MAX_DAC/1000;
	dblXVolatage8 = nXVolatage8*MAX_VOLTAGE/MAX_DAC/1000;
	dblYVolatage8 = nYVolatage8*MAX_VOLTAGE/MAX_DAC/1000;
	dblXVolatage11 = nXVolatage11*MAX_VOLTAGE/MAX_DAC/1000;
	dblYVolatage11 = nYVolatage11*MAX_VOLTAGE/MAX_DAC/1000;

    nAverageX1 = sqrt((dblXVolatage0*dblXVolatage0+dblXVolatage3*dblXVolatage3)/2)*1000; //计算平均值
	nAverageY1 = sqrt((dblYVolatage0*dblYVolatage0+dblYVolatage8*dblYVolatage8)/2)*1000;

	nAverageX2 = sqrt((dblXVolatage8*dblXVolatage8+dblXVolatage11*dblXVolatage11)/2)*1000; //计算平均值
	nAverageY2 = sqrt((dblYVolatage3*dblYVolatage3+dblYVolatage11*dblYVolatage11)/2)*1000;
    
    nAverageX = (nAverageX1+nAverageX2)/2;
	nAverageY = (nAverageY1+nAverageY2)/2;

    //m_ctrlListMsg.ResetContent();
	
	strMsg1.Format("电压值选取位置：(%d,%d)",nAverageX,nAverageY);

	//开始计算角度
	//计算比例系数
	nAverageY1 = nAverageY1/1000;
	nAverageX1 = nAverageX1/1000;
	nAverageY2 = nAverageY1/1000;
	nAverageX2 = nAverageX1/1000;
	double dblScale;
    dblScale = (nAverageY1*nAverageY1+nAverageY2*nAverageY2)/2*1.5;
//	if(dblScale==0)
//		dblScale=0;
//	else
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
//int nXVolatage0;
//	int nYVolatage0;
//	int nXVolatage3;
//	int nYVolatage3;
//	int nXVolatage8;
//	int nYVolatage8;
//	int nXVolatage11;
//	int nYVolatage11;
//	int nAverageX1;
//	int nAverageY1;
//	int nAverageX2;
//	int nAverageY2;
//	int nAverageX;
//	int nAverageY;
//	CString strMsg1;
//	CString strMsg2;
//	CString strMsg3;
//	double dblXVolatage0;
//	double dblYVolatage0;
//	double dblXVolatage3;
//	double dblYVolatage3;
//	double dblXVolatage8;
//	double dblYVolatage8;
//	double dblXVolatage11;
//	double dblYVolatage11;
//	
//	nXVolatage0 = nVolX0;
//	nYVolatage0 = nVolY0;
//	nXVolatage3 = nVolX3;
//	nYVolatage3 = nVolY3;
//	nXVolatage8 = nVolX8;
//	nYVolatage8 = nVolY8;
//	nXVolatage11 = nVolX11;
//	nYVolatage11 = nVolY11;
//
//	dblXVolatage0 = nXVolatage0/1000;
//	dblYVolatage0 = nYVolatage0/1000;
//	dblXVolatage3 = nXVolatage3/1000;
//	dblYVolatage3 = nYVolatage3/1000;
//	dblXVolatage8 = nXVolatage8/1000;
//	dblYVolatage8 = nYVolatage8/1000;
//	dblXVolatage11 = nXVolatage11/1000;
//	dblYVolatage11 = nYVolatage11/1000;
//
//    nAverageX1 = sqrt((dblXVolatage0*dblXVolatage0+dblXVolatage3*dblXVolatage3)/2)*1000; //计算平均值
//	nAverageY1 = sqrt((dblYVolatage0*dblYVolatage0+dblYVolatage8*dblYVolatage8)/2)*1000;
//
//	nAverageX2 = sqrt((dblXVolatage8*dblXVolatage8+dblXVolatage11*dblXVolatage11)/2)*1000; //计算平均值
//	nAverageY2 = sqrt((dblYVolatage3*dblYVolatage3+dblYVolatage11*dblYVolatage11)/2)*1000;
//    
//    nAverageX = (nAverageX1+nAverageX2)/2;
//	nAverageY = (nAverageY1+nAverageY2)/2;
//
//    m_ctrlListMsg.ResetContent();
//	
//	strMsg1.Format("电压值选取位置：(%d,%d)",nAverageX,nAverageY);
//
//	//开始计算角度
//	//计算比例系数
//	nAverageY1 = nAverageY1/1000;
//	nAverageX1 = nAverageX1/1000;
//	nAverageY2 = nAverageY1/1000;
//	nAverageX2 = nAverageX1/1000;
//	double dblScale;
//    dblScale = (nAverageY1*nAverageY1+nAverageY2*nAverageY2)/2*1.5;
//	if(dblScale==0)
//		dblScale=0;
//	else
//		dblScale = dblScale/((nAverageX1*nAverageX1+nAverageX2*nAverageX2)/2);
//
//	//计算转角
//	double dblXAngle0;
//	double dblXAngle3;
//	double dblXAngle8;
//	double dblXAngle11;
//	double dblYAngle0;
//	double dblYAngle3;
//	double dblYAngle8;
//	double dblYAngle11;
//	
//	dblXAngle0 = -nXVolatage0*nXVolatage0*dblScale;
//	dblXAngle3 = nXVolatage3*nXVolatage3*dblScale;
//	dblXAngle8 = -nXVolatage8*nXVolatage8*dblScale;
//	dblXAngle11 = nXVolatage11*nXVolatage11*dblScale;
//
//    dblYAngle0 = nYVolatage0*nYVolatage0;
//	dblYAngle3 = nYVolatage3*nYVolatage3;
//	dblYAngle8 = -nYVolatage8*nYVolatage8;
//	dblYAngle11 = -nYVolatage11*nYVolatage11;
//	
//	//计算斜率
//	double dblKX1;
//	double dblKY1;
//	double dblKX2;
//	double dblKY2;
//	double dblAngleX1;
//	double dblAngleX2;
//	double dblAngleY1;
//	double dblAngleY2;
//
//    dblKX1 = (dblYAngle3-dblYAngle0)/(dblXAngle3-dblXAngle0);
//	dblKX2 = (dblYAngle11-dblYAngle8)/(dblXAngle11-dblXAngle8);
//
//	dblAngleX1 = atan(dblKX1)/PI*180;
//	dblAngleX2 = atan(dblKX2)/PI*180;
//
//	strMsg2.Format("X需要转动的角度分别为：%.4f,%.4f",dblAngleX1,dblAngleX2);
//
//	if (dblXAngle8 == dblXAngle0)
//	{
//		dblAngleY1 = 90;
//	}
//	else
//	{
//		dblKY1 = (dblYAngle8-dblYAngle0)/(dblXAngle8-dblXAngle0);
//		dblAngleY1 = atan(dblKY1)/PI*180;
//	}
//
//	if (dblXAngle11 == dblXAngle3)
//	{
//		dblAngleY2 = 90;
//	}
//	else
//	{
//		dblKY2 = (dblYAngle11-dblYAngle3)/(dblXAngle11-dblXAngle3);
//		dblAngleY2 = atan(dblKY2)/PI*180;
//	}
//
//	if (dblAngleY1 != 0)
//	{
//		dblAngleY1 = (90-fabs(dblAngleY1))*(-dblAngleY1/fabs(dblAngleY1));
//	}
//	if (dblAngleY2 != 0)
//	{
//		dblAngleY2 = (90-fabs(dblAngleY2))*(-dblAngleY2/fabs(dblAngleY2));
//	}
//	
//	strMsg3.Format("Y需要转动的角度分别为：%.4f,%.4f",dblAngleY1,dblAngleY2);
//	LogInfo(strMsg1);
//	LogInfo(strMsg2);
//	LogInfo(strMsg3);
//	MessageBox(strMsg1);
//	strMsg2 = strMsg2+"\n"+strMsg3;
//	MessageBox(strMsg2);
//	
}

BOOL CMy126S_45V_Switch_AppDlg::LoadPMCfg(pstPMCfg pstPMInfo)
{
	CString strFileName;
	CString strSection;
	CString strKey;
	CHAR    chTemp[128];
	int i = 0;
	strFileName.Format("%s\\Config\\device.ini",m_strConfigPath);
	for( i=0; i<3; i++)
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

/*
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
*/

void CMy126S_45V_Switch_AppDlg::SaveDataToExcel(int nType,int nChannel)
{
	CString strValue;
	int nIndex=0;
	int nIndexCH=0;
	
	//启动EXCEL
	CreateExcelFile();
	//保存的数据类型
	switch (nType)
	{
	case VOL_DATA:
		//选择表单
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前-COM1")));
			}
			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后-COM1")));

			}	
		}
	
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%d",ScanPowerChannal[nIndex].VoltageX);
			m_range.SetItem(_variant_t((long)(10)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
			strValue.Format("%d",ScanPowerChannal[nIndex].VoltageY);
			m_range.SetItem(_variant_t((long)(12)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
		
	case IL_ROOM_DATA:
		//选择表单
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前-COM1")));
			}
			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后-COM1")));
				
			}	
		}
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblIL);
			m_range.SetItem(_variant_t((long)(14)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
	case CT_DATA:
		//选择表单
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前-COM1")));
			}
			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后-COM1")));
				
			}	
		}
		m_range.AttachDispatch(m_sheet.GetCells(),true);
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
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("调节测试单")));
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("调节测试单-COM1")));
		}	
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		if (m_cbSaveData.GetCurSel()==0)
		{
			for (nIndex=0;nIndex<4;nIndex++)
			{
				strValue.Format("%.2f",ScanPowerChannal[m_nReadPwrCH[nIndex]].MaxPower);
				m_range.SetItem(_variant_t((long)(9+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
				strValue.Format("%d",ScanPowerChannal[m_nReadPwrCH[nIndex]].VoltageX);
				m_range.SetItem(_variant_t((long)(10+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
				strValue.Format("%d",ScanPowerChannal[m_nReadPwrCH[nIndex]].VoltageY);
				m_range.SetItem(_variant_t((long)(11+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
			}
		}
		else if(m_cbSaveData.GetCurSel()==1)
		{
			for (nIndex=0;nIndex<4;nIndex++)
			{
				strValue.Format("%.2f",ScanPowerChannal[m_nReadPwrCH[nIndex]].MaxPower);
				m_range.SetItem(_variant_t((long)(23+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
				strValue.Format("%d",ScanPowerChannal[m_nReadPwrCH[nIndex]].VoltageX);
				m_range.SetItem(_variant_t((long)(24+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
				strValue.Format("%d",ScanPowerChannal[m_nReadPwrCH[nIndex]].VoltageY);
				m_range.SetItem(_variant_t((long)(25+nIndex*3)),_variant_t((long)(5)),_variant_t(strValue));
			}
		}
		break;
	case DARK_DATA:
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前-COM1")));
			}
			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后-COM1")));
				
			}	
		}
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblDarkIL);
			m_range.SetItem(_variant_t((long)(43)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;	
	case RE_IL_DATA:
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前-COM1")));
			}
			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后-COM1")));
				
			}	
		}
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblDif);
			m_range.SetItem(_variant_t((long)(22)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
	case ISO_DATA:
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶前-COM1")));
			}			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后")));
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				m_sheet.AttachDispatch(m_sheets.GetItem(_variant_t("点胶后-COM1")));
				
			}	
		}
		m_range.AttachDispatch(m_sheet.GetCells(),true);
		for(nIndex=0;nIndex<m_nCHCount;nIndex++)
		{
			strValue.Format("%.2f",ChannalMessage[nIndex].dblISO);
			m_range.SetItem(_variant_t((long)(40)),_variant_t((long)(4+nIndex)),_variant_t(strValue));
		}
		break;
		
	}
	//退出EXCEL
	CloseExcelFile();	
    //拷贝到本地
	CString strLocalExcel;
	strLocalExcel.Format("%s\\data\\%s\\%s_Optical_Test_Data.xls",
		g_tszAppFolder,m_strSN,m_strSN);
	if (m_bIfSaveToServe)
	{
		CopyFile(m_strOpticalDataPath,strLocalExcel,FALSE);
	}	
	LogInfo("保存数据到EXCEL结束！");

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
	int i = 0;

	for ( i=0;i<4;i++)
	{
		Vx[i] = (double)ScanPowerChannal[m_nReadPwrCH[i]].VoltageX;
		Vy[i] = (double)ScanPowerChannal[m_nReadPwrCH[i]].VoltageY;
	}

	//计算Vx,Vy比例系数	
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

	for (i=0;i<4;i++)
	{
		strMsg.Format("建议电压值%d为：(%d,%d)",i+1,RefVx[i],RefVy[i]);
		LogInfo(strMsg);
	}
	
}

void CMy126S_45V_Switch_AppDlg::OnButtonRlTest() 
{
	int nChannel = 0;
	int i=0;
	CString strMsg;
	UpdateData();
	if(!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bRefRL)
	{
		MessageBox("请先做RL系统归零！");
		return;
	}

	double dblPower;
	double dblMaxPower = -99;
	double dblRLSD;
	double dblRLS;
	LogInfo("开始测试产品RL,请等待......");
	if (m_cbChannel.GetCurSel()<=0)
	{
		MessageBox("请绕死产品所有的出光端，并将回损头接入1830C");
	}

	for (nChannel = 0;nChannel<m_nCHCount;nChannel++)
	{
		if (m_cbChannel.GetCurSel()!=0)
		{
			if (m_cbChannel.GetCurSel()!=(nChannel+1))
			{
				continue;
			}
		}
		dblMaxPower=-100;
		if(!SPRTProductCH(nChannel+1))
		{
			return;
		}
		if (m_cbChannel.GetCurSel()>0)
		{
			strMsg.Format("请绕死出光通道%d",nChannel+1);
	    	MessageBox(strMsg);
		}
		
		Sleep(500);
		for (i=0;i<5;i++)  //读取5次，取RL最差值
		{
			//YieldToPeers();
			if(g_pPM->GetPower(0,&dblPower))
			{
				MessageBox("读取光功率计错误！");
				return;
			}
			Sleep(100);
			if (dblPower>dblMaxPower)
			{
				dblMaxPower = dblPower;
			}
			strMsg.Format("功率值:%.2fdBm",dblPower);
			LogInfo(strMsg);
		}
		dblRLSD = m_dblReferencePower[nChannel] - dblMaxPower;
		
		dblRLS = pow(10,-m_dblSystemRL/10.0);
		dblRLSD = pow(10,-dblRLSD/10.0);
		if (dblRLSD<dblRLS)
		{
			ChannalMessage[nChannel].dblRL= 58;
		}
		else
		{
			ChannalMessage[nChannel].dblRL = -3.01-10.0*log10(dblRLSD-dblRLS);
		}
		if (!CheckParaPassOrFail(RL_DATA,ChannalMessage[nChannel].dblRL,nChannel))
		{
			strMsg.Format("CH%d,RL:%.1fdB,参数不合格",nChannel+1,ChannalMessage[nChannel].dblRL);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			strMsg.Format("CH%d,RL:%.1fdB",nChannel+1,ChannalMessage[nChannel].dblRL);
	    	LogInfo(strMsg);
		}
		
		LogInfo(strMsg);
	}
	LogInfo("RL测试完毕！");
	SaveDataToCSV(RL_DATA);
	SetVoltage(0);
}


//com14进光-12
//还是com15进光-9
VOID CMy126S_45V_Switch_AppDlg::GetVolDataFormFile(int InputPort)
{
	CString strFile;
	CStdioFile stdFile;
	char chBuffer[256];
	CString strValue;
	int i = 0;
	
	ZeroMemory(chBuffer,sizeof(chBuffer));
	strFile.Format("%s\\config\\VOL-DATA\\%s_ScanVoltage.csv",g_tszAppFolder,m_strSN);
	stdFile.Open(strFile,CFile::modeRead);
    stdFile.ReadString(chBuffer,256);
	for ( i=0;i<InputPort;i++)
	{
		stdFile.ReadString(chBuffer,256);
		strValue = strtok(chBuffer,",");
		strValue = strtok(NULL,",");
		m_Xbase[i] = atoi(strValue);
		strValue = strtok(NULL,",");
		m_Ybase[i] = atoi(strValue);
	}
	stdFile.Close();

}

//void CMy126S_45V_Switch_AppDlg::OnChangeEditSn() 
//{
//	// TODO: If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//	
//	// TODO: Add your control notification handler code here
//	m_bAutoScan = FALSE;
//	int i = 0;
//	for ( i=0;i<12;i++)
//	{
//		m_dwSingle[i] = 0;
//	}
//	
//}

/*
BOOL CMy126S_45V_Switch_AppDlg::ConnectPM()
{
	CString strInfo;
	CString strCom;
	TCHAR   tchCom[5];
	int i = 0;
	for( i=0;i<m_nPMCount;i++)
	{
		if (m_nPMType[i] == 0)
		{
			return TRUE;
		}
		else if (m_nPMType[i] == 1)
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
		else if (m_nPMType[i] == 2)
		{
			ZeroMemory(tchCom,sizeof(tchCom));
			strCom.Format("COM%d",m_nPMAddress[i]);
			memcpy(tchCom,strCom,strCom.GetLength());
			if(!m_comPM[i].OpenPort(tchCom,115200))
			{
				strInfo.Format("打开串口%d失败！",m_nPMAddress[i]);
				LogInfo(strInfo);
				return FALSE;
			}
			//ConnectIP(i);
		}
		else if (m_nPMType[i] == 3)
		{
			stDeviceSetInfo stPMInfo;
			stPMInfo.bHasDevice = TRUE;
			stPMInfo.nComType = GPIB;
			stPMInfo.nGPIBIndex =0;
			stPMInfo.nAddress = m_nPMAddress[i];
			stPMInfo.dwBaud =9600;
			stPMInfo.deCaption = N7745PM;
			stPMInfo.deType = PMDEVICE;
			
			m_PMN7744[i].InitialPM(&stPMInfo);
			
			strInfo.Format("正在初始化第%d台光功率计N7744...",i+1);
			LogInfo(strInfo);
			
			if (!m_PMN7744[i].OpenDevice())
			{
				strInfo.Format("打开光功率计%d失败!",i+1);
				MessageBox(strInfo);
				return FALSE;
			}
			int j = 1;
			for( j=1;j<=4;j++)
			{
				if(!m_PMN7744[i].SetPWMUnit(j,j,0))
				{
					strInfo.Format("设置单位到dBm失败！");
					LogInfo(strInfo,(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				if (!m_PMN7744[i].SetPWMAverageTime(j,0.01)) //平均时间：0.01ms
				{
					LogInfo("设置平均读取速率失败！",(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
			}	
			strInfo.Format("打开N7744光功率计%d成功！",i+1);
			LogInfo(strInfo);
		}
		
	}
	return TRUE;

}
*/
BOOL CMy126S_45V_Switch_AppDlg::ConnectIP(int nPMIndex)
{
	/*
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
	
	}
	*/
	return FALSE;
	

}

/*
BOOL CMy126S_45V_Switch_AppDlg::SetPWMWL(double dblWL,int nChannel)
{
    BYTE bSlot;
	BYTE bChannel;
	long lWL;
	int  i8163ACount = 0;
	int  i1830CCount = 0;
	int  iOplinkPMCount = 0;
	int  iN7744Count = 0;
	int i = 0;
	if (nChannel==32)  //设置所有通道波长
	{
		for( i=0;i<m_nPMCount;i++)
		{
			if (m_nPMType[i] == 0)
			{
				if(!m_HP816X.SetPWMWavelength(i/2+1,i%2,dblWL))
				{
					LogInfo("设置PM波长错误！",(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
				m_HP816X.SetPWMUnit(i/2+1,i%2,0);
				
			}
			else if (m_nPMType[i] == 1)
			{
				if(!m_PM1830C[i].SetPWMWavelength(1,0,dblWL))
				{
					LogInfo("设置1830C波长错误！",(BOOL)FALSE,COLOR_RED);
					return FALSE;
				}
			}
			else if (m_nPMType[i] == 2)
			{
				lWL = (long)(dblWL + 0.5);
				for (nChannel=0;nChannel<4;nChannel++)
				{
					bSlot = nChannel/2;
					bChannel = nChannel%2;		
					if(!SetWLFromOPCom(i,bSlot,bChannel,lWL))
					{
						LogInfo("设置自制光功率计波长错误！",(BOOL)FALSE,COLOR_RED);
						return FALSE;
					}				
				}	
			}
			else if (m_nPMType[i] == 3)
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
		}	
	}
	else //设置单一通道波长
	{
		int i = 0;
		for ( i=0;i<m_nPMCount;i++)
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
			else
			{
			}
		}
		if (nChannel < i8163ACount) 
		{
			if(!m_HP816X.SetPWMWavelength(i/2+1,i%2,dblWL))
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
				SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/2,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
			}
			else if ((nChannel-(i1830CCount+i8163ACount))/4==1)
			{
				SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/6,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
			}
			else if ((nChannel-(i1830CCount+i8163ACount))/4==2)
			{
				SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/10,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
			}
			else if ((nChannel-(i1830CCount+i8163ACount))/4==3)
			{
				SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/14,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
			}
			else if ((nChannel-(i1830CCount+i8163ACount))/4==4)
			{
				SetWLFromOPCom(iIndexOplink,(nChannel-(i1830CCount+i8163ACount))/16,(nChannel-(i1830CCount+i8163ACount))%2,lWL);
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
*/
void CMy126S_45V_Switch_AppDlg::OnSelchangeComboPn() 
{
	// TODO: Add your control notification handler code here
}

void CMy126S_45V_Switch_AppDlg::OnCloseupComboPn() 
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_COMBO_PN,m_strPN);
	/*
	if(m_strPN=="2x16")
	{
		//入光的选择按钮可见
		 GetDlgItem(IDC_COMBO_COMPORT)->ShowWindow(TRUE);
		 GetDlgItem(IDC_STATIC_COMPORT)->ShowWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_COMPORT)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_COMPORT)->ShowWindow(FALSE);
	}
	*/

}

BOOL CMy126S_45V_Switch_AppDlg::CheckParaPassOrFail(int nParaType,double dblValue,int nPort)
{
	//获取参数范围要求
	CString strIniFile;
	CString strSection;
	CString strValue;
	CString strMsg;
	double  dblMaxValue;
	double  dblMinValue;

	switch (nParaType)
	{
	case IL_HIGH_DATA:
	case IL_LOW_DATA:
	case IL_ROOM_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxRoomIL[nPort];
		dblMinValue = m_stLimitPara[0].dblMinRoomIL[nPort];
		break;
	case WDL_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxWDL[nPort];
		dblMinValue = m_stLimitPara[0].dblMinWDL[nPort];
		break;
	case TDL_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxTDL[nPort];
		dblMinValue = m_stLimitPara[0].dblMinTDL[nPort];
		break;
	case PDL_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxPDL[nPort];
		dblMinValue = m_stLimitPara[0].dblMinPDL[nPort];
		break;
	case RE_IL_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxRepeatIL[nPort];
		dblMinValue = m_stLimitPara[0].dblMinRepeatIL[nPort];
		break;
	case RL_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxRL[nPort];
		dblMinValue = m_stLimitPara[0].dblMinRL[nPort];
		break;
	case CT_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxCT[nPort];
		dblMinValue = m_stLimitPara[0].dblMinCT[nPort];
		break;
	case DARK_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxDarkIL[nPort];
		dblMinValue = m_stLimitPara[0].dblMinDarkIL[nPort];
		break;
	//case ISO_DATA:
		dblMaxValue = m_stLimitPara[0].dblMaxISO[nPort];
		dblMinValue = m_stLimitPara[0].dblMinISO[nPort];
		break;
	default:
		return FALSE;
	}

	/*
	strIniFile.Format("%s\\config\\%s\\ParaDemand.INI",m_strConfigPath,m_strPN);
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
	default:
		return FALSE;
	}

	GetPrivateProfileString(strSection,"MaxValue","error",strValue.GetBuffer(128),128,strIniFile);
	if (strValue == "error")
	{
		LogInfo("获取参数配置文件错误！",FALSE,COLOR_RED);
		return FALSE;
	}
	dblMaxValue = atof(strValue);

	GetPrivateProfileString(strSection,"MinValue","error",strValue.GetBuffer(128),128,strIniFile);
	if (strValue == "error")
	{
		LogInfo("获取参数配置文件错误！",FALSE,COLOR_RED);
		return FALSE;
	}
	dblMinValue = atof(strValue);
	*/

	if (dblValue>=dblMinValue&&dblValue<=dblMaxValue)
	{
		return TRUE;
	}
	//strMsg.Format("要求范围：%.2f~%.2f",dblMinValue,dblMaxValue);
	//LogInfo(strMsg);

	return FALSE;

}

double CMy126S_45V_Switch_AppDlg::GetModuleTemp()
{
	double dblTemp = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("GTMP 1\r\n");
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!m_opTesterCom.WriteBuffer(chData,strTemp.GetLength()))
    {
		LogInfo("串口打开错误！",(BOOL)FALSE,COLOR_RED);
		return -99;
	}
	Sleep(50);
	if (!m_opTesterCom.ReadBuffer(chReData,20))
	{
		LogInfo("读取错误，接收错误！",(BOOL)FALSE,COLOR_RED);
		return -99;
	}
    dblTemp = atof(chReData);	
	dblTemp = dblTemp/10.0;
	return dblTemp;


}

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
		if (!DataDownLoadZ4671(strFileName))
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
		if(!SendRevCommand("fwdl 1\r\n",&m_opTesterCom))
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
				throw "Error: Read data from bin file error";
			
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
				break;
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
		SendCommand("RSET 1\r\n");
		Sleep(200);
		SendCommand("RSET 1\r\n");

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
	if(!m_opTesterCom.WriteBuffer(pbBinData, wDownloadSize))
	{
		MessageBox("RS232发送数据错误!", "提示", MB_OK|MB_ICONERROR);
		return	XMODEM_COMMUNICATION_FAIL;
	}
	
	// Check Response
    while (1)
    {
        Sleep(50);
		nTimeOut = 0;
	 	if(m_opTesterCom.ReadBuffer(byTempBuf, MAX_BUF_DATA,&dwReadLength))
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
				if(!m_opTesterCom.WriteBuffer(pbBinData, wDownloadSize))
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

					if(!m_opTesterCom.WriteBuffer(byTempBuf, 3))
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
		
		if(!m_opTesterCom.WriteBuffer(byTempBuf, 3))
		{
			MessageBox("发送3个EOT完成Xmodem下载失败", "提示", MB_OK|MB_ICONERROR);
			return	XMODEM_COMMUNICATION_FAIL;
		}
		
		while (1)
		{
			Sleep(50);
			nTimeOut = 0;
			ZeroMemory(byTempBuf, MAX_BUF_DATA);
			if(m_opTesterCom.ReadBuffer(byTempBuf, MAX_BUF_DATA,&dwReadLength))
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
    if(!m_opTesterCom.WriteBuffer(chData,strCommand.GetLength()))
    {
		MessageBox("串口打开错误！");
		return ;
	}
	Sleep(50);
	if (!m_opTesterCom.ReadBuffer(chReData,20))
	{
		MessageBox("读取错误，接收错误！");
		return ;
	}

}

/*
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
  //  FindMyDirectory("data",m_strPN,m_strSN);

	double dblRoomTemp = GetModuleTemp();

		
	strTemp = "Room";
	LogInfo("开始常温测试，等待模块温度。。");

	strLutFile.Format("%s\\data\\%s\\%s\\LUT\\TempData_%s.csv",
		m_strLocalPath,m_strItemName,m_strSN,strTemp);
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
	int nChannel = 0;
    for ( nChannel=0;nChannel<=m_nCHCount;nChannel++)
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
			strCSVFile.Format("%s\\data\\%s\\%s\\VOL\\%s_ScanVoltage_%s_%d.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,strTemp,nChannel);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\VOL\\%s_ScanVoltage_%s_%d-COM1.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,strTemp,nChannel);
		}
		
		if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
		{
			LogInfo("创建数据文件失败！",FALSE,COLOR_RED);
			return FALSE;
		}
		strContent.Format("%d,%d,%d,%.2f\n",nChannel,ScanPowerChannal[nChannel].VoltageX,
			ScanPowerChannal[nChannel].VoltageY,ScanPowerChannal[nChannel].MaxPower);
		stdCSVFile.WriteString(strContent);		
		stdCSVFile.Close();	
	}
	SaveDataToExcel(VOL_DATA);
	SaveDataToExcel(IL_ROOM_DATA);
	LogInfo("保存常温扫描数据成功！"); 	
	LogInfo("定标完毕！");
	return TRUE;

}
*/
BOOL CMy126S_45V_Switch_AppDlg::CreateLutFile()
{
	CString strFileLut;
	CStdioFile stdFileLut;
	CString  strVendorName;
	CString  strPN;
	CString  strHWVersion;
	CString  strFWVersion;
	CString  strSN;
	CString  strTime;
	CString  strValue;
	CString  strMsg;
	SYSTEMTIME  st;
	CString  strINIFile;
	CString  strSave;
	CString strServePath;
	int iLen = 0;

	GetLocalTime(&st);
	strFileLut.Format("%s\\data\\%s\\%s\\Cal\\BIN\\ProductInfo.csv",
		m_strLocalPath, m_strPN, m_strSN);
	if (!stdFileLut.Open(strFileLut, CFile::modeCreate | CFile::modeWrite))
	{
		LogInfo("创建产品信息LUT失败！");
		return FALSE;
	}
	strSN = m_strSN + "\n";
	stdFileLut.WriteString(strSN);

	//	strPN = m_strPN+"\n";
	strPN = "OMSSMCSAMPZAB02\n";
	strHWVersion = "BBB\n";
	
	stdFileLut.WriteString(strPN);
	stdFileLut.WriteString(strHWVersion);
	strTime.Format("%04d%02d%02d\n", st.wYear, st.wMonth, st.wDay);
	stdFileLut.WriteString(strTime);
	stdFileLut.Close();

	//保存到网络路径
	strServePath.Format("%s\\data\\%s\\%s\\Cal\\BIN\\ProductInfo.csv",
		m_strNetFile, m_strPN, m_strSN);
	int nCount = 0;
	if (m_bIfSaveToServe)
	{
		nCount = 0;
		while (1)
		{
			nCount++;
			if (CopyFile(strFileLut, strServePath, FALSE))
			{
				break;
			}
			else
			{
				Sleep(1000);
				strMsg.Format("重传第%d次：%s", nCount, strServePath);
				LogInfo(strMsg);
			}
			if (nCount>600)
			{
				strMsg.Format("超过10分钟未上传成功！");
				LogInfo(strMsg);
				return FALSE;
			}
		}
	}
	LogInfo("生成产品信息LUT表完成！");
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::CreateBinFileAndDownload()
{
    CString strSave;
	CString strINIFile;
	CString strBinFile;
	CString strValue;
	int     nFile=1;
	CString strFile;
	
	UpdateData();

    strINIFile.Format("%s\\config\\DataSave.ini",g_tszAppFolder);
	int iLen = GetPrivateProfileString("Save","Save",NULL,strSave.GetBuffer(128),128,strINIFile);
	if (iLen <= 0)
    {
		LogInfo("读取配置文件DataSave.ini失败",FALSE,COLOR_RED);
		return FALSE;
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
	int i = 0;
	for ( i=0;i<m_nCHCount;i++)
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

		for( i = 0; i < m_nCHCount; i++)
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
		for (i=0;i<m_nCHCount;i++)
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
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_CT.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN);
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_CT-COM1.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN);
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
	for (i=0;i<m_nCHCount;i++)
	{
		strTemp1.Format(",%d",i+1);
		strTemp = strTemp + strTemp1;
	}
	strTemp = strTemp + "\n";
	stdCSVFile.WriteString(strTemp);
	int j = 0;
	for (i=0;i<m_nCHCount;i++)
	{
		strTemp.Format("CH%d/CT",i+1);
		for ( j=0;j<m_nCHCount;j++)
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
	Sleep(500);
	SaveDataToExcel(IL_ROOM_DATA);
	LogInfo("IL/CT测试完毕!");
	return TRUE;

}

/*
BOOL CMy126S_45V_Switch_AppDlg::WDLTest()
{
	double dblTLSPower=0.0;
	double dblStartWL=0;
	double dblENDWL=0;
	double dblPower=0.0;
	double step=0;
	int count;
    double dblWDLPower[100];
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

	strIniFile.Format("%s\\config\\%s\\WDLSET.INI",m_strConfigPath,m_strPN);
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
	        	m_HP816X.SetTLSWavelength(m_pdblWavelength);
				return FALSE;
			}
			m_HP816X.SetTLSWavelength(dbl);
			Sleep(50);
			//设置光功率计波长
			if(!SetPWMWL(dbl))
			{
				LogInfo("设置光功率计波长失败！",(BOOL)FALSE,COLOR_RED);	
				return FALSE;
			}
			Sleep(300);
					
			dblPower = ReadPWMPower(nChannel);
			m_dblWDL[nChannel][count] = m_dblWDLRef[nChannel][count]-dblPower;
			strMsg.Format("通道%d:　波长:%.2f nm , IL:%.2f dB",nChannel,dbl,m_dblWDL[nChannel][count]);
			LogInfo(strMsg);
		    count++;							
		} 
		//设置回1550nm波长
        if(!SetPWMWL(m_pdblWavelength))
		{
			LogInfo("设置波长失败！",(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		m_HP816X.SetTLSWavelength(m_pdblWavelength);
		//计算WDL
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL_%d.csv",
				m_strLocalPath,m_strItemName,m_strSN,m_strSN,nChannel+1);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s\\%s_WDL_%d-COM1.csv",
				m_strLocalPath,m_strItemName,m_strSN,m_strSN,nChannel+1);
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
		int i = 1;
		for( i=1;i<(count-1);i++)
		{
			if (dblWDLPower[i] <= dblMIN)
			{
				dblMAX = m_dblWDL[nChannel][i];
			}
			if (dblWDLPower[i] >= dblMAX)
			{
				dblMIN = m_dblWDL[nChannel][i];
			}
		}
		ChannalMessage[nChannel].dblWDL = dblMAX - dblMIN;
		strMsg.Format("通道%d的WDL为:%.2f dB",nChannel+1,ChannalMessage[nChannel].dblWDL);
		if (!CheckParaPassOrFail(WDL_DATA,ChannalMessage[nChannel].dblWDL))
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
*/
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

	if (strSave=="1")
	{
		strPath = m_strNetFile;
	}
	else if (strSave=="0")
	{
		strPath = m_strLocalPath;
	}
	else
	{
		LogInfo("读取DataSave.ini配置文件错误！");
		nFile = 0;
		return nFile;
	}

	strLUTFile.Format("%s\\Data\\%s\\LUT\\ProductInfo.csv",
		strPath,m_strSN);
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
	int i = 0;
	for( i=0;i<m_nCHCount;i++)
	{
		if (m_cbComPort.GetCurSel()==0)
		{
			strLUTFile.Format("%s\\data\\%s\\%s\\VOL\\%s_ScanVoltage_%s_%d.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,strTemp,i+1);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strLUTFile.Format("%s\\data\\%s\\%s\\VOL\\%s_ScanVoltage_%s_%d-COM1.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,strTemp,i+1);
		}
		
		if (GetFileAttributes(strLUTFile) == -1)
		{
			nFile = 0;
			bFlagRoom = FALSE;
			strMsg.Format("数据文件%s不存在！",strLUTFile);
			LogInfo(strMsg);
			return nFile;
		}
	}		

	if (bFlagRoom)
	{
		nFile = 1;
	}
	return nFile;

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

    ZeroMemory(pByteBinData,sizeof(BYTE)*100000);
	ZeroMemory(chValue,sizeof(char)*MAX_LINE);

	if (strSave == "1")
	{
		strPath = m_strNetFile;
	}
	else if (strSave == "0")
	{
		strPath = m_strLocalPath;
	}

	//bBundleTag
	count = 0;
    CString strBundle = "OPLINK";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strBundle,strBundle.GetLength());
    for (i=0;i<strBundle.GetLength();i++)
	{
		pByteBinData[count++] = (BYTE)(chValue[i]);
    }	
	//bProductType
	count = 8;
	CString strProductType = "SWITCH";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strProductType,strProductType.GetLength());
    for (i=0;i<strProductType.GetLength();i++)
	{
		pByteBinData[count++] = (BYTE)(chValue[i]);
    }
	//dwBundleHdrCRC32(Reserved)
	DWORD dwHdrCRC32 = 0;
	count = 16;
	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>24);
	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>16);
	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>8);
	pByteBinData[count++] = (BYTE)(dwHdrCRC32);
	//dwBundleSize
	DWORD dwBundleSize = 0x02A0;
	pByteBinData[count++] = (BYTE)(dwBundleSize>>24);
	pByteBinData[count++] = (BYTE)(dwBundleSize>>16);
	pByteBinData[count++] = (BYTE)(dwBundleSize>>8);
	pByteBinData[count++] = (BYTE)(dwBundleSize);
	//wBundleHdrSize
	WORD	wBundleHdrSize = 128;
	pByteBinData[count++] = (BYTE)(wBundleHdrSize>>8);
	pByteBinData[count++] = (BYTE)(wBundleHdrSize);
	//wImageCount
	WORD wImageCount = 1;
	pByteBinData[count++] = (BYTE)(wImageCount>>8);
	pByteBinData[count++] = (BYTE)(wImageCount);
	//
	//bBundleVersion
	count = 32;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (i=0;i<m_strSN.GetLength();i++)
	{
		pByteBinData[count++] = (BYTE)(chValue[i]);
    }
	//bPartNumber
	count = 48;
    strPN = "UOFS104U00IFA01G";
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,strPN,strPN.GetLength());
    for (i=0;i<strPN.GetLength();i++)
	{
		pByteBinData[count++] = (BYTE)(chValue[i]);
    }
	// Serial Number of this product,
    count = 80;
	ZeroMemory(chValue,sizeof(chValue));
	memcpy(chValue,m_strSN,m_strSN.GetLength());
    for (i=0;i<m_strSN.GetLength();i++)
	{
		pByteBinData[count++] = (BYTE)(chValue[i]);
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
		pByteBinData[count++] = (BYTE)(chValue[i]);
    }

   //lut表的文件头
	//TAG
	pByteBinData[count++] = 0x49;
	pByteBinData[count++] = 0x42;
	pByteBinData[count++] = 0x46;
	pByteBinData[count++] = 0x48;
	//ImageType
    pByteBinData[count++] = 0x81;
	//ImageHitless
	pByteBinData[count++] = 0x00;
	//StorageID
	pByteBinData[count++] = 0x00;
	//ImageIndex
	pByteBinData[count++] = 0x00;
	//dwImageVersion
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	//dwTimeStamp
	pByteBinData[count++] = (BYTE)(st.wMonth);
	pByteBinData[count++] = (BYTE)(st.wDay);
	pByteBinData[count++] = (BYTE)(st.wHour);
	pByteBinData[count++] = (BYTE)(st.wMinute);
    
	//dwBaseAddress
	DWORD dwBaseAddress=0x6E00;
	pByteBinData[count++] = (BYTE)(dwBaseAddress>>24);
	pByteBinData[count++] = (BYTE)(dwBaseAddress>>16);
	pByteBinData[count++] = (BYTE)(dwBaseAddress>>8);
	pByteBinData[count++] = (BYTE)(dwBaseAddress);
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
	count = 160;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	
	strLUTFile.Format("%s\\data\\%s\\%s\\ProductInfo.csv",
		strPath,m_strItemName,m_strSN);
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
    int AlarmMaxTemp = 700;
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
	pByteBinData[count++] = (BYTE)(dwTempShift);

	//获取需下载的通道数和通道名
	
	//5.通道定标数据
	//写入开关通道数据
	int  dwPointX[32];
	int  dwPointY[32];
	BOOL bFlag = TRUE;
	ZeroMemory(dwPointX,sizeof(dwPointX));
	ZeroMemory(dwPointY,sizeof(dwPointY));
	
	for (nTemp=0;nTemp<4;nTemp++)
	{
		if (nFile==1)
		{
			strTemperature="Room";
		}
		else if (nFile==3)
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
			else if (nTemp==3)
			{
				strTemperature="Room";
			}
		}
		else if (nFile==4)
		{
			if (nTemp==0)
			{
				strTemperature="Room";
			}
			else if (nTemp==1)
			{
				strTemperature="Room";
			}
			else if (nTemp==2)
			{
				strTemperature="High";
			}
			else if(nTemp==3)
			{
				strTemperature="High";
			}
		}
		else if (nFile==6)
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
			else if (nTemp==3)
			{
				strTemperature="High";
			}
		}
		else if (nFile==10)
		{
			if (nTemp==0)
			{
				strTemperature="Low";
			}
			else if (nTemp==1)
			{
				strTemperature="Mid";
			}
			else if (nTemp==2)
			{
				strTemperature="Room";
			}
			else if (nTemp==3)
			{
				strTemperature="High";
			}
		}

		for (ch=0;ch<m_nCHCount;ch++)
		{
			strLutFile.Format("%s\\Data\\%s\\%s\\%s_ScanVoltage_%d.csv",
				strPath,m_strItemName,m_strSN,m_strSN,ch+1);
		
			if(!stdPDFile.Open(strLutFile,CFile::modeRead|CFile::typeText))
			{
				strMsg.Format("打开文件%s失败！",strLutFile);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				//return FALSE;
				dwPointX[ch] = 0;
				dwPointY[ch] = 0;
			}
			else
			{
				ZeroMemory(chBuffer, sizeof(char)*MAX_LINE);
				lpStr1 = stdPDFile.ReadString(chBuffer, MAX_LINE);
				if (lpStr1 == NULL)
				{
					LogInfo("读取LUT表错误，数据缺失!", FALSE, COLOR_RED);
					stdPDFile.Close();
					return FALSE;
				}
				lpStr1 = stdPDFile.ReadString(chBuffer, MAX_LINE);
				strToken1 = strtok(chBuffer, sep);
				strToken1 = strtok(NULL, sep);
				dwPointX[ch] = atoi(strToken1)*MAX_DAC / MAX_VOLTAGE;
				dwPointX[ch] = dwPointX[ch];
				strToken1 = strtok(NULL, sep);
				dwPointY[ch] = atoi(strToken1)*MAX_DAC / MAX_VOLTAGE;
				dwPointY[ch] = dwPointY[ch];
				stdPDFile.Close();
			}
			
		}
		if (m_nSwitchComPortCount==2)
		{
			for (ch=0;ch<m_nCHCount;ch++)
			{
				if(ch>m_nCHCount)
				{
					dwPointX[ch+m_nCHCount]=0;
					dwPointY[ch+m_nCHCount]=0;
				}
				else
				{
					strLutFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
						strPath,m_strItemName,m_strSN,m_strSN,ch+1);
				
					bFlag = stdPDFile.Open(strLutFile,CFile::modeRead|CFile::typeText);
					if(!bFlag)
					{
						strMsg.Format("打开文件%s失败！",strLutFile);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						//return FALSE;
						dwPointX[ch+m_nCHCount] = 1;
						dwPointX[ch+m_nCHCount] = dwPointX[ch+m_nCHCount];
						dwPointY[ch+m_nCHCount] = 1;
			    		dwPointY[ch+m_nCHCount] = dwPointY[ch+m_nCHCount];
					}
					else
					{
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
						dwPointX[ch+m_nCHCount] = atoi(strToken1)*MAX_DAC/MAX_VOLTAGE;
						dwPointX[ch+m_nCHCount] = dwPointX[ch+m_nCHCount];
						strToken1 = strtok(NULL,sep);
						dwPointY[ch+m_nCHCount] = atoi(strToken1)*MAX_DAC/MAX_VOLTAGE;
						dwPointY[ch+m_nCHCount] = dwPointY[ch+m_nCHCount];	
						stdPDFile.Close();
					}	
				}
			}
		}
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
	for (nTemp=0;nTemp<4;nTemp++)
	{
		if (nFile==1)
		{
			strTemperature="Room";
		}
		else if (nFile==3)
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
			else if (nTemp==3)
			{
				strTemperature="Room";
			}
		}
		else if (nFile==4)
		{
			if (nTemp==0)
			{
				strTemperature="Room";
			}
			else if (nTemp==1)
			{
				strTemperature="Room";
			}
			else if (nTemp==2)
			{
				strTemperature="High";
			}
			else if (nTemp==3)
			{
				strTemperature="High";
			}
		}
		else if (nFile==6)
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
			else if (nTemp==3)
			{
				strTemperature="High";
			}
		}
		else if (nFile==10)
		{
			if (nTemp==0)
			{
				strTemperature="Low";
			}
			else if (nTemp==1)
			{
				strTemperature="Mid";
			}
			else if (nTemp==2)
			{
				strTemperature="Room";
			}
			else if (nTemp==3)
			{
				strTemperature="High";
			}
		}

		dblTemperature = 25;
		dwTemperature = (DWORD)(dblTemperature*10);
		pByteBinData[count++] = (BYTE)(dwTemperature>>8);
		pByteBinData[count++] = (BYTE)(dwTemperature);
//		stdPDFile.Close();
	}

	//开关切换时间10ms = 200us*500
	WORD wSwitchTime = 200;
	pByteBinData[count++] = (BYTE)(wSwitchTime>>8);
	pByteBinData[count++] = (BYTE)(wSwitchTime);

	//10.Reserved
    for (i=0;i<534;i++)
	{
		pByteBinData[count++] = 0xFF;
    }

	m_CRC32.InitCRC32();
 	for (i = 160;i < count; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pByteBinData[i]);
 	}
 	dwImageCRC32 = ~dwImageCRC32;
	pByteBinData[count++] = (BYTE)(dwImageCRC32>>24);
	pByteBinData[count++] = (BYTE)(dwImageCRC32>>16);
	pByteBinData[count++] = (BYTE)(dwImageCRC32>>8);
	pByteBinData[count++] = (BYTE)(dwImageCRC32);

	//得出文件长度
	dwBinsize = count;
	//dwImageCRC32
//    dwImageCRC32 = 0;
	m_CRC32.InitCRC32();
 	for (i = 160;i < dwBinsize; i++)
 	{
 		dwImageCRC32 = m_CRC32.GetThisCRC(pByteBinData[i]);
 	}
 	dwImageCRC32 = ~dwImageCRC32;
	count = 148;
	pByteBinData[count++] = (BYTE)(dwImageCRC32>>24);
	pByteBinData[count++] = (BYTE)(dwImageCRC32>>16);
	pByteBinData[count++] = (BYTE)(dwImageCRC32>>8);
	pByteBinData[count++] = (BYTE)(dwImageCRC32);

	//dwImageSize
	count = 152;
    DWORD dwImageSize = dwBinsize-160; //总长度减去文件头长度
	pByteBinData[count++] = (BYTE)(dwImageSize>>24);
	pByteBinData[count++] = (BYTE)(dwImageSize>>16);
	pByteBinData[count++] = (BYTE)(dwImageSize>>8);
	pByteBinData[count++] = (BYTE)(dwImageSize);
	//SectionCount
	pByteBinData[count++] = 0x01;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;
	pByteBinData[count++] = 0x00;

	//计算header的CRC
	m_CRC32.InitCRC32();
 	for (i = 20;i < 128; i++)
 	{
 		dwHdrCRC32 = m_CRC32.GetThisCRC(pByteBinData[i]);
 	}
 	dwHdrCRC32 = ~dwHdrCRC32;
	count = 16;
	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>24);
	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>16);
	pByteBinData[count++] = (BYTE)(dwHdrCRC32>>8);
	pByteBinData[count++] = (BYTE)(dwHdrCRC32);

	strMsg.Format("%s_DataBin.bin",m_strSN);
	if(!WriteBinFile(strMsg,strSave,dwBinsize,pByteBinData))
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
	
	strBinFileName.Format("%s\\data\\%s\\%s\\%s",
		m_strLocalPath,m_strItemName,m_strSN,strBinName);
	strNetFile.Format("%s\\data\\%s\\%s\\%s",
		m_strNetFile,m_strItemName,m_strSN,strBinName);
	
	hFindFile = FindFirstFile(strBinFileName,&win32FindData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		strMsg.Format("发现文件%s已经存在,直接覆盖数据",strBinFileName);
		//	iSel = MessageBox(strMsg,"确认",MB_YESNO|MB_ICONQUESTION);
		LogInfo(strMsg,(BOOL)FALSE,COLOR_BLUE);
		iSel = IDYES;
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
	
	if (strSave == "1")
	{
		hDirectory = FindFirstFile(strNetFile,&winFindFileData);
		if (hDirectory != INVALID_HANDLE_VALUE)
		{	
			DeleteFile(strNetFile);	
		}
		if (!CopyFile(strBinFileName,strNetFile,TRUE)) 
		{
			LogInfo("保存数据至网络失败!",FALSE,COLOR_RED);
			return FALSE;
		}
		strMsg.Format("开始下载文件%s",strNetFile);
		LogInfo(strMsg);
		if(!DataDownLoad(strNetFile))
		{
			return FALSE;
		}
	}
	else if (strSave == "0")
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

BOOL CMy126S_45V_Switch_AppDlg::UpdatePMData()
{
	//double dblWL=0;
	CString strValue;
	double dblPower;
	UpdateData();
	int nPMCH1 = m_cbChannel.GetCurSel();
	if (nPMCH1<=0)
	{
		KillTimer(3);
		MessageBox("请选择正确的通道！");
		return FALSE;
	}
	dblPower = m_dblReferencePower[nPMCH1-1]-ReadPWMPower(nPMCH1-1);
	strValue.Format("%.2f dB",dblPower);
	SetDlgItemText(IDC_EDIT_PM_IL1,strValue);
	//UpdateWindow();
	return TRUE;
}

double CMy126S_45V_Switch_AppDlg::ReadWL(int nCH)
{
	
	char pchCmd[256];
	char recvBuf[256];
	WORD wSendLen;
	int  count = 0;
	CString strMsg;
	int iSocket;
	int nPM;
	BYTE bSlot;
	BYTE bChannel;
	double dblWL=0;
	/*
	nPM = nCH/4;
	bSlot = nCH/2;
	bChannel = nCH%2;

	iSocket = g_socket_fd[nPM];
	
	//设置超时等待3秒
	int TimeOut = 3000;
	if (::setsockopt(iSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&TimeOut,sizeof(TimeOut)) == SOCKET_ERROR)
	{
		LogInfo("发送超时！");
		return 0;
	}
	if (::setsockopt(iSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&TimeOut,sizeof(TimeOut)) == SOCKET_ERROR)
	{
		LogInfo("接收超时！");
		return 0;
	}
		
	ZeroMemory(pchCmd, 256);
	sprintf(pchCmd, "SENS%d,CHAN%d:POW:WAV?", bSlot, bChannel);
	
	wSendLen = strlen(pchCmd);
	send(iSocket,pchCmd,wSendLen,0);
	
	ZeroMemory(recvBuf, 256);
	recv(iSocket,recvBuf,256,0);
	//	LogInfo(recvBuf);
	
	dblWL = atof(recvBuf);
	
	
	*/
	return dblWL;

}

BOOL CMy126S_45V_Switch_AppDlg::SCANRANGE()
{
	CString strMsg;

	//设置路径
	UpdateData();
	
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
	int i = 0;
	
	for ( i=0;i<m_nCHCount;i++)
	{
		m_nReadPwrCH[i] = i;
	}
	if (m_bScan)
	{
	     iSel = MessageBox("粗扫已存在,是否重新粗扫!","提示",MB_YESNO);
	}
	if (iSel==IDYES)
	{
		for( i=0;i<m_nCHCount;i++)
		{
			/*
			if (m_cbChannel.GetCurSel()!=0)
			{
				if (m_cbChannel.GetCurSel()!=(i+1)) 
				{
					continue;
				}
			}
			*/
			if (m_nCHCount==16&&m_nSwitchComPortCount==2)
			{
				YieldToPeers();
				if(m_bTestStop)
				{
					m_bTestStop=FALSE;
					MessageBox("人为终止！");
					return FALSE;
				}
				if(!AutoScan4x3(i,strFileINI,TRUE))
				{			
					m_bAdjustScan = FALSE;
					LogInfo("该通道扫描出错！",FALSE,COLOR_RED);
					continue;
				//	return FALSE;
				}
			}
			else if (m_nCHCount==19)
			{
				if(!AutoScan4x3(i,strFileINI,TRUE))
				{
					strMsg.Format("粗扫通道%d失败！",i+1);
					LogInfo(strMsg);
					continue;
				}
			}
			else
			{
				if(!AutoScan4x3(i,strFileINI,TRUE))
				{
					strMsg.Format("粗扫通道%d失败！",i+1);
					LogInfo(strMsg);
					continue;
				}				
			}			
		}
		m_bScan = TRUE;
	}
	//保存调节数据
	SaveDataToCSV(VOL_DATA);
	LogInfo("保存电压数据成功!");	
	return TRUE;
}


void CMy126S_45V_Switch_AppDlg::OnButtonRepeatTest2() 
{
	// TODO: Add your control notification handler code here
	CString strChannel;
	//开始随机切换
	if(!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}
	if (!m_bTestIL)
	{
		MessageBox("请先测试IL！");
		return;
	}
	UpdateData();

	LogInfo("开始随机切换开关3200次，间隔10ms...",TRUE);
	LogInfo("请等待40s......！");
		
	//随即平均切换100次
	if (!SendMsgToSwitch(10,3200))
	{
		LogInfo("随机切换错误！");
		return;
	}
 	m_iTimerCount = 0;
	SetTimer(2,1000,NULL);
}


HBRUSH CMy126S_45V_Switch_AppDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if(pWnd->GetDlgCtrlID() == IDC_EDIT_PM_IL1)
	{
	//	COLORREF clr = COLOR_YELLOW;
	//	pDC->SetBkColor(clr);
		//m_brMine
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CMy126S_45V_Switch_AppDlg::SetWLFromOPCom(int iPMIndex, byte bSlot, byte bChan, long lWL)
{
	char pchCmd[256];
	char recvBuf[256];
	WORD wSendLen;
	long lTempWL;
	
	UpdateData();
	ZeroMemory(pchCmd,256);
	sprintf(pchCmd,"SENS%d,CHAN%d:POW:WAV %dnm\r\n",bSlot,bChan,lWL);
	wSendLen = strlen(pchCmd);
	
	if (!m_comPM[iPMIndex].WriteBuffer(pchCmd,wSendLen))
	{
		return FALSE;
	}
	Sleep(500);
	
	ZeroMemory(pchCmd,256);
	sprintf(pchCmd,"SENS%d,CHAN%d,POW:WAV?\r\n",bSlot,bChan);
	
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
	Sleep(200);
	
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
	sprintf(pchCmd, "SENS%d,CHAN%d:POW:READ?\r\n", bSlot, bChannel);
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

void CMy126S_45V_Switch_AppDlg::OnButtonDarkTest() 
{
	// TODO: Add your control notification handler code here
	double  dblPower;
	CString strMsg;
	BOOL    bFlag;

	if(!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}

	if(!SETX(0))
	{
		MessageBox("设置X电压为0失败！");
		return;
	}
	Sleep(50);
	if(!SETY(0))
	{
		MessageBox("设置Y电压为0失败！");
		return;
	}
	Sleep(500);
	LogInfo("设置电压为0，开始dark值测试！");
	double dblDark[32];
	if(!GetCT(dblDark,m_nCHCount))
	{
		return;
	}
	int nCH = 0;
	for ( nCH = 0;nCH<m_nCHCount;nCH++)
	{
		dblPower = dblDark[nCH];
		ChannalMessage[nCH].dblDarkIL = m_dblReferencePower[nCH] -  dblPower;
		if (!CheckParaPassOrFail(DARK_DATA,ChannalMessage[nCH].dblDarkIL,nCH))
		{
			bFlag = FALSE;
			strMsg.Format("通道%d的DARK值为：%.2f dB,参数不合格",nCH+1,ChannalMessage[nCH].dblDarkIL);
	    	LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		}
		else
		{
			bFlag = TRUE;
			strMsg.Format("通道%d的DARK值为：%.2f dB",nCH+1,ChannalMessage[nCH].dblDarkIL);
	    	LogInfo(strMsg);
		}
	}
	SaveDataToCSV(DARK_DATA);
	SetVoltage(0);

}

void CMy126S_45V_Switch_AppDlg::OnButtonSetVoltage() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CString strVolt;
	int     nVoltX;
	int     nVoltY;
	CString strMsg;
	m_nSNIndex = m_cbxSNList.GetCurSel();
	int nBlockindexTemp = 0;
	m_bTesterALL = TRUE;
	if (!m_bTesterPort)
	{
		MessageBox("请先打开对应的串口！");
		return;
	}
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	int State = pBtn->GetCheck();
	if (State ==1)
	{
		GetDlgItemText(IDC_EDIT_X, strVolt);
		nVoltX = atoi(strVolt);
		GetDlgItemText(IDC_EDIT_Y, strVolt);
		nVoltY = atoi(strVolt);
	}
	else
	{
		GetDlgItemText(IDC_EDIT_X, strVolt);
		nVoltX = atoi(strVolt);
		nVoltX = nVoltX*MAX_DAC / MAX_VOLTAGE;
		GetDlgItemText(IDC_EDIT_Y, strVolt);
		nVoltY = atoi(strVolt);
		nVoltY = nVoltY*MAX_DAC / MAX_VOLTAGE;		// 勾选
	}


    //strMsg.Format("")
	//发送一个回车结束掉监控状态！
	if (m_nSNIndex >= 32)
	{
		if (!m_CmdModule.SetSwitchDAC(3, m_nSNIndex - 32 + 1, nVoltX, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}
		if (!m_CmdModule.SetSwitchDAC(3, m_nSNIndex - 32 + 1, nVoltY, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}

	}
	else if (m_nSNIndex >= 16)
	{
		if (!m_CmdModule.SetSwitchDAC(2, m_nSNIndex - 16 + 1, nVoltX, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}
		if (!m_CmdModule.SetSwitchDAC(2, m_nSNIndex - 16 + 1, nVoltY, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}

	}
	else
	{
		if (!m_CmdModule.SetSwitchDAC(1, m_nSNIndex + 1, nVoltX, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}
		if (!m_CmdModule.SetSwitchDAC(1, m_nSNIndex + 1, nVoltY, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}

	}
	strMsg.Format("X=%d Y=%d", nVoltX, nVoltY);
	LogInfo(strMsg);
	LogInfo("OK");
	OnButtonTest();
}


BOOL CMy126S_45V_Switch_AppDlg::GetATMTestTemplate()
{
	/*
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

	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&adjust=1",m_strSN); //获取调节测试模板

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
		if (strTemp=="room") //常温
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

			//TDL
			//RL
			ZeroMemory(dwBuf, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "RL", dwBuf, nNodeCount,dwWDMTemp);
			m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
			m_stLimitPara.dblMaxRL = atof(strTemp);
			m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
	    	m_stLimitPara.dblMinRL = atof(strTemp);		
		}
		else if (strTemp=="low") //低温
		{
		}
		else if (strTemp=="high")//高温
		{
		}	
	}
	*/
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

BOOL CMy126S_45V_Switch_AppDlg::AutoScan4X4(int nChannel, CString strFileINI, BOOL bScanRange)
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
	strINIFile.Format("%s\\config\\%s\\%s",m_strConfigPath,m_strPN,strFileINI);
	strKey.Format("CH%d",m_nReadPwrCH[nChannel]+1);

	DWORD nLen1 = GetPrivateProfileString(strKey,"x_start",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen1 <= 0)
		MessageBox("读取配置x_start信息失败！");
	x_start = atoi(strValue);

	DWORD nLen2 = GetPrivateProfileString(strKey,"x_end",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen2 <= 0)
		MessageBox("读取配置x_end信息失败！");
	x_stop = atoi(strValue);

	DWORD nLen3 = GetPrivateProfileString(strKey,"y_start",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen3 <= 0)
		MessageBox("读取配置y_start信息失败！");
	y_start = atoi(strValue);
	
	DWORD nLen4 = GetPrivateProfileString(strKey,"y_end",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen4 <= 0)
		MessageBox("读取配置y_end信息失败！");
	y_stop = atoi(strValue);

	DWORD nLen5 = GetPrivateProfileString("step","x_step",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen5 <= 0)
		MessageBox("读取配置x_step信息失败！");
	x_step = atoi(strValue);
	
	DWORD nLen6 = GetPrivateProfileString("step","y_step",
		NULL,strValue.GetBuffer(128),128,strINIFile);
	if (nLen6 <= 0)
		MessageBox("读取配置y_step信息失败！");
	y_step = atoi(strValue);

	//获取中心点信息：
	int nXSpace1;
	int nXSpace2;
	int nXSpace3;
	int nYSpace1;
	int nYSpace2;
	int nYSpace3;
	CString strCHFile;
	if (m_cbComPort.GetCurSel()==0)
	{
		strCHFile.Format("%s\\config\\%s\\ChannelCenter.ini",m_strConfigPath,m_strPN);
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strCHFile.Format("%s\\config\\%s\\ChannelCenter-COM1.ini",m_strConfigPath,m_strPN);
	}

	GetPrivateProfileString("XSpace1","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nXSpace1 = atoi(strValue);
	
	GetPrivateProfileString("XSpace2","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nXSpace2 = atoi(strValue);
	
	GetPrivateProfileString("XSpace3","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nXSpace3 = atoi(strValue);
	
	GetPrivateProfileString("YSpace1","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nYSpace1 = atoi(strValue);
	
	GetPrivateProfileString("YSpace2","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nYSpace2 = atoi(strValue);

	GetPrivateProfileString("YSpace3","Value","error",strValue.GetBuffer(128),128,strCHFile);
    if (strValue=="error")
    {
		LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
		return FALSE;
    }
	nYSpace3 = atoi(strValue);

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

	switch (m_nReadPwrCH[nChannel])
	{
	case 0:
		GetDlgItemText(IDC_EDIT_X,strVolt);
		x = atoi(strVolt);
		
		GetDlgItemText(IDC_EDIT_Y,strVolt);
		y = atoi(strVolt);
		break;
	case 1:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[0] + nXSpace1;
	    	y = m_Ybase[0];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[0] + nXSpace1;
		    y = m_Ybase[0];
		}
		break;
	case 2:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[1] + nXSpace2;
			y = m_Ybase[1];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[1] + nXSpace2;
			y = m_Ybase[1];
		}
		break;
	case 3:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[2] + nXSpace3;
			y = m_Ybase[2];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[2] + nXSpace3;
			y = m_Ybase[2];
		}
		if (!bScanRange)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				x = m_Xbase[0] + nXSpace1+nXSpace2+nXSpace3;
		    	y = m_Ybase[0];
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				x = m_Xbase[0] + nXSpace1+nXSpace2+nXSpace3;
		    	y = m_Ybase[0];
			}
		}
		break;
	case 4:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[3];
		    y = m_Ybase[3]-nYSpace1;
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[3];
		    y = m_Ybase[3]-nYSpace1;
		}
		break;
	case 5:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[4]-nXSpace3;
			y = m_Ybase[4];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[4]-nXSpace3;
			y = m_Ybase[4];
		}
		break;
	case 6:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[5]-nXSpace2;
			y = m_Ybase[5];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[5]-nXSpace2;
			y = m_Ybase[5];
		}
		break;
	case 7:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[6]-nXSpace1;
			y = m_Ybase[6];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[6]-nXSpace1;
			y = m_Ybase[6];
		}
		break;
	case 8:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[7];
			y = m_Ybase[7]-nYSpace2;
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[7];
			y = m_Ybase[7]-nYSpace2;
		}
		break;
	case 9:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[8]+nXSpace1;
			y = m_Ybase[8];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[8]+nXSpace1;
			y = m_Ybase[8];
		}
		break;
	case 10:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[9]+nXSpace2;
			y = m_Ybase[9];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[9]+nXSpace2;
			y = m_Ybase[9];
		}
		break;
	case 11:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[10]+nXSpace3;
			y = m_Ybase[10];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[10]+nXSpace3;
			y = m_Ybase[10];
		}
		break;
	case 12:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[11];
			y = m_Ybase[11]-nYSpace3;
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[11];
			y = m_Ybase[11]-nYSpace3;
		}
		if (!bScanRange)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				x = m_Xbase[3];
				y = m_Ybase[3]-nYSpace1-nYSpace2-nYSpace3;
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				x = m_Xbase[3];
				y = m_Ybase[3]-nYSpace1-nYSpace2-nYSpace3;
			}
		}
		break;
	case 13:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[12]-nXSpace3;
			y = m_Ybase[12];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[12]-nXSpace3;
			y = m_Ybase[12];
		}
		break;
	case 14:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[12]-nXSpace2;
			y = m_Ybase[12];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[12]-nXSpace2;
			y = m_Ybase[12];
		}
		break;
	case 15:
		if (m_cbComPort.GetCurSel()==0)
		{
			x = m_Xbase[14]-nXSpace1;
			y = m_Ybase[14];
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			x = m_Xbase[14]-nXSpace1;
			y = m_Ybase[14];
		}
		if (!bScanRange)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				x = m_Xbase[0];
				y = m_Ybase[12];
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				x = m_Xbase[0];
				y = m_Ybase[12];
			}
		}
		break;
	}
	//开始扫描通道
	int nDACX = x*MAX_DAC/MAX_VOLTAGE;
	int nDACY = y*MAX_DAC/MAX_VOLTAGE;
	CString strSendBuf;
	char chSendBuf[50];
	char chReadBuf[256];
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	strSendBuf.Format("SCAN CHANNEL 1 %d %d %d\r\n",m_nReadPwrCH[nChannel]+1,nDACX,nDACY);
	memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
	if(!m_opTesterCom.WriteBuffer(chSendBuf,strSendBuf.GetLength()))
	{
		strMsg.Format("发送指令%s失败!",strSendBuf);
		LogInfo(strMsg);
		return FALSE;
	}
	Sleep(50);
	if (!m_opTesterCom.ReadBuffer(chReadBuf,256))
	{
		strMsg.Format("接收指令%s失败!",strSendBuf);
		LogInfo(strMsg);
		return FALSE;
	}
	strToken = strtok(chReadBuf,"\r:");
	strToken = strtok(NULL,"\r:");
	ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX = atoi(strToken)*MAX_VOLTAGE/MAX_DAC;
	m_Xbase[m_nReadPwrCH[nChannel]] = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX;
	strToken = strtok(NULL,"\r:");
	strToken = strtok(NULL,"\r:");
	ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY = atoi(strToken)*MAX_VOLTAGE/MAX_DAC;
	m_Ybase[m_nReadPwrCH[nChannel]] = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY;
	strToken = strtok(NULL,"\r:");
	strToken = strtok(NULL,"\r:");
	ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower = atoi(strToken);
	
	strMsg.Format("通道%d的电压点为：%d,%d,对应的ADC为%.0f！",nChannel+1,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX,
		ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY,ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower);
	LogInfo(strMsg);

	if(ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower<200)
	{
		return FALSE;
	}

	//保存文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	int    iSel=IDYES;

	if (m_cbComPort.GetCurSel()==1)
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
	}
	else
	{
		strCSVFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage_%d.csv",
			m_strLocalPath,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
	}
	
	if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
	{
		MessageBox("创建数据文件失败！");
		return FALSE;
	}
	stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");
//	for (int i=0;i<m_nCHCount;i++)
	{
		strContent.Format("%d,%d,%d,%.2f\n",m_nReadPwrCH[nChannel]+1,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY,ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower);
		stdCSVFile.WriteString(strContent);
	}
	stdCSVFile.Close();	

	return TRUE;
}

/*
void CMy126S_45V_Switch_AppDlg::OnButtonIsoTest() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	CString strMsg;
	double  dblPower;
	double  dblISO;

	if (m_cbComPort.GetCurSel()==0)
	{
		MessageBox("请将COM0接进光，COM1接光功率计探头1！");
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		MessageBox("请将COM1接进光，COM0接光功率计探头1！");
	}
	int n = 0;
	for ( n=0;n<m_nCHCount;n++)
	{
		if (m_bTestStop)
		{
			MessageBox("测试中止！");
			return;
		}
		if (m_cbComPort.GetCurSel()==1)
		{
			if(!SPRTProductCH(n+m_nCHCount))
			{
				strMsg.Format("切换到通道%d错误",n+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				return;
			}
			else
			{
				strMsg.Format("切换到通道%d成功",n+1);
				LogInfo(strMsg);
			}
		}
		else
		{
			if(!SPRTProductCH(n))
			{
				strMsg.Format("切换到通道%d错误",n+1);
				LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				return;
			}
			else
			{
				strMsg.Format("切换到通道%d成功",n+1);
				LogInfo(strMsg);
			}
		}
		//开关切换       
		Sleep(200);
		dblPower = ReadPWMPower(0);
		dblISO = m_dblReferencePower[0] - dblPower;
		ChannalMessage[n].dblISO = dblISO;
		if (!CheckParaPassOrFail(RL_DATA, ChannalMessage[n].dblISO, n))
		{
			strMsg.Format("CH%d,ISO:%.1fdB,参数不合格", n + 1, ChannalMessage[n].dblISO);
			LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		}
		else
		{
			strMsg.Format("CH%d,ISO:%.1fdB", n + 1, ChannalMessage[n].dblISO);
			LogInfo(strMsg);
		}
	}
	SaveDataToCSV(ISO_DATA);
	SetVoltage(0);
}
*/

void CMy126S_45V_Switch_AppDlg::OnButtonOpenTesterPort() 
{
	// TODO: Add your control notification handler code here
	TCHAR tszCOM[20];
    CString strCOM;
	CString strError,strMsg;
	CString strFileName;
	m_bTestStop = FALSE;
	UpdateData(TRUE);
	ZeroMemory(tszCOM,sizeof(tszCOM));

	if (!m_bReady)
	{
		MessageBox("请先输入SN！");
		return;
	}
	int inPM = m_cbSetPDBox.GetCurSel();
	if(inPM < 0)
	{
		MessageBox("请选择PD盒！");
		return;
	}
	//产品串口
	if(m_stWorkStation.nProductPort>9)
	{
		strCOM.Format("\\\\.\\COM%d", m_stWorkStation.nProductPort);
	}
	else
	{
		strCOM.Format("COM%d", m_stWorkStation.nProductPort);
	}
    strcpy(tszCOM,strCOM);

	if (m_bTesterPort)
	{
		KillTimer(3);
		m_CmdModule.ClosePort();
		SetDlgItemText(IDC_BUTTON_OPEN_TESTER_PORT,"打开串口");
		m_bTesterPort = FALSE;
		LogInfo("关闭串口成功！");
	}
	else
	{
		if (!m_CmdModule.OpenPort(tszCOM,115200))
		{
			strMsg.Format("串口打开错误！");
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return;
		}
		else
		{
			LogInfo("串口打开成功！");
			m_bTesterPort = TRUE;
			SetDlgItemText(IDC_BUTTON_OPEN_TESTER_PORT,"关闭串口");	
			//SetTimer(3,100,NULL);
		}
	}

	int inSW4TH = m_cbSetWL.GetCurSel();

	//SW4TH
	if (m_stWorkStation.nSW4THPort > 9)
	{
		strCOM.Format("\\\\.\\COM%d", m_stWorkStation.nSW4THPort);
	}
	else
	{
		strCOM.Format("COM%d", m_stWorkStation.nSW4THPort);
	}

	strcpy(tszCOM, strCOM);

	if (!m_CmdInstrument.OpenPort(tszCOM, 115200))
	{
		strMsg.Format("第4代开关盒串口打开错误！");
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	else
	{
		LogInfo("第4代开关盒串口打开成功！");
		//m_bTesterPort = TRUE;
		//SetDlgItemText(IDC_BUTTON_OPEN_TESTER_PORT, "关闭串口");
		//SetTimer(3,100,NULL);
	}

	inPM = m_cbSetPDBox.GetCurSel();

	//PM
	if (m_stWorkStation.nPMPort > 9)
	{
		strCOM.Format("\\\\.\\COM%d", m_stWorkStation.nPMPort);
	}
	else
	{
		strCOM.Format("COM%d", m_stWorkStation.nPMPort);
	}

	strcpy(tszCOM, strCOM);

	if (inPM == 0)
	{
		if (!m_opTesterCom.OpenPort(tszCOM, 115200))
		{
			strMsg.Format("20CH_PD盒串口打开错误！");
			LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
			return;
		}
		else
		{
			LogInfo("20CH_PD盒串口打开成功！");
			//m_bTesterPort = TRUE;
			//SetDlgItemText(IDC_BUTTON_OPEN_TESTER_PORT, "关闭串口");
			//SetTimer(3,100,NULL);
		}
	}
	else if (inPM == 1)
	{
		if (!m_PDBoxCmd.OpenPort(tszCOM, 115200))
		{
			strMsg.Format("64CH_PD盒串口打开错误！");
			LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
			return;
		}
		else
		{
			LogInfo("64CH_PD盒串口打开成功！");
			//m_bTesterPort = TRUE;
			//SetDlgItemText(IDC_BUTTON_OPEN_TESTER_PORT, "关闭串口");
			//SetTimer(3,100,NULL);
		}
	}
	else
	{
		m_bDeviceOpen = FALSE;
		LogInfo("没有选择PD盒设备！", 1,COLOR_RED);
		return;	
	}
	MessageBox("请将1550光源接到第四代开关盒S1，1310光源接到第四代开关盒S2");
	m_bDeviceOpen = TRUE;
	if (m_stWorkStation.bTLSEnable)
	{
		m_bDeviceOpen = FALSE;
		if (!OpenTLSDevice())
		{
			return;
		}
		m_bDeviceOpen = TRUE;
	}
	m_bReadPowerStop = FALSE;
	//Open Device
	/*
	if(!m_bConnect1830C)
	{
		if (!OpenPMDevice()) 
		{
			return;
		}
		m_bConnect1830C = TRUE;
	}
	*/
	
}

BOOL CMy126S_45V_Switch_AppDlg::SPRTTesterCH(int nCH)
{
	CString strTemp;
	char pbyData[30];
	ZeroMemory(pbyData,sizeof(char)*20);
	
	strTemp.Format("SPRT 1 %d\r\n",nCH);
	memcpy(pbyData,strTemp,strTemp.GetLength());
	
    if(!m_opTesterCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		MessageBox("串口未打开，通道设置错误！");
		return FALSE;
	}
	Sleep(50);
    return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::AutoScan4x3(int nChannel, CString strFileINI, BOOL bScanRange)
{
	int x=0;
	int y=0;
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
    char    chValue[10];
	ZeroMemory(chValue,sizeof(char)*10);
	CString strINIFile;
	CString strValue;
	CString strStartX;
	CString strEndX;
	CString strStartY;
	CString strEndY;
	BOOL    bFindPoint = FALSE;
	m_bTestStop = FALSE;
	CString strSection;
	CString strNetFile;
	CString strKey;
	CString strCHFile;
	BOOL bFunctionOK=FALSE;
	double     dblPoint;
	//保存文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	int    iSel=IDYES;
	int nDACX = x*MAX_DAC/MAX_VOLTAGE;
	int nDACY = y*MAX_DAC/MAX_VOLTAGE;
	CString strSendBuf;
	char chSendBuf[50];
	char chReadBuf[256];
	CString strToken;
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	int nComPort;

	try
	{
		//获取中心点信息：
		strCHFile.Format("%s\\config\\%s\\ChannelCenter.ini",m_strConfigPath,m_strPN);
		nComPort = m_cbComPort.GetCurSel();
		if (nComPort==0)
		{	
			strCHFile.Format("%s\\config\\%s\\ChannelCenter.ini",m_strConfigPath,m_strPN);
		}
		else if (nComPort==1)
		{
			strCHFile.Format("%s\\config\\%s\\ChannelCenter-COM1.ini",m_strConfigPath,m_strPN);
		}
		
		GetPrivateProfileString("RangeScan","Point","error",strValue.GetBuffer(128),128,strCHFile);
		if (strValue == "error")
		{
			LogInfo("读取配置ScanConfig错误！（ChannelCenter.ini）");
			return FALSE;
		}
		dblPoint = atof(strValue);
		m_nReadPwrCH[nChannel] = m_cbChannel.GetCurSel();
		strSection.Format("CH%d", m_nReadPwrCH[nChannel]);
		GetPrivateProfileString(strSection,"x","error",strValue.GetBuffer(128),128,strCHFile);
		if (strValue=="error")
		{
			LogInfo("读取中心点配置文件错误!（ChannelCenter.ini）",(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		x = atoi(strValue);
		GetPrivateProfileString(strSection,"y","error",strValue.GetBuffer(128),128,strCHFile);
		if (strValue=="error")
		{
			LogInfo("读取中心点配置文件错误!",(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		y = atoi(strValue);
		
	/*	dblPoint = m_dblBreakIL;
		x = m_nCenterX[nComPort][m_nReadPwrCH[nChannel]];
		y = m_nCenterY[nComPort][m_nReadPwrCH[nChannel]];
		strMsg.Format("CH%d 中心电压: %dmv,%dmv\r\n",m_nReadPwrCH[nChannel]+1,x,y);
		LogInfo(strMsg);*/
		//开始扫描通道
		nDACX = x*MAX_DAC/MAX_VOLTAGE;
		nDACY = y*MAX_DAC/MAX_VOLTAGE;

		ZeroMemory(chSendBuf,sizeof(chSendBuf));
		ZeroMemory(chReadBuf,sizeof(chReadBuf));

		int  nMinPower = 1.0 * 100;
		int  nMaxPower = 5.0 * 100;

	//	strSendBuf.Format("SCAN MONPWR ALL 1 %d %d\r", nMinPower, nMaxPower);
	//	LogInfo(strSendBuf);
		strSendBuf.Format("SCAN ZONE 1 %d %d %d\r\n", m_nReadPwrCH[nChannel] + 1, nDACX, nDACY);
		LogInfo(strSendBuf);
		memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
		if(!m_opTesterCom.WriteBuffer(chSendBuf,strSendBuf.GetLength()))
		{
			strMsg.Format("发送指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		Sleep(2000);
		if (!m_opTesterCom.ReadBuffer(chReadBuf,256))
		{
			strMsg.Format("接收指令%s失败!",strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		strToken = strtok(chReadBuf, "\r");
		if (strToken.Find("OK", 0) == -1)
		{
			strMsg.Format("接收指令%s失败!", strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		LogInfo(chReadBuf);
		strToken = strtok(chReadBuf,"\r\n:");
		strToken = strtok(NULL,"\r\n:");
 		ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX = atoi(strToken);
		m_Xbase[m_nReadPwrCH[nChannel]] = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX;
		strToken = strtok(NULL,"\r\n:");
		strToken = strtok(NULL,"\r\n:");
		ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY = atoi(strToken);
		m_Ybase[m_nReadPwrCH[nChannel]] = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY;
		//
		//设置X Y
		if(!SETX(ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX))
		{
			throw"设置XDAC失败！";
		}
		if(!SETY(ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY))
		{
			throw"设置YDAC失败！";
		}
	    //转化为电压
		ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX =ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX*MAX_VOLTAGE/MAX_DAC;
		m_Xbase[m_nReadPwrCH[nChannel]] = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX;
		ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY*MAX_VOLTAGE/MAX_DAC;
		m_Ybase[m_nReadPwrCH[nChannel]] = ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY;

		//读取该通道的功率值
		if(!GetPDPower(m_nReadPwrCH[nChannel]+1,&dblPower))
		{
			throw"读取功率失败！";
		}

		//读取到的功率需要减去归零功率
		ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower = m_dblReferencePower[m_nReadPwrCH[nChannel]]-dblPower;
		
		strMsg.Format("通道%d的电压点为:%d,%d,获取功率为:%.2fdBm,对应的IL为%.2fdB！",nChannel+1,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX,
			ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY,dblPower,ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower);
		LogInfo(strMsg);
		if(ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower>dblPoint)
		{
			throw"IL太大";
		}

		if (m_cbComPort.GetCurSel()==1)
		{
			strNetFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
				m_strNetFile,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
				m_strLocalPath,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
		}
		else
		{
			strNetFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage_%d.csv",
				m_strNetFile,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage_%d.csv",
				m_strLocalPath,m_strItemName,m_strSN,m_strSN,m_nReadPwrCH[nChannel]+1);
		}
		
		if (!stdCSVFile.Open(strCSVFile,CFile::modeCreate|CFile::modeWrite))
		{
			throw("创建数据文件失败！");
		}
		stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");
	
		strContent.Format("%d,%d,%d,%.2f\n",m_nReadPwrCH[nChannel]+1,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX,ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY,ScanPowerChannal[m_nReadPwrCH[nChannel]].MaxPower);
		stdCSVFile.WriteString(strContent);
		
		stdCSVFile.Close();	
		if (m_bIfSaveToServe)
		{
			CopyFile(strCSVFile,strNetFile,FALSE);
		}

		bFunctionOK=TRUE;
		if (ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX<m_stLimitPara[nComPort].dblMinXVoltage[m_nReadPwrCH[nChannel]]||
			ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX>m_stLimitPara[nComPort].dblMaxXVoltage[m_nReadPwrCH[nChannel]])
		{
			m_bVoltagePass = FALSE;
			strMsg.Format("X电压超出范围[%.0f,%.0f]",m_stLimitPara[nComPort].dblMinXVoltage[m_nReadPwrCH[nChannel]],
				m_stLimitPara[nComPort].dblMaxXVoltage[m_nReadPwrCH[nChannel]]);
		    LogInfo(strMsg);
		}

		if (ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY<m_stLimitPara[nComPort].dblMinYVoltage[m_nReadPwrCH[nChannel]]||
			ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY>m_stLimitPara[nComPort].dblMaxYVoltage[m_nReadPwrCH[nChannel]])
		{
			m_bVoltagePass = FALSE;
			strMsg.Format("Y电压超出范围[%.0f,%.0f]",m_stLimitPara[nComPort].dblMinYVoltage[m_nReadPwrCH[nChannel]],
				m_stLimitPara[nComPort].dblMaxYVoltage[m_nReadPwrCH[nChannel]]);
		    LogInfo(strMsg);
		}
		/*
		if (nChannel==0) 
		{
			strMsg.Format("%d",ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX);
			SetDlgItemText(IDC_EDIT_X,strMsg);
			strMsg.Format("%d",ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY);
			SetDlgItemText(IDC_EDIT_Y,strMsg);
			UpdateWindow();
		}
		*/
	}
	catch(TCHAR* ptszErrorMsg)
	{
		MessageBox(ptszErrorMsg);
	}
	catch(...)
	{
		MessageBox("其他错误！扫描失败");
	}
	return bFunctionOK;
}

BOOL CMy126S_45V_Switch_AppDlg::SPRTProductCH(int nCH)
{
	CString strMsg;
	if (m_nSNIndex >= 32)
	{
		if (!m_CmdModule.SwitchSingleSwitch(3, m_nSNIndex - 32 + 1, nCH))
		{
			strMsg.Format("%s", m_CmdModule.m_strLogInfo);
			LogInfo(strMsg);
			return FALSE;
		}
	}
	else if (m_nSNIndex >= 16)
	{
		if (!m_CmdModule.SwitchSingleSwitch(2, m_nSNIndex-16 + 1, nCH))
		{
			strMsg.Format("%s", m_CmdModule.m_strLogInfo);
			LogInfo(strMsg);
			return FALSE;
		}
	}
	else
	{
		if (!m_CmdModule.SwitchSingleSwitch(1, m_nSNIndex + 1, nCH))
		{
			strMsg.Format("%s", m_CmdModule.m_strLogInfo);
			LogInfo(strMsg);
			return FALSE;
		}

	}
	
	/*
	CString strTemp;
	char pbyData[30];
	ZeroMemory(pbyData,sizeof(char)*20);
	
	strTemp.Format("SPRT 1 %d\r\n",nCH);
	memcpy(pbyData,strTemp,strTemp.GetLength());
	
    if(!m_opTesterCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		MessageBox("串口未打开，通道设置错误！");
		return FALSE;
	}
	Sleep(50);
	*/
    return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::GetCT(double *dblValue, int nCount)
{
	CString strTemp;
	CString strToken;
	char pbyData[30];
	char chData[256];
	ZeroMemory(pbyData,sizeof(pbyData));
	ZeroMemory(chData,sizeof(chData));
	
	strTemp.Format("get power 0 %d\r\n",nCount);
	memcpy(pbyData,strTemp,strTemp.GetLength());	
    if(!m_opTesterCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{
		LogInfo("串口未打开，通道设置错误！");
		return FALSE;
	}
	Sleep(50);
	if(!m_opTesterCom.ReadBuffer(chData,256))
	{
		LogInfo("串口未打开，通道设置错误！");
		return FALSE;
	}
	strToken = strtok(chData,"\r\n");
	int i = 0;
	for( i=0;i<nCount;i++)
	{
		dblValue[i] = atof(strToken)/100.0;
		strToken = strtok(NULL,"\r\n");
	}
    return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonSwitchAll() 
{
	// TODO: Add your control notification handler code here
	if (!m_bOpenPM)
	{
		MessageBox("请先打开光功率计！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}
	m_bReadPowerStop = TRUE;

	CString strMsg;
	CString strFile;
	CString strValue;
	double  dblPower;
	FILE    *fp = NULL;
	strFile.Format("%s\\data\\SWITCH_ALL.csv",g_tszAppFolder);
	fp = fopen(strFile,"wt");
	strValue.Format(",CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH9,CH10,CH11,CH12,CH13,CH14,CH15,CH16,CH17,CH18,CH19\n");
	fprintf(fp,strValue);
	int nPort = 0;
	int nCH = 0;
	for ( nCH = 0;nCH<m_nCHCount;nCH++)
	{
		strValue.Format("CH%d,",nCH+1);
		fprintf(fp,strValue);
		for( nPort=0;nPort<m_nCHCount;nPort++)
		{
			YieldToPeers();
			if (m_bTestStop)
			{
				fclose(fp);
				return;
			}
			if (nPort==nCH)
			{
				strValue.Format("XX,");
		        fprintf(fp,strValue);
				continue;
			}
			SPRTProductCH(nPort+1);
			Sleep(100);
			SPRTProductCH(nCH+1);
			dblPower = ReadPWMPower(nCH);
			dblPower = m_dblReferencePower[nCH] - dblPower;
			strMsg.Format("CH%d->CH%d,IL:%.2f dB",nPort+1,nCH+1,dblPower);
			LogInfo(strMsg);
			strValue.Format("%.2f,",dblPower);
        	fprintf(fp,strValue);
		}
		strValue.Format("\n");
        fprintf(fp,strValue);
	}
	fclose(fp);
	MessageBox("测试完毕！");

}

double CMy126S_45V_Switch_AppDlg::ReadPDPower()
{
	double	dblPower;
	char chSendBuf[256];
	char chReadBuf[256];
	CString strSend;
	CString strToken;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	
	//	SPRTTesterCH(nChannel+1);
	//	Sleep(50);
	
	strSend.Format("get power\r\n");
	memcpy(chSendBuf,strSend,strSend.GetLength());
	
	if (!m_opTesterCom.WriteBuffer(chSendBuf,strSend.GetLength()))
	{
		LogInfo("发送Get power错误！");
		dblPower=-99;
		return dblPower;
	}
	Sleep(50);
	if (!m_opTesterCom.ReadBuffer(chReadBuf,256))
	{
		LogInfo("接收Get power错误！");
		dblPower=-99;
		return dblPower;
	}
	dblPower = atof(chReadBuf);
	dblPower = dblPower/1000.0;
	return dblPower;

}

BOOL CMy126S_45V_Switch_AppDlg::SETTesterX(int x)
{
	CString strTemp;
	char pbyData[20];
	char pbyRx[20];  
	
	ZeroMemory(pbyData,sizeof(char)*20);
	ZeroMemory(pbyRx,sizeof(char)*20);
	
	CString strIniFile;
	CString strValue;
	int     nMax;
	int     nMin;
	strIniFile.Format("%s\\config\\VoltageMax.ini",m_strConfigPath);
	GetPrivateProfileString("Voltage","XMax","error",strValue.GetBuffer(128),128,strIniFile);
	nMax = atoi(strValue);
	if (x>nMax)
	{
		LogInfo("电压超过上限！");
		return FALSE;
	}
	GetPrivateProfileString("Voltage","XMin","error",strValue.GetBuffer(128),128,strIniFile);
	nMin = atoi(strValue);
	if (x<nMin)
	{
		LogInfo("电压超过上限！");
		return FALSE;
	}
	
	// strTemp.Format("setx dac %d\r\n",x);
	x = x*MAX_DAC/MAX_VOLTAGE;
	strTemp.Format("setx 1 %d\r\n",x);
	memcpy(pbyData,strTemp,strTemp.GetLength());
	if(!m_opTesterCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{ 
		MessageBox("串口没打开，设置x电压错误！");
		return FALSE;
	}
  return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::SETTesterY(int y)
{
	CString strTemp;
	char pbyData[20];
	char pbyRx[20];  
	
	ZeroMemory(pbyData,sizeof(char)*20);
	ZeroMemory(pbyRx,sizeof(char)*20);
	
	CString strIniFile;
	CString strValue;
	int     nMax;
	int     nMin;
	strIniFile.Format("%s\\config\\VoltageMax.ini",m_strConfigPath);
	GetPrivateProfileString("Voltage","YMax","error",strValue.GetBuffer(128),128,strIniFile);
	nMax = atoi(strValue);
	if (y>nMax)
	{
		LogInfo("电压超过上限！");
		return FALSE;
	}
	GetPrivateProfileString("Voltage","YMin","error",strValue.GetBuffer(128),128,strIniFile);
	nMin = atoi(strValue);
	if (y<nMin)
	{
		LogInfo("电压超过上限！");
		return FALSE;
	}
	
	// strTemp.Format("setx dac %d\r\n",x);
	y = y*MAX_DAC/MAX_VOLTAGE;
	strTemp.Format("sety 1 %d\r\n",y);
	memcpy(pbyData,strTemp,strTemp.GetLength());
	if(!m_opTesterCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{ 
		MessageBox("串口没打开，设置x电压错误！");
		return FALSE;
	}
    return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonSetTesterVoltage() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CString strVolt;
	int     nVoltX;
	int     nVoltY;
	CString strMsg;
	if (!m_bTesterPort)
	{
		MessageBox("请先打开对应的串口！");
		return;
	}
	
	GetDlgItemText(IDC_EDIT_X,strVolt);
	nVoltX = atoi(strVolt);
	if(SETTesterX(nVoltX))
	{
		strMsg.Format("X电压设置OK");
		LogInfo(strMsg);
	}
	
	GetDlgItemText(IDC_EDIT_Y,strVolt);
	nVoltY = atoi(strVolt);
	if(SETTesterY(nVoltY))
	{
		strMsg.Format("Y电压设置OK");
		LogInfo(strMsg);
	}
}

BOOL CMy126S_45V_Switch_AppDlg::GetPDPower(int nChannel,double* pdblPower)
{
	int nDACX ;
	int nDACY ;
	CString strSendBuf;
	char chSendBuf[50];
	char chReadBuf[256];
	CString strToken;
	CString strMsg;
	ZeroMemory(chSendBuf,sizeof(chSendBuf));
	ZeroMemory(chReadBuf,sizeof(chReadBuf));
	strSendBuf.Format("get power %d\r\n",nChannel);
	LogInfo(strSendBuf);
	memcpy(chSendBuf,strSendBuf,strSendBuf.GetLength());
	if(!m_opTesterCom.WriteBuffer(chSendBuf,strSendBuf.GetLength()))
	{
		strMsg.Format("发送指令%s失败!",strSendBuf);
		LogInfo(strMsg);
		return FALSE;
	}
	Sleep(50);
	if (!m_opTesterCom.ReadBuffer(chReadBuf,256))
	{
		strMsg.Format("接收指令%s失败!",strSendBuf);
		LogInfo(strMsg);
		return FALSE;
	}
	*pdblPower=(double)atoi(chReadBuf)/100;
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnButtonUp() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	UpdateData();

	CString strMsg;
	m_cbComPort.SetCurSel(0);
    if (!GetCSVData())
    {
		LogInfo("获取原始数据失败！",(BOOL)FALSE,COLOR_RED);
		SetVoltage(0);
		return;
    }
	if (m_nSwitchComPortCount >= 2)
	{
		m_cbComPort.SetCurSel(1);
		if (!GetCSVData())
		{
			LogInfo("获取原始数据失败！", (BOOL)FALSE, COLOR_RED);
			SetVoltage(0);
			return;
		}
	}
	if(m_strPN=="2x16")
	{
		UpdateATMData2X16();
	}
	else
	{
		if(!UpdateATMData())
		{
			strMsg.Format("旧模板不存在，尝试新模板");
			LogInfo(strMsg);
			if (!UpdateNewATMData()) 
			{
				strMsg.Format("无纸化模板上传失败！");
				LogInfo(strMsg);
			}
		}
	}
	SetVoltage(0);
	SetDlgItemText(IDC_EDIT_SN,""); //清空SN号
	m_bReady = FALSE;
    
}

BOOL CMy126S_45V_Switch_AppDlg::GetCSVData()
{
	CString strValue;
	int nIndex=0;
	int nIndexCH=0;
	CString strCSVFile;
	CString strNetFile;
	CStdioFile stdFile;
	CString strMsg;
	CString strToken;
	char chReadBuf[256];
	LPCTSTR   lpstr = NULL;
	double dblMax;
	double dblMin;

	UpdateData();
	//启动EXCEL
	//保存的数据类型
	int nCom = m_cbComPort.GetCurSel();
	strMsg.Format("COM%d", nCom);
	LogInfo(strMsg);
	int nType = 0;
	for ( nType=0;nType<END_DATA;nType++)
	{
		switch (nType)
		{
		case VOL_DATA:
			//选择表单
			if (m_cbSaveData.GetCurSel()==0)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\Voltage-点胶前.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\Voltage-点胶前.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\Voltage-点胶前COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\Voltage-点胶前COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				
			}
			else if (m_cbSaveData.GetCurSel()==1)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\Voltage-点胶后.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\Voltage-点胶后.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\Voltage-点胶后COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\Voltage-点胶后COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}	
			}

			if (m_bIfSaveToServe)
			{
				if (!stdFile.Open(strNetFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strNetFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			else
			{
				if (!stdFile.Open(strCSVFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strCSVFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			stdFile.ReadString(chReadBuf,256);
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				lpstr = stdFile.ReadString(chReadBuf,256);
				if (lpstr==NULL)
				{
					ScanPowerChannal[nIndex].VoltageX = -99;
					ScanPowerChannal[nIndex].VoltageY = -99;
					strMsg.Format("读取文件%s通道%d电压失败",strCSVFile,nIndex+1);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				}
				else
				{
					strToken = strtok(chReadBuf,",");
					strToken = strtok(NULL,",");
					if (nCom == 0)
					{
						ScanPowerChannal[nIndex].VoltageX = atoi(strToken);
					}
					else
					{
						ScanPowerChannal[nIndex].VoltageXCOM[nCom] = atoi(strToken);
					}
					strToken = strtok(NULL,",");
					if (nCom == 0)
					{
						ScanPowerChannal[nIndex].VoltageY = atoi(strToken);
					}
					else
					{
						ScanPowerChannal[nIndex].VoltageYCOM[nCom] = atoi(strToken);
					}
				}
			}
			stdFile.Close();
			break;
			
		case IL_ROOM_DATA:
			//选择表单
			if (m_cbSaveData.GetCurSel()==0)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\IL_Room-点胶前.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\IL_Room-点胶前.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\IL_Room-点胶前COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\IL_Room-点胶前COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				
			}
			else if (m_cbSaveData.GetCurSel()==1)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\IL_Room-点胶后.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\IL_Room-点胶后.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\IL_Room-点胶后COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\IL_Room-点胶后COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}	
			}
			if (m_bIfSaveToServe)
			{
				if (!stdFile.Open(strNetFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strNetFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			else
			{
				if (!stdFile.Open(strCSVFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strCSVFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			stdFile.ReadString(chReadBuf,256);
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				lpstr = stdFile.ReadString(chReadBuf,256);
				if (lpstr==NULL)
				{
					ChannalMessage[nIndex].dblIL = -99;
					strMsg.Format("读取文件%s通道%dIL失败",strCSVFile,nIndex+1);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				}
				else 
				{
					strToken = strtok(chReadBuf,",");
					strToken = strtok(NULL,",");
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblIL = atof(strToken);
					}
					else
					{
						ChannalMessage[nIndex].dblILCOM[nCom] = atof(strToken);
					}
		     		
				}
				
			}
			stdFile.Close();
			break;
		case CT_DATA:
			//选择表单
			if (m_cbSaveData.GetCurSel()==0)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶前.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶前.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶前COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶前COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				
			}
			else if (m_cbSaveData.GetCurSel()==1)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶后.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶后.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶后COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶后COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}	
			}
			if (m_bIfSaveToServe)
			{
				if (!stdFile.Open(strNetFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strNetFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			else
			{
				if (!stdFile.Open(strCSVFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strCSVFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			stdFile.ReadString(chReadBuf,256);
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				//dblMax = -99;
				if (nCom == 0)
				{
					ChannalMessage[nIndex].dblMinCT = 100;
				}
				else
				{
					ChannalMessage[nIndex].dblMinCTCOM[nCom] = 100;
				}
				
				lpstr = stdFile.ReadString(chReadBuf,256);
				strToken = strtok(chReadBuf,",");
				for(nIndexCH=0;nIndexCH<m_nCHCount;nIndexCH++)
				{
					if (lpstr==NULL)
					{
						if (nCom == 0)
						{
							ChannalMessage[nIndex].dblCT[nIndexCH] = -99;
						}
						else
						{
							ChannalMessage[nIndex].dblCTCOM[nIndexCH][nCom] = -99;
						}
						
						strMsg.Format("读取文件%s通道%d-%dCT失败",strCSVFile,nIndex+1,nIndexCH+1);
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					}
					else
					{
						strToken = strtok(NULL,",");
						if (nCom == 0)
						{
							ChannalMessage[nIndex].dblCT[nIndexCH] = atof(strToken);
							if (nIndexCH != nIndex)
							{
								if (ChannalMessage[nIndex].dblMinCT>ChannalMessage[nIndex].dblCT[nIndexCH])
								{
									ChannalMessage[nIndex].dblMinCT = ChannalMessage[nIndex].dblCT[nIndexCH];
								}
							}
						}
						else
						{
							ChannalMessage[nIndex].dblCTCOM[nIndexCH][nCom] = atof(strToken);
							if (nIndexCH != nIndex)
							{
								if (ChannalMessage[nIndex].dblMinCTCOM[nCom]>ChannalMessage[nIndex].dblCTCOM[nIndexCH][nCom])
								{
									ChannalMessage[nIndex].dblMinCTCOM[nCom] = ChannalMessage[nIndex].dblCTCOM[nIndexCH][nCom];
								}
							}
						}
						
					}
					
				}	
			}
			stdFile.Close();
			break;
		case DARK_DATA:
			if (m_cbSaveData.GetCurSel()==0)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶前.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
		    		strNetFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶前.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶前COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
		    		strNetFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶前COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}	
			}
			else if (m_cbSaveData.GetCurSel()==1)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶后.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶后.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶后COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶后COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}	
			}
			if (m_bIfSaveToServe)
			{
				if (!stdFile.Open(strNetFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strNetFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			else
			{
				if (!stdFile.Open(strCSVFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strCSVFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			stdFile.ReadString(chReadBuf,256);
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				lpstr = stdFile.ReadString(chReadBuf,256);
				if (lpstr==NULL)
				{
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblDarkIL = -99;
					}
					else
					{
						ChannalMessage[nIndex].dblDarkILCOM[nCom] = -99;
					}
					
					strMsg.Format("读取文件%s通道%dDARK失败",strCSVFile,nIndex+1);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				}
				else
				{
					strToken = strtok(chReadBuf,",");
					strToken = strtok(NULL,",");
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblDarkIL = atof(strToken);
					}
					else
					{
						ChannalMessage[nIndex].dblDarkILCOM[nCom] = atof(strToken);
					}
				}
			}
			stdFile.Close();
			break;	
		case RE_IL_DATA:
			if (m_cbSaveData.GetCurSel()==0)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶前.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
		    		strNetFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶前.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶前COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
		    		strNetFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶前COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				
			}
			else if (m_cbSaveData.GetCurSel()==1)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶后.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶后.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶后COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶后COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}	
			}
			if (m_bIfSaveToServe)
			{
				if (!stdFile.Open(strNetFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strNetFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			else
			{
				if (!stdFile.Open(strCSVFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strCSVFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			stdFile.ReadString(chReadBuf,256);
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				lpstr = stdFile.ReadString(chReadBuf,256);
				if (lpstr==NULL)
				{
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblDif = -99;
					}
					else
					{
						ChannalMessage[nIndex].dblDifCOM[nCom] = -99;
					}
					
					strMsg.Format("读取文件%s通道%dRepeatIL失败",strCSVFile,nIndex+1);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				}
				else
				{
					strToken = strtok(chReadBuf,",");
					strToken = strtok(NULL,",");
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblDif = atof(strToken);
					}
					else
					{
						ChannalMessage[nIndex].dblDifCOM[nCom] = atof(strToken);
					}
				}
			}
			stdFile.Close();
			break;
		case RL_DATA:
			if (m_cbSaveData.GetCurSel()==0)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\RL-点胶前.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\RL-点胶前.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\RL-点胶前COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\RL-点胶前COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				
			}
			else if (m_cbSaveData.GetCurSel()==1)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\RL-点胶后.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\RL-点胶后.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\RL-点胶后COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\RL-点胶后COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}	
			}
			if (m_bIfSaveToServe)
			{
				if (!stdFile.Open(strNetFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strNetFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			else
			{
				if (!stdFile.Open(strCSVFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strCSVFile);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
					break;
				}
			}
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			stdFile.ReadString(chReadBuf,256);
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				lpstr = stdFile.ReadString(chReadBuf,256);
				if (lpstr==NULL)
				{
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblRL = -99;
					}
					else
					{
						ChannalMessage[nIndex].dblRLCOM[nCom] = -99;
					}
					
					strMsg.Format("读取文件%s通道%dRL失败",strCSVFile,nIndex+1);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				}
				else
				{
					strToken = strtok(chReadBuf,",");
					strToken = strtok(NULL,",");
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblRL = atof(strToken);
					}
					else
					{
						ChannalMessage[nIndex].dblRLCOM[nCom] = atof(strToken);
					}
				}
			}
			stdFile.Close();
			break;
		case ISO_DATA:
			if (m_cbSaveData.GetCurSel()==0)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\ISO-点胶前.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
		    		strNetFile.Format("%s\\data\\%s\\%s\\ISO-点胶前.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\ISO-点胶前COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
		    		strNetFile.Format("%s\\data\\%s\\%s\\ISO-点胶前COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}			
			}
			else if (m_cbSaveData.GetCurSel()==1)
			{
				if (nCom ==0)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\ISO-点胶后.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
		    		strNetFile.Format("%s\\data\\%s\\%s\\ISO-点胶后.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}
				else if (nCom ==1)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\ISO-点胶后COM1.CSV",
						m_strLocalPath,m_strItemName,m_strSN);
					strNetFile.Format("%s\\data\\%s\\%s\\ISO-点胶后COM1.CSV",
						m_strNetFile,m_strItemName,m_strSN);
				}	
			}
			if (m_bIfSaveToServe)
			{
				if (!stdFile.Open(strNetFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strNetFile);
					LogInfo(strMsg,(BOOL)FALSE);
					break;
				}
			}
			else
			{
				if (!stdFile.Open(strCSVFile,CFile::modeRead))
				{
					strMsg.Format("文件%s不存在",strCSVFile);
					LogInfo(strMsg,(BOOL)FALSE);
					break;
				}
			}
			ZeroMemory(chReadBuf,sizeof(chReadBuf));
			stdFile.ReadString(chReadBuf,256);
			for(nIndex=0;nIndex<m_nCHCount;nIndex++)
			{
				lpstr = stdFile.ReadString(chReadBuf,256);
				if (lpstr==NULL)
				{
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblISO = -99;
					}
					else
					{
						ChannalMessage[nIndex].dblISOCOM[nCom] = -99;
					}
					strMsg.Format("读取文件%s通道%dISO失败",strCSVFile,nIndex+1);
					LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
				}
				else
				{
					strToken = strtok(chReadBuf,",");
					strToken = strtok(NULL,",");
					if (nCom == 0)
					{
						ChannalMessage[nIndex].dblISO = atof(strToken);
					}
					else
					{
						ChannalMessage[nIndex].dblISOCOM[nCom] = atof(strToken);
					}
				}
			}
			stdFile.Close();
			break;
		}
	}
	
	if (m_bIfSaveToServe)
	{
		CopyFile(strCSVFile,strNetFile,FALSE);
	}	
	LogInfo("保存数据到CSV结束！");
	return TRUE;

}


BOOL CMy126S_45V_Switch_AppDlg::UpdateATMData2X16()
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
	CString strURL;
	CString strName;
	CString strNodName;
//	int nIndex;

	if (m_cbComPort.GetCurSel()==0)
	{
		strName="COM0";
	}
	else if(m_cbComPort.GetCurSel()==1)
	{
		strName="COM1";
	}
	//下载模板
	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&adjust=1",m_strSN);
	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
	if(!m_ctrXMLCtr.GetHtmlSource(strURL,strLocalFile))
	{
		strMsg = "模板下载错误!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	
	if (m_cbSaveData.GetCurSel()==0)
	{
		strTemp = "点胶前"; //预调
	}
	else if (m_cbSaveData.GetCurSel()==1)
	{
		strTemp = "点胶后";  //调节
	}
	strXMLFile.Format("%s\\Data\\%s\\%s\\%s_XML-%s.xml", m_strLocalPath,m_strItemName,m_strSN,m_strSN,strTemp);
	CopyFile(strLocalFile,strXMLFile,FALSE);
	m_ctrXMLCtr.LoadXmlFile(strXMLFile);
	//增加上传测试信息
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
    m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "USER", dwBuf, nNodeCount);
    if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的用户名不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}		
	m_ctrXMLCtr.ModifyNode("NodeValue",m_strID,dwBuf[0]);

	//测试类型
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
    m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SOFTWARE", dwBuf, nNodeCount);
    if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,SOFTWARE不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}	
	if (m_cbSaveData.GetCurSel()==0)
	{
		strTemp = "1830-F"; //预调
	}
	else if (m_cbSaveData.GetCurSel()==1)
	{
		strTemp = "1830-A";  //调节
	}

	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);

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

	//获取开始时间
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "START", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的工位不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	strTemp.Format("%d-%02d-%02d %02d:%02d:%02d",m_stStartTime.wYear,m_stStartTime.wMonth,m_stStartTime.wDay,m_stStartTime.wHour,m_stStartTime.wMinute,m_stStartTime.wSecond);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);//add end

	//获取结束时间
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "DATE", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的工位不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	GetLocalTime(&st);
//	CTime curTime = CTime::GetCurrentTime();
	strTemp.Format("%d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);//add end

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的SN号!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}	
	m_ctrXMLCtr.ModifyNode("NodeValue",m_strSN,dwBuf[0]);
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	if (strTemp != m_strSN) 
	{
		strMsg = "SN号不对应,请通知相关工程师!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的PN与无纸PN不符合!";
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

	ZeroMemory(dwBufTemp, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwBufTemp, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
//	DWORD dwTemp;COM0
	int nPort = 0;
	int nTestType = 0;
	for ( nTestType=0;nTestType<END_DATA;nTestType++)
	{
		switch (nTestType)
		{
		case VOL_DATA:
			for ( nTempIndex = 0; nTempIndex < nNodeCount; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温Voltage-COM0
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNodName.Format("Voltage-%s",strName);
				//	strNodName.Format("Voltage-COM0");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNodName, dwBufCH, nNodeCount,dwWDMTemp);
					if(nNodeCount==0)
					{
						strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
					}
					
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d_X",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%d",ScanPowerChannal[nCH].VoltageX);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);

						strTemp.Format("CH%d_Y",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%d",ScanPowerChannal[nCH].VoltageY);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);																	
					}
				}
			}
			break;
		case IL_ROOM_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNodName.Format("IL-%s",strName);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNodName, dwBufCH, nNodeCount,dwWDMTemp);
					if(nNodeCount==0)
					{
						strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
					}
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblIL);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
			}
			break;
		case CT_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					for (nCH = 0;nCH<m_nCHCount;nCH++)
					{
						ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
						strTemp.Format("CT%d-%s",nCH+1,strName);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBufCH, nNodeCount,dwWDMTemp);
						if(nNodeCount==0)
						{
							strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
							LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
							break;
						}
						for( nPort = 0;nPort<m_nCHCount;nPort++)
						{
							if (nPort == nCH)
							{
								continue;
							}
							strTemp.Format("CH%d",nPort+1);
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
							strValue.Format("%.2f",ChannalMessage[nCH].dblCT[nPort]);
							m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
						}
					}				
				}
			}
			break;
		case RE_IL_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNodName.Format("RepeatIL-%s",strName);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNodName, dwBufCH, nNodeCount,dwWDMTemp);
					if(nNodeCount==0)
					{
						strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
					}
					
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblDif);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
			}
			break;
		case DARK_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNodName.Format("DarkIL-%s",strName);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNodName, dwBufCH, nNodeCount,dwWDMTemp);
					if(nNodeCount==0)
					{
						strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
				//		return FALSE;
					}
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblDarkIL);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
			}
			break;
		case RL_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNodName.Format("RL-%s",strName);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNodName, dwBufCH, nNodeCount,dwWDMTemp);
					if(nNodeCount==0)
					{
						strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
					}
					
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblRL);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
					}
				}
			}
			break;
		case ISO_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (strTemp=="room") //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					strNodName.Format("ISO-%s",strName);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNodName, dwBufCH, nNodeCount,dwWDMTemp);
					if(nNodeCount==0)
					{
						strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
						LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
						break;
					}
					
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblISO);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
					}
				}
			}
			break;
		}
	}
	
	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}	

	if (!SetFileByMSMQ(strXMLFile, strErrMsg))
	{
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		LogInfo("数据上传无纸化失败，请联系相应工程师!");
		return FALSE;
	}
	LogInfo("无纸化数据上传成功！");
	return TRUE;
}


BOOL CMy126S_45V_Switch_AppDlg::UpdateATMData()
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
//	int     nPort = 0;
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
	CString strURL;
//	int nIndex;

	//下载模板
	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&adjust=1",m_strSN);
	strLocalFile.Format("%s\\template\\Test.xml", g_tszAppFolder);
//	if (!m_bGetPaperlessTemp)
//	{		
		if(!m_ctrXMLCtr.GetHtmlSource(strURL,strLocalFile))
		{
			strMsg = "模板下载错误!";
			LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
			return FALSE;
		}
		m_bGetPaperlessTemp = TRUE;
//	}	

	/*
	if (m_cbSaveData.GetCurSel()==0)
	{
		strTemp = "点胶前"; //预调
	}
	else if (m_cbSaveData.GetCurSel()==1)
	{
		strTemp = "点胶后";  //调节
	}
	*/
	strTemp = "点胶后";
	strXMLFile.Format("%s\\Data\\%s\\%s\\%s_XML-%s.xml", 
		m_strLocalPath,m_strItemName,m_strSN,m_strSN,strTemp);
	CopyFile(strLocalFile,strXMLFile,FALSE);
	m_ctrXMLCtr.LoadXmlFile(strXMLFile);
	//增加上传测试信息
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
    m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "USER", dwBuf, nNodeCount);
    if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的用户名不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}		
	m_ctrXMLCtr.ModifyNode("NodeValue",m_strID,dwBuf[0]);

	//测试类型
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
    m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SOFTWARE", dwBuf, nNodeCount);
    if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,SOFTWARE不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	/*
	if (m_cbSaveData.GetCurSel()==0)
	{
		strTemp = "1830-F"; //预调
	}
	else if (m_cbSaveData.GetCurSel()==1)
	{
		strTemp = "1830-A";  //调节
	}
	*/
	strTemp = "1830-A";  //调节

	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);

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

	//获取开始时间
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "START", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的工位不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	strTemp.Format("%d-%02d-%02d %02d:%02d:%02d",m_stStartTime.wYear,m_stStartTime.wMonth,m_stStartTime.wDay,m_stStartTime.wHour,m_stStartTime.wMinute,m_stStartTime.wSecond);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);//add end

	//获取结束时间
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "DATE", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的工位不符合!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
	GetLocalTime(&st);
//	CTime curTime = CTime::GetCurrentTime();
	strTemp.Format("%d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);//add end

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的SN号!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}	
	m_ctrXMLCtr.ModifyNode("NodeValue",m_strSN,dwBuf[0]);
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	if (strTemp != m_strSN) 
	{
		strMsg = "SN号不对应,请通知相关工程师!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的PN与无纸PN不符合!";
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

	ZeroMemory(dwBufTemp, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwBufTemp, nNodeCount,dwWDMTemp);	
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}
//	DWORD dwTemp;
	int nPort = 0;
	int nTestType = 0;
	for ( nTestType=0;nTestType<END_DATA;nTestType++)
	{
		switch (nTestType)
		{
		case VOL_DATA:
			for ( nTempIndex = 0; nTempIndex < nNodeCount; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (stricmp(strTemp,"room")==0) //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Voltage", dwBufCH, nNodeCount,dwWDMTemp);
					if (nNodeCount!=0)
					{
						for(nCH = 0;nCH<m_nCHCount;nCH++)
						{
							strTemp.Format("CH%d_X",nCH+1);
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
							strValue.Format("%d",ScanPowerChannal[nCH].VoltageX);
							m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);

							strTemp.Format("CH%d_Y",nCH+1);
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
							strValue.Format("%d",ScanPowerChannal[nCH].VoltageY);
							m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);																	
						}
					}
					else
					{
						strMsg.Format("节点Voltage不存在");
						LogInfo(strMsg);
						return FALSE;
					}
					
				}
			}
			break;
		case IL_ROOM_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (stricmp(strTemp,"room")==0) //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "IL", dwBufCH, nNodeCount,dwWDMTemp);				
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblIL);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
			}
			break;
		case CT_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (stricmp(strTemp,"room")==0) //常温
				{
					for (nCH = 0;nCH<m_nCHCount;nCH++)
					{
						ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
						strTemp.Format("CT%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBufCH, nNodeCount,dwWDMTemp);
						for( nPort = 0;nPort<m_nCHCount;nPort++)
						{
							if (nPort == nCH)
							{
								continue;
							}
							strTemp.Format("CH%d",nPort+1);
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
							strValue.Format("%.2f",ChannalMessage[nCH].dblCT[nPort]);
							m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
						}
					}				
				}
			}
			break;
		case RE_IL_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (stricmp(strTemp,"room")==0) //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "RepeatIL", dwBufCH, nNodeCount,dwWDMTemp);
					
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblDif);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
			}
			break;
		case DARK_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (stricmp(strTemp,"room")==0) //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "DarkIL", dwBufCH, nNodeCount,dwWDMTemp);
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblDarkIL);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
					}
				}
			}
			break;
		case RL_DATA:
			for ( nTempIndex = 0; nTempIndex < 3; nTempIndex++)
			{
				dwWDMTemp = dwBufTemp[nTempIndex]; 
				m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
				if (stricmp(strTemp,"room")==0) //常温
				{
					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "RL", dwBufCH, nNodeCount,dwWDMTemp);
					
					for(nCH = 0;nCH<m_nCHCount;nCH++)
					{
						strTemp.Format("CH%d",nCH+1);
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strTemp, dwBuf, nNodeCount,dwBufCH[0]);
						strValue.Format("%.2f",ChannalMessage[nCH].dblRL);
						m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);	
					}
				}
			}
			break;
		}
	}
	
	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "无纸化关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		return FALSE;
	}	

	if (!SetFileByMSMQ(strXMLFile, strErrMsg))
	{
		LogInfo(strErrMsg,(BOOL)FALSE,COLOR_RED);
		LogInfo("数据上传无纸化失败，请联系相应工程师!");
		return FALSE;
	}
	LogInfo("无纸化数据上传成功！");
	CString strNetXML;
	strTemp = "点胶后";
	strNetXML.Format("%s\\Data\\%s\\%s\\%s_XML-%s.xml", 
		m_strNetFile,m_strItemName,m_strSN,m_strSN,strTemp);
	if (m_bIfSaveToServe) 
	{
		CopyFile(strXMLFile,strNetXML,FALSE);
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::SaveDataToCSV(int nType, int nChannel, BOOL bGetFile, int nTemp)
{
	CString strValue;
	CString strMsg;
	int nIndex = 0;
	int nIndexCH = 0;
	CString strCSVFile;
	CString strNetFile;
	FILE *fp = NULL;
	char  pchRead[256];
	double m_pdblWavelengthTemp = 0.0;
	UpdateData();
	//启动EXCEL
	//保存的数据类型
	switch (nType)
	{
	case VOL_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
				strCSVFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s.CSV",
					m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength, nChannel + 1, m_stSNList[nChannel].strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s.CSV",
					m_strNetFile, m_strPN, m_strSN, m_pdblWavelengthTemp, nChannel + 1, m_stSNList[nChannel].strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s-COM1.CSV",
				m_strLocalPath, m_strPN,m_strSN, m_pdblWavelength, m_stSNList[nChannel].strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s-COM1.CSV",
				m_strNetFile, m_strPN,m_strSN, m_pdblWavelengthTemp,nChannel + 1, m_stSNList[nChannel].strSN);
		}	
		if (bGetFile)
		{
			if (nTemp == IDX_TEMP_LOW)
			{
				m_pdblWavelengthTemp = 1310;
			}
			else
			{
				m_pdblWavelengthTemp = 1550;
			}
			//COM0
			if (m_bIfSaveToServe)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s.CSV",
					m_strNetFile, m_strPN, m_strSN, m_pdblWavelengthTemp, nChannel + 1, m_stSNList[nChannel].strSN);
			}
			else
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s.CSV",
					m_strLocalPath, m_strPN, m_strSN, m_pdblWavelengthTemp, nChannel + 1, m_stSNList[nChannel].strSN);
			}
			
			fp = fopen(strCSVFile, "r");
			if (fp == NULL)
			{
				strMsg.Format("获取文件%s失败！", strCSVFile);
				LogInfo(strMsg);
				return FALSE;
			}
			fgets(pchRead, 256, fp);
			for (nIndex = 0;nIndex<m_stSNList[nChannel].nCHCount;nIndex++)
			{
				fgets(pchRead, 256, fp);
				strValue = strtok(pchRead, ",");
				strValue = strtok(NULL, ",");
				ScanPowerChannal[nIndex].VoltageX = atoi(strValue);
				strValue = strtok(NULL, ",");
				ScanPowerChannal[nIndex].VoltageY = atoi(strValue);
				strValue = strtok(NULL, ",");
				ScanPowerChannal[nIndex].dblIL = atoi(strValue);
			}
			fclose(fp);
			//COM1
			if (m_stSNList[nChannel].nCOMCount == 2)
			{			
				if (m_bIfSaveToServe)
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s-COM1.CSV",
						m_strNetFile, m_strPN, m_strSN, m_pdblWavelengthTemp,nChannel + 1, m_stSNList[nChannel].strSN);
				}
				else
				{
					strCSVFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s-COM1.CSV",
						m_strLocalPath, m_strPN, m_strSN, m_pdblWavelengthTemp,nChannel + 1, m_stSNList[nChannel].strSN);

				}
				fp = fopen(strCSVFile, "r");
				if (fp == NULL)
				{
					strMsg.Format("获取文件%s失败！", strCSVFile);
					LogInfo(strMsg);
					return TRUE;
				}
				fgets(pchRead, 256, fp);
				for (nIndex = m_stSNList[nChannel].nCHCount;nIndex<m_stSNList[nChannel].nCHCount*2;nIndex++)
				{
					fgets(pchRead, 256, fp);
					strValue = strtok(pchRead, ",");
					strValue = strtok(NULL, ",");
					ScanPowerChannal[nIndex].VoltageX = atoi(strValue);
					strValue = strtok(NULL, ",");
					ScanPowerChannal[nIndex].VoltageY = atoi(strValue);
					strValue = strtok(NULL, ",");
					ScanPowerChannal[nIndex].dblIL = atoi(strValue);
				}
				fclose(fp);
			}
		}
		else
		{
			fp = fopen(strCSVFile, "wt");
			strValue.Format("CH,X-Voltage,Y-Voltage\n");
			fprintf(fp, strValue);

			for (nIndex = 0;nIndex<m_stSNList[nChannel].nCHCount;nIndex++)
			{
				strValue.Format("%d,%d,%d,%.3f\n", nIndex + 1, ScanPowerChannal[nIndex].VoltageX, ScanPowerChannal[nIndex].VoltageY, ScanPowerChannal[nIndex].dblIL);
				fprintf(fp, strValue);
			}
			fclose(fp);
		}	
		break;	
	case IL_ROOM_DATA:
		//选择表单
		strCSVFile.Format("%s\\data\\%s\\%s\\Optical\\%.0f",
			m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength);
		CreateFullPath(strCSVFile);
		if (m_cbComPort.GetCurSel()==0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\Optical\\%.0f\\%d_IL_Room_%s.CSV",
				m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength, nChannel + 1, m_stSNList[nChannel].strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\Optical\\%.0f\\%d_IL_Room_%s.CSV",
				m_strNetFile, m_strPN,m_strSN, m_pdblWavelength, nChannel+1,m_stSNList[nChannel].strSN);
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\Optical\\%.0f\\%d_IL_Room_%s-COM1.CSV",
				m_strLocalPath, m_strPN,m_strSN, m_pdblWavelength, nChannel+1,m_stSNList[nChannel].strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\Optical\\%.0f\\%d_IL_Room_%s-COM1.CSV",
				m_strNetFile, m_strPN,m_strSN, m_pdblWavelengthTemp,nChannel+1, m_stSNList[nChannel].strSN);
		}	
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,IL\n");
		fprintf(fp,strValue);
		if(m_cbComPort.GetCurSel()==1)
		{
			for(nIndex=0;nIndex<1;nIndex++)
			{
				strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblIL);
				fprintf(fp,strValue);
			}
			fclose(fp);
		}
		else
		{
			for(nIndex=0;nIndex<m_stSNList[nChannel].nCHCount;nIndex++)
			{
				strValue.Format("%d,%.2f\n",nIndex+1,ChannalMessage[nIndex].dblIL);
				fprintf(fp,strValue);
			}
			fclose(fp);
		}		
		break;
	case CT_DATA:
		//选择表单
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶前.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶前.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶前COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶前COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶后.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶后.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶后COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\CT_Room-点胶后COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}	
		}
		fp = fopen(strCSVFile,"wt");
		if (m_nCHCount==19)
		{
			strValue.Format("CH,CT1,CT2,CT3,CT4,CT5,CT6,CT7,CT8,CT9,CT10,CT11,CT12,CT13,CT14,CT15,CT16,CT17,CT18,CT19\n");
		    fprintf(fp,strValue);
		}
		else if (m_nCHCount==16)
		{
			strValue.Format("CH,CT1,CT2,CT3,CT4,CT5,CT6,CT7,CT8,CT9,CT10,CT11,CT12,CT13,CT14,CT15,CT16\n");
			fprintf(fp,strValue);
		}
		else
		{
			strValue.Format("CH,CT1,CT2,CT3,CT4,CT5,CT6,CT7,CT8,CT9,CT10,CT11,CT12\n");
			fprintf(fp,strValue);
		}
			
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
	case ADJUST_DATA:
		//选择表单
		if (m_cbComPort.GetCurSel()==0)
		{
			if (m_cbSaveData.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\VoltageAdjust-点胶前.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\VoltageAdjust-点胶前.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if(m_cbSaveData.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\VoltageAdjust-点胶后.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\VoltageAdjust-点胶后.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			
		}
		else if (m_cbComPort.GetCurSel()==1)
		{
			if (m_cbSaveData.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\VoltageAdjust-点胶前COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\VoltageAdjust-点胶前COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if(m_cbSaveData.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\VoltageAdjust-点胶后COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\VoltageAdjust-点胶后COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
		}
		fp = fopen(strCSVFile,"wt");
		strValue.Format("CH,X-Voltage,Y-Voltage,IL\n");
		fprintf(fp,strValue);
		for (nIndex=0;nIndex<4;nIndex++)
		{
			strValue.Format("%d,%d,%d,%.2f\n",nIndex+1,ScanPowerChannal[m_nReadPwrCH[nIndex]].VoltageX,
				ScanPowerChannal[m_nReadPwrCH[nIndex]].VoltageY,ScanPowerChannal[m_nReadPwrCH[nIndex]].MaxPower);
		    fprintf(fp,strValue);
		}
		fclose(fp);
		break;
	case DARK_DATA:

		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶前.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶前.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶前COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶前COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}	
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶后.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶后.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶后COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\DarkIL-点胶后COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}	
		}
		LogInfo(strCSVFile);
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
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶前.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶前.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶前COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶前COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶后.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶后.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶后COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\RepeatIL-点胶后COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}	
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
	case RL_DATA:
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\RL-点胶前.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\RL-点胶前.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\RL-点胶前COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\RL-点胶前COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\RL-点胶后.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\RL-点胶后.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\RL-点胶后COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\RL-点胶后COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}	
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
		if (m_bIfSaveToServe)
		{
			CopyFile(strCSVFile,strNetFile,FALSE);
		}	
		break;
	case ISO_DATA:
		if (m_cbSaveData.GetCurSel()==0)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\ISO-点胶前.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\ISO-点胶前.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\ISO-点胶前COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\ISO-点胶前COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}			
		}
		else if (m_cbSaveData.GetCurSel()==1)
		{
			if (m_cbComPort.GetCurSel()==0)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\ISO-点胶后.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
		    	strNetFile.Format("%s\\data\\%s\\%s\\ISO-点胶后.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}
			else if (m_cbComPort.GetCurSel()==1)
			{
				strCSVFile.Format("%s\\data\\%s\\%s\\ISO-点胶后COM1.CSV",
					m_strLocalPath,m_strItemName,m_strSN);
				strNetFile.Format("%s\\data\\%s\\%s\\ISO-点胶后COM1.CSV",
					m_strNetFile,m_strItemName,m_strSN);
			}	
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
	}

	if (m_bIfSaveToServe)
	{
		CopyFile(strCSVFile,strNetFile,FALSE);	
	}	
	LogInfo("保存数据到CSV结束！");
	return TRUE;

}


void CMy126S_45V_Switch_AppDlg::OnButtonTest() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	CString strToken;
	char pbyData[30];
	char chData[256];
	ZeroMemory(pbyData,sizeof(pbyData));
	ZeroMemory(chData,sizeof(chData));
	double dblValue[64]={0};
	CString strMsg;
	
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	int inPM = m_cbSetPDBox.GetCurSel();
	if (inPM == 0)
	{
		strTemp.Format("get power 0 %d\r\n", m_stSNList[m_nSNIndex].nCHCount);
		memcpy(pbyData, strTemp, strTemp.GetLength());
		if (!m_opTesterCom.WriteBuffer(pbyData, strTemp.GetLength()))
		{
			LogInfo("串口未打开，通道设置错误！");
			return;
		}
		Sleep(50);
		if (!m_opTesterCom.ReadBuffer(chData, 256))
		{
			LogInfo("串口未打开，通道设置错误！");
			return;
		}
		strToken = strtok(chData, "\r\n");
		int i = 1;
		for (i = 1; i <= m_stSNList[m_nSNIndex].nCHCount; i++)
		{

			dblValue[i] = atof(strToken) / 100.0;
			strToken = strtok(NULL, "\r\n");
			if (m_bTesterALL)
			{
				if ((m_cbChannel.GetCurSel()) != (i))
				{
					continue;
				}
			}
			strMsg.Format("通道%d 功率值为%.2f", i, dblValue[i]);
			LogInfo(strMsg);
		}
	}
	if (inPM == 1)
	{
		m_PDBoxCmd.GetPDPower(dblValue);
		for (int i = 0; i < 64; i++)
		{
			strMsg.Format("通道%d 功率值为%.2f", i+1, dblValue[i]);
			LogInfo(strMsg);
		}
		if (!m_CmdInstrument.SetVOAxAtten(1, 0.0))
		{
			return;
		}
	}
	m_bTesterALL = FALSE;
}

void CMy126S_45V_Switch_AppDlg::OnButtonCheck() 
{
	// TODO: Add your control notification handler code here
	
}

/*
void CMy126S_45V_Switch_AppDlg::OnButtonLight() 
{
	// TODO: Add your control notification handler code here
	TCHAR tszCOM[10]={0};
	CString strMsg;
	int nTime;
	int nStep;
	int iLen;
	CString strFileName;
	int nChannel;
	m_nIndex=0;
	m_iLightCount=0;

	nChannel=m_ctrlLightCH.GetCurSel()+1;
	if(nChannel==0)
	{
		MessageBox("请选择通道号！");
		return;
	}
	//打开串口
	UpdateData();
	if(m_nLightCom>9)
	{
		sprintf(tszCOM,"\\\\.\\COM%d",m_nLightCom);
	}
	else
	{
		sprintf(tszCOM,"COM%d",m_nLightCom);
	}
	if (!m_opLightCom.OpenPort(tszCOM,9600))
	{
		strMsg.Format("串口打开错误！");
		LogInfo(strMsg,(BOOL)FALSE,COLOR_RED);
		return;
	}

	//开始照光
	m_ctrlProgress.SetRange(0,50);
	m_ctrlProgress.SetPos(0);

	ConfigLight(nChannel, 5);
	Sleep(500);
	SetTimer(4,1000,NULL);
}
*/
/*
BOOL CMy126S_45V_Switch_AppDlg::StartLight(int nChannel)
{
	CString strTemp;
	char pbyData[100];
	char pbyRx[100];  
	
	ZeroMemory(pbyData,sizeof(char)*100);
	ZeroMemory(pbyRx,sizeof(char)*100);
	CString strMsg;

	strTemp.Format("start %d\r\n",nChannel);
	memcpy(pbyData,strTemp,strTemp.GetLength());
	if(!m_opLightCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{ 
		MessageBox("串口没打开，设置y电压错误！");
		return FALSE;
	}

    return TRUE;
}
*/
/*
BOOL CMy126S_45V_Switch_AppDlg::ConfigLight(int nChannel,int nTime)
{
	CString strTemp;
	char pbyData[100];
	char pbyRx[100];  
	
	ZeroMemory(pbyData,sizeof(char)*100);
	ZeroMemory(pbyRx,sizeof(char)*100);
	CString strMsg;

	strTemp.Format("config %d 100 %d 1\r\n",nChannel,nTime*10);
	memcpy(pbyData,strTemp,strTemp.GetLength());
	if(!m_opLightCom.WriteBuffer(pbyData,strTemp.GetLength()))
	{ 
		MessageBox("串口没打开，设置y电压错误！");
		return FALSE;
	}
	
    return TRUE;
}
*/

BOOL CMy126S_45V_Switch_AppDlg::SetVoltage(int nDAC)
{
	CString strMsg;
	if(!SETX(nDAC))
	{
		strMsg.Format("X电压设置失败");
		LogInfo(strMsg);
		return FALSE;
	}
	if(!SETY(nDAC))
	{
		strMsg.Format("Y电压设置失败");
		LogInfo(strMsg);
		return FALSE;
	}
	strMsg.Format("电压设置为%d",nDAC);
	LogInfo(strMsg);
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::CreateFullPath(LPCSTR lpPath)
{
	CString strTemp;
	strTemp = lpPath;
	if (-1 == GetFileAttributes(strTemp))
	{
		int nPos = strTemp.ReverseFind('\\');
		CreateFullPath(strTemp.Left(nPos));
		CreateDirectory(strTemp, NULL);
	}
	else
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::CreatePath()
{
	CString strPath;
	strPath.Format("%s\\data\\%s\\%s\\PDData\\BIN", m_strLocalPath, m_strPN, m_strSN);
	CreateFullPath(strPath);
	strPath.Format("%s\\data\\%s\\%s\\Cal\\BIN",m_strLocalPath,m_strPN,m_strSN);
	CreateFullPath(strPath);
	strPath.Format("%s\\data\\%s\\%s\\Optical", m_strLocalPath, m_strPN, m_strSN);
	CreateFullPath(strPath);
	strPath.Format("%s\\data\\Ref\\%s",m_strLocalPath,&m_chComputerName);
	CreateFullPath(strPath);
	if (m_bIfSaveToServe)
	{
		strPath.Format("%s\\data\\%s\\%s\\PDData\\BIN", m_strNetFile, m_strPN, m_strSN);
		CreateFullPath(strPath);
		strPath.Format("%s\\data\\%s\\%s\\Cal\\BIN",m_strNetFile, m_strPN,m_strSN);
    	CreateFullPath(strPath);
		strPath.Format("%s\\data\\%s\\%s\\Optical", m_strNetFile, m_strPN, m_strSN);
		CreateFullPath(strPath);
		strPath.Format("%s\\data\\Ref\\%s",m_strNetFile, &m_chComputerName);
    	CreateFullPath(strPath);
	}
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnButtonRlRef() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	UpdateData();

	if (!m_bConnect1830C)
	{
		MessageBox("请先连接设备");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}
	
	int nWL;
	int i=0;
	double dblReadPower;
	double dblMaxPower;
	MessageBox("请绕死光源出光端，并将RL头接入1830C光功率计！");
	LogInfo("开始记录系统回损,请等待...");
	
	dblMaxPower = -1000;
	for (i=0;i<3;i++)
	{
		if(g_pPM->GetPower(0,&dblReadPower))
		{
			MessageBox("读取光功率计错误！");
			return;
		}
		Sleep(100);
		if (dblReadPower>dblMaxPower) //
		{
			dblMaxPower = dblReadPower;
		}
	}
	m_dblSystemRL = m_dblReferencePower[0] - dblMaxPower;
	strMsg.Format("系统归零RL为%.2fdB",m_dblSystemRL);
    LogInfo(strMsg);
	
	m_bRefRL = TRUE;
	MessageBox("RL系统归零完毕");
}

BOOL CMy126S_45V_Switch_AppDlg::OpenPMDevice()
{
	CString strConfigFile;
	CString strMsg;
	double  dblValue;
	strConfigFile.Format("%s\\config\\WorkStation\\%s\\UDL.xml",
		m_strWorkStationPath,m_chComputerName);
	LogInfo(strConfigFile);

	//增加Golden Sample测试判断

	//try
	{
		::CoInitialize(NULL);
		//HRESULT hr 	=m_pTLS.CreateInstance( _T("UDL.DLM.ITLSInterface"));//启动组件

		HRESULT hr = g_pEngine.CreateInstance(__uuidof(EngineMgr));
		if (FAILED(hr))
		{
			_com_error e(hr);
			AfxMessageBox(e.ErrorMessage());
			return FALSE;
		}
		ASSERT( SUCCEEDED( hr ) );
		g_pEngine->RegisterUDL();

		ASSERT( SUCCEEDED( hr ) );
		hr 	=g_pPM.CreateInstance(__uuidof(IPMInterface));
		ASSERT( SUCCEEDED( hr ) );//启动组件

		LogInfo("开始加载配置");
		if(g_pEngine->LoadConfiguration((_bstr_t)strConfigFile))
		{
			AfxMessageBox(g_pEngine->GetGetLastMessage());
			return FALSE;
		}
		
		LogInfo("开始打开设备");
		if(g_pEngine->OpenEngine())
		{
			AfxMessageBox(g_pEngine->GetGetLastMessage());
			return FALSE;
		}

		if(g_pPM->SetUnit(0,0))
		{
			AfxMessageBox(g_pEngine->GetGetLastMessage());
			return FALSE;
		}
		if(g_pPM->SetRangeMode(0,1))
		{
			AfxMessageBox(g_pEngine->GetGetLastMessage());
			return FALSE;
		}
		if(g_pPM->SetWavelength(0,m_pdblWavelength))
		{
			AfxMessageBox(g_pEngine->GetGetLastMessage());
			return FALSE;
		}
		strMsg.Format("波长设置为:%.2fnm",m_pdblWavelength);
		LogInfo(strMsg);
		
		if(g_pPM->GetPower(0,&dblValue))
		{
			AfxMessageBox(g_pEngine->GetGetLastMessage());
			return FALSE;
		}
		strMsg.Format("光功率值为:%.2fdBm",dblValue);
		LogInfo(strMsg);
	}
	/*
	catch () {
	}
	*/
	
	return TRUE;
}

BOOL CMy126S_45V_Switch_AppDlg::GetConfigFile()
{
	//获得网络数据存储路径
	CString strFileName;
	CString strMsg;
	CString strValue;
	strFileName.Format("%s\\config\\DataSave.ini",g_tszAppFolder);
	int iLen = GetPrivateProfileString("ServeAddress","Address",NULL,m_strNetFile.GetBuffer(256),256,strFileName);
    if (iLen <= 0)
    {
		MessageBox("读取配置文件DataSave.ini失败");
		return FALSE;
    }

	GetPrivateProfileString("Config","Address","ERROR",strValue.GetBuffer(256),256,strFileName);
	if (strValue=="ERROR")
	{
		m_strConfigPath = g_tszAppFolder;
	}
	else
	{
		m_strConfigPath = strValue;
	}
	strMsg.Format("配置文件路径为:%s",m_strConfigPath);
	//LogInfo(strMsg);

	GetPrivateProfileString("LocalPath","Address","ERROR",strValue.GetBuffer(256),256,strFileName);
	if (strValue=="ERROR")
	{
		m_strLocalPath = g_tszAppFolder;
	}
	else
	{
		m_strLocalPath = strValue;
	}
	strMsg.Format("本地存储路径为:%s",m_strLocalPath);
	//LogInfo(strMsg);

	GetPrivateProfileString("Workstation","Address","ERROR",strValue.GetBuffer(256),256,strFileName);
	if (strValue=="ERROR")
	{
		m_strWorkStationPath = g_tszAppFolder;
	}
	else
	{
		m_strWorkStationPath = strValue;
	}
	strMsg.Format("工位配置路径为:%s",m_strWorkStationPath);
	//LogInfo(strMsg);

	GetPrivateProfileString("GetVoltageFile", "Value", "ERROR", strValue.GetBuffer(256), 256, strFileName);
	if (strValue == "ERROR")
	{
		m_strVoltagePath = g_tszAppFolder;
	}
	else
	{
		m_strVoltagePath = strValue;
	}
	strMsg.Format("工位配置路径为:%s", m_strWorkStationPath);
	return TRUE;

}

void CMy126S_45V_Switch_AppDlg::OnButtonScanCh() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	CString strTemp1;
	CString strChannel;
	CString strMsg;
	double  dblPref;
	double  dblPout;
	double  dblPower;
	char    strCT[32];
	char    strIL[32];
	int nchannel;

	UpdateData();
	m_bTestStop = FALSE;
	m_bReadPowerStop = TRUE;
	KillTimer(3);

	ZeroMemory(strCT,sizeof(strCT));
	ZeroMemory(strIL,sizeof(strIL));

	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return ;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return ;
	}
	if (m_cbChannel.GetCurSel()==0)
	{
		MessageBox("请选择正确的通道");
		return;
	}
	
	GetLocalTime(&m_stStartTime);
	strTemp.Format("开始扫描!");
    LogInfo(strTemp,FALSE);

	m_bReadPowerStop = TRUE;

	CString strFileINI;
	if (m_cbComPort.GetCurSel()==0)
	{
		strFileINI = "ScanAdjustRange.ini";
	}
	else if (m_cbComPort.GetCurSel()==1)
	{
		strFileINI = "ScanAdjustRange-com1.ini";
	}
	int i=m_cbChannel.GetCurSel()-1;
	if (i < m_nCHCount) //正常范围通道
	{
		if (m_nCHCount == 16 && m_nSwitchComPortCount == 2)
		{
			if (!AutoScan4x3(i, strFileINI, TRUE))
			{
				m_bAdjustScan = FALSE;
				LogInfo("该通道扫描出错！", FALSE, COLOR_RED);
				return;
			}
		}
		else if (m_nCHCount == 19)
		{
			if (!AutoScan4x3(i, strFileINI, TRUE))
			{
				strMsg.Format("粗扫通道%d失败！", i + 1);
				LogInfo(strMsg);
				return;
			}
		}
		else
		{
			if (!AutoScan4x3(i, strFileINI, TRUE))
			{
				strMsg.Format("粗扫通道%d失败！", i + 1);
				LogInfo(strMsg);
				return;
			}
		}
	}
	else //反向扫描范围
	{
		nchannel = i - m_nCHCount;
		strMsg.Format("请将入光接CH%d,回损头接光功率计通道%d", nchannel+m_stReCHConfig.nStartIndex, m_stReCHConfig.nPMIndex);
		MessageBox(strMsg);
		ScanReCH(nchannel);
	}

	

}

BOOL CMy126S_45V_Switch_AppDlg::GetParaRangeFromATMS()
{
	int nNodeCount;
	CString strErrMsg;
	CString strMsg;
	DWORD dwBuf[256];
	DWORD dwWDMTemp;
	DWORD dwTmptBuf[256];
	int nWDMNodeCount;
	CString strTemplate;
	CString strURL;
	int     nRand = rand()%256;
	
	strTemplate.Format("%s\\template\\Test_temp.xml",g_tszAppFolder);
	if(m_stWorkStation.strGoldenSampleName=="ERROR")
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&randno=%d&ADJUST=1",m_strSN,nRand);
	}
	else
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&randno=%d&ADJUST=1&WorkStation=%s",m_strSN,nRand, m_stWorkStation.strGoldenSampleName);
	}
	strMsg.Format("开始获取无纸化模板:%s",strURL);
	LogInfo(strMsg);

	if (!m_ctrXMLCtr.GetHtmlSource(strURL, strTemplate))
	{
		strMsg.Format("加载无纸化模板:%s错误",strURL);
		LogInfo(strMsg);
		return FALSE;
	}
	m_ctrXMLCtr.LoadXmlFile(strTemplate);
	
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Error", dwBuf, nNodeCount);
		if (nNodeCount != 0)
		{
			m_ctrXMLCtr.GetNodeAttr("NodeValue", strMsg, dwBuf[0]);
			LogInfo(strMsg, FALSE,COLOR_RED);
			return FALSE;
		}
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
		LogInfo(strMsg);
 		return FALSE;
 	}
	dwWDMTemp = dwBuf[0];
	ZeroMemory(dwTmptBuf,sizeof(dwTmptBuf));
	
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwTmptBuf, nWDMNodeCount,dwWDMTemp);	
	if (nWDMNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg);
		//m_pNetCommand->XmlSendLog(strMsg , strMsg.GetLength()+1,COLOR_RED);
		return FALSE;
	}
	int nTemp=0;
	int nPort;
	int nBlock=0;
	int nAtten;
	int nIndex;
	int nCom;
	CString strContent;
	CString strTemp;
	DWORD dwBufCH[256];
	CString strKey;
	int nWL;
	int nNode = 0;
	for ( nNode=0;nNode<nWDMNodeCount;nNode++)
	{
		dwWDMTemp = dwTmptBuf[nNode];
		m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值

		if (stricmp(strTemp,"room")==0)
		{
			nTemp = 0;
		}
		else if (stricmp(strTemp,"low")==0) 
		{
			nTemp = 1;
		}
		else if (stricmp(strTemp,"high")==0)
		{
			nTemp = 2;
		}
		
		strContent.Format("Voltage");	
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount,dwWDMTemp);
		if (nNodeCount==0) 
		{
			strMsg.Format("没有找到结点%s-%s,使用新模板获取参数",strTemp,strContent);
			LogInfo(strMsg);
			//CAL
			for(nPort=0;nPort<m_nCHCount;nPort++)
			{
				if(fabs(m_pdblWavelength-(int)m_pdblWavelength)>0.0001)
				{
					strContent.Format("SWITCH_CAL-PORT%d_WL%.2f",nPort+1,m_pdblWavelength);
				}
				else
				{
					strContent.Format("SWITCH_CAL-PORT%d_WL%.0f",nPort+1,m_pdblWavelength);
				}
				
			    ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strContent, dwBufCH, nNodeCount,dwWDMTemp);
				if (nNodeCount==0) 
				{
					strMsg.Format("没有找到新模板结点%s",strContent);
					LogInfo(strMsg);
					break;
				}
				else
				{
					strKey.Format("VOLTAGE_X");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount!=0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMinXVoltage[nPort] = atof(strTemp);
					
						m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMaxXVoltage[nPort] = atof(strTemp);

						strMsg.Format("通道%dX电压范围：%.2f~%.2f",nPort+1,m_stLimitPara[0].dblMinXVoltage[nPort],
						m_stLimitPara[0].dblMaxXVoltage[nPort]);
				    	LogInfo(strMsg);
						
					}
					strKey.Format("VOLTAGE_Y");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount!=0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMinYVoltage[nPort] = atof(strTemp);
						
						m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMaxYVoltage[nPort] = atof(strTemp);

						strMsg.Format("通道%dY电压范围：%.2f~%.2f",nPort+1,m_stLimitPara[0].dblMinYVoltage[nPort],
						m_stLimitPara[0].dblMaxYVoltage[nPort]);
				    	LogInfo(strMsg);
					}	
				}
			}
			//OPTICAL
			//CAL
			for (nPort = 0;nPort<m_nCHCount;nPort++)
			{
				if (fabs(m_pdblWavelength - (int)m_pdblWavelength)>0.0001)
				{
					strContent.Format("SWITCH_OPTICAL-PORT%d_WL%.2f", nPort + 1, m_pdblWavelength);
				}
				else
				{
					strContent.Format("SWITCH_OPTICAL-PORT%d_WL%.0f", nPort + 1, m_pdblWavelength);
				}

				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount, dwWDMTemp);
				if (nNodeCount == 0)
				{
					strMsg.Format("没有找到新模板结点%s", strContent);
					LogInfo(strMsg);
					break;
				}
				else
				{
					strKey.Format("REPEAT_IL");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
					if (nNodeCount != 0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMinRepeatIL[nPort] = atof(strTemp);

						m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMaxRepeatIL[nPort] = atof(strTemp);
						strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[0].dblMinRepeatIL[nPort],
							m_stLimitPara[0].dblMaxRepeatIL[nPort]);
						LogInfo(strMsg);

					}

					strKey.Format("RL");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
					if (nNodeCount != 0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMinRL[nPort] = atof(strTemp);

						m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMaxRL[nPort] = atof(strTemp);

						strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[0].dblMinRL[nPort],
							m_stLimitPara[0].dblMaxRL[nPort]);
						LogInfo(strMsg);
					}
					strKey.Format("IL");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
					if (nNodeCount != 0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMinRoomIL[nPort] = atof(strTemp);

						m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMaxRoomIL[nPort] = atof(strTemp);

						strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[0].dblMinRoomIL[nPort],
							m_stLimitPara[0].dblMaxRoomIL[nPort]);
						LogInfo(strMsg);
					}
					strKey.Format("MIN_CT");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
					if (nNodeCount != 0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMinCT[nPort] = atof(strTemp);

						m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMaxCT[nPort] = atof(strTemp);

						strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[0].dblMinCT[nPort],
							m_stLimitPara[0].dblMaxCT[nPort]);
						LogInfo(strMsg);
					}
					strKey.Format("DARK_IL");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
					if (nNodeCount != 0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMinDarkIL[nPort] = atof(strTemp);

						m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMaxDarkIL[nPort] = atof(strTemp);

						strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[0].dblMinDarkIL[nPort],
							m_stLimitPara[0].dblMaxDarkIL[nPort]);
						LogInfo(strMsg);
					}
					strKey.Format("ISO");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
					if (nNodeCount != 0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMaxISO[nPort] = atof(strTemp);

						m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
						m_stLimitPara[0].dblMinISO[nPort] = atof(strTemp);

						strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[0].dblMinISO[nPort],
							m_stLimitPara[0].dblMaxISO[nPort]);
						LogInfo(strMsg);
					}
				}
			}
			if (m_nSwitchComPortCount >= 2)
			{
				for (nCom = 1;nCom < m_nSwitchComPortCount;nCom++)
				{
					//CAL
					for (nPort = 0;nPort<m_nCHCount;nPort++)
					{
						if (fabs(m_pdblWavelength - (int)m_pdblWavelength)>0.0001)
						{
							strContent.Format("SWITCH_CAL_COM%d-PORT%d_WL%.2f", nCom,nPort + 1, m_pdblWavelength);
						}
						else
						{
							strContent.Format("SWITCH_CAL_COM%d-PORT%d_WL%.0f", nCom, nPort + 1, m_pdblWavelength);
						}

						ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount, dwWDMTemp);
						if (nNodeCount == 0)
						{
							strMsg.Format("没有找到新模板结点%s", strContent);
							LogInfo(strMsg);
							break;
						}
						else
						{
							strKey.Format("VOLTAGE_X");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
							if (nNodeCount != 0)
							{
								m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMinXVoltage[nPort] = atof(strTemp);

								m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMaxXVoltage[nPort] = atof(strTemp);

								strMsg.Format("通道%dX电压范围：%.2f~%.2f", nPort + 1, m_stLimitPara[nCom].dblMinXVoltage[nPort],
									m_stLimitPara[nCom].dblMaxXVoltage[nPort]);
								LogInfo(strMsg);

							}
							strKey.Format("VOLTAGE_Y");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
							if (nNodeCount != 0)
							{
								m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMinYVoltage[nPort] = atof(strTemp);

								m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMaxYVoltage[nPort] = atof(strTemp);

								strMsg.Format("通道%dY电压范围：%.2f~%.2f", nPort + 1, m_stLimitPara[nCom].dblMinYVoltage[nPort],
									m_stLimitPara[nCom].dblMaxYVoltage[nPort]);
								LogInfo(strMsg);
							}
						}
					}
					//OPTICAL
					for (nPort = 0;nPort<m_nCHCount;nPort++)
					{
						if (fabs(m_pdblWavelength - (int)m_pdblWavelength)>0.0001)
						{
							strContent.Format("SWITCH_OPTICAL_COM%d-PORT%d_WL%.2f", nCom, nPort + 1, m_pdblWavelength);
						}
						else
						{
							strContent.Format("SWITCH_OPTICAL_COM%d-PORT%d_WL%.0f", nCom, nPort + 1, m_pdblWavelength);
						}

						ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount, dwWDMTemp);
						if (nNodeCount == 0)
						{
							strMsg.Format("没有找到新模板结点%s", strContent);
							LogInfo(strMsg);
							break;
						}
						else
						{
							strKey.Format("REPEAT_IL");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
							if (nNodeCount != 0)
							{
								m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMinRepeatIL[nPort] = atof(strTemp);

								m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMaxRepeatIL[nPort] = atof(strTemp);
								strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[nCom].dblMinRepeatIL[nPort],
									m_stLimitPara[nCom].dblMaxRepeatIL[nPort]);
								LogInfo(strMsg);

							}

							strKey.Format("RL");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
							if (nNodeCount != 0)
							{
								m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMinRL[nPort] = atof(strTemp);

								m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMaxRL[nPort] = atof(strTemp);

								strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[nCom].dblMinRL[nPort],
									m_stLimitPara[nCom].dblMaxRL[nPort]);
								LogInfo(strMsg);
							}
							strKey.Format("IL");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
							if (nNodeCount != 0)
							{
								m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMinRoomIL[nPort] = atof(strTemp);

								m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMaxRoomIL[nPort] = atof(strTemp);

								strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[nCom].dblMinRoomIL[nPort],
									m_stLimitPara[nCom].dblMaxRoomIL[nPort]);
								LogInfo(strMsg);
							}
							strKey.Format("MIN_CT");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
							if (nNodeCount != 0)
							{
								m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMinCT[nPort] = atof(strTemp);

								m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMaxCT[nPort] = atof(strTemp);

								strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[nCom].dblMinCT[nPort],
									m_stLimitPara[nCom].dblMaxCT[nPort]);
								LogInfo(strMsg);
							}
							strKey.Format("DARK_IL");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
							if (nNodeCount != 0)
							{
								m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMinDarkIL[nPort] = atof(strTemp);

								m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMaxDarkIL[nPort] = atof(strTemp);

								strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[nCom].dblMinDarkIL[nPort],
									m_stLimitPara[nCom].dblMaxDarkIL[nPort]);
								LogInfo(strMsg);
							}
							strKey.Format("ISO");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount, dwBufCH[0]);
							if (nNodeCount != 0)
							{
								m_ctrXMLCtr.GetNodeAttr("MIN", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMaxISO[nPort] = atof(strTemp);

								m_ctrXMLCtr.GetNodeAttr("MAX", strTemp, dwBuf[0]);
								m_stLimitPara[nCom].dblMinISO[nPort] = atof(strTemp);

								strMsg.Format("通道%d%s范围：%.2f~%.2f", nPort + 1, strKey, m_stLimitPara[nCom].dblMinISO[nPort],
									m_stLimitPara[nCom].dblMaxISO[nPort]);
								LogInfo(strMsg);
							}
						}
					}
				}
			}			
		}
		else
		{
			ZeroMemory(dwBuf,sizeof(dwBuf));
			for(nPort=0;nPort<m_nCHCount;nPort++)
			{
				strKey.Format("CH%d_X",nPort+1);
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
				if (nNodeCount!=0)
				{
					m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
					m_stLimitPara[0].dblMinXVoltage[nPort] = atof(strTemp);
					//m_stMinLimit[nTemp].nVoltageX[nPort] = atoi(strTemp);
					m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
					m_stLimitPara[0].dblMaxXVoltage[nPort] = atof(strTemp);
					strMsg.Format("通道%dX电压范围：%.2f~%.2f",nPort+1,m_stLimitPara[0].dblMinXVoltage[nPort],
						m_stLimitPara[0].dblMaxXVoltage[nPort]);
					LogInfo(strMsg);
					//m_stMaxLimit[nTemp].nVoltageX[nPort] = atoi(strTemp);
				}
				strKey.Format("CH%d_Y",nPort+1);
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
				if (nNodeCount!=0)
				{
					m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
					m_stLimitPara[0].dblMinYVoltage[nPort] = atof(strTemp);
					//m_stMinLimit[nTemp].nVoltageY[nPort] = atoi(strTemp);
					m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
					m_stLimitPara[0].dblMaxYVoltage[nPort] = atof(strTemp);
					//m_stMaxLimit[nTemp].nVoltageY[nPort] = atoi(strTemp);
				}
			}

			strContent.Format("IL");	
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount,dwWDMTemp);
			if (nNodeCount!=0)
			{
				for(nPort=0;nPort<m_nCHCount;nPort++)
				{
					strKey.Format("CH%d",nPort+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount!=0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMinRoomIL[nPort] = atof(strTemp);
						m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMaxRoomIL[nPort] = atof(strTemp);
					}
				}
			}

			strContent.Format("RepeatIL");	
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount,dwWDMTemp);
			if (nNodeCount!=0)
			{
				for(nPort=0;nPort<m_nCHCount;nPort++)
				{
					strKey.Format("CH%d",nPort+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount!=0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMinRepeatIL[nPort] = atof(strTemp);
						m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMaxRepeatIL[nPort] = atof(strTemp);
					}
				}
			}

			strContent.Format("DarkIL");	
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount,dwWDMTemp);
			if (nNodeCount!=0)
			{
				for(nPort=0;nPort<m_nCHCount;nPort++)
				{
					strKey.Format("CH%d",nPort+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount!=0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMinDarkIL[nPort] = atof(strTemp);
						m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMaxDarkIL[nPort] = atof(strTemp);
					}
				}
			}

			strContent.Format("RL");	
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount,dwWDMTemp);
			if (nNodeCount!=0)
			{
				for(nPort=0;nPort<m_nCHCount;nPort++)
				{
					strKey.Format("CH%d",nPort+1);
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount!=0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMinRL[nPort] = atof(strTemp);
						m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMaxRL[nPort] = atof(strTemp);
					}
				}
			}
			//CT
			for(nPort=0;nPort<m_nCHCount;nPort++)
			{
				strContent.Format("CT%d",nPort+1);	
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strContent, dwBufCH, nNodeCount,dwWDMTemp);
				if (nNodeCount!=0)
				{	
					if(nPort==0)
					{
						strKey.Format("CH%d",2);
					}
					else
					{
						strKey.Format("CH%d",1);
					}
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBuf, nNodeCount,dwBufCH[0]);
					if (nNodeCount!=0)
					{
						m_ctrXMLCtr.GetNodeAttr("MIN",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMinCT[nPort] = atof(strTemp);
						m_ctrXMLCtr.GetNodeAttr("MAX",strTemp,dwBuf[0]);
						m_stLimitPara[0].dblMaxCT[nPort] = atof(strTemp);
					}
				}
			}
		}						
	}
	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "XML关闭失败,请通知相关工程师!";
		LogInfo(strMsg);
		//m_pNetCommand->XmlSendLog(strMsg , strMsg.GetLength()+1,COLOR_RED);
		return FALSE;
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::UpdateNewATMData()
{
	//将数据写入无纸化模板中
	CString strURL;
	CString strTemplate;
	CString strMsg;
	DWORD   dwBuf[256];
	DWORD   dwTmptBuf[256];
	DWORD   dwBufCH[256];
	DWORD   dwWDMTemp;
	int     nNodeCount;
	CString strTemp;
	CString strKey;
	CString strErrMsg;
	CString strNetFile;
	CString strLocalFile;
	SYSTEMTIME st;
	CString strValue;
	BOOL    bPass = TRUE;
	int     iSel;

    //////////////////////////////////////////////////////
	int     nRand = rand();
	strTemplate.Format("%s\\template\\Test_temp.xml",g_tszAppFolder);
	strNetFile.Format("%s\\data\\%s\\%s\\%s_Result.xml",m_strNetFile,m_strItemName,m_strSN,m_strSN);
	strLocalFile.Format("%s\\data\\%s\\%s\\%s_Result.xml",m_strLocalPath,m_strItemName,m_strSN,m_strSN);
    
	strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&adjust=1&randno=%d",
	m_strSN,nRand);
	
	if (m_ctrXMLCtr.GetHtmlSource(strURL, strTemplate))
	{
		m_ctrXMLCtr.LoadXmlFile(strTemplate);
	}
	else
	{
		strMsg.Format("%s模板下载错误!",strTemplate);
		LogInfo(strMsg);
		return FALSE;
	}
	//增加上传测试信息
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
    m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "USER", dwBuf, nNodeCount);
    if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,模板下载失败";
		LogInfo(strMsg);
		return FALSE;
	}		
	m_ctrXMLCtr.ModifyNode("NodeValue",m_strID,dwBuf[0]);

	//测试类型
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
    m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SOFTWARE", dwBuf, nNodeCount);
    if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,SOFTWARE不符合!";
		LogInfo(strMsg);
		return FALSE;
	}	

	strTemp = "1830-A"; //
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "COMPUTER", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的工位不符合!";
		LogInfo(strMsg);
		return FALSE;
	}
	strTemp.Format("%s",&m_chComputerName);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);

	//获取开始时间
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "START", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的开始时间不符合!";
		LogInfo(strMsg);
		return FALSE;
	}
	strTemp.Format("%d-%02d-%02d %02d:%02d:%02d",m_stStartTime.wYear,m_stStartTime.wMonth,m_stStartTime.wDay,m_stStartTime.wHour,m_stStartTime.wMinute,m_stStartTime.wSecond);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);//add end

	//获取结束时间
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "DATE", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的结束时间不符合!";
		LogInfo(strMsg);
		return FALSE;
	}
	GetLocalTime(&st);
	strTemp.Format("%d-%02d-%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	m_ctrXMLCtr.ModifyNode("NodeValue",strTemp,dwBuf[0]);//add end

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的SN号!";
		LogInfo(strMsg);
		return FALSE;
	}	
	m_ctrXMLCtr.ModifyNode("NodeValue",m_strSN,dwBuf[0]);
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	if (strTemp != m_strSN) 
	{
		strMsg = "SN号不对应,请通知相关工程师!";
		LogInfo(strMsg);
		return FALSE;
	}

	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的PN与无纸PN不符合!";
		LogInfo(strMsg);
		return FALSE;
	}
	m_ctrXMLCtr.ModifyNode("NodeValue",m_strPN,dwBuf[0]);
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	if (strTemp != m_strPN) 
	{
		strMsg = "测试产品的PN号与无纸化中PN不对应,请通知相关工程师!";
		LogInfo(strMsg);
		return FALSE;
	}//add end

	ZeroMemory(dwBuf, sizeof(dwBuf));
	nNodeCount = 0;
 	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "WDM", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
 		strMsg = "XML文件结点不完整,无纸化中无法WDM项目!";
 		LogInfo(strMsg);
 		return FALSE;
 	}
	int nWDMNodeCount;
	dwWDMTemp = dwBuf[0];
	ZeroMemory(dwTmptBuf,sizeof(dwTmptBuf));
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Temperature", dwTmptBuf, nWDMNodeCount,dwWDMTemp);	
	if (nWDMNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,无纸化中无法找到对应的温度项目!";
		LogInfo(strMsg);
		return FALSE;
	}
	int nTemp=0;
	int nCH;
	int nKeyCount;
	int nKey;
	CString strNode;
	CString strContent;
	int nWL;
	int nNode=0;
	int nWDL;
	int nPara;
	int nCom;
	//获取电压差异
	for(nCH=0;nCH<(m_nCHCount-1);nCH++)
	{
		ChannalMessage[nCH].nVoltDiffX = ScanPowerChannal[nCH+1].VoltageX - ScanPowerChannal[nCH].VoltageX;
		ChannalMessage[nCH].nVoltDiffY = ScanPowerChannal[nCH+1].VoltageY - ScanPowerChannal[nCH].VoltageY;
	}
	ChannalMessage[m_nCHCount - 1].nVoltDiffX = 0;
	ChannalMessage[m_nCHCount - 1].nVoltDiffY = 0;
	
	for (nNode=0;nNode<nWDMNodeCount;nNode++)
	{
		dwWDMTemp = dwTmptBuf[nNode];
		m_ctrXMLCtr.GetNodeAttr("value",strTemp,dwWDMTemp); //获取温度值
		if (stricmp(strTemp,"Room")==0) //常温
		{
			nTemp = 0;
		}
		else if (stricmp(strTemp,"low")==0) 
		{
			nTemp = 1;
		}
		else if (stricmp(strTemp,"high")==0)
		{
			nTemp = 2;
		}
		
		for(nCH=0;nCH<m_nCHCount;nCH++)
		{		
			if (fabs(m_pdblWavelength-(int)m_pdblWavelength)>DOUBLE_RANGE) //表示不是整数
			{
				strKey.Format("SWITCH_CAL-PORT%d_WL%.2f",nCH+1,m_pdblWavelength);
			}
			else
			{
				strKey.Format("SWITCH_CAL-PORT%d_WL%.0f",nCH+1,m_pdblWavelength);
			}
			
			ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
			m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strKey, dwBufCH, nKeyCount,dwWDMTemp);
			if (nKeyCount==0) 
			{
				strMsg.Format("没有找到结点%s-%s",strTemp,strKey);
				//LogInfo(strMsg);
				break;
			}
			for (nKey=0;nKey<nKeyCount;nKey++)
			{
				strNode.Format("VOLTAGE_X");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
				if (nNodeCount!=0) 
				{
					strValue.Format("%d",ScanPowerChannal[nCH].VoltageX);
			    	m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[nKey]);
				}
				
				strNode.Format("VOLTAGE_Y");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
				if (nNodeCount!=0) 
				{
					strValue.Format("%d",ScanPowerChannal[nCH].VoltageY);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[nKey]);
				}

				strNode.Format("VOLTAGE_X_DIFF");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
				if (nNodeCount!=0) 
				{
					strValue.Format("%d",ChannalMessage[nCH].nVoltDiffX);
			    	m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[nKey]);
				}
				
				strNode.Format("VOLTAGE_Y_DIFF");
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
				if (nNodeCount!=0) 
				{
					strValue.Format("%d",ChannalMessage[nCH].nVoltDiffY);
					m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[nKey]);
				}					
			}
											
		}
								
		for (nWL=0;nWL<1;nWL++) 
		{	
			for(nCH=0;nCH<m_nCHCount;nCH++)
			{		
				if (fabs(m_pdblWavelength-(int)m_pdblWavelength)>DOUBLE_RANGE) //表示不是整数
				{
					strKey.Format("SWITCH_OPTICAL-PORT%d_WL%.2f",nCH+1,m_pdblWavelength);
				}
				else
				{
					strKey.Format("SWITCH_OPTICAL-PORT%d_WL%.0f",nCH+1,m_pdblWavelength);
				}
				
				ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
				m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strKey, dwBufCH, nKeyCount,dwWDMTemp);
				if (nKeyCount==0) 
				{
					strMsg.Format("没有找到结点%s-%s",strTemp,strKey);
					//LogInfo(strMsg);
					break;
				}
				for (nKey=0;nKey<nKeyCount;nKey++)
				{

					{
						strNode.Format("IL");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
						if (nNodeCount!=0) 
						{
							strValue.Format("%.2f",ChannalMessage[nCH].dblIL);
			    			m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[0]);
						}
					}

					{
						strNode.Format("RL");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
						if (nNodeCount!=0) 
						{
							strValue.Format("%.1f",ChannalMessage[nCH].dblRL);
			    			m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[nKey]);
						}
					}
					
					{
						strNode.Format("REPEAT_IL");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
						if (nNodeCount!=0) 
						{
							strValue.Format("%.2f",ChannalMessage[nCH].dblDif);
			    			m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[nKey]);
						}
					}
	
					{
						strNode.Format("MIN_CT");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
						if (nNodeCount!=0) 
						{
							strValue.Format("%.1f",ChannalMessage[nCH].dblMinCT);
			    			m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[nKey]);
						}
					}
					{
						strNode.Format("DARK_IL");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName",strNode, dwBuf, nNodeCount,dwBufCH[nKey]);
						if (nNodeCount!=0) 
						{
							strValue.Format("%.1f",ChannalMessage[nCH].dblDarkIL);
			    			m_ctrXMLCtr.ModifyNode("NodeValue",strValue,dwBuf[nKey]);
						}
					}

					strNode.Format("ISO");
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
					if (nNodeCount != 0)
					{
						strValue.Format("%.1f", ChannalMessage[nCH].dblISO);
						m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
					}
																							
				}				
			}								
		}	
		if (m_nSwitchComPortCount >= 2)
		{
			for (nCom = 1;nCom < m_nSwitchComPortCount;nCom++)
			{
				for (nCH = 0;nCH<m_nCHCount;nCH++)
				{
					if (fabs(m_pdblWavelength - (int)m_pdblWavelength)>DOUBLE_RANGE) //表示不是整数
					{
						strKey.Format("SWITCH_CAL_COM%d-PORT%d_WL%.2f", nCom,nCH + 1, m_pdblWavelength);
					}
					else
					{
						strKey.Format("SWITCH_CAL_COM%d-PORT%d_WL%.0f", nCom, nCH + 1, m_pdblWavelength);
					}

					ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
					m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBufCH, nKeyCount, dwWDMTemp);
					if (nKeyCount == 0)
					{
						strMsg.Format("没有找到结点%s-%s", strTemp, strKey);
						//LogInfo(strMsg);
						break;
					}
					for (nKey = 0;nKey<nKeyCount;nKey++)
					{
						strNode.Format("VOLTAGE_X");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
						if (nNodeCount != 0)
						{
							strValue.Format("%d", ScanPowerChannal[nCH].VoltageXCOM[nCom]);
							m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
						}

						strNode.Format("VOLTAGE_Y");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
						if (nNodeCount != 0)
						{
							strValue.Format("%d", ScanPowerChannal[nCH].VoltageYCOM[nCom]);
							m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
						}
						strNode.Format("VOLTAGE_X_DIFF");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
						if (nNodeCount != 0)
						{
							strValue.Format("%d", ChannalMessage[nCH].nVoltDiffXCOM[nCom]);
							m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
						}

						strNode.Format("VOLTAGE_Y_DIFF");
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
						if (nNodeCount != 0)
						{
							strValue.Format("%d", ChannalMessage[nCH].nVoltDiffYCOM[nCom]);
							m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
						}
					}

				}

				for (nWL = 0;nWL<1;nWL++)
				{
					for (nCH = 0;nCH<m_nCHCount;nCH++)
					{
						if (fabs(m_pdblWavelength - (int)m_pdblWavelength)>DOUBLE_RANGE) //表示不是整数
						{
							strKey.Format("SWITCH_OPTICAL_COM%d-PORT%d_WL%.2f", nCom,nCH + 1, m_pdblWavelength);
						}
						else
						{
							strKey.Format("SWITCH_OPTICAL_COM%d-PORT%d_WL%.0f", nCom, nCH + 1, m_pdblWavelength);
						}

						ZeroMemory(dwBufCH, 256 * sizeof(DWORD));
						m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strKey, dwBufCH, nKeyCount, dwWDMTemp);
						if (nKeyCount == 0)
						{
							strMsg.Format("没有找到结点%s-%s", strTemp, strKey);
							//LogInfo(strMsg);
							break;
						}
						for (nKey = 0;nKey<nKeyCount;nKey++)
						{

							{
								strNode.Format("IL");
								m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
								if (nNodeCount != 0)
								{
									strValue.Format("%.2f", ChannalMessage[nCH].dblILCOM[nCom]);
									m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[0]);
								}
							}

							{
								strNode.Format("RL");
								m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
								if (nNodeCount != 0)
								{
									strValue.Format("%.1f", ChannalMessage[nCH].dblRLCOM[nCom]);
									m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
								}
							}

							{
								strNode.Format("REPEAT_IL");
								m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
								if (nNodeCount != 0)
								{
									strValue.Format("%.2f", ChannalMessage[nCH].dblDifCOM[nCom]);
									m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
								}
							}

							{
								strNode.Format("MIN_CT");
								m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
								if (nNodeCount != 0)
								{
									strValue.Format("%.1f", ChannalMessage[nCH].dblMinCTCOM[nCom]);
									m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
								}
							}
							{
								strNode.Format("DARK_IL");
								m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
								if (nNodeCount != 0)
								{
									strValue.Format("%.1f", ChannalMessage[nCH].dblDarkILCOM[nCom]);
									m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
								}
							}

							strNode.Format("ISO");
							m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", strNode, dwBuf, nNodeCount, dwBufCH[nKey]);
							if (nNodeCount != 0)
							{
								strValue.Format("%.1f", ChannalMessage[nCH].dblISOCOM[nCom]);
								m_ctrXMLCtr.ModifyNode("NodeValue", strValue, dwBuf[nKey]);
							}

						}
					}
				}
			}

		}
	}
	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "XML关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg);
		return FALSE;
	}
	CopyFile(strTemplate,strLocalFile,FALSE);
	if (m_bIfSaveToServe)
	{
		CopyFile(strTemplate,strNetFile,FALSE);
	}

	strMsg.Format("正在上传无纸化数据......");
	LogInfo(strMsg);
	if (!SetFileByMSMQ(strLocalFile, strErrMsg))
	{
		strMsg = "数据上传无纸化失败，请联系相应工程师!";
		LogInfo(strMsg);
		return FALSE;
	}
	Sleep(1000);
	strMsg.Format("数据上传无纸化成功！");
	LogInfo(strMsg);

	if (!bPass)
	{
		LogInfo("数据不齐全！");
		return FALSE;
	}
	return TRUE;

}

BOOL CMy126S_45V_Switch_AppDlg::GetGoldenSampleConfig()
{
	/*
	CString strFile;
	CString strMsg;
	CString strValue;
	strFile.Format("%s\\config\\WorkStation\\%s\\GoldenSample.ini",
		m_strWorkStationPath,m_chComputerName);
	GetPrivateProfileString("SampleName","Value","ERROR",strValue.GetBuffer(256),256,strFile);
	if(strValue=="ERROR")
	{
		m_strGoldenSampleSN = "NULL";
	}
	else
	{
		m_strGoldenSampleSN = strValue;
	}
	strMsg.Format("Golden Sample:%s",m_strGoldenSampleSN);
	LogInfo(strMsg);
	*/
	return TRUE;
}

//通过输入的SN号获取对应的无纸化PN
//并从PN配置中获取对应的配置
void CMy126S_45V_Switch_AppDlg::OnKillfocusEditSn()
{
	// TODO: Add your control notification handler code here
	//通过SN号
	//将数据写入无纸化模板中
	CString strValue;
	UpdateData();
	//m_bReady = FALSE;
	
	//创建文件目录
	m_strGetSN = m_strSN;
	if (!GetSNListFromFile())
	{
		return;
	}
	CreatePath();
	GetLocalTime(&m_stStartTime);
	//获取工位信息
	if (!GetWorkStationConfig())
	{
		return;
	}

	strValue.Format("%d", m_stWorkStation.nProductPort);
	//SetDlgItemText(IDC_EDIT_COM_TESTER_PORT, strValue);
	//获取PN信息
	/*
	if (!GetPNFromAMTS())
	{
		return;
	}
	*/
	
	if (!GetRefConfig())
	{
		//return;
	}
	//根据PN获取产品配置信息
	if (!GetConfigByPN())
	{
		return;
	}

	/*
	LogInfo("获取无纸化电压数据范围");
	if (!GetParaRangeFromATMS())
	{
		return;
	}
	*/
	m_bReady = TRUE;
}


bool CMy126S_45V_Switch_AppDlg::GetWorkStationConfig()
{
	CString strFile;
	CString strApp;
	CString strKey;
	CString strValue;
	CString strMsg;
	int     nValue;
	strFile.Format("%s\\config\\WorkStation\\%s\\Device.ini", m_strWorkStationPath, &m_chComputerName);
	strApp.Format("TLS");
	strKey.Format("Value");
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		nValue = 0;
	}
	else
	{
		nValue = atoi(strValue);
	}

	if (nValue <= 0)
	{
		m_stWorkStation.bTLSEnable = FALSE;
		strMsg.Format("无需控制光源");
		LogInfo(strMsg);
	}
	else
	{
		m_stWorkStation.bTLSEnable = TRUE;
		m_stWorkStation.nTLSPort = nValue;
		strMsg.Format("光源串口为：%d", m_stWorkStation.nTLSPort);
		LogInfo(strMsg);
	}

	strApp.Format("ModulePort");
	strKey.Format("Value");
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		nValue = 0;
	}
	else
	{
		nValue = atoi(strValue);
	}
	m_stWorkStation.nProductPort = nValue;
	strMsg.Format("产品串口为：%d", m_stWorkStation.nProductPort);
	LogInfo(strMsg);

	strApp.Format("PMPort");
	strKey.Format("Value");
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		nValue = 0;
	}
	else
	{
		nValue = atoi(strValue);
	}
	m_stWorkStation.nPMPort = nValue;
	strMsg.Format("自制光功率计串口为：%d", m_stWorkStation.nPMPort);
	LogInfo(strMsg);

	strApp.Format("SW4TH");
	strKey.Format("Value");
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		nValue = 0;
	}
	else
	{
		nValue = atoi(strValue);
	}
	m_stWorkStation.nSW4THPort = nValue;
	strMsg.Format("第四代开关盒计串口为：%d", m_stWorkStation.nSW4THPort);
	LogInfo(strMsg);

	strApp.Format("SampleName");
	strKey.Format("Value");
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
	}
	m_stWorkStation.strGoldenSampleName = strValue;
	strMsg.Format("工位%s对应的Golden Sample为：%s", m_chComputerName,m_stWorkStation.strGoldenSampleName);
	LogInfo(strMsg);

	if (m_stWorkStation.nTLSPort == 99)
	{
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
	}
	
	
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::GetPNFromAMTS()
{
	// TODO: Add your control notification handler code here
	//通过SN号
	//将数据写入无纸化模板中
	CString strURL;
	CString strTemplate;
	CString strMsg;
	DWORD   dwBuf[256];
	DWORD   dwTmptBuf[256];
	DWORD   dwBufCH[256];
	DWORD   dwWDMTemp;
	int     nNodeCount;
	CString strTemp;
	CString strKey;
	CString strErrMsg;
	CString strNetFile;
	CString strLocalFile;
	SYSTEMTIME st;
	CString strValue;
	BOOL    bPass = TRUE;
	int     iSel;
	//////////////////////////////////////////////////////
	int     nRand = rand();
	
	strTemplate.Format("%s\\template\\Test_temp.xml", g_tszAppFolder);

	if (strcmp(m_stWorkStation.strGoldenSampleName, "ERROR") == 0)
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&adjust=1&randno=%d",
			m_strSN, nRand);
	}
	else
	{
		strURL.Format("http://zh-amtsdb-srv/AMTS/atd_GenerateMessage.aspx?type=1&serialno=%s&adjust=1&randno=%d&WorkStation=%s",
			m_strSN, nRand, m_stWorkStation.strGoldenSampleName);
	}
	strMsg.Format("获取无纸化模板：%s", strURL);
	LogInfo(strMsg);

	if (m_ctrXMLCtr.GetHtmlSource(strURL, strTemplate))
	{
		m_ctrXMLCtr.LoadXmlFile(strTemplate);
	}
	else
	{
		strMsg.Format("%s模板下载错误!", strTemplate);
		LogInfo(strMsg);
		return FALSE;
	}
	ZeroMemory(dwBuf, 256 * sizeof(DWORD));
	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "PN", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的PN号!";
		LogInfo(strMsg);
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Error", dwBuf, nNodeCount);
		if (nNodeCount != 0)
		{
			m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
			LogInfo(strTemp);
		}
		return FALSE;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	m_strPN = strTemp;
	strMsg.Format("获取到的PN为：%s", m_strPN);
	LogInfo(strMsg);

	nNodeCount = 0;
	m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "SPEC", dwBuf, nNodeCount);
	if (nNodeCount == 0)
	{
		strMsg = "XML文件结点不完整,测试的SPEC号!";
		LogInfo(strMsg);
		m_ctrXMLCtr.FindNodeByTag(strErrMsg, "BaseName", "Error", dwBuf, nNodeCount);
		if (nNodeCount != 0)
		{
			m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
			LogInfo(strTemp);
		}
		return FALSE;
	}
	m_ctrXMLCtr.GetNodeAttr("NodeValue", strTemp, dwBuf[0]);
	m_strSpec = strTemp;
	strMsg.Format("获取到的SPEC为：%s", m_strSpec);
	LogInfo(strMsg);

	if (!m_ctrXMLCtr.CloseXMLFile())
	{
		strMsg = "XML关闭失败,请通知相关工程师!";
		LogInfo(strErrMsg);
		return FALSE;
	}

	
	//m_cbxPN.ResetContent();
	//m_cbxPN.AddString(m_strPN);
	//m_cbxPN.SetCurSel(0);
	UpdateWindow();
	return TRUE;
}


//void CMy126S_45V_Switch_AppDlg::OnEnChangeEditSn()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//}


bool CMy126S_45V_Switch_AppDlg::GetConfigByPN()
{
	CString strFile;
	CString strApp;
	CString strKey;
	CString strValue;
	CString strMsg;
	CString strCom;
	int     nValue;
	strFile.Format("%s\\config\\%s\\device.ini", m_strConfigPath, m_strPN);
	strApp = "TLS";
	strKey = "WaveLength";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue,"ERROR")==0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_pdblWavelength = atof(strValue);
	strMsg.Format("调节波长:%.2fnm", m_pdblWavelength);
	LogInfo(strMsg);

	strApp = "PM";
	strKey = "CHCount";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_nCHCount = atoi(strValue);
	strMsg.Format("产品出光通道数:%d", m_nCHCount);
	LogInfo(strMsg);
	m_cbChannel.ResetContent();
	m_cbSetWL.ResetContent();
	m_cbSetPDBox.ResetContent();
	int nCH;
	for (nCH = 0;nCH<=m_nCHCount;nCH++)
	{
		strCom.Format("%d", nCH);
		m_cbChannel.AddString(strCom);
	}

	m_cbSetWL.AddString("1310");
	m_cbSetWL.AddString("1550");
	m_cbSetPDBox.AddString("1:20CH");
	m_cbSetPDBox.AddString("2:64CH");
	m_cbChannel.SetCurSel(0);
//	m_cbSetWL.SetCurSel(1);
//	m_cbSetPDBox.SetCurSel(0);
	strApp = "ComPort";
	strKey = "Count";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_nSwitchComPortCount = atoi(strValue);
	strMsg.Format("入光通道数:%d", m_nSwitchComPortCount);
	LogInfo(strMsg);
	/*
	m_cbComPort.ResetContent();
	int nCom;
	for (nCom = 0;nCom<m_nSwitchComPortCount;nCom++)
	{
		strCom.Format("COM%d", nCom);
		m_cbComPort.AddString(strCom);
	}
	m_cbComPort.SetCurSel(0);
	*/

	//获取调节配置
	CString strStruct;
	strApp = "STRUCT";
	strKey = "Value";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	strStruct = strValue;
	strMsg.Format("开关结构为：%s", strStruct);
	LogInfo(strMsg);
	/*
	int i = 0;
	strFile.Format("%s\\config\\%s\\ScanAdjustRange.ini", m_strConfigPath, strStruct);
	strApp.Format("ScanChannel");
	for (i=0;i<4;i++)
	{
		strKey.Format("CH%d", i + 1);
		GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
		if (strcmp(strValue, "ERROR") == 0)
		{
			strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
			LogInfo(strMsg, FALSE);
			return FALSE;
		}
		m_nAdjustCH[i] = atoi(strValue) - 1;
		strMsg.Format("调节通道%d的位置为：CH%d", i+1, m_nAdjustCH[i] + 1);
		LogInfo(strMsg);
	}
	//获取中心点配置
	//获取中心点信息：
	GetReCHConfig(strStruct);
	
	int nComport;
	for (nComport = 0;nComport < m_nSwitchComPortCount;nComport++)
	{
		if (nComport == 0)
		{
			strFile.Format("%s\\config\\%s\\ChannelCenter.ini", m_strConfigPath, strStruct);
		}
		else
		{
			strFile.Format("%s\\config\\%s\\ChannelCenter-COM%d.ini", m_strConfigPath, strStruct, nComport);
		}
		strApp = "RangeScan";
		strKey = "Point";
		GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
		if (strcmp(strValue, "ERROR") == 0)
		{
			strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
			LogInfo(strMsg, FALSE);
			return FALSE;
		}
		m_dblBreakIL = atof(strValue);
		strMsg.Format("扫描跳出IL限制为:%.2fdB", m_dblBreakIL);
		LogInfo(strMsg);

		for (i = 0;i < m_nCHCount;i++)
		{
			strApp.Format("CH%d", i + 1);
			strKey = "x";
			GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
			if (strcmp(strValue, "ERROR") == 0)
			{
				strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
				LogInfo(strMsg, FALSE);
				return FALSE;
			}
			m_nCenterX[nComport][i] = atoi(strValue);
			strKey = "y";
			GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
			if (strcmp(strValue, "ERROR") == 0)
			{
				strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
				LogInfo(strMsg, FALSE);
				return FALSE;
			}
			m_nCenterY[nComport][i] = atoi(strValue);
			strMsg.Format("通道%d中心电压位置为：%d,%d", i+1, m_nCenterX[nComport][i], m_nCenterY[nComport][i]);
			LogInfo(strMsg);
		}	
	}
	CString strVolt;
	strVolt.Format("%d", m_nCenterX[0][0]);
	SetDlgItemText(IDC_EDIT_X, strVolt);
	strVolt.Format("%d", m_nCenterY[0][0]);
	SetDlgItemText(IDC_EDIT_Y, strVolt);
	*/
	return TRUE;
}


void CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboComport()
{
	// TODO: Add your control notification handler code here
}


bool CMy126S_45V_Switch_AppDlg::GetRefConfig()
{
	CString strFile;
	CString strApp;
	CString strKey;
	CString strValue;
	CString strMsg;
	CString strCom;
	int     nValue;
	strFile.Format("%s\\config\\RefConfig.ini", m_strConfigPath);
	strApp = "RefConfig";
	strKey = "RefTime";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_stRefConfig.nTimeLimit = atoi(strValue);
	strMsg.Format("归零时间限制为:%d小时", m_stRefConfig.nTimeLimit);
	LogInfo(strMsg);

	strKey = "RefDiff";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_stRefConfig.dblDiffLimit = atof(strValue);
	strMsg.Format("归零差异限制为:%.2fdB", m_stRefConfig.dblDiffLimit);
	LogInfo(strMsg);
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::GetReCHConfig(CString strStruct)
{
	CString strFile;
	CString strApp;
	CString strKey;
	CString strValue;
	CString strMsg;
	int i = 0;
	m_stReCHConfig.nCHCount = 0; //
	strFile.Format("%s\\config\\%s\\ChannelCenter_Re.ini", m_strConfigPath, strStruct);
	strApp = "CHCount";
	strKey = "Value";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_stReCHConfig.nCHCount = atoi(strValue);
	strMsg.Format("反向扫描通道数:%d", m_stReCHConfig.nCHCount);
	LogInfo(strMsg);

	strApp = "CHStartIndex";
	strKey = "Value";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_stReCHConfig.nStartIndex = atoi(strValue);
	strMsg.Format("反向扫描起始通道:%d", m_stReCHConfig.nStartIndex);
	LogInfo(strMsg);

	//增加通道显示
	int nCH;
	for (nCH = 0;nCH < m_stReCHConfig.nCHCount;nCH++)
	{
		strValue.Format("%d-Re", nCH+ m_stReCHConfig.nStartIndex);
		m_cbChannel.AddString(strValue);
	}

	strApp = "PMIndex";
	strKey = "Value";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	m_stReCHConfig.nPMIndex = atoi(strValue);
	strMsg.Format("反向扫描监控通道:%d", m_stReCHConfig.nPMIndex);
	LogInfo(strMsg);

	for (i = 0;i < m_stReCHConfig.nCHCount;i++)
	{
		strApp.Format("CH%d", i + m_stReCHConfig.nStartIndex);
		strKey = "x";
		GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
		if (strcmp(strValue, "ERROR") == 0)
		{
			strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
			LogInfo(strMsg, FALSE);
			return FALSE;
		}
		m_stReCHConfig.nVolatgeX[i] = atoi(strValue);
		strKey = "y";
		GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
		if (strcmp(strValue, "ERROR") == 0)
		{
			strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
			LogInfo(strMsg, FALSE);
			return FALSE;
		}
		m_stReCHConfig.nVolatgeY[i] = atoi(strValue);
		strMsg.Format("反向扫描通道%d中心电压位置为：%d,%d", i + m_stReCHConfig.nStartIndex, m_stReCHConfig.nVolatgeX[i], m_stReCHConfig.nVolatgeY[i]);
		LogInfo(strMsg);
	}
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::ScanReCH(int nCH)
{
	int x;
	int y;
	int count = 0;
	int x_count = 0;
	int nCount = 0;
	int iCount = 0;
	double  dblPower = 0;

	CString strMsg;
	int x_start = 0;
	int x_stop = 0;
	int y_start = 0;
	int y_stop = 0;
	double dblPrePower = 100;
	double dblPrePowerX = 100;
	double dblPrePowerY = 100;
	char    chValue[10];
	ZeroMemory(chValue, sizeof(char) * 10);
	CString strINIFile;
	CString strValue;
	CString strStartX;
	CString strEndX;
	CString strStartY;
	CString strEndY;
	BOOL    bFindPoint = FALSE;
	m_bTestStop = FALSE;
	CString strSection;
	CString strNetFile;
	CString strKey;
	CString strCHFile;
	BOOL bFunctionOK = FALSE;
	double     dblPoint;
	//保存文件
	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	int    iSel = IDYES;
	int nDACX = x*MAX_DAC / MAX_VOLTAGE;
	int nDACY = y*MAX_DAC / MAX_VOLTAGE;
	int nVoltageX;
	int nVoltageY;
	CString strSendBuf;
	char chSendBuf[50];
	char chReadBuf[256];
	CString strToken;
	ZeroMemory(chReadBuf, sizeof(chReadBuf));
	int nComPort;

	try
	{
		dblPoint = m_dblBreakIL+3.5;
		x = m_stReCHConfig.nVolatgeX[nCH];
		y = m_stReCHConfig.nVolatgeY[nCH];
		strMsg.Format("CH%d 反向中心电压: %dmv,%dmv", nCH + m_stReCHConfig.nStartIndex, x, y);
		LogInfo(strMsg);
		//开始扫描通道
		nDACX = x*MAX_DAC / MAX_VOLTAGE;
		nDACY = y*MAX_DAC / MAX_VOLTAGE;

		ZeroMemory(chSendBuf, sizeof(chSendBuf));
		ZeroMemory(chReadBuf, sizeof(chReadBuf));
		strSendBuf.Format("SCAN ZONE 1 %d %d %d\r\n", m_stReCHConfig.nPMIndex, nDACX, nDACY);
		LogInfo(strSendBuf);
		memcpy(chSendBuf, strSendBuf, strSendBuf.GetLength());
		if (!m_opTesterCom.WriteBuffer(chSendBuf, strSendBuf.GetLength()))
		{
			strMsg.Format("发送指令%s失败!", strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		Sleep(2000);
		if (!m_opTesterCom.ReadBuffer(chReadBuf, 256))
		{
			strMsg.Format("接收指令%s失败!", strSendBuf);
			LogInfo(strMsg);
			return FALSE;
		}
		LogInfo(chReadBuf);
		strToken = strtok(chReadBuf, "\r\n:");
		strToken = strtok(NULL, "\r\n:");
		nDACX = atoi(strToken);

		strToken = strtok(NULL, "\r\n:");
		strToken = strtok(NULL, "\r\n:");
		nDACY = atoi(strToken);
		
		//
		//设置X Y
		if (!SETX(nDACX))
		{
			throw"设置XDAC失败！";
		}
		if (!SETY(nDACY))
		{
			throw"设置YDAC失败！";
		}
		//转化为电压
		nVoltageX = nDACX*MAX_VOLTAGE / MAX_DAC;

		nVoltageY = nDACY*MAX_VOLTAGE / MAX_DAC;

		//读取该通道的功率值
		if (!GetPDPower(m_stReCHConfig.nPMIndex, &dblPower))
		{
			throw"读取功率失败！";
		}

		double dblIL;
		//读取到的功率需要减去归零功率
		dblIL = m_dblReferencePower[nCH + m_stReCHConfig.nStartIndex-1] - dblPower;

		strMsg.Format("通道%d的电压点为:%d,%d,获取功率为:%.2fdBm,对应的IL为%.2fdB！", nCH + m_stReCHConfig.nStartIndex
			, nVoltageX, nVoltageY, dblPower, dblIL);
		LogInfo(strMsg);
		if (dblIL>dblPoint)
		{
			LogInfo("IL太大");
		}

		if (m_cbComPort.GetCurSel() == 1)
		{
			strNetFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage-Re_%d-COM1.csv",
				m_strNetFile, m_strItemName, m_strSN, m_strSN, nCH + m_stReCHConfig.nStartIndex);
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage-Re_%d-COM1.csv",
				m_strLocalPath, m_strItemName, m_strSN, m_strSN, nCH + m_stReCHConfig.nStartIndex);
		}
		else
		{
			strNetFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage-Re_%d.csv",
				m_strNetFile, m_strItemName, m_strSN, m_strSN, nCH + m_stReCHConfig.nStartIndex);
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage-Re_%d.csv",
				m_strLocalPath, m_strItemName, m_strSN, m_strSN, nCH + m_stReCHConfig.nStartIndex);
		}

		if (!stdCSVFile.Open(strCSVFile, CFile::modeCreate | CFile::modeWrite))
		{
			throw("创建数据文件失败！");
		}
		stdCSVFile.WriteString("Channel,X_Voltage,Y_Voltage,IL\n");

		strContent.Format("%d,%d,%d,%.2f\n", nCH + m_stReCHConfig.nStartIndex, nVoltageX, nVoltageY, dblIL);
		stdCSVFile.WriteString(strContent);

		stdCSVFile.Close();
		if (m_bIfSaveToServe)
		{
			CopyFile(strCSVFile, strNetFile, FALSE);
		}

		bFunctionOK = TRUE;
		/*
		if (ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX<m_stLimitPara.dblMinXVoltage[m_nReadPwrCH[nChannel]] ||
			ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageX>m_stLimitPara.dblMaxXVoltage[m_nReadPwrCH[nChannel]])
		{
			m_bVoltagePass = FALSE;
			strMsg.Format("X电压超出范围[%.0f,%.0f]", m_stLimitPara.dblMinXVoltage[m_nReadPwrCH[nChannel]],
				m_stLimitPara.dblMaxXVoltage[m_nReadPwrCH[nChannel]]);
			LogInfo(strMsg);
		}

		if (ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY<m_stLimitPara.dblMinYVoltage[m_nReadPwrCH[nChannel]] ||
			ScanPowerChannal[m_nReadPwrCH[nChannel]].VoltageY>m_stLimitPara.dblMaxYVoltage[m_nReadPwrCH[nChannel]])
		{
			m_bVoltagePass = FALSE;
			strMsg.Format("Y电压超出范围[%.0f,%.0f]", m_stLimitPara.dblMinYVoltage[m_nReadPwrCH[nChannel]],
				m_stLimitPara.dblMaxYVoltage[m_nReadPwrCH[nChannel]]);
			LogInfo(strMsg);
		}
		*/
	}
	catch (TCHAR* ptszErrorMsg)
	{
		MessageBox(ptszErrorMsg);
	}
	catch (...)
	{
		MessageBox("其他错误！扫描失败");
	}
	return bFunctionOK;
}


void CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIlctTest2()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	CString strTemp1;
	CString strChannel;
	CString strMsg;
	double  dblPref;
	double  dblPout;
	double  dblPower;
	char    strCT[32];
	char    strIL[32];
	int nchannel;
	m_bTestStop = FALSE;
	m_bReadPowerStop = TRUE;
	KillTimer(3);

	ZeroMemory(strCT, sizeof(strCT));
	ZeroMemory(strIL, sizeof(strIL));

	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}

	UpdateData();
	GetLocalTime(&m_stStartTime);
	strTemp.Format("开始扫描!");
	LogInfo(strTemp, FALSE);

	m_bReadPowerStop = TRUE;
	m_bShowResult = TRUE;
	m_bVoltagePass = TRUE;
	m_bReadPowerStop = FALSE;

	CString strCSVFile;
	CStdioFile stdCSVFile;
	CString strContent;
	HANDLE  hHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA win32Find;
	int    iSel = IDYES;
	int j;
	int i;
	BOOL   bFlag = TRUE;
	double  dblCT[32];
	BOOL pbFail[20] = { 0 };
	BOOL pbyArray[20][20] = { 0 };
	CString strNetFile;
	int nVolatgeX;
	int nVolatgeY;
	CString strTempName;
	GetDlgItemText(IDC_COMBO_TEMPNAME, strTempName);

	if (m_cbChannel.GetCurSel() == 0)
	{
		MessageBox("请选择正确的通道");
		return;
	}
	i = m_cbChannel.GetCurSel() - 1;
	if (i < m_nCHCount) //正常范围通道,可以测试一次测试所有通道
	{		
		strTemp.Format("开始测试正向IL及CT测试！");
		LogInfo(strTemp, FALSE);
		strTemp.Format("通道");
		for (i = 0;i<m_nCHCount;i++)
		{
			strTemp1.Format("  CT%d", i + 1);
			strTemp = strTemp + strTemp1;
			ChannalMessage[i].dblIL = 0;
		}
		LogInfo(strTemp);

		Sleep(100);
		for (nchannel = 0; nchannel < m_nCHCount; nchannel++)
		{
			YieldToPeers();
			if (m_bTestStop)
			{
				MessageBox("测试中止！");
				return;
			}
			if (m_cbComPort.GetCurSel() == 1)
			{
				//开关切换
				if (!SPRTProductCH(nchannel + 1 + m_nCHCount))
				{
					strMsg.Format("切换到通道%d错误", nchannel + 1 + m_nCHCount);
					LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
					return;
				}

			}
			else
			{
				//开关切换
				if (!SPRTProductCH(nchannel + 1))
				{
					strMsg.Format("切换到通道%d错误", nchannel + 1);
					LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
					return;
				}
			}
			Sleep(1000);
			int i = 0;
			if (!GetCT(dblCT, m_nCHCount))
			{
				return;
			}
			bFlag = TRUE;
			strTemp.Format("%02d", nchannel + 1);
			for (i = 0; i < m_nCHCount; i++)
			{
				//调用归零数据
				dblPref = m_dblReferencePower[i];
				dblPower = dblCT[i];
				strMsg.Format("%.2f", dblPower);

				dblPout = dblPower;
				ChannalMessage[nchannel].dblCT[i] = TestIL(dblPref, dblPout);
				if (nchannel == i)
				{
					ChannalMessage[nchannel].dblIL = ChannalMessage[nchannel].dblCT[i];
					if (!CheckParaPassOrFail(IL_ROOM_DATA, ChannalMessage[nchannel].dblIL, nchannel))
					{
						bFlag = FALSE;
					}
				}
				else
				{
					if (!CheckParaPassOrFail(CT_DATA, ChannalMessage[nchannel].dblCT[i], i))
					{
						bFlag = FALSE;
					}
				}

				strTemp1.Format("  %.2f", ChannalMessage[nchannel].dblCT[i]);
				strTemp = strTemp + strTemp1;
			}
			if (!bFlag)
			{
				LogInfo(strTemp, (BOOL)FALSE, COLOR_RED);
			}
			else
			{
				LogInfo(strTemp);
			}

		}

		
		//保存到CSV文件
		if (m_cbComPort.GetCurSel() == 0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_CT_%s.csv",
				m_strLocalPath, m_strItemName, m_strSN, m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s_CT.csv",
				m_strNetFile, m_strItemName, m_strSN, m_strSN);
		}
		else if (m_cbComPort.GetCurSel() == 1)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_CT-COM1.csv",
				m_strLocalPath, m_strItemName, m_strSN, m_strSN);
			strNetFile.Format("%s\\data\\%s\\%s\\%s_CT-COM1.csv",
				m_strNetFile, m_strItemName, m_strSN, m_strSN);
		}
		hHandle = FindFirstFile(strCSVFile, &win32Find);
		if (hHandle != INVALID_HANDLE_VALUE)
		{


		}

		if (!stdCSVFile.Open(strCSVFile, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("创建CT数据文件失败！");
			LogInfo("创建CT数据文件失败！", FALSE, COLOR_RED);
			return;
		}
		strTemp.Format("Channel");
		for (i = 0;i<m_nCHCount;i++)
		{
			strTemp1.Format(",%d", i + 1);
			strTemp = strTemp + strTemp1;
		}
		strTemp = strTemp + "\n";
		stdCSVFile.WriteString(strTemp);

		for (i = 0;i<m_nCHCount;i++)
		{
			strContent.Format("CH%d/CT", i + 1);
			for (j = 0;j<m_nCHCount;j++)
			{
				strTemp1.Format(",%.2f", ChannalMessage[i].dblCT[j]);
				strContent = strContent + strTemp1;
			}
			strContent = strContent + "\n";
			stdCSVFile.WriteString(strContent);
		}
		stdCSVFile.Close();
		if (m_bIfSaveToServe)
		{
			CopyFile(strCSVFile, strNetFile, FALSE);
		}

		//保存到EXCEL文件中

		//保存到EXCEL文件中

		SaveDataToCSV(CT_DATA);
		Sleep(100);
		SaveDataToCSV(IL_ROOM_DATA);
		Sleep(100);

		//将所有不合格的数据打印下来
		for (nchannel = 0; nchannel < m_nCHCount; nchannel++)
		{
			if (pbFail[nchannel])
			{
				for (i = 0; i < m_nCHCount; i++)
				{
					if (pbyArray[nchannel][i])
					{
						if (nchannel == i)
						{
							strTemp.Format("CH%d IL不合格!", nchannel + 1);
							LogInfo(strTemp, (BOOL)FALSE, COLOR_RED);
						}
						else
						{
							strTemp.Format("CH%d-%d CT不合格!", nchannel + 1, i + 1);
							LogInfo(strTemp, (BOOL)FALSE, COLOR_RED);
						}
					}
				}

			}
		}

		if (!SetVoltage(0))
		{
		}
		LogInfo("IL/CT测试完毕!");
		if (!m_bVoltagePass)
		{
			LogInfo("电压不在合格范围内，请注意", (BOOL)FALSE, COLOR_RED);
		}
		MessageBox("IL/CT测试完毕!");

	}
	else  //反向范围通道
	{
		//只能单通道测试
		nchannel = i - m_nCHCount; //From Index0
		strMsg.Format("请将入光接CH%d,回损头接光功率计通道%d", nchannel + m_stReCHConfig.nStartIndex, m_stReCHConfig.nPMIndex);
		MessageBox(strMsg);
		strTemp.Format("开始测试通道%d反向IL及CT测试！", nchannel + m_stReCHConfig.nStartIndex);
		LogInfo(strTemp, FALSE);
		strTemp.Format("通道");
		for (i = 0;i<m_nCHCount;i++)
		{
			strTemp1.Format("  CT%d", i + 1);
			strTemp = strTemp + strTemp1;
			ChannalMessage[i].dblIL = 0;
		}
		LogInfo(strTemp);
	
		//切换反向通道位置
		if (!GetReVolatgeFromFile(nchannel, &nVolatgeX, &nVolatgeY))
		{
			return;
		}
		strMsg.Format("设置坐标（%d,%d）mv", nVolatgeX, nVolatgeY);
		LogInfo(strMsg);
		nVolatgeX = nVolatgeX*MAX_DAC / MAX_VOLTAGE;
		nVolatgeY = nVolatgeY*MAX_DAC / MAX_VOLTAGE;
		SETX(nVolatgeX);
		SETY(nVolatgeY);
		
		Sleep(1000);
		int i = 0;
		if (!GetCT(dblCT, m_stReCHConfig.nPMIndex))
		{
			return;
		}
		bFlag = TRUE;
		strTemp.Format("%02d", nchannel + 1);
		for (i = 0; i < m_nCHCount; i++)
		{
			//调用归零数据
			dblPref = m_dblReferencePower[i];
			dblPower = dblCT[i];
			strMsg.Format("%.2f", dblPower);
			dblPout = dblPower;
			ChannalMessage[nchannel].dblCT[i] = TestIL(dblPref, dblPout);
			if ((nchannel + m_stReCHConfig.nStartIndex - 1) == i)
			{
				ChannalMessage[nchannel].dblIL = m_dblRefPowerRe - dblCT[m_stReCHConfig.nPMIndex - 1];
				ChannalMessage[nchannel].dblCT[i] = ChannalMessage[nchannel].dblIL; //替换
			}
			else
			{
				if (!CheckParaPassOrFail(CT_DATA, ChannalMessage[nchannel].dblCT[i], i))
				{
					bFlag = FALSE;
				}
			}

			strTemp1.Format("  %.2f", ChannalMessage[nchannel].dblCT[i]);
			strTemp = strTemp + strTemp1;
		}
		if (!bFlag)
		{
			LogInfo(strTemp, (BOOL)FALSE, COLOR_RED);
		}
		else
		{
			LogInfo(strTemp);
		}

		//保存到CSV文件
		if (m_cbComPort.GetCurSel() == 0)
		{
			strCSVFile.Format("%s\\data\\%s\\%s\\%s_CT_RE_CH%d.csv",
				m_strLocalPath, m_strItemName, m_strSN, m_strSN, nchannel + m_stReCHConfig.nStartIndex);
			strNetFile.Format("%s\\data\\%s\\%s\\%s_CT_RE_CH%d.csv",
				m_strNetFile, m_strItemName, m_strSN, m_strSN, nchannel + m_stReCHConfig.nStartIndex);
		}
		
		hHandle = FindFirstFile(strCSVFile, &win32Find);
		if (hHandle != INVALID_HANDLE_VALUE)
		{


		}

		if (!stdCSVFile.Open(strCSVFile, CFile::modeCreate | CFile::modeWrite))
		{
			MessageBox("创建CT数据文件失败！");
			LogInfo("创建CT数据文件失败！", FALSE, COLOR_RED);
			return;
		}
		strTemp.Format("Channel");
		for (i = 0;i<m_nCHCount;i++)
		{
			strTemp1.Format(",%d", i + 1);
			strTemp = strTemp + strTemp1;
		}
		strTemp = strTemp + "\n";
		stdCSVFile.WriteString(strTemp);

		//for (i = 0;i<m_nCHCount;i++)
		{
			strContent.Format("CH%d/CT", nchannel + m_stReCHConfig.nStartIndex);
			for (j = 0;j<m_nCHCount;j++)
			{
				strTemp1.Format(",%.2f", ChannalMessage[i].dblCT[j]);
				strContent = strContent + strTemp1;
			}
			strContent = strContent + "\n";
			stdCSVFile.WriteString(strContent);
		}
		stdCSVFile.Close();
		if (m_bIfSaveToServe)
		{
			CopyFile(strCSVFile, strNetFile, FALSE);
		}

		if (!SetVoltage(0))
		{
		}
		LogInfo("IL/CT测试完毕!");
		if (!m_bVoltagePass)
		{
			LogInfo("电压不在合格范围内，请注意", (BOOL)FALSE, COLOR_RED);
		}
		MessageBox("IL/CT测试完毕!");
	}
}

bool CMy126S_45V_Switch_AppDlg::GetReVolatgeFromFile(int CHIndex, int* pnVolatgeX, int* pnVolatgeY)
{
	CString strNetFile;
	CString strCSVFile;
	FILE    *fp = NULL;
	char    pchRead[256];
	CString strMsg;
	CString strToken;
	int      nVoltageX;
	int      nVoltageY;
	
	strNetFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage-Re_%d.csv",
		m_strNetFile, m_strItemName, m_strSN, m_strSN, CHIndex + m_stReCHConfig.nStartIndex);
	strCSVFile.Format("%s\\data\\%s\\%s\\%s_ScanVoltage-Re_%d.csv",
		m_strLocalPath, m_strItemName, m_strSN, m_strSN, CHIndex + m_stReCHConfig.nStartIndex);
	
	if (m_bIfSaveToServe)
	{
		fp = fopen(strNetFile, "r");
	}
	else
	{
		fp = fopen(strCSVFile, "r");
	}
	if (fp == NULL)
	{
		strMsg.Format("打开文件%s失败", strCSVFile);
		LogInfo(strMsg);
		return FALSE;
	}
	fgets(pchRead, 256,fp);
	fgets(pchRead, 256,fp);
	strToken = strtok(pchRead, ",");
	strToken = strtok(NULL, ",");
	nVoltageX = atoi(strToken);
	strToken = strtok(NULL, ",");
	nVoltageY = atoi(strToken);
	*pnVolatgeX = nVoltageX;
	*pnVolatgeY = nVoltageY;
	fclose(fp);
	return TRUE;
}

bool CMy126S_45V_Switch_AppDlg::OpenTLSDevice()
{
	CString strMsg;
	if (!ConnectSocket())
	{
		return FALSE;
	}
	if (!SetTLSWL(m_pdblWavelength))
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

void CMy126S_45V_Switch_AppDlg::OnButtonSetWl() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CString strMsg;

	if(!SetTLSWL(m_dblTLSSetWL))
	{
		return;
	}
}


void CMy126S_45V_Switch_AppDlg::OnEnChangeEditSn()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


bool CMy126S_45V_Switch_AppDlg::GetSNListFromFile()
{
	CString strFile, strValue;
	CString strMsg;
	CString strToken;
	char    pchRead[256];
	FILE    *fp = NULL;
	LPCTSTR lpStr = NULL;
	int     nIndex = 0;
	int     nSN;
	int     nCHCount=0;
	CString strContent;
	ZeroMemory(pchRead, sizeof(pchRead));
	strFile.Format("%s\\Config\\%s_SNList.csv", m_strConfigPath,m_strSN);
	fp = fopen(strFile, "r");
	if (fp == NULL)
	{
		strMsg.Format("打开文件%s失败！", strFile);
		LogInfo(strMsg);
		return FALSE;
	}
	lpStr = fgets(pchRead, 256, fp);
	lpStr = fgets(pchRead, 256, fp); //Index,SN,CHCount
//	ZeroMemory(m_stSNList, sizeof(m_stSNList)*MAX_SW_COUNT);
	for (int ii = 0; ii < MAX_SW_COUNT;ii++)
	{
		m_stSNList[ii].nCHCount = 0;
		m_stSNList[ii].nCOMCount = 0;
	}
	while (lpStr != NULL)
	{
		strToken = strtok(pchRead, ",");
		strToken = strtok(NULL, ",");
		ZeroMemory(m_stSNList[nIndex].strSN, sizeof(m_stSNList[nIndex].strSN));
		memcpy(m_stSNList[nIndex].strSN, strToken, strToken.GetLength());
		strToken = strtok(NULL, ",");
		m_stSNList[nIndex].nCHCount = atoi(strToken);
		strToken = strtok(NULL, ",");
		m_stSNList[nIndex].nCOMCount = atoi(strToken);
		if (m_stSNList[nIndex].nCHCount > nCHCount)
		{
			nCHCount = m_stSNList[nIndex].nCHCount;
		}
		nIndex++;
		if (nIndex >= MAX_SW_COUNT)
		{
			strMsg.Format("SN数量已超过%d", MAX_SW_COUNT);
			LogInfo(strMsg);
			fclose(fp);
			return FALSE;
		}
		lpStr = fgets(pchRead, 256, fp);
	}
	fclose(fp);
	m_cbxSNList.ResetContent();
	for (nSN = 0;nSN < nIndex;nSN++)
	{
		strContent.Format("%d:%s", nSN + 1, &m_stSNList[nSN].strSN);
		m_cbxSNList.AddString(strContent);
	}
	m_cbxSNList.SetCurSel(0);
	m_nCHCount = nCHCount;
	strMsg.Format("最大通道数为：%d", m_nCHCount);
	LogInfo(strMsg);
	
	return TRUE;
}


void CMy126S_45V_Switch_AppDlg::OnBnClickedButtonSwCal()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CString strMsg;
	int     nCH;
	m_nSNIndex = m_cbxSNList.GetCurSel();
	strMsg.Format("当前测试SN为：%s", m_stSNList[m_nSNIndex].strSN);
	LogInfo(strMsg);
	m_bTestStop = FALSE;
	int inPM = m_cbSetPDBox.GetCurSel();
	//1.获取调节电压数据
	if (!m_bReady)
	{
		MessageBox("请先输入SN！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("没有归零，请先归零");
		return;
	}
	if(m_stSNList[m_nSNIndex].nCHCount ==2&& m_stSNList[m_nSNIndex].nCOMCount==2)
	{
		strMsg.Format("产品青色(CH2)端接进光,产品棕色(CH3)，紫色(CH4)依次接光功率计1,2！"); 
	}
	else
	{
		strMsg.Format("产品COM端接进光，产品出光依次接光功率计！");
	}
	MessageBox(strMsg);
	LogInfo(strMsg);
	
	if (!GetVoltageFile())
	{
		if (!GetCHCenter(m_stSNList[m_nSNIndex].nCHCount))
		{
			return;
		}
	}
	//设置扫描延时时间
	int nDelayTimeus;
	nDelayTimeus = 1000;
	
	if (!m_CmdModule.SetScanDelayTime(nDelayTimeus))
	{
		strMsg.Format("设置扫描延时时间失败！");
		LogInfo(strMsg);
		//return;
	}
	strMsg.Format("设置扫描延时时间为：%dus！", nDelayTimeus);
	LogInfo(strMsg);
	//2.扫描获取最小点
	int i = 0;
	i = m_cbChannel.GetCurSel()-1;	
	GetVoltageFileforCal();
	for (nCH=0;nCH<m_stSNList[m_nSNIndex].nCHCount;nCH++)
	{
		if (m_bTestStop)
		{
			return;
		}
		if (i != nCH&&i!=-1)
		{
			continue;
		}
		if (inPM == 0)
		{
			if (!ScanPointForOplinkPM(nCH, m_nSNIndex))
			{
				return;
			}
		}
		else if (inPM == 1)
		{
			if (!ScanZoneEXSigleCH(nCH,m_nSNIndex))
			{
				return;
			}
		}
		YieldToPeers();
	}
	//3.保存数据
	if (!SaveDataToCSV(VOL_DATA,m_nSNIndex))
	{
		return;
	}
	//4.数据下载
	if (!CreateLutFile())
	{
		return;
	}
	if(!CreateBinFileZ4671())
	{
		return;
	}
	OnButtonIlctTest();
}
BOOL CMy126S_45V_Switch_AppDlg::CreateFullPath(CString strPath)
{
	if (strPath.IsEmpty())
	{
		return FALSE;
	}
	if (-1 == GetFileAttributes(strPath))
	{
		int nPos = strPath.ReverseFind('\\');
		CreateFullPath(strPath.Left(nPos));
		CreateDirectory(strPath, NULL);
	}

	return TRUE;
}
bool CMy126S_45V_Switch_AppDlg::GetVoltageFileforCal()
{
	//获取开关SN号
	CString strDataFile;
	CString strValue;
	CString strSNFile;
	CString strSwitchSN;
	CString strVoltFile;
	CString strVoltFilePath;
	CStdioFile stdFile;
	CString strMsg;
	char    pchRead[256];
	CString strToken;
	CString strFile;
	int     nCHCount;
	CString strCSVFile, strNetFile;
	LPCTSTR lpStr = NULL;
	BOOL    bGetFile;
	int nIndex = 0;
	FILE *fp = NULL;
	//判断XML文件是否存在，如果存在就直接获取XML调用开关数据

	//优先从网络路径获取电压数据
	//模块测试从终测数据中获取
	nCHCount = m_stSNList[m_nSNIndex].nCHCount;
	bGetFile = TRUE;
	int nChannel = m_cbxSNList.GetCurSel();
	

	strCSVFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f",
		m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength);
	CreateFullPath(strCSVFile);
	strCSVFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%d_Voltage_%s.CSV",
		m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength, nChannel + 1, m_stSNList[nChannel].strSN);
		
	if (GetFileAttributes(strCSVFile) == -1)
	{
		strMsg.Format("电压文件%s不存在！", strVoltFile);
		LogInfo(strMsg);
			
	}

	fp = fopen(strCSVFile, "r");
	if (fp == NULL)
	{
		strMsg.Format("获取文件%s失败！", strCSVFile);
		LogInfo(strMsg);
		return FALSE;
	}
	ZeroMemory(pchRead, sizeof(char) * 256);
	fgets(pchRead, 256, fp);
	for (nIndex = 0; nIndex<m_stSNList[nChannel].nCHCount; nIndex++)
	{
		fgets(pchRead, 256, fp);
		strValue = strtok(pchRead, ",");
		strValue = strtok(NULL, ",");
		ScanPowerChannal[nIndex].VoltageX = atoi(strValue);
		strValue = strtok(NULL, ",");
		ScanPowerChannal[nIndex].VoltageY = atoi(strValue);
		strValue = strtok(NULL, ",");
		ScanPowerChannal[nIndex].dblIL = atof(strValue);
		strMsg.Format("校准时：调用电压数据:CH%d,X:%d,Y:%d,IL=%.2f", nIndex + 1, ScanPowerChannal[nIndex].VoltageX, ScanPowerChannal[nIndex].VoltageY, ScanPowerChannal[nIndex].dblIL);
		LogInfo(strMsg);
	}
	fclose(fp);

	return TRUE;
}
bool CMy126S_45V_Switch_AppDlg::GetVoltageFile()
{
	//获取开关SN号
	CString strDataFile;
	CString strValue;
	CString strSNFile;
	CString strSwitchSN;
	CString strVoltFile;
	CString strVoltFilePath;
	CStdioFile stdFile;
	CString strMsg;
	char    chReadBuf[256];
	CString strToken;
	CString strFile;
	int     nCHCount;
	//CString strLocalFile;
	LPCTSTR lpStr = NULL;
	BOOL    bGetFile;
	//判断XML文件是否存在，如果存在就直接获取XML调用开关数据

	//优先从网络路径获取电压数据
	//模块测试从终测数据中获取
	nCHCount = m_stSNList[m_nSNIndex].nCHCount;
	bGetFile = TRUE;
	int nch = 0;
	for (nch = 0;nch<nCHCount;nch++)
	{
		strVoltFile.Format("%s\\%s\\%s_ScanVoltage_%d.csv",
			m_strVoltagePath, m_stSNList[m_nSNIndex].strSN, m_stSNList[m_nSNIndex].strSN, nch + 1);
		if (m_cbComPort.GetCurSel() == 1)
		{
			strVoltFile.Format("%s\\%s\\%s_ScanVoltage_%d-COM1.csv",
				m_strVoltagePath, m_stSNList[m_nSNIndex].strSN, m_stSNList[m_nSNIndex].strSN, nch + 1);
		}
		if (GetFileAttributes(strVoltFile) == -1)
		{//\\zh-mfs-srv2.oplink.com.cn\Data\TestData\WS02\MEMS-SWITCH\Z5891\1831730306\K3517869\终测\Calibration\VoltData\Room
			strVoltFile.Format("%s\\%s\\终测\\Calibration\\VoltData\\Room\\ScanVoltage_%d.csv",
				m_strVoltagePath, m_stSNList[m_nSNIndex].strSN, nch + 1);
			if (m_cbComPort.GetCurSel() == 1)
			{
				strVoltFile.Format("%s\\%s\\终测\\Calibration\\VoltData\\Room\\ScanVoltage_%d-COM1.csv",
					m_strVoltagePath, m_stSNList[m_nSNIndex].strSN, nch + 1);
			}
			if (GetFileAttributes(strVoltFile) == -1)
			{
				strMsg.Format("电压文件%s不存在！", strVoltFile);
				LogInfo(strMsg);
				return FALSE;
			}
		}
		ZeroMemory(chReadBuf, sizeof(chReadBuf));
		if (!stdFile.Open(strVoltFile, CFile::modeRead))
		{
			strMsg.Format("打开电压文件%s失败！", strVoltFile);
			LogInfo(strMsg);
			return FALSE;
		}
		lpStr = stdFile.ReadString(chReadBuf, 256);
		if (lpStr == NULL)
		{
			strMsg.Format("读取电压文件%s失败！", strVoltFile);
			LogInfo(strMsg);
			stdFile.Close();
			return FALSE;
		}
		stdFile.ReadString(chReadBuf, 256);
		strToken = strtok(chReadBuf, ",");
		strToken = strtok(NULL, ",");
		m_Xbase[nch] = atoi(strToken);
		strToken = strtok(NULL, ",");
		m_Ybase[nch] = atoi(strToken);
		stdFile.Close();
		strMsg.Format("调用电压数据:CH%d,X:%d,Y:%d", nch + 1, m_Xbase[nch], m_Ybase[nch]);
		LogInfo(strMsg);
	}
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::GetCHCenter(int nCHIndex)
{
	CString strFile;
	CString strApp;
	CString strKey;
	CString strValue;
	CString strMsg;
	int     i =0;
	
	strFile.Format("%s\\config\\%s\\ChannelCenter_%d.ini", m_strConfigPath, m_strPN, nCHIndex);
	if (m_cbComPort.GetCurSel() == 1)
	{
		strFile.Format("%s\\config\\%s\\ChannelCenter_%d_COM1.ini", m_strConfigPath, m_strPN, nCHIndex);
	}
	strApp.Format("IL", i + 1);
	strKey = "RefIL";
	GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
	if (strcmp(strValue, "ERROR") == 0)
	{
		strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
		LogInfo(strMsg, FALSE);
		return FALSE;
	}
	dblRefIL = atof(strValue);
	for (i = 0;i < nCHIndex;i++)
	{
		strApp.Format("CH%d", i + 1);
		strKey = "x";
		GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
		if (strcmp(strValue, "ERROR") == 0)
		{
			strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
			LogInfo(strMsg, FALSE);
			return FALSE;
		}
		m_Xbase[i] = atoi(strValue);

		strKey = "y";
		GetPrivateProfileString(strApp, strKey, "ERROR", strValue.GetBuffer(128), 128, strFile);
		if (strcmp(strValue, "ERROR") == 0)
		{
			strMsg.Format("获取配置错误%s-%s:%s", strApp, strKey, strFile);
			LogInfo(strMsg, FALSE);
			return FALSE;
		}
		m_Ybase[i] = atoi(strValue);
		strMsg.Format("通道%d中心电压位置为：%d,%d", i + 1, m_Xbase[i], m_Ybase[i]);
		LogInfo(strMsg);
	}
	return TRUE;
}

bool CMy126S_45V_Switch_AppDlg::ScanPointForOplinkPM(int nPort, int nSwitchIndex)
{
	CString strMsg;
	int   wScanMaxIndex;
	SHORT sDACx;
	SHORT sDACy;
	SHORT sScanMinVx;
	SHORT sScanMaxVx;
	SHORT sScanMinVy;
	SHORT sScanMaxVy;
	WORD  wStep = 96;//1 chunliangw
	WORD  wRowIndex;
	WORD  wColumnIndex;
	BOOL  bAdjust = FALSE;
	CString strValue;
	CString strToken;
	CString strConfigFile;
	stSWScanPara stScanPara;
	double  dblPower;
	double  dblRef;
	int nBlock = 0;
	int nSwitch = 0;
	int nScanPoint;
	char pchRead[128];
	int nReadCount;
	double dblIL;
	ZeroMemory(pchRead, sizeof(pchRead));
	stScanPara.nBlock = nBlock;
	stScanPara.nSwitch = nSwitch;
	//stSWScanPara stScanPara;

	//ADJUST:
	m_Xbase[nPort] = m_Xbase[nPort] * MAX_DAC / MAX_VOLTAGE;
	m_Ybase[nPort] = m_Ybase[nPort] * MAX_DAC / MAX_VOLTAGE;
	sDACx = m_Xbase[nPort];
	sDACy = m_Ybase[nPort];

	strMsg.Format("%d通道基准点为:(%d,%d)", nPort + 1, sDACx, sDACy);
	LogInfo(strMsg);

	while (wStep)
	{
		YieldToPeers();
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
		stScanPara.nStartX = sScanMinVx;
		stScanPara.nEndX = sScanMaxVx;
		stScanPara.nStartY = sScanMinVy;
		stScanPara.nEndY = sScanMaxVy;
		stScanPara.nStep = wStep;
	
		stScanPara.nBlock = 0;
		stScanPara.nSwitch = nSwitchIndex;
		if (nSwitchIndex >= 16)
		{
			stScanPara.nBlock = 1;
			stScanPara.nSwitch = nSwitchIndex-16;
		}
		if (nSwitchIndex >= 32)
		{
			stScanPara.nBlock = 2;
			stScanPara.nSwitch = nSwitchIndex - 32;
		}
	

		strMsg.Format("Point:%d", nScanPoint);
		LogInfo(strMsg);

		if (!StartMonitorPD(nPort + 1, nScanPoint))
		{
			return FALSE;
		}
		
		strMsg.Format("Scan:%d,%d,%d,%d,%d", sScanMinVx, sScanMaxVx, sScanMinVy, sScanMaxVy, wStep);
		LogInfo(strMsg);
		if (!m_CmdModule.SendScanTrig(stScanPara))
		{
			if (!m_CmdModule.SendScanTrig(stScanPara))
			{
				strMsg.Format("发送Trig失败！");
				LogInfo(strMsg);
				return FALSE;
			}
		}
		int error = 0;
		Sleep(600);
		//发送指令求取区域范围内的最小IL点的序号；
		if (!GetMaxIndex(&wScanMaxIndex, nPort + 1))
		{
			Sleep(700);
			if (!GetMaxIndex(&wScanMaxIndex, nPort + 1))
			{
				return FALSE;
			}
		}

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
	ScanPowerChannal[nPort].VoltageX = sDACx;
	ScanPowerChannal[nPort].VoltageY = sDACy;

	Sleep(100);
	//发送一个回车结束掉监控状态！
	if (m_nSNIndex >= 32)
	{
		if (!m_CmdModule.SetSwitchDAC(3, m_nSNIndex - 32 + 1, sDACx, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}
		if (!m_CmdModule.SetSwitchDAC(3, m_nSNIndex - 32 + 1, sDACy, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}

	}
	else if (m_nSNIndex >= 16)
	{
		if (!m_CmdModule.SetSwitchDAC(2, m_nSNIndex-16 + 1, sDACx, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}
		if (!m_CmdModule.SetSwitchDAC(2, m_nSNIndex - 16 + 1, sDACy, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}

	}
	else
	{
		if (!m_CmdModule.SetSwitchDAC(1, m_nSNIndex + 1, sDACx, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}
		if (!m_CmdModule.SetSwitchDAC(1, m_nSNIndex + 1, sDACy, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}

	}
	
	Sleep(150);
	dblPower = ReadPWMPower(nPort);
	dblRef = m_dblReferencePower[nPort];
	dblIL = dblRef - dblPower;
	strMsg.Format("Port:%d,X:%d,Y:%d,IL:%.2fdB,Power:%.2fdBm", nPort+1,sDACx, sDACy, dblIL,dblPower);
	LogInfo(strMsg);
	ScanPowerChannal[nPort].dblIL = dblIL;
	if (dblIL > dblRefIL)
	{
		strMsg.Format("通道%dIL超标>%.2fdB!",nPort+1, dblRefIL);
		//MessageBox(strMsg);
		LogInfo(strMsg,1,COLOR_RED);
		//return FALSE;
	}

	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::StartMonitorPD(int nPort, int nScanPoint)
{
	char pchCmd[256];
	char recvBuf[256];
	CString strValue;
	CString strToken;
	CString strMsg;
	WORD wSendLen;
	int i = 0;

	ZeroMemory(pchCmd, 256);
	sprintf(pchCmd, "SCAN STARTADC %d %d\r\n", nPort, nScanPoint);
	wSendLen = strlen(pchCmd);
	if (!m_opTesterCom.WriteBuffer(pchCmd, wSendLen))
	{
		strMsg.Format("自制光功率计发送%s失败！", &pchCmd);
		LogInfo(strMsg);
		return FALSE;
	}
	Sleep(50);
	ZeroMemory(recvBuf, 256);
	ZeroMemory(recvBuf, 256);
	if (!m_opTesterCom.ReadBuffer(recvBuf, 256))
	{
		strMsg.Format("自制光功率计读取%s失败！", &pchCmd);
		LogInfo(strMsg);
		return FALSE;
	}
	strToken = strtok(recvBuf, "\r\n");
	LogInfo(strToken);
	if (strToken.Find("OK", 0) == -1)
	{
		return FALSE;
	}
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::GetMaxIndex(int* pnIndex, int nPort)
{
	char pchCmd[256];
	char recvBuf[256];
	CString strValue;
	CString strToken;
	CString strMsg;
	WORD wSendLen;
	int  nIndex;
	int i = 0;

	ZeroMemory(pchCmd, 256);
	sprintf(pchCmd, "Get MAXIDX %d\r\n", nPort);
	wSendLen = strlen(pchCmd);
	if (!m_opTesterCom.WriteBuffer(pchCmd, wSendLen))
	{
		strMsg.Format("自制光功率计发送%s失败！", &pchCmd);
		LogInfo(strMsg);
		return FALSE;
	}
	Sleep(50);
	ZeroMemory(recvBuf, 256);
	if (!m_opTesterCom.ReadBuffer(recvBuf, 256))
	{
		strMsg.Format("自制光功率计读取%s失败！", &pchCmd);
		LogInfo(strMsg);
		return FALSE;
	}
	strToken = strtok(recvBuf, "\r\n");

	if (strToken.Find("Error", 0) != -1)
	{
		return FALSE;
	}
	if (strToken.Find("data", 0) != -1)
	{
		return FALSE;
	}
	LogInfo(strToken);
	nIndex = atoi(recvBuf);
	*pnIndex = nIndex;
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::DataDownLoadZ4671(CString strBinFileName)
{
	BOOL		bFileDone = FALSE;
	BOOL		bFunctionOK = FALSE;
	char		pbBinData[MAX_DATA_LENGTH];
	DWORD		dwBytesRead;
	DWORD		dwCodeSizeLeft;
	DWORD		dwTotalDataCount;
	DWORD		dwLoadDataSize;
	WORD		wDownloadSize;
	//	WORD		wCRC16;
	CString     strMsg;
	BYTE		bPacketIndex;
	HANDLE		hBinFile = INVALID_HANDLE_VALUE;
	int			iCnt = 0, iCount = 0;
	CString     strFileName;
	//	double      dblValue;
	SYSTEMTIME  st1;
	SYSTEMTIME  st2;

	try
	{
		//strFileName.Format("%s", &pchFileName);
		// 1. 发命令去启动Xmodem
		GetLocalTime(&st1);
		if (!StartFWUpdate())
		{
			return FALSE;
		}
		Sleep(100);

		// 打开bin文件
		hBinFile = CreateFile(strBinFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hBinFile == INVALID_HANDLE_VALUE)
		{
			strMsg.Format("打开文件%s失败", strBinFileName);
			LogInfo(strMsg);
			return FALSE;
		}

		//Get Download.bin Filesize
		dwCodeSizeLeft = GetFileSize(hBinFile, NULL);
		if (dwCodeSizeLeft == 0)
		{
			strMsg.Format("文件%s为空", strFileName);
			LogInfo(strMsg);
			return FALSE;
		}
		// 2. 开始下载文件 
		bPacketIndex = 1;
		dwLoadDataSize = 0;
		dwTotalDataCount = dwCodeSizeLeft;
		iCnt = dwCodeSizeLeft / MAX_DATA_LENGTH;

		m_ctrlProgress.SetRange(0,iCnt);

		wDownloadSize = MAX_DATA_LENGTH;
		while (dwCodeSizeLeft)
		{
			strMsg.Format("升级进度:%d/%d", iCount, iCnt);
			LogInfo(strMsg);
			iCount++;
			m_ctrlProgress.SetPos(iCount);

			ZeroMemory(pbBinData, MAX_DATA_LENGTH);
			if (iCount>iCnt)
			{
				wDownloadSize = dwCodeSizeLeft%MAX_DATA_LENGTH;
			}
			dwCodeSizeLeft -= wDownloadSize;
			// File done, the last block data

			// 从指定Bin文件中读取数据
			ReadFile(hBinFile, pbBinData, wDownloadSize, &dwBytesRead, NULL);
			if (dwBytesRead != wDownloadSize)
			{
				strMsg.Format("读取文件数据错误");
				LogInfo(strMsg);
				CloseHandle(hBinFile);
				return FALSE;
			}
			if (!FWTranSportFW((BYTE*)pbBinData, wDownloadSize, iCount, iCnt + 1))
			{
				//	CloseHandle(hBinFile);
				//	return FALSE;
			}
			//
			bPacketIndex++;

			if (dwCodeSizeLeft == 0)
			{
				bFileDone = TRUE;
			}
		}
		bFunctionOK = TRUE;

		Sleep(1000);
		if (!FWUpdateEnd())
		{
			LogInfo("END ERROR!");
			CloseHandle(hBinFile);
			//m_strTaskStatus = "结束FW升级失败";
			//return FALSE;
		}
		strMsg.Format("开始复位，请等待5s...");
		LogInfo(strMsg);
		Sleep(5000);
		strMsg.Format("下载文件%s成功！", strFileName);
		LogInfo(strMsg);
		GetLocalTime(&st2);
		CTime           ctStartTime;
		CTime           ctEndTime;
		CTimeSpan       timeSpan;

		ctStartTime = st1;
		ctEndTime = st2;

		timeSpan = ctEndTime - ctStartTime;

		int nTotalDiff = timeSpan.GetTotalSeconds();
		strMsg.Format("Start:%d-%d-%d,%d:%d:%d", st1.wYear, st1.wMonth, st1.wDay, st1.wHour, st1.wMinute, st1.wSecond);
		//LogInfo(strMsg);
		strMsg.Format("End:  %d-%d-%d,%d:%d:%d", st2.wYear, st2.wMonth, st2.wDay, st2.wHour, st2.wMinute, st2.wSecond);
		//LogInfo(strMsg);
		strMsg.Format("数据下载时间:%dmin%ds", nTotalDiff / 60, nTotalDiff % 60);
		LogInfo(strMsg);
	}

	catch (TCHAR* pszErrMsg)
	{
		bFunctionOK = FALSE;
		LogInfo(pszErrMsg);
	}
	catch (...)
	{
		bFunctionOK = FALSE;
	}

	if (hBinFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hBinFile);
	}
	return	bFunctionOK;
}


bool CMy126S_45V_Switch_AppDlg::StartFWUpdate()
{
	if (!m_CmdModule.StartFWUpdate())
	{
		LogInfo(m_CmdModule.m_strLogInfo);
		//m_strTaskStatus = "启动FW升级失败";
		return FALSE;
	}
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::FWTranSportFW(unsigned char* byTransData, int nDataLength, int nIndex, int nSum)
{
	if (!m_CmdModule.FWTranSportFW(byTransData, nDataLength, nIndex, nSum))
	{
		LogInfo(m_CmdModule.m_strLogInfo);
		return FALSE;
	}
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::FWUpdateEnd()
{
	if (!m_CmdModule.FWUpdateEnd())
	{
		LogInfo(m_CmdModule.m_strLogInfo);
		return FALSE;
	}
	return TRUE;
}


bool CMy126S_45V_Switch_AppDlg::CreateBinFileZ4671()
{
	CString   strPath;
	DWORD     dwCRCValue = 0;
	CString   strMsg;
	CString   strFWversion;
	CString   strPN;
	DWORD     dwHeader = 64;
	int  i = 0;
	CString   strToken;
	char      sep[] = ",\t\n";
	CString  strToken1;
	int ch;
	CString strTemperature;
	CString strLutFile;
	int nTemp;
	CStdioFile stdPDFile;
	CString strBinFileName;
	CString strNetFile;
	CString strLocalFile;
	FILE    *fp = NULL;
	BOOL    bBinFileExist = FALSE;
	SYSTEMTIME st;
	CString strTime;
	CString strBundle;
	CString strProductType;
	CString strBundleVer;
	CString strPN2;

	LPCTSTR  lpStr = NULL;
	CString strLUTFile;
	CStdioFile stdFile;
	char chLineBuff[MAX_LINE];

	int nSwitchIndex = 0;
	int nBlock;
	int nSwitch;
	int nFileBlock = 0;
	int nFileSwitch = 0;
	int nSwitchNUM;
	int nCHNum;
	int nB;
	int nS;
	int nTempIndex;

	CString strIniFile;
	CString strCH;
	CString strValue;
	int     DarkNum;
	DWORD dwHdrCRC32;
	PBYTE  pbyHeader;
	DWORD dwCRC32Value = 0;
	CString strNewBin;
	CString strNewLocalBin;
	PBYTE pbyDataBin = new BYTE[30000];

	try
	{
		if (m_bIfSaveToServe)
		{
			strPath = m_strNetFile;
		}
		else
		{
			strPath = m_strLocalPath;
		}
		strBinFileName.Format("%s\\data\\%s\\%s\\Cal\\%.0f", strPath, m_strPN, m_strSN, m_pdblWavelength);
		CreateFullPath(strBinFileName);
		strBinFileName.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%s_DataBin-Initial.bin", strPath, m_strPN, m_strSN, m_pdblWavelength, m_strSN);
		
		strLocalFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%s_DataBin-Initial.bin", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength,m_strSN);
		strNetFile.Format("%s\\data\\%s\\%s\\Cal\\%.0f\\%s_DataBin-Initial.bin", m_strNetFile, m_strPN, m_strSN, m_pdblWavelength, m_strSN);

		//数据下载之前先判断bin文件是否存在
		if (GetFileAttributes(strBinFileName) != -1)
		{
			bBinFileExist = TRUE;
		}

		//结构体清零
		ZeroMemory(&pstBundleHeader1, sizeof(stLutBundleHeader1));
		ZeroMemory(&pstBundleHeader2, sizeof(stLutBundleHeader2));
		ZeroMemory(&pstImageHdr, sizeof(stImageHeader));
		ZeroMemory(&m_pstLUTInfoZ4671, sizeof(stLutSettingZ4671));

		//bBundleTag
		strBundle = "OPLINK";
		sprintf((LPTSTR)(pstBundleHeader1.pBundleTag), "%s", strBundle);
		//strcpy((LPTSTR)(pstBundleHeader1.pBundleTag),strBundle);

		//bProductType
		strProductType = "SWITCH";
		memcpy(pstBundleHeader1.pProductType, strProductType.GetBuffer(strProductType.GetLength()), strProductType.GetLength());

		//dwBundleSize
		//pstBundleHeader2.dwBundleSize = SwapDWORD(sizeof(pstBundleHeader1)+sizeof(pstBundleHeader2)+sizeof(pstImageHdr)*2+sizeof(stLutSettingZ4671)+sizeof(stLutSettingZ4671_Add));
		pstBundleHeader2.dwBundleSize = SwapDWORD(sizeof(pstBundleHeader1) + sizeof(pstBundleHeader2) + sizeof(pstImageHdr) * 2 + sizeof(stLutSettingZ4671));
		//wBundleHdrSize
		pstBundleHeader2.wBundleHdrSize = SwapWORD(160);
		//wImageCount
		pstBundleHeader2.wImageCount = SwapWORD(1);
		//bBundleVersion
		strBundleVer = "1.0.0.0";
		memcpy(pstBundleHeader2.pBundleVer, strBundleVer.GetBuffer(strBundleVer.GetLength()), strBundleVer.GetLength());

		strPN2 = "OMSSMCSAMPZAB01";
		//bPartNumber
		memcpy(pstBundleHeader2.pBundlePN, strPN2.GetBuffer(strPN2.GetLength()), strPN2.GetLength());

		// Serial Number of this product,
		memcpy(pstBundleHeader2.pBundleSN, m_strSN.GetBuffer(m_strSN.GetLength()), m_strSN.GetLength());

		//dwTimeStamp
		// The time at which this bundle file generated

		GetLocalTime(&st);

		strTime.Format("%04d.%02d.%02d.%02d.%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
		memcpy(pstBundleHeader2.pBundleTime, strTime.GetBuffer(strTime.GetLength()), strTime.GetLength());

		//lut表的文件头
		//TAG
		pstImageHdr.pLutTag[0] = 0x49;
		pstImageHdr.pLutTag[1] = 0x42;
		pstImageHdr.pLutTag[2] = 0x46;
		pstImageHdr.pLutTag[3] = 0x48;

		//ImageType
		pstImageHdr.byImageType = 0x88;
		//ImageHitless
		pstImageHdr.byHitless = 0x00;
		//StorageID
		pstImageHdr.byStorageID = 0x01;
		//ImageIndex
		pstImageHdr.byImageIndex = 0x00;
		//dwImageVersion
		pstImageHdr.dwImageVersion = 0;
		//dwTimeStamp
		pstImageHdr.pTimeStamp[0] = (BYTE)(st.wMonth);
		pstImageHdr.pTimeStamp[1] = (BYTE)(st.wDay);
		pstImageHdr.pTimeStamp[2] = (BYTE)(st.wHour);
		pstImageHdr.pTimeStamp[3] = (BYTE)(st.wMinute);

		//dwBaseAddress
		pstImageHdr.dwBaseAddress = SwapDWORD(0x65000);

		//dwImageCRC32
		//reserved
		//dwImageSize
		//pstImageHdr.dwImageSize = SwapDWORD(sizeof(stLutSettingZ4671)+sizeof(stLutSettingZ4671_Add));
		pstImageHdr.dwImageSize = SwapDWORD(sizeof(stLutSettingZ4671));
		//SectionCount
		pstImageHdr.bySectionCount = 0x01;

		//sector		
		//1.dwSystemFlag 0x00 length:4
		//system tag
		m_pstLUTInfoZ4671.dwSystemFlag = 0;

		//2.写入产品信息	
		strLUTFile.Format("%s\\Data\\%s\\%s\\Cal\\BIN\\ProductInfo.csv", strPath, m_strPN, m_strSN);
		if (!stdFile.Open(strLUTFile, CFile::modeRead | CFile::typeText))
		{
			strMsg.Format("打开文件%s失败！", strLUTFile);
			LogInfo(strMsg);
			return FALSE;
		}

		//Vendor,16
		//SN,16
		lpStr = stdFile.ReadString(chLineBuff, MAX_LINE);
		strToken = strtok(chLineBuff, sep);
		for (i = 0; i<strToken.GetLength(); i++)
		{
			m_pstLUTInfoZ4671.pchSN[i] = (WORD)strToken.GetAt(i);
		}

		//PN,32bytes
		lpStr = stdFile.ReadString(chLineBuff, MAX_LINE);
		strToken = strtok(chLineBuff, sep);
		for (i = 0; i<strToken.GetLength(); i++)
		{
			m_pstLUTInfoZ4671.pchPN[i] = (WORD)strToken.GetAt(i);
		}

		//HW Version：16
		lpStr = stdFile.ReadString(chLineBuff, MAX_LINE);
		strToken = strtok(chLineBuff, sep);
		for (i = 0; i<strToken.GetLength(); i++)
		{
			m_pstLUTInfoZ4671.pchHwVer[i] = (WORD)strToken.GetAt(i);
		}

		//MFG Date：16
		lpStr = stdFile.ReadString(chLineBuff, MAX_LINE);
		strToken = strtok(chLineBuff, sep);
		for (i = 0; i<strToken.GetLength(); i++)
		{
			m_pstLUTInfoZ4671.pchMfgDate[i] = (WORD)strToken.GetAt(i);
		}

		//reserved
		//CalibDateDate:16
		lpStr = stdFile.ReadString(chLineBuff, MAX_LINE);
		strToken = strtok(chLineBuff, sep);
		for (i = 0; i<strToken.GetLength(); i++)
		{
			m_pstLUTInfoZ4671.pchCalibDate[i] = (WORD)strToken.GetAt(i);
		}
		stdFile.Close();

		////////////////////////////
		//报警温度 
		m_pstLUTInfoZ4671.wAlarmMaxTemperature = 850;
		m_pstLUTInfoZ4671.wAlarmMinTemperature = -50;

		//读PD斜率
		strLUTFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_KC.csv", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength, 1);//只有1个PD
		if (!stdFile.Open(strLUTFile, CFile::modeRead | CFile::typeText))
		{
			strMsg.Format("打开文件%s失败！", strLUTFile);
			LogInfo(strMsg);
		}
		else
		{
			stdFile.ReadString(chLineBuff, MAX_LINE);
			int nCount = 0;
			double dbK = 0;
			double dbC = 0;

			lpStr = stdFile.ReadString(chLineBuff, MAX_LINE);
			if (lpStr == NULL)
				return FALSE;
			strToken = strtok(chLineBuff, sep);
			dbK = atof(strToken);
			dbC = atof(strtok(NULL, sep));
			for (int i = 0; i < 16; i++)
			{
				for (nTemp = 0; nTemp < 4; nTemp++)
				{
					m_pstLUTInfoZ4671.sPDSlope[i][nTemp] = dbK;
					m_pstLUTInfoZ4671.sPDIntercept[i][nTemp] = dbC;
				}
			}

		}
		
		//通道定标数据
		//写入开关通道数据
		//nCHNum = m_stSWPara.nCHCount;
		for (nB = 0;nB<3;nB++)
		{
			if (nB == 0)
			{
				nSwitchNUM = 16;//add //
			}
			else if (nB == 1)
			{
				nSwitchNUM = 16;//dex
			}
			else if (nB == 2)
			{
				nSwitchNUM = 2;//mo
			}
			for (nS = 0;nS<nSwitchNUM;nS++)
			{
				for (nTemp = 0;nTemp<4;nTemp++)//0 IS LOW ；1 IS ROOM；2 IS HIGH；3 IS RESE
				{
					ZeroMemory(chLineBuff, MAX_LINE);
					if (nB == 0)
					{
						nBlock = 1; //TX Data
						nSwitch = nS;
					}
					else if (nB == 1)
					{
						nBlock = 0; //RX Data
						nSwitch = nS;
					}
					else if (nB == 2)
					{
						nBlock = 2;
						if (nS == 0)
						{
							nSwitch = 1;
						}
						else if (nS == 1)
						{
							nSwitch = 0;
						}
					}
					
					if (nB == 1) //DM
					{
						if (SaveDataToCSV(VOL_DATA, nS, TRUE, nTemp))
						{
							for (ch = 0;ch<m_stSNList[nS].nCHCount*m_stSNList[nS].nCOMCount;ch++)
							{
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = ScanPowerChannal[ch].VoltageX;
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = ScanPowerChannal[ch].VoltageY;
							}
						}
						else
						{
							for (ch = 0;ch<m_stSNList[nS].nCHCount*m_stSNList[nS].nCOMCount;ch++)
							{
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = nS+1;
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = nS+1;
							}
						}
					}
					else if(nB==2) //MON
					{
						if (SaveDataToCSV(VOL_DATA, nS+31, TRUE, nTemp))//32？
						{
							for (ch = 0;ch<m_stSNList[nS + 31].nCHCount*m_stSNList[nS + 31].nCOMCount;ch++)
							{
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = ScanPowerChannal[ch].VoltageX;
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = ScanPowerChannal[ch].VoltageY;
							}
						}
						else
						{
							if (m_stSNList[nS + 31].nCHCount*m_stSNList[nS + 31].nCOMCount <= 0 || m_stSNList[nS + 31].nCHCount*m_stSNList[nS + 31].nCOMCount > 33)
							{
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = 0;
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = 0;
							}
							for (ch = 0;ch<m_stSNList[nS + 31].nCHCount*m_stSNList[nS + 31].nCOMCount;ch++)//如果是-1是没有33的开打1X8
							{
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = nS + 1;
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = nS + 1;
							}
						}
						/*
						for (ch = 0;ch<m_stSNList[nS].nCHCount*m_stSNList[nS].nCOMCount;ch++)
						{
							m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = nS+1;
							m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = nS+1;
						}
						*/
					}
					else //AM17~32
					{
						if (SaveDataToCSV(VOL_DATA, nS + 16, TRUE, nTemp))
						{
							for (ch = 0;ch<m_stSNList[nS + 15].nCHCount*m_stSNList[nS + 15].nCOMCount;ch++)//16？
							{
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = ScanPowerChannal[ch].VoltageX;
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = ScanPowerChannal[ch].VoltageY;
							}
						}
						else
						{
							for (ch = 0;ch<m_stSNList[nS + 15].nCHCount*m_stSNList[nS + 15].nCOMCount;ch++)//16？
							{
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = nS + 1;
								m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = nS + 1;
							}
						}
						/*
						for (ch = 0;ch<m_stSNList[nS].nCHCount*m_stSNList[nS].nCOMCount;ch++)
						{
						m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = nS+1;
						m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = nS+1;
						}
						*/
					}
					if(m_stSNList[nS].nCHCount*m_stSNList[nS].nCOMCount<8)
					{
						for (ch = m_stSNList[nS].nCHCount*m_stSNList[nS].nCOMCount-1;ch<8;ch++)
						{
							m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][0] = nS+1;
							m_pstLUTInfoZ4671.wCalibPtrDAC[nB * 16 + nS][nTemp][ch][1] = nS+1;
						}

					}
				}

				for (ch = 0;ch<PORT_MAX_COUNT;ch++)
				{
					m_pstLUTInfoZ4671.wMidPointSelection[nB * 16 + nS][ch] = 1;
				}
			}
			
			//Temp
			for (nS = 0;nS<nSwitchNUM;nS++)
			{
				for (nTemp = 0;nTemp<4;nTemp++)
				{
					if (nB == 0)
					{
						nBlock = 1;
						nSwitch = nS;
					}
					else if (nB == 1)
					{
						nBlock = 0;
						nSwitch = nS;
					}
					else if (nB == 2)
					{
						nBlock = 2;
						if (nS == 0)
						{
							nSwitch = 1;
						}
						else if (nS == 1)
						{
							nSwitch = 0;
						}
					}	
					if (nTemp == IDX_TEMP_LOW)
					{
						m_pstLUTInfoZ4671.wTemperaturePoint[nB * 16 + nS][nTemp] = 0;
					}
					else
					{
						m_pstLUTInfoZ4671.wTemperaturePoint[nB * 16 + nS][nTemp] = 300;
					}

				}
			}
		}

		nCHNum = m_nCHCount;
		//SWChannelMapping;	
		for (nBlock = 0;nBlock<3;nBlock++)
		{
			if (nBlock == 2)
			{
				nSwitchNUM = 2;
			}
			else
			{
				nSwitchNUM = 16;
				//nCHNum = ;
			}
			for (nSwitch = 0;nSwitch<nSwitchNUM;nSwitch++)
			{
				for (ch = 0;ch<nCHNum;ch++)//这里更新了nCHNum
				{
					m_pstLUTInfoZ4671.w1x8SWChannelMapping[nBlock * 16 + nSwitch][ch] = ch + 1;
				}
			}
		}
		//DarkMidPoint
		DarkNum = m_nCHCount+1;
		for (ch = 0;ch<34;ch++)
		{
			m_pstLUTInfoZ4671.wDarkMidPoint[ch] = DarkNum;
		}
		m_pstLUTInfoZ4671.wMaxChannelNUM = m_nCHCount;

		//开关切换时间 10ms = 50*200us；
		m_pstLUTInfoZ4671.wSwitchDelayUs = 200;  //2015-11-26 RIS

		m_CRC32.InitCRC32();
		ZeroMemory(pbyDataBin, sizeof(pbyDataBin));
		memcpy(pbyDataBin, &m_pstLUTInfoZ4671, sizeof(stLutSettingZ4671));

		for (i = 0;i < sizeof(stLutSettingZ4671) - 4; i++)
		{
			dwCRC32Value = m_CRC32.GetThisCRC(pbyDataBin[i]);  // For K20
															   // dwCRC32Value = m_CRC32.GetThisCRC(pbyDataBin[i^0x01]);  // For C2000 DSP
		}
		dwCRC32Value = ~dwCRC32Value;
		m_pstLUTInfoZ4671.dwCRC32 = dwCRC32Value;
		m_CRC32.InitCRC32();

		//数据文件CRC(包括CRC本身)
		m_CRC32.InitCRC32();
		dwCRC32Value = 0;
		ZeroMemory(pbyDataBin, sizeof(pbyDataBin));
		memcpy(&pbyDataBin[0], &m_pstLUTInfoZ4671, sizeof(stLutSettingZ4671));
		for (i = 0;i < sizeof(stLutSettingZ4671); i++)
		{
			dwCRC32Value = m_CRC32.GetThisCRC(pbyDataBin[i]);
		}

		dwCRC32Value = ~dwCRC32Value;

		//大小端需转换	
		pstImageHdr.dwImageCRC32 = SwapDWORD(dwCRC32Value);

		delete[]pbyDataBin;
		pbyDataBin = NULL;
		//计算header2的CRC

		pbyHeader = new BYTE[sizeof(stLutBundleHeader2) + sizeof(stImageHeader)];
		memcpy(pbyHeader, &pstBundleHeader2, sizeof(stLutBundleHeader2));
		memcpy(&pbyHeader[sizeof(stLutBundleHeader2)], &pstImageHdr, sizeof(stImageHeader));

		m_CRC32.InitCRC32();
		for (i = 0;i < (sizeof(stLutBundleHeader2) + sizeof(stImageHeader)); i++)
		{
			dwHdrCRC32 = m_CRC32.GetThisCRC(pbyHeader[i]);
		}
		dwHdrCRC32 = ~dwHdrCRC32;
		pstBundleHeader1.dwBundleHdrCRC32 = SwapDWORD(dwHdrCRC32);
		delete[]pbyHeader;
		pbyHeader = NULL;
		//查看bin文件是否存在,若存在，则重命名该文件

		GetLocalTime(&st);
	
		fp = fopen(strBinFileName, "wb");
		if (fp == NULL)
		{
			strMsg.Format("打开文件%s错误", strBinFileName);
			LogInfo(strMsg);
			return FALSE;
		}
		fwrite(&pstBundleHeader1, 1, sizeof(stLutBundleHeader1), fp);
		fwrite(&pstBundleHeader2, 1, sizeof(stLutBundleHeader2), fp);
		fwrite(&pstImageHdr, 1, sizeof(stImageHeader), fp);
		fwrite(&pstImageHdr, 1, sizeof(stImageHeader), fp);
		fwrite(&m_pstLUTInfoZ4671, 1, sizeof(stLutSettingZ4671), fp);
		//fwrite(&m_pstLUTInfoZ4671Add,1,sizeof(stLutSettingZ4671_Add),fp);
		fclose(fp);
		if (m_bIfSaveToServe)
		{
			if (!CopyFile(strNetFile, strLocalFile, FALSE))
			{
				Sleep(1000);
				if (!CopyFile(strNetFile, strLocalFile, FALSE))
				{
				}
			}
		}
		strMsg.Format("开始下载文件%s", strBinFileName);
		LogInfo(strMsg);
		//char pchFile[256];
		//memcpy(pchFile, strBinFileName, strBinFileName.GetLength());
		if (!DataDownLoadZ4671(strBinFileName))
		{
			return FALSE;
		}
	}
	catch (TCHAR* pszError)
	{
		
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}


unsigned long CMy126S_45V_Switch_AppDlg::SwapDWORD(unsigned long dwInData)
{
	BYTE* pch = (BYTE*)&dwInData;

	pch[0] ^= pch[3];
	pch[3] ^= pch[0];
	pch[0] ^= pch[3];

	pch[1] ^= pch[2];
	pch[2] ^= pch[1];
	pch[1] ^= pch[2];

	return dwInData;
}


unsigned int CMy126S_45V_Switch_AppDlg::SwapWORD(unsigned int wInData)
{
	BYTE* pch = (BYTE*)&wInData;

	pch[0] ^= pch[1];
	pch[1] ^= pch[0];
	pch[0] ^= pch[1];

	return wInData;
}

void CMy126S_45V_Switch_AppDlg::OnBUTTONDOWNLoad() 
{
	// TODO: Add your control notification handler code here
	//4.数据下载
	if (!CreateLutFile())
	{
		return;
	}
	if (!CreateBinFileZ4671())
	{
		return;
	}
}


void CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboSnIndex()
{
	// TODO: Add your control notificatio
	/*
	CString strValue;
	int 	nIndex = m_cbxSNList.GetCurSel();
	m_cbChannel.ResetContent();
	for (int nCH = 0; nCH <= m_stSNList[nIndex].nCHCount; nCH++)
	{
		strValue.Format("%d", nCH);
		m_cbChannel.AddString(strValue);
	}
	m_cbChannel.SetCurSel(0);*/
}


void CMy126S_45V_Switch_AppDlg::OnBnClickedButtonSetVoltage2()
{
	// TODO: Add your control notification handler code here

	UpdateData();
	CString strVolt;
	SHORT     nVoltX;
	SHORT     nVoltY;
	CString strMsg;
	int i = m_cbChannel.GetCurSel() - 1;
	m_nSNIndex = m_cbxSNList.GetCurSel();
	int nBlockindexTemp = 0;
	if (!m_bTesterPort)
	{
		MessageBox("请先打开对应的串口！");
		return;
	}

	//strMsg.Format("")
	//发送一个回车结束掉监控状态！
	if (m_nSNIndex >= 32)
	{
		if (!m_CmdModule.GetCurrentDAC(3, m_nSNIndex - 32 + 1, &nVoltX, &nVoltY))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}
	
	}
	else if (m_nSNIndex >= 16)
	{
		if (!m_CmdModule.GetCurrentDAC(2, m_nSNIndex - 16 + 1, &nVoltX, &nVoltY))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}
	}
	else
	{
		if (!m_CmdModule.GetCurrentDAC(1, m_nSNIndex + 1, &nVoltX, &nVoltY))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return;
		}	
	}
	strVolt.Format("%d", nVoltX);
	SetDlgItemText(IDC_EDIT_X, strVolt);
	strVolt.Format("%d", nVoltY);
	SetDlgItemText(IDC_EDIT_Y, strVolt);
	UpdateData(FALSE);
	strMsg.Format("DAC %d,%d", nVoltX, nVoltY);
	LogInfo(strMsg);
	LogInfo("OK");
}


void CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIlctTest4()
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	int nchannel = m_cbChannel.GetCurSel() - 1;
	m_nSNIndex = m_cbxSNList.GetCurSel();
	if (!SPRTProductCH(nchannel + 1))
	{
		strMsg.Format("切换到通道%d错误", nchannel + 1);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	else
	{
		strMsg.Format("切换到通道%d成功", nchannel + 1);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_GREEN);
	}

}

bool CMy126S_45V_Switch_AppDlg::OnButtonPdDownload()
{
	// TODO: Add your control notification handler code here
	CString strPDLutFile;
	CString strTemp;
	CStdioFile stdFile;
	CString  strMsg;
	CString  strToken;
	char     chReadBuf[MAX_LINE];
	CString  strCaliDate;
	CString  strMonth;
	CString  strPath;
	double   dblLowTemp = -5;
	double   dblRoomTemp = 25;
	double   dblHighTemp = 65;
	//	int      nValue;

	//自动将设置为初测

	SYSTEMTIME st;
	GetLocalTime(&st);

	
	if (m_bIfSaveToServe)
	{
		strPath = m_strNetFile;
	}
	else
	{
		strPath = g_tszAppFolder;
	}
	CString strBinFileName;
	strBinFileName.Format("%s\\data\\%s\\%s\\Calibration\\Bin\\%s_DataBin-Initial.bin", strPath, m_strPN, m_strSN, m_strSN);

	int nPDCount = 16;
	int nPD = 0;
	//获取PD定标温度
	//读取数据
	for (int nTempt = 0; nTempt<4; nTempt++)
	{
		m_pstLUTInfoZ4671.sTemperaturePoint[nTempt] = -1000;//默认值
		
		for (nPD = 0; nPD<nPDCount; nPD++)
		{
			ZeroMemory(chReadBuf, sizeof(chReadBuf));

			strPDLutFile.Format("%s\\data\\%s\\%s\\PDData\\room\\PD%d_LUT_room-Initial.csv", strPath, m_strPN, m_strSN, nPD + 1);

			if (!stdFile.Open(strPDLutFile, CFile::modeRead))
			{
				strMsg.Format("打开文件%s失败", strPDLutFile);
				LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
				return FALSE;
			}
			strMsg.Format("打开文件%s成功", strPDLutFile);
			//LogInfo(strMsg);
			stdFile.ReadString(chReadBuf, MAX_LINE);
			stdFile.ReadString(chReadBuf, MAX_LINE);
			strToken = strtok(chReadBuf, ",");
			m_pstLUTInfoZ4671.sPDSlope[nPD][nTempt] = atoi(strToken);

			strToken = strtok(NULL, ",");
			m_pstLUTInfoZ4671.sPDIntercept[nPD][nTempt] = atoi(strToken);
			stdFile.Close();
		}

			ZeroMemory(chReadBuf, sizeof(chReadBuf));
		
			strPDLutFile.Format("%s\\data\\%s\\%s\\PDData\\%s\\PD%d_Tempt_%s-Initial.csv", strPath, m_strPN, m_strSN, strTemp, nPD + 1, strTemp);
	
			if (!stdFile.Open(strPDLutFile, CFile::modeRead))
			{
				strMsg.Format("打开文件%s失败", strPDLutFile);
				LogInfo(strMsg, FALSE, COLOR_RED);
				return FALSE;
			}
			stdFile.ReadString(chReadBuf, MAX_LINE);
			strToken = strtok(chReadBuf, ",");
			m_pstLUTInfoZ4671.sTemperaturePoint[nTempt] = (atof(strToken)*10.0); //温度值
			stdFile.Close();

	}
	for (int nTempt = 0; nTempt<4; nTempt++)
	{
		if (fabs(m_pstLUTInfoZ4671.sTemperaturePoint[nTempt] + 1000)<0.01)
		{
			if (nTempt == 0) //低温取常温值
			{
				m_pstLUTInfoZ4671.sTemperaturePoint[nTempt] = m_pstLUTInfoZ4671.sTemperaturePoint[1];//常温值
			}
			else
			{
				m_pstLUTInfoZ4671.sTemperaturePoint[nTempt] = m_pstLUTInfoZ4671.sTemperaturePoint[nTempt - 1];//取上一个温度的值
			}

		}
		strMsg.Format("%d,%d", nTempt + 1, m_pstLUTInfoZ4671.sTemperaturePoint[nTempt]);
		LogInfo(strMsg);
	}
	LogInfo("导入PD数据结束.");
}


void CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIlctTest5()
{
	// TODO: Add your control notification handler code here
	CString strSN;
	m_CmdModule.GetProductSN(strSN.GetBuffer(0));
	CString strPDType;
	double dblDutIL = 0;
	double dblAtten = 0.0;
	CString strMsg;
	DWORD  dwSampleCount;
	double dblChannelWL;
	CString  strPDLutFile;
	CString  strTmptValue;
	HANDLE   hLUTFile = INVALID_HANDLE_VALUE;
	DWORD    dwBytesWrite;
	DWORD    dwBytesReturned;
	CString  strPDPower, strLUTFile;
	double   dblSKData[4][4];
	CString  strPDCalFile;
	CStdioFile stdPDCalFile;
	CString  strValue;
	CStdioFile stdFile;
	int nPDIndex = 0;
	CString   strToken;
	char      sep[] = ",\t\n";
	double dblCurrentTempt = m_CmdModule.GetTemperature();
	CString  strNetFile;
	HANDLE	hDirectory = INVALID_HANDLE_VALUE;
	LPCTSTR  lpStr = NULL;
	char chLineBuff[MAX_LINE];
	ZeroMemory(dblSKData, sizeof(dblSKData));
	//定标参数
	if (!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}
	dblChannelWL = 1550.00;
	double dblMaxPower;
	double dblMinPower;
	double dblStep = 0.5;
	double dblCalPDPower[4][100];
	double dblCalPDADC[4][100];
	int    nFlag1 = 0;
	int    nFlag2 = 0;
	int    nFlag3 = 0;
	int    nFlag4 = 0;
	int    nPDValue;

	m_ctrlListMsg.ResetContent();
	double dblOffset = 0;
	double dblCurrent = 0;
	double dbCalPowerCount[4] = { 3.0,0.0,-5.0,-10.0 };
	int inPM = m_cbSetPDBox.GetCurSel();
	if (inPM == 0)
	{
		SetPWMWL(0, dblChannelWL);
	}
/*

	if (!SetTLSParameter(m_dblTLSPower, dblChannelWL))
	{
		LogInfo("设置激光器参数失败！", (BOOL)FALSE, COLOR_RED);
		return FALSE;
	}
	//	double dblReadAtten;

	if (!setVOAAtten(m_dblVOAIL))
	{
		LogInfo("设置VOA衰减出错！", (BOOL)FALSE, COLOR_RED);
		return FALSE;
	}
	dblCurrent = ReadPWMPower(3);

	Sleep(100);
	SPRTSwitchBox(2, nPDIndex + 1);  //打通入光通道*/


	strNetFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f", m_strNetFile, m_strPN, m_strSN, m_pdblWavelength);
	CreateFullPath(strNetFile);
	strNetFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength);
	CreateFullPath(strNetFile);
	strNetFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_CAL.csv", m_strNetFile, m_strPN, m_strSN, m_pdblWavelength, nPDIndex + 1);
	strPDCalFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_CAL.csv", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength, nPDIndex + 1);
	if (!stdPDCalFile.Open(strPDCalFile, CFile::modeCreate | CFile::modeWrite))
	{
		strMsg.Format("创建文件%s失败！", stdPDCalFile);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	strValue.Format("PDPower,PDADC\n");
	stdPDCalFile.WriteString(strValue);

	double dblCurPowe;
	double dblRefPower;
	// Trans 1 MCS#2个带PD
	// 1X8开到8，接面板的2，#1，1X64 切到33，第#2，1x18切到 17 通道是1
	//生成BIN
	//下载
	for (int i = 0; i<4; i++)
	{
		YieldToPeers();

		if (inPM == 0)
		{
			strMsg.Format("光路连接方法，光源->VOA->PM0（1550.00nm） 手动调VOA，光功率%.2fdBm,进行归零", dbCalPowerCount[i]);
			MessageBox(strMsg);
			dblCalPDPower[0][i] = ReadPWMPower(0);    //记录当前光强
		}
		if (inPM == 1)
		{
			strMsg.Format("光路连接方法，光源->PM0（1550.00nm）自动调VOA，光功率%.2fdBm,进行归零", dbCalPowerCount[i]);
			MessageBox(strMsg);
			SetTargetPower(dbCalPowerCount[i]);
			m_PDBoxCmd.GetSiglePower(1, &dblCalPDPower[0][i]);
		}

		if (fabs(dblCalPDPower[0][i] - dbCalPowerCount[i]) > 0.2)
		{
			strMsg.Format("光路连接方法VOA没有调整，请确认");
			MessageBox(strMsg);
			return;
		}
		if (m_bTestStop)
		{
			MessageBox("测试终止");
			stdPDCalFile.Close();
			return;
		}
		strMsg.Format("光路连接方法，光源->VOA->接到产品PD端，输光为%.2fdBm,开始进行定标第%d共4个点", dblCalPDPower[0][i],i);
		MessageBox(strMsg);
		m_CmdModule.GetProductPDADC(nPDIndex + 1, &nPDValue);

		dblCalPDADC[0][i] = nPDValue;
		if (nPDValue>65000)
		{
			MessageBox("PD Input Power ERROR!");
			stdPDCalFile.Close();
			return;
		}
		strValue.Format("%.2f,%.0f\n", dblCalPDPower[0][i], dblCalPDADC[0][i]);
		stdPDCalFile.WriteString(strValue);
		
		strMsg.Format("PD%d,Input Power:%.3fdBm;ADC:%d", nPDIndex + 1, dblCalPDPower[0][i], nPDValue);
		LogInfo(strMsg);
	}
	stdPDCalFile.Close();

	//写入温度文件
	CString strTemptFile;
	strNetFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_Tempt.csv", m_strNetFile, m_strPN, m_strSN, m_pdblWavelength, nPDIndex + 1 );
	strTemptFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_Tempt.csv", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength, nPDIndex + 1);
	hLUTFile = CreateFile(strTemptFile, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hLUTFile == INVALID_HANDLE_VALUE)
	{
		LogInfo("创建PD温度文件失败！", (BOOL)FALSE, COLOR_RED);
		return;
	}
	strTmptValue.Format("%.1f\n", dblCurrentTempt);
	dwBytesWrite = strTmptValue.GetLength();
	WriteFile(hLUTFile, strTmptValue, dwBytesWrite, &dwBytesReturned, NULL);
	CloseHandle(hLUTFile);
	/////
	strLUTFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_CAL.csv", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength,nPDIndex + 1);
	if (!stdFile.Open(strLUTFile, CFile::modeRead | CFile::typeText))
	{
		strMsg.Format("打开文件%s失败！", strLUTFile);
		LogInfo(strMsg);
		return;
	}
	stdFile.ReadString(chLineBuff, MAX_LINE);
	int nCount = 0;
	while (1)
	{
		lpStr = stdFile.ReadString(chLineBuff, MAX_LINE);
		if (lpStr == NULL)
		{
			break;
		}
		strToken = strtok(chLineBuff, sep);
		dblCalPDPower[0][nCount] = atof(strToken);
		dblCalPDADC[0][nCount] = atoi(strtok(NULL, sep));
		nCount++;
	}

	//计算斜率、截距
	m_Calpolyn.EMatrix(dblCalPDADC[0], dblCalPDPower[0], 4, 2, dblSKData[0]);
	//存入PD_LUT文件;
	strNetFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_KC.csv", m_strNetFile, m_strPN, m_strSN, m_pdblWavelength, nPDIndex + 1 );
	strPDLutFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_KC.csv", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength, nPDIndex + 1);
	hLUTFile = CreateFile(strPDLutFile, GENERIC_WRITE | GENERIC_READ,
		0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hLUTFile == INVALID_HANDLE_VALUE)
	{
		LogInfo("创建KC文件失败！", (BOOL)FALSE, COLOR_RED);
		return;
	}
	strTmptValue.Format("K*1000000,C*1000000\n");
	dwBytesWrite = strTmptValue.GetLength();
	WriteFile(hLUTFile, strTmptValue, dwBytesWrite, &dwBytesReturned, NULL);
	strTmptValue.Format("%f,%f\n", dblSKData[0][2] * 1000000, dblSKData[0][1] * 1000000);  //斜率截距放大10w倍
	dwBytesWrite = strTmptValue.GetLength();
	WriteFile(hLUTFile, strTmptValue, dwBytesWrite, &dwBytesReturned, NULL);

	CloseHandle(hLUTFile);
	LogInfo("PD定标完成！", (BOOL)FALSE, COLOR_BLUE);

	MessageBox("PD定标完毕！");

}
BOOL CMy126S_45V_Switch_AppDlg::SetTargetPower(double dbPower)
{
	CString strMsg;
	double dbAttTemp=0.0;
	double dblCalPDPower = 0.0;
	if (!m_CmdInstrument.SetVOAxAtten(1, 0.0))
	{
		return FALSE;
	}
	int nerror = 0;
	Sleep(100);
	while (1)
	{
		m_PDBoxCmd.GetSiglePower(1, &dblCalPDPower);

		if (fabs(dbPower - dblCalPDPower) > 0.1)
		{
			if (dblCalPDPower - dbPower < 0 && dbAttTemp < 0.5)
			{
				strMsg.Format("光源功率不够，请确认");
				LogInfo(strMsg, strMsg.GetAllocLength(), COLOR_RED);
				return FALSE;
			}
			else
			{
				dbAttTemp += dblCalPDPower - dbPower;
				if (!m_CmdInstrument.SetVOAxAtten(1, dbAttTemp))
				{
					return FALSE;
				}
				nerror++;
				if (nerror > 10)
				{
					strMsg.Format("光功率调不到位，请确认");
					LogInfo(strMsg, strMsg.GetAllocLength(), COLOR_RED);
					return FALSE;
				}
				Sleep(100);
			}
		}
		else
		{
			break;
		}
	}
	return TRUE;
}

void CMy126S_45V_Switch_AppDlg::OnBnClickedButtonIlctTest6()
{
	// TODO: Add your control notification handler code here

	CString strSN;
	m_CmdModule.GetProductSN(strSN.GetBuffer(0));
	CString strPDType;
	double dblDutIL = 0;
	double dblAtten = 0.0;
	CString strMsg;
	DWORD  dwSampleCount;
	double dblChannelWL;
	CString  strPDLutFile;
	CString  strTmptValue;
	HANDLE   hLUTFile = INVALID_HANDLE_VALUE;
	DWORD    dwBytesWrite;
	DWORD    dwBytesReturned;
	CString  strPDPower;
	double   dblSKData[4][4];
	stSWScanPara stScanPara;
	CString  strPDVerFile;
	CStdioFile stdPDCalFile;
	CString  strValue;
	int nPDIndex = 0;
	double dblCurrentTempt = m_CmdModule.GetTemperature();
	CString  strNetFile;
	HANDLE	hDirectory = INVALID_HANDLE_VALUE;
	ZeroMemory(dblSKData, sizeof(dblSKData));
	//定标参数
	if (!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}
	dblChannelWL = 1550.00;
	double dblMaxPower;
	double dblMinPower;
	double dblStep = 0.5;
	double dblVerPDPower[4][100];
	double dblPDPower[4][100];
	double dblVerPDADC[4][100];
	int    nFlag1 = 0;
	int    nFlag2 = 0;
	int    nFlag3 = 0;
	int    nFlag4 = 0;
	int    nPDValue;

	m_ctrlListMsg.ResetContent();
	double dblOffset = 0;
	double dblCurrent = 0;
	double dbCalPowerCount[4] = { 3.0,0.0,-5.0,-10.0 };
	int inPM = m_cbSetPDBox.GetCurSel();
	if (inPM == 0)
	{
		SetPWMWL(0, dblChannelWL);

	}
	/*

	if (!SetTLSParameter(m_dblTLSPower, dblChannelWL))
	{
	LogInfo("设置激光器参数失败！", (BOOL)FALSE, COLOR_RED);
	return FALSE;
	}
	//	double dblReadAtten;

	if (!setVOAAtten(m_dblVOAIL))
	{
	LogInfo("设置VOA衰减出错！", (BOOL)FALSE, COLOR_RED);
	return FALSE;
	}
	dblCurrent = ReadPWMPower(3);

	Sleep(100);
	SPRTSwitchBox(2, nPDIndex + 1);  //打通入光通道*/




	strNetFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_VER.csv", m_strNetFile, m_strPN, m_strSN, m_pdblWavelength,nPDIndex + 1);
	strPDVerFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_VER.csv", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength,nPDIndex + 1);
	if (!stdPDCalFile.Open(strPDVerFile, CFile::modeCreate | CFile::modeWrite))
	{
		strMsg.Format("创建文件%s失败！", stdPDCalFile);
		LogInfo(strMsg, (BOOL)FALSE, COLOR_RED);
		return;
	}
	strValue.Format("GetPM,PDPDPower,PDADC,Acc\n");
	stdPDCalFile.WriteString(strValue);

	double dblCurPowe;
	double dblRefPower;
	for (int i = 0; i<4; i++)
	{
		YieldToPeers();
		strMsg.Format("光路连接方法，光源->VOA->PM0（1550.00nm） 手动调VOA光功率%.2fdBm,进行归零", dbCalPowerCount[i]);
		MessageBox(strMsg);
		if (inPM == 0)
		{
			dblVerPDPower[0][i] = ReadPWMPower(0);    //记录当前光强
		}
		if (inPM == 1)
		{
			strMsg.Format("光路连接方法，光源->PM0（1550.00nm）自动调VOA，光功率%.2fdBm,进行归零", dbCalPowerCount[i]);
			MessageBox(strMsg);
			SetTargetPower(dbCalPowerCount[i]);
			m_PDBoxCmd.GetSiglePower(1, &dblVerPDPower[0][i]);
		}
		//dblVerPDPower[0][i] = ReadPWMPower(0);    //记录当前光强
		if (m_bTestStop)
		{
			MessageBox("测试终止");
			stdPDCalFile.Close();
			return;
		}
		strMsg.Format("光路连接方法，光源->VOA->接到产品PD端，输光为%.2fdBm,开始进行验证，第%d共4个点", dblVerPDPower[0][i], i);
		MessageBox(strMsg);
		dblPDPower[0][i] = m_CmdModule.GetPDPower(nPDIndex + 1);
		if (fabs(dblVerPDPower[0][i] - dbCalPowerCount[i]) > 0.2)
		{
			strMsg.Format("光路连接方法VOA没有调整，请确认");
			MessageBox(strMsg);
			return;
		}
		m_CmdModule.GetProductPDADC(nPDIndex + 1, &nPDValue);
		dblVerPDADC[0][i] = nPDValue;
		
		strValue.Format("%.2f,%.2f,%.0f,%.2f\n", dblVerPDPower[0][i], dblPDPower[0][i], dblVerPDADC[0][i], dblPDPower[0][i] - dblVerPDPower[0][i]);
		stdPDCalFile.WriteString(strValue);

		if (fabs(dblPDPower[0][i] - dblVerPDPower[0][i]) > 0.5)
		{
			strMsg.Format("PD%d,PM Power:%.3fdBm,PD Power:%.3fdBm;ADC:%.0f,精度：%.2f 验证不合格", nPDIndex + 1, dblVerPDPower[0][i], dblPDPower[0][i],dblVerPDADC[0][i], dblPDPower[0][i] - dblVerPDPower[0][i]);
			LogInfo(strMsg, FALSE, COLOR_RED);
		}
		else
		{
			strMsg.Format("PD%d,PM Power:%.3fdBm,PD Power:%.3fdBm;ADC:%.0f,精度：%.2f 验证合格", nPDIndex + 1, dblVerPDPower[0][i], dblPDPower[0][i], dblVerPDADC[0][i], dblPDPower[0][i] - dblVerPDPower[0][i]);
			LogInfo(strMsg);
		}


	}
	stdPDCalFile.Close();

	//写入温度文件
	CString strTemptFile;
	strNetFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_Tempt.csv", m_strNetFile, m_strPN, m_strSN, m_pdblWavelength,nPDIndex + 1);
	strTemptFile.Format("%s\\data\\%s\\%s\\PDData\\%.0f\\PD%d_Tempt.csv", m_strLocalPath, m_strPN, m_strSN, m_pdblWavelength,nPDIndex + 1);
	hLUTFile = CreateFile(strTemptFile, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hLUTFile == INVALID_HANDLE_VALUE)
	{
		LogInfo("创建PD温度文件失败！", (BOOL)FALSE, COLOR_RED);
		return;
	}
	strTmptValue.Format("%.1f\n", dblCurrentTempt);
	dwBytesWrite = strTmptValue.GetLength();
	WriteFile(hLUTFile, strTmptValue, dwBytesWrite, &dwBytesReturned, NULL);
	CloseHandle(hLUTFile);


	
	LogInfo("PD验证完成！", (BOOL)FALSE, COLOR_BLUE);

	MessageBox("PD验证完毕！");
}
bool CMy126S_45V_Switch_AppDlg::ScanZoneEXSigleCH(int nPort, int nSwitchIndex)
{
	CString strMsg;
	WORD   wScanMaxIndex;
	SHORT sDACx=0;
	SHORT sDACy=0;
	SHORT sScanMinVx=0;
	SHORT sScanMaxVx=0;
	SHORT sScanMinVy=0;
	SHORT sScanMaxVy=0;
	WORD  wStep = 64;//1 chunliangw
	WORD  wRowIndex;
	WORD  wColumnIndex;
	BOOL  bAdjust = FALSE;
	CString strValue;
	CString strToken;
	CString strConfigFile;
	stSWScanPara stScanPara;
	double  dblPower;
	double  dblRef;
	int nBlock = 0;
	int nSwitch = 0;
	int nScanPoint;
	char pchRead[128];
	int nReadCount;
	double dblIL;
	ZeroMemory(pchRead, sizeof(pchRead));
	stScanPara.nBlock = nBlock;
	stScanPara.nSwitch = nSwitch;
	//stSWScanPara stScanPara;
	int nfinch[2][2] = {0,0};
	double pdpwer[64] = { -99.0 };
	int nmaxid[64] = {0};
//	SHORT sDACVXoltage = 0;
//  SHORT sDACVYoltage = 0;
	//ADJUST:
	ZeroMemory(pdpwer, sizeof(double) * 64);
	//m_Xbase[nPort] = m_Xbase[nPort] * MAX_DAC / MAX_VOLTAGE; //电压转DAC
	//m_Ybase[nPort] = m_Ybase[nPort] * MAX_DAC / MAX_VOLTAGE;
	//sDACVXoltage = m_Xbase[nPort];
	//sDACVYoltage = m_Ybase[nPort];

	sDACx = m_Xbase[nPort] * MAX_DAC / MAX_VOLTAGE; //电压转DAC
	sDACy = m_Ybase[nPort] * MAX_DAC / MAX_VOLTAGE;

	strMsg.Format("%d通道基准点为:(%d,%d)", nPort + 1, sDACx, sDACy);
	LogInfo(strMsg);

	while (wStep)
	{
		sScanMinVx = sDACx - wStep*15;
		sScanMaxVx = sDACx + wStep*16;
		sScanMinVy = sDACy - wStep*15;
		sScanMaxVy = sDACy + wStep*16;
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

		nfinch[0][0] = sScanMinVx;
		nfinch[0][1] = sScanMinVy;

		nScanPoint = ((sScanMaxVx - sScanMinVx) / wStep + 1)*((sScanMaxVy - sScanMinVy) / wStep + 1);
		stScanPara.nStartX = sScanMinVx;
		stScanPara.nEndX = sScanMaxVx;
		stScanPara.nStartY = sScanMinVy;
		stScanPara.nEndY = sScanMaxVy;
		stScanPara.nStep = wStep;

		stScanPara.nBlock = 0;
		stScanPara.nSwitch = nSwitchIndex;
		if (nSwitchIndex >= 16)
		{
			stScanPara.nBlock = 1;
			stScanPara.nSwitch = nSwitchIndex - 16;
		}
		if (nSwitchIndex >= 32)
		{
			stScanPara.nBlock = 2;
			stScanPara.nSwitch = nSwitchIndex - 32;
		}


		strMsg.Format("Point:%d", nScanPoint);
		LogInfo(strMsg);

		if (!m_PDBoxCmd.NewSendPDMon(nPort+1, 1024))
		{
			Sleep(100);
			if (!m_PDBoxCmd.NewSendPDMon(nPort+1, 1024))
			{
				LogInfo(m_PDBoxCmd.m_strErrorLog, COLOR_RED);
				return FALSE;
			}

		}
		strMsg.Format("Scan:%d,%d,%d,%d,%d", sScanMinVx, sScanMaxVx, sScanMinVy, sScanMaxVy, wStep);
		LogInfo(strMsg);
		if (!m_CmdModule.SendScanTrig(stScanPara))
		{
			if (!m_CmdModule.SendScanTrig(stScanPara))
			{
				strMsg.Format("发送Trig失败！");
				LogInfo(strMsg);
				return FALSE;
			}
		}
		int error = 0;
		Sleep(5000);
		if (!m_PDBoxCmd.NewGetPDTriCout(nPort))
		{
			LogInfo(m_PDBoxCmd.m_strErrorLog, COLOR_RED);
			return FALSE;
		}
		
		if (!m_PDBoxCmd.NewGetMaxPdCout(nPort,1024, &nmaxid[0],&pdpwer[0],0.01))//(nPort, 1024, 0.01, &wScanMaxIndex))//
		{
			if (!m_PDBoxCmd.NewGetMaxPdCout(nPort, 1024, 0.01, &wScanMaxIndex))////(nPort, 1024, 0.01, &wScanMaxIndex))
			{
				if (!m_PDBoxCmd.NewGetMaxPdCout(nPort, 1024, 0.01, &wScanMaxIndex))//(nPort, 1024, 0.01, &wScanMaxIndex))
				{
					LogInfo(m_PDBoxCmd.m_strErrorLog, COLOR_RED);
					return FALSE;
				}
			}
		}
		/*
		if (!m_PDBoxCmd.NewGetMaxPdCout(nPort, 1024, 0.01, &wScanMaxIndex))
		{
			LogInfo(m_PDBoxCmd.m_strErrorLog, COLOR_RED);
			return FALSE;
		}
		*/
		double pdchpow = 0.0;
		wScanMaxIndex = nmaxid[nPort];
		pdchpow = pdpwer[nPort];
		if (pdchpow < -30.0)
		{
			strMsg.Format("无光 %.2f。", pdchpow);
			LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);
			return FALSE;
		}
		wScanMaxIndex--;

		if (wScanMaxIndex == 0)
		{
			strMsg.Format("没有找到区域(%d,%d,%d,%d,%d)的IL最小点。", sScanMinVx, sScanMaxVx, sScanMinVy, sScanMaxVy, wStep);
			LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);

			return FALSE;
		}
		
		//求取该序号对应的坐标
		wRowIndex = (wScanMaxIndex >> 5) & 0x1F;
		wColumnIndex = wScanMaxIndex & 0x1F;
		if (wRowIndex & 0x01)
		{
			wColumnIndex = 0x1F - wColumnIndex;
		}
		sDACx = nfinch[0][0] + wColumnIndex * wStep;
		sDACy = nfinch[0][1] + wRowIndex * wStep;
/*
		if (m_nSNIndex >= 32)
		{
			if (!m_CmdModule.SetSwitchDAC(3, m_nSNIndex - 32 + 1, sDACx, 0))
			{
				strMsg = m_CmdModule.m_strLogInfo;
				LogInfo(strMsg);
				return FALSE;
			}
			if (!m_CmdModule.SetSwitchDAC(3, m_nSNIndex - 32 + 1, sDACy, 1))
			{
				strMsg = m_CmdModule.m_strLogInfo;
				LogInfo(strMsg);
				return FALSE;
			}

		}
		else if (m_nSNIndex >= 16)
		{
			if (!m_CmdModule.SetSwitchDAC(2, m_nSNIndex - 16 + 1, sDACx, 0))
			{
				strMsg = m_CmdModule.m_strLogInfo;
				LogInfo(strMsg);
				return FALSE;
			}
			if (!m_CmdModule.SetSwitchDAC(2, m_nSNIndex - 16 + 1, sDACy, 1))
			{
				strMsg = m_CmdModule.m_strLogInfo;
				LogInfo(strMsg);
				return FALSE;
			}

		}
		else
		{
			if (!m_CmdModule.SetSwitchDAC(1, m_nSNIndex + 1, sDACx, 0))
			{
				strMsg = m_CmdModule.m_strLogInfo;
				LogInfo(strMsg);
				return FALSE;
			}
			if (!m_CmdModule.SetSwitchDAC(1, m_nSNIndex + 1, sDACy, 1))
			{
				strMsg = m_CmdModule.m_strLogInfo;
				LogInfo(strMsg);
				return FALSE;
			}

		}
		*/
		/*
		CString  strmas = "";
		//double pdchpow;
		m_PDBoxCmd.GetSiglePower(nPort, &pdchpow);
		strMsg.Format("%.2f", pdchpow);
		LogInfo(strMsg);
		*/
		ScanPowerChannal[nPort].VoltageX = sDACx;
		ScanPowerChannal[nPort].VoltageY = sDACy;

		strMsg.Format("Port%d序号：%d,功率点为：%.2f,最小点坐标：(%d,%d)", nPort + 1, wScanMaxIndex, pdchpow, sDACx, sDACy);
		LogInfo(strMsg, strMsg.GetLength() + 1);	
		
		wStep = (wStep >> 2);
		/*
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
		wStep = (wStep >> 2);*/
	}
	Sleep(100);
	//发送一个回车结束掉监控状态！
	if (m_nSNIndex >= 32)
	{
		if (!m_CmdModule.SetSwitchDAC(3, m_nSNIndex - 32 + 1, sDACx, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}
		if (!m_CmdModule.SetSwitchDAC(3, m_nSNIndex - 32 + 1, sDACy, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}

	}
	else if (m_nSNIndex >= 16)
	{
		if (!m_CmdModule.SetSwitchDAC(2, m_nSNIndex - 16 + 1, sDACx, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}
		if (!m_CmdModule.SetSwitchDAC(2, m_nSNIndex - 16 + 1, sDACy, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}

	}
	else
	{
		if (!m_CmdModule.SetSwitchDAC(1, m_nSNIndex + 1, sDACx, 0))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}
		if (!m_CmdModule.SetSwitchDAC(1, m_nSNIndex + 1, sDACy, 1))
		{
			strMsg = m_CmdModule.m_strLogInfo;
			LogInfo(strMsg);
			return FALSE;
		}

	}

	Sleep(500);
	m_PDBoxCmd.GetPDPower(pdpwer);
	dblRef = m_dblReferencePower[nPort];
	dblIL = dblRef - pdpwer[nPort];
	strMsg.Format("Port:%d,X:%d,Y:%d,IL:%.2fdB,Power:%.2fdBm", nPort + 1, sDACx, sDACy, dblIL, pdpwer[nPort]);
	LogInfo(strMsg);
	ScanPowerChannal[nPort].dblIL = dblIL;
	if (dblIL > dblRefIL)
	{
		strMsg.Format("通道%dIL超标>%.2fdB!", nPort + 1, dblRefIL);
		//MessageBox(strMsg);
		LogInfo(strMsg, 1, COLOR_RED);
		//return FALSE;
	}

	return TRUE;
}
/*
BOOL CMy126S_45V_Switch_AppDlg::ScanZoneEXSigleCH(WORD  wStep, int nPow, int nPort)
{
	CString strMsg;
	WORD  wScanMaxIndex;
	double pdpwer[64];
	int nmaxid[64];
	double pdpwer1[64];
	int nmaxid1[64];
	SHORT sDACx;
	SHORT sDACy;
	SHORT sScanMinVx;
	SHORT sScanMaxVx;
	SHORT sScanMinVy;
	SHORT sScanMaxVy;
	//WORD  wStep = 64;
	WORD  wRowIndex;
	WORD  wColumnIndex;
	BOOL  bAdjust = FALSE;
	CString strValue;
	CString strConfigFile;
	CString strtempcmd, strrecv;
	int nfinch[66][2] = { 0 };
	CString strUISendPN;
	int nScanPoint = 0;
	stSWScanPara stScanPara;
	int ncurfindch;
	int nscanrealch = 0, nscanreal = 0;
	strrecv = "";

	//wStep =64;

	//	for (int dacx = start; dacx <= end; dacx += step)

	//		for (int dacy = start; dacy <= end; dacy += step)
	m_Xbase[nPort] = m_Xbase[nPort] * MAX_DAC / MAX_VOLTAGE;
	m_Ybase[nPort] = m_Ybase[nPort] * MAX_DAC / MAX_VOLTAGE;
	sDACx = m_Xbase[nPort];
	sDACy = m_Ybase[nPort];

	strMsg.Format("%d通道基准点为:(%d,%d)", nPort + 1, sDACx, sDACy);
	LogInfo(strMsg);
try
{
	while (wStep)
	{
		YieldToPeers();
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
		stScanPara.nStartX = sScanMinVx;
		stScanPara.nEndX = sScanMaxVx;
		stScanPara.nStartY = sScanMinVy;
		stScanPara.nEndY = sScanMaxVy;
		stScanPara.nStep = wStep;

		stScanPara.nBlock = 0;
		stScanPara.nSwitch = nSwitchIndex;
		if (nSwitchIndex >= 16)
		{
			stScanPara.nBlock = 1;
			stScanPara.nSwitch = nSwitchIndex - 16;
		}
		if (nSwitchIndex >= 32)
		{
			stScanPara.nBlock = 2;
			stScanPara.nSwitch = nSwitchIndex - 32;
		}


		strMsg.Format("Point:%d", nScanPoint);
		LogInfo(strMsg);


			if (!m_PDBoxCmd.NewSendPDMon(nscanreal, 1024))
			{
				Sleep(100);
				if (!m_PDBoxCmd.NewSendPDMon(nscanreal, 1024))
				{
					LogInfo(m_PDBoxCmd.m_strErrorLog, COLOR_RED);
					return FALSE;
				}

			}
			nScanPoint = ((sScanMaxVx - sScanMinVx) / wStep + 1)*((sScanMaxVy - sScanMinVy) / wStep + 1);
			stScanPara.nStartX = sScanMinVx;
			stScanPara.nEndX = sScanMaxVx;
			stScanPara.nStartY = sScanMinVy;
			stScanPara.nEndY = sScanMaxVy;
			stScanPara.nStep = wStep;

			stScanPara.nBlock = 0;
			stScanPara.nSwitch = nPort;

			strMsg.Format("Scan:%d,%d,%d,%d,%d", sScanMinVx, sScanMaxVx, sScanMinVy, sScanMaxVy, wStep);
			LogInfo(strMsg);
			if (!m_CmdModule.SendScanTrig(stScanPara))
			{
				if (!m_CmdModule.SendScanTrig(stScanPara))
				{
					strMsg.Format("发送Trig失败！");
					LogInfo(strMsg);
					return FALSE;
				}
			}
			Sleep(5000);

			if (!m_PDBoxCmd.NewGetMaxPdCout(nscanreal, 1024, &nmaxid[0], &pdpwer[0]))
			{
				if (!m_PDBoxCmd.NewGetMaxPdCout(nscanreal, 1024, &nmaxid[0], &pdpwer[0]))
				{
					if (!m_PDBoxCmd.NewGetMaxPdCout(nscanreal, 1024, &nmaxid[0], &pdpwer[0]))
					{
						LogInfo(m_PDBoxCmd.m_strErrorLog, COLOR_RED);
						return FALSE;
					}
				}
			}
			//LogInfo(m_PDBoxCmd.m_strErrorLog, m_PDBoxCmd.m_strErrorLog.GetLength());
			CString  strmas = "";
			double pdchpow;
			for (int f1 = 0; f1 < 1; f1++)
			{

				wScanMaxIndex = nmaxid[nscanreal];
				pdchpow = pdpwer[nscanreal];


				if (1)
				{
					//wScanMaxIndex--;
					if (wScanMaxIndex == 0)
					{
						strMsg.Format("没有找到区域(%d,%d,%d,%d,%d)的IL最小点。", sScanMinVx, sScanMaxVx, sScanMinVy, sScanMaxVy, wStep);
						LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);

						return FALSE;
					}

					if (pdchpow < -30.0)
					{
						strMsg.Format("%d,无光 %.2f。", f1, pdchpow);
						LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);
						return FALSE;
					}

					//求取该序号对应的坐标
					wRowIndex = (wScanMaxIndex >> 5) & 0x1F;
					wColumnIndex = wScanMaxIndex & 0x1F;
					if (wRowIndex & 0x01)
					{
						wColumnIndex = 0x1F - wColumnIndex;
					}
					//	sDACx = sScanMinVx + wColumnIndex * wStep;
					//	sDACy = sScanMinVy + wRowIndex * wStep;
					sDACx = nfinch[f1][0] + wColumnIndex * wStep;
					sDACy = nfinch[f1][1] + wRowIndex * wStep;

					//nScanswdac[0][nPort][0] = sDACx;
					//nScanswdac[0][nPort][1] = sDACy;
					ScanPowerChannal[nPort].VoltageX = sDACx;
					ScanPowerChannal[nPort].VoltageY = sDACy;

					strMsg.Format("SW%d Port%d序号：%d,功率点为：%.2f,最小点坐标：(%d,%d)", f1 + 1, nPort + 1, wScanMaxIndex, pdchpow, sDACx, sDACy);
					LogInfo(strMsg, strMsg.GetLength() + 1);

				}
			}
			LogInfo(strmas, strmas.GetLength() + 1);

			wStep = (wStep >> 2);

		}


	}
	catch (CException* e)
	{
		e->Delete();
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}


	return TRUE;

}*/



void CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboSetWl()
{
	// TODO: Add your control notification handler code here
	CString strMsg;

	if (!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		m_cbSetWL.SetCurSel(-1);
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		m_cbSetWL.SetCurSel(-1);
		return;
	}
	int inWLTH = m_cbSetWL.GetCurSel();
	if (!m_CmdInstrument.SetVOAxAtten(1, 15.0))
	{
		return;
	}

	if (!m_CmdInstrument.SetAllSwitchToDifferentChannel(0, 1, 1, 0, 0, 0, 0, 0, 0))
	{
		return;
	}
	Sleep(200);
	if (inWLTH == 0)
	{

		if (!m_CmdInstrument.SetSwitchChannel(1, 1))//1550 1 1310 0
		{
			strMsg = "EDFATestBox切换光源1550失败";
			LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);
			return;
		}
	}
	if (inWLTH == 1)
	{
		if (!m_CmdInstrument.SetSwitchChannel(1, 0))//1550 1 1310 0
		{
			strMsg = "EDFATestBox切换光源1310失败";
			LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);
			return;
		}
	}



}


void CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboSetPdbox()
{
	// TODO: Add your control notification handler code here
	CString strValue;
	int inPM = m_cbSetPDBox.GetCurSel();
	if (inPM == 0)
	{
		m_nCHCount = 18;
	}
	if (inPM == 1)
	{
		m_nCHCount = 36;
	}

	m_cbChannel.ResetContent();
	for (int nCH = 0; nCH < m_nCHCount+1; nCH++)
	{
		strValue.Format("%d", nCH);
		m_cbChannel.AddString(strValue);
	}
	m_cbChannel.SetCurSel(0);

}


void CMy126S_45V_Switch_AppDlg::OnCbnSelchangeComboChSet()
{
	// TODO: Add your control notification handler code here
}



void CMy126S_45V_Switch_AppDlg::OnBnClickedButtonSetVoltage3()
{
	// TODO: Add your control notification handler code here
	//定标参数
	double dbAtt = 0;
	CString strValue,strMsg;
	GetDlgItemText(IDC_EDIT_SET_VOA, strValue);
	dbAtt = atof(strValue);

	if (!m_bDeviceOpen)
	{
		MessageBox("请先打开激光器！");
		return;
	}
	if (!m_bTesterPort)
	{
		MessageBox("请先打开串口！");
		return;
	}
	if (!m_bHasRefForTest)
	{
		MessageBox("请先归零！");
		return;
	}
	if (!m_CmdInstrument.SetVOAxAtten(1, dbAtt))
	{
		strMsg.Format( "VOA=%.2f设置失败", dbAtt);
		LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);
		return;
	}
	else
	{
		strMsg.Format("VOA=%.2f设置成功", dbAtt);
		LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_GREEN);
	}

}
