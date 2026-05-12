#include "stdafx.h"
#include "LutPeakApply.h"
#include "CalibConstants.h"
#include <algorithm>
#include <cmath>
// MCS -> Z4671 LUT; 1x64 -> 126S stMemsSwCoef (four 2K blocks, low-temp stChnDAC).
// Clamped to int16, stored as 16-bit two's-complement in WORD/SHORT (MCS and 1x64).

static short U16ToShortDac(unsigned short w)
{
	return (short)w;
}

// Clamp to int16 range, then re-interpret as U16 (two's-complement bit pattern) for wCalibPtrDAC and mems sDAC* writes.
static unsigned short ClampedS16ToU16(int v)
{
	v = std::clamp(v, -32768, 32767);
	return (unsigned short)(short)v;
}

void RawCrossPeakDacToU16Pair(double rawDacX, double rawDacY, unsigned short& outDacX, unsigned short& outDacY)
{
	outDacX = ClampedS16ToU16((int)std::lround(rawDacX));
	outDacY = ClampedS16ToU16((int)std::lround(rawDacY));
}

// Logical cross-peak dacX/dacY from RECAL0/1 sweeps. Firmware: [0] and sDACx hold Y; [1] and sDACy hold X.
static void WriteDacPair(stLutSettingZ4671& lut, int swIdx, int chIdx, unsigned short dacX, unsigned short dacY)
{
	if (swIdx < 0 || swIdx >= 34)
		return;
	if (chIdx < 0 || chIdx >= PORT_MAX_COUNT + MID_MAX_COUNT)
		return;
	lut.wCalibPtrDAC[swIdx][IDX_TEMP_LOW][chIdx][0] = dacY;
	lut.wCalibPtrDAC[swIdx][IDX_TEMP_LOW][chIdx][1] = dacX;
}

void ApplyRecalPeakToLut(
	const SPathStep& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	stLutSettingZ4671& lut)
{
	const int t = step.targetSwitchIndex;

	if (t == 3)
	{
		(void)occTarget3;
		if (step.c1 < 1 || step.c1 > 32 || step.c2 < 1 || step.c2 > (PORT_MAX_COUNT + MID_MAX_COUNT))
			return;
		const int swIdx = M576McsBlock1To32ToLutSwIdx0(step.c1);
		const int chIdx = step.c2 - 1;
		WriteDacPair(lut, swIdx, chIdx, dacX, dacY);
		return;
	}
	if (t == 4)
	{
		(void)occTarget4;
		if (step.c1 < 1 || step.c1 > 32 || step.c2 < 1 || step.c2 > (PORT_MAX_COUNT + MID_MAX_COUNT))
			return;
		int swIdx = M576McsBlock1To32ToLutSwIdx0(step.c1) + M576_MCS2_SW_INDEX_OFFSET;
		if (swIdx > 33)
			swIdx = 33;
		const int chIdx = step.c2 - 1;
		WriteDacPair(lut, swIdx, chIdx, dacX, dacY);
		return;
	}

	if (t == 1 || t == 2)
	{
		int ch1 = step.c1;
		if (ch1 < 1 || ch1 > 64)
			return;
		const int ch0 = ch1 - 1;
		const int swSlot = (ch0 < 32) ? 32 : 33;
		const int chIdx = ch0 % 32;
		WriteDacPair(lut, swSlot, chIdx, dacX, dacY);
		return;
	}

	if (t == 5 || t == 6)
	{
		int ch4 = step.c4;
		if (ch4 < 1 || ch4 > 64)
			return;
		const int ch0 = ch4 - 1;
		const int swSlot = (ch0 < 32) ? 32 : 33;
		const int chIdx = ch0 % 32;
		WriteDacPair(lut, swSlot, chIdx, dacX, dacY);
		return;
	}
}

void WriteMems1x64LowTempDacPair(
	stM576OneX64MemsSwCoef* pSw4,
	int block0to3,
	int inBlk0based,
	unsigned short dacX,
	unsigned short dacY)
{
	if (pSw4 == NULL)
		return;
	if (block0to3 < 0 || block0to3 > 3)
		return;
	if (inBlk0based < 0 || inBlk0based >= (int)M576_1X64_MAX_CHANNEL_NUM)
		return;
	stM576OneX64ChnDAC& d = pSw4[block0to3].stCalibDAC[0];
	d.stChnDAC[inBlk0based].sDACx = U16ToShortDac(dacY);
	d.stChnDAC[inBlk0based].sDACy = U16ToShortDac(dacX);
	(void)d.wValid;
	(void)d.sTemperature;
}

void ApplyRecalPeakToMems1x64(
	const SPathStep& step,
	int /*occTarget3*/,
	int /*occTarget4*/,
	unsigned short dacX,
	unsigned short dacY,
	stM576OneX64MemsSwCoef* pSw4)
{
	if (pSw4 == NULL)
		return;
	const int t = step.targetSwitchIndex;
	// 1x64: targets 1,2,5,6 only (pm_1x64_*.csv); 3/4 are MCS
	if (t == 3 || t == 4)
		return;

	auto setPort = [&](int ch1to64)
	{
		if (ch1to64 < 1 || ch1to64 > 64)
			return;
		const int ch0 = ch1to64 - 1;
		const int block = ch0 / 16;
		const int inBlk = ch0 % 16;
		if (block < 0 || block > 3)
			return;
		stM576OneX64ChnDAC& d = pSw4[block].stCalibDAC[0];
		d.stChnDAC[inBlk].sDACx = U16ToShortDac(dacY);
		d.stChnDAC[inBlk].sDACy = U16ToShortDac(dacX);
		(void)d.wValid;
		(void)d.sTemperature;
	};

	if (t == 1 || t == 2)
		setPort(step.c1);
	else if (t == 5 || t == 6)
		setPort(step.c4);
}

void ApplyRecalPeakToLutPd(
	const SPathStepPd& step,
	int occTarget3,
	int occTarget4,
	unsigned short dacX,
	unsigned short dacY,
	stLutSettingZ4671& lut)
{
	const int t = step.targetSwitchIndex;

	if (t == 3)
	{
		(void)occTarget3;
		if (step.ch1 < 1 || step.ch1 > 32 || step.ch2 < 1 || step.ch2 > (PORT_MAX_COUNT + MID_MAX_COUNT))
			return;
		const int swIdx = M576McsBlock1To32ToLutSwIdx0(step.ch1);
		const int chIdx = step.ch2 - 1;
		WriteDacPair(lut, swIdx, chIdx, dacX, dacY);
		return;
	}
	if (t == 4)
	{
		(void)occTarget4;
		if (step.ch1 < 33 || step.ch1 > 64 || step.ch2 < 1 || step.ch2 > (PORT_MAX_COUNT + MID_MAX_COUNT))
			return;
		const int block = step.ch1 - 32;
		int swIdx = M576McsBlock1To32ToLutSwIdx0(block) + M576_MCS2_SW_INDEX_OFFSET;
		if (swIdx > 33)
			swIdx = 33;
		const int chIdx = step.ch2 - 1;
		WriteDacPair(lut, swIdx, chIdx, dacX, dacY);
		return;
	}

	if (t == 1 || t == 2)
	{
		int ch64 = step.ch1;
		if (ch64 < 1 || ch64 > 64)
			return;
		const int ch0 = ch64 - 1;
		const int swSlot = (ch0 < 32) ? 32 : 33;
		const int chIdx = ch0 % 32;
		WriteDacPair(lut, swSlot, chIdx, dacX, dacY);
		return;
	}
}

void ApplyRecalPeakToMems1x64Pd(
	const SPathStepPd& step,
	int /*occTarget3*/,
	int /*occTarget4*/,
	unsigned short dacX,
	unsigned short dacY,
	stM576OneX64MemsSwCoef* pSw4)
{
	if (pSw4 == NULL)
		return;
	const int t = step.targetSwitchIndex;
	if (t == 3 || t == 4)
		return;
	if (t != 1 && t != 2)
		return;
	if (step.ch1 < 1 || step.ch1 > 64)
		return;
	const int ch0 = step.ch1 - 1;
	const int block = ch0 / 16;
	const int inBlk = ch0 % 16;
	if (block < 0 || block > 3)
		return;
	stM576OneX64ChnDAC& d = pSw4[block].stCalibDAC[0];
	d.stChnDAC[inBlk].sDACx = U16ToShortDac(dacY);
	d.stChnDAC[inBlk].sDACy = U16ToShortDac(dacX);
}
