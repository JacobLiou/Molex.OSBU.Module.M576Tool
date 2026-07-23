#pragma once
// Fine-tune a single Backup/Standard burn file: patch 1310 low-temp DAC and write back.
// Write Bin keeps the dialog open so multiple Address slots can be patched in one session.

#include "McsFwTransport.h"
#include "M576OneX64Coef.h"
#include "Pm1x64Mapping.h"
#include "Z4767StructDefine.h"

enum class FineTuneDeviceKind
{
	Mcs1 = 0,
	Mcs2,
	OneX64_1,
	OneX64_2,
};

struct FineTuneAddress
{
	FineTuneDeviceKind device = FineTuneDeviceKind::Mcs1;
	/// MCS: block 1..32, channel 1..18.
	int mcsBlock1to32 = 1;
	int mcsCh1to18 = 1;
	/// 1x64 Mapping: SW 1..4, CH_y 1..17.
	int sw1to4 = 1;
	int chY1to17 = 1;
};

struct FineTuneSyncPayload
{
	BOOL isMcs = FALSE;
	int mcsTransIndex = 0;   // 0 or 1
	int oneX64Dev = 0;       // 0 or 1
	int oneX64Sw = 0;        // 0..3
	stLutSettingZ4671 lut{};
	stM576OneX64MemsSwCoef mems{};
};

/// Burn-file index 0..9 for the selected device (+ SW for 1x64).
BOOL FineTuneBurnIndex(const FineTuneAddress& addr, int& burnIndexOut, CString& errMsg);

/// Resolve Out-dir Backup/Standard path for the address (requires valid SN info).
BOOL FineTuneResolvePath(
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	M576BinFileRole role,
	const FineTuneAddress& addr,
	CString& pathOut,
	int& burnIndexOut,
	CString& errMsg);

/// Read current low-temp DAC_X / DAC_Y (signed) from an existing bin.
BOOL FineTuneReadCurrentDac(
	LPCTSTR binPath,
	const FineTuneAddress& addr,
	short& dacXOut,
	short& dacYOut,
	CString& errMsg);

/// Patch low-temp DAC and overwrite bin; optional sync payload for in-memory LUT/MEMS.
BOOL FineTuneWriteDac(
	LPCTSTR binPath,
	const FineTuneAddress& addr,
	short dacX,
	short dacY,
	FineTuneSyncPayload* pSyncOut,
	CString& errMsg);

/// MCS RECAL picker: 32 blocks x 18 CH = 576 (same order as pm_mcs*.csv).
inline constexpr int kFineTuneMcsRecalCount = 32 * 18;

BOOL FineTuneMcsRecalIndexToBlockCh(int index0, int& block1to32Out, int& ch1to18Out);
CString FineTuneFormatMcsRecalLabel(BOOL isMcs1, int index0);

/// Resolve exe-relative Mapping.csv for 1X64_1 / 1X64_2 (same as PM path write-bin).
BOOL FineTuneResolve1x64MappingPath(FineTuneDeviceKind device, CString& mappingPathOut, CString& errMsg);

CString FineTuneFormat1x64RecalLabel(const SMems1x64PmMapRow& row, int index1based, int total);
