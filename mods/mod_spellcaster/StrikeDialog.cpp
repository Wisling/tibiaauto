// StrikeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_spellcaster.h"
#include "StrikeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StrikeDialog dialog


StrikeDialog::StrikeDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(StrikeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(StrikeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void StrikeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StrikeDialog)
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_SPELL_DEFAULT, m_defaultStrikeSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_VIS, m_vis);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_TERA, m_tera);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_SPELL_DEFAULT_HP_MIN, m_strikeSpellHpMin);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_MANA, m_manaStrike);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE, m_strike);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SAN, m_san);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MORT, m_mort);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HUR, m_hur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_FRIGO, m_frigo);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_FLAM, m_flam);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_CON, m_con);
	DDX_Control(pDX, IDC_FRAME_STRIKE_SPELLS, m_StrikeSpellsFrame);
	DDX_Control(pDX, IDC_FRAME_PALADIN_STRIKE, m_PaladinStrikeFrame);
	DDX_Control(pDX, IDC_FRAME_MAGE_STRIKE, m_MageStrikeFrame);
	DDX_Control(pDX, IDC_FRAME_KNIGHT_STRIKE, m_KnightStrikeFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StrikeDialog, CDialog)
	//{{AFX_MSG_MAP(StrikeDialog)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_STRIKE, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_FLAM, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_FRIGO, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_MORT, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_TERA, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_VIS, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_CON, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_SAN, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_HUR, OnToolSpellcasterStrike)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StrikeDialog message handlers

void StrikeDialog::OnToolSpellcasterStrike() {
	int val = m_strike.GetCheck();
		OnToolSpellcasterMageStrike();
		OnToolSpellcasterPaladinStrike();
		OnToolSpellcasterKnightStrike();
		m_manaStrike.EnableWindow(val);
		m_defaultStrikeSpell.EnableWindow(val);
		m_strikeSpellHpMin.EnableWindow(val);	
}

void StrikeDialog::disableControls() {
	m_strike.EnableWindow(false);
	m_flam.EnableWindow(false);
	m_frigo.EnableWindow(false);
	m_mort.EnableWindow(false);
	m_tera.EnableWindow(false);
	m_vis.EnableWindow(false);
	m_con.EnableWindow(false);
	m_san.EnableWindow(false);
	m_hur.EnableWindow(false);
	m_manaStrike.EnableWindow(false);
	m_defaultStrikeSpell.EnableWindow(false);
	m_strikeSpellHpMin.EnableWindow(false);
}

void StrikeDialog::enableControls() {
	m_strike.EnableWindow(true);
	if (m_strike.GetCheck()){
		OnToolSpellcasterMageStrike();
		OnToolSpellcasterPaladinStrike();
		OnToolSpellcasterKnightStrike();
		m_manaStrike.EnableWindow(true);
		m_defaultStrikeSpell.EnableWindow(true);
		m_strikeSpellHpMin.EnableWindow(true);
	}
}

void StrikeDialog::configToControls(CConfigData *configData) {
	char buf[128];
	m_strike.SetCheck(configData->strike);
	m_flam.SetCheck(configData->flam);
	m_frigo.SetCheck(configData->frigo);
	m_mort.SetCheck(configData->mort);
	m_tera.SetCheck(configData->tera);
	m_vis.SetCheck(configData->vis);
	m_con.SetCheck(configData->con);
	m_san.SetCheck(configData->san);
	m_hur.SetCheck(configData->hur);
	sprintf(buf,"%d",configData->manaStrike);		m_manaStrike.SetWindowText(buf);
	sprintf(buf,"%s",configData->defaultStrikeSpell);		m_defaultStrikeSpell.SetWindowText(buf);
	sprintf(buf,"%d",configData->strikeSpellHpMin);		m_strikeSpellHpMin.SetWindowText(buf);
	OnToolSpellcasterStrike();
}

void StrikeDialog::controlsToConfig(CConfigData *newConfigData) {
	char buf[128];
	newConfigData->strike = m_strike.GetCheck();
	newConfigData->flam = m_flam.GetCheck();
	newConfigData->frigo = m_frigo.GetCheck();
	newConfigData->mort = m_mort.GetCheck();
	newConfigData->tera = m_tera.GetCheck();
	newConfigData->vis = m_vis.GetCheck();
	newConfigData->con = m_con.GetCheck();
	newConfigData->san = m_san.GetCheck();
	newConfigData->hur = m_hur.GetCheck();
	m_manaStrike.GetWindowText(buf,127);newConfigData->manaStrike=atoi(buf);
	m_defaultStrikeSpell.GetWindowText(newConfigData->defaultStrikeSpell,127);
	m_strikeSpellHpMin.GetWindowText(buf,127);newConfigData->strikeSpellHpMin=atoi(buf);
}

BOOL StrikeDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	
	OnToolSpellcasterStrike();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void StrikeDialog::OnToolSpellcasterMageStrike() {
	if (m_con.GetCheck() || m_san.GetCheck() || m_hur.GetCheck() || !m_strike.GetCheck()) {
		m_flam.EnableWindow(false);
		m_frigo.EnableWindow(false);
		m_mort.EnableWindow(false);
		m_tera.EnableWindow(false);
		m_vis.EnableWindow(false);
	}
	else if (!m_flam.GetCheck() && !m_frigo.GetCheck() && !m_mort.GetCheck() && !m_tera.GetCheck() && !m_vis.GetCheck()) {
		m_hur.EnableWindow(true);
		m_flam.EnableWindow(true);
		m_frigo.EnableWindow(true);
		m_mort.EnableWindow(true);
		m_tera.EnableWindow(true);
		m_vis.EnableWindow(true);
		m_san.EnableWindow(true);
		m_con.EnableWindow(true);
	}
	else {
		m_flam.EnableWindow(true);
		m_frigo.EnableWindow(true);
		m_mort.EnableWindow(true);
		m_tera.EnableWindow(true);
		m_vis.EnableWindow(true);
		m_san.EnableWindow(false);
		m_con.EnableWindow(false);
		m_hur.EnableWindow(false);
	}
}

void StrikeDialog::OnToolSpellcasterPaladinStrike() {
	if (m_flam.GetCheck() || m_frigo.GetCheck() || m_mort.GetCheck() || m_tera.GetCheck() || m_vis.GetCheck() || m_hur.GetCheck() || !m_strike.GetCheck()){
		m_san.EnableWindow(false);
		m_con.EnableWindow(false);
		m_hur.EnableWindow(false);
	}
	else if (!m_san.GetCheck() && !m_con.GetCheck()) {
		m_hur.EnableWindow(true);
		m_flam.EnableWindow(true);
		m_frigo.EnableWindow(true);
		m_mort.EnableWindow(true);
		m_tera.EnableWindow(true);
		m_vis.EnableWindow(true);
		m_san.EnableWindow(true);
		m_con.EnableWindow(true);
	}
	else {
		m_san.EnableWindow(true);
		m_con.EnableWindow(true);
		m_flam.EnableWindow(false);
		m_frigo.EnableWindow(false);
		m_mort.EnableWindow(false);
		m_tera.EnableWindow(false);
		m_vis.EnableWindow(false);
		m_hur.EnableWindow(false);
	}
}

void StrikeDialog::OnToolSpellcasterKnightStrike() {
	if (m_flam.GetCheck() || m_frigo.GetCheck() || m_mort.GetCheck() || m_tera.GetCheck() || m_vis.GetCheck() || m_con.GetCheck() || m_san.GetCheck() || !m_strike.GetCheck()){
		m_hur.EnableWindow(false);
	}
	else if (!m_hur.GetCheck()) {
		m_hur.EnableWindow(true);
		m_flam.EnableWindow(true);
		m_frigo.EnableWindow(true);
		m_mort.EnableWindow(true);
		m_tera.EnableWindow(true);
		m_vis.EnableWindow(true);
		m_san.EnableWindow(true);
		m_con.EnableWindow(true);
	}
	else {
		m_hur.EnableWindow(true);
		m_flam.EnableWindow(false);
		m_frigo.EnableWindow(false);
		m_mort.EnableWindow(false);
		m_tera.EnableWindow(false);
		m_vis.EnableWindow(false);
		m_san.EnableWindow(false);
		m_con.EnableWindow(false);
	}
}
