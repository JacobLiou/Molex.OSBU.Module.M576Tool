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
