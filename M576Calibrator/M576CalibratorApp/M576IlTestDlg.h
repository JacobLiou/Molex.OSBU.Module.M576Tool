#pragma once

#include "resource.h"
#include "IlTestMath.h"
#include "DiagnosisCsv.h"

#include <atomic>
#include <thread>
#include <vector>

class CM576CalibratorDlg;

/// Hang-up IL Test: Diagnosis SW + single wavelength pd/opm loop with ListCtrl + log + CSV.
class CM576IlTestDlg : public CDialogEx
{
public:
	enum { IDD = IDD_M576_IL_TEST };

	explicit CM576IlTestDlg(CM576CalibratorDlg* pOwner, CWnd* pParent = NULL);
	~CM576IlTestDlg() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override;
	BOOL OnInitDialog() override;
	void OnCancel() override;
	void OnOK() override;
	DECLARE_MESSAGE_MAP()

private:
	CM576CalibratorDlg* m_pOwner;
	CListCtrl m_list;
	CEdit m_editLog;

	std::thread m_worker;
	std::atomic<bool> m_running{ false };
	volatile BOOL m_stop{ FALSE };

	IlTestWlKind SelectedWl() const;
	double ReadSpanMaxDb() const;
	double ReadAbsIlMinDb() const;
	double ReadAbsIlMaxDb() const;
	BOOL ReadGateParams(IlTestGateParams& out, CString& err) const;
	void SetControlsRunning(BOOL running);
	void AppendLogLine(LPCTSTR line);
	void WorkerEntry(std::vector<M576DiagnosisRow> rows, CString outDir, IlTestWlKind wl, IlTestGateParams gate);

	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnUiLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiRow(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiFinished(WPARAM wParam, LPARAM lParam);
};
