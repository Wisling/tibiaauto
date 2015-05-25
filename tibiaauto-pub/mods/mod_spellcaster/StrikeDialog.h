#if !defined(AFX_STRIKEDIALOG_H__8E9E2108_91A8_44DB_B0EA_098C2E027771__INCLUDED_)
#define AFX_STRIKEDIALOG_H__8E9E2108_91A8_44DB_B0EA_098C2E027771__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyDialog.h"
#include "GroupBoxEx.h"
// StrikeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StrikeDialog dialog

class StrikeDialog : public MyDialog
{
// Construction
public:
	void controlsToConfig(CConfigData *);
	void configToControls(CConfigData *);
	void enableControls();
	void disableControls();
	StrikeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(StrikeDialog)
	enum { IDD = IDD_STRIKE_DIALOG };
	CEdit m_defaultStrikeSpell;
	CButton m_vis;
	CButton m_tera;
	CEdit m_strikeSpellHpMin;
	CEdit m_manaStrike;
	CButton m_strike;
	CButton m_san;
	CButton m_mort;
	CButton m_hur;
	CButton m_frigo;
	CButton m_flam;
	CButton m_con;
	CGroupBoxEx m_StrikeSpellsFrame;
	CGroupBoxEx m_PaladinStrikeFrame;
	CGroupBoxEx m_MageStrikeFrame;
	CGroupBoxEx m_KnightStrikeFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StrikeDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(StrikeDialog)
	afx_msg void OnToolSpellcasterStrike();
	afx_msg void OnToolSpellcasterMageStrike();
	afx_msg void OnToolSpellcasterPaladinStrike();
	afx_msg void OnToolSpellcasterKnightStrike();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRIKEDIALOG_H__8E9E2108_91A8_44DB_B0EA_098C2E027771__INCLUDED_)
