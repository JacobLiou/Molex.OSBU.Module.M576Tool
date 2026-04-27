#include "stdafx.h"
#include "Board439fTransTunnel.h"
#include "CalibConstants.h"
// 439F 多路 trans：写 trans/$$ 与 RX 排空；写操作带 M576_COMM_WRITE 重试（NoPurge）。

static BOOL M576WriteNoPurgeRetry439f(Z4671Command& cmd, char* p, DWORD n, CString& err, LPCTSTR ctx) // 底层多轮写
{
	for (int w = 0; w < (int)M576_COMM_WRITE_RETRY_MAX; ++w)
	{
		if (cmd.WriteBufferNoPurge(p, n))
			return TRUE;
		if (w + 1 < (int)M576_COMM_WRITE_RETRY_MAX)
			Sleep((DWORD)M576_COMM_RETRY_DELAY_MS);
	}
	err.Format(_T("%s: write failed after %d attempt(s)."), ctx, (int)M576_COMM_WRITE_RETRY_MAX);
	return FALSE;
}

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace {
// 匿名：进入 trans 后收紧读策略、按空闲判 ASCII 排空，供 Begin/End 清回声。

BOOL PushFastReadTimeouts(HANDLE h, COMMTIMEOUTS& saved)
{
	if (h == INVALID_HANDLE_VALUE || h == NULL)
		return FALSE;
	if (!GetCommTimeouts(h, &saved))
		return FALSE;
	COMMTIMEOUTS t = {};
	t.ReadIntervalTimeout = MAXDWORD;
	t.ReadTotalTimeoutMultiplier = 0;
	t.ReadTotalTimeoutConstant = 0;
	t.WriteTotalTimeoutConstant = 5000;
	t.WriteTotalTimeoutMultiplier = 0;
	return SetCommTimeouts(h, &t) ? TRUE : FALSE;
}

void PopCommTimeouts(HANDLE h, COMMTIMEOUTS& saved)
{
	if (h != INVALID_HANDLE_VALUE && h != NULL)
		SetCommTimeouts(h, &saved);
}

// Drain RX until idleMs with no bytes (cap maxWaitMs).
void DrainAscii(Z4671Command& cmd, DWORD maxWaitMs, DWORD idleMs, CStringA& accumulated)
{
	accumulated.Empty();
	HANDLE h = cmd.GetPortHandle();
	if (h == INVALID_HANDLE_VALUE || h == NULL)
		return;
	COMMTIMEOUTS saved = {};
	if (!PushFastReadTimeouts(h, saved))
		return;
	const DWORD t0 = GetTickCount();
	DWORD lastDataTick = t0;
	while (GetTickCount() - t0 < maxWaitMs)
	{
		const DWORD avail = cmd.RxBytesWaiting();
		if (avail > 0)
		{
			char buf[1024];
			const DWORD chunk = (avail < sizeof(buf) - 1) ? avail : (DWORD)(sizeof(buf) - 1);
			DWORD nread = 0;
			if (cmd.ReadBuffer(buf, chunk, &nread) && nread > 0)
			{
				buf[nread] = 0;
				accumulated += buf;
				lastDataTick = GetTickCount();
			}
			continue;
		}
		if (GetTickCount() - lastDataTick >= idleMs)
			break;
		Sleep(1);
	}
	PopCommTimeouts(h, saved);
}

} // namespace

// 发 $$ 退出透传，并短排空 UART，避免下一条 Z4671 二进制粘旧 ASCII。
BOOL Board439fTransTunnel::EndTrans(Z4671Command& cmd, CString& err)
{
	err.Empty();
	static const char kEnd[] = "$$\r\n";
	cmd.TraceInfo(_T("439F"), _T("TRANS end: send $$"));
	if (!M576WriteNoPurgeRetry439f(cmd, (char*)kEnd, (DWORD)(sizeof(kEnd) - 1), err, _T("439F $$")))
	{
		cmd.TraceError(_T("439F"), _T("%s"), err.GetString());
		return FALSE;
	}
	CStringA echo;
	DrainAscii(cmd, M576_439F_TRANS_END_DRAIN_MAX_MS, M576_439F_TRANS_DRAIN_IDLE_MS, echo);
	if (!echo.IsEmpty())
		cmd.TraceInfo(_T("439F"), _T("TRANS end drain: %s"), CString(echo).GetString());
	return TRUE;
}

// 发 trans n，排空直至空闲，使后续可安全发 Z4671 到该下位机。
BOOL Board439fTransTunnel::BeginTrans(Z4671Command& cmd, int transChannel, CString& err)
{
	err.Empty();
	if (transChannel < 1 || transChannel > 9)
	{
		err.Format(_T("439F: trans channel %d out of range."), transChannel);
		cmd.TraceError(_T("439F"), _T("%s"), err.GetString());
		return FALSE;
	}
	CStringA line;
	line.Format("trans %d\r\n", transChannel);
	cmd.TraceInfo(_T("439F"), _T("TRANS begin: %s"), CString(line).Trim().GetString());
	const int n = line.GetLength();
	if (n <= 0)
	{
		err = _T("439F: empty trans line.");
		cmd.TraceError(_T("439F"), _T("%s"), err.GetString());
		return FALSE;
	}
	{
		char* pb = line.GetBuffer(n);
		const BOOL wOk = M576WriteNoPurgeRetry439f(cmd, pb, (DWORD)n, err, _T("439F trans"));
		line.ReleaseBuffer();
		if (!wOk)
		{
			if (err.IsEmpty())
				err.Format(_T("439F: write trans %d failed."), transChannel);
			cmd.TraceError(_T("439F"), _T("%s"), err.GetString());
			return FALSE;
		}
	}
	CStringA echo;
	DrainAscii(cmd, M576_439F_TRANS_DRAIN_MAX_MS, M576_439F_TRANS_DRAIN_IDLE_MS, echo);
	if (!echo.IsEmpty())
		cmd.TraceInfo(_T("439F"), _T("TRANS begin drain: %s"), CString(echo).GetString());
	if (M576_439F_POST_TRANS_MS > 0)
		Sleep(M576_439F_POST_TRANS_MS);
	return TRUE;
}
