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
	CComboBox m_comboCh;
	CEdit m_editLog;
	CEdit m_editSummary;
	CListCtrl m_listImpact;
	CFont m_fontCardNormal;
	CFont m_fontCardBold;
	CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&> m_map1;
	CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&> m_map2;
	PathDacImpactResult m_lastImpact;
	CString m_lastImpactText;
	int m_highlightStage;
	short m_slotLowY[6];
	short m_slotLowX[6];
	short m_slotRoomY[6];
	short m_slotRoomX[6];
	short m_slotHighY[6];
	short m_slotHighX[6];
	BOOL m_slotDacOk[6];

	M576BinFileRole SelectedRole() const;
	BOOL OwnerBusy() const;
	BOOL LoadMappings(CString& err);
	int SelectedChannel() const;
	void InitImpactListColumns();
	void InitChannelCombo();
	void ClearImpactUi();
	void RefreshPathImpact();
	void ApplyImpactToUi(const PathDacImpactResult& impact);
	void HighlightCard(int stage0to5);
	static CString Utf8ToCString(const std::string& s);
	static UINT CardId(int stage0to5);

	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedOpenFolder();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedResolvePath();
	afx_msg void OnBnClickedExportImpact();
	afx_msg void OnBnClickedCopyKeys();
	afx_msg void OnCbnSelchangeCh();
	afx_msg void OnLvnItemchangedImpact(NMHDR* pNMHDR, LRESULT* pResult);
};
