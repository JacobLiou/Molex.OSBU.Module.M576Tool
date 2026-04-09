#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576CalibratorDlg.h"
#include "LutMerge1310.h"
#include "CalibConstants.h"
#include "PeakFinder2D.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CM576CalibratorDlg::CM576CalibratorDlg(CWnd* pParent)
	: CDialog(IDD, pParent)
	, m_bStop(FALSE)
{
}

void CM576CalibratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_429F, m_combo429f);
	DDX_Control(pDX, IDC_COMBO_MCS, m_comboMcs);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
	DDX_Control(pDX, IDC_PROGRESS_MAIN, m_progress);
	DDX_Text(pDX, IDC_EDIT_CSV, m_strCsv);
	DDX_Text(pDX, IDC_EDIT_BACKUP_BIN, m_strBackupBin);
	DDX_Text(pDX, IDC_EDIT_OUT_BIN, m_strOutBin);
	DDX_Text(pDX, IDC_EDIT_SN, m_strSn);
}

BEGIN_MESSAGE_MAP(CM576CalibratorDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_OPEN_PORTS, &CM576CalibratorDlg::OnBnClickedOpenPorts)
	ON_BN_CLICKED(IDC_BTN_BROWSE_CSV, &CM576CalibratorDlg::OnBnClickedBrowseCsv)
	ON_BN_CLICKED(IDC_BTN_BROWSE_BACKUP, &CM576CalibratorDlg::OnBnClickedBrowseBackup)
	ON_BN_CLICKED(IDC_BTN_BROWSE_OUT, &CM576CalibratorDlg::OnBnClickedBrowseOut)
	ON_BN_CLICKED(IDC_BTN_RUN_PATH, &CM576CalibratorDlg::OnBnClickedRunPath)
	ON_BN_CLICKED(IDC_BTN_GEN_BIN, &CM576CalibratorDlg::OnBnClickedGenBin)
	ON_BN_CLICKED(IDC_BTN_FLASH, &CM576CalibratorDlg::OnBnClickedFlash)
	ON_BN_CLICKED(IDC_BTN_STOP, &CM576CalibratorDlg::OnBnClickedStop)
END_MESSAGE_MAP()

BOOL CM576CalibratorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(_T("M576 / 1310 Calibrator (MCS Z4671)"));
	FillComPorts();
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);
	ZeroMemory(&m_lut, sizeof(m_lut));
	AppendLog(_T("Ready. Configure COM ports and CSV, then run."));
	return TRUE;
}

void CM576CalibratorDlg::FillComPorts()
{
	for (int i = 1; i <= 32; ++i)
	{
		CString s;
		s.Format(_T("COM%d"), i);
		m_combo429f.AddString(s);
		m_comboMcs.AddString(s);
	}
	m_combo429f.SetCurSel(0);
	m_comboMcs.SetCurSel(1);
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

CString CM576CalibratorDlg::GetComboCom(CComboBox& combo)
{
	CString s;
	int i = combo.GetCurSel();
	if (i >= 0)
		combo.GetLBText(i, s);
	return s;
}

BOOL CM576CalibratorDlg::OpenPorts()
{
	m_comm429f.ClosePort();
	m_cmdMcs.ClosePort();

	CString s429 = GetComboCom(m_combo429f);
	CString sMcs = GetComboCom(m_comboMcs);
	if (s429.IsEmpty() || sMcs.IsEmpty())
	{
		AppendLog(_T("Select COM ports."));
		return FALSE;
	}
	CString path429, pathMcs;
	path429.Format(_T("\\\\.\\%s"), (LPCTSTR)s429);
	pathMcs.Format(_T("\\\\.\\%s"), (LPCTSTR)sMcs);

	if (!m_comm429f.OpenPort((LPTSTR)(LPCTSTR)path429, 115200, 8, NOPARITY, ONESTOPBIT))
	{
		AppendLog(_T("429F COM open failed."));
		return FALSE;
	}
	if (!m_cmdMcs.OpenPort((LPTSTR)(LPCTSTR)pathMcs, 115200, 8, NOPARITY, ONESTOPBIT))
	{
		AppendLog(_T("MCS COM open failed."));
		m_comm429f.ClosePort();
		return FALSE;
	}
	m_pRecal.reset(new CRecalSession(m_comm429f));
	AppendLog(_T("Ports opened."));
	return TRUE;
}

void CM576CalibratorDlg::OnBnClickedOpenPorts()
{
	UpdateData(TRUE);
	if (OpenPorts())
		AppendLog(_T("Open ports OK."));
}

void CM576CalibratorDlg::OnBrowse(UINT idEdit)
{
	CFileDialog dlg(TRUE, _T("csv"), NULL, OFN_HIDEREADONLY, _T("CSV files (*.csv)|*.csv|All (*.*)|*.*||"), this);
	if (dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(idEdit, dlg.GetPathName());
}

void CM576CalibratorDlg::OnBnClickedBrowseCsv()
{
	OnBrowse(IDC_EDIT_CSV);
}

void CM576CalibratorDlg::OnBnClickedBrowseBackup()
{
	CFileDialog dlg(TRUE, _T("bin"), NULL, OFN_HIDEREADONLY, _T("BIN (*.bin)|*.bin|All (*.*)|*.*||"), this);
	if (dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(IDC_EDIT_BACKUP_BIN, dlg.GetPathName());
}

void CM576CalibratorDlg::OnBnClickedBrowseOut()
{
	CFileDialog dlg(FALSE, _T("bin"), NULL, OFN_OVERWRITEPROMPT, _T("BIN (*.bin)|*.bin|All (*.*)|*.*||"), this);
	if (dlg.DoModal() != IDOK)
		return;
	SetDlgItemText(IDC_EDIT_OUT_BIN, dlg.GetPathName());
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
		if (!OpenPorts())
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
	CStringA line;
	for (int i = 0; i < total; ++i)
	{
		if (m_bStop)
			break;
		SPathStep& st = steps[i];
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
	if (!m_cmdMcs.GetPortHandle() || m_cmdMcs.GetPortHandle() == INVALID_HANDLE_VALUE)
	{
		if (!OpenPorts())
			return;
	}
	CString err;
	m_progress.SetRange(0, 100);
	if (!McsFwUploadBinEx(m_cmdMcs, m_strOutBin, err, &CM576CalibratorDlg::ProgressThunk, this))
	{
		CString m;
		m.Format(_T("Flash failed: %s"), (LPCTSTR)err);
		AppendLog(m);
		return;
	}
	AppendLog(_T("Flash completed."));
}
