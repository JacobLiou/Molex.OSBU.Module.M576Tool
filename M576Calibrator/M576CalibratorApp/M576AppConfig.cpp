#include "stdafx.h"
#include "M576AppConfig.h"
#include "PeakFinder2D.h"
#include "M576Peak1DConstants.h"

#include <cstdlib>
#include <cmath>

namespace
{
	CString g_configLogLine;
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

void M576LoadAppConfig(LPCTSTR exeDir)
{
	M576::Peak1DResetMinProminenceDb();
	const double kDefault = (double)M576_PEAK1D_MIN_PROMINENCE_DB;
	double applied = kDefault;
	bool fromIni = false;

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
				fromIni = true;
				M576::Peak1DSetMinProminenceDb(applied, true);
			}
		}
	}

	CString src = fromIni ? _T("(ini)") : _T("(default)");
	g_configLogLine.Format(
		_T("Config: PeakFinder MinProminenceDb=%.2f %s ¡ª edit M576Calibrator.ini and restart to change."),
		applied, src.GetString());
}

CString M576GetAppConfigLogLine()
{
	return g_configLogLine;
}
