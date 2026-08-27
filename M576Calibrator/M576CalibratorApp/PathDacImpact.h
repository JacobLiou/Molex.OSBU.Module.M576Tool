#pragma once
// Product CH 1..576 -> six FineTuneAddresses (1x64 parent+leaf x2 + MCS1/2). No MFC.

#include "FullEditDacTypes.h"
#include "SmallRangeCalibSelection.h"

#include <string>
#include <vector>

bool PathDacImpactParentFromLeaf(
	int leafSw1to4,
	int leafChY1to17,
	int& parentSw1to4Out,
	int& parentChY1to17Out,
	bool& isDirectPassOut);

FullEditErrorCode PathDacImpactResolve(
	int ch1to576,
	const std::vector<SmallRangeMapRow>& map1x64_1,
	const std::vector<SmallRangeMapRow>& map1x64_2,
	PathDacImpactResult& out,
	std::string& errMsg);

std::string PathDacImpactFormatSummaryZh(const PathDacImpactResult& r);

std::string PathDacImpactFormatText(const PathDacImpactResult& r);
