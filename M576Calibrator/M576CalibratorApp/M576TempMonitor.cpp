#include "stdafx.h"
#include "M576TempMonitor.h"
#include "Board439fTransTunnel.h"

#include <cstdlib>

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

	/// Payload after decode: optional B2 echo, then 5-byte records (id + BE32 value ??0.1 ??C).
	static BOOL ParseB2BoxEdfaReply(const BYTE* data, int n, double& boxC, double& edfaC, CString& err)
	{
		err.Empty();
		boxC = NAN;
		edfaC = NAN;
		if (!data || n < 8)
		{
			err = _T("B2 temp: reply too short.");
			return FALSE;
		}
		if (data[1] != 0)
		{
			err.Format(_T("B2 temp: status 0x%02X."), data[1]);
			return FALSE;
		}
		const int payloadLen = (int)data[3] * 256 + (int)data[4];
		int off = 5;
		if (off < n && data[off] == 0xB2)
			++off;
		const int end = 5 + payloadLen;
		bool gotBox = false;
		bool gotEdfa = false;
		while (off + 5 <= n && off + 5 <= end)
		{
			const BYTE id = data[off];
			const DWORD raw = ((DWORD)data[off + 1] << 24) | ((DWORD)data[off + 2] << 16)
				| ((DWORD)data[off + 3] << 8) | (DWORD)data[off + 4];
			const double c = (double)(LONG)raw / 10.0;
			if (id == 0x00)
			{
				boxC = c;
				gotBox = true;
			}
			else if (id == 0x29)
			{
				edfaC = c;
				gotEdfa = true;
			}
			off += 5;
		}
		if (!gotBox)
		{
			err = _T("B2 temp: box ObjId 0x00 not found.");
			return FALSE;
		}
		(void)gotEdfa;
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
		err.Format(_T("B2 temp: trans %d out of range 1..4."), trans1to4);
		return FALSE;
	}

	if (!Board439fTransTunnel::BeginTrans(cmd, trans1to4, err))
		return FALSE;

	BYTE byData[16] = {};
	byData[0] = START_CMD;
	byData[1] = 0xB2;
	byData[2] = 0x00;
	byData[3] = 0x00;
	byData[4] = 0x02;
	byData[5] = 0x00;
	byData[6] = 0x29;
	const int nLength = byData[3] * 256 + byData[4] + 4;
	int nCheckSum = 0;
	for (int i = 1; i <= nLength; ++i)
		nCheckSum = nCheckSum ^ byData[i];
	byData[7] = (BYTE)(nCheckSum - 1);
	byData[8] = END_CMD;
	WORD nCmdLength = 9;
	PBYTE pSend = NULL;
	if (!cmd.CmdSendExchange(byData, nCmdLength, &pSend, &nCmdLength))
	{
		err = _T("B2 temp: CmdSendExchange failed.");
		CString e2;
		(void)Board439fTransTunnel::EndTrans(cmd, e2);
		return FALSE;
	}
	if (!cmd.WriteBuffer((char*)pSend, nCmdLength))
	{
		err = _T("B2 temp: write failed.");
		CString e2;
		(void)Board439fTransTunnel::EndTrans(cmd, e2);
		return FALSE;
	}
	Sleep(50);
	BYTE byGet[256] = {};
	DWORD got = 0;
	if (!cmd.ReadBuffer((char*)byGet, sizeof(byGet), &got) || got == 0)
	{
		err = _T("B2 temp: read failed or empty.");
		CString e2;
		(void)Board439fTransTunnel::EndTrans(cmd, e2);
		return FALSE;
	}

	PBYTE pDecoded = NULL;
	WORD decodedLen = 0;
	if (!cmd.CmdReadExchange(byGet, (WORD)got, &pDecoded, &decodedLen))
	{
		err = _T("B2 temp: CmdReadExchange failed.");
		CString e2;
		(void)Board439fTransTunnel::EndTrans(cmd, e2);
		return FALSE;
	}

	CString parseErr;
	const BOOL ok = ParseB2BoxEdfaReply(pDecoded, (int)decodedLen, boxC, edfaOut1C, parseErr);
	if (!ok)
		err = parseErr;

	CString endErr;
	if (!Board439fTransTunnel::EndTrans(cmd, endErr))
	{
		if (err.IsEmpty())
			err = endErr;
		else if (!endErr.IsEmpty())
			err += _T("; ") + endErr;
		return FALSE;
	}
	return ok;
}

BOOL M576ReadAllFiveTempsC(Z4671Command& cmd, M576FiveTemps& out, CString& errDetail)
{
	errDetail.Empty();
	out = M576FiveTemps{};

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
