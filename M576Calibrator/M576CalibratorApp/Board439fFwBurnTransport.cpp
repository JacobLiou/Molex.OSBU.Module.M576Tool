#include "stdafx.h"
#include "Board439fFwBurnTransport.h"
#include "CalibConstants.h"
// Board439fFwBurnTransport.cpp：439F 控制板本机 XMODEM 固件烧录（不经 trans 隧道）。
#include <vector>
#include <cstring>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace {
// 439F main board direct XMODEM (standalone; does not call Switch1x64FwTransport).

static BOOL g_board439fEotUpgradeOk = FALSE;

enum {
	XMODEM_SOH = 0x01,
	XMODEM_STX = 0x02,
	XMODEM_EOT = 0x04,
	XMODEM_ACK = 0x06,
	XMODEM_NAK = 0x15,
	XMODEM_CAN = 0x18,
	XMODEM_BLOCK_HEAD_INDEX = 0,
	XMODEM_BLOCK_NO_INDEX = 1,
	XMODEM_BLOCK_NON_INDEX = 2,
	XMODEM_BLOCK_DATA_INDEX = 3,
	XMODEM_BLOCK_BODY_SIZE_128 = 128,
	XMODEM_BLOCK_BODY_SIZE_1K = 1024,
	XMODEM_COMMUNICATION_FAIL = 0,
	XMODEM_DOWNLOAD_SUCCESS = 2,
	XMODEM_DOWNLOAD_FAIL = 1,
};

static const TCHAR kFwBoardTag[] = _T("FW-Board");

static WORD Board439fXmodemCRC16(const BYTE* pb, WORD wLength)
{
	WORD wCRC = 0;
	while (wLength > 0)
	{
		wCRC = (WORD)(wCRC ^ ((WORD)(*pb++) << 8));
		for (WORD k = 0; k < 8; k++)
		{
			if (wCRC & 0x8000)
				wCRC = (WORD)((wCRC << 1) ^ 0x1021);
			else
				wCRC = (WORD)(wCRC << 1);
		}
		wLength--;
	}
	return wCRC;
}

static CString Board439fBytesToPrintable(const BYTE* p, DWORD n, DWORD cap)
{
	CString s;
	if (!p || n == 0)
		return s;
	const DWORD m = (n < cap) ? n : cap;
	s.Preallocate((int)(m * 4 + 8));
	for (DWORD i = 0; i < m; ++i)
	{
		const BYTE b = p[i];
		if (b == XMODEM_ACK)
			s += _T("<ACK>");
		else if (b == XMODEM_NAK)
			s += _T("<NAK>");
		else if (b == XMODEM_CAN)
			s += _T("<CAN>");
		else if (b == XMODEM_EOT)
			s += _T("<EOT>");
		else if (b == 0x43)
			s += _T("<C>");
		else if (b >= 0x20 && b <= 0x7E)
			s.AppendFormat(_T("%c"), (TCHAR)b);
		else if (b == '\r')
			s += _T("\\r");
		else if (b == '\n')
			s += _T("\\n");
		else
			s.AppendFormat(_T("\\x%02X"), (unsigned)b);
	}
	if (n > cap)
		s.AppendFormat(_T("...(+%lu)"), (unsigned long)(n - cap));
	return s;
}

static DWORD Board439fDrainAscii(Z4671Command& cmd, BYTE* outBuf, DWORD outCap, DWORD maxWaitMs, DWORD idleMs)
{
	if (!outBuf || outCap == 0)
		return 0;
	DWORD have = 0;
	const DWORD t0 = GetTickCount();
	DWORD lastDataTick = t0;
	while (GetTickCount() - t0 < maxWaitMs)
	{
		const DWORD avail = cmd.RxBytesWaiting();
		if (avail > 0 && have < outCap)
		{
			const DWORD want = ((outCap - have) < avail) ? (outCap - have) : avail;
			DWORD nread = 0;
			if (cmd.ReadBuffer((char*)(outBuf + have), want, &nread) && nread > 0)
			{
				have += nread;
				lastDataTick = GetTickCount();
			}
			continue;
		}
		if (have > 0 && (GetTickCount() - lastDataTick) >= idleMs)
			break;
		Sleep(2);
	}
	return have;
}

static BOOL Board439fFwdlBannerIsDeviceError(const BYTE* banner, DWORD bn)
{
	if (!banner || bn == 0)
		return FALSE;
	CStringA raw((const char*)banner, (int)bn);
	CString s(raw);
	s.MakeLower();
	if (s.Find(_T("invalid command")) >= 0)
		return TRUE;
	if (s.Find(_T("e:invalid")) >= 0)
		return TRUE;
	return FALSE;
}

/// 439F EOT: firmware may reply UpgradeOK (reboot in progress) or legacy Successful.
static BOOL Board439fEotResponseIsUpgradeOk(const char* buf, DWORD len)
{
	if (!buf || len < 9)
		return FALSE;
	return strstr(buf, "UpgradeOK") != nullptr;
}

static BOOL Board439fEotResponseHasSuccessfulBanner(const char* buf, DWORD len)
{
	if (!buf || len < 2)
		return FALSE;
	return strstr(buf, "Successful") != nullptr;
}

static int Board439fXmodemSendOneBlock(
	Z4671Command& cmd, BYTE* pbBinData, WORD wWireLen, BOOL bFileDone, int blockNo, int totalBlocks)
{
	char byTempBuf[256];
	int nRetry = 0;
	const bool verbose = (blockNo == 1) || (blockNo == totalBlocks) || ((blockNo % 4) == 0) || bFileDone;

	if (verbose)
	{
		const BYTE head = pbBinData[XMODEM_BLOCK_HEAD_INDEX];
		cmd.TraceInfo(
			kFwBoardTag,
			_T("XMODEM tx block %d/%d: head=0x%02X seq=%u inv=0x%02X wire=%u eotLast=%d"),
			blockNo,
			totalBlocks,
			(unsigned)head,
			(unsigned)pbBinData[XMODEM_BLOCK_NO_INDEX],
			(unsigned)pbBinData[XMODEM_BLOCK_NON_INDEX],
			(unsigned)wWireLen,
			(int)bFileDone);
	}
	if (!cmd.WriteBuffer((char*)pbBinData, wWireLen))
	{
		cmd.TraceError(kFwBoardTag, _T("XMODEM block %d/%d: initial TX (WriteBuffer) failed."), blockNo, totalBlocks);
		return XMODEM_COMMUNICATION_FAIL;
	}

	int nTimeOut = 0;
	while (1)
	{
		Sleep(50);
		DWORD dwReadLength = 0;
		if (cmd.ReadBuffer(byTempBuf, sizeof(byTempBuf) - 1, &dwReadLength) && dwReadLength > 0)
		{
			const BYTE lastB = (BYTE)byTempBuf[dwReadLength - 1];
			if (lastB == XMODEM_ACK)
			{
				if (verbose)
				{
					cmd.TraceInfo(
						kFwBoardTag,
						_T("XMODEM block %d/%d ACK after %d retry, rx_len=%lu, wait_ms~%d"),
						blockNo,
						totalBlocks,
						nRetry,
						(unsigned long)dwReadLength,
						nTimeOut * 50);
				}
				break;
			}
			if (lastB == XMODEM_NAK)
			{
				cmd.TraceInfo(
					kFwBoardTag,
					_T("XMODEM block %d/%d NAK (retry %d/%d), rx_len=%lu head=%s"),
					blockNo,
					totalBlocks,
					nRetry + 1,
					10,
					(unsigned long)dwReadLength,
					Board439fBytesToPrintable((const BYTE*)byTempBuf, dwReadLength, 32).GetString());
				if (!cmd.WriteBuffer((char*)pbBinData, wWireLen))
				{
					cmd.TraceError(
						kFwBoardTag,
						_T("XMODEM block %d/%d retry-TX (WriteBuffer) failed at retry %d."),
						blockNo,
						totalBlocks,
						nRetry + 1);
					return XMODEM_COMMUNICATION_FAIL;
				}
				nRetry++;
				if (nRetry > 10)
				{
					cmd.TraceError(
						kFwBoardTag,
						_T("XMODEM block %d/%d aborted: NAK retries exceeded (%d)."),
						blockNo,
						totalBlocks,
						nRetry);
					char can[3] = { (char)XMODEM_CAN, (char)XMODEM_CAN, (char)XMODEM_CAN };
					(void)cmd.WriteBuffer(can, 3);
					return XMODEM_COMMUNICATION_FAIL;
				}
				nTimeOut = 0;
				continue;
			}
			if (lastB == XMODEM_CAN)
			{
				cmd.TraceError(
					kFwBoardTag,
					_T("XMODEM block %d/%d aborted: CAN received from device, rx_len=%lu head=%s"),
					blockNo,
					totalBlocks,
					(unsigned long)dwReadLength,
					Board439fBytesToPrintable((const BYTE*)byTempBuf, dwReadLength, 32).GetString());
				return XMODEM_COMMUNICATION_FAIL;
			}
			cmd.TraceError(
				kFwBoardTag,
				_T("XMODEM block %d/%d unexpected response: last=0x%02X rx_len=%lu head=%s"),
				blockNo,
				totalBlocks,
				(unsigned)lastB,
				(unsigned long)dwReadLength,
				Board439fBytesToPrintable((const BYTE*)byTempBuf, dwReadLength, 32).GetString());
			return XMODEM_COMMUNICATION_FAIL;
		}
		nTimeOut++;
		if (nTimeOut >= 210)
		{
			cmd.TraceError(
				kFwBoardTag,
				_T("XMODEM block %d/%d timeout: no ACK/NAK in %d ms (after %d retry)."),
				blockNo,
				totalBlocks,
				nTimeOut * 50,
				nRetry);
			return XMODEM_COMMUNICATION_FAIL;
		}
	}

	if (bFileDone)
	{
		const char eot3[3] = { (char)XMODEM_EOT, (char)XMODEM_EOT, (char)XMODEM_EOT };
		int eotNakResends = 0;
		cmd.TraceInfo(kFwBoardTag, _T("XMODEM EOT phase begin (block %d/%d was last)."), blockNo, totalBlocks);
		for (int eotPass = 0; eotPass < 40; eotPass++)
		{
			if (!cmd.WriteBufferNoPurge((char*)eot3, 3))
			{
				cmd.TraceError(
					kFwBoardTag,
					_T("XMODEM EOT pass %d/40 TX (WriteBufferNoPurge) failed."),
					eotPass + 1);
				return XMODEM_COMMUNICATION_FAIL;
			}
			int nEotTo = 0;
			for (; nEotTo < 210; nEotTo++)
			{
				Sleep(50);
				ZeroMemory(byTempBuf, sizeof(byTempBuf));
				DWORD dwr = 0;
				if (cmd.ReadBuffer(byTempBuf, sizeof(byTempBuf) - 1, &dwr) && dwr > 0)
				{
					if (dwr < sizeof(byTempBuf) - 1)
						byTempBuf[dwr] = 0;
					// UpgradeOK may follow NAK+ACK prefix while the board is already rebooting.
					if (Board439fEotResponseIsUpgradeOk(byTempBuf, dwr))
					{
						g_board439fEotUpgradeOk = TRUE;
						cmd.TraceInfo(
							kFwBoardTag,
							_T("XMODEM EOT complete at pass %d/40: UpgradeOK (device rebooting, skip RSET): %s"),
							eotPass + 1,
							Board439fBytesToPrintable((const BYTE*)byTempBuf, dwr, 64).GetString());
						return XMODEM_DOWNLOAD_SUCCESS;
					}
					const BYTE firstB = (BYTE)byTempBuf[0];
					// Legacy Z4671: ACK at [0] + "Successful" banner.
					if (firstB == XMODEM_ACK)
					{
						const bool gotBanner = Board439fEotResponseHasSuccessfulBanner(byTempBuf, dwr);
						cmd.TraceInfo(
							kFwBoardTag,
							_T("XMODEM EOT ACK at pass %d/40 (rx_len=%lu, banner=%d, first-byte ACK): %s"),
							eotPass + 1,
							(unsigned long)dwr,
							(int)gotBanner,
							Board439fBytesToPrintable((const BYTE*)byTempBuf, dwr, 64).GetString());
						if (gotBanner)
							return XMODEM_DOWNLOAD_SUCCESS;
						cmd.TraceError(
							kFwBoardTag,
							_T("XMODEM EOT ACK without Successful banner at pass %d/40."),
							eotPass + 1);
						return XMODEM_DOWNLOAD_FAIL;
					}
					if (firstB == XMODEM_NAK)
					{
						eotNakResends++;
						cmd.TraceInfo(
							kFwBoardTag,
							_T("XMODEM EOT NAK at pass %d/40 (resend %d/32), rx_len=%lu head=%s"),
							eotPass + 1,
							eotNakResends,
							(unsigned long)dwr,
							Board439fBytesToPrintable((const BYTE*)byTempBuf, dwr, 32).GetString());
						if (eotNakResends > 32)
						{
							cmd.TraceError(
								kFwBoardTag,
								_T("XMODEM EOT aborted: NAK resends exceeded (%d)."),
								eotNakResends);
							return XMODEM_DOWNLOAD_FAIL;
						}
						break;
					}
					if (firstB == XMODEM_CAN)
					{
						cmd.TraceError(
							kFwBoardTag,
							_T("XMODEM EOT aborted: CAN received at pass %d/40, rx_len=%lu head=%s"),
							eotPass + 1,
							(unsigned long)dwr,
							Board439fBytesToPrintable((const BYTE*)byTempBuf, dwr, 32).GetString());
						return XMODEM_COMMUNICATION_FAIL;
					}
					cmd.TraceInfo(
						kFwBoardTag,
						_T("XMODEM EOT pass %d/40 unexpected (first=0x%02X), rx_len=%lu head=%s"),
						eotPass + 1,
						(unsigned)firstB,
						(unsigned long)dwr,
						Board439fBytesToPrintable((const BYTE*)byTempBuf, dwr, 32).GetString());
				}
			}
			if (nEotTo >= 210)
			{
				cmd.TraceError(
					kFwBoardTag,
					_T("XMODEM EOT pass %d/40 timeout: no ACK/NAK in %d ms (NAK resends so far=%d)."),
					eotPass + 1,
					nEotTo * 50,
					eotNakResends);
				return XMODEM_COMMUNICATION_FAIL;
			}
		}
		cmd.TraceError(
			kFwBoardTag, _T("XMODEM EOT exhausted 40 passes (NAK resends=%d) without ACK."), eotNakResends);
		return XMODEM_DOWNLOAD_FAIL;
	}
	return XMODEM_DOWNLOAD_SUCCESS;
}

} // namespace

BOOL M576Board439fLastBurnDeviceRebooting(void)
{
	return g_board439fEotUpgradeOk;
}

int M576Board439fXmodemChunkCountForFileSize(DWORD fileBytes)
{
	DWORD left = fileBytes;
	int c = 0;
	while (left > 0)
	{
		WORD w;
		if (left >= 1024u)
			w = 1024;
		else if (left >= 128u)
			w = 128;
		else
			w = (WORD)left;
		left -= w;
		c++;
	}
	return c;
}

BOOL M576BurnBoard439fFirmware(
	Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user)
{
	err.Empty();
	g_board439fEotUpgradeOk = FALSE;

	HANDLE hPort = cmd.GetPortHandle();
	if (!hPort || hPort == INVALID_HANDLE_VALUE)
	{
		err = _T("Board burn: serial port is not open.");
		cmd.TraceError(kFwBoardTag, _T("%s"), err.GetString());
		return FALSE;
	}

	// Stage 2 first: open file to know size for stage 0 log (file must exist before burn).
	cmd.TraceInfo(kFwBoardTag, _T("XMODEM stage 2/4 (open): %s"), szBinPath);
	HANDLE hBin = CreateFile(szBinPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hBin == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Board bin file: open failed: %s (GLE=%lu)"), szBinPath, (unsigned long)GetLastError());
		cmd.TraceError(kFwBoardTag, _T("%s"), err.GetString());
		return FALSE;
	}
	const DWORD dwFile = GetFileSize(hBin, NULL);
	if (dwFile == INVALID_FILE_SIZE)
	{
		CloseHandle(hBin);
		err = _T("Board bin file: GetFileSize failed.");
		cmd.TraceError(kFwBoardTag, _T("%s (GLE=%lu)"), err.GetString(), (unsigned long)GetLastError());
		return FALSE;
	}
	if (dwFile == 0)
	{
		CloseHandle(hBin);
		err = _T("Board bin file: file is empty.");
		cmd.TraceError(kFwBoardTag, _T("%s"), err.GetString());
		return FALSE;
	}

	const int totalBlocks = M576Board439fXmodemChunkCountForFileSize(dwFile);
	cmd.TraceInfo(
		kFwBoardTag,
		_T("XMODEM burn begin: bin=%s; size=%lu B; planned_blocks=%d; fwdl_pre_ms=%u"),
		szBinPath,
		(unsigned long)dwFile,
		totalBlocks,
		(unsigned)M576_BOARD_FWDL_PRE_MS);
	cmd.TraceInfo(
		kFwBoardTag,
		_T("XMODEM stage 2/4 (open) ok: file=%lu B, blocks=%d (1K + 128/last)"),
		(unsigned long)dwFile,
		totalBlocks);

	// Stage 1: fwdl\r on direct 439F link (no trans).
	(void)PurgeComm(hPort, PURGE_RXCLEAR);
	const char* fwdl = "fwdl\r";
	cmd.TraceInfo(kFwBoardTag, _T("XMODEM stage 1/4 (fwdl): TX 'fwdl\\r' -> %s"), szBinPath);
	if (!cmd.WriteBuffer((char*)fwdl, (DWORD)strlen(fwdl)))
	{
		CloseHandle(hBin);
		err = _T("Board fwdl rejected: fwdl write failed.");
		cmd.TraceError(kFwBoardTag, _T("XMODEM stage 1/4 (fwdl): TX failed."));
		return FALSE;
	}
	{
		BYTE banner[512];
		const DWORD bn = Board439fDrainAscii(
			cmd, banner, sizeof(banner), (DWORD)M576_BOARD_FWDL_PRE_MS, 250u);
		const CString bannerPrint = (bn > 0) ? Board439fBytesToPrintable(banner, bn, 256) : CString();
		if (bn > 0)
		{
			cmd.TraceInfo(
				kFwBoardTag,
				_T("XMODEM stage 1/4 (fwdl) banner (%lu B): %s"),
				(unsigned long)bn,
				bannerPrint.GetString());
			if (Board439fFwdlBannerIsDeviceError(banner, bn))
			{
				CloseHandle(hBin);
				err.Format(_T("Board fwdl rejected: %s"), bannerPrint.GetString());
				cmd.TraceError(kFwBoardTag, _T("%s"), err.GetString());
				return FALSE;
			}
		}
		else
		{
			cmd.TraceInfo(
				kFwBoardTag,
				_T("XMODEM stage 1/4 (fwdl): no banner bytes within %u ms (continuing anyway)."),
				(unsigned)M576_BOARD_FWDL_PRE_MS);
		}
	}

	// Stage 3: XMODEM stream.
	DWORD dwCodeSizeLeft = dwFile;
	std::vector<BYTE> blockbuf((size_t)(XMODEM_BLOCK_BODY_SIZE_1K + 32u));
	BYTE* pbBinData = blockbuf.data();
	int iCount = 0;
	BYTE bPacketIndex = 1;
	BOOL bFunctionOK = TRUE;
	cmd.TraceInfo(kFwBoardTag, _T("XMODEM stage 3/4 (stream): start sending %d blocks."), totalBlocks);

	while (dwCodeSizeLeft > 0)
	{
		iCount++;
		WORD wBody = 0;
		if (dwCodeSizeLeft >= (DWORD)XMODEM_BLOCK_BODY_SIZE_1K)
			wBody = (WORD)XMODEM_BLOCK_BODY_SIZE_1K;
		else if (dwCodeSizeLeft >= (DWORD)XMODEM_BLOCK_BODY_SIZE_128)
			wBody = (WORD)XMODEM_BLOCK_BODY_SIZE_128;
		else
			wBody = (WORD)dwCodeSizeLeft;
		const DWORD thisRead = wBody;
		dwCodeSizeLeft -= wBody;
		const BOOL bFileDone = (dwCodeSizeLeft == 0);
		ZeroMemory(pbBinData, (size_t)(XMODEM_BLOCK_BODY_SIZE_1K + 32u));
		DWORD dbr = 0;
		if (!ReadFile(hBin, pbBinData + XMODEM_BLOCK_DATA_INDEX, thisRead, &dbr, NULL) || dbr != thisRead)
		{
			err = _T("Board bin file: read from bin file failed.");
			cmd.TraceError(
				kFwBoardTag,
				_T("XMODEM stage 3/4 block %d/%d: ReadFile failed (want=%lu got=%lu, GLE=%lu)."),
				iCount,
				totalBlocks,
				(unsigned long)thisRead,
				(unsigned long)dbr,
				(unsigned long)GetLastError());
			bFunctionOK = FALSE;
			break;
		}
		WORD wCrcSize = wBody;
		if (wBody < (WORD)XMODEM_BLOCK_BODY_SIZE_128)
			wCrcSize = (WORD)XMODEM_BLOCK_BODY_SIZE_128;
		WORD wCRC = Board439fXmodemCRC16(pbBinData + XMODEM_BLOCK_DATA_INDEX, wCrcSize);
		if (wCrcSize == (WORD)XMODEM_BLOCK_BODY_SIZE_1K)
			pbBinData[XMODEM_BLOCK_HEAD_INDEX] = (BYTE)XMODEM_STX;
		else
			pbBinData[XMODEM_BLOCK_HEAD_INDEX] = (BYTE)XMODEM_SOH;
		pbBinData[XMODEM_BLOCK_NO_INDEX] = bPacketIndex;
		pbBinData[XMODEM_BLOCK_NON_INDEX] = (BYTE)(0xFFu - (unsigned)bPacketIndex);
		pbBinData[XMODEM_BLOCK_DATA_INDEX + wCrcSize] = (BYTE)(wCRC >> 8);
		pbBinData[XMODEM_BLOCK_DATA_INDEX + wCrcSize + 1] = (BYTE)wCRC;
		const WORD wWire = (WORD)(wCrcSize + 5);
		const bool verbose = (iCount == 1) || (iCount == totalBlocks) || ((iCount % 4) == 0) || bFileDone;
		if (verbose)
		{
			cmd.TraceInfo(
				kFwBoardTag,
				_T("XMODEM stage 3/4 prep block %d/%d: body=%u crc_size=%u wire=%u crc=0x%04X file_left=%lu eotLast=%d"),
				iCount,
				totalBlocks,
				(unsigned)wBody,
				(unsigned)wCrcSize,
				(unsigned)wWire,
				(unsigned)wCRC,
				(unsigned long)dwCodeSizeLeft,
				(int)bFileDone);
		}
		const int xr = Board439fXmodemSendOneBlock(cmd, pbBinData, wWire, bFileDone, iCount, totalBlocks);
		if (xr != XMODEM_DOWNLOAD_SUCCESS)
		{
			if (xr == XMODEM_DOWNLOAD_FAIL)
			{
				err.Format(
					_T("Board XMODEM EOT fail: xr=%d, block#%d/%d, eotLast=%d."),
					xr,
					iCount,
					totalBlocks,
					(int)bFileDone);
			}
			else
			{
				err.Format(
					_T("Board XMODEM comm fail: xr=%d, block#%d/%d, eotLast=%d, body=%u, wire=%u, crc=0x%04X."),
					xr,
					iCount,
					totalBlocks,
					(int)bFileDone,
					(unsigned)wBody,
					(unsigned)wWire,
					(unsigned)wCRC);
			}
			bFunctionOK = FALSE;
			break;
		}
		if (cb)
			cb(iCount, totalBlocks, user);
		bPacketIndex++;
	}
	CloseHandle(hBin);

	if (!bFunctionOK)
	{
		if (err.IsEmpty())
			err = _T("Board XMODEM comm fail: unknown failure.");
		cmd.TraceError(kFwBoardTag, _T("XMODEM stage 3/4 abort: %s"), err.GetString());
		return FALSE;
	}
	cmd.TraceInfo(
		kFwBoardTag,
		_T("XMODEM stage 3/4 (stream) done: %d/%d blocks ACK'd, EOT ACK'd."),
		iCount,
		totalBlocks);

	// Stage 4: RSET (skip when UpgradeOK — firmware already rebooting).
	if (g_board439fEotUpgradeOk)
	{
		cmd.TraceInfo(
			kFwBoardTag,
			_T("XMODEM stage 4/4 (RSET): skipped (UpgradeOK; board rebooting): %s"),
			szBinPath);
		return TRUE;
	}
	cmd.TraceInfo(kFwBoardTag, _T("XMODEM stage 4/4 (RSET): sleep 4 s then send RSET\\r."));
	Sleep(4000);
	{
		const char* rset = "RSET\r";
		if (!cmd.WriteBuffer((char*)rset, (DWORD)strlen(rset)))
		{
			err = _T("Board burn: RSET write failed.");
			cmd.TraceError(kFwBoardTag, _T("XMODEM stage 4/4 (RSET): WriteBuffer returned FALSE."));
			return FALSE;
		}
	}
	cmd.TraceInfo(kFwBoardTag, _T("Board burn done (RSET sent): %s"), szBinPath);
	return TRUE;
}
