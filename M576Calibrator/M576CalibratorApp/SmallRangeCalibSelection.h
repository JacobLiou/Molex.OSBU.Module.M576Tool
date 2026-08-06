#pragma once
// Small-range PM Run Path: dedupe FineTune channels and resolve to PM CSV whitelist.
// Pure logic (std::string / std::vector) so CrossPeakTest can cover without MFC dialogs.

#include "FineTuneTypes.h"

#include <string>
#include <vector>

/// One PM path step (same fields as SPathStep; kept free of MFC for CrossPeakTest).
struct SmallRangePmStep
{
	int targetSwitchIndex = 0;
	int c1 = 0;
	int c2 = 0;
	int c3 = 0;
	int c4 = 0;
};

/// One 1x64 Mapping row (same fields as SMems1x64PmMapRow).
struct SmallRangeMapRow
{
	int targetSwitchIndex = 0;
	int c1 = 0;
	int c2 = 0;
	int c3 = 0;
	int c4 = 0;
	int sw1to4 = 0;
	int chY1based = 0;
};

/// Resolved whitelist entry: which CSV fileSlot and 0-based row to run.
struct SmallRangeWhitelistEntry
{
	int fileSlot = 0;       ///< 0..3
	int stepIndex0 = 0;     ///< 0-based row in that slot's PM CSV
	FineTuneAddress addr{};
	SmallRangePmStep step{};
	std::string label;      ///< human-readable for logs
};

/// Append addr if not already present (identity by FineTuneAddressEqual). Returns true if added.
bool SmallRangeAppendUnique(std::vector<FineTuneAddress>& channels, const FineTuneAddress& addr);

/// True if MCS FineTune address matches PM step layout used by FineTune RECAL labels / pm_mcs*.csv.
bool SmallRangeMcsAddressMatchesPmStep(const FineTuneAddress& addr, const SmallRangePmStep& step);

/// True if 1x64 FineTune address matches Mapping.csv row (SW + CH_y).
bool SmallRangeOneX64AddressMatchesMapRow(const FineTuneAddress& addr, const SmallRangeMapRow& row);

/// Build expected MCS PM step for address (target/c1..c4). Returns false if invalid.
bool SmallRangeBuildMcsPmStep(const FineTuneAddress& addr, SmallRangePmStep& outStep);

/**
 * Resolve one FineTune address to a PM path step for RECAL 1 (no CSV row index).
 * - MCS: closed form via SmallRangeBuildMcsPmStep (mapRows ignored).
 * - 1x64: exactly one Mapping row matching SW/CH_y; copy that row's target/c1..c4.
 * Fail-closed on 0 or >1 Mapping hits.
 */
bool SmallRangeResolvePmStepFromAddress(
	const FineTuneAddress& addr,
	const std::vector<SmallRangeMapRow>& mapRows,
	SmallRangePmStep& outStep,
	std::string& errMsg);

/**
 * Resolve FineTune channels to an immutable PM whitelist.
 * - MCS: exact row at FineTuneMcsPmStepIndex0 in the device fileSlot; step must match expected c1..c4.
 * - 1x64: exactly one Mapping row with SW/CH_y; that row's (target,c1..c4) must appear exactly once in PM CSV.
 * Any missing/ambiguous match fails the whole resolve (no silent skip).
 *
 * @param channels FineTune addresses (may contain duplicates; resolved unique).
 * @param pmStepsBySlot [4] vectors of loaded PM CSV rows (may be empty if file missing).
 * @param mapRowsBySlot [4] Mapping rows; only slots 2/3 are used (1x64).
 * @param outWhitelist ordered unique (fileSlot, stepIndex0) entries.
 * @param errMsg diagnostic on failure.
 */
bool SmallRangeResolvePmWhitelist(
	const std::vector<FineTuneAddress>& channels,
	const std::vector<SmallRangePmStep> pmStepsBySlot[4],
	const std::vector<SmallRangeMapRow> mapRowsBySlot[4],
	std::vector<SmallRangeWhitelistEntry>& outWhitelist,
	std::string& errMsg);

/// True if (fileSlot, stepIndex0) is in whitelist.
bool SmallRangeWhitelistContains(
	const std::vector<SmallRangeWhitelistEntry>& whitelist,
	int fileSlot,
	int stepIndex0);

/// Count whitelist entries for one fileSlot (for progress totals).
int SmallRangeWhitelistCountForSlot(
	const std::vector<SmallRangeWhitelistEntry>& whitelist,
	int fileSlot);
