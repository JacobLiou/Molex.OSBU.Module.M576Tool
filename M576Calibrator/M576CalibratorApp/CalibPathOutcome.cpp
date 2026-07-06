#include "CalibPathOutcome.h"
#include "Peak1DSweepRecenter.h"
// CalibPathOutcome.cpp：Run Path 单步失败分类、汇总统计与摘要文本（供汇总对话框/导出）。

#include <cstdio>
#include <cstring>

// ---------- 失败类别与结果标签 ----------
const char* CalibPathFailCategoryLabelA(CalibPathFailCategory c)
{
	switch (c)
	{
	case CalibPathFailCategory::CsvValidation: return "CSV/validation skip";
	case CalibPathFailCategory::FileMappingSkip: return "1x64 mapping skip";
	case CalibPathFailCategory::CommRecalSetup: return "RECAL setup (1/2)";
	case CalibPathFailCategory::CommSweep: return "sweep comm/parse";
	case CalibPathFailCategory::CommSerialBreak: return "serial break";
	case CalibPathFailCategory::YPrePeak: return "Y pre-sweep peak";
	case CalibPathFailCategory::YCrossPeak: return "cross Y peak";
	case CalibPathFailCategory::XCrossPeak: return "cross X peak";
	case CalibPathFailCategory::PmRangeMismatch: return "PM range mismatch";
	case CalibPathFailCategory::SweepDataMismatch: return "Y/X data mismatch";
	case CalibPathFailCategory::PeakPipelineExhausted: return "peak pipeline exhausted";
	case CalibPathFailCategory::UserStop: return "user stop";
	default: return "unknown";
	}
}

const wchar_t* CalibPathFailCategoryLabelW(CalibPathFailCategory c)
{
	switch (c)
	{
	case CalibPathFailCategory::CsvValidation: return L"CSV/validation skip";
	case CalibPathFailCategory::FileMappingSkip: return L"1x64 mapping skip";
	case CalibPathFailCategory::CommRecalSetup: return L"RECAL setup (1/2)";
	case CalibPathFailCategory::CommSweep: return L"sweep comm/parse";
	case CalibPathFailCategory::CommSerialBreak: return L"serial break";
	case CalibPathFailCategory::YPrePeak: return L"Y pre-sweep peak";
	case CalibPathFailCategory::YCrossPeak: return L"cross Y peak";
	case CalibPathFailCategory::XCrossPeak: return L"cross X peak";
	case CalibPathFailCategory::PmRangeMismatch: return L"PM range mismatch";
	case CalibPathFailCategory::SweepDataMismatch: return L"Y/X data mismatch";
	case CalibPathFailCategory::PeakPipelineExhausted: return L"peak pipeline exhausted";
	case CalibPathFailCategory::UserStop: return L"user stop";
	default: return L"unknown";
	}
}

const char* CalibPathStepResultLabelA(CalibPathStepResult r)
{
	return (r == CalibPathStepResult::Skipped) ? "skipped" : "failed";
}

// ---------- 扫频轮廓与 Run Path 汇总 ----------
void TruncatePathOutcomeDetail(std::string& s, size_t maxLen)
{
	if (s.size() > maxLen)
		s.resize(maxLen);
}

void FillSweepProfileContext(
	const M576::SweepProfile& profile,
	M576::SweepTrend& outTrend,
	double& outSpan,
	int& outArgmax)
{
	outTrend = profile.trend;
	outSpan = profile.span;
	outArgmax = profile.argmaxIndex;
}

SRunPathSummary BuildRunPathSummary(
	const std::vector<SCalibPathStepOutcome>& failureRows,
	int successCount,
	bool isPm,
	bool userStopped)
{
	SRunPathSummary s = {};
	s.isPm = isPm;
	s.userStopped = userStopped;
	s.successCount = (successCount >= 0) ? successCount : 0;
	s.failureRows = failureRows;
	for (const SCalibPathStepOutcome& row : failureRows)
	{
		if (row.result == CalibPathStepResult::Skipped)
			++s.skippedCount;
		else
			++s.failedCount;
		++s.failByCategory[row.failCategory];
	}
	return s;
}

static void AppendCategoryLine(std::string& out, const char* label, int count)
{
	if (count <= 0)
		return;
	char buf[128];
	std::snprintf(buf, sizeof(buf), "  %s: %d\r\n", label, count);
	out += buf;
}

std::string FormatRunPathCategoryBreakdownText(const SRunPathSummary& summary)
{
	if (summary.failedCount + summary.skippedCount <= 0)
		return std::string();
	std::string out;
	const int pmRange = summary.failByCategory.count(CalibPathFailCategory::PmRangeMismatch)
		? summary.failByCategory.at(CalibPathFailCategory::PmRangeMismatch)
		: 0;
	const int yPre = summary.failByCategory.count(CalibPathFailCategory::YPrePeak)
		? summary.failByCategory.at(CalibPathFailCategory::YPrePeak)
		: 0;
	const int yCross = summary.failByCategory.count(CalibPathFailCategory::YCrossPeak)
		? summary.failByCategory.at(CalibPathFailCategory::YCrossPeak)
		: 0;
	const int xCross = summary.failByCategory.count(CalibPathFailCategory::XCrossPeak)
		? summary.failByCategory.at(CalibPathFailCategory::XCrossPeak)
		: 0;
	int comm = 0;
	for (const auto& kv : summary.failByCategory)
	{
		switch (kv.first)
		{
		case CalibPathFailCategory::CommRecalSetup:
		case CalibPathFailCategory::CommSweep:
		case CalibPathFailCategory::CommSerialBreak:
			comm += kv.second;
			break;
		default:
			break;
		}
	}
	AppendCategoryLine(out, "PM range / power", pmRange);
	if (yPre + yCross + xCross > 0)
	{
		char buf[128];
		std::snprintf(buf, sizeof(buf), "  Peak retry not found (Ypre=%d Ycross=%d Xcross=%d): %d",
			yPre, yCross, xCross, yPre + yCross + xCross);
		out += buf;
		out += "\r\n";
	}
	AppendCategoryLine(out, "Comm / sweep", comm);
	for (const auto& kv : summary.failByCategory)
	{
		if (kv.second <= 0)
			continue;
		switch (kv.first)
		{
		case CalibPathFailCategory::PmRangeMismatch:
		case CalibPathFailCategory::YPrePeak:
		case CalibPathFailCategory::YCrossPeak:
		case CalibPathFailCategory::XCrossPeak:
		case CalibPathFailCategory::CommRecalSetup:
		case CalibPathFailCategory::CommSweep:
		case CalibPathFailCategory::CommSerialBreak:
			continue;
		default:
			AppendCategoryLine(out, CalibPathFailCategoryLabelA(kv.first), kv.second);
			break;
		}
	}
	return out;
}

std::string FormatRunPathSummaryHeaderText(const SRunPathSummary& summary)
{
	char buf[256];
	std::snprintf(
		buf,
		sizeof(buf),
		"%s | success %d | failed %d | skipped %d%s",
		summary.isPm ? "PM" : "PD",
		summary.successCount,
		summary.failedCount,
		summary.skippedCount,
		summary.userStopped ? " | STOP requested" : "");
	return std::string(buf);
}
