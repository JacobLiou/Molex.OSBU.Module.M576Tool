#pragma once

#include <afxstr.h>

struct M576IlTestLogRow
{
	CString timeStamp;
	int lap = 0;
	CString channel;
	CString mpoPath;
	CString wlLabel;
	int pdRaw = 0;
	int opmRaw = 0;
	double ilDb = 0.0;
	double ilMax = 0.0;
	double ilMin = 0.0;
	double ilSpan = 0.0;
	BOOL pass = FALSE;
	double spanMaxDb = 0.15;
	double absIlMinDb = 0.0;
	double absIlMaxDb = 3.0;
};

/// `{outBaseDir}\il_test_log.csv`
CString M576GetIlTestLogCsvPath(LPCTSTR outBaseDir);

/// Append one IL Test row; writes UTF-8 header if file empty. fflush each row.
BOOL M576AppendIlTestLogRow(LPCTSTR path, const M576IlTestLogRow& row, CString& err);
