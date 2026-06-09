#pragma once

#include "CalibPathOutcomeExport.h"
#include "resource.h"

class CM576RunPathSummaryDlg : public CDialogEx
{
public:
	enum { IDD = IDD_M576_RUN_PATH_SUMMARY };

	CM576RunPathSummaryDlg(const SRunPathSummary& summary, CWnd* pParent = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	SRunPathSummary m_summary;
	CListCtrl m_listFailures;
	void PopulateList();
	void PopulateCategoryText();
	afx_msg void OnBnClickedExportFailures();
	afx_msg void OnNMDblclkFailureList(NMHDR* pNMHDR, LRESULT* pResult);
};
