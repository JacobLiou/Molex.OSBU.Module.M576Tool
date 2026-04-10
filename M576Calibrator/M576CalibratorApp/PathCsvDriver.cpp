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
		if (line.Find(_T("target_index")) >= 0 && line.Find(_T("p1b")) >= 0)
			continue;
		if (lineNo == 1 && (line.Find(_T("target")) >= 0 || line.Find(_T("Target")) >= 0
			|| line.Find(_T("TARGET")) >= 0))
			continue;

		int vals[9] = {};
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
				if (n >= 9)
					break;
			}
			if (p < 0)
				break;
			start = p + 1;
		}
		if (n < 9)
		{
			errMsg.Format(_T("Line %d: expected 9 integer fields, got %d."), lineNo, n);
			return FALSE;
		}

		SPathStep s;
		s.targetSwitchIndex = vals[0];
		s.p1b = vals[1]; s.p1c = vals[2];
		s.p2b = vals[3]; s.p2c = vals[4];
		s.p3b = vals[5]; s.p3c = vals[6];
		s.p4b = vals[7]; s.p4c = vals[8];
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
	struct Seg
	{
		int segOrder;
		int b;
		int c;
	} segs[] = {
		{ 1, s.p1b, s.p1c },
		{ 2, s.p2b, s.p2c },
		{ 3, s.p3b, s.p3c },
		{ 4, s.p4b, s.p4c },
	};
	for (int i = 0; i < 4; ++i)
	{
		bool is1x64 = (segs[i].segOrder == 1 || segs[i].segOrder == 4);
		if (is1x64)
		{
			if (segs[i].c < 1 || segs[i].c > 64)
			{
				errMsg.Format(_T("1x64 segment %d: channel %d out of 1..64"), segs[i].segOrder, segs[i].c);
				return FALSE;
			}
		}
		else
		{
			if (segs[i].c < 1 || segs[i].c > 18)
			{
				errMsg.Format(_T("MCS segment %d: channel %d out of 1..18"), segs[i].segOrder, segs[i].c);
				return FALSE;
			}
		}
		if (segs[i].b < 1 || segs[i].b > 2)
		{
			errMsg.Format(_T("Segment %d: block %d out of 1..2"), segs[i].segOrder, segs[i].b);
			return FALSE;
		}
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
