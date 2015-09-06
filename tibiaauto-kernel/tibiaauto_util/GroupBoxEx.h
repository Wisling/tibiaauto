#pragma once

#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CGroupBoxEx : public CWnd
{
// Construction
public:
	CGroupBoxEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupBoxEx)
protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGroupBoxEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGroupBoxEx)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

