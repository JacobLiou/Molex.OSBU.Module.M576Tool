#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include "resource.h"
#include "OpComm.h"
#include "Z4671Command.h"
#include "LutBinWriter.h"
#include "M576OneX64Coef.h"
#include "Z4767StructDefine.h"
#include "RecalSession.h"
#include "PathCsvDriver.h"
#include "McsFwTransport.h"
#include "TransLutRoute.h"
#include "CalibWriteMeta.h"

/// Single serial link to 439F: ASCII RECAL + Z4671 binary (explicit `trans`/`$$` for Flash read/burn).
// 主界面对话框：单 COM 连 439F；定标为 ASCII RECAL，读/写 Flash 与上载 bin 为经 trans/$$ 的 Z4671 二进制。
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
	// --- UI 与路径字符串 ---
	CComboBox m_comboCom;
	CComboBox m_comboTls;
	CComboBox m_comboWavelength;
	CComboBox m_comboPmRange;
	CEdit m_editLog;
	CProgressCtrl m_progress;
	CString m_strCsvPm[4];
	CString m_strCsvPd[4];
	CString m_strBackupBin;
	CString m_strOutBin;
	/// Per trans 1~4 bundle header `pBundleSN` when writing MCS/1x64 bins (not 439F main board).
	// 与 g_m576TransLutBinSuffix 顺序一致：MCS1,MCS2,1x64#1,1x64#2。
	CString m_strSnTrans[4];
	CString m_strCommLogPath;

	/// One port: 439F board (RECAL ASCII + MCS LUT transport via trans/$$).
	// 单路 Z4671Command 绑定 429F 串口；CRecalSession 封装 RECAL 文本层。
	Z4671Command m_dev429f;
	std::unique_ptr<CRecalSession> m_pRecal;

	/// Trans 1-2: Z4671 LUT; Trans 3-4: 1x64 为 4x2K stMemsSwCoef（126S，与 MCS 不共用同结构体）。
	// 仅 0,1 槽使用 m_lut；2,3 使用 m_mems1x64[0]=1#1x64、 m_mems1x64[1]=2#1x64。
	stLutSettingZ4671 m_lutByTrans[4];
	stM576OneX64MemsSwCoef m_mems1x64[2][4];
	volatile BOOL m_bStop;

	// --- 工作线程与进度（路径、读备份）---
	std::thread m_pathThread;
	std::thread m_readBackupThread;
	std::thread m_readSnThread;
	std::atomic<bool> m_pathRunning{ false };
	std::atomic<bool> m_readBackupRunning{ false };
	std::atomic<bool> m_readSnRunning{ false };
	/// After user clicks Stop: ignore worker-thread progress updates until path thread exits.
	// 用户点停止后，路径线程未退出前忽略子线程的进度回写，防 UI 抖动。
	std::atomic<bool> m_suppressPathProgress{ false };
	/// Worker `SafeAppendLog` coalesces into a queue; a single `WM_M576_PATH_LOG_FLUSH` drains all pending lines
	/// (avoids thousands of per-line posts that block the UI thread on restore from minimize).
	// 路径日志批量刷 UI，减少 PostMessage 洪峰（如最小化恢复不卡死）。
	std::mutex m_pathLogQueueMutex;
	CString m_queuedPathLog;
	std::atomic<bool> m_pathLogFlushScheduled{ false };

	/// Filled by `ReadFlashBackupWorkerEntry` before `WM_M576_READ_BACKUP_FINISHED`; shown in `OnReadBackupFinished` (UI thread).
	// 读 Flash 备份线程结果，由主线程在 OnReadBackupFinished 中展示。
	BOOL m_readBackupLastOk;
	CString m_readBackupLastMsg;
	BOOL m_readSnLastOk;
	CString m_readSnLastMsg;
	CString m_readSnLastValues[4];

	// --- 定标模式与 RECAL 步参 ---
	/// 0 = power meter (RECAL 1), 1 = PD (RECAL 2). See DDX_Radio(IDC_RADIO_CAL_PM).
	int m_nCalMode;
	int m_delayMs;
	int m_dacRange;
	int m_dacStep;
	/// RECAL 0 (Command A): TLS 1-8 -> index 0-7 in `IDC_COMBO_TLS`.
	int m_tlsIndex;
	/// Wavelength (nm as string): combo preset 1310/1550 or user-typed value.
	CString m_strWavelength;
	/// PM range 0-4 -> combo index.
	int m_pmRangeIndex;

	/// 本会话定标步统计（供 CSV 导出）；与路径线程用 `m_statsRowsMutex` 同步。
	std::vector<SCalibrationStatRow> m_statsRows;
	std::mutex m_statsRowsMutex;
	void ClearCalibStats();
	void PushCalibStatRow(const SCalibrationStatRow& r);
	void SyncExportStatsButton();

	// --- 日志与 UI 安全调用（可跨线程）---
	void AppendLog(LPCTSTR sz);
	void SafeAppendLog(LPCTSTR sz);
	void SafeSetProgressRange(int minVal, int maxVal);
	void SafeSetProgressPos(int pos);
	void SetPathActionButtonsEnabled(BOOL enable);
	/// Open vs Close mutual exclusion; Close disabled while path or flash-backup worker running.
	// 串口打开/关闭与运行中互斥；跑路径或读备份时不可关错口。
	void SyncSerialPortUi();
	BOOL IsSerialPortOpen() const;
	// 后台：跑完整定标路径 / 只读 Flash 备份
	void PathWorkerEntry();
	void ReadFlashBackupWorkerEntry(CString absBackupBin);
	void ReadAllSnWorkerEntry();
	void WriteLogFileLine(const CString& line);
	void RunPathPowerMeter();
	void RunPathPd();
	void RunPathPowerMeterFile(int fileSlot, CArray<SPathStep, SPathStep const&>& steps, int& globalProgress, int globalTotal, int& occT3, int& occT4);
	void RunPathPdFile(int fileSlot, CArray<SPathStepPd, SPathStepPd const&>& steps, int& globalProgress, int globalTotal, int& occT3, int& occT4);
	/// If Backup BIN base is set, load existing `*_mcs1.bin` … `*_1x64_2.bin` (or legacy `*_tN.bin`) into `m_lutByTrans` before a path run.
	// 若设了备份基名，跑路径前把已存在的分 trans bin 预载到 m_lutByTrans（含旧 tN 名兼容）。
	void TryPreloadLutFromPerTransBackup();
	void FillComPorts();
	CString GetComboCom();
	BOOL OpenPort();
	void ClosePort();
	/// Reset PM/PD path strings to `g_m576Default*` and refresh the brief path hint for current mode.
	// 随 PM/PD 单选切换默认 CSV 显示与提示。
	void SyncCsvPathWithMode();
	/// PM: show Command A (RECAL 0) controls; PD: hide (PD flow uses Command C only, no RECAL 0).
	// PM 显示命令 A 控件；PD 流程仅命令 C，不显示 RECAL 0。
	void SyncRecal0ControlsVisibility();
	/// Before Run path: COM, PM wavelength (if PM), built-in CSV files exist. MessageBox and return FALSE when invalid.
	// 跑路径前：串口、（PM 时）波长、内置 CSV 等输入校验。
	BOOL ValidateRunPathInputs(CString& errMsg);
	/// Must match McsFwProgressCb (__cdecl, not CALLBACK/__stdcall).
	// 进度回调节点，调用约定须与 McsFwProgressCb 一致（__cdecl）。
	static void ProgressThunk(int cur, int total, void* user);
	static void __cdecl CommLogThunk(LPCTSTR line, void* user);

	// --- 按钮与自定义消息（路径日志/进度/完成）---
	afx_msg void OnBnClickedOpenPorts();
	afx_msg void OnBnClickedClosePort();
	afx_msg void OnBnClickedBrowseBackup();
	afx_msg void OnBnClickedBrowseOut();
	afx_msg void OnBnClickedReadFlashBackup();
	afx_msg void OnBnClickedCalPm();
	afx_msg void OnBnClickedCalPd();
	afx_msg void OnBnClickedRunPath();
	afx_msg void OnBnClickedClearLog();
	afx_msg void OnBnClickedGenBin();
	afx_msg void OnBnClickedReadAllSn();
	afx_msg void OnBnClickedFlash();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedExportCalibStats();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnPathLogFlush(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPathProgressRange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPathProgressPos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPathFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReadBackupFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReadAllSnFinished(WPARAM wParam, LPARAM lParam);
};
