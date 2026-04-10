#pragma once

#include "OpComm.h"
#include "PathCsvDriver.h"
#include <vector>

/// 429F ASCII RECAL session (syntax finalized with firmware; logs raw traffic).
class CRecalSession
{
public:
	explicit CRecalSession(COpComm& comm429f);

	/// Command A: `RECAL 0` — TLS, wavelength, delay (ms), DAC range, DAC step (Z4744 / PRD).
	BOOL SendRecal0(int tlsSource, double wavelengthNm, int delayMs, int dacRange, int dacStep, CString& err);

	/// Command B: `RECAL 1` — target 1..6 + eight path integers (four block/channel pairs).
	BOOL SendRecal1(const SPathStep& step, CString& err);

	/// Command C: `RECAL 2` — target 1..4 + 2#1x64 (block, ch) + MCS (block, ch).
	BOOL SendRecal2(const SPathStepPd& step, CString& err);

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
