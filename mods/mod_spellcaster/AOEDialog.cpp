// AOEDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_spellcaster.h"
#include "AOEDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AOEDialog dialog


AOEDialog::AOEDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(AOEDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(AOEDialog)
	//}}AFX_DATA_INIT
}


void AOEDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AOEDialog)
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_AOE, m_aoe);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXORI_MAS, m_exoriMas);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXORI_GRAN, m_exoriGran);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXORI, m_exori);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_VIS_LUX, m_exevoVisLux);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_TERA_HUR, m_exevoTeraHur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_VIS_HUR, m_exevoVisHur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_MAS_SAN, m_exevoMasSan);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_VIS_LUX, m_exevoGranVisLux);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_VIS, m_exevoGranMasVis);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_TERA, m_exevoGranMasTera);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_FRIGO, m_exevoGranMasFrigo);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_FLAM, m_exevoGranMasFlam);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_FRIGO_HUR, m_exevoFrigoHur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_EXEVO_FLAM_HUR, m_exevoFlamHur);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_AOE_AFFECT, m_aoeAffect);
	DDX_Control(pDX, IDC_FRAME_SORCERER_AOE, m_SorcererAOEFrame);
	DDX_Control(pDX, IDC_FRAME_PALADIN_AOE, m_PaladinAOEFrame);
	DDX_Control(pDX, IDC_FRAME_KNIGHT_AOE, m_KnightAOEFrame);
	DDX_Control(pDX, IDC_FRAME_DRUID_AOE, m_DruidAOEFrame);
	DDX_Control(pDX, IDC_FRAME_AOE_SPELLS, m_AOESpellsFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AOEDialog, CDialog)
	//{{AFX_MSG_MAP(AOEDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_AOE, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXORI, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXORI_GRAN, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXORI_MAS, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_MAS_SAN, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_FLAM_HUR, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_FRIGO_HUR, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_TERA_HUR, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_VIS_HUR, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_VIS_LUX, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_GRAN_VIS_LUX, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_VIS, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_FLAM, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_TERA, OnToolSpellcasterAOE)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_EXEVO_GRAN_MAS_FRIGO, OnToolSpellcasterAOE)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AOEDialog message handlers

void AOEDialog::disableControls() {
	m_aoe.EnableWindow(false);
	m_aoeAffect.EnableWindow(false);
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

void AOEDialog::enableControls() {
	m_aoe.EnableWindow(true);
	if (m_aoe.GetCheck()) {
		OnToolSpellcasterAOE();
	}
}

void AOEDialog::configToControls(CConfigData *configData) {
	char buf[128];
	m_aoe.SetCheck(configData->aoe);
	sprintf(buf,"%d",configData->aoeAffect);		m_aoeAffect.SetWindowText(buf);
	m_exori.SetCheck(configData->exori);
	m_exoriGran.SetCheck(configData->exoriGran);
	m_exoriMas.SetCheck(configData->exoriMas);
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
	OnToolSpellcasterAOE();
}

void AOEDialog::controlsToConfig(CConfigData *newConfigData) {
	char buf[128];
	newConfigData->aoe = m_aoe.GetCheck();
	m_aoeAffect.GetWindowText(buf,127);newConfigData->aoeAffect=atoi(buf);
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
	newConfigData->exevoGranMasFrigo = m_exevoGranMasFrigo.GetCheck();
}

BOOL AOEDialog::OnInitDialog() {
	CDialog::OnInitDialog();

	OnToolSpellcasterAOE();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AOEDialog::OnToolSpellcasterAOE()
{
	m_aoeAffect.EnableWindow(m_aoe.GetCheck());

	OnToolSpellcasterSorcererAOE();
	OnToolSpellcasterDruidAOE();
	OnToolSpellcasterPaladinAOE();
	OnToolSpellcasterKnightAOE();

}

void AOEDialog::OnToolSpellcasterSorcererAOE()
{
	int val = !m_exori.GetCheck() && !m_exoriGran.GetCheck() && !m_exoriMas.GetCheck() && !m_exevoMasSan.GetCheck() && !m_exevoFrigoHur.GetCheck() && !m_exevoTeraHur.GetCheck() && !m_exevoGranMasTera.GetCheck() && !m_exevoGranMasFrigo.GetCheck() && m_aoe.GetCheck();
	m_exevoFlamHur.EnableWindow(val);
	m_exevoVisHur.EnableWindow(val);
	m_exevoVisLux.EnableWindow(val);
	m_exevoGranVisLux.EnableWindow(val);
	m_exevoGranMasVis.EnableWindow(val);
	m_exevoGranMasFlam.EnableWindow(val);
}

void AOEDialog::OnToolSpellcasterDruidAOE()
{
	int val = !m_exori.GetCheck() && !m_exoriGran.GetCheck() && !m_exoriMas.GetCheck() && !m_exevoMasSan.GetCheck() && !m_exevoFlamHur.GetCheck() && !m_exevoVisHur.GetCheck() && !m_exevoVisLux.GetCheck() && !m_exevoGranVisLux.GetCheck() && !m_exevoGranMasVis.GetCheck() && !m_exevoGranMasFlam.GetCheck() && m_aoe.GetCheck();
	m_exevoFrigoHur.EnableWindow(val);
	m_exevoTeraHur.EnableWindow(val);
	m_exevoGranMasTera.EnableWindow(val);
	m_exevoGranMasFrigo.EnableWindow(val);
}

void AOEDialog::OnToolSpellcasterPaladinAOE()
{
	int val = !m_exori.GetCheck() && !m_exoriGran.GetCheck() && !m_exoriMas.GetCheck() && !m_exevoFlamHur.GetCheck() && !m_exevoFrigoHur.GetCheck() && !m_exevoTeraHur.GetCheck() && !m_exevoVisHur.GetCheck() && !m_exevoVisLux.GetCheck() && !m_exevoGranVisLux.GetCheck() && !m_exevoGranMasVis.GetCheck() && !m_exevoGranMasFlam.GetCheck() && !m_exevoGranMasTera.GetCheck() && !m_exevoGranMasFrigo.GetCheck() && m_aoe.GetCheck();
	m_exevoMasSan.EnableWindow(val);
}

void AOEDialog::OnToolSpellcasterKnightAOE()
{
	int val = !m_exevoMasSan.GetCheck() && !m_exevoFlamHur.GetCheck() && !m_exevoFrigoHur.GetCheck() && !m_exevoTeraHur.GetCheck() && !m_exevoVisHur.GetCheck() && !m_exevoVisLux.GetCheck() && !m_exevoGranVisLux.GetCheck() && !m_exevoGranMasVis.GetCheck() && !m_exevoGranMasFlam.GetCheck() && !m_exevoGranMasTera.GetCheck() && !m_exevoGranMasFrigo.GetCheck() && m_aoe.GetCheck();
	m_exori.EnableWindow(val);
	m_exoriGran.EnableWindow(val);
	m_exoriMas.EnableWindow(val);
}
