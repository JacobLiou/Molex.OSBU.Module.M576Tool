#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576BurnSelectDlg.h"
#include "resource.h"

namespace {
static const UINT kCheckIds[M576_BURN_FILE_COUNT] = {
	IDC_BURN_CHECK_MCS1,
	IDC_BURN_CHECK_MCS2,
	IDC_BURN_CHECK_1X64_1_SW1,
	IDC_BURN_CHECK_1X64_1_SW2,
	IDC_BURN_CHECK_1X64_1_SW3,
	IDC_BURN_CHECK_1X64_1_SW4,
	IDC_BURN_CHECK_1X64_2_SW1,
	IDC_BURN_CHECK_1X64_2_SW2,
	IDC_BURN_CHECK_1X64_2_SW3,
	IDC_BURN_CHECK_1X64_2_SW4,
};
} // namespace

CM576BurnSelectDlg::CM576BurnSelectDlg(CWnd* pParent, LPCTSTR basePathHint)
	: CDialogEx(IDD_M576_BURN_SELECT, pParent)
	, m_basePathHint(basePathHint ? basePathHint : _T(""))
{
	m_burnMaskHold.fill(true);
}

void CM576BurnSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CM576BurnSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (!m_basePathHint.IsEmpty())
		SetDlgItemText(IDC_STATIC_BURN_BASE_HINT, m_basePathHint);
	// Default all checkboxes on (CheckDlgButton, not CButton downcast).
	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i)
		CheckDlgButton(kCheckIds[i], BST_CHECKED);
	return TRUE;
}

void CM576BurnSelectDlg::OnOK()
{
	for (int i = 0; i < M576_BURN_FILE_COUNT; ++i) {
		if (GetDlgItem(kCheckIds[i]))
			m_burnMaskHold[i] = (IsDlgButtonChecked(kCheckIds[i]) == BST_CHECKED) != 0;
		else
			m_burnMaskHold[i] = true;
	}
	CDialogEx::OnOK();
}

std::array<bool, M576_BURN_FILE_COUNT> CM576BurnSelectDlg::GetMask() const
{
	return m_burnMaskHold;
}

BEGIN_MESSAGE_MAP(CM576BurnSelectDlg, CDialogEx)
	ON_COMMAND(IDOK, &CM576BurnSelectDlg::OnOK)
END_MESSAGE_MAP()
