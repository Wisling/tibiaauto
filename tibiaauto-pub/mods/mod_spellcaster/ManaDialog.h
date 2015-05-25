#if !defined(AFX_MANADIALOG_H__5C72A12C_95FB_43D8_AA3C_DF4B97847C73__INCLUDED_)
#define AFX_MANADIALOG_H__5C72A12C_95FB_43D8_AA3C_DF4B97847C73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyDialog.h"
#include "GroupBoxEX.h"

// ManaDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ManaDialog dialog

class ManaDialog : public MyDialog
{
// Construction
public:
	void controlsToConfig(CConfigData *);
	void configToControls(CConfigData *);
	void enableControls();
	void disableControls();
	ManaDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ManaDialog)
	enum { IDD = IDD_MANA_DIALOG };
	CEdit m_manaSpell;
	CEdit m_manaMana;
	CButton m_mana;
	CGroupBoxEx m_ManaFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ManaDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ManaDialog)
	afx_msg void OnToolSpellcasterMana();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANADIALOG_H__5C72A12C_95FB_43D8_AA3C_DF4B97847C73__INCLUDED_)
