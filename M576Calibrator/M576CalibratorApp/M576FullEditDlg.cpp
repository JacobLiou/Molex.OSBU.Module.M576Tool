#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576FullEditDlg.h"
#include "M576CalibratorDlg.h"
#include "CalibConstants.h"
#include "FineTuneBinPatch.h"
#include "resource.h"

#include <cstdio>
#include <shellapi.h>

CM576FullEditDlg::CM576FullEditDlg(
	CM576CalibratorDlg* pOwner,
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	CWnd* pParent)
	: CDialogEx(IDD_M576_FULL_EDIT, pParent ? pParent : pOwner)
	, m_pOwner(pOwner)
	, m_outDirAbs(outDirAbs ? outDirAbs : _T(""))
	, m_snInfo(sn)
{
}

BOOL CM576FullEditDlg::Create()
{
	return CDialogEx::Create(IDD, m_pOwner);
}

void CM576FullEditDlg::OnOK()
{
}

void CM576FullEditDlg::OnCancel()
{
	DestroyWindow();
}

void CM576FullEditDlg::PostNcDestroy()
{
	if (m_pOwner != NULL)
		m_pOwner->OnFullEditDlgClosed(this);
	CDialogEx::PostNcDestroy();
	delete this;
}

void CM576FullEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FE_COMBO_ROLE, m_comboRole);
	DDX_Control(pDX, IDC_FE_EDIT_LOG, m_editLog);
	DDX_Control(pDX, IDC_FE_EDIT_IMPACT, m_editImpact);
}

BEGIN_MESSAGE_MAP(CM576FullEditDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FE_BTN_EXPORT, &CM576FullEditDlg::OnBnClickedExport)
	ON_BN_CLICKED(IDC_FE_BTN_OPEN_FOLDER, &CM576FullEditDlg::OnBnClickedOpenFolder)
	ON_BN_CLICKED(IDC_FE_BTN_WRITE, &CM576FullEditDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_FE_BTN_RESOLVE_PATH, &CM576FullEditDlg::OnBnClickedResolvePath)
	ON_BN_CLICKED(IDC_FE_BTN_EXPORT_IMPACT, &CM576FullEditDlg::OnBnClickedExportImpact)
	ON_BN_CLICKED(IDC_FE_BTN_COPY_KEYS, &CM576FullEditDlg::OnBnClickedCopyKeys)
END_MESSAGE_MAP()

void CM576FullEditDlg::AppendLocalLog(LPCTSTR line)
{
	if (line == NULL || !::IsWindow(m_editLog.GetSafeHwnd()))
		return;
	CString cur;
	m_editLog.GetWindowText(cur);
	if (!cur.IsEmpty() && cur.Right(1) != _T("\n"))
		cur += _T("\r\n");
	cur += line;
	cur += _T("\r\n");
	m_editLog.SetWindowText(cur);
	m_editLog.LineScroll(m_editLog.GetLineCount());
}

BOOL CM576FullEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_comboRole.AddString(_T("Backup"));
	m_comboRole.AddString(_T("Standard"));
	m_comboRole.SetCurSel(0);
	AppendLocalLog(_T("FullEdit Bin (R&D debug): export CSV, edit in Excel, Validate & Write. Does not Burn."));
	AppendLocalLog(_T("Does not sync session LUT. Edit any CSV row (full table)."));
	CString mapErr;
	if (!LoadMappings(mapErr))
		AppendLocalLog(mapErr);
	return TRUE;
}

M576BinFileRole CM576FullEditDlg::SelectedRole() const
{
	return (m_comboRole.GetCurSel() == 1) ? M576BinFileRole::Standard : M576BinFileRole::Backup;
}

BOOL CM576FullEditDlg::OwnerBusy() const
{
	if (m_pOwner == NULL)
		return FALSE;
	return m_pOwner->IsBackgroundBusyForIlTest();
}

BOOL CM576FullEditDlg::LoadMappings(CString& err)
{
	err.Empty();
	m_map1.RemoveAll();
	m_map2.RemoveAll();
	CString path1, path2, e1, e2;
	if (!FineTuneResolve1x64MappingPath(FineTuneDeviceKind::OneX64_1, path1, e1)
		|| !FineTuneResolve1x64MappingPath(FineTuneDeviceKind::OneX64_2, path2, e2))
	{
		err = _T("FE_PATH_CH Mapping path: ") + e1 + _T(" ") + e2;
		return FALSE;
	}
	if (!LoadPm1x64MappingCsv(path1, m_map1, e1))
	{
		err = _T("FE_PATH_CH load map1: ") + e1;
		return FALSE;
	}
	if (!LoadPm1x64MappingCsv(path2, m_map2, e2))
	{
		err = _T("FE_PATH_CH load map2: ") + e2;
		return FALSE;
	}
	CString ok;
	ok.Format(_T("Loaded Mapping rows: 1x64_1=%d 1x64_2=%d"), m_map1.GetSize(), m_map2.GetSize());
	AppendLocalLog(ok);
	return TRUE;
}

void CM576FullEditDlg::OnBnClickedExport()
{
	if (OwnerBusy())
	{
		AppendLocalLog(_T("FE_BUSY"));
		MessageBox(_T("Another operation is running."), _T("FullEdit"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString err;
	const FullEditErrorCode rc = FullEditExportAllTen(m_outDirAbs, m_snInfo, SelectedRole(), err);
	AppendLocalLog(CString(FullEditErrorCodeName(rc)) + _T(" ") + err);
	if (rc == FullEditErrorCode::Ok)
	{
		CString roleRoot;
		FullEditEnsureDirs(m_outDirAbs, SelectedRole(), roleRoot, err);
		MessageBox(
			_T("Exported 10 CSV pairs to fulledit working/ + baseline/.\n")
			_T("Edit files under working\\ with Excel, then Validate & Write."),
			_T("FullEdit"),
			MB_OK | MB_ICONINFORMATION);
	}
	else
		MessageBox(err, _T("FullEdit Export"), MB_OK | MB_ICONWARNING);
}

void CM576FullEditDlg::OnBnClickedOpenFolder()
{
	CString roleRoot, err;
	if (!FullEditEnsureDirs(m_outDirAbs, SelectedRole(), roleRoot, err))
	{
		AppendLocalLog(err);
		return;
	}
	const CString working = roleRoot + _T("\\working");
	ShellExecute(m_hWnd, _T("open"), working, NULL, NULL, SW_SHOWNORMAL);
}

void CM576FullEditDlg::OnBnClickedWrite()
{
	if (OwnerBusy())
	{
		AppendLocalLog(_T("FE_BUSY"));
		MessageBox(_T("Another operation is running."), _T("FullEdit"), MB_OK | MB_ICONWARNING);
		return;
	}
	int files = 0, rows = 0;
	CString err;
	// R&D FullEdit: allow full-table edits (no unlock UI).
	FullEditUnlockFlags unlock{};
	unlock.unlockSn33_34 = true;
	unlock.unlockMcsChGe18 = true;
	unlock.unlockMemsChGe18OrMid = true;
	const FullEditErrorCode rc = FullEditValidateAndWriteAllTen(
		m_outDirAbs, m_snInfo, SelectedRole(), unlock, files, rows, err);
	AppendLocalLog(CString(FullEditErrorCodeName(rc)) + _T(" ") + err);
	if (rc == FullEditErrorCode::Ok)
		MessageBox(err, _T("FullEdit Write"), MB_OK | MB_ICONINFORMATION);
	else if (rc == FullEditErrorCode::DiffEmpty)
		MessageBox(_T("No changes vs baseline."), _T("FullEdit"), MB_OK | MB_ICONINFORMATION);
	else
		MessageBox(err, _T("FullEdit Write"), MB_OK | MB_ICONWARNING);
}

void CM576FullEditDlg::OnBnClickedResolvePath()
{
	CString chText;
	GetDlgItemText(IDC_FE_EDIT_CH, chText);
	chText.Trim();
	const int ch = _ttoi(chText);
	std::vector<SmallRangeMapRow> map1, map2;
	for (INT_PTR i = 0; i < m_map1.GetSize(); ++i)
	{
		const SMems1x64PmMapRow& r = m_map1[i];
		SmallRangeMapRow s{};
		s.targetSwitchIndex = r.targetSwitchIndex;
		s.c1 = r.c1;
		s.c2 = r.c2;
		s.c3 = r.c3;
		s.c4 = r.c4;
		s.sw1to4 = r.sw1to4;
		s.chY1based = r.chY1based;
		map1.push_back(s);
	}
	for (INT_PTR i = 0; i < m_map2.GetSize(); ++i)
	{
		const SMems1x64PmMapRow& r = m_map2[i];
		SmallRangeMapRow s{};
		s.targetSwitchIndex = r.targetSwitchIndex;
		s.c1 = r.c1;
		s.c2 = r.c2;
		s.c3 = r.c3;
		s.c4 = r.c4;
		s.sw1to4 = r.sw1to4;
		s.chY1based = r.chY1based;
		map2.push_back(s);
	}
	PathDacImpactResult impact;
	std::string err;
	const FullEditErrorCode rc = PathDacImpactResolve(ch, map1, map2, impact, err);
	if (rc != FullEditErrorCode::Ok)
	{
		AppendLocalLog(CString(FullEditErrorCodeName(rc)) + _T(" ") + CString(err.c_str()));
		SetDlgItemText(IDC_FE_EDIT_IMPACT, CString(err.c_str()));
		return;
	}

	std::string text = PathDacImpactFormatText(impact);
	// Append current LOW DAC from selected role bins when present
	std::array<CString, M576_BURN_FILE_COUNT> paths;
	CString pathErr;
	if (M576BuildBurnFilePaths(m_outDirAbs, m_snInfo, SelectedRole(), paths, pathErr))
	{
		text += "Current LOW dac (Y,X) from bin:\n";
		for (const PathDacImpactSlot& s : impact.slots)
		{
			if (s.burnIndex < 0 || s.burnIndex >= M576_BURN_FILE_COUNT)
				continue;
			short dx = 0, dy = 0;
			CString e;
			if (FineTuneReadCurrentDac(paths[s.burnIndex], s.addr, dx, dy, e))
			{
				char line[160] = {};
				std::snprintf(line, sizeof(line), "  %s LOW Y=%d X=%d\n", s.label.c_str(), (int)dy, (int)dx);
				text += line;
			}
			else
			{
				text += "  ";
				text += s.label;
				text += " (bin DAC unread)\n";
			}
		}
	}
	SetDlgItemText(IDC_FE_EDIT_IMPACT, CString(text.c_str()));
	AppendLocalLog(_T("FE_OK path impact resolved"));
}

void CM576FullEditDlg::OnBnClickedExportImpact()
{
	CString impactText;
	m_editImpact.GetWindowText(impactText);
	if (impactText.IsEmpty())
	{
		MessageBox(_T("Resolve a CH first."), _T("FullEdit"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString chText;
	GetDlgItemText(IDC_FE_EDIT_CH, chText);
	const int ch = _ttoi(chText);
	CString roleRoot, err;
	if (!FullEditEnsureDirs(m_outDirAbs, SelectedRole(), roleRoot, err))
	{
		AppendLocalLog(err);
		return;
	}
	CString path;
	path.Format(_T("%s\\path_impact\\CH%03d_impact.txt"), roleRoot.GetString(), ch);
	CT2A utf8(impactText, CP_UTF8);
	std::string body(utf8);
	if (!FullEditWriteFileUtf8Bom(path, body, err))
	{
		AppendLocalLog(err);
		return;
	}
	AppendLocalLog(_T("Wrote ") + path);
}

void CM576FullEditDlg::OnBnClickedCopyKeys()
{
	CString impactText;
	m_editImpact.GetWindowText(impactText);
	if (impactText.IsEmpty())
		return;
	if (!OpenClipboard())
		return;
	EmptyClipboard();
	const CStringA a(impactText);
	HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)a.GetLength() + 1);
	if (h)
	{
		char* p = (char*)GlobalLock(h);
		if (p)
		{
			memcpy(p, a.GetString(), (size_t)a.GetLength() + 1);
			GlobalUnlock(h);
			SetClipboardData(CF_TEXT, h);
		}
	}
	CloseClipboard();
	AppendLocalLog(_T("Impact text copied to clipboard"));
}
