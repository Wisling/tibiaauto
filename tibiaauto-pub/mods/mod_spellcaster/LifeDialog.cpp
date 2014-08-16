// LifeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_spellcaster.h"
#include "LifeDialog.h"
#include "HealList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LifeDialog dialog


LifeDialog::LifeDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(LifeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(LifeDialog)
	//}}AFX_DATA_INIT
}


void LifeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LifeDialog)
	DDX_Control(pDX, IDC_FRAME_LIFE, m_lifeFrame);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_CUSTOM, m_customSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_VITA_MANA, m_vitaSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_VITA_LIFE, m_vitaHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_VITA, m_vitaSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SIO_MANA, m_sioSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SIO, m_sioSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MIN_POISON_DAMAGE, m_minPoisonDmg);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_SPELL_MANA, m_lifeSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_SPELL, m_lifeSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE, m_life);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_HP, m_lifeHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXURA, m_exuraSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXURA_LIFE, m_exuraHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXURA_MANA, m_exuraSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HEAL_PARALYSIS, m_paralysisSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HEAL_PARALYSIS_ICO, m_paralysisIco);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HEAL_POISON, m_poisonSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_HEAL_LIST, m_healList);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_GRAN, m_granSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_GRAN_LIFE, m_granHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_GRAN_MANA, m_granSpellMana);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LifeDialog, CDialog)
	//{{AFX_MSG_MAP(LifeDialog)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_LIFE, OnToolSpellcasterLife)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXURA, OnToolSpellcasterExura)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_GRAN, OnToolSpellcasterGran)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_VITA, OnToolSpellcasterVita)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_SIO, OnToolSpellcasterSio)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_CUSTOM, OnToolSpellcasterCustom)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_HEAL_LIST, OnToolSpellcasterHealList)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_HEAL_POISON, OnToolSpellcasterPoison)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LifeDialog message handlers
/////////////////////////////////////////////////////////////////////////////

void LifeDialog::OnToolSpellcasterLife()
{
	int val = m_life.GetCheck();
	int exuraVal = m_exuraSpell.GetCheck();
	int granVal = m_granSpell.GetCheck();
	int vitaVal = m_vitaSpell.GetCheck();
	int sioVal = m_sioSpell.GetCheck();
	int customVal = m_customSpell.GetCheck();
	int poisonVal = m_poisonSpell.GetCheck();
	int icoVal = m_paralysisSpell.GetCheck();
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
	m_sioSpellMana.EnableWindow(val && sioVal?true:false);
	m_healList.EnableWindow(val && sioVal?true:false);
	m_customSpell.EnableWindow(val);
	m_lifeHp.EnableWindow(val && customVal?true:false);
	m_lifeSpell.EnableWindow(val && customVal?true:false);
	m_lifeSpellMana.EnableWindow(val && customVal?true:false);
	m_paralysisSpell.EnableWindow(val);
	m_paralysisIco.EnableWindow(val);
	m_poisonSpell.EnableWindow(val);
	m_minPoisonDmg.EnableWindow(val && poisonVal?true:false);
}

void LifeDialog::OnToolSpellcasterExura() {
	int val = m_exuraSpell.GetCheck();
		m_exuraHp.EnableWindow(val);
		m_exuraSpellMana.EnableWindow(val);
}

void LifeDialog::OnToolSpellcasterGran() {
	int val = m_granSpell.GetCheck();
		m_granHp.EnableWindow(val);
		m_granSpellMana.EnableWindow(val);
}

void LifeDialog::OnToolSpellcasterVita() {
	int val = m_vitaSpell.GetCheck();
		m_vitaHp.EnableWindow(val);
		m_vitaSpellMana.EnableWindow(val);
}

void LifeDialog::OnToolSpellcasterSio() {
	int val = m_sioSpell.GetCheck();
		m_sioSpellMana.EnableWindow(val);
		m_healList.EnableWindow(val);
}

void LifeDialog::OnToolSpellcasterHealList() {
	CHealList *dialog = new CHealList(memHealList);
	dialog->DoModal();
	memHealList = dialog->healList;
	delete dialog;
}

void LifeDialog::OnToolSpellcasterCustom() {
	int val = m_customSpell.GetCheck();
		m_lifeHp.EnableWindow(val);
		m_lifeSpell.EnableWindow(val);
		m_lifeSpellMana.EnableWindow(val);

}
void LifeDialog::OnToolSpellcasterPoison() {
	int val = m_poisonSpell.GetCheck();
		m_minPoisonDmg.EnableWindow(val);
}

void LifeDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_healList);
}

BOOL LifeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();
	
	OnToolSpellcasterLife();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void LifeDialog::disableControls() {
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
	m_sioSpellMana.EnableWindow(false);
	m_healList.EnableWindow(false);
	m_lifeSpell.EnableWindow(false);
	m_lifeHp.EnableWindow(false);
	m_lifeSpell.EnableWindow(false);
	m_lifeSpellMana.EnableWindow(false);
	m_poisonSpell.EnableWindow(false);
	m_paralysisSpell.EnableWindow(false);
	m_paralysisIco.EnableWindow(false);
	m_minPoisonDmg.EnableWindow(false);
}

void LifeDialog::enableControls() {
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
		m_paralysisIco.EnableWindow(true);
		m_poisonSpell.EnableWindow(true);
		OnToolSpellcasterPoison();
	}
}

void LifeDialog::configToControls(CConfigData *configData) {
	char buf[128];
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
	m_sioSpell.SetCheck(configData->sioSpell);
	sprintf(buf,"%d",configData->sioSpellMana);		m_sioSpellMana.SetWindowText(buf);
	memHealList = configData->healList;
	m_paralysisSpell.SetCheck(configData->paralysisSpell);
	m_paralysisIco.SetCheck(configData->paralysisIco);
	m_poisonSpell.SetCheck(configData->poisonSpell);
	sprintf(buf,"%d",configData->minPoisonDmg);		m_minPoisonDmg.SetWindowText(buf);
	OnToolSpellcasterLife();
}

void LifeDialog::controlsToConfig(CConfigData *newConfigData) {
	char buf[128];
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
	m_sioSpellMana.GetWindowText(buf,127); newConfigData->sioSpellMana=atoi(buf);
	newConfigData->paralysisSpell = m_paralysisSpell.GetCheck();
	newConfigData->paralysisIco = m_paralysisIco.GetCheck();
	newConfigData->poisonSpell = m_poisonSpell.GetCheck();
	m_minPoisonDmg.GetWindowText(buf,127);newConfigData->minPoisonDmg=atoi(buf);
	newConfigData->healList = memHealList;
}
