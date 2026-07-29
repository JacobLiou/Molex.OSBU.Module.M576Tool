// listDlg.cpp : implementation file
//

#include "stdafx.h"
#include "list.h"
#include "listDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// UDL 设备接口
UDLSERVERLib::IEngineMgrPtr		m_pEngine;
UDLSERVERLib::IIPMInterfacePtr  m_pPM;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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
// CListDlg dialog

CListDlg::CListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListDlg)
	m_nCom = 1;
	m_1x64Com = 1;
	m_dbSetAtten = 0.0;
	m_bOpenCOM =0;
	bAB =0;
	bPNtpye = 0;
	nReadPD=0;
	m_bOpenTestDevice = FALSE; 
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListDlg)
	DDX_Control(pDX, IDC_COMBOMCS2_CH, m_MCS_CH2);
	DDX_Control(pDX, IDC_COMBOMCS1_CH, m_MCS_CH1);
	DDX_Control(pDX, IDC_COMBOMCS_2, m_MCS_2);
	DDX_Control(pDX, IDC_COMBO1x8, m_1X8);
	DDX_Control(pDX, IDC_COMBOMCS_1, m_MCS_1);
	DDX_Control(pDX, IDC_COMBO1x2, m_1X2);
	DDX_Control(pDX, IDC_COMBO1x64_2, m_1X64_2);
	DDX_Control(pDX, IDC_COMBO1x64_1, m_1X64_1);
	DDX_Control(pDX, IDC_COMBO1, m_SWch);
	DDX_Control(pDX, IDC_LIST2, m_ctrMsgList);
	DDX_Text(pDX, IDC_EDIT1, m_nCom);
	DDX_Text(pDX, IDC_EDIT3, m_1x64Com);
	DDX_Text(pDX, IDC_EDIT_ATT, m_dbSetAtten);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CListDlg, CDialog)
	//{{AFX_MSG_MAP(CListDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON10, OnButton10)
	ON_BN_CLICKED(IDC_BUTTON_1x8, OnBUTTON1x8)
	ON_BN_CLICKED(IDC_BUTTON_1x2, OnBUTTON1x2)
	ON_BN_CLICKED(IDC_BUTTON_1x64, OnBUTTON1x64)
	ON_BN_CLICKED(IDC_BUTTON_1x11, OnBUTTON1x11)
	ON_BN_CLICKED(IDC_BUTTON_1x12, OnBUTTON1x12)
	ON_BN_CLICKED(IDC_BUTTON_1x13, OnBUTTON1x13)
	ON_BN_CLICKED(IDC_BUTTON11, OnButton11)
	ON_BN_CLICKED(IDC_BUTTON12, OnButton12)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListDlg message handlers

BOOL CListDlg::OnInitDialog()
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	CoInitialize(NULL);
	// TODO: Add extra initialization here
	CString strMsg;
//	TCHAR carCH[2][13]={"IN_IN1_PD1","OUT1_OUT_PD2"};
	for (int i=0;i<8;i++)
	{

		strMsg.Format("%d",i+1);
		m_1X8.InsertString(i,strMsg);
	}
	m_1X8.SetCurSel(0);
	for (i=0;i<2;i++)
	{

		strMsg.Format("%d",i+1);
		m_1X2.InsertString(i,strMsg);
	}
	m_1X2.SetCurSel(0);

	for (i=0;i<64;i++)
	{

		strMsg.Format("%d",i+1);
		m_1X64_1.InsertString(i,strMsg);
	}
	m_1X64_1.SetCurSel(0);
	for (i=0;i<64;i++)
	{

		strMsg.Format("%d",i+1);
		m_1X64_2.InsertString(i,strMsg);
	}
	m_1X64_2.SetCurSel(0);


	for (i=0;i<18;i++)
	{

		strMsg.Format("%d",i+1);
		m_MCS_1.InsertString(i,strMsg);
	}
	m_MCS_1.SetCurSel(0);
	for (i=0;i<32;i++)
	{

		strMsg.Format("%d",i+1);
		m_MCS_CH1.InsertString(i,strMsg);
	}
	m_MCS_CH1.SetCurSel(0);
		for (i=0;i<32;i++)
	{

		strMsg.Format("%d",i+1);
		m_MCS_CH2.InsertString(i,strMsg);
	}
	m_MCS_CH2.SetCurSel(0);

	for (i=0;i<18;i++)
	{

		strMsg.Format("%d",i+1);
		m_MCS_2.InsertString(i,strMsg);
	}
	m_MCS_2.SetCurSel(0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CListDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CListDlg::OnPaint() 
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
HCURSOR CListDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CListDlg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CListDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CListDlg::ControlDev1X64Switch(int channel)
{
	CString strSendBuf;
	CString strRevBuf,strMsg;
	char    chReadBuff[MAX_PATH];
	//DWORD   dwLength;
	ZeroMemory(chReadBuff,sizeof(char)*MAX_PATH);
	int nerrorCount=0;

	
loop1:	strSendBuf.Format("set:0:sw:0::channel=%d;",channel);
		if (!m_1X64OpCom.WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
		{
			Sleep(500);
			if (!m_1X64OpCom.WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
			{
				strMsg.Format("1X64开关，第%d通道，切换开关失败！",channel);
				LogInfo(strMsg, TRUE, COLOR_RED);
				return FALSE;
			}
		}
		strSendBuf.ReleaseBuffer();
		
		if(nerrorCount>10)
		{
			LogInfo("1x64开关切换通道失败",COLOR_RED);
			return FALSE;
		}
		Sleep(100);
		if (!m_1X64OpCom.ReadBuffer(chReadBuff,MAX_PATH))
		{
			LogInfo("获取开关信息失败！！请检查该光开关及控制串口！",COLOR_RED);
			return FALSE;
		}
		strRevBuf = chReadBuff;
		if(0>strRevBuf.Find("successfully*/"))
		{
			nerrorCount++;
			goto loop1;
		}
		else
		{
			return TRUE;
		}
		
		//	if(!atoi(chReadBuff)!=channel)
		//	{
		//		LogInfo("1*64SWITCH设置通道与读不一致，请检查！");
		//		return FALSE;	
		// 	}
		
		return TRUE;
}
BOOL CListDlg::Control1X64Switch(COpComm *m_OpCom1,int index, int channel)
{
	CString strSendBuf;
	CString strRevBuf,strMsg,strBreakMsg;
	COpComm  *OpCom=NULL;
	char    chReadBuff[MAX_PATH];
	//DWORD   dwLength;
	BYTE	bSwComand;

	int nerrorCount=0;
	
	OpCom = m_OpCom1;

	//首先知道是那个开关，再发什么命令

loop1:	strSendBuf.Format(";");
	if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(200);
	}
	ZeroMemory(chReadBuff,sizeof(char)*MAX_PATH);
	Sleep(50);
	strSendBuf.ReleaseBuffer();
	if (!OpCom->ReadBuffer(chReadBuff,MAX_PATH))
	{			
		return FALSE;
	}
	strRevBuf = chReadBuff;
	int Count = strRevBuf.GetLength();

	if ( Count >10 && Count < 30)
	{
		bSwComand = 1;
	}
	else if(Count > 30 && Count < 100)
	{
		bSwComand = 2;
	}
	else
	{
		bSwComand = 0;
	}
//结束
	if (channel < 0 && channel >8)
	{
		return FALSE;
	}
	switch(bSwComand)
		{
		case 1:
			if (index > 0 && index < 6)  //index 1~5,channel 1~8
			{
				strSendBuf.Format("set:0:sw:%d::channel=%d;",index,channel);
			}
			strBreakMsg ="OK";		
			break;
		case 2:
			if (index > 0 && index < 2)//index 1,channel 1~8 
			{
			  strSendBuf.Format("set:0:sw:%d::channel=%d;",index,channel);
			}
			else
			{
				 strSendBuf.Format("set:0:sw:2::channel=%d;",(8*(index-2))+channel);//index 2,channel 1~24
			}
			strBreakMsg ="successfully*/";
			break;
		default:
			break;
		}

		if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
		{
			Sleep(200);
		}
		ZeroMemory(chReadBuff,sizeof(char)*MAX_PATH);
		strSendBuf.ReleaseBuffer();
		if(nerrorCount>50)
		{
			return FALSE;
		}
		Sleep(120); //有时读不全
		if (!OpCom->ReadBuffer(chReadBuff,MAX_PATH))
		{		
			return FALSE;
		}
		strRevBuf = chReadBuff;
		if(0>strRevBuf.Find(strBreakMsg))
		{
			nerrorCount++;
			goto loop1;
		}
		else
		{
			return TRUE;
		}

	return TRUE;
}

BOOL CListDlg::PDPrepareScan(COpComm *m_OpCom1 ,DWORD dwContPoint)
{
	CString strSendBuf;
	CString strRevBuf,strMsg,strBreakMsg;
	PBYTE		pbRecvData = NULL;
	COpComm  *OpCom=NULL;
	char    chReadBuff[MAX_DATA];
	int nerrorCount=0;
	OpCom = m_OpCom1;

	strSendBuf.Format("\r\n");
	if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(200);
	}
	ZeroMemory(chReadBuff,sizeof(char)*MAX_DATA);
	Sleep(10);
	strSendBuf.ReleaseBuffer();
	if (!OpCom->ReadBuffer(chReadBuff,MAX_DATA))
	{			
		return FALSE;
	}
loop1:	strSendBuf.Format("SCANWAVE 0 %d\r\n",dwContPoint);
	if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(200);
	}
	ZeroMemory(chReadBuff,sizeof(char)*MAX_DATA);
	Sleep(10);
	strSendBuf.ReleaseBuffer();
	if (!OpCom->ReadBuffer(chReadBuff,MAX_DATA))
	{			
		return FALSE;
	}
	strRevBuf = chReadBuff;

	if(0>strRevBuf.Find("OK"))
	{
		nerrorCount++;
		if (nerrorCount>50)
		{
			return FALSE;
		}
		goto loop1;
	}
	Sleep(200);

	return TRUE;
}
BOOL CListDlg::PDGetCRC(COpComm *m_OpCom1 ,DWORD *dwCRC)
{
	CString strSendBuf;
	CString strRevBuf,strMsg,strBreakMsg;
	PBYTE		pbRecvData = NULL;
	COpComm  *OpCom=NULL;
	char    chReadBuff[MAX_DATA];
	int nerrorCount=0;
	OpCom = m_OpCom1;

	strSendBuf.Format("GETDATACRC\r\n");
	if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(200);
	}
	ZeroMemory(chReadBuff,sizeof(char)*MAX_DATA);
	Sleep(10);
	strSendBuf.ReleaseBuffer();
	if (!OpCom->ReadBuffer(chReadBuff,MAX_DATA))
	{			
		return FALSE;
	}
   strSendBuf.Format("GETDATACRC\r\n");
	if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(200);
	}
	ZeroMemory(chReadBuff,sizeof(char)*MAX_DATA);
	Sleep(10);
	strSendBuf.ReleaseBuffer();
	if (!OpCom->ReadBuffer(chReadBuff,MAX_DATA))
	{			
		return FALSE;
	}
	*dwCRC ++ = chReadBuff[0];
	*dwCRC ++ = chReadBuff[1];	
	*dwCRC ++ = chReadBuff[2];
	*dwCRC ++ = chReadBuff[3];	
	return TRUE;
}
BOOL CListDlg::FinalTestGetPower(COpComm *m_OpCom1,PBYTE pbReadData, DWORD dwContPoint)
{
	CString strSendBuf;
	CString strRevBuf,strMsg,strBreakMsg;
	PBYTE		pbRecvData = NULL;
	COpComm  *OpCom=NULL;
	char    chReadBuff[MAX_DATA];
	int nerrorCount=0;
	DWORD dwReadLength,dwLenthALL=0;
	OpCom = m_OpCom1;
	strSendBuf.Format("GETDATA %d\r\n",dwContPoint);//多一位CRC
	if (!OpCom->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(200);
	}
	while (1)
	{		
		if (dwContPoint==dwLenthALL/2) 
		{
			break;
		}
		Sleep(300);//等待串口装满
		if (!OpCom->ReadBuffer(chReadBuff,MAX_DATA,&dwReadLength))
		{			
			return FALSE;//读不到会超时，这个地方时间有点长
		}
		for (DWORD i=0;i<dwReadLength/2;i++) 
		{
			*pbReadData ++ = chReadBuff[i*2];
			*pbReadData ++ = chReadBuff[i*2+1];	
		}
		dwLenthALL +=dwReadLength;
		ZeroMemory(chReadBuff,sizeof(char)*MAX_DATA);
		strSendBuf.Format("%d %d",dwLenthALL,dwReadLength);
		LogInfo(strSendBuf);	
	}		


	return TRUE;
}
void CListDlg::LogInfo(LPCTSTR tszlogMsg,DWORD dwRGB, BOOL bAppend)
{
	int     dx=0;
	TCHAR			  m_tszAppFolder [MAX_PATH];
	CDC*    pDC;
	DWORD	dwLineIndex;
	FILE*	pLogFile = NULL;
	CString strList;
	CSize   sz;
/*	GetCurrentDirectory(MAX_LINE , m_tszAppFolder);
	CString strLogFile;
	strLogFile.Format("%s\\Log.txt" , m_tszAppFolder);
	if(m_pLogFile != NULL)
	{
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}	*/
	pDC			= m_ctrMsgList.GetDC();
	dwLineIndex = m_ctrMsgList.GetCount();

	if( dwLineIndex > 64)
	{
		m_ctrMsgList.ResetContent();
		dwLineIndex = 0;
	}
	if (dwLineIndex > 20)
	{
		m_ctrMsgList.SetTopIndex(dwLineIndex - 15);
	}

	m_ctrMsgList.InsertString(-1, tszlogMsg,dwRGB);

	for (DWORD i = 0; i < dwLineIndex + 1; i++)
	{
	   m_ctrMsgList.GetText(i, strList);
	   sz = pDC->GetTextExtent(strList);

	   if (sz.cx > dx)
		  dx = sz.cx;
	}
	m_ctrMsgList.ReleaseDC(pDC);
	::SendMessage(m_ctrMsgList.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);
/*	if(bAppend)
	{
		if(m_pLogFile == NULL)
		{
			return;
		}
		strList = tszlogMsg;
		strList+="\n";
		fwrite(strList.GetBuffer(strList.GetLength()+1),sizeof(char),strList.GetLength()+1,m_pLogFile);
	}*/
	UpdateWindow();
}
void CListDlg::PreTest()
{
	CString strError,strTemp;
	double powr[8]={0.0};
	m_OpCom.OpenPort("COM5");
	m_CmdSFPVOA.m_pRS232 = &m_OpCom;
	while (1)
	{
		for (int i=1;i<9;i++)
		{	
			if(!m_CmdSFPVOA.SelectWorkStation(i-1))
			{

			}
			if(!m_CmdSFPVOA.GetLWDMPDPower(1,i,powr,&strError))
			{
				//continue;
			}
			strTemp.Empty();
			for (int k =0;k<8;k++)
			{
				strError.Format("%.4f",powr[k]);	
				strTemp+=strError;
			}
			LogInfo(strTemp);
		}
}
}
void CListDlg::CalCrc32data() 
{
	
	BYTE data[14];
	PBYTE pBufferPtr= (BYTE*)VirtualAlloc(NULL,0xff,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE); ;
	CString strTemp="OPLINK";
	memset(pBufferPtr,0xFF,sizeof(BYTE)*32);

	memcpy(pBufferPtr,strTemp,strTemp.GetLength()+1);

	DWORD dwCRC32;


	ZeroMemory(data,sizeof(BYTE)*14);
	memcpy(data,"ABCDEFGHIJKLMN",14);
	dwCRC32=CRC32.GetCRC(data,14);
	dwCRC32 = ~dwCRC32;
/*	while (1) 
	{
		for (int i=1;i<5;i++)
		{
			for (int k=1;k<9;k++)
			{
				Control1X64Switch(&m_OpCom,i,k);//index 1~4 Channel 1~8 总共32个开关
			}
			
		}
	}
*/	
	// TODO: Add your control notification handler code here
	/*
	UpdateData(TRUE);
	DWORD i=1;
	for(int ii=0;ii<30000;ii++)//24647
	{
		m_strMsgDataArray[i].RemoveAll();
	}
	for(i=1;i<=m_Qty;i++)
	{
	 ReadRefMappFinal(i);
	}
	SaveRefMappFile();
	MessageBox("导表完成","OK",MB_OK | MB_ICONWARNING);
}*/
}

void CListDlg::OnButton1() 
{
	CString strTemp;
	m_OpCom.OpenPort("COM5");

		for (int i=1;i<5;i++)
		{
			for (int k=1;k<9;k++)
			{

				Control1X64Switch(&m_OpCom,i,k);//index 1~4 Channel 1~8 总共32个开关
				strTemp.Format("切到第%d开关的第%d通道成功",i,k);
				MessageBox(strTemp,"OK",MB_OK | MB_ICONWARNING);
			}
			
		}
	
	/*
	CString strTemp;
	m_OpCom.OpenPort("COM3");
	WORD dwContPoint=500;
	
	SweepTLSMon(dwContPoint);*/
	
}
BOOL CListDlg::SweepTLSMon(DWORD dwContPoint)
{
	CString strTemp;
	SHORT*	pnReadData=(SHORT*)VirtualAlloc(NULL, sizeof(SHORT) * dwContPoint, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	double*	pdblPDPower=(double*)VirtualAlloc(NULL, sizeof(double) * dwContPoint, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!FinalTestGetPower(&m_OpCom,(PBYTE)pnReadData,dwContPoint))
	{
		return FALSE;
	}
	DWORD mm=0;
	for (WORD i=0;i<dwContPoint;i++)
	{
		pdblPDPower[i] = (double)pnReadData[i] / 100.0;
		strTemp.Format("%.3f,%d",pdblPDPower[i],mm++);
		LogInfo(strTemp);
	}
	return TRUE;
}

DWORD CListDlg::SwapDWORD(DWORD dwInData)
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

UINT32 CListDlg::CalCrc32(BYTE data[], UINT32 size)
 {
     UINT32 i,j,temp,crc = 0xFFFFFFFF;
     for(i=0; i<size; i++)
     {
         temp = (UINT32)data[i];
         for(j=0; j<32; j++)
         {
             if( (crc ^ temp) & 0x80000000 )
             {
                 crc = 0x04C11DB7 ^ (crc<<1);
             }
             else
             {
                 crc <<=1;
             }
             temp<<=1;
         }
     }
     return crc;
 }
CString CListDlg::LoadIniDataString(LPCTSTR strSection, LPCTSTR szID)
{
	CString szValue;
	DWORD dwSize = 1000;
	GetPrivateProfileString(strSection,szID,"",
	szValue.GetBuffer(dwSize),dwSize,m_strConfigFilePath);
	szValue.ReleaseBuffer();

	szValue.Replace("\\n","\n");	//替换回换行符号
	return szValue;
}
BOOL CListDlg::SaveRefMappFile(DWORD dWCONUT)
{

 //   WIN32_FIND_DATA	winFindFileData;

    CString  strFileName,strTemp;
	HANDLE	 hDirectory = INVALID_HANDLE_VALUE;
//	int			nSelection;
//	DWORD		i
	DWORD		dwBytesWrite, dwBytesReturned;
	CString		 strTmptValue, strMsg,strTmptValue2;		
	HANDLE		hCSVFile   = INVALID_HANDLE_VALUE;
//	double dbWL;
	CFileFind	findDataFile;
	TCHAR tszAppFolder [MAX_PATH];
	GetCurrentDirectory(256 , tszAppFolder);
	strFileName.Format("%s\\FinalData.csv",tszAppFolder);
	hCSVFile = CreateFile(strFileName, GENERIC_WRITE, 0, NULL,
						                       CREATE_ALWAYS, 0, NULL);
	if(INVALID_HANDLE_VALUE == hCSVFile)
	{
		strTemp.Format("保存归零数据文件失败!");
		MessageBox(strTemp,"Error",MB_OK | MB_ICONERROR);
		return FALSE;
	}

	int nRefDataCount = m_strMsgDataArray[0].GetSize();	
	for(int i=0;i<nCount;i++)
	{
		strTmptValue="";
		for(int all=0;all<nRefDataCount;all++)
		{
				strTmptValue2.Format("%s,",m_strMsgDataArray[i].GetAt(all));
				strTmptValue+=strTmptValue2;

		}
		strTmptValue+="\n";
		dwBytesWrite = strTmptValue.GetLength() * sizeof(char);		
		WriteFile(hCSVFile, strTmptValue, dwBytesWrite, &dwBytesReturned, NULL);
	}
	
	CloseHandle(hCSVFile);
	return TRUE;
}

BOOL CListDlg::ReadRefMappFinal(DWORD k)
{
	TCHAR tszAppFolder [MAX_PATH];
	CString strItemPath,strFileName,strTemp;
	GetCurrentDirectory(256 , tszAppFolder);
	m_strConfigFilePath.Format("%s\\Config\\Config.ini",tszAppFolder);
	strFileName = LoadIniDataString("INFO","DATADIR");
//RefMappingAndData_Final_1
	strItemPath.Format("%s\\RefMappingAndData_Final_%d.csv",strFileName,k);
	CString     tempstr;
	TCHAR		seps[] = " ,\n";
	TCHAR		*token = NULL;
	TCHAR		lineBuf[256];
	CStdioFile	hRefFile;
	int         i = 0;
	LPTSTR		pStr;
	m_strConfigFilePath.Format("%s\\Config\\Config.ini",tszAppFolder);
	strTemp = LoadIniDataString("INFO","TESTPORT");
	int         nline=atoi(strTemp);
	BOOL bReturnValue = TRUE;
	try
	{
		if (GetFileAttributes(strItemPath)!=-1)
		{
			if (!(hRefFile.Open(strItemPath, CFile::modeRead | CFile::typeText)))
			{
				throw "打开 RefMappingTemp.csv 失败!";
			}
			do {

				pStr = hRefFile.ReadString(lineBuf, 256);
				if (NULL == pStr)
				{
					break;	// Reach file end
				}
				//TYPE / DUT PORT
				token = strtok(lineBuf, seps);
				tempstr = token;
				

				token = strtok(NULL, seps);
				tempstr = token;

				token = strtok(NULL, seps);
				tempstr = token;
				if(nline!=atoi(tempstr))
				{
					continue;	
				}
				token = strtok(NULL, seps);
				tempstr = token;

				token = strtok(NULL, seps);
				tempstr = token;
				token = strtok(NULL, seps);
				tempstr = token;
				token = strtok(NULL, seps);
				tempstr = token;
				token = strtok(NULL, seps);
				tempstr = token;
				token = strtok(NULL, seps);
				tempstr = token;
				token = strtok(NULL, seps);
				tempstr = token;
				m_strMsgDataArray[i].Add(tempstr);
			//atof(tempstr);
				
				i++;
			} while(1);
			nCount =i;
			hRefFile.Close();
		}
		
	}
	catch(char* tszErrMsg)
	{
		strTemp.Format("%s",tszErrMsg);
		MessageBox(strTemp,"Error",MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		strTemp.Format("Other exception occurred");
		MessageBox(strTemp,"Error",MB_OK | MB_ICONERROR);
	}
	return bReturnValue;
}

void CListDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
		CString strTemp;
	m_OpCom.OpenPort("COM6");

	
		for (int i=1;i<5;i++)
		{
			for (int k=1;k<9;k++)
			{
				if (!Control1X64Switch(&m_OpCom,i,k))
				{
					strTemp.Format("切到第%d开关的第%d通道失败",i,k);
					MessageBox(strTemp,"OK",MB_OK | MB_ICONERROR);
				}//index 1~4 Channel 1~8 总共32个开关
				else
				{
					strTemp.Format("切到第%d开关的第%d通道成功",i,k);
					MessageBox(strTemp,"OK",MB_OK | MB_ICONWARNING);
				}
		
			}
			
		}
}

void CListDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
//	CString strTemp;
//	CTime time = CTime::GetCurrentTime();
//	strTemp = time.Format("%U");//星期日作为每周的第一天
//	strTemp = time.Format("%W");//星期一作为每周的第一天
//
	CString strTemp;
	m_OpCom.OpenPort("COM4");
	while (1)
	{
		for (int i=1;i<9;i++)
		{
			Sleep(1000);
			if (!Control1X8Switch(&m_OpCom,i)) 
			{
				strTemp.Format("切到第SW1开关的第%d通道失败！",i);
				LogInfo(strTemp,COLOR_RED);
				break;
			}
		}


	}
}

BOOL CListDlg::Control1X8Switch(COpComm *m_OpCom1, int channel)
{
	CString strSendBuf;
	CString strRevBuf,strMsg,strBreakMsg;
	char    chReadBuff[MAX_PATH];
	int nerrorCount=0;


	//首先知道是那个开关，再发什么命令

loop1:	strSendBuf.Format("SW1PORT%d\r\n",channel);
	if (!m_OpCom1->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(200);
	}
	ZeroMemory(chReadBuff,sizeof(char)*MAX_PATH);
	Sleep(50);

	strSendBuf.ReleaseBuffer();
	if (!m_OpCom1->ReadBuffer(chReadBuff,MAX_PATH))
	{			
		return FALSE;
	}
	strRevBuf = chReadBuff;
	if (strRevBuf.Find("Fail")!=-1)
	{
		nerrorCount++;
		if (nerrorCount >50)
		{
			return FALSE;
		}
		goto loop1;
	}
	else
	{
		return TRUE;
	}
//	nerrorCount =0;
//loop2:	strSendBuf.Format("STATUS?\r\n");
//	if (!m_OpCom1->WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
//	{
//		Sleep(200);
//	}
//	ZeroMemory(chReadBuff,sizeof(char)*MAX_PATH);
//	Sleep(50);
//
//	Sleep(120); //有时读不全
//	if (!m_OpCom1->ReadBuffer(chReadBuff,MAX_PATH))
//	{		
//		return FALSE;
//	}
//	strRevBuf = chReadBuff;
//	if(0>strRevBuf.Find(strBreakMsg))
//	{
//		nerrorCount++;
//		if (nerrorCount >50)
//		{
//			return FALSE;
//		}
//		goto loop2;
//	}
//	else
//	{
//		return TRUE;
//	}

	return TRUE;
}
BOOL CListDlg::OpZHDevice() 
{

	CString strMsg,strConfigFile;
	TCHAR m_tszAppFolder[MAX_PATH];
	ZeroMemory(m_tszAppFolder,MAX_PATH);
	
	////// 获取程序路径 ////////////////////////		
	GetCurrentDirectory(MAX_PATH , m_tszAppFolder);
	// TODO: Add your control notification handler code here
	if (m_bOpenTestDevice)
	{
		return TRUE;
	}
	HRESULT hr = m_pEngine.CreateInstance(__uuidof(EngineMgr));
	ASSERT( SUCCEEDED( hr ) );
	//	m_pEngine->RegisterUDL();
	hr = m_pPM.CreateInstance( __uuidof(IPMInterface));//启动组件
	ASSERT( SUCCEEDED( hr ) );
  
	strConfigFile.Format("%s\\Config\\ConfigurationWizard.xml", m_tszAppFolder);
	
	
	if(m_pEngine->LoadConfiguration((_bstr_t)strConfigFile)!= S_OK)
	{
		strMsg.Format("%s",m_pEngine->GetGetLastMessage());
		
		LogInfo(strMsg,strMsg.GetLength()+1);
		m_bOpenTestDevice = FALSE;
		return FALSE;
	}
	hr = m_pEngine->OpenEngine();
	if (hr == S_FALSE)
	{
		if (FAILED(hr))
		{
			_com_error e(hr);
			AfxMessageBox(e.ErrorMessage());
			
		}
		ASSERT( SUCCEEDED( hr ) );
		strMsg = (LPCSTR) m_pEngine->GetGetLastMessage();
		
		LogInfo(strMsg,strMsg.GetLength()+1);
		m_bOpenTestDevice = FALSE;
		return FALSE;
	}	
	if(m_pPM->SetUnit(0,0) != S_OK)//设置通道1单位为dBm
	{
		strMsg.Format("%s",m_pEngine->GetGetLastMessage());
		LogInfo(strMsg,strMsg.GetLength()+1);
		return FALSE;
	}

	Sleep(1000);
	if(m_pPM->SetWavelength(0, 1550) != S_OK)//设置通道波长为 1550
	{
		strMsg.Format("%s",m_pEngine->GetGetLastMessage());
		LogInfo(strMsg,strMsg.GetLength()+1);
		return FALSE;
	}
	if(m_pPM->SetAverageTime(0, 20) != S_OK)//设置平均时间50ms
	{
		strMsg.Format("%s",m_pEngine->GetGetLastMessage());
		LogInfo(strMsg,strMsg.GetLength()+1);
		return FALSE;
	}

	m_bOpenTestDevice = TRUE;	

	return TRUE;
}
void CListDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	UpdateData(TRUE);
	if (!m_bOpenCOM)
	{
		strTemp.Format("串口没打开");
		LogInfo(strTemp,COLOR_RED);
		return;
	}
	if(!Set1X2SwitchForASCII(bAB))
	{
		strTemp.Format("切换产品开关到通道%d失败",bAB);
		LogInfo(strTemp,COLOR_RED);
	}
	else
	{
		strTemp.Format("切换产品开关到通道%d成功",bAB);
		LogInfo(strTemp,COLOR_GREEN);
	}	/*
	if(!Control1X64Switch(mSWch))
	{
		strTemp.Format("切换产品1X64开关到通道%d失败",mSWch);
		LogInfo(strTemp,COLOR_RED);
	}
	else
	{
		strTemp.Format("切换产品1X64开关到通道%d成功",mSWch);
		LogInfo(strTemp,COLOR_GREEN);
	}*/
}
void CListDlg::YieldToPeers()
{
	MSG	msg;	
	//	Do not remove message, keep until PumpMessage processes it.
	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if(!AfxGetThread()->PumpMessage())
			break;
	}
}

BOOL CListDlg::Control1X64Switch(int channel)
{
	CString strSendBuf;
	CString strRevBuf,strMsg,strBreakMsg;
	char    chReadBuff[MAX_PATH];
	//DWORD   dwLength;
	BYTE	bSwComand;

	int nerrorCount=0;


	//首先知道是那个开关，再发什么命令

loop1:	strSendBuf.Format("SPRT %d\r\n",channel);
	if (!m_OpCom.WriteBuffer(strSendBuf.GetBuffer(strSendBuf.GetLength()),strSendBuf.GetLength()))
	{
		Sleep(200);
	}
//	ZeroMemory(chReadBuff,sizeof(char)*MAX_PATH);
//	Sleep(50);
//
//	strSendBuf.ReleaseBuffer();
//	if (!m_OpCom1->ReadBuffer(chReadBuff,MAX_PATH))
//	{			
//		return FALSE;
//	}
//	strRevBuf = chReadBuff;
//	if (strRevBuf.Find("Fail")!=-1)
//	{
//		nerrorCount++;
//		if (nerrorCount >50)
//		{
//			return FALSE;
//		}
//		goto loop1;
//	}
//	else
//	{
//		return TRUE;
//	}
	return TRUE;
}

void CListDlg::OnButton5() 
{
	// TODO: Add your control notification handler code here
	CString strTemp,strComPort;
	UpdateData(TRUE);
	if (m_nCom >=10)
	{
		strComPort.Format("\\\\.\\COM%d", m_nCom);
	}
	else
	{
		strComPort.Format("COM%d", m_nCom);
	}
	if(!m_OpCom.OpenPort((LPSTR)(LPCSTR)strComPort,115200))
	{
		strTemp.Format("打开串口COM%d失败",m_nCom);
		LogInfo(strTemp,COLOR_RED);
		m_bOpenCOM = FALSE;
	}
	else
	{
		strTemp.Format("打开串口COM%d成功",m_nCom);
		LogInfo(strTemp,COLOR_GREEN);
		m_bOpenCOM = TRUE;
	}
	
}

void CListDlg::OnButton6() 
{
	// TODO: Add your control notification handler code here
	CString strTemp,strComPort;
	CString strMsg;
	int nCh =0;
	double dblReadPower =0.0;
	int Wch=0;
	DWORD dwCount=0;
	BOOL bFunctionOK=FALSE;
	UpdateData(TRUE);
	OnButton5();
	srand((unsigned)time(NULL));
	if (m_1x64Com >=10)
	{
		strComPort.Format("\\\\.\\COM%d", m_1x64Com);
	}
	else
	{
		strComPort.Format("COM%d", m_1x64Com);
	}
	if(!m_1X64OpCom.OpenPort((LPSTR)(LPCSTR)strComPort))
	{
		strTemp.Format("打开设备串口COM%d失败",m_1x64Com);
		LogInfo(strTemp,COLOR_RED);
	}
	else
	{
		strTemp.Format("打开串口COM%d成功",m_nCom);
		LogInfo(strTemp,COLOR_GREEN);
	}
	if (!OpZHDevice())
	{
		strMsg.Format("打开光功率计失败");
		LogInfo(strMsg,TRUE,TRUE);
		return;
	}
	while (1)
	{
		YieldToPeers();
		nCh = rand()%(64);
		strTemp.Format("现在开始把产品切换到%d通道",nCh+1);
		LogInfo(strTemp,TRUE,TRUE);
		if (Wch>63)
		{
			Wch=0;dwCount++;
			SaveRefMappFile(dwCount);
			strTemp.Format("产品1x64设备已切换第%d轮",dwCount);
			LogInfo(strTemp,TRUE,TRUE);
		}
		if(!Control1X64Switch(nCh+1))
		{
			strTemp.Format("切换产品1X64开关到通道%d失败",nCh+1);
			LogInfo(strTemp,COLOR_RED);
		}
		else
		{
			strTemp.Format("切换产品1X64开关到通道%d成功",nCh+1);
			LogInfo(strTemp,COLOR_GREEN);
		}	
		Sleep(300);
		if(m_pPM->GetPower(0, &dblReadPower)== S_FALSE)
		{
			if(m_pPM->GetPower(0, &dblReadPower)== S_FALSE)
			{
				strMsg.Format("%s",m_pEngine->GetGetLastMessage());
				LogInfo(strMsg,TRUE,TRUE);
			}
		}
		if (!ControlDev1X64Switch(nCh))//切换设备1X64开关
		{
			strMsg.Format("第%d通道%fdBm",nCh+1,dblReadPower);
			LogInfo(strMsg,TRUE,TRUE);
			AfxMessageBox(strMsg);
		}

		if (dblReadPower < -20)
		{
			strMsg.Format("第%d通道%fdBm",nCh+1,dblReadPower);
			LogInfo(strMsg,TRUE,TRUE);
			AfxMessageBox(strMsg);
		//	break ;
		}
	
		strMsg.Format("%d,%f", nCh+1,dblReadPower);
		m_strMsgDataArray[0].Add(strMsg);
		strMsg.Format("读到的光功率为：%f",dblReadPower);
		LogInfo(strMsg,TRUE);
		Wch++;
	}
	
}
BOOL CListDlg::SetSWPORT(int nCH,BOOL bHL)
{
	BOOL bSetCommand = FALSE;
	double dbGetThrPower=0.0;
	CString strCommand,strMsg,strTemp;
	DWORD dwFeedBackLength;
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
	//ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(chSetCommand, sizeof(char) * MAX_COUNT);
	ZeroMemory(chReadCommand, sizeof(char)*MAX_COUNT);

	if (!SetSWDUTtestCMD(&strMsg,FALSE))
	{
	  	AfxMessageBox("进入测试板，无法驱动开关失败！"+strMsg);
	}
	strCommand.Format("\r\n");
	memcpy(chSetCommand, strCommand, strCommand.GetLength());
	if (!m_OpCom.WriteBuffer(chSetCommand, strCommand.GetLength()))
	{
		AfxMessageBox("发送指令错误！");
		return FALSE;
	}
	strCommand.Format("SW %d %d\r\n",nCH+1,bHL);//0是切开A ,1是切到B
	memcpy(chSetCommand, strCommand, strCommand.GetLength());
	if (!m_OpCom.WriteBuffer(chSetCommand, strCommand.GetLength()))
	{
		AfxMessageBox("发送指令错误！");
		return FALSE;
	}
	Sleep(50);
	if (!m_OpCom.ReadBuffer(chReadCommand, MAX_COUNT, &dwFeedBackLength))
	{
		AfxMessageBox("读取串口数据错误！");
		return FALSE;
	}	
	strTemp.Format("%s", chReadCommand);
	if(strTemp.Find("OK")==-1)
	{	
		return FALSE;
	}
	if (!SetSWDUTtestCMD(&strMsg))
	{
	  	AfxMessageBox("退出测试板，无法驱动开关失败！"+strMsg);
	}
	return TRUE;
}
BOOL CListDlg::GetRS232PIN(int nBlock,int nPINIndex,double *InPower)
{
	BOOL bSetCommand = FALSE;
	CString strMsg,strCommand;
	DWORD dwFeedBackLength;
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
	//ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(chSetCommand, sizeof(char) * MAX_COUNT);
	ZeroMemory(chReadCommand, sizeof(char)*MAX_COUNT);
	switch(nBlock) 
	{
	case 0:	
		strCommand.Format("SP_SIG%d_PIN\r\n",nPINIndex+1);
		break;
	case 1:
		strCommand.Format("SW_A%d_PIN\r\n",nPINIndex+1);
		break;
	case 2:
		strCommand.Format("SW_B%d_PIN\r\n",nPINIndex+1);
		break;
	default:
		break;
	}
	if (nPINIndex < 0 || nPINIndex > 5) 
	{
		AfxMessageBox("PIN的索引大于5");
		return FALSE;
	}
	memcpy(chSetCommand, strCommand, strCommand.GetLength());
	if (!m_OpCom.WriteBuffer(chSetCommand, strCommand.GetLength()))
	{
		AfxMessageBox("发送指令错误！");
		return FALSE;
	}
	Sleep(10);
	if (!m_OpCom.ReadBuffer(chReadCommand, MAX_COUNT, &dwFeedBackLength))
	{
		AfxMessageBox("读取串口数据错误！");
		return FALSE;
	}
	strCommand.Format("%s", chReadCommand);
	AfxExtractSubString(strMsg,strCommand,1,':');
	InPower[0] = atof(strMsg);
	return TRUE;
}
BOOL CListDlg::SetSWDUTtestCMD(CString *strTemp,BOOL bExitCMD)//1 进入产品默认，0进行测试板
{
	BOOL bSetCommand = FALSE;
	double dbGetThrPower=0.0;
	CString strCommand;
	DWORD dwFeedBackLength;
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
	//ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(chSetCommand, sizeof(char) * MAX_COUNT);
	ZeroMemory(chReadCommand, sizeof(char)*MAX_COUNT);
	if(bExitCMD)
	{
		strCommand.Format("MCMD\r\n");
	}
	else
	{
		strCommand.Format("TodayisHappyBythewayTomorrowisHappy123456MingTianHuiGengMeiHao78\r\n");
	}
	memcpy(chSetCommand, strCommand, strCommand.GetLength());
	if (!m_OpCom.WriteBuffer(chSetCommand, strCommand.GetLength()))
	{
		AfxMessageBox("发送指令错误！");
		return FALSE;
	}
	Sleep(50);
	if (!m_OpCom.ReadBuffer(chReadCommand, MAX_COUNT, &dwFeedBackLength))
	{
		AfxMessageBox("读取串口数据错误！");
		return FALSE;
	}	
	strTemp[0].Format("%s", chReadCommand);
	if(strTemp[0].Find("OK")==-1)
	{	
		return FALSE;
	}
	return TRUE;
}

void CListDlg::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	bAB =0;//切开关
	nReadPD =1;//读功率
}

void CListDlg::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	bAB =1;
	nReadPD = 2;
}

/////////////////////////////////////////////////////////////////////
//函数说明：
//    设置EEROM是否可以读写，0--可写，1--可读
//Add by wanxin
//Time:20171116
////////////////////////////////////////////////////////////////////
BOOL CListDlg::SetEEROMWritePinFroASCII(int nStatus)
{
	char pbBuffer[MAX_PATH];
	DWORD dwFeedBackLength;
	CString strMsg,m_strErrorMsg;

	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);

	sprintf(pbBuffer, "PIN WP %d\r\n", nStatus);

	if (!m_OpCom.WriteBuffer(pbBuffer, strlen(pbBuffer)))
	{
		m_strErrorMsg = _T("发送写EEROM WP PIN指令错误！");
		return FALSE;
	}
	Sleep(100);
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);
	if (!m_OpCom.ReadBuffer(pbBuffer, MAX_PATH, &dwFeedBackLength))
	{
		m_strErrorMsg = _T("发送写EEROM WP PIN返回错误！");
		return FALSE;
	}
	strMsg = pbBuffer;
	if (strMsg.Find("OK") == -1)
	{
		m_strErrorMsg = _T("发送写EEROM WP PIN返回%s错误！", strMsg);
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//函数说明：
//    从EEROM中读取VOA Lut表，并根据回读数据，计算每两个Lut表点斜率截距
//用来后续根据衰减计算DAC值
//Add by wanxin
//Time:20171120
//////////////////////////////////////////////////////////////////////
BOOL CListDlg::ReadVOALutFromEEROMHandLutData(int nVOAIndex, pVOALut pstVOALutData)
{
	CString  strMsg;
	BOOL bFunctionOK = TRUE;
	int nReadLength = 0;
	int nCalPointCount = 0;
	PBYTE pbBuffer = NULL;
	WORD wAddress = 0;
	DWORD dwGetLength = 0;

	DWORD dwAtten;
	DWORD dwADC;

	try
	{
		SetEEROMWritePinFroASCII(1);
		//	m_ModuleCMD.InitialEEROM();
		int nCalPointCount = (int)((250 - 0) / 3) + 1; //84个定标点
		nReadLength = nCalPointCount * 8;

		pbBuffer = (BYTE*)VirtualAlloc(NULL, sizeof(BYTE)*MAX_VOA_CAL_DATA, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		ZeroMemory(pbBuffer, sizeof(BYTE)*MAX_VOA_CAL_DATA);
	    if(bPNtpye==2)
		{
				switch (nVOAIndex)
				{
				case 1:
				{
					wAddress = 0x12AD;
					break;
				}
				case 2:
				{
					wAddress = 0x15ED;
					break;
				}
				default:
					wAddress = 0x12AD;
					break;

			}
		}
		if(bPNtpye==1)
		{
			switch (nVOAIndex)
			{
				case 1:
				{
					wAddress = 0x15f5 + 0xa0;
					break;
				}
				case 2:
				{
					wAddress = (0x15f5 + 0xa0 + 8 * nCalPointCount) + 0xa0;
					break;
				}
				case 3:
				{
					wAddress = (0x15f5 + 0x140 + 8 * nCalPointCount * 2) + 0xa0;
					break;
				}
				case 4:
				{
					wAddress = (0x15f5 + 0x1e0 + 8 * nCalPointCount * 3) + 0xa0;
					break;
				}
				case 5:
				{
					wAddress = (0x15f5 + 0x280 + 8 * nCalPointCount * 4) + 0xa0;
					break;
				}
				case 6:
				{
					wAddress = (0x15f5 + 0x320 + 8 * nCalPointCount * 5) + 0xa0;
					break;
				}
				default:
				wAddress = 0x15f5 + 0xa0;
				break;
		}
		}
		if (!ReadEEROMData(wAddress, pbBuffer, nReadLength, &dwGetLength))
		{
			strMsg.Format("从产品中读取VOA %d Lut表数存在错误！", nVOAIndex);
			throw (char*)(LPCTSTR)strMsg;
		}
		int nCount = 0;
		for (int i = 0;i < nReadLength;i += 8)
		{
			m_stVOALut.stLut[nCount].nIndex = nCount;

			dwADC = 0xff000000 & (*(pbBuffer + i) << 24);
			dwADC += 0xffff0000 & (*(pbBuffer + i + 1) << 16);
			dwADC += 0xffffff00 & (*(pbBuffer + i + 2) << 8);
			dwADC += 0xffffffff & (*(pbBuffer + i + 3));

			m_stVOALut.stLut[nCount].dwADC = dwADC;

			dwAtten = 0xff000000 & (*(pbBuffer + i + 4) << 24);
			dwAtten += 0xffff0000 & (*(pbBuffer + i + 5) << 16);
			dwAtten += 0xffffff00 & (*(pbBuffer + i + 6) << 8);
			dwAtten += 0xffffffff & (*(pbBuffer + i + 7));

			m_stVOALut.stLut[nCount].dwPower = dwAtten;

			nCount++;
		}

		m_stVOALut.nPointCount = nCount;

	}
	catch (char *pstError)
	{
		strMsg = pstError;
		LogInfo(strMsg,COLOR_RED);
		bFunctionOK = FALSE;
	}

	if (pbBuffer != NULL)
	{
		VirtualFree(pbBuffer, 0, MEM_RELEASE);
		pbBuffer = NULL;
	}
//	SetEEROMWritePinFroASCII(1);
	//m_ModuleCMD.ReleaseEEROM();
	return bFunctionOK;
}


//////////////////////////////////////////////////////////////////////////
//函数说明：
//    从EEROM中读取所有内容,每次读取100字节
//Add by ChunliangW
//Time:20160303
//////////////////////////////////////////////////////////////////////////
BOOL CListDlg::ReadEEROMData(WORD wStartAddress, PBYTE pbData, DWORD dwReadLength, DWORD* pdwFeedbackLength)
{
	PBYTE pGetDataPtr = NULL;
	WORD wAddress;
	DWORD dwLength = 0;
	DWORD dwGetLength;
	DWORD dwReadLine = 0x40;
	BYTE  bReabBuffer[128];
	int nTempGet = -0;
	CString m_strErrorMsg;
	//pGetDataPtr = pbData;
	//DWORD dwTotalLength = wStopAddress - wStopAddress +1;
	if (dwReadLength <= dwReadLine)
	{
		if (!RxDataFromI2C(0, wStartAddress, dwReadLength, pbData, &dwGetLength))
		{
			return FALSE;
		}
	}
	else
	{
		DWORD dwLineCount = 0;
		wAddress = wStartAddress;
		while (TRUE)
		{
			DWORD dwTempLength = dwReadLength - dwLineCount*dwReadLine;
			if (dwTempLength >= dwReadLine)
			{
				wAddress = (WORD)(wStartAddress + dwLineCount*dwReadLine);
				nTempGet = dwLineCount*dwReadLine;
				if (!RxDataFromI2C(0, wAddress, dwReadLine, pbData + nTempGet, &dwGetLength))
				{
					return FALSE;
				}

				dwLength += (dwGetLength);
			}
			else
			{
				if (dwTempLength != 0)
				{
					wAddress = (WORD)(wStartAddress + dwLineCount*dwReadLine);
					nTempGet = dwLineCount*dwReadLine;
					if (!RxDataFromI2C(0, wAddress, dwTempLength, pbData + nTempGet, &dwGetLength))
					{
						return FALSE;
					}
					dwLength += (dwGetLength);
					break;
				}
				else
				{
					break;
				}
			}
			dwLineCount++;
		}

		if (dwLength != dwReadLength)
		{
			m_strErrorMsg = "读取EEROM返回内容长度与实际长度不相等";
			return FALSE;
		}
		*pdwFeedbackLength = dwLength;
	}
	return TRUE;
}
void CListDlg::OnButton7() 
{
	// TODO: Add your control notification handler code here
	CString strMsg,StrTemp;
	WORD	wDacValue;
	double pdblReadPower;

	UpdateData(TRUE);
	if (!m_bOpenCOM)
	{
		strMsg.Format("串口没打开");
		LogInfo(strMsg,COLOR_RED);
		return;
	}
//	GetDlgItemText(IDC_COMBO1,strMsg);
	
	int mSWch = m_SWch.GetCurSel()+1;
	if (!ReadVOALutFromEEROMHandLutData(mSWch, &m_stVOALut))
	{
		strMsg.Format("从产品读取EEROM VOA%d定标点Lut存在错误", mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
	if (!CalcuVOAADCbyAtten(m_dbSetAtten, &wDacValue))
	{
		strMsg.Format("VOA %d 验证过程中根据衰减计算设置DAC存在错误", mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
	if (!SetVOAActualDACForASCII(mSWch-1, wDacValue))
	{
		strMsg.Format("设置VOA%dDAC错误", mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
	else
	{
		strMsg.Format("设置VOA%d DAC:%d,Att:%.1f ", mSWch,wDacValue,m_dbSetAtten);
		LogInfo(strMsg,COLOR_GREEN);	
	}

}

///////////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    根据VOA LUT和设置的衰减值计算范围的ADC值
//Add by wanxin
//Time：20171121
///////////////////////////////////////////////////////////////////////////////////////
BOOL CListDlg::CalcuVOAADCbyAtten(double dbAtten, PWORD pwADC)
{
	CString strMsg;
	BOOL bFunctionOK = TRUE;
	DWORD dwAtten;

	int nLeftIndex = -1;
	int nRightIndex = -1;
	double pdbAtten[2];
	double dbADC[2];
	double dbK, dbC;
	double dbCalADC;
	try
	{
		dwAtten = (DWORD)(dbAtten * 10);

		if (dwAtten <= m_stVOALut.stLut[0].dwPower)
		{
			nLeftIndex = 0;
		}
		else if (dwAtten > m_stVOALut.stLut[m_stVOALut.nPointCount - 1].dwPower)
		{
			nLeftIndex = m_stVOALut.nPointCount - 1;
		}
		else
		{
			for (int i = 0;i < (m_stVOALut.nPointCount - 1);i++)
			{
				if (dwAtten >= m_stVOALut.stLut[i].dwPower &&
					dwAtten <= m_stVOALut.stLut[i + 1].dwPower)
				{
					nLeftIndex = i;
				}
			}
			if (nRightIndex > m_stVOALut.nPointCount)
			{
				strMsg.Format("根据衰减点计算Lut右索引值%d，大于总数：%d不一致！", nRightIndex, m_stVOALut.nPointCount);
				throw (char*)(LPCTSTR)strMsg;
			}
			if (m_stVOALut.stLut[nLeftIndex].nIndex != nLeftIndex)
			{
				strMsg.Format(_T("根据衰减点计算Lut索引值%d与实际读取Lut表索引值%d不一致！", nLeftIndex，m_stVOALut.stLut[nLeftIndex - 1].nIndex + 1));
				throw (char*)(LPCTSTR)strMsg;
			}
		}
		nRightIndex = nLeftIndex + 1;

		dbADC[0] = (double)m_stVOALut.stLut[nLeftIndex].dwADC;
		pdbAtten[0] = (double)m_stVOALut.stLut[nLeftIndex].dwPower;

		dbADC[1] = (double)m_stVOALut.stLut[nRightIndex].dwADC;
		pdbAtten[1] = (double)m_stVOALut.stLut[nRightIndex].dwPower;
		if (!LinearFitArray(dbADC, pdbAtten, 2, dbK, dbC))
		{
			strMsg.Format(_T("计算VOA定标点斜率截距存在错误！"));
			throw (char*)(LPCTSTR)strMsg;
		}
		dbCalADC = ((double)dwAtten - dbC) / dbK;
		*pwADC = (WORD)dbCalADC;
	}
	catch (char *pstError)
	{
		strMsg = pstError;
		LogInfo(strMsg, strMsg.GetLength() + 1);
		bFunctionOK = FALSE;
	}

	return bFunctionOK;
}

double CListDlg::Sum(double *pdblArray, int nCount)
{

	double dblSum = 0;
	for (int i = 0; i < nCount; i++)
	{
		dblSum += pdblArray[i];
	}

	return dblSum;

}
BOOL CListDlg::LinearFitArray(double *pdblX, double *pdblY, int nCount, double &k, double &c)
{
	double *pdblTempArray1 = new double[nCount];
	double	*pdblTempArray2 = new double[nCount];


	for (int i = 0; i < nCount; i++)
	{
		pdblTempArray1[i] = pdblX[i] * pdblY[i];
		pdblTempArray2[i] = pdblX[i] * pdblX[i];
	}

	k = (nCount * Sum(pdblTempArray1, nCount) - Sum(pdblX, nCount) * Sum(pdblY, nCount))
		/ (nCount * Sum(pdblTempArray2, nCount) - Sum(pdblX, nCount) * Sum(pdblX, nCount));
	c = Sum(pdblY, nCount) / nCount - k * Sum(pdblX, nCount) / nCount;


	if (pdblTempArray1 != NULL)
	{
		delete[] pdblTempArray1;
		pdblTempArray1 = NULL;
	}
	if (pdblTempArray2 != NULL)
	{
		delete[] pdblTempArray2;
		pdblTempArray2 = NULL;
	}
	return TRUE;
}

BOOL CListDlg::GetRS232PDADC(int nBlock,int nPDIndex,PWORD pADCValue)
{
	BOOL bSetCommand = FALSE;
	CString strMsg,strCommand;
	DWORD dwFeedBackLength;
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
	//ZeroMemory(pchReadBuf,sizeof(pchReadBuf));
	ZeroMemory(chSetCommand, sizeof(char) * MAX_COUNT);
	ZeroMemory(chReadCommand, sizeof(char)*MAX_COUNT);
	switch(nBlock) 
	{
	case 0:	
		strCommand.Format("SP_ADC %d\r\n",nPDIndex+1);	
		break;
	case 1:
		strCommand.Format("SWA_ADC %d\r\n",nPDIndex+1);
		break;
	case 2:
		strCommand.Format("SWB_ADC %d\r\n",nPDIndex+1);
		break;
	default:
		break;
	}
	if (nPDIndex < 0 || nPDIndex > 5) 
	{
		AfxMessageBox("PD的索引大于5");
		return FALSE;
	}
	memcpy(chSetCommand, strCommand, strCommand.GetLength());
	if (!m_OpCom.WriteBuffer(chSetCommand, strCommand.GetLength()))
	{
		AfxMessageBox("发送指令错误！");
		return FALSE;
	}
	Sleep(10);
	if (!m_OpCom.ReadBuffer(chReadCommand, MAX_COUNT, &dwFeedBackLength))
	{
		AfxMessageBox("读取串口数据错误！");
		return FALSE;
	}
	strCommand.Format("%s", chReadCommand);
	AfxExtractSubString(strMsg,strCommand,1,':');
	pADCValue[0] = atoi(strMsg);

	return pADCValue[0];
}
void CListDlg::OnButton8() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_OpCom.ClosePort();
	strTemp.Format("关闭串口COM%d成功",m_nCom);
	LogInfo(strTemp,COLOR_BLUE);
}

void CListDlg::OnRadio3() 
{
	// TODO: Add your control notification handler code here
	nReadPD = 0;
}
/////////////////////////////////////////////////////////////////////
//函数说明：
//    设置设置产品1x2开关，0--1，1--2
//Add by wanxin
//Time:20171116
////////////////////////////////////////////////////////////////////
BOOL CListDlg::Set1X2SwitchForASCII(int nStatus)
{
	char pbBuffer[MAX_PATH];
	DWORD dwFeedBackLength;
	CString strMsg,m_strErrorMsg;

	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);

	sprintf(pbBuffer, "sw 0 spos %d\r\n", nStatus);

	if (!m_OpCom.WriteBuffer(pbBuffer, strlen(pbBuffer)))
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令错误！");
		return FALSE;
	}
	Sleep(100);
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);
	if (!m_OpCom.ReadBuffer(pbBuffer, MAX_PATH, &dwFeedBackLength))
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令返回错误！");
		return FALSE;
	}
	strMsg = pbBuffer;
	if (strMsg.Find("OK") == -1)
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令返回%s错误！", strMsg);
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    根据PD LUT和回读ADC值，计算反馈功率
//Add by wanxin
//Time：20171121
///////////////////////////////////////////////////////////////////////////////////////
BOOL CListDlg::CalcuPDReportPowerByADC(DWORD dwADC, double &pdbReportPower)
{
	CString strMsg;
	BOOL bFunctionOK = TRUE;
	int nPower=0;

	int nLeftIndex = -1;
	int nRightIndex = -1;
	double pdbAtten[2]={0.0};
	double dbADC[2]={0};
	double dbK, dbC;
	int nTotalPoint;
	try
	{
		nTotalPoint = m_stPDLut.nLutCount;
		if (dwADC <= m_stPDLut.stLutData[0].dwADC)
		{
			nLeftIndex = 0;
			nRightIndex = 1;
		}
		else if (dwADC >= m_stPDLut.stLutData[nTotalPoint - 1].dwADC)
		{
			nLeftIndex = nTotalPoint - 1;
			nRightIndex = nTotalPoint - 2;
		}
		else
		{
			for (int i = 0;i < m_stPDLut.nLutCount - 1;i++)
			{
				if (dwADC >= m_stPDLut.stLutData[i].dwADC && dwADC <= m_stPDLut.stLutData[i + 1].dwADC)
				{
					nLeftIndex = i;
					nRightIndex = i + 1;
					break;
				}
			}
		}

		if (nLeftIndex > nTotalPoint || nLeftIndex < 0 || nRightIndex < 0)
		{
			strMsg.Format("计算PD定标点斜率截距索引：%d超出范围值存在错误！", nLeftIndex);
			throw (char*)(LPCTSTR)strMsg;
		}
		dbADC[0] = (double)m_stPDLut.stLutData[nLeftIndex].dwADC;
		pdbAtten[0] = (double)m_stPDLut.stLutData[nLeftIndex].nPower;

		dbADC[1] = (double)m_stPDLut.stLutData[nRightIndex].dwADC;
		pdbAtten[1] = (double)m_stPDLut.stLutData[nRightIndex].nPower;
		if (!LinearFitArray(dbADC, pdbAtten, 2, dbK, dbC))
		{
			strMsg.Format(_T("计算PD定标点斜率截距存在错误！"));
			throw (char*)(LPCTSTR)strMsg;
		}
		double dbTemp = dbK * dwADC + dbC;
		nPower = (int)dbTemp;
		pdbReportPower = (double)nPower / 10.0;
	}
	catch (char *pstError)
	{
		strMsg = pstError;
		LogInfo(strMsg,COLOR_RED);
		bFunctionOK = FALSE;
	}

	return bFunctionOK;
}

//////////////////////////////////////////////////////////////////////////////////
//函数说明
//   从产品中读取PD定标Lut表
//Add by wanxin
//Time:20171121
/////////////////////////////////////////////////////////////////////////////////
BOOL CListDlg::ReadPDCalLutFromEEROM(int nPDIndex, pPDCalLut pData)
{
	CString  strMsg;
	BOOL bFunctionOK = TRUE;
	int nReadLength = 0;
	int nCalPointCount = 0;
	PBYTE pbBuffer = NULL;
	WORD wAddress = 0;
	DWORD dwGetLength = 0;

	int nAtten;
	DWORD dwADC;

	try
	{
		nReadLength = pData->nLutCount * 8;

		pbBuffer = (BYTE*)VirtualAlloc(NULL, sizeof(BYTE)*MAX_PD_CAL_SAMPLE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		ZeroMemory(pbBuffer, sizeof(BYTE)*MAX_PD_CAL_SAMPLE);

		switch (nPDIndex)
		{
			case 1:
			{
				wAddress = EEROM_TABLE1_AD_ADRR;
				break;
			}
			case 2:
			{
				wAddress = EEROM_TABLE2_AD_ADRR;
				break;
			}
			case 3:
			{
				wAddress = EEROM_TABLE3_AD_ADRR;
				break;
			}
			case 4:
			{
				wAddress = EEROM_TABLE4_AD_ADRR;
				break;
			}
	
		}

		if (!ReadEEROMData(wAddress, pbBuffer, nReadLength, &dwGetLength))
		{
			strMsg.Format(_T("从产品中读取PD %d Lut表数存在错误！", nPDIndex));
			throw (char*)(LPCTSTR)strMsg;
		}
		int nCount = 0;
		for (int i = 0;i < nReadLength;i += 8)
		{
			pData->stLutData[nCount].nIndex = nCount;

			dwADC = 0xff000000 & (*(pbBuffer + i) << 24);
			dwADC += 0xffff0000 & (*(pbBuffer + i + 1) << 16);
			dwADC += 0xffffff00 & (*(pbBuffer + i + 2) << 8);
			dwADC += 0xffffffff & (*(pbBuffer + i + 3));

			pData->stLutData[nCount].dwADC = dwADC;

			nAtten = 0xff000000 & (*(pbBuffer + i + 4) << 24);
			nAtten += 0xffff0000 & (*(pbBuffer + i + 5) << 16);
			nAtten += 0xffffff00 & (*(pbBuffer + i + 6) << 8);
			nAtten += 0xffffffff & (*(pbBuffer + i + 7));

			pData->stLutData[nCount].nPower = nAtten;

			nCount++;
		}
		if (pData->nLutCount != nCount)
		{
			strMsg.Format(_T("从产品中读取PD %d Lut定标个数与实际给定不一致在错误！", nPDIndex));
			throw (char*)(LPCTSTR)strMsg;
		}
	}
	catch (char *pstError)
	{
		strMsg = pstError;
		LogInfo(strMsg, strMsg.GetLength() + 1, COLOR_RED);
		bFunctionOK = FALSE;
	}

	if (pbBuffer != NULL)
	{
		VirtualFree(pbBuffer, 0, MEM_RELEASE);
		pbBuffer = NULL;
	}
	return bFunctionOK;
}

/////////////////////////////////////////////////////////////////////
//函数说明：
//    读取PD ADC For ASCII码
//Add by wanxin
//Time:20171116
////////////////////////////////////////////////////////////////////
BOOL CListDlg::GetPDADCForASCII(int nPDIndex, PWORD pADCValue)
{
	char pbBuffer[MAX_PATH];
	DWORD dwFeedBackLength;
	CString strMsg;
	int ADCsum=0;
	int nbreak=0;
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);


	sprintf(pbBuffer, "PD %d ADC\r\n", nPDIndex);

	if (!m_OpCom.WriteBuffer(pbBuffer, strlen(pbBuffer)))
	{
		m_strErrorMsg = _T("发送读取PD ADC指令错误！");
		return FALSE;
	}
	Sleep(50);
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);
	if (!m_OpCom.ReadBuffer(pbBuffer, MAX_PATH, &dwFeedBackLength))
	{
		m_strErrorMsg = _T("发送读取PD ADC指令返回错误！");
		return FALSE;
	}
	strMsg = pbBuffer;
	*pADCValue =atoi(strMsg);
		
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    发送数据到IIC通讯
//Add by ChunliangW
//Time:2017-10-20
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CListDlg::TxDataToI2C(BYTE byIndex, WORD wAddress, BYTE *pbySetValue, DWORD dwSetLength)
{
	DWORD dwFeedBackLength;
	DWORD dwWaitTime = 100;
	BYTE bytSlaveAddr;
	CString strCommand,m_strErrorMsg;
	CString strTemp = "";
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
	BYTE byHiAddr = HIBYTE(wAddress);
	BYTE byLoAddr = LOBYTE(wAddress);
	bytSlaveAddr = SLAVE_WRITE;

	strCommand.Format("SetI2C %01x %02x %02x%02x", byIndex, bytSlaveAddr, byHiAddr, byLoAddr);
	for (DWORD n = 0;n < dwSetLength;n++)
	{
		strTemp.Format("%02x", pbySetValue[n]);
		strCommand = strCommand + strTemp;
	}
	strCommand = strCommand + "\r\n";
	ZeroMemory(chSetCommand, sizeof(char)*MAX_COUNT);
	memcpy(chSetCommand, strCommand, strCommand.GetLength());

	if (!m_OpCom.WriteBuffer(chSetCommand, strCommand.GetLength()))
	{
		m_strErrorMsg = "发送指令错误！";
		return FALSE;
	}
	Sleep(dwWaitTime);
	ZeroMemory(chReadCommand, sizeof(char)*MAX_COUNT);
	if (!m_OpCom.ReadBuffer(chReadCommand, MAX_COUNT, &dwFeedBackLength))
	{
		m_strErrorMsg = "读取I2C数据错误！";
		return FALSE;
	}

	strTemp = chReadCommand;

	if (strTemp.Find("E: Invalid Command") != -1)
	{
		m_strErrorMsg = "无效的指令";
		return FALSE;
	}
	else if (strTemp.Find("E: Invalid para") != -1)
	{
		m_strErrorMsg = "无效的参数";
		return FALSE;
	}
	else if (strTemp.Find("E: Invalid data") != -1)
	{
		m_strErrorMsg = "无效的数据";
		return FALSE;
	}
	else if (strTemp.Find("E: Execute Fail") != -1)
	{
		m_strErrorMsg = "指令执行错误";
		return FALSE;
	}

	return TRUE;
}

void CListDlg::AsiicToHex(BYTE *str, BYTE *hex, int cnt, int *nGetCount)
{
	int i, j;
	BYTE bTemp[3];
	CString strTemp;
	j = 0;
	for (i = 0; i < cnt - 1; i += 2)
	{
		ZeroMemory(bTemp, 3);
		memcpy(bTemp, str + i, 2);
		hex[j] = (BYTE)strtol((char*)bTemp, NULL, 16);
		j++;
	}

	*nGetCount = j;

}

////////////////////////////////////////////////////////////////////////////////
//函数说明：
//    根据IIC地址回读相应信息
//Add by ChunliangW
//Time：2017-10-20
//////////////////////////////////////////////////////////////////////////////
BOOL CListDlg::RxDataFromI2C(BYTE byIndex, WORD wAddress, DWORD dwReadLength, BYTE *pbySetValue, PDWORD pdwSetLength)
{
	DWORD dwFeedBackLength;
	DWORD dwWaitTime = 100;
	BYTE bytSlaveAddr;
	CString strCommand,m_strErrorMsg;
	CString strTemp = "";
	BYTE bSendBuffer;
	char        chSetCommand[MAX_COUNT];
	char        chReadCommand[MAX_COUNT];
	//bytSlaveAddr = SLAVE_WRITE;

	ZeroMemory(chSetCommand, sizeof(char)*MAX_COUNT);
	bytSlaveAddr = SLAVE_READ;//Read

	//这里需要先调用SetI2C才能调用GetI2C
	bSendBuffer = 0;
	if (!TxDataToI2C(byIndex, wAddress, &bSendBuffer, 0))
	{
		return FALSE;
	}
	Sleep(20);
	sprintf(chSetCommand, "GetI2C %02x %02x %02x\r\n", byIndex, bytSlaveAddr, dwReadLength);

	if (!m_OpCom.WriteBuffer(chSetCommand, strlen(chSetCommand)))
	{
		m_strErrorMsg = "发送指令错误！";
		return FALSE;
	}
	Sleep(dwWaitTime);
	ZeroMemory(chReadCommand, sizeof(char)*MAX_COUNT);
	if (!m_OpCom.ReadBuffer(chReadCommand, MAX_COUNT, &dwFeedBackLength))
	{
		m_strErrorMsg = "读取I2C数据错误";
		return FALSE;
	}

	strTemp = chReadCommand;

	if (strTemp.Find("E: Invalid Command") != -1)
	{
		m_strErrorMsg = "无效的指令";
		return FALSE;
	}
	else if (strTemp.Find("E: Invalid para") != -1)
	{
		m_strErrorMsg = "无效的参数";
		return FALSE;
	}
	else if (strTemp.Find("E: Invalid data") != -1)
	{
		m_strErrorMsg = "无效的数据";
		return FALSE;
	}
	else if (strTemp.Find("E: Execute Fail") != -1)
	{
		m_strErrorMsg = "指令执行错误";
		return FALSE;
	}
	int nGetLength;
	AsiicToHex((BYTE*)(chReadCommand), pbySetValue, dwFeedBackLength - 4, &nGetLength);
	*pdwSetLength = nGetLength;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//函数说明：
//   设置VOA DAC 通过ASCII指令码
//Add by wanxin
//Time:20171116
/////////////////////////////////////////////////////////////////////////////
BOOL CListDlg::SetVOAActualDACForASCII(int nVOAIndex, WORD wDACValue)
{
	char pbBuffer[MAX_PATH];
	DWORD dwFeedBackLength;
	CString strMsg,m_strErrorMsg;

	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);

	sprintf(pbBuffer, "VOA %d DAC %d\r\n", nVOAIndex, wDACValue);

	if (!m_OpCom.WriteBuffer(pbBuffer, strlen(pbBuffer)))
	{
		m_strErrorMsg = _T("发送设置VOA DAC指令错误！");
		return FALSE;
	}
	Sleep(50);
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);
	if (!m_OpCom.ReadBuffer(pbBuffer, MAX_PATH, &dwFeedBackLength))
	{
		m_strErrorMsg = _T("发送设置VOA DAC指令返回错误！");
		return FALSE;
	}
	strMsg.Format("%s", pbBuffer);
	strMsg.MakeUpper();
	if (strMsg.Find("OK") == -1)
	{
		m_strErrorMsg = _T("发送设置VOA DAC指令返回结果不为OK,实际返回：%s", strMsg);
		return FALSE;
	}
	return TRUE;
}

void CListDlg::OnRadio4() 
{
	// TODO: Add your control notification handler code here
	bPNtpye = 1;
	CString strMsg;
	for(int i=0;i<m_SWch.GetCount();i++)
	{
		m_SWch.DeleteString(i);i--;
		UpdateData(FALSE);
	}
	TCHAR carCH[6][13]={"A_TM2","A_TM1","B_TM2","B_TM1","TOA1","TOA2"};
	for (i=0;i<6;i++)
	{

		strMsg.Format("%s",carCH[i]);
		m_SWch.InsertString(i,strMsg);
	}
	m_SWch.SetCurSel(0);
}

void CListDlg::OnRadio5() 
{
	// TODO: Add your control notification handler code here
	bPNtpye =2;
	CString strMsg;
	for(int i=0;i<m_SWch.GetCount();i++)
	{
		m_SWch.DeleteString(i);i--;
		UpdateData(FALSE);
	}
	TCHAR carCH[2][13]={"TM1","TM2"};
	for (i=0;i<2;i++)
	{

		strMsg.Format("%s",carCH[i]);
		m_SWch.InsertString(i,strMsg);
	}
	m_SWch.SetCurSel(0);
}

void CListDlg::OnButton9() 
{
	// TODO: Add your control notification handler code here
		// 读取PD ADC值
	CString strMsg;
	int nPDActualIndex = -1;
	WORD	wADCValue;
	double dbReportPower = 0.0;
	if (!m_bOpenCOM)
	{
		strMsg.Format("串口没打开");
		LogInfo(strMsg,COLOR_RED);
		return;
	}
	int  nPDIndex= m_SWch.GetCurSel()+1;
	switch (nPDIndex)
		{
		case 1:
			{
				nPDActualIndex = 0;
				m_stPDLut.nLutCount  = 10;
				break;
			}
			case 2:
			{
				nPDActualIndex = 1;
				m_stPDLut.nLutCount  = 10;
				break;
			}
			case 3:
			{

				nPDActualIndex = 2;	
				m_stPDLut.nLutCount  = 16;
				break;
			}
			default:
				{
					nPDActualIndex = -1;
					break;
				}
	}	
	//从产品中读取PD Lut表数据
	if (!ReadPDCalLutFromEEROM(nPDIndex, &m_stPDLut))
	{
		strMsg.Format("从产品中读取PD %d Lut表数据存在错误", nPDIndex);
		LogInfo(strMsg,COLOR_RED);
	}
	if (!GetPDADCForASCII(nPDActualIndex, &wADCValue))
	{
		strMsg.Format("读取PD ADC失败！");
		LogInfo(strMsg,COLOR_RED);
	}


	if (!CalcuPDReportPowerByADC(wADCValue, dbReportPower))
	{
		strMsg.Format("通过PD ADC计算上报功率存在错误");
		LogInfo(strMsg,COLOR_RED);
	}
    else
	{
		strMsg.Format("PD%d:ADC:%d,ReportPower:%.3fdBm",nPDIndex,wADCValue,dbReportPower);
		LogInfo(strMsg,COLOR_BLUE);
	}
}

void CListDlg::OnButton10() 
{
	// TODO: Add your control notification handler code here
	CString strMsg,strALL;
	double dbPower =0.0;
	int nSelection;
	int nMPO=0;
	int nMPOCH=0;
	CString Port;
	BOOL nlist = 0;
	int mSWch1X64 =1;
	for(int mSWch1X2 = 1; mSWch1X2 < 3 ; mSWch1X2++)
	{
		mSWch1X64=1;
		if (!SetSwitchForASCII(4,1, mSWch1X2))
		{
			strMsg.Format("切1X2开关到%d通道失败",mSWch1X2);
			LogInfo(strMsg,COLOR_RED);
		}
		else
		{
			strMsg.Format("切1X2开关到%d通道成功",mSWch1X2);
			LogInfo(strMsg,COLOR_BLUE);
		}
		if(mSWch1X2==1)
		{
			MessageBox("提醒：请检查好FANOUT连接器端面，分别1~12接到PM进行测试！","提示",MB_YESNO | MB_ICONWARNING);
			Port = "IN_";
		}
		else
		{
			MessageBox("提醒：请把LC4接FANOUT LC头分别1~12，LC1接到PM上！","提示",MB_YESNO | MB_ICONWARNING);
			Port = "OUT_";
		}

	
		for(int mSWch1X18 = 1; mSWch1X18 < 65 ; mSWch1X18++)//64
		{
		
			if (!SetSwitchForASCII(1,mSWch1X2, mSWch1X64))
			{
				strMsg.Format("切#1 1X64开关到%d通道失败",mSWch1X64);
				LogInfo(strMsg,COLOR_RED);
			}
			else
			{
				strMsg.Format("切#1 1X64开关到%d通道成功",mSWch1X64);
				LogInfo(strMsg,COLOR_BLUE);
			}
			for(int mSWchx1X18 = 1; mSWchx1X18 < 19 ; mSWchx1X18++)//18
			{
	
				if(mSWchx1X18 > 12)
				{
					if(mSWch1X64%2)
					{
						if(mSWchx1X18==13)
						{
							nMPO ++;
						}
						nMPOCH = (mSWchx1X18)-12;						
					}
					else
					{
						nMPOCH = mSWchx1X18-6;
					}
				}
				else
				{
					if(mSWch1X64%2)
					{
						nMPOCH = mSWchx1X18;
						if(mSWchx1X18==1 && (mSWch1X64%2))
						nMPO ++;
					}
					else
					{
					   nMPOCH = mSWchx1X18+6;
					   if(nMPOCH > 12)
					   {
						   
						   if(nMPOCH==13)
						   {
							nMPO++;
						   }
						   nMPOCH = mSWchx1X18-6;
					   }
					}
				}
		

				strMsg.Format("请把MPO%d的第%d通道接到PM上",nMPO,nMPOCH);
				LogInfo(strMsg,COLOR_BLACK);
				nSelection = MessageBox(strMsg+"是：测试下一通道，否：跳过","提示",MB_YESNOCANCEL | MB_ICONWARNING);
				if (IDNO == nSelection)
				{
					continue;
				}
				if (nSelection == IDCANCEL)
				{
					return;
				}
				if (IDYES == nSelection)
				{

					if (!SetSwitchForASCII(2,mSWch1X18, mSWchx1X18))
					{
						strMsg.Format("切#1 MCS开关到%d通道失败",mSWchx1X18);
						LogInfo(strMsg,COLOR_RED);
					}
					else
					{
						strMsg.Format("切#1 MCS开关到%d通道成功",mSWchx1X18);
						LogInfo(strMsg,COLOR_BLUE);
					}
				
					if (!ReadPM(&dbPower))
					{
						strMsg.Format("读到的功率失败");
						LogInfo(strMsg,COLOR_RED);
					}
					else
					{
						strMsg.Format("读到的功率%.4f成功",dbPower);
						LogInfo(strMsg,COLOR_BLUE);
					}
					strALL.Format("%sMPO%d,CH%d,%.4f",Port, nMPO, nMPOCH,dbPower);
					LogInfoSave(strALL, strALL.GetLength());
				}
				

			}
				
			mSWch1X64++;
		}
	
	

	}

	strMsg.Format("开发中。。。。");
	LogInfo(strMsg,COLOR_RED);
}
void CListDlg::LogInfoSave(CString strLogInfo, int nLength)
{
	CString	strLogMsg, m_strLogFileName;
	FILE*	pLogFile = NULL;
	SYSTEMTIME time;

	char m_szAppFolder[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, (CHAR*)m_szAppFolder);
	m_strLogFileName.Format("%s\\data\\ALL_EEROM.Txt", m_szAppFolder);

	if (m_strLogFileName != "")
	{
		GetLocalTime(&time);

		strLogMsg.Format("%4d-%2d-%2d %2d:%2d:%2d %s", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, strLogInfo);

		pLogFile = fopen(m_strLogFileName, "at");

		fprintf(pLogFile, "%s\n", strLogMsg);
		fclose(pLogFile);
	}

}
void CListDlg::OnBUTTON1x8() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	UpdateData(TRUE);
	if (!m_bOpenCOM)
	{
		strMsg.Format("串口没打开");
		LogInfo(strMsg,COLOR_RED);
		return;
	}	
	int mSWch = m_1X8.GetCurSel()+1;
	if (!SetSwitchForASCII(3,1, mSWch))
	{
		strMsg.Format("切1X8开关到%d通道失败",mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
    else
	{
		strMsg.Format("切1X8开关到%d通道成功",mSWch);
		LogInfo(strMsg,COLOR_BLUE);
	}
	
}
BOOL CListDlg::SetSwitchForASCII(int nSwtype,int nindex,int nChannal)
{
	char pbBuffer[MAX_PATH];
	DWORD dwFeedBackLength;
	CString strMsg,m_strErrorMsg;
    //return true;
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);
	switch(nSwtype) 
	{
	case 4://	为1X2 
		sprintf(pbBuffer, "sw %d %d\r\n", nSwtype,nChannal);
		break;
	case 3://为1X8 
		sprintf(pbBuffer, "sw %d %d %d\r\n", nSwtype,nindex,nChannal);
		break;
	case 2://1 为1X18
		sprintf(pbBuffer, "sw %d %d %d\r\n", nSwtype,nindex,nChannal);
		break;
	case 1://1 为1X64
		sprintf(pbBuffer, "sw %d %d %d\r\n", nSwtype,nindex,nChannal);
		break;
	default:
		break;
	}

	if (!m_OpCom.WriteBuffer(pbBuffer, strlen(pbBuffer)))
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令错误！");
		return FALSE;
	}
	Sleep(100);
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);
	if (!m_OpCom.ReadBuffer(pbBuffer, MAX_PATH, &dwFeedBackLength))
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令返回错误！");
		return FALSE;
	}
	strMsg = pbBuffer;
	if (strMsg.Find("OK") == -1)
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令返回%s错误！", strMsg);
		return FALSE;
	}
	return TRUE;
}
BOOL CListDlg::ReadPM(double *dbPower)
{
	char pbBuffer[MAX_PATH];
	DWORD dwFeedBackLength;
	CString strMsg,m_strErrorMsg;
    //return true;
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);

	sprintf(pbBuffer, "OPM 3 1\r\n");


	if (!m_OpCom.WriteBuffer(pbBuffer, strlen(pbBuffer)))
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令错误！");
		return FALSE;
	}
	Sleep(100);
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);
	if (!m_OpCom.ReadBuffer(pbBuffer, MAX_PATH, &dwFeedBackLength))
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令返回错误！");
		return FALSE;
	}
	strMsg = pbBuffer;
	*dbPower = (atof(strMsg))/10000;

	return TRUE;
}
BOOL CListDlg::ReadTLSPM(double *dbPower)
{
	char pbBuffer[MAX_PATH];
	DWORD dwFeedBackLength;
	CString strMsg,m_strErrorMsg;

	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);

	sprintf(pbBuffer, "PD 1\r\n");


	if (!m_OpCom.WriteBuffer(pbBuffer, strlen(pbBuffer)))
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令错误！");
		return FALSE;
	}
	Sleep(100);
	ZeroMemory(pbBuffer, sizeof(char)*MAX_PATH);
	if (!m_OpCom.ReadBuffer(pbBuffer, MAX_PATH, &dwFeedBackLength))
	{
		m_strErrorMsg = _T("发送设置产品1x2开关指令返回错误！");
		return FALSE;
	}
	strMsg = pbBuffer;
	*dbPower = (atof(strMsg))/10;

	return TRUE;
}
void CListDlg::OnBUTTON1x2() 
{
	// TODO: Add your control notification handler code here
		CString strMsg;
	UpdateData(TRUE);
	if (!m_bOpenCOM)
	{
		strMsg.Format("串口没打开");
		LogInfo(strMsg,COLOR_RED);
		return;
	}	
	int mSWch = m_1X2.GetCurSel()+1;
	if (!SetSwitchForASCII(4,1, mSWch))
	{
		strMsg.Format("切1X2开关到%d通道失败",mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
    else
	{
		strMsg.Format("切1X2开关到%d通道成功",mSWch);
		LogInfo(strMsg,COLOR_BLUE);
	}
}

void CListDlg::OnBUTTON1x64() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	UpdateData(TRUE);
	if (!m_bOpenCOM)
	{
		strMsg.Format("串口没打开");
		LogInfo(strMsg,COLOR_RED);
		return;
	}	
	int mSWch = m_1X64_1.GetCurSel()+1;
	if (!SetSwitchForASCII(1,1, mSWch))
	{
		strMsg.Format("切#1 1X64开关到%d通道失败",mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
    else
	{
		strMsg.Format("切#1 1X64开关到%d通道成功",mSWch);
		LogInfo(strMsg,COLOR_BLUE);
	}
}

void CListDlg::OnBUTTON1x11() 
{
	// TODO: Add your control notification handler code here
		CString strMsg;
	UpdateData(TRUE);
	if (!m_bOpenCOM)
	{
		strMsg.Format("串口没打开");
		LogInfo(strMsg,COLOR_RED);
		return;
	}	
	int mSWch = m_1X64_2.GetCurSel()+1;
	if (!SetSwitchForASCII(1,2, mSWch))
	{
		strMsg.Format("切#2 1X64开关到%d通道失败",mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
    else
	{
		strMsg.Format("切#2 1X64开关到%d通道成功",mSWch);
		LogInfo(strMsg,COLOR_BLUE);
	}
}

void CListDlg::OnBUTTON1x12() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	UpdateData(TRUE);
	if (!m_bOpenCOM)
	{
		strMsg.Format("串口没打开");
		LogInfo(strMsg,COLOR_RED);
		return;
	}	
	int mSWch = m_MCS_1.GetCurSel()+1;
	int mSW1ch = m_MCS_CH1.GetCurSel()+1;
	if (!SetSwitchForASCII(2,mSW1ch, mSWch))
	{
		strMsg.Format("切#1 MCS开关到%d通道失败",mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
    else
	{
		strMsg.Format("切#1 MCS开关到%d通道成功",mSWch);
		LogInfo(strMsg,COLOR_BLUE);
	}	
}

void CListDlg::OnBUTTON1x13() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	UpdateData(TRUE);
	if (!m_bOpenCOM)
	{
		strMsg.Format("串口没打开");
		LogInfo(strMsg,COLOR_RED);
		return;
	}	
	int mSWch = m_MCS_2.GetCurSel()+1;
	int mSW2ch = m_MCS_CH2.GetCurSel()+1+32;
	if (!SetSwitchForASCII(2,mSW2ch, mSWch))
	{
		strMsg.Format("切#2 MCS开关到%d通道失败",mSWch);
		LogInfo(strMsg,COLOR_RED);
	}
    else
	{
		strMsg.Format("切#2 MCS开关到%d通道成功",mSWch);
		LogInfo(strMsg,COLOR_BLUE);
	}	
}

void CListDlg::OnButton11() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	double dbPower =0.0;
	if (!ReadPM(&dbPower))
	{
		strMsg.Format("功率失败");
		LogInfo(strMsg,COLOR_RED);
	}
	else
	{
			strMsg.Format("功率为：%.4fdBm成功",dbPower);
		LogInfo(strMsg,COLOR_BLUE);
	}
}

void CListDlg::OnButton12() 
{
	// TODO: Add your control notification handler code here
		CString strMsg;
	double dbPower =0.0;
	if (!ReadTLSPM(&dbPower))
	{
		strMsg.Format("光源的功率失败");
		LogInfo(strMsg,COLOR_RED);
	}
	else
	{
			strMsg.Format("光源的功率为：%.2fdBm成功",dbPower);
		LogInfo(strMsg,COLOR_BLUE);
	}
}
