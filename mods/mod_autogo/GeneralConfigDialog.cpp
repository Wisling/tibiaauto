// GeneralConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_autogo.h"
#include "GeneralConfigDialog.h"
#include "MemReaderProxy.h"
#include "WhiteList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeneralConfigDialog dialog


GeneralConfigDialog::GeneralConfigDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(GeneralConfigDialog::IDD, pParent)
{
	memset(memWhiteList,0,3200);
	memMkBlack=0;
	//{{AFX_DATA_INIT(GeneralConfigDialog)
	lastX = 0;
	lastY = 0;
	lastZ = 0;
	//}}AFX_DATA_INIT
}


void GeneralConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GeneralConfigDialog)
	DDX_Control(pDX, IDC_MAINTAIN_START, m_maintainStart);
	DDX_Control(pDX, IDC_AUTOGO_CUR_Y, m_curY);
	DDX_Control(pDX, IDC_GENERAL_CONFIG_PROXIMITY_OPTIONS_FRAME, m_proximityFrame);
	DDX_Control(pDX, IDC_GENERAL_CONFIG_POSITION_OPTIONS_FRAME, m_postionFrame);
	DDX_Control(pDX, IDC_GENERAL_CONFIG_MESSAGE_OPTIONS_FRAME, m_messageFrame);
	DDX_Control(pDX, IDC_AUTOGO_TOSTART, m_SetStart);
	DDX_Control(pDX, IDC_AUTOGO_TORUNAWAY, m_SetRunaway);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_Z, m_runawayZ);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_Y, m_runawayY);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_X, m_runawayX);
	DDX_Control(pDX, IDC_AUTOGO_CUR_Z, m_curZ);
	DDX_Control(pDX, IDC_AUTOGO_CUR_X, m_curX);
	DDX_Control(pDX, IDC_AUTOGO_ACT_Z, m_actZ);
	DDX_Control(pDX, IDC_AUTOGO_ACT_Y, m_actY);
	DDX_Control(pDX, IDC_AUTOGO_ACT_X, m_actX);
	DDX_Control(pDX, IDC_AUTOGO_ACT_DIRECTION, m_actDirection);
	DDX_Control(pDX, IDC_AUTOGO_MESSAGE_IGNORE_SPELLS, m_ignoreSpells);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEPARANOIA, m_battleParanoia);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEANXIETY, m_battleAnxiety);
	DDX_Control(pDX, IDC_GENERAL_CONFIG_FRAME, m_GeneralConfigFrame);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEWHITELIST, m_battleWhiteList);
	DDX_Text(pDX, IDC_AUTOGO_CUR_X, lastX);
	DDX_Text(pDX, IDC_AUTOGO_CUR_Y, lastY);
	DDX_Text(pDX, IDC_AUTOGO_CUR_Z, lastZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GeneralConfigDialog, CDialog)
	//{{AFX_MSG_MAP(GeneralConfigDialog)
	ON_BN_CLICKED(IDC_AUTOGO_WHITELIST, OnAutogoWhitelist)
	ON_BN_CLICKED(IDC_AUTOGO_BATTLEPARANOIA, OnAutogoBattleparanoia)
	ON_BN_CLICKED(IDC_AUTOGO_BATTLEANXIETY, OnAutogoBattleanxiety)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_AUTOGO_TORUNAWAY, OnAutogoTorunaway)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_AUTOGO_TOSTART, OnAutogoTostart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GeneralConfigDialog message handlers

void GeneralConfigDialog::OnAutogoWhitelist() {
	CWhiteList *dialog = new CWhiteList(memWhiteList,&memMkBlack);
	dialog->DoModal();
	delete dialog;
}

void GeneralConfigDialog::disableControls() {
	m_battleWhiteList.EnableWindow(false);
	m_curX.EnableWindow(false);
	m_curY.EnableWindow(false);
	m_curZ.EnableWindow(false);
	m_runawayZ.EnableWindow(false);
	m_runawayY.EnableWindow(false);
	m_runawayX.EnableWindow(false);
	m_actZ.EnableWindow(false);
	m_actY.EnableWindow(false);
	m_actX.EnableWindow(false);
	m_actDirection.EnableWindow(false);
	m_battleAnxiety.EnableWindow(false);
	m_battleParanoia.EnableWindow(false);
	m_ignoreSpells.EnableWindow(false);
	m_SetRunaway.EnableWindow(false);
	m_SetStart.EnableWindow(false);
	m_maintainStart.EnableWindow(false);
}

BOOL GeneralConfigDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_battleWhiteList);
	skin.SetButtonSkin(	m_SetStart);
	skin.SetButtonSkin(	m_SetRunaway);
	
	m_actDirection.InsertString(0,"<None>");
	m_actDirection.InsertString(DIR_LEFT,"Left");
	m_actDirection.InsertString(DIR_RIGHT,"Right");
	m_actDirection.InsertString(DIR_UP,"Up");
	m_actDirection.InsertString(DIR_DOWN,"Down");

	SetTimer(1001,250,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void GeneralConfigDialog::enableControls() {
	m_battleWhiteList.EnableWindow(true);
	m_curX.EnableWindow(true);
	m_curY.EnableWindow(true);
	m_curZ.EnableWindow(true);
	m_runawayZ.EnableWindow(true);
	m_runawayY.EnableWindow(true);
	m_runawayX.EnableWindow(true);
	m_actZ.EnableWindow(true);
	m_actY.EnableWindow(true);
	m_actX.EnableWindow(true);
	m_actDirection.EnableWindow(true);
	m_battleAnxiety.EnableWindow(true);
	m_battleParanoia.EnableWindow(true);
	m_ignoreSpells.EnableWindow(true);
	m_SetRunaway.EnableWindow(true);
	m_SetStart.EnableWindow(true);
	m_maintainStart.EnableWindow(true);
}

void GeneralConfigDialog::configToControls(CConfigData *configData) {
	CString buf;
	memcpy(memWhiteList, configData->whiteList, 3200);
	memMkBlack = configData->mkBlack;
	buf.Format("%d",configData->actX);		m_actX.SetWindowText(buf);
	buf.Format("%d",configData->actY);		m_actY.SetWindowText(buf);
	buf.Format("%d",configData->actZ);		m_actZ.SetWindowText(buf);
	m_actDirection.SetCurSel(configData->actDirection);
	buf.Format("%d",configData->runawayX);	m_runawayX.SetWindowText(buf);
	buf.Format("%d",configData->runawayY);	m_runawayY.SetWindowText(buf);
	buf.Format("%d",configData->runawayZ);	m_runawayZ.SetWindowText(buf);
	m_ignoreSpells.SetCheck(configData->options & OPTIONS_IGNORE_SPELLS);
	m_battleParanoia.SetCheck(configData->options & OPTIONS_BATTLE_PARANOIA);
	m_battleAnxiety.SetCheck(configData->options & OPTIONS_BATTLE_ANXIETY);
	memMkBlack = configData->options & OPTIONS_MAKE_BLACKLIST;
	m_maintainStart.SetCheck(configData->maintainStart);
}

void GeneralConfigDialog::controlsToConfig(CConfigData *newConfigData) {
	CString buf;
	memcpy(newConfigData->whiteList, memWhiteList, 3200);
	m_actX.GetWindowText(buf);		newConfigData->actX=atoi(buf);
	m_actY.GetWindowText(buf);		newConfigData->actY=atoi(buf);
	m_actZ.GetWindowText(buf);		newConfigData->actZ=atoi(buf);
	newConfigData->actDirection = m_actDirection.GetCurSel();
	m_runawayX.GetWindowText(buf);	newConfigData->runawayX=atoi(buf);
	m_runawayY.GetWindowText(buf);	newConfigData->runawayY=atoi(buf);
	m_runawayZ.GetWindowText(buf);	newConfigData->runawayZ=atoi(buf);		
	if (m_ignoreSpells.GetCheck())	newConfigData->options |= OPTIONS_IGNORE_SPELLS;
	if (m_battleParanoia.GetCheck())	newConfigData->options |= OPTIONS_BATTLE_PARANOIA;
	if (m_battleAnxiety.GetCheck())	newConfigData->options |= OPTIONS_BATTLE_ANXIETY;
	if (memMkBlack) newConfigData->options |= OPTIONS_MAKE_BLACKLIST;
	newConfigData->maintainStart = m_maintainStart.GetCheck();
}

void GeneralConfigDialog::OnAutogoBattleparanoia() {
	m_battleAnxiety.EnableWindow(!m_battleParanoia.GetCheck());	
}

void GeneralConfigDialog::OnAutogoBattleanxiety() {
	m_battleParanoia.EnableWindow(!m_battleAnxiety.GetCheck());	
}

void GeneralConfigDialog::OnTimer(UINT nIDEvent) {
	if (nIDEvent==1001) {
		CMemReaderProxy reader;
		CTibiaCharacter *self = reader.readSelfCharacter();
		CString buf;
		if (lastX != self->x || lastY != self->y || lastZ != self->z) {
			buf.Format("%d",self->x);
			m_curX.SetWindowText(buf);
			buf.Format("%d",self->y);
			m_curY.SetWindowText(buf);
			buf.Format("%d",self->z);
			m_curZ.SetWindowText(buf);
		}
		delete self;
	}	
	CDialog::OnTimer(nIDEvent);
}

void GeneralConfigDialog::OnAutogoTorunaway() {
	CString buf;
	m_curX.GetWindowText(buf);
	m_runawayX.SetWindowText(buf);
	m_curY.GetWindowText(buf);
	m_runawayY.SetWindowText(buf);
	m_curZ.GetWindowText(buf);
	m_runawayZ.SetWindowText(buf);
}

void GeneralConfigDialog::OnAutogoTostart() {
	CString buf;
	m_curX.GetWindowText(buf);
	m_actX.SetWindowText(buf);
	m_curY.GetWindowText(buf);
	m_actY.SetWindowText(buf);
	m_curZ.GetWindowText(buf);
	m_actZ.SetWindowText(buf);
}
