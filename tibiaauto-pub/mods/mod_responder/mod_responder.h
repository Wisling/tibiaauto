#if !defined(AFX_MOD_RESPONDER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_RESPONDER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif // ifndef __AFXWIN_H__

#include "resource.h"           // main symbols
#include <IModuleInterface.h>
#include "ConfigData.h"
#include "ToolAutoRespond.h"

/////////////////////////////////////////////////////////////////////////////
//

class CMod_responderApp : public IModuleInterface
{
public:
	CMod_responderApp();
	~CMod_responderApp();
public:
	void getNewSkin(CSkin);
	void stop();
	void start();
	// functions from IModuleInterface
	char * getName();
	int isStarted();
	void showConfigDialog();
	void enableControls();
	char *getVersion();
private:
	CConfigData *m_configData;
	CToolAutoRespond *m_infoDialog;
	int currentPointNr;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_RESPONDER_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
