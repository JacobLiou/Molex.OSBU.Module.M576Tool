#include "stdafx.h"
#include "CalibWriteMeta.h"
#include "CalibConstants.h"
#include <cstdio>
#include <vector>

namespace
{
static short U16ToShortDac(unsigned short w)
{
	if (w > 0x7FFFu)
		return 0x7FFF;
	return (short)(int)w;
}

static SIZE_T McsWCalibByteOffsetInLut(int swIdx, int tempIdx, int chIdx, int axis01)
{
	const int chCount = PORT_MAX_COUNT + MID_MAX_COUNT;
	if (swIdx < 0 || swIdx > 33 || tempIdx < 0 || tempIdx >= TEMP_CALIB_NUM
		|| chIdx < 0 || chIdx >= chCount || (axis01 & ~1) != 0)
		return (SIZE_T)-1;
	int lin = ((swIdx * TEMP_CALIB_NUM + tempIdx) * chCount + chIdx) * 2 + axis01;
	return offsetof(stLutSettingZ4671, wCalibPtrDAC) + (SIZE_T)lin * sizeof(WORD);
}

static BOOL Mems1x64OffsetsIn8k(int block, int inBlk, SIZE_T& oX, SIZE_T& oY)
{
	if (block < 0 || block > 3 || inBlk < 0 || inBlk >= (int)M576_1X64_MAX_CHANNEL_NUM)
		return FALSE;
	SIZE_T base = (SIZE_T)block * (SIZE_T)M576_1X64_MEMS_BODY_SIZE
		+ offsetof(stM576OneX64MemsSwCoef, stCalibDAC[0])
		- offsetof(stM576OneX64MemsSwCoef, bSWTypeChanNum)
		+ offsetof(stM576OneX64ChnDAC, stChnDAC)
		+ (SIZE_T)inBlk * sizeof(stM576OneX64AxisDAC);
	oX = base;
	oY = base + offsetof(stM576OneX64AxisDAC, sDACy);
	return TRUE;
}
} // namespace

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
	SCalibrationStatRow& row)
{
	if (fileSlot < 0 || fileSlot > 3)
		return FALSE;
	const int t = step.targetSwitchIndex;
	row.calMode = _T("PM");
	row.transSlot1to4 = fileSlot + 1;
	row.pathLine1Based = pathLine1Based;
	CStringA cmdA;
	cmdA.Format("RECAL 1 %d %d %d %d %d", step.targetSwitchIndex, step.c1, step.c2, step.c3, step.c4);
	row.primaryCommand = CString(cmdA);
	row.targetType = _T("MCS_LUT");
	row.peakRow = peakRow;
	row.peakCol = peakCol;
	row.gridN = gridN;
	row.storeType = _T("WORD");
	row.rawDacX = rawDacX;
	row.rawDacY = rawDacY;
	row.dacX = (int)dac.uX;
	row.dacY = (int)dac.uY;
	const SIZE_T poff = (SIZE_T)CLutBinWriter::LutPayloadOffset();
	int swIdx = 0, chIdx = 0, tempI = (int)IDX_TEMP_LOW;
	SIZE_T bx = 0, by = 0;

	if (t == 3)
	{
		if (occT3 < 0 || occT3 > 575)
			return FALSE;
		const int sw = occT3 / 18 + 1;
		const int ch = occT3 % 18 + 1;
		swIdx = (sw - 1);
		chIdx = (ch - 1);
	}
	else if (t == 4)
	{
		if (occT4 < 0 || occT4 > 575)
			return FALSE;
		const int sw = occT4 / 18 + 1;
		const int ch = occT4 % 18 + 1;
		swIdx = (sw - 1) + (int)M576_MCS2_SW_INDEX_OFFSET;
		if (swIdx > 33)
			swIdx = 33;
		chIdx = (ch - 1);
	}
	else if (t == 1 || t == 2)
	{
		int ch1 = step.c1;
		if (ch1 < 1 || ch1 > 64)
			return FALSE;
		const int ch0 = ch1 - 1;
		swIdx = (ch0 < 32) ? 32 : 33;
		chIdx = ch0 % 32;
	}
	else if (t == 5 || t == 6)
	{
		int ch4 = step.c4;
		if (ch4 < 1 || ch4 > 64)
			return FALSE;
		const int ch0 = ch4 - 1;
		swIdx = (ch0 < 32) ? 32 : 33;
		chIdx = ch0 % 32;
	}
	else
	{
		return FALSE;
	}
	bx = McsWCalibByteOffsetInLut(swIdx, tempI, chIdx, 0);
	by = McsWCalibByteOffsetInLut(swIdx, tempI, chIdx, 1);
	if (bx == (SIZE_T)-1 || by == (SIZE_T)-1)
		return FALSE;
	CStringA pathXA, pathYA;
	pathXA.Format(
		"wCalibPtrDAC[%d][%d(=IDX_TEMP_LOW) low temp][%d][0]  ; 1b channel=%d  swIdx(MCS)=%d",
		swIdx, (int)IDX_TEMP_LOW, chIdx, chIdx + 1, swIdx + 1);
	pathYA.Format("wCalibPtrDAC[%d][%d(=IDX_TEMP_LOW) low temp][%d][1]", swIdx, (int)IDX_TEMP_LOW, chIdx);
	row.structPathDacX = CString(pathXA);
	row.structPathDacY = CString(pathYA);
	row.offsetLutOrMems8kDacX = bx;
	row.offsetLutOrMems8kDacY = by;
	row.offsetTransBinDacX = poff + bx;
	row.offsetTransBinDacY = poff + by;
	return TRUE;
}

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
	SCalibrationStatRow& row)
{
	if (fileSlot < 2 || fileSlot > 3)
		return FALSE;
	const int t = step.targetSwitchIndex;
	if (t == 3 || t == 4)
		return FALSE;
	row.calMode = _T("PM");
	row.transSlot1to4 = fileSlot + 1;
	row.pathLine1Based = pathLine1Based;
	CStringA cmdA;
	cmdA.Format("RECAL 1 %d %d %d %d %d", step.targetSwitchIndex, step.c1, step.c2, step.c3, step.c4);
	row.primaryCommand = CString(cmdA);
	row.targetType = _T("1X64_MEMS");
	row.peakRow = peakRow;
	row.peakCol = peakCol;
	row.gridN = gridN;
	row.storeType = _T("SHORT");
	row.rawDacX = rawDacX;
	row.rawDacY = rawDacY;
	row.dacX = (int)U16ToShortDac(dac.uX);
	row.dacY = (int)U16ToShortDac(dac.uY);

	auto go = [&](int ch1to64)
	{
		if (ch1to64 < 1 || ch1to64 > 64)
			return FALSE;
		const int ch0 = ch1to64 - 1;
		const int block = ch0 / 16;
		const int inBlk = ch0 % 16;
		SIZE_T oX, oY;
		if (!Mems1x64OffsetsIn8k(block, inBlk, oX, oY))
			return FALSE;
		CStringA sx, sy;
		sx.Format("stM576OneX64MemsSwCoef[blk=%d].stCalibDAC[0].stChnDAC[%d] (stAxisDAC).sDACx  ; 1#1x64 ch(1b)=%d",
			block, inBlk, ch1to64);
		sy.Format("stM576OneX64MemsSwCoef[blk=%d].stCalibDAC[0].stChnDAC[%d] (stAxisDAC).sDACy", block, inBlk);
		row.structPathDacX = CString(sx);
		row.structPathDacY = CString(sy);
		row.offsetLutOrMems8kDacX = oX;
		row.offsetLutOrMems8kDacY = oY;
		row.offsetTransBinDacX = oX;
		row.offsetTransBinDacY = oY;
		return TRUE;
	};

	if (t == 1 || t == 2)
		return go(step.c1);
	if (t == 5 || t == 6)
		return go(step.c4);
	return FALSE;
}

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
	SCalibrationStatRow& row)
{
	const int t = step.targetSwitchIndex;
	SPathStep pm;
	ZeroMemory(&pm, sizeof(pm));
	pm.targetSwitchIndex = t;
	if (t == 1 || t == 2)
		pm.c1 = step.ch1;
	if (!CalibBuildStatRowPmLut(
			pm, occT3, occT4, fileSlot, pathLine1Based, peakRow, peakCol, gridN, rawDacX, rawDacY, dac, row))
		return FALSE;
	row.calMode = _T("PD");
	CStringA a;
	a.Format("RECAL 2 %d %d %d", step.targetSwitchIndex, step.ch1, step.ch2);
	row.primaryCommand = CString(a);
	return TRUE;
}

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
	SCalibrationStatRow& row)
{
	SPathStep s;
	ZeroMemory(&s, sizeof(s));
	s.targetSwitchIndex = step.targetSwitchIndex;
	s.c1 = step.ch1;
	if (!CalibBuildStatRowPmMems(
			s, fileSlot, pathLine1Based, peakRow, peakCol, gridN, rawDacX, rawDacY, dac, row))
		return FALSE;
	row.calMode = _T("PD");
	CStringA a;
	a.Format("RECAL 2 %d %d %d", step.targetSwitchIndex, step.ch1, step.ch2);
	row.primaryCommand = CString(a);
	return TRUE;
}

static void AppendCsvField(CStringA& line, const CString& s)
{
	CStringA sa(s);
	sa.Replace("\"", "\"\"");
	line += '\"';
	line += (LPCSTR)sa;
	line += '\"';
}

static CStringA FormatU(SIZE_T v)
{
	CStringA a;
	a.Format("%Iu", (unsigned __int64)v);
	return a;
}

BOOL WriteCalibrationStatsCsv(LPCTSTR path, const std::vector<SCalibrationStatRow>& rows, CString& err)
{
	if (path == NULL || path[0] == 0)
	{
		err = _T("Path empty");
		return FALSE;
	}
	CFile f;
	if (!f.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		err = _T("Cannot create file");
		return FALSE;
	}
	{
		const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
		f.Write(bom, 3);
	}
	auto writeA = [&f](LPCSTR s) {
		f.Write(s, (UINT)strlen(s));
		f.Write("\r\n", 2);
	};

	const char* legend
		= "# MCS: WORD wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0/1] in stLutSettingZ4671. "
		  "1x64: SHORT stM576OneX64AxisDAC (firmware stAxisDAC) in 8K. "
		  "MCS off_trans = LutPayloadOffset + off_in_struct. "
		  "raw_dac_x/raw_dac_y: linear DAC at cross-peak = col0 + peakIndex*step per axis (not raw line col0 only), before 12b grid->BIN map.";
	writeA(legend);
	const char* hdr = "cal_mode,trans_slot,path_line,primary_cmd,target,peak_r,peak_c,gridN,store_type,"
		"raw_dac_x,raw_dac_y,dac_x_in_bin,dac_y_in_bin,struct_path_dacX,struct_path_dacY,"
		"off_lut_or_mem8k_dacX,off_lut_or_mem8k_dacY,off_trans_bin_dacX,off_trans_bin_dacY";
	writeA(hdr);

	for (size_t i = 0; i < rows.size(); ++i)
	{
		const SCalibrationStatRow& r = rows[i];
		CStringA line;
		AppendCsvField(line, r.calMode);
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.transSlot1to4);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.pathLine1Based);
			line += t;
		}
		line += ',';
		AppendCsvField(line, r.primaryCommand);
		line += ',';
		AppendCsvField(line, r.targetType);
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.peakRow);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.peakCol);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.gridN);
			line += t;
		}
		line += ',';
		AppendCsvField(line, r.storeType);
		line += ',';
		{
			CStringA t;
			t.Format("%.15g", r.rawDacX);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%.15g", r.rawDacY);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.dacX);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.dacY);
			line += t;
		}
		line += ',';
		AppendCsvField(line, r.structPathDacX);
		line += ',';
		AppendCsvField(line, r.structPathDacY);
		line += ',';
		line += (LPCSTR)FormatU(r.offsetLutOrMems8kDacX);
		line += ',';
		line += (LPCSTR)FormatU(r.offsetLutOrMems8kDacY);
		line += ',';
		line += (LPCSTR)FormatU(r.offsetTransBinDacX);
		line += ',';
		line += (LPCSTR)FormatU(r.offsetTransBinDacY);
		f.Write((LPCSTR)line, (UINT)line.GetLength());
		f.Write("\r\n", 2);
	}
	f.Close();
	(void)err;
	return TRUE;
}
