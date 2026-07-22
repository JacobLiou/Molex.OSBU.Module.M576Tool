#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576FineTuneDlg.h"
#include "M576CalibratorDlg.h"
#include "resource.h"

CM576FineTuneDlg::CM576FineTuneDlg(
	CM576CalibratorDlg* pOwner,
	LPCTSTR outDirAbs,
	const M576TransSnPnInfo& sn,
	CWnd* pParent)
	: CDialogEx(IDD_M576_FINE_TUNE, pParent ? pParent : pOwner)
	, m_pOwner(pOwner)
	, m_outDirAbs(outDirAbs ? outDirAbs : _T(""))
	, m_snInfo(sn)
{
}

void CM576FineTuneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FT_COMBO_ROLE, m_comboRole);
}

BEGIN_MESSAGE_MAP(CM576FineTuneDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FT_BTN_REFRESH, &CM576FineTuneDlg::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_FT_BTN_WRITE, &CM576FineTuneDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_FT_RADIO_MCS1, &CM576FineTuneDlg::OnDeviceRadioChanged)
	ON_BN_CLICKED(IDC_FT_RADIO_MCS2, &CM576FineTuneDlg::OnDeviceRadioChanged)
	ON_BN_CLICKED(IDC_FT_RADIO_1X64_1, &CM576FineTuneDlg::OnDeviceRadioChanged)
	ON_BN_CLICKED(IDC_FT_RADIO_1X64_2, &CM576FineTuneDlg::OnDeviceRadioChanged)
	ON_CBN_SELCHANGE(IDC_FT_COMBO_ROLE, &CM576FineTuneDlg::OnRoleChanged)
	ON_EN_CHANGE(IDC_FT_EDIT_SW, &CM576FineTuneDlg::OnRoleChanged)
END_MESSAGE_MAP()

BOOL CM576FineTuneDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_comboRole.AddString(_T("Backup"));
	m_comboRole.AddString(_T("Standard"));
	m_comboRole.SetCurSel(0);
	CheckRadioButton(IDC_FT_RADIO_MCS1, IDC_FT_RADIO_1X64_2, IDC_FT_RADIO_MCS1);
	SetDlgItemInt(IDC_FT_EDIT_MCS_BLOCK, 1, FALSE);
	SetDlgItemInt(IDC_FT_EDIT_MCS_CH, 1, FALSE);
	SetDlgItemInt(IDC_FT_EDIT_SW, 1, FALSE);
	SetDlgItemInt(IDC_FT_EDIT_CHY, 1, FALSE);
	SetDlgItemText(IDC_FT_EDIT_CUR_DACX, _T(""));
	SetDlgItemText(IDC_FT_EDIT_CUR_DACY, _T(""));
	SetDlgItemText(IDC_FT_EDIT_NEW_DACX, _T("0"));
	SetDlgItemText(IDC_FT_EDIT_NEW_DACY, _T("0"));
	SyncAddressVisibility();
	CString err;
	(void)ResolveAndShowPath(err);
	return TRUE;
}

M576BinFileRole CM576FineTuneDlg::SelectedRole() const
{
	return (m_comboRole.GetCurSel() == 1) ? M576BinFileRole::Standard : M576BinFileRole::Backup;
}

FineTuneAddress CM576FineTuneDlg::CollectAddress(CString& errMsg) const
{
	FineTuneAddress addr;
	errMsg.Empty();
	if (IsDlgButtonChecked(IDC_FT_RADIO_MCS1) == BST_CHECKED)
		addr.device = FineTuneDeviceKind::Mcs1;
	else if (IsDlgButtonChecked(IDC_FT_RADIO_MCS2) == BST_CHECKED)
		addr.device = FineTuneDeviceKind::Mcs2;
	else if (IsDlgButtonChecked(IDC_FT_RADIO_1X64_1) == BST_CHECKED)
		addr.device = FineTuneDeviceKind::OneX64_1;
	else if (IsDlgButtonChecked(IDC_FT_RADIO_1X64_2) == BST_CHECKED)
		addr.device = FineTuneDeviceKind::OneX64_2;
	else
	{
		errMsg = _T("Select a device.");
		return addr;
	}

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

void CM576FineTuneDlg::OnDeviceRadioChanged()
{
	SyncAddressVisibility();
	CString err;
	(void)ResolveAndShowPath(err);
}

void CM576FineTuneDlg::OnRoleChanged()
{
	CString err;
	(void)ResolveAndShowPath(err);
}

void CM576FineTuneDlg::OnBnClickedRefresh()
{
	CString err;
	FineTuneAddress addr = CollectAddress(err);
	if (!err.IsEmpty())
	{
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString path;
	int burnIdx = -1;
	if (!FineTuneResolvePath(m_outDirAbs, m_snInfo, SelectedRole(), addr, path, burnIdx, err))
	{
		SetDlgItemText(IDC_FT_STATIC_PATH, err);
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	SetDlgItemText(IDC_FT_STATIC_PATH, path);

	short dacX = 0, dacY = 0;
	if (!FineTuneReadCurrentDac(path, addr, dacX, dacY, err))
	{
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
}

void CM576FineTuneDlg::OnBnClickedWrite()
{
	CString err;
	FineTuneAddress addr = CollectAddress(err);
	if (!err.IsEmpty())
	{
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString path;
	int burnIdx = -1;
	if (!FineTuneResolvePath(m_outDirAbs, m_snInfo, SelectedRole(), addr, path, burnIdx, err))
	{
		SetDlgItemText(IDC_FT_STATIC_PATH, err);
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
		MessageBox(_T("Enter New DAC_X and DAC_Y."), _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}
	const int dacX = _ttoi(sx);
	const int dacY = _ttoi(sy);
	if (dacX < -32768 || dacX > 32767 || dacY < -32768 || dacY > 32767)
	{
		MessageBox(_T("DAC_X / DAC_Y must be in int16 range (-32768..32767)."), _T("FineTune"), MB_OK | MB_ICONWARNING);
		return;
	}

	FineTuneSyncPayload sync{};
	if (!FineTuneWriteDac(path, addr, (short)dacX, (short)dacY, &sync, err))
	{
		MessageBox(err, _T("FineTune"), MB_OK | MB_ICONERROR);
		return;
	}

	CString curX, curY;
	curX.Format(_T("%d"), dacX);
	curY.Format(_T("%d"), dacY);
	SetDlgItemText(IDC_FT_EDIT_CUR_DACX, curX);
	SetDlgItemText(IDC_FT_EDIT_CUR_DACY, curY);

	if (m_pOwner != NULL)
		m_pOwner->OnFineTuneBinPatched(sync, path);

	MessageBox(_T("Bin updated successfully."), _T("FineTune"), MB_OK | MB_ICONINFORMATION);
}
