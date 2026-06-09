#include "stdafx.h"
#include "CalibPathOutcome.h"

#include <atlconv.h>

static void AppendCsvFieldA(CStringA& line, LPCSTR s)
{
	if (s == NULL)
		s = "";
	const bool needQuote = (std::strchr(s, ',') != NULL || std::strchr(s, '"') != NULL);
	if (needQuote)
	{
		line += '"';
		for (const char* p = s; *p; ++p)
		{
			if (*p == '"')
				line += "\"\"";
			else
				line += *p;
		}
		line += '"';
	}
	else
		line += s;
}

BOOL WriteRunPathFailureCsv(LPCTSTR path, const SRunPathSummary& summary, CString& err)
{
	if (path == NULL || path[0] == 0)
	{
		err = _T("Path empty");
		return FALSE;
	}
	CFile f;
	if (!f.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		err = _T("Cannot create file");
		return FALSE;
	}
	{
		const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
		f.Write(bom, 3);
	}
	auto writeA = [&f](LPCSTR s) {
		f.Write(s, (UINT)strlen(s));
		f.Write("\r\n", 2);
	};
	writeA("# Run Path failure/skip rows only (success steps omitted)");
	const char* hdr =
		"cal_mode,result,category,trans_slot,csv_file,line,route,fail_stage,peak_code,peak_attempts,cross_round,"
		"baseY,baseX,offsetY,offsetX,samplesY,samplesX,peak_dbm,lo_dbm,hi_dbm,pm_range,t_peakY,t_peakX,detail";
	writeA(hdr);
	const CStringA modeA(summary.isPm ? "PM" : "PD");
	for (size_t i = 0; i < summary.failureRows.size(); ++i)
	{
		const SCalibPathStepOutcome& r = summary.failureRows[i];
		CStringA line;
		AppendCsvFieldA(line, modeA);
		line += ',';
		AppendCsvFieldA(line, CalibPathStepResultLabelA(r.result));
		line += ',';
		AppendCsvFieldA(line, CalibPathFailCategoryLabelA(r.failCategory));
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.fileSlot + 1);
			line += t;
		}
		line += ',';
		AppendCsvFieldA(line, r.csvPath.c_str());
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.pathLine1Based);
			line += t;
		}
		line += ',';
		AppendCsvFieldA(line, r.routeLabel.c_str());
		line += ',';
		AppendCsvFieldA(line, r.failStage.c_str());
		line += ',';
		AppendCsvFieldA(line, r.peakCodeText.c_str());
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.peakAttempts);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%d", r.crossRound);
			line += t;
		}
		line += ',';
		{
			CStringA t;
			t.Format("%d,%d,%d,%d,%d,%d,%.6g,%.6g,%.6g,%d,%.6g,%.6g",
				r.lastBaseY,
				r.lastBaseX,
				r.lastOffsetY,
				r.lastOffsetX,
				r.sampleCountY,
				r.sampleCountX,
				r.peakDbm,
				r.loDbm,
				r.hiDbm,
				r.pmRangeIndex,
				r.hasTPeakY ? r.tPeakY : 0.0,
				r.hasTPeakX ? r.tPeakX : 0.0);
			line += t;
		}
		line += ',';
		AppendCsvFieldA(line, r.commDetail.c_str());
		writeA(line);
	}
	f.Close();
	return TRUE;
}
