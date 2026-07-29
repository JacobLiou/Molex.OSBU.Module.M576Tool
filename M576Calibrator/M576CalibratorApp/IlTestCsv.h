#pragma once

#include <afxstr.h>
#include <vector>

struct M576IlTestLogRow
{
	CString timeStamp;
	int lap = 0;
	CString half; // IN / OUT (SW4=1 / SW4=2)
	CString channel;
	CString mpoPath;
	CString wlLabel;
	int pdRaw = 0;
	int opmRaw = 0;
	double ilDb = 0.0;
	double ilMax = 0.0;
	double ilMin = 0.0;
	double ilSpan = 0.0; // rolling Max-Min across same channel+wl+half
	BOOL pass = FALSE;
	double absIlMinDb = -5.0;
	double absIlMaxDb = 5.0;
	double spanMaxDb = 0.15;
};

/// One row of `{SN}_ILMax-Min_Span.csv` (final per-channel×half summary for trend charts).
struct M576IlTestSpanRow
{
	CString channel;
	CString half; // IN / OUT
	CString inPort;
	CString outPort;
	CString wlLabel;
	int sampleCount = 0;
	double ilMax = 0.0;
	double ilMin = 0.0;
	double ilSpan = 0.0;
};

/// `{outBaseDir}\{MCS1_SN}_il_test_log.csv` (empty SN -> `unknown`).
CString M576GetIlTestLogCsvPath(LPCTSTR outBaseDir, LPCTSTR mcs1Sn);

/// `{outBaseDir}\{MCS1_SN}_ILMax-Min_Span.csv` (empty SN -> `unknown`).
CString M576GetIlTestSpanCsvPath(LPCTSTR outBaseDir, LPCTSTR mcs1Sn);

/// `{outBaseDir}\ILTestCommLog_YYYY-MM-DD.log` (local calendar day; hang-up DIAG/SEND/RECV only).
CString M576GetIlTestCommLogPath(LPCTSTR outBaseDir);

/// Append one IL Test row; writes UTF-8 header if file empty. fflush each row.
BOOL M576AppendIlTestLogRow(LPCTSTR path, const M576IlTestLogRow& row, CString& err);

/// Overwrite Span summary CSV (UTF-8 BOM + header). One row per channel+wl with samples.
BOOL M576WriteIlTestSpanCsv(LPCTSTR path, const std::vector<M576IlTestSpanRow>& rows, CString& err);

/// Append one UTF-8 line to IL Test comm log (creates file + BOM if new). Thread-safe enough via open/append/close.
BOOL M576AppendIlTestCommLogLine(LPCTSTR path, LPCTSTR line, CString& err);
