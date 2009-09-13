#if !defined(AFX_MOD_MONSTERSHOW_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_MONSTERSHOW_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <IModuleInterface.h>
#include "ConfigData.h"
#include "ToolMonsterShow.h"
 
/////////////////////////////////////////////////////////////////////////////
//

class CMod_monstershowApp : public CWinApp, public IModuleInterface
{
public:
	CMod_monstershowApp();
	~CMod_monstershowApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMod_monstershowApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMod_monstershowApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:	
	void getNewSkin(CSkin);
	// functions from IModuleInterface
	char * getName();
	void showConfigDialog();
	char *getVersion();
	void resetConfig();
	char *getConfigParamName(int nr);
private:	
	CConfigData *m_configData;	
	CToolMonsterShow *m_infoDialog;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_MONSTERSHOW_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
