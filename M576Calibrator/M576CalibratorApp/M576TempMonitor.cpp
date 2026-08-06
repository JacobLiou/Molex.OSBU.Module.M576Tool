#include "stdafx.h"
#include "M576TempMonitor.h"
#include "Board439fTransTunnel.h"
#include "CalibConstants.h"

#include <cstdlib>
#include <cstring>

namespace
{
	static CString FormatOneTemp(bool has, double c)
	{
		if (!has || !std::isfinite(c))
			return _T("--.-");
		CString s;
		s.Format(_T("%.1f"), c);
		return s;
	}

	static BOOL ParseMtReply(const CStringA& reply, double& outC, CString& err)
	{
		err.Empty();
		const char* p = reply.GetString();
		while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'))
			++p;
		if (!*p)
		{
			err = _T("MT: empty reply.");
			return FALSE;
		}
		char* end = nullptr;
		const long v = strtol(p, &end, 10);
		if (end == p)
		{
			err.Format(_T("MT: cannot parse integer from '%s'."), CString(reply).GetString());
			return FALSE;
		}
		outC = (double)v / 10.0;
		return TRUE;
	}

	static bool ReplyLooksInvalidCommand(const CStringA& reply)
	{
		CStringA low(reply);
		low.MakeLower();
		return low.Find("invalid") >= 0;
	}

	/// 1x64 Switch GetModuleTemp: ASCII `GTMP\r`, reply /100 °C.
	static BOOL ParseGtmpReply(const CStringA& reply, double& outC, CString& err)
	{
		err.Empty();
		if (ReplyLooksInvalidCommand(reply))
		{
			err.Format(_T("GTMP: Invalid command (%s)."), CString(reply).GetString());
			return FALSE;
		}
		const char* p = reply.GetString();
		while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'))
			++p;
		if (!*p)
		{
			err = _T("GTMP: empty reply.");
			return FALSE;
		}
		char* end = nullptr;
		const double v = strtod(p, &end);
		if (end == p)
		{
			err.Format(_T("GTMP: cannot parse from '%s'."), CString(reply).GetString());
			return FALSE;
		}
		outC = v / 100.0;
		return TRUE;
	}

	static BOOL Read1x64GtmpOnTunnel(Z4671Command& cmd, double& boxC, CString& err)
	{
		err.Empty();
		boxC = NAN;
		char wire[] = "GTMP\r";
		if (!cmd.WriteBufferNoPurge(wire, (DWORD)strlen(wire)))
		{
			err = _T("GTMP: write failed.");
			return FALSE;
		}
		Sleep(50);
		BYTE buf[256] = {};
		DWORD n = 0;
		if (!cmd.ReadBuffer((char*)buf, sizeof(buf), &n) || n == 0)
		{
			err = _T("GTMP: read failed or empty.");
			return FALSE;
		}
		buf[sizeof(buf) - 1] = 0;
		CStringA reply((const char*)buf, (int)strnlen((const char*)buf, n));
		cmd.TraceInfo(_T("TEMP"), _T("GTMP reply: %s"), CString(reply).GetString());
		return ParseGtmpReply(reply, boxC, err);
	}

	static BOOL ReadMcsTempOnTunnel(Z4671Command& cmd, double& boxC, CString& err)
	{
		err.Empty();
		boxC = NAN;
		double t = 0.0;
		if (!cmd.GetMCSTemp(&t))
		{
			err = _T("MCS temp: GetMCSTemp failed.");
			if (!cmd.m_strLogInfo.IsEmpty())
			{
				err += _T(" ");
				err += cmd.m_strLogInfo;
				if (ReplyLooksInvalidCommand(CStringA(cmd.m_strLogInfo)))
					err += _T(" (Invalid command)");
			}
			return FALSE;
		}
		boxC = t;
		return TRUE;
	}
}

BOOL M576ReadMainBoardTempC(Z4671Command& cmd, double& outC, CString& err)
{
	err.Empty();
	outC = NAN;
	// Leave any stale tunnel.
	CString discard;
	(void)Board439fTransTunnel::EndTrans(cmd, discard);

	char wire[] = "MT\r";
	if (!cmd.WriteBufferNoPurge(wire, (DWORD)strlen(wire)))
	{
		err = _T("MT: write failed.");
		return FALSE;
	}
	Sleep(50);
	BYTE buf[256] = {};
	DWORD n = 0;
	if (!cmd.ReadBuffer((char*)buf, sizeof(buf), &n) || n == 0)
	{
		err = _T("MT: read failed or empty.");
		return FALSE;
	}
	buf[sizeof(buf) - 1] = 0;
	CStringA reply((const char*)buf, (int)strnlen((const char*)buf, n));
	cmd.TraceInfo(_T("TEMP"), _T("MT reply: %s"), CString(reply).GetString());
	return ParseMtReply(reply, outC, err);
}

BOOL M576ReadSubBoardBoxTempC(
	Z4671Command& cmd,
	int trans1to4,
	double& boxC,
	double& edfaOut1C,
	CString& err)
{
	err.Empty();
	boxC = NAN;
	edfaOut1C = NAN;
	if (trans1to4 < 1 || trans1to4 > 4)
	{
		err.Format(_T("sub temp: trans %d out of range 1..4."), trans1to4);
		return FALSE;
	}

	CString lastErr;
	const int maxAttempts = (int)M576_TEMP_SUB_RETRY_MAX;
	for (int attempt = 1; attempt <= maxAttempts; ++attempt)
	{
		boxC = NAN;
		CString attemptErr;

		// Clear stale tunnel / ASCII echo before each try (439F sometimes replies Invalid command).
		CString discard;
		(void)Board439fTransTunnel::EndTrans(cmd, discard);

		if (!Board439fTransTunnel::BeginTrans(cmd, trans1to4, attemptErr))
		{
			lastErr = attemptErr;
			cmd.TraceError(
				_T("TEMP"),
				_T("trans %d begin failed attempt %d/%d: %s"),
				trans1to4,
				attempt,
				maxAttempts,
				attemptErr.GetString());
			if (attempt < maxAttempts)
				Sleep(M576_TEMP_SUB_RETRY_DELAY_MS);
			continue;
		}

		BOOL ok = FALSE;
		if (trans1to4 <= 2)
			ok = ReadMcsTempOnTunnel(cmd, boxC, attemptErr);
		else
			ok = Read1x64GtmpOnTunnel(cmd, boxC, attemptErr);

		CString endErr;
		const BOOL endOk = Board439fTransTunnel::EndTrans(cmd, endErr);
		if (!endOk)
		{
			if (attemptErr.IsEmpty())
				attemptErr = endErr;
			else if (!endErr.IsEmpty())
				attemptErr += _T("; ") + endErr;
			ok = FALSE;
		}

		if (ok)
		{
			if (attempt > 1)
			{
				cmd.TraceInfo(
					_T("TEMP"),
					_T("trans %d temp OK on attempt %d/%d"),
					trans1to4,
					attempt,
					maxAttempts);
			}
			err.Empty();
			return TRUE;
		}

		lastErr = attemptErr;
		cmd.TraceError(
			_T("TEMP"),
			_T("trans %d temp failed attempt %d/%d: %s"),
			trans1to4,
			attempt,
			maxAttempts,
			attemptErr.GetString());
		if (attempt < maxAttempts)
			Sleep(M576_TEMP_SUB_RETRY_DELAY_MS);
	}

	err = lastErr;
	if (err.IsEmpty())
		err.Format(_T("sub temp: failed after %d attempts."), maxAttempts);
	return FALSE;
}

BOOL M576ReadAllFiveTempsC(Z4671Command& cmd, M576FiveTemps& out, CString& errDetail)
{
	errDetail.Empty();
	out = M576FiveTemps{};

	// Main MT calls EndTrans ($$) first; disabled by default (interferes with IL/Diagnosis ASCII).
#if M576_TEMP_READ_MAIN_BOARD
	CString err;
	double mainC = NAN;
	if (M576ReadMainBoardTempC(cmd, mainC, err))
	{
		out.mainC = mainC;
		out.hasMain = true;
	}
	else
	{
		errDetail = err;
		cmd.TraceError(_T("TEMP"), _T("Main MT failed: %s"), err.GetString());
	}
#else
	(void)cmd;
#endif

	// Sub-board temps use trans 1..4; disabled by default (interferes with IL/Diagnosis ASCII).
#if M576_TEMP_READ_SUB_BOARDS
	static const LPCTSTR kSubNames[4] = {
		_T("MCS1"), _T("MCS2"), _T("1x64#1"), _T("1x64#2")
	};
	for (int i = 0; i < 4; ++i)
	{
		double box = NAN;
		double edfa = NAN;
		CString e;
		if (M576ReadSubBoardBoxTempC(cmd, i + 1, box, edfa, e))
		{
			out.subC[i] = box;
			out.hasSub[i] = true;
			if (std::isfinite(edfa))
			{
				out.edfaOut1C[i] = edfa;
				out.hasEdfa[i] = true;
			}
		}
		else
		{
			CString one;
			one.Format(_T("%s (trans %d): %s"), kSubNames[i], i + 1, e.GetString());
			if (!errDetail.IsEmpty())
				errDetail += _T("; ");
			errDetail += one;
			cmd.TraceError(_T("TEMP"), _T("%s"), one.GetString());
		}
	}
#endif
	return out.hasMain || out.hasSub[0] || out.hasSub[1] || out.hasSub[2] || out.hasSub[3];
}

CString M576FormatFiveTempsUiLine(const M576FiveTemps& t)
{
	CString s;
	s.Format(
		_T("Temp: Main %s  MCS1 %s  MCS2 %s  1x64#1 %s  1x64#2 %s C"),
		FormatOneTemp(t.hasMain, t.mainC).GetString(),
		FormatOneTemp(t.hasSub[0], t.subC[0]).GetString(),
		FormatOneTemp(t.hasSub[1], t.subC[1]).GetString(),
		FormatOneTemp(t.hasSub[2], t.subC[2]).GetString(),
		FormatOneTemp(t.hasSub[3], t.subC[3]).GetString());
	return s;
}

void M576AppendFiveTempsLogLines(const M576FiveTemps& t, CStringArray& outLines)
{
	CString mainLine;
	mainLine.Format(
		_T("[TEMP] Main=%sC MCS1=%sC MCS2=%sC 1x64#1=%sC 1x64#2=%sC"),
		FormatOneTemp(t.hasMain, t.mainC).GetString(),
		FormatOneTemp(t.hasSub[0], t.subC[0]).GetString(),
		FormatOneTemp(t.hasSub[1], t.subC[1]).GetString(),
		FormatOneTemp(t.hasSub[2], t.subC[2]).GetString(),
		FormatOneTemp(t.hasSub[3], t.subC[3]).GetString());
	outLines.Add(mainLine);

	static const LPCTSTR kSubNames[4] = {
		_T("MCS1"), _T("MCS2"), _T("1x64#1"), _T("1x64#2")
	};
	for (int i = 0; i < 4; ++i)
	{
		if (!t.hasEdfa[i])
			continue;
		CString aux;
		aux.Format(_T("[TEMP] %s EDFA OUT1=%.1fC (aux)"), kSubNames[i], t.edfaOut1C[i]);
		outLines.Add(aux);
	}
}
