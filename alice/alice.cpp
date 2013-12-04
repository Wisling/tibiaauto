// alice.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "alice.h"
#include "Kernel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CAliceApp

BEGIN_MESSAGE_MAP(CAliceApp, CWinApp)
	//{{AFX_MSG_MAP(CAliceApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAliceApp construction

CAliceApp::CAliceApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAliceApp object

CAliceApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAliceApp initialization

BOOL CAliceApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////


int kernelInitDone=0;

void kernelInit()
{
	Kernel *kernel = new Kernel();
	
	kernel->bootstrap();
}


/////////////////////////////////////////////////////////////////////////////

char *kernelRespond(char *text,char *id)
{
	if (!kernelInitDone)
	{
		kernelInit();
		kernelInitDone=1;
	}
		

	char *ret=(char *)malloc(65536);
	memset(ret,0,65536);
	string aliceString = Kernel::respond(text,id);
	sprintf(ret,"%s",aliceString.c_str());
	return ret;
}
