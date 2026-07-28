#pragma once

#include <afxstr.h>

/// Load M576Calibrator.ini from exeDir; applies PeakFinder runtime knobs. Call once at startup.
void M576LoadAppConfig(LPCTSTR exeDir);

/// Human-readable line for UI log (MinProminenceDb + SweepAxisOrder).
CString M576GetAppConfigLogLine();

/// true = YThenX (mode0 then mode1, production default); false = XThenY (mode1 then mode0).
bool M576Peak1DSweepYAxisFirst();

/// "YThenX" or "XThenY" for logs.
LPCTSTR M576Peak1DSweepAxisOrderName();
