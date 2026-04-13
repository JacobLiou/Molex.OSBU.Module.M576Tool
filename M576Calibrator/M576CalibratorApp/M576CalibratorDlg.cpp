#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576CalibratorDlg.h"
#include "LutMerge1310.h"
#include "CalibConstants.h"
#include "PeakFinder2D.h"
#include "LutPeakApply.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace {

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

} // namespace

CM576CalibratorDlg::CM576CalibratorDlg(CWnd* pParent)
	: CDialogEx(IDD, pParent)
	, m_bStop(FALSE)
	, m_nCalMode(0)
	, m_delayMs(M576_DEFAULT_RECAL_DELAY_MS)
	, m_dacRange(M576_DEFAULT_DAC_RANGE)
	, m_dacStep(M576_DEFAULT_DAC_STEP)
{
	m_strCsv        = _T("output\\standard_pm.csv");
	m_strOutBin     = _T("output\\standard.bin");
	m_strBackupBin  = _T("output\\mcs_lut_backup.bin");
}

void CM576CalibratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM, m_comboCom);
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
	ON_BN_CLICKED(IDC_BTN_BROWSE_BACKUP, &CM576CalibratorDlg::OnBnClickedBrowseBackup)
	ON_BN_CLICKED(IDC_BTN_BROWSE_OUT, &CM576CalibratorDlg::OnBnClickedBrowseOut)
	ON_BN_CLICKED(IDC_BTN_READ_FLASH_BACKUP, &CM576CalibratorDlg::OnBnClickedReadFlashBackup)
	ON_BN_CLICKED(IDC_RADIO_CAL_PM, &CM576CalibratorDlg::OnBnClickedCalPm)
	ON_BN_CLICKED(IDC_RADIO_CAL_PD, &CM576CalibratorDlg::OnBnClickedCalPd)
	ON_BN_CLICKED(IDC_BTN_RUN_PATH, &CM576CalibratorDlg::OnBnClickedRunPath)
	ON_BN_CLICKED(IDC_BTN_GEN_BIN, &CM576CalibratorDlg::OnBnClickedGenBin)
	ON_BN_CLICKED(IDC_BTN_FLASH, &CM576CalibratorDlg::OnBnClickedFlash)
	ON_BN_CLICKED(IDC_BTN_STOP, &CM576CalibratorDlg::OnBnClickedStop)
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
	::SetDlgItemText(m_hWnd, IDC_BTN_OPEN_PORTS, _T("Open Port"));
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
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT_CSV)->EnableWindow(FALSE);
	FillComPorts();
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	ZeroMemory(&m_lut, sizeof(m_lut));
	AppendLog(_T("Ready. Select 429F COM port, open port, then run."));
	AppendLog(_T("Path CSV: PM mode -> .\\output\\standard_pm.csv (9 cols); PD mode -> .\\output\\standard_pd.csv (5 cols)."));
	AppendLog(_T("Backup BIN: use [Read Flash backup] for device LUT, or pick a local .bin to merge."));
	AppendLog(_T("Path CSV is fixed by mode selection and cannot be edited manually."));
	return TRUE;
}

void CM576CalibratorDlg::FillComPorts()
{
	for (int i = 1; i <= 32; ++i)
	{
		CString s;
		s.Format(_T("COM%d"), i);
		m_comboCom.AddString(s);
	}
	m_comboCom.SetCurSel(0);
}

void CM576CalibratorDlg::AppendLog(LPCTSTR sz)
{
	CString t;
	m_editLog.GetWindowText(t);
	if (!t.IsEmpty())
		t += _T("\r\n");
	t += sz;
	m_editLog.SetWindowText(t);
	int n = m_editLog.GetWindowTextLength();
	m_editLog.SetSel(n, n);
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

BOOL CM576CalibratorDlg::OpenPort()
{
	m_dev429f.ClosePort();

	CString sCom = GetComboCom();
	if (sCom.IsEmpty())
	{
		AppendLog(_T("Select COM port."));
		return FALSE;
	}
	CString path;
	path.Format(_T("\\\\.\\%s"), (LPCTSTR)sCom);

	if (!m_dev429f.OpenPort((LPTSTR)(LPCTSTR)path, 115200, 8, NOPARITY, ONESTOPBIT))
	{
		AppendLog(_T("Serial port open failed."));
		return FALSE;
	}
	m_pRecal.reset(new CRecalSession(m_dev429f));
	AppendLog(_T("Port opened (429F)."));
	return TRUE;
}

void CM576CalibratorDlg::OnBnClickedOpenPorts()
{
	UpdateData(TRUE);
	if (OpenPort())
		AppendLog(_T("Open port OK."));
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
	CString err;
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	if (!McsReadLutBundleFromDevice(m_dev429f, absBackupBin, err, &CM576CalibratorDlg::ProgressThunk, this))
	{
		CString m;
		m.Format(_T("Read Flash backup failed: %s"), (LPCTSTR)err);
		AppendLog(m);
		return;
	}
	UpdateData(FALSE);
	m_progress.SetPos(100);
	CString ok;
	ok.Format(_T("Flash LUT backup saved: %s"), (LPCTSTR)absBackupBin);
	AppendLog(ok);
}

void CM576CalibratorDlg::OnBnClickedStop()
{
	m_bStop = TRUE;
	AppendLog(_T("Stop requested."));
}

void CM576CalibratorDlg::OnBnClickedCalPm()
{
	UpdateData(TRUE);
	SyncCsvPathWithMode();
}

void CM576CalibratorDlg::OnBnClickedCalPd()
{
	UpdateData(TRUE);
	SyncCsvPathWithMode();
}

void CM576CalibratorDlg::OnBnClickedRunPath()
{
	UpdateData(TRUE);
	m_bStop = FALSE;
	if (!m_pRecal.get())
	{
		if (!OpenPort())
			return;
	}
	if (m_nCalMode == 0)
		RunPathPowerMeter();
	else
		RunPathPd();
}

void CM576CalibratorDlg::RunPathPowerMeter()
{
	CArray<SPathStep, SPathStep const&> steps;
	CString err;
	if (!LoadPathCsv(ResolveFilePath(m_strCsv), steps, err))
	{
		AppendLog(err);
		return;
	}
	int total = (int)steps.GetSize();
	m_progress.SetRange(0, total);

	ZeroMemory(&m_lut, sizeof(m_lut));
	int occT3 = 0, occT4 = 0;

	if (!m_pRecal->SendRecal0(M576_DEFAULT_TLS_SOURCE, M576_DEFAULT_WAVELENGTH_NM, m_delayMs, m_dacRange, m_dacStep, err))
	{
		AppendLog(err);
		return;
	}
	{
		CStringA line0;
		if (!m_pRecal->ReadAsciiResponse(line0, 3000, err))
			AppendLog(_T("RECAL 0: timeout waiting for response."));
		else
		{
			CString msg;
			msg.Format(_T("RECAL 0 -> %s"), CString(line0));
			AppendLog(msg);
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
			AppendLog(verr);
			continue;
		}
		if (!m_pRecal->SendRecal1(st, err))
		{
			AppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineOk, 3000, err))
		{
			AppendLog(_T("RECAL 1: timeout waiting for OK."));
			m_progress.SetPos(i + 1);
			continue;
		}
		{
			CString msg;
			lineOk.Trim();
			msg.Format(_T("Step %d/%d RECAL 1 -> %s"), i + 1, total, CString(lineOk));
			AppendLog(msg);
			if (lineOk.CompareNoCase("OK") != 0)
			{
				msg.Format(_T("  (expected OK; continuing with RECAL 3)"));
				AppendLog(msg);
			}
		}

		const int kBaseDac = 0;
		if (!m_pRecal->SendRecal3(0, kBaseDac, m_dacRange, m_dacStep, m_delayMs, err))
		{
			AppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineY, readTimeout1d, err))
		{
			AppendLog(_T("RECAL 3 0 (Y sweep): timeout or empty."));
			m_progress.SetPos(i + 1);
			continue;
		}
		double yAxisStart = 0.0;
		std::vector<double> powY;
		if (!CRecalSession::ParseRecal3SweepLine(lineY, yAxisStart, powY))
		{
			AppendLog(_T("RECAL 3 0: could not parse [X_start] P1..Pn."));
			m_progress.SetPos(i + 1);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 3 0 -> %d samples (X_start=%.4g)"), (int)powY.size(), yAxisStart);
			AppendLog(msg);
		}

		if (!m_pRecal->SendRecal3(1, kBaseDac, m_dacRange, m_dacStep, m_delayMs, err))
		{
			AppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineX, readTimeout1d, err))
		{
			AppendLog(_T("RECAL 3 1 (X sweep): timeout or empty."));
			m_progress.SetPos(i + 1);
			continue;
		}
		double xAxisStart = 0.0;
		std::vector<double> powX;
		if (!CRecalSession::ParseRecal3SweepLine(lineX, xAxisStart, powX))
		{
			AppendLog(_T("RECAL 3 1: could not parse [Y_start] P1..Pn."));
			m_progress.SetPos(i + 1);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 3 1 -> %d samples (Y_start=%.4g)"), (int)powX.size(), xAxisStart);
			AppendLog(msg);
		}

		if ((int)powY.size() != gridN || (int)powX.size() != gridN)
		{
			CString msg;
			msg.Format(_T("  warning: sample count (%d, %d) != expected axis points %d"),
				(int)powY.size(), (int)powX.size(), gridN);
			AppendLog(msg);
		}

		int br = 0, bc = 0;
		if (powY.size() != powX.size() || powY.empty())
		{
			AppendLog(_T("  peak: Y/X sweep lengths differ or empty; skip LUT update."));
		}
		else if (!M576::PeakMax1D(powY, br) || !M576::PeakMax1D(powX, bc))
		{
			AppendLog(_T("  peak: empty sweep data."));
		}
		else
		{
			CString msg;
			msg.Format(_T("  -> peak row=%d col=%d (0-based, RECAL 3 0 / 3 1)"), br, bc);
			AppendLog(msg);
			const int nLut = (int)powY.size();
			ApplyRecalPeakToLut(st, idxOcc3, idxOcc4, nLut, br, bc, m_lut);
		}

		m_progress.SetPos(i + 1);
	}
	AppendLog(_T("Path run finished."));
}

void CM576CalibratorDlg::RunPathPd()
{
	CArray<SPathStepPd, SPathStepPd const&> steps;
	CString err;
	if (!LoadPathCsvPd(ResolveFilePath(m_strCsv), steps, err))
	{
		AppendLog(err);
		return;
	}
	int total = (int)steps.GetSize();
	m_progress.SetRange(0, total);

	ZeroMemory(&m_lut, sizeof(m_lut));
	int occT3 = 0, occT4 = 0;

	if (!m_pRecal->SendRecal0(M576_DEFAULT_TLS_SOURCE, M576_DEFAULT_WAVELENGTH_NM, m_delayMs, m_dacRange, m_dacStep, err))
	{
		AppendLog(err);
		return;
	}
	{
		CStringA line0;
		if (!m_pRecal->ReadAsciiResponse(line0, 3000, err))
			AppendLog(_T("RECAL 0: timeout waiting for response."));
		else
		{
			CString msg;
			msg.Format(_T("RECAL 0 -> %s"), CString(line0));
			AppendLog(msg);
		}
	}

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
			AppendLog(verr);
			continue;
		}
		if (!m_pRecal->SendRecal2(st, err))
		{
			AppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineOk, 3000, err))
		{
			AppendLog(_T("RECAL 2: timeout waiting for OK."));
			m_progress.SetPos(i + 1);
			continue;
		}
		{
			CString msg;
			lineOk.Trim();
			msg.Format(_T("PD step %d/%d RECAL 2 -> %s"), i + 1, total, CString(lineOk));
			AppendLog(msg);
			if (lineOk.CompareNoCase("OK") != 0)
			{
				msg.Format(_T("  (expected OK; continuing with RECAL 5)"));
				AppendLog(msg);
			}
		}

		const int kBaseDac = 0;
		if (!m_pRecal->SendRecal5(0, kBaseDac, m_dacRange, m_dacStep, m_delayMs, err))
		{
			AppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineY, readTimeout1d, err))
		{
			AppendLog(_T("RECAL 5 0 (Y sweep): timeout or empty."));
			m_progress.SetPos(i + 1);
			continue;
		}
		double yAxisStart = 0.0;
		std::vector<double> powY;
		if (!CRecalSession::ParseRecal3SweepLine(lineY, yAxisStart, powY))
		{
			AppendLog(_T("RECAL 5 0: could not parse [X_start] P1..Pn."));
			m_progress.SetPos(i + 1);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 5 0 -> %d samples (X_start=%.4g)"), (int)powY.size(), yAxisStart);
			AppendLog(msg);
		}

		if (!m_pRecal->SendRecal5(1, kBaseDac, m_dacRange, m_dacStep, m_delayMs, err))
		{
			AppendLog(err);
			break;
		}
		if (!m_pRecal->ReadAsciiResponse(lineX, readTimeout1d, err))
		{
			AppendLog(_T("RECAL 5 1 (X sweep): timeout or empty."));
			m_progress.SetPos(i + 1);
			continue;
		}
		double xAxisStart = 0.0;
		std::vector<double> powX;
		if (!CRecalSession::ParseRecal3SweepLine(lineX, xAxisStart, powX))
		{
			AppendLog(_T("RECAL 5 1: could not parse [Y_start] P1..Pn."));
			m_progress.SetPos(i + 1);
			continue;
		}
		{
			CString msg;
			msg.Format(_T("  RECAL 5 1 -> %d samples (Y_start=%.4g)"), (int)powX.size(), xAxisStart);
			AppendLog(msg);
		}

		if ((int)powY.size() != gridN || (int)powX.size() != gridN)
		{
			CString msg;
			msg.Format(_T("  warning: sample count (%d, %d) != expected axis points %d"),
				(int)powY.size(), (int)powX.size(), gridN);
			AppendLog(msg);
		}

		int br = 0, bc = 0;
		if (powY.size() != powX.size() || powY.empty())
		{
			AppendLog(_T("  peak: Y/X sweep lengths differ or empty; skip LUT update."));
		}
		else if (!M576::PeakMax1D(powY, br) || !M576::PeakMax1D(powX, bc))
		{
			AppendLog(_T("  peak: empty sweep data."));
		}
		else
		{
			CString msg;
			msg.Format(_T("  -> peak row=%d col=%d (0-based, RECAL 5 0 / 5 1)"), br, bc);
			AppendLog(msg);
			const int nLut = (int)powY.size();
			ApplyRecalPeakToLutPd(st, idxOcc3, idxOcc4, nLut, br, bc, m_lut);
		}

		m_progress.SetPos(i + 1);
	}
	AppendLog(_T("Path run finished (PD)."));
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
	p->m_progress.SetPos(pct);
}

void CM576CalibratorDlg::OnBnClickedFlash()
{
	UpdateData(TRUE);
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
