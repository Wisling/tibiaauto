#if !defined(AFX_TIMEDDIALOG_H__D54C0336_A54A_4EDF_92D3_FD6F72E0A58C__INCLUDED_)
#define AFX_TIMEDDIALOG_H__D54C0336_A54A_4EDF_92D3_FD6F72E0A58C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyDialog.h"
#include "GroupBoxEx.h"
// TimedDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TimedDialog dialog

class TimedDialog : public MyDialog
{
// Construction
public:
	std::vector<TimedSpell> spellList;
	void controlsToConfig(CConfigData *);
	void configToControls(CConfigData *);
	void enableControls();
	void disableControls();
	void DoSetButtonSkin();
	TimedDialog(std::vector<TimedSpell>, CWnd* pParent = NULL);
	TimedDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TimedDialog)
	enum { IDD = IDD_TIMED_DIALOG };
	CListCtrl	m_timedSpellList;
	CButtonST	m_timedSpellDelete;
	CButtonST	m_timedSpellAdd;
	CButton	m_UsePotions;
	CEdit	m_timedSpellMana;
	CEdit	m_timedSpellDelay;
	CEdit	m_timedSpellWords;
	CButton	m_timedSpell;
	CGroupBoxEx	m_timedSpellFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TimedDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TimedDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimedSpellAdd();
	afx_msg void OnTimedSpellDelete();
	afx_msg void OnToolSpellcasterTimed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDDIALOG_H__D54C0336_A54A_4EDF_92D3_FD6F72E0A58C__INCLUDED_)
