#if !defined(AFX_SRCBANKSOCKET_H__12396D83_9D7B_11D6_B27F_00E01808A7F0__INCLUDED_)
#define AFX_SRCBANKSOCKET_H__12396D83_9D7B_11D6_B27F_00E01808A7F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SrcBankSocket.h : header file
//

#include "afxsock.h"
//#include "SAStatusLog.h"

/////////////////////////////////////////////////////////////////////////////
// CSrcBankSocket command target

class CSrcBankSocket : public CSocket
{
// Attributes
public:
	HWND	hWnd;
	DWORD	m_dwSocketIndex;
	int		m_nSocketSN;
	//Added by ris
	BOOL    m_bFunctionOK;
	CString m_strError;

// Operations
public:
	CSrcBankSocket();
	virtual ~CSrcBankSocket();
    
	BOOL Open();
	BOOL AnalysisMsg(CString strRecvMsg, CString* strNewRes, char* pszErrorMsg);
	BOOL InitLogFile(BOOL bFirstOpen, int nFileIndex);

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSrcBankSocket)
	public:
//	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode); 
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSrcBankSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	CWnd*	m_pParentWnd;
	CHAR	m_pszRespondMsg[1024];
	int 	m_nBytesTotal;
	char	m_pszRecvTemp[1024];
	int				m_nReceiveCount;	//OnReceive接收次数
	CString			m_strLogFile;
	//CSAStatusLog	//m_cLogOnReceive;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SRCBANKSOCKET_H__12396D83_9D7B_11D6_B27F_00E01808A7F0__INCLUDED_)
