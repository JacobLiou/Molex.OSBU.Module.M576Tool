// TestRL.cpp : implementation file
//

#include "stdafx.h"
#include "126s_45v_switch_app.h"
#include "TestRL.h"
#include "126S_45V_Switch_AppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestRL dialog


CTestRL::CTestRL(CWnd* pParent /*=NULL*/)
	: CDialog(CTestRL::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestRL)
	m_dbRL1 = 0.0;
	m_dbRL10 = 0.0;
	m_dbRL11 = 0.0;
	m_dbRL12 = 0.0;
	m_dbRL13 = 0.0;
	m_dbRL14 = 0.0;
	m_dbRL15 = 0.0;
	m_dbRL16 = 0.0;
	m_dbRL17 = 0.0;
	m_dbRL18 = 0.0;
	m_dbRL19 = 0.0;
	m_dbRL2 = 0.0;
	m_dbRL3 = 0.0;
	m_dbRL4 = 0.0;
	m_dbRL5 = 0.0;
	m_dbRL6 = 0.0;
	m_dbRL7 = 0.0;
	m_dbRL8 = 0.0;
	m_dbRL9 = 0.0;
	//}}AFX_DATA_INIT
}


void CTestRL::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestRL)
	DDX_Text(pDX, IDC_EDIT_RL1, m_dbRL1);
	DDX_Text(pDX, IDC_EDIT_RL10, m_dbRL10);
	DDX_Text(pDX, IDC_EDIT_RL11, m_dbRL11);
	DDX_Text(pDX, IDC_EDIT_RL12, m_dbRL12);
	DDX_Text(pDX, IDC_EDIT_RL13, m_dbRL13);
	DDX_Text(pDX, IDC_EDIT_RL14, m_dbRL14);
	DDX_Text(pDX, IDC_EDIT_RL15, m_dbRL15);
	DDX_Text(pDX, IDC_EDIT_RL16, m_dbRL16);
	DDX_Text(pDX, IDC_EDIT_RL17, m_dbRL17);
	DDX_Text(pDX, IDC_EDIT_RL18, m_dbRL18);
	DDX_Text(pDX, IDC_EDIT_RL19, m_dbRL19);
	DDX_Text(pDX, IDC_EDIT_RL2, m_dbRL2);
	DDX_Text(pDX, IDC_EDIT_RL3, m_dbRL3);
	DDX_Text(pDX, IDC_EDIT_RL4, m_dbRL4);
	DDX_Text(pDX, IDC_EDIT_RL5, m_dbRL5);
	DDX_Text(pDX, IDC_EDIT_RL6, m_dbRL6);
	DDX_Text(pDX, IDC_EDIT_RL7, m_dbRL7);
	DDX_Text(pDX, IDC_EDIT_RL8, m_dbRL8);
	DDX_Text(pDX, IDC_EDIT_RL9, m_dbRL9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestRL, CDialog)
	//{{AFX_MSG_MAP(CTestRL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestRL message handlers

void CTestRL::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	m_dbRL[0] = m_dbRL1;
	m_dbRL[1] = m_dbRL2;
	m_dbRL[2] = m_dbRL3;
	m_dbRL[3] = m_dbRL4;
	m_dbRL[4] = m_dbRL5;
	m_dbRL[5] = m_dbRL6;
	m_dbRL[6] = m_dbRL7;
	m_dbRL[7] = m_dbRL8;
	m_dbRL[8] = m_dbRL9;
	m_dbRL[9] = m_dbRL10;
	m_dbRL[10] = m_dbRL11;
	m_dbRL[11] = m_dbRL12;
	m_dbRL[12] = m_dbRL13;
	m_dbRL[13] = m_dbRL14;
	m_dbRL[14] = m_dbRL15;
	m_dbRL[15] = m_dbRL16;
	m_dbRL[16] = m_dbRL17;
	m_dbRL[17] = m_dbRL18;
	m_dbRL[18] = m_dbRL19;
	CDialog::OnOK();
}
