#include "stdafx.h"
#include "IlTestMath.h"

#include <limits.h>
#include <stdlib.h>

BOOL IlTestParseIntReply(const CStringA& reply, int& outVal)
{
	outVal = 0;
	CStringA s = reply;
	s.Trim();
	if (s.IsEmpty())
		return FALSE;
	char* end = NULL;
	const long v = strtol(s.GetString(), &end, 10);
	if (end == s.GetString())
		return FALSE;
	while (end && *end)
	{
		if (*end != ' ' && *end != '\t' && *end != '\r' && *end != '\n')
			return FALSE;
		++end;
	}
	if (v < (long)INT_MIN || v > (long)INT_MAX)
		return FALSE;
	outVal = (int)v;
	return TRUE;
}

int IlTestParseChannelIndex(const CString& channelLabel)
{
	CString s = channelLabel;
	s.Trim();
	if (s.IsEmpty())
		return 0;
	int i = 0;
	if (s.GetLength() >= 2 && (s[0] == _T('C') || s[0] == _T('c'))
		&& (s[1] == _T('H') || s[1] == _T('h')))
		i = 2;
	CString num = s.Mid(i);
	num.Trim();
	if (num.IsEmpty())
		return 0;
	const int v = _ttoi(num);
	if (v < 1 || v > 576)
		return 0;
	return v;
}

BOOL IlTestChannelToMpoPorts(int channel1to576, CString& inPort, CString& outPort)
{
	inPort.Empty();
	outPort.Empty();
	if (channel1to576 < 1 || channel1to576 > 576)
		return FALSE;
	const int idx0 = channel1to576 - 1;
	const int mpoIn = idx0 / 12 + 1;
	const int fiber = idx0 % 12 + 1;
	const int mpoOut = mpoIn + 48;
	inPort.Format(_T("MPO%d-%d"), mpoIn, fiber);
	outPort.Format(_T("MPO%d-%d"), mpoOut, fiber);
	return TRUE;
}

BOOL IlTestChannelToMpoPath(int channel1to576, CString& outPath)
{
	outPath.Empty();
	CString inPort, outPort;
	if (!IlTestChannelToMpoPorts(channel1to576, inPort, outPort))
		return FALSE;
	outPath.Format(_T("%s->%s"), inPort.GetString(), outPort.GetString());
	return TRUE;
}

BOOL IlTestBuildSwCommandsForChannelHalf(
	int ch1to576, int half, std::vector<CStringA>& outCmds, CString& err)
{
	err.Empty();
	outCmds.clear();
	if (ch1to576 < 1 || ch1to576 > 576)
	{
		err.Format(_T("Channel %d out of range 1..576."), ch1to576);
		return FALSE;
	}
	if (half != 1 && half != 2)
	{
		err.Format(_T("IL half %d invalid (use 1=IN or 2=OUT)."), half);
		return FALSE;
	}
	const int sw = (ch1to576 - 1) / 18 + 1;
	const int mcsCh = (ch1to576 - 1) % 18 + 1;
	const int pb = sw + 32;
	// IN: SW1 ports sw / sw+32; OUT: SW1 ports swapped (firmware diagnosis_sw group2).
	const int sw1a = (half == 2) ? pb : sw;
	const int sw1b = (half == 2) ? sw : pb;

	CStringA c1, c2, c3, c4;
	c1.Format("SW 1 1 %d", sw1a);
	c2.Format("SW 1 2 %d", sw1b);
	c3.Format("SW 2 %d %d", sw, mcsCh);
	c4.Format("SW 2 %d %d", pb, mcsCh);
	outCmds.push_back(c1);
	outCmds.push_back(c2);
	outCmds.push_back(c3);
	outCmds.push_back(c4);
	return TRUE;
}

BOOL IlTestBuildDiagnosisRowFromChannel(int ch1to576, M576DiagnosisRow& out, CString& err)
{
	err.Empty();
	out = M576DiagnosisRow{};
	std::vector<CStringA> cmds;
	if (!IlTestBuildSwCommandsForChannelHalf(ch1to576, 1, cmds, err))
		return FALSE;
	CStringA chLabel;
	chLabel.Format("CH%d", ch1to576);
	out.channel = chLabel;
	out.label = chLabel;
	out.swCommands = cmds;
	return TRUE;
}
