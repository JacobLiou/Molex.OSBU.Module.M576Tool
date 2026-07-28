#include "stdafx.h"
#include "M576AppConfig.h"
#include "PeakFinder2D.h"
#include "M576Peak1DConstants.h"

#include <cstdlib>
#include <cmath>

namespace
{
	CString g_configLogLine;
	bool g_sweepYAxisFirst = true;
	CString g_sweepAxisOrderName = _T(M576_PEAK1D_SWEEP_AXIS_ORDER_DEFAULT);
}

static double ClampProminenceDb(double v)
{
	const double lo = (double)M576_PEAK1D_MIN_PROMINENCE_DB_MIN;
	const double hi = (double)M576_PEAK1D_MIN_PROMINENCE_DB_MAX;
	if (!std::isfinite(v))
		return (double)M576_PEAK1D_MIN_PROMINENCE_DB;
	if (v < lo)
		return lo;
	if (v > hi)
		return hi;
	return v;
}

static void ApplySweepAxisOrder(LPCTSTR raw, bool& yFirst, CString& nameOut, bool& fromIni)
{
	yFirst = true;
	nameOut = _T(M576_PEAK1D_SWEEP_AXIS_ORDER_DEFAULT);
	fromIni = false;
	if (raw == nullptr || raw[0] == _T('\0'))
		return;

	CString v(raw);
	v.Trim();
	if (v.CompareNoCase(_T(M576_PEAK1D_SWEEP_AXIS_ORDER_X_THEN_Y)) == 0)
	{
		yFirst = false;
		nameOut = _T(M576_PEAK1D_SWEEP_AXIS_ORDER_X_THEN_Y);
		fromIni = true;
		return;
	}
	if (v.CompareNoCase(_T(M576_PEAK1D_SWEEP_AXIS_ORDER_DEFAULT)) == 0)
	{
		yFirst = true;
		nameOut = _T(M576_PEAK1D_SWEEP_AXIS_ORDER_DEFAULT);
		fromIni = true;
		return;
	}
	// Illegal value ? YThenX default (not counted as ini-applied).
}

void M576LoadAppConfig(LPCTSTR exeDir)
{
	M576::Peak1DResetMinProminenceDb();
	const double kDefault = (double)M576_PEAK1D_MIN_PROMINENCE_DB;
	double applied = kDefault;
	bool promFromIni = false;
	bool orderFromIni = false;
	g_sweepYAxisFirst = true;
	g_sweepAxisOrderName = _T(M576_PEAK1D_SWEEP_AXIS_ORDER_DEFAULT);

	if (exeDir != nullptr && exeDir[0] != _T('\0'))
	{
		CString iniPath = exeDir;
		if (iniPath.Right(1) != _T("\\"))
			iniPath += _T("\\");
		iniPath += _T("M576Calibrator.ini");

		TCHAR buf[64] = {};
		const DWORD n = GetPrivateProfileString(
			_T("PeakFinder"), _T("MinProminenceDb"), _T(""), buf, _countof(buf), iniPath);
		if (n > 0)
		{
			const double parsed = _tstof(buf);
			if (std::isfinite(parsed) && parsed > 0.0)
			{
				applied = ClampProminenceDb(parsed);
				promFromIni = true;
				M576::Peak1DSetMinProminenceDb(applied, true);
			}
		}

		TCHAR orderBuf[64] = {};
		const DWORD nOrder = GetPrivateProfileString(
			_T("PeakFinder"), _T("SweepAxisOrder"), _T(""), orderBuf, _countof(orderBuf), iniPath);
		if (nOrder > 0)
			ApplySweepAxisOrder(orderBuf, g_sweepYAxisFirst, g_sweepAxisOrderName, orderFromIni);
	}

	const CString promSrc = promFromIni ? _T("(ini)") : _T("(default)");
	const CString orderSrc = orderFromIni ? _T("(ini)") : _T("(default)");
	g_configLogLine.Format(
		_T("Config: PeakFinder MinProminenceDb=%.2f %s SweepAxisOrder=%s %s — edit M576Calibrator.ini and restart to change."),
		applied,
		promSrc.GetString(),
		g_sweepAxisOrderName.GetString(),
		orderSrc.GetString());
}

CString M576GetAppConfigLogLine()
{
	return g_configLogLine;
}

bool M576Peak1DSweepYAxisFirst()
{
	return g_sweepYAxisFirst;
}

LPCTSTR M576Peak1DSweepAxisOrderName()
{
	return g_sweepAxisOrderName.GetString();
}
