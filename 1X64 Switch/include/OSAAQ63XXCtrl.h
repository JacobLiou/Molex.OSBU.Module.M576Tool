// OSAAQ63XXCtrl.h: interface for the COSAAQ63XXCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OSAAQ63XXCTRL_H__303F8512_4842_494F_84B8_1F1D5862045E__INCLUDED_)
#define AFX_OSAAQ63XXCTRL_H__303F8512_4842_494F_84B8_1F1D5862045E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "op816x.h"
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")

#define		WINSOCK_VERSION					0x11
#define		MAX_FIELD						8192
#define		MAX_DATA_COUNT					2048

#define		DEFAULT_USERNAME				"open \"anonymous\"\r\n"
#define		DEFAULT_PASSWORD				"1234\r\n"
#define		DEFAULT_PORTNUM					10001
#define		CMD_TAIL						"\r\n"

//UNIT
#define		UNIT_DBM						"dBm"
#define		UNIT_THZ						"THz"
#define		UNIT_GHZ						"GHz"
#define		UNIT_NM							"NM"

#define		CMD_RST							"*RST\r\n"
#define		CMD_COMPLETE_QUERY				"*OPC?\r\n"
#define		CMD_STOP						":ABOR\r\n"
#define		CMD_UINT_FREQUENCY				":UNIT:X FREQUENCY\r\n"
#define		CMD_UINT_WAVELENGTH				":UNIT:X WAVELENGTH\r\n"

#define		CMD_SCANSMOD_SINGLE				":INIT:SMOD SINGLE\r\n"
#define		CMD_SCANSMOD_REPEAT				":INIT:SMOD REPEAT\r\n"
#define		CMD_SCANSMOD_AUTO				":INIT:SMOD AUTO\r\n"
#define		CMD_SCAN_EXECUTE				":INIT \r\n"

#define		CMD_SET_WAVELENGTH_SPAN			":SENS:WAV:SPAN"
#define		CMD_SET_WAVELENGTH_CENTER		":SENS:WAV:CENT"
#define		CMD_SET_WAVELENGTH_START		":SENS:WAV:STAR"
#define		CMD_SET_WAVELENGTH_STOP			":SENS:WAV:STOP"
#define		CMD_SET_RESOLUTION				":SENS:BWID:RES"
#define		CMD_SET_SWEEP_POINT				":SENS:SWE:POIN"
#define		CMD_SET_SWEEP_REF_LEVEL			":DISP:TRAC:Y1:RLEV"
#define		CMD_SET_POWER_OFFSET_LEVEL		":DISP:TRAC:Y2:OLEV"
#define		CMD_SET_SENS_LEVEL				":SENS:SENS"
#define		CMD_SET_SUB_SCALE_LEVEL			":DISP:TRAC:Y1:PDIV"
#define		CMD_SET_TRACE_ATTRIBUTE			":TRAC:ATTR:"
#define		CMD_SET_TRACE_ACTIVE			":TRAC:ACT"
#define		CMD_SET_TRACE_DISPLAY			":TRAC:STAT:"
#define		CMD_SET_TRACE_MATH				":CALC:MATH:"
#define		CMD_SET_MARKER_X_POINT			":CALC:MARK:X 0"
#define		CMD_SET_LINE_MARKER_X1_POINT	":CALC:LMAR:X 1"
#define		CMD_SET_LINE_MARKER_X2_POINT	":CALC:LMAR:X 2"
#define		CMD_SET_LINE_MARKER_Y1_POINT	":CALC:LMAR:Y 3"
#define		CMD_SET_LINE_MARKER_Y2_POINT	":CALC:LMAR:Y 4"

#define		CMD_SET_SCAN_DATA_FORMAT		":FORM:DATA"
#define		CMD_GET_SCAN_DATA_COUNT			":TRAC:DATA:SNUM?"
#define		CMD_GET_SCAN_XAXIS_DATA			":TRAC:DATA:X?"
#define		CMD_GET_SCAN_YAXIS_DATA			":TRAC:DATA:Y?"

//Output Data Format
#define		DATA_FORMAT_REAL_64				"REAL,64"
#define		DATA_FORMAT_REAL_32				"REAL,32"
#define		DATA_FORMAT_ASCII				"ASCII"
//PARAMETER
#define		SENS_NORMAL_HOLD				"NHLD"
#define		SENS_NORMAL_AUTO				"NAUT"
#define		SENS_NORMAL						"NORM"
#define		SENS_MID						"MID"
#define		SENS_HIGH1						"HIGH1"
#define		SENS_HIGH2						"HIGH2"
#define		SENS_HIGH3						"HIGH3"

#define		TRACE_A							"TRA"
#define		TRACE_B							"TRB"
#define		TRACE_C							"TRC"
#define		TRACE_D							"TRD"
#define		TRACE_E							"TRE"
#define		TRACE_F							"TRF"
#define		TRACE_G							"TRG"

#define		TRACE_WRITE						"WRITE"
#define		TRACE_FIX						"FIX"
#define		TRACE_MAX						"MAX"
#define		TRACE_MIN						"MIN"
#define		TRACE_RAVG						"RAVG"
#define		TRACE_CALC						"CALC"


class COSAAQ63XXCtrl  
{
public:
	BOOL GetAllEDFAResult(PAutoOSAData pOSAResult);
	BOOL SetRBW(double dblRBW);
	BOOL SetPowOffsetTable(USHORT usWavelength,double dblPowOffset);
	BOOL GetWLOffsetTable(USHORT usWavelength,double* pdblWLOffset);
	BOOL SetWLOffsetTable(USHORT usWavelength,double dblWLOffset);
	BOOL RunSweep(double* pdblSweepDataX,double* pdblSweepDataY,char szSelTrace);
	BOOL SetOSALocalCtrl();
	BOOL GetAmpOutSigOffset(double* pdblOutSigOffset);
	BOOL GetAmpSrcSigOffset(double* pdblSrcSigOffset);
    BOOL SetOffset(BOOL bSource, double dblOffset);
	BOOL SetAmpOutSigOffset(double dblOutSigOffset);
	BOOL SetAmpSrcSigOffset(double dblSrcSigOffset);
	BOOL GetChannelSourcePower(double* pdblSourcePow, WORD* pdwChannelCount);
	BOOL GetPowOffsetTable(USHORT usWavelength,double* pdblPowOffset);
	BOOL InitAmpTest(double dblStartWL,double dblStopWL,double dblRBW);
	BOOL GetGPIBAress(int *iGpibAddress);
	BOOL GetRBW(double* pdblRBW);
	BOOL StartAnalysis();
	BOOL SetTraceAct(char szSelTrace);
	BOOL SetTraceAttribute(char szSelTrace,USHORT uiAttribute);
	BOOL MeasureSource();
	BOOL MeasureAmplifier();
	BOOL GetEDFAResult(double *pdblSumSrcSigPwr, double *pdblSumAmpSigPwr, double *pdblSunGain);
	BOOL etAllEDFAResult(PAutoOSAData pOSAResult);
	BOOL GetEDFAResult(char* pszResult,USHORT uiLength);
	BOOL InitSingleSweep(double dblStartWL, double dblStopWL, double dblRBW, WORD dwSweepPoint);
	BOOL GetWavelengthOffset(double* pdblWLOffset);
	BOOL GetPowerOffset(double* pdblOSAPowOffset);
	BOOL GetChannelSNR(double* pdblSNR,WORD* pdwChannelCount);
	BOOL GetChannelNF(double* pdblNF,WORD* pdwChannelCount);
	BOOL GetChannelGain(double* pdblGain, WORD* pdwChannelCount);
	BOOL GetChannelWL(double* pdblWavelens,WORD* pdwChannelCount);
	BOOL GetChannelCount(WORD* pdwChannelCount);
	BOOL SetSweepRange(double dblStartWL, double dblStopWL);
	BOOL SetAnalysisType(WORD wAnalysisType);
	long fnDistillScanData(CString strTraceName, CString strDataFormat, char *pchAxisX, char *pchAxisY, DWORD dwBufferSize);
//	long fnDistillScanData(CString strTraceName, CString strDataFormat, double *pchAxisX, double *pchAxisY);
	BOOL fnSetLineMarkerY2Point(double dblSetPoint, CString strUnit);
	BOOL fnSetLineMarkerY1Point(double dblSetPoint, CString strUnit);
	BOOL fnSetLineMarkerX2Point(double dblSetPoint, CString strUnit);
	BOOL fnSetLineMarkerX1Point(double dblSetPoint, CString strUnit);
	BOOL fnSetMarkerXPoint(double dblSetPoint, CString strUnit);
	BOOL fnSetTraceCalcMath(CString strTraceName, CString strMath = "A-B(LOG)");
	BOOL fnSetTraceDisplay(CString strTraceName, BOOL bDisplay);
	BOOL fnSetTraceActive(CString strTraceName);
	BOOL fnSetTracAttribute(CString strTraceName, CString strAttribute);
	BOOL fnSetSubScaleLevel(double dblSetValue);
	BOOL fnSetScanSensitivity(CString strSetValue = SENS_MID);
	BOOL fnSetPowerOffsetLevel(double dblSetValue);
	BOOL fnSetReferenceLevel(double dblSetValue);
	BOOL fnSetSweepPoint(long lSetPoint);
	BOOL fnCompleteQuery();
	BOOL fnSetResolution(double dblSetValue, CString strUnit);
	BOOL fnSetWavelengthStop(double dblStopWav, CString strUnit);
	BOOL fnSetWavelengthStart(double dblStartWav, CString strUnit);
	BOOL fnSetWavelengthCenter(double dblSetWavelength, CString strUnit);
	BOOL fnSetWavelengthSpan(double dblSpan, CString strUnit);
	BOOL fnSwitchUNIT(CString	strSwitchUNIT = CMD_UINT_FREQUENCY);
	BOOL fnReset(CString strSendCommand = CMD_RST);
	BOOL fnDoSweep(CString strSCANSMOD = CMD_SCANSMOD_SINGLE);	//SCANSMOD_SINGLE\SCANSMOD_REPEAT\SCANSMOD_AUTO
	BOOL fnOpenDeviceViaEthernet(LPCTSTR lpDeviceAddr, LPCTSTR lpUserName = DEFAULT_USERNAME, LPCTSTR lpPassword = DEFAULT_PASSWORD, UINT uPort = DEFAULT_PORTNUM);
	BOOL fnInitEthernet(byte byVersion = WINSOCK_VERSION);	//if winsock version is 1.1 byVersion = 0x11
	COSAAQ63XXCtrl();
	virtual ~COSAAQ63XXCtrl();
private:
	BOOL fnReceive(char * pchReceive, DWORD dwBufferSize);
	BOOL fnSend(CString strSendCommand);
	sockaddr_in			m_addr;
	SOCKET				m_hSocket;
};

#endif // !defined(AFX_OSAAQ63XXCTRL_H__303F8512_4842_494F_84B8_1F1D5862045E__INCLUDED_)
