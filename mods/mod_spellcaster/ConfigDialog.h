#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_spellcaster.h"
#include "ConfigData.h"

class CMod_spellcasterApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public CDialog
{
// Construction
public:
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	CConfigDialog(CMod_spellcasterApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CEdit	m_summonLessThan;
	CButton	m_mana;
	CEdit	m_manaMana;
	CEdit	m_manaSpell;
	CButton	m_life;
	CButton	m_customSpell;
	CEdit	m_lifeHp;
	CEdit	m_lifeSpell;
	CEdit	m_lifeSpellMana;
	CButton	m_exuraSpell;
	CEdit	m_exuraHp;
	CEdit	m_exuraSpellMana;
	CButton	m_granSpell;
	CEdit	m_granHp;
	CEdit	m_granSpellMana;
	CButton	m_vitaSpell;
	CEdit	m_vitaHp;
	CEdit	m_vitaSpellMana;
	CButton	m_paralysisSpell;
	CButton	m_poisonSpell;
	CEdit	m_minPoisonDmg;
	CButton	m_summon;
	CEdit	m_summonMana;
	CEdit	m_summonName;
	CButton	m_strike;
	CButton	m_flam;
	CButton	m_frigo;
	CButton	m_mort;
	CButton	m_tera;
	CButton	m_vis;
	CButton	m_con;
	CButton	m_san;
	CButton	m_hur;
	CEdit	m_manaStrike;
	CEdit	m_defaultStrikeSpell;
	CEdit	m_strikeSpellHpMin;
	CButton	m_enable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDialog)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnEnable();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnToolSpellcasterMana();
	afx_msg void OnToolSpellcasterLife();
	afx_msg void OnToolSpellcasterExura();
	afx_msg void OnToolSpellcasterGran();
	afx_msg void OnToolSpellcasterVita();
	afx_msg void OnToolSpellcasterPoison();
	afx_msg void OnToolSpellcasterCustom();
	afx_msg void OnToolSpellcasterSummon();
	afx_msg void OnToolSpellcasterStrike();
	afx_msg void OnToolSpellcasterMageStrike();
	afx_msg void OnToolSpellcasterPaladinStrike();
	afx_msg void OnToolSpellcasterKnightStrike();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CMod_spellcasterApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
