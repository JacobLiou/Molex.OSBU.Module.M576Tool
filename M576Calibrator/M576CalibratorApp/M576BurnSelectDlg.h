#pragma once
#include "Resource.h"
#include "McsFwTransport.h"
#include <array>

/// Which of the 10 per-trans .bin files to burn (order = M576_BURN_FILE_COUNT / McsFwUploadBinEx; default all on).
class CM576BurnSelectDlg : public CDialogEx
{
public:
	CM576BurnSelectDlg(CWnd* pParent, LPCTSTR basePathHint);
	std::array<bool, M576_BURN_FILE_COUNT> GetMask() const;
	enum
	{
		IDD = IDD_M576_BURN_SELECT
	};

protected:
	void DoDataExchange(CDataExchange* pDX) override;
	BOOL OnInitDialog() override;
	/// Capture checkbox state before the modal `HWND` is destroyed (`DoModal` returns).
	void OnOK() override;
	DECLARE_MESSAGE_MAP()

private:
	CString m_basePathHint;
	/// Filled in `OnOK` while child controls are valid; read after `DoModal` in `GetMask()`.
	std::array<bool, M576_BURN_FILE_COUNT> m_burnMaskHold{};
};
