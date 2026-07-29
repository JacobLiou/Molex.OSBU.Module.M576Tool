#pragma once

#include "resource.h"
#include "IlTestMath.h"
#include "DiagnosisCsv.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

class CM576CalibratorDlg;

/// One ListCtrl row for the current lap only (virtual list).
struct M576IlTestUiRow
{
	CString channel;
	CString half; // IN / OUT
	CString mpoPath;
	CString wl;
	int lap = 0;
	int pd = 0;
	int opm = 0;
	double il = 0;
	double mx = 0;
	double mn = 0;
	double span = 0;
	CString result;
};

/// Hang-up IL Test: ListCtrl shows latest lap (~576 ch); Log shows CommLog + IL summary.
/// Comm SEND/RECV -> `output\ILTestCommLog_YYYY-MM-DD.log` and this dialog Log (not main UI).
class CM576IlTestDlg : public CDialogEx
{
public:
	enum { IDD = IDD_M576_IL_TEST };

	explicit CM576IlTestDlg(CM576CalibratorDlg* pOwner, CWnd* pParent = NULL);
	~CM576IlTestDlg() override;

	/// Keep last N lines in Log edit (full history is in ILTestCommLog_*.log).
	static const int kMaxLogLines = 800;

	/// Thread-safe: enqueue one CommLog / summary line into the dialog Log edit.
	void PostCommLogLine(LPCTSTR line);

protected:
	void DoDataExchange(CDataExchange* pDX) override;
	BOOL OnInitDialog() override;
	void OnCancel() override;
	void OnOK() override;
	DECLARE_MESSAGE_MAP()

private:
	CM576CalibratorDlg* m_pOwner;
	CListCtrl m_list;
	CListBox m_listSelCh;
	CEdit m_editLog;

	std::thread m_worker;
	std::atomic<bool> m_running{ false };
	volatile BOOL m_stop{ FALSE };
	ULONGLONG m_hangupStartTick = 0;
	bool m_hangupTimerOn = false;

	std::mutex m_uiRowMutex;
	std::vector<M576IlTestUiRow> m_uiRows;       // current lap only
	std::vector<M576IlTestUiRow> m_pendingUiRows; // same size; slot updates
	int m_uiLap = 0;
	int m_pendingLap = 0;
	bool m_pendingNewLap = false;
	int m_channelCount = 0;
	std::atomic<bool> m_rowFlushScheduled{ false };

	IlTestWlKind SelectedWl() const;
	BOOL IsSelectedChannelsMode() const;
	double ReadAbsIlMinDb() const;
	double ReadAbsIlMaxDb() const;
	double ReadSpanMaxDb() const;
	BOOL ReadGateParams(IlTestGateParams& out, CString& err) const;
	void UpdateModeControlsEnabled();
	BOOL CollectSelectedChannelRows(std::vector<M576DiagnosisRow>& rows, CString& err) const;
	void SetControlsRunning(BOOL running);
	void AppendLogLine(LPCTSTR line);
	void BeginLapUi(int channelCount, int lap);
	void QueueLapRow(int channelIndex0, M576IlTestUiRow row);
	void FlushPendingUiRows();
	void StartHangupClock();
	void StopHangupClock();
	void UpdateHangupClockText();
	void WorkerEntry(
		std::vector<M576DiagnosisRow> rows,
		CString outDir,
		IlTestWlKind wl,
		IlTestGateParams gate,
		CString mcs1Sn);

	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedModeFull();
	afx_msg void OnBnClickedModeSel();
	afx_msg void OnBnClickedSelAdd();
	afx_msg void OnBnClickedSelRemove();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUiLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiRowFlush(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiTemps(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUiFinished(WPARAM wParam, LPARAM lParam);
};
