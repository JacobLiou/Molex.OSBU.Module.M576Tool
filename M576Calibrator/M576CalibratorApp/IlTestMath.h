#pragma once

#include <afxstr.h>
#include <cmath>
#include <limits>
#include <map>

/// IL Test wavelength/source (matches Diagnosis s1/s2/s3).
enum class IlTestWlKind
{
	Sfp1550 = 0,
	Sfp1310 = 1,
	Laser1310 = 2,
};

inline LPCTSTR IlTestWlLabel(IlTestWlKind k)
{
	switch (k)
	{
	case IlTestWlKind::Sfp1550: return _T("SFP_1550");
	case IlTestWlKind::Sfp1310: return _T("SFP_1310");
	case IlTestWlKind::Laser1310: return _T("Laser_1310");
	default: return _T("?");
	}
}

/// 1x8 source port (FIM SetTestWL / DriveNewBox1X8Switch): 1550->1, 1310 SFP->4, Laser/Ext->8.
inline int IlTestSw3Third(IlTestWlKind k)
{
	switch (k)
	{
	case IlTestWlKind::Sfp1550: return 1;
	case IlTestWlKind::Sfp1310: return 4;
	case IlTestWlKind::Laser1310: return 8;
	default: return 1;
	}
}

/// Same port as IlTestSw3Third — FIM SetNewBoxWavelength: `SWL <port> <nm>`.
inline int IlTestSwlChannel(IlTestWlKind k)
{
	return IlTestSw3Third(k);
}

inline int IlTestWavelengthNm(IlTestWlKind k)
{
	return (k == IlTestWlKind::Sfp1550) ? 1550 : 1310;
}

/// Parse Diagnosis pd/opm reply as signed integer (trims whitespace).
BOOL IlTestParseIntReply(const CStringA& reply, int& outVal);

/// PD monitor and OPM in dBm (same scales as FIM GetSacnResultData).
inline double IlTestPdDbm(int pdRaw) { return (double)pdRaw / 100.0; }
inline double IlTestOpmDbm(int opmRaw) { return (double)opmRaw / 10000.0; }

/// Compatibility aliases (historical Pref/Pout naming).
inline double IlTestPrefDbm(int pdRaw) { return IlTestPdDbm(pdRaw); }
inline double IlTestPoutDbm(int opmRaw) { return IlTestOpmDbm(opmRaw); }

/// IL_dB = OPM - PD (align FIM CTestTaskManager::ILTest: pScanData - pScanADCData).
inline double IlTestComputeIlDb(int pdRaw, int opmRaw)
{
	return IlTestOpmDbm(opmRaw) - IlTestPdDbm(pdRaw);
}

/// Parse "CH70" / "70" -> 1..576. Returns 0 if invalid.
int IlTestParseChannelIndex(const CString& channelLabel);

/// CH k (1..576) -> "MPOin->MPOout" using 12-fiber MPO packing (same as diff.csv).
/// idx0=(k-1); mpo=idx0/12+1; fiber=idx0%12+1; outMpo=mpo+48.
BOOL IlTestChannelToMpoPath(int channel1to576, CString& outPath);

/// CH k (1..576) -> InPort "MPOa-b", OutPort "MPOc-b" (same packing as IlTestChannelToMpoPath).
BOOL IlTestChannelToMpoPorts(int channel1to576, CString& inPort, CString& outPort);

struct IlTestGateParams
{
	double absIlMinDb = -5.0;
	double absIlMaxDb = 5.0;
	double spanMaxDb = 0.15; // fail if rolling (Max-Min) across laps exceeds this
};

/// PASS when absIlMin <= IL <= absIlMax and rolling Span=Max-Min <= spanMaxDb.
inline BOOL IlTestJudgePass(double currentIlDb, double rollingSpanDb, const IlTestGateParams& gate)
{
	if (currentIlDb < gate.absIlMinDb || currentIlDb > gate.absIlMaxDb)
		return FALSE;
	if (rollingSpanDb > gate.spanMaxDb)
		return FALSE;
	return TRUE;
}

struct IlTestRollingStats
{
	int sampleCount = 0;
	double ilMax = -std::numeric_limits<double>::infinity();
	double ilMin = std::numeric_limits<double>::infinity();

	void Add(double il)
	{
		if (sampleCount == 0)
		{
			ilMax = il;
			ilMin = il;
		}
		else
		{
			if (il > ilMax)
				ilMax = il;
			if (il < ilMin)
				ilMin = il;
		}
		++sampleCount;
	}

	double Span() const
	{
		if (sampleCount <= 0)
			return 0.0;
		return ilMax - ilMin;
	}
};

using IlTestStatsMap = std::map<CString, IlTestRollingStats>;

inline CString IlTestStatsKey(const CString& channel, const CString& wlLabel)
{
	CString k = channel;
	k += _T('|');
	k += wlLabel;
	return k;
}
