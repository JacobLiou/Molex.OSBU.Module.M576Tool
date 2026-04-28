#pragma once
// Peak grid to DAC; writes MCS Z4671 or 1x64 four 2K Mems blocks (low-temp stChnDAC).

#include "Z4767StructDefine.h"
#include "M576OneX64Coef.h"
#include "PathCsvDriver.h"

// step = 2*halfRange/(n-1). X = xGridStart + col*step, Y = yGridStart + row*step. Negative col0 OK; result via 12-bit ring to U16.
void PeakGridToDacWord(
	int peakRow,
	int peakCol,
	int n,
	int halfRange,
	double yGridStart,
	double xGridStart,
	unsigned short& outDacX,
	unsigned short& outDacY);

void ApplyRecalPeakToLut(
	const SPathStep& step,
	int occTarget3,
	int occTarget4,
	int gridN,
	int halfRange,
	double yGridStart,
	double xGridStart,
	int peakRow,
	int peakCol,
	stLutSettingZ4671& lut);

void ApplyRecalPeakToLutPd(
	const SPathStepPd& step,
	int occTarget3,
	int occTarget4,
	int gridN,
	int halfRange,
	double yGridStart,
	double xGridStart,
	int peakRow,
	int peakCol,
	stLutSettingZ4671& lut);

void ApplyRecalPeakToMems1x64(
	const SPathStep& step,
	int occTarget3,
	int occTarget4,
	int gridN,
	int halfRange,
	double yGridStart,
	double xGridStart,
	int peakRow,
	int peakCol,
	stM576OneX64MemsSwCoef* pSw4);

void ApplyRecalPeakToMems1x64Pd(
	const SPathStepPd& step,
	int occTarget3,
	int occTarget4,
	int gridN,
	int halfRange,
	double yGridStart,
	double xGridStart,
	int peakRow,
	int peakCol,
	stM576OneX64MemsSwCoef* pSw4);
