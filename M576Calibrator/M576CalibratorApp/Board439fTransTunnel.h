#pragma once
// 439F 多路透传：发 trans n 进某下位、$$ 出透明模式；其后再发 Z4671 二进制到当前隧道指向的设备。

#include "Z4671Command.h"

/// 439F serial passthrough: enter "trans <n>", leave "$$", then Z4671 binary to downstream.
// Begin=进入 trans 并排空；End=退出透传。transChannel 与产线 1#MCS/2#MCS/1#1x64/2#1x64 一致。
struct Board439fTransTunnel
{
	static BOOL EndTrans(Z4671Command& cmd, CString& err);
	static BOOL BeginTrans(Z4671Command& cmd, int transChannel, CString& err);
};
