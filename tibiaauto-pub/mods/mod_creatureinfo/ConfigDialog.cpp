// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_creatureinfo.h"
#include "ConfigDialog.h"
#include <MemReader.h>
#include "NameChanger.h"
#include "KnownInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_creatureinfoApp *app, CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app = app;
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_NAMECHANGER, m_NameChanger);
	DDX_Control(pDX, IDC_FRAME_SELF_INFO, m_SelfInfoFrame);
	DDX_Control(pDX, IDC_FRAME_CREATURE_DETECTION, m_CreatureDetectionFrame);
	DDX_Control(pDX, IDC_FRAME_MONSTERS_INFO, m_MonstersInfoFrame);
	DDX_Control(pDX, IDC_FRAME_PLAYER_INFO, m_PlayerInfoFrame);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_SELF_WINDOW, m_selfWindow);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_SELF_TRAY, m_selfTray);
	DDX_Control(pDX, IDC_RANGE_Z, m_rangeZ);
	DDX_Control(pDX, IDC_RANGE_XY, m_rangeXY);
	DDX_Control(pDX, IDC_COLLECT_STATS, m_collectStats);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_KNOWNINFO, m_knownInfo);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_ADDITIONALREQUEST, m_addRequest);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_SELF, m_self);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_PLAYER, m_player);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_MONSTER, m_monster);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_ALLINFO, m_allFloorInfo);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_MONSTER_UNIQUEMONSTERNAMES, m_uniqueMonsterNames);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_SELF_2LINE, m_self2);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_SELF_1LINE, m_self1);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_PLAYER_2LINE, m_player2);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_PLAYER_1LINE, m_player1);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_MONSTER_2LINE, m_monster2);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_MONSTER_1LINE, m_monster1);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_ENABLE, OnEnable)
ON_BN_CLICKED(IDC_TOOLCREATURINFO_NAMECHANGER, OnToolcreaturinfoNamechanger)
ON_BN_CLICKED(IDC_TOOLCREATURINFO_PLAYER, OnToolcreaturinfoPlayer)
ON_BN_CLICKED(IDC_TOOLCREATURINFO_MONSTER, OnToolcreaturinfoMonster)
ON_BN_CLICKED(IDC_TOOLCREATURINFO_SELF, OnToolcreaturinfoSelf)
ON_BN_CLICKED(IDC_TOOLCREATURINFO_KNOWNINFO, OnToolcreaturinfoKnowninfo)
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
			m_app->start();
		else
			m_enable.SetCheck(0);
		m_knownInfo.EnableWindow(true);
	}
	else
	{
		m_app->stop();
		m_knownInfo.EnableWindow(false);
	}
}

void CConfigDialog::disableControls()
{
	m_player.EnableWindow(false);
	m_player1.EnableWindow(false);
	m_player2.EnableWindow(false);
	m_monster.EnableWindow(false);
	m_monster1.EnableWindow(false);
	m_monster2.EnableWindow(false);
	m_uniqueMonsterNames.EnableWindow(false);
	m_self.EnableWindow(false);
	m_self1.EnableWindow(false);
	m_self2.EnableWindow(false);
	m_selfWindow.EnableWindow(false);
	m_selfTray.EnableWindow(false);
	m_allFloorInfo.EnableWindow(false);
	m_addRequest.EnableWindow(false);
	m_rangeXY.EnableWindow(false);
	m_rangeZ.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_player.EnableWindow(true);
	if (m_player.GetCheck())
	{
		m_player1.EnableWindow(true);
		m_player2.EnableWindow(true);
	}
	m_monster.EnableWindow(true);
	if (m_monster.GetCheck())
	{
		m_monster1.EnableWindow(true);
		m_monster2.EnableWindow(true);
		m_uniqueMonsterNames.EnableWindow(true);
	}
	m_self.EnableWindow(true);
	if (m_self.GetCheck())
	{
		m_self1.EnableWindow(true);
		m_self2.EnableWindow(true);
		m_selfWindow.EnableWindow(true);
		m_selfTray.EnableWindow(true);
	}
	m_allFloorInfo.EnableWindow(true);
	m_addRequest.EnableWindow(true);
	m_rangeXY.EnableWindow(true);
	m_rangeZ.EnableWindow(true);
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];

	m_player.SetCheck(configData->player);
	OnToolcreaturinfoPlayer();
	m_player1.SetWindowText(configData->player1);
	m_player2.SetWindowText(configData->player2);
	m_monster.SetCheck(configData->monster);
	OnToolcreaturinfoMonster();
	m_monster1.SetWindowText(configData->monster1);
	m_monster2.SetWindowText(configData->monster2);
	m_uniqueMonsterNames.SetCheck(configData->uniqueMonsterNames);
	m_self.SetCheck(configData->self);
	OnToolcreaturinfoSelf();
	m_self1.SetWindowText(configData->self1);
	m_self2.SetWindowText(configData->self2);
	m_selfWindow.SetWindowText(configData->selfWindow);
	m_selfTray.SetWindowText(configData->selfTray);
	m_allFloorInfo.SetCheck(configData->allFloorInfo);
	m_addRequest.SetCheck(configData->allFloorInfo);
	m_collectStats.SetCheck(configData->collectStats);

	sprintf(buf, "%d", configData->rangeXY);
	m_rangeXY.SetWindowText(buf);
	sprintf(buf, "%d", configData->rangeZ);
	m_rangeZ.SetWindowText(buf);

	OnToolcreaturinfoPlayer();
	OnToolcreaturinfoMonster();
	OnToolcreaturinfoSelf();
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CConfigData *newConfigData = new CConfigData();
	char buf[128];

	newConfigData->player = m_player.GetCheck();
	m_player1.GetWindowText(newConfigData->player1, 260);
	m_player2.GetWindowText(newConfigData->player2, 260);
	newConfigData->monster = m_monster.GetCheck();
	m_monster1.GetWindowText(newConfigData->monster1, 260);
	m_monster2.GetWindowText(newConfigData->monster2, 260);
	newConfigData->uniqueMonsterNames = m_uniqueMonsterNames.GetCheck();
	newConfigData->self               = m_self.GetCheck();
	m_self1.GetWindowText(newConfigData->self1, 260);
	m_self2.GetWindowText(newConfigData->self2, 260);
	m_selfWindow.GetWindowText(newConfigData->selfWindow, 260);
	m_selfTray.GetWindowText(newConfigData->selfTray, 260);
	newConfigData->allFloorInfo = m_allFloorInfo.GetCheck();
	newConfigData->addRequest   = m_addRequest.GetCheck();
	//newConfigData->collectStats = m_collectStats.GetCheck();
	newConfigData->collectStats = 1;
	m_rangeXY.GetWindowText(buf, 127);
	newConfigData->rangeXY = atoi(buf);
	m_rangeZ.GetWindowText(buf, 127);
	newConfigData->rangeZ = atoi(buf);

	return newConfigData;
}

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}

void CConfigDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_knownInfo);
	skin.SetButtonSkin(m_enable);
	skin.SetButtonSkin(m_OK);
	skin.SetButtonSkin(m_NameChanger);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDialog::OnToolcreaturinfoNamechanger()
{
	CNameChanger *nameChanger;

	nameChanger = new CNameChanger(this);
	nameChanger->Create(IDD_NAME);
	nameChanger->ShowWindow(SW_SHOW);
}

void CConfigDialog::OnToolcreaturinfoPlayer()
{
	int val = m_player.GetCheck();

	m_player1.EnableWindow(val);
	m_player2.EnableWindow(val);
}

void CConfigDialog::OnToolcreaturinfoMonster()
{
	int val = m_monster.GetCheck();

	m_monster1.EnableWindow(val);
	m_monster2.EnableWindow(val);
	m_uniqueMonsterNames.EnableWindow(val);
}

void CConfigDialog::OnToolcreaturinfoSelf()
{
	int val = m_self.GetCheck();

	m_self1.EnableWindow(val);
	m_self2.EnableWindow(val);
	m_selfWindow.EnableWindow(val);
	m_selfTray.EnableWindow(val);
}

void CConfigDialog::OnToolcreaturinfoKnowninfo()
{
	CKnownInfo *knownInfo;

	knownInfo = new CKnownInfo(this);
	knownInfo->Create(IDD_INFO);
	knownInfo->ShowWindow(SW_SHOW);
}
