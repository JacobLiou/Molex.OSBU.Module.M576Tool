#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file
#endif

#include "resource.h"

class CM576CalibratorApp : public CWinAppEx
{
public:
	CM576CalibratorApp();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CM576CalibratorApp theApp;
