#include "stdafx.h"
#include "Pm1x64Mapping.h"

BOOL Pm1x64ResolveMappingPath(LPCTSTR pmCsvPath, CString& outMappingPath)
{
	outMappingPath.Empty();
	if (pmCsvPath == NULL || pmCsvPath[0] == 0)
		return FALSE;
	CString p(pmCsvPath);
	int dot = p.ReverseFind(_T('.'));
	if (dot < 0)
		return FALSE;
	outMappingPath = p.Left(dot) + _T("Mapping.csv");
	return !outMappingPath.IsEmpty();
}

BOOL LoadPm1x64MappingCsv(
	LPCTSTR szPath,
	CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&>& rows,
	CString& errMsg)
{
	rows.RemoveAll();
	CStdioFile f;
	if (!f.Open(szPath, CFile::modeRead | CFile::typeText))
	{
		errMsg.Format(_T("Cannot open mapping CSV: %s"), szPath);
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
		if (line.Find(_T("target_index")) >= 0)
			continue;

		int vals[7] = {};
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
				if (n >= 7)
					break;
			}
			if (p < 0)
				break;
			start = p + 1;
		}
		if (n < 7)
		{
			errMsg.Format(_T("Mapping line %d: expected 7 integer fields, got %d."), lineNo, n);
			return FALSE;
		}
		if (vals[5] < 1 || vals[5] > 4 || vals[6] < 1 || vals[6] > 17)
		{
			errMsg.Format(
				_T("Mapping line %d: SW_x must be 1..4 and CH_y 1..17 (got SW=%d CH_y=%d)."),
				lineNo,
				vals[5],
				vals[6]);
			return FALSE;
		}

		SMems1x64PmMapRow r;
		r.targetSwitchIndex = vals[0];
		r.c1 = vals[1];
		r.c2 = vals[2];
		r.c3 = vals[3];
		r.c4 = vals[4];
		r.sw1to4 = vals[5];
		r.chY1based = vals[6];
		rows.Add(r);
	}
	f.Close();
	if (rows.GetSize() == 0)
	{
		errMsg = _T("Mapping CSV has no data rows.");
		return FALSE;
	}
	return TRUE;
}

BOOL ValidatePmStepsAgainstMapping(
	const CArray<SPathStep, SPathStep const&>& steps,
	const CArray<SMems1x64PmMapRow, SMems1x64PmMapRow const&>& mapRows,
	CString& errMsg)
{
	const int ns = (int)steps.GetSize();
	const int nm = (int)mapRows.GetSize();
	if (ns != nm)
	{
		errMsg.Format(_T("PM path rows (%d) != mapping rows (%d)."), ns, nm);
		return FALSE;
	}
	for (int i = 0; i < ns; ++i)
	{
		const SPathStep& s = steps[i];
		const SMems1x64PmMapRow& m = mapRows[i];
		if (s.targetSwitchIndex != m.targetSwitchIndex || s.c1 != m.c1 || s.c2 != m.c2 || s.c3 != m.c3
			|| s.c4 != m.c4)
		{
			errMsg.Format(
				_T("PM row %d does not match mapping first 5 columns (target %d,%d,%d,%d,%d vs map %d,%d,%d,%d,%d)."),
				i + 1,
				s.targetSwitchIndex,
				s.c1,
				s.c2,
				s.c3,
				s.c4,
				m.targetSwitchIndex,
				m.c1,
				m.c2,
				m.c3,
				m.c4);
			return FALSE;
		}
	}
	return TRUE;
}
