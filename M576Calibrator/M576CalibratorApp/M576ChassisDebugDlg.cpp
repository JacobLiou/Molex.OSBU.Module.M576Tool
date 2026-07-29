#include "stdafx.h"
#include "M576ChassisDebugDlg.h"
#include "M576CalibratorDlg.h"
#include "DiagnosisSession.h"

#include <cstdlib>

namespace
{
	const DWORD kChassisAsciiTimeoutMs = 3000;
}

CM576ChassisDebugDlg::CM576ChassisDebugDlg(CM576CalibratorDlg* pOwner, CWnd* pParent)
	: CDialogEx(IDD_M576_CHASSIS_DEBUG, pParent)
	, m_pOwner(pOwner)
{
}

void CM576ChassisDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHASSIS_COMBO_1X8, m_cmb1x8);
	DDX_Control(pDX, IDC_CHASSIS_COMBO_1X2, m_cmb1x2);
	DDX_Control(pDX, IDC_CHASSIS_COMBO_1X64_1, m_cmb1x64_1);
	DDX_Control(pDX, IDC_CHASSIS_COMBO_1X64_2, m_cmb1x64_2);
	DDX_Control(pDX, IDC_CHASSIS_COMBO_MCS1_IDX, m_cmbMcs1Idx);
	DDX_Control(pDX, IDC_CHASSIS_COMBO_MCS1_PORT, m_cmbMcs1Port);
	DDX_Control(pDX, IDC_CHASSIS_COMBO_MCS2_IDX, m_cmbMcs2Idx);
	DDX_Control(pDX, IDC_CHASSIS_COMBO_MCS2_PORT, m_cmbMcs2Port);
	DDX_Control(pDX, IDC_CHASSIS_EDIT_LOG, m_editLog);
}

BEGIN_MESSAGE_MAP(CM576ChassisDebugDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHASSIS_BTN_1X8, &CM576ChassisDebugDlg::OnBnClicked1x8)
	ON_BN_CLICKED(IDC_CHASSIS_BTN_1X2, &CM576ChassisDebugDlg::OnBnClicked1x2)
	ON_BN_CLICKED(IDC_CHASSIS_BTN_1X64_1, &CM576ChassisDebugDlg::OnBnClicked1x64_1)
	ON_BN_CLICKED(IDC_CHASSIS_BTN_1X64_2, &CM576ChassisDebugDlg::OnBnClicked1x64_2)
	ON_BN_CLICKED(IDC_CHASSIS_BTN_MCS1, &CM576ChassisDebugDlg::OnBnClickedMcs1)
	ON_BN_CLICKED(IDC_CHASSIS_BTN_MCS2, &CM576ChassisDebugDlg::OnBnClickedMcs2)
	ON_BN_CLICKED(IDC_CHASSIS_BTN_READ_TLS, &CM576ChassisDebugDlg::OnBnClickedReadTls)
	ON_BN_CLICKED(IDC_CHASSIS_BTN_READ_OPM, &CM576ChassisDebugDlg::OnBnClickedReadOpm)
END_MESSAGE_MAP()

void CM576ChassisDebugDlg::FillComboRange(CComboBox& combo, int lo, int hiInclusive)
{
	combo.ResetContent();
	for (int v = lo; v <= hiInclusive; ++v)
	{
		CString s;
		s.Format(_T("%d"), v);
		combo.AddString(s);
	}
	if (combo.GetCount() > 0)
		combo.SetCurSel(0);
}

int CM576ChassisDebugDlg::SelectedChannel1Based(CComboBox& combo) const
{
	const int sel = combo.GetCurSel();
	if (sel < 0)
		return 1;
	CString s;
	combo.GetLBText(sel, s);
	const int v = _ttoi(s);
	return (v > 0) ? v : 1;
}

BOOL CM576ChassisDebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	FillComboRange(m_cmb1x8, 1, 8);
	FillComboRange(m_cmb1x2, 1, 2);
	FillComboRange(m_cmb1x64_1, 1, 64);
	FillComboRange(m_cmb1x64_2, 1, 64);
	FillComboRange(m_cmbMcs1Idx, 1, 32);
	FillComboRange(m_cmbMcs1Port, 1, 18);
	FillComboRange(m_cmbMcs2Idx, 1, 32);
	FillComboRange(m_cmbMcs2Port, 1, 18);
	AppendLogLine(_T("Chassis Debug ready. Commands use the main Connection serial port."));
	return TRUE;
}

void CM576ChassisDebugDlg::AppendLogLine(LPCTSTR line)
{
	if (!line)
		return;
	CString existing;
	m_editLog.GetWindowText(existing);
	if (!existing.IsEmpty() && existing.Right(1) != _T("\n"))
		existing += _T("\r\n");
	existing += line;
	existing += _T("\r\n");
	m_editLog.SetWindowText(existing);
	m_editLog.LineScroll(m_editLog.GetLineCount());
}

CDiagnosisSession* CM576ChassisDebugDlg::Session() const
{
	if (!m_pOwner)
		return nullptr;
	return m_pOwner->GetDiagnosisSessionForIlTest();
}

BOOL CM576ChassisDebugDlg::ReplyLooksOk(const CStringA& reply) const
{
	CStringA upper(reply);
	upper.MakeUpper();
	return upper.Find("OK") >= 0;
}

BOOL CM576ChassisDebugDlg::ExchangeSwitch(LPCTSTR label, const CStringA& wire, CString& err, CStringA& reply)
{
	err.Empty();
	reply.Empty();
	CDiagnosisSession* session = Session();
	if (!session)
	{
		err = _T("Serial session not ready. Open Port on the main window first.");
		return FALSE;
	}
	DWORD ms = 0;
	if (!session->ExchangeAsciiLine(label, wire, reply, kChassisAsciiTimeoutMs, ms, err))
		return FALSE;
	if (!ReplyLooksOk(reply))
	{
		err.Format(_T("Unexpected reply: %s"), CString(reply).GetString());
		return FALSE;
	}
	return TRUE;
}

void CM576ChassisDebugDlg::OnBnClicked1x8()
{
	const int ch = SelectedChannel1Based(m_cmb1x8);
	CStringA wire;
	wire.Format("SW 3 1 %d", ch);
	CString err;
	CStringA reply;
	CString label;
	label.Format(_T("SW 1x8 ch%d"), ch);
	if (!ExchangeSwitch(label, wire, err, reply))
	{
		CString msg;
		msg.Format(_T("Failed to switch 1x8 to channel %d: %s"), ch, err.GetString());
		AppendLogLine(msg);
		return;
	}
	CString msg;
	msg.Format(_T("Switched 1x8 to channel %d OK"), ch);
	AppendLogLine(msg);
}

void CM576ChassisDebugDlg::OnBnClicked1x2()
{
	const int ch = SelectedChannel1Based(m_cmb1x2);
	CStringA wire;
	wire.Format("SW 4 %d", ch);
	CString err;
	CStringA reply;
	CString label;
	label.Format(_T("SW 1x2 ch%d"), ch);
	if (!ExchangeSwitch(label, wire, err, reply))
	{
		CString msg;
		msg.Format(_T("Failed to switch 1x2 to channel %d: %s"), ch, err.GetString());
		AppendLogLine(msg);
		return;
	}
	CString msg;
	msg.Format(_T("Switched 1x2 to channel %d OK"), ch);
	AppendLogLine(msg);
}

void CM576ChassisDebugDlg::OnBnClicked1x64_1()
{
	const int ch = SelectedChannel1Based(m_cmb1x64_1);
	CStringA wire;
	wire.Format("SW 1 1 %d", ch);
	CString err;
	CStringA reply;
	CString label;
	label.Format(_T("SW 1x64#1 ch%d"), ch);
	if (!ExchangeSwitch(label, wire, err, reply))
	{
		CString msg;
		msg.Format(_T("Failed to switch 1x64 #1 to channel %d: %s"), ch, err.GetString());
		AppendLogLine(msg);
		return;
	}
	CString msg;
	msg.Format(_T("Switched 1x64 #1 to channel %d OK"), ch);
	AppendLogLine(msg);
}

void CM576ChassisDebugDlg::OnBnClicked1x64_2()
{
	const int ch = SelectedChannel1Based(m_cmb1x64_2);
	CStringA wire;
	wire.Format("SW 1 2 %d", ch);
	CString err;
	CStringA reply;
	CString label;
	label.Format(_T("SW 1x64#2 ch%d"), ch);
	if (!ExchangeSwitch(label, wire, err, reply))
	{
		CString msg;
		msg.Format(_T("Failed to switch 1x64 #2 to channel %d: %s"), ch, err.GetString());
		AppendLogLine(msg);
		return;
	}
	CString msg;
	msg.Format(_T("Switched 1x64 #2 to channel %d OK"), ch);
	AppendLogLine(msg);
}

void CM576ChassisDebugDlg::OnBnClickedMcs1()
{
	const int block = SelectedChannel1Based(m_cmbMcs1Idx);
	const int port = SelectedChannel1Based(m_cmbMcs1Port);
	CStringA wire;
	wire.Format("SW 2 %d %d", block, port);
	CString err;
	CStringA reply;
	CString label;
	label.Format(_T("SW MCS#1 b%d p%d"), block, port);
	if (!ExchangeSwitch(label, wire, err, reply))
	{
		CString msg;
		msg.Format(_T("Failed to switch MCS #1 block %d port %d: %s"), block, port, err.GetString());
		AppendLogLine(msg);
		return;
	}
	CString msg;
	msg.Format(_T("Switched MCS #1 block %d port %d OK"), block, port);
	AppendLogLine(msg);
}

void CM576ChassisDebugDlg::OnBnClickedMcs2()
{
	const int blockUi = SelectedChannel1Based(m_cmbMcs2Idx);
	const int blockWire = blockUi + 32;
	const int port = SelectedChannel1Based(m_cmbMcs2Port);
	CStringA wire;
	wire.Format("SW 2 %d %d", blockWire, port);
	CString err;
	CStringA reply;
	CString label;
	label.Format(_T("SW MCS#2 b%d p%d"), blockWire, port);
	if (!ExchangeSwitch(label, wire, err, reply))
	{
		CString msg;
		msg.Format(
			_T("Failed to switch MCS #2 block %d (wire %d) port %d: %s"),
			blockUi,
			blockWire,
			port,
			err.GetString());
		AppendLogLine(msg);
		return;
	}
	CString msg;
	msg.Format(_T("Switched MCS #2 block %d (wire %d) port %d OK"), blockUi, blockWire, port);
	AppendLogLine(msg);
}

void CM576ChassisDebugDlg::OnBnClickedReadTls()
{
	CDiagnosisSession* session = Session();
	if (!session)
	{
		AppendLogLine(_T("Serial session not ready. Open Port on the main window first."));
		return;
	}
	CStringA reply;
	CString err;
	DWORD ms = 0;
	if (!session->ExchangeAsciiLine(_T("PD TLS"), CStringA("pd 1"), reply, kChassisAsciiTimeoutMs, ms, err))
	{
		CString msg;
		msg.Format(_T("Failed to read TLS (PD) power: %s"), err.GetString());
		AppendLogLine(msg);
		return;
	}
	const double dbm = atof(reply.GetString()) / 10.0;
	CString msg;
	msg.Format(_T("TLS (PD) power: %.2f dBm"), dbm);
	AppendLogLine(msg);
}

void CM576ChassisDebugDlg::OnBnClickedReadOpm()
{
	CDiagnosisSession* session = Session();
	if (!session)
	{
		AppendLogLine(_T("Serial session not ready. Open Port on the main window first."));
		return;
	}
	CStringA reply;
	CString err;
	DWORD ms = 0;
	if (!session->ExchangeAsciiLine(_T("OPM"), CStringA("OPM 3 1"), reply, kChassisAsciiTimeoutMs, ms, err))
	{
		CString msg;
		msg.Format(_T("Failed to read OPM power: %s"), err.GetString());
		AppendLogLine(msg);
		return;
	}
	const double dbm = atof(reply.GetString()) / 10000.0;
	CString msg;
	msg.Format(_T("OPM power: %.4f dBm"), dbm);
	AppendLogLine(msg);
}
