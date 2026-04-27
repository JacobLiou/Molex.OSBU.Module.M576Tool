#pragma once

// Polly-style retry helper for M576 (Z4671Core, no CalibConstants dependency).
// Defaults match M576CalibratorApp CalibConstants.h.
#ifndef M576_COMM_RETRY_MAX_ATTEMPTS
#define M576_COMM_RETRY_MAX_ATTEMPTS 4
#endif
#ifndef M576_COMM_RETRY_DELAY_MS
#define M576_COMM_RETRY_DELAY_MS 100U
#endif

#include <windows.h>

/// Run `op()` up to `maxAttempts` times until it returns true. Fixed delay between failures.
/// `op` must be invocable with no args and return bool (convertible).
template <typename Op>
inline bool M576WithRetry(int maxAttempts, DWORD delayMs, Op op)
{
	if (maxAttempts < 1)
		maxAttempts = 1;
	for (int attempt = 1; attempt <= maxAttempts; ++attempt)
	{
		if (op())
			return true;
		if (attempt < maxAttempts && delayMs > 0)
			::Sleep(delayMs);
	}
	return false;
}

/// Same with optional per-failure callback `onFail(attemptIndex1Based, maxAttempts)` after a failed `op` (not called after the last fail).
template <typename Op, typename OnFail>
inline bool M576WithRetry(int maxAttempts, DWORD delayMs, Op op, OnFail onFail)
{
	if (maxAttempts < 1)
		maxAttempts = 1;
	for (int attempt = 1; attempt <= maxAttempts; ++attempt)
	{
		if (op())
			return true;
		if (attempt < maxAttempts)
		{
			onFail(attempt, maxAttempts);
			if (delayMs > 0)
				::Sleep(delayMs);
		}
	}
	return false;
}
