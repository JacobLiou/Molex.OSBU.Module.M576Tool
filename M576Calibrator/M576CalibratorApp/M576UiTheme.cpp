#include "stdafx.h"
#include "M576UiTheme.h"
#include "resource.h"

namespace {
CFont g_logFont;
bool g_logFontOk = false;

BOOL CALLBACK ApplyFontEnumProc(HWND hwnd, LPARAM lParam)
{
	const HFONT hUiFont = reinterpret_cast<HFONT>(lParam);
	if (!hUiFont)
		return TRUE;
	const int id = ::GetDlgCtrlID(hwnd);
	HFONT hFont = hUiFont;
	if (id == IDC_EDIT_LOG && g_logFontOk)
		hFont = static_cast<HFONT>(static_cast<HGDIOBJ>(g_logFont.GetSafeHandle()));
	if (hFont)
		::SendMessage(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
	if (id == IDC_EDIT_LOG)
		::SendMessage(hwnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(8, 8));
	return TRUE;
}

} // namespace

namespace M576UiTheme {

static CBrush g_brDialog;
static CBrush g_brLog;
static bool g_brushesCreated = false;

void ApplyDialog(CWnd* pDlg)
{
	if (!pDlg || !pDlg->GetSafeHwnd())
		return;

	CFont* pUiFont = pDlg->GetFont();
	if (!pUiFont)
		return;

	if (!g_logFontOk)
	{
		// 9pt Consolas for log; UI uses dialog template font (10pt Segoe UI).
		if (CDC* pDC = pDlg->GetDC())
		{
			g_logFontOk = g_logFont.CreatePointFont(90, _T("Consolas"), pDC);
			pDlg->ReleaseDC(pDC);
		}
	}

	HFONT hUi = static_cast<HFONT>(static_cast<HGDIOBJ>(pUiFont->GetSafeHandle()));
	::EnumChildWindows(pDlg->GetSafeHwnd(), ApplyFontEnumProc, reinterpret_cast<LPARAM>(hUi));
}

void ApplyProgress(CProgressCtrl* pProgress)
{
	if (!pProgress || !pProgress->GetSafeHwnd())
		return;
	pProgress->SendMessage(PBM_SETBARCOLOR, 0, static_cast<LPARAM>(RGB(37, 99, 235)));
}

void SetupMainDynamicLayout(CWnd* pDlg)
{
	if (!pDlg || !pDlg->GetSafeHwnd())
		return;

	// Only stretch the log panel and progress bar; left column stays fixed in the RC layout.
	pDlg->EnableDynamicLayout(TRUE);
	CMFCDynamicLayout* pLayout = pDlg->GetDynamicLayout();
	if (!pLayout || !pLayout->Create(pDlg))
		return;

	auto addItemIfPresent = [pDlg, pLayout](UINT id, CMFCDynamicLayout::MoveSettings move,
		CMFCDynamicLayout::SizeSettings size) {
		if (CWnd* pChild = pDlg->GetDlgItem(id))
		{
			if (HWND hwnd = pChild->GetSafeHwnd())
				pLayout->AddItem(hwnd, move, size);
		}
	};

	addItemIfPresent(
		IDC_PROGRESS_MAIN,
		CMFCDynamicLayout::MoveNone(),
		CMFCDynamicLayout::SizeHorizontal(100));

	addItemIfPresent(
		IDC_GROUP_LOG,
		CMFCDynamicLayout::MoveHorizontal(100),
		CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
	addItemIfPresent(
		IDC_EDIT_LOG,
		CMFCDynamicLayout::MoveHorizontal(100),
		CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
	addItemIfPresent(
		IDC_BTN_CLEAR_LOG,
		CMFCDynamicLayout::MoveHorizontal(100),
		CMFCDynamicLayout::SizeNone());
}

void GetMainMinTrackSize(CWnd* pDlg, CSize& out)
{
	CRect rcDlg(0, 0, 666, 365);
	if (CDialog* pDialog = DYNAMIC_DOWNCAST(CDialog, pDlg))
		pDialog->MapDialogRect(&rcDlg);
	out = rcDlg.Size();
}

HBRUSH GetDialogBrush()
{
	if (!g_brushesCreated)
	{
		g_brDialog.CreateSolidBrush(RGB(245, 246, 248));
		g_brLog.CreateSolidBrush(RGB(250, 250, 250));
		g_brushesCreated = true;
	}
	return g_brDialog;
}

HBRUSH OnCtlColor(CWnd* pWnd, CDC* pDC, UINT nCtlColor)
{
	if (!g_brushesCreated)
	{
		g_brDialog.CreateSolidBrush(RGB(245, 246, 248));
		g_brLog.CreateSolidBrush(RGB(250, 250, 250));
		g_brushesCreated = true;
	}
	if (!pDC)
		return g_brDialog;

	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
		pDC->SetBkColor(RGB(245, 246, 248));
		return g_brDialog;
	case CTLCOLOR_STATIC:
	{
		pDC->SetBkColor(RGB(245, 246, 248));
		const int sid = pWnd ? pWnd->GetDlgCtrlID() : 0;
		if (sid == IDC_GROUP_BK_READ_SN || sid == IDC_GROUP_BK_READ_BIN)
			pDC->SetTextColor(RGB(55, 65, 81));
		else
			pDC->SetTextColor(RGB(107, 114, 128));
		return g_brDialog;
	}
	case CTLCOLOR_BTN:
	{
		const int id = pWnd ? pWnd->GetDlgCtrlID() : 0;
		if (id == IDC_GROUP_CONN || id == IDC_GROUP_DIAG || id == IDC_GROUP_BACKUP || id == IDC_GROUP_PATHS
			|| id == IDC_GROUP_ACTIONS || id == IDC_GROUP_LOG || id == IDC_GROUP_BK_READ_SN
			|| id == IDC_GROUP_BK_READ_BIN)
		{
			pDC->SetBkColor(RGB(245, 246, 248));
			pDC->SetTextColor(RGB(55, 65, 81));
			return g_brDialog;
		}
		break;
	}
	case CTLCOLOR_EDIT:
		if (pWnd && pWnd->GetDlgCtrlID() == IDC_EDIT_LOG)
		{
			pDC->SetBkColor(RGB(250, 250, 250));
			pDC->SetTextColor(RGB(17, 24, 39));
			return g_brLog;
		}
		pDC->SetBkColor(RGB(255, 255, 255));
		return reinterpret_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	default:
		break;
	}
	return g_brDialog;
}

} // namespace M576UiTheme
