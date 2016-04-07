#if !defined(AFX_MOD_ITEMCONFIG_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_ITEMCONFIG_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif // ifndef __AFXWIN_H__

#include "resource.h"           // main symbols
#include <IModuleInterface.h>
#include "ConfigData.h"
#include "ToolItemConfig.h"

/////////////////////////////////////////////////////////////////////////////
//

struct CGUITreeItemData {
	int id;
	int type;
	CGUITreeItemData(int a_id, int a_type)
	{
		id   = a_id;
		type = a_type;
	}
};

class CMod_itemconfigApp : public IModuleInterface
{
public:
	CMod_itemconfigApp();
	~CMod_itemconfigApp();
public:
	void getNewSkin(CSkin);
	// functions from IModuleInterface
	char * getName();
	int isStarted();
	void showConfigDialog();
	void enableControls();
	void configToControls();
	void controlsToConfig();
	void loadConfigParam(const char *paramName, char *paramValue);
	char *saveConfigParam(const char *paramName);
	const char **getConfigParamNames();
	int isMultiParam(const char *paramName);
	void resetMultiParamAccess(const char *paramName);
	char *getVersion();
private:
	CConfigData *m_configData;
	CToolItemConfig *m_infoDialog;
	int currentPos;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_ITEMCONFIG_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
