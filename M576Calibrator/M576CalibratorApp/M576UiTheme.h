#pragma once

class CWnd;
class CProgressCtrl;

/// MFC dialog theming only (fonts, colors, dynamic layout). No business logic.
namespace M576UiTheme {

void ApplyDialog(CWnd* pDlg);
void ApplyProgress(CProgressCtrl* pProgress);
void SetupMainDynamicLayout(CWnd* pDlg);

/// Minimum tracking size in pixels (from dialog template 666x365 DLU).
void GetMainMinTrackSize(CWnd* pDlg, CSize& out);

HBRUSH GetDialogBrush();
HBRUSH OnCtlColor(CWnd* pWnd, CDC* pDC, UINT nCtlColor);

} // namespace M576UiTheme
