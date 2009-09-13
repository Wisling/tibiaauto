#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_spellcaster.h"
#include "ConfigData.h"
#include "MyDialog.h"
#include "BtnST.h"
#include "GroupBoxEx.h"

class CMod_spellcasterApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public MyDialog
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
	CGroupBoxEx	m_SummonControlFrame;
	CGroupBoxEx	m_StrikeSpellsFrame;
	CGroupBoxEx	m_SorcererAOEFrame;
	CGroupBoxEx	m_PaladinStrikeFrame;
	CGroupBoxEx	m_PaladinAOEFrame;
	CGroupBoxEx	m_ManaFrame;
	CGroupBoxEx	m_LifeFrame;
	CGroupBoxEx	m_MageStrikeFrame;
	CGroupBoxEx	m_KnightStrikeFrame;
	CGroupBoxEx	m_KnightAOEFrame;
	CGroupBoxEx	m_DruidAOEFrame;
	CGroupBoxEx	m_AOESpellsFrame;
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
	CButton	m_sioSpell;
	CEdit	m_sioHp;
	CEdit	m_sioSpellMana;
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
	CButton m_aoe;
	CEdit	m_aoeAffect;
	CButton m_exori;
	CButton m_exoriGran;
	CButton m_exoriMas;
	CButton m_exevoMasSan;
	CButton m_exevoFlamHur;
	CButton m_exevoFrigoHur;
	CButton m_exevoTeraHur;
	CButton m_exevoVisHur;
	CButton m_exevoVisLux;
	CButton m_exevoGranVisLux;
	CButton m_exevoGranMasVis;
	CButton m_exevoGranMasFlam;
	CButton m_exevoGranMasTera;
	CButton m_exevoGranMasFrigo;
	CButton	m_disableWarning;
	CButton m_randomCast;//new
	CButtonST	m_enable;
	CButtonST	m_healList;
	CButtonST	m_OK;
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
	afx_msg void OnToolSpellcasterSio();
	afx_msg void OnToolSpellcasterPoison();
	afx_msg void OnToolSpellcasterCustom();
	afx_msg void OnToolSpellcasterSummon();
	afx_msg void OnToolSpellcasterStrike();
	afx_msg void OnToolSpellcasterMageStrike();
	afx_msg void OnToolSpellcasterPaladinStrike();
	afx_msg void OnToolSpellcasterKnightStrike();
	afx_msg void OnToolSpellcasterAOE();
	afx_msg void OnToolSpellcasterSorcererAOE();
	afx_msg void OnToolSpellcasterDruidAOE();
	afx_msg void OnToolSpellcasterPaladinAOE();
	afx_msg void OnToolSpellcasterKnightAOE();
	afx_msg void OnToolSpellcasterHealList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CMod_spellcasterApp * m_app;
	char memWhiteList[100][32];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
