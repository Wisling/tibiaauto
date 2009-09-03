#if !defined(AFX_CHARDIALOG_H__6FC79594_AADF_4AD3_9CAD_53DD468ECCA5__INCLUDED_)
#define AFX_CHARDIALOG_H__6FC79594_AADF_4AD3_9CAD_53DD468ECCA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CharDialog.h : header file
//
#include "MyDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CCharDialog dialog

class CCharDialog : public MyDialog
{
// Construction
public:
	CCharDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCharDialog)
	enum { IDD = IDD_CHAR_DIALOG };
	CComboBox	m_charList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCharDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCharDialog)
	afx_msg void OnCharRefresh();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHARDIALOG_H__6FC79594_AADF_4AD3_9CAD_53DD468ECCA5__INCLUDED_)
