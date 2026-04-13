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

	/// Command B: `RECAL 1` — target 1..6 + four path channel numbers; response line is `OK`.
	BOOL SendRecal1(const SPathStep& step, CString& err);

	/// PM sweep: `RECAL 3` — sweepMode 0 = X fixed / Y sweep; 1 = Y fixed / X sweep.
	/// Params: Base DAC (0 = FW uses channel DAC), Offset, Step, Delay (ms), per firmware.
	BOOL SendRecal3(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err);

	/// PD sweep: `RECAL 5` — same sweep modes / params as `RECAL 3`.
	BOOL SendRecal5(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err);

	/// Command C: `RECAL 2` — target 1..4 + two path channel pairs; response line is `OK`.
	BOOL SendRecal2(const SPathStepPd& step, CString& err);

	/// Read until \\n (or \\r\\n), up to timeout. Appends to `accumulatedLog`.
	BOOL ReadAsciiResponse(CStringA& outLine, DWORD timeoutMs, CString& err);

	/// Split payload by comma/space/tab into doubles (best-effort for bring-up).
	static BOOL ParsePowerDoubles(const CStringA& line, std::vector<double>& out);

	/// `RECAL 3` / `RECAL 5` line: first value = fixed-axis start DAC; rest = samples along the moving axis.
	static BOOL ParseRecal3SweepLine(const CStringA& line, double& outAxisStart, std::vector<double>& outPowers);

	COpComm& Comm() { return m_comm; }

private:
	COpComm& m_comm;
	BOOL ReadLineBlocking(CStringA& line, DWORD timeoutMs);
	void PushCommTimeouts(DWORD readTotalMs);
	void PopCommTimeouts();

	COMMTIMEOUTS m_savedTimeouts;
	BOOL m_haveSaved;
};
