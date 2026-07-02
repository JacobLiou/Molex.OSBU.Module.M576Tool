#include "stdafx.h"
#include "M576OutputArchive.h"
#include "CalibConstants.h"
// M576OutputArchive.cpp??latest ????????????archive ??????bin/comm ????????? meta.json??

namespace {

CStringA M576JsonEscapeA(LPCTSTR s)
{
	CStringA out;
	if (s == NULL)
		return out;
	for (; *s != 0; ++s)
	{
		const TCHAR c = *s;
		if (c == _T('\\'))
			out += "\\\\";
		else if (c == _T('"'))
			out += "\\\"";
		else if (c >= 32 && c < 127)
			out += (char)c;
		else
			out += '?';
	}
	return out;
}

BOOL M576CopyOneFile(LPCTSTR src, LPCTSTR dst, CString& err)
{
	if (GetFileAttributes(src) == INVALID_FILE_ATTRIBUTES)
		return TRUE;
	if (!CopyFile(src, dst, FALSE))
	{
		err.Format(_T("CopyFile failed: %s -> %s (err=%lu)"), src, dst, GetLastError());
		return FALSE;
	}
	return TRUE;
}

int M576CopyDacCsvIfPresent(LPCTSTR latestDir, LPCTSTR destDir, LPCTSTR mcsSnSanitized, CString& err)
{
	static const LPCTSTR kLeaves[] = {
		_T("backupAll1310DAC.csv"),
		_T("standardAll1310DAC.csv"),
		_T("backupAll1550DAC.csv"),
		_T("standardAll1550DAC.csv"),
	};
	int n = 0;
	for (LPCTSTR leaf : kLeaves)
	{
		CString src;
		src.Format(_T("%s\\%s_%s"), latestDir, mcsSnSanitized, leaf);
		if (GetFileAttributes(src) == INVALID_FILE_ATTRIBUTES)
			continue;
		CString dst;
		dst.Format(_T("%s\\%s_%s"), destDir, mcsSnSanitized, leaf);
		if (!M576CopyOneFile(src, dst, err))
			return -1;
		++n;
	}
	return n;
}

} // namespace

static CString M576JoinExeRelativePath(LPCTSTR exeFolderAbs, LPCTSTR relFromExe)
{
	if (exeFolderAbs == NULL || exeFolderAbs[0] == 0)
		return CString();
	CString base = exeFolderAbs;
	while (!base.IsEmpty() && (base[base.GetLength() - 1] == _T('\\') || base[base.GetLength() - 1] == _T('/')))
		base = base.Left(base.GetLength() - 1);
	CString rel = relFromExe;
	while (!rel.IsEmpty() && (rel[0] == _T('\\') || rel[0] == _T('/')))
		rel = rel.Mid(1);
	if (base.IsEmpty())
		return rel;
	if (rel.IsEmpty())
		return base;
	return base + _T("\\") + rel;
}

CString M576FormatUtcIso8601Z()
{
	SYSTEMTIME st = {};
	GetSystemTime(&st);
	CString s;
	s.Format(
		_T("%04d-%02d-%02dT%02d:%02d:%02dZ"),
		(int)st.wYear,
		(int)st.wMonth,
		(int)st.wDay,
		(int)st.wHour,
		(int)st.wMinute,
		(int)st.wSecond);
	return s;
}

CString M576ResolveLatestBinDirAbs(LPCTSTR exeFolderAbs)
{
	return M576JoinExeRelativePath(exeFolderAbs, M576_BIN_LATEST_DIR_REL);
}

CString M576ResolveArchiveRootAbs(LPCTSTR exeFolderAbs)
{
	return M576JoinExeRelativePath(exeFolderAbs, M576_BIN_ARCHIVE_DIR_REL);
}

CString M576BuildSessionFolderName(const M576TransSnPnInfo& sn)
{
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	CString snPart = M576SanitizeSnForFilename(sn.mcsSn[0]);
	if (snPart.IsEmpty())
		snPart = _T("unknown");
	CString name;
	name.Format(
		_T("%04d%02d%02d_%02d%02d%02d_%s"),
		(int)st.wYear,
		(int)st.wMonth,
		(int)st.wDay,
		(int)st.wHour,
		(int)st.wMinute,
		(int)st.wSecond,
		snPart.GetString());
	return name;
}

BOOL M576EnsureDirTree(LPCTSTR absPath, CString& err)
{
	err.Empty();
	if (absPath == NULL || absPath[0] == 0)
	{
		err = _T("Directory path is empty.");
		return FALSE;
	}
	CString path = absPath;
	path.Trim();
	while (!path.IsEmpty() && (path[path.GetLength() - 1] == _T('\\') || path[path.GetLength() - 1] == _T('/')))
		path = path.Left(path.GetLength() - 1);
	for (int i = 0; i < path.GetLength(); ++i)
	{
		if (path[i] == _T('/'))
			path.SetAt(i, _T('\\'));
	}
	int start = 0;
	if (path.GetLength() >= 3 && path[1] == _T(':') && path[2] == _T('\\'))
		start = 3;
	for (int i = start; i <= path.GetLength(); ++i)
	{
		if (i == path.GetLength() || path[i] == _T('\\'))
		{
			const CString sub = path.Left(i);
			if (sub.IsEmpty())
				continue;
			if (!CreateDirectory(sub, NULL))
			{
				const DWORD e = GetLastError();
				if (e != ERROR_ALREADY_EXISTS)
				{
					err.Format(_T("CreateDirectory failed: %s (err=%lu)"), sub.GetString(), e);
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

BOOL M576WriteSessionMeta(
	LPCTSTR sessionDirAbs,
	LPCTSTR sessionId,
	const M576TransSnPnInfo& sn,
	LPCTSTR comPort,
	const std::vector<M576ArchiveStageEntry>& stages,
	CString& err)
{
	err.Empty();
	if (sessionDirAbs == NULL || sessionDirAbs[0] == 0)
	{
		err = _T("Session directory is empty.");
		return FALSE;
	}
	CString path;
	path.Format(_T("%s\\meta.json"), sessionDirAbs);
	CStringA body;
	body = "{\r\n";
	body += "  \"session_id\": \"";
	body += M576JsonEscapeA(sessionId ? sessionId : _T(""));
	body += "\",\r\n";
	body += "  \"created_utc\": \"";
	if (!stages.empty() && !stages.front().utcIso.IsEmpty())
		body += M576JsonEscapeA(stages.front().utcIso);
	else
		body += M576JsonEscapeA(M576FormatUtcIso8601Z());
	body += "\",\r\n";
	body += "  \"com_port\": \"";
	body += M576JsonEscapeA(comPort ? comPort : _T(""));
	body += "\",\r\n";
	body += "  \"mcs_sn\": [\"";
	body += M576JsonEscapeA(sn.mcsSn[0]);
	body += "\", \"";
	body += M576JsonEscapeA(sn.mcsSn[1]);
	body += "\"],\r\n";
	body += "  \"onex64_sn\": [\r\n";
	for (int d = 0; d < 2; ++d)
	{
		body += "    [";
		for (int sw = 0; sw < 4; ++sw)
		{
			if (sw > 0)
				body += ", ";
			body += "\"";
			body += M576JsonEscapeA(sn.oneX64Sn[d][sw]);
			body += "\"";
		}
		body += "]";
		if (d == 0)
			body += ",";
		body += "\r\n";
	}
	body += "  ],\r\n";
	body += "  \"stages\": [\r\n";
	for (size_t i = 0; i < stages.size(); ++i)
	{
		const M576ArchiveStageEntry& st = stages[i];
		body += "    {\"name\": \"";
		body += M576JsonEscapeA(st.stageName);
		body += "\", \"utc\": \"";
		body += M576JsonEscapeA(st.utcIso);
		body += "\", \"files\": ";
		CStringA n;
		n.Format("%d", st.filesCopied);
		body += n;
		body += "}";
		if (i + 1 < stages.size())
			body += ",";
		body += "\r\n";
	}
	body += "  ]\r\n";
	body += "}\r\n";

	CFile f;
	if (!f.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		err.Format(_T("Cannot write meta.json: %s"), path.GetString());
		return FALSE;
	}
	const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
	f.Write(bom, 3);
	f.Write(body.GetString(), (UINT)body.GetLength());
	f.Close();
	return TRUE;
}

BOOL M576ArchiveCopyBinSet(
	LPCTSTR latestDirAbs,
	LPCTSTR destSubDirAbs,
	const M576TransSnPnInfo& sn,
	M576BinFileRole role,
	BOOL includeDacCsv,
	int& outFilesCopied,
	CString& err)
{
	err.Empty();
	outFilesCopied = 0;
	if (latestDirAbs == NULL || latestDirAbs[0] == 0 || destSubDirAbs == NULL || destSubDirAbs[0] == 0)
	{
		err = _T("Archive copy: directory path is empty.");
		return FALSE;
	}
	if (!M576EnsureDirTree(destSubDirAbs, err))
		return FALSE;

	std::array<CString, M576_BURN_FILE_COUNT> paths;
	CString buildErr;
	if (!M576BuildBurnFilePaths(latestDirAbs, sn, role, paths, buildErr))
	{
		err = buildErr;
		return FALSE;
	}

	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
	{
		const CString& src = paths[i];
		if (GetFileAttributes(src) == INVALID_FILE_ATTRIBUTES)
			continue;
		const int slash = src.ReverseFind(_T('\\'));
		const CString leaf = (slash >= 0) ? src.Mid(slash + 1) : src;
		CString dst;
		dst.Format(_T("%s\\%s"), destSubDirAbs, leaf.GetString());
		if (!M576CopyOneFile(src, dst, err))
			return FALSE;
		++outFilesCopied;
	}

	if (includeDacCsv)
	{
		const CString snPart = M576SanitizeSnForFilename(sn.mcsSn[0]);
		const int dacN = M576CopyDacCsvIfPresent(latestDirAbs, destSubDirAbs, snPart, err);
		if (dacN < 0)
			return FALSE;
		outFilesCopied += dacN;
	}
	return TRUE;
}

BOOL M576ArchiveCopyRunPathLogs(
	LPCTSTR sessionLogsDirAbs,
	LPCTSTR commLogAbs,
	LPCTSTR recalSweepCsvAbs,
	CString& outCopiedCommPath,
	CString& outCopiedSweepPath,
	CString& err)
{
	err.Empty();
	outCopiedCommPath.Empty();
	outCopiedSweepPath.Empty();
	if (sessionLogsDirAbs == NULL || sessionLogsDirAbs[0] == 0)
	{
		err = _T("Session logs directory is empty.");
		return FALSE;
	}
	if (!M576EnsureDirTree(sessionLogsDirAbs, err))
		return FALSE;

	SYSTEMTIME st = {};
	GetLocalTime(&st);
	BOOL ok = TRUE;

	if (commLogAbs != NULL && commLogAbs[0] != 0 && GetFileAttributes(commLogAbs) != INVALID_FILE_ATTRIBUTES)
	{
		CString dst;
		dst.Format(
			_T("%s\\comm_%04d%02d%02d_%02d%02d%02d.log"),
			sessionLogsDirAbs,
			(int)st.wYear,
			(int)st.wMonth,
			(int)st.wDay,
			(int)st.wHour,
			(int)st.wMinute,
			(int)st.wSecond);
		CString copyErr;
		if (!M576CopyOneFile(commLogAbs, dst, copyErr))
		{
			err = copyErr;
			ok = FALSE;
		}
		else
			outCopiedCommPath = dst;
	}

	if (recalSweepCsvAbs != NULL && recalSweepCsvAbs[0] != 0
		&& GetFileAttributes(recalSweepCsvAbs) != INVALID_FILE_ATTRIBUTES)
	{
		const CString sweepSrc(recalSweepCsvAbs);
		int slash = sweepSrc.ReverseFind(_T('\\'));
		if (slash < 0)
			slash = sweepSrc.ReverseFind(_T('/'));
		const CString fname = (slash >= 0) ? sweepSrc.Mid(slash + 1) : sweepSrc;
		CString dst;
		dst.Format(_T("%s\\%s"), sessionLogsDirAbs, fname.GetString());
		CString copyErr;
		if (!M576CopyOneFile(recalSweepCsvAbs, dst, copyErr))
		{
			if (!err.IsEmpty())
				err += _T("; ");
			err += copyErr;
			ok = FALSE;
		}
		else
			outCopiedSweepPath = dst;
	}
	return ok;
}

BOOL M576ArchiveCopyCommLogSnapshot(
	LPCTSTR sessionLogsDirAbs,
	LPCTSTR commLogAbs,
	CString& outCopiedPath,
	CString& err)
{
	CString unusedSweep;
	return M576ArchiveCopyRunPathLogs(
		sessionLogsDirAbs,
		commLogAbs,
		NULL,
		outCopiedPath,
		unusedSweep,
		err);
}
