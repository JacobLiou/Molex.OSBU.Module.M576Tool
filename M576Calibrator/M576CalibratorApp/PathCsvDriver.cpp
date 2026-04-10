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
		if (line.Find(_T("target_index")) >= 0 && line.Find(_T("p2b")) >= 0)
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
			errMsg.Format(_T("Line %d: PD CSV expected 5 integer fields (target_index,p2b,p2c,p3b,p3c), got %d."), lineNo, n);
			return FALSE;
		}

		SPathStepPd s;
		s.targetSwitchIndex = vals[0];
		s.p2b = vals[1];
		s.p2c = vals[2];
		s.p3b = vals[3];
		s.p3c = vals[4];
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
	if (s.p2b < 1 || s.p2b > 2)
	{
		errMsg.Format(_T("2#1x64 block %d out of 1..2 (stage)"), s.p2b);
		return FALSE;
	}
	if (s.p2c < 1 || s.p2c > 64)
	{
		errMsg.Format(_T("2#1x64 channel %d out of 1..64"), s.p2c);
		return FALSE;
	}
	if (s.p3b < 1 || s.p3b > 2)
	{
		errMsg.Format(_T("MCS block %d out of 1..2 (1# / 2# MCS)"), s.p3b);
		return FALSE;
	}
	if (s.p3c < 1 || s.p3c > 18)
	{
		errMsg.Format(_T("MCS channel %d out of 1..18"), s.p3c);
		return FALSE;
	}
	const int mcsSide = (s.p2c <= 32) ? 1 : 2;
	if (s.p3b != mcsSide)
	{
		errMsg.Format(_T("PD routing: 2#1x64 ch %d implies MCS side %d, but p3b=%d."),
			s.p2c, mcsSide, s.p3b);
		return FALSE;
	}
	if (s.targetSwitchIndex == 1 && s.p2b != 1)
	{
		errMsg = _T("target_index 1 (2#1x64 Stage_1) requires p2b=1.");
		return FALSE;
	}
	if (s.targetSwitchIndex == 2 && s.p2b != 2)
	{
		errMsg = _T("target_index 2 (2#1x64 Stage_2) requires p2b=2.");
		return FALSE;
	}
	if (s.targetSwitchIndex == 3 && s.p3b != 1)
	{
		errMsg = _T("target_index 3 (1# MCS) requires p3b=1.");
		return FALSE;
	}
	if (s.targetSwitchIndex == 4 && s.p3b != 2)
	{
		errMsg = _T("target_index 4 (2# MCS) requires p3b=2.");
		return FALSE;
	}
	return TRUE;
}
