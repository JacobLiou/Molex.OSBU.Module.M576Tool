#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576FimIlTestDlg.h"
#include "M576CalibratorDlg.h"
#include "DiagnosisSession.h"
#include "IlTestCsv.h"
#include "CalibConstants.h"
#include "McsFwTransport.h"
#include "M576TempMonitor.h"
#include "resource.h"

#include <commctrl.h>
#include <io.h>
#include <stdio.h>
#include <vector>

// =============================================================================
// FIM IL protocol (ported from SOFTWARE3312_FIM_Reliability) ?? keep in this cpp.
// =============================================================================
namespace
{
constexpr UINT WM_M576_FIM_IL_LOG = WM_APP + 220;
constexpr UINT WM_M576_FIM_IL_ROW_FLUSH = WM_APP + 221;
constexpr UINT WM_M576_FIM_IL_STATUS = WM_APP + 222;
constexpr UINT WM_M576_FIM_IL_FINISHED = WM_APP + 223;
constexpr UINT WM_M576_FIM_IL_TEMPS = WM_APP + 224;

constexpr int kFimChannelCount = 576;

constexpr int MAX_CMD = 256;
constexpr BYTE XMODEM_SOH = 0x01;
constexpr BYTE XMODEM_STX = 0x02;
constexpr BYTE XMODEM_EOT = 0x04;
constexpr BYTE XMODEM_ACK = 0x06;
constexpr BYTE XMODEM_NAK = 0x15;
constexpr BYTE XMODEM_CAN = 0x18;
constexpr BYTE XMODEM_CRCCHR = 'C';
constexpr int XMODEM_BLOCK_HEAD_INDEX = 0;
constexpr int XMODEM_BLOCK_NO_INDEX = 1;
constexpr int XMODEM_BLOCK_NON_INDEX = 2;
constexpr int XMODEM_BLOCK_DATA_INDEX = 3;
constexpr WORD XMODEM_BLOCK_BODY_SIZE_1K = 1024;
constexpr BYTE XMODEM_COMMUNICATION_FAIL = 0;
constexpr BYTE XMODEM_DOWNLOAD_FAIL = 1;
constexpr BYTE XMODEM_DOWNLOAD_SUCCESS = 2;
constexpr BYTE XMODEM_USER_STOP = 3;

inline bool FimStopRequested(volatile BOOL* pStop)
{
	return pStop != NULL && *pStop;
}

CStringA FimPrintableRx(const CStringA& raw, int maxChars = 120)
{
	CStringA out;
	const int n = raw.GetLength();
	const int lim = (n > maxChars) ? maxChars : n;
	for (int i = 0; i < lim; ++i)
	{
		const unsigned char c = (unsigned char)raw[i];
		if (c >= 32 && c < 127)
			out += (char)c;
		else
			out.AppendFormat("\\x%02X", (unsigned)c);
	}
	if (n > lim)
		out += "...";
	return out;
}

bool FimBufHasByte(const CStringA& a, BYTE b)
{
	const int n = a.GetLength();
	for (int i = 0; i < n; ++i)
	{
		if ((BYTE)a[i] == b)
			return true;
	}
	return false;
}

bool FimBufHasSuccessful(const CStringA& a)
{
	return a.Find("Successful") >= 0;
}

void FimAppendRxCap(CStringA& accum, const BYTE* p, DWORD n, int maxKeep = 480)
{
	if (p == NULL || n == 0)
		return;
	accum.Append((const char*)p, (int)n);
	if (accum.GetLength() > maxKeep)
		accum = accum.Right(maxKeep);
}

constexpr DWORD kPolyCrc32 = 0xEDB88320L;

BOOL ParseDoubleEdit(CWnd* dlg, int id, double& out)
{
	if (!dlg)
		return FALSE;
	CString s;
	dlg->GetDlgItemText(id, s);
	s.Trim();
	if (s.IsEmpty())
		return FALSE;
	out = _ttof(s);
	return TRUE;
}

class FimCrc32
{
public:
	FimCrc32()
	{
		for (LONG i = 0; i < 256; ++i)
		{
			DWORD e = (DWORD)i;
			for (int j = 0; j < 8; ++j)
			{
				if (e & 1)
					e = (e >> 1) ^ kPolyCrc32;
				else
					e >>= 1;
			}
			m_table[i] = e;
		}
		Init();
	}

	void Init() { m_crc = 0xFFFFFFFF; }

	DWORD Feed(BYTE b)
	{
		m_crc = ((m_crc >> 8) & 0x00FFFFFF) ^ m_table[(m_crc ^ b) & 0xFF];
		return m_crc;
	}

	DWORD Value() const { return m_crc; }

private:
	DWORD m_crc = 0xFFFFFFFF;
	DWORD m_table[256]{};
};

struct FimTableRow
{
	BYTE b[8]{};
	CString channel;
};

void BuildFimTestTableRows(BYTE port1x8, BYTE sw4Port, std::vector<FimTableRow>& out)
{
	out.clear();
	out.reserve((size_t)kFimChannelCount);
	const BYTE dut = (sw4Port == 2) ? (BYTE)2 : (BYTE)1;
	for (int sw = 1; sw <= 32; ++sw)
	{
		for (int ch = 1; ch <= 18; ++ch)
		{
			FimTableRow r;
			const int pb = sw + 32;
			r.b[0] = port1x8;
			r.b[1] = dut; // 1x2 DUT: 1=IN, 2=OUT
			r.b[2] = (BYTE)sw;
			r.b[3] = (BYTE)pb;
			r.b[4] = (BYTE)sw;
			r.b[5] = (BYTE)pb;
			r.b[6] = (BYTE)ch;
			r.b[7] = (BYTE)ch;
			const int k = (sw - 1) * 18 + ch;
			r.channel.Format(_T("CH%d"), k);
			out.push_back(r);
		}
	}
}

BOOL WriteOplkBinFile(LPCTSTR path, const std::vector<FimTableRow>& rows, CString& err)
{
	err.Empty();
	if (!path || path[0] == 0 || rows.empty())
	{
		err = _T("OPLK: empty path or table.");
		return FALSE;
	}
	const DWORD n = (DWORD)rows.size();
	const DWORD dwLen = n * 8 + 1;
	std::vector<BYTE> payload((size_t)dwLen, 0);
	FimCrc32 crc;
	crc.Init();
	DWORD idx = 0;
	for (DWORD i = 0; i < n; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			payload[(size_t)idx] = rows[(size_t)i].b[j];
			crc.Feed(payload[(size_t)idx]);
			++idx;
		}
	}
	payload[(size_t)idx] = 0xFF;
	crc.Feed(0xFF);
	const DWORD dwCrcVal = ~crc.Value();

	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("wb")) != 0 || fp == NULL)
	{
		err.Format(_T("OPLK: cannot create %s"), path);
		return FALSE;
	}
	static const BYTE kMagic[4] = { 'O', 'P', 'L', 'K' };
	BYTE crcBytes[4] = {
		(BYTE)(0xff & dwCrcVal),
		(BYTE)(0xff & (dwCrcVal >> 8)),
		(BYTE)(0xff & (dwCrcVal >> 16)),
		(BYTE)(0xff & (dwCrcVal >> 24))
	};
	if (fwrite(kMagic, 1, 4, fp) != 4
		|| fwrite(crcBytes, 1, 4, fp) != 4
		|| fwrite(payload.data(), 1, (size_t)dwLen, fp) != (size_t)dwLen)
	{
		err = _T("OPLK: write failed.");
		fclose(fp);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}

WORD XmodemCrc16(const BYTE* pb, WORD wLength)
{
	WORD wCRCValue = 0;
	while (wLength > 0)
	{
		wCRCValue = (WORD)(wCRCValue ^ ((WORD)(*pb++) << 8));
		for (WORD wIndex = 0; wIndex < 8; ++wIndex)
		{
			if (wCRCValue & 0x8000)
				wCRCValue = (WORD)((wCRCValue << 1) ^ 0x1021);
			else
				wCRCValue = (WORD)(wCRCValue << 1);
		}
		--wLength;
	}
	return wCRCValue;
}

BOOL StartFwdl0Xmodem(COpComm& comm, CString& err, volatile BOOL* pStop)
{
	err.Empty();
	// Drain leftover ASCII (pd/SW reply) so it is not mistaken for Xmodem traffic.
	{
		BYTE drain[256]{};
		DWORD dn = 0;
		(void)comm.ReadBuffer(drain, sizeof(drain), &dn);
	}
	::Sleep(100);

	BYTE cmd[8] = { 'f', 'w', 'd', 'l', ' ', '0', 0x0D, 0 };
	// Purge-on-write is intentional here: enter a clean wait for 'C'.
	if (!comm.WriteBuffer(cmd, 7))
	{
		err = _T("fwdl 0: write failed.");
		return FALSE;
	}
	int nTimeOut = 0;
	CStringA rxAcc;
	BYTE rx[64]{};
	while (nTimeOut < 210)
	{
		if (FimStopRequested(pStop))
		{
			err = _T("fwdl 0: stopped by user.");
			return FALSE;
		}
		::Sleep(50);
		DWORD n = 0;
		if (comm.ReadBuffer(rx, sizeof(rx), &n) && n > 0)
		{
			FimAppendRxCap(rxAcc, rx, n, 200);
			for (DWORD i = 0; i < n; ++i)
			{
				if (rx[i] == XMODEM_CRCCHR)
					return TRUE;
			}
		}
		++nTimeOut;
	}
	err.Format(_T("fwdl 0: wait Xmodem 'C' timeout (>10s). RX=%hs"),
		FimPrintableRx(rxAcc).GetString());
	return FALSE;
}

/// EOT after last data block. Prefer ACK+"Successful"; if ACK seen and banner never
/// arrives (common for test-table fwdl 0), accept ACK-only after a short grace.
/// Never fail on the first bare ACK. Uses WriteBufferNoPurge so a late ACK is not purged.
BYTE XmodemFinishEot(COpComm& comm, volatile BOOL* pStop, CStringA& rxAccum, bool& outGotBanner)
{
	outGotBanner = false;
	BYTE eot[3] = { XMODEM_EOT, XMODEM_EOT, XMODEM_EOT };
	int nakResends = 0;
	bool sawAck = false;
	constexpr int kAckBannerGraceLoops = 40; // ~2s after first ACK for late banner

	for (int eotPass = 0; eotPass < 16; ++eotPass)
	{
		if (FimStopRequested(pStop))
			return XMODEM_USER_STOP;

		if (!comm.WriteBufferNoPurge(eot, 3))
			return XMODEM_COMMUNICATION_FAIL;

		int ackGrace = 0;
		bool resendEot = false;
		for (int nEotTo = 0; nEotTo < 210; ++nEotTo)
		{
			if (FimStopRequested(pStop))
				return XMODEM_USER_STOP;
			::Sleep(50);

			BYTE byTempBuf[MAX_CMD]{};
			DWORD dwr = 0;
			if (comm.ReadBuffer(byTempBuf, MAX_CMD - 1, &dwr) && dwr > 0)
			{
				byTempBuf[dwr] = 0;
				FimAppendRxCap(rxAccum, byTempBuf, dwr);

				if (FimBufHasSuccessful(rxAccum))
				{
					outGotBanner = true;
					return XMODEM_DOWNLOAD_SUCCESS;
				}

				const CStringA chunk((const char*)byTempBuf, (int)dwr);
				const BYTE firstB = byTempBuf[0];
				const BYTE lastB = byTempBuf[dwr - 1];
				if (firstB == XMODEM_CAN || lastB == XMODEM_CAN || FimBufHasByte(chunk, XMODEM_CAN))
					return XMODEM_COMMUNICATION_FAIL;

				if (firstB == XMODEM_ACK || lastB == XMODEM_ACK || FimBufHasByte(chunk, XMODEM_ACK))
					sawAck = true;

				if (firstB == XMODEM_NAK || lastB == XMODEM_NAK)
				{
					++nakResends;
					if (nakResends > 32)
						return XMODEM_DOWNLOAD_FAIL;
					resendEot = true;
					break;
				}
			}

			if (sawAck)
			{
				++ackGrace;
				if (ackGrace >= kAckBannerGraceLoops)
					return XMODEM_DOWNLOAD_SUCCESS; // ACK-only OK for test table
			}
		}

		if (resendEot)
			continue;
		if (sawAck)
			return XMODEM_DOWNLOAD_SUCCESS;
	}

	if (sawAck)
		return XMODEM_DOWNLOAD_SUCCESS;
	return XMODEM_COMMUNICATION_FAIL;
}

BYTE XmodemSendBlock(COpComm& comm, BYTE* pbBinData, WORD wWireLen, BOOL bFileDone,
	volatile BOOL* pStop, CStringA* pEotRx, bool* pEotBanner)
{
	if (!comm.WriteBuffer(pbBinData, wWireLen))
		return XMODEM_COMMUNICATION_FAIL;

	int nRetry = 0;
	int nTimeOut = 0;
	while (nTimeOut < 210)
	{
		if (FimStopRequested(pStop))
			return XMODEM_USER_STOP;
		::Sleep(50);
		BYTE byTempBuf[MAX_CMD]{};
		DWORD dwReadLength = 0;
		if (comm.ReadBuffer(byTempBuf, MAX_CMD, &dwReadLength) && dwReadLength > 0)
		{
			const BYTE last = byTempBuf[dwReadLength - 1];
			const BYTE first = byTempBuf[0];
			if (last == XMODEM_ACK || first == XMODEM_ACK)
				break;
			if (last == XMODEM_NAK || first == XMODEM_NAK)
			{
				if (!comm.WriteBuffer(pbBinData, wWireLen))
					return XMODEM_COMMUNICATION_FAIL;
				++nRetry;
				if (nRetry > 10)
				{
					BYTE can[3] = { XMODEM_CAN, XMODEM_CAN, XMODEM_CAN };
					(void)comm.WriteBuffer(can, 3);
					return XMODEM_COMMUNICATION_FAIL;
				}
				nTimeOut = 0;
				continue;
			}
			if (last == XMODEM_CAN || first == XMODEM_CAN)
				return XMODEM_COMMUNICATION_FAIL;
			// Ignore echo/noise; keep waiting for ACK/NAK/CAN.
		}
		++nTimeOut;
	}
	if (nTimeOut >= 210)
		return XMODEM_COMMUNICATION_FAIL;

	if (!bFileDone)
		return XMODEM_DOWNLOAD_SUCCESS;

	// Brief settle so last-block ACK is not confused with EOT reply.
	::Sleep(50);
	CStringA eotRx;
	bool gotBanner = false;
	const BYTE st = XmodemFinishEot(comm, pStop, eotRx, gotBanner);
	if (pEotRx)
		*pEotRx = eotRx;
	if (pEotBanner)
		*pEotBanner = gotBanner;
	return st;
}

BOOL DownloadTestBinXmodem(COpComm& comm, LPCTSTR binPath, CString& err, volatile BOOL* pStop,
	bool* pEotBanner)
{
	err.Empty();
	if (pEotBanner)
		*pEotBanner = false;
	if (!StartFwdl0Xmodem(comm, err, pStop))
		return FALSE;

	HANDLE hBin = CreateFile(binPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hBin == INVALID_HANDLE_VALUE)
	{
		err.Format(_T("Xmodem: open bin failed: %s"), binPath);
		return FALSE;
	}
	DWORD dwLeft = GetFileSize(hBin, NULL);
	if (dwLeft == 0 || dwLeft == INVALID_FILE_SIZE)
	{
		CloseHandle(hBin);
		err = _T("Xmodem: bin empty.");
		return FALSE;
	}

	BYTE pbBinData[XMODEM_BLOCK_BODY_SIZE_1K + 32]{};
	BYTE bPacketIndex = 1;
	BOOL ok = FALSE;
	CStringA eotRx;
	bool eotBanner = false;
	while (dwLeft > 0)
	{
		if (FimStopRequested(pStop))
		{
			CloseHandle(hBin);
			err = _T("Xmodem: stopped by user.");
			return FALSE;
		}
		ZeroMemory(pbBinData, sizeof(pbBinData));
		WORD wDownloadSize = (dwLeft >= XMODEM_BLOCK_BODY_SIZE_1K)
			? XMODEM_BLOCK_BODY_SIZE_1K
			: (WORD)dwLeft;
		dwLeft -= wDownloadSize;
		const BOOL bFileDone = (dwLeft == 0);

		DWORD dwBytesRead = 0;
		if (!ReadFile(hBin, pbBinData + XMODEM_BLOCK_DATA_INDEX, wDownloadSize, &dwBytesRead, NULL)
			|| dwBytesRead != wDownloadSize)
		{
			CloseHandle(hBin);
			err = _T("Xmodem: read bin failed.");
			return FALSE;
		}

		const WORD wWireBody = XMODEM_BLOCK_BODY_SIZE_1K;
		const WORD wCRC16 = XmodemCrc16(pbBinData + XMODEM_BLOCK_DATA_INDEX, wWireBody);
		pbBinData[XMODEM_BLOCK_HEAD_INDEX] = XMODEM_STX;
		pbBinData[XMODEM_BLOCK_NO_INDEX] = bPacketIndex;
		pbBinData[XMODEM_BLOCK_NON_INDEX] = (BYTE)(0xFF - bPacketIndex);
		pbBinData[XMODEM_BLOCK_DATA_INDEX + wWireBody] = (BYTE)(wCRC16 >> 8);
		pbBinData[XMODEM_BLOCK_DATA_INDEX + wWireBody + 1] = (BYTE)wCRC16;

		CStringA eotRxThis;
		bool bannerThis = false;
		const BYTE st = XmodemSendBlock(comm, pbBinData, (WORD)(wWireBody + 5), bFileDone, pStop,
			bFileDone ? &eotRxThis : NULL, bFileDone ? &bannerThis : NULL);
		if (bFileDone)
		{
			eotRx = eotRxThis;
			eotBanner = bannerThis;
		}
		if (st == XMODEM_USER_STOP)
		{
			CloseHandle(hBin);
			err = _T("Xmodem: stopped by user.");
			return FALSE;
		}
		if (st == XMODEM_DOWNLOAD_FAIL)
		{
			CloseHandle(hBin);
			err.Format(_T("Xmodem EOT failed (NAK/reject, block %u). RX=%hs"),
				(unsigned)bPacketIndex, FimPrintableRx(eotRx).GetString());
			return FALSE;
		}
		if (st != XMODEM_DOWNLOAD_SUCCESS)
		{
			CloseHandle(hBin);
			err.Format(_T("Xmodem: block %u failed (code=%u). EOT_RX=%hs"),
				(unsigned)bPacketIndex, (unsigned)st, FimPrintableRx(eotRx).GetString());
			return FALSE;
		}
		++bPacketIndex;
		ok = TRUE;
	}
	CloseHandle(hBin);
	if (!ok)
	{
		err = _T("Xmodem: no blocks sent.");
		return FALSE;
	}
	if (pEotBanner)
		*pEotBanner = eotBanner;
	return TRUE;
}

/// FIM CheckScanFinal + wall-clock timeout + cooperative stop. logFn may be null.
BOOL WaitScanOk(COpComm& comm, CString& err, volatile BOOL* pStop,
	void (*logFn)(void*, const CString&), void* logCtx)
{
	err.Empty();
	CStringA tot;
	int iRunLoop = 0;
	const int kMaxLoops = 1000; // ~200s like FIM
	do
	{
		if (FimStopRequested(pStop))
		{
			err.Format(_T("CheckScanFinal: stopped by user. RX=%hs"),
				FimPrintableRx(tot).GetString());
			return FALSE;
		}
		::Sleep(200);
		BYTE bRxData[32]{};
		DWORD n = 0;
		if (comm.ReadBuffer(bRxData, sizeof(bRxData), &n) && n > 0)
		{
			for (DWORD i = 0; i < n; ++i)
				tot += (char)bRxData[i];
			// FIM: "OK" or 'O' and 'K' seen across packets.
			if (tot.Find("OK") >= 0
				|| (tot.Find('O') >= 0 && tot.Find('K') >= 0))
				return TRUE;
		}
		++iRunLoop; // every iteration (not only empty read)
		if (logFn && (iRunLoop % 50) == 0)
		{
			CString prog;
			prog.Format(_T("[FIMIL] WaitScan loop=%d/%d RX=%hs"),
				iRunLoop, kMaxLoops, FimPrintableRx(tot).GetString());
			logFn(logCtx, prog);
		}
	} while (iRunLoop < kMaxLoops);

	err.Format(_T("CheckScanFinal: wait OK timeout. RX=%hs"),
		FimPrintableRx(tot).GetString());
	return FALSE;
}

BOOL ParseResultPayload(const CStringA& strResult, DWORD channelCount,
	std::vector<double>& opmDbm, std::vector<double>& pdDbm, int& nOpm, int& nPd)
{
	opmDbm.assign((size_t)channelCount, 0.0);
	pdDbm.assign((size_t)channelCount, 0.0);
	nOpm = 0;
	nPd = 0;
	const int expectTok = (int)channelCount * 2;
	int start = 0;
	int tokIndex = 0;
	while (tokIndex < expectTok)
	{
		const int comma = strResult.Find(',', start);
		CStringA tok;
		if (comma < 0)
		{
			tok = strResult.Mid(start);
			tok.Trim();
			if (tok.IsEmpty() && tokIndex == 0)
				return FALSE;
		}
		else
		{
			tok = strResult.Mid(start, comma - start);
			tok.Trim();
			start = comma + 1;
		}
		const double v = atof(tok.GetString());
		if (tokIndex % 2 == 0)
		{
			if (nOpm < (int)channelCount)
				opmDbm[(size_t)nOpm++] = v / 10000.0;
		}
		else
		{
			if (nPd < (int)channelCount)
				pdDbm[(size_t)nPd++] = v / 100.0;
		}
		++tokIndex;
		if (comma < 0)
			break;
	}
	return (nOpm == (int)channelCount && nPd == (int)channelCount);
}

BOOL ReadResultIl(COpComm& comm, DWORD channelCount,
	std::vector<double>& opmDbm, std::vector<double>& pdDbm, CString& err, volatile BOOL* pStop)
{
	err.Empty();
	opmDbm.assign((size_t)channelCount, 0.0);
	pdDbm.assign((size_t)channelCount, 0.0);

	if (FimStopRequested(pStop))
	{
		err = _T("RESULT: stopped by user.");
		return FALSE;
	}

	BYTE tx[8] = { 'R', 'E', 'S', 'U', 'L', 'T', 0x0D, 0 };
	if (!comm.WriteBuffer(tx, 7))
	{
		err = _T("RESULT: write failed.");
		return FALSE;
	}
	::Sleep(350);
	if (FimStopRequested(pStop))
	{
		err = _T("RESULT: stopped by user.");
		return FALSE;
	}

	const DWORD dwNeed = channelCount * 32;
	CStringA strResult;
	const int kMaxReadRounds = 8;
	for (int round = 0; round < kMaxReadRounds; ++round)
	{
		if (FimStopRequested(pStop))
		{
			err = _T("RESULT: stopped by user.");
			return FALSE;
		}
		std::vector<BYTE> rx((size_t)dwNeed + 64, 0);
		DWORD nRead = 0;
		if (comm.ReadBuffer(rx.data(), dwNeed, &nRead) && nRead > 0)
		{
			if (round == 0 && nRead >= 2 && rx[0] == 'F' && rx[1] == 'A')
			{
				err = _T("RESULT: FAIL response.");
				return FALSE;
			}
			strResult += CStringA((const char*)rx.data(), (int)nRead);
			int nOpm = 0;
			int nPd = 0;
			if (ParseResultPayload(strResult, channelCount, opmDbm, pdDbm, nOpm, nPd))
				return TRUE;
		}
		::Sleep(100);
	}

	int nOpm = 0;
	int nPd = 0;
	(void)ParseResultPayload(strResult, channelCount, opmDbm, pdDbm, nOpm, nPd);
	err.Format(_T("RESULT: token count mismatch (opm=%d pd=%d expect=%u) bytes=%d."),
		nOpm, nPd, (unsigned)channelCount, strResult.GetLength());
	return FALSE;
}

BOOL AppendFimIlCsvRow(LPCTSTR path, int lap, LPCTSTR half, LPCTSTR channel, LPCTSTR wl,
	double opm, double pd, double il, double mx, double mn, double span, BOOL pass, CString& err)
{
	err.Empty();
	if (!path || path[0] == 0)
	{
		err = _T("FIM CSV: empty path.");
		return FALSE;
	}
	{
		const CString full(path);
		const int slash = full.ReverseFind(_T('\\'));
		if (slash > 0)
		{
			const CString dir = full.Left(slash);
			CreateDirectory(dir, NULL);
		}
	}
	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("ab")) != 0 || fp == NULL)
	{
		err.Format(_T("FIM CSV: cannot open %s"), path);
		return FALSE;
	}
	const int fh = _fileno(fp);
	const long len = (fh >= 0) ? _filelength(fh) : 0;
	if (len <= 0)
	{
		static const unsigned char kBom[] = { 0xEF, 0xBB, 0xBF };
		fwrite(kBom, 1, sizeof(kBom), fp);
		static const char kHdr[] =
			"Lap,Half,Channel,WlLabel,OPM_dBm,PD_dBm,IL,IL_Max,IL_Min,IL_Span,Pass\r\n";
		fwrite(kHdr, 1, sizeof(kHdr) - 1, fp);
	}
	CString lineW;
	lineW.Format(_T("%d,%s,%s,%s,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%s\r\n"),
		lap, half, channel, wl, opm, pd, il, mx, mn, span, pass ? _T("PASS") : _T("FAIL"));
	const CStringA line(lineW);
	fwrite(line.GetString(), 1, (size_t)line.GetLength(), fp);
	fflush(fp);
	fclose(fp);
	return TRUE;
}

/// stats sized 2*N: [0..N) = IN, [N..2N) = OUT.
BOOL WriteFimSpanCsv(LPCTSTR path, const std::vector<FimTableRow>& table,
	const std::vector<IlTestRollingStats>& stats, LPCTSTR wl, CString& err)
{
	err.Empty();
	FILE* fp = NULL;
	if (_tfopen_s(&fp, path, _T("wb")) != 0 || fp == NULL)
	{
		err.Format(_T("FIM Span CSV: cannot open %s"), path);
		return FALSE;
	}
	static const unsigned char kBom[] = { 0xEF, 0xBB, 0xBF };
	fwrite(kBom, 1, sizeof(kBom), fp);
	static const char kHdr[] =
		"Channel,Half,InPort,OutPort,WlLabel,SampleCount,IL_Max,IL_Min,IL_Span\r\n";
	fwrite(kHdr, 1, sizeof(kHdr) - 1, fp);
	const size_t N = table.size();
	for (int half = 1; half <= 2; ++half)
	{
		const LPCTSTR halfLabel = IlTestHalfLabel(half);
		const size_t base = (size_t)(half - 1) * N;
		for (size_t i = 0; i < N; ++i)
		{
			const size_t si = base + i;
			if (si >= stats.size() || stats[si].sampleCount <= 0)
				continue;
			CString inPort, outPort;
			const int chIdx = IlTestParseChannelIndex(table[i].channel);
			if (!IlTestChannelToMpoPorts(chIdx, inPort, outPort))
			{
				inPort = _T("-");
				outPort = _T("-");
			}
			CString lineW;
			lineW.Format(_T("%s,%s,%s,%s,%s,%d,%.6f,%.6f,%.6f\r\n"),
				table[i].channel.GetString(), halfLabel, inPort.GetString(), outPort.GetString(), wl,
				stats[si].sampleCount, stats[si].ilMax, stats[si].ilMin, stats[si].Span());
			const CStringA line(lineW);
			fwrite(line.GetString(), 1, (size_t)line.GetLength(), fp);
		}
	}
	fflush(fp);
	fclose(fp);
	return TRUE;
}
} // namespace

CM576FimIlTestDlg::CM576FimIlTestDlg(CM576CalibratorDlg* pOwner, CWnd* pParent)
	: CDialogEx(IDD_M576_FIM_IL_TEST, pParent ? pParent : pOwner)
	, m_pOwner(pOwner)
{
}

CM576FimIlTestDlg::~CM576FimIlTestDlg()
{
	m_stop = TRUE;
	if (m_worker.joinable())
		m_worker.join();
}

void CM576FimIlTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIM_IL_LIST, m_list);
	DDX_Control(pDX, IDC_FIM_IL_EDIT_LOG, m_editLog);
}

BEGIN_MESSAGE_MAP(CM576FimIlTestDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FIM_IL_BTN_START, &CM576FimIlTestDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_FIM_IL_BTN_STOP, &CM576FimIlTestDlg::OnBnClickedStop)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FIM_IL_LIST, &CM576FimIlTestDlg::OnGetDispInfo)
	ON_MESSAGE(WM_M576_FIM_IL_LOG, &CM576FimIlTestDlg::OnUiLog)
	ON_MESSAGE(WM_M576_FIM_IL_ROW_FLUSH, &CM576FimIlTestDlg::OnUiRowFlush)
	ON_MESSAGE(WM_M576_FIM_IL_STATUS, &CM576FimIlTestDlg::OnUiStatus)
	ON_MESSAGE(WM_M576_FIM_IL_TEMPS, &CM576FimIlTestDlg::OnUiTemps)
	ON_MESSAGE(WM_M576_FIM_IL_FINISHED, &CM576FimIlTestDlg::OnUiFinished)
END_MESSAGE_MAP()

BOOL CM576FimIlTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CheckRadioButton(IDC_FIM_IL_RADIO_SFP1550, IDC_FIM_IL_RADIO_LASER1310, IDC_FIM_IL_RADIO_LASER1310);
	CString thr;
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_ABS_MIN_DB);
	SetDlgItemText(IDC_FIM_IL_EDIT_ABS_MIN, thr);
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_ABS_MAX_DB);
	SetDlgItemText(IDC_FIM_IL_EDIT_ABS_MAX, thr);
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_SPAN_MAX_DB);
	SetDlgItemText(IDC_FIM_IL_EDIT_SPAN_MAX, thr);
	SetDlgItemText(IDC_FIM_IL_STATIC_STATUS,
		_T("Idle - FIM BIN scan; gate same as IL Test: IL abs + Span<= (editable)."));

	m_list.ModifyStyle(0, LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_list.InsertColumn(0, _T("Channel"), LVCFMT_LEFT, 70);
	m_list.InsertColumn(1, _T("Half"), LVCFMT_LEFT, 45);
	m_list.InsertColumn(2, _T("Wl"), LVCFMT_LEFT, 75);
	m_list.InsertColumn(3, _T("Lap"), LVCFMT_RIGHT, 40);
	m_list.InsertColumn(4, _T("PD_dBm"), LVCFMT_RIGHT, 75);
	m_list.InsertColumn(5, _T("OPM_dBm"), LVCFMT_RIGHT, 80);
	m_list.InsertColumn(6, _T("IL"), LVCFMT_RIGHT, 70);
	m_list.InsertColumn(7, _T("Max"), LVCFMT_RIGHT, 70);
	m_list.InsertColumn(8, _T("Min"), LVCFMT_RIGHT, 70);
	m_list.InsertColumn(9, _T("Span"), LVCFMT_RIGHT, 70);
	m_list.InsertColumn(10, _T("Result"), LVCFMT_RIGHT, 65);

	if (CWnd* p = GetDlgItem(IDC_FIM_IL_BTN_STOP))
		p->EnableWindow(FALSE);
	SetDlgItemText(IDC_FIM_IL_STATIC_ELAPSED, _T("00:00:00"));
	return TRUE;
}

namespace
{
constexpr UINT_PTR kFimHangupTimerId = 1;
}

void CM576FimIlTestDlg::UpdateHangupClockText()
{
	ULONGLONG sec = 0;
	if (m_hangupStartTick != 0)
	{
		const ULONGLONG now = GetTickCount64();
		sec = (now >= m_hangupStartTick) ? ((now - m_hangupStartTick) / 1000ull) : 0;
	}
	const unsigned h = (unsigned)(sec / 3600ull);
	const unsigned m = (unsigned)((sec % 3600ull) / 60ull);
	const unsigned s = (unsigned)(sec % 60ull);
	CString t;
	t.Format(_T("%02u:%02u:%02u"), h, m, s);
	SetDlgItemText(IDC_FIM_IL_STATIC_ELAPSED, t);
}

void CM576FimIlTestDlg::StartHangupClock()
{
	StopHangupClock();
	m_hangupStartTick = GetTickCount64();
	UpdateHangupClockText();
	SetTimer(kFimHangupTimerId, 1000, NULL);
	m_hangupTimerOn = true;
}

void CM576FimIlTestDlg::StopHangupClock()
{
	if (m_hangupTimerOn)
	{
		KillTimer(kFimHangupTimerId);
		m_hangupTimerOn = false;
	}
	UpdateHangupClockText();
}

void CM576FimIlTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kFimHangupTimerId)
		UpdateHangupClockText();
	CDialogEx::OnTimer(nIDEvent);
}

IlTestWlKind CM576FimIlTestDlg::SelectedWl() const
{
	if (IsDlgButtonChecked(IDC_FIM_IL_RADIO_SFP1550) == BST_CHECKED)
		return IlTestWlKind::Sfp1550;
	if (IsDlgButtonChecked(IDC_FIM_IL_RADIO_SFP1310) == BST_CHECKED)
		return IlTestWlKind::Sfp1310;
	return IlTestWlKind::Laser1310;
}

double CM576FimIlTestDlg::ReadAbsIlMinDb() const
{
	double v = 0.0;
	if (!ParseDoubleEdit(const_cast<CM576FimIlTestDlg*>(this), IDC_FIM_IL_EDIT_ABS_MIN, v))
		return (double)M576_IL_TEST_DEFAULT_ABS_MIN_DB;
	return v;
}

double CM576FimIlTestDlg::ReadAbsIlMaxDb() const
{
	double v = 0.0;
	if (!ParseDoubleEdit(const_cast<CM576FimIlTestDlg*>(this), IDC_FIM_IL_EDIT_ABS_MAX, v))
		return (double)M576_IL_TEST_DEFAULT_ABS_MAX_DB;
	return v;
}

double CM576FimIlTestDlg::ReadSpanMaxDb() const
{
	double v = 0.0;
	if (!ParseDoubleEdit(const_cast<CM576FimIlTestDlg*>(this), IDC_FIM_IL_EDIT_SPAN_MAX, v))
		return (double)M576_IL_TEST_DEFAULT_SPAN_MAX_DB;
	return v;
}

BOOL CM576FimIlTestDlg::ReadGateParams(IlTestGateParams& out, CString& err) const
{
	err.Empty();
	out.absIlMinDb = ReadAbsIlMinDb();
	out.absIlMaxDb = ReadAbsIlMaxDb();
	out.spanMaxDb = ReadSpanMaxDb();
	if (!(out.absIlMinDb <= out.absIlMaxDb))
	{
		err = _T("IL abs min must be <= abs max.");
		return FALSE;
	}
	if (out.absIlMaxDb > 100.0 || out.absIlMinDb < -50.0)
	{
		err = _T("IL abs window out of range.");
		return FALSE;
	}
	if (out.spanMaxDb < 0.0 || out.spanMaxDb > 100.0)
	{
		err = _T("Span max must be in [0, 100] dB.");
		return FALSE;
	}
	return TRUE;
}

void CM576FimIlTestDlg::SetControlsRunning(BOOL running)
{
	const BOOL idle = !running;
	if (CWnd* p = GetDlgItem(IDC_FIM_IL_RADIO_SFP1550))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_FIM_IL_RADIO_SFP1310))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_FIM_IL_RADIO_LASER1310))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_FIM_IL_EDIT_ABS_MIN))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_FIM_IL_EDIT_ABS_MAX))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_FIM_IL_EDIT_SPAN_MAX))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_FIM_IL_BTN_START))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_FIM_IL_BTN_STOP))
		p->EnableWindow(running);
}

void CM576FimIlTestDlg::AppendLogLine(LPCTSTR line)
{
	if (!line)
		return;
	CString cur;
	m_editLog.GetWindowText(cur);
	if (!cur.IsEmpty())
		cur += _T("\r\n");
	cur += line;

	int lines = 0;
	for (int i = 0; i < cur.GetLength(); ++i)
	{
		if (cur[i] == _T('\n'))
			++lines;
	}
	while (lines > kMaxLogLines)
	{
		const int p = cur.Find(_T('\n'));
		if (p < 0)
			break;
		cur = cur.Mid(p + 1);
		--lines;
	}
	m_editLog.SetWindowText(cur);
	m_editLog.LineScroll(m_editLog.GetLineCount());
}

void CM576FimIlTestDlg::BeginLapUi(int channelCount, int lap)
{
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		m_pendingNewLap = true;
		m_pendingLap = lap;
		m_channelCount = channelCount > 0 ? channelCount : 0;
		m_pendingUiRows.assign((size_t)m_channelCount, M576FimIlUiRow{});
	}
	if (!m_rowFlushScheduled.exchange(true))
	{
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_FIM_IL_ROW_FLUSH, 0, 0);
	}
}

void CM576FimIlTestDlg::QueueLapRows(const std::vector<M576FimIlUiRow>& rows)
{
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		if ((int)rows.size() != m_channelCount && m_channelCount > 0)
			m_pendingUiRows.assign((size_t)m_channelCount, M576FimIlUiRow{});
		if (m_pendingUiRows.size() != rows.size())
			m_pendingUiRows = rows;
		else
		{
			for (size_t i = 0; i < rows.size(); ++i)
				m_pendingUiRows[i] = rows[i];
		}
		if (m_channelCount <= 0)
			m_channelCount = (int)rows.size();
	}
	if (!m_rowFlushScheduled.exchange(true))
	{
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_FIM_IL_ROW_FLUSH, 0, 0);
	}
}

void CM576FimIlTestDlg::FlushPendingUiRows()
{
	bool newLap = false;
	int lap = 0;
	int n = 0;
	std::vector<M576FimIlUiRow> batch;
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		newLap = m_pendingNewLap;
		m_pendingNewLap = false;
		lap = m_pendingLap;
		n = m_channelCount;
		batch = m_pendingUiRows; // copy ? keep pending for coalesced updates (same as IL Test)
		m_rowFlushScheduled = false;
	}
	if (n <= 0 && batch.empty())
		return;

	// Always sync virtual-list count when size changes (do not use LVSICF_NOINVALIDATEALL).
	if (newLap || (int)m_uiRows.size() != n || m_list.GetItemCount() != n)
	{
		m_uiLap = lap;
		if ((int)m_uiRows.size() != n)
			m_uiRows.assign((size_t)n, M576FimIlUiRow{});
		m_list.SetItemCountEx(n > 0 ? n : (int)batch.size(), 0);
		if (n <= 0)
			n = (int)batch.size();
	}

	const int count = (std::min)(n, (int)batch.size());
	bool anyFilled = false;
	for (int i = 0; i < count; ++i)
	{
		if (!batch[(size_t)i].channel.IsEmpty() || batch[(size_t)i].lap > 0)
		{
			m_uiRows[(size_t)i] = batch[(size_t)i];
			anyFilled = true;
		}
	}

	m_list.Invalidate(FALSE);
	m_list.UpdateWindow();
	if (anyFilled && n > 0)
		m_list.EnsureVisible(n - 1, FALSE);

	// If worker queued more while we painted, schedule another flush.
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		const bool needAgain = m_pendingNewLap;
		if (needAgain && !m_rowFlushScheduled.exchange(true))
		{
			if (m_hWnd && ::IsWindow(m_hWnd))
				::PostMessage(m_hWnd, WM_M576_FIM_IL_ROW_FLUSH, 0, 0);
		}
	}
}

void CM576FimIlTestDlg::OnBnClickedStart()
{
	if (m_running.load())
		return;
	if (!m_pOwner)
	{
		MessageBox(_T("No owner dialog."), _T("FIM IL"), MB_OK | MB_ICONERROR);
		return;
	}
	IlTestGateParams gate;
	CString gateErr;
	if (!ReadGateParams(gate, gateErr))
	{
		MessageBox(gateErr.IsEmpty() ? _T("Invalid IL/Span gate.") : gateErr,
			_T("FIM IL"), MB_OK | MB_ICONWARNING);
		return;
	}

	CString outDir;
	CString err;
	if (!m_pOwner->BeginIlTestSession(outDir, err))
	{
		MessageBox(err.IsEmpty() ? _T("Cannot start FIM IL session.") : err,
			_T("FIM IL"), MB_OK | MB_ICONWARNING);
		return;
	}

	const IlTestWlKind wl = SelectedWl();
	const CString mcs1Sn = m_pOwner->GetMcs1SnSanitizedForFilename();
	AppendLogLine(_T("[FIMIL] start BIN+RESULT hang-up (no diagnosis_sw.csv, no ref zero)"));
	{
		CString s;
		s.Format(_T("[FIMIL] %s SWL %d %d SW4 alternate IN/OUT (odd/even lap) OPM4 AUTO IL=OPM-PD abs=[%.3f,%.3f] Span<=%.3f"),
			IlTestWlLabel(wl), IlTestSwlChannel(wl), IlTestWavelengthNm(wl),
			gate.absIlMinDb, gate.absIlMaxDb, gate.spanMaxDb);
		AppendLogLine(s);
	}
	AppendLogLine(_T("[FIMIL] Half: odd lap=IN (SW4 1 / OPLK b1=1), even lap=OUT (SW4 2 / OPLK b1=2); Span per half."));

	m_stop = FALSE;
	m_running = true;
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		m_uiRows.clear();
		m_pendingUiRows.clear();
		m_pendingNewLap = false;
		m_channelCount = 0;
		m_rowFlushScheduled = false;
	}
	m_list.SetItemCountEx(0, 0);
	SetControlsRunning(TRUE);
	StartHangupClock();
	m_worker = std::thread([this, outDir, wl, gate, mcs1Sn]() {
		WorkerEntry(outDir, wl, gate, mcs1Sn);
	});
}

void CM576FimIlTestDlg::OnBnClickedStop()
{
	if (!m_running.load())
		return;
	m_stop = TRUE;
	AppendLogLine(_T("Stop requested..."));
}

void CM576FimIlTestDlg::OnCancel()
{
	if (m_running.load())
	{
		m_stop = TRUE;
		if (m_worker.joinable())
			m_worker.join();
		m_running = false;
		if (m_pOwner)
			m_pOwner->EndIlTestSession();
	}
	else if (m_worker.joinable())
	{
		m_worker.join();
	}
	StopHangupClock();
	CDialogEx::OnCancel();
}

void CM576FimIlTestDlg::OnOK()
{
}

void CM576FimIlTestDlg::WorkerEntry(
	CString outDir, IlTestWlKind wl, IlTestGateParams gate, CString mcs1Sn)
{
	auto PostLog = [this](const CString& s) {
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_FIM_IL_LOG, 0, (LPARAM)new CString(s));
	};
	auto PostStatus = [this](const CString& s) {
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_FIM_IL_STATUS, 0, (LPARAM)new CString(s));
	};
	auto PostTemps = [this](const CString& s) {
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_FIM_IL_TEMPS, 0, (LPARAM)new CString(s));
	};
	auto RefreshTemps = [this, &PostLog, &PostTemps]() {
		if (!m_pOwner)
			return;
		M576FiveTemps temps;
		CString detail;
		(void)M576ReadAllFiveTempsC(m_pOwner->Device429f(), temps, detail);
		PostTemps(M576FormatFiveTempsUiLine(temps));
		CStringArray lines;
		M576AppendFiveTempsLogLines(temps, lines);
		for (int i = 0; i < lines.GetSize(); ++i)
			PostLog(lines[i]);
		if (!detail.IsEmpty())
			PostLog(_T("[TEMP] partial failures: ") + detail);
	};

	const int port1x8 = IlTestSwlChannel(wl);
	const int wlNm = IlTestWavelengthNm(wl);
	const CString wlLabel(IlTestWlLabel(wl));
	CString sn = M576SanitizeSnForFilename(mcs1Sn);
	if (sn.IsEmpty())
		sn = _T("unknown");
	CString csvPath;
	csvPath.Format(_T("%s\\%s_fim_il_test_log.csv"), outDir.GetString(), sn.GetString());
	CString binPath;
	binPath.Format(_T("%s\\FimIlTest_%s.bin"), outDir.GetString(), sn.GetString());
	CString spanPath;
	spanPath.Format(_T("%s\\%s_fim_ILMax-Min_Span.csv"), outDir.GetString(), sn.GetString());

	std::vector<FimTableRow> table;
	BuildFimTestTableRows((BYTE)port1x8, 1, table); // geometry fixed; b[1] rebuilt each lap
	const int N = (int)table.size();
	std::vector<IlTestRollingStats> stats((size_t)N * 2); // [0..N)=IN, [N..2N)=OUT

	bool armedWl = false;
	int fullLaps = 0;

	struct LogBridge
	{
		decltype(PostLog)* pPost;
	};
	auto logThunk = [](void* ctx, const CString& s) {
		LogBridge* b = (LogBridge*)ctx;
		if (b && b->pPost)
			(*b->pPost)(s);
	};

	for (;;)
	{
		if (m_stop)
			break;

		++fullLaps;
		const int half = IlTestHalfFromLap(fullLaps);
		const CString halfLabel(IlTestHalfLabel(half));
		BuildFimTestTableRows((BYTE)port1x8, (BYTE)half, table);
		BeginLapUi(N, fullLaps);
		{
			CString note;
			note.Format(_T("[FIMIL] --- lap %d begin half=%s SW4 %d (%d ch) ---"),
				fullLaps, halfLabel.GetString(), half, N);
			PostLog(note);
		}

		CDiagnosisSession* session = m_pOwner ? m_pOwner->GetDiagnosisSessionForIlTest() : NULL;
		if (session == NULL)
		{
			PostLog(_T("[FIMIL] serial session closed; aborting."));
			break;
		}
		COpComm& comm = session->Comm();
		CString err;

		if (!m_stop)
			RefreshTemps();
		if (m_stop)
			break;

		// Once: OPM AUTO + SWL (FIM SetTestWL; no SW 3 ?? driver stub).
		if (!armedWl)
		{
			CStringA reply;
			DWORD ms = 0;
			(void)session->ExchangeAsciiLine(_T("OPM4"), CStringA("OPM 4 1 0"), reply, 3000, ms, err);
			CStringA swl;
			swl.Format("SWL %d %d", port1x8, wlNm);
			CString label;
			label.Format(_T("SWL %d %d"), port1x8, wlNm);
			(void)session->ExchangeAsciiLine(label, swl, reply, 3000, ms, err);
			armedWl = true;
			PostLog(_T("[FIMIL] armed OPM4 AUTO + SWL (no SW 3, FIM stub align)"));
		}

		if (m_stop)
		{
			PostLog(_T("[FIMIL] stopped by user."));
			break;
		}

		// Per lap: SW 4 {half} -> PD (log only) -> BIN (b[1]=half) -> scan -> RESULT.
		{
			CStringA reply;
			DWORD ms = 0;
			CStringA sw4;
			sw4.Format("SW 4 %d", half);
			CString labelSw4;
			labelSw4.Format(_T("SW4 %s"), halfLabel.GetString());
			(void)session->ExchangeAsciiLine(labelSw4, sw4, reply, 3000, ms, err);
			(void)session->ExchangeAsciiLine(_T("PD"), CStringA("pd 1"), reply, 3000, ms, err);
			CString pdNote;
			pdNote.Format(_T("[FIMIL] SW 4 %d (%s) + pd 1 reply=%hs (log only, not used in IL)"),
				half, halfLabel.GetString(), reply.GetString());
			PostLog(pdNote);
		}
		::Sleep(150); // settle after ASCII before binary fwdl 0
		if (m_stop)
		{
			PostLog(_T("[FIMIL] stopped by user."));
			break;
		}

		if (!WriteOplkBinFile(binPath, table, err))
		{
			PostLog(_T("[FIMIL] ") + err);
			break;
		}
		{
			CString s;
			s.Format(_T("[FIMIL] wrote OPLK %s half=%s (%d rows)"),
				binPath.GetString(), halfLabel.GetString(), (int)table.size());
			PostLog(s);
		}

		PostStatus(_T("Xmodem fwdl 0 ..."));
		bool eotBanner = false;
		if (!DownloadTestBinXmodem(comm, binPath, err, &m_stop, &eotBanner))
		{
			PostLog(_T("[FIMIL] ") + err);
			if (m_stop)
				PostLog(_T("[FIMIL] stopped by user."));
			break;
		}
		{
			CString ok;
			ok.Format(_T("[FIMIL] Xmodem download OK (%s)"),
				eotBanner ? _T("ACK+Successful") : _T("ACK, no Successful banner"));
			PostLog(ok);
		}

		if (m_stop)
		{
			PostLog(_T("[FIMIL] stopped by user."));
			break;
		}

		PostStatus(_T("Wait scan OK ..."));
		LogBridge bridge{ &PostLog };
		if (!WaitScanOk(comm, err, &m_stop, logThunk, &bridge))
		{
			PostLog(_T("[FIMIL] ") + err);
			if (m_stop)
				PostLog(_T("[FIMIL] stopped by user."));
			break;
		}
		PostLog(_T("[FIMIL] CheckScanFinal OK"));

		if (m_stop)
		{
			PostLog(_T("[FIMIL] stopped by user."));
			break;
		}

		std::vector<double> opm;
		std::vector<double> pd;
		PostStatus(_T("RESULT ..."));
		if (!ReadResultIl(comm, (DWORD)table.size(), opm, pd, err, &m_stop))
		{
			PostLog(_T("[FIMIL] ") + err);
			if (m_stop)
				PostLog(_T("[FIMIL] stopped by user."));
			break;
		}
		PostLog(_T("[FIMIL] RESULT parsed"));

		std::vector<M576FimIlUiRow> uiRows(table.size());
		int failCount = 0;
		const size_t statsBase = (size_t)(half - 1) * (size_t)N;
		for (size_t i = 0; i < table.size(); ++i)
		{
			const double il = opm[i] - pd[i];
			IlTestRollingStats& st = stats[statsBase + i];
			st.Add(il);
			const double span = st.Span();
			const BOOL pass = IlTestJudgePass(il, span, gate);
			if (!pass)
				++failCount;

			M576FimIlUiRow& u = uiRows[i];
			u.channel = table[i].channel;
			u.half = halfLabel;
			u.wl = wlLabel;
			u.lap = fullLaps;
			u.pdDbm = pd[i];
			u.opmDbm = opm[i];
			u.il = il;
			u.mx = st.ilMax;
			u.mn = st.ilMin;
			u.span = span;
			u.result = pass ? _T("PASS") : _T("FAIL");

			CString csvErr;
			(void)AppendFimIlCsvRow(csvPath, fullLaps, halfLabel, table[i].channel, wlLabel,
				opm[i], pd[i], il, st.ilMax, st.ilMin, span, pass, csvErr);
		}
		QueueLapRows(uiRows);
		// Sync paint on UI thread so ListCtrl shows RESULT before next lap / Finish.
		if (m_hWnd && ::IsWindow(m_hWnd))
			::SendMessage(m_hWnd, WM_M576_FIM_IL_ROW_FLUSH, 0, 0);

		{
			CString s;
			s.Format(_T("[FIMIL] lap %d half=%s done: failGate=%d/%d (abs=[%.3f,%.3f] Span<=%.3f) csv=%s"),
				fullLaps, halfLabel.GetString(), failCount, (int)table.size(),
				gate.absIlMinDb, gate.absIlMaxDb, gate.spanMaxDb, csvPath.GetString());
			PostLog(s);
			s.Format(_T("Lap %d %s | failGate=%d/%d shown=%d | Span<=%.3f"),
				fullLaps, halfLabel.GetString(), failCount, (int)table.size(),
				(int)uiRows.size(), gate.spanMaxDb);
			PostStatus(s);
		}
	}

	{
		CString spanErr;
		if (WriteFimSpanCsv(spanPath, table, stats, wlLabel, spanErr))
		{
			CString s;
			s.Format(_T("[FIMIL] wrote %s"), spanPath.GetString());
			PostLog(s);
		}
		else if (!spanErr.IsEmpty())
		{
			PostLog(_T("[FIMIL] Span CSV: ") + spanErr);
		}
	}

	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		::SendMessage(m_hWnd, WM_M576_FIM_IL_ROW_FLUSH, 0, 0);
		::PostMessage(m_hWnd, WM_M576_FIM_IL_FINISHED, m_stop ? 1 : 0, (LPARAM)fullLaps);
	}
}

void CM576FimIlTestDlg::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDisp = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
	if (!pDisp || !(pDisp->item.mask & LVIF_TEXT) || !pDisp->item.pszText || pDisp->item.cchTextMax <= 0)
		return;
	const int i = pDisp->item.iItem;
	if (i < 0 || i >= (int)m_uiRows.size())
		return;
	const M576FimIlUiRow& r = m_uiRows[(size_t)i];
	CString s;
	switch (pDisp->item.iSubItem)
	{
	case 0: s = r.channel; break;
	case 1: s = r.half; break;
	case 2: s = r.wl; break;
	case 3: s.Format(_T("%d"), r.lap); break;
	case 4: s.Format(_T("%.4f"), r.pdDbm); break;
	case 5: s.Format(_T("%.4f"), r.opmDbm); break;
	case 6: s.Format(_T("%.4f"), r.il); break;
	case 7: s.Format(_T("%.4f"), r.mx); break;
	case 8: s.Format(_T("%.4f"), r.mn); break;
	case 9: s.Format(_T("%.4f"), r.span); break;
	case 10: s = r.result; break;
	default: s.Empty(); break;
	}
	_tcsncpy_s(pDisp->item.pszText, (size_t)pDisp->item.cchTextMax, s.GetString(), _TRUNCATE);
}

LRESULT CM576FimIlTestDlg::OnUiLog(WPARAM, LPARAM lParam)
{
	CString* p = (CString*)lParam;
	if (p)
	{
		AppendLogLine(*p);
		delete p;
	}
	return 0;
}

LRESULT CM576FimIlTestDlg::OnUiStatus(WPARAM, LPARAM lParam)
{
	CString* p = (CString*)lParam;
	if (p)
	{
		SetDlgItemText(IDC_FIM_IL_STATIC_STATUS, *p);
		delete p;
	}
	return 0;
}

LRESULT CM576FimIlTestDlg::OnUiTemps(WPARAM, LPARAM lParam)
{
	CString* p = (CString*)lParam;
	if (p)
	{
		SetDlgItemText(IDC_FIM_IL_STATIC_TEMPS, *p);
		delete p;
	}
	return 0;
}

LRESULT CM576FimIlTestDlg::OnUiRowFlush(WPARAM, LPARAM)
{
	FlushPendingUiRows();
	return 0;
}

LRESULT CM576FimIlTestDlg::OnUiFinished(WPARAM wParam, LPARAM lParam)
{
	if (m_worker.joinable())
		m_worker.join();
	m_running = false;
	StopHangupClock();
	SetControlsRunning(FALSE);
	if (m_pOwner)
		m_pOwner->EndIlTestSession();
	CString s;
	s.Format(_T("Finished (stop=%d laps=%ld)"), (int)wParam, (long)lParam);
	SetDlgItemText(IDC_FIM_IL_STATIC_STATUS, s);
	AppendLogLine(_T("[FIMIL] ") + s);
	return 0;
}
