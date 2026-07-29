#include "stdafx.h"
#include "IlTestCsv.h"
#include "McsFwTransport.h"

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

	BOOL EnsureParentDir(LPCTSTR path, CString& err)
	{
		err.Empty();
		if (!path || path[0] == 0)
			return TRUE;
		const CString full(path);
		const int slash = full.ReverseFind(_T('\\'));
		if (slash <= 0)
			return TRUE;
		const CString dir = full.Left(slash);
		struct _stat sb;
		if (_tstat(dir, &sb) == 0)
			return TRUE;
		if (!CreateDirectory(dir, NULL))
		{
			const DWORD ec = GetLastError();
			if (ec != ERROR_ALREADY_EXISTS)
			{
				err.Format(_T("IL Test CSV: create dir failed: %s (err=%lu)"),
					dir.GetString(), (unsigned long)ec);
				return FALSE;
			}
		}
		return TRUE;
	}
}

CString M576GetIlTestLogCsvPath(LPCTSTR outBaseDir, LPCTSTR mcs1Sn)
{
	CString sn = M576SanitizeSnForFilename(mcs1Sn);
	if (sn.IsEmpty())
		sn = _T("unknown");
	CString leaf;
	leaf.Format(_T("%s_il_test_log.csv"), sn.GetString());
	CString out;
	if (outBaseDir && outBaseDir[0])
		out.Format(_T("%s\\%s"), outBaseDir, leaf.GetString());
	else
		out = leaf;
	return out;
}

CString M576GetIlTestSpanCsvPath(LPCTSTR outBaseDir, LPCTSTR mcs1Sn)
{
	CString sn = M576SanitizeSnForFilename(mcs1Sn);
	if (sn.IsEmpty())
		sn = _T("unknown");
	CString leaf;
	leaf.Format(_T("%s_ILMax-Min_Span.csv"), sn.GetString());
	CString out;
	if (outBaseDir && outBaseDir[0])
		out.Format(_T("%s\\%s"), outBaseDir, leaf.GetString());
	else
		out = leaf;
	return out;
}

BOOL M576WriteIlTestSpanCsv(LPCTSTR path, const std::vector<M576IlTestSpanRow>& rows, CString& err)
{
	err.Empty();
	if (!path || path[0] == 0)
	{
		err = _T("IL Span CSV: empty path.");
		return FALSE;
	}
	if (!EnsureParentDir(path, err))
		return FALSE;

	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("wb")) != 0 || fp == NULL)
	{
		err.Format(_T("IL Span CSV: cannot open %s"), path);
		return FALSE;
	}

	static const unsigned char kBom[] = { 0xEF, 0xBB, 0xBF };
	if (fwrite(kBom, 1, sizeof(kBom), fp) != sizeof(kBom))
	{
		err = _T("IL Span CSV: failed to write BOM.");
		fclose(fp);
		return FALSE;
	}

	static const char kHeader[] =
		"Channel,Half,InPort,OutPort,WlLabel,SampleCount,IL_Max,IL_Min,IL_Span\r\n";
	if (fwrite(kHeader, 1, sizeof(kHeader) - 1, fp) != sizeof(kHeader) - 1)
	{
		err = _T("IL Span CSV: failed to write header.");
		fclose(fp);
		return FALSE;
	}

	for (size_t i = 0; i < rows.size(); ++i)
	{
		const M576IlTestSpanRow& r = rows[i];
		CString line;
		line.Format(
			_T("%s,%s,%s,%s,%s,%d,%.6f,%.6f,%.6f\r\n"),
			r.channel.GetString(),
			r.half.GetString(),
			r.inPort.GetString(),
			r.outPort.GetString(),
			r.wlLabel.GetString(),
			r.sampleCount,
			r.ilMax,
			r.ilMin,
			r.ilSpan);
		const CStringA utf8 = NarrowUtf8(line);
		const size_t n = (size_t)utf8.GetLength();
		if (n > 0 && fwrite(utf8.GetString(), 1, n, fp) != n)
		{
			err = _T("IL Span CSV: write failed.");
			fclose(fp);
			return FALSE;
		}
	}

	fflush(fp);
	fclose(fp);
	return TRUE;
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
			"Time,Lap,Half,Channel,MpoPath,WlLabel,PdRaw,OpmRaw,IL_dB,IlMax,IlMin,IlSpan,Pass,"
			"IlAbsMin_dB,IlAbsMax_dB,SpanMax_dB\r\n";
		if (fwrite(kHeader, 1, sizeof(kHeader) - 1, fp) != (size_t)(sizeof(kHeader) - 1))
		{
			err = _T("IL Test CSV: failed to write header.");
			fclose(fp);
			return FALSE;
		}
	}

	CStringA line;
	line.Format(
		"%s,%d,%s,%s,%s,%s,%d,%d,%.6f,%.6f,%.6f,%.6f,%s,%.4f,%.4f,%.4f\r\n",
		NarrowUtf8(row.timeStamp).GetString(),
		row.lap,
		NarrowUtf8(row.half).GetString(),
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
		row.absIlMinDb,
		row.absIlMaxDb,
		row.spanMaxDb);

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

CString M576GetIlTestCommLogPath(LPCTSTR outBaseDir)
{
	SYSTEMTIME st{};
	::GetLocalTime(&st);
	CString leaf;
	leaf.Format(_T("ILTestCommLog_%04u-%02u-%02u.log"),
		(unsigned)st.wYear, (unsigned)st.wMonth, (unsigned)st.wDay);
	CString out;
	if (outBaseDir && outBaseDir[0])
		out.Format(_T("%s\\%s"), outBaseDir, leaf.GetString());
	else
		out = leaf;
	return out;
}

BOOL M576AppendIlTestCommLogLine(LPCTSTR path, LPCTSTR line, CString& err)
{
	err.Empty();
	if (!path || path[0] == 0)
	{
		err = _T("IL Test CommLog: empty path.");
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
						err.Format(_T("IL Test CommLog: create dir failed: %s (err=%lu)"),
							dir.GetString(), (unsigned long)ec);
						return FALSE;
					}
				}
			}
		}
	}

	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("ab")) != 0 || fp == NULL)
	{
		err.Format(_T("IL Test CommLog: cannot open %s"), path);
		return FALSE;
	}

	const int fh = _fileno(fp);
	const long len = (fh >= 0) ? _filelength(fh) : 0;
	if (len <= 0)
	{
		static const unsigned char kBom[] = { 0xEF, 0xBB, 0xBF };
		if (fwrite(kBom, 1, sizeof(kBom), fp) != sizeof(kBom))
		{
			err = _T("IL Test CommLog: failed to write BOM.");
			fclose(fp);
			return FALSE;
		}
	}

	SYSTEMTIME st{};
	::GetLocalTime(&st);
	CString stamped;
	stamped.Format(_T("%04u-%02u-%02u %02u:%02u:%02u.%03u %s"),
		(unsigned)st.wYear, (unsigned)st.wMonth, (unsigned)st.wDay,
		(unsigned)st.wHour, (unsigned)st.wMinute, (unsigned)st.wSecond,
		(unsigned)st.wMilliseconds,
		line ? line : _T(""));
	const CStringA utf8 = NarrowUtf8(stamped);
	const size_t n = (size_t)utf8.GetLength();
	if (n > 0 && fwrite(utf8.GetString(), 1, n, fp) != n)
	{
		err = _T("IL Test CommLog: write failed.");
		fclose(fp);
		return FALSE;
	}
	static const char kCrLf[] = "\r\n";
	if (fwrite(kCrLf, 1, 2, fp) != 2)
	{
		err = _T("IL Test CommLog: write CRLF failed.");
		fclose(fp);
		return FALSE;
	}
	fflush(fp);
	fclose(fp);
	return TRUE;
}
