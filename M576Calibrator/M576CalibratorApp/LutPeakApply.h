#pragma once
// Cross-peak raw DACs -> WORD/SHORT in MCS LUT or 1x64 mems (low-temp stChnDAC). Same lround+int16 as CalibWriteMeta CSV.

#include "Z4767StructDefine.h"
#include "M576OneX64Coef.h"
#include "PathCsvDriver.h"

/// `rawDacX` / `rawDacY` (lrounded linear DAC; CSV raw columns) -> int16 U16. MCS: Y at wCalibPtrDAC[][0], X at [][1]. Mems: Y in sDACx, X in sDACy.
void RawCrossPeakDacToU16Pair(double rawDacX, double rawDacY, unsigned short& outDacX, unsigned short& outDacY);

void ApplyRecalPeakToLut(
	const SPathStep& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	stLutSettingZ4671& lut);

void ApplyRecalPeakToLutPd(
	const SPathStepPd& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	stLutSettingZ4671& lut);

void ApplyRecalPeakToMems1x64(
	const SPathStep& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	stM576OneX64MemsSwCoef* pSw4);

void ApplyRecalPeakToMems1x64Pd(
	const SPathStepPd& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	stM576OneX64MemsSwCoef* pSw4);
