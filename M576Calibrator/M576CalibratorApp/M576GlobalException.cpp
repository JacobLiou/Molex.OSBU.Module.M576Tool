#include "stdafx.h"
#include "M576GlobalException.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <mutex>

static std::mutex g_fatalLogMutex;
static LPTOP_LEVEL_EXCEPTION_FILTER g_prevSehFilter = NULL;

static CStringW M576GetExeFolderW()
{
	WCHAR sz[MAX_PATH] = {};
	const DWORD n = GetModuleFileNameW(NULL, sz, MAX_PATH);
	if (n == 0 || n >= MAX_PATH)
		return CStringW();
	CStringW path = sz;
	const int p = path.ReverseFind(L'\\');
	if (p > 0)
		return path.Left(p);
	return path;
}

static void M576EnsureOutputUnderExe(const CStringW& exeDir)
{
	if (exeDir.IsEmpty())
		return;
	(void)CreateDirectoryW((exeDir + L"\\output").GetString(), NULL);
}

static void M576AppendFatalLogUtf8Unlocked(const char* utf8Line, size_t len)
{
	if (!utf8Line || len == 0)
		return;
	const CStringW base = M576GetExeFolderW();
	if (base.IsEmpty())
		return;
	M576EnsureOutputUnderExe(base);
	const CStringW path = base + L"\\output\\m576_fatal.log";
	HANDLE h = CreateFileW(
		path,
		FILE_APPEND_DATA,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (h == INVALID_HANDLE_VALUE)
		return;
	LARGE_INTEGER fileSz = {};
	const BOOL has = GetFileSizeEx(h, &fileSz);
	if (has && fileSz.QuadPart == 0)
	{
		static const BYTE bom[3] = { 0xEF, 0xBB, 0xBF };
		DWORD w0 = 0;
		(void)WriteFile(h, bom, sizeof(bom), &w0, NULL);
	}
	DWORD wr = 0;
	(void)WriteFile(h, utf8Line, (DWORD)len, &wr, NULL);
	const char crlf[] = "\r\n";
	(void)WriteFile(h, crlf, 2, &wr, NULL);
	(void)CloseHandle(h);
}

void M576AppendFatalLogUtf8(const char* utf8Line)
{
	if (!utf8Line)
		return;
	const size_t n = std::strlen(utf8Line);
	if (n == 0)
		return;
	try
	{
		std::lock_guard<std::mutex> lock(g_fatalLogMutex);
		M576AppendFatalLogUtf8Unlocked(utf8Line, n);
	}
	catch (...)
	{
	}
}

static void M576AppendLocalTimestampPrefix(char* buf, size_t bufSize, const char* kind)
{
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	_snprintf_s(
		buf,
		bufSize,
		_TRUNCATE,
		"[%04u-%02u-%02u %02u:%02u:%02u] [%s] ",
		(unsigned)st.wYear,
		(unsigned)st.wMonth,
		(unsigned)st.wDay,
		(unsigned)st.wHour,
		(unsigned)st.wMinute,
		(unsigned)st.wSecond,
		kind);
}

static LONG WINAPI M576SehFilter(_EXCEPTION_POINTERS* ep)
{
	if (!ep || !ep->ExceptionRecord)
	{
		if (g_prevSehFilter)
			return g_prevSehFilter(ep);
		return EXCEPTION_CONTINUE_SEARCH;
	}
	const DWORD code = ep->ExceptionRecord->ExceptionCode;
	const void* addr = ep->ExceptionRecord->ExceptionAddress;
	char line[1024] = {};
	char head[80] = {};
	M576AppendLocalTimestampPrefix(head, sizeof head, "SEH");
	_snprintf_s(
		line,
		sizeof line,
		_TRUNCATE,
		"%s0x%08lX at %p (unhandled SEH)",
		head,
		(unsigned long)code,
		addr);
	try
	{
		std::lock_guard<std::mutex> lock(g_fatalLogMutex);
		M576AppendFatalLogUtf8Unlocked(line, std::strlen(line));
	}
	catch (...)
	{
	}
	MessageBoxW(
		NULL,
		L"A fatal error occurred (see output\\m576_fatal.log). The application will close.\n\n"
		L"Send the log and repro steps to the developer.",
		L"M576Calibrator - fatal error",
		MB_OK | MB_ICONERROR | MB_TOPMOST);
	if (g_prevSehFilter)
		(void)g_prevSehFilter(ep);
	return EXCEPTION_CONTINUE_SEARCH;
}

static void __cdecl M576TerminateHandler()
{
	char line[1024] = {};
	char head[80] = {};
	M576AppendLocalTimestampPrefix(head, sizeof head, "terminate");
	const std::exception_ptr cur = std::current_exception();
	if (cur)
	{
		try
		{
			std::rethrow_exception(cur);
		}
		catch (const std::exception& ex)
		{
			_snprintf_s(
				line,
				sizeof line,
				_TRUNCATE,
				"%sstd::terminate: %s",
				head,
				ex.what());
		}
		catch (...)
		{
			_snprintf_s(
				line,
				sizeof line,
				_TRUNCATE,
				"%sstd::terminate: non-std::exception",
				head);
		}
	}
	else
	{
		_snprintf_s(
			line,
			sizeof line,
			_TRUNCATE,
			"%sstd::terminate: no active exception",
			head);
	}
	if (line[0] == 0)
		_snprintf_s(line, sizeof line, _TRUNCATE, "%sstd::terminate", head);
	try
	{
		std::lock_guard<std::mutex> lock(g_fatalLogMutex);
		M576AppendFatalLogUtf8Unlocked(line, std::strlen(line));
	}
	catch (...)
	{
	}
	MessageBoxW(
		NULL,
		L"Unhandled C++ exception caused std::terminate.\n"
		L"Details: output\\m576_fatal.log",
		L"M576Calibrator - unhandled C++",
		MB_OK | MB_ICONERROR | MB_TOPMOST);
	std::abort();
}

#if defined(_MSC_VER)
static void __cdecl M576InvalidParameter(
	const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t /*pReserved*/)
{
	(void)expression;
	char linebuf[2048] = {};
	char head[80] = {};
	M576AppendLocalTimestampPrefix(head, sizeof head, "invalid_param");
	_snprintf_s(
		linebuf,
		sizeof linebuf,
		_TRUNCATE,
		"%sfile=%ls line=%u func=%ls",
		head,
		file ? file : L"?", line,
		function ? function : L"?");
	if (g_fatalLogMutex.try_lock())
	{
		M576AppendFatalLogUtf8Unlocked(linebuf, std::strlen(linebuf));
		g_fatalLogMutex.unlock();
	}
}

static void __cdecl M576PureCallHandler()
{
	const char* msg = "[purecall] pure virtual function call";
	M576AppendFatalLogUtf8(msg);
	MessageBoxW(
		NULL, L"Runtime error: pure virtual call. See output\\m576_fatal.log", L"M576Calibrator", MB_OK | MB_ICONERROR);
	std::abort();
}
#endif

void M576InstallGlobalExceptionHooks()
{
	static bool done = false;
	if (done)
		return;
	done = true;
	g_prevSehFilter = SetUnhandledExceptionFilter(M576SehFilter);
	std::set_terminate(M576TerminateHandler);
#if defined(_MSC_VER)
	_set_invalid_parameter_handler(M576InvalidParameter);
	_set_purecall_handler(M576PureCallHandler);
#endif
}
