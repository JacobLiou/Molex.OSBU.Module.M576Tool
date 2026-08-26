#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576FineTuneDlg.h"
#include "M576CalibratorDlg.h"
#include "PathCsvDriver.h"
#include "CalibConstants.h"
#include "resource.h"

#include <uxtheme.h>
#include <vector>

BEGIN_MESSAGE_MAP(CFtTabCtrl, CTabCtrl)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CFtTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	if (pDC == NULL)
		return FALSE;
	// Chassis Debug page covers this with a DS_CONTROL dialog (Control gray).
	// FineTune hosts loose controls over the tab body; default tab erase is white.
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_BTNFACE));
	return TRUE;
}

CM576FineTuneDlg::CM576FineTuneDlg(
	CM576CalibratorDlg* pOwner,
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	CWnd* pParent)
	: CDialogEx(IDD_M576_FINE_TUNE, pParent ? pParent : pOwner)
	, m_pOwner(pOwner)
	, m_outDirAbs(outDirAbs ? outDirAbs : _T(""))
	, m_snInfo(sn)
	, m_chassisPage(pOwner)
{
}

BOOL CM576FineTuneDlg::Create()
{
	return CDialogEx::Create(IDD, m_pOwner);
}

void CM576FineTuneDlg::OnOK()
{
	// Modeless: Enter must not dismiss (Write Bin is DEFPUSHBUTTON).
}

void CM576FineTuneDlg::OnCancel()
{
	DestroyWindow();
}

void CM576FineTuneDlg::PostNcDestroy()
{
	if (m_pOwner != NULL)
		m_pOwner->OnFineTuneDlgClosed(this);
	CDialogEx::PostNcDestroy();
	delete this;
}

void CM576FineTuneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FT_TAB, m_tab);
	DDX_Control(pDX, IDC_FT_COMBO_ROLE, m_comboRole);
	DDX_Control(pDX, IDC_FT_COMBO_RECAL, m_comboRecal);
	DDX_Control(pDX, IDC_FT_COMBO_TLS, m_comboTls);
	DDX_Control(pDX, IDC_FT_COMBO_WL, m_comboWl);
	DDX_Control(pDX, IDC_FT_EDIT_LOG, m_editLog);
}

BEGIN_MESSAGE_MAP(CM576FineTuneDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FT_BTN_REFRESH, &CM576FineTuneDlg::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_FT_BTN_WRITE, &CM576FineTuneDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_FT_BTN_SMALL_RANGE, &CM576FineTuneDlg::OnBnClickedSmallRange)
	ON_BN_CLICKED(IDC_FT_BTN_READ_BEFORE, &CM576FineTuneDlg::OnBnClickedReadBefore)
	ON_BN_CLICKED(IDC_FT_BTN_READ_AFTER, &CM576FineTuneDlg::OnBnClickedReadAfter)
	ON_BN_CLICKED(IDC_FT_BTN_RDAC1, &CM576FineTuneDlg::OnBnClickedRdac1)
	ON_BN_CLICKED(IDC_FT_BTN_RDAC4, &CM576FineTuneDlg::OnBnClickedRdac4)
	ON_BN_CLICKED(IDC_FT_RADIO_MCS1, &CM576FineTuneDlg::OnDeviceRadioChanged)
	ON_BN_CLICKED(IDC_FT_RADIO_MCS2, &CM576FineTuneDlg::OnDeviceRadioChanged)
	ON_BN_CLICKED(IDC_FT_RADIO_1X64_1, &CM576FineTuneDlg::OnDeviceRadioChanged)
	ON_BN_CLICKED(IDC_FT_RADIO_1X64_2, &CM576FineTuneDlg::OnDeviceRadioChanged)
	ON_CBN_SELCHANGE(IDC_FT_COMBO_ROLE, &CM576FineTuneDlg::OnRoleChanged)
	ON_CBN_SELCHANGE(IDC_FT_COMBO_RECAL, &CM576FineTuneDlg::OnRecalSelChanged)
	ON_EN_CHANGE(IDC_FT_EDIT_MCS_BLOCK, &CM576FineTuneDlg::OnAddressFieldChanged)
	ON_EN_CHANGE(IDC_FT_EDIT_MCS_CH, &CM576FineTuneDlg::OnAddressFieldChanged)
	ON_EN_CHANGE(IDC_FT_EDIT_SW, &CM576FineTuneDlg::OnAddressFieldChanged)
	ON_EN_CHANGE(IDC_FT_EDIT_CHY, &CM576FineTuneDlg::OnAddressFieldChanged)
	ON_NOTIFY(TCN_SELCHANGE, IDC_FT_TAB, &CM576FineTuneDlg::OnTcnSelchangeTab)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CM576FineTuneDlg::AppendLocalLog(LPCTSTR line)
{
	if (line == NULL || !::IsWindow(m_editLog.GetSafeHwnd()))
		return;
	CString existing;
	m_editLog.GetWindowText(existing);
	if (!existing.IsEmpty() && existing.Right(1) != _T("\n"))
		existing += _T("\r\n");
	existing += line;
	existing += _T("\r\n");
	m_editLog.SetWindowText(existing);
	m_editLog.LineScroll(m_editLog.GetLineCount());
}

BOOL CM576FineTuneDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// Themed SysTabControl32 paints a white page body; Chassis covers it with a gray child dlg.
	// Disable theme so OnEraseBkgnd COLOR_BTNFACE matches Chassis (Control) for FineTune page.
	if (m_tab.GetSafeHwnd())
		::SetWindowTheme(m_tab.GetSafeHwnd(), L"", L"");
	m_tab.InsertItem(0, _T("FineTune"));
	m_tab.InsertItem(1, _T("Chassis Debug"));
	if (m_chassisPage.Create(IDD_M576_CHASSIS_DEBUG, this))
	{
		LayoutChassisPage();
		m_chassisPage.ShowWindow(SW_HIDE);
	}
	else
	{
		AppendLocalLog(_T("Failed to create Chassis Debug page."));
	}
	m_comboRole.AddString(_T("Backup"));
	m_comboRole.AddString(_T("Standard"));
	m_comboRole.SetCurSel(0);
	m_comboTls.ResetContent();
	for (int i = M576_MIN_TLS_SOURCE; i <= M576_MAX_TLS_SOURCE; ++i)
	{
		CString s;
		s.Format(_T("%d"), i);
		m_comboTls.AddString(s);
	}
	{
		const int idx = M576_DEFAULT_TLS_SOURCE - M576_MIN_TLS_SOURCE;
		if (idx >= 0 && idx < m_comboTls.GetCount())
			m_comboTls.SetCurSel(idx);
	}
	m_comboWl.ResetContent();
	m_comboWl.AddString(_T("1310"));
	m_comboWl.AddString(_T("1550"));
	m_comboWl.SetCurSel(0);
	CheckRadioButton(IDC_FT_RADIO_MCS1, IDC_FT_RADIO_1X64_2, IDC_FT_RADIO_MCS1);
	m_clearPmOnAddressChange = FALSE;
	SetDlgItemInt(IDC_FT_EDIT_MCS_BLOCK, 1, FALSE);
	SetDlgItemInt(IDC_FT_EDIT_MCS_CH, 1, FALSE);
	SetDlgItemInt(IDC_FT_EDIT_SW, 1, FALSE);
	SetDlgItemInt(IDC_FT_EDIT_CHY, 1, FALSE);
	m_clearPmOnAddressChange = TRUE;
	SetDlgItemText(IDC_FT_EDIT_CUR_DACX, _T(""));
	SetDlgItemText(IDC_FT_EDIT_CUR_DACY, _T(""));
	SetDlgItemText(IDC_FT_EDIT_NEW_DACX, _T("0"));
	SetDlgItemText(IDC_FT_EDIT_NEW_DACY, _T("0"));
	SyncAddressVisibility();
	RebuildRecalCombo();
	CString err;
	(void)ResolveAndShowPath(err);
	ClearPmCompare();
	RefreshStatusHint();
	AppendLocalLog(_T("FineTune ready (local log; not main window)."));
	ShowActiveTab();
	return TRUE;
}

M576BinFileRole CM576FineTuneDlg::SelectedRole() const
{
	return (m_comboRole.GetCurSel() == 1) ? M576BinFileRole::Standard : M576BinFileRole::Backup;
}

FineTuneDeviceKind CM576FineTuneDlg::SelectedDevice() const
{
	if (IsDlgButtonChecked(IDC_FT_RADIO_MCS2) == BST_CHECKED)
		return FineTuneDeviceKind::Mcs2;
	if (IsDlgButtonChecked(IDC_FT_RADIO_1X64_1) == BST_CHECKED)
		return FineTuneDeviceKind::OneX64_1;
	if (IsDlgButtonChecked(IDC_FT_RADIO_1X64_2) == BST_CHECKED)
		return FineTuneDeviceKind::OneX64_2;
	return FineTuneDeviceKind::Mcs1;
}

FineTuneAddress CM576FineTuneDlg::CollectAddress(CString& errMsg) const
{
	FineTuneAddress addr;
	errMsg.Empty();
	addr.device = SelectedDevice();

	BOOL ok = FALSE;
	addr.mcsBlock1to32 = (int)GetDlgItemInt(IDC_FT_EDIT_MCS_BLOCK, &ok, FALSE);
	if (!ok)
		addr.mcsBlock1to32 = 0;
	addr.mcsCh1to18 = (int)GetDlgItemInt(IDC_FT_EDIT_MCS_CH, &ok, FALSE);
	if (!ok)
		addr.mcsCh1to18 = 0;
	addr.sw1to4 = (int)GetDlgItemInt(IDC_FT_EDIT_SW, &ok, FALSE);
	if (!ok)
		addr.sw1to4 = 0;
	addr.chY1to17 = (int)GetDlgItemInt(IDC_FT_EDIT_CHY, &ok, FALSE);
	if (!ok)
		addr.chY1to17 = 0;
	return addr;
}

void CM576FineTuneDlg::SyncAddressVisibility()
{
	const BOOL mcs = (IsDlgButtonChecked(IDC_FT_RADIO_MCS1) == BST_CHECKED)
		|| (IsDlgButtonChecked(IDC_FT_RADIO_MCS2) == BST_CHECKED);
	const int showMcs = mcs ? SW_SHOW : SW_HIDE;
	const int show1x64 = mcs ? SW_HIDE : SW_SHOW;
	if (CWnd* p = GetDlgItem(IDC_FT_STATIC_MCS_BLOCK))
		p->ShowWindow(showMcs);
	if (CWnd* p = GetDlgItem(IDC_FT_EDIT_MCS_BLOCK))
		p->ShowWindow(showMcs);
	if (CWnd* p = GetDlgItem(IDC_FT_STATIC_MCS_CH))
		p->ShowWindow(showMcs);
	if (CWnd* p = GetDlgItem(IDC_FT_EDIT_MCS_CH))
		p->ShowWindow(showMcs);
	if (CWnd* p = GetDlgItem(IDC_FT_STATIC_SW))
		p->ShowWindow(show1x64);
	if (CWnd* p = GetDlgItem(IDC_FT_EDIT_SW))
		p->ShowWindow(show1x64);
	if (CWnd* p = GetDlgItem(IDC_FT_STATIC_CHY))
		p->ShowWindow(show1x64);
	if (CWnd* p = GetDlgItem(IDC_FT_EDIT_CHY))
		p->ShowWindow(show1x64);
}

void CM576FineTuneDlg::RebuildRecalCombo()
{
	m_rebuildingRecal = TRUE;
	m_comboRecal.ResetContent();
	m_map1x64Rows.RemoveAll();

	const FineTuneDeviceKind device = SelectedDevice();
	const BOOL isMcs = (device == FineTuneDeviceKind::Mcs1 || device == FineTuneDeviceKind::Mcs2);

	if (isMcs)
	{
		const BOOL isMcs1 = (device == FineTuneDeviceKind::Mcs1);
		for (int i = 0; i < kFineTuneMcsRecalCount; ++i)
			m_comboRecal.AddString(FineTuneFormatMcsRecalLabel(isMcs1, i));
		m_comboRecal.SetCurSel(0);
		m_rebuildingRecal = FALSE;
		ApplyRecalSelection(FALSE);
		return;
	}

	CString mapPath, err;
	if (!FineTuneResolve1x64MappingPath(device, mapPath, err)
		|| !LoadPm1x64MappingCsv(mapPath, m_map1x64Rows, err))
	{
		m_comboRecal.AddString(err.IsEmpty() ? _T("(Mapping.csv missing)") : err);
		m_comboRecal.SetCurSel(0);
		m_rebuildingRecal = FALSE;
		SetDlgItemText(IDC_FT_STATIC_PATH, err.IsEmpty() ? _T("Mapping.csv missing") : err);
		return;
	}

	const int n = (int)m_map1x64Rows.GetSize();
	for (int i = 0; i < n; ++i)
		m_comboRecal.AddString(FineTuneFormat1x64RecalLabel(m_map1x64Rows[i], i + 1, n));
	m_comboRecal.SetCurSel(0);
	m_rebuildingRecal = FALSE;
	ApplyRecalSelection(FALSE);
}

void CM576FineTuneDlg::ApplyRecalSelection(BOOL doRefresh)
{
	const int sel = m_comboRecal.GetCurSel();
	if (sel < 0)
		return;

	const BOOL prevClear = m_clearPmOnAddressChange;
	m_clearPmOnAddressChange = FALSE;

	const FineTuneDeviceKind device = SelectedDevice();
	if (device == FineTuneDeviceKind::Mcs1 || device == FineTuneDeviceKind::Mcs2)
	{
		int block = 0, ch = 0;
		if (!FineTuneMcsRecalIndexToBlockCh(sel, block, ch))
		{
			m_clearPmOnAddressChange = prevClear;
			return;
		}
		SetDlgItemInt(IDC_FT_EDIT_MCS_BLOCK, block, FALSE);
		SetDlgItemInt(IDC_FT_EDIT_MCS_CH, ch, FALSE);
	}
	else
	{
		if (sel >= m_map1x64Rows.GetSize())
		{
			m_clearPmOnAddressChange = prevClear;
			return;
		}
		const SMems1x64PmMapRow& row = m_map1x64Rows[sel];
		SetDlgItemInt(IDC_FT_EDIT_SW, row.sw1to4, FALSE);
		SetDlgItemInt(IDC_FT_EDIT_CHY, row.chY1based, FALSE);
	}

	m_clearPmOnAddressChange = prevClear;

	CString err;
	(void)ResolveAndShowPath(err);
	if (doRefresh)
		OnBnClickedRefresh();
}

BOOL CM576FineTuneDlg::ResolveAndShowPath(CString& errMsg)
{
	FineTuneAddress addr = CollectAddress(errMsg);
	if (!errMsg.IsEmpty())
	{
		SetDlgItemText(IDC_FT_STATIC_PATH, _T(""));
		return FALSE;
	}
	CString path;
	int burnIdx = -1;
	if (!FineTuneResolvePath(m_outDirAbs, m_snInfo, SelectedRole(), addr, path, burnIdx, errMsg))
	{
		SetDlgItemText(IDC_FT_STATIC_PATH, errMsg);
		return FALSE;
	}
	SetDlgItemText(IDC_FT_STATIC_PATH, path);
	return TRUE;
}

void CM576FineTuneDlg::ClearPmCompare()
{
	m_havePmBefore = FALSE;
	m_havePmAfter = FALSE;
	m_pmBeforeDbm = 0.0;
	m_pmAfterDbm = 0.0;
	UpdatePmCompareUi();
}

void CM576FineTuneDlg::UpdatePmCompareUi()
{
	if (m_havePmBefore)
	{
		CString s;
		s.Format(_T("%.4f dBm"), m_pmBeforeDbm);
		SetDlgItemText(IDC_FT_STATIC_PM_BEFORE, s);
	}
	else
	{
		SetDlgItemText(IDC_FT_STATIC_PM_BEFORE, _T("-"));
	}

	if (m_havePmAfter)
	{
		CString s;
		s.Format(_T("%.4f dBm"), m_pmAfterDbm);
		SetDlgItemText(IDC_FT_STATIC_PM_AFTER, s);
	}
	else
	{
		SetDlgItemText(IDC_FT_STATIC_PM_AFTER, _T("-"));
	}

	if (m_havePmBefore && m_havePmAfter)
	{
		CString s;
		s.Format(_T("%+.4f dB"), m_pmAfterDbm - m_pmBeforeDbm);
		SetDlgItemText(IDC_FT_STATIC_PM_DELTA, s);
	}
	else
	{
		SetDlgItemText(IDC_FT_STATIC_PM_DELTA, _T("-"));
	}
}

void CM576FineTuneDlg::BuildMapRowsForResolve(std::vector<SmallRangeMapRow>& out) const
{
	out.clear();
	const int n = (int)m_map1x64Rows.GetSize();
	out.reserve((size_t)n);
	for (int i = 0; i < n; ++i)
	{
		const SMems1x64PmMapRow& src = m_map1x64Rows[i];
		SmallRangeMapRow row{};
		row.targetSwitchIndex = src.targetSwitchIndex;
		row.c1 = src.c1;
		row.c2 = src.c2;
		row.c3 = src.c3;
		row.c4 = src.c4;
		row.sw1to4 = src.sw1to4;
		row.chY1based = src.chY1based;
		out.push_back(row);
	}
}

void CM576FineTuneDlg::SetPmBusy(BOOL busy)
{
	const BOOL en = !busy;
	if (CWnd* p = GetDlgItem(IDC_FT_BTN_READ_BEFORE))
		p->EnableWindow(en);
	if (CWnd* p = GetDlgItem(IDC_FT_BTN_READ_AFTER))
		p->EnableWindow(en);
	if (CWnd* p = GetDlgItem(IDC_FT_BTN_RDAC1))
		p->EnableWindow(en);
	if (CWnd* p = GetDlgItem(IDC_FT_BTN_RDAC4))
		p->EnableWindow(en);
	if (CWnd* p = GetDlgItem(IDC_FT_COMBO_TLS))
		p->EnableWindow(en);
	if (CWnd* p = GetDlgItem(IDC_FT_COMBO_WL))
		p->EnableWindow(en);
	if (m_chassisPage.GetSafeHwnd())
		m_chassisPage.EnableCommands(en);
}

BOOL CM576FineTuneDlg::CollectTlsAndWavelength(int& tlsSource, int& wavelengthNm, CString& errMsg) const
{
	tlsSource = 0;
	wavelengthNm = 0;
	errMsg.Empty();

	const int tlsSel = m_comboTls.GetCurSel();
	CString tlsText;
	if (tlsSel >= 0)
		m_comboTls.GetLBText(tlsSel, tlsText);
	tlsSource = _ttoi(tlsText);
	if (tlsSource < M576_MIN_TLS_SOURCE || tlsSource > M576_MAX_TLS_SOURCE)
	{
		errMsg.Format(
			_T("FineTune: select TLS %d..%d."),
			M576_MIN_TLS_SOURCE,
			M576_MAX_TLS_SOURCE);
		return FALSE;
	}

	const int wlSel = m_comboWl.GetCurSel();
	CString wlText;
	if (wlSel >= 0)
		m_comboWl.GetLBText(wlSel, wlText);
	wavelengthNm = _ttoi(wlText);
	if (wavelengthNm != 1310 && wavelengthNm != 1550)
	{
		errMsg = _T("FineTune: select wavelength 1310 or 1550.");
		return FALSE;
	}
	return TRUE;
}

void CM576FineTuneDlg::LayoutChassisPage()
{
	if (!m_chassisPage.GetSafeHwnd() || !m_tab.GetSafeHwnd())
		return;
	CRect rc;
	m_tab.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_tab.AdjustRect(FALSE, &rc);
	m_chassisPage.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
}

void CM576FineTuneDlg::ShowActiveTab()
{
	const BOOL showFt = (m_tab.GetCurSel() <= 0);
	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild != NULL)
	{
		CWnd* const pNext = pChild->GetWindow(GW_HWNDNEXT);
		const HWND h = pChild->GetSafeHwnd();
		if (h != m_tab.GetSafeHwnd() && h != m_chassisPage.GetSafeHwnd())
			pChild->ShowWindow(showFt ? SW_SHOW : SW_HIDE);
		pChild = pNext;
	}
	if (m_chassisPage.GetSafeHwnd())
		m_chassisPage.ShowWindow(showFt ? SW_HIDE : SW_SHOW);
	if (showFt && m_tab.GetSafeHwnd())
		m_tab.Invalidate(TRUE);
}

BOOL CM576FineTuneDlg::OnEraseBkgnd(CDC* pDC)
{
	if (pDC == NULL)
		return FALSE;
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_BTNFACE));
	return TRUE;
}

HBRUSH CM576FineTuneDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_DLG
		|| nCtlColor == CTLCOLOR_STATIC
		|| nCtlColor == CTLCOLOR_BTN)
	{
		pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));
		pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
		return ::GetSysColorBrush(COLOR_BTNFACE);
	}
	return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CM576FineTuneDlg::OnTcnSelchangeTab(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	ShowActiveTab();
	if (pResult != NULL)
		*pResult = 0;
}

void CM576FineTuneDlg::ReadPmSlot(BOOL isBefore)
{
	if (m_pOwner == NULL)
	{
		MessageBox(_T("Internal error: no owner dialog."), _T("FineTune"), MB_OK | MB_ICONERROR);
		return;
	}

	CString err;
	FineTuneAddress addr = CollectAddress(err);
	if (!err.IsEmpty())
	{
		AppendLocalLog(err);
		MessageBox(err, _T("FineTune PM"), MB_OK | MB_ICONWARNING);
		return;
	}

	std::vector<SmallRangeMapRow> mapRows;
	if (!FineTuneIsMcsDevice(addr.device))
		BuildMapRowsForResolve(mapRows);

	SmallRangePmStep pmStep{};
	std::string resolveErr;
	if (!SmallRangeResolvePmStepFromAddress(addr, mapRows, pmStep, resolveErr))
	{
		CString msg(resolveErr.empty() ? "resolve failed" : resolveErr.c_str());
		CString log;
		log.Format(_T("[PM] %s resolve failed: %s"),
			isBefore ? _T("Before") : _T("After"),
			msg.GetString());
		AppendLocalLog(log);
		SetDlgItemText(IDC_FT_STATIC_STATUS, msg);
		MessageBox(msg, _T("FineTune PM"), MB_OK | MB_ICONWARNING);
		return;
	}

	SPathStep step;
	step.targetSwitchIndex = pmStep.targetSwitchIndex;
	step.c1 = pmStep.c1;
	step.c2 = pmStep.c2;
	step.c3 = pmStep.c3;
	step.c4 = pmStep.c4;

	CString curX, curY, newX, newY;
	GetDlgItemText(IDC_FT_EDIT_CUR_DACX, curX);
	GetDlgItemText(IDC_FT_EDIT_CUR_DACY, curY);
	GetDlgItemText(IDC_FT_EDIT_NEW_DACX, newX);
	GetDlgItemText(IDC_FT_EDIT_NEW_DACY, newY);
	if (curX.IsEmpty())
		curX = _T("?");
	if (curY.IsEmpty())
		curY = _T("?");
	if (newX.IsEmpty())
		newX = _T("?");
	if (newY.IsEmpty())
		newY = _T("?");

	CString addrLabel;
	if (FineTuneIsMcsDevice(addr.device))
	{
		addrLabel.Format(
			_T("%hs B%d CH%d"),
			FineTuneDeviceKindNameA(addr.device),
			addr.mcsBlock1to32,
			addr.mcsCh1to18);
	}
	else
	{
		addrLabel.Format(
			_T("%hs SW%d CH_y%d"),
			FineTuneDeviceKindNameA(addr.device),
			addr.sw1to4,
			addr.chY1to17);
	}

	CString recalCmd;
	recalCmd.Format(
		_T("RECAL 1 %d %d %d %d %d"),
		step.targetSwitchIndex,
		step.c1,
		step.c2,
		step.c3,
		step.c4);

	int tlsSource = 0;
	int wavelengthNm = 0;
	if (!CollectTlsAndWavelength(tlsSource, wavelengthNm, err))
	{
		AppendLocalLog(err);
		SetDlgItemText(IDC_FT_STATIC_STATUS, err);
		MessageBox(err, _T("FineTune PM"), MB_OK | MB_ICONWARNING);
		return;
	}

	SetPmBusy(TRUE);

	CString status;
	status.Format(_T("Reading %s: %s ..."), isBefore ? _T("Before") : _T("After"), recalCmd.GetString());
	SetDlgItemText(IDC_FT_STATIC_STATUS, status);
	{
		CString startLog;
		startLog.Format(_T("[PM] %s start TLS=%d nm=%d %s %s"),
			isBefore ? _T("Before") : _T("After"),
			tlsSource,
			wavelengthNm,
			recalCmd.GetString(),
			addrLabel.GetString());
		AppendLocalLog(startLog);
	}

	double dbm = 0.0;
	int raw = 0;
	const BOOL ok = m_pOwner->FineTuneSwitchPathAndReadOpm(step, tlsSource, wavelengthNm, dbm, raw, err);

	SetPmBusy(FALSE);

	if (!ok)
	{
		CString log;
		log.Format(
			_T("[PM] %s FAIL %s %s %s"),
			isBefore ? _T("Before") : _T("After"),
			recalCmd.GetString(),
			addrLabel.GetString(),
			err.IsEmpty() ? _T("unknown error") : err.GetString());
		AppendLocalLog(log);
		SetDlgItemText(IDC_FT_STATIC_STATUS, err.IsEmpty() ? _T("PM read failed.") : err);
		MessageBox(
			err.IsEmpty() ? _T("PM read failed.") : err,
			_T("FineTune PM"),
			MB_OK | MB_ICONWARNING);
		RefreshStatusHint();
		return;
	}

	if (isBefore)
	{
		m_havePmBefore = TRUE;
		m_pmBeforeDbm = dbm;
	}
	else
	{
		m_havePmAfter = TRUE;
		m_pmAfterDbm = dbm;
	}
	UpdatePmCompareUi();

	CString logOk;
	logOk.Format(
		_T("[PM] %s %s %s OPM_raw=%d OPM=%.4f dBm curDAC=%s,%s newDAC=%s,%s"),
		isBefore ? _T("Before") : _T("After"),
		recalCmd.GetString(),
		addrLabel.GetString(),
		raw,
		dbm,
		curX.GetString(),
		curY.GetString(),
		newX.GetString(),
		newY.GetString());
	AppendLocalLog(logOk);

	if (m_havePmBefore && m_havePmAfter)
	{
		CString logDelta;
		logDelta.Format(
			_T("[PM] delta=%+.4f dB (After-Before) %s %s"),
			m_pmAfterDbm - m_pmBeforeDbm,
			recalCmd.GetString(),
			addrLabel.GetString());
		AppendLocalLog(logDelta);
	}

	status.Format(
		_T("%s OK: %.4f dBm (%s)"),
		isBefore ? _T("Before") : _T("After"),
		dbm,
		recalCmd.GetString());
	SetDlgItemText(IDC_FT_STATIC_STATUS, status);
}

void CM576FineTuneDlg::OnBnClickedReadBefore()
{
	ReadPmSlot(TRUE);
}

void CM576FineTuneDlg::OnBnClickedReadAfter()
{
	ReadPmSlot(FALSE);
}

void CM576FineTuneDlg::ReadRdacSlot(int rdacIndex)
{
	if (m_pOwner == NULL)
	{
		MessageBox(_T("Internal error: no owner dialog."), _T("FineTune"), MB_OK | MB_ICONERROR);
		return;
	}
	if (rdacIndex != 1 && rdacIndex != 4)
	{
		MessageBox(_T("Internal error: RDAC index must be 1 or 4."), _T("FineTune RDAC"), MB_OK | MB_ICONERROR);
		return;
	}

	const LPCTSTR wlTag = (rdacIndex == 1) ? _T("1550") : _T("1310");
	SetPmBusy(TRUE);
	{
		CString st;
		st.Format(_T("RDAC %d (%s): reading..."), rdacIndex, wlTag);
		SetDlgItemText(IDC_FT_STATIC_STATUS, st);
	}
	{
		CString sep;
		sep.Format(_T("--- RDAC %d (%s) ---"), rdacIndex, wlTag);
		AppendLocalLog(sep);
	}
	{
		CString tx;
		tx.Format(_T("TX: rdac %d"), rdacIndex);
		AppendLocalLog(tx);
	}

	CStringA raw;
	CString err;
	const BOOL ok = m_pOwner->FineTuneReadRdac(rdacIndex, raw, err);

	SetPmBusy(FALSE);

	if (!ok)
	{
		CString fail;
		fail.Format(_T("RDAC %d FAIL: %s"), rdacIndex, err.IsEmpty() ? _T("unknown error") : err.GetString());
		AppendLocalLog(fail);
		SetDlgItemText(IDC_FT_STATIC_STATUS, fail);
		MessageBox(fail, _T("FineTune RDAC"), MB_OK | MB_ICONWARNING);
		RefreshStatusHint();
		return;
	}

	int nRows = 0;
	int nBad = 0;
	int pos = 0;
	const CStringA text(raw);
	while (pos < text.GetLength())
	{
		int end = text.Find('\n', pos);
		if (end < 0)
			end = text.GetLength();
		CStringA line = text.Mid(pos, end - pos);
		pos = end + 1;
		line.Trim(" \t\r\n");
		if (line.IsEmpty())
			continue;

		const int colon = line.Find(':');
		if (colon <= 0)
		{
			CString warn;
			warn.Format(_T("RDAC skip: %hs"), line.GetString());
			AppendLocalLog(warn);
			++nBad;
			continue;
		}
		CStringA name = line.Left(colon);
		CStringA rest = line.Mid(colon + 1);
		name.Trim();
		rest.Trim();
		const int comma = rest.Find(',');
		if (comma < 0)
		{
			CString warn;
			warn.Format(_T("RDAC skip: %hs"), line.GetString());
			AppendLocalLog(warn);
			++nBad;
			continue;
		}
		const int dacX = atoi(rest.Left(comma));
		const int dacY = atoi(rest.Mid(comma + 1));
		CString row;
		row.Format(_T("%hs: %d, %d"), name.GetString(), dacX, dacY);
		AppendLocalLog(row);
		++nRows;
	}

	CString status;
	status.Format(
		_T("RDAC %d (%s): %d rows%s"),
		rdacIndex,
		wlTag,
		nRows,
		nBad > 0 ? _T(" (some lines skipped)") : _T(""));
	SetDlgItemText(IDC_FT_STATIC_STATUS, status);
	AppendLocalLog(status);
}

void CM576FineTuneDlg::OnBnClickedRdac1()
{
	ReadRdacSlot(1);
}

void CM576FineTuneDlg::OnBnClickedRdac4()
{
	ReadRdacSlot(4);
}

void CM576FineTuneDlg::OnDeviceRadioChanged()
{
	ClearPmCompare();
	SyncAddressVisibility();
	RebuildRecalCombo();
	CString err;
	(void)ResolveAndShowPath(err);
	AppendLocalLog(_T("Device selection changed."));
}

void CM576FineTuneDlg::OnRoleChanged()
{
	CString err;
	(void)ResolveAndShowPath(err);
	AppendLocalLog(_T("Bin role changed."));
}

void CM576FineTuneDlg::OnRecalSelChanged()
{
	if (m_rebuildingRecal)
		return;
	ClearPmCompare();
	ApplyRecalSelection(TRUE);
}

void CM576FineTuneDlg::OnAddressFieldChanged()
{
	if (!m_clearPmOnAddressChange)
		return;
	ClearPmCompare();
	CString err;
	(void)ResolveAndShowPath(err);
}

void CM576FineTuneDlg::OnBnClickedRefresh()
{
	AppendLocalLog(_T("Refresh: read current DAC from bin..."));
	CString err;
	FineTuneAddress addr = CollectAddress(err);
	if (!err.IsEmpty())
	{
		AppendLocalLog(err);
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString path;
	int burnIdx = -1;
	if (!FineTuneResolvePath(m_outDirAbs, m_snInfo, SelectedRole(), addr, path, burnIdx, err))
	{
		SetDlgItemText(IDC_FT_STATIC_PATH, err);
		AppendLocalLog(err);
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	SetDlgItemText(IDC_FT_STATIC_PATH, path);

	short dacX = 0, dacY = 0;
	if (!FineTuneReadCurrentDac(path, addr, dacX, dacY, err))
	{
		AppendLocalLog(err);
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString sx, sy;
	sx.Format(_T("%d"), (int)dacX);
	sy.Format(_T("%d"), (int)dacY);
	SetDlgItemText(IDC_FT_EDIT_CUR_DACX, sx);
	SetDlgItemText(IDC_FT_EDIT_CUR_DACY, sy);
	SetDlgItemText(IDC_FT_EDIT_NEW_DACX, sx);
	SetDlgItemText(IDC_FT_EDIT_NEW_DACY, sy);
	CString ok;
	ok.Format(_T("Refresh OK %s DAC=%d,%d"), FineTuneAddressFormatLabel(addr).GetString(), (int)dacX, (int)dacY);
	AppendLocalLog(ok);
}

void CM576FineTuneDlg::OnBnClickedWrite()
{
	AppendLocalLog(_T("Write Bin..."));
	CString err;
	FineTuneAddress addr = CollectAddress(err);
	if (!err.IsEmpty())
	{
		AppendLocalLog(err);
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString path;
	int burnIdx = -1;
	if (!FineTuneResolvePath(m_outDirAbs, m_snInfo, SelectedRole(), addr, path, burnIdx, err))
	{
		SetDlgItemText(IDC_FT_STATIC_PATH, err);
		AppendLocalLog(err);
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	SetDlgItemText(IDC_FT_STATIC_PATH, path);

	CString sx, sy;
	GetDlgItemText(IDC_FT_EDIT_NEW_DACX, sx);
	GetDlgItemText(IDC_FT_EDIT_NEW_DACY, sy);
	sx.Trim();
	sy.Trim();
	if (sx.IsEmpty() || sy.IsEmpty())
	{
		AppendLocalLog(_T("Write Bin: enter New DAC_X and DAC_Y."));
		MessageBox(_T("Enter New DAC_X and DAC_Y."), _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	const int dacX = _ttoi(sx);
	const int dacY = _ttoi(sy);
	if (dacX < -32768 || dacX > 32767 || dacY < -32768 || dacY > 32767)
	{
		AppendLocalLog(_T("Write Bin: DAC out of int16 range."));
		MessageBox(_T("DAC_X / DAC_Y must be in int16 range (-32768..32767)."), _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}

	FineTuneSyncPayload sync{};
	if (!FineTuneWriteDac(path, addr, (short)dacX, (short)dacY, &sync, err))
	{
		AppendLocalLog(err);
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONERROR);
		return;
	}

	CString curX, curY;
	curX.Format(_T("%d"), dacX);
	curY.Format(_T("%d"), dacY);
	SetDlgItemText(IDC_FT_EDIT_CUR_DACX, curX);
	SetDlgItemText(IDC_FT_EDIT_CUR_DACY, curY);

	if (m_pOwner != NULL)
		m_pOwner->OnFineTuneBinPatched(sync, path, addr, SelectedRole());

	RefreshStatusHint();
	AppendLocalLog(_T("Write Bin OK."));
	MessageBox(_T("Bin updated successfully."), _T("FineTune"), MB_OK | MB_ICONINFORMATION);
}

void CM576FineTuneDlg::RefreshStatusHint()
{
	const int n = (m_pOwner != NULL) ? m_pOwner->GetFineTuneChannelCount() : 0;
	CString line;
	line.Format(
		_T("Recorded %d FineTune channel(s). Read Before/After: RECAL 1 + OPM. After Write Bin, Burn Flash on main window; then Small Range if needed."),
		n);
	SetDlgItemText(IDC_FT_STATIC_STATUS, line);
}

void CM576FineTuneDlg::OnBnClickedSmallRange()
{
	if (m_pOwner == NULL)
	{
		MessageBox(_T("Internal error: no owner dialog."), _T("FineTune"), MB_OK | MB_ICONERROR);
		return;
	}
	const int n = m_pOwner->GetFineTuneChannelCount();
	if (n <= 0)
	{
		AppendLocalLog(_T("Small Range: no recorded channels."));
		MessageBox(
			_T("No FineTune channels recorded. Write Bin for at least one channel first, then run Small Range."),
			_T("Small Range"),
			MB_OK | MB_ICONWARNING);
		return;
	}

	CString prompt;
	prompt.Format(
		_T("Run Small Range calibration (PM Run Path) for the following %d recorded channel(s).\n\n")
		_T("%s\n\n")
		_T("Confirm the matching Bin has been downloaded to device Flash via Burn Flash or Recover Flash.\n\n")
		_T("Continue?"),
		n,
		m_pOwner->FormatFineTuneChannelsListForPrompt().GetString());
	if (MessageBox(prompt, _T("Small Range"), MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		AppendLocalLog(_T("Small Range: cancelled."));
		return;
	}

	AppendLocalLog(_T("Small Range: starting on main window..."));
	CString err;
	if (!m_pOwner->StartSmallRangePmRunPath(err))
	{
		CString fail = err.IsEmpty() ? _T("Small Range failed to start.") : err;
		AppendLocalLog(fail);
		MessageBox(fail, _T("Small Range"), MB_OK | MB_ICONWARNING);
		return;
	}
	AppendLocalLog(_T("Small Range started (FineTune stays open)."));
	SetDlgItemText(
		IDC_FT_STATIC_STATUS,
		_T("Small Range started on main window (this FineTune dialog stays open)."));
}
