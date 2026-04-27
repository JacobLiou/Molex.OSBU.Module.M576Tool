#include "stdafx.h"
#include "McsFwTransport.h"
#include "Board439fTransTunnel.h"
#include "LutBinWriter.h"
#include "CalibConstants.h"
#include "Switch1x64FwTransport.h"
#include <vector>
// McsFwTransport.cpp：Z4671 0xC4 分块读 LUT、StartFWUpdate 上载、多 trans 读备份/烧录与路径名规则。

#if defined(__cplusplus) && __cplusplus >= 201103L
#include <cstddef>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace {
// 匿名：0xC4 步长、单隧道整包读、分块上载到 Flash、MCS/1x64 通道判断与 1x64 基址。

/// 0xC4 bytes for this read step: at most `maxBlock` (128) per frame; the tail is `rem` when
/// `rem` < 128, never a merged (128,255] single request.
static int LutReadChunkSize(size_t rem, int maxBlock)
{
	if (rem == 0 || maxBlock < 1)
		return 0;
	if (rem <= (size_t)maxBlock)
		return (int)rem;
	return maxBlock;
}

static int CountReadChunksSimulate(size_t total, int maxBlock)
{
	if (total == 0 || maxBlock < 1)
		return 0;
	size_t off = 0;
	int n = 0;
	while (off < total)
	{
		const size_t rem = total - off;
		const int step = LutReadChunkSize(rem, maxBlock);
		if (step < 1)
			break;
		off += (size_t)step;
		++n;
	}
	return n;
}

// Z4671 flash read: caller already opened trans to target.
static BOOL ReadLutBundleOnCurrentTunnel(Z4671Command& cmd, LPCTSTR szOutPath, CString& err,
	McsFwProgressCb cb, void* user, int progressBase, int progressTotal, const CString& strBundleSn)
{
	err.Empty();
	cmd.TraceInfo(_T("FW"), _T("Read LUT bundle (tunnel): output=%s"), szOutPath);
	const size_t readBytes = CLutBinWriter::LutDevicePayloadSize();
	const size_t outFileBytes = CLutBinWriter::FullBundleFileSize();
	cmd.TraceInfo(_T("FW"), _T("Read LUT: 0xC4 fileType=%d flashBase=0x%08lX devicePayload=%Iu fileOut=%Iu"),
		M576_FLASH_FILE_TYPE, (unsigned long)M576_FLASH_LUT_READ_BASE, readBytes, outFileBytes);
	if (readBytes == 0)
	{
		err = _T("Invalid device payload size.");
		cmd.TraceError(_T("FW"), _T("Read LUT bundle aborted: invalid device payload size."));
		return FALSE;
	}
	std::vector<BYTE> buf;
	buf.resize(readBytes);
	size_t offset = 0;
	const int typ = M576_FLASH_FILE_TYPE;
	// 0xC4: always min(128, rem) so full steps are 128; last call is 1..128 (remainder when total%128 != 0).
	const int nChunkTotal = CountReadChunksSimulate(readBytes, M576_FLASH_READ_CHUNK_MAX);
	int nChunkDone = 0;

	while (offset < readBytes)
	{
		const size_t rem = readBytes - offset;
		const int kBlock = M576_FLASH_READ_CHUNK_MAX;
		const int req = LutReadChunkSize(rem, kBlock);
		const DWORD flashAddr = M576_FLASH_LUT_READ_BASE + (DWORD)offset;
		cmd.TraceInfo(_T("FW"), _T("Read flash chunk %d/%d: addr=0x%08lX req=%d"), nChunkDone + 1, nChunkTotal, flashAddr, req);
		BYTE* pPayload = NULL;
		int nLen = 0;
		if (!cmd.GetLogFileData(typ, req, flashAddr, &pPayload, &nLen))
		{
			err = cmd.m_strLogInfo.IsEmpty() ? _T("GetLogFileData failed") : cmd.m_strLogInfo;
			cmd.TraceError(_T("FW"), _T("Read flash chunk failed at addr=0x%08lX req=%d: %s"), flashAddr, req, err.GetString());
			return FALSE;
		}
		if (pPayload == NULL || nLen <= 0)
		{
			err = _T("Empty flash read payload.");
			cmd.TraceError(_T("FW"), _T("Empty flash payload at addr=0x%08lX req=%d"), flashAddr, req);
			return FALSE;
		}
		const int copyLen = (nLen < req) ? nLen : req;
		if ((size_t)copyLen > readBytes - offset)
		{
			err = _T("Flash read length overflow.");
			cmd.TraceError(_T("FW"), _T("Flash read overflow at addr=0x%08lX len=%d"), flashAddr, copyLen);
			return FALSE;
		}
		memcpy(&buf[offset], pPayload, (size_t)copyLen);
		offset += (size_t)copyLen;
		nChunkDone++;
		cmd.TraceInfo(_T("FW"), _T("Read flash chunk %d/%d complete: received=%d device=%Iu/%Iu"), nChunkDone, nChunkTotal, copyLen, offset, readBytes);
		if (cb)
			cb(progressBase + nChunkDone, progressTotal, user);
		if ((size_t)copyLen < (size_t)req)
		{
			err = _T("Short flash read; check type/base with firmware.");
			cmd.TraceError(_T("FW"), _T("Short flash read at addr=0x%08lX req=%d got=%d"), flashAddr, req, copyLen);
			return FALSE;
		}
		Sleep(20);
	}

	if (readBytes != sizeof(stLutSettingZ4671))
	{
		err = _T("Device payload size mismatch stLutSettingZ4671.");
		cmd.TraceError(_T("FW"), _T("LutDevicePayloadSize %Iu != sizeof(stLutSettingZ4671)"), (size_t)readBytes);
		return FALSE;
	}
	stLutSettingZ4671 lut;
	memcpy(&lut, buf.data(), readBytes);
	SLutBinWriteParams wparams;
	wparams.strOutputPath = szOutPath;
	wparams.pLut = &lut;
	wparams.strBundleSN = strBundleSn;
	if (!CLutBinWriter::Write(wparams))
	{
		err = _T("CLutBinWriter::Write (assemble bundle) failed.");
		cmd.TraceError(_T("FW"), _T("CLutBinWriter::Write failed: %s"), szOutPath);
		return FALSE;
	}
	cmd.TraceInfo(_T("FW"), _T("Read LUT complete: device=%Iu out=%Iu path=%s"), readBytes, outFileBytes, szOutPath);
	return TRUE;
}

// Z4671 burn: caller already opened trans to target.
static BOOL UploadBinOnCurrentTunnel(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user,
	int progressBase, int progressTotal)
{
	err.Empty();
	cmd.TraceInfo(_T("FW"), _T("Flash start (tunnel): bin=%s"), szBinPath);
	if (!cmd.StartFWUpdate())
	{
		err = cmd.m_strLogInfo;
		cmd.TraceError(_T("FW"), _T("StartFWUpdate failed: %s"), err.GetString());
		return FALSE;
	}
	cmd.TraceInfo(_T("FW"), _T("StartFWUpdate acknowledged."));
	Sleep(100);

	HANDLE hBin = CreateFile(szBinPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hBin == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Open BIN failed: %s"), szBinPath);
		cmd.TraceError(_T("FW"), _T("Open BIN failed: %s"), szBinPath);
		(void)cmd.FWUpdateEnd();
		return FALSE;
	}
	DWORD dwCodeSizeLeft = GetFileSize(hBin, NULL);
	if (dwCodeSizeLeft == 0)
	{
		err = _T("BIN file is empty.");
		cmd.TraceError(_T("FW"), _T("BIN file is empty: %s"), szBinPath);
		CloseHandle(hBin);
		(void)cmd.FWUpdateEnd();
		return FALSE;
	}
	int iCnt = (int)(dwCodeSizeLeft / MAX_DATA_LENGTH);
	const int totalChunks = iCnt + 1;
	cmd.TraceInfo(_T("FW"), _T("Flash bin opened: bytes=%lu chunks=%d"), dwCodeSizeLeft, totalChunks);
	char pbBinData[MAX_DATA_LENGTH];
	DWORD dwBytesRead = 0;
	int iCount = 0;

	while (dwCodeSizeLeft)
	{
		iCount++;
		WORD wDownloadSize = MAX_DATA_LENGTH;
		if (iCount > iCnt)
			wDownloadSize = (WORD)(dwCodeSizeLeft % MAX_DATA_LENGTH);
		dwCodeSizeLeft -= wDownloadSize;

		ZeroMemory(pbBinData, sizeof(pbBinData));
		if (!ReadFile(hBin, pbBinData, wDownloadSize, &dwBytesRead, NULL) || dwBytesRead != wDownloadSize)
		{
			err = _T("Read BIN chunk failed.");
			cmd.TraceError(_T("FW"), _T("Read BIN chunk failed: chunk=%d/%d size=%u"), iCount, totalChunks, wDownloadSize);
			CloseHandle(hBin);
			(void)cmd.FWUpdateEnd();
			return FALSE;
		}
		cmd.TraceInfo(_T("FW"), _T("Flash chunk %d/%d: size=%u remaining=%lu"), iCount, totalChunks, wDownloadSize, dwCodeSizeLeft);
		if (cb)
			cb(progressBase + iCount, progressTotal, user);
		if (!cmd.FWTranSportFW((BYTE*)pbBinData, wDownloadSize, iCount, totalChunks))
		{
			err = cmd.m_strLogInfo;
			cmd.TraceError(_T("FW"), _T("FWTranSportFW failed: chunk=%d/%d size=%u err=%s"), iCount, totalChunks, wDownloadSize, err.GetString());
			CloseHandle(hBin);
			(void)cmd.FWUpdateEnd();
			return FALSE;
		}
		cmd.TraceInfo(_T("FW"), _T("Flash chunk %d/%d acknowledged."), iCount, totalChunks);
	}
	CloseHandle(hBin);

	Sleep(1000);
	if (!cmd.FWUpdateEnd())
	{
		err = cmd.m_strLogInfo.IsEmpty() ? _T("FWUpdateEnd failed") : cmd.m_strLogInfo;
		cmd.TraceError(_T("FW"), _T("FWUpdateEnd failed: %s"), err.GetString());
		return FALSE;
	}
	cmd.TraceInfo(_T("FW"), _T("FWUpdateEnd acknowledged. Waiting for firmware settle."));
	Sleep(5000);
	cmd.TraceInfo(_T("FW"), _T("Flash complete (tunnel): bin=%s"), szBinPath);
	return TRUE;
}

static BOOL IsMcsTransChannel(int ch) { return ch == 1 || ch == 2; }

static DWORD FlashBase1x64ForTrans(int ch)
{
	if (ch == 3) return (DWORD)M576_1X64_FLASH_BASE_TRANS3;
	if (ch == 4) return (DWORD)M576_1X64_FLASH_BASE_TRANS4;
	return 0;
}

} // namespace

// 在路径扩展名前插入 _mcs1 等后缀，用于分 trans 输出文件名。
static CString M576PathInsertSuffixBeforeExt(LPCTSTR szBasePath, const CString& suffix)
{
	CString base(szBasePath);
	base.Trim();
	const int dot = base.ReverseFind(_T('.'));
	if (dot < 0)
		return base + suffix + _T(".bin");
	return base.Left(dot) + suffix + base.Mid(dot);
}

CString M576TransBackupPathFromBase(LPCTSTR szBasePath, int transChannel)
{
	CString suffix;
	if (transChannel >= 1 && transChannel <= 4)
		suffix = g_m576TransLutBinSuffix[transChannel - 1];
	else
		suffix.Format(_T("_t%d"), transChannel);
	return M576PathInsertSuffixBeforeExt(szBasePath, suffix);
}

CString M576TransBinPathForRead(LPCTSTR szBasePath, int transChannel)
{
	const CString primary = M576TransBackupPathFromBase(szBasePath, transChannel);
	if (GetFileAttributes(primary) != INVALID_FILE_ATTRIBUTES)
		return primary;
	CString legacySuf;
	legacySuf.Format(_T("_t%d"), transChannel);
	const CString leg = M576PathInsertSuffixBeforeExt(szBasePath, legacySuf);
	if (GetFileAttributes(leg) != INVALID_FILE_ATTRIBUTES)
		return leg;
	return primary;
}

CString M576TransBinPathForSwitch(LPCTSTR szBasePath, int transChannel, int swIdx)
{
	if (transChannel == 1 || transChannel == 2)
		return M576TransBackupPathFromBase(szBasePath, transChannel);
	const CString primary = M576TransBackupPathFromBase(szBasePath, transChannel);
	if (swIdx < 0 || swIdx > 3)
		return primary;
	CString suf;
	suf.Format(_T("_sw%d"), swIdx + 1);
	const int dot = primary.ReverseFind(_T('.'));
	if (dot < 0)
		return primary + suf + _T(".bin");
	return primary.Left(dot) + suf + primary.Mid(dot);
}

// 无进度回调的 MCS LUT 上载（内部转 McsFwUploadBinEx）。
BOOL McsFwUploadBin(Z4671Command& cmd, LPCTSTR szBinPath, CString& err)
{
	return McsFwUploadBinEx(cmd, szBinPath, err, NULL, NULL);
}

// 按 g_m576FlashReadTransChannels 逐路 trans：MCS 用 0xC4+ReadLutBundle，1x64 用 MEM 8KB（见 Switch1x64）。
BOOL McsReadLutBundleFromDevice(
	Z4671Command& cmd, LPCTSTR szOutPathBase, CString& err, McsFwProgressCb cb, void* user, const CString snTrans[4])
{
	err.Empty();
	if (g_m576FlashReadTransChannelCount == 0)
	{
		err = _T("No read trans channels configured.");
		return FALSE;
	}
	CString discard;
	(void)Board439fTransTunnel::EndTrans(cmd, discard);

	const size_t devicePayload = CLutBinWriter::LutDevicePayloadSize();
	if (devicePayload == 0)
	{
		err = _T("Invalid device payload size.");
		return FALSE;
	}
	const int mcsChunks = CountReadChunksSimulate(devicePayload, M576_FLASH_READ_CHUNK_MAX);
	const int x64Steps = M5761x64MemReadStepCount();
	int progressTotal = 0;
	for (std::size_t j = 0; j < g_m576FlashReadTransChannelCount; ++j)
	{
		const int tch = g_m576FlashReadTransChannels[j];
		progressTotal += IsMcsTransChannel(tch) ? mcsChunks : x64Steps;
	}
	int progressAt = 0;
	for (std::size_t i = 0; i < g_m576FlashReadTransChannelCount; ++i)
	{
		const int ch = g_m576FlashReadTransChannels[i];
		CString path = M576TransBackupPathFromBase(szOutPathBase, ch);
		cmd.TraceInfo(_T("FW"), _T("Read backup multi: trans=%d file=%s (%s)"),
			ch, path.GetString(), IsMcsTransChannel(ch) ? _T("MCS 0xC4+LUT") : _T("1x64 MEM 4x2K"));
		if (!Board439fTransTunnel::BeginTrans(cmd, ch, err))
		{
			err.Format(_T("trans %d: %s"), ch, err.GetString());
			(void)Board439fTransTunnel::EndTrans(cmd, discard);
			return FALSE;
		}
		const int progressBase = progressAt;
		if (IsMcsTransChannel(ch))
		{
			if (!ReadLutBundleOnCurrentTunnel(
					cmd, path, err, cb, user, progressBase, progressTotal, snTrans[ch - 1]))
			{
				err.Format(_T("trans %d: %s"), ch, err.GetString());
				(void)Board439fTransTunnel::EndTrans(cmd, discard);
				return FALSE;
			}
			progressAt += mcsChunks;
		}
		else
		{
			const DWORD x64Base = FlashBase1x64ForTrans(ch);
			if (x64Base == 0)
			{
				err = _T("1x64 trans channel must be 3 or 4.");
				(void)Board439fTransTunnel::EndTrans(cmd, discard);
				return FALSE;
			}
			if (!M576Read1x64MemsBinOnCurrentTunnel(
					cmd, szOutPathBase, ch, x64Base, err, cb, user, progressBase, progressTotal, snTrans[ch - 1]))
			{
				err.Format(_T("trans %d: %s"), ch, err.GetString());
				(void)Board439fTransTunnel::EndTrans(cmd, discard);
				return FALSE;
			}
			progressAt += x64Steps;
		}
		if (!Board439fTransTunnel::EndTrans(cmd, err))
		{
			err.Format(_T("trans %d end $$: %s"), ch, err.GetString());
			return FALSE;
		}
	}
	cmd.TraceInfo(_T("FW"), _T("All read-backup channels done (base path=%s)."), szOutPathBase);
	return TRUE;
}

// 多路烧录：对 g_m576FlashBurnTransChannels 中存在的分 trans bin 上载（MCS 400B 流 / 1x64 XMODEM），合并进度。
BOOL McsFwUploadBinEx(Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user)
{
	err.Empty();
	if (g_m576FlashBurnTransChannelCount == 0)
	{
		err = _T("No burn trans channels configured.");
		return FALSE;
	}
	CString discard;
	(void)Board439fTransTunnel::EndTrans(cmd, discard);

	std::vector<int> chunksPerChannel;
	chunksPerChannel.resize(g_m576FlashBurnTransChannelCount);
	int progressTotal = 0;
	for (std::size_t pi = 0; pi < g_m576FlashBurnTransChannelCount; ++pi)
	{
		const int pch = g_m576FlashBurnTransChannels[pi];
		const CString pathProbe = M576TransBinPathForRead(szBinPath, pch);
		DWORD sz = 0;
		HANDLE hProbe = CreateFile(pathProbe, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hProbe != INVALID_HANDLE_VALUE)
		{
			sz = GetFileSize(hProbe, NULL);
			CloseHandle(hProbe);
		}
		int chunks = 0;
		if (sz > 0)
		{
			if (IsMcsTransChannel(pch))
			{
				const int iCnt = (int)(sz / MAX_DATA_LENGTH);
				chunks = iCnt + 1;
			}
			else
			{
				// 1x64：每路 2K 一个 XMODEM 链；四个 sw 文件 `*_1x64_*_swN.bin` 的块数累加
				int c = 0;
				for (int sw = 0; sw < 4; ++sw)
				{
					const CString p = M576TransBinPathForSwitch(szBinPath, pch, sw);
					if (GetFileAttributes(p) == INVALID_FILE_ATTRIBUTES)
						continue;
					HANDLE hP = CreateFile(
						p, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if (hP == INVALID_HANDLE_VALUE)
						continue;
					const DWORD fsz = GetFileSize(hP, NULL);
					CloseHandle(hP);
					if (fsz != (DWORD)M576_1X64_MEMS_BIN_SIZE)
						continue;
					c += M5761x64XmodemChunkCountForFileSize(fsz);
				}
				chunks = c;
			}
		}
		chunksPerChannel[pi] = chunks;
		progressTotal += chunks;
	}
	if (progressTotal <= 0)
	{
		err.Format(_T("No non-empty per-trans .bin (e.g. *_mcs1.bin) from base: %s"), szBinPath);
		return FALSE;
	}

	int progressBase = 0;
	for (std::size_t i = 0; i < g_m576FlashBurnTransChannelCount; ++i)
	{
		const int ch = g_m576FlashBurnTransChannels[i];
		const CString binPath = M576TransBinPathForRead(szBinPath, ch);
		const int chunksThis = chunksPerChannel[i];
		if (chunksThis <= 0)
		{
			cmd.TraceInfo(_T("FW"), _T("Burn skip trans=%d (missing or empty %s)."), ch, binPath.GetString());
			continue;
		}
		cmd.TraceInfo(_T("FW"), _T("Burn: trans=%d bin=%s (%s)"), ch, binPath.GetString(),
			IsMcsTransChannel(ch) ? _T("MCS FW stream") : _T("1x64 XMODEM"));
		if (!Board439fTransTunnel::BeginTrans(cmd, ch, err))
		{
			err.Format(_T("trans %d: %s"), ch, err.GetString());
			(void)Board439fTransTunnel::EndTrans(cmd, discard);
			return FALSE;
		}
		if (IsMcsTransChannel(ch))
		{
			if (!UploadBinOnCurrentTunnel(cmd, binPath, err, cb, user, progressBase, progressTotal))
			{
				err.Format(_T("trans %d: %s"), ch, err.GetString());
				(void)Board439fTransTunnel::EndTrans(cmd, discard);
				return FALSE;
			}
		}
		else
		{
			std::vector<CString> x64Paths;
			for (int sw = 0; sw < 4; ++sw)
			{
				const CString p = M576TransBinPathForSwitch(szBinPath, ch, sw);
				if (GetFileAttributes(p) == INVALID_FILE_ATTRIBUTES)
				{
					cmd.TraceInfo(
						_T("FW"), _T("Burn 1x64 trans=%d: skip missing sw %d: %s"), ch, sw + 1, p.GetString());
					continue;
				}
				HANDLE hP = CreateFile(
					p, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				if (hP == INVALID_HANDLE_VALUE)
					continue;
				const DWORD fsz = GetFileSize(hP, NULL);
				CloseHandle(hP);
				if (fsz != (DWORD)M576_1X64_MEMS_BIN_SIZE)
				{
					cmd.TraceInfo(
						_T("FW"),
						_T("Burn 1x64 trans=%d: skip sw %d (expected %u B, got %lu): %s"),
						ch, sw + 1, (unsigned)M576_1X64_MEMS_BIN_SIZE, (unsigned long)fsz, p.GetString());
					continue;
				}
				x64Paths.push_back(p);
			}
			if (x64Paths.empty())
			{
				err.Format(
					_T("trans %d: no valid 1x64 2K per-switch .bin (*_sw1..4) for base %s"),
					ch, szBinPath);
				(void)Board439fTransTunnel::EndTrans(cmd, discard);
				return FALSE;
			}
			int progressLocal = progressBase;
			for (size_t u = 0; u < x64Paths.size(); ++u)
			{
				const BOOL last = (u + 1u == x64Paths.size());
				if (!M576Upload1x64MemsBinOnCurrentTunnel(
						cmd, x64Paths[u], err, cb, user, progressLocal, progressTotal, last))
				{
					err.Format(_T("trans %d: %s"), ch, err.GetString());
					(void)Board439fTransTunnel::EndTrans(cmd, discard);
					return FALSE;
				}
				progressLocal += M5761x64XmodemChunkCountForFileSize((DWORD)M576_1X64_MEMS_BIN_SIZE);
			}
		}
		if (!Board439fTransTunnel::EndTrans(cmd, err))
		{
			err.Format(_T("trans %d end $$: %s"), ch, err.GetString());
			return FALSE;
		}
		progressBase += chunksThis;
	}
	cmd.TraceInfo(_T("FW"), _T("Burn finished all configured trans channels."));
	return TRUE;
}

BOOL McsReadAllTransProductSn(Z4671Command& cmd, CString snOut4[4], CString& err)
{
	err.Empty();
	CString discard;
	(void)Board439fTransTunnel::EndTrans(cmd, discard);
	for (int ti = 0; ti < 4; ++ti)
	{
		snOut4[ti].Empty();
		const int tch = ti + 1;
		if (!Board439fTransTunnel::BeginTrans(cmd, tch, err))
		{
			err.Format(_T("trans %d: %s"), tch, err.GetString());
			(void)Board439fTransTunnel::EndTrans(cmd, discard);
			return FALSE;
		}
		BOOL stepOk = FALSE;
		if (tch == 1 || tch == 2)
		{
			char pch[80];
			ZeroMemory(pch, sizeof(pch));
			if (cmd.GetProductSN(pch))
			{
				snOut4[ti] = pch;
				stepOk = TRUE;
			}
			else
				err = cmd.m_strLogInfo.IsEmpty() ? _T("MCS GetProductSN (0xA2) failed.") : cmd.m_strLogInfo;
		}
		else
		{
			CString e1;
			if (M576Read1x64SnStringOnCurrentTunnel(cmd, (DWORD)M576_1X64_SN_MEM_ADDR, snOut4[ti], e1))
				stepOk = TRUE;
			else
				err = e1;
		}
		if (!Board439fTransTunnel::EndTrans(cmd, discard))
		{
			err = _T("439F $$ at end of trans SN read failed.");
			return FALSE;
		}
		if (!stepOk)
		{
			if (err.IsEmpty())
				err.Format(_T("trans %d: read SN failed."), tch);
			return FALSE;
		}
		cmd.TraceInfo(_T("SN"), _T("trans %d -> %s"), tch, snOut4[ti].GetString());
	}
	return TRUE;
}
