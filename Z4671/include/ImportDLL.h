// ImportDLL.h : header file

// Include the head file of class CCommonFunction
#include "CommonFunction.h"

#include "op816x.h"
#include "op8169.h"
#include "Hp8156A.h"		// use to operate attenuator
#include "OSA_HP86142.h"	// use to operate OSA

// ----------------------------------------------------------------------------------
// Import the function from Configuration.dll
#if !defined(FLAG_CONFIGURATION_INTERFACE)
	extern "C" __declspec(dllimport) int ShowConfigDlg(WORD wPageIndex = 0);
//	extern "C" __declspec(dllimport) BOOL GetAllParameters(int iParaIndex, CString strConfigFileName = "");
	extern "C" __declspec(dllimport) PDWORD GetConfigParamPtr(int iParaIndex);
	extern "C" __declspec(dllimport) BOOL UpdateSetting(BOOL bIsRead, int iParaIndex);
	extern "C" __declspec(dllimport) BOOL WriteConfigToFile(BOOL bIsRead, int iParaIndex, PDWORD pStruct);
	extern "C" __declspec(dllimport) BOOL IsSettingApplied();
	extern "C" __declspec(dllimport) VOID ReadWriteScanningTaskParam(BOOL bIsRead, CString & strConfigFileName);
	extern "C" __declspec(dllimport) VOID ReadWritePolarizerAngle(BOOL bIsRead);
	extern "C" __declspec(dllimport) VOID GetConfigChildDir(CString & strConfigChildDir);
#endif
// ----------------------------------------------------------------------------------
// Import the function from Reporter.dll
#if !defined(FLAG_REPORTER_INTERFACE)
	extern "C" __declspec(dllimport) BOOL ShowReporterDlg();
#endif

// ----------------------------------------------------------------------------------
// Import the function from OWUseParallelPort.dll
extern "C" __declspec(dllimport) void SetSpeed(int iStandard);
extern "C" __declspec(dllimport) void CreateParallelPortInstance();
extern "C" __declspec(dllimport) void SetPCDelayCoefficient(int iPCDelayCoefficient);

extern "C" __declspec(dllimport) void TickDelay(int iTickCounter);
extern "C" __declspec(dllimport) int OWTouchReset(void);
extern "C" __declspec(dllimport) void OWWriteBit(int iBit);
extern "C" __declspec(dllimport) int OWReadBit(void);
extern "C" __declspec(dllimport) void OWWriteByte(int iData);
extern "C" __declspec(dllimport) int OWReadByte(void);
extern "C" __declspec(dllimport) int OWTouchByte(int iData);
extern "C" __declspec(dllimport) void OWBlock(unsigned char *pchData, int iDataLength);
extern "C" __declspec(dllimport) int OWOverdriveSkip(unsigned char *pchData, int iDataLength);
extern "C" __declspec(dllimport) int ReadPageMAC(int iPage, unsigned char *pChSavedData, unsigned char *pChMac);
extern "C" __declspec(dllimport) void ParallelPortTest();
extern "C" __declspec(dllimport) bool WriteDataByOneWireWithCheck(unsigned long dwChipInnerAddress,
																  unsigned char * pchDataForWrite,
																  unsigned char * pchReceivedData,
																  int iDataLength,
																  int iWriteCheckTimes);
extern "C" __declspec(dllimport) bool ReadSpecifiedAddress(unsigned long dwStartAddress, unsigned long dwDataSize, unsigned char * pChSavedData);
extern "C" __declspec(dllimport) bool ReadDataByOneWireWithCheck(unsigned long dwStartAddress, unsigned long dwDataSize, unsigned char * pchSavedData, int iReadCheckTimes);

// ----------------------------------------------------------------------------------
// Import the function from OpticsScanning.dll
#if !defined(FLAG_OPTICAL_SCAN_INTERFACE)
	extern "C" __declspec(dllimport) HWND ShowOpticsScanDlg(UINT nIDTemplate, CWnd * pWnd, int iWindowMode);
	extern "C" __declspec(dllimport) void UpdateParamOpticsScan();
	extern "C" __declspec(dllimport) HWND GetWindowHandleOpticsScan();
	extern "C" __declspec(dllimport) VOID SetOp816XOpticsScan(COp816X * HP816X);
	extern "C" __declspec(dllimport) VOID SetOp8169OpticsScan(COp8169 * HP8169);
	extern "C" __declspec(dllimport) COp816X * GetOp816XOpticsScan();
	extern "C" __declspec(dllimport) COp8169 * GetOp8169OpticsScan();

	extern "C" __declspec(dllimport) VOID SetOp816XOpenedFlagOpTest(BOOL b8164Opened);
	extern "C" __declspec(dllimport) VOID SetOp8169OpenedFlagOpTest(BOOL b8169Opened);
	extern "C" __declspec(dllimport) BOOL GetOp816XOpenedFlagOpTest();
	extern "C" __declspec(dllimport) BOOL GetOp8169OpenedFlagOpTest();
#endif

// ----------------------------------------------------------------------------------
// Import the function from Communication.dll
#if !defined(FLAG_COMMUNICATION_INTERFACE)
	extern "C" __declspec(dllimport) BOOL ProcessCommand(HWND hWnd,
													pstConfigInfo _pstConfigInfo,
													int bIsRead);
	extern "C" __declspec(dllimport) BOOL OpenPort(HWND hWnd,
													pstConfigInfo _pstConfigInfo,
													LPTSTR strPortName,			// COM port index
													DWORD dwBaudRate,			// Baud Rate:115200
													BYTE btByteSize,			// 8 data bits
													BYTE btParity,				// no parity
													BYTE btStopBits);			// one stop bit
	extern "C" __declspec(dllimport) VOID ClosePort(HWND hWnd, pstConfigInfo _pstConfigInfo, LPTSTR strPortName);
	extern "C" __declspec(dllimport) BOOL IsPortOpened();
	extern "C" __declspec(dllimport) BOOL StartFWUpgrade(HWND hWnd, pstConfigInfo _pstConfigInfo);
	extern "C" __declspec(dllimport) BOOL DownloadFWImage(HWND hWnd, pstConfigInfo _pstConfigInfo);
	extern "C" __declspec(dllimport) void SoftWareReset(HWND hWnd, pstConfigInfo _pstConfigInfo);
	extern "C" __declspec(dllimport) HWND ShowCommunicationDlg(UINT nIDTemplate, CWnd * pWnd, int iWindowMode);
	extern "C" __declspec(dllimport) void UpdateParamComm();
	extern "C" __declspec(dllimport) HWND GetWindowHandleComm();

#endif

// ----------------------------------------------------------------------------------
// Import the function from SerialPort.dll
#if !defined(FLAG_SERIAL_PORT_INTERFACE)
	extern "C" __declspec(dllimport) BOOL OpenSerialPortDLL(LPTSTR strPortName,	// COM port index
														DWORD dwBaudRate,		// Baud Rate:115200
														BYTE btByteSize,		// 8 data bits
														BYTE btParity,			// no parity
														BYTE btStopBits);		// one stop bit;
	extern "C" __declspec(dllimport) VOID CloseSerialPortDLL();
	extern "C" __declspec(dllimport) BOOL ReadSerialPortBuffer(unsigned char* pInBuffer, DWORD dwBufferSize);
	extern "C" __declspec(dllimport) BOOL WriteSerialPortBuffer(unsigned char* pOutBuffer, DWORD dwBufferSize);
#endif

// ----------------------------------------------------------------------------------
// Import the function from I2C.dll
#if !defined(FLAG_I2C_INTERFACE)
	extern "C" __declspec(dllimport)
			BOOL OpenI2CPortDLL(WORD wDeviceAddr,			//the device under test slave address
							WORD wI2CAdapterSlaveAddr,	//the I2C Adapter slave address
							WORD wComPortIndex,			//RS-232 Serial Port index (1 or 2)
							DWORD dwCOMBaudRate,		//RS-232 Serial Port Baud Rate (9600,..,115200)
							DWORD dwI2CBUSRATE,			//the I2C Adapter bus rate for iPort(0: 23, 1:86, 2:100, 3: 400KHz)
							pstProcStatus _pstProcStatus);

	extern "C" __declspec(dllimport) BOOL CloseI2CPortDLL();
	extern "C" __declspec(dllimport) BOOL SendI2CDataDLL(WORD wDeviceAddr,
														BYTE *pbtCommandStr,
														WORD wTxLen,
														pstProcStatus _pstProcStatus);
	extern "C" __declspec(dllimport) BOOL ReceiveI2CDataDLL(WORD wDeviceAddr,
															BYTE *pbtReceivedMsg,
															WORD wRxLen,
															pstProcStatus _pstProcStatus);
#endif

// ----------------------------------------------------------------------------------
// Import the function from FirmwareUpgrade.dll
#if !defined(FLAG_UPGRADE_FIRMWARE_INTERFACE)
	extern "C" __declspec(dllimport) HWND ShowUpgradeDlg(UINT nIDTemplate, CWnd * pWnd, int iWindowMode);
	extern "C" __declspec(dllimport) void UpdateParamUpgrade();
	extern "C" __declspec(dllimport) HWND GetWindowHandleUpgrade();
	extern "C" __declspec(dllimport) CWnd * GetWindowPointerUpgrade();
	extern "C" __declspec(dllimport) BOOL CSVFileToDataBin(const CString strCFileFullName,
														CString strDataBinFileFullName,
														const WORD wDataNum,
														const int iMCUDataSequenceType,
														int iDataSizeType,
														char * pchErrorMsg);
	extern "C" __declspec(dllimport) BOOL DataBinToUpgradableBin(const CString strDataBinFileFullName,
																CString strUpgradableBinFileFullName,
																const WORD wSectorSizeInByte,
																const int iMCUDataSequenceType,
																DWORD dwVOALUTStartAddrInMCU,
																WORD wChannelIndex,
																char * pchErrorMsg,
																BYTE btBinFileType);
	extern "C" __declspec(dllimport) BOOL ExecuteUpgrade(HWND hWnd,
														pstConfigInfo _pstConfigInfo,
														CCommonFunction & instanceComFunc,
														CProgressCtrl * pctrlUpgradeProgress,
														CListBox * pListBox,
														int iUpgradeFileCount,
														char * pchMultiBinFileName[][MAX_PATH],
														DWORD dwSleepTime);
#endif

// ----------------------------------------------------------------------------------
// Import the function from CalibratePD.dll
#if !defined(FLAG_CALIBRATE_PD_INTERFACE)
	extern "C" __declspec(dllimport) HWND ShowCalibPDDlg(UINT nIDTemplate, CWnd * pWnd, int iWindowMode);
	extern "C" __declspec(dllimport) void UpdateParamCalibPD();
	extern "C" __declspec(dllimport) HWND GetWindowHandleCalibPD();
	extern "C" __declspec(dllimport) VOID SetOp816XCalibPD(COp816X * HP816X);
	extern "C" __declspec(dllimport) VOID SetOp8169CalibPD(COp8169 * HP8169);
	extern "C" __declspec(dllimport) COp816X * GetOp816XCalibPD();
	extern "C" __declspec(dllimport) COp8169 * GetOp8169CalibPD();
	extern "C" __declspec(dllimport) BOOL GetRefDoneFlagCalibPD();
	extern "C" __declspec(dllimport) VOID SetRefDoneFlagCalibPD(BOOL bRefDone);
	extern "C" __declspec(dllimport) DWORD * GetRefDataPointerCalibPD();
	extern "C" __declspec(dllimport) VOID SetRefDataPointerCalibPD(DWORD * pdwRefDataPointer);
#endif

// ----------------------------------------------------------------------------------
// Import the function from CalibrateVOA.dll
#if !defined(FLAG_CALIBRATE_VOA_INTERFACE)
	extern "C" __declspec(dllimport) HWND ShowCalibVOADlg(UINT nIDTemplate, CWnd * pWnd, int iWindowMode);
	extern "C" __declspec(dllimport) void UpdateParamCalibVOA();
	extern "C" __declspec(dllimport) HWND GetWindowHandleCalibVOA();
	extern "C" __declspec(dllimport) VOID SetOp816XCalibVOA(COp816X * HP816X);
	extern "C" __declspec(dllimport) VOID SetOp8169CalibVOA(COp8169 * HP8169);
	extern "C" __declspec(dllimport) COp816X * GetOp816XCalibVOA();
	extern "C" __declspec(dllimport) COp8169 * GetOp8169CalibVOA();
	extern "C" __declspec(dllimport) BOOL GetRefDoneFlagCalibVOA();
	extern "C" __declspec(dllimport) VOID SetRefDoneFlagCalibVOA(BOOL bRefDone);
	extern "C" __declspec(dllimport) DWORD * GetRefDataPointerCalibVOA();
	extern "C" __declspec(dllimport) VOID SetRefDataPointerCalibVOA(DWORD * pdwRefDataPointer);
	extern "C" __declspec(dllimport) VOID GetCurVOAMinILDAC(CCommonFunction & _instanceComFunc,
															pstConfigInfo _pstConfigInfo,
															char * _tszAppPath,
															BOOL bIsRead,
															pstSingleVOAMinILDAC _pstSingleVOAMinILDAC,
															int iVOAChIndex,
															int iILDACFlag,
															double * pdblReturnIL,
															double * pdblReturnDAC);
	extern "C" __declspec(dllimport) BOOL GetAllVOAMinILDAC(CCommonFunction & _instanceComFunc,
															pstConfigInfo _pstConfigInfo,
															pstSingleVOAMinILDAC _pstSingleVOAMinILDAC,
															char * _tszAppPath,
															int piMinILDAC[][MAX_VOA_NUMBER],
															BOOL bNeedSave,
															int iGetILDACFlag,
															CString & strVOAMinILDACCSVFile);
#endif

// ----------------------------------------------------------------------------------
// Import the function from ReferenceTLS.dll
#if !defined(FLAG_REF_TLS_INTERFACE)
	extern "C" __declspec(dllimport) BOOL ShowReferenceTLSDlg();
	extern "C" __declspec(dllimport) BOOL IsReferenceDone();
	extern "C" __declspec(dllimport) DWORD * GetRefDataPointerRefTLS();
	extern "C" __declspec(dllimport) VOID SetOp816XReferenceTLS(COp816X * HP816X);
	extern "C" __declspec(dllimport) VOID SetOp8169ReferenceTLS(COp8169 * HP8169);
	extern "C" __declspec(dllimport) COp816X * GetOp816XReferenceTLS();
	extern "C" __declspec(dllimport) COp8169 * GetOp8169ReferenceTLS();

	extern "C" __declspec(dllimport) VOID SetOp816XOpenedFlagRefTLS(BOOL b8164Opened);
	extern "C" __declspec(dllimport) VOID SetOp8169OpenedFlagRefTLS(BOOL b8169Opened);
	extern "C" __declspec(dllimport) BOOL GetOp816XOpenedFlagRefTLS();
	extern "C" __declspec(dllimport) BOOL GetOp8169OpenedFlagRefTLS();
#endif

// ----------------------------------------------------------------------------------
// Import the function from EDFATesting.dll
#if !defined(FLAG_EDFA_TESTING_INTERFACE)
	extern "C" __declspec(dllimport) HWND ShowEDFATestingDlg(UINT nIDTemplate, CWnd * pWnd, int iWindowMode);
	extern "C" __declspec(dllimport) void UpdateParamEDFATesting();
	extern "C" __declspec(dllimport) HWND GetWindowHandleEDFATesting();
	extern "C" __declspec(dllimport) VOID SetOp816XEDFATesting(COp816X * pHP816X);
	extern "C" __declspec(dllimport) VOID SetOp81618AEDFATesting(COp816X * pHP81618A);
	extern "C" __declspec(dllimport) VOID SetOp8169EDFATesting(COp8169 * pHP8169);
	extern "C" __declspec(dllimport) COp816X * GetOp816XEDFATesting();
	extern "C" __declspec(dllimport) COp816X * GetOp81618AEDFATesting();
	extern "C" __declspec(dllimport) COp8169 * GetOp8169EDFATesting();
	extern "C" __declspec(dllimport) VOID SetOp816XOpenedFlagEDFATesting(BOOL b8164Opened);
	extern "C" __declspec(dllimport) VOID SetOp81618AOpenedFlagEDFATesting(BOOL b81618AOpened);
	extern "C" __declspec(dllimport) VOID SetOp8169OpenedFlagEDFATesting(BOOL b8169Opened);
	extern "C" __declspec(dllimport) BOOL GetOp816XOpenedFlagEDFATesting();
	extern "C" __declspec(dllimport) BOOL GetOp81618AOpenedFlagEDFATesting();
	extern "C" __declspec(dllimport) BOOL GetOp8169OpenedFlagEDFATesting();

	extern "C" __declspec(dllimport) VOID Set8156AEDFATesting(CHp8156A * pHP8156A);
	extern "C" __declspec(dllimport) CHp8156A * Get8156AEDFATesting();
	extern "C" __declspec(dllimport) VOID Set8156AOpenedFlagEDFATesting(BOOL bHP8156AOpened);
	extern "C" __declspec(dllimport) BOOL Get8156AOpenedFlagEDFATesting();

	extern "C" __declspec(dllimport) VOID SetOSA86142EDFATesting(COSA_HP86142 * _pHPOSA86142);
	extern "C" __declspec(dllimport) COSA_HP86142 * GetOSA86142EDFATesting();
	extern "C" __declspec(dllimport) VOID SetOSA86142OpenedFlagEDFATesting(BOOL bHPOSA86142Opened);
	extern "C" __declspec(dllimport) BOOL GetOSA86142OpenedFlagEDFATesting();
#endif

// ----------------------------------------------------------------------------------
// Import the function from Xmodem.dll
#if !defined(FLAG_XMODEM_INTERFACE)
	//Import the function
	extern "C" __declspec(dllimport) BOOL StartFWUpgradeXmodem(HWND hWnd, pstConfigInfo _pstConfigInfo);
	extern "C" __declspec(dllimport) BOOL DownloadFWImageXmodem(HWND hWnd, pstConfigInfo _pstConfigInfo);
	extern "C" __declspec(dllimport) void SoftWareResetXmodem(HWND hWnd, pstConfigInfo _pstConfigInfo);
#endif

// ----------------------------------------------------------------------------------
//Import the function from OWUseDS9097U.dll
extern "C" __declspec(dllimport) short ReadDefaultPort(short * pshPortNum, short * pshPortType);
extern "C" __declspec(dllimport) short iButtonFound(void);
extern "C" __declspec(dllimport) short ProgramBlock(unsigned short start_address, unsigned short pagetype, 
													unsigned char *writebuf, short writelen);
extern "C" __declspec(dllimport) short VerifyBlock(unsigned short start_address, unsigned short pagetype, 
													unsigned char *writebuf, short writelen);

// ----------------------------------------------------------------------------------
// Import the function from DeliverProduct.dll
#if !defined(FLAG_DELIVER_PRODUCT_INTERFACE)
	extern "C" __declspec(dllimport) HWND ShowDeliverProductDlg(UINT nIDTemplate, CWnd * pWnd, int iWindowMode);
	extern "C" __declspec(dllimport) void UpdateParamDeliverProduct();
	extern "C" __declspec(dllimport) HWND GetWindowHandleDeliverProduct();
#endif

// ----------------------------------------------------------------------------------
// Import the function from ViewTestData.dll
#if !defined(FLAG_VIEW_TEST_DATA_INTERFACE)
	extern "C" __declspec(dllimport) HWND ShowViewTestDataDlg(UINT nIDTemplate, CWnd * pWnd, int iWindowMode);
	extern "C" __declspec(dllimport) void UpdateParamViewTestData();
	extern "C" __declspec(dllimport) HWND GetWindowHandleViewTestData();
#endif
