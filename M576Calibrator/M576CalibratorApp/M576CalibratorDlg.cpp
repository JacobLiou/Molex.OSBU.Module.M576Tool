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

constexpr UINT WM_M576_PATH_LOG = WM_APP + 100;
constexpr UINT WM_M576_PATH_PROGRESS_RANGE = WM_APP + 101;
constexpr UINT WM_M576_PATH_PROGRESS_POS = WM_APP + 102;
constexpr UINT WM_M576_PATH_FINISHED = WM_APP + 103;
constexpr UINT WM_M576_READ_BACKUP_FINISHED = WM_APP + 104;

/// One `RECAL 3` / `RECAL 5` axis sweep: one sample per grid step (not full 2D grid).
static DWORD ComputeRecal1DReadTimeoutMs(int delayMs, int dacRange, int dacStep)
{
	if (delayMs < 1)
		delayMs = 1;
	if (dacStep < 1)
		dacStep = 1;
	int axisPts = (2 * dacRange + dacStep - 1) / dacStep + 1;
	if (axisPts < 2)
		axisPts = 2;
	__int64 t = (__int64)axisPts * delayMs + 10000;
	if (t > 600000)
		t = 600000;
	if (t < 5000)
		t = 5000;
	return (DWORD)t;
}

static int AxisPointCount(int dacRange, int dacStep)
{
	if (dacStep < 1)
		dacStep = 1;
	int axisPts = (2 * dacRange + dacStep - 1) / dacStep + 1;
	if (axisPts < 2)
		axisPts = 2;
	return axisPts;
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
{
	m_strCsv        = _T("output\\standard_pm.csv");
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
	DDX_Text(pDX, IDC_EDIT_CSV, m_strCsv);
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
	ON_BN_CLICKED(IDC_BTN_GEN_BIN, &CM576CalibratorDlg::OnBnClickedGenBin)
	ON_BN_CLICKED(IDC_BTN_FLASH, &CM576CalibratorDlg::OnBnClickedFlash)
	ON_BN_CLICKED(IDC_BTN_STOP, &CM576CalibratorDlg::OnBnClickedStop)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_M576_PATH_LOG, &CM576CalibratorDlg::OnPathLog)
	ON_MESSAGE(WM_M576_PATH_PROGRESS_RANGE, &CM576CalibratorDlg::OnPathProgressRange)
	ON_MESSAGE(WM_M576_PATH_PROGRESS_POS, &CM576CalibratorDlg::OnPathProgressPos)
	ON_MESSAGE(WM_M576_PATH_FINISHED, &CM576CalibratorDlg::OnPathFinished)
	ON_MESSAGE(WM_M576_READ_BACKUP_FINISHED, &CM576CalibratorDlg::OnReadBackupFinished)
END_MESSAGE_MAP()

BOOL CM576CalibratorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if (hIcon)
	{
		SetIcon(hIcon, TRUE);
		SetIcon(hIcon, FALSE);
	}
	SetWindowText(_T("M576 / 1310 Calibrator (429F)"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_CONN, _T("Connection"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_PATHS, _T("Config"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_ACTIONS, _T("Actions"));
	::SetDlgItemText(m_hWnd, IDC_GROUP_LOG, _T("Log"));
	::SetDlgItemText(m_hWnd, IDC_STATIC_LABEL_COM, _T("Port (429F):"));
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
	GetDlgItem(IDC_EDIT_CSV)->EnableWindow(FALSE);
	FillComPorts();
	SyncSerialPortUi();
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	ZeroMemory(&m_lut, sizeof(m_lut));
	AppendLog(_T("Ready. Select 429F COM port, open port, then run."));
	AppendLog(_T("Backup BIN: use [Read Flash backup] for device LUT, or pick a local .bin to merge."));

	AppendLog(_T("Path CSV: PM mode -> .\\output\\standard_pm.csv; PD mode -> .\\output\\standard_pd.csv"));
	AppendLog(_T("Path CSV is fixed by mode selection and cannot be edited manually."));
	AppendLog(_T("PM: Command A (RECAL 0) + Command B (RECAL 1) + RECAL 3; PD: Command C (RECAL 2) + RECAL 5 only (no RECAL 0)."));
	return TRUE;
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
	CString* p = new CString(sz);
	if (!::PostMessage(m_hWnd, WM_M576_PATH_LOG, 0, reinterpret_cast<LPARAM>(p)))
		delete p;
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

LRESULT CM576CalibratorDlg::OnPathLog(WPARAM, LPARAM lParam)
{
	CString* p = reinterpret_cast<CString*>(lParam);
	if (p)
	{
		AppendLog(p->GetString());
		delete p;
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
		ok.Format(_T("Flash LUT backup saved: %s"), (LPCTSTR)absBackupBin);
		SafeAppendLog(ok);
	}
	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_READ_BACKUP_FINISHED, 0, 0);
}

void CM576CalibratorDlg::WriteLogFileLine(const CString& line)
{
	const CString absPath = ResolveFilePath(m_strCommLogPath);
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

CString CM576CalibratorDlg::GetDefaultCsvPathForMode() const
{
	return (m_nCalMode == 0) ? _T("output\\standard_pm.csv") : _T("output\\standard_pd.csv");
}

void CM576CalibratorDlg::SyncCsvPathWithMode()
{
	m_strCsv = GetDefaultCsvPathForMode();
	if (m_hWnd)
		UpdateData(FALSE);
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
	AppendLog(_T("Port opened (429F)."));
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

void CM576CalibratorDlg::OnBrowse(UINT idEdit)
{
	const CString exe = GetExeFolder();
	CString initDir = exe;
	const CString outDir = exe + _T("\\output");
	if (GetFileAttributes(outDir) != INVALID_FILE_ATTRIBUTES)
		initDir = outDir;
	const CString defaultName = (idEdit == IDC_EDIT_CSV) ? GetDefaultCsvPathForMode().Mid(GetDefaultCsvPathForMode().ReverseFind(_T('\\')) + 1) : _T("standard.csv");
	CFileDialog dlg(TRUE, _T("csv"), defaultName, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("CSV files (*.csv)|*.csv|All (*.*)|*.*||"), this);
	dlg.GetOFN().lpstrInitialDir = initDir.GetString();
	if (dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(idEdit, ToRelPath(dlg.GetPathName()));
	UpdateData(TRUE);
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
		errMsg = _T("Select 429F COM port (Port).");
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
	const CString absCsv = ResolveFilePath(m_strCsv);
	if (absCsv.IsEmpty() || GetFileAttributes(absCsv) == INVALID_FILE_ATTRIBUTES)
	{
		errMsg.Format(_T("Path CSV not found or not accessible for this mode:\n%s"), (LPCTSTR)m_strCsv);
		return FALSE;
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
	SetPathActionButtonsEnabled(FALSE);
	if (CWnd* pStop = GetDlgItem(IDC_BTN_STOP))
		pStop->EnableWindow(TRUE);
	m_pathThread = std::thread([this]() { PathWorkerEntry(); });
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

void CM576CalibratorDlg::RunPathPowerMeter()
{
	CArray<SPathStep, SPathStep const&> steps;
	CString err;
	if (!LoadPathCsv(ResolveFilePath(m_strCsv), steps, err))
	{
		SafeAppendLog(err);
		return;
	}
	int total = (int)steps.GetSize();
	SafeSetProgressRange(0, total);

	ZeroMemory(&m_lut, sizeof(m_lut));
	int occT3 = 0, occT4 = 0;

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

	const DWORD readTimeout1d = ComputeRecal1DReadTimeoutMs(m_delayMs, m_dacRange, m_dacStep);
	const int gridN = AxisPointCount(m_dacRange, m_dacStep);
	CStringA lineOk, lineY, lineX;
	for (int i = 0; i < total; ++i)
	{
		if (m_bStop)
			break;
		SPathStep& st = steps[i];
		const int idxOcc3 = (st.targetSwitchIndex == 3) ? occT3 : -1;
		const int idxOcc4 = (st.targetSwitchIndex == 4) ? occT4 : -1;
		if (st.targetSwitchIndex == 3)
			occT3++;
		else if (st.targetSwitchIndex == 4)
			occT4++;

		CString verr;
		if (!ValidatePathStep(st, verr))
		{
			SafeAppendLog(verr);
			continue;
		}
		if (!m_pRecal->SendRecal1(st, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineOk, 3000, err))
		{
			SafeAppendLog(_T("RECAL 1: timeout waiting for OK."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		{
			CString msg;
			lineOk.Trim();
			msg.Format(_T("Step %d/%d RECAL 1 -> %s"), i + 1, total, CString(lineOk));
			SafeAppendLog(msg);
			if (lineOk.CompareNoCase("OK") != 0)
			{
				msg.Format(_T("  (expected OK; continuing with RECAL 3)"));
				SafeAppendLog(msg);
			}
		}

		const int kBaseDac = 0;
		if (!m_pRecal->SendRecal3(0, kBaseDac, m_dacRange, m_dacStep, m_delayMs, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineY, readTimeout1d, err))
		{
			SafeAppendLog(_T("RECAL 3 0 (Y sweep): timeout or empty."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		double yAxisStart = 0.0;
		std::vector<double> powY;
		if (!CRecalSession::ParseRecal3SweepLine(lineY, yAxisStart, powY))
		{
			SafeAppendLog(_T("RECAL 3 0: could not parse [X_start] P1..Pn."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 3 0 -> %d samples (X_start=%.4g)"), (int)powY.size(), yAxisStart);
			SafeAppendLog(msg);
		}

		if (!m_pRecal->SendRecal3(1, kBaseDac, m_dacRange, m_dacStep, m_delayMs, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineX, readTimeout1d, err))
		{
			SafeAppendLog(_T("RECAL 3 1 (X sweep): timeout or empty."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		double xAxisStart = 0.0;
		std::vector<double> powX;
		if (!CRecalSession::ParseRecal3SweepLine(lineX, xAxisStart, powX))
		{
			SafeAppendLog(_T("RECAL 3 1: could not parse [Y_start] P1..Pn."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 3 1 -> %d samples (Y_start=%.4g)"), (int)powX.size(), xAxisStart);
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
			msg.Format(_T("  -> peak row=%d col=%d (0-based, RECAL 3 0 / 3 1)"), br, bc);
			SafeAppendLog(msg);
			const int nLut = (int)powY.size();
			ApplyRecalPeakToLut(st, idxOcc3, idxOcc4, nLut, br, bc, m_lut);
		}

		SafeSetProgressPos(i + 1);
	}
	SafeAppendLog(_T("Path run finished."));
}

void CM576CalibratorDlg::RunPathPd()
{
	CArray<SPathStepPd, SPathStepPd const&> steps;
	CString err;
	if (!LoadPathCsvPd(ResolveFilePath(m_strCsv), steps, err))
	{
		SafeAppendLog(err);
		return;
	}
	int total = (int)steps.GetSize();
	SafeSetProgressRange(0, total);

	ZeroMemory(&m_lut, sizeof(m_lut));
	int occT3 = 0, occT4 = 0;

	/// PD: Command C only (RECAL 2 + RECAL 5). No Command A (RECAL 0).

	const DWORD readTimeout1d = ComputeRecal1DReadTimeoutMs(m_delayMs, m_dacRange, m_dacStep);
	const int gridN = AxisPointCount(m_dacRange, m_dacStep);
	CStringA lineOk, lineY, lineX;
	for (int i = 0; i < total; ++i)
	{
		if (m_bStop)
			break;
		SPathStepPd& st = steps[i];
		const int idxOcc3 = (st.targetSwitchIndex == 3) ? occT3 : -1;
		const int idxOcc4 = (st.targetSwitchIndex == 4) ? occT4 : -1;
		if (st.targetSwitchIndex == 3)
			occT3++;
		else if (st.targetSwitchIndex == 4)
			occT4++;

		CString verr;
		if (!ValidatePathStepPd(st, verr))
		{
			SafeAppendLog(verr);
			continue;
		}
		if (!m_pRecal->SendRecal2(st, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineOk, 3000, err))
		{
			SafeAppendLog(_T("RECAL 2: timeout waiting for OK."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		{
			CString msg;
			lineOk.Trim();
			msg.Format(_T("PD step %d/%d RECAL 2 -> %s"), i + 1, total, CString(lineOk));
			SafeAppendLog(msg);
			if (lineOk.CompareNoCase("OK") != 0)
			{
				msg.Format(_T("  (expected OK; continuing with RECAL 5)"));
				SafeAppendLog(msg);
			}
		}

		const int kBaseDac = 0;
		if (!m_pRecal->SendRecal5(0, kBaseDac, m_dacRange, m_dacStep, m_delayMs, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineY, readTimeout1d, err))
		{
			SafeAppendLog(_T("RECAL 5 0 (Y sweep): timeout or empty."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		double yAxisStart = 0.0;
		std::vector<double> powY;
		if (!CRecalSession::ParseRecal3SweepLine(lineY, yAxisStart, powY))
		{
			SafeAppendLog(_T("RECAL 5 0: could not parse [X_start] P1..Pn."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 5 0 -> %d samples (X_start=%.4g)"), (int)powY.size(), yAxisStart);
			SafeAppendLog(msg);
		}

		if (!m_pRecal->SendRecal5(1, kBaseDac, m_dacRange, m_dacStep, m_delayMs, err))
		{
			SafeAppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineX, readTimeout1d, err))
		{
			SafeAppendLog(_T("RECAL 5 1 (X sweep): timeout or empty."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		double xAxisStart = 0.0;
		std::vector<double> powX;
		if (!CRecalSession::ParseRecal3SweepLine(lineX, xAxisStart, powX))
		{
			SafeAppendLog(_T("RECAL 5 1: could not parse [Y_start] P1..Pn."));
			SafeSetProgressPos(i + 1);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 5 1 -> %d samples (Y_start=%.4g)"), (int)powX.size(), xAxisStart);
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
			ApplyRecalPeakToLutPd(st, idxOcc3, idxOcc4, nLut, br, bc, m_lut);
		}

		SafeSetProgressPos(i + 1);
	}
	SafeAppendLog(_T("Path run finished (PD)."));
}

void CM576CalibratorDlg::OnBnClickedGenBin()
{
	UpdateData(TRUE);
	if (m_strOutBin.IsEmpty())
	{
		AppendLog(_T("Set output BIN path."));
		return;
	}
	stLutSettingZ4671 merged;
	ZeroMemory(&merged, sizeof(merged));
	const CString absBackupBin = ResolveFilePath(m_strBackupBin);
	const CString absOutBin = ResolveFilePath(m_strOutBin);
	if (!m_strBackupBin.IsEmpty() && GetFileAttributes(absBackupBin) != INVALID_FILE_ATTRIBUTES)
	{
		if (!CLutBinWriter::ReadLutFromFile(absBackupBin, merged))
		{
			AppendLog(_T("Read backup BIN failed."));
			return;
		}
		MergeLut1310LowTempSlot(merged, m_lut);
		AppendLog(_T("Merged 1310 low-temp slot from session LUT into backup."));
	}
	else
	{
		merged = m_lut;
		AppendLog(_T("No backup BIN (or missing file); using in-memory LUT only."));
	}

	SLutBinWriteParams p;
	p.strOutputPath = absOutBin;
	p.pLut = &merged;
	p.strBundleSN = m_strSn;
	if (p.strBundleSN.IsEmpty())
		p.strBundleSN = _T("SN000000");
	if (!CLutBinWriter::Write(p))
	{
		AppendLog(_T("Write BIN failed."));
		return;
	}
	m_lut = merged;
	AppendLog(_T("BIN written."));
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
		AppendLog(_T("Set output BIN path (same file used for flash)."));
		return;
	}
	const CString absOutBin = ResolveFilePath(m_strOutBin);
	if (GetFileAttributes(absOutBin) == INVALID_FILE_ATTRIBUTES)
	{
		AppendLog(_T("BIN not found."));
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
	AppendLog(_T("Flash completed (via 429F forward to MCS)."));
}
