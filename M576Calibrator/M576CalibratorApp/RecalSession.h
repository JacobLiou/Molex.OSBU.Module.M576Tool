#pragma once

#include "OpComm.h"
#include "PathCsvDriver.h"
#include <vector>

/// 429F ASCII RECAL session (syntax finalized with firmware; logs raw traffic).
class CRecalSession
{
public:
	explicit CRecalSession(COpComm& comm429f);

	/// `RECAL 0 ...` — parameters per firmware; placeholder builds a minimal line.
	BOOL SendRecal0(int tlsSource, double wavelengthNm, CString& err);

	/// `RECAL 1` with nine integers after opcode (target + 8 path numbers).
	BOOL SendRecal1(const SPathStep& step, CString& err);

	/// Read until \\n (or \\r\\n), up to timeout. Appends to `accumulatedLog`.
	BOOL ReadAsciiResponse(CStringA& outLine, DWORD timeoutMs, CString& err);

	/// Split payload by comma/space/tab into doubles (best-effort for bring-up).
	static BOOL ParsePowerDoubles(const CStringA& line, std::vector<double>& out);

	COpComm& Comm() { return m_comm; }

private:
	COpComm& m_comm;
	BOOL ReadLineBlocking(CStringA& line, DWORD timeoutMs);
	void PushCommTimeouts(DWORD readTotalMs);
	void PopCommTimeouts();

	COMMTIMEOUTS m_savedTimeouts;
	BOOL m_haveSaved;
};
