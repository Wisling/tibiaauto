#include "stdafx.h"
#include "mod_spellcaster.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_spellcasterApp *app,CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON_LESSTHAN, m_summonLessThan);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MANA, m_mana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MANA_MANA, m_manaMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MANA_SPELL, m_manaSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE, m_life);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_CUSTOM, m_customSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_HP, m_lifeHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_SPELL, m_lifeSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_SPELL_MANA, m_lifeSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXURA, m_exuraSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXURA_LIFE, m_exuraHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXURA_MANA, m_exuraSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_GRAN, m_granSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_GRAN_LIFE, m_granHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_GRAN_MANA, m_granSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_VITA, m_vitaSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_VITA_LIFE, m_vitaHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_VITA_MANA, m_vitaSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HEAL_POISON, m_poisonSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HEAL_PARALYSIS, m_paralysisSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MIN_POISON_DAMAGE, m_minPoisonDmg);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON, m_summon);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON_MANA, m_summonMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON_NAME, m_summonName);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE, m_strike);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_FLAM, m_flam);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_FRIGO, m_frigo);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MORT, m_mort);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_TERA, m_tera);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_VIS, m_vis);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_MANA, m_manaStrike);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_SPELL_DEFAULT, m_defaultStrikeSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_SPELL_DEFAULT_HP_MIN, m_strikeSpellHpMin);

	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_MANA, OnToolSpellcasterMana)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_LIFE, OnToolSpellcasterLife)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_CUSTOM, OnToolSpellcasterCustom)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXURA, OnToolSpellcasterExura)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_GRAN, OnToolSpellcasterGran)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_VITA, OnToolSpellcasterVita)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_SUMMON, OnToolSpellcasterSummon)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_STRIKE, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_HEAL_POISON, OnToolSpellcasterPoison)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK() 
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable() 
{
	if (m_enable.GetCheck())
	{
		m_app->controlsToConfig();
		if (m_app->validateConfig(1))
		{			
			m_app->start();
		} else {
			m_enable.SetCheck(0);
		}
	} else {
		m_app->stop(); 
	}
}

void CConfigDialog::disableControls()
{
	m_mana.EnableWindow(false);
	m_manaMana.EnableWindow(false);
	m_manaSpell.EnableWindow(false);
	m_customSpell.EnableWindow(false);
	m_life.EnableWindow(false);
	m_exuraSpell.EnableWindow(false);
	m_exuraHp.EnableWindow(false);
	m_exuraSpellMana.EnableWindow(false);
	m_granHp.EnableWindow(false);
	m_granSpell.EnableWindow(false);
	m_granSpellMana.EnableWindow(false);
	m_vitaSpell.EnableWindow(false);
	m_vitaHp.EnableWindow(false);
	m_vitaSpellMana.EnableWindow(false);
	m_lifeSpell.EnableWindow(false);
	m_lifeHp.EnableWindow(false);
	m_lifeSpell.EnableWindow(false);
	m_lifeSpellMana.EnableWindow(false);
	m_poisonSpell.EnableWindow(false);
	m_paralysisSpell.EnableWindow(false);
	m_minPoisonDmg.EnableWindow(false);
	m_summon.EnableWindow(false);
	m_summonLessThan.EnableWindow(false);
	m_summonMana.EnableWindow(false);
	m_summonName.EnableWindow(false);
	m_strike.EnableWindow(false);
	m_flam.EnableWindow(false);
	m_frigo.EnableWindow(false);
	m_mort.EnableWindow(false);
	m_tera.EnableWindow(false);
	m_vis.EnableWindow(false);
	m_manaStrike.EnableWindow(false);
	m_defaultStrikeSpell.EnableWindow(false);
	m_strikeSpellHpMin.EnableWindow(false);

}

void CConfigDialog::enableControls()
{
	m_mana.EnableWindow(true);
	if (m_mana.GetCheck()){
		m_manaMana.EnableWindow(true);
		m_manaSpell.EnableWindow(true);
	}
	m_life.EnableWindow(true);
	if (m_life.GetCheck()){
		m_customSpell.EnableWindow(true);
		OnToolSpellcasterCustom();
		m_exuraSpell.EnableWindow(true);
		OnToolSpellcasterExura();
		m_granSpell.EnableWindow(true);
		OnToolSpellcasterGran();
		m_vitaSpell.EnableWindow(true);
		OnToolSpellcasterVita();
		m_paralysisSpell.EnableWindow(true);
		m_poisonSpell.EnableWindow(true);
		OnToolSpellcasterPoison();
	}
	m_summon.EnableWindow(true);
	if (m_summon.GetCheck()){
		m_summonLessThan.EnableWindow(true);
		m_summonMana.EnableWindow(true);
		m_summonName.EnableWindow(true);
	}
	m_strike.EnableWindow(true);
		if (m_strike.GetCheck()){
			m_flam.EnableWindow(true);
			m_frigo.EnableWindow(true);
			m_mort.EnableWindow(true);
			m_tera.EnableWindow(true);
			m_vis.EnableWindow(true);
			m_manaStrike.EnableWindow(true);
			m_defaultStrikeSpell.EnableWindow(true);
			m_strikeSpellHpMin.EnableWindow(true);
	}
}



void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];
	m_mana.SetCheck(configData->mana);
	sprintf(buf,"%d",configData->manaMana);			m_manaMana.SetWindowText(buf);
	sprintf(buf,"%s",configData->manaSpell);		m_manaSpell.SetWindowText(buf);
	m_life.SetCheck(configData->life);
	m_customSpell.SetCheck(configData->customSpell);
	sprintf(buf,"%d",configData->lifeHp);			m_lifeHp.SetWindowText(buf);
	sprintf(buf,"%s",configData->lifeSpell);		m_lifeSpell.SetWindowText(buf);	
	sprintf(buf,"%d",configData->lifeSpellMana);	m_lifeSpellMana.SetWindowText(buf);
	m_exuraSpell.SetCheck(configData->exuraSpell);
	sprintf(buf,"%d",configData->exuraHp);			m_exuraHp.SetWindowText(buf);
	sprintf(buf,"%d",configData->exuraSpellMana);	m_exuraSpellMana.SetWindowText(buf);
	m_granSpell.SetCheck(configData->granSpell);
	sprintf(buf,"%d",configData->granHp);			m_granHp.SetWindowText(buf);
	sprintf(buf,"%d",configData->granSpellMana);	m_granSpellMana.SetWindowText(buf);
	m_vitaSpell.SetCheck(configData->vitaSpell);
	sprintf(buf,"%d",configData->vitaHp);			m_vitaHp.SetWindowText(buf);
	sprintf(buf,"%d",configData->vitaSpellMana);	m_vitaSpellMana.SetWindowText(buf);
	m_paralysisSpell.SetCheck(configData->paralysisSpell);
	m_poisonSpell.SetCheck(configData->poisonSpell);
	sprintf(buf,"%d",configData->minPoisonDmg);		m_minPoisonDmg.SetWindowText(buf);
	m_summon.SetCheck(configData->summon);
	sprintf(buf,"%d",configData->summonLessThan);	m_summonLessThan.SetWindowText(buf);
	sprintf(buf,"%s",configData->summonName);		m_summonName.SetWindowText(buf);	
	sprintf(buf,"%d",configData->summonMana);		m_summonMana.SetWindowText(buf);
	m_strike.SetCheck(configData->strike);
	m_flam.SetCheck(configData->flam);
	m_frigo.SetCheck(configData->frigo);
	m_mort.SetCheck(configData->mort);
	m_tera.SetCheck(configData->tera);
	m_vis.SetCheck(configData->vis);
	sprintf(buf,"%d",configData->manaStrike);		m_manaStrike.SetWindowText(buf);
	sprintf(buf,"%s",configData->defaultStrikeSpell);		m_defaultStrikeSpell.SetWindowText(buf);
	sprintf(buf,"%d",configData->strikeSpellHpMin);		m_strikeSpellHpMin.SetWindowText(buf);

	OnToolSpellcasterMana();
	OnToolSpellcasterLife();
	OnToolSpellcasterSummon();
	OnToolSpellcasterStrike();
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	newConfigData->mana = m_mana.GetCheck();
	m_manaMana.GetWindowText(buf,127);newConfigData->manaMana=atoi(buf);
	m_manaSpell.GetWindowText(newConfigData->manaSpell,127);

	newConfigData->life = m_life.GetCheck();
	newConfigData->customSpell = m_customSpell.GetCheck();
	m_lifeHp.GetWindowText(buf,127);newConfigData->lifeHp=atoi(buf);
	m_lifeSpell.GetWindowText(newConfigData->lifeSpell,127);
	m_lifeSpellMana.GetWindowText(buf,127); newConfigData->lifeSpellMana=atoi(buf);
	newConfigData->exuraSpell = m_exuraSpell.GetCheck();
	m_exuraHp.GetWindowText(buf,127);newConfigData->exuraHp=atoi(buf);
	m_exuraSpellMana.GetWindowText(buf,127); newConfigData->exuraSpellMana=atoi(buf);
	newConfigData->granSpell = m_granSpell.GetCheck();
	m_granHp.GetWindowText(buf,127);newConfigData->granHp=atoi(buf);
	m_granSpellMana.GetWindowText(buf,127); newConfigData->granSpellMana=atoi(buf);
	newConfigData->vitaSpell = m_vitaSpell.GetCheck();
	m_vitaHp.GetWindowText(buf,127);newConfigData->vitaHp=atoi(buf);
	m_vitaSpellMana.GetWindowText(buf,127); newConfigData->vitaSpellMana=atoi(buf);
	newConfigData->paralysisSpell = m_paralysisSpell.GetCheck();
	newConfigData->poisonSpell = m_poisonSpell.GetCheck();
	m_minPoisonDmg.GetWindowText(buf,127);newConfigData->minPoisonDmg=atoi(buf);

	newConfigData->summon = m_summon.GetCheck();
	m_summonLessThan.GetWindowText(buf,127);newConfigData->summonLessThan=atoi(buf);
	m_summonMana.GetWindowText(buf,127);newConfigData->summonMana=atoi(buf);
	m_summonName.GetWindowText(newConfigData->summonName,127);

	newConfigData->strike = m_strike.GetCheck();
	newConfigData->flam = m_flam.GetCheck();
	newConfigData->frigo = m_frigo.GetCheck();
	newConfigData->mort = m_mort.GetCheck();
	newConfigData->tera = m_tera.GetCheck();
	newConfigData->vis = m_vis.GetCheck();
	m_manaStrike.GetWindowText(buf,127);newConfigData->manaStrike=atoi(buf);
	m_defaultStrikeSpell.GetWindowText(newConfigData->defaultStrikeSpell,127);
	m_strikeSpellHpMin.GetWindowText(buf,127);newConfigData->strikeSpellHpMin=atoi(buf);

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) 
{

	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnToolSpellcasterMana();
	OnToolSpellcasterLife();
	OnToolSpellcasterSummon();
	OnToolSpellcasterStrike();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg) 
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}

void CConfigDialog::OnToolSpellcasterMana() 
{
	int val = m_mana.GetCheck();
	m_manaMana.EnableWindow(val);
	m_manaSpell.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterLife() 
{
	int val = m_life.GetCheck();
		int exuraVal = m_exuraSpell.GetCheck();
		int granVal = m_granSpell.GetCheck();
		int vitaVal = m_vitaSpell.GetCheck();
		int customVal = m_customSpell.GetCheck();
		int poisonVal = m_poisonSpell.GetCheck();
			m_exuraSpell.EnableWindow(val);
			m_exuraHp.EnableWindow(val && exuraVal?true:false);
				m_exuraSpellMana.EnableWindow(val && exuraVal?true:false);
			m_granSpell.EnableWindow(val);
				m_granHp.EnableWindow(val && granVal?true:false);
				m_granSpellMana.EnableWindow(val && granVal?true:false);
			m_vitaSpell.EnableWindow(val);
				m_vitaHp.EnableWindow(val && vitaVal?true:false);
				m_vitaSpellMana.EnableWindow(val && vitaVal?true:false);
			m_customSpell.EnableWindow(val);
				m_lifeHp.EnableWindow(val && customVal?true:false);
				m_lifeSpell.EnableWindow(val && customVal?true:false);
				m_lifeSpellMana.EnableWindow(val && customVal?true:false);
			m_paralysisSpell.EnableWindow(val);
			m_poisonSpell.EnableWindow(val);
				m_minPoisonDmg.EnableWindow(val && poisonVal?true:false);

}

void CConfigDialog::OnToolSpellcasterSummon() 
{
	int val = m_summon.GetCheck();
	m_summonLessThan.EnableWindow(val);
	m_summonMana.EnableWindow(val);
	m_summonName.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterStrike() 
{
	int val = m_strike.GetCheck();
		m_flam.EnableWindow(val);
		m_frigo.EnableWindow(val);
		m_mort.EnableWindow(val);
		m_tera.EnableWindow(val);
		m_vis.EnableWindow(val);
		m_manaStrike.EnableWindow(val);
		m_defaultStrikeSpell.EnableWindow(val);
		m_strikeSpellHpMin.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterExura() 
{
	int val = m_exuraSpell.GetCheck();
		m_exuraHp.EnableWindow(val);
		m_exuraSpellMana.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterGran() 
{
	int val = m_granSpell.GetCheck();
		m_granHp.EnableWindow(val);
		m_granSpellMana.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterVita() 
{
	int val = m_vitaSpell.GetCheck();
		m_vitaHp.EnableWindow(val);
		m_vitaSpellMana.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterCustom() 
{
	int val = m_customSpell.GetCheck();
		m_lifeHp.EnableWindow(val);
		m_lifeSpell.EnableWindow(val);
		m_lifeSpellMana.EnableWindow(val);

}
void CConfigDialog::OnToolSpellcasterPoison() 
{
	int val = m_poisonSpell.GetCheck();
		m_minPoisonDmg.EnableWindow(val);
}

