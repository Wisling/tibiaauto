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
#endif // ifdef _DEBUG

int tprogrammingEdition = 0;
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
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	if (!AfxSocketInit())
		AfxMessageBox("Failed to Initialize Sockets", MB_OK | MB_ICONSTOP);
	CTibiaautoDlg* dlg = new CTibiaautoDlg();
	m_pMainWnd = dlg;
	dlg->Create(CTibiaautoDlg::IDD);
	// No need to show the window as it shows itself.

	return TRUE;
}
