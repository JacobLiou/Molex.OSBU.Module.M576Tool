#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576RecoverSelectDlg.h"

namespace {
const UINT kRecoverCheckIds[M576_BURN_FILE_COUNT] = {
	IDC_RECOVER_CHECK_MCS1,
	IDC_RECOVER_CHECK_MCS2,
	IDC_RECOVER_CHECK_1X64_1_SW1,
	IDC_RECOVER_CHECK_1X64_1_SW2,
	IDC_RECOVER_CHECK_1X64_1_SW3,
	IDC_RECOVER_CHECK_1X64_1_SW4,
	IDC_RECOVER_CHECK_1X64_2_SW1,
	IDC_RECOVER_CHECK_1X64_2_SW2,
	IDC_RECOVER_CHECK_1X64_2_SW3,
	IDC_RECOVER_CHECK_1X64_2_SW4,
};

const UINT kRecoverEditIds[M576_BURN_FILE_COUNT] = {
	IDC_RECOVER_EDIT_PATH_0,
	IDC_RECOVER_EDIT_PATH_1,
	IDC_RECOVER_EDIT_PATH_2,
	IDC_RECOVER_EDIT_PATH_3,
	IDC_RECOVER_EDIT_PATH_4,
	IDC_RECOVER_EDIT_PATH_5,
	IDC_RECOVER_EDIT_PATH_6,
	IDC_RECOVER_EDIT_PATH_7,
	IDC_RECOVER_EDIT_PATH_8,
	IDC_RECOVER_EDIT_PATH_9,
};

const UINT kRecoverBrowseIds[M576_BURN_FILE_COUNT] = {
	IDC_RECOVER_BTN_BROWSE_0,
	IDC_RECOVER_BTN_BROWSE_1,
	IDC_RECOVER_BTN_BROWSE_2,
	IDC_RECOVER_BTN_BROWSE_3,
	IDC_RECOVER_BTN_BROWSE_4,
	IDC_RECOVER_BTN_BROWSE_5,
	IDC_RECOVER_BTN_BROWSE_6,
	IDC_RECOVER_BTN_BROWSE_7,
	IDC_RECOVER_BTN_BROWSE_8,
	IDC_RECOVER_BTN_BROWSE_9,
};
} // namespace

CM576RecoverSelectDlg::CM576RecoverSelectDlg(CWnd* pParent, LPCTSTR backupBasePathAbs)
	: CDialogEx(IDD_M576_RECOVER_SELECT, pParent)
	, m_backupBasePathAbs(backupBasePathAbs ? backupBasePathAbs : _T(""))
{
	m_maskHold.fill(true);
	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
		m_pathEdits[i].Empty();
}

void CM576RecoverSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
		DDX_Text(pDX, kRecoverEditIds[i], m_pathEdits[i]);
}

BOOL CM576RecoverSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
	{
		CheckDlgButton(kRecoverCheckIds[i], BST_CHECKED);
		const CString p = (i <= 1)
			? M576TransBinPathForRead(m_backupBasePathAbs, i + 1)
			: M576TransBinPathForSwitch(m_backupBasePathAbs, (i < 6) ? 3 : 4, (i < 6) ? (i - 2) : (i - 6));
		m_pathEdits[i] = p;
	}
	UpdateData(FALSE);
	return TRUE;
}

void CM576RecoverSelectDlg::OnBrowseBin(UINT nID)
{
	int idx = -1;
	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
	{
		if (kRecoverBrowseIds[i] == nID)
		{
			idx = i;
			break;
		}
	}
	if (idx < 0)
		return;

	UpdateData(TRUE);
	CFileDialog dlg(
		TRUE,
		_T("bin"),
		m_pathEdits[idx],
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		_T("BIN files (*.bin)|*.bin|All files (*.*)|*.*||"),
		this);
	if (dlg.DoModal() != IDOK)
		return;
	m_pathEdits[idx] = dlg.GetPathName();
	UpdateData(FALSE);
}

void CM576RecoverSelectDlg::OnOK()
{
	UpdateData(TRUE);
	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
		m_maskHold[i] = (IsDlgButtonChecked(kRecoverCheckIds[i]) == BST_CHECKED) != 0;
	CDialogEx::OnOK();
}

std::array<bool, M576_BURN_FILE_COUNT> CM576RecoverSelectDlg::GetMask() const
{
	return m_maskHold;
}

std::array<CString, M576_BURN_FILE_COUNT> CM576RecoverSelectDlg::GetPaths() const
{
	return m_pathEdits;
}

BEGIN_MESSAGE_MAP(CM576RecoverSelectDlg, CDialogEx)
	ON_COMMAND_RANGE(IDC_RECOVER_BTN_BROWSE_0, IDC_RECOVER_BTN_BROWSE_9, &CM576RecoverSelectDlg::OnBrowseBin)
	ON_COMMAND(IDOK, &CM576RecoverSelectDlg::OnOK)
END_MESSAGE_MAP()

