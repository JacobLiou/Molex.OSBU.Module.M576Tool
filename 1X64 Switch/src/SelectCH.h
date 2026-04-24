#if !defined(AFX_SELECTCH_H__07DDC05B_5FF2_40C9_BA04_5C56309CC842__INCLUDED_)
#define AFX_SELECTCH_H__07DDC05B_5FF2_40C9_BA04_5C56309CC842__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectCH.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectCH dialog

class CSelectCH : public CDialog
{
// Construction
public:
	CSelectCH(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectCH)
	enum { IDD = IDD_DIALOG_CH_SELECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString m_strPN;
	CString m_strSN;
	int     m_nCHCount;
	CString m_strPath;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectCH)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectCH)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTCH_H__07DDC05B_5FF2_40C9_BA04_5C56309CC842__INCLUDED_)
