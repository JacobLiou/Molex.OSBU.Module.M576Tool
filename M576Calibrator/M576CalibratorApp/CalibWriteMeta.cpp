#include "stdafx.h"
#include "CalibWriteMeta.h"
#include "CalibConstants.h"
#include <cstdio>
#include <vector>

namespace
{
// WORD on disk is int16 two's-complement (same for MCS wCalibPtrDAC and 1x64 sDAC*).
static short DacU16ToSigned16(unsigned short w)
{
	return (short)w;
}

/// Four uppercase hex digits: low byte then high byte (LE on-disk order for WORD/SHORT in trans .bin).
static CStringA FormatInt16LeHex4(int v)
{
	const unsigned short w = (unsigned short)(short)v;
	const unsigned lo = (unsigned)(w & 0xFFu);
	const unsigned hi = (unsigned)((w >> 8) & 0xFFu);
	CStringA a;
	a.Format("%02X%02X", lo, hi);
	return a;
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
	int rawDacX,
	int rawDacY,
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
	row.dacX = (int)DacU16ToSigned16(dac.uX);
	row.dacY = (int)DacU16ToSigned16(dac.uY);
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
	const SIZE_T offY = McsWCalibByteOffsetInLut(swIdx, tempI, chIdx, 0);
	const SIZE_T offX = McsWCalibByteOffsetInLut(swIdx, tempI, chIdx, 1);
	if (offY == (SIZE_T)-1 || offX == (SIZE_T)-1)
		return FALSE;
	CStringA pathYword, pathXword;
	pathYword.Format(
		"wCalibPtrDAC[%d][%d(=IDX_TEMP_LOW) low temp][%d][0]  ; 1b channel=%d  swIdx(MCS)=%d  (Y word)",
		swIdx, (int)IDX_TEMP_LOW, chIdx, chIdx + 1, swIdx + 1);
	pathXword.Format(
		"wCalibPtrDAC[%d][%d(=IDX_TEMP_LOW) low temp][%d][1]  (X word)", swIdx, (int)IDX_TEMP_LOW, chIdx);
	row.structPathDacX = CString(pathXword);
	row.structPathDacY = CString(pathYword);
	row.offsetLutOrMems8kDacX = offX;
	row.offsetLutOrMems8kDacY = offY;
	row.offsetTransBinDacX = poff + offX;
	row.offsetTransBinDacY = poff + offY;
	return TRUE;
}

BOOL CalibBuildStatRowPmMems(
	const SPathStep& step,
	int fileSlot,
	int pathLine1Based,
	int peakRow,
	int peakCol,
	int gridN,
	int rawDacX,
	int rawDacY,
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
	row.dacX = (int)DacU16ToSigned16(dac.uX);
	row.dacY = (int)DacU16ToSigned16(dac.uY);

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
		// oX = sDACx (Y value in bin), oY = sDACy (X value); see LutPeakApply mems write.
		CStringA pathSdacXfield, pathSdacYfield;
		pathSdacXfield.Format("stM576OneX64MemsSwCoef[blk=%d].stCalibDAC[0].stChnDAC[%d] (stAxisDAC).sDACx  (Y)  ; 1#1x64 ch(1b)=%d",
			block, inBlk, ch1to64);
		pathSdacYfield.Format("stM576OneX64MemsSwCoef[blk=%d].stCalibDAC[0].stChnDAC[%d] (stAxisDAC).sDACy (X)", block, inBlk);
		row.structPathDacX = CString(pathSdacYfield);
		row.structPathDacY = CString(pathSdacXfield);
		row.offsetLutOrMems8kDacX = oY;
		row.offsetLutOrMems8kDacY = oX;
		row.offsetTransBinDacX = oY;
		row.offsetTransBinDacY = oX;
		return TRUE;
	};

	if (t == 1 || t == 2)
		return go(step.c1);
	if (t == 5 || t == 6)
		return go(step.c4);
	return FALSE;
}

BOOL CalibBuildStatRowPmMemsMapped(
	const SPathStep& step,
	int fileSlot,
	int pathLine1Based,
	int peakRow,
	int peakCol,
	int gridN,
	int rawDacX,
	int rawDacY,
	SDacU16 dac,
	int block0to3,
	int inBlk0based,
	SCalibrationStatRow& row)
{
	if (fileSlot < 2 || fileSlot > 3)
		return FALSE;
	const int t = step.targetSwitchIndex;
	if (t == 3 || t == 4)
		return FALSE;
	if (block0to3 < 0 || block0to3 > 3 || inBlk0based < 0 || inBlk0based >= (int)M576_1X64_MAX_CHANNEL_NUM)
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
	row.dacX = (int)DacU16ToSigned16(dac.uX);
	row.dacY = (int)DacU16ToSigned16(dac.uY);

	SIZE_T oX, oY;
	if (!Mems1x64OffsetsIn8k(block0to3, inBlk0based, oX, oY))
		return FALSE;

	const int sw1b = block0to3 + 1;
	const int chY1b = inBlk0based + 1;
	CStringA pathSdacXfield, pathSdacYfield;
	pathSdacXfield.Format(
		"stM576OneX64MemsSwCoef[blk=%d].stCalibDAC[0].stChnDAC[%d] (stAxisDAC).sDACx  (Y)  ; FW_map SW=%d CH_y(1b)=%d",
		block0to3,
		inBlk0based,
		sw1b,
		chY1b);
	pathSdacYfield.Format(
		"stM576OneX64MemsSwCoef[blk=%d].stCalibDAC[0].stChnDAC[%d] (stAxisDAC).sDACy (X)",
		block0to3,
		inBlk0based);
	row.structPathDacX = CString(pathSdacYfield);
	row.structPathDacY = CString(pathSdacXfield);
	row.offsetLutOrMems8kDacX = oY;
	row.offsetLutOrMems8kDacY = oX;
	row.offsetTransBinDacX = oY;
	row.offsetTransBinDacY = oX;
	return TRUE;
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
	int rawDacX,
	int rawDacY,
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
	int rawDacX,
	int rawDacY,
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

static void AppendCsvQuotedCStringA(CStringA& line, const CStringA& s)
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
		= "# MCS: Y at wCalibPtrDAC[][0], X at [][1] (int16 LE in file). 1x64: Y in sDACx, X in sDACy. "
		  "MCS off_trans = LutPayloadOffset + off_in_struct. "
		  "raw_dac_x / raw_dac_y: lrounded linear cross-peak DAC. dac_x_in_bin = bytes at [1]/sDACy, dac_y_in_bin = [0]/sDACx. "
		  "Quoted 4 hex chars LE per field; Excel text.";
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
			t.Format("%d", r.rawDacX);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.rawDacY);
			line += t;
		}
		line += ',';
		AppendCsvQuotedCStringA(line, FormatInt16LeHex4(r.dacX));
		line += ',';
		AppendCsvQuotedCStringA(line, FormatInt16LeHex4(r.dacY));
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
