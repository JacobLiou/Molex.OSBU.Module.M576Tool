#pragma once
// Pairs with LutPeakApply: MCS wCalibPtrDAC(WORD, byte off) and 1x64 stM576OneX64AxisDAC(SHORT); row metadata for CSV and trans .bin field offsets.

#include "PathCsvDriver.h"
#include "LutBinWriter.h"
#include "M576OneX64Coef.h"
#include "Z4767StructDefine.h"

#include <vector>

struct SCalibrationStatRow
{
	CString calMode;
	int transSlot1to4{ 0 };
	int pathLine1Based{ 0 };
	CString primaryCommand;
	CString targetType; // MCS_LUT or 1X64_MEMS
	int peakRow{ 0 };
	int peakCol{ 0 };
	int gridN{ 0 };
	CString storeType; // WORD (MCS) or SHORT (1x64 stAxisDAC in bin)
	/// Linear DAC at cross-peak (before 12b ring): RECAL3/5 axis0 col0 + row*step, axis1 col0 + col*step (same grid as PeakGridToDacWord).
	double rawDacX{ 0.0 };
	double rawDacY{ 0.0 };
	int dacX{ 0 };
	int dacY{ 0 };
	CString structPathDacX;
	CString structPathDacY;
	/// Byte offset in stLutSettingZ4671, or in 1x64 8K mems file (0..8191) for the DAC X/Y field.
	SIZE_T offsetLutOrMems8kDacX{ 0 };
	SIZE_T offsetLutOrMems8kDacY{ 0 };
	/// Offset in the on-disk per-trans .bin: MCS = LutPayload + offset in struct; 1x64 = same as 8K for pure mems file.
	SIZE_T offsetTransBinDacX{ 0 };
	SIZE_T offsetTransBinDacY{ 0 };
};

struct SDacU16
{
	unsigned short uX{ 0 };
	unsigned short uY{ 0 };
};

BOOL CalibBuildStatRowPmLut(
	const SPathStep& step,
	int occT3,
	int occT4,
	int fileSlot,
	int pathLine1Based,
	int peakRow,
	int peakCol,
	int gridN,
	double rawDacX,
	double rawDacY,
	SDacU16 dac,
	SCalibrationStatRow& row);

BOOL CalibBuildStatRowPmMems(
	const SPathStep& step,
	int fileSlot,
	int pathLine1Based,
	int peakRow,
	int peakCol,
	int gridN,
	double rawDacX,
	double rawDacY,
	SDacU16 dac,
	SCalibrationStatRow& row);

BOOL CalibBuildStatRowPdLut(
	const SPathStepPd& step,
	int occT3,
	int occT4,
	int fileSlot,
	int pathLine1Based,
	int peakRow,
	int peakCol,
	int gridN,
	double rawDacX,
	double rawDacY,
	SDacU16 dac,
	SCalibrationStatRow& row);

BOOL CalibBuildStatRowPdMems(
	const SPathStepPd& step,
	int fileSlot,
	int pathLine1Based,
	int peakRow,
	int peakCol,
	int gridN,
	double rawDacX,
	double rawDacY,
	SDacU16 dac,
	SCalibrationStatRow& row);

/// UTF-8 BOM + CSV, one header block then rows.
BOOL WriteCalibrationStatsCsv(LPCTSTR path, const std::vector<SCalibrationStatRow>& rows, CString& err);
