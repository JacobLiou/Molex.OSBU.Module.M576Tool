#if !defined(AFX_TESTRL_H__A3B405A3_9571_4461_93E7_7A14AA9B506D__INCLUDED_)
#define AFX_TESTRL_H__A3B405A3_9571_4461_93E7_7A14AA9B506D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestRL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestRL dialog

class CTestRL : public CDialog
{
// Construction
public:
	CTestRL(CWnd* pParent = NULL);   // standard constructor
	double  m_dbRL[20];

// Dialog Data
	//{{AFX_DATA(CTestRL)
	enum { IDD = IDD_DIALOG_RL };
	double	m_dbRL1;
	double	m_dbRL10;
	double	m_dbRL11;
	double	m_dbRL12;
	double	m_dbRL13;
	double	m_dbRL14;
	double	m_dbRL15;
	double	m_dbRL16;
	double	m_dbRL17;
	double	m_dbRL18;
	double	m_dbRL19;
	double	m_dbRL2;
	double	m_dbRL3;
	double	m_dbRL4;
	double	m_dbRL5;
	double	m_dbRL6;
	double	m_dbRL7;
	double	m_dbRL8;
	double	m_dbRL9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestRL)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTestRL)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTRL_H__A3B405A3_9571_4461_93E7_7A14AA9B506D__INCLUDED_)
