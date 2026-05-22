#pragma once
// Cross-peak raw DACs -> WORD/SHORT in MCS LUT or 1x64 mems. Same lround+int16 as CalibWriteMeta CSV.

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
	int mcsTempSlot,
	stLutSettingZ4671& lut);

void ApplyRecalPeakToLutPd(
	const SPathStepPd& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	int mcsTempSlot,
	stLutSettingZ4671& lut);

void WriteMems1x64DacPair(
	stM576OneX64MemsSwCoef* pSw4,
	int block0to3,
	int inBlk0based,
	unsigned short dacX,
	unsigned short dacY,
	int calibSlot);

inline void WriteMems1x64LowTempDacPair(
	stM576OneX64MemsSwCoef* pSw4,
	int block0to3,
	int inBlk0based,
	unsigned short dacX,
	unsigned short dacY)
{
	WriteMems1x64DacPair(pSw4, block0to3, inBlk0based, dacX, dacY, 0);
}

void ApplyRecalPeakToMems1x64(
	const SPathStep& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	int calibSlot,
	stM576OneX64MemsSwCoef* pSw4);

void ApplyRecalPeakToMems1x64Pd(
	const SPathStepPd& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	int calibSlot,
	stM576OneX64MemsSwCoef* pSw4);
