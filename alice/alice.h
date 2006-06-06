// alice.h : main header file for the ALICE DLL
//

#if !defined(AFX_ALICE_H__B192649E_2135_42C0_AF00_36005281832E__INCLUDED_)
#define AFX_ALICE_H__B192649E_2135_42C0_AF00_36005281832E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAliceApp
// See alice.cpp for the implementation of this class
//

class CAliceApp : public CWinApp
{
public:
	CAliceApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAliceApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAliceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALICE_H__B192649E_2135_42C0_AF00_36005281832E__INCLUDED_)
