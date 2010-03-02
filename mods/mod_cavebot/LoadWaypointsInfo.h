#if !defined(AFX_LOADWAYPOINTSINFO_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
#define AFX_LOADWAYPOINTSINFO_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadWaypointsInfo.h : header file
//
#include "MyDialog.h"
#include "BtnSt.h"
/////////////////////////////////////////////////////////////////////////////
// CLoadWaypointsInfo dialog

class CLoadWaypointsInfo : public MyDialog
{
// Construction
public:
	CLoadWaypointsInfo(char* a_prefix, CWnd* pParent = NULL);
	char* prefix;

// Dialog Data
	//{{AFX_DATA(CWhiteList)
	enum { IDD = IDD_LOADWAYPOINTSINFO };
	CGroupBoxEx m_frame;
	CEdit	m_prefix;
	CButtonST	m_CANCEL;
	CButtonST	m_YES;
	CButtonST	m_NO;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhiteList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWhiteList)
	afx_msg void OnYES();
	afx_msg void OnNO();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADWAYPOINTSINFO_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
