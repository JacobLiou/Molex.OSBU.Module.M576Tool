#pragma once

#include "resource.h"
#include "FineTuneBinPatch.h"
#include "McsFwTransport.h"
#include "Pm1x64Mapping.h"

class CM576CalibratorDlg;

/// Manual low-temp DAC patch for one Backup/Standard burn file (A/B experiment).
/// Write Bin does not close the dialog — change Address and write additional slots in one session.
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
	CComboBox m_comboRecal;
	CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&> m_map1x64Rows;
	BOOL m_rebuildingRecal = FALSE;

	FineTuneAddress CollectAddress(CString& errMsg) const;
	FineTuneDeviceKind SelectedDevice() const;
	M576BinFileRole SelectedRole() const;
	void SyncAddressVisibility();
	void RebuildRecalCombo();
	void ApplyRecalSelection(BOOL doRefresh);
	BOOL ResolveAndShowPath(CString& errMsg);
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnDeviceRadioChanged();
	afx_msg void OnRoleChanged();
	afx_msg void OnRecalSelChanged();
};
