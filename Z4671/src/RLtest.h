#if !defined(AFX_RLTEST_H__BDD23AC5_4729_4B17_B6E1_FF239B5D67D0__INCLUDED_)
#define AFX_RLTEST_H__BDD23AC5_4729_4B17_B6E1_FF239B5D67D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RLtest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRLtest dialog

class CRLtest : public CDialog
{
// Construction
public:
	CRLtest(CWnd* pParent = NULL);   // standard constructor
	double  m_dbRL[20];

// Dialog Data
	//{{AFX_DATA(CRLtest)
	enum { IDD = IDD_DIALOG_RL };
	double	m_dblRL1;
	double	m_dblRL2;
	double	m_dblRL3;
	double	m_dblRL4;
	double	m_dblRL5;
	double	m_dblRL6;
	double	m_dblRL7;
	double	m_dblRL8;
	double	m_dblRL9;
	double	m_dblRL10;
	double	m_dblRL11;
	double	m_dblRL12;
	double	m_dblRL13;
	double	m_dblRL14;
	double	m_dblRL15;
	double	m_dblRL16;
	double	m_dblRL17;
	double	m_dblRL18;
	double	m_dblRL19;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRLtest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRLtest)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RLTEST_H__BDD23AC5_4729_4B17_B6E1_FF239B5D67D0__INCLUDED_)
