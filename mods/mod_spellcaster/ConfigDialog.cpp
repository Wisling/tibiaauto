#include "stdafx.h"
#include "mod_spellcaster.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "Whitelist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_spellcasterApp *app,CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
	memset(memWhiteList,0,3200);
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
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SIO, m_sioSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SIO_LIFE, m_sioHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SIO_MANA, m_sioSpellMana);
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
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_CON, m_con);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SAN, m_san);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HUR, m_hur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_MANA, m_manaStrike);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_SPELL_DEFAULT, m_defaultStrikeSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_STRIKE_SPELL_DEFAULT_HP_MIN, m_strikeSpellHpMin);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_AOE, m_aoe);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXORI, m_exori);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXORI_GRAN, m_exoriGran);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXORI_MAS, m_exoriMas);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_MAS_SAN, m_exevoMasSan);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_FLAM_HUR, m_exevoFlamHur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_FRIGO_HUR, m_exevoFrigoHur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_TERA_HUR, m_exevoTeraHur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_VIS_HUR, m_exevoVisHur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_VIS_LUX, m_exevoVisLux);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_VIS_LUX, m_exevoGranVisLux);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_VIS, m_exevoGranMasVis);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_FLAM, m_exevoGranMasFlam);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_TERA, m_exevoGranMasTera);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_FRIGO, m_exevoGranMasFrigo);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HEAL_LIST, m_healList);
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
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_SIO, OnToolSpellcasterSio)	
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_HEAL_LIST, OnToolSpellcasterHealList)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_SUMMON, OnToolSpellcasterSummon)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_STRIKE, OnToolSpellcasterStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_FLAM, OnToolSpellcasterMageStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_FRIGO, OnToolSpellcasterMageStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_MORT, OnToolSpellcasterMageStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_TERA, OnToolSpellcasterMageStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_VIS, OnToolSpellcasterMageStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_CON, OnToolSpellcasterPaladinStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_SAN, OnToolSpellcasterPaladinStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_HUR, OnToolSpellcasterKnightStrike)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_HEAL_POISON, OnToolSpellcasterPoison)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_AOE, OnToolSpellcasterAOE)
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
	m_sioSpell.EnableWindow(false);
	m_sioHp.EnableWindow(false);
	m_sioSpellMana.EnableWindow(false);
	m_healList.EnableWindow(false);
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
	m_con.EnableWindow(false);
	m_san.EnableWindow(false);
	m_hur.EnableWindow(false);
	m_manaStrike.EnableWindow(false);
	m_defaultStrikeSpell.EnableWindow(false);
	m_strikeSpellHpMin.EnableWindow(false);
	m_aoe.EnableWindow(false);
	m_exori.EnableWindow(false);
	m_exoriGran.EnableWindow(false);
	m_exoriMas.EnableWindow(false);
	m_exevoMasSan.EnableWindow(false);
	m_exevoFlamHur.EnableWindow(false);
	m_exevoFrigoHur.EnableWindow(false);
	m_exevoTeraHur.EnableWindow(false);
	m_exevoVisHur.EnableWindow(false);
	m_exevoVisLux.EnableWindow(false);
	m_exevoGranVisLux.EnableWindow(false);
	m_exevoGranMasVis.EnableWindow(false);
	m_exevoGranMasFlam.EnableWindow(false);
	m_exevoGranMasTera.EnableWindow(false);
	m_exevoGranMasFrigo.EnableWindow(false);

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
		m_sioSpell.EnableWindow(true);
		OnToolSpellcasterSio();
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
			OnToolSpellcasterMageStrike();
			OnToolSpellcasterPaladinStrike();
			OnToolSpellcasterKnightStrike();
			m_manaStrike.EnableWindow(true);
			m_defaultStrikeSpell.EnableWindow(true);
			m_strikeSpellHpMin.EnableWindow(true);
	}
	m_aoe.EnableWindow(true);
		if (m_aoe.GetCheck()) {
			m_exori.EnableWindow(true);
			m_exoriGran.EnableWindow(true);
			m_exoriMas.EnableWindow(true);
			m_exevoMasSan.EnableWindow(true);
			m_exevoFlamHur.EnableWindow(true);
			m_exevoFrigoHur.EnableWindow(true);
			m_exevoTeraHur.EnableWindow(true);
			m_exevoVisHur.EnableWindow(true);
			m_exevoVisLux.EnableWindow(true);
			m_exevoGranVisLux.EnableWindow(true);
			m_exevoGranMasVis.EnableWindow(true);
			m_exevoGranMasFlam.EnableWindow(true);
			m_exevoGranMasTera.EnableWindow(true);
			m_exevoGranMasFrigo.EnableWindow(true);
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
	m_sioSpell.SetCheck(configData->vitaSpell);
	sprintf(buf,"%d",configData->sioHp);			m_sioHp.SetWindowText(buf);
	sprintf(buf,"%d",configData->sioSpellMana);		m_sioSpellMana.SetWindowText(buf);
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
	m_con.SetCheck(configData->con);
	m_san.SetCheck(configData->san);
	m_hur.SetCheck(configData->hur);
	sprintf(buf,"%d",configData->manaStrike);		m_manaStrike.SetWindowText(buf);
	sprintf(buf,"%s",configData->defaultStrikeSpell);		m_defaultStrikeSpell.SetWindowText(buf);
	sprintf(buf,"%d",configData->strikeSpellHpMin);		m_strikeSpellHpMin.SetWindowText(buf);
	m_aoe.SetCheck(configData->aoe);
	m_exori.SetCheck(configData->exori);
	m_exoriGran.SetCheck(configData->exoriGran);
	m_exoriMas.SetCheck(configData->exoriGran);
	m_exevoMasSan.SetCheck(configData->exevoMasSan);
	m_exevoFlamHur.SetCheck(configData->exevoFlamHur);
	m_exevoFrigoHur.SetCheck(configData->exevoFrigoHur);
	m_exevoTeraHur.SetCheck(configData->exevoTeraHur);
	m_exevoVisHur.SetCheck(configData->exevoVisHur);
	m_exevoVisLux.SetCheck(configData->exevoVisLux);
	m_exevoGranVisLux.SetCheck(configData->exevoGranVisLux);
	m_exevoGranMasVis.SetCheck(configData->exevoGranMasVis);
	m_exevoGranMasFlam.SetCheck(configData->exevoGranMasFlam);
	m_exevoGranMasTera.SetCheck(configData->exevoGranMasTera);
	m_exevoGranMasFrigo.SetCheck(configData->exevoGranMasFrigo);
	
	memcpy(memWhiteList,configData->healList,3200);

	OnToolSpellcasterMana();
	OnToolSpellcasterLife();
	OnToolSpellcasterSummon();
	OnToolSpellcasterStrike();
	OnToolSpellcasterAOE();
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
	newConfigData->sioSpell = m_sioSpell.GetCheck();
	m_sioHp.GetWindowText(buf,127);newConfigData->sioHp=atoi(buf);
	m_sioSpellMana.GetWindowText(buf,127); newConfigData->sioSpellMana=atoi(buf);
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
	newConfigData->con = m_con.GetCheck();
	newConfigData->san = m_san.GetCheck();
	newConfigData->hur = m_hur.GetCheck();
	m_manaStrike.GetWindowText(buf,127);newConfigData->manaStrike=atoi(buf);
	m_defaultStrikeSpell.GetWindowText(newConfigData->defaultStrikeSpell,127);
	m_strikeSpellHpMin.GetWindowText(buf,127);newConfigData->strikeSpellHpMin=atoi(buf);

	newConfigData->aoe = m_aoe.GetCheck();
	newConfigData->exori = m_exori.GetCheck();
	newConfigData->exoriGran = m_exoriGran.GetCheck();
	newConfigData->exoriMas = m_exoriMas.GetCheck();
	newConfigData->exevoMasSan = m_exevoMasSan.GetCheck();
	newConfigData->exevoFlamHur = m_exevoFlamHur.GetCheck();
	newConfigData->exevoFrigoHur = m_exevoFrigoHur.GetCheck();
	newConfigData->exevoTeraHur = m_exevoTeraHur.GetCheck();
	newConfigData->exevoVisHur = m_exevoVisHur.GetCheck();
	newConfigData->exevoVisLux = m_exevoVisLux.GetCheck();
	newConfigData->exevoGranVisLux = m_exevoGranVisLux.GetCheck();
	newConfigData->exevoGranMasVis = m_exevoGranMasVis.GetCheck();
	newConfigData->exevoGranMasFlam = m_exevoGranMasFlam.GetCheck();
	newConfigData->exevoGranMasTera = m_exevoGranMasTera.GetCheck();
	newConfigData->exevoGranMasTera = m_exevoGranMasFrigo.GetCheck();

	memcpy(newConfigData->healList,memWhiteList,3200);

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
	OnToolSpellcasterAOE();
	
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
		int sioVal = m_sioSpell.GetCheck();
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
			m_sioSpell.EnableWindow(val);
				m_sioHp.EnableWindow(val && sioVal?true:false);
				m_sioSpellMana.EnableWindow(val && sioVal?true:false);
				m_healList.EnableWindow(val && sioVal?true:false);
			m_customSpell.EnableWindow(val);
				m_lifeHp.EnableWindow(val && customVal?true:false);
				m_lifeSpell.EnableWindow(val && customVal?true:false);
				m_lifeSpellMana.EnableWindow(val && customVal?true:false);
			m_paralysisSpell.EnableWindow(val);
			m_poisonSpell.EnableWindow(val);
				m_minPoisonDmg.EnableWindow(val && poisonVal?true:false);

}

void CConfigDialog::OnToolSpellcasterSummon() {
	int val = m_summon.GetCheck();
	m_summonLessThan.EnableWindow(val);
	m_summonMana.EnableWindow(val);
	m_summonName.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterStrike() {
	int val = m_strike.GetCheck();
		OnToolSpellcasterMageStrike();
		OnToolSpellcasterPaladinStrike();
		OnToolSpellcasterKnightStrike();
		m_manaStrike.EnableWindow(val);
		m_defaultStrikeSpell.EnableWindow(val);
		m_strikeSpellHpMin.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterExura() {
	int val = m_exuraSpell.GetCheck();
		m_exuraHp.EnableWindow(val);
		m_exuraSpellMana.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterGran() {
	int val = m_granSpell.GetCheck();
		m_granHp.EnableWindow(val);
		m_granSpellMana.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterVita() {
	int val = m_vitaSpell.GetCheck();
		m_vitaHp.EnableWindow(val);
		m_vitaSpellMana.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterSio() {
	int val = m_sioSpell.GetCheck();
		m_sioHp.EnableWindow(val);
		m_sioSpellMana.EnableWindow(val);
		m_healList.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterHealList() {
	CWhiteList *dialog = new CWhiteList(memWhiteList);
	dialog->DoModal();
	delete dialog;
}

void CConfigDialog::OnToolSpellcasterCustom() {
	int val = m_customSpell.GetCheck();
		m_lifeHp.EnableWindow(val);
		m_lifeSpell.EnableWindow(val);
		m_lifeSpellMana.EnableWindow(val);

}
void CConfigDialog::OnToolSpellcasterPoison() {
	int val = m_poisonSpell.GetCheck();
		m_minPoisonDmg.EnableWindow(val);
}

void CConfigDialog::OnToolSpellcasterMageStrike() {
	if (m_con.GetCheck() || m_san.GetCheck() || m_hur.GetCheck() || !m_strike.GetCheck()) {
		m_flam.EnableWindow(false);
		m_frigo.EnableWindow(false);
		m_mort.EnableWindow(false);
		m_tera.EnableWindow(false);
		m_vis.EnableWindow(false);
	}
	else {
		m_flam.EnableWindow(true);
		m_frigo.EnableWindow(true);
		m_mort.EnableWindow(true);
		m_tera.EnableWindow(true);
		m_vis.EnableWindow(true);
	}
}

void CConfigDialog::OnToolSpellcasterPaladinStrike() {
	if (m_flam.GetCheck() || m_frigo.GetCheck() || m_mort.GetCheck() || m_tera.GetCheck() || m_vis.GetCheck() || m_hur.GetCheck() || !m_strike.GetCheck()){
		m_san.EnableWindow(false);
		m_con.EnableWindow(false);
	}
	else {
		m_san.EnableWindow(true);
		m_con.EnableWindow(true);
	}
}

void CConfigDialog::OnToolSpellcasterKnightStrike() {
	if (m_flam.GetCheck() || m_frigo.GetCheck() || m_mort.GetCheck() || m_tera.GetCheck() || m_vis.GetCheck() || m_con.GetCheck() || m_san.GetCheck() || !m_strike.GetCheck()){
		m_hur.EnableWindow(false);
	}
	else {
		m_hur.EnableWindow(true);
	}
}

void CConfigDialog::OnToolSpellcasterAOE() 
{
	int val = m_aoe.GetCheck();
	m_exori.EnableWindow(val);
	m_exoriGran.EnableWindow(val);
	m_exoriMas.EnableWindow(val);
	m_exevoMasSan.EnableWindow(val);
	m_exevoFlamHur.EnableWindow(val);
	m_exevoFrigoHur.EnableWindow(val);
	m_exevoTeraHur.EnableWindow(val);
	m_exevoVisHur.EnableWindow(val);
	m_exevoVisLux.EnableWindow(val);
	m_exevoGranVisLux.EnableWindow(val);
	m_exevoGranMasVis.EnableWindow(val);
	m_exevoGranMasFlam.EnableWindow(val);
	m_exevoGranMasTera.EnableWindow(val);
	m_exevoGranMasFrigo.EnableWindow(val);
}