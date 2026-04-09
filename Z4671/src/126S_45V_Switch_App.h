// 126S_45V_Switch_App.h : main header file for the 126S_45V_SWITCH_APP application
//

#if !defined(AFX_126S_45V_SWITCH_APP_H__AAE9D1BA_2916_4A45_9841_7ECE2FA3DD66__INCLUDED_)
#define AFX_126S_45V_SWITCH_APP_H__AAE9D1BA_2916_4A45_9841_7ECE2FA3DD66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy126S_45V_Switch_AppApp:
// See 126S_45V_Switch_App.cpp for the implementation of this class
//

class CMy126S_45V_Switch_AppApp : public CWinApp
{
public:
	CMy126S_45V_Switch_AppApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy126S_45V_Switch_AppApp)
	public:
	virtual BOOL InitInstance();
	afx_msg void OnRefreshDataMessage(WPARAM wPara, LPARAM lPara);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy126S_45V_Switch_AppApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_126S_45V_SWITCH_APP_H__AAE9D1BA_2916_4A45_9841_7ECE2FA3DD66__INCLUDED_)
