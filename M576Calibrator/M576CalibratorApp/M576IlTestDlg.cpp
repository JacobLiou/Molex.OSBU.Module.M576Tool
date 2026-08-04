#include "stdafx.h"
#include "M576Calibrator.h"
#include "M576IlTestDlg.h"
#include "M576CalibratorDlg.h"
#include "IlTestCsv.h"
#include "CalibConstants.h"
#include "DiagnosisSession.h"
#include "M576TempMonitor.h"
#include "resource.h"

#include <commctrl.h>
#include <algorithm>
#include <vector>

namespace
{
constexpr UINT WM_M576_IL_LOG = WM_APP + 210;
constexpr UINT WM_M576_IL_ROW_FLUSH = WM_APP + 211;
constexpr UINT WM_M576_IL_STATUS = WM_APP + 212;
constexpr UINT WM_M576_IL_FINISHED = WM_APP + 213;
constexpr UINT WM_M576_IL_TEMPS = WM_APP + 214;

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
	if (m_pOwner)
		m_pOwner->SetActiveIlTestDlg(NULL);
}

void CM576IlTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IL_LIST, m_list);
	DDX_Control(pDX, IDC_IL_LIST_SEL_CH, m_listSelCh);
	DDX_Control(pDX, IDC_IL_EDIT_LOG, m_editLog);
}

BEGIN_MESSAGE_MAP(CM576IlTestDlg, CDialogEx)
	ON_BN_CLICKED(IDC_IL_BTN_START, &CM576IlTestDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_IL_BTN_STOP, &CM576IlTestDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_IL_RADIO_MODE_FULL, &CM576IlTestDlg::OnBnClickedModeFull)
	ON_BN_CLICKED(IDC_IL_RADIO_MODE_SEL, &CM576IlTestDlg::OnBnClickedModeSel)
	ON_BN_CLICKED(IDC_IL_BTN_SEL_ADD, &CM576IlTestDlg::OnBnClickedSelAdd)
	ON_BN_CLICKED(IDC_IL_BTN_SEL_REMOVE, &CM576IlTestDlg::OnBnClickedSelRemove)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_IL_LIST, &CM576IlTestDlg::OnGetDispInfo)
	ON_MESSAGE(WM_M576_IL_LOG, &CM576IlTestDlg::OnUiLog)
	ON_MESSAGE(WM_M576_IL_ROW_FLUSH, &CM576IlTestDlg::OnUiRowFlush)
	ON_MESSAGE(WM_M576_IL_STATUS, &CM576IlTestDlg::OnUiStatus)
	ON_MESSAGE(WM_M576_IL_TEMPS, &CM576IlTestDlg::OnUiTemps)
	ON_MESSAGE(WM_M576_IL_FINISHED, &CM576IlTestDlg::OnUiFinished)
END_MESSAGE_MAP()

BOOL CM576IlTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (m_pOwner)
		m_pOwner->SetActiveIlTestDlg(this);
	CheckRadioButton(IDC_IL_RADIO_SFP1550, IDC_IL_RADIO_LASER1310, IDC_IL_RADIO_LASER1310);
	CheckRadioButton(IDC_IL_RADIO_MODE_FULL, IDC_IL_RADIO_MODE_SEL, IDC_IL_RADIO_MODE_FULL);
	CString thr;
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_ABS_MIN_DB);
	SetDlgItemText(IDC_IL_EDIT_ABS_MIN, thr);
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_ABS_MAX_DB);
	SetDlgItemText(IDC_IL_EDIT_ABS_MAX, thr);
	thr.Format(_T("%.2f"), (double)M576_IL_TEST_DEFAULT_SPAN_MAX_DB);
	SetDlgItemText(IDC_IL_EDIT_SPAN_MAX, thr);
	SetDlgItemText(IDC_IL_STATIC_STATUS,
		_T("Idle - Full=CSV 576; Selected=suspect CH list only."));

	// Virtual list: one lap (Full ~576 / Selected N) only.
	m_list.ModifyStyle(0, LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_list.InsertColumn(0, _T("Channel"), LVCFMT_LEFT, 70);
	m_list.InsertColumn(1, _T("Half"), LVCFMT_LEFT, 45);
	m_list.InsertColumn(2, _T("MPO Path"), LVCFMT_LEFT, 160);
	m_list.InsertColumn(3, _T("Wl"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(4, _T("Lap"), LVCFMT_RIGHT, 45);
	m_list.InsertColumn(5, _T("PD"), LVCFMT_RIGHT, 75);
	m_list.InsertColumn(6, _T("OPM"), LVCFMT_RIGHT, 75);
	m_list.InsertColumn(7, _T("IL"), LVCFMT_RIGHT, 75);
	m_list.InsertColumn(8, _T("Max"), LVCFMT_RIGHT, 75);
	m_list.InsertColumn(9, _T("Min"), LVCFMT_RIGHT, 75);
	m_list.InsertColumn(10, _T("Span"), LVCFMT_RIGHT, 75);
	m_list.InsertColumn(11, _T("Result"), LVCFMT_RIGHT, 70);

	if (CWnd* p = GetDlgItem(IDC_IL_BTN_STOP))
		p->EnableWindow(FALSE);
	SetDlgItemText(IDC_IL_STATIC_ELAPSED, _T("00:00:00"));
	UpdateModeControlsEnabled();
	return TRUE;
}

namespace
{
constexpr UINT_PTR kIlHangupTimerId = 1;
}

void CM576IlTestDlg::UpdateHangupClockText()
{
	ULONGLONG sec = 0;
	if (m_hangupStartTick != 0)
	{
		const ULONGLONG now = GetTickCount64();
		sec = (now >= m_hangupStartTick) ? ((now - m_hangupStartTick) / 1000ull) : 0;
	}
	const unsigned h = (unsigned)(sec / 3600ull);
	const unsigned m = (unsigned)((sec % 3600ull) / 60ull);
	const unsigned s = (unsigned)(sec % 60ull);
	CString t;
	t.Format(_T("%02u:%02u:%02u"), h, m, s);
	SetDlgItemText(IDC_IL_STATIC_ELAPSED, t);
}

void CM576IlTestDlg::StartHangupClock()
{
	StopHangupClock();
	m_hangupStartTick = GetTickCount64();
	UpdateHangupClockText();
	SetTimer(kIlHangupTimerId, 1000, NULL);
	m_hangupTimerOn = true;
}

void CM576IlTestDlg::StopHangupClock()
{
	if (m_hangupTimerOn)
	{
		KillTimer(kIlHangupTimerId);
		m_hangupTimerOn = false;
	}
	UpdateHangupClockText();
}

void CM576IlTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == kIlHangupTimerId)
		UpdateHangupClockText();
	CDialogEx::OnTimer(nIDEvent);
}

IlTestWlKind CM576IlTestDlg::SelectedWl() const
{
	if (IsDlgButtonChecked(IDC_IL_RADIO_SFP1550) == BST_CHECKED)
		return IlTestWlKind::Sfp1550;
	if (IsDlgButtonChecked(IDC_IL_RADIO_SFP1310) == BST_CHECKED)
		return IlTestWlKind::Sfp1310;
	return IlTestWlKind::Laser1310;
}

BOOL CM576IlTestDlg::IsSelectedChannelsMode() const
{
	return IsDlgButtonChecked(IDC_IL_RADIO_MODE_SEL) == BST_CHECKED;
}

void CM576IlTestDlg::UpdateModeControlsEnabled()
{
	const BOOL sel = IsSelectedChannelsMode();
	const BOOL idle = !m_running.load();
	if (CWnd* p = GetDlgItem(IDC_IL_EDIT_SEL_CH))
		p->EnableWindow(idle && sel);
	if (CWnd* p = GetDlgItem(IDC_IL_BTN_SEL_ADD))
		p->EnableWindow(idle && sel);
	if (CWnd* p = GetDlgItem(IDC_IL_BTN_SEL_REMOVE))
		p->EnableWindow(idle && sel);
	m_listSelCh.EnableWindow(idle && sel);
}

BOOL CM576IlTestDlg::CollectSelectedChannelRows(std::vector<M576DiagnosisRow>& rows, CString& err) const
{
	err.Empty();
	rows.clear();
	const int n = m_listSelCh.GetCount();
	if (n <= 0)
	{
		err = _T("Selected mode: add at least one channel (CH 1..576).");
		return FALSE;
	}
	std::vector<int> chs;
	chs.reserve((size_t)n);
	for (int i = 0; i < n; ++i)
	{
		CString item;
		m_listSelCh.GetText(i, item);
		const int ch = IlTestParseChannelIndex(item);
		if (ch < 1)
		{
			err.Format(_T("Selected mode: invalid list item '%s'."), item.GetString());
			return FALSE;
		}
		chs.push_back(ch);
	}
	std::sort(chs.begin(), chs.end());
	chs.erase(std::unique(chs.begin(), chs.end()), chs.end());
	rows.reserve(chs.size());
	for (size_t i = 0; i < chs.size(); ++i)
	{
		M576DiagnosisRow row;
		CString buildErr;
		if (!IlTestBuildDiagnosisRowFromChannel(chs[i], row, buildErr))
		{
			err = buildErr;
			rows.clear();
			return FALSE;
		}
		rows.push_back(row);
	}
	return TRUE;
}

void CM576IlTestDlg::OnBnClickedModeFull()
{
	UpdateModeControlsEnabled();
}

void CM576IlTestDlg::OnBnClickedModeSel()
{
	UpdateModeControlsEnabled();
}

void CM576IlTestDlg::OnBnClickedSelAdd()
{
	if (m_running.load() || !IsSelectedChannelsMode())
		return;
	CString text;
	GetDlgItemText(IDC_IL_EDIT_SEL_CH, text);
	text.Trim();
	const int ch = IlTestParseChannelIndex(text);
	if (ch < 1)
	{
		MessageBox(_T("Enter a channel number 1..576 (or CH123)."), _T("IL Test"), MB_OK | MB_ICONWARNING);
		return;
	}
	CString label;
	label.Format(_T("CH%d"), ch);
	for (int i = 0; i < m_listSelCh.GetCount(); ++i)
	{
		CString exist;
		m_listSelCh.GetText(i, exist);
		if (exist.CompareNoCase(label) == 0)
			return; // duplicate ignore
	}
	// Insert sorted by channel index.
	int insertAt = m_listSelCh.GetCount();
	for (int i = 0; i < m_listSelCh.GetCount(); ++i)
	{
		CString exist;
		m_listSelCh.GetText(i, exist);
		if (IlTestParseChannelIndex(exist) > ch)
		{
			insertAt = i;
			break;
		}
	}
	m_listSelCh.InsertString(insertAt, label);
	SetDlgItemText(IDC_IL_EDIT_SEL_CH, _T(""));
}

void CM576IlTestDlg::OnBnClickedSelRemove()
{
	if (m_running.load() || !IsSelectedChannelsMode())
		return;
	for (int i = m_listSelCh.GetCount() - 1; i >= 0; --i)
	{
		if (m_listSelCh.GetSel(i) > 0)
			m_listSelCh.DeleteString(i);
	}
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

double CM576IlTestDlg::ReadSpanMaxDb() const
{
	double v = 0.0;
	if (!ParseDoubleEdit(const_cast<CM576IlTestDlg*>(this), IDC_IL_EDIT_SPAN_MAX, v))
		return (double)M576_IL_TEST_DEFAULT_SPAN_MAX_DB;
	return v;
}

BOOL CM576IlTestDlg::ReadGateParams(IlTestGateParams& out, CString& err) const
{
	err.Empty();
	out.absIlMinDb = ReadAbsIlMinDb();
	out.absIlMaxDb = ReadAbsIlMaxDb();
	out.spanMaxDb = ReadSpanMaxDb();
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
	if (out.spanMaxDb < 0.0 || out.spanMaxDb > 100.0)
	{
		err = _T("Span max must be in [0, 100] dB.");
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
	if (CWnd* p = GetDlgItem(IDC_IL_EDIT_ABS_MIN))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_EDIT_ABS_MAX))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_EDIT_SPAN_MAX))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_RADIO_MODE_FULL))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_RADIO_MODE_SEL))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_BTN_START))
		p->EnableWindow(idle);
	if (CWnd* p = GetDlgItem(IDC_IL_BTN_STOP))
		p->EnableWindow(running);
	UpdateModeControlsEnabled();
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

	// Cap by line count — file already has full CommLog; UI is a short window.
	int lines = 1;
	for (int i = 0; i < cur.GetLength(); ++i)
	{
		if (cur[i] == _T('\n'))
			++lines;
	}
	if (lines > kMaxLogLines)
	{
		int drop = lines - kMaxLogLines;
		int pos = 0;
		while (drop > 0 && pos < cur.GetLength())
		{
			const int nl = cur.Find(_T('\n'), pos);
			if (nl < 0)
			{
				pos = cur.GetLength();
				break;
			}
			pos = nl + 1;
			--drop;
		}
		if (pos > 0 && pos < cur.GetLength())
			cur = cur.Mid(pos);
		else if (pos >= cur.GetLength())
			cur.Empty();
	}

	m_editLog.SetWindowText(cur);
	m_editLog.LineScroll(m_editLog.GetLineCount());
}

void CM576IlTestDlg::PostCommLogLine(LPCTSTR line)
{
	if (!line || !m_hWnd || !::IsWindow(m_hWnd))
		return;
	::PostMessage(m_hWnd, WM_M576_IL_LOG, 0, (LPARAM)new CString(line));
}

void CM576IlTestDlg::BeginLapUi(int channelCount, int lap)
{
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		m_channelCount = channelCount > 0 ? channelCount : 0;
		m_pendingLap = lap;
		m_pendingNewLap = true;
		m_pendingUiRows.assign((size_t)m_channelCount, M576IlTestUiRow());
	}
	if (!m_rowFlushScheduled.exchange(true))
	{
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_IL_ROW_FLUSH, 0, 0);
	}
}

void CM576IlTestDlg::QueueLapRow(int channelIndex0, M576IlTestUiRow row)
{
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		if (channelIndex0 < 0)
			return;
		if ((int)m_pendingUiRows.size() != m_channelCount)
			m_pendingUiRows.assign((size_t)m_channelCount, M576IlTestUiRow());
		if (channelIndex0 >= m_channelCount)
			return;
		m_pendingUiRows[(size_t)channelIndex0] = std::move(row);
	}
	if (!m_rowFlushScheduled.exchange(true))
	{
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_IL_ROW_FLUSH, 0, 0);
	}
}

void CM576IlTestDlg::FlushPendingUiRows()
{
	bool newLap = false;
	int lap = 0;
	int n = 0;
	std::vector<M576IlTestUiRow> batch;
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		newLap = m_pendingNewLap;
		lap = m_pendingLap;
		n = m_channelCount;
		batch = m_pendingUiRows; // copy slots (keep pending for further updates in same lap)
		m_pendingNewLap = false;
		m_rowFlushScheduled = false;
	}
	if (n <= 0 && batch.empty())
		return;
	if (newLap || (int)m_uiRows.size() != n)
	{
		m_uiLap = lap;
		m_uiRows.assign((size_t)n, M576IlTestUiRow());
		m_list.SetItemCountEx(n, 0);
	}
	const int count = (std::min)(n, (int)batch.size());
	for (int i = 0; i < count; ++i)
	{
		if (!batch[(size_t)i].channel.IsEmpty() || batch[(size_t)i].lap > 0)
			m_uiRows[(size_t)i] = batch[(size_t)i];
	}
	m_list.Invalidate(FALSE);
	if (n > 0)
	{
		// Follow the last filled channel in this lap.
		int last = 0;
		for (int i = n - 1; i >= 0; --i)
		{
			if (!m_uiRows[(size_t)i].channel.IsEmpty())
			{
				last = i;
				break;
			}
		}
		m_list.EnsureVisible(last, FALSE);
	}
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		if (m_pendingNewLap && !m_rowFlushScheduled.exchange(true))
		{
			if (m_hWnd && ::IsWindow(m_hWnd))
				::PostMessage(m_hWnd, WM_M576_IL_ROW_FLUSH, 0, 0);
		}
	}
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

	std::vector<M576DiagnosisRow> rows;
	const BOOL selectedMode = IsSelectedChannelsMode();
	if (selectedMode)
	{
		if (!CollectSelectedChannelRows(rows, err))
		{
			m_pOwner->EndIlTestSession();
			MessageBox(err.IsEmpty() ? _T("Selected channels invalid.") : err, _T("IL Test"), MB_OK | MB_ICONWARNING);
			return;
		}
	}
	else
	{
		const CString swCsvPath = outDir + _T("\\diagnosis_sw.csv");
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
	}

	const IlTestWlKind wl = SelectedWl();
	CString ui;
	ui.Format(_T("%.4f"), gate.absIlMinDb);
	SetDlgItemText(IDC_IL_EDIT_ABS_MIN, ui);
	ui.Format(_T("%.4f"), gate.absIlMaxDb);
	SetDlgItemText(IDC_IL_EDIT_ABS_MAX, ui);
	ui.Format(_T("%.4f"), gate.spanMaxDb);
	SetDlgItemText(IDC_IL_EDIT_SPAN_MAX, ui);

	if (m_worker.joinable())
		m_worker.join();
	{
		std::lock_guard<std::mutex> lock(m_uiRowMutex);
		m_uiRows.clear();
		m_pendingUiRows.clear();
		m_uiLap = 0;
		m_pendingLap = 0;
		m_pendingNewLap = false;
		m_channelCount = (int)rows.size();
		m_rowFlushScheduled = false;
	}
	m_list.SetItemCountEx(0, 0);
	m_editLog.SetWindowText(_T(""));
	m_stop = FALSE;
	m_running = true;
	SetControlsRunning(TRUE);
	StartHangupClock();

	const CString mcs1Sn = m_pOwner->GetMcs1SnSanitizedForFilename();
	const CString commLogPath = m_pOwner->GetIlTestCommLogPathAbs();
	if (selectedMode)
	{
		CString chList;
		for (size_t i = 0; i < rows.size(); ++i)
		{
			if (i)
				chList += _T(",");
			chList += CString(rows[i].channel.IsEmpty() ? rows[i].label : rows[i].channel);
		}
		CString modeMsg;
		modeMsg.Format(_T("[ILTEST] mode=Selected ch=%d: %s"), (int)rows.size(), chList.GetString());
		AppendLogLine(modeMsg);
	}
	else
	{
		AppendLogLine(_T("[ILTEST] mode=Full (diagnosis_sw.csv)"));
	}
	CString startMsg;
	startMsg.Format(
		_T("[ILTEST] start ch=%d %s SW3=1 %d SWL %d %d SW1 IN/OUT swap (odd/even lap) OPM4 AUTO IL=OPM-PD abs=[%.3f,%.3f] Span<=%.3f"),
		(int)rows.size(),
		IlTestWlLabel(wl),
		IlTestSw3Third(wl),
		IlTestSwlChannel(wl),
		IlTestWavelengthNm(wl),
		gate.absIlMinDb,
		gate.absIlMaxDb,
		gate.spanMaxDb);
	AppendLogLine(startMsg);
	AppendLogLine(_T("[ILTEST] Half: odd lap=IN (SW1 sw/sw+32), even lap=OUT (SW1 swapped); SW2 unchanged; Span per half."));
	AppendLogLine(_T("[ILTEST] CSV ") + M576GetIlTestLogCsvPath(outDir, mcs1Sn));
	AppendLogLine(_T("[ILTEST] Span CSV (on Stop) ") + M576GetIlTestSpanCsvPath(outDir, mcs1Sn));
	AppendLogLine(_T("[ILTEST] CommLog file ") + (commLogPath.IsEmpty() ? CString(_T("(none)")) : commLogPath));
	AppendLogLine(_T("[ILTEST] ListCtrl=latest lap; Log=last 800 lines (full file=ILTestCommLog)."));

	m_worker = std::thread([this, rows, outDir, wl, gate, mcs1Sn]() {
		WorkerEntry(rows, outDir, wl, gate, mcs1Sn);
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
	StopHangupClock();
	if (m_pOwner)
		m_pOwner->SetActiveIlTestDlg(NULL);
	CDialogEx::OnCancel();
}

void CM576IlTestDlg::OnOK()
{
}

void CM576IlTestDlg::WorkerEntry(
	std::vector<M576DiagnosisRow> rows,
	CString outDir,
	IlTestWlKind wl,
	IlTestGateParams gate,
	CString mcs1Sn)
{
	const int N = (int)rows.size();
	const CString csvPath = M576GetIlTestLogCsvPath(outDir, mcs1Sn);
	const CString wlLabel(IlTestWlLabel(wl));
	const int sw3Third = IlTestSw3Third(wl);
	const int swlCh = IlTestSwlChannel(wl); // FIM: same as 1x8 port
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
	auto PostTemps = [this](const CString& s) {
		if (m_hWnd && ::IsWindow(m_hWnd))
			::PostMessage(m_hWnd, WM_M576_IL_TEMPS, 0, (LPARAM)new CString(s));
	};
	auto RefreshTemps = [this, &PostLog, &PostTemps]() {
		if (!m_pOwner)
			return;
		M576FiveTemps temps;
		CString detail;
		(void)M576ReadAllFiveTempsC(m_pOwner->Device429f(), temps, detail);
		PostTemps(M576FormatFiveTempsUiLine(temps));
		CStringArray lines;
		M576AppendFiveTempsLogLines(temps, lines);
		for (int i = 0; i < lines.GetSize(); ++i)
			PostLog(lines[i]);
		if (!detail.IsEmpty())
			PostLog(_T("[TEMP] partial failures: ") + detail);
	};
	DWORD lastStatusTick = 0;

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

	// FIM-align source/WL once per run: SW 3 1 <port>, SWL <port> <nm>, OPM 4 1 0.
	// Half: odd=IN / even=OUT via SW1 1x64 port swap (not SW4).
	int armedSw3 = -1;
	int armedSwlCh = -1;
	int armedWlNm = -1;
	bool armedOpmAuto = false;

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
		const int half = IlTestHalfFromLap(fullLaps);
		const CString halfLabel(IlTestHalfLabel(half));
		firstCmd = true;
		BeginLapUi(N, fullLaps);
		{
			CString lapNote;
			lapNote.Format(_T("[ILTEST] --- lap %d begin half=%s SW1 %s (%d ch) ---"),
				fullLaps,
				halfLabel.GetString(),
				(half == 2) ? _T("swap(OUT)") : _T("normal(IN)"),
				N);
			PostLog(lapNote);
		}

		CDiagnosisSession* session = m_pOwner ? m_pOwner->GetDiagnosisSessionForIlTest() : NULL;
		if (session == NULL)
		{
			PostLog(_T("[ILTEST] serial session closed; aborting."));
			break;
		}

		// Temperature monitor: main MT + four sub-board box temps (does not abort on failure).
		if (!m_stop)
			RefreshTemps();
		if (m_stop)
			break;

		// Once per run: OPM AUTO.
		if (!armedOpmAuto)
		{
			MaybeDelay();
			if (m_stop)
				break;
			CStringA opm4Reply;
			DWORD ms = 0;
			CString errOpm4;
			(void)session->ExchangeAsciiLine(_T("OPM4"), CStringA("OPM 4 1 0"), opm4Reply, 3000, ms, errOpm4);
			armedOpmAuto = true;
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
			std::vector<CStringA> swCmds;
			if (chIdx >= 1)
			{
				CString buildErr;
				if (!IlTestBuildSwCommandsForChannelHalf(chIdx, half, swCmds, buildErr))
				{
					PostLog(buildErr.IsEmpty()
						? _T("IL Test: SW build failed.")
						: buildErr);
					swCmds = src.swCommands;
				}
			}
			else
			{
				swCmds = src.swCommands;
			}
			for (size_t k = 0; k < swCmds.size(); ++k)
			{
				if (m_stop)
					break;
				MaybeDelay();
				if (m_stop)
					break;
				CString labelSw;
				labelSw.Format(_T("SW %d.%d/%d"), i + 1, (int)k + 1, (int)swCmds.size());
				CStringA reply;
				DWORD ms = 0;
				const BOOL ok = session->ExchangeAsciiLine(labelSw, swCmds[k], reply, 3000, ms, err);
				if (!ok || reply.CompareNoCase("OK") != 0)
				{
					CString log;
					log.Format(_T("%s: SW fail reply=%hs"), chName.GetString(), reply.GetString());
					PostLog(log);
				}
			}
			if (m_stop)
				break;

			// FIM SetTestWL: SW 3 1 <port> (host; FIM driver currently stubs this) + SWL <port> <nm>.
			if (armedSw3 != sw3Third)
			{
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
				armedSw3 = sw3Third;
			}

			if (armedSwlCh != swlCh || armedWlNm != wlNm)
			{
				MaybeDelay();
				if (m_stop)
					break;
				{
					CStringA wire;
					FormatSwlWireLocal(wire, swlCh, wlNm);
					CStringA wlReply;
					DWORD ms = 0;
					CString label;
					label.Format(_T("SWL %d %d"), swlCh, wlNm);
					(void)session->ExchangeAsciiLine(label, wire, wlReply, 3000, ms, err);
				}
				armedSwlCh = swlCh;
				armedWlNm = wlNm;
			}

			MaybeDelay();
			if (m_stop)
				break;
			DelayMs(kPdPreMs);
			if (m_stop)
				break;

			// Board firmware expects `pd 1` (bare `PD` returns "Invalid data"). Scale /100.
			// OPM: `OPM 3 1` (/10000), same as FIM / Diagnosis.
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
				(void)session->ExchangeAsciiLine(_T("OPM"), CStringA("OPM 3 1"), opmReply, 3000, ms, err);
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
			const CString key = IlTestStatsKey(chName, wlLabel, halfLabel);
			IlTestRollingStats& st = stats[key];
			st.Add(il);
			const double span = st.Span();
			const BOOL pass = IlTestJudgePass(il, span, gate);

			M576IlTestLogRow crow;
			crow.timeStamp = NowTimeStamp();
			crow.lap = fullLaps;
			crow.half = halfLabel;
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
			crow.absIlMinDb = gate.absIlMinDb;
			crow.absIlMaxDb = gate.absIlMaxDb;
			crow.spanMaxDb = gate.spanMaxDb;
			CString csvErr;
			if (!M576AppendIlTestLogRow(csvPath, crow, csvErr))
			{
				CString log;
				log.Format(_T("CSV append failed: %s"), csvErr.GetString());
				PostLog(log);
			}

			M576IlTestUiRow uiRow;
			uiRow.channel = chName;
			uiRow.half = halfLabel;
			uiRow.mpoPath = mpoPath;
			uiRow.wl = wlLabel;
			uiRow.lap = fullLaps;
			uiRow.pd = pdRaw;
			uiRow.opm = opmRaw;
			uiRow.il = il;
			uiRow.mx = st.ilMax;
			uiRow.mn = st.ilMin;
			uiRow.span = span;
			uiRow.result = pass ? _T("PASS") : _T("FAIL");
			QueueLapRow(i, std::move(uiRow));

			{
				CString sum;
				sum.Format(
					_T("[IL] %s half=%s %s lap=%d IL=%.4f Span=%.4f %s PD=%d(%.2fdBm) OPM=%d(%.4fdBm)"),
					chName.GetString(), halfLabel.GetString(), mpoPath.GetString(), fullLaps, il, span,
					pass ? _T("PASS") : _T("FAIL"),
					pdRaw, IlTestPdDbm(pdRaw), opmRaw, IlTestOpmDbm(opmRaw));
				PostLog(sum);
				if (m_pOwner)
				{
					const CString commPath = m_pOwner->GetIlTestCommLogPathAbs();
					if (!commPath.IsEmpty())
					{
						CString ignore;
						(void)M576AppendIlTestCommLogLine(commPath, sum, ignore);
					}
				}
			}

			const DWORD now = ::GetTickCount();
			if (lastStatusTick == 0 || (now - lastStatusTick) >= 200)
			{
				lastStatusTick = now;
				CString status;
				status.Format(
					_T("Lap %d %s | %s %s | IL=%.4f Span=%.4f | %s"),
					fullLaps, halfLabel.GetString(), chName.GetString(), mpoPath.GetString(), il, span,
					pass ? _T("PASS") : _T("FAIL"));
				PostStatus(status);
			}
		}
	}

	// Stop / finish: one-row-per-channel×half Span summary for trend charts.
	{
		std::vector<M576IlTestSpanRow> spanRows;
		spanRows.reserve(stats.size());
		for (IlTestStatsMap::const_iterator it = stats.begin(); it != stats.end(); ++it)
		{
			const CString& key = it->first;
			const IlTestRollingStats& st = it->second;
			if (st.sampleCount <= 0)
				continue;

			// key = channel|wl|half
			CString channel = key;
			CString wl = wlLabel;
			CString halfPart = _T("IN");
			const int bar1 = key.Find(_T('|'));
			if (bar1 >= 0)
			{
				channel = key.Left(bar1);
				const CString rest = key.Mid(bar1 + 1);
				const int bar2 = rest.Find(_T('|'));
				if (bar2 >= 0)
				{
					wl = rest.Left(bar2);
					halfPart = rest.Mid(bar2 + 1);
				}
				else
					wl = rest;
			}

			M576IlTestSpanRow row;
			row.channel = channel;
			row.half = halfPart;
			row.wlLabel = wl;
			row.sampleCount = st.sampleCount;
			row.ilMax = st.ilMax;
			row.ilMin = st.ilMin;
			row.ilSpan = st.Span();
			const int chIdx = IlTestParseChannelIndex(channel);
			if (!IlTestChannelToMpoPorts(chIdx, row.inPort, row.outPort))
			{
				row.inPort = _T("-");
				row.outPort = _T("-");
			}
			spanRows.push_back(row);
		}

		if (!spanRows.empty())
		{
			const CString spanPath = M576GetIlTestSpanCsvPath(outDir, mcs1Sn);
			CString spanErr;
			if (M576WriteIlTestSpanCsv(spanPath, spanRows, spanErr))
			{
				CString log;
				log.Format(_T("[ILTEST] wrote %s (%u channel×half rows)"),
					spanPath.GetString(), (unsigned)spanRows.size());
				PostLog(log);
				if (m_pOwner)
				{
					const CString commPath = m_pOwner->GetIlTestCommLogPathAbs();
					if (!commPath.IsEmpty())
					{
						CString ignore;
						(void)M576AppendIlTestCommLogLine(commPath, log, ignore);
					}
				}
			}
			else
			{
				CString log;
				log.Format(_T("[ILTEST] Span CSV failed: %s"), spanErr.GetString());
				PostLog(log);
			}
		}
		else
		{
			PostLog(_T("[ILTEST] Span CSV skipped (no valid IL samples)."));
		}
	}

	if (m_hWnd && ::IsWindow(m_hWnd))
	{
		::PostMessage(m_hWnd, WM_M576_IL_ROW_FLUSH, 0, 0);
		::PostMessage(m_hWnd, WM_M576_IL_FINISHED, m_stop ? 1 : 0, (LPARAM)fullLaps);
	}
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

LRESULT CM576IlTestDlg::OnUiTemps(WPARAM, LPARAM lParam)
{
	CString* p = (CString*)lParam;
	if (p)
	{
		SetDlgItemText(IDC_IL_STATIC_TEMPS, *p);
		delete p;
	}
	return 0;
}

LRESULT CM576IlTestDlg::OnUiRowFlush(WPARAM, LPARAM)
{
	FlushPendingUiRows();
	return 0;
}

void CM576IlTestDlg::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDisp = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
	if (!pDisp || !(pDisp->item.mask & LVIF_TEXT) || !pDisp->item.pszText || pDisp->item.cchTextMax <= 0)
		return;
	const int i = pDisp->item.iItem;
	if (i < 0 || i >= (int)m_uiRows.size())
		return;
	const M576IlTestUiRow& r = m_uiRows[(size_t)i];
	CString s;
	switch (pDisp->item.iSubItem)
	{
	case 0: s = r.channel; break;
	case 1: s = r.half; break;
	case 2: s = r.mpoPath; break;
	case 3: s = r.wl; break;
	case 4: s.Format(_T("%d"), r.lap); break;
	case 5: s.Format(_T("%d"), r.pd); break;
	case 6: s.Format(_T("%d"), r.opm); break;
	case 7: s.Format(_T("%.4f"), r.il); break;
	case 8: s.Format(_T("%.4f"), r.mx); break;
	case 9: s.Format(_T("%.4f"), r.mn); break;
	case 10: s.Format(_T("%.4f"), r.span); break;
	case 11: s = r.result; break;
	default: s.Empty(); break;
	}
	_tcsncpy_s(pDisp->item.pszText, (size_t)pDisp->item.cchTextMax, s.GetString(), _TRUNCATE);
}

LRESULT CM576IlTestDlg::OnUiFinished(WPARAM wParam, LPARAM lParam)
{
	FlushPendingUiRows();
	if (m_worker.joinable())
		m_worker.join();
	m_running = false;
	StopHangupClock();
	SetControlsRunning(FALSE);
	if (m_pOwner)
		m_pOwner->EndIlTestSession();

	CString msg;
	msg.Format(_T("[ILTEST] finished stopped=%d laps=%d listRows=%d (latest lap only)"),
		(int)wParam, (int)lParam, (int)m_uiRows.size());
	AppendLogLine(msg);
	SetDlgItemText(IDC_IL_STATIC_STATUS, msg);
	return 0;
}
