#if !defined(AFX_GROUPBOXEX_H__7560D496_6B10_42BE_8634_8428203E63E6__INCLUDED_)
#define AFX_GROUPBOXEX_H__7560D496_6B10_42BE_8634_8428203E63E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupBoxEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupBoxEx window

class CGroupBoxEx : public CWnd
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

#endif // !defined(AFX_GROUPBOXEX_H__7560D496_6B10_42BE_8634_8428203E63E6__INCLUDED_)
