#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576CalibratorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CM576CalibratorApp, CWinAppEx)
END_MESSAGE_MAP()

CM576CalibratorApp::CM576CalibratorApp()
{
}

CM576CalibratorApp theApp;

BOOL CM576CalibratorApp::InitInstance()
{
	CWinAppEx::InitInstance();

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	CM576CalibratorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	return FALSE;
}
