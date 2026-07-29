#pragma once
// Run Path 实时追加 comm_*_recal_sweeps.csv（与 comm 日志同目录、按日命名）。

#include "PeakFinder2D.h"
#include <vector>

#ifndef M576_RECAL_SWEEP_CSV_MAX_POWER_COLS
#define M576_RECAL_SWEEP_CSV_MAX_POWER_COLS 128
#endif

/// `output\comm.log` → `output\comm_YYYY-MM-DD_recal_sweeps.csv`（与 CommLogPathForCurrentDay 同目录/日期规则）。
CString M576RecalSweepCsvRelPathForCurrentDay(LPCTSTR commLogPathRel);

const char* M576Peak1DValidateCodeShortName(M576::Peak1DValidateCode c);

/// 开始一次 Run Path（Path worker 入口）；`sweepCsvAbsPath` 为 ResolveFilePath 后的绝对路径。
BOOL M576RecalSweepCsvBeginRun(LPCTSTR sweepCsvAbsPath, BOOL isPm);
/// 每 Path Step 在 RECAL 1/2 OK 后设置（`pathColumn` 见计划格式）。
void M576RecalSweepCsvSetStepContext(LPCTSTR pathColumn);
/// 每次 RECAL 3/5 扫频解析并完成寻峰后追加一行（含失败 attempt）。
void M576RecalSweepCsvAppendRow(
	LPCTSTR cmdWire,
	double col0,
	const std::vector<double>& powers,
	int attempt1Based,
	BOOL peakOk,
	M576::Peak1DValidateCode code);
void M576RecalSweepCsvEndRun();
/// 当前会话 CSV 绝对路径；未 BeginRun 时为空。
CString M576RecalSweepCsvGetActiveAbsPath();
