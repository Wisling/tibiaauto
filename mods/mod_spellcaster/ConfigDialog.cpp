// ConfigDialog.cpp : implementation file
//

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
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_HP, m_lifeHp);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_SPELL, m_lifeSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_LIFE_SPELL_MANA, m_lifeSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON, m_summon);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON_MANA, m_summonMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON_NAME, m_summonName);
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
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_SUMMON, OnToolSpellcasterSummon)
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
	m_life.EnableWindow(false);
	m_lifeHp.EnableWindow(false);
	m_lifeSpell.EnableWindow(false);
	m_lifeSpellMana.EnableWindow(false);
	m_summon.EnableWindow(false);
	m_summonLessThan.EnableWindow(false);
	m_summonMana.EnableWindow(false);
	m_summonName.EnableWindow(false);
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
		m_lifeHp.EnableWindow(true);
		m_lifeSpell.EnableWindow(true);
		m_lifeSpellMana.EnableWindow(true);
	}
	m_summon.EnableWindow(true);
	if (m_summon.GetCheck()){
		m_summonLessThan.EnableWindow(true);
		m_summonMana.EnableWindow(true);
		m_summonName.EnableWindow(true);
	}
}



void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];
	m_mana.SetCheck(configData->mana);
	sprintf(buf,"%d",configData->manaMana);			m_manaMana.SetWindowText(buf);
	sprintf(buf,"%s",configData->manaSpell);		m_manaSpell.SetWindowText(buf);
	m_life.SetCheck(configData->life);
	sprintf(buf,"%d",configData->lifeHp);			m_lifeHp.SetWindowText(buf);
	sprintf(buf,"%s",configData->lifeSpell);		m_lifeSpell.SetWindowText(buf);	
	sprintf(buf,"%d",configData->lifeSpellMana);	m_lifeSpellMana.SetWindowText(buf);
	m_summon.SetCheck(configData->summon);
	sprintf(buf,"%d",configData->summonLessThan);	m_summonLessThan.SetWindowText(buf);
	sprintf(buf,"%s",configData->summonName);		m_summonName.SetWindowText(buf);	
	sprintf(buf,"%d",configData->summonMana);		m_summonMana.SetWindowText(buf);

	OnToolSpellcasterMana();
	OnToolSpellcasterLife();
	OnToolSpellcasterSummon();
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	newConfigData->mana = m_mana.GetCheck();
	m_manaMana.GetWindowText(buf,127);newConfigData->manaMana=atoi(buf);
	m_manaSpell.GetWindowText(newConfigData->manaSpell,127);

	newConfigData->life = m_life.GetCheck();
	m_lifeHp.GetWindowText(buf,127);newConfigData->lifeHp=atoi(buf);
	m_lifeSpell.GetWindowText(newConfigData->lifeSpell,127);
	m_lifeSpellMana.GetWindowText(buf,127); newConfigData->lifeSpellMana=atoi(buf);

	newConfigData->summon = m_summon.GetCheck();
	m_summonLessThan.GetWindowText(buf,127);newConfigData->summonLessThan=atoi(buf);
	m_summonMana.GetWindowText(buf,127);newConfigData->summonMana=atoi(buf);
	m_summonName.GetWindowText(newConfigData->summonName,127);

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
	m_lifeHp.EnableWindow(val);
	m_lifeSpell.EnableWindow(val);
	m_lifeSpellMana.EnableWindow(val);	
}

void CConfigDialog::OnToolSpellcasterSummon() 
{
	int val = m_summon.GetCheck();
	m_summonLessThan.EnableWindow(val);
	m_summonMana.EnableWindow(val);
	m_summonName.EnableWindow(val);
}
