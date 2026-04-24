// cFusionClass.cpp : implementation file
//

#include "stdafx.h"

#include "cFusionClass.h"

// cFusionClass

#if 0
#pragma managed
#using "..\\bin\\USL.TAS.dll"
using namespace USLTASLibrary;
using namespace System;
using namespace std;
#endif

IMPLEMENT_DYNAMIC(cFusionClass, CWnd)



HINSTANCE m_hLoaddll = LoadLibrary(_T("USL.TAS.C.dll"));

cFusionClass::cFusionClass()
{
	if (m_hLoaddll)
	{
		m_pFucGetPorductInfo = (GetModuleProductInfo)GetProcAddress(m_hLoaddll, "GetModuleProductInfo");//GetProductInfo
		m_pFucGetProductKeyInfo = (GetProductKeyInfo)GetProcAddress(m_hLoaddll, "GetProductKeyInfo");
		m_pFucGetMIMSLoginInfo = (GetMIMSLoginInfo)GetProcAddress(m_hLoaddll, "GetMIMSLoginInfo");
		m_pFucGetTPConfigInfo = (GetTPConfigInfo)GetProcAddress(m_hLoaddll, "GetTPConfigInfo");
		m_pFucGetFWConfigInfo = (GetFWConfigInfo)GetProcAddress(m_hLoaddll, "GetFWConfigInfo");
		m_pFucGetProdTestTemplate = (GetProdTestTemplate)GetProcAddress(m_hLoaddll, "GetProdTestTemplate");
		m_pFucGetTestProcessCode = (GetTestProcessCode)GetProcAddress(m_hLoaddll, "GetTestProcessCode");
		m_pFucGetMESProcessCode = (GetMESProcessCode)GetProcAddress(m_hLoaddll, "GetMESProcessCode");
		m_pFucTriggerProcessMoveIn = (TriggerProcessMoveIn)GetProcAddress(m_hLoaddll, "TriggerProcessMoveIn");
		m_pFucTriggerProcessMoveOut = (TriggerProcessMoveOut)GetProcAddress(m_hLoaddll, "TriggerProcessMoveOut");
		m_pFucUploadTestData = (UploadTestData)GetProcAddress(m_hLoaddll, "UploadTestData");//UploadTestData
		m_pFucGetWOHeaderList = (GetWOHeaderList)GetProcAddress(m_hLoaddll, "GetWOHeaderList");
		m_pFucTriggerUserSkillVerify = (TriggerUserSkillVerify)GetProcAddress(m_hLoaddll, "TriggerUserSkillVerify");
		m_pFucGetStationName = (GetStationName)GetProcAddress(m_hLoaddll, "GetStationName");
		m_pFucTriggerConversationPN = (TriggerConversationPN)GetProcAddress(m_hLoaddll, "TriggerConversationPN");
		m_pFucTriggerWorkStationVerify = (TriggerWorkStationVerify)GetProcAddress(m_hLoaddll, "TriggerWorkStationVerify");
		m_pFucGetOMSTestTemplate = (GetOMSTestTemplate)GetProcAddress(m_hLoaddll, "GetOMSTestTemplate");
		m_pFucTriggerTestResultUpload = (TriggerTestResultUpload)GetProcAddress(m_hLoaddll, "TriggerTestResultUpload");

		m_pFucGetSoftwareID = (GetSoftwareID)GetProcAddress(m_hLoaddll, "GetSoftwareID");
		m_pFucSetSystemToSpecialMode = (SetSystemToSpecialMode)GetProcAddress(m_hLoaddll, "SetSystemToSpecialMode");
		m_pFucGetSystemCurrentMode = (GetSystemCurrentMode)GetProcAddress(m_hLoaddll, "GetSystemCurrentMode");
		m_pFucSetEmployeeAccount = (SetEmployeeAccount)GetProcAddress(m_hLoaddll, "SetEmployeeAccount");
		m_pFucGetFullProductKeyInfo  = (GetFullProductKeyInfo)GetProcAddress(m_hLoaddll, "GetFullProductKeyInfo");
		m_pFucGetOplinkUserInfo   = (GetOplinkUserInfo)GetProcAddress(m_hLoaddll, "GetOplinkUserInfo");
		m_pFucCreateSNDataRootDir   = (CreateSNDataRootDir)GetProcAddress(m_hLoaddll, "CreateSNDataRootDir");
		m_pFucGetFWStandardPath    = (GetFWStandardPath)GetProcAddress(m_hLoaddll, "GetFWStandardPath");

	}
	else
	{
		m_pFucGetPorductInfo = NULL;
		m_pFucGetProductKeyInfo = NULL;
		m_pFucGetMIMSLoginInfo = NULL;
		m_pFucGetTPConfigInfo = NULL;
		m_pFucGetFWConfigInfo = NULL;
		m_pFucGetProdTestTemplate = NULL;
		m_pFucGetTestProcessCode = NULL;
		m_pFucGetMESProcessCode = NULL;
		m_pFucTriggerProcessMoveIn = NULL;
		m_pFucTriggerProcessMoveOut = NULL;
		m_pFucUploadTestData = NULL;
		m_pFucGetWOHeaderList = NULL;
		m_pFucTriggerUserSkillVerify = NULL;
		m_pFucGetStationName = NULL;
		m_pFucTriggerConversationPN = NULL;
		m_pFucTriggerWorkStationVerify = NULL;
		m_pFucGetOMSTestTemplate = NULL;
		m_pFucTriggerTestResultUpload = NULL;

		m_pFucGetSoftwareID = NULL;
		m_pFucSetSystemToSpecialMode = NULL;
		m_pFucGetSystemCurrentMode = NULL;
		m_pFucSetEmployeeAccount = NULL;
		m_pFucGetFullProductKeyInfo = NULL;
		m_pFucGetOplinkUserInfo = NULL;
		m_pFucCreateSNDataRootDir = NULL;
		m_pFucGetFWStandardPath = NULL;

	
	}

}

cFusionClass::~cFusionClass()
{
}

#if 1
BOOL cFusionClass::bSaveXmlFile(CString strData, CString strXMLURL)
{
	CStdioFile file;
	DWORD      dwCodeLen = strData.GetLength();
	char	  *ptStr = new char[dwCodeLen];
	ZeroMemory(ptStr, sizeof(char)*dwCodeLen);

	strncpy(ptStr, (char*)strData.GetBuffer(0), dwCodeLen);
	if (!file.Open(strXMLURL, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))
	{
		return FALSE;
	} 

	file.Write(ptStr, sizeof(char)*dwCodeLen);

	file.Close();
	delete[] ptStr;
	ptStr = NULL;
	return TRUE; 
}

#if 0
BOOL cFusionClass::bDownloadProdInfo(CString strSN, CString strCheckItem, CString strXMLURL)
{
	CString strErrorMsg;

	USLTASLibrary::USLTASLibraryInterface ^tas = gcnew USLTASLibrary::USLTASLibraryInterface();
	String ^stSN = gcnew String(strSN);
	System::String ^strReturnMsg = gcnew System::String("1234");
	CString str = tas->GetProductInfo(stSN, strReturnMsg);
	//AfxMessageBox(str);
	if (str.IsEmpty() || str == "")
	{
		strErrorMsg = strReturnMsg;
		AfxMessageBox(strErrorMsg);
	}

	if (!bSaveXmlFile(str, strXMLURL))
	{
		return FALSE;
	}
	return TRUE;
}
#endif

BOOL cFusionClass::bDownLoadItemInfo(CString strSN, CString strProcess, CString strUser, CString strFreeCheck, BOOL bShowData, CString strXMLURL)
{

	CString strErrorMsg;
	int     nLength;

	char* pcharOutString = NULL;
	char* pcharOutTempName = NULL;
	char pcharTemplateNmeGet[1024];
	ZeroMemory(pcharTemplateNmeGet, sizeof(pcharTemplateNmeGet));

	bool bRunFlag = m_pFucGetProdTestTemplate(strSN, strProcess, strXMLURL, bShowData, pcharTemplateNmeGet, 1024, &pcharOutString);
	if (!bRunFlag)
	{
		strErrorMsg = pcharOutString;
		AfxMessageBox(strErrorMsg);
	}

	return TRUE;
}

BOOL cFusionClass::bGetProductProcess(CString strSN, CString *pstrWO, CString *pstrPN, CString *pstrSPEC, CString *pstrSTATUS, CString *strProductProcess, CString *pstrErrorMsg)
{
	if (m_pFucGetProductKeyInfo)
	{
		char pchrPNBuffer[256];
		ZeroMemory(pchrPNBuffer, 256 * sizeof(char));
		char pchrSpecBuffer[256];
		ZeroMemory(pchrSpecBuffer, 256 * sizeof(char));
		char pchrWOBuffer[256];
		ZeroMemory(pchrWOBuffer, 256 * sizeof(char));
		char pchrProcessBuffer[256];
		ZeroMemory(pchrProcessBuffer, 256 * sizeof(char));
		char pchrStatusBuffer[256];
		ZeroMemory(pchrStatusBuffer, 256 * sizeof(char));

		char* pcharOutString = NULL;
		bool bRunFlag = m_pFucGetProductKeyInfo(strSN, pchrPNBuffer, 256,
			pchrSpecBuffer, 256, pchrWOBuffer, 256, pchrProcessBuffer, 256,
			pchrStatusBuffer, 256, &pcharOutString);
		CString strMsg;
		if (bRunFlag)
		{
			*pstrWO = pchrWOBuffer;
			*strProductProcess = pchrProcessBuffer;
			*pstrSPEC = pchrSpecBuffer;
			*pstrPN = pchrPNBuffer;
			*pstrSTATUS = pchrStatusBuffer;
		}
		else
		{
			*pstrErrorMsg = pcharOutString;
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL cFusionClass::bLoadXMLTemplet(CString strFilePath, CString *pstrErrorMsg)
{
	int nErrCode = 99;
	char ChErrMsg[1024] = { "0" };
	CString strErrorMsg;

	nErrCode = atmsTest.LoadTemplate(strFilePath.GetBuffer());
	if (nErrCode != 0)
	{
		atmsTest.GetErrMsg(nErrCode, ChErrMsg, 1024);
		strErrorMsg.Format("%s", ChErrMsg);
		*pstrErrorMsg = strErrorMsg;
		return FALSE;
	}

	return TRUE;
}

BOOL cFusionClass::bSaveXMLTemplet(CString strFilePath, CString *pstrErrorMsg)
{
	int nErrCode = 99;
	char ChErrMsg[1024] = { "0" };
	CString strErrorMsg;

	nErrCode = atmsTest.SaveTestData(strFilePath.GetBuffer());
	if (nErrCode != 0)
	{
		atmsTest.GetErrMsg(nErrCode, ChErrMsg, 1024);
		strErrorMsg.Format("%s", ChErrMsg);
		*pstrErrorMsg = strErrorMsg;
		return FALSE;
	}
	
	return TRUE;
}


BOOL cFusionClass::bUpdateXMLTemplet(sttagXMLItem stItem, CString *pstrErrorMsg)
{
	int nErrCode = 99;
	char ChErrMsg[1024] = { "0" };
	CString strErrorMsg;

	nErrCode = atmsTest.SetParam(stItem.strTenvKey.GetBuffer(), stItem.strObjectKey.GetBuffer(), 
								 stItem.strPortKey.GetBuffer(), stItem.strcondKey.GetBuffer(),
								 stItem.strParamName.GetBuffer(), stItem.strParamValue.GetBuffer(), 
								 stItem.strPassfail.GetBuffer(), stItem.strFile.GetBuffer(), stItem.strDate.GetBuffer());
	if (nErrCode != 0)
	{
		atmsTest.GetErrMsg(nErrCode, ChErrMsg, 1024);
		strErrorMsg.Format("%s", ChErrMsg);
		*pstrErrorMsg = strErrorMsg;
		return FALSE;
	}

	return TRUE;
}

BOOL cFusionClass::bGetXMLParamValue(sttagXMLItem stItem, CString *strData, CString *pstrErrorMsg)
{
	int nErrCode = 99;
	char ChErrMsg[1024] = { "0" };
	char ChstrData[1024] = { "0" };
	CString strErrorMsg,strtempdata;

	nErrCode = atmsTest.GetParamValue(stItem.strTenvKey.GetBuffer(), stItem.strObjectKey.GetBuffer(),
		stItem.strPortKey.GetBuffer(), stItem.strcondKey.GetBuffer(),
		stItem.strParamName.GetBuffer(), ChstrData);
	if (nErrCode != 0)
	{
		atmsTest.GetErrMsg(nErrCode, ChErrMsg, 1024);
		strErrorMsg.Format("%s", ChErrMsg);
		*pstrErrorMsg = strErrorMsg;
		return FALSE;
	}
	strtempdata.Format("%s", ChstrData);
	*strData = strtempdata;

	return TRUE;
}


BOOL cFusionClass::bUpdateXMLTemplet(CString strFilePath, sttagXMLItem stItem, CString *pstrErrorMsg)
{
	int     ErrorCode = 99;
	char    chErrMsg[1024] = { 0 };
	CString strTime, strValue, strErrorMsg, strMsg, strResult, strParaValue;
	CString strTenvKey, strObjectKey, strPortKey, strCondkey, strParaName;
	double  dblMinValue = 99, dblMaxValue = 99, dblParaValue = 0;
	BOOL    bWriteState = FALSE;

	CTime m_time = CTime::GetCurrentTime();
	strTime = m_time.Format("%Y-%m-%d %H:%M:%S");

	ErrorCode = atmsTest.LoadTemplate(strFilePath.GetBuffer());
	if (ErrorCode != 0)
	{
		atmsTest.GetErrMsg(ErrorCode, chErrMsg, 1024);
		strErrorMsg.Format("%s", chErrMsg);
		*pstrErrorMsg = strErrorMsg;
		return FALSE;
	}

	//获取所有参数
	int nParamCount = atmsTest.GetAllParamsCount();
	if (nParamCount == 0)
	{
		return FALSE;
	}
	pAllParams = new CParamAttr[nParamCount];//比如有10个测试项,则nParamCount=10

	ErrorCode = 99;
	ErrorCode = atmsTest.GetParams(pAllParams, nParamCount);
	if (ErrorCode != 0)
	{
		atmsTest.GetErrMsg(ErrorCode, chErrMsg, 1024);
		strErrorMsg.Format("%s", chErrMsg);
		*pstrErrorMsg = strErrorMsg;
		return FALSE;
	}

	ErrorCode = 99;
	for (int i = 0; i < nParamCount; i++)
	{
		strTenvKey = pAllParams[i].m_TenvKey;// .c_str();
		strObjectKey = pAllParams[i].m_ObjectKey;// .c_str();
		strPortKey = pAllParams[i].m_PortKey;// .c_str();
		strCondkey = pAllParams[i].m_CondKey;// .c_str();
		strParaName = pAllParams[i].m_Name;// .c_str();
		dblMinValue = atof(pAllParams[i].m_Min);// .c_str());
		dblMaxValue = atof(pAllParams[i].m_Max);// .c_str());
		dblParaValue = atof(stItem.strParamValue);

		if (strTenvKey == stItem.strTenvKey)
		{
			if (strObjectKey == stItem.strObjectKey)
			{
				if (strPortKey == stItem.strPortKey)
				{
					if (strCondkey == stItem.strcondKey)
					{
						if (strParaName == stItem.strParamName)
						{
							if (dblParaValue < dblMinValue || dblParaValue > dblMaxValue)
							{
								strResult = "Fail";
							}
							else
							{
								strResult = "Pass";
							}

							ErrorCode = atmsTest.SetParam(pAllParams[i].m_TenvKey, pAllParams[i].m_ObjectKey, pAllParams[i].m_PortKey, pAllParams[i].m_CondKey,
								pAllParams[i].m_Name, strParaValue.GetBuffer(), strResult.GetBuffer(), NULL, strTime.GetBuffer());
							if (ErrorCode != 0)
							{
								atmsTest.GetErrMsg(ErrorCode, chErrMsg, 1024);
								strErrorMsg.Format("%s", chErrMsg);
								*pstrErrorMsg = strErrorMsg;
								return FALSE;
							}
							bWriteState = TRUE;
						}
					}
				}
			}
		}
	}
	
	if (bWriteState == FALSE)
	{
		strErrorMsg.Format("写XML文件失败，未找到对应的测试项名称，请检查XML模板是否正确......", chErrMsg);
		*pstrErrorMsg = strErrorMsg;
		return FALSE;
	}
	return TRUE;
}

BOOL cFusionClass::bXML()
{

	//加载模板EVOABC1100ZAB01-Z4085-23-Test-EVOA.xml
	TCHAR tszCurrentFolder[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, tszCurrentFolder);
	CString strFile, strValue;
	strFile.Format("%s\\1831810234_Test_Z4746-08_1_V1.0.xml", tszCurrentFolder);
	string strPath = strFile.GetBuffer();
	string stValue;

	int errCode = atmsTest.LoadTemplate(strFile.GetBuffer());
	if (errCode != 0)
	{
		char errMsg[1024] = { 0 };
		//atmsTest.GetErrMsg(errCode, errMsg, 1024);
		return FALSE;
	}
	//GetTempValue();

	//获取所有参数
	int nParamCount = atmsTest.GetAllParamsCount();
	if (nParamCount == 0)
	{
		return FALSE;
	}
	pAllParams = new CParamAttr[nParamCount];//比如有10个测试项,则nParamCount=10

	errCode = atmsTest.GetParams(pAllParams, nParamCount);
	if (errCode != 0)
	{
		char errMsg[1024] = { 0 };
		//atmsTest.GetErrMsg(errCode, errMsg, 1024);
		return FALSE;
	}


	int j = 0, k = 0, m = 0, n = 0;
	//第一步温度排序，先常温-高温-低温-全温
	CString strItem;
	CString strTemperature[3] = { _T("RT"), _T("LT"), _T("HT") };

	//	strFile.Format("%s\\Temple\\%s", tszCurrentFolder, "1831810234_Test_Z4746-08_1_V1.01.xml");


	for (int i = 0; i < nParamCount; i++)
	{
		CParamAttr param = pAllParams[i];
		CString strTempData;
		//1.
		CString strTenvKey = pAllParams[i].m_TenvKey;// .c_str();
		//CString strObjectKey = pAllParams[i].m_ObjectKey.c_str();
		//CString strPortKey = pAllParams[i].m_PortKey.c_str();
		//CString strCondkey = pAllParams[i].m_CondKey.c_str();
		CString strName;// = pAllParams[i].m_Name.c_str();
						//atmsTest.SetParam();
		for (size_t j = 0; j < 3; j++)
		{
			if (strTenvKey == strTemperature[j])
			{
				if (strName == "FSR")
				{
					strValue = "0";
				}
				else if (strName == "BW3dB")
				{
					strValue = "0";
				}
				else if (strName == "BW15dB")
				{
					strValue = "0";
				}
				else if (strName == "PDL")
				{
					strValue = "0";
				}
				else if (strName == "NonInterval")
				{
					strValue = "0";
				}
				else if (strName == "PDL")
				{
					strValue = "0";
				}
				else if (strName == "NonInterval")
				{
					strValue = "0";
				}
				else if (strName == "PZTLength")
				{
					strValue = "0";
				}
				else if (strName == "Bulge")
				{
					strValue = "0";
				}
				else if (strName == "PeakJump")
				{
					strValue = "0";
				}
				else if (strName == "Sidelobe")
				{
					strValue = "0";
				}
				else if (strName == "RES")
				{
					strValue = "0";
				}
				else if (strName == "TECTemperature")
				{
					strValue = "0";
				}
				else if (strName == "TECStability")
				{
					strValue = "0";
				}
				else
				{
					AfxMessageBox("未知参数，请检查！");
				}

				atmsTest.SetParam(pAllParams[i].m_TenvKey, pAllParams[i].m_ObjectKey, pAllParams[i].m_PortKey, pAllParams[i].m_CondKey, pAllParams[i].m_Name, strValue.GetBuffer(), "P", "sdfdf", "1");
			}

		}
	}
	atmsTest.SaveTestData(strFile.GetBuffer());
	return TRUE;
}

void cFusionClass::ConvertGBKToUtf8(CString &strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL, 0);
	wchar_t * wszUtf8 = new wchar_t[len];
	memset(wszUtf8, 0, len);
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, wszUtf8, len);



	len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL, NULL);



	strGBK = szUtf8;
	delete[] szUtf8;
	delete[] wszUtf8;
}

BOOL cFusionClass::bUploadXMLData(CString strFilePath, CString *pstrErrorMsg)
{
	char* pcharOutString = NULL;
	bool bRunFlag = m_pFucUploadTestData(strFilePath, &pcharOutString);
	if (!bRunFlag)
	{		
		*pstrErrorMsg = pcharOutString;
		return FALSE;
	}
	else
	{
		//MessageBox("Update data Succesfully");
		return TRUE;
	}
}

BOOL cFusionClass::bUpdateTestResult(CString strSN, CString *pstrErrorMsg)
{
	char* pcharOutString = NULL;
	int nReturn = m_pFucTriggerTestResultUpload(strSN, &pcharOutString);
	if (nReturn < 0)
	{
		*pstrErrorMsg = pcharOutString;
		return FALSE;
	}
	else
	{
		//MessageBox("Update TestResult Succesfully");
		return TRUE;
	}
}

#endif
BOOL cFusionClass::bGetInfo()
{
	//UpdateData();
	if (m_pFucGetProductKeyInfo)
	{
		char pchrPNBuffer[256];
		ZeroMemory(pchrPNBuffer, 256 * sizeof(char));
		char pchrSpecBuffer[256];
		ZeroMemory(pchrSpecBuffer, 256 * sizeof(char));
		char pchrWOBuffer[256];
		ZeroMemory(pchrWOBuffer, 256 * sizeof(char));
		char pchrProcessBuffer[256];
		ZeroMemory(pchrProcessBuffer, 256 * sizeof(char));
		char pchrStatusBuffer[256];
		ZeroMemory(pchrStatusBuffer, 256 * sizeof(char));

		char* pcharOutString = NULL;
		bool bRunFlag = m_pFucGetProductKeyInfo("E1970864", pchrPNBuffer, 256,
			pchrSpecBuffer, 256, pchrWOBuffer, 256, pchrProcessBuffer, 256,
			pchrStatusBuffer, 256, &pcharOutString);
		CString strMsg;
		if (bRunFlag)
		{
			strMsg.Format("PN:%s\r\nSpec:%s\r\nWO:%s\r\nProcess:%s\r\nStuts:%s\r\n",
				pchrPNBuffer, pchrSpecBuffer, pchrWOBuffer, pchrProcessBuffer, pchrStatusBuffer);
		}
		else
		{
			strMsg.Format("%s", pcharOutString);
		}

		//m_strShowMsg = strMsg;
		UpdateData(false);
	}
	else
	{
		//MessageBox("����δ���أ�");
	}

	return TRUE;
}
BEGIN_MESSAGE_MAP(cFusionClass, CWnd)
END_MESSAGE_MAP()



// cFusionClass message handlers


