#pragma once

#include "resource.h"
#include "FullEditDacIo.h"
#include "McsFwTransport.h"
#include "PathDacImpact.h"
#include "Pm1x64Mapping.h"

class CM576CalibratorDlg;

/// R&D debug: FullEdit Bin CSV export/diff-write + path DAC impact. Modeless; does not Burn.
class CM576FullEditDlg : public CDialogEx
{
public:
	enum { IDD = IDD_M576_FULL_EDIT };

	CM576FullEditDlg(
		CM576CalibratorDlg* pOwner,
		LPCTSTR outDirAbs,
		const M576TransSnPnInfo& sn,
		CWnd* pParent = NULL);

	BOOL Create();
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
	CComboBox m_comboRole;
	CEdit m_editLog;
	CEdit m_editImpact;
	CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&> m_map1;
	CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&> m_map2;

	M576BinFileRole SelectedRole() const;
	BOOL OwnerBusy() const;
	BOOL LoadMappings(CString& err);
	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedOpenFolder();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedResolvePath();
	afx_msg void OnBnClickedExportImpact();
	afx_msg void OnBnClickedCopyKeys();
};
