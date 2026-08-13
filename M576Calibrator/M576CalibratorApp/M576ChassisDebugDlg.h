#pragma once

#include "Resource.h"

class CM576CalibratorDlg;
class CDiagnosisSession;

/// Chassis switch / power debug UI (FIM-compatible ASCII on the main 439F port).
/// Hosted as a WS_CHILD page inside FineTune (not a standalone modal).
class CM576ChassisDebugDlg : public CDialogEx
{
public:
	explicit CM576ChassisDebugDlg(CM576CalibratorDlg* pOwner, CWnd* pParent = nullptr);

	enum
	{
		IDD = IDD_M576_CHASSIS_DEBUG
	};

	void EnableCommands(BOOL enable);

protected:
	void DoDataExchange(CDataExchange* pDX) override;
	BOOL OnInitDialog() override;
	void OnOK() override;
	void OnCancel() override;
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClicked1x8();
	afx_msg void OnBnClicked1x2();
	afx_msg void OnBnClicked1x64_1();
	afx_msg void OnBnClicked1x64_2();
	afx_msg void OnBnClickedMcs1();
	afx_msg void OnBnClickedMcs2();
	afx_msg void OnBnClickedReadTls();
	afx_msg void OnBnClickedReadOpm();

private:
	CM576CalibratorDlg* m_pOwner = nullptr;
	CComboBox m_cmb1x8;
	CComboBox m_cmb1x2;
	CComboBox m_cmb1x64_1;
	CComboBox m_cmb1x64_2;
	CComboBox m_cmbMcs1Idx;
	CComboBox m_cmbMcs1Port;
	CComboBox m_cmbMcs2Idx;
	CComboBox m_cmbMcs2Port;
	CEdit m_editLog;

	void AppendLogLine(LPCTSTR line);
	void FillComboRange(CComboBox& combo, int lo, int hiInclusive);
	int SelectedChannel1Based(CComboBox& combo) const;
	CDiagnosisSession* Session() const;
	BOOL CanExchange(CString& err) const;
	BOOL ExchangeSwitch(LPCTSTR label, const CStringA& wire, CString& err, CStringA& reply);
	BOOL ReplyLooksOk(const CStringA& reply) const;
};
