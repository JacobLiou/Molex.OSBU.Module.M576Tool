#include "stdafx.h"
#include "RecalSession.h"
#include <stdlib.h>

CRecalSession::CRecalSession(COpComm& comm429f)
	: m_comm(comm429f)
	, m_haveSaved(FALSE)
{
	ZeroMemory(&m_savedTimeouts, sizeof(m_savedTimeouts));
}

void CRecalSession::PushCommTimeouts(DWORD readTotalMs)
{
	HANDLE h = m_comm.GetPortHandle();
	if (h == INVALID_HANDLE_VALUE)
		return;
	GetCommTimeouts(h, &m_savedTimeouts);
	m_haveSaved = TRUE;
	COMMTIMEOUTS t = {};
	t.ReadIntervalTimeout = MAXDWORD;
	t.ReadTotalTimeoutMultiplier = 0;
	t.ReadTotalTimeoutConstant = readTotalMs;
	t.WriteTotalTimeoutConstant = 5000;
	t.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(h, &t);
}

void CRecalSession::PopCommTimeouts()
{
	if (!m_haveSaved)
		return;
	HANDLE h = m_comm.GetPortHandle();
	if (h != INVALID_HANDLE_VALUE)
		SetCommTimeouts(h, &m_savedTimeouts);
	m_haveSaved = FALSE;
}

BOOL CRecalSession::ReadLineBlocking(CStringA& line, DWORD timeoutMs)
{
	line.Empty();
	PushCommTimeouts(timeoutMs);
	char buf[512];
	DWORD start = GetTickCount();
	while (GetTickCount() - start < timeoutMs + 50)
	{
		DWORD nread = 0;
		if (!m_comm.ReadBuffer(buf, sizeof(buf) - 1, &nread) || nread == 0)
		{
			Sleep(5);
			continue;
		}
		buf[nread] = 0;
		line += buf;
		int p = line.Find('\n');
		if (p >= 0)
		{
			line = line.Left(p);
			line.TrimRight("\r");
			PopCommTimeouts();
			return TRUE;
		}
		if (line.GetLength() > 8000)
			break;
	}
	PopCommTimeouts();
	return !line.IsEmpty();
}

BOOL CRecalSession::SendRecal0(int tlsSource, double wavelengthNm, int delayMs, int dacRange, int dacStep, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 0 %d %.4f %d %d %d\r\n", tlsSource, wavelengthNm, delayMs, dacRange, dacStep);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 0 failed");
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::SendRecal1(const SPathStep& step, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 1 %d %d %d %d %d\r\n",
		step.targetSwitchIndex,
		step.c1, step.c2, step.c3, step.c4);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 1 failed");
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::SendRecal3(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 3 %d %d %d %d %d\r\n",
		sweepMode, baseDac, offsetDac, stepDac, delayMs);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 3 failed");
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::SendRecal5(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 5 %d %d %d %d %d\r\n",
		sweepMode, baseDac, offsetDac, stepDac, delayMs);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 5 failed");
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::SendRecal2(const SPathStepPd& step, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 2 %d %d %d %d %d\r\n",
		step.targetSwitchIndex,
		step.p2b, step.p2c, step.p3b, step.p3c);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 2 failed");
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::ReadAsciiResponse(CStringA& outLine, DWORD timeoutMs, CString& err)
{
	UNREFERENCED_PARAMETER(err);
	return ReadLineBlocking(outLine, timeoutMs);
}

BOOL CRecalSession::ParsePowerDoubles(const CStringA& line, std::vector<double>& out)
{
	out.clear();
	const char* p = (LPCSTR)line;
	while (p && *p)
	{
		char* end = NULL;
		double v = strtod(p, &end);
		if (end == p)
		{
			++p;
			continue;
		}
		out.push_back(v);
		p = end;
		while (*p == ',' || *p == ' ' || *p == '\t' || *p == ';')
			++p;
	}
	return !out.empty();
}

BOOL CRecalSession::ParseRecal3SweepLine(const CStringA& line, double& outAxisStart, std::vector<double>& outPowers)
{
	outPowers.clear();
	outAxisStart = 0.0;
	std::vector<double> all;
	if (!ParsePowerDoubles(line, all) || all.size() < 2)
		return FALSE;
	outAxisStart = all[0];
	outPowers.assign(all.begin() + 1, all.end());
	return !outPowers.empty();
}
