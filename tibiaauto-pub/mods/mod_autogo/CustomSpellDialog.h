#if !defined(AFX_CUSTOMSPELLDIALOG_H__92031B70_100F_44C0_B831_40FCDBC8A647__INCLUDED_)
#define AFX_CUSTOMSPELLDIALOG_H__92031B70_100F_44C0_B831_40FCDBC8A647__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomSpellDialog.h : header file
//
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnST.h"
/////////////////////////////////////////////////////////////////////////////
// CCustomSpellDialog dialog

class CCustomSpellDialog : public MyDialog
{
// Construction
public:
	CCustomSpellDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomSpellDialog)
	enum { IDD = IDD_ADD_CUSTOM_SPELL };
	CGroupBoxEx m_spellConfigFrame;
	CButtonST m_OK;
	CButtonST m_cancel;
	CString m_spellWords;
	int m_castingDelay;
	int m_manaCost;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomSpellDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomSpellDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMSPELLDIALOG_H__92031B70_100F_44C0_B831_40FCDBC8A647__INCLUDED_)
