#include "FullEditDacCsv.h"

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <sstream>

namespace {

std::string StripUtf8Bom(std::string s)
{
	if (s.size() >= 3
		&& (unsigned char)s[0] == 0xEF
		&& (unsigned char)s[1] == 0xBB
		&& (unsigned char)s[2] == 0xBF)
		s.erase(0, 3);
	return s;
}

std::vector<std::string> SplitCsvLine(const std::string& line)
{
	std::vector<std::string> cols;
	std::string cur;
	for (char c : line)
	{
		if (c == ',')
		{
			cols.push_back(cur);
			cur.clear();
		}
		else if (c != '\r')
			cur.push_back(c);
	}
	cols.push_back(cur);
	return cols;
}

bool ParseIntStrict(const std::string& s, int& out)
{
	out = 0;
	if (s.empty())
		return false;
	char* end = nullptr;
	const long v = std::strtol(s.c_str(), &end, 10);
	if (end == s.c_str() || (end && *end != '\0'))
		return false;
	out = (int)v;
	return true;
}

bool ParseOptInt(const std::string& s, int& out, bool& valid)
{
	valid = false;
	out = 0;
	if (s.empty())
		return true;
	if (!ParseIntStrict(s, out))
		return false;
	valid = true;
	return true;
}

bool TripleInRange(const FullEditTempDacTriple& t)
{
	const int vals[] = {
		t.lowY, t.lowX, t.roomY, t.roomX, t.highY, t.highX,
		t.tempPointLow, t.tempPointRoom, t.tempPointHigh
	};
	for (int v : vals)
	{
		if (!FullEditDacInInt16Range(v))
			return false;
	}
	return true;
}

bool TripleEqualDacs(const FullEditTempDacTriple& a, const FullEditTempDacTriple& b)
{
	return a.lowY == b.lowY && a.lowX == b.lowX
		&& a.roomY == b.roomY && a.roomX == b.roomX
		&& a.highY == b.highY && a.highX == b.highX;
}

bool TripleEqualTemps(const FullEditTempDacTriple& a, const FullEditTempDacTriple& b)
{
	return a.tempPointLow == b.tempPointLow
		&& a.tempPointRoom == b.tempPointRoom
		&& a.tempPointHigh == b.tempPointHigh;
}

int ColIndex(const std::vector<std::string>& header, const char* name)
{
	for (size_t i = 0; i < header.size(); ++i)
	{
		if (header[i] == name)
			return (int)i;
	}
	return -1;
}

bool GetCol(const std::vector<std::string>& cols, int idx, std::string& out)
{
	out.clear();
	if (idx < 0 || idx >= (int)cols.size())
		return false;
	out = cols[(size_t)idx];
	return true;
}

} // namespace

FullEditErrorCode FullEditParseMcsCsv(
	const std::string& textIn,
	std::vector<FullEditMcsCsvRow>& outRows,
	std::string& errMsg)
{
	outRows.clear();
	errMsg.clear();
	const std::string text = StripUtf8Bom(textIn);
	std::istringstream iss(text);
	std::string line;
	std::vector<std::string> header;
	bool haveHeader = false;
	std::map<std::pair<int, int>, int> seen;

	while (std::getline(iss, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.empty() || line[0] == '#')
			continue;
		auto cols = SplitCsvLine(line);
		if (!haveHeader)
		{
			header = cols;
			haveHeader = true;
			continue;
		}
		const int iSchema = ColIndex(header, "schema");
		const int iBurn = ColIndex(header, "burn_index");
		const int iSw = ColIndex(header, "sw_lut_idx");
		const int iOpt = ColIndex(header, "optical_block");
		const int iCh = ColIndex(header, "ch_idx");
		const int iKind = ColIndex(header, "ch_kind");
		const int iTpL = ColIndex(header, "temp_point_LOW_0p1C");
		const int iTpR = ColIndex(header, "temp_point_ROOM_0p1C");
		const int iTpH = ColIndex(header, "temp_point_HIGH_0p1C");
		const int iLy = ColIndex(header, "LOW_dac_y");
		const int iLx = ColIndex(header, "LOW_dac_x");
		const int iRy = ColIndex(header, "ROOM_dac_y");
		const int iRx = ColIndex(header, "ROOM_dac_x");
		const int iHy = ColIndex(header, "HIGH_dac_y");
		const int iHx = ColIndex(header, "HIGH_dac_x");
		if (iSw < 0 || iCh < 0 || iLy < 0 || iLx < 0 || iRy < 0 || iRx < 0 || iHy < 0 || iHx < 0)
		{
			errMsg = "MCS CSV missing required columns";
			return FullEditErrorCode::CsvParse;
		}
		std::string schema, kind, optS;
		(void)GetCol(cols, iSchema, schema);
		if (!schema.empty() && schema != "mcs")
		{
			errMsg = "expected schema=mcs";
			return FullEditErrorCode::CsvParse;
		}
		FullEditMcsCsvRow row{};
		if (iBurn >= 0)
		{
			std::string s;
			GetCol(cols, iBurn, s);
			if (!s.empty() && !ParseIntStrict(s, row.burnIndex))
			{
				errMsg = "bad burn_index";
				return FullEditErrorCode::CsvParse;
			}
		}
		std::string sSw, sCh;
		GetCol(cols, iSw, sSw);
		GetCol(cols, iCh, sCh);
		if (!ParseIntStrict(sSw, row.swLutIdx) || !ParseIntStrict(sCh, row.chIdx))
		{
			errMsg = "bad sw/ch";
			return FullEditErrorCode::CsvParse;
		}
		if (row.swLutIdx < 0 || row.swLutIdx > 33 || row.chIdx < 0 || row.chIdx > 31)
		{
			errMsg = "sw/ch out of range";
			return FullEditErrorCode::CsvKey;
		}
		if (iOpt >= 0)
		{
			GetCol(cols, iOpt, optS);
			if (!ParseOptInt(optS, row.opticalBlock, row.opticalBlockValid))
			{
				errMsg = "bad optical_block";
				return FullEditErrorCode::CsvParse;
			}
		}
		if (iKind >= 0)
			GetCol(cols, iKind, row.chKind);
		else
			row.chKind = (row.chIdx < 20) ? "PORT" : "MID";

		auto takeShort = [&](int idx, short& dest) -> bool {
			std::string s;
			int v = 0;
			if (!GetCol(cols, idx, s) || !ParseIntStrict(s, v))
				return false;
			if (!FullEditDacInInt16Range(v))
				return false;
			dest = (short)v;
			return true;
		};
		if (!takeShort(iLy, row.dacs.lowY) || !takeShort(iLx, row.dacs.lowX)
			|| !takeShort(iRy, row.dacs.roomY) || !takeShort(iRx, row.dacs.roomX)
			|| !takeShort(iHy, row.dacs.highY) || !takeShort(iHx, row.dacs.highX))
		{
			errMsg = "bad DAC or out of int16";
			return FullEditErrorCode::DacRange;
		}
		if (iTpL >= 0)
		{
			if (!takeShort(iTpL, row.dacs.tempPointLow) || !takeShort(iTpR, row.dacs.tempPointRoom)
				|| !takeShort(iTpH, row.dacs.tempPointHigh))
			{
				errMsg = "bad temp_point";
				return FullEditErrorCode::CsvParse;
			}
		}
		const auto key = std::make_pair(row.swLutIdx, row.chIdx);
		if (seen.count(key))
		{
			errMsg = "duplicate MCS row key";
			return FullEditErrorCode::CsvKey;
		}
		seen[key] = 1;
		outRows.push_back(row);
	}
	if (!haveHeader)
	{
		errMsg = "empty MCS CSV";
		return FullEditErrorCode::CsvParse;
	}
	return FullEditErrorCode::Ok;
}

FullEditErrorCode FullEditParseMemsCsv(
	const std::string& textIn,
	std::vector<FullEditMemsCsvRow>& outRows,
	std::string& errMsg)
{
	outRows.clear();
	errMsg.clear();
	const std::string text = StripUtf8Bom(textIn);
	std::istringstream iss(text);
	std::string line;
	std::vector<std::string> header;
	bool haveHeader = false;
	std::map<std::pair<std::string, int>, int> seen;

	auto takeShort = [&](const std::vector<std::string>& cols, int idx, short& dest) -> bool {
		std::string s;
		int v = 0;
		if (idx < 0 || idx >= (int)cols.size() || !ParseIntStrict(cols[(size_t)idx], v))
			return false;
		if (!FullEditDacInInt16Range(v))
			return false;
		dest = (short)v;
		return true;
	};

	while (std::getline(iss, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.empty() || line[0] == '#')
			continue;
		auto cols = SplitCsvLine(line);
		if (!haveHeader)
		{
			header = cols;
			haveHeader = true;
			continue;
		}
		const int iSchema = ColIndex(header, "schema");
		const int iBurn = ColIndex(header, "burn_index");
		const int iSw = ColIndex(header, "sw1to4");
		const int iKind = ColIndex(header, "row_kind");
		const int iIdx = ColIndex(header, "row_idx");
		const int iTpL = ColIndex(header, "temp_centi_LOW");
		const int iTpR = ColIndex(header, "temp_centi_ROOM");
		const int iTpH = ColIndex(header, "temp_centi_HIGH");
		const int iLy = ColIndex(header, "LOW_dac_y");
		const int iLx = ColIndex(header, "LOW_dac_x");
		const int iRy = ColIndex(header, "ROOM_dac_y");
		const int iRx = ColIndex(header, "ROOM_dac_x");
		const int iHy = ColIndex(header, "HIGH_dac_y");
		const int iHx = ColIndex(header, "HIGH_dac_x");
		if (iKind < 0 || iIdx < 0 || iLy < 0 || iLx < 0 || iRy < 0 || iRx < 0 || iHy < 0 || iHx < 0)
		{
			errMsg = "MEMS CSV missing required columns";
			return FullEditErrorCode::CsvParse;
		}
		std::string schema;
		if (iSchema >= 0)
			schema = cols[(size_t)iSchema];
		if (!schema.empty() && schema != "mems")
		{
			errMsg = "expected schema=mems";
			return FullEditErrorCode::CsvParse;
		}
		FullEditMemsCsvRow row{};
		if (iBurn >= 0 && !cols[(size_t)iBurn].empty()
			&& !ParseIntStrict(cols[(size_t)iBurn], row.burnIndex))
		{
			errMsg = "bad burn_index";
			return FullEditErrorCode::CsvParse;
		}
		if (iSw >= 0 && !ParseIntStrict(cols[(size_t)iSw], row.sw1to4))
		{
			errMsg = "bad sw1to4";
			return FullEditErrorCode::CsvParse;
		}
		row.rowKind = cols[(size_t)iKind];
		if (row.rowKind != "CH" && row.rowKind != "MID")
		{
			errMsg = "row_kind must be CH or MID";
			return FullEditErrorCode::CsvKey;
		}
		if (!ParseIntStrict(cols[(size_t)iIdx], row.rowIdx))
		{
			errMsg = "bad row_idx";
			return FullEditErrorCode::CsvParse;
		}
		const bool isMid = (row.rowKind == "MID");
		if (isMid)
		{
			if (row.rowIdx < 0 || row.rowIdx >= 25)
			{
				errMsg = "MID row_idx out of range";
				return FullEditErrorCode::CsvKey;
			}
		}
		else if (row.rowIdx < 0 || row.rowIdx >= 36)
		{
			errMsg = "CH row_idx out of range";
			return FullEditErrorCode::CsvKey;
		}
		if (!takeShort(cols, iLy, row.dacs.lowY) || !takeShort(cols, iLx, row.dacs.lowX)
			|| !takeShort(cols, iRy, row.dacs.roomY) || !takeShort(cols, iRx, row.dacs.roomX)
			|| !takeShort(cols, iHy, row.dacs.highY) || !takeShort(cols, iHx, row.dacs.highX))
		{
			errMsg = "bad DAC or out of int16";
			return FullEditErrorCode::DacRange;
		}
		if (iTpL >= 0)
		{
			if (!takeShort(cols, iTpL, row.dacs.tempPointLow)
				|| !takeShort(cols, iTpR, row.dacs.tempPointRoom)
				|| !takeShort(cols, iTpH, row.dacs.tempPointHigh))
			{
				errMsg = "bad temp_centi";
				return FullEditErrorCode::CsvParse;
			}
		}
		const auto key = std::make_pair(row.rowKind, row.rowIdx);
		if (seen.count(key))
		{
			errMsg = "duplicate MEMS row key";
			return FullEditErrorCode::CsvKey;
		}
		seen[key] = 1;
		outRows.push_back(row);
	}
	if (!haveHeader)
	{
		errMsg = "empty MEMS CSV";
		return FullEditErrorCode::CsvParse;
	}
	return FullEditErrorCode::Ok;
}

FullEditErrorCode FullEditDiffMcsCsv(
	const std::vector<FullEditMcsCsvRow>& working,
	const std::vector<FullEditMcsCsvRow>& baseline,
	const FullEditUnlockFlags& unlock,
	std::vector<FullEditMcsPatch>& outPatches,
	std::string& errMsg)
{
	outPatches.clear();
	errMsg.clear();
	std::map<std::pair<int, int>, FullEditMcsCsvRow> baseMap;
	for (const auto& r : baseline)
		baseMap[{r.swLutIdx, r.chIdx}] = r;

	for (const auto& w : working)
	{
		const auto key = std::make_pair(w.swLutIdx, w.chIdx);
		auto it = baseMap.find(key);
		if (it == baseMap.end())
		{
			errMsg = "working row missing in baseline";
			return FullEditErrorCode::CsvKey;
		}
		const FullEditMcsCsvRow& b = it->second;
		const bool dacDiff = !TripleEqualDacs(w.dacs, b.dacs);
		const bool tpDiff = !TripleEqualTemps(w.dacs, b.dacs);
		if (!dacDiff && !tpDiff)
			continue;
		if (!TripleInRange(w.dacs))
		{
			errMsg = "DAC out of int16";
			return FullEditErrorCode::DacRange;
		}
		if (!FullEditDangerAllowed(true, w.swLutIdx, w.chIdx, false, unlock))
		{
			errMsg = "dangerous MCS row changed without unlock";
			return FullEditErrorCode::DangerousLocked;
		}
		FullEditMcsPatch p{};
		p.swLutIdx = w.swLutIdx;
		p.chIdx = w.chIdx;
		p.dacs = w.dacs;
		p.patchTempPoints = tpDiff;
		outPatches.push_back(p);
	}
	return FullEditErrorCode::Ok;
}

FullEditErrorCode FullEditDiffMemsCsv(
	const std::vector<FullEditMemsCsvRow>& working,
	const std::vector<FullEditMemsCsvRow>& baseline,
	const FullEditUnlockFlags& unlock,
	std::vector<FullEditMemsPatch>& outPatches,
	std::string& errMsg)
{
	outPatches.clear();
	errMsg.clear();
	std::map<std::pair<std::string, int>, FullEditMemsCsvRow> baseMap;
	for (const auto& r : baseline)
		baseMap[{r.rowKind, r.rowIdx}] = r;

	for (const auto& w : working)
	{
		const auto key = std::make_pair(w.rowKind, w.rowIdx);
		auto it = baseMap.find(key);
		if (it == baseMap.end())
		{
			errMsg = "working row missing in baseline";
			return FullEditErrorCode::CsvKey;
		}
		const FullEditMemsCsvRow& b = it->second;
		const bool dacDiff = !TripleEqualDacs(w.dacs, b.dacs);
		const bool tpDiff = !TripleEqualTemps(w.dacs, b.dacs);
		if (!dacDiff && !tpDiff)
			continue;
		if (!TripleInRange(w.dacs))
		{
			errMsg = "DAC out of int16";
			return FullEditErrorCode::DacRange;
		}
		const bool isMid = (w.rowKind == "MID");
		if (!FullEditDangerAllowed(false, 0, w.rowIdx, isMid, unlock))
		{
			errMsg = "dangerous MEMS row changed without unlock";
			return FullEditErrorCode::DangerousLocked;
		}
		FullEditMemsPatch p{};
		p.isMid = isMid;
		p.rowIdx = w.rowIdx;
		p.dacs = w.dacs;
		p.patchTempCenti = tpDiff;
		outPatches.push_back(p);
	}
	return FullEditErrorCode::Ok;
}

std::string FullEditFormatMcsCsv(
	int burnIndex,
	const std::string& snLabel,
	const std::vector<FullEditMcsCsvRow>& rows)
{
	std::ostringstream oss;
	oss << "# schema=mcs FullEdit DAC CSV\n";
	oss << "schema,burn_index,sn_label,sw_lut_idx,optical_block,ch_idx,ch_kind,"
		   "temp_point_LOW_0p1C,temp_point_ROOM_0p1C,temp_point_HIGH_0p1C,"
		   "LOW_dac_y,LOW_dac_x,ROOM_dac_y,ROOM_dac_x,HIGH_dac_y,HIGH_dac_x,"
		   "delta_ROOM_LOW_y,delta_ROOM_LOW_x,delta_HIGH_LOW_y,delta_HIGH_LOW_x,"
		   "delta_HIGH_ROOM_y,delta_HIGH_ROOM_x\n";
	for (const auto& r : rows)
	{
		const int ly = r.dacs.lowY, lx = r.dacs.lowX;
		const int ry = r.dacs.roomY, rx = r.dacs.roomX;
		const int hy = r.dacs.highY, hx = r.dacs.highX;
		oss << "mcs," << burnIndex << "," << snLabel << ","
			<< r.swLutIdx << ",";
		if (r.opticalBlockValid)
			oss << r.opticalBlock;
		oss << "," << r.chIdx << "," << r.chKind << ","
			<< r.dacs.tempPointLow << "," << r.dacs.tempPointRoom << "," << r.dacs.tempPointHigh << ","
			<< ly << "," << lx << "," << ry << "," << rx << "," << hy << "," << hx << ","
			<< (ry - ly) << "," << (rx - lx) << ","
			<< (hy - ly) << "," << (hx - lx) << ","
			<< (hy - ry) << "," << (hx - rx) << "\n";
	}
	return oss.str();
}

std::string FullEditFormatMemsCsv(
	int burnIndex,
	const std::string& snLabel,
	int sw1to4,
	const std::vector<FullEditMemsCsvRow>& rows)
{
	std::ostringstream oss;
	oss << "# schema=mems FullEdit DAC CSV; dac_y=sDACx dac_x=sDACy (FineTune)\n";
	oss << "schema,burn_index,sn_label,sw1to4,row_kind,row_idx,"
		   "temp_centi_LOW,temp_centi_ROOM,temp_centi_HIGH,"
		   "LOW_dac_y,LOW_dac_x,ROOM_dac_y,ROOM_dac_x,HIGH_dac_y,HIGH_dac_x,"
		   "delta_ROOM_LOW_y,delta_ROOM_LOW_x,delta_HIGH_LOW_y,delta_HIGH_LOW_x,"
		   "delta_HIGH_ROOM_y,delta_HIGH_ROOM_x\n";
	for (const auto& r : rows)
	{
		const int ly = r.dacs.lowY, lx = r.dacs.lowX;
		const int ry = r.dacs.roomY, rx = r.dacs.roomX;
		const int hy = r.dacs.highY, hx = r.dacs.highX;
		oss << "mems," << burnIndex << "," << snLabel << "," << sw1to4 << ","
			<< r.rowKind << "," << r.rowIdx << ","
			<< r.dacs.tempPointLow << "," << r.dacs.tempPointRoom << "," << r.dacs.tempPointHigh << ","
			<< ly << "," << lx << "," << ry << "," << rx << "," << hy << "," << hx << ","
			<< (ry - ly) << "," << (rx - lx) << ","
			<< (hy - ly) << "," << (hx - lx) << ","
			<< (hy - ry) << "," << (hx - rx) << "\n";
	}
	return oss.str();
}
