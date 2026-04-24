// SelectCH.cpp : implementation file
//

#include "stdafx.h"
#include "126s_45v_switch_app.h"
#include "SelectCH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectCH dialog


CSelectCH::CSelectCH(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectCH::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectCH)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelectCH::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectCH)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectCH, CDialog)
	//{{AFX_MSG_MAP(CSelectCH)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectCH message handlers

void CSelectCH::OnOK() 
{
	// TODO: Add extra validation here
	// TODO: Add extra validation here
	CString strCHINIFile;
	CStdioFile stdFile;
	CString strKey;
	CString strValue;
	strCHINIFile.Format("%s\\data\\终测\\%s\\%s\\%s_SelectCH.ini",m_strPath,m_strPN,m_strSN,m_strSN);
	if (!stdFile.Open(strCHINIFile,CFile::modeCreate|CFile::modeReadWrite))
	{
		MessageBox("创建通道选择配置文件错误！");
		return;
	}
	stdFile.Close();
	for (int i=0;i<m_nCHCount;i++)
	{
		strKey.Format("CH%d",i+1);
		GetDlgItemText(IDC_EDIT_CH1+i,strValue);
		WritePrivateProfileString("SelectCH",strKey,strValue,strCHINIFile);
	}
	
	CDialog::OnOK();
}

BOOL CSelectCH::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strValue;
	for (int i=0;i<19;i++)
	{
		strValue.Format("%d",i+1);
		SetDlgItemText(IDC_EDIT_CH1+i,strValue);
	}
	
	if(m_nCHCount<12)
	{
		GetDlgItem(IDC_EDIT_CH19)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH19)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH18)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH18)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH17)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH17)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH16)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH16)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH15)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH15)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH14)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH14)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH13)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH13)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH12)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH12)->ShowWindow(FALSE);
	}
	if (m_nCHCount<11)
	{
		GetDlgItem(IDC_EDIT_CH11)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH11)->ShowWindow(FALSE);
	}
	if (m_nCHCount<10)
	{
		GetDlgItem(IDC_EDIT_CH10)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH10)->ShowWindow(FALSE);
	}
	if (m_nCHCount<9)
	{
		GetDlgItem(IDC_EDIT_CH9)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH9)->ShowWindow(FALSE);
	}
	if (m_nCHCount<8)
	{
		GetDlgItem(IDC_EDIT_CH8)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH8)->ShowWindow(FALSE);
	}
	if (m_nCHCount<7)
	{
		GetDlgItem(IDC_EDIT_CH7)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH7)->ShowWindow(FALSE);
	}
	if (m_nCHCount<6)
	{
		GetDlgItem(IDC_EDIT_CH6)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH6)->ShowWindow(FALSE);
	}
	if (m_nCHCount<5)
	{
		GetDlgItem(IDC_EDIT_CH5)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH5)->ShowWindow(FALSE);
	}
	if (m_nCHCount<4)
	{
		GetDlgItem(IDC_EDIT_CH4)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH4)->ShowWindow(FALSE);
	}
	if (m_nCHCount<3)
	{
		GetDlgItem(IDC_EDIT_CH3)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH3)->ShowWindow(FALSE);
	}
	if (m_nCHCount<2)
	{
		GetDlgItem(IDC_EDIT_CH2)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_CH2)->ShowWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
