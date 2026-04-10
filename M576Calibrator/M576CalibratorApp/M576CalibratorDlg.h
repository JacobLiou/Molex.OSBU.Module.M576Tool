#pragma once

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

	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_comboCom;
	CEdit m_editLog;
	CProgressCtrl m_progress;
	CString m_strCsv;
	CString m_strBackupBin;
	CString m_strOutBin;
	CString m_strSn;

	/// One port: 429F board (RECAL ASCII + MCS LUT transport via firmware forwarding).
	Z4671Command m_dev429f;
	std::unique_ptr<CRecalSession> m_pRecal;

	stLutSettingZ4671 m_lut;
	volatile BOOL m_bStop;

	void AppendLog(LPCTSTR sz);
	void FillComPorts();
	CString GetComboCom();
	BOOL OpenPort();
	void OnBrowse(UINT idEdit);
	/// Must match McsFwProgressCb (__cdecl, not CALLBACK/__stdcall).
	static void ProgressThunk(int cur, int total, void* user);

	afx_msg void OnBnClickedOpenPorts();
	afx_msg void OnBnClickedBrowseCsv();
	afx_msg void OnBnClickedBrowseBackup();
	afx_msg void OnBnClickedBrowseOut();
	afx_msg void OnBnClickedReadFlashBackup();
	afx_msg void OnBnClickedRunPath();
	afx_msg void OnBnClickedGenBin();
	afx_msg void OnBnClickedFlash();
	afx_msg void OnBnClickedStop();
};
