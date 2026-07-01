#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576CalibratorDlg.h"
#include "CalibPathOutcome.h"
#include "M576RunPathSummaryDlg.h"
#include "M576BurnSelectDlg.h"
#include "M576RecoverSelectDlg.h"
#include "LutMerge1310.h"
#include "LutMerge1550.h"
#include "CalibWavelengthPolicy.h"
#include "Mems1x64LutBinWriter.h"
#include "CalibConstants.h"
#include "M576OutputArchive.h"
#include "PeakFinder2D.h"
#include "Peak1DSweepRecenter.h"
#include "PmRangeValidation.h"
#include "LutPeakApply.h"
#include "Pm1x64Mapping.h"
#include "M576GlobalException.h"
#include "M576AppConfig.h"
#include "M576Version.h"
#include <math.h>
#include <cmath>
#include <algorithm>
#include <array>
#include <exception>
// M576CalibratorDlg.cpp：M576 定标主界面。单 COM 经 439F 发 RECAL、经 trans/$$ 跑 Z4671；
// 后台线程执行 Read Bin / Run Path / Burn / Diagnosis，UI 线程仅收 WM_M576_* 消息刷日志与进度。

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {
// ---------- 文件内静态工具：COM 枚举、通信日志、UTF-8/ACP 窄串、路径 outcome 构造、RECAL 超时估算 ----------

static const TCHAR* kM576FixedBackupBinRel = M576_BIN_OUTPUT_DIR_REL;
static const TCHAR* kM576FixedOutBinRel = M576_BIN_OUTPUT_DIR_REL;

static int ComPortSortKey(const CString& s)
{
	if (s.GetLength() < 4 || _tcsnicmp(s, _T("COM"), 3) != 0)
		return 999999;
	return _ttoi(s.GetString() + 3);
}

/// List present COM ports: QueryDosDevice(COM1..COM256), fallback HARDWARE\\DEVICEMAP\\SERIALCOMM.
static void EnumPresentComPorts(std::vector<CString>& out)
{
	out.clear();
	std::vector<CString> found;
	TCHAR target[16384];
	for (int i = 1; i <= 256; ++i)
	{
		CString name;
		name.Format(_T("COM%d"), i);
		if (::QueryDosDevice(name, target, _countof(target)) != 0)
			found.push_back(name);
	}
	if (found.empty())
	{
		HKEY hKey = NULL;
		if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD idx = 0;
			TCHAR vn[256];
			BYTE data[256];
			for (;; ++idx)
			{
				DWORD vnLen = _countof(vn);
				DWORD dataLen = sizeof(data);
				DWORD typ = 0;
				LONG e = ::RegEnumValue(hKey, idx, vn, &vnLen, NULL, &typ, data, &dataLen);
				if (e == ERROR_NO_MORE_ITEMS)
					break;
				if (e != ERROR_SUCCESS)
					continue;
				if (typ == REG_SZ || typ == REG_EXPAND_SZ)
				{
					CString port((LPCTSTR)data);
					port.Trim();
					if (port.GetLength() >= 4 && _tcsnicmp(port, _T("COM"), 3) == 0)
						found.push_back(port);
				}
			}
			::RegCloseKey(hKey);
		}
	}
	std::sort(found.begin(), found.end(), [](const CString& a, const CString& b) {
		return ComPortSortKey(a) < ComPortSortKey(b);
	});
	for (size_t i = 0; i < found.size(); ++i)
	{
		if (i == 0 || found[i].CompareNoCase(found[i - 1]) != 0)
			out.push_back(found[i]);
	}
}

/// `Exchange*` write-retry error messages contain "write" (EN); used to `break` path vs `continue` step.
static bool M576CommErrIsSerialWriteFailure(const CString& e)
{
	if (e.IsEmpty())
		return false;
	CString t(e);
	t.MakeLower();
	return t.Find(_T("write")) >= 0;
}

static const TCHAR* M576Peak1DWhy(M576::Peak1DValidateCode c)
{
	using M576::Peak1DValidateCode;
	switch (c)
	{
	case Peak1DValidateCode::Ok:
		return _T("");
	case Peak1DValidateCode::Empty:
		return _T("empty or invalid");
	case Peak1DValidateCode::LowSpan:
		return _T("low span (flat / no contrast)");
	case Peak1DValidateCode::NotStrictLocal:
		return _T("argmax not a strict local peak");
	case Peak1DValidateCode::EdgeNotAllowed:
		return _T("peak at sweep edge (policy)");
	case Peak1DValidateCode::MultiLocalMax:
		return _T("too many local maxima");
	case Peak1DValidateCode::NotEnoughSamples:
		return _T("not enough points for cubic fit (need >=4)");
	case Peak1DValidateCode::NotEnoughValidSamples:
		return _T("not enough valid samples after preprocessing (need >=4)");
	case Peak1DValidateCode::ParabolaNotDownward:
		return _T("1D cubic fit rejected (flat / monotone / no peak)");
	case Peak1DValidateCode::ParabolaFitSingular:
		return _T("cubic fit singular / ill-conditioned");
	case Peak1DValidateCode::VertexOutOfRange:
		return _T("peak position outside sweep [0..n-1]");
	case Peak1DValidateCode::PmRangeMismatch:
		return _T("peak power dBm outside selected PM range");
	default:
		return _T("unknown");
	}
}

enum class M576Peak1DLogStage
{
	YPre,
	YCross,
	XCross
};

static CString M576FormatPeak1DMsg(bool isPm, M576Peak1DLogStage st, M576::Peak1DValidateCode c)
{
	const TCHAR* w = M576Peak1DWhy(c);
	const TCHAR* cmd = isPm ? _T("RECAL 3") : _T("RECAL 5");
	CString a;
	switch (st)
	{
	case M576Peak1DLogStage::YPre:
		a.Format(_T("  peak: Y (%s axis0) — %s; skip next axis sweep."), cmd, w);
		break;
	case M576Peak1DLogStage::YCross:
		a.Format(_T("  peak: Y (%s cross) — %s; skip LUT update."), cmd, w);
		break;
	case M576Peak1DLogStage::XCross:
		a.Format(_T("  peak: X (%s cross) — %s; skip LUT update."), cmd, w);
		break;
	}
	return a;
}

static std::string M576WideCStringToUtf8(const CStringW& w)
{
	if (w.IsEmpty())
		return std::string();
	const int wlen = w.GetLength();
	const int n8 = ::WideCharToMultiByte(CP_UTF8, 0, w, wlen, NULL, 0, NULL, NULL);
	if (n8 < 1)
		return std::string();
	std::string out(static_cast<size_t>(n8), '\0');
	::WideCharToMultiByte(CP_UTF8, 0, w, wlen, &out[0], n8, NULL, NULL);
	return out;
}

static std::string M576CStringToUtf8(const CString& s)
{
	if (s.IsEmpty())
		return std::string();
	const int n = s.GetLength();
	const char* p = reinterpret_cast<const char*>(reinterpret_cast<LPCTSTR>(s.GetString()));
	int cch = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, p, n, NULL, 0);
	CStringW w;
	if (cch > 0)
	{
		::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, p, n, w.GetBuffer(cch), cch);
		w.ReleaseBuffer(cch);
	}
	else
	{
		cch = ::MultiByteToWideChar(CP_ACP, 0, p, n, NULL, 0);
		if (cch <= 0)
			return std::string(p, static_cast<size_t>(n));
		::MultiByteToWideChar(CP_ACP, 0, p, n, w.GetBuffer(cch), cch);
		w.ReleaseBuffer(cch);
	}
	return M576WideCStringToUtf8(w);
}

static std::string M576PathBasenameUtf8(LPCTSTR path)
{
	if (path == NULL || path[0] == 0)
		return std::string();
	CString base(path);
	const int bs = base.ReverseFind(_T('\\'));
	const int fs = base.ReverseFind(_T('/'));
	const int slash = (bs > fs) ? bs : fs;
	if (slash >= 0)
		base = base.Mid(slash + 1);
	return M576CStringToUtf8(base);
}

static void M576FillPeakCodeFields(SCalibPathStepOutcome& o, M576::Peak1DValidateCode code)
{
	o.peakCode = code;
	if (code != M576::Peak1DValidateCode::Ok)
		o.peakCodeText = M576CStringToUtf8(CString(M576Peak1DWhy(code)));
}

static std::string M576FormatPathStepRouteLabelPm(const SPathStep& step)
{
	char buf[128];
	std::snprintf(
		buf,
		sizeof(buf),
		"tgt=%d c1=%d c2=%d c3=%d c4=%d",
		step.targetSwitchIndex,
		step.c1,
		step.c2,
		step.c3,
		step.c4);
	return std::string(buf);
}

static std::string M576FormatPathStepRouteLabelPd(const SPathStepPd& step)
{
	char buf[96];
	std::snprintf(
		buf,
		sizeof(buf),
		"tgt=%d ch1=%d ch2=%d",
		step.targetSwitchIndex,
		step.ch1,
		step.ch2);
	return std::string(buf);
}

static SCalibPathStepOutcome M576MakePmOutcome(
	int fileSlot,
	const std::string& csvPath,
	int line1,
	const SPathStep& st)
{
	SCalibPathStepOutcome o = {};
	o.isPm = true;
	o.fileSlot = fileSlot;
	o.csvPath = csvPath;
	o.pathLine1Based = line1;
	o.targetSwitchIndex = st.targetSwitchIndex;
	o.routeLabel = M576FormatPathStepRouteLabelPm(st);
	return o;
}

static SCalibPathStepOutcome M576MakePdOutcome(
	int fileSlot,
	const std::string& csvPath,
	int line1,
	const SPathStepPd& st)
{
	SCalibPathStepOutcome o = {};
	o.isPm = false;
	o.fileSlot = fileSlot;
	o.csvPath = csvPath;
	o.pathLine1Based = line1;
	o.targetSwitchIndex = st.targetSwitchIndex;
	o.routeLabel = M576FormatPathStepRouteLabelPd(st);
	return o;
}

static void M576FillSweepPowContext(
	SCalibPathStepOutcome& o,
	const std::vector<double>& pow,
	bool isY)
{
	if (pow.empty())
		return;
	const M576::SweepProfile p = M576::AnalyzeRecal1DSweepProfile(pow);
	if (isY)
		FillSweepProfileContext(p, o.sweepTrendY, o.spanY, o.argmaxY);
	else
		FillSweepProfileContext(p, o.sweepTrendX, o.spanX, o.argmaxX);
}

/// 439F 管理口文本：发送 `info<CR>`，在超时内收齐应答（空闲 200ms 或最长 3s 结束），用于“测试连接”。
static BOOL M576Try439fInfoTest(Z4671Command& comm, CString& outResponseOneLine, CString& err)
{
	outResponseOneLine.Empty();
	err.Empty();
	HANDLE h = comm.GetPortHandle();
	if (!h || h == INVALID_HANDLE_VALUE)
	{
		err = _T("Port is not open.");
		return FALSE;
	}
	(void)PurgeComm(h, PURGE_RXCLEAR);
	static const char kInfoCmd[] = "info\r";
	if (!comm.WriteBufferNoPurge((char*)kInfoCmd, (DWORD)sizeof(kInfoCmd) - 1u))
	{
		err = _T("Serial write failed (info<CR>).");
		return FALSE;
	}
	::Sleep(20);
	CStringA acc;
	const DWORD t0 = ::GetTickCount();
	DWORD lastDataTick = 0;
	for (;;)
	{
		const DWORD now = ::GetTickCount();
		if (comm.RxBytesWaiting() > 0)
		{
			BYTE buf[600];
			DWORD rd = 0;
			if (comm.ReadBuffer(buf, sizeof(buf), &rd) && rd > 0u)
			{
				acc += CStringA(reinterpret_cast<const char*>(buf), (int)rd);
				lastDataTick = now;
			}
		}
		else
		{
			if (!acc.IsEmpty() && (now - lastDataTick) >= 200u)
				break;
			if (acc.IsEmpty() && (now - t0) > 3000u)
			{
				err = _T("Timeout: no reply to info<CR> (3s).");
				return FALSE;
			}
		}
		if (!acc.IsEmpty() && (now - t0) > 3000u)
			break;
		::Sleep(5);
	}
	acc.Replace("\r", " ");
	acc.Replace("\n", " ");
	acc.Trim();
	if (acc.IsEmpty())
	{
		err = _T("No text in reply to info<CR> (device silent).");
		return FALSE;
	}
	outResponseOneLine = CString(acc);
	return TRUE;
}

// 工作线程 -> UI 线程：日志批量刷新、进度条、各后台任务完成通知（禁止在工作线程直接改控件）。
constexpr UINT WM_M576_PATH_LOG_FLUSH = WM_APP + 100;
constexpr UINT WM_M576_PATH_PROGRESS_RANGE = WM_APP + 101;
constexpr UINT WM_M576_PATH_PROGRESS_POS = WM_APP + 102;
constexpr UINT WM_M576_PATH_FINISHED = WM_APP + 103;
constexpr UINT WM_M576_READ_BACKUP_FINISHED = WM_APP + 104;
constexpr UINT WM_M576_READ_SN_FINISHED = WM_APP + 105;
constexpr UINT WM_M576_BURN_FLASH_FINISHED = WM_APP + 106;
constexpr UINT WM_M576_DIAG_FINISHED = WM_APP + 107;
constexpr UINT WM_M576_BURN_BOARD_FINISHED = WM_APP + 108;

/// RECAL 3/5 一行：`[轴上 DAC 或首列][P1..Pn]`。沿动轴在 [-range,+range] 上含端点步进时，功率点数
/// N = floor((2*range)/step)+1 = (2*range)/step+1（整型除法），与固件一致（例 range=64 step=4 -> N=33；step=5 -> N=26；整行 1+N 个数）。旧式 ceil(2*range/step) 在 2*range 整除 step 时会少 1 点。
static int RecalSweepPowerSampleCount(int dacRange, int dacStep)
{
	if (dacStep < 1)
		dacStep = 1;
	if (dacRange < 1)
		dacRange = 1;
	const int twoR = 2 * dacRange;
	const int n = twoR / dacStep + 1;
	return (n < 1) ? 1 : n;
}

/// One `RECAL 3` / `RECAL 5` axis sweep: timeout = min(n*delay + margin, M576_MAX_RECAL_SWEEP_READ_MS), clamped by min (CalibConstants.h).
static DWORD ComputeRecal1DReadTimeoutMs(int delayMs, int dacRange, int dacStep)
{
	if (delayMs < 1)
		delayMs = 1;
	int n = RecalSweepPowerSampleCount(dacRange, dacStep);
	if (n < 2)
		n = 2;
	__int64 t = (__int64)n * delayMs + (__int64)M576_RECAL_SWEEP_READ_MARGIN_MS;
	if (t > (__int64)M576_MAX_RECAL_SWEEP_READ_MS)
		t = (__int64)M576_MAX_RECAL_SWEEP_READ_MS;
	if (t < (__int64)M576_MIN_RECAL_SWEEP_READ_MS)
		t = (__int64)M576_MIN_RECAL_SWEEP_READ_MS;
	return (DWORD)t;
}

static int AxisPointCount(int dacRange, int dacStep)
{
	return RecalSweepPowerSampleCount(dacRange, dacStep);
}

/// Cross-peak uX/uY: `SweepCol0PlusPeakOffsetDac` + lround. MCS: uY->wCalibPtrDAC[][0], uX->[1]. Mems: uY->sDACx, uX->sDACy (LutPeakApply).
static double SweepCol0PlusPeakOffsetDac(double sweepLineCol0, int peakIndex, int sampleCount, int halfRange)
{
	if (sampleCount <= 1)
		return sweepLineCol0;
	const double step = (2.0 * halfRange) / (double)(sampleCount - 1);
	return sweepLineCol0 + (double)peakIndex * step;
}

/// 连续峰位下标（抛物线顶点 t*），与步进相乘得亚格点 DAC。
static double SweepCol0PlusPeakOffsetDac(double sweepLineCol0, double peakIndexFloat, int sampleCount, int halfRange)
{
	if (sampleCount <= 1)
		return sweepLineCol0;
	const double step = (2.0 * halfRange) / (double)(sampleCount - 1);
	return sweepLineCol0 + peakIndexFloat * step;
}

/// After `RECAL 3 0` / `RECAL 5 0` with Base=9999, firmware returns the **first sweep cell** in col0; the moving-axis
/// DAC at peak = col0 + peakIndex * (2*halfRange/(n-1)) — *not* `M576_PEAK_GRID_DAC_BASE` ± range (avoids 2048 vs 2289).
static int RecalDacAtPeakIndexFromSweepCol0(int peakIndex, int sampleCount, int halfRange, double sweepLineCol0)
{
	if (sampleCount <= 1)
		return M576_RECAL_FW_READ_BASE_DAC;
	const double y = SweepCol0PlusPeakOffsetDac(sweepLineCol0, peakIndex, sampleCount, halfRange);
	int iy = (int)floor(y + 0.5);
	// col0+idx*gridStep is a signed linear coordinate; do not map negatives to 0.
	if (iy > 65535)
		iy = 65535;
	return iy;
}

static int RecalDacAtPeakIndexFromSweepCol0(double peakIndexFloat, int sampleCount, int halfRange, double sweepLineCol0)
{
	if (sampleCount <= 1)
		return M576_RECAL_FW_READ_BASE_DAC;
	const double y = SweepCol0PlusPeakOffsetDac(sweepLineCol0, peakIndexFloat, sampleCount, halfRange);
	int iy = (int)floor(y + 0.5);
	if (iy > 65535)
		iy = 65535;
	return iy;
}

static double RecalSweepCenterFromCol0(double col0, int halfRange)
{
	return col0 + (double)halfRange;
}

CString FormatLogTimestamp()
{
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	CString ts;
	ts.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d.%03d]"),
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);
	return ts;
}

/// Z4671 等库在 /utf-8 下为 UTF-8 窄串；GetModuleFileName 等为系统 ACP。统一先尝试严格 UTF-8，再回退 ACP。
static CStringW M576NarrowToWideForUi(const CString& narrow)
{
	if (narrow.IsEmpty())
		return L"";
	const int n = narrow.GetLength();
	const char* p = reinterpret_cast<const char*>(reinterpret_cast<LPCTSTR>(narrow.GetString()));
	int cch = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, p, n, NULL, 0);
	if (cch > 0)
	{
		CStringW w;
		::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, p, n, w.GetBuffer(cch), cch);
		w.ReleaseBuffer(cch);
		return w;
	}
	cch = ::MultiByteToWideChar(CP_ACP, 0, p, n, NULL, 0);
	if (cch <= 0)
		return L"(text decode error)";
	CStringW w;
	::MultiByteToWideChar(CP_ACP, 0, p, n, w.GetBuffer(cch), cch);
	w.ReleaseBuffer(cch);
	return w;
}

static void M576TrimEditLogTextW(CStringW& text)
{
	const int kMaxChars = 200000;
	const int kKeepChars = 120000;
	if (text.GetLength() <= kMaxChars)
		return;
	text = text.Right(kKeepChars);
	int firstBreak = text.Find(L"\n");
	if (firstBreak >= 0 && firstBreak + 1 < text.GetLength())
		text = text.Mid(firstBreak + 1);
}

void TrimEditLogText(CString& text)
{
	const int kMaxChars = 200000;
	const int kKeepChars = 120000;
	if (text.GetLength() <= kMaxChars)
		return;
	text = text.Right(kKeepChars);
	int firstBreak = text.Find(_T("\n"));
	if (firstBreak >= 0 && firstBreak + 1 < text.GetLength())
		text = text.Mid(firstBreak + 1);
}

CString GetExeFolder()
{
	TCHAR sz[MAX_PATH];
	DWORD n = GetModuleFileName(NULL, sz, MAX_PATH);
	if (n == 0 || n >= MAX_PATH)
		return CString();
	CString path = sz;
	int p = path.ReverseFind(_T('\\'));
	if (p > 0)
		path = path.Left(p);
	return path;
}

void EnsureOutputFolderUnderExe(const CString& exeFolder)
{
	if (exeFolder.IsEmpty())
		return;
	CString err;
	(void)M576EnsureDirTree(exeFolder + _T("\\output"), err);
	(void)M576EnsureDirTree(M576ResolveLatestBinDirAbs(exeFolder), err);
	(void)M576EnsureDirTree(M576ResolveArchiveRootAbs(exeFolder), err);
}

/// Convert an absolute path to a relative path from the exe folder.
/// Returns the original path unchanged if conversion is not possible.
CString ToRelPath(const CString& absPath)
{
	const CString base = GetExeFolder();
	if (base.IsEmpty() || absPath.IsEmpty())
		return absPath;
	TCHAR rel[MAX_PATH] = {};
	if (PathRelativePathTo(rel, base.GetString(), FILE_ATTRIBUTE_DIRECTORY,
						   absPath.GetString(), 0))
	{
		CString r = rel;
		// Strip leading ".\" for a cleaner display
		if (r.GetLength() > 2 && r[0] == _T('.') && r[1] == _T('\\'))
			r = r.Mid(2);
		return r;
	}
	return absPath;
}

/// Resolve a stored path (may be relative) to an absolute path using the exe folder as base.
CString ResolveFilePath(const CString& path)
{
	if (path.IsEmpty())
		return path;
	// Already absolute if it starts with a drive letter or UNC prefix
	if ((path.GetLength() >= 2 && path[1] == _T(':')) ||
		(path.GetLength() >= 2 && path[0] == _T('\\')))
		return path;
	const CString base = GetExeFolder();
	if (base.IsEmpty())
		return path;
	TCHAR abs[MAX_PATH] = {};
	CString combined = base + _T("\\") + path;
	if (PathCanonicalize(abs, combined.GetString()))
		return CString(abs);
	return combined;
}

/// From base path `output\comm.log` build `output\comm_2026-04-24.log` (new file each local calendar day).
static CString CommLogPathForCurrentDay(const CString& commLogPathRel)
{
	if (commLogPathRel.IsEmpty())
		return commLogPathRel;
	int sep = commLogPathRel.ReverseFind(_T('\\'));
	if (sep < 0)
		sep = commLogPathRel.ReverseFind(_T('/'));
	CString dir, fname;
	if (sep >= 0)
	{
		dir = commLogPathRel.Left(sep);
		fname = commLogPathRel.Mid(sep + 1);
	}
	else
	{
		fname = commLogPathRel;
	}
	int dot = fname.ReverseFind(_T('.'));
	CString stem, ext;
	if (dot > 0)
	{
		stem = fname.Left(dot);
		ext = fname.Mid(dot);
	}
	else
	{
		stem = fname;
		ext = _T(".log");
	}
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	CString day;
	day.Format(_T("%04d-%02d-%02d"), (int)st.wYear, (int)st.wMonth, (int)st.wDay);
	CString rel;
	if (dir.IsEmpty())
		rel.Format(_T("%s_%s%s"), stem.GetString(), day.GetString(), ext.GetString());
	else
		rel.Format(_T("%s\\%s_%s%s"), dir.GetString(), stem.GetString(), day.GetString(), ext.GetString());
	return rel;
}

/// Parse wavelength nm from combo edit (presets 1310/1550 or typed value).
static BOOL ParseWavelengthNm(const CString& raw, int& outNm, CString& err)
{
	CString s = raw;
	s.Trim();
	if (s.IsEmpty())
	{
		err = _T("Wavelength is empty.");
		return FALSE;
	}
	TCHAR* end = NULL;
	const long v = _tcstol(s, &end, 10);
	if (!end || end != (LPCTSTR)s + s.GetLength())
	{
		err = _T("Invalid wavelength (enter integer nm, e.g. 1310 or 1550).");
		return FALSE;
	}
	if (v < M576_MIN_WAVELENGTH_NM || v > M576_MAX_WAVELENGTH_NM)
	{
		err.Format(_T("Wavelength %ld nm out of range %d..%d."),
			v, M576_MIN_WAVELENGTH_NM, M576_MAX_WAVELENGTH_NM);
		return FALSE;
	}
	outNm = (int)v;
	return TRUE;
}

static void FormatSwlWire(CStringA& wire, int tlsSource, int wavelengthNm)
{
	wire.Format("SWL %d %d", tlsSource, wavelengthNm);
}

static void FormatSwlLabel(CString& label, LPCTSTR tag, int tlsSource, int wavelengthNm)
{
	label.Format(_T("SWL%d %d %s"), tlsSource, wavelengthNm, tag);
}

static void StripUtfBomForFirstLine(CString& line, int lineNo)
{
	if (lineNo != 1 || line.IsEmpty())
		return;
	if (line[0] == 0xFEFF)
	{
		line = line.Mid(1);
		return;
	}
	// Some UTF-8 CSV files are read as ANSI text by CStdioFile, exposing BOM as "ï»¿".
	if (line.GetLength() >= 3
		&& line[0] == (TCHAR)0x00EF
		&& line[1] == (TCHAR)0x00BB
		&& line[2] == (TCHAR)0x00BF)
	{
		line = line.Mid(3);
	}
}

static void SplitCsvColumns(const CString& line, CStringArray& cols)
{
	cols.RemoveAll();
	int start = 0;
	for (;;)
	{
		const int pos = line.Find(_T(','), start);
		CString field = (pos < 0) ? line.Mid(start) : line.Mid(start, pos - start);
		field.Trim();
		cols.Add(field);
		if (pos < 0)
			break;
		start = pos + 1;
	}
}

static BOOL ParseStrictIntField(const CString& text, int& value)
{
	CString s = text;
	s.Trim();
	if (s.IsEmpty())
		return FALSE;
	TCHAR* end = NULL;
	const long v = _tcstol(s, &end, 10);
	if (end == NULL || end != (LPCTSTR)s + s.GetLength())
		return FALSE;
	value = (int)v;
	return TRUE;
}

/// Session buffers all zero: no cal path run in this session (or equivalent). Write BIN may preload local `backup*.bin` like run path.
static bool M576SessionLutMemsAllZero(
	const stLutSettingZ4671 lut4[4],
	const stM576OneX64MemsSwCoef mems2[2][4])
{
	static const stLutSettingZ4671 zL = {};
	static const stM576OneX64MemsSwCoef zM = {};
	for (int i = 0; i < 4; ++i)
	{
		if (memcmp(&lut4[i], &zL, sizeof(zL)) != 0)
			return false;
	}
	for (int t = 0; t < 2; ++t)
		for (int s = 0; s < 4; ++s)
		{
			if (memcmp(&mems2[t][s], &zM, sizeof(zM)) != 0)
				return false;
		}
	return true;
}

} // namespace

static CString M576FormatRecalPowersForLog(const std::vector<double>& powers)
{
	CString s;
	for (size_t i = 0; i < powers.size(); ++i)
	{
		if (i > 0)
			s += _T(",");
		CString one;
		one.Format(_T("%.0f"), powers[i]);
		s += one;
	}
	return s;
}

void M576AppendPmRangeRejectLog(
	CM576CalibratorDlg* dlg,
	int pmRangeIndex,
	LPCTSTR stageLabel,
	LPCTSTR axisTag,
	double col0,
	const std::vector<double>& powers,
	int peakIdx,
	double peakRaw,
	double peakDbm,
	double loDbm,
	double hiDbm)
{
	if (!dlg)
		return;
	CString head;
	head.Format(
		_T("[PM range] REJECT %s %s: pm_range=%d need %hs dBm, peak_raw=%.0f peak_dBm=%.4f idx=%d col0=%.4g"),
		stageLabel,
		axisTag,
		pmRangeIndex,
		M576::PmRangeDbmIntervalDesc(pmRangeIndex),
		peakRaw,
		peakDbm,
		peakIdx,
		col0);
	dlg->SafeAppendLog(head);
	CString plist;
	plist.Format(_T("  P1..Pn: %s"), M576FormatRecalPowersForLog(powers).GetString());
	dlg->SafeAppendLog(plist);
	dlg->SafeAppendLog(_T("  => discard this path step (no LUT update)."));
}

// ---------- RECAL 3/5 一维扫频 + 寻峰重试（平坦/贴边 recenter，供 PM/PD 双轴交叉峰） ----------
BOOL CM576CalibratorDlg::RunRecal1DSweepWithPeakRecenterRetry(
	BOOL isPm,
	int pmRangeIndex,
	int sweepMode,
	int fixedBaseDac,
	int initialMovingBase,
	int initialDacRange,
	DWORD readTimeoutMs,
	LPCTSTR axisTag,
	LPCTSTR recalStageLabel,
	std::vector<double>& outPow,
	double& outCol0,
	int& outPeakIdx,
	M576::Peak1DValidateCode& outCode,
	M576::Peak1DFitTrace& outTrace,
	double& outTPeak,
	int& outAttemptCount,
	int& outDacRangeUsed,
	CString& err)
{
	err.Empty();
	outCode = M576::Peak1DValidateCode::Empty;
	outPeakIdx = 0;
	outTPeak = 0.0;
	outAttemptCount = 0;
	outDacRangeUsed = m_dacRange;
	outCol0 = 0.0;
	outPow.clear();
	outTrace = M576::Peak1DFitTrace();

	if (!m_pRecal)
	{
		err = _T("RECAL session not open.");
		return FALSE;
	}

	const int uiFineRange = (initialDacRange >= M576_MIN_DAC_RANGE) ? initialDacRange : m_dacRange;
	M576::SweepRecenterSessionState retryState = {};
	M576::InitSweepRecenterSessionState(retryState, uiFineRange, initialMovingBase);
	int movingBase = retryState.movingBase;
	int attemptDacRange = retryState.attemptRange;
	DWORD attemptTimeout = (initialDacRange >= M576_MIN_DAC_RANGE)
		? ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRange, m_dacStep)
		: readTimeoutMs;
	CStringA lineY;
	for (int attempt = 0; attempt < (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS; ++attempt)
	{
		outAttemptCount = attempt + 1;
		const int baseX = (sweepMode == 0) ? fixedBaseDac : movingBase;
		const int baseY = (sweepMode == 0) ? movingBase : fixedBaseDac;
		const BOOL got = isPm
			? m_pRecal->ExchangeRecal3ReadSweep(
				sweepMode, baseX, baseY, attemptDacRange, m_dacStep, m_delayMs, lineY, attemptTimeout, err)
			: m_pRecal->ExchangeRecal5ReadSweep(
				sweepMode, baseX, baseY, attemptDacRange, m_dacStep, m_delayMs, lineY, attemptTimeout, err);
		if (!got)
			return FALSE;

		outPow.clear();
		if (!CRecalSession::ParseRecal3SweepLine(lineY, outCol0, outPow))
		{
			err = _T("Could not parse sweep line [col0] P1..Pn.");
			return FALSE;
		}

		const int n = (int)outPow.size();
		if (n <= 0)
		{
			outCode = M576::Peak1DValidateCode::Empty;
			return FALSE;
		}

		const M576::Peak1DFitPolicy fitPolicy = M576::IsFineRefineSweepAttempt(retryState)
			? M576::Peak1DFitPolicy::FineRefineRelaxed
			: M576::Peak1DFitPolicy::Strict;
		if (M576::FindUnimodalPeak1DIndex(outPow, outPeakIdx, outCode, &outTPeak, &outTrace, fitPolicy))
		{
			if (outTrace.usedArgmaxFallback)
			{
				CString msg;
				msg.Format(
					_T("  fine refine: %s %s cubic fallback to argmax t*=%.4g idx=%d"),
					recalStageLabel,
					axisTag,
					outTPeak,
					outPeakIdx);
				SafeAppendLog(msg);
			}
			if (isPm && pmRangeIndex != M576_MAX_PM_RANGE)
			{
				const int peakHint = (outTrace.globalMaxIndex >= 0) ? outTrace.globalMaxIndex : outPeakIdx;
				double peakRaw = 0.0, peakDbm = 0.0, loDbm = 0.0, hiDbm = 0.0;
				int peakIdxUsed = -1;
				if (!M576::ValidatePeakPowerInPmRange(
						pmRangeIndex, outPow, peakHint, peakRaw, peakDbm, loDbm, hiDbm, peakIdxUsed))
				{
					outCode = M576::Peak1DValidateCode::PmRangeMismatch;
					M576AppendPmRangeRejectLog(
						this,
						pmRangeIndex,
						recalStageLabel,
						axisTag,
						outCol0,
						outPow,
						peakIdxUsed,
						peakRaw,
						peakDbm,
						loDbm,
						hiDbm);
					return FALSE;
				}
			}
			if (M576::NeedsFineRefineAfterSuccess(attemptDacRange, uiFineRange)
				&& !retryState.fineConsumed)
			{
				const M576::SweepRetryPlan finePlan = M576::PlanFineRefineAfterCoarseSuccess(
					retryState, outCol0, outTPeak, n, attemptDacRange);
				if (finePlan.action == M576::SweepRetryAction::FineRefine)
				{
					const int prevRange = attemptDacRange;
					M576::ApplySweepRetryPlan(retryState, finePlan);
					movingBase = retryState.movingBase;
					attemptDacRange = retryState.attemptRange;
					attemptTimeout = ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRange, m_dacStep);
					{
						CString msg;
						msg.Format(
							_T("  %hs %s %s attempt %d/%d trend=coarseOk span=%.4g refine offset %d->%d base %d->%d (fine @%d)"),
							M576::SweepRetryActionLogTag(finePlan.action),
							recalStageLabel,
							axisTag,
							attempt + 1,
							(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
							M576::AnalyzeRecal1DSweepProfile(outPow).span,
							prevRange,
							attemptDacRange,
							movingBase,
							finePlan.nextBase,
							uiFineRange);
						SafeAppendLog(msg);
					}
					movingBase = finePlan.nextBase;
					retryState.movingBase = movingBase;
					continue;
				}
			}
			outDacRangeUsed = attemptDacRange;
			return TRUE;
		}

		const M576::SweepProfile profile = M576::AnalyzeRecal1DSweepProfile(outPow);
		const BOOL lastAttempt = (attempt >= (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS - 1);

		M576::SweepRecenterFailureInfo failInfo = {};
		failInfo.code = outCode;
		failInfo.tPeak = outTPeak;
		failInfo.hasTPeak = std::isfinite(outTPeak);
		failInfo.prevArgmaxIndex = retryState.prevArgmax;
		failInfo.hasPrevAttempt = (attempt > 0);

		retryState.movingBase = movingBase;
		retryState.attemptRange = attemptDacRange;
		const double centerDac = RecalSweepCenterFromCol0(outCol0, attemptDacRange);
		const M576::SweepRetryPlan plan = M576::PlanNextRecal1DSweepAttempt(
			retryState, outCode, profile, outPow, centerDac, attempt, lastAttempt, failInfo);
		if (plan.action == M576::SweepRetryAction::GiveUp)
			return FALSE;

		const int prevRange = attemptDacRange;
		const int prevBase = movingBase;
		M576::ApplySweepRetryPlan(retryState, plan);
		movingBase = retryState.movingBase;
		attemptDacRange = retryState.attemptRange;
		attemptTimeout = ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRange, m_dacStep);
		{
			CString msg;
			if (plan.action == M576::SweepRetryAction::JumpFlatMax)
			{
				msg.Format(
					_T("  %hs %s %s attempt %d/%d code=%hs trend=%hs span=%.4g offset %d->%d (base unchanged)"),
					M576::SweepRetryActionLogTag(plan.action),
					recalStageLabel,
					axisTag,
					attempt + 1,
					(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
					M576Peak1DWhy(outCode),
					M576::SweepTrendName(profile.trend),
					profile.span,
					prevRange,
					attemptDacRange);
			}
			else if (plan.action == M576::SweepRetryAction::FineRefine)
			{
				msg.Format(
					_T("  %hs %s %s attempt %d/%d code=%hs trend=%hs argmax=%d t*=%.4g span=%.4g offset %d->%d base %d->%d"),
					M576::SweepRetryActionLogTag(plan.action),
					recalStageLabel,
					axisTag,
					attempt + 1,
					(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
					M576Peak1DWhy(outCode),
					M576::SweepTrendName(profile.trend),
					profile.argmaxIndex,
					outTPeak,
					profile.span,
					prevRange,
					attemptDacRange,
					prevBase,
					movingBase);
			}
			else
			{
				const M576::SweepProfile recenterProfile =
					M576::AdjustProfileForMonoRecenter(profile, outPow, retryState.inCoarsePhase, &failInfo);
				const double deltaDac = M576::SuggestSweepRecenterDeltaDac(
					recenterProfile, n, attemptDacRange, attempt, failInfo);
				msg.Format(
					_T("  %hs %s %s attempt %d/%d code=%hs trend=%hs argmax=%d t*=%.4g span=%.4g col0=%.4g offset=%d deltaDac=%.4g newMoving=%d fixed=%d (center=%.4g moving=%d->%d)%s"),
					M576::SweepRetryActionLogTag(plan.action),
					recalStageLabel,
					axisTag,
					attempt + 1,
					(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
					M576Peak1DWhy(outCode),
					M576::SweepTrendName(recenterProfile.trend),
					recenterProfile.argmaxIndex,
					outTPeak,
					recenterProfile.span,
					outCol0,
					attemptDacRange,
					deltaDac,
					movingBase,
					fixedBaseDac,
					centerDac,
					prevBase,
					movingBase,
					(recenterProfile.trend != profile.trend) ? _T(" postCoarse") : _T(""));
			}
			SafeAppendLog(msg);
		}
		retryState.prevArgmax = profile.argmaxIndex;
	}
	return FALSE;
}

void M576AppendPeakFitTraceLog(CM576CalibratorDlg* dlg, const TCHAR* stageTag, const M576::Peak1DFitTrace& tr)
{
	if (!dlg)
		return;
	if (tr.globalMaxIndex >= 0)
	{
		CString line;
		line.Format(_T("  %s全局最大点: 下标=%d 功率=%.12g"), stageTag, tr.globalMaxIndex, tr.globalMaxY);
		dlg->SafeAppendLog(line);
	}
	const size_t nf = tr.fitIndex.size();
	if (nf == 0 || tr.fitY.size() != nf)
	{
		CString line;
		line.Format(_T("  %s拟合点: (无，预处理未产出)"), stageTag);
		dlg->SafeAppendLog(line);
		return;
	}
	const int head = (int)M576_PEAK1D_LOG_FIT_FIRST;
	const int tail = (int)M576_PEAK1D_LOG_FIT_LAST;
	CString seg;
	auto appendOne = [&](size_t k, bool withSep)
	{
		CString one;
		one.Format(_T("%s[%d]=%.8g"), withSep ? _T(";") : _T(""), tr.fitIndex[k], tr.fitY[k]);
		seg += one;
	};
	if ((int)nf <= head + tail)
	{
		for (size_t k = 0; k < nf; ++k)
			appendOne(k, k != 0);
	}
	else
	{
		for (int k = 0; k < head; ++k)
			appendOne((size_t)k, k != 0);
		seg += _T(" ... ");
		for (size_t k = nf - (size_t)tail; k < nf; ++k)
			appendOne(k, true);
	}
	CString full;
	full.Format(_T("  %s拟合点(共%zu个): "), stageTag, nf);
	full += seg;
	dlg->SafeAppendLog(full);
}

// --- 构造与 DDX：默认 CSV/LUT/日志路径、定标步参与 PM 模式初值 ---

CM576CalibratorDlg::CM576CalibratorDlg(CWnd* pParent)
	: CDialogEx(IDD, pParent)
	, m_bStop(FALSE)
	, m_nCalMode(0)
	, m_delayMs(M576_DEFAULT_RECAL_DELAY_MS)
	, m_dacRange(M576_DEFAULT_DAC_RANGE)
	, m_dacStep(M576_DEFAULT_DAC_STEP)
	, m_tlsIndex(M576_DEFAULT_TLS_SOURCE - 1)
	, m_strWavelength(_T("1310"))
	, m_pmRangeIndex(M576_DEFAULT_PM_RANGE)
	, m_sessionCalibPolicy(M576CalibBinWritePolicy::Slot1310Low)
	, m_sessionCalibWavelengthNm(M576_DEFAULT_WAVELENGTH_NM)
	, m_readBackupLastOk(FALSE)
	, m_readSnLastOk(FALSE)
	, m_burnFlashLastOk(FALSE)
	, m_burnFlashLastPartial(FALSE)
	, m_burnFlashLastRecover(FALSE)
{
	for (int i = 0; i < 4; ++i)
	{
		m_strCsvPm[i] = g_m576DefaultPmCsvRel[i];
		m_strCsvPd[i] = g_m576DefaultPdCsvRel[i];
	}
	m_strOutBin     = kM576FixedOutBinRel;
	m_strBackupBin  = kM576FixedBackupBinRel;
	m_strCommLogPath = _T("output\\comm.log");
	for (int m = 0; m < 2; ++m)
		m_snInfo.mcsSn[m].Empty();
	for (int d = 0; d < 2; ++d)
	{
		for (int s = 0; s < 4; ++s)
			m_snInfo.oneX64Sn[d][s].Empty();
	}
}

// 控件与 PM/PD 单选、路径框绑定等。
void CM576CalibratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM, m_comboCom);
	DDX_Control(pDX, IDC_COMBO_TLS, m_comboTls);
	DDX_Control(pDX, IDC_COMBO_WAVELENGTH, m_comboWavelength);
	DDX_Control(pDX, IDC_COMBO_PM_RANGE, m_comboPmRange);
	DDX_CBIndex(pDX, IDC_COMBO_TLS, m_tlsIndex);
	DDV_MinMaxInt(pDX, m_tlsIndex, 0, M576_MAX_TLS_SOURCE - 1);
	DDX_CBString(pDX, IDC_COMBO_WAVELENGTH, m_strWavelength);
	DDX_CBIndex(pDX, IDC_COMBO_PM_RANGE, m_pmRangeIndex);
	DDV_MinMaxInt(pDX, m_pmRangeIndex, M576_MIN_PM_RANGE, M576_MAX_PM_RANGE);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
	DDX_Control(pDX, IDC_PROGRESS_MAIN, m_progress);
	DDX_Text(pDX, IDC_EDIT_BACKUP_BIN, m_strBackupBin);
	DDX_Text(pDX, IDC_EDIT_OUT_BIN, m_strOutBin);
	DDX_Text(pDX, IDC_EDIT_MCS1_SN, m_snInfo.mcsSn[0]);
	DDX_Text(pDX, IDC_EDIT_MCS2_SN, m_snInfo.mcsSn[1]);
	DDX_Text(pDX, IDC_EDIT_1X641_SW1_SN, m_snInfo.oneX64Sn[0][0]);
	DDX_Text(pDX, IDC_EDIT_1X641_SW2_SN, m_snInfo.oneX64Sn[0][1]);
	DDX_Text(pDX, IDC_EDIT_1X641_SW3_SN, m_snInfo.oneX64Sn[0][2]);
	DDX_Text(pDX, IDC_EDIT_1X641_SW4_SN, m_snInfo.oneX64Sn[0][3]);
	DDX_Text(pDX, IDC_EDIT_1X642_SW1_SN, m_snInfo.oneX64Sn[1][0]);
	DDX_Text(pDX, IDC_EDIT_1X642_SW2_SN, m_snInfo.oneX64Sn[1][1]);
	DDX_Text(pDX, IDC_EDIT_1X642_SW3_SN, m_snInfo.oneX64Sn[1][2]);
	DDX_Text(pDX, IDC_EDIT_1X642_SW4_SN, m_snInfo.oneX64Sn[1][3]);
	DDX_Radio(pDX, IDC_RADIO_CAL_PM, m_nCalMode);
	DDX_Text(pDX, IDC_EDIT_RECAL_DELAY, m_delayMs);
	DDV_MinMaxInt(pDX, m_delayMs, M576_MIN_RECAL_DELAY_MS, M576_MAX_RECAL_DELAY_MS);
	DDX_Text(pDX, IDC_EDIT_DAC_RANGE, m_dacRange);
	DDV_MinMaxInt(pDX, m_dacRange, M576_MIN_DAC_RANGE, M576_MAX_DAC_RANGE);
	DDX_Text(pDX, IDC_EDIT_DAC_STEP, m_dacStep);
	DDV_MinMaxInt(pDX, m_dacStep, M576_MIN_DAC_STEP, M576_MAX_DAC_STEP);
}

BEGIN_MESSAGE_MAP(CM576CalibratorDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OPEN_PORTS, &CM576CalibratorDlg::OnBnClickedOpenPorts)
	ON_BN_CLICKED(IDC_BTN_TEST_CONNECTION, &CM576CalibratorDlg::OnBnClickedTestConnection)
	ON_BN_CLICKED(IDC_BTN_BURN_BOARD, &CM576CalibratorDlg::OnBnClickedBurnBoard)
	ON_BN_CLICKED(IDC_BTN_CLOSE_PORT, &CM576CalibratorDlg::OnBnClickedClosePort)
	ON_BN_CLICKED(IDC_BTN_BROWSE_BACKUP, &CM576CalibratorDlg::OnBnClickedBrowseBackup)
	ON_BN_CLICKED(IDC_BTN_BROWSE_OUT, &CM576CalibratorDlg::OnBnClickedBrowseOut)
	ON_BN_CLICKED(IDC_BTN_READ_FLASH_BACKUP, &CM576CalibratorDlg::OnBnClickedReadFlashBackup)
	ON_BN_CLICKED(IDC_RADIO_CAL_PM, &CM576CalibratorDlg::OnBnClickedCalPm)
	ON_BN_CLICKED(IDC_RADIO_CAL_PD, &CM576CalibratorDlg::OnBnClickedCalPd)
	ON_BN_CLICKED(IDC_BTN_RUN_PATH, &CM576CalibratorDlg::OnBnClickedRunPath)
	ON_BN_CLICKED(IDC_BTN_CLEAR_LOG, &CM576CalibratorDlg::OnBnClickedClearLog)
	ON_BN_CLICKED(IDC_BTN_GEN_BIN, &CM576CalibratorDlg::OnBnClickedGenBin)
	ON_BN_CLICKED(IDC_BTN_MAKE_BIN, &CM576CalibratorDlg::OnBnClickedMakeBin)
	ON_BN_CLICKED(IDC_BTN_READ_ALL_SN, &CM576CalibratorDlg::OnBnClickedReadAllSn)
	ON_BN_CLICKED(IDC_BTN_FLASH, &CM576CalibratorDlg::OnBnClickedFlash)
	ON_BN_CLICKED(IDC_BTN_RECOVER_FLASH, &CM576CalibratorDlg::OnBnClickedRecoverFlash)
	ON_BN_CLICKED(IDC_BTN_STOP, &CM576CalibratorDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_BTN_RUN_DIAG, &CM576CalibratorDlg::OnBnClickedRunDiag)
	ON_BN_CLICKED(IDC_BTN_STOP_DIAG, &CM576CalibratorDlg::OnBnClickedStopDiag)
	ON_BN_CLICKED(IDC_BTN_EXPORT_CALIB_STATS, &CM576CalibratorDlg::OnBnClickedExportCalibStats)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_M576_PATH_LOG_FLUSH, &CM576CalibratorDlg::OnPathLogFlush)
	ON_MESSAGE(WM_M576_PATH_PROGRESS_RANGE, &CM576CalibratorDlg::OnPathProgressRange)
	ON_MESSAGE(WM_M576_PATH_PROGRESS_POS, &CM576CalibratorDlg::OnPathProgressPos)
	ON_MESSAGE(WM_M576_PATH_FINISHED, &CM576CalibratorDlg::OnPathFinished)
	ON_MESSAGE(WM_M576_READ_BACKUP_FINISHED, &CM576CalibratorDlg::OnReadBackupFinished)
	ON_MESSAGE(WM_M576_READ_SN_FINISHED, &CM576CalibratorDlg::OnReadAllSnFinished)
	ON_MESSAGE(WM_M576_BURN_FLASH_FINISHED, &CM576CalibratorDlg::OnBurnFlashFinished)
	ON_MESSAGE(WM_M576_BURN_BOARD_FINISHED, &CM576CalibratorDlg::OnBurnBoardFinished)
	ON_MESSAGE(WM_M576_DIAG_FINISHED, &CM576CalibratorDlg::OnDiagFinished)
END_MESSAGE_MAP()

BOOL CM576CalibratorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ModifyStyle(WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);
	if (CMenu* pSys = GetSystemMenu(FALSE))
	{
		pSys->RemoveMenu(SC_MINIMIZE, MF_BYCOMMAND);
		pSys->RemoveMenu(SC_MAXIMIZE, MF_BYCOMMAND);
	}
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if (hIcon)
	{
		SetIcon(hIcon, TRUE);
		SetIcon(hIcon, FALSE);
	}
	SetWindowText(M576_APP_WINDOW_TITLE);
	::SetDlgItemText(m_hWnd, IDC_GROUP_CONN, _T("Connection"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_PATHS, _T("Config"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_ACTIONS, _T("Actions"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_LOG, _T("Log"));
	::SetDlgItemText(m_hWnd, IDC_STATIC_LABEL_COM, _T("Port (439F):"));
	::SetDlgItemText(m_hWnd, IDC_BTN_FLASH, _T("Burn Flash"));
	::SetDlgItemText(m_hWnd, IDC_BTN_READ_FLASH_BACKUP, _T("Read Flash Backup"));
	::SetDlgItemText(m_hWnd, IDC_BTN_RECOVER_FLASH, _T("Recover Flash"));
	::SetDlgItemText(m_hWnd, IDC_STATIC_LABEL_MODE, _T("Mode:"));
	{
		CString hint;
		hint.Format(_T("%d-%d"), M576_MIN_RECAL_DELAY_MS, M576_MAX_RECAL_DELAY_MS);
		::SetDlgItemText(m_hWnd, IDC_STATIC_RECAL_DELAY_HINT, hint);
		hint.Format(_T("%d-%d"), M576_MIN_DAC_RANGE, M576_MAX_DAC_RANGE);
		::SetDlgItemText(m_hWnd, IDC_STATIC_DAC_RANGE_HINT, hint);
		hint.Format(_T("%d-%d"), M576_MIN_DAC_STEP, M576_MAX_DAC_STEP);
		::SetDlgItemText(m_hWnd, IDC_STATIC_DAC_STEP_HINT, hint);
	}
	EnsureOutputFolderUnderExe(GetExeFolder());
	ApplyFixedBinBasePaths(FALSE);
	SyncCsvPathWithMode();
	// RECAL 0 combos: must list items before UpdateData(FALSE) (DDX_CBIndex).
	if (CComboBox* pTls = (CComboBox*)GetDlgItem(IDC_COMBO_TLS))
	{
		pTls->ResetContent();
		for (int i = M576_MIN_TLS_SOURCE; i <= M576_MAX_TLS_SOURCE; ++i)
		{
			CString s;
			s.Format(_T("%d"), i);
			pTls->AddString(s);
		}
		pTls->SetCurSel(m_tlsIndex);
	}
	if (CComboBox* pPm = (CComboBox*)GetDlgItem(IDC_COMBO_PM_RANGE))
	{
		pPm->ResetContent();
		for (int r = M576_MIN_PM_RANGE; r <= M576_MAX_PM_RANGE; ++r)
		{
			CString s;
			s.Format(_T("%d"), r);
			pPm->AddString(s);
		}
		pPm->SetCurSel(m_pmRangeIndex);
	}
	if (CComboBox* pWl = (CComboBox*)GetDlgItem(IDC_COMBO_WAVELENGTH))
	{
		pWl->ResetContent();
		pWl->AddString(_T("1310"));
		pWl->AddString(_T("1550"));
		pWl->SetWindowText(m_strWavelength);
	}
	UpdateData(FALSE);
	if (CWnd* p = GetDlgItem(IDC_EDIT_BACKUP_BIN))
		::SendMessage(p->m_hWnd, EM_SETREADONLY, TRUE, 0);
	if (CWnd* p = GetDlgItem(IDC_EDIT_OUT_BIN))
		::SendMessage(p->m_hWnd, EM_SETREADONLY, TRUE, 0);
	if (CWnd* p = GetDlgItem(IDC_BTN_BROWSE_BACKUP))
		p->EnableWindow(FALSE);
	if (CWnd* p = GetDlgItem(IDC_BTN_BROWSE_OUT))
		p->EnableWindow(FALSE);
	SyncRecal0ControlsVisibility();
	FillComPorts();
	SyncSerialPortUi();
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	for (int li = 0; li < 4; ++li)
		ZeroMemory(&m_lutByTrans[li], sizeof(m_lutByTrans[li]));
	ZeroMemory(m_mems1x64, sizeof(m_mems1x64));
	AppendLog(_T("Ready. Select 439F COM port, open port, then run."));
	AppendLog(
		_T("Backup BIN: Read Flash writes {SN}_backup.bin to output\\latest\\; archives under output\\archive\\; run Read All SN first."));
	AppendLog(
		_T("BIN workspace is output\\latest\\; legacy bins in output\\ root are not used — re-read Flash or move files into latest\\."));
	AppendLog(_T("Path CSV: built-in output\\pm_*.csv (PM) or pd_*.csv (PD); missing file skips that trans slot."));
	AppendLog(_T("PM: RECAL 0 + RECAL 1 + RECAL 3; PD: RECAL 2 + RECAL 5 (no RECAL 0)."));
	{
		const CString cfgLine = M576GetAppConfigLogLine();
		if (!cfgLine.IsEmpty())
			AppendLog(cfgLine);
	}
	SyncExportStatsButton();
	return TRUE;
}

void CM576CalibratorDlg::ApplyFixedBinBasePaths(BOOL syncUi)
{
	m_strBackupBin = kM576FixedBackupBinRel;
	m_strOutBin = kM576FixedOutBinRel;
	if (syncUi && m_hWnd && ::IsWindow(m_hWnd))
	{
		SetDlgItemText(IDC_EDIT_BACKUP_BIN, m_strBackupBin);
		SetDlgItemText(IDC_EDIT_OUT_BIN, m_strOutBin);
	}
}

CString CM576CalibratorDlg::ResolveBinOutputDirAbs() const
{
	return M576ResolveLatestBinDirAbs(GetExeFolder());
}

void CM576CalibratorDlg::EnsureOutputDirTree()
{
	EnsureOutputFolderUnderExe(GetExeFolder());
}

void CM576CalibratorDlg::BeginArchiveSession()
{
	EnsureOutputDirTree();
	if (!m_archiveSessionDirAbs.IsEmpty())
		AppendLog(_T("Archive session rotated (new Read SN)."));
	m_archiveSessionId = M576BuildSessionFolderName(m_snInfo);
	m_archiveSessionDirAbs = M576ResolveArchiveRootAbs(GetExeFolder()) + m_archiveSessionId;
	m_archiveStages.clear();

	CString err;
	CString logsDir;
	logsDir.Format(_T("%s\\logs"), m_archiveSessionDirAbs.GetString());
	if (!M576EnsureDirTree(m_archiveSessionDirAbs, err))
	{
		CString m;
		m.Format(_T("Archive session: mkdir failed: %s"), err.GetString());
		AppendLog(m);
		return;
	}
	(void)M576EnsureDirTree(logsDir, err);

	M576ArchiveStageEntry st;
	st.stageName = _T("read_sn");
	st.utcIso = M576FormatUtcIso8601Z();
	st.filesCopied = 0;
	m_archiveStages.push_back(st);

	CString metaErr;
	if (!M576WriteSessionMeta(
			m_archiveSessionDirAbs,
			m_archiveSessionId,
			m_snInfo,
			GetComboCom(),
			m_archiveStages,
			metaErr))
	{
		CString m;
		m.Format(_T("Archive session: meta.json failed: %s"), metaErr.GetString());
		AppendLog(m);
		return;
	}
	CString ok;
	ok.Format(_T("Archive session started: %s"), m_archiveSessionDirAbs.GetString());
	AppendLog(ok);
}

void CM576CalibratorDlg::ArchiveCurrentBinSet(
	LPCTSTR subFolder,
	M576BinFileRole role,
	LPCTSTR stageTag,
	BOOL includeDacCsv)
{
	if (m_archiveSessionDirAbs.IsEmpty())
	{
		AppendLog(_T("Archive skipped: no session (Read All SN first)."));
		return;
	}
	if (subFolder == NULL || subFolder[0] == 0 || stageTag == NULL || stageTag[0] == 0)
		return;

	CString destSub;
	destSub.Format(_T("%s\\%s"), m_archiveSessionDirAbs.GetString(), subFolder);
	const CString latestDir = ResolveBinOutputDirAbs();
	CString err;
	int filesCopied = 0;
	if (!M576ArchiveCopyBinSet(latestDir, destSub, m_snInfo, role, includeDacCsv, filesCopied, err))
	{
		CString m;
		m.Format(_T("Archive warn (%s): %s"), stageTag, err.GetString());
		AppendLog(m);
		return;
	}

	CString logsDir;
	logsDir.Format(_T("%s\\logs"), m_archiveSessionDirAbs.GetString());
	const CString commAbs = ResolveFilePath(CommLogPathForCurrentDay(m_strCommLogPath));
	CString copiedComm;
	CString commErr;
	if (!M576ArchiveCopyCommLogSnapshot(logsDir, commAbs, copiedComm, commErr))
	{
		CString m;
		m.Format(_T("Archive warn (%s) comm snapshot: %s"), stageTag, commErr.GetString());
		AppendLog(m);
	}

	M576ArchiveStageEntry st;
	st.stageName = stageTag;
	st.utcIso = M576FormatUtcIso8601Z();
	st.filesCopied = filesCopied;
	m_archiveStages.push_back(st);

	CString metaErr;
	if (!M576WriteSessionMeta(
			m_archiveSessionDirAbs,
			m_archiveSessionId,
			m_snInfo,
			GetComboCom(),
			m_archiveStages,
			metaErr))
	{
		CString m;
		m.Format(_T("Archive warn (%s) meta.json: %s"), stageTag, metaErr.GetString());
		AppendLog(m);
	}

	CString ok;
	ok.Format(_T("Archive %s: %d file(s) -> %s"), stageTag, filesCopied, destSub.GetString());
	AppendLog(ok);
}

BOOL CM576CalibratorDlg::ValidateSnBeforeBinOp(CString& errMsg) const
{
	return M576ValidateSnInfoForBinOps(m_snInfo, errMsg);
}

CString CM576CalibratorDlg::BuildSessionDacCsvPath(M576CalibBinWritePolicy policy, M576BinFileRole role) const
{
	const CString outDir = ResolveBinOutputDirAbs();
	CString sn = M576SanitizeSnForFilename(m_snInfo.mcsSn[0]);
	if (sn.IsEmpty())
		sn = _T("unknown");
	LPCTSTR leaf = nullptr;
	if (policy == M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh)
		leaf = (role == M576BinFileRole::Backup) ? _T("backupAll1550DAC.csv") : _T("standardAll1550DAC.csv");
	else
		leaf = (role == M576BinFileRole::Backup) ? _T("backupAll1310DAC.csv") : _T("standardAll1310DAC.csv");
	CString path;
	path.Format(_T("%s\\%s_%s"), outDir.GetString(), sn.GetString(), leaf);
	return path;
}

void CM576CalibratorDlg::LogBurnFilePaths(
	CM576CalibratorDlg* dlg,
	const std::array<CString, M576_BURN_FILE_COUNT>& paths,
	LPCTSTR roleLabel)
{
	if (!dlg)
		return;
	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
	{
		CString line;
		line.Format(_T("  [%d] %s: %s"), i, roleLabel, paths[i].GetString());
		dlg->SafeAppendLog(line);
	}
}

void CM576CalibratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_MINIMIZE)
		return;
	CDialogEx::OnSysCommand(nID, lParam);
}

void CM576CalibratorDlg::FillComPorts()
{
	m_comboCom.ResetContent();
	std::vector<CString> ports;
	EnumPresentComPorts(ports);
	for (const CString& p : ports)
		m_comboCom.AddString(p);
	if (m_comboCom.GetCount() > 0)
		m_comboCom.SetCurSel(0);
}

void CM576CalibratorDlg::AppendLog(LPCTSTR sz)
{
	CString line;
	line.Format(_T("%s %s"), FormatLogTimestamp().GetString(), sz);
	HWND hEd = m_editLog.GetSafeHwnd();
	CStringW t;
	if (hEd)
	{
		const int cch = ::GetWindowTextLengthW(hEd);
		if (cch > 0)
		{
			::GetWindowTextW(hEd, t.GetBuffer(cch + 1), cch + 1);
			t.ReleaseBuffer();
		}
	}
	M576TrimEditLogTextW(t);
	if (!t.IsEmpty())
		t += L"\r\n";
	t += M576NarrowToWideForUi(line);
	if (hEd)
		::SetWindowTextW(hEd, t);
	int n = hEd ? (int)::GetWindowTextLengthW(hEd) : m_editLog.GetWindowTextLength();
	m_editLog.SetSel(n, n);
	WriteLogFileLine(line);
}

int CM576CalibratorDlg::MessageBoxM576(const CString& text, UINT nType, const wchar_t* pTitle)
{
	const CStringW w = M576NarrowToWideForUi(text);
	return (int)::MessageBoxW(
		m_hWnd,
		w,
		(pTitle && pTitle[0] != L'\0') ? pTitle : L"M576CalibratorApp",
		nType);
}

int CM576CalibratorDlg::MessageBoxM576(LPCTSTR text, UINT nType, const wchar_t* pTitle)
{
	return MessageBoxM576(CString(text), nType, pTitle);
}

void CM576CalibratorDlg::SafeAppendLog(LPCTSTR sz)
{
	if (!m_hWnd || !::IsWindow(m_hWnd))
		return;
	const DWORD tidWnd = GetWindowThreadProcessId(m_hWnd, NULL);
	if (GetCurrentThreadId() == tidWnd)
	{
		AppendLog(sz);
		return;
	}
	CString line;
	line.Format(_T("%s %s"), FormatLogTimestamp().GetString(), sz);
	{
		std::lock_guard<std::mutex> lock(m_pathLogQueueMutex);
		if (!m_queuedPathLog.IsEmpty())
			m_queuedPathLog += _T("\r\n");
		m_queuedPathLog += line;
	}
	if (!m_pathLogFlushScheduled.exchange(true))
		::PostMessage(m_hWnd, WM_M576_PATH_LOG_FLUSH, 0, 0);
}

void CM576CalibratorDlg::SafeSetProgressRange(int minVal, int maxVal)
{
	if (!m_hWnd || !::IsWindow(m_hWnd))
		return;
	const DWORD tidWnd = GetWindowThreadProcessId(m_hWnd, NULL);
	if (GetCurrentThreadId() != tidWnd && m_suppressPathProgress.load())
		return;
	if (GetCurrentThreadId() == tidWnd)
	{
		m_progress.SetRange(minVal, maxVal);
		return;
	}
	::PostMessage(m_hWnd, WM_M576_PATH_PROGRESS_RANGE, static_cast<WPARAM>(minVal), static_cast<LPARAM>(maxVal));
}

void CM576CalibratorDlg::SafeSetProgressPos(int pos)
{
	if (!m_hWnd || !::IsWindow(m_hWnd))
		return;
	const DWORD tidWnd = GetWindowThreadProcessId(m_hWnd, NULL);
	if (GetCurrentThreadId() != tidWnd && m_suppressPathProgress.load())
		return;
	if (GetCurrentThreadId() == tidWnd)
	{
		m_progress.SetPos(pos);
		return;
	}
	::PostMessage(m_hWnd, WM_M576_PATH_PROGRESS_POS, static_cast<WPARAM>(pos), 0);
}

void CM576CalibratorDlg::SetPathActionButtonsEnabled(BOOL enable)
{
	if (CWnd* p = GetDlgItem(IDC_BTN_RUN_PATH))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_GEN_BIN))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_MAKE_BIN))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_FLASH))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_RECOVER_FLASH))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_READ_FLASH_BACKUP))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_READ_ALL_SN))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_STOP))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_RUN_DIAG))
		p->EnableWindow(enable && !m_diagRunning.load());
	if (enable)
		SyncExportStatsButton();
	else if (CWnd* p = GetDlgItem(IDC_BTN_EXPORT_CALIB_STATS))
		p->EnableWindow(FALSE);
	SyncSerialPortUi();
}

void CM576CalibratorDlg::ClearCalibStats()
{
	std::lock_guard<std::mutex> lock(m_statsRowsMutex);
	m_statsRows.clear();
}

void CM576CalibratorDlg::ClearPathOutcomes()
{
	std::lock_guard<std::mutex> lock(m_pathOutcomesMutex);
	m_pathFailureOutcomes.clear();
}

void CM576CalibratorDlg::PushPathFailureOutcome(const SCalibPathStepOutcome& o)
{
	std::lock_guard<std::mutex> lock(m_pathOutcomesMutex);
	m_pathFailureOutcomes.push_back(o);
}

void CM576CalibratorDlg::ShowRunPathSummaryDialog(BOOL userStopped)
{
	std::vector<SCalibPathStepOutcome> failures;
	int successCount = 0;
	{
		std::lock_guard<std::mutex> lockOut(m_pathOutcomesMutex);
		failures = m_pathFailureOutcomes;
	}
	{
		std::lock_guard<std::mutex> lockStats(m_statsRowsMutex);
		successCount = (int)m_statsRows.size();
	}
	const SRunPathSummary summary = BuildRunPathSummary(
		failures, successCount, m_nCalMode == 0, userStopped != FALSE);
	CM576RunPathSummaryDlg dlg(summary, this);
	dlg.DoModal();
}

void CM576CalibratorDlg::PushCalibStatRow(const SCalibrationStatRow& r)
{
	std::lock_guard<std::mutex> lock(m_statsRowsMutex);
	m_statsRows.push_back(r);
}

void CM576CalibratorDlg::SyncExportStatsButton()
{
	if (!m_hWnd || !::IsWindow(m_hWnd))
		return;
	std::lock_guard<std::mutex> lock(m_statsRowsMutex);
	const BOOL en = !m_statsRows.empty() && !m_pathRunning.load();
	if (CWnd* p = GetDlgItem(IDC_BTN_EXPORT_CALIB_STATS))
		p->EnableWindow(en);
}

BOOL CM576CalibratorDlg::IsSerialPortOpen() const
{
	const HANDLE h = m_dev429f.GetPortHandle();
	return h != NULL && h != INVALID_HANDLE_VALUE;
}

void CM576CalibratorDlg::SyncSerialPortUi()
{
	if (!m_hWnd || !::IsWindow(m_hWnd))
		return;
	const BOOL open = IsSerialPortOpen();
	const BOOL busy = m_pathRunning.load() || m_readBackupRunning.load() || m_readSnRunning.load()
		|| m_burnFlashRunning.load() || m_burnBoardRunning.load() || m_diagRunning.load();
	if (CWnd* p = GetDlgItem(IDC_BTN_OPEN_PORTS))
		p->EnableWindow(!open && !busy);
	if (CWnd* p = GetDlgItem(IDC_BTN_CLOSE_PORT))
		p->EnableWindow(open && !busy);
	if (CWnd* p = GetDlgItem(IDC_COMBO_COM))
		p->EnableWindow(!open);
	if (CWnd* p = GetDlgItem(IDC_BTN_TEST_CONNECTION))
		p->EnableWindow(!busy);
	if (CWnd* p = GetDlgItem(IDC_BTN_BURN_BOARD))
		p->EnableWindow(!busy);
	if (CWnd* p = GetDlgItem(IDC_BTN_RUN_DIAG))
		p->EnableWindow(!busy);
	if (CWnd* p = GetDlgItem(IDC_BTN_STOP_DIAG))
		p->EnableWindow(m_diagRunning.load());
}

// ---------- UI 线程：处理工作线程 Post 的日志/进度/完成消息 ----------
LRESULT CM576CalibratorDlg::OnPathLogFlush(WPARAM, LPARAM)
{
	CString batch;
	{
		std::lock_guard<std::mutex> lock(m_pathLogQueueMutex);
		batch = m_queuedPathLog;
		m_queuedPathLog.Empty();
	}
	if (batch.IsEmpty())
	{
		m_pathLogFlushScheduled = false;
		{
			std::lock_guard<std::mutex> lock(m_pathLogQueueMutex);
			if (!m_queuedPathLog.IsEmpty() && !m_pathLogFlushScheduled.exchange(true))
				::PostMessage(m_hWnd, WM_M576_PATH_LOG_FLUSH, 0, 0);
		}
		return 0;
	}
	CStringW t;
	HWND hEd = m_editLog.GetSafeHwnd();
	if (hEd)
	{
		const int cch0 = ::GetWindowTextLengthW(hEd);
		if (cch0 > 0)
		{
			::GetWindowTextW(hEd, t.GetBuffer(cch0 + 1), cch0 + 1);
			t.ReleaseBuffer();
		}
	}
	M576TrimEditLogTextW(t);
	if (!t.IsEmpty())
		t += L"\r\n";
	t += M576NarrowToWideForUi(batch);
	if (hEd)
		::SetWindowTextW(hEd, t);
	int n = hEd ? (int)::GetWindowTextLengthW(hEd) : m_editLog.GetWindowTextLength();
	m_editLog.SetSel(n, n);
	{
		int pos = 0;
		for (;;)
		{
			const int nl = batch.Find(_T("\r\n"), pos);
			if (nl < 0)
			{
				if (pos < batch.GetLength())
					WriteLogFileLine(batch.Mid(pos));
				break;
			}
			if (nl > pos)
				WriteLogFileLine(batch.Mid(pos, nl - pos));
			pos = nl + 2;
		}
	}
	m_pathLogFlushScheduled = false;
	{
		std::lock_guard<std::mutex> lock(m_pathLogQueueMutex);
		if (!m_queuedPathLog.IsEmpty() && !m_pathLogFlushScheduled.exchange(true))
			::PostMessage(m_hWnd, WM_M576_PATH_LOG_FLUSH, 0, 0);
	}
	return 0;
}

LRESULT CM576CalibratorDlg::OnPathProgressRange(WPARAM wParam, LPARAM lParam)
{
	m_progress.SetRange(static_cast<int>(wParam), static_cast<int>(lParam));
	return 0;
}

LRESULT CM576CalibratorDlg::OnPathProgressPos(WPARAM wParam, LPARAM)
{
	m_progress.SetPos(static_cast<int>(wParam));
	return 0;
}

LRESULT CM576CalibratorDlg::OnPathFinished(WPARAM, LPARAM)
{
	if (m_pathThread.joinable())
		m_pathThread.join();
	m_pathRunning = false;
	m_suppressPathProgress = false;
	SetPathActionButtonsEnabled(TRUE);
	const BOOL stopped = m_bStop;
	m_bStop = FALSE;
	if (m_pathShowFinishInfoBox)
	{
		if (stopped)
			AppendLog(_T("Run Path stopped by user."));
		ShowRunPathSummaryDialog(stopped);
	}
	m_pathShowFinishInfoBox = TRUE;
	return 0;
}

LRESULT CM576CalibratorDlg::OnReadBackupFinished(WPARAM, LPARAM)
{
	if (m_readBackupThread.joinable())
		m_readBackupThread.join();
	m_readBackupRunning = false;
	SetPathActionButtonsEnabled(TRUE);
	UpdateData(FALSE);
	if (m_readBackupLastOk)
	{
		ArchiveCurrentBinSet(_T("backup"), M576BinFileRole::Backup, _T("read_flash"), TRUE);
		MessageBoxM576(m_readBackupLastMsg, MB_OK | MB_ICONINFORMATION);
	}
	else
		MessageBoxM576(m_readBackupLastMsg, MB_OK | MB_ICONERROR);
	return 0;
}

LRESULT CM576CalibratorDlg::OnReadAllSnFinished(WPARAM, LPARAM)
{
	if (m_readSnThread.joinable())
		m_readSnThread.join();
	m_readSnRunning = false;
	SetPathActionButtonsEnabled(TRUE);
	if (m_readSnLastOk)
	{
		m_snInfo = m_readSnLastValues;
		UpdateData(FALSE);
		BeginArchiveSession();
		AppendLog(
			_T("Read SN: MCS trans1-2 = GetProductSN (0xA2); 1x64 trans3-4 = 4x mem (ADDR_SWITCHn_COEF+0x7E0, 16 B SN)."));
		for (int m = 0; m < 2; ++m)
		{
			CString line;
			line.Format(
				_T("  trans %d: SN=%s"),
				m + 1,
				m_snInfo.mcsSn[m].GetString());
			AppendLog(line);
		}
		for (int d = 0; d < 2; ++d)
		{
			const int tch = d + 3;
			for (int sw = 0; sw < 4; ++sw)
			{
				CString line;
				line.Format(
					_T("  trans %d sw%d: SN=%s"),
					tch,
					sw + 1,
					m_snInfo.oneX64Sn[d][sw].GetString());
				AppendLog(line);
			}
		}
	}
	else
	{
		CString oneLine;
		oneLine.Format(_T("Read SN failed: %s"), m_readSnLastMsg.GetString());
		AppendLog(oneLine);
		CString box;
		box.Format(_T("Read SN (trans 1-4) failed:\n\n%s"), m_readSnLastMsg.GetString());
		MessageBoxM576(box, MB_OK | MB_ICONERROR);
	}
	return 0;
}

LRESULT CM576CalibratorDlg::OnBurnFlashFinished(WPARAM, LPARAM)
{
	if (m_burnFlashThread.joinable())
		m_burnFlashThread.join();
	m_burnFlashRunning = false;
	SetPathActionButtonsEnabled(TRUE);
	if (m_burnFlashLastOk)
	{
		if (m_burnFlashLastRecover)
		{
			AppendLog(_T("Recover Flash completed: trans1-2 via MCS update stream, trans3-4 via 1x64 XMODEM (per selected file path)."));
			if (m_burnFlashLastPartial)
				AppendLog(_T("Partial recover: only the selected file entries were programmed."));
			MessageBoxM576(
				_T("Recover Flash completed successfully.\n\nSelected MCS/1x64 files were restored to device flash."),
				MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			AppendLog(_T("Flash completed: trans1-2 via MCS update stream, trans3-4 via 1x64 XMODEM (per-file from output base)."));
			if (m_burnFlashLastPartial)
				AppendLog(_T("Partial burn: only the selected per-trans .bin files were programmed."));
			MessageBoxM576(
				_T("Burn Flash completed successfully.\n\nMCS and 1x64 paths finished for the configured .bin set."),
				MB_OK | MB_ICONINFORMATION);
		}
	}
	else
	{
		CString oneLine;
		oneLine.Format(m_burnFlashLastRecover ? _T("Recover Flash failed: %s") : _T("Flash failed: %s"), m_burnFlashLastMsg.GetString());
		AppendLog(oneLine);
		CString box;
		box.Format(
			m_burnFlashLastRecover ? _T("Recover Flash failed:\n\n%s") : _T("Burn Flash failed:\n\n%s"),
			m_burnFlashLastMsg.GetString());
		MessageBoxM576(box, MB_OK | MB_ICONERROR);
	}
	return 0;
}

LRESULT CM576CalibratorDlg::OnBurnBoardFinished(WPARAM, LPARAM)
{
	if (m_burnBoardThread.joinable())
		m_burnBoardThread.join();
	m_burnBoardRunning = false;
	SetPathActionButtonsEnabled(TRUE);
	if (m_burnBoardLastOk)
	{
		AppendLog(m_burnBoardLastMsg);
		MessageBoxM576(m_burnBoardLastMsg, MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		CString oneLine;
		oneLine.Format(_T("Burn Board failed: %s"), m_burnBoardLastMsg.GetString());
		AppendLog(oneLine);
		CString box;
		box.Format(_T("Burn Board failed:\n\n%s"), m_burnBoardLastMsg.GetString());
		MessageBoxM576(box, MB_OK | MB_ICONERROR);
	}
	return 0;
}

// --- 定标路径后台线程：按 PM/PD 调 RunPath*，结束 Post WM_M576_PATH_FINISHED ---

void CM576CalibratorDlg::PathWorkerEntry()
{
	try
	{
		if (m_nCalMode == 0)
			RunPathPowerMeter();
		else
			RunPathPd();
	}
	catch (const std::exception& e)
	{
		CStringA a;
		a.Format("Path worker: std::exception: %s", e.what());
		SafeAppendLog(CString(a));
		M576AppendFatalLogUtf8(a.GetString());
	}
	catch (...)
	{
		SafeAppendLog(_T("Path worker: unknown C++ exception (see output\\m576_fatal.log)."));
		M576AppendFatalLogUtf8("[Path worker] unknown C++ exception");
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_PATH_FINISHED, 0, 0);
}

// --- 读 Flash 备份后台线程：McsReadLutBundleFromDevice，结果供 OnReadBackupFinished ---

void CM576CalibratorDlg::ReadFlashBackupWorkerEntry(CString absOutDir)
{
	try
	{
		SafeSetProgressRange(0, 100);
		SafeSetProgressPos(0);
		CString err;
		const M576TransSnPnInfo snSnap = m_snInfo;
		if (!McsReadLutBundleFromDevice(m_dev429f, absOutDir, err, &CM576CalibratorDlg::ProgressThunk, this, snSnap))
		{
			m_readBackupLastOk = FALSE;
			m_readBackupLastMsg.Format(_T("Read Flash backup failed:\n\n%s"), (LPCTSTR)err);
			CString m;
			m.Format(_T("Read Flash backup failed: %s"), (LPCTSTR)err);
			SafeAppendLog(m);
		}
		else
		{
			m_readBackupLastOk = TRUE;
			std::array<CString, M576_BURN_FILE_COUNT> paths;
			if (M576BuildBurnFilePaths(absOutDir, snSnap, M576BinFileRole::Backup, paths, err))
			{
				LogBurnFilePaths(this, paths, _T("backup"));
				m_readBackupLastMsg.Format(
					_T("Read Flash backup finished.\n\nOutput directory:\n%s\n\n10 files: {SN}_backup.bin (see log)."),
					(LPCTSTR)absOutDir);
			}
			else
				m_readBackupLastMsg.Format(_T("Read Flash backup finished (path list: %s)."), (LPCTSTR)err);
			SafeSetProgressPos(100);
			CString ok;
			ok.Format(_T("Flash backups saved under %s ({SN}_backup.bin x10)."), (LPCTSTR)absOutDir);
			SafeAppendLog(ok);
		}
	}
	catch (const std::exception& e)
	{
		m_readBackupLastOk = FALSE;
		CStringA a;
		a.Format("Read Flash backup: std::exception: %s", e.what());
		m_readBackupLastMsg = CString(a);
		SafeAppendLog(m_readBackupLastMsg);
		M576AppendFatalLogUtf8(a.GetString());
	}
	catch (...)
	{
		m_readBackupLastOk = FALSE;
		m_readBackupLastMsg = _T("Read Flash backup: unknown C++ exception (see m576_fatal.log).");
		SafeAppendLog(m_readBackupLastMsg);
		M576AppendFatalLogUtf8("[Read Flash backup] unknown C++ exception");
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_READ_BACKUP_FINISHED, 0, 0);
}

// --- 读四路 SN 后台线程：透传读各 trans 设备序列号 ---

void CM576CalibratorDlg::ReadAllSnWorkerEntry()
{
	try
	{
		M576TransSnPnInfo sn;
		CString err;
		if (!McsReadAllTransProductSn(m_dev429f, sn, err))
		{
			m_readSnLastOk = FALSE;
			m_readSnLastMsg = err;
		}
		else
		{
			m_readSnLastOk = TRUE;
			m_readSnLastMsg.Empty();
			m_readSnLastValues = sn;
		}
	}
	catch (const std::exception& e)
	{
		m_readSnLastOk = FALSE;
		CStringA a;
		a.Format("Read all SN: std::exception: %s", e.what());
		m_readSnLastMsg = CString(a);
		SafeAppendLog(m_readSnLastMsg);
		M576AppendFatalLogUtf8(a.GetString());
	}
	catch (...)
	{
		m_readSnLastOk = FALSE;
		m_readSnLastMsg = _T("Read all SN: unknown C++ exception (see m576_fatal.log).");
		SafeAppendLog(m_readSnLastMsg);
		M576AppendFatalLogUtf8("[Read all SN] unknown C++ exception");
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_READ_SN_FINISHED, 0, 0);
}

// --- 烧录 Flash 后台线程：McsFwUploadBinEx 按勾选 mask 上载各 trans 分 bin ---

void CM576CalibratorDlg::BurnFlashWorkerEntry(
	std::array<CString, M576_BURN_FILE_COUNT> filePaths, std::array<bool, M576_BURN_FILE_COUNT> burnMask)
{
	try
	{
		CString err;
		SafeSetProgressRange(0, 100);
		SafeSetProgressPos(0);
		if (!McsFwUploadBinByPathsEx(
				m_dev429f,
				filePaths,
				err,
				&CM576CalibratorDlg::ProgressThunk,
				this,
				burnMask.data()))
		{
			m_burnFlashLastOk = FALSE;
			m_burnFlashLastMsg = err;
		}
		else
		{
			m_burnFlashLastOk = TRUE;
			m_burnFlashLastMsg.Empty();
			SafeSetProgressPos(100);
		}
	}
	catch (const std::exception& e)
	{
		m_burnFlashLastOk = FALSE;
		CStringA a;
		a.Format("Burn Flash: std::exception: %s", e.what());
		m_burnFlashLastMsg = CString(a);
		SafeAppendLog(m_burnFlashLastMsg);
		M576AppendFatalLogUtf8(a.GetString());
	}
	catch (...)
	{
		m_burnFlashLastOk = FALSE;
		m_burnFlashLastMsg = _T("Burn Flash: unknown C++ exception (see m576_fatal.log).");
		SafeAppendLog(m_burnFlashLastMsg);
		M576AppendFatalLogUtf8("[Burn Flash] unknown C++ exception");
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_BURN_FLASH_FINISHED, 0, 0);
}

// --- 从备份恢复烧录后台线程：Recover 对话框选定路径与 mask ---

void CM576CalibratorDlg::RecoverFlashWorkerEntry(
	std::array<CString, M576_BURN_FILE_COUNT> filePaths,
	std::array<bool, M576_BURN_FILE_COUNT> burnMask)
{
	try
	{
		CString err;
		SafeSetProgressRange(0, 100);
		SafeSetProgressPos(0);
		if (!McsFwUploadBinByPathsEx(
				m_dev429f,
				filePaths,
				err,
				&CM576CalibratorDlg::ProgressThunk,
				this,
				burnMask.data()))
		{
			m_burnFlashLastOk = FALSE;
			m_burnFlashLastMsg = err;
		}
		else
		{
			m_burnFlashLastOk = TRUE;
			m_burnFlashLastMsg.Empty();
			SafeSetProgressPos(100);
		}
	}
	catch (const std::exception& e)
	{
		m_burnFlashLastOk = FALSE;
		CStringA a;
		a.Format("Recover Flash: std::exception: %s", e.what());
		m_burnFlashLastMsg = CString(a);
		SafeAppendLog(m_burnFlashLastMsg);
		M576AppendFatalLogUtf8(a.GetString());
	}
	catch (...)
	{
		m_burnFlashLastOk = FALSE;
		m_burnFlashLastMsg = _T("Recover Flash: unknown C++ exception (see m576_fatal.log).");
		SafeAppendLog(m_burnFlashLastMsg);
		M576AppendFatalLogUtf8("[Recover Flash] unknown C++ exception");
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_BURN_FLASH_FINISHED, 0, 0);
}

void CM576CalibratorDlg::BurnBoardProgressThunk(int cur, int total, void* user)
{
	CM576CalibratorDlg* p = (CM576CalibratorDlg*)user;
	if (!p || !::IsWindow(p->m_hWnd))
		return;
	int pct = (total > 0) ? (cur * 100 / total) : 0;
	p->SafeSetProgressPos(pct);
	if (cur == total || (cur % 10) == 0)
	{
		CString line;
		line.Format(_T("Burn Board progress: block %d/%d"), cur, total);
		p->SafeAppendLog(line);
	}
}

void CM576CalibratorDlg::BurnBoardWorkerEntry(CString absBinPath)
{
	try
	{
		CString err;
		SafeSetProgressRange(0, 100);
		SafeSetProgressPos(0);
		CString startLine;
		startLine.Format(_T("Burn Board started: %s"), absBinPath.GetString());
		SafeAppendLog(startLine);
		if (!M576BurnBoard439fFirmware(
				m_dev429f,
				absBinPath,
				err,
				&CM576CalibratorDlg::BurnBoardProgressThunk,
				this))
		{
			m_burnBoardLastOk = FALSE;
			m_burnBoardLastMsg = err;
			CString failLine;
			failLine.Format(_T("Burn Board failed: %s"), err.GetString());
			SafeAppendLog(failLine);
		}
		else
		{
			m_burnBoardLastOk = TRUE;
			if (M576Board439fLastBurnDeviceRebooting())
			{
				m_burnBoardLastMsg.Format(
					_T("Main board firmware upgrade succeeded.\n\n")
					_T("File: %s\n\n")
					_T("The 439F board reported UpgradeOK and is rebooting.\n")
					_T("Wait for it to come back, then use Test connection to verify."),
					absBinPath.GetString());
			}
			else
			{
				m_burnBoardLastMsg.Format(
					_T("Main board firmware upgrade succeeded.\n\n")
					_T("File: %s\n\n")
					_T("Use Test connection to verify the board is online."),
					absBinPath.GetString());
			}
			SafeSetProgressPos(100);
			SafeAppendLog(m_burnBoardLastMsg);
		}
	}
	catch (const std::exception& e)
	{
		m_burnBoardLastOk = FALSE;
		CStringA a;
		a.Format("Burn Board: std::exception: %s", e.what());
		m_burnBoardLastMsg = CString(a);
		SafeAppendLog(m_burnBoardLastMsg);
		M576AppendFatalLogUtf8(a.GetString());
	}
	catch (...)
	{
		m_burnBoardLastOk = FALSE;
		m_burnBoardLastMsg = _T("Burn Board: unknown C++ exception (see m576_fatal.log).");
		SafeAppendLog(m_burnBoardLastMsg);
		M576AppendFatalLogUtf8("[Burn Board] unknown C++ exception");
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_BURN_BOARD_FINISHED, 0, 0);
}

void CM576CalibratorDlg::WriteLogFileLine(const CString& line)
{
	const CString absPath = ResolveFilePath(CommLogPathForCurrentDay(m_strCommLogPath));
	HANDLE h = CreateFile(absPath, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
		return;
	LARGE_INTEGER size = {};
	const BOOL hasSize = GetFileSizeEx(h, &size);
	if (hasSize && size.QuadPart == 0)
	{
		const BYTE bom[] = { 0xEF, 0xBB, 0xBF };
		DWORD wr0 = 0;
		WriteFile(h, bom, sizeof(bom), &wr0, NULL);
	}
	const CStringW w = M576NarrowToWideForUi(line) + L"\r\n";
	int n8 = WideCharToMultiByte(CP_UTF8, 0, w, w.GetLength(), NULL, 0, NULL, NULL);
	if (n8 < 1)
	{
		CloseHandle(h);
		return;
	}
	CStringA utf8;
	LPSTR buf = utf8.GetBuffer(n8);
	const int wlen = w.GetLength();
	(void)WideCharToMultiByte(CP_UTF8, 0, w, wlen, buf, n8, NULL, NULL);
	utf8.ReleaseBuffer(n8);
	DWORD written = 0;
	WriteFile(h, utf8.GetString(), (DWORD)utf8.GetLength(), &written, NULL);
	CloseHandle(h);
}

CString CM576CalibratorDlg::GetComboCom()
{
	CString s;
	int i = m_comboCom.GetCurSel();
	if (i >= 0)
		m_comboCom.GetLBText(i, s);
	return s;
}

void CM576CalibratorDlg::SyncCsvPathWithMode()
{
	for (int i = 0; i < 4; ++i)
	{
		m_strCsvPm[i] = g_m576DefaultPmCsvRel[i];
		m_strCsvPd[i] = g_m576DefaultPdCsvRel[i];
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		CString hint;
		if (m_nCalMode == 0)
			hint = _T("CSV: output\\pm_mcs1|2, pm_1x64_1|2.csv (built-in)");
		else
			hint = _T("CSV: output\\pd_mcs1|2, pd_1x64_1|2.csv (built-in)");
		::SetDlgItemText(m_hWnd, IDC_STATIC_PATH_CSV_HINT, hint);
		UpdateData(FALSE);
	}
}

void CM576CalibratorDlg::SyncRecal0ControlsVisibility()
{
	const BOOL showCmdA = (m_nCalMode == 0);
	const int sw = showCmdA ? SW_SHOW : SW_HIDE;
	if (CWnd* p = GetDlgItem(IDC_STATIC_LABEL_TLS))
		p->ShowWindow(sw);
	if (CWnd* p = GetDlgItem(IDC_COMBO_TLS))
		p->ShowWindow(sw);
	if (CWnd* p = GetDlgItem(IDC_STATIC_LABEL_WL))
		p->ShowWindow(sw);
	if (CWnd* p = GetDlgItem(IDC_COMBO_WAVELENGTH))
		p->ShowWindow(sw);
	if (CWnd* p = GetDlgItem(IDC_STATIC_LABEL_PM))
		p->ShowWindow(sw);
	if (CWnd* p = GetDlgItem(IDC_COMBO_PM_RANGE))
		p->ShowWindow(sw);
}

BOOL CM576CalibratorDlg::OpenPort()
{
	m_dev429f.ClosePort();

	CString sCom = GetComboCom();
	sCom.Trim();
	if (sCom.IsEmpty() || _tcsnicmp(sCom, _T("COM"), 3) != 0)
	{
		AppendLog(_T("Select a valid COM port."));
		return FALSE;
	}
	CString path;
	path.Format(_T("\\\\.\\%s"), (LPCTSTR)sCom);

	if (!m_dev429f.OpenPort((LPTSTR)(LPCTSTR)path, 115200, 8, NOPARITY, ONESTOPBIT))
	{
		AppendLog(_T("Serial port open failed."));
		return FALSE;
	}
	M576CommLogTarget logTarget(&CM576CalibratorDlg::CommLogThunk, this);
	m_dev429f.SetCommLogTarget(logTarget);
	m_pRecal.reset(new CRecalSession(m_dev429f, logTarget));
	m_pDiag.reset(new CDiagnosisSession(m_dev429f, logTarget));
	AppendLog(_T("Port opened (439F control board)."));
	return TRUE;
}

void CM576CalibratorDlg::ClosePort()
{
	m_pRecal.reset();
	m_pDiag.reset();
	m_dev429f.ClosePort();
	AppendLog(_T("Port closed."));
}

void __cdecl CM576CalibratorDlg::CommLogThunk(LPCTSTR line, void* user)
{
	CM576CalibratorDlg* dlg = (CM576CalibratorDlg*)user;
	if (!dlg || !::IsWindow(dlg->m_hWnd))
		return;
	dlg->SafeAppendLog(line);
}

void CM576CalibratorDlg::OnBnClickedOpenPorts()
{
	UpdateData(TRUE);
	const CString prev = GetComboCom();
	FillComPorts();
	if (!prev.IsEmpty())
	{
		const int idx = m_comboCom.FindStringExact(-1, prev);
		if (idx >= 0)
			m_comboCom.SetCurSel(idx);
	}
	if (OpenPort())
		AppendLog(_T("Open port OK."));
	SyncSerialPortUi();
}

void CM576CalibratorDlg::OnBnClickedTestConnection()
{
	UpdateData(TRUE);
	const BOOL busy = m_pathRunning.load() || m_readBackupRunning.load() || m_readSnRunning.load()
		|| m_burnFlashRunning.load() || m_burnBoardRunning.load();
	if (busy)
	{
		AppendLog(_T("Test connection: a background task is running; wait for it to finish."));
		return;
	}
	if (!IsSerialPortOpen())
	{
		AppendLog(_T("Test connection: opening port, then sending info<CR>…"));
		if (!OpenPort())
		{
			AppendLog(_T("Test connection: failed to open serial port."));
			return;
		}
		SyncSerialPortUi();
	}
	else
		AppendLog(_T("Test connection: sending info<CR>…"));
	CString resp, e;
	if (!M576Try439fInfoTest(m_dev429f, resp, e))
	{
		CString line;
		line.Format(_T("Test connection failed: %s"), (LPCTSTR)e);
		AppendLog(line);
		CString box;
		box.Format(_T("439F connection test failed:\n\n%s"), (LPCTSTR)e);
		MessageBoxM576(box, MB_OK | MB_ICONWARNING);
		return;
	}
	int showLen = resp.GetLength();
	if (showLen > 500)
		resp = resp.Left(500) + _T("…");
	CString ok;
	ok.Format(_T("Test connection OK. Reply: %s"), (LPCTSTR)resp);
	AppendLog(ok);
	{
		CString box;
		box.Format(_T("439F connection test OK.\n\ninfo<CR> reply:\n\n%s"), (LPCTSTR)resp);
		MessageBoxM576(box, MB_OK | MB_ICONINFORMATION);
	}
}

void CM576CalibratorDlg::OnBnClickedBurnBoard()
{
	UpdateData(TRUE);
	const BOOL busy = m_pathRunning.load() || m_readBackupRunning.load() || m_readSnRunning.load()
		|| m_burnFlashRunning.load() || m_burnBoardRunning.load() || m_diagRunning.load();
	if (busy)
	{
		AppendLog(_T("Burn Board: a background task is running; wait for it to finish."));
		return;
	}
	if (!IsSerialPortOpen())
	{
		AppendLog(_T("Burn Board: opening port first..."));
		if (!OpenPort())
		{
			AppendLog(_T("Burn Board: failed to open serial port."));
			return;
		}
		SyncSerialPortUi();
	}
	CFileDialog dlg(
		TRUE,
		_T("bin"),
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("Firmware (*.bin)|*.bin||"),
		this);
	if (dlg.DoModal() != IDOK)
	{
		AppendLog(_T("Burn Board cancelled (file selection)."));
		return;
	}
	const CString absBinPath = ResolveFilePath(dlg.GetPathName());
	HANDLE hProbe = CreateFile(absBinPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hProbe == INVALID_HANDLE_VALUE)
	{
		AppendLog(_T("Burn Board: cannot open selected .bin file."));
		return;
	}
	const DWORD sz = GetFileSize(hProbe, NULL);
	CloseHandle(hProbe);
	if (sz == 0 || sz == INVALID_FILE_SIZE)
	{
		AppendLog(_T("Burn Board: selected .bin is empty or unreadable."));
		return;
	}
	if (MessageBoxM576(
			_T("Warning: Burn Board will program the 439F main board firmware directly (no trans).\n\n")
			_T("File: ") + absBinPath + _T("\n\nContinue?"),
			MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2)
		!= IDYES)
	{
		AppendLog(_T("Burn Board cancelled by user."));
		return;
	}
	if (m_burnBoardThread.joinable())
		m_burnBoardThread.join();
	m_burnBoardRunning = true;
	SetPathActionButtonsEnabled(FALSE);
	AppendLog(_T("Burn Board started in background..."));
	m_burnBoardThread = std::thread([this, absBinPath]() { BurnBoardWorkerEntry(absBinPath); });
}

void CM576CalibratorDlg::OnBnClickedClosePort()
{
	UpdateData(TRUE);
	ClosePort();
	SyncSerialPortUi();
}

void CM576CalibratorDlg::OnBnClickedBrowseBackup()
{
	ApplyFixedBinBasePaths(TRUE);
	AppendLog(_T("BIN output directory is fixed to output\\latest\\ (selection disabled)."));
}

void CM576CalibratorDlg::OnBnClickedBrowseOut()
{
	ApplyFixedBinBasePaths(TRUE);
	AppendLog(_T("BIN output directory is fixed to output\\latest\\ (selection disabled)."));
}

void CM576CalibratorDlg::OnBnClickedReadFlashBackup()
{
	ApplyFixedBinBasePaths(TRUE);
	if (m_readBackupRunning.load())
		return;
	if (m_burnFlashRunning.load() || m_burnBoardRunning.load())
	{
		AppendLog(_T("Flash/board burn in progress; wait before reading Flash backup."));
		return;
	}
	if (m_pathRunning.load())
	{
		AppendLog(_T("Path run in progress; wait for it to finish before reading Flash backup."));
		return;
	}
	UpdateData(TRUE);
	ApplyFixedBinBasePaths(TRUE);
	CString snErr;
	if (!ValidateSnBeforeBinOp(snErr))
	{
		AppendLog(snErr);
		MessageBoxM576(
			snErr + _T("\n\nRun Read All SN first, then Read Flash backup."),
			MB_OK | MB_ICONWARNING);
		return;
	}
	EnsureOutputFolderUnderExe(GetExeFolder());
	if (!m_dev429f.GetPortHandle() || m_dev429f.GetPortHandle() == INVALID_HANDLE_VALUE)
	{
		if (!OpenPort())
			return;
	}
	const CString absOutDir = ResolveBinOutputDirAbs();
	if (m_readBackupThread.joinable())
		m_readBackupThread.join();
	m_readBackupRunning = true;
	m_suppressPathProgress = false;
	SetPathActionButtonsEnabled(FALSE);
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	AppendLog(_T("Read Flash: writes {SN}_backup.bin x10 under output\\latest\\ (MCS 0xC4; 1x64 MEM 4x2K per trans)."));
	m_readBackupThread = std::thread([this, absOutDir]() { ReadFlashBackupWorkerEntry(absOutDir); });
}

void CM576CalibratorDlg::OnBnClickedStop()
{
	m_bStop = TRUE;
	AppendLog(_T("Stop requested."));
	if (m_pathRunning.load())
	{
		m_suppressPathProgress = true;
		m_progress.SetRange(0, 100);
		m_progress.SetPos(0);
		// Completion / notice when the path worker exits: OnPathFinished
	}
}

// --- Diagnosis: CSV SW group(s), then three measure blocks (1550 / 1310-SFP / 1310-laser paths) ---

void CM576CalibratorDlg::OnBnClickedRunDiag()
{
	if (m_diagRunning.load())
	{
		AppendLog(_T("Diagnosis: already running."));
		return;
	}
	const BOOL otherBusy = m_pathRunning.load() || m_readBackupRunning.load() || m_readSnRunning.load()
		|| m_burnFlashRunning.load() || m_burnBoardRunning.load();
	if (otherBusy)
	{
		AppendLog(_T("Diagnosis: another background task is running; wait for it to finish."));
		return;
	}
	UpdateData(TRUE);
	if (!IsSerialPortOpen())
	{
		AppendLog(_T("Diagnosis: opening port…"));
		if (!OpenPort())
		{
			AppendLog(_T("Diagnosis: failed to open serial port."));
			return;
		}
		SyncSerialPortUi();
	}

	const CString exeFolder = GetExeFolder();
	EnsureOutputFolderUnderExe(exeFolder);
	const CString outDir = exeFolder + _T("\\output");
	const CString swCsvPath = outDir + _T("\\diagnosis_sw.csv");

	std::vector<M576DiagnosisRow> rows;
	CString loadErr;
	if (!M576LoadDiagnosisSwCsv(swCsvPath, rows, loadErr))
	{
		CString msg;
		msg.Format(_T("Diagnosis: %s"), loadErr.IsEmpty() ? _T("CSV load failed.") : loadErr.GetString());
		AppendLog(msg);
		MessageBoxM576(msg, MB_OK | MB_ICONWARNING);
		return;
	}

	if (m_diagThread.joinable())
		m_diagThread.join();
	m_diagStop = FALSE;
	m_diagRunning = true;
	m_progress.SetRange32(0, (int)rows.size());
	m_progress.SetPos(0);
	{
		const CString logPath = M576GetDiagnosisUnifiedLogCsvPath(outDir);
		CString msg;
		msg.Format(
			_T("Diagnosis started (loops full CSV until Stop): %d group(s) from %s — appends PD/OPM rows to %s."),
			(int)rows.size(),
			swCsvPath.GetString(),
			logPath.GetString());
		AppendLog(msg);
	}
	SetPathActionButtonsEnabled(FALSE);
	if (CWnd* p = GetDlgItem(IDC_BTN_STOP_DIAG))
		p->EnableWindow(TRUE);
	if (CWnd* p = GetDlgItem(IDC_BTN_RUN_DIAG))
		p->EnableWindow(FALSE);
	m_diagThread = std::thread([this, rows, outDir]() { DiagnosisWorkerEntry(rows, outDir); });
}

void CM576CalibratorDlg::OnBnClickedStopDiag()
{
	if (!m_diagRunning.load())
		return;
	m_diagStop = TRUE;
	AppendLog(_T("Diagnosis: stop requested."));
}

void CM576CalibratorDlg::DiagnosisWorkerEntry(std::vector<M576DiagnosisRow> rows, CString outDir)
{
	const int N = (int)rows.size();
	int completed = 0;
	int fullLapsDone = 0;
	const CString appendPath = M576GetDiagnosisUnifiedLogCsvPath(outDir);

	// Fixed inter-command pause (stop-aware, sliced) between consecutive serial exchanges
	// within a lap. The first SW of the first group of lap 0 has no preceding pause; every
	// later command in that lap uses kDiagInterCmdDelayMs. Before each `pd 1`, an additional
	// kDiagPdPreDelayMs wait runs after the normal gap. Between full laps, one extra DiagDelay()
	// runs before the next lap's first SW (see outer loop).
	constexpr DWORD kDiagInterCmdDelayMs = 40;
	constexpr DWORD kDiagPdPreDelayMs = 1000;
	constexpr DWORD kDiagInterCmdSliceMs = 50;

	auto JoinPipe = [](const std::vector<CStringA>& parts) -> CStringA
	{
		CStringA out;
		for (size_t k = 0; k < parts.size(); ++k)
		{
			if (k > 0)
				out += '|';
			out += parts[k];
		}
		return out;
	};

	auto DiagDelayMs = [this, kDiagInterCmdSliceMs](DWORD delayMs)
	{
		DWORD remaining = delayMs;
		while (remaining > 0 && !m_diagStop)
		{
			const DWORD t = remaining > kDiagInterCmdSliceMs ? kDiagInterCmdSliceMs : remaining;
			::Sleep(t);
			remaining -= t;
		}
	};

	auto DiagDelay = [&]() { DiagDelayMs(kDiagInterCmdDelayMs); };

	bool firstDiagCmd = true;
	auto MaybeDelay = [&]()
	{
		if (firstDiagCmd)
		{
			firstDiagCmd = false;
			return;
		}
		DiagDelay();
	};

	for (;;)
	{
		if (m_diagStop)
			break;

		if (fullLapsDone > 0)
			DiagDelay();

		if (m_diagStop)
			break;

		completed = 0;
		firstDiagCmd = true;

		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_PATH_PROGRESS_POS, (WPARAM)0, 0);

		BOOL abortNoSession = FALSE;
		CDiagnosisSession* session = m_pDiag.get();
		if (session == NULL)
		{
			SafeAppendLog(_T("Diagnosis: serial port closed mid-run; aborting."));
			abortNoSession = TRUE;
		}
		else
		{
			CString err;

			for (int i = 0; i < N; ++i)
			{
				if (m_diagStop)
					break;
				session = m_pDiag.get();
				if (session == NULL)
				{
					SafeAppendLog(_T("Diagnosis: serial port closed mid-run; aborting."));
					abortNoSession = TRUE;
					break;
				}
				const M576DiagnosisRow& src = rows[(size_t)i];

				M576DiagnosisResultRow r;
				r.step = i + 1;
				r.label = src.label;
				r.swCount = (int)src.swCommands.size();
				r.swCmds = JoinPipe(src.swCommands);

				if (!src.channel.IsEmpty())
					session->EmitNote(_T("[group %d/%d] %hs (SWx%d)"), i + 1, N, src.channel.GetString(), r.swCount);
				else if (!src.label.IsEmpty())
					session->EmitNote(_T("[group %d/%d] %hs (SWx%d)"), i + 1, N, src.label.GetString(), r.swCount);
				else
					session->EmitNote(_T("[group %d/%d] (SWx%d)"), i + 1, N, r.swCount);

				DWORD elapsedSw = 0;
				std::vector<CStringA> swReplies;
				swReplies.reserve(src.swCommands.size());

				BOOL stoppedMid = FALSE;
				for (size_t k = 0; k < src.swCommands.size(); ++k)
				{
					if (m_diagStop)
					{
						stoppedMid = TRUE;
						break;
					}
					CString labelSw;
					labelSw.Format(_T("SW %d.%d/%d"), i + 1, (int)k + 1, (int)src.swCommands.size());
					CStringA reply;
					DWORD ms = 0;
					MaybeDelay();
					if (m_diagStop) { stoppedMid = TRUE; break; }
					const BOOL ok = session->ExchangeAsciiLine(labelSw.GetString(), src.swCommands[k], reply, 3000, ms, err);
					elapsedSw += ms;
					swReplies.push_back(reply);
					if (ok && reply.CompareNoCase("OK") == 0)
						++r.swOkCount;
				}
				r.swReplies = JoinPipe(swReplies);

				if (stoppedMid)
				{
					r.totalMs = elapsedSw;
					completed = i + 1;
					break;
				}

				DWORD totalElapsed = elapsedSw;

				// Six-step dark/light precheck disabled per production requirement (see #if 0 block below).
				static const int kDiagSw3Third[] = { 1, 4, 8 };
#if 0
				int sw11Light = 0;
				int sw12Light = 0;
				CStringA parseErrA;
				if (!M576DiagnosisParseFirstSw11Sw12LightPorts(src.swCommands, sw11Light, sw12Light, parseErrA))
				{
					CString log;
					log.Format(
						_T("Diagnosis: group %d/%d: %hs"),
						i + 1,
						N,
						parseErrA.IsEmpty() ? "SW1 port parse failed." : parseErrA.GetString());
					SafeAppendLog(log);
					stoppedMid = TRUE;
					completed = i + 1;
					r.totalMs = totalElapsed;
					break;
				}

				static const BOOL kDiagPreIsPd[6] = { TRUE, TRUE, FALSE, FALSE, FALSE, FALSE };
				auto RunSixStepPrecheck = [&](int preIdx) -> void
				{
					for (int pi = 0; pi < 6; ++pi)
					{
						if (m_diagStop)
						{
							stoppedMid = TRUE;
							completed = i + 1;
							return;
						}
						CStringA swLine;
						if (pi == 0)
							swLine = "SW 3 1 2";
						else if (pi == 1)
							swLine.Format("SW 3 1 %d", kDiagSw3Third[preIdx]);
						else if (pi == 2)
							swLine.Format("SW 1 1 %d", sw11Light - 1);
						else if (pi == 3)
							swLine.Format("SW 1 1 %d", sw11Light);
						else if (pi == 4)
							swLine.Format("SW 1 2 %d", sw12Light - 1);
						else
							swLine.Format("SW 1 2 %d", sw12Light);
						CString labelPreSw;
						labelPreSw.Format(_T("S%d pre %d/6 "), preIdx + 1, pi + 1);
						labelPreSw += CString(swLine);
						DWORD ms = 0;
						MaybeDelay();
						if (m_diagStop)
						{
							stoppedMid = TRUE;
							completed = i + 1;
							return;
						}
						CStringA swReply;
						(void)session->ExchangeAsciiLine(labelPreSw.GetString(), swLine, swReply, 3000, ms, err);
						totalElapsed += ms;
						if (m_diagStop)
						{
							stoppedMid = TRUE;
							completed = i + 1;
							return;
						}
						DiagDelayMs(kDiagPdPreDelayMs);
						if (m_diagStop)
						{
							stoppedMid = TRUE;
							completed = i + 1;
							return;
						}
						CString labelRead;
						if (kDiagPreIsPd[pi])
							labelRead.Format(_T("S%d pre pd %hs"), preIdx + 1, swLine.GetString());
						else
							labelRead.Format(_T("S%d pre opm %hs"), preIdx + 1, swLine.GetString());
						ms = 0;
						const CStringA readPayload = kDiagPreIsPd[pi] ? CStringA("pd 1") : CStringA("opm 3 1");
						(void)session->ExchangeAsciiLine(
							labelRead.GetString(), readPayload, r.prePdPm[preIdx][pi], 3000, ms, err);
						totalElapsed += ms;
						if (m_diagStop)
						{
							stoppedMid = TRUE;
							completed = i + 1;
							return;
						}
					}
				};
#endif

				// Three paths: after each source switch, set wavelength then read PD/OPM.
				// s1: SW 3 1 1, SWL 8 1550, pd 1, opm 3 1 — s2/s3: SW 3 1 4|8, SWL 8 1310, pd 1, opm 3 1.
				for (int scen = 0; scen < 3; ++scen)
				{
					if (m_diagStop)
					{
						r.totalMs = totalElapsed;
						completed = i + 1;
						stoppedMid = TRUE;
						break;
					}
					const int third = kDiagSw3Third[scen];
					r.wlScen[scen].sw3Third = third;
					DWORD ms = 0;

					CStringA sw3;
					sw3.Format("SW 3 1 %d", third);
					CString labelSw3;
					labelSw3.Format(_T("SW31 %d s%d"), third, scen + 1);
					MaybeDelay();
					if (m_diagStop)
					{
						r.totalMs = totalElapsed;
						completed = i + 1;
						stoppedMid = TRUE;
						break;
					}
					(void)session->ExchangeAsciiLine(labelSw3.GetString(), sw3, r.wlScen[scen].sw3Reply, 3000, ms, err);
					totalElapsed += ms;

					if (m_diagStop)
					{
						r.totalMs = totalElapsed;
						completed = i + 1;
						stoppedMid = TRUE;
						break;
					}

					if (scen == 0)
					{
						const int wlNm = 1550;
						CString label1550;
						CStringA wire1550;
						FormatSwlLabel(label1550, _T("after SW3"), M576_DIAG_SWL_TLS_SOURCE, wlNm);
						label1550.AppendFormat(_T(" 1 %d s%d"), third, scen + 1);
						FormatSwlWire(wire1550, M576_DIAG_SWL_TLS_SOURCE, wlNm);
						MaybeDelay();
						if (m_diagStop)
						{
							r.totalMs = totalElapsed;
							completed = i + 1;
							stoppedMid = TRUE;
							break;
						}
						(void)session->ExchangeAsciiLine(label1550.GetString(), wire1550, r.wlScen[scen].wl1550Reply, 3000, ms, err);
						totalElapsed += ms;
					}
					else
					{
						const int wlNm = 1310;
						CString label1310;
						CStringA wire1310;
						FormatSwlLabel(label1310, _T("after SW3"), M576_DIAG_SWL_TLS_SOURCE, wlNm);
						label1310.AppendFormat(_T(" 1 %d s%d"), third, scen + 1);
						FormatSwlWire(wire1310, M576_DIAG_SWL_TLS_SOURCE, wlNm);
						MaybeDelay();
						if (m_diagStop)
						{
							r.totalMs = totalElapsed;
							completed = i + 1;
							stoppedMid = TRUE;
							break;
						}
						(void)session->ExchangeAsciiLine(label1310.GetString(), wire1310, r.wlScen[scen].wl1310Reply, 3000, ms, err);
						totalElapsed += ms;
					}

					if (m_diagStop)
					{
						r.totalMs = totalElapsed;
						completed = i + 1;
						stoppedMid = TRUE;
						break;
					}

					CString labelPd;
					labelPd.Format(_T("PD s%d (SW3 1 %d)"), scen + 1, third);
					MaybeDelay();
					if (m_diagStop)
					{
						r.totalMs = totalElapsed;
						completed = i + 1;
						stoppedMid = TRUE;
						break;
					}
					DiagDelayMs(kDiagPdPreDelayMs);
					if (m_diagStop)
					{
						r.totalMs = totalElapsed;
						completed = i + 1;
						stoppedMid = TRUE;
						break;
					}
					(void)session->ExchangeAsciiLine(labelPd.GetString(), CStringA("pd 1"), r.wlScen[scen].pdReply, 3000, ms, err);
					totalElapsed += ms;

					if (m_diagStop)
					{
						r.totalMs = totalElapsed;
						completed = i + 1;
						stoppedMid = TRUE;
						break;
					}

					CString labelOpm;
					labelOpm.Format(_T("OPM s%d (SW3 1 %d)"), scen + 1, third);
					MaybeDelay();
					if (m_diagStop)
					{
						r.totalMs = totalElapsed;
						completed = i + 1;
						stoppedMid = TRUE;
						break;
					}
					(void)session->ExchangeAsciiLine(labelOpm.GetString(), CStringA("opm 3 1"), r.wlScen[scen].opmReply, 3000, ms, err);
					totalElapsed += ms;
				}

				if (stoppedMid)
					break;

				r.totalMs = totalElapsed;
				CStringA channelOut = src.channel;
				if (channelOut.IsEmpty())
					channelOut = src.label;
				CString appendErr;
				if (!M576AppendDiagnosisPythonRow(appendPath, channelOut, r.wlScen, appendErr))
				{
					CString msg;
					msg.Format(_T("Diagnosis: append CSV failed: %s"), appendErr.IsEmpty() ? _T("(unknown)") : appendErr.GetString());
					SafeAppendLog(msg);
				}
				completed = i + 1;

				if (m_hWnd && ::IsWindow(m_hWnd))
					::PostMessage(m_hWnd, WM_M576_PATH_PROGRESS_POS, (WPARAM)completed, 0);
			}
		}

		const BOOL fullLap = (!abortNoSession && completed == N);
		if (fullLap)
			++fullLapsDone;

		if (abortNoSession || !fullLap || m_diagStop)
			break;
	}

	if (m_pDiag && m_diagStop)
		m_pDiag->EmitNote(_T("Diagnosis stopped at step %d/%d"), completed, N);

	m_diagFinishFullLaps = fullLapsDone;
	m_diagFinishLastSteps = completed;
	m_diagFinishTotalGroups = N;

	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_DIAG_FINISHED, (WPARAM)(m_diagStop ? 1 : 0), (LPARAM)completed);
}

LRESULT CM576CalibratorDlg::OnDiagFinished(WPARAM wParam, LPARAM lParam)
{
	if (m_diagThread.joinable())
		m_diagThread.join();
	m_diagRunning = false;
	const BOOL stopped = wParam ? TRUE : FALSE;
	(void)lParam;
	m_diagStop = FALSE;

	SetPathActionButtonsEnabled(TRUE);
	if (CWnd* p = GetDlgItem(IDC_BTN_STOP_DIAG))
		p->EnableWindow(FALSE);

	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);

	CString summary;
	summary.Format(
		_T("Diagnosis %s: %d full lap(s); last progress %d/%d group(s). PD/OPM log appended under output\\diagnosis_log.csv."),
		stopped ? _T("stopped by user") : _T("ended (port closed or incomplete lap)"),
		m_diagFinishFullLaps,
		m_diagFinishLastSteps,
		m_diagFinishTotalGroups);
	AppendLog(summary);
	return 0;
}

void CM576CalibratorDlg::OnBnClickedExportCalibStats()
{
	std::vector<SCalibrationStatRow> copy;
	{
		std::lock_guard<std::mutex> lock(m_statsRowsMutex);
		if (m_statsRows.empty())
		{
			MessageBoxM576(
				_T("No calibration statistics to export. Run a path (PM or PD) first."),
				MB_OK | MB_ICONINFORMATION);
			return;
		}
		copy = m_statsRows;
	}
	CTime t = CTime::GetCurrentTime();
	CString defName;
	defName.Format(
		_T("m576_calib_stats_%04d%02d%02d_%02d%02d%02d.csv"),
		(int)t.GetYear(),
		(int)t.GetMonth(),
		(int)t.GetDay(),
		(int)t.GetHour(),
		(int)t.GetMinute(),
		(int)t.GetSecond());
	CFileDialog dlg(
		FALSE,
		_T("csv"),
		defName,
		OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
		_T("CSV files (*.csv)|*.csv|All files (*.*)|*.*||"),
		this);
	if (dlg.DoModal() != IDOK)
		return;
	CString err;
	if (!WriteCalibrationStatsCsv(dlg.GetPathName(), copy, err))
	{
		CString m = err.IsEmpty() ? _T("Failed to write CSV.") : err;
		MessageBoxM576(m, MB_OK | MB_ICONERROR);
		return;
	}
	AppendLog(_T("Calibration stats CSV written."));
}

void CM576CalibratorDlg::OnBnClickedCalPm()
{
	UpdateData(TRUE);
	/// PM defaults: TLS=8, 1310 nm, PM range=1 (Command A).
	m_tlsIndex = M576_DEFAULT_TLS_SOURCE - M576_MIN_TLS_SOURCE;
	m_strWavelength = _T("1310");
	m_pmRangeIndex = M576_DEFAULT_PM_RANGE;
	if (CComboBox* pTls = (CComboBox*)GetDlgItem(IDC_COMBO_TLS))
		pTls->SetCurSel(m_tlsIndex);
	if (CComboBox* pWl = (CComboBox*)GetDlgItem(IDC_COMBO_WAVELENGTH))
		pWl->SetWindowText(m_strWavelength);
	if (CComboBox* pPm = (CComboBox*)GetDlgItem(IDC_COMBO_PM_RANGE))
		pPm->SetCurSel(m_pmRangeIndex);
	SyncCsvPathWithMode();
	SyncRecal0ControlsVisibility();
}

void CM576CalibratorDlg::OnBnClickedCalPd()
{
	UpdateData(TRUE);
	SyncCsvPathWithMode();
	SyncRecal0ControlsVisibility();
}

BOOL CM576CalibratorDlg::ValidateRunPathInputs(CString& errMsg)
{
	if (!IsSerialPortOpen())
	{
		errMsg = _T("Open the serial port (Open Port) before Run path.");
		return FALSE;
	}
	CString com = GetComboCom();
	com.Trim();
	if (com.IsEmpty())
	{
		errMsg = _T("Select 439F COM port (Port).");
		return FALSE;
	}
	if (com.GetLength() < 4 || _tcsnicmp(com, _T("COM"), 3) != 0)
	{
		errMsg = _T("Select a valid COM port (e.g. COM3).");
		return FALSE;
	}
	if (m_nCalMode == 0)
	{
		CString wl = m_strWavelength;
		wl.Trim();
		if (wl.IsEmpty())
		{
			errMsg = _T("PM mode: enter or select wavelength (nm).");
			return FALSE;
		}
		int nm = 0;
		if (!ParseWavelengthNm(m_strWavelength, nm, errMsg))
			return FALSE;
	}
	int nFound = 0;
	if (m_nCalMode == 0)
	{
		for (int i = 0; i < 4; ++i)
		{
			const CString p = m_strCsvPm[i].Trim();
			if (p.IsEmpty())
				continue;
			if (GetFileAttributes(ResolveFilePath(p)) != INVALID_FILE_ATTRIBUTES)
				nFound++;
		}
		if (nFound == 0)
		{
			errMsg = _T("PM mode: no built-in PM CSV found under exe\\output (e.g. pm_mcs1.csv). Check PostBuild / output folder.");
			return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			const CString p = m_strCsvPd[i].Trim();
			if (p.IsEmpty())
				continue;
			if (GetFileAttributes(ResolveFilePath(p)) != INVALID_FILE_ATTRIBUTES)
				nFound++;
		}
		if (nFound == 0)
		{
			errMsg = _T("PD mode: no built-in PD CSV found under exe\\output (e.g. pd_mcs1.csv). Check PostBuild / output folder.");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CM576CalibratorDlg::ParseLowTemp1310DacCsv(
	LPCTSTR csvPath,
	stLutSettingZ4671 lutOut[2],
	stM576OneX64MemsSwCoef memsOut[2][4],
	CString& errMsg)
{
	errMsg.Empty();
	if (csvPath == NULL || csvPath[0] == 0)
	{
		errMsg = _T("CSV path is empty.");
		return FALSE;
	}

	ZeroMemory(lutOut, sizeof(stLutSettingZ4671) * 2);
	ZeroMemory(memsOut, sizeof(stM576OneX64MemsSwCoef) * 8);

	BOOL seenMcs[2][34][PORT_MAX_COUNT + MID_MAX_COUNT] = {};
	BOOL seenX64Ch[2][4][M576_1X64_MAX_CHANNEL_NUM] = {};
	BOOL seenX64Mid[2][4][M576_1X64_MAX_MIDPTR_NUM] = {};
	int mcsRows = 0;
	int x64ChRows = 0;
	int x64MidRows = 0;

	CStdioFile f;
	if (!f.Open(csvPath, CFile::modeRead | CFile::typeText))
	{
		errMsg.Format(_T("Cannot open CSV: %s"), csvPath);
		return FALSE;
	}

	CString line;
	int lineNo = 0;
	while (f.ReadString(line))
	{
		++lineNo;
		StripUtfBomForFirstLine(line, lineNo);
		line.Trim();
		if (line.IsEmpty() || line[0] == _T('#'))
			continue;
		if (line.Find(_T("bin_role")) >= 0 && line.Find(_T("file_suffix")) >= 0)
			continue;

		CStringArray cols;
		SplitCsvColumns(line, cols);
		if (cols.GetCount() < 8)
		{
			errMsg.Format(_T("CSV line %d: expected >=8 fields, got %d."), lineNo, cols.GetCount());
			return FALSE;
		}

		const CString suffix = cols[1];
		int swIdx = 0;
		int tempIdx = 0;
		int chIdx = 0;
		int isMid = 0;
		int dacY = 0;
		int dacX = 0;
		if (!ParseStrictIntField(cols[2], swIdx)
			|| !ParseStrictIntField(cols[3], tempIdx)
			|| !ParseStrictIntField(cols[4], chIdx)
			|| !ParseStrictIntField(cols[5], isMid)
			|| !ParseStrictIntField(cols[6], dacY)
			|| !ParseStrictIntField(cols[7], dacX))
		{
			errMsg.Format(_T("CSV line %d: integer field parse failed."), lineNo);
			return FALSE;
		}

		if (dacY < -32768 || dacY > 32767 || dacX < -32768 || dacX > 32767)
		{
			errMsg.Format(_T("CSV line %d: dac_y/dac_x out of int16 range."), lineNo);
			return FALSE;
		}

		int mcsIdx = -1;
		if (suffix.CompareNoCase(g_m576TransLutBinSuffix[0]) == 0)
			mcsIdx = 0;
		else if (suffix.CompareNoCase(g_m576TransLutBinSuffix[1]) == 0)
			mcsIdx = 1;

		if (mcsIdx >= 0)
		{
			if (tempIdx != IDX_TEMP_LOW)
			{
				errMsg.Format(_T("CSV line %d: MCS temp_idx must be %d."), lineNo, IDX_TEMP_LOW);
				return FALSE;
			}
			if (isMid != 0)
			{
				errMsg.Format(_T("CSV line %d: MCS is_mid must be 0."), lineNo);
				return FALSE;
			}
			if (swIdx < 0 || swIdx >= 34 || chIdx < 0 || chIdx >= PORT_MAX_COUNT + MID_MAX_COUNT)
			{
				errMsg.Format(_T("CSV line %d: MCS sw/ch index out of range."), lineNo);
				return FALSE;
			}
			if (seenMcs[mcsIdx][swIdx][chIdx])
			{
				errMsg.Format(_T("CSV line %d: duplicate MCS key (%d,%d,%d)."), lineNo, mcsIdx, swIdx, chIdx);
				return FALSE;
			}
			seenMcs[mcsIdx][swIdx][chIdx] = TRUE;
			lutOut[mcsIdx].wCalibPtrDAC[swIdx][IDX_TEMP_LOW][chIdx][0] = (WORD)(short)dacY;
			lutOut[mcsIdx].wCalibPtrDAC[swIdx][IDX_TEMP_LOW][chIdx][1] = (WORD)(short)dacX;
			++mcsRows;
			continue;
		}

		int dev = -1;
		int sw = -1;
		for (int d = 0; d < 2 && dev < 0; ++d)
		{
			for (int s = 0; s < 4; ++s)
			{
				CString expect;
				expect.Format(_T("%s_sw%d"), g_m576TransLutBinSuffix[2 + d], s + 1);
				if (suffix.CompareNoCase(expect) == 0)
				{
					dev = d;
					sw = s;
					break;
				}
			}
		}
		if (dev < 0 || sw < 0)
		{
			errMsg.Format(_T("CSV line %d: unknown file_suffix '%s'."), lineNo, suffix.GetString());
			return FALSE;
		}
		if (tempIdx != 0)
		{
			errMsg.Format(_T("CSV line %d: 1x64 temp_idx must be 0."), lineNo);
			return FALSE;
		}
		if (swIdx != sw)
		{
			errMsg.Format(_T("CSV line %d: 1x64 sw_lut_idx mismatch suffix sw."), lineNo);
			return FALSE;
		}
		if (isMid == 0)
		{
			if (chIdx < 0 || chIdx >= M576_1X64_MAX_CHANNEL_NUM)
			{
				errMsg.Format(_T("CSV line %d: 1x64 channel index out of range."), lineNo);
				return FALSE;
			}
			if (seenX64Ch[dev][sw][chIdx])
			{
				errMsg.Format(_T("CSV line %d: duplicate 1x64 channel key (%d,%d,%d)."), lineNo, dev, sw, chIdx);
				return FALSE;
			}
			seenX64Ch[dev][sw][chIdx] = TRUE;
			memsOut[dev][sw].stCalibDAC[0].stChnDAC[chIdx].sDACx = (short)dacY;
			memsOut[dev][sw].stCalibDAC[0].stChnDAC[chIdx].sDACy = (short)dacX;
			++x64ChRows;
		}
		else if (isMid == 1)
		{
			if (chIdx < 0 || chIdx >= M576_1X64_MAX_MIDPTR_NUM)
			{
				errMsg.Format(_T("CSV line %d: 1x64 mid index out of range."), lineNo);
				return FALSE;
			}
			if (seenX64Mid[dev][sw][chIdx])
			{
				errMsg.Format(_T("CSV line %d: duplicate 1x64 mid key (%d,%d,%d)."), lineNo, dev, sw, chIdx);
				return FALSE;
			}
			seenX64Mid[dev][sw][chIdx] = TRUE;
			memsOut[dev][sw].stCalibDAC[0].stMidDAC[chIdx].sDACx = (short)dacY;
			memsOut[dev][sw].stCalibDAC[0].stMidDAC[chIdx].sDACy = (short)dacX;
			++x64MidRows;
		}
		else
		{
			errMsg.Format(_T("CSV line %d: is_mid must be 0/1."), lineNo);
			return FALSE;
		}
	}
	f.Close();

	const int expectMcsRows = 2 * 34 * (PORT_MAX_COUNT + MID_MAX_COUNT);
	const int expectX64ChRows = 2 * 4 * M576_1X64_MAX_CHANNEL_NUM;
	const int expectX64MidRows = 2 * 4 * M576_1X64_MAX_MIDPTR_NUM;
	if (mcsRows != expectMcsRows || x64ChRows != expectX64ChRows || x64MidRows != expectX64MidRows)
	{
		errMsg.Format(
			_T("CSV data rows incomplete: MCS %d/%d, 1x64 ch %d/%d, 1x64 mid %d/%d."),
			mcsRows,
			expectMcsRows,
			x64ChRows,
			expectX64ChRows,
			x64MidRows,
			expectX64MidRows);
		return FALSE;
	}
	return TRUE;
}

BOOL CM576CalibratorDlg::ValidateMakeBinInputs(const CString& absOutDir, const CString& absCsvPath, CString& errMsg)
{
	errMsg.Empty();
	if (absCsvPath.IsEmpty() || GetFileAttributes(absCsvPath) == INVALID_FILE_ATTRIBUTES)
	{
		errMsg.Format(_T("MakeBin: CSV not found: %s"), absCsvPath.GetString());
		return FALSE;
	}

	stLutSettingZ4671 csvLut[2];
	stM576OneX64MemsSwCoef csvMems[2][4];
	if (!ParseLowTemp1310DacCsv(absCsvPath, csvLut, csvMems, errMsg))
	{
		CString msg;
		msg.Format(_T("MakeBin: invalid CSV '%s': %s"), absCsvPath.GetString(), errMsg.GetString());
		errMsg = msg;
		return FALSE;
	}

	const CString legacyBk = M576LegacyBackupBasePath(absOutDir);
	stLutSettingZ4671 tmpLut = {};
	for (int burnIdx = 0; burnIdx < 2; ++burnIdx)
	{
		const CString p = M576ResolveBinPathForBurnIndex(absOutDir, legacyBk, m_snInfo, burnIdx, M576BinFileRole::Backup);
		if (GetFileAttributes(p) == INVALID_FILE_ATTRIBUTES)
		{
			errMsg.Format(_T("MakeBin: required backup missing: %s"), p.GetString());
			return FALSE;
		}
		if (!CLutBinWriter::ReadLutFromFile(p, tmpLut))
		{
			errMsg.Format(_T("MakeBin: cannot read backup LUT: %s"), p.GetString());
			return FALSE;
		}
	}

	stM576OneX64MemsSwCoef tmpMems = {};
	for (int burnIdx = 2; burnIdx < M576_BURN_FILE_COUNT; ++burnIdx)
	{
		const CString p = M576ResolveBinPathForBurnIndex(absOutDir, legacyBk, m_snInfo, burnIdx, M576BinFileRole::Backup);
		if (GetFileAttributes(p) == INVALID_FILE_ATTRIBUTES)
		{
			errMsg.Format(_T("MakeBin: required backup missing: %s"), p.GetString());
			return FALSE;
		}
		if (!CMems1x64LutBinWriter::ReadMemsFromFile(p, &tmpMems))
		{
			errMsg.Format(_T("MakeBin: cannot read backup 1x64 bin: %s"), p.GetString());
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CM576CalibratorDlg::GenerateStandardBinFiles(
	const CString& absOutDir,
	CString& errMsg,
	BOOL preserveMcsMetaFromBackup)
{
	errMsg.Empty();
	std::array<CString, M576_BURN_FILE_COUNT> stdPaths;
	if (!M576BuildBurnFilePaths(absOutDir, m_snInfo, M576BinFileRole::Standard, stdPaths, errMsg))
		return FALSE;
	const CString legacyBk = M576LegacyBackupBasePath(absOutDir);

	for (int i = 0; i < 2; ++i)
	{
		stLutSettingZ4671 merged;
		ZeroMemory(&merged, sizeof(merged));
		BOOL haveBackup = FALSE;
		CString mcsBundleSrcPath;
		const CString perTransBk = M576ResolveBinPathForBurnIndex(absOutDir, legacyBk, m_snInfo, i, M576BinFileRole::Backup);
		if (GetFileAttributes(perTransBk) != INVALID_FILE_ATTRIBUTES)
		{
			if (CLutBinWriter::ReadLutFromFile(perTransBk, merged))
			{
				haveBackup = TRUE;
				mcsBundleSrcPath = perTransBk;
			}
		}
		if (haveBackup)
		{
			if (preserveMcsMetaFromBackup)
			{
				for (int sw = 0; sw < M576_MCS_LUT_SW_MERGE_COUNT; ++sw)
				{
					for (unsigned ch = 0; ch < M576_MCS_LUT_MERGE_CHN_COUNT; ++ch)
					{
						merged.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0] = m_lutByTrans[i].wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][0];
						merged.wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][1] = m_lutByTrans[i].wCalibPtrDAC[sw][IDX_TEMP_LOW][ch][1];
					}
				}
			}
			else if (m_sessionCalibPolicy == M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh)
				MergeLut1550RoomHighSlots(merged, m_lutByTrans[i]);
			else
				MergeLut1310LowTempSlot(merged, m_lutByTrans[i]);
			CString m;
			m.Format(
				preserveMcsMetaFromBackup
					? _T("Trans %d: merged CSV low-temp DAC into backup (preserved LUT temp/date meta).")
					: (m_sessionCalibPolicy == M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh)
						? _T("Trans %d: merged 1550 room+high LUT into backup.")
						: _T("Trans %d: merged session LUT into per-trans backup."),
				i + 1);
			AppendLog(m);
		}
		else
		{
			memcpy(&merged, &m_lutByTrans[i], sizeof(merged));
			CString m;
			m.Format(_T("Trans %d: no backup bin; writing in-memory LUT only."), i + 1);
			AppendLog(m);
		}

		SLutBinWriteParams p;
		p.strOutputPath = stdPaths[i];
		p.pLut = &merged;
		{
			CString sn = m_snInfo.mcsSn[i].Trim();
			if (sn.IsEmpty() && haveBackup && !mcsBundleSrcPath.IsEmpty())
				(void)CLutBinWriter::ReadBundleSnFromFile(mcsBundleSrcPath, sn);
			p.strBundleSN = sn;
		}
		if (!CLutBinWriter::Write(p))
		{
			errMsg.Format(_T("Write BIN failed (trans %d): %s"), i + 1, stdPaths[i].GetString());
			return FALSE;
		}
		memcpy(&m_lutByTrans[i], &merged, sizeof(m_lutByTrans[i]));
		CString ok;
		ok.Format(_T("Trans %d: wrote %s"), i + 1, stdPaths[i].GetString());
		AppendLog(ok);
	}

	for (int li = 2; li < 4; ++li)
	{
		const int dev = li - 2;
		stM576OneX64MemsSwCoef merged4[4];
		ZeroMemory(merged4, sizeof(merged4));
		BOOL haveBackup = FALSE;
		for (int sw = 0; sw < 4; ++sw)
		{
			const int burnIdx = (dev == 0) ? (2 + sw) : (6 + sw);
			const CString perSwBk = M576ResolveBinPathForBurnIndex(absOutDir, legacyBk, m_snInfo, burnIdx, M576BinFileRole::Backup);
			if (GetFileAttributes(perSwBk) != INVALID_FILE_ATTRIBUTES)
			{
				if (CMems1x64LutBinWriter::ReadMemsFromFile(perSwBk, &merged4[sw]))
					haveBackup = TRUE;
			}
		}
		if (haveBackup)
		{
			if (preserveMcsMetaFromBackup)
				MergeMems1310LowTempSlot(merged4, m_mems1x64[dev]);
			else if (m_sessionCalibPolicy == M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh)
				MergeMems1550RoomHighSlots(merged4, m_mems1x64[dev]);
			else
				MergeMems1310LowTempSlot(merged4, m_mems1x64[dev]);
			CString m;
			m.Format(_T("Trans %d: merged Mems session (4x2K) into backup."), li + 1);
			AppendLog(m);
		}
		else
		{
			memcpy(merged4, m_mems1x64[dev], sizeof(merged4));
			CString m;
			m.Format(_T("Trans %d: no 1x64 per-switch Mems backup; writing in-memory 4x2K only."), li + 1);
			AppendLog(m);
		}
		for (int sw = 0; sw < 4; ++sw)
		{
			const int burnIdx = (dev == 0) ? (2 + sw) : (6 + sw);
			M576OneX64ApplyStandardTempMeta(merged4[sw]);
			CString sn = m_snInfo.oneX64Sn[dev][sw].Trim();
			if (sn.IsEmpty())
				sn = CMems1x64LutBinWriter::ReadBundleVer16FromCoef(merged4[sw]);
			const CString absOutSw = stdPaths[burnIdx];
			if (!CMems1x64LutBinWriter::WriteSingleSwitch(merged4[sw], sw, absOutSw, sn, CString()))
			{
				errMsg.Format(_T("Write 1x64 Mems BIN failed (trans %d sw %d): %s"),
					li + 1, sw + 1, absOutSw.GetString());
				return FALSE;
			}
		}
		memcpy(m_mems1x64[dev], merged4, sizeof(m_mems1x64[dev]));
		CString ok;
		ok.Format(_T("Trans %d: wrote 1x64 4x2K ({SN}_standard.bin x4)"), li + 1);
		AppendLog(ok);
	}
	LogBurnFilePaths(this, stdPaths, _T("standard"));
	return TRUE;
}

// 起路径定标工作线程（PathWorkerEntry），并设 UI 为运行中态。

void CM576CalibratorDlg::OnBnClickedRunPath()
{
	if (m_pathRunning.load())
		return;
	if (m_burnFlashRunning.load() || m_burnBoardRunning.load())
	{
		AppendLog(_T("Flash/board burn in progress; wait before running path."));
		return;
	}
	if (m_readBackupRunning.load())
	{
		AppendLog(_T("Read Flash backup in progress; wait for it to finish before running path."));
		return;
	}
	if (!UpdateData(TRUE))
		return;
	SyncCsvPathWithMode();
	CString valErr;
	if (!ValidateRunPathInputs(valErr))
	{
		MessageBoxM576(valErr, MB_OK | MB_ICONWARNING);
		return;
	}
	CString snErr;
	if (!ValidateSnBeforeBinOp(snErr))
	{
		AppendLog(snErr);
		MessageBoxM576(
			snErr + _T("\n\nRun Read All SN first, then Run Path."),
			MB_OK | MB_ICONWARNING);
		return;
	}
	m_bStop = FALSE;
	if (m_pathThread.joinable())
		m_pathThread.join();
	m_pathShowFinishInfoBox = TRUE;
	m_pathRunning = true;
	m_suppressPathProgress = false;
	AppendLog(_T("Run Path Started"));
	SetPathActionButtonsEnabled(FALSE);
	if (CWnd* pStop = GetDlgItem(IDC_BTN_STOP))
		pStop->EnableWindow(TRUE);
	m_pathThread = std::thread([this]() { PathWorkerEntry(); });
}

void CM576CalibratorDlg::OnBnClickedClearLog()
{
	m_editLog.SetWindowText(_T(""));
}

void CM576CalibratorDlg::OnDestroy()
{
	m_bStop = TRUE;
	m_diagStop = TRUE;
	if (m_diagThread.joinable())
	{
		HANDLE h = (HANDLE)m_diagThread.native_handle();
		for (;;)
		{
			const DWORD w = MsgWaitForMultipleObjects(1, &h, FALSE, INFINITE, QS_ALLINPUT);
			if (w == WAIT_OBJECT_0)
				break;
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!IsDialogMessage(&msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		m_diagThread.join();
	}
	if (m_pathThread.joinable())
	{
		HANDLE h = (HANDLE)m_pathThread.native_handle();
		for (;;)
		{
			const DWORD w = MsgWaitForMultipleObjects(1, &h, FALSE, INFINITE, QS_ALLINPUT);
			if (w == WAIT_OBJECT_0)
				break;
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!IsDialogMessage(&msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		m_pathThread.join();
	}
	if (m_readBackupThread.joinable())
	{
		HANDLE h = (HANDLE)m_readBackupThread.native_handle();
		for (;;)
		{
			const DWORD w = MsgWaitForMultipleObjects(1, &h, FALSE, INFINITE, QS_ALLINPUT);
			if (w == WAIT_OBJECT_0)
				break;
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!IsDialogMessage(&msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		m_readBackupThread.join();
	}
	if (m_readSnThread.joinable())
	{
		HANDLE h = (HANDLE)m_readSnThread.native_handle();
		for (;;)
		{
			const DWORD w = MsgWaitForMultipleObjects(1, &h, FALSE, INFINITE, QS_ALLINPUT);
			if (w == WAIT_OBJECT_0)
				break;
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!IsDialogMessage(&msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		m_readSnThread.join();
	}
	if (m_burnFlashThread.joinable())
	{
		HANDLE h = (HANDLE)m_burnFlashThread.native_handle();
		for (;;)
		{
			const DWORD w = MsgWaitForMultipleObjects(1, &h, FALSE, INFINITE, QS_ALLINPUT);
			if (w == WAIT_OBJECT_0)
				break;
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!IsDialogMessage(&msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		m_burnFlashThread.join();
	}
	if (m_burnBoardThread.joinable())
	{
		HANDLE h = (HANDLE)m_burnBoardThread.native_handle();
		for (;;)
		{
			const DWORD w = MsgWaitForMultipleObjects(1, &h, FALSE, INFINITE, QS_ALLINPUT);
			if (w == WAIT_OBJECT_0)
				break;
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!IsDialogMessage(&msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		m_burnBoardThread.join();
	}
	m_pathRunning = false;
	m_readBackupRunning = false;
	m_readSnRunning = false;
	m_burnFlashRunning = false;
	m_burnBoardRunning = false;
	m_diagRunning = false;
	m_suppressPathProgress = false;
	CDialogEx::OnDestroy();
}

// 若备份基路径已设，从分 trans 的 bin 预载 m_lutByTrans（含 legacy _tN 名）。

void CM576CalibratorDlg::TryPreloadLutFromPerTransBackup()
{
	const CString absOutDir = ResolveBinOutputDirAbs();
	if (absOutDir.IsEmpty())
		return;
	const CString legacyBk = M576LegacyBackupBasePath(absOutDir);
	for (int li = 0; li < 4; ++li)
	{
		if (li < 2)
		{
			const CString p = M576ResolveBinPathForBurnIndex(absOutDir, legacyBk, m_snInfo, li, M576BinFileRole::Backup);
			if (GetFileAttributes(p) == INVALID_FILE_ATTRIBUTES)
				continue;
			if (CLutBinWriter::ReadLutFromFile(p, m_lutByTrans[li]))
			{
				CString m;
				m.Format(_T("Run path: preloaded MCS trans %d from %s"), li + 1, p.GetString());
				SafeAppendLog(m);
			}
			else
			{
				CString m;
				m.Format(_T("Run path: read MCS backup failed for %s"), p.GetString());
				SafeAppendLog(m);
			}
		}
		else
		{
			const int dev = li - 2;
			BOOL anySw = FALSE;
			for (int sw = 0; sw < 4; ++sw)
			{
				const int burnIdx = (dev == 0) ? (2 + sw) : (6 + sw);
				const CString ps = M576ResolveBinPathForBurnIndex(absOutDir, legacyBk, m_snInfo, burnIdx, M576BinFileRole::Backup);
				if (GetFileAttributes(ps) == INVALID_FILE_ATTRIBUTES)
					continue;
				if (CMems1x64LutBinWriter::ReadMemsFromFile(ps, &m_mems1x64[dev][sw]))
					anySw = TRUE;
			}
			if (anySw)
			{
				CString m;
				m.Format(_T("Run path: preloaded 1x64 trans %d (per-switch 2K Mems, SN paths)."), li + 1);
				SafeAppendLog(m);
			}
			else
			{
				CString m;
				m.Format(_T("Run path: no 1x64 per-switch Mems backup for trans %d."), li + 1);
				SafeAppendLog(m);
			}
		}
	}
}

BOOL CM576CalibratorDlg::PreloadRunPathBackupOrFail(CString& errMsg)
{
	errMsg.Empty();
	const CString absOutDir = ResolveBinOutputDirAbs();
	if (absOutDir.IsEmpty())
	{
		errMsg = _T("Run Path: BIN output directory is empty.");
		return FALSE;
	}
	const CString legacyBk = M576LegacyBackupBasePath(absOutDir);

	for (int li = 0; li < 2; ++li)
	{
		const CString p = M576ResolveBinPathForBurnIndex(absOutDir, legacyBk, m_snInfo, li, M576BinFileRole::Backup);
		if (GetFileAttributes(p) == INVALID_FILE_ATTRIBUTES)
		{
			errMsg.Format(_T("Run Path preload failed: MCS backup missing:\n%s"), p.GetString());
			return FALSE;
		}
		if (!CLutBinWriter::ReadLutFromFile(p, m_lutByTrans[li]))
		{
			errMsg.Format(_T("Run Path preload failed: cannot read MCS backup:\n%s"), p.GetString());
			return FALSE;
		}
		CString m;
		m.Format(_T("Run path: preloaded MCS trans %d from %s"), li + 1, p.GetString());
		SafeAppendLog(m);
	}

	for (int li = 2; li < 4; ++li)
	{
		const int dev = li - 2;
		for (int sw = 0; sw < 4; ++sw)
		{
			const int burnIdx = (dev == 0) ? (2 + sw) : (6 + sw);
			const CString ps = M576ResolveBinPathForBurnIndex(absOutDir, legacyBk, m_snInfo, burnIdx, M576BinFileRole::Backup);
			if (GetFileAttributes(ps) == INVALID_FILE_ATTRIBUTES)
			{
				errMsg.Format(_T("Run Path preload failed: 1x64 backup missing:\n%s"), ps.GetString());
				return FALSE;
			}
			if (!CMems1x64LutBinWriter::ReadMemsFromFile(ps, &m_mems1x64[dev][sw]))
			{
				errMsg.Format(_T("Run Path preload failed: cannot read 1x64 backup:\n%s"), ps.GetString());
				return FALSE;
			}
		}
		CString m;
		m.Format(_T("Run path: preloaded 1x64 trans %d (4x per-switch Mems, SN paths)."), li + 1);
		SafeAppendLog(m);
	}
	return TRUE;
}

void CM576CalibratorDlg::AbortRunPathPreloadFailed(const CString& errMsg)
{
	SafeAppendLog(errMsg);
	CString box;
	box.Format(
		_T("备份 BIN 预载失败，已停止 Run Path。\n\n%s\n\n请先 Read Flash 或检查 Backup BIN 路径。"),
		errMsg.GetString());
	MessageBoxM576(box, MB_OK | MB_ICONERROR);
	m_pathShowFinishInfoBox = FALSE;
}

static CStringA M576AsciiSuffixToA(LPCTSTR suf)
{
	CStringA a;
	if (suf == NULL)
		return a;
	for (; *suf != 0; ++suf)
		a += (char)(*suf & 0xFF);
	return a;
}

void CM576CalibratorDlg::StartSessionCalibPolicyFromWavelength(int wavelengthNm)
{
	m_sessionCalibWavelengthNm = wavelengthNm;
	m_sessionCalibPolicy = PolicyFromWavelengthNm(wavelengthNm);
	CString m;
	if (m_sessionCalibPolicy == M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh)
		m.Format(_T("Session cal policy: 1550 nm -> room slot (high copied on Write BIN), 30 C metadata."));
	else
		m.Format(_T("Session cal policy: %d nm -> low-temp slot (IDX_TEMP_LOW)."), wavelengthNm);
	SafeAppendLog(m);
}

void CM576CalibratorDlg::WarnIfUiWavelengthDiffersFromSession()
{
	int uiNm = 0;
	CString err;
	if (!ParseWavelengthNm(m_strWavelength, uiNm, err))
		return;
	if (uiNm != m_sessionCalibWavelengthNm)
	{
		CString m;
		m.Format(
			_T("Warning: UI wavelength %d nm != session %d nm; Write BIN/CSV use session policy."),
			uiNm,
			m_sessionCalibWavelengthNm);
		AppendLog(m);
	}
}

void CM576CalibratorDlg::ExportSessionDacCsv(M576CalibBinWritePolicy policy, LPCTSTR csvLeafName, LPCTSTR logPreamble)
{
	if (csvLeafName == NULL || csvLeafName[0] == 0)
		return;
	if (logPreamble == NULL)
		logPreamble = _T("");

	CString absOut = ResolveBinOutputDirAbs();
	if (absOut.IsEmpty())
		absOut = GetExeFolder() + _T("\\output");

	const CString fullPath = absOut + _T("\\") + CString(csvLeafName);

	CFile f;
	if (!f.Open(fullPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		CString m;
		m.Format(_T("%s: session DAC CSV open failed: %s"), logPreamble, fullPath.GetString());
		SafeAppendLog(m);
		return;
	}
	const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
	f.Write(bom, 3);
	auto writeA = [&f](LPCSTR s) {
		f.Write(s, (UINT)strlen(s));
		f.Write("\r\n", 2);
	};

	if (policy == M576CalibBinWritePolicy::Slot1550RoomThenCopyHigh)
		writeA(
			"# Room slot only (1550 cal): MCS wCalibPtrDAC[][IDX_TEMP_ROOM][], 1x64 stCalibDAC[1]. "
			"MCS dac_y=[0] dac_x=[1]; 1x64 dac_y=sDACx dac_x=sDACy.");
	else
		writeA(
			"# Low-temp slot only (1310 cal): MCS wCalibPtrDAC[][IDX_TEMP_LOW][], 1x64 stCalibDAC[0]. "
			"MCS dac_y=[0] dac_x=[1]; 1x64 dac_y=sDACx dac_x=sDACy.");
	writeA("bin_role,file_suffix,sw_lut_idx,temp_idx,ch_idx,is_mid,dac_y,dac_x");

	const int tiMcs = McsPrimaryTempSlot(policy);
	const int tiX64 = MemsPrimaryCalibSlot(policy);
	for (int mcs = 0; mcs < 2; ++mcs)
	{
		const CStringA sufA = M576AsciiSuffixToA(g_m576TransLutBinSuffix[mcs]);
		for (int sw = 0; sw < 34; ++sw)
		{
			for (int ch = 0; ch < PORT_MAX_COUNT + MID_MAX_COUNT; ++ch)
			{
				const WORD wy = m_lutByTrans[mcs].wCalibPtrDAC[sw][tiMcs][ch][0];
				const WORD wx = m_lutByTrans[mcs].wCalibPtrDAC[sw][tiMcs][ch][1];
				CStringA line;
				line.Format(
					"MCS%d,%s,%d,%d,%d,0,%d,%d",
					mcs + 1,
					sufA.GetString(),
					sw,
					tiMcs,
					ch,
					(int)(short)wy,
					(int)(short)wx);
				writeA(line);
			}
		}
	}

	for (int dev = 0; dev < 2; ++dev)
	{
		const int sufIdx = 2 + dev;
		const CStringA sufStemA = M576AsciiSuffixToA(g_m576TransLutBinSuffix[sufIdx]);
		for (int sw = 0; sw < 4; ++sw)
		{
			CStringA sufSwA;
			sufSwA.Format("%s_sw%d", sufStemA.GetString(), sw + 1);
			CStringA roleA;
			roleA.Format("1x64_%d_sw%d", dev + 1, sw + 1);
			const stM576OneX64MemsSwCoef& coef = m_mems1x64[dev][sw];
			const stM576OneX64ChnDAC& cal = coef.stCalibDAC[tiX64];
			for (unsigned ci = 0; ci < M576_1X64_MAX_CHANNEL_NUM; ++ci)
			{
				CStringA line;
				line.Format(
					"%s,%s,%d,%d,%u,0,%d,%d",
					roleA.GetString(),
					sufSwA.GetString(),
					sw,
					tiX64,
					ci,
					(int)cal.stChnDAC[ci].sDACx,
					(int)cal.stChnDAC[ci].sDACy);
				writeA(line);
			}
			for (unsigned mi = 0; mi < M576_1X64_MAX_MIDPTR_NUM; ++mi)
			{
				CStringA line;
				line.Format(
					"%s,%s,%d,%d,%u,1,%d,%d",
					roleA.GetString(),
					sufSwA.GetString(),
					sw,
					tiX64,
					mi,
					(int)cal.stMidDAC[mi].sDACx,
					(int)cal.stMidDAC[mi].sDACy);
				writeA(line);
			}
		}
	}

	f.Close();
	{
		CString m;
		m.Format(_T("%s: session DAC CSV -> %s"), logPreamble, fullPath.GetString());
		SafeAppendLog(m);
	}
}

void CM576CalibratorDlg::ExportRunPathBackupDacSnapshotCsvIfBackupBaseSet()
{
	if (ResolveBinOutputDirAbs().IsEmpty())
		return;
	const CString fullPath = BuildSessionDacCsvPath(m_sessionCalibPolicy, M576BinFileRole::Backup);
	const int slash = fullPath.ReverseFind(_T('\\'));
	const CString leaf = (slash >= 0) ? fullPath.Mid(slash + 1) : fullPath;
	ExportSessionDacCsv(m_sessionCalibPolicy, leaf, _T("Run path"));
}

BOOL CM576CalibratorDlg::ExchangeSwlBeforeRunPath(int wavelengthNm, CString& err)
{
	err.Empty();
	if (!m_pDiag)
	{
		err = _T("Run Path: port session not ready (open port first).");
		return FALSE;
	}
	const int tlsSource = m_tlsIndex + 1;
	if (tlsSource < M576_MIN_TLS_SOURCE || tlsSource > M576_MAX_TLS_SOURCE)
	{
		err.Format(_T("Run Path: TLS source %d out of range %d..%d."),
			tlsSource, M576_MIN_TLS_SOURCE, M576_MAX_TLS_SOURCE);
		return FALSE;
	}
	if (wavelengthNm != 1310 && wavelengthNm != 1550)
	{
		err.Format(_T("Run Path: SWL requires wavelength 1310 or 1550 nm (UI has %d)."),
			wavelengthNm);
		return FALSE;
	}
	CStringA wire;
	FormatSwlWire(wire, tlsSource, wavelengthNm);
	CString label;
	FormatSwlLabel(label, _T("(Run Path)"), tlsSource, wavelengthNm);
	CStringA reply;
	DWORD ms = 0;
	if (!m_pDiag->ExchangeAsciiLine(label, wire, reply, 3000, ms, err))
		return FALSE;
	CString msg;
	msg.Format(_T("  %s -> %s (%lums)"), label.GetString(), CString(reply), (unsigned long)ms);
	SafeAppendLog(msg);
	return TRUE;
}

// --- PM 定标：RECAL0 + 分文件 CSV、RECAL1/3、寻峰写 LUT 槽、合并/进度 ---

void CM576CalibratorDlg::RunPathPowerMeter()
{
	ClearCalibStats();
	ClearPathOutcomes();
	CString err;
	int totalAll = 0;
	for (int fs = 0; fs < 4; ++fs)
	{
		const CString rel = m_strCsvPm[fs].Trim();
		if (rel.IsEmpty())
			continue;
		const CString abs = ResolveFilePath(rel);
		if (GetFileAttributes(abs) == INVALID_FILE_ATTRIBUTES)
			continue;
		CArray<SPathStep, SPathStep const&> tmp;
		if (!LoadPathCsv(abs, tmp, err))
			continue;
		totalAll += (int)tmp.GetSize();
	}
	if (totalAll == 0)
	{
		SafeAppendLog(_T("PM: no CSV rows (missing paths or empty files)."));
		return;
	}
	SafeSetProgressRange(0, totalAll);
	for (int li = 0; li < 4; ++li)
		ZeroMemory(&m_lutByTrans[li], sizeof(m_lutByTrans[li]));
	ZeroMemory(m_mems1x64, sizeof(m_mems1x64));
	{
		CString preloadErr;
		if (!PreloadRunPathBackupOrFail(preloadErr))
		{
			AbortRunPathPreloadFailed(preloadErr);
			return;
		}
	}
	ExportRunPathBackupDacSnapshotCsvIfBackupBaseSet();

	int wavelengthNm = 0;
	if (!ParseWavelengthNm(m_strWavelength, wavelengthNm, err))
	{
		SafeAppendLog(err);
		return;
	}
	StartSessionCalibPolicyFromWavelength(wavelengthNm);
	//if (!ExchangeSwlBeforeRunPath(wavelengthNm, err))
	//{
	//	SafeAppendLog(err.IsEmpty() ? _T("Run Path: SWL (set TLS/wavelength) failed.") : err);
	//	return;
	//}
	const int tlsSource = m_tlsIndex + 1;
	const int pmRange = m_pmRangeIndex;
	{
		CStringA line0;
		if (!m_pRecal->ExchangeRecal0ReadLine(tlsSource, wavelengthNm, pmRange, line0, 3000, err))
		{
			if (err.IsEmpty())
				SafeAppendLog(_T("RECAL 0: no OK line after retries."));
			else
				SafeAppendLog(err);
			return;
		}
		{
			CString msg;
			msg.Format(_T("RECAL 0 (TLS=%d nm=%d PM=%d) -> %s"),
				tlsSource, wavelengthNm, pmRange, CString(line0));
			SafeAppendLog(msg);
		}
	}
	{
		const int kOpmIds[2] = { 4, 5 };
		CStringA opmLines[2];
		int readRanges[2] = { -1, -1 };
		for (int oi = 0; oi < 2; ++oi)
		{
			const int opmId = kOpmIds[oi];
			if (!m_pRecal->ExchangeOpmReadPmRange(opmId, opmLines[oi], 3000, err))
			{
				CString log = err.IsEmpty()
					? CString()
					: err;
				if (log.IsEmpty())
				{
					log.Format(
						_T("[PM range] OPM %d 1: no valid readback after retries — Run Path stopped."),
						opmId);
				}
				SafeAppendLog(log);
				CString box;
				box.Format(
					_T("无法读取功率计挡位，已停止定标。\n\n%s\n\n请检查串口与固件后重新 Run Path。"),
					log.GetString());
				MessageBoxM576(box, MB_OK | MB_ICONERROR);
				return;
			}
			readRanges[oi] = CRecalSession::ParseOpmPmRangeReply(opmLines[oi]);
			if (readRanges[oi] < M576_MIN_PM_RANGE || readRanges[oi] > M576_MAX_PM_RANGE)
			{
				CString log;
				log.Format(
					_T("[PM range] OPM %d 1: invalid reply \"%hs\" — Run Path stopped."),
					opmId,
					opmLines[oi].GetString());
				SafeAppendLog(log);
				CString box;
				box.Format(
					_T("功率计挡位应答无效，已停止定标。\n\nopm %d 1 应答: %hs\n\n请检查固件后重新 Run Path。"),
					opmId,
					opmLines[oi].GetString());
				MessageBoxM576(box, MB_OK | MB_ICONERROR);
				return;
			}
			{
				CString msg;
				msg.Format(_T("OPM %d 1: pm_range readback %d."), opmId, readRanges[oi]);
				SafeAppendLog(msg);
			}
		}
		if (readRanges[0] != readRanges[1])
		{
			CString log;
			log.Format(
				_T("[PM range] OPM 4 1 (%d) vs OPM 5 1 (%d) mismatch — Run Path stopped."),
				readRanges[0],
				readRanges[1]);
			SafeAppendLog(log);
			CString box;
			box.Format(
				_T("两路功率计挡位读回不一致，已停止定标。\n\nopm 4 1: %d\nopm 5 1: %d\n\n请检查 PM 挡位后重新 Run Path。"),
				readRanges[0],
				readRanges[1]);
			MessageBoxM576(box, MB_OK | MB_ICONERROR);
			return;
		}
		const int readRange = readRanges[0];
		if (pmRange != M576_MAX_PM_RANGE)
		{
			if (readRange != pmRange)
			{
				CString log;
				log.Format(
					_T("[PM range] OPM 4/5 1 mismatch: RECAL0 set %d, readback %d — Run Path stopped."),
					pmRange,
					readRange);
				SafeAppendLog(log);
				CString box;
				box.Format(
					_T("功率计挡位不一致，已停止定标。\n\n界面/RECAL 0 挡位: %d\nopm 4/5 1 读回: %d\n\n请检查 PM 挡位后重新 Run Path。"),
					pmRange,
					readRange);
				MessageBoxM576(box, MB_OK | MB_ICONERROR);
				return;
			}
			{
				CString msg;
				msg.Format(_T("OPM 4/5 1: pm_range readback %d matches RECAL 0."), readRange);
				SafeAppendLog(msg);
			}
		}
		else
		{
			CString msg;
			msg.Format(_T("OPM 4/5 1: RECAL0 auto(%d), device readback=%d (informational)."),
				M576_MAX_PM_RANGE,
				readRange);
			SafeAppendLog(msg);
		}
	}

	int globalProgress = 0;
	for (int fs = 0; fs < 4; ++fs)
	{
		const CString rel = m_strCsvPm[fs].Trim();
		if (rel.IsEmpty())
		{
			CString m;
			m.Format(_T("PM slot %d: path empty, skip."), fs + 1);
			SafeAppendLog(m);
			continue;
		}
		const CString abs = ResolveFilePath(rel);
		if (GetFileAttributes(abs) == INVALID_FILE_ATTRIBUTES)
		{
			CString m;
			m.Format(_T("PM slot %d: file not found, skip: %s"), fs + 1, rel.GetString());
			SafeAppendLog(m);
			continue;
		}
		CArray<SPathStep, SPathStep const&> steps;
		if (!LoadPathCsv(abs, steps, err))
		{
			SafeAppendLog(err);
			continue;
		}
		if (steps.GetSize() == 0)
			continue;
		{
			CString m;
			m.Format(_T("PM slot %d (%s): %d rows"), fs + 1, rel.GetString(), (int)steps.GetSize());
			SafeAppendLog(m);
		}
		int occT3 = 0;
		int occT4 = 0;
		RunPathPowerMeterFile(fs, steps, globalProgress, totalAll, occT3, occT4, abs);
	}
	SafeAppendLog(_T("Path run finished (PM all slots)."));
	{
		std::lock_guard<std::mutex> lock(m_statsRowsMutex);
		CString m;
		m.Format(
			_T("Calibration stats rows recorded: %d (use Export calib stats CSV)."), (int)m_statsRows.size());
		SafeAppendLog(m);
	}
}

// 单路 pm_*.csv：RECAL1/3 扫点、双轴寻峰、回写 m_lutByTrans[fileSlot] 与 trans3/4 占用计数。

void CM576CalibratorDlg::RunPathPowerMeterFile(
	int fileSlot,
	CArray<SPathStep, SPathStep const&>& steps,
	int& globalProgress,
	int globalTotal,
	int& occT3,
	int& occT4,
	LPCTSTR pmCsvAbsPath)
{
	const int mcsTempSlot = McsPrimaryTempSlot(m_sessionCalibPolicy);
	const int memsCalibSlot = MemsPrimaryCalibSlot(m_sessionCalibPolicy);
	CString err;
	const int total = (int)steps.GetSize();
	CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&> map1x64Rows;
	if (fileSlot >= 2)
	{
		CString mapErr;
		CString mapPath;
		if (pmCsvAbsPath == NULL || pmCsvAbsPath[0] == 0 || !Pm1x64ResolveMappingPath(pmCsvAbsPath, mapPath)
			|| !LoadPm1x64MappingCsv(mapPath, map1x64Rows, mapErr)
			|| !ValidatePmStepsAgainstMapping(steps, map1x64Rows, mapErr))
		{
			SafeAppendLog(mapErr.IsEmpty() ? _T("PM 1x64: mapping CSV missing or invalid.") : mapErr);
			CString m;
			m.Format(_T("PM slot %d: skipping path run (1x64 firmware mapping required)."), fileSlot + 1);
			SafeAppendLog(m);
			const std::string csvBase = M576PathBasenameUtf8(pmCsvAbsPath);
			std::string detail = M576CStringToUtf8(mapErr.IsEmpty()
				? _T("PM 1x64: mapping CSV missing or invalid.")
				: mapErr);
			TruncatePathOutcomeDetail(detail);
			for (int si = 0; si < total; ++si)
			{
				SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBase, si + 1, steps[si]);
				o.result = CalibPathStepResult::Skipped;
				o.failCategory = CalibPathFailCategory::FileMappingSkip;
				o.failStage = "1x64 mapping";
				o.commDetail = detail;
				PushPathFailureOutcome(o);
			}
			globalProgress += total;
			SafeSetProgressPos(globalProgress);
			return;
		}
	}
	const std::string csvBasename = M576PathBasenameUtf8(pmCsvAbsPath);
	const DWORD readTimeout1d = ComputeRecal1DReadTimeoutMs(m_delayMs, m_dacRange, m_dacStep);
	const int gridN = AxisPointCount(m_dacRange, m_dacStep);
	CStringA lineOk, lineY, lineX;
	for (int i = 0; i < total; ++i)
	{
		if (m_bStop)
		{
			for (int j = i; j < total; ++j)
			{
				SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, j + 1, steps[j]);
				o.result = CalibPathStepResult::Skipped;
				o.failCategory = CalibPathFailCategory::UserStop;
				o.failStage = "user stop";
				PushPathFailureOutcome(o);
			}
			break;
		}
		SPathStep& st = steps[i];
		CString verr;
		if (!ValidatePathStep(st, verr))
		{
			SafeAppendLog(verr);
			{
				SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, i + 1, st);
				o.result = CalibPathStepResult::Skipped;
				o.failCategory = CalibPathFailCategory::CsvValidation;
				o.failStage = "CSV validation";
				o.commDetail = M576CStringToUtf8(verr);
				TruncatePathOutcomeDetail(o.commDetail);
				PushPathFailureOutcome(o);
			}
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		CString wslot;
		if (!PmStepMatchesFileSlot(st, fileSlot, &wslot))
		{
			SafeAppendLog(wslot);
			{
				SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, i + 1, st);
				o.result = CalibPathStepResult::Skipped;
				o.failCategory = CalibPathFailCategory::CsvValidation;
				o.failStage = "file slot filter";
				o.commDetail = M576CStringToUtf8(wslot);
				TruncatePathOutcomeDetail(o.commDetail);
				PushPathFailureOutcome(o);
			}
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		const int idxOcc3 = (st.targetSwitchIndex == 3) ? occT3 : -1;
		const int idxOcc4 = (st.targetSwitchIndex == 4) ? occT4 : -1;
		if (st.targetSwitchIndex == 3)
			occT3++;
		else if (st.targetSwitchIndex == 4)
			occT4++;

		if (!m_pRecal->ExchangeRecal1ReadLine(st, lineOk, 3000, err))
		{
			if (err.IsEmpty())
				SafeAppendLog(_T("RECAL 1: no OK line after retries."));
			else
				SafeAppendLog(err);
			{
				SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, i + 1, st);
				o.result = CalibPathStepResult::Failed;
				o.failCategory = M576CommErrIsSerialWriteFailure(err)
					? CalibPathFailCategory::CommSerialBreak
					: CalibPathFailCategory::CommRecalSetup;
				o.failStage = "RECAL 1";
				o.commDetail = M576CStringToUtf8(err.IsEmpty() ? _T("RECAL 1: no OK line") : err);
				TruncatePathOutcomeDetail(o.commDetail);
				PushPathFailureOutcome(o);
			}
			if (M576CommErrIsSerialWriteFailure(err))
				break;
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		{
			CString msg;
			lineOk.Trim();
			msg.Format(_T("Step %d/%d (slot %d) RECAL 1 -> %s"), i + 1, total, fileSlot + 1, CString(lineOk));
			SafeAppendLog(msg);
			if (lineOk.CompareNoCase("OK") != 0)
			{
				msg.Format(_T("  (expected OK; continuing with RECAL 3)"));
				SafeAppendLog(msg);
			}
		}

		double xFixedDac = 0.0;
		std::vector<double> powY;
		int brForYBase = 0;
		M576::Peak1DValidateCode yPreCode = M576::Peak1DValidateCode::Ok;
		double tYPre = 0.0;
		int yPreAttemptsPm = 0;
		int ySweepRangePm = m_dacRange;
		M576::Peak1DFitTrace yPreTracePm;
		int yMovingBasePm = M576_RECAL_FW_READ_BASE_DAC;
		int ySeedDacRangePm = m_dacRange;
		int yCrossPrevArgmaxPm = -1;
		bool yCrossMonoRangeExpandedPm = false;

		std::vector<double> powX;
		double sweep1LineCol0 = 0.0;
		int br = 0;
		int bc = 0;
		M576::Peak1DValidateCode yCross = M576::Peak1DValidateCode::Ok;
		M576::Peak1DValidateCode xCross = M576::Peak1DValidateCode::Ok;
		double tYPm = 0.0;
		double tXPm = 0.0;
		M576::Peak1DFitTrace trCrossYPm;
		M576::Peak1DFitTrace trCrossXPm;
		BOOL crossOk = FALSE;
		BOOL pmSkipStep = FALSE;
		BOOL pmBreakPath = FALSE;
		BOOL pmFailRecorded = FALSE;
		int fixedY = 0;
		int xSweepRange = m_dacRange;
		int lastMovingX = M576_RECAL_FW_READ_BASE_DAC;
		int lastAttemptDacRangeX = m_dacRange;
		int lastCrossRound = 0;
		double pmRejectDbm = 0.0;
		double pmRejectLo = 0.0;
		double pmRejectHi = 0.0;
		int pmRejectIdx = -1;
		int pmRejectRangeIdx = -1;

		for (int yCrossRound = 0;
			yCrossRound < (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS && !crossOk && !pmSkipStep && !pmBreakPath;
			++yCrossRound)
		{
			lastCrossRound = yCrossRound;
			if (yCrossRound > 0)
			{
				CString msg;
				msg.Format(
					_T("  cross retry: re-sweep RECAL 3 0 round %d/%d (baseY=%d offset=%d)"),
					yCrossRound + 1,
					(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
					yMovingBasePm,
					ySeedDacRangePm);
				SafeAppendLog(msg);
			}

			if (!RunRecal1DSweepWithPeakRecenterRetry(
					TRUE,
					m_pmRangeIndex,
					0,
					M576_RECAL_FW_READ_BASE_DAC,
					yMovingBasePm,
					ySeedDacRangePm,
					readTimeout1d,
					_T("Y"),
					_T("RECAL 3 0"),
					powY,
					xFixedDac,
					brForYBase,
					yPreCode,
					yPreTracePm,
					tYPre,
					yPreAttemptsPm,
					ySweepRangePm,
					err))
			{
				if (yCrossRound == 0)
				{
					if (M576CommErrIsSerialWriteFailure(err))
					{
						SafeAppendLog(err);
						pmBreakPath = TRUE;
					}
					else if (!err.IsEmpty())
						SafeAppendLog(err);
					else if (powY.empty())
						SafeAppendLog(_T("RECAL 3 0 (Y sweep): no line after retries."));
					if (!powY.empty() && yPreCode != M576::Peak1DValidateCode::Ok)
					{
						CString retryInfo;
						retryInfo.Format(
							_T("  peak retry summary: attempts=%d, retries=%d."),
							yPreAttemptsPm,
							(yPreAttemptsPm > 0) ? (yPreAttemptsPm - 1) : 0);
						SafeAppendLog(retryInfo);
						SafeAppendLog(M576FormatPeak1DMsg(true, M576Peak1DLogStage::YPre, yPreCode));
					}
					else if (powY.empty())
						SafeAppendLog(_T("  RECAL 3 1: no Y samples; skip X sweep (RECAL 3 1)."));
					pmSkipStep = TRUE;
					if (!pmFailRecorded)
					{
						SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, i + 1, st);
						o.result = CalibPathStepResult::Failed;
						o.crossRound = yCrossRound;
						o.lastBaseY = yMovingBasePm;
						o.lastOffsetY = ySeedDacRangePm;
						o.sweepCol0Y = xFixedDac;
						o.sampleCountY = (int)powY.size();
						M576FillSweepPowContext(o, powY, true);
						if (!powY.empty() && yPreCode != M576::Peak1DValidateCode::Ok)
						{
							o.failCategory = CalibPathFailCategory::YPrePeak;
							o.failStage = "Y pre-sweep";
							o.peakAttempts = yPreAttemptsPm;
							M576FillPeakCodeFields(o, yPreCode);
							o.tPeakY = tYPre;
							o.hasTPeakY = std::isfinite(tYPre);
						}
						else
						{
							o.failCategory = powY.empty()
								? CalibPathFailCategory::CommSweep
								: CalibPathFailCategory::CommSerialBreak;
							o.failStage = powY.empty() ? "RECAL 3 0 sweep" : "serial";
							o.commDetail = M576CStringToUtf8(err);
							TruncatePathOutcomeDetail(o.commDetail);
						}
						PushPathFailureOutcome(o);
						pmFailRecorded = TRUE;
					}
				}
				else
					SafeAppendLog(_T("  cross retry: RECAL 3 0 re-sweep failed."));
				break;
			}
			{
				CString msg;
				msg.Format(_T("  RECAL 3 0 -> %d power samples, sweep col0=%.4g (first cell)"),
					(int)powY.size(), xFixedDac);
				SafeAppendLog(msg);
			}
			M576AppendPeakFitTraceLog(this, _T("RECAL3 Y预瞄(PM) "), yPreTracePm);
			const int nY = (int)powY.size();
			fixedY = RecalDacAtPeakIndexFromSweepCol0(tYPre, nY, ySweepRangePm, xFixedDac);
			{
				CString msg;
				msg.Format(_T("  RECAL 3 1 Base DAC (Y@peak, row=%d)=%d"), brForYBase, fixedY);
				SafeAppendLog(msg);
			}
			ySeedDacRangePm = ySweepRangePm;

			int movingX = M576_RECAL_FW_READ_BASE_DAC;
			const int uiFineRangeX = m_dacRange;
			M576::SweepRecenterSessionState xRetryState = {};
			M576::InitSweepRecenterSessionState(xRetryState, uiFineRangeX, movingX);
			int attemptDacRangeX = (ySweepRangePm > m_dacRange) ? ySweepRangePm : m_dacRange;
			xRetryState.attemptRange = attemptDacRangeX;
			DWORD xAttemptTimeout = ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRangeX, m_dacStep);
			BOOL retryYAfterCross = FALSE;

			for (int xAttempt = 0; xAttempt < (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS; ++xAttempt)
			{
				if (!m_pRecal->ExchangeRecal3ReadSweep(
						1, movingX, fixedY, attemptDacRangeX, m_dacStep, m_delayMs, lineX, xAttemptTimeout, err))
				{
					if (err.IsEmpty())
						SafeAppendLog(_T("RECAL 3 1 (X sweep): no line after retries."));
					else
						SafeAppendLog(err);
					pmSkipStep = TRUE;
					if (M576CommErrIsSerialWriteFailure(err))
						pmBreakPath = TRUE;
					if (!pmFailRecorded)
					{
						SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, i + 1, st);
						o.result = CalibPathStepResult::Failed;
						o.failCategory = M576CommErrIsSerialWriteFailure(err)
							? CalibPathFailCategory::CommSerialBreak
							: CalibPathFailCategory::CommSweep;
						o.failStage = "RECAL 3 1 sweep";
						o.crossRound = yCrossRound;
						o.lastBaseY = yMovingBasePm;
						o.lastBaseX = movingX;
						o.lastOffsetY = ySeedDacRangePm;
						o.lastOffsetX = attemptDacRangeX;
						o.commDetail = M576CStringToUtf8(err.IsEmpty() ? _T("RECAL 3 1: no line") : err);
						TruncatePathOutcomeDetail(o.commDetail);
						PushPathFailureOutcome(o);
						pmFailRecorded = TRUE;
					}
					break;
				}
				if (!CRecalSession::ParseRecal3SweepLine(lineX, sweep1LineCol0, powX))
				{
					SafeAppendLog(_T("RECAL 3 1: could not parse [axis0] P1..Pn."));
					pmSkipStep = TRUE;
					if (!pmFailRecorded)
					{
						SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, i + 1, st);
						o.result = CalibPathStepResult::Failed;
						o.failCategory = CalibPathFailCategory::CommSweep;
						o.failStage = "RECAL 3 1 parse";
						o.crossRound = yCrossRound;
						o.commDetail = "could not parse sweep line";
						PushPathFailureOutcome(o);
						pmFailRecorded = TRUE;
					}
					break;
				}
				{
					CString msg;
					msg.Format(
						_T("  RECAL 3 1 -> %d power samples, sweep col0=%.4g (baseX=%d baseY=%d offset=%d Y@peak fixed)"),
						(int)powX.size(),
						sweep1LineCol0,
						movingX,
						fixedY,
						attemptDacRangeX);
					SafeAppendLog(msg);
				}
				if (xAttempt == 0 && yCrossRound == 0
					&& ((int)powY.size() != gridN || (int)powX.size() != gridN))
				{
					CString msg;
					msg.Format(_T("  warning: firmware power count (%d, %d) vs host AxisPointCount estimate %d (range=%d step=%d); OK if FW grid differs."),
						(int)powY.size(), (int)powX.size(), gridN, m_dacRange, m_dacStep);
					SafeAppendLog(msg);
				}
				if (powY.size() != powX.size() || powY.empty())
				{
					SafeAppendLog(_T("  peak: Y/X sweep lengths differ or empty; skip LUT update."));
					if (!pmFailRecorded)
					{
						SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, i + 1, st);
						o.result = CalibPathStepResult::Failed;
						o.failCategory = CalibPathFailCategory::SweepDataMismatch;
						o.failStage = "Y/X length";
						o.crossRound = yCrossRound;
						o.sampleCountY = (int)powY.size();
						o.sampleCountX = (int)powX.size();
						PushPathFailureOutcome(o);
						pmFailRecorded = TRUE;
					}
					break;
				}
				lastMovingX = movingX;
				lastAttemptDacRangeX = attemptDacRangeX;
				yCross = M576::Peak1DValidateCode::Ok;
				xCross = M576::Peak1DValidateCode::Ok;
				const M576::Peak1DFitPolicy crossPolicyY =
					M576::Peak1DFitPolicyForSweepResult(ySweepRangePm, m_dacRange);
				const M576::Peak1DFitPolicy crossPolicyX =
					M576::Peak1DFitPolicyForCrossAxis(xRetryState, attemptDacRangeX, uiFineRangeX);
				crossOk = M576::PeakCrossFrom1DScans(
					powY,
					powX,
					br,
					bc,
					&yCross,
					&xCross,
					&tYPm,
					&tXPm,
					&trCrossYPm,
					&trCrossXPm,
					crossPolicyY,
					crossPolicyX);
				if (crossOk && trCrossYPm.usedArgmaxFallback)
				{
					CString msg;
					msg.Format(
						_T("  fine refine: RECAL 3 0 Y cross cubic fallback to argmax t*=%.4g idx=%d"),
						tYPm,
						br);
					SafeAppendLog(msg);
				}
				if (crossOk && trCrossXPm.usedArgmaxFallback)
				{
					CString msg;
					msg.Format(
						_T("  fine refine: RECAL 3 1 X cubic fallback to argmax t*=%.4g idx=%d"),
						tXPm,
						bc);
					SafeAppendLog(msg);
				}
				if (crossOk)
				{
					if (M576::NeedsFineRefineAfterSuccess(attemptDacRangeX, uiFineRangeX)
						&& !xRetryState.fineConsumed
						&& RecalSweepPowerSampleCount(uiFineRangeX, m_dacStep) == (int)powY.size())
					{
						const M576::SweepRetryPlan fineX = M576::PlanFineRefineAfterCoarseSuccess(
							xRetryState, sweep1LineCol0, tXPm, (int)powX.size(), attemptDacRangeX);
						if (fineX.action == M576::SweepRetryAction::FineRefine)
						{
							M576::ApplySweepRetryPlan(xRetryState, fineX);
							movingX = xRetryState.movingBase = fineX.nextBase;
							attemptDacRangeX = xRetryState.attemptRange;
							xAttemptTimeout = ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRangeX, m_dacStep);
							CString msg;
							msg.Format(
								_T("  fine refine: RECAL 3 1 X attempt %d/%d offset %d->%d newMovingX=%d fixedY=%d"),
								xAttempt + 1,
								(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
								(int)M576_PEAK1D_COARSE_DAC_RANGE,
								attemptDacRangeX,
								movingX,
								fixedY);
							SafeAppendLog(msg);
							continue;
						}
					}
					xSweepRange = attemptDacRangeX;
					break;
				}
				if (yCross != M576::Peak1DValidateCode::Ok)
				{
					SafeAppendLog(M576FormatPeak1DMsg(true, M576Peak1DLogStage::YCross, yCross));
					const BOOL yCrossLastRound =
						(yCrossRound >= (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS - 1);
					bool usedExpand = false;
					if (!yCrossLastRound
						&& M576::PlanRecalYCrossResweep(
							yCross,
							powY,
							RecalSweepCenterFromCol0(xFixedDac, ySweepRangePm),
							yCrossRound,
							yMovingBasePm,
							ySeedDacRangePm,
							yCrossPrevArgmaxPm,
							tYPm,
							std::isfinite(tYPm),
							usedExpand,
							yCrossMonoRangeExpandedPm))
					{
						CString msg;
						msg.Format(
							_T("  cross retry: Y rejected — next RECAL 3 0 %s baseY=%d offset=%d"),
							usedExpand ? _T("expand,") : _T("recenter,"),
							yMovingBasePm,
							ySeedDacRangePm);
						SafeAppendLog(msg);
						retryYAfterCross = TRUE;
					}
					break;
				}
				if (xCross != M576::Peak1DValidateCode::Ok)
					SafeAppendLog(M576FormatPeak1DMsg(true, M576Peak1DLogStage::XCross, xCross));
				const int nX = (int)powX.size();
				const M576::SweepProfile xProfile = M576::AnalyzeRecal1DSweepProfile(powX);
				const BOOL xLastAttempt = (xAttempt >= (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS - 1);
				M576::SweepRecenterFailureInfo xFailInfo = {};
				xFailInfo.code = xCross;
				double tXPeak = 0.0;
				int xIdxTmp = 0;
				M576::Peak1DValidateCode xFitCode = xCross;
				(void)M576::FindUnimodalPeak1DIndex(powX, xIdxTmp, xFitCode, &tXPeak, nullptr);
				xFailInfo.tPeak = tXPeak;
				xFailInfo.hasTPeak = std::isfinite(tXPeak);
				xFailInfo.prevArgmaxIndex = xRetryState.prevArgmax;
				xFailInfo.hasPrevAttempt = (xAttempt > 0);
				xRetryState.movingBase = movingX;
				xRetryState.attemptRange = attemptDacRangeX;
				const double xCenter = RecalSweepCenterFromCol0(sweep1LineCol0, attemptDacRangeX);
				const M576::SweepRetryPlan xPlan = M576::PlanNextRecal1DSweepAttempt(
					xRetryState, xCross, xProfile, powX, xCenter, xAttempt, xLastAttempt, xFailInfo);
				if (xPlan.action == M576::SweepRetryAction::GiveUp)
					break;
				if (RecalSweepPowerSampleCount(xPlan.nextRange, m_dacStep) != (int)powY.size())
					break;
				const int prevRangeX = attemptDacRangeX;
				const int prevBaseX = movingX;
				M576::ApplySweepRetryPlan(xRetryState, xPlan);
				movingX = xRetryState.movingBase;
				attemptDacRangeX = xRetryState.attemptRange;
				xAttemptTimeout = ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRangeX, m_dacStep);
				{
					CString msg;
					if (xPlan.action == M576::SweepRetryAction::JumpFlatMax)
					{
						msg.Format(
							_T("  %hs RECAL 3 1 X attempt %d/%d code=%hs trend=%hs span=%.4g offset %d->%d"),
							M576::SweepRetryActionLogTag(xPlan.action),
							xAttempt + 1,
							(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
							M576Peak1DWhy(xCross),
							M576::SweepTrendName(xProfile.trend),
							xProfile.span,
							prevRangeX,
							attemptDacRangeX);
					}
					else
					{
						const M576::SweepProfile xRecenterProfile =
							M576::AdjustProfileForMonoRecenter(xProfile, powX, xRetryState.inCoarsePhase, &xFailInfo);
						const double deltaDac = M576::SuggestSweepRecenterDeltaDac(
							xRecenterProfile, nX, attemptDacRangeX, xAttempt, xFailInfo);
						msg.Format(
							_T("  %hs RECAL 3 1 X attempt %d/%d code=%hs trend=%hs argmax=%d t*=%.4g span=%.4g offset=%d deltaDac=%.4g newMovingX=%d fixedY=%d (%d->%d)"),
							M576::SweepRetryActionLogTag(xPlan.action),
							xAttempt + 1,
							(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
							M576Peak1DWhy(xCross),
							M576::SweepTrendName(xRecenterProfile.trend),
							xRecenterProfile.argmaxIndex,
							tXPeak,
							xRecenterProfile.span,
							attemptDacRangeX,
							deltaDac,
							movingX,
							fixedY,
							prevBaseX,
							movingX);
					}
					SafeAppendLog(msg);
				}
				xRetryState.prevArgmax = xProfile.argmaxIndex;
			}

			if (retryYAfterCross)
				continue;
			break;
		}

		if (pmBreakPath)
			break;
		if (pmSkipStep)
		{
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}

		if (crossOk && m_pmRangeIndex != M576_MAX_PM_RANGE)
		{
			auto checkCrossAxisPmRange = [&](const std::vector<double>& pow, const M576::Peak1DFitTrace& tr,
				M576::Peak1DValidateCode& axisCode, LPCTSTR stageLabel, LPCTSTR axisTag, double col0) -> BOOL
			{
				const int peakHint = (tr.globalMaxIndex >= 0) ? tr.globalMaxIndex : 0;
				double peakRaw = 0.0, peakDbm = 0.0, loDbm = 0.0, hiDbm = 0.0;
				int peakIdxUsed = -1;
				if (M576::ValidatePeakPowerInPmRange(
						m_pmRangeIndex, pow, peakHint, peakRaw, peakDbm, loDbm, hiDbm, peakIdxUsed))
					return TRUE;
				axisCode = M576::Peak1DValidateCode::PmRangeMismatch;
				pmRejectDbm = peakDbm;
				pmRejectLo = loDbm;
				pmRejectHi = hiDbm;
				pmRejectIdx = peakIdxUsed;
				pmRejectRangeIdx = m_pmRangeIndex;
				M576AppendPmRangeRejectLog(
					this, m_pmRangeIndex, stageLabel, axisTag, col0, pow, peakIdxUsed, peakRaw, peakDbm, loDbm, hiDbm);
				return FALSE;
			};
			if (!checkCrossAxisPmRange(powY, trCrossYPm, yCross, _T("RECAL 3 0"), _T("Y"), xFixedDac))
			{
				SafeAppendLog(M576FormatPeak1DMsg(true, M576Peak1DLogStage::YCross, yCross));
				crossOk = FALSE;
			}
			else if (!checkCrossAxisPmRange(powX, trCrossXPm, xCross, _T("RECAL 3 1"), _T("X"), sweep1LineCol0))
			{
				SafeAppendLog(M576FormatPeak1DMsg(true, M576Peak1DLogStage::XCross, xCross));
				crossOk = FALSE;
			}
		}

		if (crossOk)
		{
			M576AppendPeakFitTraceLog(this, _T("RECAL3 交叉 Y轴(PM) "), trCrossYPm);
			M576AppendPeakFitTraceLog(this, _T("RECAL3 交叉 X轴(PM) "), trCrossXPm);
			const int nLut = (int)powY.size();
			const double rawDacXAtPeak = SweepCol0PlusPeakOffsetDac(xFixedDac, tYPm, nLut, ySweepRangePm);
			const double rawDacYAtPeak = SweepCol0PlusPeakOffsetDac(sweep1LineCol0, tXPm, nLut, xSweepRange);
			const int rawXi = static_cast<int>(std::lround(rawDacXAtPeak));
			const int rawYi = static_cast<int>(std::lround(rawDacYAtPeak));
			{
				CString msg;
				msg.Format(
					_T("  -> peak row=%d col=%d; linear DAC at cross-peak: Y=%.4g (RECAL3 0 col0 + %d*step), X=%.4g (RECAL3 1 col0 + %d*step)"),
					br,
					bc,
					rawDacXAtPeak,
					br,
					rawDacYAtPeak,
					bc);
				SafeAppendLog(msg);
			}
			SDacU16 dacU;
			RawCrossPeakDacToU16Pair((double)rawXi, (double)rawYi, dacU.uX, dacU.uY);
			if (fileSlot < 2)
			{
				ApplyRecalPeakToLut(
					st, idxOcc3, idxOcc4, dacU.uX, dacU.uY, mcsTempSlot, m_lutByTrans[fileSlot]);
				SCalibrationStatRow srow;
				if (CalibBuildStatRowPmLut(
						st,
						idxOcc3,
						idxOcc4,
						fileSlot,
						i + 1,
						br,
						bc,
						nLut,
						rawXi,
						rawYi,
						dacU,
						mcsTempSlot,
						srow))
					PushCalibStatRow(srow);
			}
			else
			{
				const SMems1x64PmMapRow& mr = map1x64Rows[i];
				WriteMems1x64DacPair(
					m_mems1x64[fileSlot - 2],
					mr.sw1to4 - 1,
					mr.chY1based - 1,
					dacU.uX,
					dacU.uY,
					memsCalibSlot);
				SCalibrationStatRow srow;
				if (CalibBuildStatRowPmMemsMapped(
						st,
						fileSlot,
						i + 1,
						br,
						bc,
						nLut,
						rawXi,
						rawYi,
						dacU,
						mr.sw1to4 - 1,
						mr.chY1based - 1,
						memsCalibSlot,
						srow))
					PushCalibStatRow(srow);
			}
		}
		else if (!pmFailRecorded)
		{
			SCalibPathStepOutcome o = M576MakePmOutcome(fileSlot, csvBasename, i + 1, st);
			o.result = CalibPathStepResult::Failed;
			o.crossRound = lastCrossRound;
			o.lastBaseY = yMovingBasePm;
			o.lastBaseX = lastMovingX;
			o.lastOffsetY = ySeedDacRangePm;
			o.lastOffsetX = lastAttemptDacRangeX;
			o.sweepCol0Y = xFixedDac;
			o.sweepCol0X = sweep1LineCol0;
			o.sampleCountY = (int)powY.size();
			o.sampleCountX = (int)powX.size();
			M576FillSweepPowContext(o, powY, true);
			M576FillSweepPowContext(o, powX, false);
			o.tPeakY = tYPm;
			o.tPeakX = tXPm;
			o.hasTPeakY = std::isfinite(tYPm);
			o.hasTPeakX = std::isfinite(tXPm);
			if (yCross == M576::Peak1DValidateCode::PmRangeMismatch
				|| xCross == M576::Peak1DValidateCode::PmRangeMismatch)
			{
				o.failCategory = CalibPathFailCategory::PmRangeMismatch;
				o.failStage = (yCross == M576::Peak1DValidateCode::PmRangeMismatch) ? "PM range Y" : "PM range X";
				M576FillPeakCodeFields(o, M576::Peak1DValidateCode::PmRangeMismatch);
				o.peakDbm = pmRejectDbm;
				o.loDbm = pmRejectLo;
				o.hiDbm = pmRejectHi;
				o.peakIdxUsed = pmRejectIdx;
				o.pmRangeIndex = pmRejectRangeIdx;
			}
			else if (yCross != M576::Peak1DValidateCode::Ok)
			{
				o.failCategory = CalibPathFailCategory::YCrossPeak;
				o.failStage = "cross Y";
				o.peakAttempts = (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS;
				M576FillPeakCodeFields(o, yCross);
			}
			else if (xCross != M576::Peak1DValidateCode::Ok)
			{
				o.failCategory = CalibPathFailCategory::XCrossPeak;
				o.failStage = "cross X";
				o.peakAttempts = (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS;
				M576FillPeakCodeFields(o, xCross);
			}
			else if (yPreCode != M576::Peak1DValidateCode::Ok)
			{
				o.failCategory = CalibPathFailCategory::YPrePeak;
				o.failStage = "Y pre-sweep";
				o.peakAttempts = yPreAttemptsPm;
				M576FillPeakCodeFields(o, yPreCode);
			}
			else
			{
				o.failCategory = CalibPathFailCategory::XCrossPeak;
				o.failStage = "cross peak";
				o.peakAttempts = (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS;
				o.commDetail = "peak not found after retries";
			}
			PushPathFailureOutcome(o);
			pmFailRecorded = TRUE;
		}

		++globalProgress;
		SafeSetProgressPos(globalProgress);
	}
	(void)globalTotal;
}

// --- PD 定标：无 RECAL0，仅 RECAL2/5 与双轴寻峰，分 pd_*.csv 槽 ---

void CM576CalibratorDlg::RunPathPd()
{
	ClearCalibStats();
	ClearPathOutcomes();
	CString err;
	int totalAll = 0;
	for (int fs = 0; fs < 4; ++fs)
	{
		const CString rel = m_strCsvPd[fs].Trim();
		if (rel.IsEmpty())
			continue;
		const CString abs = ResolveFilePath(rel);
		if (GetFileAttributes(abs) == INVALID_FILE_ATTRIBUTES)
			continue;
		CArray<SPathStepPd, SPathStepPd const&> tmp;
		if (!LoadPathCsvPd(abs, tmp, err))
			continue;
		totalAll += (int)tmp.GetSize();
	}
	if (totalAll == 0)
	{
		SafeAppendLog(_T("PD: no CSV rows (missing paths or empty files)."));
		return;
	}
	SafeSetProgressRange(0, totalAll);
	for (int li = 0; li < 4; ++li)
		ZeroMemory(&m_lutByTrans[li], sizeof(m_lutByTrans[li]));
	ZeroMemory(m_mems1x64, sizeof(m_mems1x64));
	{
		CString preloadErr;
		if (!PreloadRunPathBackupOrFail(preloadErr))
		{
			AbortRunPathPreloadFailed(preloadErr);
			return;
		}
	}
	int wavelengthNm = M576_DEFAULT_WAVELENGTH_NM;
	{
		CString wlErr;
		if (ParseWavelengthNm(m_strWavelength, wavelengthNm, wlErr))
			StartSessionCalibPolicyFromWavelength(wavelengthNm);
		else
			StartSessionCalibPolicyFromWavelength(M576_DEFAULT_WAVELENGTH_NM);
	}
	//if (!ExchangeSwlBeforeRunPath(wavelengthNm, err))
	//{
	//	SafeAppendLog(err.IsEmpty() ? _T("Run Path: SWL (set TLS/wavelength) failed.") : err);
	//	return;
	//}
	ExportRunPathBackupDacSnapshotCsvIfBackupBaseSet();

	/// PD: Command C only (RECAL 2 + RECAL 5). No Command A (RECAL 0).

	int globalProgress = 0;
	for (int fs = 0; fs < 4; ++fs)
	{
		const CString rel = m_strCsvPd[fs].Trim();
		if (rel.IsEmpty())
		{
			CString m;
			m.Format(_T("PD slot %d: path empty, skip."), fs + 1);
			SafeAppendLog(m);
			continue;
		}
		const CString abs = ResolveFilePath(rel);
		if (GetFileAttributes(abs) == INVALID_FILE_ATTRIBUTES)
		{
			CString m;
			m.Format(_T("PD slot %d: file not found, skip: %s"), fs + 1, rel.GetString());
			SafeAppendLog(m);
			continue;
		}
		CArray<SPathStepPd, SPathStepPd const&> steps;
		if (!LoadPathCsvPd(abs, steps, err))
		{
			SafeAppendLog(err);
			continue;
		}
		if (steps.GetSize() == 0)
			continue;
		{
			CString m;
			m.Format(_T("PD slot %d (%s): %d rows"), fs + 1, rel.GetString(), (int)steps.GetSize());
			SafeAppendLog(m);
		}
		int occT3 = 0;
		int occT4 = 0;
		RunPathPdFile(fs, steps, globalProgress, totalAll, occT3, occT4);
	}
	SafeAppendLog(_T("Path run finished (PD all slots)."));
	{
		std::lock_guard<std::mutex> lock(m_statsRowsMutex);
		CString m;
		m.Format(
			_T("Calibration stats rows recorded: %d (use Export calib stats CSV)."), (int)m_statsRows.size());
		SafeAppendLog(m);
	}
}

// 单路 pd_*.csv：RECAL2/5、Apply 到 m_lutByTrans[fileSlot]（PD 目标语义）。

void CM576CalibratorDlg::RunPathPdFile(int fileSlot, CArray<SPathStepPd, SPathStepPd const&>& steps, int& globalProgress, int globalTotal, int& occT3, int& occT4)
{
	const int mcsTempSlot = McsPrimaryTempSlot(m_sessionCalibPolicy);
	const int memsCalibSlot = MemsPrimaryCalibSlot(m_sessionCalibPolicy);
	CString err;
	const int total = (int)steps.GetSize();
	const CString pdAbs = ResolveFilePath(m_strCsvPd[fileSlot].Trim());
	const std::string csvBasename = M576PathBasenameUtf8(pdAbs);
	const DWORD readTimeout1d = ComputeRecal1DReadTimeoutMs(m_delayMs, m_dacRange, m_dacStep);
	const int gridN = AxisPointCount(m_dacRange, m_dacStep);
	CStringA lineOk, lineY, lineX;
	for (int i = 0; i < total; ++i)
	{
		if (m_bStop)
		{
			for (int j = i; j < total; ++j)
			{
				SCalibPathStepOutcome o = M576MakePdOutcome(fileSlot, csvBasename, j + 1, steps[j]);
				o.result = CalibPathStepResult::Skipped;
				o.failCategory = CalibPathFailCategory::UserStop;
				o.failStage = "user stop";
				PushPathFailureOutcome(o);
			}
			break;
		}
		SPathStepPd& st = steps[i];
		CString verr;
		if (!ValidatePathStepPd(st, verr))
		{
			SafeAppendLog(verr);
			{
				SCalibPathStepOutcome o = M576MakePdOutcome(fileSlot, csvBasename, i + 1, st);
				o.result = CalibPathStepResult::Skipped;
				o.failCategory = CalibPathFailCategory::CsvValidation;
				o.failStage = "CSV validation";
				o.commDetail = M576CStringToUtf8(verr);
				TruncatePathOutcomeDetail(o.commDetail);
				PushPathFailureOutcome(o);
			}
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		CString wslot;
		if (!PdStepMatchesFileSlot(st, fileSlot, &wslot))
		{
			SafeAppendLog(wslot);
			{
				SCalibPathStepOutcome o = M576MakePdOutcome(fileSlot, csvBasename, i + 1, st);
				o.result = CalibPathStepResult::Skipped;
				o.failCategory = CalibPathFailCategory::CsvValidation;
				o.failStage = "file slot filter";
				o.commDetail = M576CStringToUtf8(wslot);
				TruncatePathOutcomeDetail(o.commDetail);
				PushPathFailureOutcome(o);
			}
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		const int idxOcc3 = (st.targetSwitchIndex == 3) ? occT3 : -1;
		const int idxOcc4 = (st.targetSwitchIndex == 4) ? occT4 : -1;
		if (st.targetSwitchIndex == 3)
			occT3++;
		else if (st.targetSwitchIndex == 4)
			occT4++;

		if (!m_pRecal->ExchangeRecal2ReadLine(st, lineOk, 3000, err))
		{
			if (err.IsEmpty())
				SafeAppendLog(_T("RECAL 2: no OK line after retries."));
			else
				SafeAppendLog(err);
			{
				SCalibPathStepOutcome o = M576MakePdOutcome(fileSlot, csvBasename, i + 1, st);
				o.result = CalibPathStepResult::Failed;
				o.failCategory = M576CommErrIsSerialWriteFailure(err)
					? CalibPathFailCategory::CommSerialBreak
					: CalibPathFailCategory::CommRecalSetup;
				o.failStage = "RECAL 2";
				o.commDetail = M576CStringToUtf8(err.IsEmpty() ? _T("RECAL 2: no OK line") : err);
				TruncatePathOutcomeDetail(o.commDetail);
				PushPathFailureOutcome(o);
			}
			if (M576CommErrIsSerialWriteFailure(err))
				break;
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		{
			CString msg;
			lineOk.Trim();
			msg.Format(_T("PD step %d/%d (slot %d) RECAL 2 -> %s"), i + 1, total, fileSlot + 1, CString(lineOk));
			SafeAppendLog(msg);
			if (lineOk.CompareNoCase("OK") != 0)
			{
				msg.Format(_T("  (expected OK; continuing with RECAL 5)"));
				SafeAppendLog(msg);
			}
		}

		double xFixedDacPd = 0.0;
		std::vector<double> powY;
		int brForYBasePd = 0;
		M576::Peak1DValidateCode yPreCodePd = M576::Peak1DValidateCode::Ok;
		double tYPrePd = 0.0;
		int yPreAttemptsPd = 0;
		int ySweepRangePd = m_dacRange;
		M576::Peak1DFitTrace yPreTracePd;
		int yMovingBasePd = M576_RECAL_FW_READ_BASE_DAC;
		int ySeedDacRangePd = m_dacRange;
		int yCrossPrevArgmaxPd = -1;
		bool yCrossMonoRangeExpandedPd = false;

		double sweep1LineCol0Pd = 0.0;
		std::vector<double> powX;
		int br = 0;
		int bc = 0;
		M576::Peak1DValidateCode yCrossPd = M576::Peak1DValidateCode::Ok;
		M576::Peak1DValidateCode xCrossPd = M576::Peak1DValidateCode::Ok;
		double tYpd = 0.0;
		double tXpd = 0.0;
		M576::Peak1DFitTrace trCrossYPd;
		M576::Peak1DFitTrace trCrossXPd;
		BOOL crossOkPd = FALSE;
		BOOL pdSkipStep = FALSE;
		BOOL pdBreakPath = FALSE;
		BOOL pdFailRecorded = FALSE;
		int fixedYPd = 0;
		int xSweepRangePd = m_dacRange;
		int lastMovingXPd = M576_RECAL_FW_READ_BASE_DAC;
		int lastAttemptDacRangeXPd = m_dacRange;
		int lastCrossRoundPd = 0;

		for (int yCrossRoundPd = 0;
			yCrossRoundPd < (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS && !crossOkPd && !pdSkipStep && !pdBreakPath;
			++yCrossRoundPd)
		{
			lastCrossRoundPd = yCrossRoundPd;
			if (yCrossRoundPd > 0)
			{
				CString msg;
				msg.Format(
					_T("  cross retry: re-sweep RECAL 5 0 round %d/%d (baseY=%d offset=%d)"),
					yCrossRoundPd + 1,
					(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
					yMovingBasePd,
					ySeedDacRangePd);
				SafeAppendLog(msg);
			}

			if (!RunRecal1DSweepWithPeakRecenterRetry(
					FALSE,
					M576_MAX_PM_RANGE,
					0,
					M576_RECAL_FW_READ_BASE_DAC,
					yMovingBasePd,
					ySeedDacRangePd,
					readTimeout1d,
					_T("Y"),
					_T("RECAL 5 0"),
					powY,
					xFixedDacPd,
					brForYBasePd,
					yPreCodePd,
					yPreTracePd,
					tYPrePd,
					yPreAttemptsPd,
					ySweepRangePd,
					err))
			{
				if (yCrossRoundPd == 0)
				{
					if (M576CommErrIsSerialWriteFailure(err))
					{
						SafeAppendLog(err);
						pdBreakPath = TRUE;
					}
					else if (!err.IsEmpty())
						SafeAppendLog(err);
					else if (powY.empty())
						SafeAppendLog(_T("RECAL 5 0 (Y sweep): no line after retries."));
					if (!powY.empty() && yPreCodePd != M576::Peak1DValidateCode::Ok)
					{
						CString retryInfo;
						retryInfo.Format(
							_T("  peak retry summary: attempts=%d, retries=%d."),
							yPreAttemptsPd,
							(yPreAttemptsPd > 0) ? (yPreAttemptsPd - 1) : 0);
						SafeAppendLog(retryInfo);
						SafeAppendLog(M576FormatPeak1DMsg(false, M576Peak1DLogStage::YPre, yPreCodePd));
					}
					else if (powY.empty())
						SafeAppendLog(_T("  RECAL 5 1: no Y samples; skip X sweep (RECAL 5 1)."));
					pdSkipStep = TRUE;
				}
				else
					SafeAppendLog(_T("  cross retry: RECAL 5 0 re-sweep failed."));
				break;
			}
			{
				CString msg;
				msg.Format(_T("  RECAL 5 0 -> %d samples (X_start=%.4g)"), (int)powY.size(), xFixedDacPd);
				SafeAppendLog(msg);
			}
			M576AppendPeakFitTraceLog(this, _T("RECAL5 Y预瞄(PD) "), yPreTracePd);
			const int nYpd = (int)powY.size();
			fixedYPd = RecalDacAtPeakIndexFromSweepCol0(tYPrePd, nYpd, ySweepRangePd, xFixedDacPd);
			{
				CString msg;
				msg.Format(_T("  RECAL 5 1 Base DAC (Y@peak, row=%d)=%d"), brForYBasePd, fixedYPd);
				SafeAppendLog(msg);
			}
			ySeedDacRangePd = ySweepRangePd;

			int movingXPd = M576_RECAL_FW_READ_BASE_DAC;
			const int uiFineRangeXPd = m_dacRange;
			M576::SweepRecenterSessionState xRetryStatePd = {};
			M576::InitSweepRecenterSessionState(xRetryStatePd, uiFineRangeXPd, movingXPd);
			int attemptDacRangeXPd = (ySweepRangePd > m_dacRange) ? ySweepRangePd : m_dacRange;
			xRetryStatePd.attemptRange = attemptDacRangeXPd;
			DWORD xAttemptTimeoutPd = ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRangeXPd, m_dacStep);
			BOOL retryYAfterCrossPd = FALSE;

			for (int xAttempt = 0; xAttempt < (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS; ++xAttempt)
			{
				if (!m_pRecal->ExchangeRecal5ReadSweep(
						1, movingXPd, fixedYPd, attemptDacRangeXPd, m_dacStep, m_delayMs, lineX, xAttemptTimeoutPd, err))
				{
					if (err.IsEmpty())
						SafeAppendLog(_T("RECAL 5 1 (X sweep): no line after retries."));
					else
						SafeAppendLog(err);
					pdSkipStep = TRUE;
					if (M576CommErrIsSerialWriteFailure(err))
						pdBreakPath = TRUE;
					break;
				}
				if (!CRecalSession::ParseRecal3SweepLine(lineX, sweep1LineCol0Pd, powX))
				{
					SafeAppendLog(_T("RECAL 5 1: could not parse [axis0] P1..Pn."));
					pdSkipStep = TRUE;
					break;
				}
				{
					CString msg;
					msg.Format(
						_T("  RECAL 5 1 -> %d samples, sweep col0=%.4g (baseX=%d baseY=%d offset=%d Y@peak fixed)"),
						(int)powX.size(),
						sweep1LineCol0Pd,
						movingXPd,
						fixedYPd,
						attemptDacRangeXPd);
					SafeAppendLog(msg);
				}
				if (xAttempt == 0 && yCrossRoundPd == 0
					&& ((int)powY.size() != gridN || (int)powX.size() != gridN))
				{
					CString msg;
					msg.Format(_T("  warning: sample count (%d, %d) != expected axis points %d"),
						(int)powY.size(), (int)powX.size(), gridN);
					SafeAppendLog(msg);
				}
				if (powY.size() != powX.size() || powY.empty())
				{
					SafeAppendLog(_T("  peak: Y/X sweep lengths differ or empty; skip LUT update."));
					break;
				}
				lastMovingXPd = movingXPd;
				lastAttemptDacRangeXPd = attemptDacRangeXPd;
				yCrossPd = M576::Peak1DValidateCode::Ok;
				xCrossPd = M576::Peak1DValidateCode::Ok;
				const M576::Peak1DFitPolicy crossPolicyYPd =
					M576::Peak1DFitPolicyForSweepResult(ySweepRangePd, m_dacRange);
				const M576::Peak1DFitPolicy crossPolicyXPd =
					M576::Peak1DFitPolicyForCrossAxis(xRetryStatePd, attemptDacRangeXPd, uiFineRangeXPd);
				crossOkPd = M576::PeakCrossFrom1DScans(
					powY,
					powX,
					br,
					bc,
					&yCrossPd,
					&xCrossPd,
					&tYpd,
					&tXpd,
					&trCrossYPd,
					&trCrossXPd,
					crossPolicyYPd,
					crossPolicyXPd);
				if (crossOkPd && trCrossYPd.usedArgmaxFallback)
				{
					CString msg;
					msg.Format(
						_T("  fine refine: RECAL 5 0 Y cross cubic fallback to argmax t*=%.4g idx=%d"),
						tYpd,
						br);
					SafeAppendLog(msg);
				}
				if (crossOkPd && trCrossXPd.usedArgmaxFallback)
				{
					CString msg;
					msg.Format(
						_T("  fine refine: RECAL 5 1 X cubic fallback to argmax t*=%.4g idx=%d"),
						tXpd,
						bc);
					SafeAppendLog(msg);
				}
				if (crossOkPd)
				{
					if (M576::NeedsFineRefineAfterSuccess(attemptDacRangeXPd, uiFineRangeXPd)
						&& !xRetryStatePd.fineConsumed
						&& RecalSweepPowerSampleCount(uiFineRangeXPd, m_dacStep) == (int)powY.size())
					{
						const M576::SweepRetryPlan fineXPd = M576::PlanFineRefineAfterCoarseSuccess(
							xRetryStatePd, sweep1LineCol0Pd, tXpd, (int)powX.size(), attemptDacRangeXPd);
						if (fineXPd.action == M576::SweepRetryAction::FineRefine)
						{
							M576::ApplySweepRetryPlan(xRetryStatePd, fineXPd);
							movingXPd = xRetryStatePd.movingBase = fineXPd.nextBase;
							attemptDacRangeXPd = xRetryStatePd.attemptRange;
							xAttemptTimeoutPd = ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRangeXPd, m_dacStep);
							CString msg;
							msg.Format(
								_T("  fine refine: RECAL 5 1 X attempt %d/%d offset %d->%d newMovingX=%d fixedY=%d"),
								xAttempt + 1,
								(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
								(int)M576_PEAK1D_COARSE_DAC_RANGE,
								attemptDacRangeXPd,
								movingXPd,
								fixedYPd);
							SafeAppendLog(msg);
							continue;
						}
					}
					xSweepRangePd = attemptDacRangeXPd;
					break;
				}
				if (yCrossPd != M576::Peak1DValidateCode::Ok)
				{
					SafeAppendLog(M576FormatPeak1DMsg(false, M576Peak1DLogStage::YCross, yCrossPd));
					const BOOL yCrossLastRoundPd =
						(yCrossRoundPd >= (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS - 1);
					bool usedExpandPd = false;
					if (!yCrossLastRoundPd
						&& M576::PlanRecalYCrossResweep(
							yCrossPd,
							powY,
							RecalSweepCenterFromCol0(xFixedDacPd, ySweepRangePd),
							yCrossRoundPd,
							yMovingBasePd,
							ySeedDacRangePd,
							yCrossPrevArgmaxPd,
							tYpd,
							std::isfinite(tYpd),
							usedExpandPd,
							yCrossMonoRangeExpandedPd))
					{
						CString msg;
						msg.Format(
							_T("  cross retry: Y rejected — next RECAL 5 0 %s baseY=%d offset=%d"),
							usedExpandPd ? _T("expand,") : _T("recenter,"),
							yMovingBasePd,
							ySeedDacRangePd);
						SafeAppendLog(msg);
						retryYAfterCrossPd = TRUE;
					}
					break;
				}
				if (xCrossPd != M576::Peak1DValidateCode::Ok)
					SafeAppendLog(M576FormatPeak1DMsg(false, M576Peak1DLogStage::XCross, xCrossPd));
				const int nXpd = (int)powX.size();
				const M576::SweepProfile xProfilePd = M576::AnalyzeRecal1DSweepProfile(powX);
				const BOOL xLastAttemptPd = (xAttempt >= (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS - 1);
				M576::SweepRecenterFailureInfo xFailInfoPd = {};
				xFailInfoPd.code = xCrossPd;
				double tXPeakPd = 0.0;
				int xIdxTmpPd = 0;
				M576::Peak1DValidateCode xFitCodePd = xCrossPd;
				(void)M576::FindUnimodalPeak1DIndex(powX, xIdxTmpPd, xFitCodePd, &tXPeakPd, nullptr);
				xFailInfoPd.tPeak = tXPeakPd;
				xFailInfoPd.hasTPeak = std::isfinite(tXPeakPd);
				xFailInfoPd.prevArgmaxIndex = xRetryStatePd.prevArgmax;
				xFailInfoPd.hasPrevAttempt = (xAttempt > 0);
				xRetryStatePd.movingBase = movingXPd;
				xRetryStatePd.attemptRange = attemptDacRangeXPd;
				const double xCenterPd = RecalSweepCenterFromCol0(sweep1LineCol0Pd, attemptDacRangeXPd);
				const M576::SweepRetryPlan xPlanPd = M576::PlanNextRecal1DSweepAttempt(
					xRetryStatePd, xCrossPd, xProfilePd, powX, xCenterPd, xAttempt, xLastAttemptPd, xFailInfoPd);
				if (xPlanPd.action == M576::SweepRetryAction::GiveUp)
					break;
				if (RecalSweepPowerSampleCount(xPlanPd.nextRange, m_dacStep) != (int)powY.size())
					break;
				const int prevRangeXPd = attemptDacRangeXPd;
				const int prevBaseXPd = movingXPd;
				M576::ApplySweepRetryPlan(xRetryStatePd, xPlanPd);
				movingXPd = xRetryStatePd.movingBase;
				attemptDacRangeXPd = xRetryStatePd.attemptRange;
				xAttemptTimeoutPd = ComputeRecal1DReadTimeoutMs(m_delayMs, attemptDacRangeXPd, m_dacStep);
				{
					CString msg;
					if (xPlanPd.action == M576::SweepRetryAction::JumpFlatMax)
					{
						msg.Format(
							_T("  %hs RECAL 5 1 X attempt %d/%d code=%hs trend=%hs span=%.4g offset %d->%d"),
							M576::SweepRetryActionLogTag(xPlanPd.action),
							xAttempt + 1,
							(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
							M576Peak1DWhy(xCrossPd),
							M576::SweepTrendName(xProfilePd.trend),
							xProfilePd.span,
							prevRangeXPd,
							attemptDacRangeXPd);
					}
					else
					{
						const M576::SweepProfile xRecenterProfilePd =
							M576::AdjustProfileForMonoRecenter(xProfilePd, powX, xRetryStatePd.inCoarsePhase, &xFailInfoPd);
						const double deltaDacPd = M576::SuggestSweepRecenterDeltaDac(
							xRecenterProfilePd, nXpd, attemptDacRangeXPd, xAttempt, xFailInfoPd);
						msg.Format(
							_T("  %hs RECAL 5 1 X attempt %d/%d code=%hs trend=%hs argmax=%d t*=%.4g span=%.4g offset=%d deltaDac=%.4g newMovingX=%d fixedY=%d (%d->%d)"),
							M576::SweepRetryActionLogTag(xPlanPd.action),
							xAttempt + 1,
							(int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS,
							M576Peak1DWhy(xCrossPd),
							M576::SweepTrendName(xRecenterProfilePd.trend),
							xRecenterProfilePd.argmaxIndex,
							tXPeakPd,
							xRecenterProfilePd.span,
							attemptDacRangeXPd,
							deltaDacPd,
							movingXPd,
							fixedYPd,
							prevBaseXPd,
							movingXPd);
					}
					SafeAppendLog(msg);
				}
				xRetryStatePd.prevArgmax = xProfilePd.argmaxIndex;
			}

			if (retryYAfterCrossPd)
				continue;
			break;
		}

		if (pdBreakPath)
			break;
		if (pdSkipStep)
		{
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}

		if (crossOkPd)
		{
			M576AppendPeakFitTraceLog(this, _T("RECAL5 交叉 Y轴(PD) "), trCrossYPd);
			M576AppendPeakFitTraceLog(this, _T("RECAL5 交叉 X轴(PD) "), trCrossXPd);
			const int nLut = (int)powY.size();
			const double rawDacXAtPeakPd = SweepCol0PlusPeakOffsetDac(xFixedDacPd, tYpd, nLut, ySweepRangePd);
			const double rawDacYAtPeakPd = SweepCol0PlusPeakOffsetDac(sweep1LineCol0Pd, tXpd, nLut, xSweepRangePd);
			const int rawXiPd = static_cast<int>(std::lround(rawDacXAtPeakPd));
			const int rawYiPd = static_cast<int>(std::lround(rawDacYAtPeakPd));
			{
				CString msg;
				msg.Format(
					_T("  -> peak row=%d col=%d; linear DAC at cross-peak: Y=%.4g (RECAL5 0 col0 + %d*step), X=%.4g (RECAL5 1 col0 + %d*step)"),
					br,
					bc,
					rawDacXAtPeakPd,
					br,
					rawDacYAtPeakPd,
					bc);
				SafeAppendLog(msg);
			}
			SDacU16 dacU;
			RawCrossPeakDacToU16Pair((double)rawXiPd, (double)rawYiPd, dacU.uX, dacU.uY);
			if (fileSlot < 2)
			{
				ApplyRecalPeakToLutPd(
					st, idxOcc3, idxOcc4, dacU.uX, dacU.uY, mcsTempSlot, m_lutByTrans[fileSlot]);
				SCalibrationStatRow srow;
				if (CalibBuildStatRowPdLut(
						st,
						idxOcc3,
						idxOcc4,
						fileSlot,
						i + 1,
						br,
						bc,
						nLut,
						rawXiPd,
						rawYiPd,
						dacU,
						mcsTempSlot,
						srow))
					PushCalibStatRow(srow);
			}
			else
			{
				ApplyRecalPeakToMems1x64Pd(
					st, idxOcc3, idxOcc4, dacU.uX, dacU.uY, memsCalibSlot, m_mems1x64[fileSlot - 2]);
				SCalibrationStatRow srow;
				if (CalibBuildStatRowPdMems(
						st, fileSlot, i + 1, br, bc, nLut, rawXiPd, rawYiPd, dacU, memsCalibSlot, srow))
					PushCalibStatRow(srow);
			}
		}
		else if (!pdFailRecorded)
		{
			SCalibPathStepOutcome o = M576MakePdOutcome(fileSlot, csvBasename, i + 1, st);
			o.result = CalibPathStepResult::Failed;
			o.crossRound = lastCrossRoundPd;
			o.lastBaseY = yMovingBasePd;
			o.lastBaseX = lastMovingXPd;
			o.lastOffsetY = ySeedDacRangePd;
			o.lastOffsetX = lastAttemptDacRangeXPd;
			o.sweepCol0Y = xFixedDacPd;
			o.sweepCol0X = sweep1LineCol0Pd;
			o.sampleCountY = (int)powY.size();
			o.sampleCountX = (int)powX.size();
			M576FillSweepPowContext(o, powY, true);
			M576FillSweepPowContext(o, powX, false);
			o.tPeakY = tYpd;
			o.tPeakX = tXpd;
			o.hasTPeakY = std::isfinite(tYpd);
			o.hasTPeakX = std::isfinite(tXpd);
			if (yCrossPd != M576::Peak1DValidateCode::Ok)
			{
				o.failCategory = CalibPathFailCategory::YCrossPeak;
				o.failStage = "cross Y";
				o.peakAttempts = (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS;
				M576FillPeakCodeFields(o, yCrossPd);
			}
			else if (xCrossPd != M576::Peak1DValidateCode::Ok)
			{
				o.failCategory = CalibPathFailCategory::XCrossPeak;
				o.failStage = "cross X";
				o.peakAttempts = (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS;
				M576FillPeakCodeFields(o, xCrossPd);
			}
			else if (yPreCodePd != M576::Peak1DValidateCode::Ok)
			{
				o.failCategory = CalibPathFailCategory::YPrePeak;
				o.failStage = "Y pre-sweep";
				o.peakAttempts = yPreAttemptsPd;
				M576FillPeakCodeFields(o, yPreCodePd);
			}
			else if (pdSkipStep)
			{
				o.failCategory = CalibPathFailCategory::CommSweep;
				o.failStage = "RECAL 5 sweep";
				o.commDetail = M576CStringToUtf8(err);
				TruncatePathOutcomeDetail(o.commDetail);
			}
			else
			{
				o.failCategory = CalibPathFailCategory::XCrossPeak;
				o.failStage = "cross peak";
				o.peakAttempts = (int)M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS;
				o.commDetail = "peak not found after retries";
			}
			PushPathFailureOutcome(o);
			pdFailRecorded = TRUE;
		}

		++globalProgress;
		SafeSetProgressPos(globalProgress);
	}
	(void)globalTotal;
}

// --- 生成 BIN：MCS 为 Z4671 包，1x64 为四路 2K Mems（m_lut / m_mems1x64 合并 1310）---

void CM576CalibratorDlg::OnBnClickedGenBin()
{
	UpdateData(TRUE);
	ApplyFixedBinBasePaths(TRUE);
	CString snErr;
	if (!ValidateSnBeforeBinOp(snErr))
	{
		AppendLog(snErr);
		MessageBoxM576(
			snErr + _T("\n\nRun Read All SN first, then Write BIN."),
			MB_OK | MB_ICONWARNING);
		return;
	}
	const CString absOutDir = ResolveBinOutputDirAbs();
	if (absOutDir.IsEmpty())
	{
		MessageBoxM576(_T("BIN output directory is empty."), MB_OK | MB_ICONWARNING);
		return;
	}
	EnsureOutputFolderUnderExe(GetExeFolder());

	if (M576SessionLutMemsAllZero(m_lutByTrans, m_mems1x64))
	{
		AppendLog(_T("Write BIN: session LUT/Mems empty; preloading from {SN}_backup.bin (if present)."));
		TryPreloadLutFromPerTransBackup();
	}

	WarnIfUiWavelengthDiffersFromSession();
	{
		const CString fullCsv = BuildSessionDacCsvPath(m_sessionCalibPolicy, M576BinFileRole::Standard);
		const int slash = fullCsv.ReverseFind(_T('\\'));
		const CString leaf = (slash >= 0) ? fullCsv.Mid(slash + 1) : fullCsv;
		ExportSessionDacCsv(m_sessionCalibPolicy, leaf, _T("Write BIN"));
	}

	CString err;
	if (!GenerateStandardBinFiles(absOutDir, err, FALSE))
	{
		AppendLog(err);
		MessageBoxM576(err, MB_OK | MB_ICONERROR);
		return;
	}
	ArchiveCurrentBinSet(_T("standard"), M576BinFileRole::Standard, _T("write_bin"), TRUE);
	AppendLog(_T("All {SN}_standard.bin files written."));
	MessageBoxM576(
		_T("Write BIN completed.\n\nAll {SN}_standard.bin files were written successfully."),
		MB_OK | MB_ICONINFORMATION);
}

// --- Make Bin：读 standardAll1310DAC.csv 合并进 backup 生成 standard 分 trans 文件 ---

void CM576CalibratorDlg::OnBnClickedMakeBin()
{
	UpdateData(TRUE);
	ApplyFixedBinBasePaths(TRUE);

	CString snErr;
	if (!ValidateSnBeforeBinOp(snErr))
	{
		MessageBoxM576(snErr + _T("\n\nRun Read All SN first."), MB_OK | MB_ICONWARNING);
		return;
	}

	const CString absOutDir = ResolveBinOutputDirAbs();
	if (absOutDir.IsEmpty())
	{
		MessageBoxM576(_T("MakeBin: BIN output directory is empty."), MB_OK | MB_ICONWARNING);
		return;
	}

	const CString absCsvPath = BuildSessionDacCsvPath(M576CalibBinWritePolicy::Slot1310Low, M576BinFileRole::Standard);

	CString err;
	if (!ValidateMakeBinInputs(absOutDir, absCsvPath, err))
	{
		AppendLog(err);
		MessageBoxM576(err, MB_OK | MB_ICONERROR);
		return;
	}

	stLutSettingZ4671 csvLut[2];
	stM576OneX64MemsSwCoef csvMems[2][4];
	if (!ParseLowTemp1310DacCsv(absCsvPath, csvLut, csvMems, err))
	{
		CString msg;
		msg.Format(_T("MakeBin: CSV parse failed: %s"), err.GetString());
		AppendLog(msg);
		MessageBoxM576(msg, MB_OK | MB_ICONERROR);
		return;
	}

	ZeroMemory(m_lutByTrans, sizeof(m_lutByTrans));
	ZeroMemory(m_mems1x64, sizeof(m_mems1x64));
	memcpy(&m_lutByTrans[0], &csvLut[0], sizeof(csvLut));
	memcpy(&m_mems1x64[0][0], &csvMems[0][0], sizeof(csvMems));
	{
		CString msg;
		msg.Format(_T("MakeBin: loaded low-temp DAC values from %s."), absCsvPath.GetString());
		AppendLog(msg);
	}

	if (!GenerateStandardBinFiles(absOutDir, err, TRUE))
	{
		AppendLog(err);
		MessageBoxM576(err, MB_OK | MB_ICONERROR);
		return;
	}
	ArchiveCurrentBinSet(_T("standard"), M576BinFileRole::Standard, _T("make_bin"), TRUE);

	AppendLog(_T("MakeBin: all {SN}_standard.bin files generated/overwritten successfully."));
	MessageBoxM576(
		_T("MakeBin completed.\n\nLoaded standard DAC CSV and regenerated all {SN}_standard.bin files."),
		MB_OK | MB_ICONINFORMATION);
}

void CM576CalibratorDlg::OnBnClickedReadAllSn()
{
	if (m_readSnRunning.load())
		return;
	if (m_burnFlashRunning.load() || m_burnBoardRunning.load())
	{
		AppendLog(_T("Flash/board burn in progress; wait before reading SN."));
		return;
	}
	if (m_pathRunning.load())
	{
		AppendLog(_T("Path run in progress; wait for it to finish before reading SN."));
		return;
	}
	if (m_readBackupRunning.load())
	{
		AppendLog(_T("Read Flash backup in progress; wait before reading SN."));
		return;
	}
	UpdateData(TRUE);
	if (!m_dev429f.GetPortHandle() || m_dev429f.GetPortHandle() == INVALID_HANDLE_VALUE)
	{
		if (!OpenPort())
			return;
		SyncSerialPortUi();
	}
	if (m_readSnThread.joinable())
		m_readSnThread.join();
	m_readSnRunning = true;
	SetPathActionButtonsEnabled(FALSE);
	AppendLog(_T("Read SN started in background..."));
	m_readSnThread = std::thread([this]() { ReadAllSnWorkerEntry(); });
}

void CM576CalibratorDlg::ProgressThunk(int cur, int total, void* user)
{
	CM576CalibratorDlg* p = (CM576CalibratorDlg*)user;
	if (!p || !::IsWindow(p->m_hWnd))
		return;
	int pct = (total > 0) ? (cur * 100 / total) : 0;
	p->SafeSetProgressPos(pct);
}

// --- 按配置烧录各 trans 分 bin（McsFwUploadBinEx 等）---

void CM576CalibratorDlg::OnBnClickedFlash()
{
	if (m_burnFlashRunning.load() || m_burnBoardRunning.load())
		return;
	UpdateData(TRUE);
	ApplyFixedBinBasePaths(TRUE);
	if (m_pathRunning.load())
	{
		AppendLog(_T("Path run in progress; wait before burning flash."));
		return;
	}
	if (m_readSnRunning.load())
	{
		AppendLog(_T("Read SN in progress; wait before burning flash."));
		return;
	}
	if (m_readBackupRunning.load())
	{
		AppendLog(_T("Read Flash backup in progress; wait before burning flash."));
		return;
	}
	CString snErr;
	if (!ValidateSnBeforeBinOp(snErr))
	{
		AppendLog(snErr);
		MessageBoxM576(
			snErr + _T("\n\nRun Read All SN first, then Write BIN."),
			MB_OK | MB_ICONWARNING);
		return;
	}
	const CString absOutDir = ResolveBinOutputDirAbs();
	std::array<CString, M576_BURN_FILE_COUNT> stdPaths;
	CString pathErr;
	if (!M576BuildBurnFilePaths(absOutDir, m_snInfo, M576BinFileRole::Standard, stdPaths, pathErr))
	{
		AppendLog(pathErr);
		MessageBoxM576(pathErr, MB_OK | MB_ICONWARNING);
		return;
	}
	BOOL anyBin = FALSE;
	for (const CString& p : stdPaths)
	{
		if (GetFileAttributes(p) == INVALID_FILE_ATTRIBUTES)
			continue;
		HANDLE h = CreateFile(p, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (h == INVALID_HANDLE_VALUE)
			continue;
		const DWORD sz = GetFileSize(h, NULL);
		CloseHandle(h);
		if (sz > 0)
		{
			anyBin = TRUE;
			break;
		}
	}
	if (!anyBin)
	{
		AppendLog(_T("No non-empty {SN}_standard.bin files; run Write BIN first."));
		MessageBoxM576(
			_T("Cannot burn: no non-empty {SN}_standard.bin files.\n\nRun Write BIN or copy standard files first."),
			MB_OK | MB_ICONWARNING);
		return;
	}
	if (!m_dev429f.GetPortHandle() || m_dev429f.GetPortHandle() == INVALID_HANDLE_VALUE)
	{
		if (!OpenPort())
			return;
		SyncSerialPortUi();
	}
	if (MessageBoxM576(
			_T("Warning: Burn Flash will program the device on the current 439F tunnel(s):\n\n")
			_T("Trans 1-2: MCS firmware stream\n")
			_T("Trans 3-4: 1x64 XMODEM 4x per trans ({SN}_standard.bin)\n\n")
			_T("Continue?"),
			MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2)
		!= IDYES)
	{
		AppendLog(_T("Burn Flash cancelled by user."));
		return;
	}
	CM576BurnSelectDlg burnDlg(this, absOutDir);
	if (burnDlg.DoModal() != IDOK)
	{
		AppendLog(_T("Burn Flash cancelled (file selection)."));
		return;
	}
	std::array<bool, M576_BURN_FILE_COUNT> burnMask = burnDlg.GetMask();
	bool anySel = false;
	for (bool b : burnMask) {
		if (b) {
			anySel = true;
			break;
		}
	}
	if (!anySel) {
		MessageBoxM576(_T("No part selected for burn."), MB_OK | MB_ICONWARNING);
		return;
	}
	CString validateErr;
	if (!M576ValidateBurnSelectionByPaths(stdPaths, burnMask.data(), validateErr))
	{
		AppendLog(validateErr);
		MessageBoxM576(validateErr, MB_OK | MB_ICONERROR);
		return;
	}
	LogBurnFilePaths(this, stdPaths, _T("standard (burn)"));
	ArchiveCurrentBinSet(_T("pre_burn"), M576BinFileRole::Standard, _T("pre_burn"), TRUE);
	m_burnFlashLastPartial = false;
	for (bool b : burnMask) {
		if (!b) {
			m_burnFlashLastPartial = TRUE;
			break;
		}
	}
	m_burnFlashLastRecover = FALSE;
	if (m_burnFlashThread.joinable())
		m_burnFlashThread.join();
	m_burnFlashRunning = true;
	SetPathActionButtonsEnabled(FALSE);
	AppendLog(_T("Burn Flash started in background..."));
	m_burnFlashThread = std::thread(
		[this, stdPaths, burnMask]() { BurnFlashWorkerEntry(stdPaths, burnMask); });
}

void CM576CalibratorDlg::OnBnClickedRecoverFlash()
{
	if (m_burnFlashRunning.load() || m_burnBoardRunning.load())
		return;
	UpdateData(TRUE);
	ApplyFixedBinBasePaths(TRUE);
	if (m_pathRunning.load())
	{
		AppendLog(_T("Path run in progress; wait before recover flash."));
		return;
	}
	if (m_readSnRunning.load())
	{
		AppendLog(_T("Read SN in progress; wait before recover flash."));
		return;
	}
	if (m_readBackupRunning.load())
	{
		AppendLog(_T("Read Flash backup in progress; wait before recover flash."));
		return;
	}
	if (!m_dev429f.GetPortHandle() || m_dev429f.GetPortHandle() == INVALID_HANDLE_VALUE)
	{
		if (!OpenPort())
			return;
		SyncSerialPortUi();
	}

	const CString absOutDir = ResolveBinOutputDirAbs();
	CM576RecoverSelectDlg dlg(this, absOutDir, m_snInfo);
	if (dlg.DoModal() != IDOK)
	{
		AppendLog(_T("Recover Flash cancelled (file selection)."));
		return;
	}
	std::array<bool, M576_BURN_FILE_COUNT> burnMask = dlg.GetMask();
	std::array<CString, M576_BURN_FILE_COUNT> burnPaths = dlg.GetPaths();

	bool anySel = false;
	for (bool b : burnMask)
	{
		if (b)
		{
			anySel = true;
			break;
		}
	}
	if (!anySel)
	{
		MessageBoxM576(_T("No part selected for recover."), MB_OK | MB_ICONWARNING);
		return;
	}
	if (MessageBoxM576(
			_T("Warning: Recover Flash will program selected files into current device tunnel(s).\n\nContinue?"),
			MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2)
		!= IDYES)
	{
		AppendLog(_T("Recover Flash cancelled by user."));
		return;
	}

	m_burnFlashLastPartial = false;
	for (bool b : burnMask)
	{
		if (!b)
		{
			m_burnFlashLastPartial = TRUE;
			break;
		}
	}
	m_burnFlashLastRecover = TRUE;
	if (m_burnFlashThread.joinable())
		m_burnFlashThread.join();
	m_burnFlashRunning = true;
	SetPathActionButtonsEnabled(FALSE);
	AppendLog(_T("Recover Flash started in background..."));
	m_burnFlashThread = std::thread(
		[this, burnPaths, burnMask]() { RecoverFlashWorkerEntry(burnPaths, burnMask); });
}
