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

} // namespace

CM576CalibratorDlg::CM576CalibratorDlg(CWnd* pParent)
	: CDialogEx(IDD, pParent)
	, m_bStop(FALSE)
{
	const CString exe = GetExeFolder();
	if (!exe.IsEmpty())
	{
		const CString sub = exe + _T("\\output");
		m_strCsv = sub + _T("\\standard.csv");
		m_strOutBin = sub + _T("\\standard.bin");
		m_strBackupBin = sub + _T("\\mcs_lut_backup.bin");
	}
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
}

BEGIN_MESSAGE_MAP(CM576CalibratorDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OPEN_PORTS, &CM576CalibratorDlg::OnBnClickedOpenPorts)
	ON_BN_CLICKED(IDC_BTN_BROWSE_CSV, &CM576CalibratorDlg::OnBnClickedBrowseCsv)
	ON_BN_CLICKED(IDC_BTN_BROWSE_BACKUP, &CM576CalibratorDlg::OnBnClickedBrowseBackup)
	ON_BN_CLICKED(IDC_BTN_BROWSE_OUT, &CM576CalibratorDlg::OnBnClickedBrowseOut)
	ON_BN_CLICKED(IDC_BTN_READ_FLASH_BACKUP, &CM576CalibratorDlg::OnBnClickedReadFlashBackup)
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
	/// Chinese UI: avoid UTF-8 literals in .rc (RC code page); use Unicode API + \\u escapes.
	::SetDlgItemTextW(m_hWnd, IDC_GROUP_CONN,
		L"\u8fde\u63a5");
	::SetDlgItemTextW(m_hWnd, IDC_GROUP_PATHS,
		L"\u8def\u5f84\u4e0e\u6587\u4ef6");
	::SetDlgItemTextW(m_hWnd, IDC_GROUP_ACTIONS,
		L"\u64cd\u4f5c");
	::SetDlgItemTextW(m_hWnd, IDC_GROUP_LOG,
		L"\u65e5\u5fd7");
	::SetDlgItemTextW(m_hWnd, IDC_STATIC_LABEL_COM,
		L"\u4e32\u53e3 (429F):");
	::SetDlgItemTextW(m_hWnd, IDC_BTN_OPEN_PORTS,
		L"\u6253\u5f00\u4e32\u53e3");
	::SetDlgItemTextW(m_hWnd, IDC_BTN_FLASH,
		L"\u70e7\u5f55\u5b9a\u6807");
	::SetDlgItemTextW(m_hWnd, IDC_BTN_READ_FLASH_BACKUP,
		L"\u4ece\u8bbe\u5907\u8bfbFlash\u5907\u4efd");
	::SetDlgItemTextW(m_hWnd, IDC_STATIC_VERSION,
		L"\u4ec5\u8fde\u63a5 429F\uff1b\u5b9a\u6807\u70e7\u5f55\u7531\u4e3b\u677f\u900f\u4f20\u81f3 MCS");
	EnsureOutputFolderUnderExe(GetExeFolder());
	UpdateData(FALSE);
	FillComPorts();
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	ZeroMemory(&m_lut, sizeof(m_lut));
	AppendLog(_T("Ready. Select 429F COM port, open port, then run."));
	AppendLog(_T("Path CSV default: .\\output\\standard.csv (generate with tools if missing)."));
	AppendLog(_T("Backup BIN: use [Read Flash backup] for device LUT, or pick a local .bin to merge."));
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
	CFileDialog dlg(TRUE, _T("csv"), _T("standard.csv"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("CSV files (*.csv)|*.csv|All (*.*)|*.*||"), this);
	dlg.GetOFN().lpstrInitialDir = initDir.GetString();
	if (dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(idEdit, dlg.GetPathName());
	UpdateData(TRUE);
}

void CM576CalibratorDlg::OnBnClickedBrowseCsv()
{
	OnBrowse(IDC_EDIT_CSV);
}

void CM576CalibratorDlg::OnBnClickedBrowseBackup()
{
	const CString exe = GetExeFolder();
	CFileDialog dlg(TRUE, _T("bin"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("BIN (*.bin)|*.bin|All (*.*)|*.*||"), this);
	dlg.GetOFN().lpstrInitialDir = exe.GetString();
	if (dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(IDC_EDIT_BACKUP_BIN, dlg.GetPathName());
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
	SetDlgItemText(IDC_EDIT_OUT_BIN, dlg.GetPathName());
	UpdateData(TRUE);
}

void CM576CalibratorDlg::OnBnClickedReadFlashBackup()
{
	UpdateData(TRUE);
	EnsureOutputFolderUnderExe(GetExeFolder());
	if (m_strBackupBin.IsEmpty())
		m_strBackupBin = GetExeFolder() + _T("\\output\\mcs_lut_backup.bin");
	if (!m_dev429f.GetPortHandle() || m_dev429f.GetPortHandle() == INVALID_HANDLE_VALUE)
	{
		if (!OpenPort())
			return;
	}
	CString err;
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	if (!McsReadLutBundleFromDevice(m_dev429f, m_strBackupBin, err, &CM576CalibratorDlg::ProgressThunk, this))
	{
		CString m;
		m.Format(_T("Read Flash backup failed: %s"), (LPCTSTR)err);
		AppendLog(m);
		return;
	}
	UpdateData(FALSE);
	m_progress.SetPos(100);
	CString ok;
	ok.Format(_T("Flash LUT backup saved: %s"), (LPCTSTR)m_strBackupBin);
	AppendLog(ok);
}

void CM576CalibratorDlg::OnBnClickedStop()
{
	m_bStop = TRUE;
	AppendLog(_T("Stop requested."));
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
	CArray<SPathStep, SPathStep const&> steps;
	CString err;
	if (!LoadPathCsv(m_strCsv, steps, err))
	{
		AppendLog(err);
		return;
	}
	int total = (int)steps.GetSize();
	m_progress.SetRange(0, total);

	ZeroMemory(&m_lut, sizeof(m_lut));
	int occT3 = 0, occT4 = 0;

	// PRD: Command A — RECAL 0 (TLS source + wavelength) before RECAL 1 loop.
	// Full scan parameters (DAC range/step/delay) may be added when firmware contract is fixed.
	if (!m_pRecal->SendRecal0(M576_DEFAULT_TLS_SOURCE, M576_DEFAULT_WAVELENGTH_NM, err))
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

	CStringA line;
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
		if (!m_pRecal->ReadAsciiResponse(line, 5000, err))
		{
			AppendLog(_T("Timeout or empty response."));
		}
		else
		{
			CString msg;
			msg.Format(_T("Step %d/%d: %s"), i + 1, total, line.GetString());
			AppendLog(msg);
			std::vector<double> powers;
			if (CRecalSession::ParsePowerDoubles(line, powers) && powers.size() > 4)
			{
				int n = (int)sqrt((double)powers.size());
				if (n > 1 && n * n == (int)powers.size())
				{
					int br = 0, bc = 0;
					if (M576::PeakCross2D(powers, n, n, br, bc))
					{
						msg.Format(_T("  -> peak (cross) row=%d col=%d (0-based)"), br, bc);
						AppendLog(msg);
						ApplyRecalPeakToLut(st, idxOcc3, idxOcc4, n, br, bc, m_lut);
					}
				}
			}
		}
		m_progress.SetPos(i + 1);
	}
	AppendLog(_T("Path run finished."));
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
	if (!m_strBackupBin.IsEmpty() && GetFileAttributes(m_strBackupBin) != INVALID_FILE_ATTRIBUTES)
	{
		if (!CLutBinWriter::ReadLutFromFile(m_strBackupBin, merged))
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
	p.strOutputPath = m_strOutBin;
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
	if (GetFileAttributes(m_strOutBin) == INVALID_FILE_ATTRIBUTES)
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
	if (!McsFwUploadBinEx(m_dev429f, m_strOutBin, err, &CM576CalibratorDlg::ProgressThunk, this))
	{
		CString m;
		m.Format(_T("Flash failed: %s"), (LPCTSTR)err);
		AppendLog(m);
		return;
	}
	AppendLog(_T("Flash completed (via 429F forward to MCS)."));
}
