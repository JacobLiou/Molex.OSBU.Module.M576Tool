#pragma once
#include "OpComm.h"
class CCMDInstrument :
	public COpComm
{
public:
	CCMDInstrument();
	~CCMDInstrument();

	
	BYTE GetChecksum(BYTE* pbBuffer, int nLength);
	CString GetMsgString();
	void AnlysisErrorString(BYTE bStatus);

	BOOL SetVOAxAtten(BYTE bInstance, double dbAtten);
	BOOL GetVOAxAtten(BYTE bInstance, double *pdbAtten);
	BOOL SetVOAxAttenToDiffrentValue(double dbAtten1, double dbAtten2);
	BOOL GetAllVOAAtten(double *pdbAtten1, double*pdbAtten2);
	BOOL SetVOAxDAC(BYTE bInstance, WORD dwDAC);
	BOOL GetVOAxDAC(BYTE bInstance, WORD* pwDAC);
	BOOL SetVOAxDACToDiffrentValue(WORD dwADC1, WORD dwADC2, WORD dwADC3, WORD dwADC4);
	BOOL GetVOAxMinIL(BYTE bInstance, double * pdbGetMinIL);
	BOOL GetAllVOAMinIL(double * pdbGetMinIL1, double*pdbGetMinIL2, double*pdbGetMinIL3);
	BOOL SetSwitchChannel(BYTE bInstance, BYTE bChannel);
	BOOL GetSwitchChannel(BYTE bInstance, BYTE* bGetChIndex);
	BOOL SetAllSwitchToDifferentChannel(BYTE bChannel, BYTE bChanne2, BYTE bChanne3, BYTE bChanne4, BYTE bChanne5, BYTE bChanne6, BYTE bChanne7, BYTE bChanne8, BYTE bChanne9);
	BOOL GetAllSwitchChannel(BYTE* bGetChArry);
	BOOL GetSwitchActualChannel(BYTE bInstance, BYTE* bGetChIndex);
	BOOL GetAllSwitchActualChannel( BYTE* bGetChArry);
	BOOL GetAllVOADAC(WORD* pwDAC1, WORD* pwDAC2, WORD* pwDAC3, WORD* pwDAC4);

	BOOL GetVersionInfo(CString & strPN, CString& strSN, CString& strEquipment, CString& strHWVersion, CString &strFWVersion, CString& strOplinFWVersion, CString & strMFGDate);
	BOOL GetPNInfo(CString& strPN);
	BOOL GetSNInfo(CString& strSN);
	BOOL GetEquipmentInfo(CString& strEquipment);
	BOOL GetHWInfo(CString& strHWVersion);
	BOOL GetFWInfo(CString& strFWVersion);
	BOOL GetOplinkFWInfo(CString& strOplinkVersion);
	BOOL SetInputVOAAtten(double dbAtten);
	BOOL GetInputVOAAtten(double *pdbAtten);
	BOOL SetMSAVOAAtten(double dbAtten);
	BOOL GetMSAVOAAtten(double *pdbAtten);
	BOOL GetInputMinIL(double * pdbGetMinIL);
	BOOL GetOutputMinIL(double * pdbGetMinIL);
	BOOL GetMSAMinIL(double * pdbGetMinIL);
	BOOL ConfigureGPIO(long nStatus);
	BOOL GetGPIOConfigure(long * plStatus);
	BOOL ConfigureGPIOx(BYTE bInstance, BOOL bStatus);
	BOOL GetGPIOxSetPoint(BYTE bInstance, BYTE* bStatus);
	BOOL SetAllInputGPIOPoint(long lStatus);
	BOOL GetAllInputGPIOPoint(long * plStatus);
	BOOL GetGPIOxStatus(BYTE bInstance, BYTE* bStatus);
	BOOL ConfigUARTx(BYTE bInstance, DWORD dwBaudRate = 115200, BYTE bByteSize = 8, BYTE bParity = NOPARITY, BYTE bStopBits = ONESTOPBIT, DWORD dwTimeOut = 0);
	BOOL ConfigAllUART(DWORD dwBaudRate = 115200, BYTE bByteSize = 8, BYTE bParity = NOPARITY, BYTE bStopBits = ONESTOPBIT, DWORD dwTimeOut = 0);
	BOOL WriteUARTxMessage(BYTE bInstance, BYTE* pBuffer, int nLength);
	BOOL ReadUARTxMessage(BYTE bInstance, BYTE* pBuffer, int nLength, int &nGetLength);
	BOOL WriteAllUARTMessage(BYTE* pBuffer, int nLength);
	BOOL SetSwitchToInputPath(int nChInput, int n2X2Ch, int nChOutput);
	BOOL SetSwitchToOutputPath(int nChInput, int n2X2Ch, int nChOutput);
	BOOL SetSwitchToMSAInToMSAOut(int nChInput, int nChOutput);
	BOOL SetSwitchToExpendPath(int nChInput, int nChOutput);
	BOOL SetSwitchMSAIToExtendPath(int nChInput, int nChOutput);
	BOOL SetSwitchSxInToMonitor(int nChInput, int nChOutput);
	BOOL SetSwitchReferencePath(int nChIndex, int nChOutput);
	BOOL SetSwitchMSAIMonitorPath(int nChInput, int nChOutput);
	BOOL SetSwitchMSAOToExtendPath(int nChInput, int nChOutput);
	BOOL SetSwitchInxToOutputPath(int nChInput, int nChOutput);
private:
	CString m_strMsg;
	CString m_strPN;
	CString m_strSerialNumber;
	CString m_strEquipmentNo;
	CString m_HWVersion;
	CString m_FWVersion;
	CString m_strOplinkFWVersion;
	CString m_strMFGDate;
public:
	BOOL SetInOutPath(bool bMutilChannel, BYTE byInPort, BYTE byOutPort, bool bOpm);
	BOOL SetReferencePath(BYTE byRefIndex, bool bOpemOrOsa);
};

