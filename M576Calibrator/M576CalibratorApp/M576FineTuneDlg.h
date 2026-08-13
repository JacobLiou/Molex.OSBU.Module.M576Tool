#pragma once

#include "resource.h"
#include "FineTuneBinPatch.h"
#include "McsFwTransport.h"
#include "Pm1x64Mapping.h"
#include "SmallRangeCalibSelection.h"
#include "M576ChassisDebugDlg.h"

class CM576CalibratorDlg;

/// Manual low-temp DAC patch for one Backup/Standard burn file (A/B experiment).
/// Modeless: stay open while main window Burn Flash / Recover Flash runs.
/// Tab 0 FineTune / Tab 1 Chassis Debug (child page; same 439F port).
/// Right multiline edit is FineTune-local log (incl. RDAC); does not write main-window Log.
/// Write Bin does not close - change Address and write additional slots in one session.
/// Small Range starts PM Run Path on the owner without closing this dialog.
/// Read Before/After: RECAL 1 path switch + OPM 3 1 for burn-before/after PM compare.
class CM576FineTuneDlg : public CDialogEx
{
public:
	enum { IDD = IDD_M576_FINE_TUNE };

	CM576FineTuneDlg(
		CM576CalibratorDlg* pOwner,
		LPCTSTR outDirAbs,
		const M576TransSnPnInfo& sn,
		CWnd* pParent = NULL);

	BOOL Create();

	/// Append one line to the FineTune-local edit log (never main window).
	void AppendLocalLog(LPCTSTR line);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()

private:
	CM576CalibratorDlg* m_pOwner;
	CString m_outDirAbs;
	M576TransSnPnInfo m_snInfo;
	CTabCtrl m_tab;
	CM576ChassisDebugDlg m_chassisPage;
	CComboBox m_comboRole;
	CComboBox m_comboRecal;
	CEdit m_editLog;
	CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&> m_map1x64Rows;
	BOOL m_rebuildingRecal = FALSE;
	BOOL m_havePmBefore = FALSE;
	BOOL m_havePmAfter = FALSE;
	double m_pmBeforeDbm = 0.0;
	double m_pmAfterDbm = 0.0;
	BOOL m_clearPmOnAddressChange = TRUE;

	FineTuneAddress CollectAddress(CString& errMsg) const;
	FineTuneDeviceKind SelectedDevice() const;
	M576BinFileRole SelectedRole() const;
	void SyncAddressVisibility();
	void RebuildRecalCombo();
	void ApplyRecalSelection(BOOL doRefresh);
	BOOL ResolveAndShowPath(CString& errMsg);
	void RefreshStatusHint();
	void ClearPmCompare();
	void UpdatePmCompareUi();
	void BuildMapRowsForResolve(std::vector<SmallRangeMapRow>& out) const;
	void ReadPmSlot(BOOL isBefore);
	void ReadRdacSlot(int rdacIndex);
	void SetPmBusy(BOOL busy);
	void LayoutChassisPage();
	void ShowActiveTab();
	void BringCloseToTop();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedSmallRange();
	afx_msg void OnBnClickedReadBefore();
	afx_msg void OnBnClickedReadAfter();
	afx_msg void OnBnClickedRdac1();
	afx_msg void OnBnClickedRdac4();
	afx_msg void OnDeviceRadioChanged();
	afx_msg void OnRoleChanged();
	afx_msg void OnRecalSelChanged();
	afx_msg void OnAddressFieldChanged();
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
};
