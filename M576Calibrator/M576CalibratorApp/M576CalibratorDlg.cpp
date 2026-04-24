#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576CalibratorDlg.h"
#include "LutMerge1310.h"
#include "CalibConstants.h"
#include "PeakFinder2D.h"
#include "LutPeakApply.h"
#include <math.h>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {

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

constexpr UINT WM_M576_PATH_LOG_FLUSH = WM_APP + 100;
constexpr UINT WM_M576_PATH_PROGRESS_RANGE = WM_APP + 101;
constexpr UINT WM_M576_PATH_PROGRESS_POS = WM_APP + 102;
constexpr UINT WM_M576_PATH_FINISHED = WM_APP + 103;
constexpr UINT WM_M576_READ_BACKUP_FINISHED = WM_APP + 104;

/// RECAL 3/5 一行：`[轴上 DAC 或首列][P1..Pn]`。功率个数 N = ceil((2*range)/step)，与固件一致（例 range=64 step=5 → N=26，整行 1+26=27 个数）。
static int RecalSweepPowerSampleCount(int dacRange, int dacStep)
{
	if (dacStep < 1)
		dacStep = 1;
	if (dacRange < 1)
		dacRange = 1;
	const int n = (2 * dacRange + dacStep - 1) / dacStep;
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

/// After `RECAL 3 0` / `RECAL 5 0` (moving Y) with Base=FW-read, map peak index to Y-axis DAC for `RECAL 3 1` / `RECAL 5 1` Base.
/// Same symmetric indexing as `PeakGridToDacWord` / UI half-range, with nominal center `M576_PEAK_GRID_DAC_BASE`.
static int RecalYBaseDacFromYPeakIndex(int peakRow, int sampleCount, int halfRange)
{
	if (sampleCount <= 1)
		return M576_RECAL_FW_READ_BASE_DAC;
	const double step = (2.0 * halfRange) / (double)(sampleCount - 1);
	const double y = (double)M576_PEAK_GRID_DAC_BASE - halfRange + (double)peakRow * step;
	int iy = (int)floor(y + 0.5);
	if (iy < 0)
		iy = 0;
	if (iy > 65535)
		iy = 65535;
	return iy;
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
	CString outDir = exeFolder + _T("\\output");
	(void)CreateDirectory(outDir, NULL);
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

} // namespace

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
	, m_readBackupLastOk(FALSE)
{
	for (int i = 0; i < 4; ++i)
	{
		m_strCsvPm[i] = g_m576DefaultPmCsvRel[i];
		m_strCsvPd[i] = g_m576DefaultPdCsvRel[i];
	}
	m_strOutBin     = _T("output\\standard.bin");
	m_strBackupBin  = _T("output\\backup.bin");
	m_strCommLogPath = _T("output\\comm.log");
	m_strSn         = _T("429F1 Tester");
}

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
	DDX_Text(pDX, IDC_EDIT_SN, m_strSn);
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
	ON_BN_CLICKED(IDC_BTN_CLOSE_PORT, &CM576CalibratorDlg::OnBnClickedClosePort)
	ON_BN_CLICKED(IDC_BTN_BROWSE_BACKUP, &CM576CalibratorDlg::OnBnClickedBrowseBackup)
	ON_BN_CLICKED(IDC_BTN_BROWSE_OUT, &CM576CalibratorDlg::OnBnClickedBrowseOut)
	ON_BN_CLICKED(IDC_BTN_READ_FLASH_BACKUP, &CM576CalibratorDlg::OnBnClickedReadFlashBackup)
	ON_BN_CLICKED(IDC_RADIO_CAL_PM, &CM576CalibratorDlg::OnBnClickedCalPm)
	ON_BN_CLICKED(IDC_RADIO_CAL_PD, &CM576CalibratorDlg::OnBnClickedCalPd)
	ON_BN_CLICKED(IDC_BTN_RUN_PATH, &CM576CalibratorDlg::OnBnClickedRunPath)
	ON_BN_CLICKED(IDC_BTN_CLEAR_LOG, &CM576CalibratorDlg::OnBnClickedClearLog)
	ON_BN_CLICKED(IDC_BTN_GEN_BIN, &CM576CalibratorDlg::OnBnClickedGenBin)
	ON_BN_CLICKED(IDC_BTN_FLASH, &CM576CalibratorDlg::OnBnClickedFlash)
	ON_BN_CLICKED(IDC_BTN_STOP, &CM576CalibratorDlg::OnBnClickedStop)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_M576_PATH_LOG_FLUSH, &CM576CalibratorDlg::OnPathLogFlush)
	ON_MESSAGE(WM_M576_PATH_PROGRESS_RANGE, &CM576CalibratorDlg::OnPathProgressRange)
	ON_MESSAGE(WM_M576_PATH_PROGRESS_POS, &CM576CalibratorDlg::OnPathProgressPos)
	ON_MESSAGE(WM_M576_PATH_FINISHED, &CM576CalibratorDlg::OnPathFinished)
	ON_MESSAGE(WM_M576_READ_BACKUP_FINISHED, &CM576CalibratorDlg::OnReadBackupFinished)
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
	SetWindowText(_T("M576 / 1310 Calibrator (439F)"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_CONN, _T("Connection"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_PATHS, _T("Config"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_ACTIONS, _T("Actions"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_LOG, _T("Log"));
	::SetDlgItemText(m_hWnd, IDC_STATIC_LABEL_COM, _T("Port (439F):"));
	::SetDlgItemText(m_hWnd, IDC_BTN_FLASH, _T("Burn Flash"));
	::SetDlgItemText(m_hWnd, IDC_BTN_READ_FLASH_BACKUP, _T("Read Flash Backup"));
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
	SyncRecal0ControlsVisibility();
	FillComPorts();
	SyncSerialPortUi();
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	for (int li = 0; li < 4; ++li)
		ZeroMemory(&m_lutByTrans[li], sizeof(m_lutByTrans[li]));
	AppendLog(_T("Ready. Select 439F COM port, open port, then run."));
	AppendLog(_T("Backup BIN: Read Flash — trans1–2 (MCS): 0xC4 LUT bundle; trans3–4 (1x64): MEM full range (e.g. 12×2K/24K from base); files *_mcs1/2.bin, *_1x64_1/2.bin."));
	AppendLog(_T("Path CSV: built-in output\\pm_*.csv (PM) or pd_*.csv (PD); missing file skips that trans slot."));
	AppendLog(_T("PM: RECAL 0 + RECAL 1 + RECAL 3; PD: RECAL 2 + RECAL 5 (no RECAL 0)."));
	return TRUE;
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
	CString t;
	m_editLog.GetWindowText(t);
	TrimEditLogText(t);
	if (!t.IsEmpty())
		t += _T("\r\n");
	CString line;
	line.Format(_T("%s %s"), FormatLogTimestamp().GetString(), sz);
	t += line;
	m_editLog.SetWindowText(t);
	int n = m_editLog.GetWindowTextLength();
	m_editLog.SetSel(n, n);
	WriteLogFileLine(line);
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
	if (CWnd* p = GetDlgItem(IDC_BTN_FLASH))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_READ_FLASH_BACKUP))
		p->EnableWindow(enable);
	if (CWnd* p = GetDlgItem(IDC_BTN_STOP))
		p->EnableWindow(enable);
	SyncSerialPortUi();
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
	const BOOL busy = m_pathRunning.load() || m_readBackupRunning.load();
	if (CWnd* p = GetDlgItem(IDC_BTN_OPEN_PORTS))
		p->EnableWindow(!open && !busy);
	if (CWnd* p = GetDlgItem(IDC_BTN_CLOSE_PORT))
		p->EnableWindow(open && !busy);
	if (CWnd* p = GetDlgItem(IDC_COMBO_COM))
		p->EnableWindow(!open);
}

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
	CString t;
	m_editLog.GetWindowText(t);
	TrimEditLogText(t);
	if (!t.IsEmpty())
		t += _T("\r\n");
	t += batch;
	m_editLog.SetWindowText(t);
	int n = m_editLog.GetWindowTextLength();
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
	return 0;
}

LRESULT CM576CalibratorDlg::OnReadBackupFinished(WPARAM, LPARAM)
{
	if (m_readBackupThread.joinable())
		m_readBackupThread.join();
	m_readBackupRunning = false;
	SetPathActionButtonsEnabled(TRUE);
	UpdateData(FALSE);
	return 0;
}

void CM576CalibratorDlg::PathWorkerEntry()
{
	if (m_nCalMode == 0)
		RunPathPowerMeter();
	else
		RunPathPd();
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_PATH_FINISHED, 0, 0);
}

void CM576CalibratorDlg::ReadFlashBackupWorkerEntry(CString absBackupBin)
{
	SafeSetProgressRange(0, 100);
	SafeSetProgressPos(0);
	CString err;
	if (!McsReadLutBundleFromDevice(m_dev429f, absBackupBin, err, &CM576CalibratorDlg::ProgressThunk, this))
	{
		CString m;
		m.Format(_T("Read Flash backup failed: %s"), (LPCTSTR)err);
		SafeAppendLog(m);
	}
	else
	{
		SafeSetProgressPos(100);
		CString ok;
		ok.Format(_T("Flash backups saved (439F per trans): base=%s — MCS=0xC4 LUT, 1x64=MEM full read (see M576_1X64_MEMS_BACKUP_TOTAL_SIZE in CalibConstants)."),
			(LPCTSTR)absBackupBin);
		SafeAppendLog(ok);
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_READ_BACKUP_FINISHED, 0, 0);
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
		DWORD written = 0;
		WriteFile(h, bom, sizeof(bom), &written, NULL);
	}
	CString lineWithBreak = line + _T("\r\n");
	CStringA encoded(lineWithBreak);
	const char* raw = encoded.GetString();
	DWORD len = (DWORD)encoded.GetLength();
	DWORD written = 0;
	WriteFile(h, raw, len, &written, NULL);
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
	AppendLog(_T("Port opened (439F control board)."));
	return TRUE;
}

void CM576CalibratorDlg::ClosePort()
{
	m_pRecal.reset();
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

void CM576CalibratorDlg::OnBnClickedClosePort()
{
	UpdateData(TRUE);
	ClosePort();
	SyncSerialPortUi();
}

void CM576CalibratorDlg::OnBnClickedBrowseBackup()
{
	const CString exe = GetExeFolder();
	CFileDialog dlg(TRUE, _T("bin"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("BIN (*.bin)|*.bin|All (*.*)|*.*||"), this);
	dlg.GetOFN().lpstrInitialDir = exe.GetString();
	if (dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(IDC_EDIT_BACKUP_BIN, ToRelPath(dlg.GetPathName()));
	UpdateData(TRUE);
}

void CM576CalibratorDlg::OnBnClickedBrowseOut()
{
	const CString exe = GetExeFolder();
	CFileDialog dlg(FALSE, _T("bin"), _T("standard.bin"), OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
		_T("BIN (*.bin)|*.bin|All (*.*)|*.*||"), this);
	dlg.GetOFN().lpstrInitialDir = exe.GetString();
	if (dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(IDC_EDIT_OUT_BIN, ToRelPath(dlg.GetPathName()));
	UpdateData(TRUE);
}

void CM576CalibratorDlg::OnBnClickedReadFlashBackup()
{
	if (m_readBackupRunning.load())
		return;
	if (m_pathRunning.load())
	{
		AppendLog(_T("Path run in progress; wait for it to finish before reading Flash backup."));
		return;
	}
	UpdateData(TRUE);
	EnsureOutputFolderUnderExe(GetExeFolder());
	if (m_strBackupBin.IsEmpty())
		m_strBackupBin = _T("output\\mcs_lut_backup.bin");
	if (!m_dev429f.GetPortHandle() || m_dev429f.GetPortHandle() == INVALID_HANDLE_VALUE)
	{
		if (!OpenPort())
			return;
	}
	const CString absBackupBin = ResolveFilePath(m_strBackupBin);
	if (m_readBackupThread.joinable())
		m_readBackupThread.join();
	m_readBackupRunning = true;
	m_suppressPathProgress = false;
	SetPathActionButtonsEnabled(FALSE);
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	AppendLog(_T("Read Flash: trans1–2 MCS 0xC4 LUT; trans3–4 1x64 MEM (full range) → <base>_mcs1/2.bin, <base>_1x64_1/2.bin."));
	m_readBackupThread = std::thread([this, absBackupBin]() { ReadFlashBackupWorkerEntry(absBackupBin); });
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
		AfxMessageBox(_T("User Stopped"), MB_OK | MB_ICONINFORMATION);
	}
}

void CM576CalibratorDlg::OnBnClickedCalPm()
{
	UpdateData(TRUE);
	/// PM defaults: TLS=4, 1310 nm, PM range=1 (Command A).
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

void CM576CalibratorDlg::OnBnClickedRunPath()
{
	if (m_pathRunning.load())
		return;
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
		AfxMessageBox(valErr, MB_OK | MB_ICONWARNING);
		return;
	}
	m_bStop = FALSE;
	if (m_pathThread.joinable())
		m_pathThread.join();
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
	m_pathRunning = false;
	m_readBackupRunning = false;
	m_suppressPathProgress = false;
	CDialogEx::OnDestroy();
}

void CM576CalibratorDlg::TryPreloadLutFromPerTransBackup()
{
	CString base = m_strBackupBin;
	base.Trim();
	if (base.IsEmpty())
		return;
	const CString absBk = ResolveFilePath(base);
	for (int li = 0; li < 4; ++li)
	{
		const CString p = M576TransBinPathForRead(absBk, li + 1);
		if (GetFileAttributes(p) == INVALID_FILE_ATTRIBUTES)
			continue;
		if (CLutBinWriter::ReadLutFromFile(p, m_lutByTrans[li]))
		{
			CString m;
			m.Format(_T("Run path: preloaded trans %d from %s"), li + 1, p.GetString());
			SafeAppendLog(m);
		}
		else
		{
			CString m;
			m.Format(_T("Run path: read backup failed for %s"), p.GetString());
			SafeAppendLog(m);
		}
	}
}

void CM576CalibratorDlg::RunPathPowerMeter()
{
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
	TryPreloadLutFromPerTransBackup();

	int wavelengthNm = 0;
	if (!ParseWavelengthNm(m_strWavelength, wavelengthNm, err))
	{
		SafeAppendLog(err);
		return;
	}
	const int tlsSource = m_tlsIndex + 1;
	const int pmRange = m_pmRangeIndex;
	if (!m_pRecal->SendRecal0(tlsSource, wavelengthNm, pmRange, err))
	{
		SafeAppendLog(err);
		return;
	}
	{
		CStringA line0;
		if (!m_pRecal->ReadAsciiResponse(line0, 3000, err))
			SafeAppendLog(_T("RECAL 0: timeout waiting for response."));
		else
		{
			CString msg;
			msg.Format(_T("RECAL 0 (TLS=%d nm=%d PM=%d) -> %s"),
				tlsSource, wavelengthNm, pmRange, CString(line0));
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
		RunPathPowerMeterFile(fs, steps, globalProgress, totalAll, occT3, occT4);
	}
	SafeAppendLog(_T("Path run finished (PM all slots)."));
}

void CM576CalibratorDlg::RunPathPowerMeterFile(int fileSlot, CArray<SPathStep, SPathStep const&>& steps, int& globalProgress, int globalTotal, int& occT3, int& occT4)
{
	CString err;
	const int total = (int)steps.GetSize();
	const DWORD readTimeout1d = ComputeRecal1DReadTimeoutMs(m_delayMs, m_dacRange, m_dacStep);
	const int gridN = AxisPointCount(m_dacRange, m_dacStep);
	CStringA lineOk, lineY, lineX;
	for (int i = 0; i < total; ++i)
	{
		if (m_bStop)
			break;
		SPathStep& st = steps[i];
		CString verr;
		if (!ValidatePathStep(st, verr))
		{
			SafeAppendLog(verr);
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		CString wslot;
		if (!PmStepMatchesFileSlot(st, fileSlot, &wslot))
		{
			SafeAppendLog(wslot);
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

		if (!m_pRecal->SendRecal1(st, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineOk, 3000, err))
		{
			SafeAppendLog(_T("RECAL 1: timeout waiting for OK."));
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

		if (!m_pRecal->SendRecal3(0, M576_RECAL_FW_READ_BASE_DAC, m_dacRange, m_dacStep, m_delayMs, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiSweepResponse(lineY, readTimeout1d, err))
		{
			SafeAppendLog(_T("RECAL 3 0 (Y sweep): timeout or empty."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		double xFixedDac = 0.0;
		std::vector<double> powY;
		if (!CRecalSession::ParseRecal3SweepLine(lineY, xFixedDac, powY))
		{
			SafeAppendLog(_T("RECAL 3 0: could not parse [X_start] P1..Pn."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 3 0 -> %d power samples, fixed-X DAC=%.4g (from response col0)"),
				(int)powY.size(), xFixedDac);
			SafeAppendLog(msg);
		}

		int brForYBase = 0;
		if (powY.empty() || !M576::PeakMax1D(powY, brForYBase))
		{
			SafeAppendLog(_T("  RECAL 3 1: no Y peak index; skip X sweep."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		const int yBaseDacForSweep1 =
			RecalYBaseDacFromYPeakIndex(brForYBase, (int)powY.size(), m_dacRange);
		{
			CString msg;
			msg.Format(_T("  RECAL 3 1 Base DAC (Y@peak, row=%d)=%d"), brForYBase, yBaseDacForSweep1);
			SafeAppendLog(msg);
		}

		if (!m_pRecal->SendRecal3(1, yBaseDacForSweep1, m_dacRange, m_dacStep, m_delayMs, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiSweepResponse(lineX, readTimeout1d, err))
		{
			SafeAppendLog(_T("RECAL 3 1 (X sweep): timeout or empty."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		double yFixedDac = 0.0;
		std::vector<double> powX;
		if (!CRecalSession::ParseRecal3SweepLine(lineX, yFixedDac, powX))
		{
			SafeAppendLog(_T("RECAL 3 1: could not parse [Y_start] P1..Pn."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 3 1 -> %d power samples, fixed-Y DAC=%.4g (from response col0)"),
				(int)powX.size(), yFixedDac);
			SafeAppendLog(msg);
		}
		{
			CString msg;
			msg.Format(_T("  -> DAC pair (X,Y)=(%.4g,%.4g) (X from RECAL 3 0 header, Y from RECAL 3 1 header)"),
				xFixedDac, yFixedDac);
			SafeAppendLog(msg);
		}

		if ((int)powY.size() != gridN || (int)powX.size() != gridN)
		{
			CString msg;
			msg.Format(_T("  warning: firmware power count (%d, %d) vs host AxisPointCount estimate %d (range=%d step=%d); OK if FW grid differs."),
				(int)powY.size(), (int)powX.size(), gridN, m_dacRange, m_dacStep);
			SafeAppendLog(msg);
		}

		int br = 0, bc = 0;
		if (powY.size() != powX.size() || powY.empty())
		{
			SafeAppendLog(_T("  peak: Y/X sweep lengths differ or empty; skip LUT update."));
		}
		else if (!M576::PeakCrossFrom1DScans(powY, powX, br, bc))
		{
			SafeAppendLog(_T("  peak: empty sweep data."));
		}
		else
		{
			CString msg;
			msg.Format(_T("  -> peak row=%d col=%d (0-based, RECAL 3 0 / 3 1)"), br, bc);
			SafeAppendLog(msg);
			const int nLut = (int)powY.size();
			ApplyRecalPeakToLut(st, idxOcc3, idxOcc4, nLut, br, bc, m_lutByTrans[fileSlot]);
		}

		++globalProgress;
		SafeSetProgressPos(globalProgress);
	}
	(void)globalTotal;
}

void CM576CalibratorDlg::RunPathPd()
{
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
	TryPreloadLutFromPerTransBackup();

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
}

void CM576CalibratorDlg::RunPathPdFile(int fileSlot, CArray<SPathStepPd, SPathStepPd const&>& steps, int& globalProgress, int globalTotal, int& occT3, int& occT4)
{
	CString err;
	const int total = (int)steps.GetSize();
	const DWORD readTimeout1d = ComputeRecal1DReadTimeoutMs(m_delayMs, m_dacRange, m_dacStep);
	const int gridN = AxisPointCount(m_dacRange, m_dacStep);
	CStringA lineOk, lineY, lineX;
	for (int i = 0; i < total; ++i)
	{
		if (m_bStop)
			break;
		SPathStepPd& st = steps[i];
		CString verr;
		if (!ValidatePathStepPd(st, verr))
		{
			SafeAppendLog(verr);
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		CString wslot;
		if (!PdStepMatchesFileSlot(st, fileSlot, &wslot))
		{
			SafeAppendLog(wslot);
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

		if (!m_pRecal->SendRecal2(st, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineOk, 3000, err))
		{
			SafeAppendLog(_T("RECAL 2: timeout waiting for OK."));
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

		if (!m_pRecal->SendRecal5(0, M576_RECAL_FW_READ_BASE_DAC, m_dacRange, m_dacStep, m_delayMs, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiSweepResponse(lineY, readTimeout1d, err))
		{
			SafeAppendLog(_T("RECAL 5 0 (Y sweep): timeout or empty."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		double xFixedDacPd = 0.0;
		std::vector<double> powY;
		if (!CRecalSession::ParseRecal3SweepLine(lineY, xFixedDacPd, powY))
		{
			SafeAppendLog(_T("RECAL 5 0: could not parse [X_start] P1..Pn."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 5 0 -> %d samples (X_start=%.4g)"), (int)powY.size(), xFixedDacPd);
			SafeAppendLog(msg);
		}

		int brForYBasePd = 0;
		if (powY.empty() || !M576::PeakMax1D(powY, brForYBasePd))
		{
			SafeAppendLog(_T("  RECAL 5 1: no Y peak index; skip X sweep."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		const int yBaseDacForSweep1Pd =
			RecalYBaseDacFromYPeakIndex(brForYBasePd, (int)powY.size(), m_dacRange);
		{
			CString msg;
			msg.Format(_T("  RECAL 5 1 Base DAC (Y@peak, row=%d)=%d"), brForYBasePd, yBaseDacForSweep1Pd);
			SafeAppendLog(msg);
		}

		if (!m_pRecal->SendRecal5(1, yBaseDacForSweep1Pd, m_dacRange, m_dacStep, m_delayMs, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiSweepResponse(lineX, readTimeout1d, err))
		{
			SafeAppendLog(_T("RECAL 5 1 (X sweep): timeout or empty."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		double yFixedDacPd = 0.0;
		std::vector<double> powX;
		if (!CRecalSession::ParseRecal3SweepLine(lineX, yFixedDacPd, powX))
		{
			SafeAppendLog(_T("RECAL 5 1: could not parse [Y_start] P1..Pn."));
			++globalProgress;
			SafeSetProgressPos(globalProgress);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 5 1 -> %d samples (Y_start=%.4g)"), (int)powX.size(), yFixedDacPd);
			SafeAppendLog(msg);
		}

		if ((int)powY.size() != gridN || (int)powX.size() != gridN)
		{
			CString msg;
			msg.Format(_T("  warning: sample count (%d, %d) != expected axis points %d"),
				(int)powY.size(), (int)powX.size(), gridN);
			SafeAppendLog(msg);
		}

		int br = 0, bc = 0;
		if (powY.size() != powX.size() || powY.empty())
		{
			SafeAppendLog(_T("  peak: Y/X sweep lengths differ or empty; skip LUT update."));
		}
		else if (!M576::PeakCrossFrom1DScans(powY, powX, br, bc))
		{
			SafeAppendLog(_T("  peak: empty sweep data."));
		}
		else
		{
			CString msg;
			msg.Format(_T("  -> peak row=%d col=%d (0-based, RECAL 5 0 / 5 1)"), br, bc);
			SafeAppendLog(msg);
			const int nLut = (int)powY.size();
			ApplyRecalPeakToLutPd(st, idxOcc3, idxOcc4, nLut, br, bc, m_lutByTrans[fileSlot]);
		}

		++globalProgress;
		SafeSetProgressPos(globalProgress);
	}
	(void)globalTotal;
}

void CM576CalibratorDlg::OnBnClickedGenBin()
{
	UpdateData(TRUE);
	if (m_strOutBin.IsEmpty())
	{
		AppendLog(_T("Set output BIN base path (writes <base>_mcs1.bin … <base>_1x64_2.bin)."));
		return;
	}
	const CString absBackupBin = ResolveFilePath(m_strBackupBin);
	const CString absOutBase = ResolveFilePath(m_strOutBin);
	CString sn = m_strSn;
	if (sn.IsEmpty())
		sn = _T("SN000000");

	for (int i = 0; i < 4; ++i)
	{
		stLutSettingZ4671 merged;
		ZeroMemory(&merged, sizeof(merged));
		BOOL haveBackup = FALSE;
		if (!m_strBackupBin.IsEmpty())
		{
			const CString perTransBk = M576TransBinPathForRead(absBackupBin, i + 1);
			if (GetFileAttributes(perTransBk) != INVALID_FILE_ATTRIBUTES)
				haveBackup = CLutBinWriter::ReadLutFromFile(perTransBk, merged);
			if (!haveBackup && i == 0 && GetFileAttributes(absBackupBin) != INVALID_FILE_ATTRIBUTES)
			{
				haveBackup = CLutBinWriter::ReadLutFromFile(absBackupBin, merged);
				if (haveBackup)
					AppendLog(_T("Trans1: read legacy single backup file for merge."));
			}
		}
		if (haveBackup)
		{
			MergeLut1310LowTempSlot(merged, m_lutByTrans[i]);
			CString m;
			m.Format(_T("Trans %d: merged session LUT into per-trans backup."), i + 1);
			AppendLog(m);
		}
		else
		{
			memcpy(&merged, &m_lutByTrans[i], sizeof(merged));
			CString m;
			m.Format(_T("Trans %d: no per-trans backup (*%s*.bin); writing in-memory LUT only."), i + 1,
				g_m576TransLutBinSuffix[i]);
			AppendLog(m);
		}

		const CString absOutOne = M576TransBackupPathFromBase(absOutBase, i + 1);
		SLutBinWriteParams p;
		p.strOutputPath = absOutOne;
		p.pLut = &merged;
		p.strBundleSN = sn;
		if (!CLutBinWriter::Write(p))
		{
			CString m;
			m.Format(_T("Write BIN failed (trans %d): %s"), i + 1, absOutOne.GetString());
			AppendLog(m);
			return;
		}
		memcpy(&m_lutByTrans[i], &merged, sizeof(m_lutByTrans[i]));
		CString ok;
		ok.Format(_T("Trans %d: wrote %s"), i + 1, absOutOne.GetString());
		AppendLog(ok);
	}
	AppendLog(_T("All trans BIN files written."));
}

void CM576CalibratorDlg::ProgressThunk(int cur, int total, void* user)
{
	CM576CalibratorDlg* p = (CM576CalibratorDlg*)user;
	if (!p || !::IsWindow(p->m_hWnd))
		return;
	int pct = (total > 0) ? (cur * 100 / total) : 0;
	p->SafeSetProgressPos(pct);
}

void CM576CalibratorDlg::OnBnClickedFlash()
{
	UpdateData(TRUE);
	if (m_readBackupRunning.load())
	{
		AppendLog(_T("Read Flash backup in progress; wait before burning flash."));
		return;
	}
	if (m_strOutBin.IsEmpty())
	{
		AppendLog(_T("Set output BIN base; burn: MCS (trans1–2) FW stream, 1x64 (trans3–4) fwdl+XMODEM, per *_mcs* / *_1x64_*.bin."));
		return;
	}
	const CString absOutBin = ResolveFilePath(m_strOutBin);
	BOOL anyBin = FALSE;
	for (int ti = 1; ti <= 4; ++ti)
	{
		const CString p = M576TransBinPathForRead(absOutBin, ti);
		if (GetFileAttributes(p) != INVALID_FILE_ATTRIBUTES)
		{
			HANDLE h = CreateFile(p, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (h != INVALID_HANDLE_VALUE)
			{
				const DWORD sz = GetFileSize(h, NULL);
				CloseHandle(h);
				if (sz > 0)
					anyBin = TRUE;
			}
		}
	}
	if (!anyBin)
	{
		AppendLog(_T("No non-empty per-trans .bin (*_mcs* / *_1x64_*) for this base; run Write BIN or place backups first."));
		return;
	}
	if (!m_dev429f.GetPortHandle() || m_dev429f.GetPortHandle() == INVALID_HANDLE_VALUE)
	{
		if (!OpenPort())
			return;
		SyncSerialPortUi();
	}
	CString err;
	m_progress.SetRange(0, 100);
	if (!McsFwUploadBinEx(m_dev429f, absOutBin, err, &CM576CalibratorDlg::ProgressThunk, this))
	{
		CString m;
		m.Format(_T("Flash failed: %s"), (LPCTSTR)err);
		AppendLog(m);
		return;
	}
	AppendLog(_T("Flash completed: trans1–2 via MCS update stream, trans3–4 via 1x64 XMODEM (per-file from output base)."));
}
