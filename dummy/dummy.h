// dummy.h : main header file for the DUMMY application
//

#if !defined(AFX_DUMMY_H__F5E80F44_E6FF_4612_8DA7_3AD53EFB479B__INCLUDED_)
#define AFX_DUMMY_H__F5E80F44_E6FF_4612_8DA7_3AD53EFB479B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDummyApp:
// See dummy.cpp for the implementation of this class
//

class CDummyApp : public CWinApp
{
public:
	CDummyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDummyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDummyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMMY_H__F5E80F44_E6FF_4612_8DA7_3AD53EFB479B__INCLUDED_)
