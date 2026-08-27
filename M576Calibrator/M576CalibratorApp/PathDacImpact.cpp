#include "PathDacImpact.h"

#include <cstdio>
#include <sstream>

namespace {

int LutSwIdxFromBlock(int block1to32)
{
	if (block1to32 < 1 || block1to32 > 32)
		return 0;
	const int ch0 = block1to32 - 1;
	return (ch0 < 16) ? (ch0 + 16) : (ch0 - 16);
}

bool FindUniqueMapByPorts(
	const std::vector<SmallRangeMapRow>& rows,
	int c1,
	int c4,
	SmallRangeMapRow& out,
	std::string& err)
{
	out = SmallRangeMapRow{};
	int hits = 0;
	SmallRangeMapRow found{};
	for (const SmallRangeMapRow& r : rows)
	{
		if (r.c1 == c1 && r.c4 == c4)
		{
			++hits;
			found = r;
		}
	}
	if (hits == 0)
	{
		err = "no Mapping row for c1=" + std::to_string(c1) + " c4=" + std::to_string(c4);
		return false;
	}
	if (hits > 1)
	{
		err = "ambiguous Mapping (" + std::to_string(hits) + " rows) for c1=" + std::to_string(c1)
			+ " c4=" + std::to_string(c4);
		return false;
	}
	out = found;
	return true;
}

void FillMcsSlot(
	PathDacImpactSlot& slot,
	FineTuneDeviceKind dev,
	int block,
	int mcsCh,
	const char* roleZh)
{
	slot = PathDacImpactSlot{};
	slot.addr.device = dev;
	slot.addr.mcsBlock1to32 = block;
	slot.addr.mcsCh1to18 = mcsCh;
	slot.burnIndex = (dev == FineTuneDeviceKind::Mcs1) ? 0 : 1;
	slot.lutSwIdx = LutSwIdxFromBlock(block);
	slot.lutChIdx = mcsCh - 1;
	char key[64] = {};
	std::snprintf(key, sizeof(key), "sw=%d,ch=%d", slot.lutSwIdx, slot.lutChIdx);
	slot.csvRowKey = key;
	slot.label = FineTuneAddressFormatLabelA(slot.addr);
	slot.roleZh = roleZh;
	slot.stageKind = PathDacImpactStageKind::Mcs;
	slot.isDirectPass = false;
	char hint[32] = {};
	std::snprintf(hint, sizeof(hint), "burn=%d", slot.burnIndex);
	slot.csvFileHint = hint;
}

void FillMemsSlot(
	PathDacImpactSlot& slot,
	FineTuneDeviceKind dev,
	int sw1to4,
	int chY1to17,
	PathDacImpactStageKind kind,
	const char* roleZh,
	bool isDirectPass)
{
	slot = PathDacImpactSlot{};
	slot.addr.device = dev;
	slot.addr.sw1to4 = sw1to4;
	slot.addr.chY1to17 = chY1to17;
	slot.memsSw1to4 = sw1to4;
	slot.memsChY1to17 = chY1to17;
	slot.burnIndex = (dev == FineTuneDeviceKind::OneX64_1)
		? (2 + (sw1to4 - 1))
		: (6 + (sw1to4 - 1));
	char key[64] = {};
	std::snprintf(key, sizeof(key), "CH,%d", chY1to17 - 1);
	slot.csvRowKey = key;
	slot.label = FineTuneAddressFormatLabelA(slot.addr);
	slot.roleZh = roleZh;
	slot.stageKind = kind;
	slot.isDirectPass = isDirectPass;
	char hint[32] = {};
	std::snprintf(hint, sizeof(hint), "burn=%d", slot.burnIndex);
	slot.csvFileHint = hint;
}

} // namespace

bool PathDacImpactParentFromLeaf(
	int leafSw1to4,
	int leafChY1to17,
	int& parentSw1to4Out,
	int& parentChY1to17Out,
	bool& isDirectPassOut)
{
	parentSw1to4Out = 0;
	parentChY1to17Out = 0;
	isDirectPassOut = false;
	if (leafSw1to4 < 1 || leafSw1to4 > 4 || leafChY1to17 < 1 || leafChY1to17 > 17)
		return false;
	if (leafSw1to4 == 1)
	{
		parentSw1to4Out = 1;
		parentChY1to17Out = leafChY1to17;
		isDirectPassOut = true;
		return true;
	}
	parentSw1to4Out = 1;
	if (leafSw1to4 == 2)
		parentChY1to17Out = 8;
	else if (leafSw1to4 == 3)
		parentChY1to17Out = 9;
	else
		parentChY1to17Out = 10;
	isDirectPassOut = false;
	return true;
}

FullEditErrorCode PathDacImpactResolve(
	int ch1to576,
	const std::vector<SmallRangeMapRow>& map1x64_1,
	const std::vector<SmallRangeMapRow>& map1x64_2,
	PathDacImpactResult& out,
	std::string& errMsg)
{
	out = PathDacImpactResult{};
	errMsg.clear();
	if (ch1to576 < 1 || ch1to576 > 576)
	{
		errMsg = "channel out of range 1..576";
		return FullEditErrorCode::PathCh;
	}

	const int sw = (ch1to576 - 1) / 18 + 1;
	const int mcsCh = (ch1to576 - 1) % 18 + 1;
	const int pb = sw + 32;
	out.ch1to576 = ch1to576;
	out.opticalBlock1to32 = sw;
	out.mcsCh1to18 = mcsCh;
	out.peerPort1to64 = pb;

	{
		const int idx0 = ch1to576 - 1;
		const int mpoIn = idx0 / 12 + 1;
		const int fiber = idx0 % 12 + 1;
		const int mpoOut = mpoIn + 48;
		char buf[96] = {};
		std::snprintf(buf, sizeof(buf), "MPO%d-%d->MPO%d-%d", mpoIn, fiber, mpoOut, fiber);
		out.mpoPath = buf;
	}

	SmallRangeMapRow leaf1{}, leaf2{};
	std::string e1, e2;
	if (!FindUniqueMapByPorts(map1x64_1, sw, pb, leaf1, e1))
	{
		errMsg = "1x64_1: " + e1;
		return FullEditErrorCode::PathCh;
	}
	if (!FindUniqueMapByPorts(map1x64_2, sw, pb, leaf2, e2))
	{
		errMsg = "1x64_2: " + e2;
		return FullEditErrorCode::PathCh;
	}
	if (leaf1.sw1to4 < 1 || leaf1.sw1to4 > 4 || leaf1.chY1based < 1 || leaf1.chY1based > 17
		|| leaf2.sw1to4 < 1 || leaf2.sw1to4 > 4 || leaf2.chY1based < 1 || leaf2.chY1based > 17)
	{
		errMsg = "Mapping SW/CH_y out of range";
		return FullEditErrorCode::PathCh;
	}

	int pSw1 = 0, pCh1 = 0, pSw2 = 0, pCh2 = 0;
	bool dir1 = false, dir2 = false;
	if (!PathDacImpactParentFromLeaf(leaf1.sw1to4, leaf1.chY1based, pSw1, pCh1, dir1)
		|| !PathDacImpactParentFromLeaf(leaf2.sw1to4, leaf2.chY1based, pSw2, pCh2, dir2))
	{
		errMsg = "ParentFromLeaf failed";
		return FullEditErrorCode::PathCh;
	}
	out.cascade1x64 = !dir1 || !dir2;

	out.slots.resize(6);
	// roleZh Chinese as UTF-8 escapes (source stays ASCII-safe)
	static const char kParent1[] = "1#1x64 \xe4\xb8\x8a\xe7\xba\xa7";
	static const char kParent2[] = "2#1x64 \xe4\xb8\x8a\xe7\xba\xa7";
	static const char kLeaf1[] = "1#1x64 \xe5\x8f\xb6\xe5\xad\x90";
	static const char kLeaf1D[] =
		"1#1x64 \xe5\x8f\xb6\xe5\xad\x90(\xe7\x9b\xb4\xe9\x80\x9a\xe6\x97\xa0\xe7\xba\xa7\xe8\x81\x94)";
	static const char kLeaf2[] = "2#1x64 \xe5\x8f\xb6\xe5\xad\x90";
	static const char kLeaf2D[] =
		"2#1x64 \xe5\x8f\xb6\xe5\xad\x90(\xe7\x9b\xb4\xe9\x80\x9a\xe6\x97\xa0\xe7\xba\xa7\xe8\x81\x94)";
	static const char kMcs1[] = "1#MCS";
	static const char kMcs2[] = "2#MCS";
	const char* leafRole1 = dir1 ? kLeaf1D : kLeaf1;
	const char* leafRole2 = dir2 ? kLeaf2D : kLeaf2;
	FillMemsSlot(out.slots[0], FineTuneDeviceKind::OneX64_1, pSw1, pCh1,
		PathDacImpactStageKind::Parent, kParent1, dir1);
	FillMemsSlot(out.slots[1], FineTuneDeviceKind::OneX64_1, leaf1.sw1to4, leaf1.chY1based,
		PathDacImpactStageKind::Leaf, leafRole1, dir1);
	FillMcsSlot(out.slots[2], FineTuneDeviceKind::Mcs1, sw, mcsCh, kMcs1);
	FillMcsSlot(out.slots[3], FineTuneDeviceKind::Mcs2, sw, mcsCh, kMcs2);
	FillMemsSlot(out.slots[4], FineTuneDeviceKind::OneX64_2, pSw2, pCh2,
		PathDacImpactStageKind::Parent, kParent2, dir2);
	FillMemsSlot(out.slots[5], FineTuneDeviceKind::OneX64_2, leaf2.sw1to4, leaf2.chY1based,
		PathDacImpactStageKind::Leaf, leafRole2, dir2);
	return FullEditErrorCode::Ok;
}

std::string PathDacImpactFormatSummaryZh(const PathDacImpactResult& r)
{
	std::ostringstream oss;
	oss << "\xe4\xba\xa7\xe5\x93\x81\xe9\x80\x9a\xe9\x81\x93 CH" << r.ch1to576
		<< " | \xe5\x85\x89\xe8\xb7\xaf " << r.mpoPath
		<< " | 1x64\xe5\x8f\xa3 " << r.opticalBlock1to32 << "<->" << r.peerPort1to64
		<< " | MCS\xe5\x9d\x97" << r.opticalBlock1to32 << "/ch" << r.mcsCh1to18
		<< (r.cascade1x64
			? " | \xe7\xba\xa7\xe8\x81\x94:SW1->\xe5\x8f\xb6\xe5\xad\x90"
			: " | \xe7\x9b\xb4\xe9\x80\x9a:\xe4\xbb\x85SW1");
	return oss.str();
}

std::string PathDacImpactFormatText(const PathDacImpactResult& r)
{
	std::ostringstream oss;
	oss << PathDacImpactFormatSummaryZh(r) << "\n";
	oss << "\xe7\xba\xa7\t\xe8\xa7\x92\xe8\x89\xb2\t\xe8\xae\xbe\xe5\xa4\x87\tburn\t\xe6\xa7\xbd\xe4\xbd\x8d\tkey\n";
	int i = 1;
	for (const PathDacImpactSlot& s : r.slots)
	{
		std::string slotDesc;
		if (FineTuneIsMcsDevice(s.addr.device))
		{
			slotDesc = "block=" + std::to_string(s.addr.mcsBlock1to32)
				+ " ch=" + std::to_string(s.addr.mcsCh1to18)
				+ " (LUT sw=" + std::to_string(s.lutSwIdx) + ",ch=" + std::to_string(s.lutChIdx) + ")";
		}
		else
		{
			slotDesc = "SW" + std::to_string(s.memsSw1to4)
				+ " CH_y=" + std::to_string(s.memsChY1to17);
		}
		oss << i << "\t" << s.roleZh << "\t" << s.label
			<< "\tburn=" << s.burnIndex
			<< "\t" << slotDesc
			<< "\t" << s.csvRowKey << "\n";
		++i;
	}
	return oss.str();
}
