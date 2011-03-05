#if !defined(AFX_LIFEDIALOG_H__D9593346_FB4A_483E_A67B_32EE204BD18B__INCLUDED_)
#define AFX_LIFEDIALOG_H__D9593346_FB4A_483E_A67B_32EE204BD18B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mydialog.h"

// LifeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LifeDialog dialog

class LifeDialog : public MyDialog
{
// Construction
public:
	void controlsToConfig(CConfigData *);
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	friend class CConfigDialog;
	LifeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(LifeDialog)
	enum { IDD = IDD_LIFE_DIALOG };
	CGroupBoxEx	m_lifeFrame;
	CButton	m_customSpell;
	CEdit	m_vitaSpellMana;
	CEdit	m_vitaHp;
	CButton	m_vitaSpell;
	CEdit	m_sioSpellMana;
	CButton	m_sioSpell;
	CEdit	m_minPoisonDmg;
	CEdit	m_lifeSpellMana;
	CEdit	m_lifeSpell;
	CButton	m_life;
	CEdit	m_lifeHp;
	CButton	m_exuraSpell;
	CEdit	m_exuraHp;
	CEdit	m_exuraSpellMana;
	CButton	m_paralysisSpell;
	CButton	m_paralysisIco;
	CButton	m_poisonSpell;
	CButtonST	m_healList;
	CButton	m_granSpell;
	CEdit	m_granHp;
	CEdit	m_granSpellMana;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LifeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(LifeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnToolSpellcasterLife();
	afx_msg void OnToolSpellcasterExura();
	afx_msg void OnToolSpellcasterGran();
	afx_msg void OnToolSpellcasterVita();
	afx_msg void OnToolSpellcasterSio();
	afx_msg void OnToolSpellcasterCustom();
	afx_msg void OnToolSpellcasterHealList();
	afx_msg void OnToolSpellcasterPoison();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	std::vector<Player> memHealList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIFEDIALOG_H__D9593346_FB4A_483E_A67B_32EE204BD18B__INCLUDED_)
