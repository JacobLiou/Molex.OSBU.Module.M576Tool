#include "stdafx.h"
#include "DiagnosisCsv.h"

#include <stdio.h>
#include <io.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace
{
	void StripUtf8Bom(CStringA& s)
	{
		if (s.GetLength() >= 3
			&& (BYTE)s[0] == 0xEF
			&& (BYTE)s[1] == 0xBB
			&& (BYTE)s[2] == 0xBF)
		{
			s.Delete(0, 3);
		}
	}

	CStringA RfcQuote(const CStringA& field)
	{
		BOOL needsQuote = FALSE;
		for (int i = 0; i < field.GetLength(); ++i)
		{
			const char c = field[i];
			if (c == ',' || c == '"' || c == '\r' || c == '\n')
			{
				needsQuote = TRUE;
				break;
			}
		}
		if (!needsQuote)
			return field;
		CStringA out;
		out.Preallocate(field.GetLength() + 8);
		out += '"';
		for (int i = 0; i < field.GetLength(); ++i)
		{
			const char c = field[i];
			if (c == '"')
				out += "\"\"";
			else
				out += c;
		}
		out += '"';
		return out;
	}

	void SplitPipe(const CStringA& line, std::vector<CStringA>& tokens)
	{
		tokens.clear();
		int start = 0;
		const int L = line.GetLength();
		while (start <= L)
		{
			int p = line.Find('|', start);
			CStringA tok = (p < 0) ? line.Mid(start) : line.Mid(start, p - start);
			tok.Trim();
			if (!tok.IsEmpty())
				tokens.push_back(tok);
			if (p < 0)
				break;
			start = p + 1;
		}
	}

	BOOL TokenStartsWithSw(const CStringA& tok)
	{
		return tok.GetLength() >= 2 && tok.Left(2).CompareNoCase("SW") == 0;
	}
}

BOOL M576LoadDiagnosisSwCsv(LPCTSTR path, std::vector<M576DiagnosisRow>& rows, CString& err)
{
	rows.clear();
	err.Empty();
	if (!path || path[0] == 0)
	{
		err = _T("Diagnosis CSV: empty path.");
		return FALSE;
	}

	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("rb")) != 0 || fp == NULL)
	{
		err.Format(_T("Cannot open diagnosis CSV: %s"), path);
		return FALSE;
	}

	CStringA buf;
	{
		char chunk[4096];
		size_t got = 0;
		while ((got = fread(chunk, 1, sizeof(chunk), fp)) > 0)
			buf.Append(chunk, (int)got);
	}
	fclose(fp);

	StripUtf8Bom(buf);

	CStringA pendingLabel;
	int start = 0;
	const int L = buf.GetLength();
	while (start <= L)
	{
		int p = start;
		while (p < L && buf[p] != '\n' && buf[p] != '\r')
			++p;
		CStringA line = buf.Mid(start, p - start);
		if (p < L && buf[p] == '\r' && (p + 1) < L && buf[p + 1] == '\n')
			start = p + 2;
		else if (p < L)
			start = p + 1;
		else
			start = L + 1;

		line.Trim();
		if (line.IsEmpty())
			continue;
		if (line[0] == '#' || line[0] == ';')
		{
			CStringA label = line.Mid(1);
			label.Trim();
			label.TrimLeft("#;*= \t");
			label.TrimRight("#;*= \t");
			pendingLabel = label;
			continue;
		}

		std::vector<CStringA> tokens;
		SplitPipe(line, tokens);
		if (tokens.empty())
			continue;

		M576DiagnosisRow r;
		r.label = pendingLabel;
		if (TokenStartsWithSw(tokens[0]))
		{
			r.channel.Empty();
			r.swCommands.swap(tokens);
		}
		else
		{
			r.channel = tokens[0];
			std::vector<CStringA> swTok;
			swTok.reserve(tokens.size() > 0 ? (size_t)tokens.size() - 1 : 0);
			for (size_t ti = 1; ti < tokens.size(); ++ti)
				swTok.push_back(tokens[ti]);
			r.swCommands.swap(swTok);
		}
		if (r.swCommands.empty())
			continue;
		rows.push_back(r);
		pendingLabel.Empty();
	}

	if (rows.empty())
	{
		err.Format(_T("Diagnosis CSV has no data rows: %s"), path);
		return FALSE;
	}
	return TRUE;
}

CString M576GetDiagnosisUnifiedLogCsvPath(LPCTSTR outBaseDir)
{
	CString out;
	if (outBaseDir && outBaseDir[0])
		out.Format(_T("%s\\diagnosis_log.csv"), outBaseDir);
	else
		out = _T("diagnosis_log.csv");
	return out;
}

BOOL M576AppendDiagnosisPythonRow(LPCTSTR path, const CStringA& channel, const M576DiagnosisWlScenarioResult wlScen[3], CString& err)
{
	err.Empty();
	if (!path || path[0] == 0)
	{
		err = _T("Diagnosis append CSV: empty path.");
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
						err.Format(_T("Create dir failed: %s (err=%lu)"), dir.GetString(), (unsigned long)ec);
						return FALSE;
					}
				}
			}
		}
	}

	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("ab")) != 0 || fp == NULL)
	{
		err.Format(_T("Cannot open diagnosis log CSV for append: %s"), path);
		return FALSE;
	}

	const int fh = _fileno(fp);
	const long len = (fh >= 0) ? _filelength(fh) : 0;
	if (len <= 0)
	{
		static const char kHeader[] =
			"Channel,s1_pd_reply,s1_opm_reply,s2_pd_reply,s2_opm_reply,s3_pd_reply,s3_opm_reply\r\n";
		if (fwrite(kHeader, 1, sizeof(kHeader) - 1, fp) != (size_t)(sizeof(kHeader) - 1))
		{
			err = _T("Diagnosis append CSV: failed to write header.");
			fclose(fp);
			return FALSE;
		}
	}

	CStringA line;
	line.Format(
		"%s,%s,%s,%s,%s,%s,%s\r\n",
		RfcQuote(channel).GetString(),
		RfcQuote(wlScen[0].pdReply).GetString(),
		RfcQuote(wlScen[0].opmReply).GetString(),
		RfcQuote(wlScen[1].pdReply).GetString(),
		RfcQuote(wlScen[1].opmReply).GetString(),
		RfcQuote(wlScen[2].pdReply).GetString(),
		RfcQuote(wlScen[2].opmReply).GetString());
	if (fwrite(line.GetString(), 1, line.GetLength(), fp) != (size_t)line.GetLength())
	{
		err = _T("Diagnosis append CSV: failed to write data row.");
		fclose(fp);
		return FALSE;
	}
	fflush(fp);
	fclose(fp);
	return TRUE;
}

CString M576MakeDiagnosisResultCsvPath(LPCTSTR outBaseDir)
{
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	CString ts;
	ts.Format(_T("%04u%02u%02u_%02u%02u%02u"),
		(unsigned)st.wYear, (unsigned)st.wMonth, (unsigned)st.wDay,
		(unsigned)st.wHour, (unsigned)st.wMinute, (unsigned)st.wSecond);
	CString out;
	if (outBaseDir && outBaseDir[0])
		out.Format(_T("%s\\diagnosis_%s.csv"), outBaseDir, ts.GetString());
	else
		out.Format(_T("diagnosis_%s.csv"), ts.GetString());
	return out;
}

BOOL M576WriteDiagnosisResultCsv(LPCTSTR path, const std::vector<M576DiagnosisResultRow>& rows, CString& err)
{
	err.Empty();
	if (!path || path[0] == 0)
	{
		err = _T("Diagnosis result CSV: empty path.");
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
						err.Format(_T("Create dir failed: %s (err=%lu)"), dir.GetString(), (unsigned long)ec);
						return FALSE;
					}
				}
			}
		}
	}

	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("wb")) != 0 || fp == NULL)
	{
		err.Format(_T("Cannot open diagnosis result CSV for write: %s"), path);
		return FALSE;
	}

	const char* header =
		"step,label,sw_count,sw_ok_count,sw_cmds,sw_replies,"
		"s1_wl1550_reply,s1_sw3_reply,s1_wl1310_reply,s1_pd_reply,s1_opm_reply,"
		"s2_wl1550_reply,s2_sw3_reply,s2_wl1310_reply,s2_pd_reply,s2_opm_reply,"
		"s3_wl1550_reply,s3_sw3_reply,s3_wl1310_reply,s3_pd_reply,s3_opm_reply,"
		"total_ms\r\n";
	fwrite(header, 1, strlen(header), fp);

	for (size_t i = 0; i < rows.size(); ++i)
	{
		const M576DiagnosisResultRow& r = rows[i];
		CStringA stepStr, swCount, swOkCount, totalMs;
		stepStr.Format("%d", r.step);
		swCount.Format("%d", r.swCount);
		swOkCount.Format("%d", r.swOkCount);
		totalMs.Format("%lu", (unsigned long)r.totalMs);
		CStringA line;
		line.Format(
			"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\r\n",
			stepStr.GetString(),
			RfcQuote(r.label).GetString(),
			swCount.GetString(),
			swOkCount.GetString(),
			RfcQuote(r.swCmds).GetString(),
			RfcQuote(r.swReplies).GetString(),
			RfcQuote(r.wlScen[0].wl1550Reply).GetString(),
			RfcQuote(r.wlScen[0].sw3Reply).GetString(),
			RfcQuote(r.wlScen[0].wl1310Reply).GetString(),
			RfcQuote(r.wlScen[0].pdReply).GetString(),
			RfcQuote(r.wlScen[0].opmReply).GetString(),
			RfcQuote(r.wlScen[1].wl1550Reply).GetString(),
			RfcQuote(r.wlScen[1].sw3Reply).GetString(),
			RfcQuote(r.wlScen[1].wl1310Reply).GetString(),
			RfcQuote(r.wlScen[1].pdReply).GetString(),
			RfcQuote(r.wlScen[1].opmReply).GetString(),
			RfcQuote(r.wlScen[2].wl1550Reply).GetString(),
			RfcQuote(r.wlScen[2].sw3Reply).GetString(),
			RfcQuote(r.wlScen[2].wl1310Reply).GetString(),
			RfcQuote(r.wlScen[2].pdReply).GetString(),
			RfcQuote(r.wlScen[2].opmReply).GetString(),
			totalMs.GetString());
		fwrite(line.GetString(), 1, line.GetLength(), fp);
	}
	fclose(fp);
	return TRUE;
}
