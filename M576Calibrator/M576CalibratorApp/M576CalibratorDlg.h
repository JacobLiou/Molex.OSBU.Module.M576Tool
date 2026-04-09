#pragma once

#include "resource.h"
#include "OpComm.h"
#include "Z4671Command.h"
#include "LutBinWriter.h"
#include "Z4767StructDefine.h"
#include "RecalSession.h"
#include "PathCsvDriver.h"
#include "McsFwTransport.h"

class CM576CalibratorDlg : public CDialog
{
public:
	enum { IDD = IDD_M576CALIBRATOR_DIALOG };

	CM576CalibratorDlg(CWnd* pParent = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_combo429f;
	CComboBox m_comboMcs;
	CEdit m_editLog;
	CProgressCtrl m_progress;
	CString m_strCsv;
	CString m_strBackupBin;
	CString m_strOutBin;
	CString m_strSn;

	COpComm m_comm429f;
	Z4671Command m_cmdMcs;
	std::unique_ptr<CRecalSession> m_pRecal;

	stLutSettingZ4671 m_lut;
	volatile BOOL m_bStop;

	void AppendLog(LPCTSTR sz);
	void FillComPorts();
	CString GetComboCom(CComboBox& combo);
	BOOL OpenPorts();
	void OnBrowse(UINT idEdit);
	static void CALLBACK ProgressThunk(int cur, int total, void* user);

	afx_msg void OnBnClickedOpenPorts();
	afx_msg void OnBnClickedBrowseCsv();
	afx_msg void OnBnClickedBrowseBackup();
	afx_msg void OnBnClickedBrowseOut();
	afx_msg void OnBnClickedRunPath();
	afx_msg void OnBnClickedGenBin();
	afx_msg void OnBnClickedFlash();
	afx_msg void OnBnClickedStop();
};
