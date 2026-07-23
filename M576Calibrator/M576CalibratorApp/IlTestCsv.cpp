#include "stdafx.h"
#include "IlTestCsv.h"

#include <stdio.h>
#include <io.h>
#include <sys/stat.h>

namespace
{
	CStringA NarrowUtf8(const CString& s)
	{
#ifdef _UNICODE
		if (s.IsEmpty())
			return CStringA();
		const int n = ::WideCharToMultiByte(CP_UTF8, 0, s.GetString(), s.GetLength(), NULL, 0, NULL, NULL);
		if (n <= 0)
			return CStringA();
		CStringA out;
		::WideCharToMultiByte(CP_UTF8, 0, s.GetString(), s.GetLength(), out.GetBuffer(n), n, NULL, NULL);
		out.ReleaseBuffer(n);
		return out;
#else
		return CStringA(s);
#endif
	}
}

CString M576GetIlTestLogCsvPath(LPCTSTR outBaseDir)
{
	CString out;
	if (outBaseDir && outBaseDir[0])
		out.Format(_T("%s\\il_test_log.csv"), outBaseDir);
	else
		out = _T("il_test_log.csv");
	return out;
}

BOOL M576AppendIlTestLogRow(LPCTSTR path, const M576IlTestLogRow& row, CString& err)
{
	err.Empty();
	if (!path || path[0] == 0)
	{
		err = _T("IL Test CSV: empty path.");
		return FALSE;
	}
	{
		const CString full(path);
		const int slash = full.ReverseFind(_T('\\'));
		if (slash > 0)
		{
			const CString dir = full.Left(slash);
			struct _stat sb;
			if (_tstat(dir, &sb) != 0)
			{
				if (!CreateDirectory(dir, NULL))
				{
					const DWORD ec = GetLastError();
					if (ec != ERROR_ALREADY_EXISTS)
					{
						err.Format(_T("IL Test CSV: create dir failed: %s (err=%lu)"), dir.GetString(), (unsigned long)ec);
						return FALSE;
					}
				}
			}
		}
	}

	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("ab")) != 0 || fp == NULL)
	{
		err.Format(_T("IL Test CSV: cannot open %s"), path);
		return FALSE;
	}

	const int fh = _fileno(fp);
	const long len = (fh >= 0) ? _filelength(fh) : 0;
	if (len <= 0)
	{
		static const char kHeader[] =
			"Time,Lap,Channel,MpoPath,WlLabel,PdRaw,OpmRaw,IL_dB,IlMax,IlMin,IlSpan,Pass,"
			"SpanMax_dB,IlAbsMin_dB,IlAbsMax_dB\r\n";
		if (fwrite(kHeader, 1, sizeof(kHeader) - 1, fp) != (size_t)(sizeof(kHeader) - 1))
		{
			err = _T("IL Test CSV: failed to write header.");
			fclose(fp);
			return FALSE;
		}
	}

	CStringA line;
	line.Format(
		"%s,%d,%s,%s,%s,%d,%d,%.6f,%.6f,%.6f,%.6f,%s,%.4f,%.4f,%.4f\r\n",
		NarrowUtf8(row.timeStamp).GetString(),
		row.lap,
		NarrowUtf8(row.channel).GetString(),
		NarrowUtf8(row.mpoPath).GetString(),
		NarrowUtf8(row.wlLabel).GetString(),
		row.pdRaw,
		row.opmRaw,
		row.ilDb,
		row.ilMax,
		row.ilMin,
		row.ilSpan,
		row.pass ? "PASS" : "FAIL",
		row.spanMaxDb,
		row.absIlMinDb,
		row.absIlMaxDb);

	const size_t n = (size_t)line.GetLength();
	if (fwrite(line.GetString(), 1, n, fp) != n)
	{
		err = _T("IL Test CSV: write failed.");
		fclose(fp);
		return FALSE;
	}
	fflush(fp);
	fclose(fp);
	return TRUE;
}
