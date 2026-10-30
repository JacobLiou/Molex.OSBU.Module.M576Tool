#include "stdafx.h"
#include "Pd1x64Mapping.h"
// Pd1x64Mapping.cpp：PD 路径与 pd_1x64_*Mapping.csv 行对齐；解析 SW/CH_y 供 2#1x64 MEMS 低温 DAC 写入。

BOOL Pd1x64ResolveMappingPath(LPCTSTR pdCsvPath, CString& outMappingPath)
{
	outMappingPath.Empty();
	if (pdCsvPath == NULL || pdCsvPath[0] == 0)
		return FALSE;
	CString p(pdCsvPath);
	const int dot = p.ReverseFind(_T('.'));
	if (dot < 0)
		return FALSE;
	outMappingPath = p.Left(dot) + _T("Mapping.csv");
	return !outMappingPath.IsEmpty();
}

BOOL LoadPd1x64MappingCsv(
	LPCTSTR szPath,
	CArray<SMems1x64PdMapRow, SMems1x64PdMapRow const&>& rows,
	CString& errMsg)
{
	rows.RemoveAll();
	CStdioFile f;
	if (!f.Open(szPath, CFile::modeRead | CFile::typeText))
	{
		errMsg.Format(_T("Cannot open PD mapping CSV: %s"), szPath);
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

		int vals[5] = {};
		int n = 0;
		int start = 0;
		for (;;)
		{
			const int p = line.Find(_T(','), start);
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
			errMsg.Format(_T("PD mapping line %d: expected 5 integer fields, got %d."), lineNo, n);
			return FALSE;
		}
		if (vals[3] < 1 || vals[3] > 4 || vals[4] < 1 || vals[4] > 17)
		{
			errMsg.Format(
				_T("PD mapping line %d: SW_x must be 1..4 and CH_y 1..17 (got SW=%d CH_y=%d)."),
				lineNo,
				vals[3],
				vals[4]);
			return FALSE;
		}

		SMems1x64PdMapRow r;
		r.targetSwitchIndex = vals[0];
		r.ch1 = vals[1];
		r.ch2 = vals[2];
		r.sw1to4 = vals[3];
		r.chY1based = vals[4];
		rows.Add(r);
	}
	f.Close();
	if (rows.GetSize() == 0)
	{
		errMsg = _T("PD mapping CSV has no data rows.");
		return FALSE;
	}
	return TRUE;
}

BOOL ValidatePdStepsAgainstMapping(
	const CArray<SPathStepPd, SPathStepPd const&>& steps,
	const CArray<SMems1x64PdMapRow, SMems1x64PdMapRow const&>& mapRows,
	CString& errMsg)
{
	const int ns = (int)steps.GetSize();
	const int nm = (int)mapRows.GetSize();
	if (ns != nm)
	{
		errMsg.Format(_T("PD path rows (%d) != mapping rows (%d)."), ns, nm);
		return FALSE;
	}
	for (int i = 0; i < ns; ++i)
	{
		const SPathStepPd& s = steps[i];
		const SMems1x64PdMapRow& m = mapRows[i];
		if (s.targetSwitchIndex != m.targetSwitchIndex || s.ch1 != m.ch1 || s.ch2 != m.ch2)
		{
			errMsg.Format(
				_T("PD row %d does not match mapping first 3 columns (target %d,%d,%d vs map %d,%d,%d)."),
				i + 1,
				s.targetSwitchIndex,
				s.ch1,
				s.ch2,
				m.targetSwitchIndex,
				m.ch1,
				m.ch2);
			return FALSE;
		}
	}
	return TRUE;
}
