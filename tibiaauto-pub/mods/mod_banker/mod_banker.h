// mod_banker.h : main header file for the MOD_BANKER DLL
//

#if !defined(AFX_MOD_BANKER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_BANKER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif // ifndef __AFXWIN_H__

#define MAX_BANKERS 16
#define MAX_BANKER_NPCS 50

#include "resource.h"           // main symbols
#include <IModuleInterface.h>
#include "ConfigDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CMod_bankerApp
// See mod_banker.cpp for the implementation of this class
//

class CMod_bankerApp : public IModuleInterface
{
public:
	CMod_bankerApp();
	~CMod_bankerApp();
public:
	void getNewSkin(CSkin);
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
	char *getVersion();
	void resetConfig();
	void loadConfigParam(const char *paramName, char *paramValue);
	char *saveConfigParam(const char *paramName);
	const char **getConfigParamNames();
private:
	int m_started;
	CConfigDialog * m_configDialog;
	CConfigData *m_configData;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_bankER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
