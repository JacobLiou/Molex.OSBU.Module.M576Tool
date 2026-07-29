#pragma once

#include "PeakFinder2D.h"
#include "Peak1DSweepRecenter.h"

#include <map>
#include <string>
#include <vector>

enum class CalibPathStepResult
{
	Failed,
	Skipped,
};

enum class CalibPathFailCategory
{
	CsvValidation,
	FileMappingSkip,
	CommRecalSetup,
	CommSweep,
	CommSerialBreak,
	YPrePeak,
	YCrossPeak,
	XCrossPeak,
	PmRangeMismatch,
	SweepDataMismatch,
	PeakPipelineExhausted,
	UserStop,
};

struct SCalibPathStepOutcome
{
	bool isPm = true;
	int fileSlot = 0;
	std::string csvPath;
	int pathLine1Based = 0;
	int targetSwitchIndex = 0;
	std::string routeLabel;
	CalibPathStepResult result = CalibPathStepResult::Failed;
	CalibPathFailCategory failCategory = CalibPathFailCategory::CommSweep;
	std::string failStage;
	M576::Peak1DValidateCode peakCode = M576::Peak1DValidateCode::Empty;
	std::string peakCodeText;
	int peakAttempts = 0;
	int crossRound = 0;
	int lastBaseY = 0;
	int lastBaseX = 0;
	int lastOffsetY = 0;
	int lastOffsetX = 0;
	double sweepCol0Y = 0.0;
	double sweepCol0X = 0.0;
	int sampleCountY = 0;
	int sampleCountX = 0;
	M576::SweepTrend sweepTrendY = M576::SweepTrend::Unknown;
	M576::SweepTrend sweepTrendX = M576::SweepTrend::Unknown;
	double spanY = 0.0;
	double spanX = 0.0;
	int argmaxY = -1;
	int argmaxX = -1;
	double tPeakY = 0.0;
	double tPeakX = 0.0;
	bool hasTPeakY = false;
	bool hasTPeakX = false;
	double peakDbm = 0.0;
	double loDbm = 0.0;
	double hiDbm = 0.0;
	int pmRangeIndex = -1;
	int peakIdxUsed = -1;
	std::string commDetail;
};

struct SRunPathSummary
{
	bool isPm = true;
	bool userStopped = false;
	int successCount = 0;
	int failedCount = 0;
	int skippedCount = 0;
	std::map<CalibPathFailCategory, int> failByCategory;
	std::vector<SCalibPathStepOutcome> failureRows;
};

const char* CalibPathFailCategoryLabelA(CalibPathFailCategory c);
const wchar_t* CalibPathFailCategoryLabelW(CalibPathFailCategory c);
const char* CalibPathStepResultLabelA(CalibPathStepResult r);

SRunPathSummary BuildRunPathSummary(
	const std::vector<SCalibPathStepOutcome>& failureRows,
	int successCount,
	bool isPm,
	bool userStopped);

std::string FormatRunPathCategoryBreakdownText(const SRunPathSummary& summary);
std::string FormatRunPathSummaryHeaderText(const SRunPathSummary& summary);

void TruncatePathOutcomeDetail(std::string& s, size_t maxLen = 240);

void FillSweepProfileContext(
	const M576::SweepProfile& profile,
	M576::SweepTrend& outTrend,
	double& outSpan,
	int& outArgmax);
