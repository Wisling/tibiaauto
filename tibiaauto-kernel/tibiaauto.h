// tibiaauto.h : main header file for the TIBIAAUTO application
//

#if !defined(AFX_TIBIAAUTO_H__8FBFFE53_4F68_4A78_AE4A_54000BBD5A77__INCLUDED_)
#define AFX_TIBIAAUTO_H__8FBFFE53_4F68_4A78_AE4A_54000BBD5A77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif // ifndef __AFXWIN_H__

#include "resource.h"           // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoApp:
// See tibiaauto.cpp for the implementation of this class
//

class CTibiaautoApp : public CWinApp
{
public:
	CTibiaautoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTibiaautoApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTibiaautoApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIBIAAUTO_H__8FBFFE53_4F68_4A78_AE4A_54000BBD5A77__INCLUDED_)
