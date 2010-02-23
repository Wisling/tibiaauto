#include "stdafx.h"
#include "mod_spellcaster.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
//#include "HealList.h"
#include "LifeDialog.h"
#include "ManaDialog.h"
#include "SummonDialog.h"
#include "StrikeDialog.h"
#include "AOEDialog.h"
#include "TimedDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_spellcasterApp *app,CWnd* pParent /*=NULL*/)
: MyDialog(CConfigDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;

	m_DialogID[0] = IDD_LIFE_DIALOG;
	m_DialogID[1] = IDD_MANA_DIALOG;
	m_DialogID[2] = IDD_SUMMON_DIALOG;
	m_DialogID[3] = IDD_STRIKE_DIALOG;
	m_DialogID[4] = IDD_AOE_DIALOG;
	m_DialogID[5] = IDD_TIMED_DIALOG;
	
	m_Dialog[0] = new LifeDialog;
	m_Dialog[1] = new ManaDialog;
	m_Dialog[2] = new SummonDialog;
	m_Dialog[3] = new StrikeDialog;
	m_Dialog[4] = new AOEDialog;
	m_Dialog[5] = new TimedDialog;
	
	m_nPageCount = 6;	
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_DISABLE_WARNING, m_disableWarning);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_RANDOM_CAST,m_randomCast);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDOK, m_OK);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK() {
		ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable() {
	if (m_enable.GetCheck()) {
		m_app->controlsToConfig();
		if (m_app->validateConfig(1)) {			
			m_app->start();
		} 
		else {
			m_enable.SetCheck(0);
		}
	} 
	else {
		m_app->stop(); 
	}
}

void CConfigDialog::disableControls() {
	m_Dialog[0]->disableControls();
	m_Dialog[1]->disableControls();
	m_Dialog[2]->disableControls();
	m_Dialog[3]->disableControls();
	m_Dialog[4]->disableControls();
	m_Dialog[5]->disableControls();

	m_disableWarning.EnableWindow(false);
	m_randomCast.EnableWindow(false);
}

void CConfigDialog::enableControls() {
	m_Dialog[0]->enableControls();
	m_Dialog[1]->enableControls();
	m_Dialog[2]->enableControls();
	m_Dialog[3]->enableControls();
	m_Dialog[4]->enableControls();
	m_Dialog[5]->enableControls();

	m_disableWarning.EnableWindow(true);
	m_randomCast.EnableWindow(true);
}

void CConfigDialog::configToControls(CConfigData *configData) {
	m_Dialog[0]->configToControls(configData);
	m_Dialog[1]->configToControls(configData);
	m_Dialog[2]->configToControls(configData);
	m_Dialog[3]->configToControls(configData);
	m_Dialog[4]->configToControls(configData);
	m_Dialog[5]->configToControls(configData);

	m_disableWarning.SetCheck(configData->disableWarning);
	m_randomCast.SetCheck(configData->randomCast);
}

CConfigData * CConfigDialog::controlsToConfig() {
	CConfigData *newConfigData = new CConfigData();

	m_Dialog[0]->controlsToConfig(newConfigData);
	m_Dialog[1]->controlsToConfig(newConfigData);
	m_Dialog[2]->controlsToConfig(newConfigData);
	m_Dialog[3]->controlsToConfig(newConfigData);
	m_Dialog[4]->controlsToConfig(newConfigData);
	m_Dialog[5]->controlsToConfig(newConfigData);

	newConfigData->disableWarning = m_disableWarning.GetCheck();
	newConfigData->randomCast = m_randomCast.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) {	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_enable);
	skin.SetButtonSkin(	m_OK);

	m_Dialog[0]->Create(m_DialogID[0],&m_tabCtrl);
	m_Dialog[1]->Create(m_DialogID[1],&m_tabCtrl);
	m_Dialog[2]->Create(m_DialogID[2],&m_tabCtrl);
	m_Dialog[3]->Create(m_DialogID[3],&m_tabCtrl);
	m_Dialog[4]->Create(m_DialogID[4],&m_tabCtrl);
	m_Dialog[5]->Create(m_DialogID[5],&m_tabCtrl);

	m_tabCtrl.SetTabColor(RGB(skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue));
	m_tabCtrl.SetNormalColor(RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));

	m_tabCtrl.AddTab(m_Dialog[0], "Healing");
	m_tabCtrl.AddTab(m_Dialog[1], "Mana Burning");
	m_tabCtrl.AddTab(m_Dialog[2], "Summon Control");
	m_tabCtrl.AddTab(m_Dialog[3], "Strike Spells");
	m_tabCtrl.AddTab(m_Dialog[4], "AOE Spells");
	m_tabCtrl.AddTab(m_Dialog[5], "Timed Spells");

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


