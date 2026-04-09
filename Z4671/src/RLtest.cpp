// RLtest.cpp : implementation file
//

#include "stdafx.h"
#include "126s_45v_switch_app.h"
#include "RLtest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRLtest dialog


CRLtest::CRLtest(CWnd* pParent /*=NULL*/)
	: CDialog(CRLtest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRLtest)
	m_dblRL1 = 0.0;
	m_dblRL2 = 0.0;
	m_dblRL3 = 0.0;
	m_dblRL4 = 0.0;
	m_dblRL5 = 0.0;
	m_dblRL6 = 0.0;
	m_dblRL7 = 0.0;
	m_dblRL8 = 0.0;
	m_dblRL9 = 0.0;
	m_dblRL10 = 0.0;
	m_dblRL11 = 0.0;
	m_dblRL12 = 0.0;
	m_dblRL13 = 0.0;
	m_dblRL14 = 0.0;
	m_dblRL15 = 0.0;
	m_dblRL16 = 0.0;
	m_dblRL17 = 0.0;
	m_dblRL18 = 0.0;
	m_dblRL19 = 0.0;
	//}}AFX_DATA_INIT
}


void CRLtest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRLtest)
	DDX_Text(pDX, IDC_EDIT_RL1, m_dblRL1);
	DDX_Text(pDX, IDC_EDIT_RL2, m_dblRL2);
	DDX_Text(pDX, IDC_EDIT_RL3, m_dblRL3);
	DDX_Text(pDX, IDC_EDIT_RL4, m_dblRL4);
	DDX_Text(pDX, IDC_EDIT_RL5, m_dblRL5);
	DDX_Text(pDX, IDC_EDIT_RL6, m_dblRL6);
	DDX_Text(pDX, IDC_EDIT_RL7, m_dblRL7);
	DDX_Text(pDX, IDC_EDIT_RL8, m_dblRL8);
	DDX_Text(pDX, IDC_EDIT_RL9, m_dblRL9);
	DDX_Text(pDX, IDC_EDIT_RL10, m_dblRL10);
	DDX_Text(pDX, IDC_EDIT_RL11, m_dblRL11);
	DDX_Text(pDX, IDC_EDIT_RL12, m_dblRL12);
	DDX_Text(pDX, IDC_EDIT_RL13, m_dblRL13);
	DDX_Text(pDX, IDC_EDIT_RL14, m_dblRL14);
	DDX_Text(pDX, IDC_EDIT_RL15, m_dblRL15);
	DDX_Text(pDX, IDC_EDIT_RL16, m_dblRL16);
	DDX_Text(pDX, IDC_EDIT_RL17, m_dblRL17);
	DDX_Text(pDX, IDC_EDIT_RL18, m_dblRL18);
	DDX_Text(pDX, IDC_EDIT_RL19, m_dblRL19);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRLtest, CDialog)
	//{{AFX_MSG_MAP(CRLtest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRLtest message handlers

void CRLtest::OnOK() 
{
	// TODO: Add extra validation here
	
	UpdateData();
	m_dbRL[0] = m_dblRL1;
	m_dbRL[1] = m_dblRL2;
	m_dbRL[2] = m_dblRL3;
	m_dbRL[3] = m_dblRL4;
	m_dbRL[4] = m_dblRL5;
	m_dbRL[5] = m_dblRL6;
	m_dbRL[6] = m_dblRL7;
	m_dbRL[7] = m_dblRL8;
	m_dbRL[8] = m_dblRL9;
	m_dbRL[9] = m_dblRL10;
	m_dbRL[10] = m_dblRL11;
	m_dbRL[11] = m_dblRL12;
	m_dbRL[12] = m_dblRL13;
	m_dbRL[13] = m_dblRL14;
	m_dbRL[14] = m_dblRL15;
	m_dbRL[15] = m_dblRL16;
	m_dbRL[16] = m_dblRL17;
	m_dbRL[17] = m_dblRL18;
	m_dbRL[18] = m_dblRL19;
	CDialog::OnOK();
}
