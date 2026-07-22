#pragma once

#include "resource.h"
#include "FineTuneBinPatch.h"
#include "McsFwTransport.h"

class CM576CalibratorDlg;

/// Manual low-temp DAC patch for one Backup/Standard burn file (A/B experiment).
class CM576FineTuneDlg : public CDialogEx
{
public:
	enum { IDD = IDD_M576_FINE_TUNE };

	CM576FineTuneDlg(
		CM576CalibratorDlg* pOwner,
		LPCTSTR outDirAbs,
		const M576TransSnPnInfo& sn,
		CWnd* pParent = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	CM576CalibratorDlg* m_pOwner;
	CString m_outDirAbs;
	M576TransSnPnInfo m_snInfo;
	CComboBox m_comboRole;

	FineTuneAddress CollectAddress(CString& errMsg) const;
	M576BinFileRole SelectedRole() const;
	void SyncAddressVisibility();
	BOOL ResolveAndShowPath(CString& errMsg);
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnDeviceRadioChanged();
	afx_msg void OnRoleChanged();
};
