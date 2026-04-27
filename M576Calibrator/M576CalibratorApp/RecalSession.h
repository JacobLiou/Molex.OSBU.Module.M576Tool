#pragma once

#include "OpComm.h"
#include "CommLog.h"
#include "PathCsvDriver.h"
#include <vector>

/// 429F ASCII RECAL session (syntax finalized with firmware; logs raw traffic).
// 与 429F 之间的 ASCII RECAL 会话层：发 RECAL 0~5 与读行/扫频行，并记通信 trace；语法与固件定稿一致。
class CRecalSession
{
public:
	explicit CRecalSession(COpComm& comm429f, const M576CommLogTarget& logTarget = M576CommLogTarget());
	void SetCommLogTarget(const M576CommLogTarget& logTarget) { m_logTarget = logTarget; }

	/// Command A (PRD): `RECAL 0 <tls 1-8> <wavelength nm> <pm_range 0-4>\r`. Delay / DAC range / step use RECAL 3 / RECAL 5.
	// 命令 A：定标前环境（TLS、波长nm、PM 档）；单步 DAC/延时在 RECAL 3/5 中。
	BOOL SendRecal0(int tlsSource, int wavelengthNm, int pmRange, CString& err);

	/// Command B: `RECAL 1` — target 1..6 + four path channel numbers; response line is `OK`.
	// 命令 B：RECAL 1，target + 四路 Z4744 通道号，应答为 OK 行。
	BOOL SendRecal1(const SPathStep& step, CString& err);

	/// PM sweep: `RECAL 3` — sweepMode 0 = X fixed / Y sweep; 1 = Y fixed / X sweep.
	/// Params: Base DAC (`M576_RECAL_FW_READ_BASE_DAC` = FW reads channel as base; else host sets), Offset, Step, Delay (ms).
	// 功率计扫频 RECAL 3；sweepMode 0/1 为固定一轴、扫另一轴，参数含基准/偏置/步进/点间延时。
	BOOL SendRecal3(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err);

	/// Command C: `RECAL 2` — target 1..4 + two channel numbers (2#1x64 ch, MCS ch); response line is `OK`.
	// 命令 C：RECAL 2，target + 2#1x64 与 MCS 两路通道号，应答为 OK 行（PD 路径用）。
	BOOL SendRecal2(const SPathStepPd& step, CString& err);

	/// PD sweep: `RECAL 5` — same sweep modes / params as `RECAL 3`.
	// PD 扫频 RECAL 5，模式/参数与 RECAL 3 同构。
	BOOL SendRecal5(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CString& err);

	/// Read until end-of-line: first of \\n or \\r (firmware uses CR-only lines).
	// 读到行尾（\\n 或 \\r 先到先结束；固件可能只发 CR）。
	BOOL ReadAsciiResponse(CStringA& outLine, DWORD timeoutMs, CString& err);
	/// @param isFinalReadFailure If false, a timeout is logged as [RETRY] (transient) instead of [ERROR] (for Polly-style retries).
	// isFinalReadFailure 为假时，超时记为可重试；为真时记为终态错误（配合多次 Exchange）。
	BOOL ReadAsciiResponse(CStringA& outLine, DWORD timeoutMs, CString& err, BOOL isFinalReadFailure);

	/// `RECAL 3` / `RECAL 5` sweep lines: some firmware omits trailing CR/LF; end is detected by RX idle gap.
	// 扫频数据行：部分固件无尾换行，用接收空闲间隙判一帧结束。
	BOOL ReadAsciiSweepResponse(CStringA& outLine, DWORD timeoutMs, CString& err);
	BOOL ReadAsciiSweepResponse(CStringA& outLine, DWORD timeoutMs, CString& err, BOOL isFinalReadFailure);

	/// Polly-style: send+read with up to `M576_COMM_RETRY_MAX_ATTEMPTS` full exchanges.
	// 发一条 RECAL 并收一行/一帧扫频，带 Polly 式多轮全交换重试（次数见 M576_COMM_RETRY_MAX_ATTEMPTS）。
	BOOL ExchangeRecal0ReadLine(int tlsSource, int wavelengthNm, int pmRange, CStringA& outLine, DWORD timeoutMs, CString& err);
	BOOL ExchangeRecal1ReadLine(const SPathStep& step, CStringA& outLine, DWORD timeoutMs, CString& err);
	BOOL ExchangeRecal2ReadLine(const SPathStepPd& step, CStringA& outLine, DWORD timeoutMs, CString& err);
	BOOL ExchangeRecal3ReadSweep(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CStringA& outLine, DWORD readTimeoutMs, CString& err);
	BOOL ExchangeRecal5ReadSweep(int sweepMode, int baseDac, int offsetDac, int stepDac, int delayMs, CStringA& outLine, DWORD readTimeoutMs, CString& err);

	/// Split payload by comma/space/tab into doubles (best-effort for bring-up).
	// 按逗号/空白拆分功率样点（样机期宽松解析）。
	static BOOL ParsePowerDoubles(const CStringA& line, std::vector<double>& out);

	/// `RECAL 3` / `RECAL 5` line: first value = fixed-axis start DAC; rest = samples along the moving axis.
	// 扫频行：首值=固定轴起点 DAC，其余=动轴上各点功率/测量值。
	static BOOL ParseRecal3SweepLine(const CStringA& line, double& outAxisStart, std::vector<double>& outPowers);

	COpComm& Comm() { return m_comm; }

private:
	// 串口与通信日志、命令 trace、读阻塞/扫频读、COMMTIMEOUTS 推栈、无 purge 可靠写
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
	void TraceReceive(const CStringA& payload, DWORD elapsedMs, BOOL ok, DWORD timeoutMs, BOOL isFinalFailure = TRUE);

	COMMTIMEOUTS m_savedTimeouts;
	BOOL m_haveSaved;
	BOOL WriteNoPurgeReliable(const CStringA& fullLine, LPCTSTR commandName, CString& err);
};
