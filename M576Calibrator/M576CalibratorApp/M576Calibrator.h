#pragma once
// MFC 应用程序对象：进程序入口、InitInstance 中主对话框，全局 theApp 供资源与异常路径使用。

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
