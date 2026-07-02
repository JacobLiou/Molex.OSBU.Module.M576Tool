#include "stdafx.h"
#include "M576RecalSweepCsv.h"
#include "M576OutputArchive.h"
#include <cmath>
#include <mutex>

namespace
{
	std::mutex g_csvMu;
	BOOL g_active = FALSE;
	BOOL g_isPm = TRUE;
	CString g_csvAbsPath;
	CString g_pathColumn;
	int g_maxPowerColsWritten = 0;

	static CString EscapeCsvField(const CString& s)
	{
		bool needQuote = false;
		for (int i = 0; i < s.GetLength(); ++i)
		{
			const TCHAR c = s[i];
			if (c == _T(',') || c == _T('"') || c == _T('\r') || c == _T('\n'))
			{
				needQuote = true;
				break;
			}
		}
		if (!needQuote)
			return s;
		CString o = _T("\"");
		for (int i = 0; i < s.GetLength(); ++i)
		{
			const TCHAR c = s[i];
			if (c == _T('"'))
				o += _T("\"\"");
			else
				o += c;
		}
		o += _T('"');
		return o;
	}

	static CString IntToCsvField(long long v)
	{
		CString s;
		s.Format(_T("%lld"), v);
		return s;
	}

	static void AppendUtf8BomIfEmpty(HANDLE h)
	{
		LARGE_INTEGER size = {};
		if (!GetFileSizeEx(h, &size) || size.QuadPart != 0)
			return;
		const BYTE bom[] = { 0xEF, 0xBB, 0xBF };
		DWORD wr = 0;
		WriteFile(h, bom, sizeof(bom), &wr, NULL);
	}

	static BOOL WriteUtf8Line(HANDLE h, const CString& line)
	{
       CStringW w;
#ifdef _UNICODE
		w = line;
#else
       w = CStringW(CA2W(line.GetString(), CP_ACP));
#endif
		w += L"\r\n";
		int n8 = WideCharToMultiByte(CP_UTF8, 0, w, w.GetLength(), NULL, 0, NULL, NULL);
		if (n8 < 1)
			return FALSE;
		CStringA utf8;
		LPSTR buf = utf8.GetBuffer(n8);
		(void)WideCharToMultiByte(CP_UTF8, 0, w, w.GetLength(), buf, n8, NULL, NULL);
		utf8.ReleaseBuffer(n8);
		DWORD written = 0;
		return WriteFile(h, utf8.GetString(), (DWORD)utf8.GetLength(), &written, NULL) != FALSE;
	}

	static CString BuildHeaderRow()
	{
		CString hdr = _T("path,cmd,attempt,peak_ok,code");
		for (int i = 0; i < M576_RECAL_SWEEP_CSV_MAX_POWER_COLS; ++i)
		{
			CString c;
			c.Format(_T(",c%d"), i);
			hdr += c;
		}
		return hdr;
	}

	static BOOL FileNeedsHeader(LPCTSTR absPath)
	{
		const DWORD attr = GetFileAttributes(absPath);
		if (attr == INVALID_FILE_ATTRIBUTES)
			return TRUE;
		HANDLE h = CreateFile(absPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h == INVALID_HANDLE_VALUE)
			return TRUE;
		LARGE_INTEGER size = {};
		const BOOL ok = GetFileSizeEx(h, &size);
		CloseHandle(h);
		return !ok || size.QuadPart == 0;
	}

	static BOOL AppendRowLocked(
		const CString& pathCol,
		const CString& cmdWire,
		long long col0Int,
		const std::vector<double>& powers,
		int attempt1Based,
		BOOL peakOk,
		const char* codeName)
	{
		if (!g_active || g_csvAbsPath.IsEmpty())
			return FALSE;

		const int nPow = (int)powers.size();
		const int totalPowCols = 1 + nPow;
		if (totalPowCols > g_maxPowerColsWritten)
			g_maxPowerColsWritten = totalPowCols;

		CString row;
		row += EscapeCsvField(pathCol);
		row += _T(",");
		row += EscapeCsvField(cmdWire);
		row += _T(",");
		row += IntToCsvField((long long)attempt1Based);
		row += _T(",");
		row += peakOk ? _T("1") : _T("0");
		row += _T(",");
		row += CString(CA2T(codeName, CP_UTF8));

		row += _T(",");
		row += IntToCsvField(col0Int);
		for (int i = 0; i < nPow; ++i)
		{
			row += _T(",");
			const long long pi = (long long)std::llround(powers[(size_t)i]);
			row += IntToCsvField(pi);
		}
		for (int i = totalPowCols; i < M576_RECAL_SWEEP_CSV_MAX_POWER_COLS; ++i)
			row += _T(",");

		HANDLE h = CreateFile(
			g_csvAbsPath,
			FILE_APPEND_DATA,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (h == INVALID_HANDLE_VALUE)
			return FALSE;

		const BOOL needHdr = FileNeedsHeader(g_csvAbsPath);
		AppendUtf8BomIfEmpty(h);
		if (needHdr)
			WriteUtf8Line(h, BuildHeaderRow());
		const BOOL ok = WriteUtf8Line(h, row);
		FlushFileBuffers(h);
		CloseHandle(h);
		return ok;
	}
}

CString M576RecalSweepCsvRelPathForCurrentDay(LPCTSTR commLogPathRel)
{
	if (commLogPathRel == NULL || commLogPathRel[0] == 0)
		return CString();
	int sep = -1;
	for (int i = 0; commLogPathRel[i] != 0; ++i)
	{
		if (commLogPathRel[i] == _T('\\') || commLogPathRel[i] == _T('/'))
			sep = i;
	}
	CString dir, fname;
	if (sep >= 0)
	{
		dir = commLogPathRel;
		dir = dir.Left(sep);
		fname = commLogPathRel + sep + 1;
	}
	else
		fname = commLogPathRel;

	int dot = fname.ReverseFind(_T('.'));
	CString stem, ext;
	if (dot > 0)
	{
		stem = fname.Left(dot);
		ext = fname.Mid(dot);
	}
	else
	{
		stem = fname;
		ext = _T(".log");
	}
	(void)ext;
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	CString day;
	day.Format(_T("%04d-%02d-%02d"), (int)st.wYear, (int)st.wMonth, (int)st.wDay);
	CString rel;
	if (dir.IsEmpty())
		rel.Format(_T("%s_%s_recal_sweeps.csv"), stem.GetString(), day.GetString());
	else
		rel.Format(_T("%s\\%s_%s_recal_sweeps.csv"), dir.GetString(), stem.GetString(), day.GetString());
	return rel;
}

const char* M576Peak1DValidateCodeShortName(M576::Peak1DValidateCode c)
{
	using M576::Peak1DValidateCode;
	switch (c)
	{
	case Peak1DValidateCode::Ok: return "Ok";
	case Peak1DValidateCode::Empty: return "Empty";
	case Peak1DValidateCode::LowSpan: return "LowSpan";
	case Peak1DValidateCode::NotStrictLocal: return "NotStrictLocal";
	case Peak1DValidateCode::EdgeNotAllowed: return "EdgeNotAllowed";
	case Peak1DValidateCode::MultiLocalMax: return "MultiLocalMax";
	case Peak1DValidateCode::NotEnoughSamples: return "NotEnoughSamples";
	case Peak1DValidateCode::NotEnoughValidSamples: return "NotEnoughValidSamples";
	case Peak1DValidateCode::ParabolaNotDownward: return "ParabolaNotDownward";
	case Peak1DValidateCode::ParabolaFitSingular: return "ParabolaFitSingular";
	case Peak1DValidateCode::VertexOutOfRange: return "VertexOutOfRange";
	case Peak1DValidateCode::PmRangeMismatch: return "PmRangeMismatch";
	default: return "?";
	}
}

BOOL M576RecalSweepCsvBeginRun(LPCTSTR sweepCsvAbsPath, BOOL isPm)
{
	std::lock_guard<std::mutex> lock(g_csvMu);
	g_active = FALSE;
	g_pathColumn.Empty();
	g_maxPowerColsWritten = 0;
	g_isPm = isPm;
	g_csvAbsPath.Empty();
	if (sweepCsvAbsPath == NULL || sweepCsvAbsPath[0] == 0)
		return FALSE;
	int slash = -1;
	for (int i = 0; sweepCsvAbsPath[i] != 0; ++i)
	{
		if (sweepCsvAbsPath[i] == _T('\\') || sweepCsvAbsPath[i] == _T('/'))
			slash = i;
	}
	if (slash >= 0)
	{
		CString dir = sweepCsvAbsPath;
		dir = dir.Left(slash);
		CString err;
		if (!M576EnsureDirTree(dir, err))
			return FALSE;
	}
	g_csvAbsPath = sweepCsvAbsPath;
	g_active = TRUE;
	return TRUE;
}

void M576RecalSweepCsvSetStepContext(LPCTSTR pathColumn)
{
	std::lock_guard<std::mutex> lock(g_csvMu);
	if (pathColumn != NULL)
		g_pathColumn = pathColumn;
	else
		g_pathColumn.Empty();
}

void M576RecalSweepCsvAppendRow(
	LPCTSTR cmdWire,
	double col0,
	const std::vector<double>& powers,
	int attempt1Based,
	BOOL peakOk,
	M576::Peak1DValidateCode code)
{
	std::lock_guard<std::mutex> lock(g_csvMu);
	if (!g_active)
		return;
	const CString cmd = (cmdWire != NULL) ? cmdWire : _T("");
	const long long col0Int = (long long)std::llround(col0);
	const char* codeName = M576Peak1DValidateCodeShortName(code);
	(void)AppendRowLocked(g_pathColumn, cmd, col0Int, powers, attempt1Based, peakOk, codeName);
}

void M576RecalSweepCsvEndRun()
{
	std::lock_guard<std::mutex> lock(g_csvMu);
	g_active = FALSE;
	g_pathColumn.Empty();
}

CString M576RecalSweepCsvGetActiveAbsPath()
{
	std::lock_guard<std::mutex> lock(g_csvMu);
	return g_csvAbsPath;
}
