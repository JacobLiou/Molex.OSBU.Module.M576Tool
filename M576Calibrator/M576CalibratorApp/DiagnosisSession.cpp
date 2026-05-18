#include "stdafx.h"
#include "DiagnosisSession.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace
{
	constexpr DWORD kDiagWriteRetryMax = 3;
	constexpr DWORD kDiagWriteRetryDelayMs = 30;
}

CDiagnosisSession::CDiagnosisSession(COpComm& comm, const M576CommLogTarget& logTarget)
	: m_comm(comm)
	, m_logTarget(logTarget)
	, m_traceSeq(0)
	, m_haveSaved(FALSE)
{
	ZeroMemory(&m_savedTimeouts, sizeof(m_savedTimeouts));
}

void CDiagnosisSession::PushNonBlockingTimeouts()
{
	HANDLE h = m_comm.GetPortHandle();
	if (h == INVALID_HANDLE_VALUE)
		return;
	GetCommTimeouts(h, &m_savedTimeouts);
	m_haveSaved = TRUE;
	COMMTIMEOUTS t = {};
	t.ReadIntervalTimeout = MAXDWORD;
	t.ReadTotalTimeoutMultiplier = 0;
	t.ReadTotalTimeoutConstant = 0;
	t.WriteTotalTimeoutConstant = 5000;
	t.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(h, &t);
}

void CDiagnosisSession::PopTimeouts()
{
	if (!m_haveSaved)
		return;
	HANDLE h = m_comm.GetPortHandle();
	if (h != INVALID_HANDLE_VALUE)
		SetCommTimeouts(h, &m_savedTimeouts);
	m_haveSaved = FALSE;
}

BOOL CDiagnosisSession::ReadLineBlocking(CStringA& line, DWORD timeoutMs)
{
	line.Empty();
	PushNonBlockingTimeouts();
	CStringA rx;
	char buf[16384];
	const DWORD start = GetTickCount();
	while (GetTickCount() - start < timeoutMs + 50)
	{
		const DWORD avail = m_comm.RxBytesWaiting();
		if (avail == 0)
		{
			Sleep(1);
			continue;
		}
		const DWORD chunk = (avail < (DWORD)(sizeof(buf) - 1)) ? avail : (DWORD)(sizeof(buf) - 1);
		DWORD nread = 0;
		if (!m_comm.ReadBuffer(buf, chunk, &nread) || nread == 0)
			continue;
		buf[nread] = 0;
		rx += buf;
		// Peel complete lines; ignore empty frames (bare \r / \n) so we do not return
		// success with an empty reply while the real OK/FAIL or numeric line is still in flight.
		for (;;)
		{
			const int pN = rx.Find('\n');
			const int pR = rx.Find('\r');
			int p = -1;
			if (pN >= 0 && pR >= 0)
				p = (pN < pR) ? pN : pR;
			else if (pN >= 0)
				p = pN;
			else if (pR >= 0)
				p = pR;
			else
				break;
			CStringA piece = rx.Left(p);
			rx = rx.Mid(p + 1);
			while (rx.GetLength() > 0 && (rx[0] == '\n' || rx[0] == '\r'))
				rx = rx.Mid(1);
			piece.Trim();
			if (piece.IsEmpty())
				continue;
			line = piece;
			PopTimeouts();
			return TRUE;
		}
		if (rx.GetLength() > 8000)
			break;
	}
	PopTimeouts();
	rx.Trim();
	if (!rx.IsEmpty())
	{
		line = rx;
		return TRUE;
	}
	return FALSE;
}

BOOL CDiagnosisSession::WriteNoPurgeReliable(const CStringA& wire, LPCTSTR cmdLabel, CString& err)
{
	err.Empty();
	const int n = wire.GetLength();
	if (n <= 0)
	{
		err = _T("Empty diagnosis command line.");
		return FALSE;
	}
	CStringA copy(wire);
	char* pb = copy.GetBuffer(n);
	BOOL ok = FALSE;
	for (DWORD a = 0; a < kDiagWriteRetryMax; ++a)
	{
		ok = m_comm.WriteBufferNoPurge(pb, (DWORD)n);
		if (ok)
			break;
		if (a + 1 < kDiagWriteRetryMax)
			Sleep(kDiagWriteRetryDelayMs);
	}
	copy.ReleaseBuffer();
	if (ok)
		return TRUE;
	err.Format(_T("%s: write failed after %lu attempt(s)."), cmdLabel, (unsigned long)kDiagWriteRetryMax);
	return FALSE;
}

void CDiagnosisSession::TraceSend(DWORD seq, LPCTSTR cmdLabel, const CStringA& wire)
{
	if (!m_logTarget.IsEnabled())
		return;
	m_logTarget.EmitFormat(
		_T("[DIAG] #%lu SEND %s | %s"),
		(unsigned long)seq,
		cmdLabel,
		M576EscapeAscii(wire).GetString());
}

void CDiagnosisSession::TraceRecv(DWORD seq, LPCTSTR cmdLabel, const CStringA& reply, DWORD elapsedMs, BOOL ok, DWORD budgetMs)
{
	if (!m_logTarget.IsEnabled())
		return;
	if (ok)
	{
		m_logTarget.EmitFormat(
			_T("[DIAG] #%lu RECV %s | %s | %lums"),
			(unsigned long)seq,
			cmdLabel,
			M576EscapeAscii(reply).GetString(),
			(unsigned long)elapsedMs);
	}
	else if (!reply.IsEmpty())
	{
		m_logTarget.EmitFormat(
			_T("[ERROR] [DIAG] #%lu UNEXPECTED %s | partial=%s | elapsed=%lums budget=%lums"),
			(unsigned long)seq,
			cmdLabel,
			M576EscapeAscii(reply).GetString(),
			(unsigned long)elapsedMs,
			(unsigned long)budgetMs);
	}
	else
	{
		m_logTarget.EmitFormat(
			_T("[ERROR] [DIAG] #%lu TIMEOUT %s | elapsed=%lums budget=%lums"),
			(unsigned long)seq,
			cmdLabel,
			(unsigned long)elapsedMs,
			(unsigned long)budgetMs);
	}
}

BOOL CDiagnosisSession::ExchangeAsciiLine(
	LPCTSTR cmdLabel,
	const CStringA& payloadNoCr,
	CStringA& reply,
	DWORD readTimeoutMs,
	DWORD& outElapsedMs,
	CString& err)
{
	reply.Empty();
	outElapsedMs = 0;
	err.Empty();
	CStringA wire = payloadNoCr;
	wire.Trim();
	if (wire.IsEmpty())
	{
		err = _T("Diagnosis: empty payload.");
		return FALSE;
	}
	if (wire[wire.GetLength() - 1] != '\r')
		wire += '\r';

	const DWORD seq = ++m_traceSeq;
	TraceSend(seq, cmdLabel, wire);

	if (!WriteNoPurgeReliable(wire, cmdLabel, err))
	{
		if (m_logTarget.IsEnabled())
		{
			m_logTarget.EmitFormat(
				_T("[ERROR] [DIAG] #%lu WRITE %s | %s"),
				(unsigned long)seq,
				cmdLabel,
				err.GetString());
		}
		return FALSE;
	}

	const DWORD t0 = GetTickCount();
	const BOOL ok = ReadLineBlocking(reply, readTimeoutMs);
	outElapsedMs = GetTickCount() - t0;
	reply.Trim();
	TraceRecv(seq, cmdLabel, reply, outElapsedMs, ok, readTimeoutMs);
	if (!ok)
	{
		err.Format(_T("%s: read timeout (%lums)."), cmdLabel, (unsigned long)readTimeoutMs);
		return FALSE;
	}
	return TRUE;
}

void CDiagnosisSession::EmitNote(LPCTSTR fmt, ...)
{
	if (!m_logTarget.IsEnabled())
		return;
	va_list args;
	va_start(args, fmt);
	CString detail;
	detail.FormatV(fmt, args);
	va_end(args);
	const DWORD seq = ++m_traceSeq;
	m_logTarget.EmitFormat(_T("[DIAG] #%lu NOTE | %s"), (unsigned long)seq, detail.GetString());
}
