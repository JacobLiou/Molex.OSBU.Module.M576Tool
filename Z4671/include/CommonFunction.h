//CommonFunction.h
//Created by xiaoyong wang
//Date: 2006-3-16 

#if !defined(AFX_CCOMMONFUNCTION_H__5F9E481C_89F4_463A_AF1D_6DCEF015B195__INCLUDED_)
#define AFX_CCOMMONFUNCTION_H__5F9E481C_89F4_463A_AF1D_6DCEF015B195__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "math.h"
// Include the head file of data type definition
#include "ConfigDataType.h"

#define MAX_BYTE						256
#define MAX_STEP						1024
#define MAX_LENGTH						256

#define DATA_SCALE						100

// Define firmware upgrade status word
#define DOWNLOAD_SUCCESS					0
#define FIRMWARE_BUSY						1
#define COMMUNICATION_FAILURE				2
#define NOT_ENOUGH_FIRMWARE_DATA			-1
#define FILE_SIZE_DOESNOT_MATCH				-2
#define CRC32_DOESNOT_MATCH					-3
#define FLASH_PROGRAMMING_ERROR				-4
#define EEPROM_PROGRAMMING_ERROR			-5
#define USER_ABORT_UPGRADE					10
#define CANNOT_OPEN_BIN_FILE				11

class CMyPoint;

class CCommonFunction
{
public:
	CCommonFunction();

	~CCommonFunction();

	BYTE HighByte(WORD wVar);

	BYTE LowByte(WORD wVar);

	WORD HighWord(DWORD dwVar);

	WORD LowWord(DWORD dwVar);

	WORD MergeToWord(BYTE btHighByte, BYTE btLowByte);

	WORD MergeToWord(CString strHighByte, CString strLowByte);

	DWORD MergeToDoubleWord(BYTE btFirstByte, BYTE btSecondByte, BYTE btThirdByte, BYTE btFourthByte);

	DWORD MergeToDoubleWord(CString strFirst, CString strSecond, CString strThird, CString strFourth);

	int Contain(char * pchStrBuffer, const double dblPowerValue);

	CString GetCurrentTime();

	VOID SetMaxMessageLine(DWORD dwMaxMessageLine);

	BOOL ConvertToHexString(CString strInputString,
							int iDataType,
							int iDataSize,
							CString &strInnerString,
							int iAlignType,
							int iFilledCharInValue,
							BOOL bNeedSeparate = FALSE,
							BOOL bGetASCII = TRUE);
	BOOL HexStrToEyeableStr(BYTE * pchByteArray,
							int iDataType,
							int iDataSize,
							CString &strObviousString);
	WORD GetDataTypeWord(CString strDataType);
	CString GetDataTypeString(WORD wDataType);
	WORD GetAccessTypeWord(CString strAccessType);
	CString GetAccessTypeString(WORD wAccessType);
	WORD GetCmdTypeWord(CString strCmdType);
	CString GetCmdTypeString(WORD wCmdType);
	BYTE GetUpgradeFileTypeByte(CString strFileType);
	WORD GetParamAlterabilityWord(CString strParamAlterability);
	CString GetParamAlterabilityString(WORD wParamAlterability);

	CString ConvertToStandardLength(CString strInputStr, WORD wDataType);
	CString ParseStatus(int iStatus);
	VOID YieldToPeers();
	void iapcir(double x[], double y[], int n, double a[], int m, double dt[]);
//	void CalculateCurveParameter(CDoubleArray * X, CDoubleArray * Y, CDoubleArray * a, int M, int N);
//	BOOL CalculateCurveParameter(CDoubleArray *X,CDoubleArray *Y,long M,long N,CDoubleArray *A);
	void pcir(double X[], double Y[], double A[], int N, int M, double &DT1, double &DT2, double &DT3);
	VOID ShowInfo(CListBox& pListCtrl, LPCTSTR tszlogMsg, BOOL bAppend);
	VOID ShowInfo(CListBox *pListCtrl, LPCTSTR tszlogMsg, BOOL bAppend);
	VOID LogInfo(CString strFileName, LPCTSTR tszlogMsg, BOOL bAppend);
	CString ProcessError(int nRetCode);
	CString ParseCommunicationCode(int nRetCode);
	CString ParseCommunicationCodeChs(int nRetCode);
	VOID FindMaxMinValue(double * pdblSourceArray,
						int iArrayLength,
						double * pdblMaxValue,
						double * pdblMinValue);
	VOID HexStrToHexArray(CString strInnerString,
							int iDataType,
							int iDataLength,
							BYTE *pchSendMsg,
							BYTE btFilledCharInValue);
	VOID HexArrayToHexStr(BYTE *pchCmdArray,
						int iCmdLength,
						CString & strHexString);
	VOID UpdateChNumber(CComboBox& pComboBox, int iChNumber, int * piChIndex, int iStartChID = 1);
	CString IntArrayToStr(int * pintArray, int iValidLength, char chDelimiter = ' ');
	VOID StrToIntArray(int * pintArray, CString &strInputString, int &iValidLength, DWORD dwArraySize);
	CString GetSelectedPath(HWND hWnd);
	double FrequencyOffsetToWLOffset(double dblFrequencyOffset);
	double WLOffsetToFrequencyOffset(double dblWLOffset);
	BOOL OpenCommandFile(BOOL bNeedOpenDialog,
						CListCtrl& pctrlList,
						CString& strCommandFileFullName,
						int iCmdTypeForFilter,
						int iStartCol,
						BOOL bPopMsg,
						stListCtrlCol _stListCtrlCol);
	BOOL SaveListBoxContent(CString& strCommandFileFullName,
							CString& strFileHead,
							CListBox& pctrlListBox);
	BOOL SaveListCtrlContent(CString& strCommandFileFullName,
							CString& strFileHead,
							CListCtrl& pctrlList,
							int iColumnCount);
	BOOL SaveListCtrlContent(CString& strCommandFileFullName,
							CString& strFileHead,
							CListCtrl& pctrlList,
							int iColumnCount,
							int iDataTypeColID,
							int * piColForGet);
	BOOL SaveListCtrlContent(CString& strCommandFileFullName,
							CString& strFileHead,
							CListCtrl& pctrlList,
							int iColumnCount,
							stListCtrlCol& _stListCtrlCol);
	CString GetOneLineFromListCtrl( CListCtrl& pctrlList,
									int iColumnCount,
									int iRow,
									int iStartCol = 0);
	CString GetOneLineFromListCtrl( CListCtrl& pctrlList,
									int iColumnCount,
									int iRow,
									int iDataType,
									int * piColForGet);
	CString GetOneLineFromListCtrl( CListCtrl& pctrlList,
									int iColumnCount,
									int iRow,
									stListCtrlCol& _stListCtrlCol);
	BOOL ShowCommandInfoToListCtrl( CListCtrl& pctrlList,
									CString strCommandFileFullName,
									int iCmdTypeForFilter,
									int iStartCol,
									BOOL bPopMsg,
									stListCtrlCol _stListCtrlCol);
	BOOL ShowOneLineToListCtrl(char* pchLineBuf,
								CListCtrl& pctrlList,
								int iStartCol,
								int iItem,
								BOOL bNeedInsertItem,
								stListCtrlCol _stListCtrlCol);
	BOOL ShowOneLineToListCtrl( char* pchLineBuf,
								CListCtrl& pctrlList,
								int iCmdTypeForFilter,
								int iStartCol,
								int iItem,
								BOOL bNeedInsertItem,
								stListCtrlCol _stListCtrlCol);
	VOID SetListCtrlColumn(CListCtrl& pListCtrl, LVCOLUMN * pColumn, int iColumnCount, int iStartInsCol = 0);
	void DeleteAllColumns(CListCtrl& pctrlList, int iColumnCount);
	void ClearItemData( CListCtrl& pctrlList,
						int iItem,
						int iColumnCount,
						int iStartCol = 0);
	BOOL DeleteMultiItem(CListCtrl& pctrlList);
	BOOL MoveItemUpOrDown(CListCtrl& pctrlList,
						int iColumnCount,
						BOOL bIsMoveDown);
	VOID ComputeCycleTimesInOneUS(  long lSampleTimes,
									double& dblRefDelay,
									long& lCycleTimesInOneUS,
									double& dblExecuteTimeOfOneCycle);
	CString GetTemperatureIdentifier(int iTestingTemperType, int iLanguageType = LANGUAGE_ENGLISH);
	CString GetTemperatureIdentifier(double dblCurTemper, int iLanguageType = LANGUAGE_ENGLISH);
	WORD ComputeComplementCode(WORD wOriginalValue);
	double ComputeLinearity(double * pdblFirstArray,
							double * pdblSecondArray,
							int iPointNum,
							double * pdblSlope,
							double * pdblIntercept);
	double LinearFit(CMyPoint pointArray[], int iPointNum, double * pdblSlope, double * pdblIntercept);
	WORD ComputeChecksum(BYTE * pchDataArray, int iDataLength, BOOL bNeedReverse);
	WORD ComputeChecksum(WORD * pwDataArray, int iDataLength, BOOL bNeedReverse);
	VOID SetCmdInfoStruct(pstConfigInfo _pstConfigInfo,
						pstProtocolConfig _pstProtocolConfig,
						pstCommandInfo _pstCommandInfo,
						char * pchCmdInfo,
						int iSelectedItemIndex,
						BOOL bHasInnerData,
						BOOL bIsRead);
	VOID SetCmdInfoStruct(pstConfigInfo _pstConfigInfo,
						pstProtocolConfig _pstProtocolConfig,
						pstCommandInfo _pstCommandInfo,
						int iProtType);
	VOID GetFDBKValueParamName(pstCommandInfo _pstCommandInfo,
								int iMaxFDBKDataParamCount,
								int * piFDBKDataCount,
								int * piKeyValuePos);
	VOID GetCMDValueParam(pstCommandInfo _pstCommandInfo,
						int iMaxDataParamCount);
	VOID FeedbackMsgToProtParam(int iParamSN,
								pstProtocolConfig _pstProtocolConfig,
								pstProtParamItem _pstProtParamItem,
								pstCommandInfo _pstCommandInfo);
	VOID ProtParamToCmdStrTemplate(pstProtocolConfig _pstProtocolConfig,
									pstProtParamItem _pstProtParamItem,
									pstCommandInfo _pstCommandInfo);
	VOID FillProtParam(pstProtocolConfig _pstProtocolConfig,
						pstCommandInfo _pstCommandInfo);
	VOID FillCmdStrTemplate(pstProtocolConfig _pstProtocolConfig,
							pstCommandInfo _pstCommandInfo);
	VOID FeedbackMsgToCmdInfo(int iParamSN,
							pstProtocolConfig _pstProtocolConfig,
							pstProtParamItem &_pstProtParamItem,
							pstCommandInfo _pstCommandInfo);
	BOOL CmdInfoToProtParam(int iParamSN,
							pstProtocolConfig _pstProtocolConfig,
							pstProtParamItem & _pstProtParamItem,
							pstCommandInfo _pstCommandInfo);
	BOOL AssignProtParamValue(pstCommandInfo _pstCommandInfo,
							  pstProtParamItem _pstProtParamItem);
	void GetProtConfigFromFile(CString strConfigFileName, pstProtocolConfig _pstProtocolConfig);
	void InitProtConfig(pstProtocolConfig _pstProtocolConfig, int iMaxProtParamCount);
	void SetProtConfigStruct(pstProtocolConfig _pstProtocolConfig,
							 pstProtParamItem _pstProtParamItem,
							 int iParamIndex);
	void SetProtParamItemStruct(char* pchLineBuf,
								pstProtParamItem _pstProtParamItem,
								int * piPosInArray);
	void GetGainConfigFromFile(CString strConfigFileName, pstAllGainConfig _pstAllGainConfig);
	void SetSingleGainConfigStruct(char* pchLineBuf,
									pstSingleGainConfigInfo _pstSingleGainConfigInfo);
	VOID SaveData(  HWND hWnd,
					CString strFileFullName,
					CString strColumnName,
					int * piFirstArray,
					int * piSecondArray,
					int * piThirdArray,
					int * piForthArray,
					int iArraySize,
					int iDataArrayCount,
					BOOL bAppend);
	VOID SaveData(  HWND hWnd,
					CString strFileFullName,
					CString strColumnName,
					double * pdblFirstArray,
					double * pdblSecondArray,
					double * pdblThirdArray,
					double * pdblForthArray,
					int iArraySize,
					int iDataArrayCount,
					BOOL bAppend);
	VOID SaveData(  HWND hWnd,
					CString strFileFullName,
					CString strColumnName,
					DWORD * pdwArrayAddr,
					int iArraySize,
					int iDataArrayCount,
					BOOL bAppend);
	VOID SaveIntData(HWND hWnd,
					CString strFileFullName,
					CString strColumnName,
					int piData[][MAX_VOA_NUMBER],
					int iRowCount,
					int iColCount,
					BOOL bAppend);
	VOID SaveMatrix(HWND hWnd,
					CString strFileFullName,
					pstMatrix _pstMatrix,
					double **pdblMatrixData,
					BOOL bAppend);
	BOOL AllocateResource(double **pArrayAddress,
						  DWORD dwArraySize,
						  BYTE btDefaultValue);
	VOID FreeResource(double **pArrayAddress,
					  DWORD dwArraySize);
	VOID RegularizeArray(double * pdblFirstArray,
						double * pdblSecondArray,
						double * pdblThirdArray,
						double * pdblForthArray,
						int iArraySize,
						int iRegularPointQuantity,
						double dblStep,
						int iArrayType,
						double dblStartValue);
	BOOL FindValidDataIndex(const double * pdblTargetArray,
							const int iArraySize,
							int iArrayType,
							const double dblMinValidData,
							const double dblMaxValidData,
							int * piValidDataStartID,
							int * piValidDataEndID);
	BOOL ReadMappingFile(CString strMappingFileName,
						double * pdblFirstArray,
						double * pdblSecondArray,
						double * pdblThirdArray,
						double * pdblForthArray,
						int * piArraySize,
						char * pchrrorMsg);
	int FindNearestPosInArray(const double * pdblTargetArray,
							int iArraySize,
							double dblMatchValue,
							int * piLessEqualGreatFlag,
							int iArrayType);
	double FindMostFitValue(double * pdblFirstArray,
							double * pdblSecondArray,
							int iArraySize,
							int iArrayType,
							double dblMatchValue,
							BOOL bOnlyFindNearestValue);
	BOOL ParseOneLineData(char* pchLineBuf,
							double * pdblFirstArray,
							double * pdblSecondArray,
							double * pdblThirdArray,
							double * pdblForthArray,
							int iCurArrayIndex);
	double ComputeCurrentChannelWL(pstModulePara _pstModulePara, int iChannelID);
	VOID GetFileExtName(CString strFileFullName, CString & strFileExtName, CString & strFileName);
	VOID SplitString(CString strSourceString,
					int iTargetStrPos,
					LPCSTR lpcstrTargetStr);
	CString GetFilePath(CString strFileFullName);
	BOOL GetKeyValueFromFile(const CString strProtParamFullName,
							 CMapStringToString &mapProtParam,
							 CListBox & ctrlboxAvaliParam);
	BOOL GetStringFromFile(const CString strProtConfigFullName,
							CListBox * pboxCtrlAvailProt,
							CComboBox * pcomboboxProtocolType = NULL);
	int GetArrayIndex(DWORD * pdwArray, DWORD dwArraySize, int iArrayType, double dblMatchValue);
	BOOL ReadWritePDSlopeIntercept( BOOL bIsRead,
									CString strConfigFileName,
									pstPDSlopeIntercept _pstPDSlopeIntercept,
									int iTemperType,
									int iPDChIndex);
	BOOL ReadWritePDVerifyError(BOOL bIsRead,
								CString strConfigFileName,
								pstPDVerifyError _pstPDVerifyError,
								int iTemperType,
								int iPDChIndex);
	BOOL ReadWriteVOAMinILDAC(  BOOL bIsRead,
								CString strConfigFileName,
								pstSingleVOAMinILDAC _pstSingleVOAMinILDAC,
								pstVOAPara _pstVOAPara,
								int iVOAChIndex);

	BOOL ReadWriteVOAVerifyError(BOOL bIsRead,
								CString strConfigFileName,
								pstVOAVerifyError _pstVOAVerifyError,
								pstVOAPara _pstVOAPara,
								int iVOAChIndex);
	VOID SetComboBoxSel(CComboBox& pctrlComboBox, CString strFindStr, int& piCurSel);
	CString GetComboBoxString(CComboBox& pctrlComboBox, int iSelItem);
	BOOL ComputeAverage(double * pdblTargetArray,
						int iArraySize,
						double * pdblAverage);
	BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName);

private:

	DWORD			m_dwMaxMessageLine;

};

class CMyPoint
{
public:
	CMyPoint(double dblNewX = 0, double dblNewY = 0)
	{
		dblX = dblNewX;
		dblY = dblNewY;
	}
	double	GetX() {return dblX;}
	double	GetY() {return dblY;}

	void	SetX(double dblNewX) {dblX = dblNewX;}
	void	SetY(double dblNewY) {dblY = dblNewY;}

	friend	double CCommonFunction::LinearFit(CMyPoint pointArray[], int iPointNum, double * pdblSlope, double * pdblIntercept);   
private:
	double	dblX, dblY;
};

#endif //!defined(AFX_CCOMMONFUNCTION_H__5F9E481C_89F4_463A_AF1D_6DCEF015B195__INCLUDED_)

