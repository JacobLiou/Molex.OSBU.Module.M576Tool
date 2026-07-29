#pragma once

#include "resource.h"
#include "IlTestMath.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

class CM576CalibratorDlg;

/// One ListCtrl row for the current FIM scan lap (virtual list).
struct M576FimIlUiRow
{
	CString channel;
	CString half; // IN / OUT
	CString wl;
	int lap = 0;
	double pdDbm = 0;
	double opmDbm = 0;
	double il = 0;
	double mx = 0;
	double mn = 0;
	double span = 0;
	CString result;
};

/// FIM-aligned IL hang-up: OPLK test-table BIN + fwdl 0 Xmodem + RESULT.
/// All FIM protocol lives in M576FimIlTestDlg.cpp (not in the main dialog).
class CM576FimIlTestDlg : public CDialogEx
{
public:
	enum { IDD = IDD_M576_FIM_IL_TEST };

	explicit CM576FimIlTestDlg(CM576CalibratorDlg* pOwner, CWnd* pParent = NULL);
	~CM576FimIlTestDlg() override;

	static const int kMaxLogLines = 800;

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
	ULONGLONG m_hangupStartTick = 0;
	bool m_hangupTimerOn = false;

	std::mutex m_uiRowMutex;
	std::vector<M576FimIlUiRow> m_uiRows;
	std::vector<M576FimIlUiRow> m_pendingUiRows;
	int m_uiLap = 0;
	int m_pendingLap = 0;
	bool m_pendingNewLap = false;
	int m_channelCount = 0;
	std::atomic<bool> m_rowFlushScheduled{ false };

	IlTestWlKind SelectedWl() const;
	double ReadAbsIlMinDb() const;
	double ReadAbsIlMaxDb() const;
	double ReadSpanMaxDb() const;
	BOOL ReadGateParams(IlTestGateParams& out, CString& err) const;
	void SetControlsRunning(BOOL running);
	void AppendLogLine(LPCTSTR line);
	void BeginLapUi(int channelCount, int lap);
	void QueueLapRows(const std::vector<M576FimIlUiRow>& rows);
	void FlushPendingUiRows();
	void StartHangupClock();
	void StopHangupClock();
	void UpdateHangupClockText();
	void WorkerEntry(CString outDir, IlTestWlKind wl, IlTestGateParams gate, CString mcs1Sn);

	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUiLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiRowFlush(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiTemps(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiFinished(WPARAM wParam, LPARAM lParam);
};
