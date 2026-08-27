#include "stdafx.h"
#include "FullEditDacIo.h"
#include "CalibConstants.h"
#include "LutBinWriter.h"
#include "LutPeakApply.h"
#include "Mems1x64LutBinWriter.h"
#include "M576OneX64Coef.h"

#include <fstream>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

namespace {

short LutWordAsSigned(WORD w)
{
	return (short)w;
}

void WriteMcsDacPairTemp(stLutSettingZ4671& lut, int sw, int temp, int ch, short dacY, short dacX)
{
	unsigned short uy = 0, ux = 0;
	// Same arg order as FineTuneWriteDac: (dacX, dacY) -> (ux, uy); store [0]=Y [1]=X.
	RawCrossPeakDacToU16Pair((double)dacX, (double)dacY, ux, uy);
	lut.wCalibPtrDAC[sw][temp][ch][0] = uy;
	lut.wCalibPtrDAC[sw][temp][ch][1] = ux;
}

void ReadMcsTriple(const stLutSettingZ4671& lut, int sw, int ch, FullEditTempDacTriple& t)
{
	t.lowY = LutWordAsSigned(lut.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0]);
	t.lowX = LutWordAsSigned(lut.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][1]);
	t.roomY = LutWordAsSigned(lut.wCalibPtrDAC[sw][IDX_TEMP_ROOM][ch][0]);
	t.roomX = LutWordAsSigned(lut.wCalibPtrDAC[sw][IDX_TEMP_ROOM][ch][1]);
	t.highY = LutWordAsSigned(lut.wCalibPtrDAC[sw][IDX_TEMP_HIGH][ch][0]);
	t.highX = LutWordAsSigned(lut.wCalibPtrDAC[sw][IDX_TEMP_HIGH][ch][1]);
	t.tempPointLow = lut.wTemperaturePoint[sw][IDX_TEMP_LOW];
	t.tempPointRoom = lut.wTemperaturePoint[sw][IDX_TEMP_ROOM];
	t.tempPointHigh = lut.wTemperaturePoint[sw][IDX_TEMP_HIGH];
}

void ReadMemsAxis(const stM576OneX64AxisDAC& a, short& dacY, short& dacX)
{
	// FineTune: dacY = sDACx, dacX = sDACy
	dacY = a.sDACx;
	dacX = a.sDACy;
}

void WriteMemsAxis(stM576OneX64AxisDAC& a, short dacY, short dacX)
{
	a.sDACx = dacY;
	a.sDACy = dacX;
}

int OpticalBlockFromSw(int sw)
{
	if (0 <= sw && sw <= 15)
		return sw + 17;
	if (16 <= sw && sw <= 31)
		return sw - 15;
	return 0;
}

BOOL MkDirDeep(const CString& path)
{
	if (path.IsEmpty())
		return FALSE;
	if (PathFileExists(path))
		return TRUE;
	const int slash = path.ReverseFind(_T('\\'));
	if (slash > 0)
	{
		CString parent = path.Left(slash);
		if (!MkDirDeep(parent))
			return FALSE;
	}
	return CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

} // namespace

CString FullEditRoleDirName(M576BinFileRole role)
{
	return (role == M576BinFileRole::Backup) ? _T("backup") : _T("standard");
}

BOOL FullEditEnsureDirs(LPCTSTR outDirAbs, M576BinFileRole role, CString& roleRootOut, CString& errMsg)
{
	errMsg.Empty();
	roleRootOut.Empty();
	if (outDirAbs == NULL || outDirAbs[0] == 0)
	{
		errMsg = _T("FE_EXPORT_FAIL empty outDir");
		return FALSE;
	}
	CString root = outDirAbs;
	if (root.Right(1) != _T("\\"))
		root += _T("\\");
	roleRootOut = root + _T("fulledit\\") + FullEditRoleDirName(role);
	const CString working = roleRootOut + _T("\\working");
	const CString baseline = roleRootOut + _T("\\baseline");
	const CString impact = roleRootOut + _T("\\path_impact");
	if (!MkDirDeep(working) || !MkDirDeep(baseline) || !MkDirDeep(impact))
	{
		errMsg = _T("FE_EXPORT_FAIL cannot create fulledit dirs");
		return FALSE;
	}
	return TRUE;
}

BOOL FullEditReadFileUtf8(LPCTSTR path, std::string& out, CString& errMsg)
{
	out.clear();
	errMsg.Empty();
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs)
	{
		errMsg.Format(_T("cannot read %s"), path);
		return FALSE;
	}
	out.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	return TRUE;
}

BOOL FullEditWriteFileUtf8Bom(LPCTSTR path, const std::string& bodyNoBom, CString& errMsg)
{
	errMsg.Empty();
	std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
	if (!ofs)
	{
		errMsg.Format(_T("cannot write %s"), path);
		return FALSE;
	}
	static const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
	ofs.write(reinterpret_cast<const char*>(bom), 3);
	ofs.write(bodyNoBom.data(), (std::streamsize)bodyNoBom.size());
	return ofs.good() ? TRUE : FALSE;
}

FullEditErrorCode FullEditExportMcsBinToCsv(
	LPCTSTR binPath,
	int burnIndex,
	LPCTSTR snLabel,
	LPCTSTR csvPath,
	CString& errMsg)
{
	errMsg.Empty();
	stLutSettingZ4671 lut;
	ZeroMemory(&lut, sizeof(lut));
	if (!CLutBinWriter::ReadLutFromFile(binPath, lut))
	{
		errMsg.Format(_T("FE_BIN_MISSING or read fail: %s"), binPath);
		return FullEditErrorCode::BinMissing;
	}
	std::vector<FullEditMcsCsvRow> rows;
	rows.reserve(34 * 32);
	for (int sw = 0; sw < 34; ++sw)
	{
		for (int ch = 0; ch < 32; ++ch)
		{
			FullEditMcsCsvRow r{};
			r.burnIndex = burnIndex;
			r.swLutIdx = sw;
			r.chIdx = ch;
			r.chKind = (ch < 20) ? "PORT" : "MID";
			const int opt = OpticalBlockFromSw(sw);
			if (opt > 0)
			{
				r.opticalBlock = opt;
				r.opticalBlockValid = true;
			}
			ReadMcsTriple(lut, sw, ch, r.dacs);
			rows.push_back(r);
		}
	}
	CT2A snA(snLabel ? snLabel : _T(""), CP_UTF8);
	const std::string body = FullEditFormatMcsCsv(burnIndex, std::string(snA), rows);
	if (!FullEditWriteFileUtf8Bom(csvPath, body, errMsg))
		return FullEditErrorCode::ExportFail;
	return FullEditErrorCode::Ok;
}

FullEditErrorCode FullEditExportMemsBinToCsv(
	LPCTSTR binPath,
	int burnIndex,
	LPCTSTR snLabel,
	int sw1to4,
	LPCTSTR csvPath,
	CString& errMsg)
{
	errMsg.Empty();
	stM576OneX64MemsSwCoef mems;
	ZeroMemory(&mems, sizeof(mems));
	if (!CMems1x64LutBinWriter::ReadMemsFromFile(binPath, &mems))
	{
		errMsg.Format(_T("FE_BIN_MISSING or read fail: %s"), binPath);
		return FullEditErrorCode::BinMissing;
	}
	std::vector<FullEditMemsCsvRow> rows;
	for (int i = 0; i < (int)M576_1X64_MAX_CHANNEL_NUM; ++i)
	{
		FullEditMemsCsvRow r{};
		r.burnIndex = burnIndex;
		r.sw1to4 = sw1to4;
		r.rowKind = "CH";
		r.rowIdx = i;
		r.dacs.tempPointLow = mems.stCalibDAC[0].sTemperature;
		r.dacs.tempPointRoom = mems.stCalibDAC[1].sTemperature;
		r.dacs.tempPointHigh = mems.stCalibDAC[2].sTemperature;
		ReadMemsAxis(mems.stCalibDAC[0].stChnDAC[i], r.dacs.lowY, r.dacs.lowX);
		ReadMemsAxis(mems.stCalibDAC[1].stChnDAC[i], r.dacs.roomY, r.dacs.roomX);
		ReadMemsAxis(mems.stCalibDAC[2].stChnDAC[i], r.dacs.highY, r.dacs.highX);
		rows.push_back(r);
	}
	for (int i = 0; i < (int)M576_1X64_MAX_MIDPTR_NUM; ++i)
	{
		FullEditMemsCsvRow r{};
		r.burnIndex = burnIndex;
		r.sw1to4 = sw1to4;
		r.rowKind = "MID";
		r.rowIdx = i;
		r.dacs.tempPointLow = mems.stCalibDAC[0].sTemperature;
		r.dacs.tempPointRoom = mems.stCalibDAC[1].sTemperature;
		r.dacs.tempPointHigh = mems.stCalibDAC[2].sTemperature;
		ReadMemsAxis(mems.stCalibDAC[0].stMidDAC[i], r.dacs.lowY, r.dacs.lowX);
		ReadMemsAxis(mems.stCalibDAC[1].stMidDAC[i], r.dacs.roomY, r.dacs.roomX);
		ReadMemsAxis(mems.stCalibDAC[2].stMidDAC[i], r.dacs.highY, r.dacs.highX);
		rows.push_back(r);
	}
	CT2A snA(snLabel ? snLabel : _T(""), CP_UTF8);
	const std::string body = FullEditFormatMemsCsv(burnIndex, std::string(snA), sw1to4, rows);
	if (!FullEditWriteFileUtf8Bom(csvPath, body, errMsg))
		return FullEditErrorCode::ExportFail;
	return FullEditErrorCode::Ok;
}

FullEditErrorCode FullEditApplyMcsPatchesToBin(
	LPCTSTR binPath,
	const std::vector<FullEditMcsPatch>& patches,
	CString& errMsg)
{
	errMsg.Empty();
	if (patches.empty())
		return FullEditErrorCode::Ok;
	stLutSettingZ4671 lut;
	ZeroMemory(&lut, sizeof(lut));
	if (!CLutBinWriter::ReadLutFromFile(binPath, lut))
	{
		errMsg.Format(_T("read fail %s"), binPath);
		return FullEditErrorCode::WriteFail;
	}
	for (const FullEditMcsPatch& p : patches)
	{
		WriteMcsDacPairTemp(lut, p.swLutIdx, IDX_TEMP_LOW, p.chIdx, p.dacs.lowY, p.dacs.lowX);
		WriteMcsDacPairTemp(lut, p.swLutIdx, IDX_TEMP_ROOM, p.chIdx, p.dacs.roomY, p.dacs.roomX);
		WriteMcsDacPairTemp(lut, p.swLutIdx, IDX_TEMP_HIGH, p.chIdx, p.dacs.highY, p.dacs.highX);
		if (p.patchTempPoints)
		{
			lut.wTemperaturePoint[p.swLutIdx][IDX_TEMP_LOW] = p.dacs.tempPointLow;
			lut.wTemperaturePoint[p.swLutIdx][IDX_TEMP_ROOM] = p.dacs.tempPointRoom;
			lut.wTemperaturePoint[p.swLutIdx][IDX_TEMP_HIGH] = p.dacs.tempPointHigh;
		}
	}
	CString sn;
	(void)CLutBinWriter::ReadBundleSnFromFile(binPath, sn);
	SLutBinWriteParams wp;
	wp.strOutputPath = binPath;
	wp.pLut = &lut;
	wp.strBundleSN = sn;
	if (!CLutBinWriter::Write(wp))
	{
		errMsg.Format(_T("write fail %s"), binPath);
		return FullEditErrorCode::WriteFail;
	}
	// verify first patch
	stLutSettingZ4671 v;
	ZeroMemory(&v, sizeof(v));
	if (!CLutBinWriter::ReadLutFromFile(binPath, v))
		return FullEditErrorCode::VerifyFail;
	const FullEditMcsPatch& p0 = patches[0];
	FullEditTempDacTriple got{};
	ReadMcsTriple(v, p0.swLutIdx, p0.chIdx, got);
	if (got.lowY != p0.dacs.lowY || got.lowX != p0.dacs.lowX
		|| got.roomY != p0.dacs.roomY || got.roomX != p0.dacs.roomX
		|| got.highY != p0.dacs.highY || got.highX != p0.dacs.highX)
	{
		errMsg = _T("verify mismatch after MCS write");
		return FullEditErrorCode::VerifyFail;
	}
	return FullEditErrorCode::Ok;
}

FullEditErrorCode FullEditApplyMemsPatchesToBin(
	LPCTSTR binPath,
	int sw0,
	const std::vector<FullEditMemsPatch>& patches,
	CString& errMsg)
{
	errMsg.Empty();
	if (patches.empty())
		return FullEditErrorCode::Ok;
	stM576OneX64MemsSwCoef mems;
	ZeroMemory(&mems, sizeof(mems));
	if (!CMems1x64LutBinWriter::ReadMemsFromFile(binPath, &mems))
	{
		errMsg.Format(_T("read fail %s"), binPath);
		return FullEditErrorCode::WriteFail;
	}
	for (const FullEditMemsPatch& p : patches)
	{
		auto applyOne = [&](int tempIdx, short y, short x) {
			if (p.isMid)
				WriteMemsAxis(mems.stCalibDAC[tempIdx].stMidDAC[p.rowIdx], y, x);
			else
				WriteMemsAxis(mems.stCalibDAC[tempIdx].stChnDAC[p.rowIdx], y, x);
		};
		applyOne(0, p.dacs.lowY, p.dacs.lowX);
		applyOne(1, p.dacs.roomY, p.dacs.roomX);
		applyOne(2, p.dacs.highY, p.dacs.highX);
		if (p.patchTempCenti)
		{
			mems.stCalibDAC[0].sTemperature = p.dacs.tempPointLow;
			mems.stCalibDAC[1].sTemperature = p.dacs.tempPointRoom;
			mems.stCalibDAC[2].sTemperature = p.dacs.tempPointHigh;
		}
	}
	M576OneX64ApplyStandardTempMeta(mems);
	// If user patched temp centi, re-apply their values after standard meta? Spec says temp editable.
	// Re-apply last patch temps if any patchTempCenti.
	for (const FullEditMemsPatch& p : patches)
	{
		if (p.patchTempCenti)
		{
			mems.stCalibDAC[0].sTemperature = p.dacs.tempPointLow;
			mems.stCalibDAC[1].sTemperature = p.dacs.tempPointRoom;
			mems.stCalibDAC[2].sTemperature = p.dacs.tempPointHigh;
		}
	}
	CString sn = CMems1x64LutBinWriter::ReadBundleVer16FromCoef(mems);
	if (!CMems1x64LutBinWriter::WriteSingleSwitch(mems, sw0, binPath, sn, CString()))
	{
		errMsg.Format(_T("write fail %s"), binPath);
		return FullEditErrorCode::WriteFail;
	}
	return FullEditErrorCode::Ok;
}

FullEditErrorCode FullEditExportAllTen(
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	M576BinFileRole role,
	CString& errMsg)
{
	errMsg.Empty();
	CString roleRoot;
	if (!FullEditEnsureDirs(outDirAbs, role, roleRoot, errMsg))
		return FullEditErrorCode::ExportFail;

	std::array<CString, M576_BURN_FILE_COUNT> paths;
	if (!M576BuildBurnFilePaths(outDirAbs, sn, role, paths, errMsg))
		return FullEditErrorCode::NoSn;

	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
	{
		CString snOne;
		CString e;
		if (!M576SnForBurnFileIndex(sn, i, snOne, e))
		{
			errMsg = e;
			return FullEditErrorCode::NoSn;
		}
		if (GetFileAttributes(paths[i]) == INVALID_FILE_ATTRIBUTES)
		{
			errMsg.Format(_T("FE_BIN_MISSING %s"), paths[i].GetString());
			return FullEditErrorCode::BinMissing;
		}
		const CString fileName = snOne + _T("_dac.csv");
		const CString workPath = roleRoot + _T("\\working\\") + fileName;
		const CString basePath = roleRoot + _T("\\baseline\\") + fileName;
		FullEditErrorCode rc = FullEditErrorCode::Ok;
		if (i < 2)
			rc = FullEditExportMcsBinToCsv(paths[i], i, snOne, workPath, errMsg);
		else
		{
			const int sw1to4 = (i < 6) ? (i - 2 + 1) : (i - 6 + 1);
			rc = FullEditExportMemsBinToCsv(paths[i], i, snOne, sw1to4, workPath, errMsg);
		}
		if (rc != FullEditErrorCode::Ok)
			return rc;
		if (!CopyFile(workPath, basePath, FALSE))
		{
			errMsg.Format(_T("FE_EXPORT_FAIL copy baseline %s"), fileName.GetString());
			return FullEditErrorCode::ExportFail;
		}
	}
	return FullEditErrorCode::Ok;
}

FullEditErrorCode FullEditValidateAndWriteAllTen(
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	M576BinFileRole role,
	const FullEditUnlockFlags& unlock,
	int& outFilesPatched,
	int& outRowsPatched,
	CString& errMsg)
{
	outFilesPatched = 0;
	outRowsPatched = 0;
	errMsg.Empty();
	CString roleRoot = CString(outDirAbs);
	if (roleRoot.Right(1) != _T("\\"))
		roleRoot += _T("\\");
	roleRoot += _T("fulledit\\") + FullEditRoleDirName(role);

	std::array<CString, M576_BURN_FILE_COUNT> paths;
	if (!M576BuildBurnFilePaths(outDirAbs, sn, role, paths, errMsg))
		return FullEditErrorCode::NoSn;

	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
	{
		CString snOne;
		CString e;
		if (!M576SnForBurnFileIndex(sn, i, snOne, e))
		{
			errMsg = e;
			return FullEditErrorCode::NoSn;
		}
		const CString fileName = snOne + _T("_dac.csv");
		const CString workPath = roleRoot + _T("\\working\\") + fileName;
		const CString basePath = roleRoot + _T("\\baseline\\") + fileName;

		if (GetFileAttributes(workPath) == INVALID_FILE_ATTRIBUTES)
			continue; // no working => skip file
		if (GetFileAttributes(basePath) == INVALID_FILE_ATTRIBUTES)
		{
			errMsg.Format(_T("FE_BASELINE_MISSING %s"), fileName.GetString());
			return FullEditErrorCode::BaselineMissing;
		}

		std::string workText, baseText;
		CString re;
		if (!FullEditReadFileUtf8(workPath, workText, re)
			|| !FullEditReadFileUtf8(basePath, baseText, re))
		{
			errMsg = re;
			return FullEditErrorCode::CsvParse;
		}

		std::string perr;
		if (i < 2)
		{
			std::vector<FullEditMcsCsvRow> wRows, bRows;
			FullEditErrorCode pc = FullEditParseMcsCsv(workText, wRows, perr);
			if (pc != FullEditErrorCode::Ok)
			{
				errMsg = CString(perr.c_str());
				return pc;
			}
			pc = FullEditParseMcsCsv(baseText, bRows, perr);
			if (pc != FullEditErrorCode::Ok)
			{
				errMsg = CString(perr.c_str());
				return pc;
			}
			std::vector<FullEditMcsPatch> patches;
			pc = FullEditDiffMcsCsv(wRows, bRows, unlock, patches, perr);
			if (pc != FullEditErrorCode::Ok)
			{
				errMsg = CString(perr.c_str());
				return pc;
			}
			if (patches.empty())
				continue;
			pc = FullEditApplyMcsPatchesToBin(paths[i], patches, errMsg);
			if (pc != FullEditErrorCode::Ok)
				return pc;
			outFilesPatched++;
			outRowsPatched += (int)patches.size();
		}
		else
		{
			std::vector<FullEditMemsCsvRow> wRows, bRows;
			FullEditErrorCode pc = FullEditParseMemsCsv(workText, wRows, perr);
			if (pc != FullEditErrorCode::Ok)
			{
				errMsg = CString(perr.c_str());
				return pc;
			}
			pc = FullEditParseMemsCsv(baseText, bRows, perr);
			if (pc != FullEditErrorCode::Ok)
			{
				errMsg = CString(perr.c_str());
				return pc;
			}
			std::vector<FullEditMemsPatch> patches;
			pc = FullEditDiffMemsCsv(wRows, bRows, unlock, patches, perr);
			if (pc != FullEditErrorCode::Ok)
			{
				errMsg = CString(perr.c_str());
				return pc;
			}
			if (patches.empty())
				continue;
			const int sw0 = (i < 6) ? (i - 2) : (i - 6);
			pc = FullEditApplyMemsPatchesToBin(paths[i], sw0, patches, errMsg);
			if (pc != FullEditErrorCode::Ok)
				return pc;
			outFilesPatched++;
			outRowsPatched += (int)patches.size();
		}
	}

	if (outFilesPatched == 0)
	{
		errMsg = _T("FE_DIFF_EMPTY no changes vs baseline");
		return FullEditErrorCode::DiffEmpty;
	}
	errMsg.Format(_T("FE_OK files=%d rows=%d"), outFilesPatched, outRowsPatched);
	return FullEditErrorCode::Ok;
}
