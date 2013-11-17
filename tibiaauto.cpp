// tibiaauto.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "tibiaautoDlg.h"
#include "afxsock.h"
#include "md5class.h"

#ifdef _DEBUG
#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
#endif

int tprogrammingEdition=0;
/*
#include "c:\temp\HookAlloc\MemoryTracking.h"
USE_MEMORYTRACKING("c:\\temp\\Tibia_Tibia.log",true)
#pragma warning(disable:4073)
#pragma init_seg(lib)
*/
/////////////////////////////////////////////////////////////////////////////
// CTibiaautoApp

BEGIN_MESSAGE_MAP(CTibiaautoApp, CWinApp)
	//{{AFX_MSG_MAP(CTibiaautoApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoApp construction

CTibiaautoApp::CTibiaautoApp()

{	
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CTibiaautoApp object

CTibiaautoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoApp initialization

BOOL CTibiaautoApp::InitInstance()
{


	AfxEnableControlContainer();

	if(!AfxSocketInit())
      AfxMessageBox("Failed to Initialize Sockets",MB_OK| MB_ICONSTOP);


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CTibiaautoDlg dlg;
	m_pMainWnd = &dlg;	
	dlg.DoModal();
	dlg.~CTibiaautoDlg();
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


