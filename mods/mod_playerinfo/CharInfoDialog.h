#if !defined(AFX_CHARINFODIALOG_H__469ECCB1_066B_43C0_A03D_CA28CE79A73E__INCLUDED_)
#define AFX_CHARINFODIALOG_H__469ECCB1_066B_43C0_A03D_CA28CE79A73E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CharInfoDialog.h : header file
//

#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CCharInfoDialog dialog

class CCharInfoDialog : public MyDialog
{
// Construction
public:
	CCharInfoDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCharInfoDialog)
	enum { IDD = IDD_CHARINFO };
	CButtonST	m_ResetCounters;
	CButtonST	m_OK;
	CGroupBoxEx	m_SpellStats;
	CGroupBoxEx	m_CharStats;
	CStatic	m_expUpin;
	CStatic	m_lvlUpin;
	CStatic	m_lvlSpeed;
	CStatic	m_mLvlUpin;
	CStatic	m_mLvlSpeed;
	CStatic	m_mLvl;
	CStatic	m_fistUpin;
	CStatic	m_fistSpeed;
	CStatic	m_fist;
	CStatic	m_time;
	CStatic	m_swordUpin;
	CStatic	m_shieldUpin;
	CStatic	m_fishUpin;
	CStatic	m_fishSpeed;
	CStatic	m_fish;
	CStatic	m_distUpin;
	CStatic	m_distSpeed;
	CStatic	m_axeUpin;
	CStatic	m_clubUpin;
	CStatic	m_dist;
	CStatic	m_shield;
	CStatic	m_sword;
	CStatic	m_swordSpeed;
	CStatic	m_shieldSpeed;
	CStatic	m_expSpeed;
	CStatic	m_clubSpeed;
	CStatic	m_axeSpeed;
	CStatic	m_axe;
	CStatic	m_club;
	CStatic	m_exp;
	CStatic m_lvl;
	CStatic m_invisRemaining;
	CStatic m_hasteRemaining;
	CStatic m_stronghasteRemaining;
	CStatic m_magicshieldRemaining;
	CStatic	m_mana;
	CStatic	m_hp;
	CStatic m_sp;
	CStatic m_cap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCharInfoDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCharInfoDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnResetCounters();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void dataShow();
	void dataCalc();
	void resetCounters();

	int counterStartInvisible;
	int counterStartHaste;
	int counterStartStrongHaste;
	int counterStartMagicShield;

	bool warnedInvisible1;
	bool warnedHaste1;
	bool warnedStrongHaste1;
	bool warnedMagicShield1;

	bool warnedInvisible2;
	bool warnedHaste2;
	bool warnedStrongHaste2;
	bool warnedMagicShield2;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHARINFODIALOG_H__469ECCB1_066B_43C0_A03D_CA28CE79A73E__INCLUDED_)
