// mod_runemaker.h : main header file for the MOD_RUNEMAKER DLL
//

#if !defined(AFX_MOD_RUNEMAKER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_RUNEMAKER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif // ifndef __AFXWIN_H__

#include "resource.h"           // main symbols
#include <IModuleInterface.h>
#include "ConfigDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMod_runemakerApp
// See mod_runemaker.cpp for the implementation of this class
//

class CMod_runemakerApp : public IModuleInterface
{
public:
	CMod_runemakerApp();
	~CMod_runemakerApp();
public:
	void getNewSkin(CSkin);
	void resetMultiParamAccess(const char *paramName);
	int isMultiParam(const char *paramName);
	int validateConfig(int showAlerts);
	// functions from IModuleInterface
	char * getName();
	int isStarted();
	void start();
	void stop();
	void showConfigDialog();
	void configToControls();
	void controlsToConfig();
	void disableControls();
	void enableControls();
	void makeNow(int enable);
	char *getVersion();
	void resetConfig();
	void loadConfigParam(const char *paramName, char *paramValue);
	char *saveConfigParam(const char *paramName);
	const char **getConfigParamNames();
private:
	int m_started;
	CConfigDialog * m_configDialog;
	CConfigData *m_configData;
	int m_currentSpellNr;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_RUNEMAKER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
