#pragma once

#include "Z4671Command.h"

/// 439F serial passthrough: enter "trans <n>", leave "$$", then Z4671 binary to downstream.
struct Board439fTransTunnel
{
	static BOOL EndTrans(Z4671Command& cmd, CString& err);
	static BOOL BeginTrans(Z4671Command& cmd, int transChannel, CString& err);
};
