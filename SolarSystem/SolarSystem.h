
// SolarSystem.h : main header file for the SolarSystem application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include "Options.h"

// CSolarSystemApp:
// See SolarSystem.cpp for the implementation of this class
//

class CSolarSystemApp : public CWinAppEx
{
public:
	CSolarSystemApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	Options options;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSolarSystemApp theApp;
