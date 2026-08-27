#pragma once
// FullEdit / PathImpact shared types (no MFC). Safe for CrossPeakTest.

#include "FineTuneTypes.h"

#include <cstdint>
#include <string>
#include <vector>

enum class FullEditErrorCode
{
	Ok = 0,
	NoSn,
	BinMissing,
	ExportFail,
	BaselineMissing,
	CsvParse,
	CsvKey,
	DacRange,
	DangerousLocked,
	DiffEmpty,
	WriteFail,
	VerifyFail,
	PathCh,
	Busy,
};

inline const char* FullEditErrorCodeName(FullEditErrorCode c)
{
	switch (c)
	{
	case FullEditErrorCode::Ok: return "FE_OK";
	case FullEditErrorCode::NoSn: return "FE_NO_SN";
	case FullEditErrorCode::BinMissing: return "FE_BIN_MISSING";
	case FullEditErrorCode::ExportFail: return "FE_EXPORT_FAIL";
	case FullEditErrorCode::BaselineMissing: return "FE_BASELINE_MISSING";
	case FullEditErrorCode::CsvParse: return "FE_CSV_PARSE";
	case FullEditErrorCode::CsvKey: return "FE_CSV_KEY";
	case FullEditErrorCode::DacRange: return "FE_DAC_RANGE";
	case FullEditErrorCode::DangerousLocked: return "FE_DANGEROUS_LOCKED";
	case FullEditErrorCode::DiffEmpty: return "FE_DIFF_EMPTY";
	case FullEditErrorCode::WriteFail: return "FE_WRITE_FAIL";
	case FullEditErrorCode::VerifyFail: return "FE_VERIFY_FAIL";
	case FullEditErrorCode::PathCh: return "FE_PATH_CH";
	case FullEditErrorCode::Busy: return "FE_BUSY";
	default: return "FE_UNKNOWN";
	}
}

struct FullEditUnlockFlags
{
	bool unlockSn33_34 = false;
	bool unlockMcsChGe18 = false;
	bool unlockMemsChGe18OrMid = false;
};

enum class PathDacImpactStageKind
{
	Parent = 0,
	Leaf,
	Mcs,
};

struct PathDacImpactSlot
{
	FineTuneAddress addr{};
	int burnIndex = -1;
	int lutSwIdx = -1;
	int lutChIdx = -1;
	int memsSw1to4 = 0;
	int memsChY1to17 = 0;
	std::string csvRowKey;
	std::string label;
	std::string roleZh;
	std::string csvFileHint;
	PathDacImpactStageKind stageKind = PathDacImpactStageKind::Mcs;
	bool isDirectPass = false;
};

struct PathDacImpactResult
{
	int ch1to576 = 0;
	int opticalBlock1to32 = 0;
	int mcsCh1to18 = 0;
	int peerPort1to64 = 0;
	std::string mpoPath;
	bool cascade1x64 = false;
	std::vector<PathDacImpactSlot> slots;
};

struct FullEditTempDacTriple
{
	short lowY = 0, lowX = 0;
	short roomY = 0, roomX = 0;
	short highY = 0, highX = 0;
	short tempPointLow = 0, tempPointRoom = 0, tempPointHigh = 0;
};

struct FullEditMcsCsvRow
{
	int burnIndex = -1;
	int swLutIdx = 0;
	int opticalBlock = 0;
	bool opticalBlockValid = false;
	int chIdx = 0;
	std::string chKind;
	FullEditTempDacTriple dacs{};
};

struct FullEditMemsCsvRow
{
	int burnIndex = -1;
	int sw1to4 = 1;
	std::string rowKind;
	int rowIdx = 0;
	FullEditTempDacTriple dacs{};
};

struct FullEditMcsPatch
{
	int swLutIdx = 0;
	int chIdx = 0;
	FullEditTempDacTriple dacs{};
	bool patchTempPoints = false;
};

struct FullEditMemsPatch
{
	bool isMid = false;
	int rowIdx = 0;
	FullEditTempDacTriple dacs{};
	bool patchTempCenti = false;
};

inline bool FullEditDacInInt16Range(int v)
{
	return v >= -32768 && v <= 32767;
}

inline bool FullEditMcsRowIsDangerous(int swLutIdx, int chIdx)
{
	if (swLutIdx == 32 || swLutIdx == 33)
		return true;
	if (chIdx >= 18)
		return true;
	return false;
}

inline bool FullEditMemsRowIsDangerous(bool isMid, int rowIdx)
{
	if (isMid)
		return true;
	return rowIdx >= 17;
}

inline bool FullEditDangerAllowed(
	bool isMcs,
	int swLutIdx,
	int chOrRowIdx,
	bool isMid,
	const FullEditUnlockFlags& unlock)
{
	if (isMcs)
	{
		if ((swLutIdx == 32 || swLutIdx == 33) && !unlock.unlockSn33_34)
			return false;
		if (chOrRowIdx >= 18 && !unlock.unlockMcsChGe18)
			return false;
		return true;
	}
	if (FullEditMemsRowIsDangerous(isMid, chOrRowIdx) && !unlock.unlockMemsChGe18OrMid)
		return false;
	return true;
}
