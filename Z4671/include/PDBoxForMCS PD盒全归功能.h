// PDBoxForMCS.h: interface for the CPDBoxForMCS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PDBOXFORMCS_H__F461E905_3B6B_4AC6_8E98_9FCEC14CB70C__INCLUDED_)
#define AFX_PDBOXFORMCS_H__F461E905_3B6B_4AC6_8E98_9FCEC14CB70C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpComm.h"

class CPDBoxForMCS : public COpComm  
{
public:
	BOOL HitlessTest(int nMinADC,int nSWTime);
	BOOL StartMonitor(int nPort);
	BOOL StopScan();
	BOOL GetScanData();
	BOOL GetLengthAndCRC(DWORD *dwLength,DWORD *dwCRC);
	BOOL ScanTest(int nMinADC,int nMaxADC);
	CString m_strErrorLog;
	BOOL GetAllPDADC(int *pnADC);
	BOOL GetAllPower(double *dbreadpow);
	BOOL GetSiglePower(int nSigle, double *dbreadpow);
	BOOL NewSendPDMon(int chan, int npoint);
	BOOL NewGetPDTriCout(int chan);
	BOOL NewGetMaxPdCout(int chan, int npoint, int *maxid, double *maxpow);
	BOOL NewGetMaxADC(int chan, int npoint, double checkdb, WORD *maxindex);
	CPDBoxForMCS();
	virtual ~CPDBoxForMCS();
	bool GetHitless(WORD* wValue);
};

#endif // !defined(AFX_PDBOXFORMCS_H__F461E905_3B6B_4AC6_8E98_9FCEC14CB70C__INCLUDED_)
