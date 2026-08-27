#pragma once
// FullEdit CSV parse / diff (no MFC, no bin writers). CrossPeakTest-friendly.

#include "FullEditDacTypes.h"

#include <string>
#include <vector>

/// Parse MCS working/baseline CSV text (UTF-8, optional BOM, # comments).
FullEditErrorCode FullEditParseMcsCsv(
	const std::string& text,
	std::vector<FullEditMcsCsvRow>& outRows,
	std::string& errMsg);

FullEditErrorCode FullEditParseMemsCsv(
	const std::string& text,
	std::vector<FullEditMemsCsvRow>& outRows,
	std::string& errMsg);

/// Diff working vs baseline; emit patches. Fails on dangerous locked rows.
FullEditErrorCode FullEditDiffMcsCsv(
	const std::vector<FullEditMcsCsvRow>& working,
	const std::vector<FullEditMcsCsvRow>& baseline,
	const FullEditUnlockFlags& unlock,
	std::vector<FullEditMcsPatch>& outPatches,
	std::string& errMsg);

FullEditErrorCode FullEditDiffMemsCsv(
	const std::vector<FullEditMemsCsvRow>& working,
	const std::vector<FullEditMemsCsvRow>& baseline,
	const FullEditUnlockFlags& unlock,
	std::vector<FullEditMemsPatch>& outPatches,
	std::string& errMsg);

/// Build MCS CSV body (no BOM) from rows; caller may prepend BOM.
std::string FullEditFormatMcsCsv(
	int burnIndex,
	const std::string& snLabel,
	const std::vector<FullEditMcsCsvRow>& rows);

std::string FullEditFormatMemsCsv(
	int burnIndex,
	const std::string& snLabel,
	int sw1to4,
	const std::vector<FullEditMemsCsvRow>& rows);
