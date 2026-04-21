#pragma once

#include "OpComm.h"
#include "CommLog.h"
#include "PathCsvDriver.h"
#include <vector>

/// 429F ASCII RECAL session (syntax finalized with firmware; logs raw traffic).
class CRecalSession
{
public:
	explicit CRecalSession(COpComm& comm429f, const M576CommLogTarget& logTarget = M576CommLogTarget());
	void SetCommLogTarget(const M576CommLogTarget& logTarget) { m_logTarget = logTarget; }

	/// Command A (PRD): `RECAL 0 <tls 1-8> <wavelength nm> <pm_range 0-4>\r`. Delay / DAC range / step use RECAL 3 / RECAL 5.
	BOOL SendRecal0(int tlsSource, int wavelengthNm, int pmRange, CString& err);

	/// Command B: `RECAL 1` 鈥?target 1..6 + four path channel numbers; response line is `OK`.
	BOOL SendRecal1(const SPathStep& step, CString& err);

	/// PM sweep: `RECAL 3` 鈥?sweepMode 0 = X fixed / Y sweep; 1 = Y fixed / X sweep.
	/// Params: Base DAC (0 = FW uses channel DAC), Offset, Step, Delay (ms), per firmware.
	BOOL SendRecal3(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err);

	/// Command C: `RECAL 2` — target 1..4 + two channel numbers (2#1x64 ch, MCS ch); response line is `OK`.
	BOOL SendRecal2(const SPathStepPd& step, CString& err);

	/// PD sweep: `RECAL 5` 鈥?same sweep modes / params as `RECAL 3`.
	BOOL SendRecal5(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err);

	/// Read until end-of-line: first of \\n or \\r (firmware uses CR-only lines).
	BOOL ReadAsciiResponse(CStringA& outLine, DWORD timeoutMs, CString& err);

	/// `RECAL 3` / `RECAL 5` sweep lines: some firmware omits trailing CR/LF; end is detected by RX idle gap.
	BOOL ReadAsciiSweepResponse(CStringA& outLine, DWORD timeoutMs, CString& err);

	/// Split payload by comma/space/tab into doubles (best-effort for bring-up).
	static BOOL ParsePowerDoubles(const CStringA& line, std::vector<double>& out);

	/// `RECAL 3` / `RECAL 5` line: first value = fixed-axis start DAC; rest = samples along the moving axis.
	static BOOL ParseRecal3SweepLine(const CStringA& line, double& outAxisStart, std::vector<double>& outPowers);

	COpComm& Comm() { return m_comm; }

private:
	COpComm& m_comm;
	M576CommLogTarget m_logTarget;
	DWORD m_traceSeq;
	DWORD m_pendingTraceSeq;
	DWORD m_pendingTick;
	CString m_pendingCommand;
	BOOL ReadLineBlocking(CStringA& line, DWORD timeoutMs);
	BOOL ReadSweepLineBlocking(CStringA& line, DWORD timeoutMs);
	void PushCommTimeouts(DWORD readTotalMs);
	void PopCommTimeouts();
	void TraceSend(LPCTSTR commandName, const CStringA& payload);
	void TraceReceive(const CStringA& payload, DWORD elapsedMs, BOOL ok, DWORD timeoutMs);

	COMMTIMEOUTS m_savedTimeouts;
	BOOL m_haveSaved;
};
