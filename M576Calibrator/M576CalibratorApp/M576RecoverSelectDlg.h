#pragma once

#include "resource.h"
#include "McsFwTransport.h"
#include <array>

class CM576RecoverSelectDlg : public CDialogEx
{
public:
	CM576RecoverSelectDlg(CWnd* pParent, LPCTSTR backupBasePathAbs);
	enum
	{
		IDD = IDD_M576_RECOVER_SELECT
	};

	std::array<bool, M576_BURN_FILE_COUNT> GetMask() const;
	std::array<CString, M576_BURN_FILE_COUNT> GetPaths() const;

protected:
	void DoDataExchange(CDataExchange* pDX) override;
	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnBrowseBin(UINT nID);
	DECLARE_MESSAGE_MAP()

private:
	CString m_backupBasePathAbs;
	std::array<CString, M576_BURN_FILE_COUNT> m_pathEdits;
	std::array<bool, M576_BURN_FILE_COUNT> m_maskHold;
};

