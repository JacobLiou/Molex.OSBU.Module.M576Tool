#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576IlTestDlg.h"
#include "M576CalibratorDlg.h"
#include "IlTestCsv.h"
#include "CalibConstants.h"
#include "DiagnosisSession.h"
#include "resource.h"

#include <commctrl.h>

namespace
{
constexpr UINT WM_M576_IL_LOG = WM_APP + 210;
constexpr UINT WM_M576_IL_ROW = WM_APP + 211;
constexpr UINT WM_M576_IL_STATUS = WM_APP + 212;
constexpr UINT WM_M576_IL_FINISHED = WM_APP + 213;

struct IlUiRow
{
	CString channel;
	CString mpoPath;
	CString wl;
	int lap = 0;
	int pd = 0;
	int opm = 0;
	double il = 0;
	double mx = 0;
	double mn = 0;
	double span = 0;
	CString result;
};

CString NowTimeStamp()
{
	SYSTEMTIME st{};
	::GetLocalTime(&st);
	CString s;
	s.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"),
		(unsigned)st.wYear, (unsigned)st.wMonth, (unsigned)st.wDay,
		(unsigned)st.wHour, (unsigned)st.wMinute, (unsigned)st.wSecond);
	return s;
}

void FormatSwlWireLocal(CStringA& wire, int tlsSource, int wavelengthNm)
{
	wire.Format("SWL %d %d", tlsSource, wavelengthNm);
}

BOOL ParseDoubleEdit(CWnd* dlg, int id, double& out)
{
	if (!dlg)
		return FALSE;
	CString s;
	dlg->GetDlgItemText(id, s);
	s.Trim();
	if (s.IsEmpty())
		return FALSE;
	out = _ttof(s);
	return TRUE;
}
} // namespace

CM576IlTestDlg::CM576IlTestDlg(CM576CalibratorDlg* pOwner, CWnd* pParent)
	: CDialogEx(IDD_M576_IL_TEST, pParent ? pParent : pOwner)
	, m_pOwner(pOwner)
{
}

CM576IlTestDlg::~CM576IlTestDlg()
{
	m_stop = TRUE;
	if (m_worker.joinable())
		m_worker.join();
}

void CM576IlTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IL_LIST, m_list);
	DDX_Control(pDX, IDC_IL_EDIT_LOG, m_editLog);
}

BEGIN_MESSAGE_MAP(CM576IlTestDlg, CDialogEx)
	ON_BN_CLICKED(IDC_IL_BTN_START, &CM576IlTestDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_IL_BTN_STOP, &CM576IlTestDlg::OnBnClickedStop)
	ON_MESSAGE(WM_M576_IL_LOG, &CM576IlTestDlg::OnUiLog)
	ON_MESSAGE(WM_M576_IL_ROW, &CM576IlTestDlg::OnUiRow)
	ON_MESSAGE(WM_M576_IL_STATUS, &CM576IlTestDlg::OnUiStatus)
	ON_MESSAGE(WM_M576_IL_FINISHED, &CM576IlTestDlg::OnUiFinished)
END_MESSAGE_MAP()

BOOL CM576IlTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CheckRadioButton(IDC_IL_RADIO_SFP1550, IDC_IL_RADIO_LASER1310, IDC_IL_RADIO_LASER1310);
	CString thr;
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_SPAN_DB);
	SetDlgItemText(IDC_IL_EDIT_THRESHOLD, thr);
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_ABS_MIN_DB);
	SetDlgItemText(IDC_IL_EDIT_ABS_MIN, thr);
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_ABS_MAX_DB);
	SetDlgItemText(IDC_IL_EDIT_ABS_MAX, thr);
	SetDlgItemText(IDC_IL_STATIC_STATUS, _T("Idle - IL = Pref(PD)-Pout(OPM); gate: span + abs IL."));

	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("Channel"), LVCFMT_LEFT, 85);
	m_list.InsertColumn(1, _T("MPO Path"), LVCFMT_LEFT, 180);
	m_list.InsertColumn(2, _T("Wl"), LVCFMT_LEFT, 75);
	m_list.InsertColumn(3, _T("Lap"), LVCFMT_RIGHT, 85);
	m_list.InsertColumn(4, _T("PD"), LVCFMT_RIGHT, 85);
	m_list.InsertColumn(5, _T("OPM"), LVCFMT_RIGHT, 85);
	m_list.InsertColumn(6, _T("IL"), LVCFMT_RIGHT, 85);
	m_list.InsertColumn(7, _T("Max"), LVCFMT_RIGHT, 85);
	m_list.InsertColumn(8, _T("Min"), LVCFMT_RIGHT, 85);
	m_list.InsertColumn(9, _T("Span"), LVCFMT_RIGHT, 85);
	m_list.InsertColumn(10, _T("Result"), LVCFMT_LEFT, 100);

	if (CWnd* p = GetDlgItem(IDC_IL_BTN_STOP))
		p->EnableWindow(FALSE);
	return TRUE;
}

IlTestWlKind CM576IlTestDlg::SelectedWl() const
{
	if (IsDlgButtonChecked(IDC_IL_RADIO_SFP1550) == BST_CHECKED)
		return IlTestWlKind::Sfp1550;
	if (IsDlgButtonChecked(IDC_IL_RADIO_SFP1310) == BST_CHECKED)
		return IlTestWlKind::Sfp1310;
	return IlTestWlKind::Laser1310;
}

double CM576IlTestDlg::ReadSpanMaxDb() const
{
	double v = 0.0;
	if (!ParseDoubleEdit(const_cast<CM576IlTestDlg*>(this), IDC_IL_EDIT_THRESHOLD, v))
		return (double)M576_IL_TEST_DEFAULT_SPAN_DB;
	if (!(v > 0.0) || v > 100.0)
		return (double)M576_IL_TEST_DEFAULT_SPAN_DB;
	return v;
}

double CM576IlTestDlg::ReadAbsIlMinDb() const
{
	double v = 0.0;
	if (!ParseDoubleEdit(const_cast<CM576IlTestDlg*>(this), IDC_IL_EDIT_ABS_MIN, v))
		return (double)M576_IL_TEST_DEFAULT_ABS_MIN_DB;
	return v;
}

double CM576IlTestDlg::ReadAbsIlMaxDb() const
{
	double v = 0.0;
	if (!ParseDoubleEdit(const_cast<CM576IlTestDlg*>(this), IDC_IL_EDIT_ABS_MAX, v))
		return (double)M576_IL_TEST_DEFAULT_ABS_MAX_DB;
	return v;
}

BOOL CM576IlTestDlg::ReadGateParams(IlTestGateParams& out, CString& err) const
{
	err.Empty();
	out.spanMaxDb = ReadSpanMaxDb();
	out.absIlMinDb = ReadAbsIlMinDb();
	out.absIlMaxDb = ReadAbsIlMaxDb();
	if (!(out.absIlMinDb <= out.absIlMaxDb))
	{
		err = _T("IL abs min must be <= abs max.");
		return FALSE;
	}
	if (out.absIlMaxDb > 100.0 || out.absIlMinDb < -50.0)
	{
		err = _T("IL abs window out of range.");
		return FALSE;
	}
	return TRUE;
}

void CM576IlTestDlg::SetControlsRunning(BOOL running)
{
	const BOOL idle = !running;
	if (CWnd* p = GetDlgItem(IDC_IL_RADIO_SFP1550))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_RADIO_SFP1310))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_RADIO_LASER1310))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_EDIT_THRESHOLD))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_EDIT_ABS_MIN))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_EDIT_ABS_MAX))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_BTN_START))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_BTN_STOP))
		p->EnableWindow(running);
}

void CM576IlTestDlg::AppendLogLine(LPCTSTR line)
{
	if (!line)
		return;
	CString cur;
	m_editLog.GetWindowText(cur);
	if (!cur.IsEmpty())
		cur += _T("\r\n");
	cur += line;
	const int maxChars = 120000;
	if (cur.GetLength() > maxChars)
		cur = cur.Right(maxChars);
	m_editLog.SetWindowText(cur);
	m_editLog.LineScroll(m_editLog.GetLineCount());
}

void CM576IlTestDlg::OnBnClickedStart()
{
	if (m_running.load() || m_pOwner == NULL)
		return;

	IlTestGateParams gate;
	CString gateErr;
	if (!ReadGateParams(gate, gateErr))
	{
		MessageBox(gateErr, _T("IL Test"), MB_OK | MB_ICONWARNING);
		return;
	}

	CString outDir;
	CString err;
	if (!m_pOwner->BeginIlTestSession(outDir, err))
	{
		MessageBox(err.IsEmpty() ? _T("Cannot start IL Test.") : err, _T("IL Test"), MB_OK | MB_ICONWARNING);
		return;
	}

	const CString swCsvPath = outDir + _T("\\diagnosis_sw.csv");
	std::vector<M576DiagnosisRow> rows;
	if (!M576LoadDiagnosisSwCsv(swCsvPath, rows, err))
	{
		m_pOwner->EndIlTestSession();
		CString msg;
		msg.Format(_T("Load %s failed: %s"), swCsvPath.GetString(), err.IsEmpty() ? _T("(unknown)") : err.GetString());
		MessageBox(msg, _T("IL Test"), MB_OK | MB_ICONWARNING);
		return;
	}
	if (rows.empty())
	{
		m_pOwner->EndIlTestSession();
		MessageBox(_T("diagnosis_sw.csv has no channel groups."), _T("IL Test"), MB_OK | MB_ICONWARNING);
		return;
	}

	const IlTestWlKind wl = SelectedWl();
	CString ui;
	ui.Format(_T("%.4f"), gate.spanMaxDb);
	SetDlgItemText(IDC_IL_EDIT_THRESHOLD, ui);
	ui.Format(_T("%.4f"), gate.absIlMinDb);
	SetDlgItemText(IDC_IL_EDIT_ABS_MIN, ui);
	ui.Format(_T("%.4f"), gate.absIlMaxDb);
	SetDlgItemText(IDC_IL_EDIT_ABS_MAX, ui);

	if (m_worker.joinable())
		m_worker.join();
	m_stop = FALSE;
	m_running = true;
	SetControlsRunning(TRUE);

	CString startMsg;
	startMsg.Format(
		_T("IL Test started: %d ch, %s, IL=Pref-Pout, span<=%.3f, abs=[%.3f,%.3f] dB, log=%s"),
		(int)rows.size(),
		IlTestWlLabel(wl),
		gate.spanMaxDb,
		gate.absIlMinDb,
		gate.absIlMaxDb,
		M576GetIlTestLogCsvPath(outDir).GetString());
	AppendLogLine(startMsg);

	m_worker = std::thread([this, rows, outDir, wl, gate]() {
		WorkerEntry(rows, outDir, wl, gate);
	});
}

void CM576IlTestDlg::OnBnClickedStop()
{
	if (!m_running.load())
		return;
	m_stop = TRUE;
	AppendLogLine(_T("Stop requested..."));
}

void CM576IlTestDlg::OnCancel()
{
	if (m_running.load())
	{
		m_stop = TRUE;
		if (m_worker.joinable())
			m_worker.join();
		m_running = false;
		if (m_pOwner)
			m_pOwner->EndIlTestSession();
	}
	else if (m_worker.joinable())
	{
		m_worker.join();
	}
	CDialogEx::OnCancel();
}

void CM576IlTestDlg::OnOK()
{
}

void CM576IlTestDlg::WorkerEntry(std::vector<M576DiagnosisRow> rows, CString outDir, IlTestWlKind wl, IlTestGateParams gate)
{
	const int N = (int)rows.size();
	const CString csvPath = M576GetIlTestLogCsvPath(outDir);
	const CString wlLabel(IlTestWlLabel(wl));
	const int sw3Third = IlTestSw3Third(wl);
	const int wlNm = IlTestWavelengthNm(wl);

	constexpr DWORD kInterCmdMs = 40;
	constexpr DWORD kPdPreMs = 1000;
	constexpr DWORD kSliceMs = 50;

	IlTestStatsMap stats;

	auto PostLog = [this](const CString& s) {
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_IL_LOG, 0, (LPARAM)new CString(s));
	};
	auto PostStatus = [this](const CString& s) {
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_IL_STATUS, 0, (LPARAM)new CString(s));
	};
	auto PostRow = [this](IlUiRow* r) {
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_IL_ROW, 0, (LPARAM)r);
		else
			delete r;
	};

	auto DelayMs = [this, kSliceMs](DWORD delayMs) {
		DWORD remaining = delayMs;
		while (remaining > 0 && !m_stop)
		{
			const DWORD t = remaining > kSliceMs ? kSliceMs : remaining;
			::Sleep(t);
			remaining -= t;
		}
	};

	bool firstCmd = true;
	auto MaybeDelay = [&]() {
		if (firstCmd)
		{
			firstCmd = false;
			return;
		}
		DelayMs(kInterCmdMs);
	};

	int fullLaps = 0;
	for (;;)
	{
		if (m_stop)
			break;
		if (fullLaps > 0)
			DelayMs(kInterCmdMs);
		if (m_stop)
			break;

		++fullLaps;
		firstCmd = true;

		CDiagnosisSession* session = m_pOwner ? m_pOwner->GetDiagnosisSessionForIlTest() : NULL;
		if (session == NULL)
		{
			PostLog(_T("IL Test: serial session closed; aborting."));
			break;
		}

		for (int i = 0; i < N; ++i)
		{
			if (m_stop)
				break;
			session = m_pOwner->GetDiagnosisSessionForIlTest();
			if (session == NULL)
			{
				PostLog(_T("IL Test: serial session closed; aborting."));
				m_stop = TRUE;
				break;
			}

			const M576DiagnosisRow& src = rows[(size_t)i];
			CStringA channelA = src.channel;
			if (channelA.IsEmpty())
				channelA = src.label;
			const CString chName(channelA);
			CString mpoPath;
			const int chIdx = IlTestParseChannelIndex(chName);
			if (!IlTestChannelToMpoPath(chIdx, mpoPath))
				mpoPath = _T("-");

			CString err;
			for (size_t k = 0; k < src.swCommands.size(); ++k)
			{
				if (m_stop)
					break;
				MaybeDelay();
				if (m_stop)
					break;
				CString labelSw;
				labelSw.Format(_T("SW %d.%d/%d"), i + 1, (int)k + 1, (int)src.swCommands.size());
				CStringA reply;
				DWORD ms = 0;
				const BOOL ok = session->ExchangeAsciiLine(labelSw, src.swCommands[k], reply, 3000, ms, err);
				if (!ok || reply.CompareNoCase("OK") != 0)
				{
					CString log;
					log.Format(_T("%s: SW fail reply=%hs"), chName.GetString(), reply.GetString());
					PostLog(log);
				}
			}
			if (m_stop)
				break;

			CStringA sw3;
			sw3.Format("SW 3 1 %d", sw3Third);
			MaybeDelay();
			if (m_stop)
				break;
			{
				CStringA sw3Reply;
				DWORD ms = 0;
				CString label;
				label.Format(_T("SW31 %d"), sw3Third);
				(void)session->ExchangeAsciiLine(label, sw3, sw3Reply, 3000, ms, err);
			}

			MaybeDelay();
			if (m_stop)
				break;
			{
				CStringA wire;
				FormatSwlWireLocal(wire, M576_DIAG_SWL_TLS_SOURCE, wlNm);
				CStringA wlReply;
				DWORD ms = 0;
				CString label;
				label.Format(_T("SWL %d %d"), M576_DIAG_SWL_TLS_SOURCE, wlNm);
				(void)session->ExchangeAsciiLine(label, wire, wlReply, 3000, ms, err);
			}

			MaybeDelay();
			if (m_stop)
				break;
			DelayMs(kPdPreMs);
			if (m_stop)
				break;

			CStringA pdReply;
			CStringA opmReply;
			{
				DWORD ms = 0;
				(void)session->ExchangeAsciiLine(_T("PD"), CStringA("pd 1"), pdReply, 3000, ms, err);
			}
			MaybeDelay();
			if (m_stop)
				break;
			{
				DWORD ms = 0;
				(void)session->ExchangeAsciiLine(_T("OPM"), CStringA("opm 3 1"), opmReply, 3000, ms, err);
			}

			int pdRaw = 0;
			int opmRaw = 0;
			if (!IlTestParseIntReply(pdReply, pdRaw) || !IlTestParseIntReply(opmReply, opmRaw))
			{
				CString log;
				log.Format(_T("%s: parse PD/OPM failed (pd=%hs opm=%hs)"),
					chName.GetString(), pdReply.GetString(), opmReply.GetString());
				PostLog(log);
				continue;
			}

			const double il = IlTestComputeIlDb(pdRaw, opmRaw);
			const CString key = IlTestStatsKey(chName, wlLabel);
			IlTestRollingStats& st = stats[key];
			st.Add(il);
			const double span = st.Span();
			const BOOL pass = IlTestJudgePass(il, span, gate);

			M576IlTestLogRow crow;
			crow.timeStamp = NowTimeStamp();
			crow.lap = fullLaps;
			crow.channel = chName;
			crow.mpoPath = mpoPath;
			crow.wlLabel = wlLabel;
			crow.pdRaw = pdRaw;
			crow.opmRaw = opmRaw;
			crow.ilDb = il;
			crow.ilMax = st.ilMax;
			crow.ilMin = st.ilMin;
			crow.ilSpan = span;
			crow.pass = pass;
			crow.spanMaxDb = gate.spanMaxDb;
			crow.absIlMinDb = gate.absIlMinDb;
			crow.absIlMaxDb = gate.absIlMaxDb;
			CString csvErr;
			if (!M576AppendIlTestLogRow(csvPath, crow, csvErr))
			{
				CString log;
				log.Format(_T("CSV append failed: %s"), csvErr.GetString());
				PostLog(log);
			}

			IlUiRow* uiRow = new IlUiRow();
			uiRow->channel = chName;
			uiRow->mpoPath = mpoPath;
			uiRow->wl = wlLabel;
			uiRow->lap = fullLaps;
			uiRow->pd = pdRaw;
			uiRow->opm = opmRaw;
			uiRow->il = il;
			uiRow->mx = st.ilMax;
			uiRow->mn = st.ilMin;
			uiRow->span = span;
			uiRow->result = pass ? _T("PASS") : _T("FAIL");
			PostRow(uiRow);

			CString status;
			status.Format(
				_T("Current: %s %s | Lap %d | %s | IL=%.4f | span=%.4f | %s"),
				chName.GetString(), mpoPath.GetString(), fullLaps, wlLabel.GetString(), il, span,
				pass ? _T("PASS") : _T("FAIL"));
			PostStatus(status);

			CString logLine;
			logLine.Format(
				_T("%s %s lap=%d IL=%.4f span=%.4f %s (PD=%d OPM=%d Pref=%.2f Pout=%.4f)"),
				chName.GetString(), mpoPath.GetString(), fullLaps, il, span,
				pass ? _T("PASS") : _T("FAIL"), pdRaw, opmRaw,
				IlTestPrefDbm(pdRaw), IlTestPoutDbm(opmRaw));
			PostLog(logLine);
		}
	}

	if (m_hWnd && ::IsWindow(m_hWnd))
		::PostMessage(m_hWnd, WM_M576_IL_FINISHED, m_stop ? 1 : 0, (LPARAM)fullLaps);
}

LRESULT CM576IlTestDlg::OnUiLog(WPARAM, LPARAM lParam)
{
	CString* p = (CString*)lParam;
	if (p)
	{
		AppendLogLine(*p);
		delete p;
	}
	return 0;
}

LRESULT CM576IlTestDlg::OnUiStatus(WPARAM, LPARAM lParam)
{
	CString* p = (CString*)lParam;
	if (p)
	{
		SetDlgItemText(IDC_IL_STATIC_STATUS, *p);
		delete p;
	}
	return 0;
}

LRESULT CM576IlTestDlg::OnUiRow(WPARAM, LPARAM lParam)
{
	IlUiRow* r = (IlUiRow*)lParam;
	if (!r)
		return 0;
	const int idx = m_list.InsertItem(m_list.GetItemCount(), r->channel);
	m_list.SetItemText(idx, 1, r->mpoPath);
	m_list.SetItemText(idx, 2, r->wl);
	CString t;
	t.Format(_T("%d"), r->lap);
	m_list.SetItemText(idx, 3, t);
	t.Format(_T("%d"), r->pd);
	m_list.SetItemText(idx, 4, t);
	t.Format(_T("%d"), r->opm);
	m_list.SetItemText(idx, 5, t);
	t.Format(_T("%.4f"), r->il);
	m_list.SetItemText(idx, 6, t);
	t.Format(_T("%.4f"), r->mx);
	m_list.SetItemText(idx, 7, t);
	t.Format(_T("%.4f"), r->mn);
	m_list.SetItemText(idx, 8, t);
	t.Format(_T("%.4f"), r->span);
	m_list.SetItemText(idx, 9, t);
	m_list.SetItemText(idx, 10, r->result);
	m_list.EnsureVisible(idx, FALSE);
	delete r;
	return 0;
}

LRESULT CM576IlTestDlg::OnUiFinished(WPARAM wParam, LPARAM lParam)
{
	if (m_worker.joinable())
		m_worker.join();
	m_running = false;
	SetControlsRunning(FALSE);
	if (m_pOwner)
		m_pOwner->EndIlTestSession();

	CString msg;
	msg.Format(_T("IL Test finished (stopped=%d, laps=%d)."), (int)wParam, (int)lParam);
	AppendLogLine(msg);
	SetDlgItemText(IDC_IL_STATIC_STATUS, msg);
	return 0;
}
