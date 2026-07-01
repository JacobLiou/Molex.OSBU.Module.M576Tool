#pragma once

#include <afxstr.h>

/// Load M576Calibrator.ini from exeDir; applies PeakFinder runtime knobs. Call once at startup.
void M576LoadAppConfig(LPCTSTR exeDir);

/// Human-readable line for UI log, e.g. "Config: PeakFinder MinProminenceDb=0.30 (ini)".
CString M576GetAppConfigLogLine();
