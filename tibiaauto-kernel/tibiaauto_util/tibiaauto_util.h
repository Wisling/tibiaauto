// tibiaauto_util.h : main header file for the TIBIAAUTO_UTIL DLL
//

#if !defined(AFX_TIBIAAUTO_UTIL_H__C560839E_24A7_434C_BB41_0F361F6FE69A__INCLUDED_)
#define AFX_TIBIAAUTO_UTIL_H__C560839E_24A7_434C_BB41_0F361F6FE69A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif // ifndef __AFXWIN_H__

#include "resource.h"           // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTibiaauto_utilApp
// See tibiaauto_util.cpp for the implementation of this class
//

class CTibiaauto_utilApp : public CWinApp
{
public:
	CTibiaauto_utilApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTibiaauto_utilApp)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTibiaauto_utilApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIBIAAUTO_UTIL_H__C560839E_24A7_434C_BB41_0F361F6FE69A__INCLUDED_)
