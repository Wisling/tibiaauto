// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_addressfinder.h"
#include "ConfigDialog.h"
#include "ModuleUtil.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include <fstream>
#include <List>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_addressfinderApp *app,CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfigDialog)
	m_outputStartByte = 0;
	m_outputEndByte = 0;
	//}}AFX_DATA_INIT
	m_app=app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_STAMINA_ADDRESS, m_staminaAddress);
	DDX_Control(pDX, IDC_STAMINA, m_stamina);
	DDX_Control(pDX, IDC_CAPACITY_ADDRESS, m_capacityAddress);
	DDX_Control(pDX, IDC_CAPACITY, m_capacity);
	DDX_Control(pDX, IDC_BATTLE_LIST_MAX, m_battleListMax);
	DDX_Control(pDX, IDC_TILES_LEFT_ADDRESS, m_tilesLeftAddress);
	DDX_Control(pDX, IDC_TILES_LEFT, m_tilesLeft);
	DDX_Control(pDX, IDC_SOUL_POINTS_ADDRESS, m_soulPointsAddress);
	DDX_Control(pDX, IDC_SOUL_POINTS, m_soulPoints);
	DDX_Control(pDX, IDC_SELF_ID, m_selfID);
	DDX_Control(pDX, IDC_SELF_ID_ADDRESS, m_selfIDAddress);
	DDX_Control(pDX, IDC_MANA_MAX_ADDRESS, m_manaMaxAddress);
	DDX_Control(pDX, IDC_MANA_MAX, m_manaMax);
	DDX_Control(pDX, IDC_MANA_ADDRESS, m_manaAddress);
	DDX_Control(pDX, IDC_MANA, m_mana);
	DDX_Control(pDX, IDC_MAGIC_LEVEL_PERCENT_LEFT, m_magicLevelPercentLeft);
	DDX_Control(pDX, IDC_MAGIC_LEVEL_PERCENT_LEFT_ADDRESS, m_magicLevelPrecentLeftAddress);
	DDX_Control(pDX, IDC_MAGIC_LEVEL_ADDRESS, m_magicLevelAddress);
	DDX_Control(pDX, IDC_MAGIC_LEVEL, m_magicLevel);
	DDX_Control(pDX, IDC_LEVEL_ADDRESS, m_levelAddress);
	DDX_Control(pDX, IDC_LEVEL, m_level);
	DDX_Control(pDX, IDC_HP_MAX_ADDRESS, m_HPMaxAddress);
	DDX_Control(pDX, IDC_HP_ADDRESS, m_HPAddress);
	DDX_Control(pDX, IDC_HP, m_HP);
	DDX_Control(pDX, IDC_HP_MAX, m_HPMax);
	DDX_Control(pDX, IDC_GO_Z_ADDRESS, m_goZAddress);
	DDX_Control(pDX, IDC_GO_Z, m_goZ);
	DDX_Control(pDX, IDC_GO_Y_ADDRESS, m_goYAddress);
	DDX_Control(pDX, IDC_GO_Y, m_goY);
	DDX_Control(pDX, IDC_GO_X, m_goX);
	DDX_Control(pDX, IDC_GO_X_ADDRESS, m_goXAddress);
	DDX_Control(pDX, IDC_GO_TO_TILE_ADDRESS, m_goToTileAddress);
	DDX_Control(pDX, IDC_GO_TO_TILE, m_goToTile);
	DDX_Control(pDX, IDC_FLAGS_ADDRESS, m_flagsAddress);
	DDX_Control(pDX, IDC_FLAGS, m_flags);
	DDX_Control(pDX, IDC_FIST_SKILL_ADDRESS, m_fistSkillAddress);
	DDX_Control(pDX, IDC_FIST_SKILL, m_fistSkill);
	DDX_Control(pDX, IDC_FIST_PERCENT_LEFT_ADDRESS, m_fistPercentLeftAddress);
	DDX_Control(pDX, IDC_FIST_PERCENT_LEFT, m_fistPercentLeft);
	DDX_Control(pDX, IDC_FISH_SKILL_ADDRESS, m_fishSkillAddress);
	DDX_Control(pDX, IDC_FISH_SKILL, m_fishSkill);
	DDX_Control(pDX, IDC_FIRST_CREATURE_ADDRESS, m_firstCreatureAddress);
	DDX_Control(pDX, IDC_EXPERIENCE_ADDRESS, m_experienceAddress);
	DDX_Control(pDX, IDC_FIRST_CREATURE, m_firstCreature);
	DDX_Control(pDX, IDC_EXPERIENCE, m_experience);
	DDX_Control(pDX, IDC_BATTLE_LIST_MIN_ADDRESS, m_battleListMinAddress);
	DDX_Control(pDX, IDC_BATTLE_LIST_MIN, m_battleListMin);
	DDX_Control(pDX, IDC_BATTLE_LIST_MAX_ADDRESS, m_battleListMaxAddress);
	DDX_Control(pDX, IDC_ATTACKED_CREATURE_ADDRESS, m_attackedCreatureAddress);
	DDX_Control(pDX, IDC_ATTACKED_CREATURE, m_attackedCreature);
	DDX_Control(pDX, IDC_BEGIN_OUTPUT, m_beginOutput);
	DDX_Control(pDX, IDC_FRAME_2, m_addressOutput);
	DDX_Control(pDX, IDC_FRAME_ADDRESSFINDER, m_AddressFinderFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Text(pDX, IDC_START_BYTE, m_outputStartByte);
	DDX_Text(pDX, IDC_END_BYTE, m_outputEndByte);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BEGIN_OUTPUT, OnBeginOutput)
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
		if (m_app->validateConfig(1))			
			m_app->start();
		else
			m_enable.SetCheck(0);
	} 
	else
		m_app->stop(); 
}

void CConfigDialog::disableControls() {
	m_experience.EnableWindow(false);
	m_experienceAddress.EnableWindow(false);
}

void CConfigDialog::enableControls() {
	m_experience.EnableWindow(true);
	m_experienceAddress.EnableWindow(true);

}

void CConfigDialog::configToControls(CConfigData *configData) {
	char buf[128];
	sprintf(buf, "%d", configData->experience); m_experience.SetWindowText(buf);
	sprintf(buf, "0x%x", configData->experienceAddress); m_experienceAddress.SetWindowText(buf);
}

CConfigData * CConfigDialog::controlsToConfig() {	
	char buf[128];
	CConfigData *newConfigData = new CConfigData();
	m_experience.GetWindowText(buf,127);newConfigData->experience=atoi(buf);
	m_experienceAddress.GetWindowText(buf,127);newConfigData->experienceAddress=atoi(buf);

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) {
	CMemReaderProxy reader;
	KillTimer(nIDEvent);

	if (nIDEvent==1001) {
		char buf[32];
		sprintf(buf, "0x%X", m_addressFinder.addresses.expAddress);
		m_experienceAddress.SetWindowText(buf);
		sprintf(buf, "%d", reader.getMemIntValue(m_addressFinder.addresses.expAddress));
		m_experience.SetWindowText(buf);
		sprintf(buf, "0x%X", m_addressFinder.addresses.flagsAddress);
		m_flagsAddress.SetWindowText(buf);
		sprintf(buf, "%d", reader.getMemIntValue(m_addressFinder.addresses.flagsAddress));
		m_flags.SetWindowText(buf);
		sprintf(buf, "0x%X", m_addressFinder.addresses.fistPercentLeftAddress);
		m_fistPercentLeftAddress.SetWindowText(buf);
		sprintf(buf, "%d", reader.getMemIntValue(m_addressFinder.addresses.fistPercentLeftAddress));
		m_fistPercentLeft.SetWindowText(buf);
		sprintf(buf, "0x%X", m_addressFinder.addresses.fistSkillAddress);
		m_fistSkillAddress.SetWindowText(buf);
		sprintf(buf, "%d", reader.getMemIntValue(m_addressFinder.addresses.fistSkillAddress));
		m_fistSkill.SetWindowText(buf);
		sprintf(buf, "%d", reader.getMemIntValue(m_addressFinder.addresses.fishSkillAddress));
		m_fishSkill.SetWindowText(buf);
		sprintf(buf, "%d", reader.getMemIntValue(m_addressFinder.addresses.fishSkillAddress));
		m_fishSkill.SetWindowText(buf);
		
	SetTimer(nIDEvent, 500, NULL);	
	}	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_enable);
	skin.SetButtonSkin(	m_beginOutput);
	CMemReaderProxy reader;

	m_addressFinder.addresses.flagsAddress = reader.getMemIntValue(m_addressFinder.getFlagsAddress());
	m_addressFinder.addresses.fistPercentLeftAddress = m_addressFinder.addresses.flagsAddress + 4;
	m_addressFinder.addresses.fistSkillAddress = m_addressFinder.addresses.flagsAddress + 32;
	m_addressFinder.addresses.fishSkillAddress = m_addressFinder.addresses.flagsAddress + 56;
	m_addressFinder.addresses.attackedCreatureAddress = m_addressFinder.addresses.flagsAddress + 68;
	m_addressFinder.addresses.capacityAddress = m_addressFinder.addresses.flagsAddress + 72;
	m_addressFinder.addresses.staminaAddress = m_addressFinder.addresses.flagsAddress + 76;
	m_addressFinder.addresses.soulPointsAddress = m_addressFinder.addresses.flagsAddress + 80;
	m_addressFinder.addresses.manaMaxAddress = m_addressFinder.addresses.flagsAddress + 84;
	m_addressFinder.addresses.manaAddress = m_addressFinder.addresses.flagsAddress + 88;
	m_addressFinder.addresses.magicLevelPercentLeftAddress = m_addressFinder.addresses.flagsAddress + 92;
	m_addressFinder.addresses.magicLevelAddress = m_addressFinder.addresses.flagsAddress + 100;
	m_addressFinder.addresses.levelAddress = m_addressFinder.addresses.flagsAddress + 104;
	m_addressFinder.addresses.expAddress = m_addressFinder.addresses.flagsAddress + 108;
	m_addressFinder.addresses.HPMaxAddress = m_addressFinder.addresses.flagsAddress + 112;
	m_addressFinder.addresses.HPAddress = m_addressFinder.addresses.flagsAddress + 116;
	m_addressFinder.addresses.selfIDAddress = m_addressFinder.addresses.flagsAddress + 120;
	m_addressFinder.addresses.goToTileAddress = m_addressFinder.addresses.flagsAddress + 128;
	m_addressFinder.addresses.tilesToGoAddress = m_addressFinder.addresses.flagsAddress + 132;
	m_addressFinder.addresses.goZAddress = m_addressFinder.addresses.flagsAddress + 180;
	m_addressFinder.addresses.goYAddress = m_addressFinder.addresses.flagsAddress + 184;
	m_addressFinder.addresses.goXAddress = m_addressFinder.addresses.flagsAddress + 188;
	m_addressFinder.addresses.battleListMaxAddress = m_addressFinder.addresses.flagsAddress + 208;
	m_addressFinder.addresses.battleListMinAddress = m_addressFinder.addresses.flagsAddress + 212;
	m_addressFinder.addresses.firstCreatureAddress = m_addressFinder.addresses.flagsAddress + 216;

	SetTimer(1001,250,NULL);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg) {
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable) {
	m_enable.SetCheck(enable);
}


void CConfigDialog::OnBeginOutput() 
{
	CMemReaderProxy reader;
	char buf[128];
	UpdateData();	
	FILE *outputFile = fopen("C:\\temp\\AddressFinderOutput.txt","wb");
	fprintf(outputFile,"Start\r\n");
	fflush(outputFile);
	for (int loop = m_outputStartByte; loop < m_outputEndByte; loop++) {
		byte foundByte = reader.getMemIntValue(loop);
		sprintf(buf, "\tComparison.push_back(0x%x);\r\n\tMask.push_back(1);\r\n", foundByte);
		fprintf(outputFile, buf);
		fflush(outputFile);
	}
	fprintf(outputFile,"End\r\n");
	fflush(outputFile);
	fclose(outputFile);
	AfxMessageBox("Done");
}
