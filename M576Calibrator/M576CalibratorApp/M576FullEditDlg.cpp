#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576FullEditDlg.h"
#include "M576CalibratorDlg.h"
#include "CalibConstants.h"
#include "FineTuneBinPatch.h"
#include "IlTestMath.h"
#include "resource.h"

#include <array>
#include <cstdio>
#include <sstream>
#include <shellapi.h>

namespace {

static const int kFeCardIds[6] = {
	IDC_FE_CARD1, IDC_FE_CARD2, IDC_FE_CARD3,
	IDC_FE_CARD4, IDC_FE_CARD5, IDC_FE_CARD6
};

} // namespace

CM576FullEditDlg::CM576FullEditDlg(
	CM576CalibratorDlg* pOwner,
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	CWnd* pParent)
	: CDialogEx(IDD_M576_FULL_EDIT, pParent ? pParent : pOwner)
	, m_pOwner(pOwner)
	, m_outDirAbs(outDirAbs ? outDirAbs : _T(""))
	, m_snInfo(sn)
	, m_highlightStage(-1)
{
	for (int i = 0; i < 6; ++i)
	{
		m_slotLowY[i] = 0;
		m_slotLowX[i] = 0;
		m_slotRoomY[i] = 0;
		m_slotRoomX[i] = 0;
		m_slotHighY[i] = 0;
		m_slotHighX[i] = 0;
		m_slotDacOk[i] = FALSE;
	}
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
	DDX_Control(pDX, IDC_FE_COMBO_CH, m_comboCh);
	DDX_Control(pDX, IDC_FE_EDIT_LOG, m_editLog);
	DDX_Control(pDX, IDC_FE_EDIT_SUMMARY, m_editSummary);
	DDX_Control(pDX, IDC_FE_LIST_IMPACT, m_listImpact);
}

BEGIN_MESSAGE_MAP(CM576FullEditDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FE_BTN_EXPORT, &CM576FullEditDlg::OnBnClickedExport)
	ON_BN_CLICKED(IDC_FE_BTN_OPEN_FOLDER, &CM576FullEditDlg::OnBnClickedOpenFolder)
	ON_BN_CLICKED(IDC_FE_BTN_WRITE, &CM576FullEditDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_FE_BTN_RESOLVE_PATH, &CM576FullEditDlg::OnBnClickedResolvePath)
	ON_BN_CLICKED(IDC_FE_BTN_EXPORT_IMPACT, &CM576FullEditDlg::OnBnClickedExportImpact)
	ON_BN_CLICKED(IDC_FE_BTN_COPY_KEYS, &CM576FullEditDlg::OnBnClickedCopyKeys)
	ON_CBN_SELCHANGE(IDC_FE_COMBO_CH, &CM576FullEditDlg::OnCbnSelchangeCh)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FE_LIST_IMPACT, &CM576FullEditDlg::OnLvnItemchangedImpact)
END_MESSAGE_MAP()

CString CM576FullEditDlg::Utf8ToCString(const std::string& s)
{
	// Path Impact strings are ASCII English; avoid CP_UTF8 round-trip mojibake on MBCS builds.
	if (s.empty())
		return CString();
	return CString(s.c_str());
}

UINT CM576FullEditDlg::CardId(int stage0to5)
{
	if (stage0to5 < 0 || stage0to5 >= 6)
		return IDC_FE_CARD1;
	return (UINT)kFeCardIds[stage0to5];
}

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

void CM576FullEditDlg::InitImpactListColumns()
{
	m_listImpact.SetExtendedStyle(
		m_listImpact.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_listImpact.InsertColumn(0, _T("Stage"), LVCFMT_LEFT, 42);
	m_listImpact.InsertColumn(1, _T("Role"), LVCFMT_LEFT, 120);
	m_listImpact.InsertColumn(2, _T("Device"), LVCFMT_LEFT, 88);
	m_listImpact.InsertColumn(3, _T("CSV/SN"), LVCFMT_LEFT, 96);
	m_listImpact.InsertColumn(4, _T("Slot"), LVCFMT_LEFT, 96);
	m_listImpact.InsertColumn(5, _T("LOW Y"), LVCFMT_RIGHT, 52);
	m_listImpact.InsertColumn(6, _T("LOW X"), LVCFMT_RIGHT, 52);
	m_listImpact.InsertColumn(7, _T("ROOM Y"), LVCFMT_RIGHT, 56);
	m_listImpact.InsertColumn(8, _T("ROOM X"), LVCFMT_RIGHT, 56);
	m_listImpact.InsertColumn(9, _T("HIGH Y"), LVCFMT_RIGHT, 56);
	m_listImpact.InsertColumn(10, _T("HIGH X"), LVCFMT_RIGHT, 56);
}

void CM576FullEditDlg::InitChannelCombo()
{
	m_comboCh.ResetContent();
	for (int ch = 1; ch <= 576; ++ch)
	{
		CString mpo;
		if (!IlTestChannelToMpoPath(ch, mpo))
			mpo = _T("?");
		CString item;
		item.Format(_T("CH%03d  %s"), ch, mpo.GetString());
		m_comboCh.AddString(item);
	}
	m_comboCh.SetCurSel(0);
}

BOOL CM576FullEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_comboRole.AddString(_T("Backup"));
	m_comboRole.AddString(_T("Standard"));
	m_comboRole.SetCurSel(0);

	LOGFONT lf = {};
	CFont* pFont = GetFont();
	if (pFont != NULL)
		pFont->GetLogFont(&lf);
	else
	{
		lf.lfHeight = -12;
		_tcscpy_s(lf.lfFaceName, _T("Segoe UI"));
	}
	m_fontCardNormal.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_fontCardBold.CreateFontIndirect(&lf);
	for (int i = 0; i < 6; ++i)
	{
		if (CWnd* p = GetDlgItem(CardId(i)))
			p->SetFont(&m_fontCardNormal);
	}

	InitImpactListColumns();
	InitChannelCombo();
	ClearImpactUi();

	AppendLocalLog(_T("FullEdit Bin (R&D debug): export CSV, edit in Excel, Validate & Write. Does not Burn."));
	AppendLocalLog(_T("Path Impact: pick CH (MPO) -> six stages auto-refresh. Refresh DAC reloads LOW/ROOM/HIGH Y/X from bins."));
	CString mapErr;
	if (!LoadMappings(mapErr))
		AppendLocalLog(mapErr);
	else
		RefreshPathImpact();
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

int CM576FullEditDlg::SelectedChannel() const
{
	const int sel = m_comboCh.GetCurSel();
	if (sel < 0)
		return 0;
	return sel + 1;
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

void CM576FullEditDlg::ClearImpactUi()
{
	m_lastImpact = PathDacImpactResult{};
	m_lastImpactText.Empty();
	m_highlightStage = -1;
	m_editSummary.SetWindowText(_T(""));
	m_listImpact.DeleteAllItems();
	for (int i = 0; i < 6; ++i)
	{
		m_slotLowY[i] = 0;
		m_slotLowX[i] = 0;
		m_slotRoomY[i] = 0;
		m_slotRoomX[i] = 0;
		m_slotHighY[i] = 0;
		m_slotHighX[i] = 0;
		m_slotDacOk[i] = FALSE;
		if (CWnd* p = GetDlgItem(CardId(i)))
		{
			p->SetWindowText(_T(""));
			p->SetFont(&m_fontCardNormal);
		}
	}
}

void CM576FullEditDlg::HighlightCard(int stage0to5)
{
	m_highlightStage = stage0to5;
	for (int i = 0; i < 6; ++i)
	{
		CWnd* p = GetDlgItem(CardId(i));
		if (p == NULL)
			continue;
		p->SetFont((i == stage0to5) ? &m_fontCardBold : &m_fontCardNormal);
		p->Invalidate();
	}
}

void CM576FullEditDlg::ApplyImpactToUi(const PathDacImpactResult& impact)
{
	m_lastImpact = impact;
	m_editSummary.SetWindowText(Utf8ToCString(PathDacImpactFormatSummaryZh(impact)));

	std::array<CString, M576_BURN_FILE_COUNT> paths;
	CString pathErr;
	const BOOL haveBins = M576BuildBurnFilePaths(m_outDirAbs, m_snInfo, SelectedRole(), paths, pathErr);

	m_listImpact.DeleteAllItems();
	std::ostringstream exportBody;
	exportBody << PathDacImpactFormatSummaryZh(impact) << "\n";
	exportBody << "stage\trole\tdevice\tcsv_sn\tslot\tLOW_Y\tLOW_X\tROOM_Y\tROOM_X\tHIGH_Y\tHIGH_X\tkey\n";

	for (size_t i = 0; i < impact.slots.size() && i < 6; ++i)
	{
		const PathDacImpactSlot& s = impact.slots[i];
		m_slotDacOk[i] = FALSE;
		m_slotLowY[i] = m_slotLowX[i] = 0;
		m_slotRoomY[i] = m_slotRoomX[i] = 0;
		m_slotHighY[i] = m_slotHighX[i] = 0;
		if (haveBins && s.burnIndex >= 0 && s.burnIndex < M576_BURN_FILE_COUNT)
		{
			short lx = 0, ly = 0, rx = 0, ry = 0, hx = 0, hy = 0;
			CString e;
			if (FineTuneReadTripleDac(paths[s.burnIndex], s.addr, lx, ly, rx, ry, hx, hy, e))
			{
				m_slotLowY[i] = ly;
				m_slotLowX[i] = lx;
				m_slotRoomY[i] = ry;
				m_slotRoomX[i] = rx;
				m_slotHighY[i] = hy;
				m_slotHighX[i] = hx;
				m_slotDacOk[i] = TRUE;
			}
		}

		CString snHint = Utf8ToCString(s.csvFileHint);
		CString snOne, snErr;
		if (s.burnIndex >= 0 && M576SnForBurnFileIndex(m_snInfo, s.burnIndex, snOne, snErr) && !snOne.IsEmpty())
			snHint.Format(_T("%s_dac.csv"), snOne.GetString());

		CString slotDesc;
		if (FineTuneIsMcsDevice(s.addr.device))
		{
			slotDesc.Format(_T("block=%d ch=%d"), s.addr.mcsBlock1to32, s.addr.mcsCh1to18);
		}
		else
		{
			slotDesc.Format(_T("SW%d CH_y=%d"), s.memsSw1to4, s.memsChY1to17);
		}

		CString stage;
		stage.Format(_T("%d"), (int)i + 1);
		const int row = m_listImpact.InsertItem((int)i, stage);
		m_listImpact.SetItemText(row, 1, Utf8ToCString(s.roleZh));
		m_listImpact.SetItemText(row, 2, Utf8ToCString(s.label));
		m_listImpact.SetItemText(row, 3, snHint);
		m_listImpact.SetItemText(row, 4, slotDesc);

		auto setDacCols = [&](BOOL ok) {
			if (!ok)
			{
				for (int c = 5; c <= 10; ++c)
					m_listImpact.SetItemText(row, c, _T("-"));
				return;
			}
			CString t;
			t.Format(_T("%d"), (int)m_slotLowY[i]); m_listImpact.SetItemText(row, 5, t);
			t.Format(_T("%d"), (int)m_slotLowX[i]); m_listImpact.SetItemText(row, 6, t);
			t.Format(_T("%d"), (int)m_slotRoomY[i]); m_listImpact.SetItemText(row, 7, t);
			t.Format(_T("%d"), (int)m_slotRoomX[i]); m_listImpact.SetItemText(row, 8, t);
			t.Format(_T("%d"), (int)m_slotHighY[i]); m_listImpact.SetItemText(row, 9, t);
			t.Format(_T("%d"), (int)m_slotHighX[i]); m_listImpact.SetItemText(row, 10, t);
		};
		setDacCols(m_slotDacOk[i]);

		CStringA snA(CT2A(snHint, CP_UTF8));
		CStringA slotA(CT2A(slotDesc, CP_UTF8));
		exportBody << (int)i + 1 << "\t" << s.roleZh << "\t" << s.label
			<< "\t" << snA.GetString() << "\t" << slotA.GetString() << "\t";
		if (m_slotDacOk[i])
		{
			exportBody << (int)m_slotLowY[i] << "\t" << (int)m_slotLowX[i]
				<< "\t" << (int)m_slotRoomY[i] << "\t" << (int)m_slotRoomX[i]
				<< "\t" << (int)m_slotHighY[i] << "\t" << (int)m_slotHighX[i];
		}
		else
			exportBody << "-\t-\t-\t-\t-\t-";
		exportBody << "\t" << s.csvRowKey << "\n";

		CString card;
		if (m_slotDacOk[i])
		{
			card.Format(_T("%s\r\n%s\r\nL:%d,%d R:%d,%d H:%d,%d"),
				Utf8ToCString(s.roleZh).GetString(),
				slotDesc.GetString(),
				(int)m_slotLowY[i], (int)m_slotLowX[i],
				(int)m_slotRoomY[i], (int)m_slotRoomX[i],
				(int)m_slotHighY[i], (int)m_slotHighX[i]);
		}
		else
		{
			card.Format(_T("%s\r\n%s\r\nL/R/H=-"),
				Utf8ToCString(s.roleZh).GetString(),
				slotDesc.GetString());
		}
		if (CWnd* p = GetDlgItem(CardId((int)i)))
			p->SetWindowText(card);
	}

	m_lastImpactText = Utf8ToCString(exportBody.str());
	if (m_listImpact.GetItemCount() > 0)
	{
		m_listImpact.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		HighlightCard(0);
	}
	else
		HighlightCard(-1);
}

void CM576FullEditDlg::RefreshPathImpact()
{
	const int ch = SelectedChannel();
	if (ch < 1 || ch > 576)
	{
		ClearImpactUi();
		return;
	}
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
		ClearImpactUi();
		m_editSummary.SetWindowText(CString(err.c_str()));
		AppendLocalLog(CString(FullEditErrorCodeName(rc)) + _T(" ") + CString(err.c_str()));
		return;
	}
	ApplyImpactToUi(impact);
	AppendLocalLog(_T("FE_OK path impact six-stage refreshed"));
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
	RefreshPathImpact();
}

void CM576FullEditDlg::OnCbnSelchangeCh()
{
	RefreshPathImpact();
}

void CM576FullEditDlg::OnLvnItemchangedImpact(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	if ((pNMLV->uChanged & LVIF_STATE) == 0)
		return;
	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return;
	if (pNMLV->iItem >= 0 && pNMLV->iItem < 6)
		HighlightCard(pNMLV->iItem);
}

void CM576FullEditDlg::OnBnClickedExportImpact()
{
	if (m_lastImpactText.IsEmpty() || m_lastImpact.slots.size() != 6)
	{
		MessageBox(_T("Select a CH first."), _T("FullEdit"), MB_OK | MB_ICONWARNING);
		return;
	}
	const int ch = m_lastImpact.ch1to576;
	CString roleRoot, err;
	if (!FullEditEnsureDirs(m_outDirAbs, SelectedRole(), roleRoot, err))
	{
		AppendLocalLog(err);
		return;
	}
	CString path;
	path.Format(_T("%s\\path_impact\\CH%03d_impact.txt"), roleRoot.GetString(), ch);
	CT2A utf8(m_lastImpactText, CP_UTF8);
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
	if (m_lastImpactText.IsEmpty())
		return;
	if (!OpenClipboard())
		return;
	EmptyClipboard();
	const int cch = m_lastImpactText.GetLength();
	HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)(cch + 1) * sizeof(TCHAR));
	if (h)
	{
		LPTSTR p = (LPTSTR)GlobalLock(h);
		if (p)
		{
			memcpy(p, m_lastImpactText.GetString(), (size_t)(cch + 1) * sizeof(TCHAR));
			GlobalUnlock(h);
#ifdef _UNICODE
			SetClipboardData(CF_UNICODETEXT, h);
#else
			SetClipboardData(CF_TEXT, h);
#endif
		}
	}
	CloseClipboard();
	AppendLocalLog(_T("Impact text copied to clipboard"));
}
