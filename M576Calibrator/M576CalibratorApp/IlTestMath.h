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

inline int IlTestWavelengthNm(IlTestWlKind k)
{
	return (k == IlTestWlKind::Sfp1550) ? 1550 : 1310;
}

/// Parse Diagnosis pd/opm reply as signed integer (trims whitespace).
BOOL IlTestParseIntReply(const CStringA& reply, int& outVal);

/// Pref (Power Monitor / pd) and Pout (OPM) in dBm from raw replies.
inline double IlTestPrefDbm(int pdRaw) { return (double)pdRaw / 100.0; }
inline double IlTestPoutDbm(int opmRaw) { return (double)opmRaw / 10000.0; }

/// IL_dB = Pref - Pout (Z4671 TestIL; topology Power Monitor -> Sensor Head).
inline double IlTestComputeIlDb(int pdRaw, int opmRaw)
{
	return IlTestPrefDbm(pdRaw) - IlTestPoutDbm(opmRaw);
}

/// Parse "CH70" / "70" -> 1..576. Returns 0 if invalid.
int IlTestParseChannelIndex(const CString& channelLabel);

/// CH k (1..576) -> "MPOin->MPOout" using 12-fiber MPO packing (same as diff.csv).
/// idx0=(k-1); mpo=idx0/12+1; fiber=idx0%12+1; outMpo=mpo+48.
BOOL IlTestChannelToMpoPath(int channel1to576, CString& outPath);

struct IlTestGateParams
{
	double spanMaxDb = 0.15;
	double absIlMinDb = 0.0;
	double absIlMaxDb = 3.0;
};

/// PASS when span OK and current IL within absolute window.
inline BOOL IlTestJudgePass(double currentIlDb, double spanDb, const IlTestGateParams& gate)
{
	if (!(spanDb <= gate.spanMaxDb))
		return FALSE;
	if (currentIlDb < gate.absIlMinDb || currentIlDb > gate.absIlMaxDb)
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
