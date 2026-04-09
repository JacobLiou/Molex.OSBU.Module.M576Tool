// XMLControl.h: interface for the CXMLControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLCONTROL_H__4CCD1197_00B7_4847_BFF4_189280407ABA__INCLUDED_)
#define AFX_XMLCONTROL_H__4CCD1197_00B7_4847_BFF4_189280407ABA__INCLUDED_

//#include "ColorListBox.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CP_ACP                    0           // default to ANSI code page
#define CP_OEMCP                  1           // default to OEM  code page
#define CP_MACCP                  2           // default to MAC  code page
#define CP_THREAD_ACP             3           // current thread's ANSI code page
#define CP_SYMBOL                 42          // SYMBOL translations

#define CP_UTF7                   65000       // UTF-7 translation
#define CP_UTF8                   65001       // UTF-8 translation

class CXMLControl  
{
public:
//	CColorListBox m_ctrMsgList;
	BOOL GetHtmlSource(CString strURL, CString strDataFile, int nCodeType = CP_UTF8);
	CString VAR2Cstr(VARIANT varValue);
	BOOL GetNodeAttr(CString strAttrTag, CString &strContent, DWORD dwNodePtr);
	BOOL CloseXMLFile();
	BOOL ModifyNode(CString strTagName, CString strContent, DWORD dwNodePtr);
	BOOL SaveXmlFile(CString strTarget);
	BOOL LoadXmlFile(CString strXMLFile);
	BOOL FindNodeByTag(CString &strErr, CString strTagName, CString strContent, PDWORD pRetBuf, int &nCount, DWORD dwParent=0);
	CXMLControl();
	virtual ~CXMLControl();

private:
	CString	m_strFileName;
	MSXML2::IXMLDOMDocumentPtr m_pDoc;

};

#endif // !defined(AFX_XMLCONTROL_H__4CCD1197_00B7_4847_BFF4_189280407ABA__INCLUDED_)
