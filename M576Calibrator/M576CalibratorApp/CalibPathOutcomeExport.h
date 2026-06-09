#pragma once

#include "CalibPathOutcome.h"

BOOL WriteRunPathFailureCsv(LPCTSTR path, const SRunPathSummary& summary, CString& err);
