#if !defined(AFX_SUMMONDIALOG_H__43D8EAA3_BF4F_4236_B3C3_81B54B939205__INCLUDED_)
#define AFX_SUMMONDIALOG_H__43D8EAA3_BF4F_4236_B3C3_81B54B939205__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyDialog.h"
#include "GroupBoxEx.h"

// SummonDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SummonDialog dialog

class SummonDialog : public MyDialog
{
// Construction
public:
	void controlsToConfig(CConfigData * );
	void configToControls(CConfigData *);
	void enableControls();
	void disableControls();
	SummonDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SummonDialog)
	enum { IDD = IDD_SUMMON_DIALOG };
	CListCtrl	m_creatureList;
	CEdit	m_summonName;
	CEdit	m_summonMana;
	CButton	m_summon;
	CEdit	m_summonLessThan;
	CGroupBoxEx	m_SummonFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SummonDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_LargeImg;
	// Generated message map functions
	//{{AFX_MSG(SummonDialog)
	afx_msg void OnToolSpellcasterSummon();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangingCreatureList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUMMONDIALOG_H__43D8EAA3_BF4F_4236_B3C3_81B54B939205__INCLUDED_)
