#pragma once

#include "OpComm.h"
#include "CommLog.h"

// Lightweight ASCII line exchange for the Diagnosis flow (SW / WL / pd / opm).
// Independent from CRecalSession; logs into a dedicated [DIAG] channel
// with its own monotonic sequence so Python tooling can parse without
// colliding with the [RECAL] log stream.
class CDiagnosisSession
{
public:
	explicit CDiagnosisSession(COpComm& comm, const M576CommLogTarget& logTarget = M576CommLogTarget());

	void SetCommLogTarget(const M576CommLogTarget& logTarget) { m_logTarget = logTarget; }

	/// Send `payloadNoCr` (CR is appended automatically) and read one ASCII line.
	/// `cmdLabel` is the user-visible token in trace lines (e.g. "SW 1/200", "PD 1/200").
	/// Always returns the actual elapsed time in `outElapsedMs`. On failure, `reply` may
	/// still hold the partial bytes read (best effort) and the function returns FALSE.
	BOOL ExchangeAsciiLine(LPCTSTR cmdLabel,
		const CStringA& payloadNoCr,
		CStringA& reply,
		DWORD readTimeoutMs,
		DWORD& outElapsedMs,
		CString& err);

	/// Optional informational line (no SEND/RECV pair). Useful for "stopped at i/N".
	void EmitNote(LPCTSTR fmt, ...);

	COpComm& Comm() { return m_comm; }

private:
	COpComm& m_comm;
	M576CommLogTarget m_logTarget;
	DWORD m_traceSeq;
	COMMTIMEOUTS m_savedTimeouts;
	BOOL m_haveSaved;

	BOOL WriteNoPurgeReliable(const CStringA& wire, LPCTSTR cmdLabel, CString& err);
	BOOL ReadLineBlocking(CStringA& outLine, DWORD timeoutMs);
	void PushNonBlockingTimeouts();
	void PopTimeouts();
	void TraceSend(DWORD seq, LPCTSTR cmdLabel, const CStringA& wire);
	void TraceRecv(DWORD seq, LPCTSTR cmdLabel, const CStringA& reply, DWORD elapsedMs, BOOL ok, DWORD budgetMs);
};
