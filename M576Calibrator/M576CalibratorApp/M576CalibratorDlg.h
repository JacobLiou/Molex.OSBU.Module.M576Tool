#pragma once

#include <atomic>
#include <thread>

#include "resource.h"
#include "OpComm.h"
#include "Z4671Command.h"
#include "LutBinWriter.h"
#include "Z4767StructDefine.h"
#include "RecalSession.h"
#include "PathCsvDriver.h"
#include "McsFwTransport.h"

/// Single serial link to 429F: ASCII RECAL + Z4671 binary frames (forwarded to MCS by firmware).
class CM576CalibratorDlg : public CDialogEx
{
public:
	enum { IDD = IDD_M576CALIBRATOR_DIALOG };

	CM576CalibratorDlg(CWnd* pParent = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_comboCom;
	CEdit m_editLog;
	CProgressCtrl m_progress;
	CString m_strCsv;
	CString m_strBackupBin;
	CString m_strOutBin;
	CString m_strSn;
	CString m_strCommLogPath;

	/// One port: 429F board (RECAL ASCII + MCS LUT transport via firmware forwarding).
	Z4671Command m_dev429f;
	std::unique_ptr<CRecalSession> m_pRecal;

	stLutSettingZ4671 m_lut;
	volatile BOOL m_bStop;

	std::thread m_pathThread;
	std::atomic<bool> m_pathRunning{ false };
	/// After user clicks Stop: ignore worker-thread progress updates until path thread exits.
	std::atomic<bool> m_suppressPathProgress{ false };

	/// 0 = power meter (RECAL 1), 1 = PD (RECAL 2). See DDX_Radio(IDC_RADIO_CAL_PM).
	int m_nCalMode;
	int m_delayMs;
	int m_dacRange;
	int m_dacStep;

	void AppendLog(LPCTSTR sz);
	void SafeAppendLog(LPCTSTR sz);
	void SafeSetProgressRange(int minVal, int maxVal);
	void SafeSetProgressPos(int pos);
	void SetPathActionButtonsEnabled(BOOL enable);
	void PathWorkerEntry();
	void WriteLogFileLine(const CString& line);
	void RunPathPowerMeter();
	void RunPathPd();
	void FillComPorts();
	CString GetComboCom();
	BOOL OpenPort();
	CString GetDefaultCsvPathForMode() const;
	void SyncCsvPathWithMode();
	void OnBrowse(UINT idEdit);
	/// Must match McsFwProgressCb (__cdecl, not CALLBACK/__stdcall).
	static void ProgressThunk(int cur, int total, void* user);
	static void __cdecl CommLogThunk(LPCTSTR line, void* user);

	afx_msg void OnBnClickedOpenPorts();
	afx_msg void OnBnClickedBrowseBackup();
	afx_msg void OnBnClickedBrowseOut();
	afx_msg void OnBnClickedReadFlashBackup();
	afx_msg void OnBnClickedCalPm();
	afx_msg void OnBnClickedCalPd();
	afx_msg void OnBnClickedRunPath();
	afx_msg void OnBnClickedGenBin();
	afx_msg void OnBnClickedFlash();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnPathLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPathProgressRange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPathProgressPos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPathFinished(WPARAM wParam, LPARAM lParam);
};
