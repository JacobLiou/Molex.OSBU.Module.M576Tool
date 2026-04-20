#include "stdafx.h"
#include "PathCsvDriver.h"

BOOL LoadPathCsv(LPCTSTR szPath, CArray<SPathStep, SPathStep const&>& steps, CString& errMsg)
{
	steps.RemoveAll();
	CStdioFile f;
	if (!f.Open(szPath, CFile::modeRead | CFile::typeText))
	{
		errMsg.Format(_T("Cannot open CSV: %s"), szPath);
		return FALSE;
	}
	CString line;
	int lineNo = 0;
	while (f.ReadString(line))
	{
		lineNo++;
		line.Trim();
		if (line.IsEmpty() || line[0] == _T('#'))
			continue;
		/// Skip CSV header row (allows leading # comments; tolerates UTF-8 BOM on first column).
		if (line.Find(_T("target_index")) >= 0
			&& (line.Find(_T("ch1")) >= 0 || line.Find(_T(",c1,")) >= 0 || line.Find(_T("p1b")) >= 0))
			continue;
		if (lineNo == 1 && (line.Find(_T("target")) >= 0 || line.Find(_T("Target")) >= 0
			|| line.Find(_T("TARGET")) >= 0))
			continue;

		int vals[5] = {};
		int n = 0;
		int start = 0;
		for (;;)
		{
			int p = line.Find(_T(','), start);
			CString t = (p < 0) ? line.Mid(start) : line.Mid(start, p - start);
			t.Trim();
			if (!t.IsEmpty())
			{
				vals[n++] = _ttoi(t);
				if (n >= 5)
					break;
			}
			if (p < 0)
				break;
			start = p + 1;
		}
		if (n < 5)
		{
			errMsg.Format(_T("Line %d: expected 5 integer fields (target_index,c1,c2,c3,c4), got %d."), lineNo, n);
			return FALSE;
		}

		SPathStep s;
		s.targetSwitchIndex = vals[0];
		s.c1 = vals[1];
		s.c2 = vals[2];
		s.c3 = vals[3];
		s.c4 = vals[4];
		steps.Add(s);
	}
	f.Close();
	if (steps.GetSize() == 0)
	{
		errMsg = _T("CSV has no data rows.");
		return FALSE;
	}
	return TRUE;
}

BOOL ValidatePathStep(const SPathStep& s, CString& errMsg)
{
	if (s.targetSwitchIndex < 1 || s.targetSwitchIndex > 6)
	{
		errMsg.Format(_T("target_index %d out of 1..6"), s.targetSwitchIndex);
		return FALSE;
	}
	if (s.c1 < 1 || s.c1 > 64 || s.c4 < 1 || s.c4 > 64)
	{
		errMsg.Format(_T("1x64 channel out of 1..64 (c1=%d c4=%d)"), s.c1, s.c4);
		return FALSE;
	}
	if (s.c2 < 1 || s.c2 > 18 || s.c3 < 1 || s.c3 > 18)
	{
		errMsg.Format(_T("MCS channel out of 1..18 (c2=%d c3=%d)"), s.c2, s.c3);
		return FALSE;
	}
	return TRUE;
}

BOOL LoadPathCsvPd(LPCTSTR szPath, CArray<SPathStepPd, SPathStepPd const&>& steps, CString& errMsg)
{
	steps.RemoveAll();
	CStdioFile f;
	if (!f.Open(szPath, CFile::modeRead | CFile::typeText))
	{
		errMsg.Format(_T("Cannot open CSV: %s"), szPath);
		return FALSE;
	}
	CString line;
	int lineNo = 0;
	while (f.ReadString(line))
	{
		lineNo++;
		line.Trim();
		if (line.IsEmpty() || line[0] == _T('#'))
			continue;
		/// Skip CSV header row (tolerates UTF-8 BOM on first column) — new PD schema: target_index,ch1,ch2.
		if (line.Find(_T("target_index")) >= 0
			&& (line.Find(_T("ch1")) >= 0 || line.Find(_T(",c1,")) >= 0))
			continue;
		if (lineNo == 1 && (line.Find(_T("target")) >= 0 || line.Find(_T("Target")) >= 0
			|| line.Find(_T("TARGET")) >= 0))
			continue;

		int vals[3] = {};
		int n = 0;
		int start = 0;
		for (;;)
		{
			int p = line.Find(_T(','), start);
			CString t = (p < 0) ? line.Mid(start) : line.Mid(start, p - start);
			t.Trim();
			if (!t.IsEmpty())
			{
				vals[n++] = _ttoi(t);
				if (n >= 3)
					break;
			}
			if (p < 0)
				break;
			start = p + 1;
		}
		if (n < 3)
		{
			errMsg.Format(_T("Line %d: PD CSV expected 3 integer fields (target_index,ch1,ch2), got %d."), lineNo, n);
			return FALSE;
		}

		SPathStepPd s;
		s.targetSwitchIndex = vals[0];
		s.ch1 = vals[1];
		s.ch2 = vals[2];
		steps.Add(s);
	}
	f.Close();
	if (steps.GetSize() == 0)
	{
		errMsg = _T("CSV has no data rows.");
		return FALSE;
	}
	return TRUE;
}

BOOL ValidatePathStepPd(const SPathStepPd& s, CString& errMsg)
{
	if (s.targetSwitchIndex < 1 || s.targetSwitchIndex > 4)
	{
		errMsg.Format(_T("PD target_index %d out of 1..4"), s.targetSwitchIndex);
		return FALSE;
	}
	if (s.ch1 < 1 || s.ch1 > 64)
	{
		errMsg.Format(_T("PD ch1 (2#1x64 ch) %d out of 1..64"), s.ch1);
		return FALSE;
	}
	if (s.ch2 < 1 || s.ch2 > 18)
	{
		errMsg.Format(_T("PD ch2 (MCS ch) %d out of 1..18"), s.ch2);
		return FALSE;
	}
	/// target=3 (1# MCS) only reachable via 2#1x64 ch 1..32; target=4 (2# MCS) via ch 33..64.
	/// target=1/2 (2#1x64 Stage_1/2) do not constrain the half — Stage is implied by target alone.
	if (s.targetSwitchIndex == 3 && s.ch1 > 32)
	{
		errMsg.Format(_T("PD routing: target_index 3 (1# MCS) requires ch1 in 1..32, got %d."), s.ch1);
		return FALSE;
	}
	if (s.targetSwitchIndex == 4 && s.ch1 < 33)
	{
		errMsg.Format(_T("PD routing: target_index 4 (2# MCS) requires ch1 in 33..64, got %d."), s.ch1);
		return FALSE;
	}
	return TRUE;
}
