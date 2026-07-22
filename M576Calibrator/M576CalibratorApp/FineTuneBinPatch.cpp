#include "stdafx.h"
#include "FineTuneBinPatch.h"
#include "CalibConstants.h"
#include "LutBinWriter.h"
#include "Mems1x64LutBinWriter.h"
#include "LutPeakApply.h"

namespace {

BOOL IsMcsDevice(FineTuneDeviceKind d)
{
	return d == FineTuneDeviceKind::Mcs1 || d == FineTuneDeviceKind::Mcs2;
}

BOOL ValidateAddress(const FineTuneAddress& addr, CString& errMsg)
{
	if (IsMcsDevice(addr.device))
	{
		if (addr.mcsBlock1to32 < 1 || addr.mcsBlock1to32 > 32)
		{
			errMsg = _T("MCS block must be 1..32.");
			return FALSE;
		}
		if (addr.mcsCh1to18 < 1 || addr.mcsCh1to18 > 18)
		{
			errMsg = _T("MCS channel must be 1..18.");
			return FALSE;
		}
		return TRUE;
	}
	if (addr.sw1to4 < 1 || addr.sw1to4 > 4)
	{
		errMsg = _T("1x64 SW must be 1..4.");
		return FALSE;
	}
	if (addr.chY1to17 < 1 || addr.chY1to17 > 17)
	{
		errMsg = _T("1x64 CH_y must be 1..17.");
		return FALSE;
	}
	return TRUE;
}

void ReadMcsDacPair(const stLutSettingZ4671& lut, int swIdx, int chIdx, short& dacX, short& dacY)
{
	dacY = (short)lut.wCalibPtrDAC[swIdx][IDX_TEMP_LOW][chIdx][0];
	dacX = (short)lut.wCalibPtrDAC[swIdx][IDX_TEMP_LOW][chIdx][1];
}

void ReadMemsDacPair(const stM576OneX64MemsSwCoef& sw, int inBlk0, short& dacX, short& dacY)
{
	const stM576OneX64AxisDAC& a = sw.stCalibDAC[0].stChnDAC[inBlk0];
	dacY = a.sDACx;
	dacX = a.sDACy;
}

} // namespace

BOOL FineTuneBurnIndex(const FineTuneAddress& addr, int& burnIndexOut, CString& errMsg)
{
	errMsg.Empty();
	if (!ValidateAddress(addr, errMsg))
		return FALSE;
	switch (addr.device)
	{
	case FineTuneDeviceKind::Mcs1:
		burnIndexOut = 0;
		return TRUE;
	case FineTuneDeviceKind::Mcs2:
		burnIndexOut = 1;
		return TRUE;
	case FineTuneDeviceKind::OneX64_1:
		burnIndexOut = 2 + (addr.sw1to4 - 1);
		return TRUE;
	case FineTuneDeviceKind::OneX64_2:
		burnIndexOut = 6 + (addr.sw1to4 - 1);
		return TRUE;
	default:
		errMsg = _T("Unknown FineTune device.");
		return FALSE;
	}
}

BOOL FineTuneResolvePath(
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	M576BinFileRole role,
	const FineTuneAddress& addr,
	CString& pathOut,
	int& burnIndexOut,
	CString& errMsg)
{
	errMsg.Empty();
	pathOut.Empty();
	if (outDirAbs == NULL || outDirAbs[0] == _T('\0'))
	{
		errMsg = _T("Out directory is empty.");
		return FALSE;
	}
	if (!M576ValidateSnInfoForBinOps(sn, errMsg))
		return FALSE;
	if (!FineTuneBurnIndex(addr, burnIndexOut, errMsg))
		return FALSE;

	std::array<CString, M576_BURN_FILE_COUNT> paths;
	if (!M576BuildBurnFilePaths(outDirAbs, sn, role, paths, errMsg))
		return FALSE;
	pathOut = paths[burnIndexOut];
	if (pathOut.IsEmpty())
	{
		errMsg = _T("Resolved bin path is empty.");
		return FALSE;
	}
	return TRUE;
}

BOOL FineTuneReadCurrentDac(
	LPCTSTR binPath,
	const FineTuneAddress& addr,
	short& dacXOut,
	short& dacYOut,
	CString& errMsg)
{
	errMsg.Empty();
	dacXOut = 0;
	dacYOut = 0;
	if (!ValidateAddress(addr, errMsg))
		return FALSE;
	if (binPath == NULL || binPath[0] == _T('\0'))
	{
		errMsg = _T("Bin path is empty.");
		return FALSE;
	}
	if (GetFileAttributes(binPath) == INVALID_FILE_ATTRIBUTES)
	{
		errMsg.Format(_T("Bin file not found:\n%s"), binPath);
		return FALSE;
	}

	if (IsMcsDevice(addr.device))
	{
		stLutSettingZ4671 lut;
		ZeroMemory(&lut, sizeof(lut));
		if (!CLutBinWriter::ReadLutFromFile(binPath, lut))
		{
			errMsg.Format(_T("Failed to read MCS LUT bin:\n%s"), binPath);
			return FALSE;
		}
		const int swIdx = M576McsBlock1To32ToLutSwIdx0(addr.mcsBlock1to32);
		const int chIdx = addr.mcsCh1to18 - 1;
		ReadMcsDacPair(lut, swIdx, chIdx, dacXOut, dacYOut);
		return TRUE;
	}

	stM576OneX64MemsSwCoef mems;
	ZeroMemory(&mems, sizeof(mems));
	if (!CMems1x64LutBinWriter::ReadMemsFromFile(binPath, &mems))
	{
		errMsg.Format(_T("Failed to read 1x64 Mems bin:\n%s"), binPath);
		return FALSE;
	}
	ReadMemsDacPair(mems, addr.chY1to17 - 1, dacXOut, dacYOut);
	return TRUE;
}

BOOL FineTuneWriteDac(
	LPCTSTR binPath,
	const FineTuneAddress& addr,
	short dacX,
	short dacY,
	FineTuneSyncPayload* pSyncOut,
	CString& errMsg)
{
	errMsg.Empty();
	if (pSyncOut != NULL)
		ZeroMemory(pSyncOut, sizeof(*pSyncOut));
	if (!ValidateAddress(addr, errMsg))
		return FALSE;
	if (binPath == NULL || binPath[0] == _T('\0'))
	{
		errMsg = _T("Bin path is empty.");
		return FALSE;
	}
	if (GetFileAttributes(binPath) == INVALID_FILE_ATTRIBUTES)
	{
		errMsg.Format(_T("Bin file not found:\n%s"), binPath);
		return FALSE;
	}

	unsigned short ux = 0, uy = 0;
	RawCrossPeakDacToU16Pair((double)dacX, (double)dacY, ux, uy);

	if (IsMcsDevice(addr.device))
	{
		stLutSettingZ4671 lut;
		ZeroMemory(&lut, sizeof(lut));
		if (!CLutBinWriter::ReadLutFromFile(binPath, lut))
		{
			errMsg.Format(_T("Failed to read MCS LUT bin:\n%s"), binPath);
			return FALSE;
		}
		const int swIdx = M576McsBlock1To32ToLutSwIdx0(addr.mcsBlock1to32);
		const int chIdx = addr.mcsCh1to18 - 1;
		WriteMcsLutDacPair(lut, swIdx, chIdx, IDX_TEMP_LOW, ux, uy);

		CString sn;
		(void)CLutBinWriter::ReadBundleSnFromFile(binPath, sn);
		SLutBinWriteParams p;
		p.strOutputPath = binPath;
		p.pLut = &lut;
		p.strBundleSN = sn;
		if (!CLutBinWriter::Write(p))
		{
			errMsg.Format(_T("Failed to write MCS LUT bin:\n%s"), binPath);
			return FALSE;
		}
		if (pSyncOut != NULL)
		{
			pSyncOut->isMcs = TRUE;
			pSyncOut->mcsTransIndex = (addr.device == FineTuneDeviceKind::Mcs2) ? 1 : 0;
			pSyncOut->lut = lut;
		}
		return TRUE;
	}

	stM576OneX64MemsSwCoef mems;
	ZeroMemory(&mems, sizeof(mems));
	if (!CMems1x64LutBinWriter::ReadMemsFromFile(binPath, &mems))
	{
		errMsg.Format(_T("Failed to read 1x64 Mems bin:\n%s"), binPath);
		return FALSE;
	}

	stM576OneX64MemsSwCoef four[4];
	ZeroMemory(four, sizeof(four));
	const int sw0 = addr.sw1to4 - 1;
	four[sw0] = mems;
	WriteMems1x64LowTempDacPair(four, sw0, addr.chY1to17 - 1, ux, uy);
	mems = four[sw0];
	M576OneX64ApplyStandardTempMeta(mems);

	CString sn = CMems1x64LutBinWriter::ReadBundleVer16FromCoef(mems);
	if (!CMems1x64LutBinWriter::WriteSingleSwitch(mems, sw0, binPath, sn, CString()))
	{
		errMsg.Format(_T("Failed to write 1x64 Mems bin:\n%s"), binPath);
		return FALSE;
	}
	if (pSyncOut != NULL)
	{
		pSyncOut->isMcs = FALSE;
		pSyncOut->oneX64Dev = (addr.device == FineTuneDeviceKind::OneX64_2) ? 1 : 0;
		pSyncOut->oneX64Sw = sw0;
		pSyncOut->mems = mems;
	}
	return TRUE;
}
