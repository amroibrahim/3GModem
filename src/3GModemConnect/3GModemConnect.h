
// 3GModemConnect.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMy3GModemConnectApp:
// See 3GModemConnect.cpp for the implementation of this class
//


#include <gdiplus.h>
using namespace Gdiplus;

class CMy3GModemConnectApp : public CWinAppEx
{
public:
	CMy3GModemConnectApp();
	GdiplusStartupInput gdiplusstartupinput;
	ULONG_PTR gdiplusptr;

	// Overrides
public:
	virtual BOOL InitInstance();


	// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CMy3GModemConnectApp theApp;