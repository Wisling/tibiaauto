// mod_autogo.h : main header file for the MOD_AUTOGO DLL
//

#if !defined(AFX_MOD_AUTOGO_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_AUTOGO_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <IModuleInterface.h>
#include "ConfigDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMod_autogoApp
// See mod_autogo.cpp for the implementation of this class
//

struct tibiaMessage
{
	int type;
	int chanType;
	char nick[128];
	char msg[1000];
};

int actionPos2ID(int);
struct tibiaMessage *triggerMessage();

class CMod_autogoApp : public CWinApp, public IModuleInterface
{
public:
	CMod_autogoApp();
	~CMod_autogoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMod_autogoApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMod_autogoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void getNewSkin(CSkin);
	void resetMultiParamAccess(char *paramName);
	int isMultiParam(char *paramName);
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
	void loadConfigParam(char *paramName,char *paramValue);
	char *saveConfigParam(char *paramName);
	char *getConfigParamName(int nr);
private:
	int m_started;
	int currentPos;
	list<Alarm>::iterator currentAlarmPos;
	CConfigDialog * m_configDialog;
	CConfigData *m_configData;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_AUTOGO_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
