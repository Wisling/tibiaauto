// mod_grouping.h : main header file for the MOD_GROUPING DLL
//

#if !defined(AFX_MOD_GROUPING_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_GROUPING_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <IModuleInterface.h>
#include "ConfigData.h"

/////////////////////////////////////////////////////////////////////////////
// CMod_groupingApp
// See mod_grouping.cpp for the implementation of this class
//
 
class CMod_groupingApp : public CWinApp, public IModuleInterface
{
public:
	CMod_groupingApp();
	~CMod_groupingApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMod_groupingApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMod_groupingApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:	
	// functions from IModuleInterface
	char * getName();
	int isStarted();
	void start();
	void stop();
	char *getVersion();
private:	
	int m_started;
	CConfigData *m_configData;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_GROUPING_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
