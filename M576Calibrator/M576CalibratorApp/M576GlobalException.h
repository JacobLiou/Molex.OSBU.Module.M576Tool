#pragma once
// Install at process start (InitInstance): SEH, std::terminate, MSVC purecall/invalid-param; log to output\m576_fatal.log
#include <stddef.h>

void M576InstallGlobalExceptionHooks();
/// Optional: append one UTF-8 line (with trailing \\r\\n inside) to the same crash log; thread-safe.
void M576AppendFatalLogUtf8(const char* utf8Line);
/// Writes "[YYYY-MM-DD HH:MM:SS] [kind] " into buf (UTF-8 ASCII). kind may be empty -> "[ts] ".
void M576FormatLocalTimestampPrefix(char* buf, size_t bufSize, const char* kind);
