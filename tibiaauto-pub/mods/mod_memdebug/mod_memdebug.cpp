// mod_memdebug.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_memdebug.h"

#include "ConfigDialog.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMod_memdebugApp

BEGIN_MESSAGE_MAP(CMod_memdebugApp, CWinApp)
//{{AFX_MSG_MAP(CMod_memdebugApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMod_memdebugApp construction

CMod_memdebugApp::CMod_memdebugApp()
{
	m_started = 0;
}

CMod_memdebugApp::~CMod_memdebugApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
}

char * CMod_memdebugApp::getName()
{
	return "Mem debug";
}

int CMod_memdebugApp::isStarted()
{
	return m_started;
}

void CMod_memdebugApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

char *CMod_memdebugApp::getVersion()
{
	return "1.0";
}
