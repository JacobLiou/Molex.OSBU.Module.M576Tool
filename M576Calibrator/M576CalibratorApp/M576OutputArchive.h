#pragma once
// output\latest ???????? output\archive\{sessionId} ????ùr???? MFC ??????

#include "McsFwTransport.h"
#include <vector>

struct M576ArchiveStageEntry
{
	CString stageName;
	CString utcIso;
	int filesCopied{ 0 };
};

CString M576FormatUtcIso8601Z();
CString M576ResolveLatestBinDirAbs(LPCTSTR exeFolderAbs);
CString M576ResolveArchiveRootAbs(LPCTSTR exeFolderAbs);
CString M576BuildSessionFolderName(const M576TransSnPnInfo& sn);
BOOL M576EnsureDirTree(LPCTSTR absPath, CString& err);
BOOL M576WriteSessionMeta(
	LPCTSTR sessionDirAbs,
	LPCTSTR sessionId,
	const M576TransSnPnInfo& sn,
	LPCTSTR comPort,
	const std::vector<M576ArchiveStageEntry>& stages,
	CString& err);
BOOL M576ArchiveCopyBinSet(
	LPCTSTR latestDirAbs,
	LPCTSTR destSubDirAbs,
	const M576TransSnPnInfo& sn,
	M576BinFileRole role,
	BOOL includeDacCsv,
	int& outFilesCopied,
	CString& err);
BOOL M576ArchiveCopyCommLogSnapshot(
	LPCTSTR sessionLogsDirAbs,
	LPCTSTR commLogAbs,
	CString& outCopiedPath,
	CString& err);
