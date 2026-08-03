#pragma once

#include "Z4671Command.h"
#include <cmath>

/// Five temperature points for IL / FIM IL monitoring (main board + four sub-boards).
struct M576FiveTemps
{
	double mainC = NAN;
	double subC[4] = { NAN, NAN, NAN, NAN };       // MCS1, MCS2, 1x64#1, 1x64#2 (box)
	double edfaOut1C[4] = { NAN, NAN, NAN, NAN }; // unused (kept for API)
	bool hasMain = false;
	bool hasSub[4] = {};
	bool hasEdfa[4] = {};
};

/// ASCII `MT` on 439F management port; reply integer is degC * 10 (259 -> 25.9).
BOOL M576ReadMainBoardTempC(Z4671Command& cmd, double& outC, CString& err);

/// `trans N` then: MCS (1/2) GetMCSTemp; 1x64 (3/4) ASCII `GTMP\r` (°C = reply/100).
/// Retries whole tunnel+read up to M576_TEMP_SUB_RETRY_MAX (flaky 439F / Invalid command). edfaOut1C unset.
BOOL M576ReadSubBoardBoxTempC(
	Z4671Command& cmd,
	int trans1to4,
	double& boxC,
	double& edfaOut1C,
	CString& err);

/// Read all five; per-channel failures leave has*=false and continue.
BOOL M576ReadAllFiveTempsC(Z4671Command& cmd, M576FiveTemps& out, CString& errDetail);

/// UI line: "Temp: Main 25.9  MCS1 25.0  MCS2 --.-  1x64#1 25.0  1x64#2 25.0 C"
CString M576FormatFiveTempsUiLine(const M576FiveTemps& t);

/// Log lines (may include EDFA aux). Caller posts to dialog log.
void M576AppendFiveTempsLogLines(const M576FiveTemps& t, CStringArray& outLines);
