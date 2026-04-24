#include "stdafx.h"
#include "Switch1x64FwTransport.h"
#include "CalibConstants.h"
#include <vector>
#include <cstring>
#include <cctype>
#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace {

// --- XMODEM (from legacy 126S DataDownLoad / XmodemloadData) ---
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

static WORD XmodemCRC16Bytes(const BYTE* pb, WORD wLength)
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

static int XmodemSendOneBlock(Z4671Command& cmd, BYTE* pbBinData, WORD wWireLen, BOOL bFileDone)
{
	char byTempBuf[256];
	int nRetry = 0;

	if (!cmd.WriteBuffer((char*)pbBinData, wWireLen))
		return XMODEM_COMMUNICATION_FAIL;

	while (1)
	{
		Sleep(50);
		int nTimeOut = 0;
		DWORD dwReadLength = 0;
		if (cmd.ReadBuffer(byTempBuf, sizeof(byTempBuf) - 1, &dwReadLength) && dwReadLength > 0)
		{
			if ((BYTE)byTempBuf[dwReadLength - 1] == XMODEM_ACK)
				break;
			if ((BYTE)byTempBuf[dwReadLength - 1] == XMODEM_NAK)
			{
				if (!cmd.WriteBuffer((char*)pbBinData, wWireLen))
					return XMODEM_COMMUNICATION_FAIL;
				nRetry++;
				if (nRetry > 10)
				{
					char can[3] = { (char)XMODEM_CAN, (char)XMODEM_CAN, (char)XMODEM_CAN };
					(void)cmd.WriteBuffer(can, 3);
					return XMODEM_COMMUNICATION_FAIL;
				}
			}
			else if ((BYTE)byTempBuf[dwReadLength - 1] == XMODEM_CAN)
				return XMODEM_COMMUNICATION_FAIL;
			else
				return XMODEM_COMMUNICATION_FAIL;
		}
		nTimeOut++;
		if (nTimeOut >= 210)
			return XMODEM_COMMUNICATION_FAIL;
	}

	if (bFileDone)
	{
		char eot[3] = { (char)XMODEM_EOT, (char)XMODEM_EOT, (char)XMODEM_EOT };
		if (!cmd.WriteBuffer(eot, 3))
			return XMODEM_COMMUNICATION_FAIL;
		int nEotTo = 0;
		for (;;)
		{
			Sleep(50);
			ZeroMemory(byTempBuf, sizeof(byTempBuf));
			DWORD dwr = 0;
			if (cmd.ReadBuffer(byTempBuf, sizeof(byTempBuf) - 1, &dwr) && dwr > 0)
			{
				if ((BYTE)byTempBuf[0] == XMODEM_ACK)
				{
					if (dwr > 1 && strncmp(byTempBuf + 1, "Successful", 10) == 0)
						return XMODEM_DOWNLOAD_SUCCESS;
					if (dwr == 1)
						return XMODEM_DOWNLOAD_SUCCESS;
					return XMODEM_DOWNLOAD_FAIL;
				}
				if ((BYTE)byTempBuf[0] == XMODEM_NAK)
					return XMODEM_DOWNLOAD_FAIL;
			}
			nEotTo++;
			if (nEotTo >= 210)
				return XMODEM_COMMUNICATION_FAIL;
		}
	}
	return XMODEM_DOWNLOAD_SUCCESS;
}

static int HexNibble(int c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return 10 + c - 'A';
	if (c >= 'a' && c <= 'f') return 10 + c - 'a';
	return -1;
}

static int ParseMemHexLine(const char* s, int len, BYTE* out, int needBytes)
{
	if (!s || !out || needBytes < 1)
		return 0;
	std::string hex;
	hex.reserve((size_t)len);
	for (int i = 0; i < len; i++)
	{
		if (std::isxdigit((unsigned char)s[i]))
			hex.push_back(s[i]);
	}
	const size_t needPair = (size_t)needBytes * 2u;
	if (hex.length() < needPair)
		return 0;
	// Use the *last* 32-byte worth of hex: echo/prompt (e.g. address "57344") contributes xdigits that must not be the first 64 of the stream.
	if (hex.length() > needPair)
		hex = hex.substr(hex.length() - needPair, needPair);
	for (int b = 0; b < needBytes; b++)
	{
		const int hi = HexNibble((unsigned char)hex[(size_t)b * 2]);
		const int lo = HexNibble((unsigned char)hex[(size_t)b * 2 + 1]);
		if (hi < 0 || lo < 0)
			return 0;
		out[b] = (BYTE)((hi << 4) | lo);
	}
	return needBytes;
}

static CString M5761x64ChunkHeadHex8(const BYTE* p)
{
	if (!p)
		return _T("?");
	CString s;
	for (int b = 0; b < 8; ++b)
		s.AppendFormat(_T("%02X"), p[b]);
	return s;
}

static unsigned MemCountHexInString(const std::string& a)
{
	unsigned c = 0;
	for (size_t i = 0; i < a.size(); i++)
	{
		if (std::isxdigit((unsigned char)a[i]))
			c++;
	}
	return c;
}

/// Do not end early when <64 xdigits (dwr=19 case): wait until maxWaitMs. When >=64 xdigits, end after idleOkMs. See `Board439fTransTunnel::DrainAscii`.
static BOOL MemDrainHexResponse(
	Z4671Command& cmd, char* out, size_t outCap, DWORD* pOutLen, int needPayloadBytes, DWORD maxWaitMs)
{
	if (!out || outCap < 2 || !pOutLen || needPayloadBytes < 1)
		return FALSE;
	*pOutLen = 0;
	HANDLE h = cmd.GetPortHandle();
	if (h == INVALID_HANDLE_VALUE || h == NULL)
		return FALSE;
	COMMTIMEOUTS saved = {};
	if (!GetCommTimeouts(h, &saved))
		return FALSE;
	{
		COMMTIMEOUTS t = {};
		t.ReadIntervalTimeout = MAXDWORD;
		t.ReadTotalTimeoutMultiplier = 0;
		t.ReadTotalTimeoutConstant = 0;
		t.WriteTotalTimeoutConstant = 5000;
		t.WriteTotalTimeoutMultiplier = 0;
		if (!SetCommTimeouts(h, &t))
		{
			SetCommTimeouts(h, &saved);
			return FALSE;
		}
	}
	const unsigned needHex = (unsigned)needPayloadBytes * 2u;
	const DWORD idleOkMs = (DWORD)M576_1X64_MEM_READ_IDLE_OK_MS;
	std::string acc;
	acc.reserve(256);
	const DWORD t0 = GetTickCount();
	const DWORD maxAcc = 4096u;
	DWORD lastDataTick = t0;
	while (GetTickCount() - t0 < maxWaitMs)
	{
		const DWORD avail = cmd.RxBytesWaiting();
		if (avail > 0)
		{
			char tmp[1024];
			const DWORD want = (avail < (DWORD)sizeof(tmp) - 1) ? avail : (DWORD)(sizeof(tmp) - 1);
			DWORD nread = 0;
			if (cmd.ReadBuffer(tmp, want, &nread) && nread > 0)
			{
				acc.append(tmp, nread);
				lastDataTick = GetTickCount();
				if (acc.length() > (size_t)maxAcc)
					break;
			}
		}
		else if (!acc.empty())
		{
			// If we already have 64+ hex digits, stop after a short RX idle. If not, do NOT end early: a 2nd chunk
			// may arrive >500ms after the first (log: dwr=19) ?? only maxWaitMs caps the wait.
			const DWORD idle = GetTickCount() - lastDataTick;
			if (MemCountHexInString(acc) >= needHex && idle >= idleOkMs)
				break;
		}
		else
		{ /* no data yet, keep until maxWait */ }
		Sleep(1);
	}
	(void)SetCommTimeouts(h, &saved);
	if (acc.empty())
		return FALSE;
	if (acc.length() >= outCap)
		acc.resize(outCap - 1);
	memcpy(out, acc.data(), acc.length());
	out[acc.length()] = 0;
	*pOutLen = (DWORD)acc.length();
	return TRUE;
}

} // namespace

int M5761x64MemReadStepCount()
{
	if (M576_1X64_MEM_ADDR_STEP == 0)
		return 0;
	if (M576_1X64_MEMS_BACKUP_TOTAL_SIZE % M576_1X64_MEM_ADDR_STEP != 0)
		return 0;
	return (int)(M576_1X64_MEMS_BACKUP_TOTAL_SIZE / M576_1X64_MEM_ADDR_STEP);
}

int M5761x64XmodemChunkCountForFileSize(DWORD fileBytes)
{
	DWORD left = fileBytes;
	int c = 0;
	while (left > 0)
	{
		WORD w;
		if (left >= 1024u) w = 1024;
		else if (left >= 128u) w = 128;
		else w = (WORD)left;
		left -= w;
		c++;
	}
	return c;
}

BOOL M576Read1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szOutPath, DWORD flashBase, CString& err, McsFwProgressCb cb, void* user, int progressBase, int progressTotal)
{
	err.Empty();
	{
		const unsigned numBlocks2k = (unsigned)(M576_1X64_MEMS_BACKUP_TOTAL_SIZE / M576_1X64_MEMS_BIN_SIZE);
		cmd.TraceInfo(
			_T("FW-1x64"),
			_T("Read 1x64 (MEM) start: base=0x%08lX total=%u B (%u x %u B blocks) -> file %s"),
			(unsigned long)flashBase,
			(unsigned)M576_1X64_MEMS_BACKUP_TOTAL_SIZE,
			numBlocks2k,
			(unsigned)M576_1X64_MEMS_BIN_SIZE,
			szOutPath);
	}
	const int steps = M5761x64MemReadStepCount();
	if (steps < 1 || (size_t)steps * (size_t)M576_1X64_MEM_ADDR_STEP != (size_t)M576_1X64_MEMS_BACKUP_TOTAL_SIZE)
	{
		err = _T("Invalid 1x64 MEM step/backup size configuration.");
		cmd.TraceError(_T("FW-1x64"), _T("%s"), err.GetString());
		return FALSE;
	}
	const int kStepsPer2k = (int)(M576_1X64_MEMS_BIN_SIZE / M576_1X64_MEM_ADDR_STEP);
	const int kNum2k = (int)(M576_1X64_MEMS_BACKUP_TOTAL_SIZE / M576_1X64_MEMS_BIN_SIZE);
	cmd.TraceInfo(
		_T("FW-1x64"),
		_T("MEM schedule: %d read steps, addr +0x%X per step, %d x 2K block(s) in range."),
		steps,
		(unsigned)M576_1X64_MEM_ADDR_STEP,
		kNum2k);
	std::vector<BYTE> buf;
	buf.resize((size_t)M576_1X64_MEMS_BACKUP_TOTAL_SIZE);
	char readBuf[4096];
	const int needPayload = (int)M576_1X64_MEM_PAYLOAD_BYTES;
	/// 0..2 = MEM line style; locked after first success.
	int memCmdAttempt = -1;
	for (int i = 0; i < steps; i++)
	{
		const DWORD addr = flashBase + (DWORD)i * (DWORD)M576_1X64_MEM_ADDR_STEP;
		if (i % kStepsPer2k == 0)
		{
			const int blockIdx = i / kStepsPer2k;
			cmd.TraceInfo(
				_T("FW-1x64"),
				_T("MEM block %d/%d: from @0x%08X (step %d/%d)"),
				blockIdx + 1,
				kNum2k,
				(unsigned)addr,
				i + 1,
				steps);
		}
		BYTE chunk[32];
		ZeroMemory(chunk, sizeof(chunk));
		int pr = 0;
		DWORD dwr = 0;
		CStringA line;
		int attUsed = -1;
		int attLo = 0, attHi = 2;
		if (i > 0 && memCmdAttempt >= 0)
		{
			attLo = attHi = memCmdAttempt;
		}
		for (int att = attLo; att <= attHi; att++)
		{
			if (att == 0)
				line.Format("MEM %u\r", (unsigned)addr);
			else if (att == 1)
				line.Format("MEM 0x%X\r", (unsigned)addr);
			else
				line.Format("mem 0x%x\r", (unsigned)addr);
			const bool logProbe = (i == 0 && attLo < attHi);
			if (logProbe)
			{
				cmd.TraceInfo(
					_T("FW-1x64"),
					_T("MEM[probe step0] try format %d/3, TX (ASCII) -> %hs"),
					att + 1,
					(LPCSTR)line);
			}
			else
			{
				/// Log every 8th step, first 4 steps, and last (fine-grained + bounded volume).
				const bool logThisStep =
					(i < 4) || (i + 1 == steps) || ((i % 8) == 0) || (i > 0 && (i % kStepsPer2k) == 0);
				if (logThisStep)
				{
					cmd.TraceInfo(
						_T("FW-1x64"),
						_T("MEM step %d/%d @0x%08X TX -> %hs"),
						i + 1,
						steps,
						(unsigned)addr,
						(LPCSTR)line);
				}
			}
			if (!cmd.WriteBuffer((char*)(LPCSTR)line, (DWORD)line.GetLength()))
			{
				err = _T("MEM write failed.");
				cmd.TraceError(_T("FW-1x64"), _T("MEM @0x%08X failed: TX (serial write)"), (unsigned)addr);
				return FALSE;
			}
			Sleep((DWORD)M576_439F_POST_TRANS_MS);
			Sleep((DWORD)M576_1X64_MEM_AFTER_CMD_MS);
			ZeroMemory(readBuf, sizeof(readBuf));
			dwr = 0;
			if (!MemDrainHexResponse(cmd, readBuf, sizeof(readBuf) - 1, &dwr, needPayload, (DWORD)M576_1X64_MEM_READ_MAX_MS) || dwr == 0)
			{
				if (i == 0 && att < attHi)
					cmd.TraceInfo(
						_T("FW-1x64"),
						_T("MEM[probe] format %d: no RX or drain empty; next format..."),
						att + 1);
				if (att < attHi)
					continue;
				if (i > 0)
				{
					err = _T("MEM read failed (empty/timeout).");
					cmd.TraceError(_T("FW-1x64"), _T("MEM @0x%08X no full reply (locked format)"), (unsigned)addr);
					return FALSE;
				}
				err = _T("MEM read failed (empty/timeout) for all MEM line formats (dec, MEM 0x, mem 0x).");
				cmd.TraceError(_T("FW-1x64"), _T("MEM @0x%08X no full reply (all attempts)"), (unsigned)addr);
				return FALSE;
			}
			readBuf[dwr] = 0;
			const std::string rbs(readBuf, (size_t)dwr);
			const unsigned nXd = MemCountHexInString(rbs);
			pr = ParseMemHexLine(readBuf, (int)dwr, chunk, needPayload);
			attUsed = att;
			{
				const bool logRx =
					logProbe || (i < 4) || (i + 1 == steps) || ((i % 8) == 0) || (i > 0 && (i % kStepsPer2k) == 0);
				if (logRx)
				{
					if (pr == needPayload)
					{
						const CString head8 = M5761x64ChunkHeadHex8(chunk);
						cmd.TraceInfo(
							_T("FW-1x64"),
							_T("MEM step %d/%d RX: ascii_len=%lu hex_chars=%u parse32=OK  head8=%s"),
							i + 1,
							steps,
							(unsigned long)dwr,
							(unsigned)nXd,
							head8.GetString());
					}
					else
					{
						cmd.TraceInfo(
							_T("FW-1x64"),
							_T("MEM step %d/%d RX: ascii_len=%lu hex_chars=%u parse32=%d (expect %d)"),
							i + 1,
							steps,
							(unsigned long)dwr,
							(unsigned)nXd,
							pr,
							needPayload);
					}
				}
			}
			if (pr == needPayload)
			{
				if (i == 0)
				{
					memCmdAttempt = att;
					LPCTSTR fmtName = _T("MEM");
					if (att == 0) fmtName = _T("MEM <decimal> <CR>");
					else if (att == 1) fmtName = _T("MEM 0x<hex> <CR>");
					else if (att == 2) fmtName = _T("mem 0x<hex> <CR>");
					cmd.TraceInfo(
						_T("FW-1x64"),
						_T("MEM line style locked: %s (use for next %d steps)."),
						fmtName,
						steps - 1);
				}
				break;
			}
			if (i == 0 && att < attHi)
				cmd.TraceInfo(
					_T("FW-1x64"),
					_T("MEM[probe] format %d: parse not 32B; try next..."),
					att + 1);
		}
		if (pr != needPayload)
		{
			err.Format(
				_T("MEM parse failed at 0x%08X (tried att %d, expected %d payload bytes from hex)."),
				(unsigned)addr,
				attUsed,
				needPayload);
			cmd.TraceError(_T("FW-1x64"), _T("%s"), err.GetString());
			return FALSE;
		}
		memcpy(&buf[(size_t)i * (size_t)M576_1X64_MEM_ADDR_STEP], chunk, (size_t)needPayload);
		if (cb)
			cb(progressBase + i + 1, progressTotal, user);
	}
	HANDLE h = CreateFile(szOutPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Cannot create %s"), szOutPath);
		return FALSE;
	}
	DWORD nWr = 0;
	const BOOL ok = WriteFile(h, buf.data(), (DWORD)M576_1X64_MEMS_BACKUP_TOTAL_SIZE, &nWr, NULL) && nWr == (DWORD)M576_1X64_MEMS_BACKUP_TOTAL_SIZE;
	CloseHandle(h);
	if (!ok)
	{
		err = _T("Write 1x64 bin failed.");
		return FALSE;
	}
	cmd.TraceInfo(
		_T("FW-1x64"),
		_T("Read 1x64 (MEM) done: wrote %u B to %s"),
		(unsigned)M576_1X64_MEMS_BACKUP_TOTAL_SIZE,
		szOutPath);
	return TRUE;
}

BOOL M576Upload1x64MemsBinOnCurrentTunnel(
	Z4671Command& cmd, LPCTSTR szBinPath, CString& err, McsFwProgressCb cb, void* user, int progressBase, int progressTotal)
{
	err.Empty();
	cmd.TraceInfo(_T("FW-1x64"), _T("XMODEM burn (fwdl): %s"), szBinPath);
	const char* fwdl = "fwdl\r";
	if (!cmd.WriteBuffer((char*)fwdl, (DWORD)strlen(fwdl)))
	{
		err = _T("fwdl write failed.");
		return FALSE;
	}
	Sleep((DWORD)M576_1X64_FWDL_PRE_MS);
	HANDLE hBin = CreateFile(szBinPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hBin == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Open 1x64 bin failed: %s"), szBinPath);
		return FALSE;
	}
	const DWORD dwFile = GetFileSize(hBin, NULL);
	if (dwFile == INVALID_FILE_SIZE)
	{
		CloseHandle(hBin);
		err = _T("1x64 bin: GetFileSize failed.");
		return FALSE;
	}
	if (dwFile < (DWORD)M576_1X64_MEMS_BIN_SIZE)
	{
		CloseHandle(hBin);
		err.Format(
			_T("1x64 XMODEM needs at least %u B MemsSw (0x%04X); file has %lu B."),
			(unsigned)M576_1X64_MEMS_BIN_SIZE, (unsigned)M576_1X64_MEMS_BIN_SIZE, (unsigned long)dwFile);
		cmd.TraceError(_T("FW-1x64"), _T("%s"), err.GetString());
		return FALSE;
	}
	if (dwFile > (DWORD)M576_1X64_MEMS_BIN_SIZE)
		cmd.TraceInfo(
			_T("FW-1x64"), _T("Full backup is %lu B; XMODEM burns first %u B (MemsSw 2K) only."),
			(unsigned long)dwFile, (unsigned)M576_1X64_MEMS_BIN_SIZE);
	DWORD dwCodeSizeLeft = (DWORD)M576_1X64_MEMS_BIN_SIZE;
	std::vector<BYTE> blockbuf((size_t)(XMODEM_BLOCK_BODY_SIZE_1K + 32u));
	BYTE* pbBinData = blockbuf.data();
	int iCount = 0;
	BYTE bPacketIndex = 1;
	BOOL bFunctionOK = TRUE;

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
			err = _T("1x64 XMODEM: read from bin file failed.");
			bFunctionOK = FALSE;
			break;
		}
		WORD wCrcSize = wBody;
		if (wBody < (WORD)XMODEM_BLOCK_BODY_SIZE_128)
			wCrcSize = (WORD)XMODEM_BLOCK_BODY_SIZE_128;
		WORD wCRC = XmodemCRC16Bytes(pbBinData + XMODEM_BLOCK_DATA_INDEX, wCrcSize);
		if (wCrcSize == (WORD)XMODEM_BLOCK_BODY_SIZE_1K)
			pbBinData[XMODEM_BLOCK_HEAD_INDEX] = (BYTE)XMODEM_STX;
		else
			pbBinData[XMODEM_BLOCK_HEAD_INDEX] = (BYTE)XMODEM_SOH;
		pbBinData[XMODEM_BLOCK_NO_INDEX] = bPacketIndex;
		pbBinData[XMODEM_BLOCK_NON_INDEX] = (BYTE)(0xFFu - (unsigned)bPacketIndex);
		pbBinData[XMODEM_BLOCK_DATA_INDEX + wCrcSize] = (BYTE)(wCRC >> 8);
		pbBinData[XMODEM_BLOCK_DATA_INDEX + wCrcSize + 1] = (BYTE)wCRC;
		const WORD wWire = (WORD)(wCrcSize + 5);
		const int xr = XmodemSendOneBlock(cmd, pbBinData, wWire, bFileDone);
		if (xr != XMODEM_DOWNLOAD_SUCCESS)
		{
			err = _T("1x64 XMODEM: block NAK/timeout or EOT failed.");
			bFunctionOK = FALSE;
			break;
		}
		if (cb)
			cb(progressBase + iCount, progressTotal, user);
		bPacketIndex++;
	}
	CloseHandle(hBin);
	if (!bFunctionOK)
	{
		if (err.IsEmpty())
			err = _T("1x64 XMODEM failed.");
		cmd.TraceError(_T("FW-1x64"), _T("%s"), err.GetString());
		return FALSE;
	}
	Sleep(4000);
	{
		const char* rset = "RSET\r";
		(void)cmd.WriteBuffer((char*)rset, (DWORD)strlen(rset));
	}
	cmd.TraceInfo(_T("FW-1x64"), _T("1x64 burn done (RSET sent): %s"), szBinPath);
	return TRUE;
}
