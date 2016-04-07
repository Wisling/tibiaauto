#if !defined(AFX_MOD_SHOWMAP_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_SHOWMAP_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif // ifndef __AFXWIN_H__

#include "resource.h"           // main symbols
#include <IModuleInterface.h>
#include "ConfigData.h"
#include "ToolMapShow.h"

/////////////////////////////////////////////////////////////////////////////
//

class CMod_showmapApp : public CWinApp, public IModuleInterface
{
public:
	CMod_showmapApp();
	~CMod_showmapApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMod_showmapApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMod_showmapApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

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
	char *getVersion();
	void resetConfig();
	void loadConfigParam(const char *paramName, char *paramValue);
	char *saveConfigParam(const char *paramName);
	const char **getConfigParamNames();
private:
	CConfigData *m_configData;
	CToolMapShow *m_infoDialog;
	int currentPointNr;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_SHOWMAP_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
