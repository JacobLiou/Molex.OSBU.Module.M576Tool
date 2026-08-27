#pragma once
// FullEdit bin <-> CSV IO (MFC / Z4671 writers). Not for CrossPeakTest.

#include "FullEditDacCsv.h"
#include "FullEditDacTypes.h"
#include "McsFwTransport.h"

#include <string>
#include <vector>

/// Ensure fulledit/{role}/working|baseline|path_impact dirs under outDirAbs.
BOOL FullEditEnsureDirs(LPCTSTR outDirAbs, M576BinFileRole role, CString& roleRootOut, CString& errMsg);

CString FullEditRoleDirName(M576BinFileRole role);

/// Export one MCS bin to UTF-8-SIG CSV path.
FullEditErrorCode FullEditExportMcsBinToCsv(
	LPCTSTR binPath,
	int burnIndex,
	LPCTSTR snLabel,
	LPCTSTR csvPath,
	CString& errMsg);

FullEditErrorCode FullEditExportMemsBinToCsv(
	LPCTSTR binPath,
	int burnIndex,
	LPCTSTR snLabel,
	int sw1to4,
	LPCTSTR csvPath,
	CString& errMsg);

/// Apply MCS patches and rewrite bin (CRC via CLutBinWriter).
FullEditErrorCode FullEditApplyMcsPatchesToBin(
	LPCTSTR binPath,
	const std::vector<FullEditMcsPatch>& patches,
	CString& errMsg);

FullEditErrorCode FullEditApplyMemsPatchesToBin(
	LPCTSTR binPath,
	int sw0,
	const std::vector<FullEditMemsPatch>& patches,
	CString& errMsg);

/// Read file to std::string (binary-safe for UTF-8).
BOOL FullEditReadFileUtf8(LPCTSTR path, std::string& out, CString& errMsg);

BOOL FullEditWriteFileUtf8Bom(LPCTSTR path, const std::string& bodyNoBom, CString& errMsg);

/// Export all 10 burn bins for role into working/ + baseline/.
FullEditErrorCode FullEditExportAllTen(
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	M576BinFileRole role,
	CString& errMsg);

/// Diff+apply all 10; returns DiffEmpty if nothing changed.
FullEditErrorCode FullEditValidateAndWriteAllTen(
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	M576BinFileRole role,
	const FullEditUnlockFlags& unlock,
	int& outFilesPatched,
	int& outRowsPatched,
	CString& errMsg);
