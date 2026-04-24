#pragma once

#include "..\\include\ATMSRecord.h"
// cFusionClass
typedef struct tagXMLData
{
	double dbl;

}sttagXMLData, *psttagXMLData;


typedef struct tagXMLItem
{
	CString strTenvKey;
	CString strObjectKey;
	CString strPortKey;
	CString strcondKey;
	CString strParamName;
	CString strParamValue;
	CString strPassfail; 
	CString strFile;
	CString strDate;
	double  dblValue;
}sttagXMLItem, *psttagXMLItem;


class cFusionClass : public CWnd
{
	DECLARE_DYNAMIC(cFusionClass)

public:
	cFusionClass();
	virtual ~cFusionClass();

	typedef bool(*GetModuleProductInfo)(const char* pchrSerialNo, char** ppchrOutMsg);
	typedef bool(*GetProductKeyInfo)(const char* pchrSerialNo, char* pchrOutPN, int iPNBufLen,
		char* pchrOutSpec, int iSpecBufLen, char* pchrOutWO, int iWOBufLen, char* pchrOutCurProcess, int iProcessBufLen,
		char* pchrOutCurStatus, int iStatusBufLen, char** ppchrOutErrorMsg);
	typedef bool(*GetMIMSLoginInfo)(const char* pchrUserName, const char* pchrPassWord, const char* pchrLoginMode, char** ppchrOutMsg);
	typedef bool(*GetTPConfigInfo)(const char* pchrPN, const char* pchrProcess, const char* pchrLoginMode,
		char* pchrOutConfig, int iConfigBufLen, char** ppchrOutErrorMsg);
	typedef bool(*GetFWConfigInfo)(const char* pchrPN, char* pchrOutConfig, int iConfigBufLen, char** ppchrOutErrorMsg);
	typedef bool(*GetProdTestTemplate)(const char* pchrSN, const char* pchrProcess,
		const char* pchrSaveFileName, bool bShowData, char* pchrOutTempName, int iOutBufLen, char** ppchrOutErrorMsg);
	typedef bool(*GetTestProcessCode)(const char* pchrPN, const char* pchrMESProcess,
		char* pchrOutTestCode, int iCodeBufLen, char** ppchrErrorMsg);
	typedef bool(*GetMESProcessCode)(const char* pchrPN, const char* pchrTestProcess,
		char* pchrMESCode, int icodeBufLen, char** ppchrErrorMsg);
	typedef bool(*TriggerProcessMoveIn)(const char* pchrSN, const char* pchrMESProcess, const char* pchrStationID, char** ppchrOutErrorMsg);
	typedef bool(*TriggerProcessMoveOut)(const char* pchrSN, const char* pchrStationID, char** ppchrOutErrorMsg);
	typedef bool(*UploadTestData)(const char* pchrDataFileName, char** ppchrOutErrorMsg);
	typedef bool(*GetWOHeaderList)(const char* pchrSN, const char* pchrDataCollectionDef,
		const char* pchrPostion, char* pchrOutValueList, int iOutBufLen, char** ppchrErrorMsg);
	typedef bool(*TriggerUserSkillVerify)(const char* pchrEmployee, const char* pchrProcess, const char* pchrOperation, char** ppchrOutErrorMsg);
	typedef bool(*GetStationName)(const char* pchrPCName, char* pchrOutStationID,
		int iIDBufLen, char* pchrOutStationName, int iNameBufLen, char** ppchrOutErrorMsg);
	typedef bool(*TriggerConversationPN)(const char* pchrOrigin, int nConvType,
		char* pchrOutValue, int iOutBufLen, char** ppchrOutErrorMsg);
	typedef bool(*TriggerWorkStationVerify)(const char* pchrStationID, const char* pchrUserID, const char* pchrType, char** ppchrOutErrorMsg);
	typedef bool(*GetOMSTestTemplate)(const char* pchrPN, const char* pchrTemplateType, const char* pchrSaveFileName, char** ppchrOutErrorMsg);
	typedef int(*TriggerTestResultUpload)(const char* pchrSN, char** ppchrOutErrorMsg);
	typedef bool(*GetSoftwareID)(const char* pchrPN, const char* pchrProcess, const char* pchrSpec, const char* pchrWO, char** ppchrOutErrorMsg);
	typedef bool(*SetSystemToSpecialMode)(const char* pchrUserName, const char* pchrPassWord, const char* pchrModeType, char** ppchrOutErrorMsg);
	typedef bool(*GetSystemCurrentMode)(char* pchrOutCurrentMode, int iCurrentModeBufLen, char** ppchrOutMsg);
	typedef bool (*SetEmployeeAccount)(const char* pchrEmployeeOplinkID, char** ppchrOutErrorMsg);
	typedef bool(*GetFullProductKeyInfo)(const char* pchrSerialNo, char* pchrOutContainer, int iContainerBufLen,
		char* pchrOutMTSPN, int iMTSPNBufLen, char* pchrOutMTOPN, int iMTOPNBufLen, char* pchrOutOPLKPN, int iOPLKPNBufLen,
		char* pchrOutSpec, int iSpecBufLen, char* pchrOutWO, int iWOBufLen, char* pchrOutCurProcess, int iProcessBufLen,
		int* pintOutStuts, char** ppchrOutErrorMsg);
	typedef bool(*GetOplinkUserInfo)(const char* pchrOplinkID, char* pchrOutUserName, int iUserNameBufLen, char* pchrOutDptName, int iDptNameBufLen,
		char* pchrPosName, int iPosNameBufLen, char* pchrOPCID, int iOPCIDBufLen, char** ppchrOutErrorMsg);
	typedef bool(*CreateSNDataRootDir)(const char* pchrDUTSN, const char* pchrProdFamilyRootDir, char* pchrOutSNRootDir, int iSNRootDirBufLen,
		char** ppchrOutErrorMsg);
	typedef bool(*GetFWStandardPath)(const char* pchrDUTPN, char* pchrOutFWBOMPN, int iFWBOMPNBufLen, char* pchrOutFWStandardPath, int iPathBufLen,
		char** ppchrOutErrorMsg);




	GetModuleProductInfo m_pFucGetPorductInfo;
	GetProductKeyInfo   m_pFucGetProductKeyInfo;
	GetMIMSLoginInfo    m_pFucGetMIMSLoginInfo;
	GetTPConfigInfo     m_pFucGetTPConfigInfo;
	GetFWConfigInfo     m_pFucGetFWConfigInfo;
	GetProdTestTemplate m_pFucGetProdTestTemplate;
	GetTestProcessCode  m_pFucGetTestProcessCode;
	GetMESProcessCode   m_pFucGetMESProcessCode;
	TriggerProcessMoveIn    m_pFucTriggerProcessMoveIn;
	TriggerProcessMoveOut   m_pFucTriggerProcessMoveOut;
	UploadTestData          m_pFucUploadTestData;
	GetWOHeaderList         m_pFucGetWOHeaderList;
	TriggerUserSkillVerify  m_pFucTriggerUserSkillVerify;
	GetStationName          m_pFucGetStationName;
	TriggerConversationPN       m_pFucTriggerConversationPN;
	TriggerWorkStationVerify    m_pFucTriggerWorkStationVerify;
	GetOMSTestTemplate          m_pFucGetOMSTestTemplate;
	TriggerTestResultUpload     m_pFucTriggerTestResultUpload;

	GetSoftwareID		   m_pFucGetSoftwareID;
	SetSystemToSpecialMode m_pFucSetSystemToSpecialMode;
	GetSystemCurrentMode   m_pFucGetSystemCurrentMode;
	SetEmployeeAccount      m_pFucSetEmployeeAccount;
	GetFullProductKeyInfo   m_pFucGetFullProductKeyInfo;
	GetOplinkUserInfo       m_pFucGetOplinkUserInfo;
	CreateSNDataRootDir       m_pFucCreateSNDataRootDir;
	GetFWStandardPath         m_pFucGetFWStandardPath;

	//BOOL bDownloadProdInfo(CString strSN, CString strCheckItem, CString strXMLURL);
	BOOL bDownLoadItemInfo(CString strSN, CString strProcess, CString strUser, CString strFreeCheck, BOOL bShowData, CString strXMLURL);
	BOOL bGetProductProcess(CString strSN, CString *pstrWO,CString *pstrPN, CString *pstrSPEC, CString *pstrSTATUS, CString *strProductProcess, CString *pstrErrorMsg);
	BOOL bSaveXmlFile(CString strData, CString strXMLURL);

	BOOL bLoadXMLTemplet(CString strFilePath, CString *pstrErrorMsg);
	BOOL bUpdateXMLTemplet(sttagXMLItem stItem, CString *pstrErrorMsg);
	BOOL bGetXMLParamValue(sttagXMLItem stItem, CString *strData,CString *pstrErrorMsg);
	BOOL bSaveXMLTemplet(CString strFilePath, CString *pstrErrorMsg);
	BOOL bUploadXMLData(CString strFilePath, CString *pstrErrorMsg);

	BOOL bUpdateTestResult(CString strSN, CString *pstrErrorMsg);
	BOOL bUpdateXMLTemplet(CString strFilePath, sttagXMLItem stItem, CString *pstrErrorMsg);
	BOOL bXML();


	BOOL bGetInfo();

	void ConvertGBKToUtf8(CString &strGBK);

	CATMSRecord atmsTest;
	CParamAttr *pAllParams;
	CCommAttr *pAllCFG;

protected:
	DECLARE_MESSAGE_MAP()
};


