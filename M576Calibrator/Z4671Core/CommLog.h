#pragma once

#include <ctype.h>
#include <stdarg.h>

typedef void (__cdecl *M576CommLogFn)(LPCTSTR line, void* user);

struct M576CommLogTarget
{
	M576CommLogFn fn;
	void* user;

	M576CommLogTarget()
		: fn(NULL)
		, user(NULL)
	{
	}

	M576CommLogTarget(M576CommLogFn logFn, void* logUser)
		: fn(logFn)
		, user(logUser)
	{
	}

	BOOL IsEnabled() const
	{
		return fn != NULL;
	}

	void Emit(LPCTSTR line) const
	{
		if (fn != NULL)
			fn(line, user);
	}

	void EmitFormat(LPCTSTR format, ...) const
	{
		if (fn == NULL)
			return;
		va_list args;
		va_start(args, format);
		CString line;
		line.FormatV(format, args);
		va_end(args);
		fn(line.GetString(), user);
	}

	void EmitError(LPCTSTR format, ...) const
	{
		if (fn == NULL)
			return;
		va_list args;
		va_start(args, format);
		CString detail;
		detail.FormatV(format, args);
		va_end(args);
		CString line;
		line.Format(_T("[ERROR] %s"), detail.GetString());
		fn(line.GetString(), user);
	}

	void EmitInfo(LPCTSTR format, ...) const
	{
		if (fn == NULL)
			return;
		va_list args;
		va_start(args, format);
		CString detail;
		detail.FormatV(format, args);
		va_end(args);
		CString line;
		line.Format(_T("[INFO] %s"), detail.GetString());
		fn(line.GetString(), user);
	}

	void EmitRaw(LPCTSTR channel, DWORD seq, LPCTSTR direction, const CString& payload, DWORD elapsedMs = 0) const
	{
		if (fn == NULL)
			return;
		CString line;
		if (elapsedMs > 0)
			line.Format(_T("[%s] #%lu %s %s | %s | %lums"), channel, seq, direction, channel, payload.GetString(), elapsedMs);
		else
			line.Format(_T("[%s] #%lu %s %s | %s"), channel, seq, direction, channel, payload.GetString());
		fn(line.GetString(), user);
	}
	};

inline CString M576EscapeAscii(const CStringA& text)
{
	CString escaped;
	for (int i = 0; i < text.GetLength(); ++i)
	{
		const unsigned char ch = (unsigned char)text[i];
		if (ch == '\r')
			escaped += _T("\\r");
		else if (ch == '\n')
			escaped += _T("\\n");
		else if (ch == '\t')
			escaped += _T("\\t");
		else if (isprint(ch))
			escaped += (TCHAR)ch;
		else
		{
			CString t;
			t.Format(_T("\\x%02X"), ch);
			escaped += t;
		}
	}
	return escaped;
}

inline CString M576HexDump(const BYTE* data, int len)
{
	CString hex;
	for (int i = 0; i < len; ++i)
	{
		CString t;
		t.Format((i == 0) ? _T("%02X") : _T(" %02X"), data[i]);
		hex += t;
	}
	return hex;
}