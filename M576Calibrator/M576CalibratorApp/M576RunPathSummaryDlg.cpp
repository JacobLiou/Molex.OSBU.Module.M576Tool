#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576RunPathSummaryDlg.h"
// M576RunPathSummaryDlg.cpp：Run Path 结束后展示失败/跳过汇总，支持导出 CSV 与双击详情。

#include <commctrl.h>

namespace
{
static CString Utf8ToCString(const std::string& s)
{
	if (s.empty())
		return CString();
	CStringW w;
	const int n = ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
	if (n <= 0)
		return CString(CStringA(s.c_str()));
	::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.GetBuffer(n), n);
	w.ReleaseBuffer(n);
	return CString(w);
}

static CString TrendShort(M576::SweepTrend t)
{
	switch (t)
	{
	case M576::SweepTrend::Flat: return _T("Flat");
	case M576::SweepTrend::StrictInc: return _T("Inc");
	case M576::SweepTrend::StrictDec: return _T("Dec");
	case M576::SweepTrend::NonMono: return _T("NonMono");
	default: return _T("-");
	}
}
} // namespace

CM576RunPathSummaryDlg::CM576RunPathSummaryDlg(const SRunPathSummary& summary, CWnd* pParent)
	: CDialogEx(IDD, pParent)
	, m_summary(summary)
{
}

void CM576RunPathSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RUNPATH_FAILURE_LIST, m_listFailures);
}

BOOL CM576RunPathSummaryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (CWnd* pOwner = GetParent())
	{
		CRect rcOwner;
		CRect rcDlg;
		pOwner->GetWindowRect(&rcOwner);
		GetWindowRect(&rcDlg);
		const int x = rcOwner.left + (rcOwner.Width() - rcDlg.Width()) / 2;
		const int y = rcOwner.top + (rcOwner.Height() - rcDlg.Height()) / 2;
		SetWindowPos(nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	const std::string hdr = FormatRunPathSummaryHeaderText(m_summary);
	SetDlgItemText(IDC_RUNPATH_SUMMARY_HDR, Utf8ToCString(hdr));
	PopulateCategoryText();
	PopulateList();
	const BOOL hasIssues = (m_summary.failedCount + m_summary.skippedCount) > 0;
	if (CWnd* pExport = GetDlgItem(IDC_RUNPATH_BTN_EXPORT))
		pExport->EnableWindow(hasIssues);
	if (hasIssues)
		SetWindowText(_T("Run Path - failures / skips"));
	else
		SetWindowText(_T("Run Path - all steps succeeded"));
	return TRUE;
}

void CM576RunPathSummaryDlg::PopulateCategoryText()
{
	const std::string cat = FormatRunPathCategoryBreakdownText(m_summary);
	SetDlgItemText(IDC_RUNPATH_CATEGORY_TEXT, Utf8ToCString(cat));
}

void CM576RunPathSummaryDlg::PopulateList()
{
	m_listFailures.SetExtendedStyle(
		m_listFailures.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	static const TCHAR* kCols[] = {
		_T("Trans"),
		_T("Line"),
		_T("Route"),
		_T("Category"),
		_T("Stage"),
		_T("Peak reason"),
		_T("Retry"),
		_T("baseY/X"),
		_T("offY/X"),
		_T("N Y/X"),
		_T("dBm"),
		_T("Detail"),
	};
	for (int c = 0; c < (int)(sizeof(kCols) / sizeof(kCols[0])); ++c)
		m_listFailures.InsertColumn(c, kCols[c], LVCFMT_LEFT, c == 2 ? 88 : (c == 11 ? 96 : 52));
	for (size_t i = 0; i < m_summary.failureRows.size(); ++i)
	{
		const SCalibPathStepOutcome& r = m_summary.failureRows[i];
		const int idx = m_listFailures.InsertItem((int)i, _T(""));
		CString t;
		t.Format(_T("%d"), r.fileSlot + 1);
		m_listFailures.SetItemText(idx, 0, t);
		t.Format(_T("%d"), r.pathLine1Based);
		m_listFailures.SetItemText(idx, 1, t);
		m_listFailures.SetItemText(idx, 2, Utf8ToCString(r.routeLabel));
		{
			CString catLabel(CalibPathFailCategoryLabelW(r.failCategory));
			m_listFailures.SetItemText(idx, 3, catLabel);
		}
		m_listFailures.SetItemText(idx, 4, Utf8ToCString(r.failStage));
		if (!r.peakCodeText.empty())
			m_listFailures.SetItemText(idx, 5, Utf8ToCString(r.peakCodeText));
		else
			m_listFailures.SetItemText(idx, 5, _T("-"));
		t.Format(_T("%d"), r.peakAttempts);
		m_listFailures.SetItemText(idx, 6, t);
		t.Format(_T("%d/%d"), r.lastBaseY, r.lastBaseX);
		m_listFailures.SetItemText(idx, 7, t);
		t.Format(_T("%d/%d"), r.lastOffsetY, r.lastOffsetX);
		m_listFailures.SetItemText(idx, 8, t);
		t.Format(_T("%d/%d"), r.sampleCountY, r.sampleCountX);
		m_listFailures.SetItemText(idx, 9, t);
		if (r.failCategory == CalibPathFailCategory::PmRangeMismatch)
		{
			t.Format(_T("%.3f [%.3f..%.3f]"), r.peakDbm, r.loDbm, r.hiDbm);
			m_listFailures.SetItemText(idx, 10, t);
		}
		else
			m_listFailures.SetItemText(idx, 10, _T("-"));
		CString detail = Utf8ToCString(r.commDetail);
		if (detail.IsEmpty() && r.hasTPeakY)
		{
			detail.Format(_T("tY=%.3g trendY=%s"), r.tPeakY, TrendShort(r.sweepTrendY).GetString());
		}
		m_listFailures.SetItemText(idx, 11, detail);
	}
}

void CM576RunPathSummaryDlg::OnBnClickedExportFailures()
{
	CFileDialog dlg(FALSE, _T("csv"), _T("run_path_failures.csv"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV (*.csv)|*.csv||"), this);
	if (dlg.DoModal() != IDOK)
		return;
	CString err;
	if (!WriteRunPathFailureCsv(dlg.GetPathName(), m_summary, err))
	{
		CString box;
		box.Format(_T("Export failed:\n\n%s"), err.GetString());
		MessageBox(box, _T("Run Path failures"), MB_OK | MB_ICONERROR);
		return;
	}
	CString ok;
	ok.Format(_T("Exported %d failure/skip rows to:\n%s"),
		(int)m_summary.failureRows.size(),
		dlg.GetPathName().GetString());
	MessageBox(ok, _T("Run Path failures"), MB_OK | MB_ICONINFORMATION);
}

void CM576RunPathSummaryDlg::OnNMDblclkFailureList(NMHDR* pNMHDR, LRESULT* pResult)
{
	(void)pNMHDR;
	*pResult = 0;
	const int sel = m_listFailures.GetNextItem(-1, LVNI_SELECTED);
	if (sel < 0 || sel >= (int)m_summary.failureRows.size())
		return;
	const SCalibPathStepOutcome& r = m_summary.failureRows[(size_t)sel];
	CString box;
	box.Format(
		_T("Trans %d  line %d  %s\r\n\r\n")
		_T("Category: %s\r\nStage: %s\r\nPeak: %s  attempts=%d  crossRound=%d\r\n")
		_T("baseY=%d baseX=%d  offsetY=%d offsetX=%d\r\n")
		_T("samples Y/X=%d/%d  trend Y=%s X=%s  span Y=%.4g X=%.4g\r\n")
		_T("tY=%.4g tX=%.4g\r\n")
		_T("PM dBm=%.4g range [%.4g..%.4g] pmRange=%d\r\n\r\n")
		_T("%s"),
		r.fileSlot + 1,
		r.pathLine1Based,
		Utf8ToCString(r.routeLabel).GetString(),
		CalibPathFailCategoryLabelW(r.failCategory),
		Utf8ToCString(r.failStage).GetString(),
		Utf8ToCString(r.peakCodeText).GetString(),
		r.peakAttempts,
		r.crossRound,
		r.lastBaseY,
		r.lastBaseX,
		r.lastOffsetY,
		r.lastOffsetX,
		r.sampleCountY,
		r.sampleCountX,
		TrendShort(r.sweepTrendY).GetString(),
		TrendShort(r.sweepTrendX).GetString(),
		r.spanY,
		r.spanX,
		r.hasTPeakY ? r.tPeakY : 0.0,
		r.hasTPeakX ? r.tPeakX : 0.0,
		r.peakDbm,
		r.loDbm,
		r.hiDbm,
		r.pmRangeIndex,
		Utf8ToCString(r.commDetail).GetString());
	MessageBox(box, _T("Failure detail"), MB_OK | MB_ICONINFORMATION);
}

BEGIN_MESSAGE_MAP(CM576RunPathSummaryDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RUNPATH_BTN_EXPORT, &CM576RunPathSummaryDlg::OnBnClickedExportFailures)
	ON_NOTIFY(NM_DBLCLK, IDC_RUNPATH_FAILURE_LIST, &CM576RunPathSummaryDlg::OnNMDblclkFailureList)
END_MESSAGE_MAP()
