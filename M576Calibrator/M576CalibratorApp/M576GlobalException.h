#pragma once
// Install at process start (InitInstance): SEH, std::terminate, MSVC purecall/invalid-param; log to output\m576_fatal.log

void M576InstallGlobalExceptionHooks();
/// Optional: append one UTF-8 line (with trailing \\r\\n inside) to the same crash log; thread-safe.
void M576AppendFatalLogUtf8(const char* utf8Line);
