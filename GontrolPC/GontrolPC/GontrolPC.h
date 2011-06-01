
// GontrolPC.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGontrolPCApp:
// See GontrolPC.cpp for the implementation of this class
//

class CGontrolPCApp : public CWinAppEx
{
public:
	CGontrolPCApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CGontrolPCApp theApp;