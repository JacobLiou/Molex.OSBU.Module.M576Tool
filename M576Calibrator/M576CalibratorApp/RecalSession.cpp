#include "stdafx.h"
#include "RecalSession.h"
#include "CalibConstants.h"
#include "CommRetry.h"
#include <stdlib.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
// RecalSession.cpp：与 429F 间 ASCII RECAL0~5、读行/扫频行、Trace 与通信心跳（timeBeginPeriod 等）。

CRecalSession::CRecalSession(COpComm& comm429f, const M576CommLogTarget& logTarget)
	: m_comm(comm429f)
	, m_logTarget(logTarget)
	, m_traceSeq(0)
	, m_pendingTraceSeq(0)
	, m_pendingTick(0)
	, m_haveSaved(FALSE)
{
	ZeroMemory(&m_savedTimeouts, sizeof(m_savedTimeouts));
}

// ---------- Trace：发送/收包落盘、耗时与 [ERROR]/[RETRY] 标记 ----------
void CRecalSession::TraceSend(LPCTSTR commandName, const CStringA& payload)
{
	if (!m_logTarget.IsEnabled())
		return;
	m_pendingTraceSeq = ++m_traceSeq;
	m_pendingTick = GetTickCount();
	m_pendingCommand = commandName;
	CString line;
	line.Format(_T("[RECAL] #%lu SEND %s | %s"),
		m_pendingTraceSeq,
		commandName,
		M576EscapeAscii(payload).GetString());
	m_logTarget.Emit(line);
}

void CRecalSession::TraceReceive(const CStringA& payload, DWORD elapsedMs, BOOL ok, DWORD timeoutMs, BOOL isFinalFailure)
{
	if (!m_logTarget.IsEnabled())
		return;
	CString line;
	const DWORD seq = (m_pendingTraceSeq != 0) ? m_pendingTraceSeq : ++m_traceSeq;
	const CString command = m_pendingCommand.IsEmpty() ? _T("RECAL") : m_pendingCommand;
	if (ok)
	{
		line.Format(_T("[RECAL] #%lu RECV %s | %s | %lums"),
			seq,
			command.GetString(),
			M576EscapeAscii(payload).GetString(),
			elapsedMs);
	}
	else if (!isFinalFailure)
	{
		// 有 partial 时失败多为「有回复但非预期」：应显示 elapsed 与读预算 budget，勿把 budget 标成 waited。
		if (!payload.IsEmpty())
		{
			line.Format(
				_T("[RETRY] [RECAL] #%lu UNEXPECTED %s | partial=%s | elapsed=%lums budget=%lums (will retry)"),
				seq,
				command.GetString(),
				M576EscapeAscii(payload).GetString(),
				elapsedMs,
				timeoutMs);
		}
		else
		{
			line.Format(
				_T("[RETRY] [RECAL] #%lu TIMEOUT %s | elapsed=%lums budget=%lums (will retry)"),
				seq,
				command.GetString(),
				elapsedMs,
				timeoutMs);
		}
	}
	else if (!payload.IsEmpty())
	{
		line.Format(
			_T("[ERROR] [RECAL] #%lu UNEXPECTED %s | partial=%s | elapsed=%lums budget=%lums"),
			seq,
			command.GetString(),
			M576EscapeAscii(payload).GetString(),
			elapsedMs,
			timeoutMs);
	}
	else
	{
		line.Format(
			_T("[ERROR] [RECAL] #%lu TIMEOUT %s | elapsed=%lums budget=%lums"),
			seq,
			command.GetString(),
			elapsedMs,
			timeoutMs);
	}
	m_logTarget.Emit(line);
	if (ok || isFinalFailure)
	{
		m_pendingTraceSeq = 0;
		m_pendingTick = 0;
		m_pendingCommand.Empty();
	}
}

// ---------- COM 超时：非阻塞读 + 外层总时限（ReadLine* 内用 GetTickCount 轮询）----------
void CRecalSession::PushCommTimeouts(DWORD readTotalMs)
{
	(void)readTotalMs;
	HANDLE h = m_comm.GetPortHandle();
	if (h == INVALID_HANDLE_VALUE)
		return;
	GetCommTimeouts(h, &m_savedTimeouts);
	m_haveSaved = TRUE;
	COMMTIMEOUTS t = {};
	// Non-blocking ReadFile: return immediately with whatever RX bytes are queued.
	// If we set ReadTotalTimeoutConstant to e.g. 3000ms here, each ReadFile can block ~3s
	// even when firmware already sent "OK\\r\\n", which matches ~3000ms latency reports.
	// Overall deadline is enforced in ReadLineBlocking via GetTickCount() + outer loop.
	t.ReadIntervalTimeout = MAXDWORD;
	t.ReadTotalTimeoutMultiplier = 0;
	t.ReadTotalTimeoutConstant = 0;
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

// 读到首换行/回车为一条 ASCII 行（OK/应答用）。
BOOL CRecalSession::ReadLineBlocking(CStringA& line, DWORD timeoutMs)
{
	line.Empty();
	PushCommTimeouts(timeoutMs);
	char buf[16384];
	DWORD start = GetTickCount();
	while (GetTickCount() - start < timeoutMs + 50)
	{
		DWORD nread = 0;
		const DWORD avail = m_comm.RxBytesWaiting();
		if (avail == 0)
		{
			Sleep(1);
			continue;
		}
		const DWORD chunk = (avail < (DWORD)(sizeof(buf) - 1)) ? avail : (DWORD)(sizeof(buf) - 1);
		if (!m_comm.ReadBuffer(buf, chunk, &nread) || nread == 0)
			continue;
		buf[nread] = 0;
		line += buf;
		int p = line.Find('\n');
		if (p < 0)
			p = line.Find('\r');
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

// RECAL3/5 数据行：以尾空闲判帧、允许无 CR/LF。
BOOL CRecalSession::ReadSweepLineBlocking(CStringA& line, DWORD timeoutMs)
{
	line.Empty();
	// Default timer resolution makes Sleep(1) ~15ms on many PCs; tail-idle uses 150 consecutive Sleeps
	// -> ~2.3s extra. timeBeginPeriod(1) lets idle polling sleep ~1ms on this thread's scope.
	struct M576TimeBegin1ms
	{
		M576TimeBegin1ms() { (void)timeBeginPeriod(1); }
		~M576TimeBegin1ms() { (void)timeEndPeriod(1); }
	} timer1ms;
	PushCommTimeouts(timeoutMs);
	char buf[16384];
	const DWORD kMinCharsForTailDone = 30;
	const DWORD kTailIdleMs = 150;
	DWORD start = GetTickCount();
	DWORD tailStreak = 0;
	while (GetTickCount() - start < timeoutMs + 50)
	{
		DWORD nread = 0;
		const DWORD avail = m_comm.RxBytesWaiting();
		if (avail == 0)
		{
			if (!line.IsEmpty() && (DWORD)line.GetLength() >= kMinCharsForTailDone)
			{
				if (++tailStreak >= kTailIdleMs)
				{
					line.Trim();
					PopCommTimeouts();
					return TRUE;
				}
			}
			else
				tailStreak = 0;
			Sleep(1);
			continue;
		}
		tailStreak = 0;
		const DWORD chunk = (avail < (DWORD)(sizeof(buf) - 1)) ? avail : (DWORD)(sizeof(buf) - 1);
		if (!m_comm.ReadBuffer(buf, chunk, &nread) || nread == 0)
			continue;
		buf[nread] = 0;
		line += buf;
		int p = line.Find('\n');
		if (p < 0)
			p = line.Find('\r');
		if (p >= 0)
		{
			line = line.Left(p);
			line.TrimRight("\r");
			line.Trim();
			PopCommTimeouts();
			return TRUE;
		}
		if (line.GetLength() > 8000)
			break;
	}
	PopCommTimeouts();
	line.Trim();
	return !line.IsEmpty();
}

// ---------- 仅发送单条 RECAL（不含 Polly 重试；应答由另函数读）----------
BOOL CRecalSession::SendRecal0(int tlsSource, int wavelengthNm, int pmRange, CString& err)
{
	if (tlsSource < M576_MIN_TLS_SOURCE || tlsSource > M576_MAX_TLS_SOURCE)
	{
		err.Format(_T("RECAL 0: TLS source %d out of range %d..%d."),
			tlsSource, M576_MIN_TLS_SOURCE, M576_MAX_TLS_SOURCE);
		return FALSE;
	}
	if (wavelengthNm < M576_MIN_WAVELENGTH_NM || wavelengthNm > M576_MAX_WAVELENGTH_NM)
	{
		err.Format(_T("RECAL 0: wavelength %d out of PRD range %d..%d."),
			wavelengthNm, M576_MIN_WAVELENGTH_NM, M576_MAX_WAVELENGTH_NM);
		return FALSE;
	}
	if (pmRange < M576_MIN_PM_RANGE || pmRange > M576_MAX_PM_RANGE)
	{
		err.Format(_T("RECAL 0: pm range %d out of range %d..%d."),
			pmRange, M576_MIN_PM_RANGE, M576_MAX_PM_RANGE);
		return FALSE;
	}
	CStringA cmd;
	cmd.Format("RECAL 0 %d %d %d\r", tlsSource, wavelengthNm, pmRange);
	TraceSend(_T("RECAL 0"), cmd);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 0 failed");
		TraceReceive(CStringA(), 0, FALSE, 0);
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::SendRecal1(const SPathStep& step, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 1 %d %d %d %d %d\r",
		step.targetSwitchIndex,
		step.c1, step.c2, step.c3, step.c4);
	TraceSend(_T("RECAL 1"), cmd);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 1 failed");
		TraceReceive(CStringA(), 0, FALSE, 0);
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::SendRecal3(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 3 %d %d %d %d %d\r",
		sweepMode, baseDac, offsetDac, stepDac, delayMs);
	CString name;
	name.Format(_T("RECAL 3 %d"), sweepMode);
	TraceSend(name, cmd);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 3 failed");
		TraceReceive(CStringA(), 0, FALSE, 0);
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::SendRecal5(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 5 %d %d %d %d %d\r",
		sweepMode, baseDac, offsetDac, stepDac, delayMs);
	CString name;
	name.Format(_T("RECAL 5 %d"), sweepMode);
	TraceSend(name, cmd);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 5 failed");
		TraceReceive(CStringA(), 0, FALSE, 0);
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

BOOL CRecalSession::SendRecal2(const SPathStepPd& step, CString& err)
{
	CStringA cmd;
	cmd.Format("RECAL 2 %d %d %d\r",
		step.targetSwitchIndex,
		step.ch1, step.ch2);
	TraceSend(_T("RECAL 2"), cmd);
	int n = cmd.GetLength();
	if (!m_comm.WriteBufferNoPurge(cmd.GetBuffer(n), (DWORD)n))
	{
		cmd.ReleaseBuffer();
		err = _T("Write RECAL 2 failed");
		TraceReceive(CStringA(), 0, FALSE, 0);
		return FALSE;
	}
	cmd.ReleaseBuffer();
	return TRUE;
}

// ---------- 读普通 ASCII 一行 / 扫频一行，并打 Trace（含 isFinal 语义）----------
BOOL CRecalSession::ReadAsciiResponse(CStringA& outLine, DWORD timeoutMs, CString& err)
{
	return ReadAsciiResponse(outLine, timeoutMs, err, TRUE);
}

BOOL CRecalSession::ReadAsciiResponse(CStringA& outLine, DWORD timeoutMs, CString& err, BOOL isFinalReadFailure)
{
	UNREFERENCED_PARAMETER(err);
	const DWORD start = GetTickCount();
	const BOOL ok = ReadLineBlocking(outLine, timeoutMs);
	TraceReceive(outLine, GetTickCount() - start, ok, timeoutMs, isFinalReadFailure);
	return ok;
}

BOOL CRecalSession::ReadAsciiSweepResponse(CStringA& outLine, DWORD timeoutMs, CString& err)
{
	return ReadAsciiSweepResponse(outLine, timeoutMs, err, TRUE);
}

BOOL CRecalSession::ReadAsciiSweepResponse(CStringA& outLine, DWORD timeoutMs, CString& err, BOOL isFinalReadFailure)
{
	UNREFERENCED_PARAMETER(err);
	const DWORD start = GetTickCount();
	const BOOL ok = ReadSweepLineBlocking(outLine, timeoutMs);
	TraceReceive(outLine, GetTickCount() - start, ok, timeoutMs, isFinalReadFailure);
	return ok;
}

// 写 RECAL 行：WriteBufferNoPurge 多轮，不打 purge 以免清掉下位机已发数据。
BOOL CRecalSession::WriteNoPurgeReliable(const CStringA& fullLine, LPCTSTR commandName, CString& err)
{
	err.Empty();
	const int n = fullLine.GetLength();
	if (n <= 0)
	{
		err = _T("Empty command line.");
		return FALSE;
	}
	CStringA lineCopy(fullLine);
	char* pb = lineCopy.GetBuffer(n);
	BOOL okW = FALSE;
	for (int w = 0; w < (int)M576_COMM_WRITE_RETRY_MAX; ++w)
	{
		okW = m_comm.WriteBufferNoPurge(pb, (DWORD)n);
		if (okW)
			break;
		if (w + 1 < (int)M576_COMM_WRITE_RETRY_MAX)
			Sleep((DWORD)M576_COMM_RETRY_DELAY_MS);
	}
	lineCopy.ReleaseBuffer();
	if (okW)
		return TRUE;
	err.Format(_T("%s: write failed after %d attempt(s)."), commandName, (int)M576_COMM_WRITE_RETRY_MAX);
	return FALSE;
}

// ---------- 发+读 Polly 式全交换：读 OK 行/扫频行，失败可重试到 M576_COMM_RETRY_MAX_ATTEMPTS ----------
BOOL CRecalSession::ExchangeRecal0ReadLine(
	int tlsSource, int wavelengthNm, int pmRange, CStringA& outLine, DWORD timeoutMs, CString& err)
{
	if (tlsSource < M576_MIN_TLS_SOURCE || tlsSource > M576_MAX_TLS_SOURCE)
	{
		err.Format(_T("RECAL 0: TLS source %d out of range %d..%d."),
			tlsSource, M576_MIN_TLS_SOURCE, M576_MAX_TLS_SOURCE);
		return FALSE;
	}
	if (wavelengthNm < M576_MIN_WAVELENGTH_NM || wavelengthNm > M576_MAX_WAVELENGTH_NM)
	{
		err.Format(_T("RECAL 0: wavelength %d out of PRD range %d..%d."),
			wavelengthNm, M576_MIN_WAVELENGTH_NM, M576_MAX_WAVELENGTH_NM);
		return FALSE;
	}
	if (pmRange < M576_MIN_PM_RANGE || pmRange > M576_MAX_PM_RANGE)
	{
		err.Format(_T("RECAL 0: pm range %d out of range %d..%d."),
			pmRange, M576_MIN_PM_RANGE, M576_MAX_PM_RANGE);
		return FALSE;
	}
	const int kMax = (int)M576_COMM_RETRY_MAX_ATTEMPTS;
	for (int a = 1; a <= kMax; ++a)
	{
		const BOOL isFinal = (a >= kMax);
		CStringA cmd;
		cmd.Format("RECAL 0 %d %d %d\r", tlsSource, wavelengthNm, pmRange);
		TraceSend(_T("RECAL 0"), cmd);
		if (!WriteNoPurgeReliable(cmd, _T("RECAL 0"), err))
		{
			TraceReceive(CStringA(), 0, FALSE, 0, TRUE);
			return FALSE;
		}
		const DWORD t0 = GetTickCount();
		const BOOL gotLine = ReadLineBlocking(outLine, timeoutMs);
		const DWORD elapsed0 = GetTickCount() - t0;
		if (gotLine)
			outLine.Trim();
		// 仅首行 = OK 视为本命令成功；FAIL/其它/超时 与可重试（下位机有回复但不符合协议）
		const BOOL good = gotLine && (outLine.CompareNoCase("OK") == 0);
		TraceReceive(outLine, elapsed0, good, timeoutMs, isFinal);
		if (good)
			return TRUE;
		if (!isFinal)
			Sleep((DWORD)M576_COMM_RETRY_DELAY_MS);
	}
	if (err.IsEmpty())
		err = _T("RECAL 0: no OK line after retries.");
	return FALSE;
}

BOOL CRecalSession::ExchangeRecal1ReadLine(const SPathStep& step, CStringA& outLine, DWORD timeoutMs, CString& err)
{
	const int kMax = (int)M576_COMM_RETRY_MAX_ATTEMPTS;
	for (int a = 1; a <= kMax; ++a)
	{
		const BOOL isFinal = (a >= kMax);
		CStringA cmd;
		cmd.Format("RECAL 1 %d %d %d %d %d\r",
			step.targetSwitchIndex,
			step.c1, step.c2, step.c3, step.c4);
		TraceSend(_T("RECAL 1"), cmd);
		if (!WriteNoPurgeReliable(cmd, _T("RECAL 1"), err))
		{
			TraceReceive(CStringA(), 0, FALSE, 0, TRUE);
			return FALSE;
		}
		const DWORD t0 = GetTickCount();
		const BOOL gotLine = ReadLineBlocking(outLine, timeoutMs);
		const DWORD elapsed1 = GetTickCount() - t0;
		if (gotLine)
			outLine.Trim();
		const BOOL good = gotLine && (outLine.CompareNoCase("OK") == 0);
		TraceReceive(outLine, elapsed1, good, timeoutMs, isFinal);
		if (good)
			return TRUE;
		if (!isFinal)
			Sleep((DWORD)M576_COMM_RETRY_DELAY_MS);
	}
	if (err.IsEmpty())
		err = _T("RECAL 1: no OK line after retries.");
	return FALSE;
}

BOOL CRecalSession::ExchangeRecal2ReadLine(const SPathStepPd& step, CStringA& outLine, DWORD timeoutMs, CString& err)
{
	const int kMax = (int)M576_COMM_RETRY_MAX_ATTEMPTS;
	for (int a = 1; a <= kMax; ++a)
	{
		const BOOL isFinal = (a >= kMax);
		CStringA cmd;
		cmd.Format("RECAL 2 %d %d %d\r",
			step.targetSwitchIndex,
			step.ch1, step.ch2);
		TraceSend(_T("RECAL 2"), cmd);
		if (!WriteNoPurgeReliable(cmd, _T("RECAL 2"), err))
		{
			TraceReceive(CStringA(), 0, FALSE, 0, TRUE);
			return FALSE;
		}
		const DWORD t0 = GetTickCount();
		const BOOL gotLine = ReadLineBlocking(outLine, timeoutMs);
		const DWORD elapsed2 = GetTickCount() - t0;
		if (gotLine)
			outLine.Trim();
		const BOOL good = gotLine && (outLine.CompareNoCase("OK") == 0);
		TraceReceive(outLine, elapsed2, good, timeoutMs, isFinal);
		if (good)
			return TRUE;
		if (!isFinal)
			Sleep((DWORD)M576_COMM_RETRY_DELAY_MS);
	}
	if (err.IsEmpty())
		err = _T("RECAL 2: no OK line after retries.");
	return FALSE;
}

// 扫频重试用的 Trace/错误串名称（RECAL3 vs RECAL5）。
static void R3R5Name(int sweepMode, BOOL isFive, CString& o)
{
	if (isFive)
		o.Format(_T("RECAL 5 %d"), sweepMode);
	else
		o.Format(_T("RECAL 3 %d"), sweepMode);
}

BOOL CRecalSession::ExchangeRecal3ReadSweep(
	int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CStringA& outLine, DWORD readTimeoutMs, CString& err)
{
	const int kMax = (int)M576_COMM_RETRY_MAX_ATTEMPTS;
	for (int a = 1; a <= kMax; ++a)
	{
		const BOOL isFinal = (a >= kMax);
		CStringA cmd;
		cmd.Format("RECAL 3 %d %d %d %d %d\r",
			sweepMode, baseDac, offsetDac, stepDac, delayMs);
		CString traceName;
		R3R5Name(sweepMode, FALSE, traceName);
		TraceSend(traceName, cmd);
		if (!WriteNoPurgeReliable(cmd, traceName, err))
		{
			TraceReceive(CStringA(), 0, FALSE, 0, TRUE);
			return FALSE;
		}
		const DWORD t0 = GetTickCount();
		const BOOL gotSweep = ReadSweepLineBlocking(outLine, readTimeoutMs);
		const DWORD elapsed3 = GetTickCount() - t0;
		if (gotSweep)
			outLine.Trim();
		double axTmp = 0.0;
		std::vector<double> pTmp;
		// 超时无行、显式 FAIL、或样点行解析失败 均不视为成功，可整包重发
		const BOOL good = gotSweep
			&& (outLine.CompareNoCase("FAIL") != 0)
			&& ParseRecal3SweepLine(outLine, axTmp, pTmp);
		TraceReceive(outLine, elapsed3, good, readTimeoutMs, isFinal);
		if (good)
			return TRUE;
		if (!isFinal)
			Sleep((DWORD)M576_COMM_RETRY_DELAY_MS);
	}
	if (err.IsEmpty())
		err = _T("RECAL 3: no valid sweep line after retries.");
	return FALSE;
}

BOOL CRecalSession::ExchangeRecal5ReadSweep(
	int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CStringA& outLine, DWORD readTimeoutMs, CString& err)
{
	const int kMax = (int)M576_COMM_RETRY_MAX_ATTEMPTS;
	for (int a = 1; a <= kMax; ++a)
	{
		const BOOL isFinal = (a >= kMax);
		CStringA cmd;
		cmd.Format("RECAL 5 %d %d %d %d %d\r",
			sweepMode, baseDac, offsetDac, stepDac, delayMs);
		CString traceName;
		R3R5Name(sweepMode, TRUE, traceName);
		TraceSend(traceName, cmd);
		if (!WriteNoPurgeReliable(cmd, traceName, err))
		{
			TraceReceive(CStringA(), 0, FALSE, 0, TRUE);
			return FALSE;
		}
		const DWORD t0 = GetTickCount();
		const BOOL gotSweep = ReadSweepLineBlocking(outLine, readTimeoutMs);
		const DWORD elapsed5 = GetTickCount() - t0;
		if (gotSweep)
			outLine.Trim();
		double ax5 = 0.0;
		std::vector<double> p5;
		const BOOL good = gotSweep
			&& (outLine.CompareNoCase("FAIL") != 0)
			&& ParseRecal3SweepLine(outLine, ax5, p5);
		TraceReceive(outLine, elapsed5, good, readTimeoutMs, isFinal);
		if (good)
			return TRUE;
		if (!isFinal)
			Sleep((DWORD)M576_COMM_RETRY_DELAY_MS);
	}
	if (err.IsEmpty())
		err = _T("RECAL 5: no valid sweep line after retries.");
	return FALSE;
}

// ---------- 从逗号分隔文本解析样点，以及 RECAL3/5 行首格点 + 动轴样点切分 ----------
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
