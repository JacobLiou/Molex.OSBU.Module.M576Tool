#include "stdafx.h"
#include "Board439fTransTunnel.h"
#include "CalibConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace {

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

BOOL Board439fTransTunnel::EndTrans(Z4671Command& cmd, CString& err)
{
	err.Empty();
	static const char kEnd[] = "$$\r\n";
	cmd.TraceInfo(_T("439F"), _T("TRANS end: send $$"));
	if (!cmd.WriteBufferNoPurge((char*)kEnd, (DWORD)(sizeof(kEnd) - 1)))
	{
		err = _T("439F: write $$ failed.");
		cmd.TraceError(_T("439F"), _T("%s"), err.GetString());
		return FALSE;
	}
	CStringA echo;
	DrainAscii(cmd, M576_439F_TRANS_END_DRAIN_MAX_MS, M576_439F_TRANS_DRAIN_IDLE_MS, echo);
	if (!echo.IsEmpty())
		cmd.TraceInfo(_T("439F"), _T("TRANS end drain: %s"), CString(echo).GetString());
	return TRUE;
}

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
	if (n <= 0 || !cmd.WriteBufferNoPurge((char*)(LPCSTR)line, (DWORD)n))
	{
		err.Format(_T("439F: write trans %d failed."), transChannel);
		cmd.TraceError(_T("439F"), _T("%s"), err.GetString());
		return FALSE;
	}
	CStringA echo;
	DrainAscii(cmd, M576_439F_TRANS_DRAIN_MAX_MS, M576_439F_TRANS_DRAIN_IDLE_MS, echo);
	if (!echo.IsEmpty())
		cmd.TraceInfo(_T("439F"), _T("TRANS begin drain: %s"), CString(echo).GetString());
	if (M576_439F_POST_TRANS_MS > 0)
		Sleep(M576_439F_POST_TRANS_MS);
	return TRUE;
}
