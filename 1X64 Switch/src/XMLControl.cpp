// XMLControl.cpp: implementation of the CXMLControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "CTestClient.h"
//
//#include "CTestClientDlg.h"
//#include "TLSSettingDlg.h"
// #include "ColorListBox.h"
// #include "oplinkuniformparatool.h"
#include "XMLControl.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLControl::CXMLControl()
{
	m_pDoc = NULL;
	m_strFileName.Empty();
}

CXMLControl::~CXMLControl()
{

}

BOOL CXMLControl::FindNodeByTag(
	CString &strErr, CString strTagName, CString strContent, PDWORD pRetBuf, int &nCount, DWORD dwParent)
{
	try
	{
		if (m_pDoc == NULL)
			throw "请先选择xml文件!";
		
		//MSXML2::IXMLDOMNodePtr pNodePtr = NULL;
		MSXML2::IXMLDOMNodePtr pNodePtr = NULL;
		MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
		HRESULT hr;
		if (dwParent == 0)
			//hr = m_pDoc->get_childNodes(&pNodeList);
			pNodePtr = m_pDoc->selectSingleNode("//");
		else
			//hr = pNodePtr->get_childNodes(&pNodeList);
			pNodePtr.Attach((MSXML2::IXMLDOMNode*)dwParent);
		
		//if (!SUCCEEDED(hr))
		if (pNodePtr == NULL)
			throw "无法读取xml文件内容";
		
		hr = pNodePtr->get_childNodes(&pNodeList);
		if (!SUCCEEDED(hr))
			throw "无法读取xml文件内容";
		
		long lListItem;
		hr = pNodeList->get_length(&lListItem);
		if (!SUCCEEDED(hr))
			throw "无法读取xml文件内容";

		nCount = 0;
		for (long i = 0; i < lListItem; i++)
		{
			MSXML2::IXMLDOMNodePtr pCurNodePtr = NULL;
			hr = pNodeList->get_item(i, &pCurNodePtr);
			if (!SUCCEEDED(hr))
				throw "无法读取xml文件内容";
			
			BSTR bstrTemp;
			CString	strTemp;
			if (strTagName.Compare("BaseName") == 0)
			{
				pCurNodePtr->get_baseName(&bstrTemp);
				if (!SUCCEEDED(hr))
					throw "无法读取xml文件内容";
				strTemp = bstrTemp;
			}
			else
			{
				MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap = NULL;
				hr = pCurNodePtr->get_attributes(&pAttrMap);
				if (!SUCCEEDED(hr))
					throw "无法读取xml文件内容";

				MSXML2::IXMLDOMNodePtr pAttrNode = pAttrMap->getNamedItem(strTagName.AllocSysString());
				if (pAttrNode == NULL)
					strTemp.Empty();
				else
				{
					VARIANT	varValue;
					hr = pAttrNode->get_nodeValue(&varValue);
					if (!SUCCEEDED(hr))
						throw "无法读取xml文件内容";
					strTemp = varValue.bstrVal;
				}
			}

			if (strTemp.Compare(strContent) == 0)
				pRetBuf[nCount++] = (DWORD)pCurNodePtr.Detach();
		}

		pNodePtr.Detach();
		return TRUE;
	}

	catch (LPTSTR lpErrMsg)
	{
		strErr = (lpErrMsg);
		return FALSE;
	}
}

BOOL CXMLControl::LoadXmlFile(CString strXMLFile)
{
	HRESULT hr = m_pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (!SUCCEEDED(hr))
		return FALSE;

	if (VARIANT_FALSE == m_pDoc->load(strXMLFile.AllocSysString()))
		return FALSE;

	m_strFileName = strXMLFile;
	return TRUE;
}

BOOL CXMLControl::SaveXmlFile(CString strTarget)
{
	if (m_pDoc != NULL)
	{
		HRESULT hr = m_pDoc->save(strTarget.AllocSysString());
		if (!SUCCEEDED(hr))
			return FALSE;
	}

	return TRUE;
}

BOOL CXMLControl::ModifyNode(CString strTagName, CString strContent, DWORD dwNodePtr)
{
	try
	{
		if (m_pDoc == NULL)
			throw "请先选择xml文件!";
		
		MSXML2::IXMLDOMNodePtr pNodePtr;
		pNodePtr.Attach((MSXML2::IXMLDOMNode*)dwNodePtr);
		HRESULT hr;
		
		if (strTagName.Compare("BaseName") == 0)
		{
		}
		else if (strTagName.Compare("NodeValue") == 0)
		{
			VARIANT varNodeValue;
			varNodeValue.vt = VT_BSTR;
			varNodeValue.bstrVal = strContent.AllocSysString();
			hr = pNodePtr->put_nodeTypedValue(varNodeValue);
			if (!SUCCEEDED(hr))
				throw "无法读取xml文件内容";
		}
		else
		{
			MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap = NULL;
			hr = pNodePtr->get_attributes(&pAttrMap);
			if (!SUCCEEDED(hr))
				throw "无法读取xml文件内容";
			
			MSXML2::IXMLDOMNodePtr pAttrNode = pAttrMap->getNamedItem(strTagName.AllocSysString());
			if (pAttrNode == NULL)
				throw "无法读取xml文件内容";
			else
			{
				VARIANT	varValue;
				varValue.vt = VT_BSTR;
				varValue.bstrVal = strContent.AllocSysString();
				hr = pAttrNode->put_nodeValue(varValue);
				if (!SUCCEEDED(hr))
					throw "无法读取xml文件内容";
			}
		}
	
		pNodePtr.Detach();
		return TRUE;
	}
	
	catch (LPTSTR lpErrMsg)
	{
		//修改重复弹出对话框 by nieruijiang 2012-10-25
//		m_ctrMsgList.AddString(lpErrMsg,COLOR_RED);
		//AfxMessageBox(lpErrMsg);
		return FALSE;
	}
}

BOOL CXMLControl::CloseXMLFile()
{
	BOOL	bFun = TRUE;
	if (m_pDoc != NULL)
	{
		HRESULT hr = m_pDoc->save(m_strFileName.AllocSysString());
//		m_pDoc = NULL;
		if (SUCCEEDED(hr))
			bFun = TRUE;
		else
			bFun = FALSE;
	}

	return bFun;
}

BOOL CXMLControl::GetNodeAttr(CString strAttrTag, CString &strContent, DWORD dwNodePtr)
{
	try
	{
		MSXML2::IXMLDOMNodePtr pNodePtr = NULL;
		MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
		HRESULT hr;
		pNodePtr.Attach((MSXML2::IXMLDOMNode*)dwNodePtr);

		if (pNodePtr == NULL)
			throw "无法读取xml文件内容";
		
		if (strAttrTag.Compare("BaseName") == 0)
		{
			BSTR	bstrBaseName;
			hr = pNodePtr->get_baseName(&bstrBaseName);
			if (!SUCCEEDED(hr))
				throw "无法读取xml文件内容";
			strContent = bstrBaseName;
		}
		else if (strAttrTag.Compare("NodeValue") == 0)
		{
			VARIANT varValue;
			hr = pNodePtr->get_nodeTypedValue(&varValue);
			if (!SUCCEEDED(hr))
				throw "无法读取xml文件内容";

			strContent = VAR2Cstr(varValue);
		}
		else
		{
			MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap = NULL;
			hr = pNodePtr->get_attributes(&pAttrMap);
			if (!SUCCEEDED(hr))
				throw "无法读取xml文件内容";
			
			MSXML2::IXMLDOMNodePtr pAttrNode = pAttrMap->getNamedItem(strAttrTag.AllocSysString());
			if (pAttrNode == NULL)
				throw "无法读取xml文件内容";
			else
			{
				VARIANT	varValue;
				hr = pAttrNode->get_nodeValue(&varValue);
				if (!SUCCEEDED(hr))
					throw "无法读取xml文件内容";
				strContent = VAR2Cstr(varValue);
			}
		}
		
		pNodePtr.Detach();
		return TRUE;
	}
	
	catch (LPTSTR lpErrMsg)
	{
		return FALSE;
	}
}

CString CXMLControl::VAR2Cstr(VARIANT varValue)
{
	CString	strRet;
	switch(varValue.vt)
    { /* vt */
	case VT_BSTR:
		strRet = varValue.bstrVal;
		break;
	case VT_I4:
		//s.Format(_T("%d"), va->lVal);
		//return s;
		strRet.Format("%d", varValue.lVal);
		break;
	case VT_R8:
		strRet.Format(_T("%f"), varValue.dblVal);
		break;
		// add more and more
	default:
		strRet.Empty();
		break;
	} /* vt */

	return strRet;
}

BOOL CXMLControl::GetHtmlSource(CString strURL, CString strDataFile, int nCodeType)
{
	IXMLHTTPRequestPtr xmlrequest;
	xmlrequest.CreateInstance("MSXML2.XMLHTTP");
	_variant_t varp(false);
	xmlrequest->open(_bstr_t("GET"), _bstr_t(strURL.AllocSysString()),varp);
	xmlrequest->send();
	BSTR bstrbody;
	xmlrequest->get_responseText(&bstrbody);
	DWORD	dwCodeLen = WideCharToMultiByte(nCodeType, NULL, bstrbody, -1, NULL, NULL, 0, FALSE);
	TCHAR	*ptStr = new TCHAR[dwCodeLen];
	WideCharToMultiByte(nCodeType, NULL, bstrbody, -1, ptStr, dwCodeLen, NULL, FALSE);
	CStdioFile file;
	//if (-1 == GetFileAttributes(FileName))
	if(!file.Open(strDataFile, CFile::typeBinary|CFile::modeCreate|CFile::modeWrite))
	{
		return FALSE;
	}
	//else
	//	file.Open(FileName, CFile::typeText|CFile::modeWrite);

	file.Write(ptStr, sizeof(TCHAR)*dwCodeLen-1);

	//delete [] pchar;
	file.Close();
	delete [] ptStr;
	ptStr = NULL;
	return TRUE;
}

BOOL CXMLControl::GetPNBySNFromPaperless(LPCSTR lpstrURL, CString &lpstrPN)
{
	return TRUE;
}

BOOL CXMLControl::GetNodeValueByTag(CString strTag, CString &strValue)
{
	return TRUE;
}
