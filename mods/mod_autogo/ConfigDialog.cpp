// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_autogo.h"
#include "ConfigDialog.h"
#include "WhiteList.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_autogoApp *app,CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
	memset(memWhiteList,0,3200);
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_MANABELOW_UNTIL_RECOVERY, m_manaBelowUntilRecovery);
	DDX_Control(pDX, IDC_HPBELOW_UNTIL_RECOVERY, m_hpBelowUntilRecovery);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGEROUTOF, m_triggerOutOf);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERSOULPOINTS_ABOVE, m_triggerSoulPointAbove);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERMANABELOW, m_triggerManaBelow);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERMANAABOVE, m_triggerManaAbove);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERHPABOVE, m_triggerHpAbove);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDSOULPOINT_ABOVE, m_soundSoulPointAbove);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDRUNAWAY_REACHED, m_soundRunawayReached);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDOUTOF_SPACE, m_soundOutOfSpace);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDOUTOF_FOOD, m_soundOutOfFood);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDOUTOF_CUSTOM, m_soundOutOfCustom);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDMANABELOW, m_soundManaBelow);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDMANAABOVE, m_soundManaAbove);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDHPABOVE, m_soundHpAbove);
	DDX_Control(pDX, IDC_AUTOGO_SOULPOINT_ABOVE, m_soulPointAbove);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_REACHED_RADIUS, m_runawayReachedRadius);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_REACHED, m_runawayReached);
	DDX_Control(pDX, IDC_AUTOGO_OUTOFCUSTOM, m_outOfCustom);
	DDX_Control(pDX, IDC_AUTOGO_MESSAGE_IGNORE_SPELLS, m_messageIgnoreSpells);
	DDX_Control(pDX, IDC_AUTOGO_MANABELOW, m_manaBelow);
	DDX_Control(pDX, IDC_AUTOGO_MANAABOVE, m_manaAbove);
	DDX_Control(pDX, IDC_AUTOGO_HPABOVE, m_hpAbove);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONSOULPOINT_ABOVE, m_actionSoulpointAbove);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONRUNAWAY_REACHED, m_actionRunawayReached);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONOUTOF_SPACE, m_actionOutOfSpace);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONOUTOF_FOOD, m_actionOutOfFood);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONOUTOF_CUSTOM_ITEM, m_outOfCustomItem);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONOUTOF_CUSTOM, m_actionOutOfCustom);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONMANABELOW, m_actionManaBelow);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONMANAABOVE, m_actionManaAbove);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONHPABOVE, m_actionHpAbove);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDBATTLELIST_PLAYER, m_soundBattleListPlayer);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDBATTLELIST_MONSTER, m_soundBattleListMonster);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDBATTLELIST_LIST, m_soundBattleListList);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDBATTLELIST_GM, m_soundBattleListGm);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONBATTLELIST_PLAYER, m_actionBattleListPlayer);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONBATTLELIST_MONSTER, m_actionBattleListMonster);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONBATTLELIST_LIST, m_actionBattleListList);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONBATTLELIST_GM, m_actionBattleListGm);
	DDX_Control(pDX, IDC_AUTOGO_BATTELIST, m_battleBattlelist);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEPARANOIAM, m_battleParanoiaM);
	DDX_Control(pDX, IDC_AUTOGO_ACT_DIRECTION, m_actDirection);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDHPBELOW, m_soundHpBelow);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERHPBELOW, m_triggerHpBelow);
	DDX_Control(pDX, IDC_AUTOGO_HPBELOW, m_hpBelow);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONHPBELOW, m_actionHpBelow);
	DDX_Control(pDX, IDC_AUTOGO_OUTOFSPACE, m_outOfSpace);
	DDX_Control(pDX, IDC_AUTOGO_ACT_X, m_actX);
	DDX_Control(pDX, IDC_AUTOGO_ACT_Z, m_actZ);
	DDX_Control(pDX, IDC_AUTOGO_ACT_Y, m_actY);
	DDX_Control(pDX, IDC_AUTOGO_SOULPOINT, m_soulPoint);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDSIGN, m_soundSign);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERSOULPOINTS, m_triggerSoulPoint);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERSIGN, m_triggerSign);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERMESSAGE, m_triggerMessage);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERMOVE, m_triggerMove);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERHPLOSS, m_triggerHpLoss);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERCAPACITY, m_triggerCapacity);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERBLANK, m_triggerBlank);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERBATTLELIST, m_triggerBattleList);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDSOULPOINT, m_soundSoulPoint);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDMOVE, m_soundMove);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDMESSAGE, m_soundMessage);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDHPLOSS, m_soundHpLoss);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDCAPACITY, m_soundCapacity);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDBLANK, m_soundBlank);
	DDX_Control(pDX, IDC_AUTOGO_SIGNPOISON, m_signPoison);
	DDX_Control(pDX, IDC_AUTOGO_SIGNFIRE, m_signFire);
	DDX_Control(pDX, IDC_AUTOGO_SIGNENERGY, m_signEnergy);
	DDX_Control(pDX, IDC_AUTOGO_SIGNBATTLE, m_signBattle);
	DDX_Control(pDX, IDC_AUTOGO_MESSAGEPUBLIC, m_messagePublic);
	DDX_Control(pDX, IDC_AUTOGO_OUTOFFOOD, m_outOfFood);
	DDX_Control(pDX, IDC_AUTOGO_MESSAGEPRIVATE, m_messagePrivate);
	DDX_Control(pDX, IDC_AUTOGO_CAPACITY, m_capacity);
	DDX_Control(pDX, IDC_AUTOGO_BLANK, m_blank);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEWHITELIST, m_battleWhiteList);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEPLAYER, m_battlePlayer);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEMONSTER, m_battleMonster);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEGM, m_battleGM);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONSOULPOINT, m_actionSoulPoint);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONSIGN, m_actionSign);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONMOVE, m_actionMove);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONMESSAGE, m_actionMessage);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONHPLOSS, m_actionHpLoss);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONCAPACITY, m_actionCapacity);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONBLANK, m_actionBlank);
	DDX_Control(pDX, IDC_AUTOGO_STATUS, m_status);
	DDX_Control(pDX, IDC_AUTOGO_CUR_Z, m_curZ);
	DDX_Control(pDX, IDC_AUTOGO_CUR_Y, m_curY);
	DDX_Control(pDX, IDC_AUTOGO_CUR_X, m_curX);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_Z, m_runawayZ);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_Y, m_runawayY);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_X, m_runawayX);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_AUTOGO_TORUNAWAY, OnAutogoTorunaway)	
	ON_BN_CLICKED(IDC_AUTOGO_TOSTART, OnAutogoTostart)
	ON_BN_CLICKED(IDC_AUTOGO_WHITELIST, OnAutogoWhitelist)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERBATTLELIST, OnAutogoTriggerbattlelist)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERSIGN, OnAutogoTriggersign)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERMESSAGE, OnAutogoTriggermessage)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERHPLOSS, OnAutogoTriggerhploss)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERMOVE, OnAutogoTriggermove)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERSOULPOINTS, OnAutogoTriggersoulpoints)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERBLANK, OnAutogoTriggerblank)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERCAPACITY, OnAutogoTriggercapacity)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGEROUTOF, OnAutogoTriggeroutof)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERHPBELOW, OnAutogoTriggerhpbelow)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERHPABOVE, OnAutogoTriggerhpabove)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERMANABELOW, OnAutogoTriggermanabelow)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERMANAABOVE, OnAutogoTriggermanaabove)
	ON_BN_CLICKED(IDC_AUTOGO_TRIGGERSOULPOINTS_ABOVE, OnAutogoTriggersoulpointsAbove)
	ON_BN_CLICKED(IDC_AUTOGO_OUTOFFOOD, OnAutogoOutoffood)
	ON_BN_CLICKED(IDC_AUTOGO_OUTOFSPACE, OnAutogoOutofspace)
	ON_BN_CLICKED(IDC_AUTOGO_OUTOFCUSTOM, OnAutogoOutofcustom)
	ON_BN_CLICKED(IDC_AUTOGO_RUNAWAY_REACHED, OnAutogoRunawayReached)
	ON_BN_CLICKED(IDC_AUTOGO_BATTLEPLAYER, OnAutogoBattleplayer)
	ON_BN_CLICKED(IDC_AUTOGO_BATTLEMONSTER, OnAutogoBattlemonster)
	ON_BN_CLICKED(IDC_AUTOGO_BATTLEGM, OnAutogoBattlegm)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_AUTOGO_BATTELIST, OnAutogoBattelist)
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
	m_triggerBattleList.EnableWindow(false);
	m_battlePlayer.EnableWindow(false);
	m_battleMonster.EnableWindow(false);
	m_battleGM.EnableWindow(false);
	m_battleBattlelist.EnableWindow(false);
	m_battleParanoiaM.EnableWindow(false);
	m_battleWhiteList.EnableWindow(false);	
	
	m_triggerSign.EnableWindow(false);
	m_signBattle.EnableWindow(false);
	m_signPoison.EnableWindow(false);
	m_signFire.EnableWindow(false);
	m_signEnergy.EnableWindow(false);
	m_actionSign.EnableWindow(false);
	m_soundSign.EnableWindow(false);
	
	m_triggerMessage.EnableWindow(false);
	m_messagePrivate.EnableWindow(false);
	m_messagePublic.EnableWindow(false);
	m_actionMessage.EnableWindow(false);
	m_soundMessage.EnableWindow(false);
	
	m_triggerHpLoss.EnableWindow(false);
	m_actionHpLoss.EnableWindow(false);
	m_soundHpLoss.EnableWindow(false);

	m_triggerHpBelow.EnableWindow(false);
	m_hpBelow.EnableWindow(false);
	m_actionHpBelow.EnableWindow(false);
	m_soundHpBelow.EnableWindow(false);
	
	m_triggerMove.EnableWindow(false);
	m_actionMove.EnableWindow(false);
	m_soundMove.EnableWindow(false);
	
	m_triggerSoulPoint.EnableWindow(false);
	m_soulPoint.EnableWindow(false);
	m_actionSoulPoint.EnableWindow(false);
	m_soundSoulPoint.EnableWindow(false);
	
	m_triggerBlank.EnableWindow(false);
	m_blank.EnableWindow(false);
	m_actionBlank.EnableWindow(false);
	m_soundBlank.EnableWindow(false);
	
	m_triggerCapacity.EnableWindow(false);
	m_capacity.EnableWindow(false);
	m_actionCapacity.EnableWindow(false);
	m_soundCapacity.EnableWindow(false);
	
	m_triggerOutOf.EnableWindow(false);
	m_outOfFood.EnableWindow(false);	
	m_outOfSpace.EnableWindow(false);	
	
	//m_status.EnableWindow(false);
	m_curZ.EnableWindow(false);
	m_curY.EnableWindow(false);
	m_curX.EnableWindow(false);
	m_runawayZ.EnableWindow(false);
	m_runawayY.EnableWindow(false);
	m_runawayX.EnableWindow(false);
	m_actZ.EnableWindow(false);
	m_actY.EnableWindow(false);
	m_actX.EnableWindow(false);
	m_actDirection.EnableWindow(false);

	m_triggerOutOf.EnableWindow(false);
	m_triggerSoulPointAbove.EnableWindow(false);
	m_triggerManaBelow.EnableWindow(false);
	m_triggerManaAbove.EnableWindow(false);
	m_triggerHpAbove.EnableWindow(false);
	m_soundSoulPointAbove.EnableWindow(false);
	m_soundRunawayReached.EnableWindow(false);
	m_soundOutOfSpace.EnableWindow(false);
	m_soundOutOfFood.EnableWindow(false);
	m_soundOutOfCustom.EnableWindow(false);
	m_soundManaBelow.EnableWindow(false);
	m_soundManaAbove.EnableWindow(false);
	m_soundHpAbove.EnableWindow(false);
	m_soulPointAbove.EnableWindow(false);
	m_runawayReachedRadius.EnableWindow(false);
	m_runawayReached.EnableWindow(false);
	m_outOfCustom.EnableWindow(false);
	m_messageIgnoreSpells.EnableWindow(false);
	m_manaBelow.EnableWindow(false);
	m_manaAbove.EnableWindow(false);
	m_hpAbove.EnableWindow(false);
	m_actionSoulpointAbove.EnableWindow(false);
	m_actionRunawayReached.EnableWindow(false);
	m_actionOutOfSpace.EnableWindow(false);
	m_actionOutOfFood.EnableWindow(false);
	m_outOfCustomItem.EnableWindow(false);
	m_actionOutOfCustom.EnableWindow(false);
	m_actionManaBelow.EnableWindow(false);
	m_actionManaAbove.EnableWindow(false);
	m_actionHpAbove.EnableWindow(false);
	m_soundBattleListPlayer.EnableWindow(false);
	m_soundBattleListMonster.EnableWindow(false);
	m_soundBattleListList.EnableWindow(false);
	m_soundBattleListGm.EnableWindow(false);
	m_actionBattleListPlayer.EnableWindow(false);
	m_actionBattleListMonster.EnableWindow(false);
	m_actionBattleListList.EnableWindow(false);
	m_manaBelowUntilRecovery.EnableWindow(false);
	m_hpBelowUntilRecovery.EnableWindow(false);
	m_actionBattleListGm.EnableWindow(false);
}


void CConfigDialog::enableControls()
{
	m_triggerBattleList.EnableWindow(true);
	OnAutogoTriggerbattlelist();
	
	OnAutogoBattleplayer();
	
	OnAutogoBattlemonster();
	
	OnAutogoBattlegm();
	
	OnAutogoBattelist();
	
	m_triggerSign.EnableWindow(true);
	OnAutogoTriggersign();
	
	m_triggerMessage.EnableWindow(true);
	OnAutogoTriggermessage();
	
	m_triggerHpLoss.EnableWindow(true);
	OnAutogoTriggerhploss();

	m_triggerHpBelow.EnableWindow(true);
	OnAutogoTriggerhpbelow();
	
	m_triggerMove.EnableWindow(true);
	OnAutogoTriggermove();
	
	m_triggerSoulPoint.EnableWindow(true);
	OnAutogoTriggersoulpoints();
	
	m_triggerBlank.EnableWindow(true);
	OnAutogoTriggerblank();
	
	m_triggerCapacity.EnableWindow(true);
	OnAutogoTriggercapacity();
	
	m_triggerOutOf.EnableWindow(true);
	OnAutogoTriggeroutof();

	m_triggerHpAbove.EnableWindow(true);
	OnAutogoTriggerhpabove();

	m_triggerManaBelow.EnableWindow(true);
	OnAutogoTriggermanabelow();

	m_triggerManaAbove.EnableWindow(true);
	OnAutogoTriggermanaabove();

	m_triggerSoulPointAbove.EnableWindow(true);
	OnAutogoTriggersoulpointsAbove();

	m_triggerOutOf.EnableWindow(true);
	
	OnAutogoOutoffood();
	
	OnAutogoOutofspace();
	
	OnAutogoOutofcustom();

	m_runawayReached.EnableWindow(true);
	OnAutogoRunawayReached();	

	
	//m_status.EnableWindow(true);
	m_curZ.EnableWindow(true);
	m_curY.EnableWindow(true);
	m_curX.EnableWindow(true);
	m_runawayZ.EnableWindow(true);
	m_runawayY.EnableWindow(true);
	m_runawayX.EnableWindow(true);
	m_actZ.EnableWindow(true);
	m_actY.EnableWindow(true);
	m_actX.EnableWindow(true);
	m_actDirection.EnableWindow(true);





}



void CConfigDialog::configToControls(CConfigData *configData)
{	
	CTibiaItemProxy itemProxy;
	char buf[128];

	sprintf(buf,"%d",configData->actX);		m_actX.SetWindowText(buf);
	sprintf(buf,"%d",configData->actY);		m_actY.SetWindowText(buf);
	sprintf(buf,"%d",configData->actZ);		m_actZ.SetWindowText(buf);
	m_actDirection.SetCurSel(configData->actDirection);
	sprintf(buf,"%d",configData->runawayX);	m_runawayX.SetWindowText(buf);
	sprintf(buf,"%d",configData->runawayY);	m_runawayY.SetWindowText(buf);
	sprintf(buf,"%d",configData->runawayZ);	m_runawayZ.SetWindowText(buf);	

	m_triggerBattleList.SetCheck((configData->trigger&TRIGGER_BATTLELIST?1:0));	
	m_battlePlayer.SetCheck((configData->trigger&TRIGGER_BATTLELIST_PLAYER?1:0));	
	m_battleMonster.SetCheck((configData->trigger&TRIGGER_BATTLELIST_MONSTER?1:0));	
	m_battleGM.SetCheck((configData->trigger&TRIGGER_BATTLELIST_GM?1:0));	
	m_battleBattlelist.SetCheck((configData->trigger&TRIGGER_BATTLELIST_LIST?1:0));		
	m_battleParanoiaM.SetCheck((configData->optionsBattleList&BATTLELIST_PARANOIAM?1:0));	
	m_actionBattleListGm.SetCurSel(configData->actionBattleListGm);
	m_actionBattleListPlayer.SetCurSel(configData->actionBattleListPlayer);
	m_actionBattleListMonster.SetCurSel(configData->actionBattleListMonster);
	m_actionBattleListList.SetCurSel(configData->actionBattleListList);
	m_soundBattleListGm.SetCheck((configData->sound&TRIGGER_BATTLELIST_GM?1:0));	
	m_soundBattleListMonster.SetCheck((configData->sound&TRIGGER_BATTLELIST_MONSTER?1:0));	
	m_soundBattleListPlayer.SetCheck((configData->sound&TRIGGER_BATTLELIST_PLAYER?1:0));
	m_soundBattleListList.SetCheck((configData->sound&TRIGGER_BATTLELIST_LIST?1:0));

	m_triggerOutOf.SetCheck((configData->trigger&TRIGGER_OUTOF?1:0));
	
	m_actionOutOfFood.SetCurSel(configData->actionOutOfFood);
	m_outOfFood.SetCheck((configData->trigger&TRIGGER_OUTOF_FOOD?1:0));
	m_soundOutOfFood.SetCheck((configData->sound&TRIGGER_OUTOF_FOOD?1:0));

	m_actionOutOfCustom.SetCurSel(configData->actionOutOfCustom);
	m_outOfCustom.SetCheck((configData->trigger&TRIGGER_OUTOF_CUSTOM?1:0));
	m_soundOutOfCustom.SetCheck((configData->sound&TRIGGER_OUTOF_CUSTOM?1:0));

	m_actionOutOfSpace.SetCurSel(configData->actionOutOfSpace);
	m_outOfSpace.SetCheck((configData->trigger&TRIGGER_OUTOF_SPACE?1:0));
	m_soundOutOfSpace.SetCheck((configData->sound&TRIGGER_OUTOF_SPACE?1:0));
	m_outOfCustomItem.SelectString(-1,itemProxy.getName(configData->optionsOutOfCustomItem));	

	
	m_triggerSign.SetCheck((configData->trigger&TRIGGER_SIGN?1:0));
	m_signBattle.SetCheck((configData->optionsSign&SIGN_BATTLE?1:0));
	m_signPoison.SetCheck((configData->optionsSign&SIGN_POISON?1:0));
	m_signFire.SetCheck((configData->optionsSign&SIGN_FIRE?1:0));
	m_signEnergy.SetCheck((configData->optionsSign&SIGN_ENERGY?1:0));
	m_actionSign.SetCurSel(configData->actionSign);
	m_soundSign.SetCheck((configData->sound&TRIGGER_SIGN?1:0));	
	
	m_triggerMessage.SetCheck((configData->trigger&TRIGGER_MESSAGE?1:0));
	m_messagePrivate.SetCheck((configData->optionsMessage&MESSAGE_PRIVATE?1:0));
	m_messagePublic.SetCheck((configData->optionsMessage&MESSAGE_PUBLIC?1:0));
	m_messageIgnoreSpells.SetCheck((configData->optionsMessage&MESSAGE_IGNORE_SPELLS?1:0));
	m_actionMessage.SetCurSel(configData->actionMessage);
	m_soundMessage.SetCheck((configData->sound&TRIGGER_MESSAGE?1:0));

	m_triggerMove.SetCheck((configData->trigger&TRIGGER_MOVE?1:0));
	m_actionMove.SetCurSel(configData->actionMove);
	m_soundMove.SetCheck((configData->sound&TRIGGER_MOVE?1:0));
	
	m_triggerHpLoss.SetCheck((configData->trigger&TRIGGER_HPLOSS?1:0));
	m_actionHpLoss.SetCurSel(configData->actionHpLoss);
	m_soundHpLoss.SetCheck((configData->sound&TRIGGER_HPLOSS?1:0));

	m_triggerHpBelow.SetCheck((configData->trigger&TRIGGER_HPBELOW?1:0));
	sprintf(buf,"%d",configData->optionsHpBelow);m_hpBelow.SetWindowText(buf);
	m_actionHpBelow.SetCurSel(configData->actionHpBelow);
	m_soundHpBelow.SetCheck((configData->sound&TRIGGER_HPBELOW?1:0));

	m_triggerHpAbove.SetCheck((configData->trigger&TRIGGER_HPABOVE?1:0));
	sprintf(buf,"%d",configData->optionsHpAbove);m_hpAbove.SetWindowText(buf);
	m_actionHpAbove.SetCurSel(configData->actionHpAbove);
	m_soundHpAbove.SetCheck((configData->sound&TRIGGER_HPABOVE?1:0));

	m_triggerManaBelow.SetCheck((configData->trigger&TRIGGER_MANABELOW?1:0));
	sprintf(buf,"%d",configData->optionsManaBelow);m_manaBelow.SetWindowText(buf);	
	m_actionManaBelow.SetCurSel(configData->actionManaBelow);
	m_soundManaBelow.SetCheck((configData->sound&TRIGGER_MANABELOW?1:0));

	m_triggerManaAbove.SetCheck((configData->trigger&TRIGGER_MANAABOVE?1:0));
	sprintf(buf,"%d",configData->optionsManaAbove);m_manaAbove.SetWindowText(buf);
	m_actionManaAbove.SetCurSel(configData->actionManaAbove);
	m_soundManaAbove.SetCheck((configData->sound&TRIGGER_MANAABOVE?1:0));
	
	m_triggerSoulPoint.SetCheck((configData->trigger&TRIGGER_SOULPOINT_BELOW?1:0));
	sprintf(buf,"%d",configData->optionsSoulPointBelow);m_soulPoint.SetWindowText(buf);
	m_actionSoulPoint.SetCurSel(configData->actionSoulPointBelow);
	m_soundSoulPoint.SetCheck((configData->sound&TRIGGER_SOULPOINT_BELOW?1:0));

	m_triggerSoulPointAbove.SetCheck((configData->trigger&TRIGGER_SOULPOINT_ABOVE?1:0));
	sprintf(buf,"%d",configData->optionsSoulPointAbove);m_soulPointAbove.SetWindowText(buf);
	m_actionSoulpointAbove.SetCurSel(configData->actionSoulPointAbove);
	m_soundSoulPointAbove.SetCheck((configData->sound&TRIGGER_SOULPOINT_ABOVE?1:0));
	
	m_triggerBlank.SetCheck((configData->trigger&TRIGGER_BLANK?1:0));
	sprintf(buf,"%d",configData->optionsBlank);m_blank.SetWindowText(buf);
	m_actionBlank.SetCurSel(configData->actionBlank);
	m_soundBlank.SetCheck((configData->sound&TRIGGER_BLANK?1:0));
	
	m_triggerCapacity.SetCheck((configData->trigger&TRIGGER_CAPACITY?1:0));
	sprintf(buf,"%d",configData->optionsCapacity);m_capacity.SetWindowText(buf);
	m_actionCapacity.SetCurSel(configData->actionCapacity);
	m_soundCapacity.SetCheck((configData->sound&TRIGGER_CAPACITY?1:0));
	
	m_runawayReached.SetCheck((configData->trigger&TRIGGER_RUNAWAY_REACHED?1:0));
	sprintf(buf,"%d",configData->optionsRunawayReached);m_runawayReachedRadius.SetWindowText(buf);
	m_actionRunawayReached.SetCurSel(configData->actionRunawayReached);
	m_soundRunawayReached.SetCheck((configData->sound&TRIGGER_RUNAWAY_REACHED?1:0));

	


	memcpy(memWhiteList,configData->whiteList,3200);

	OnAutogoTriggerbattlelist();
	OnAutogoTriggersign();
	OnAutogoTriggermessage();
	OnAutogoTriggermove();
	OnAutogoTriggerhploss();
	OnAutogoTriggerhpbelow();
	OnAutogoTriggersoulpoints();
	OnAutogoTriggerblank();
	OnAutogoTriggercapacity();
	OnAutogoTriggeroutof();
	OnAutogoTriggerhpabove();
	OnAutogoTriggermanabelow();
	OnAutogoTriggermanaabove();
	OnAutogoTriggersoulpointsAbove();
	OnAutogoOutoffood();
	OnAutogoOutofspace();
	OnAutogoOutofcustom();
	OnAutogoRunawayReached();
	OnAutogoBattleplayer();
	OnAutogoBattlemonster();
	OnAutogoBattlegm();
	OnAutogoBattelist();
	
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CTibiaItemProxy itemProxy;
	char buf[128];	
	CConfigData *newConfigData = new CConfigData();
	
	m_actX.GetWindowText(buf,127);		newConfigData->actX=atoi(buf);
	m_actY.GetWindowText(buf,127);		newConfigData->actY=atoi(buf);
	m_actZ.GetWindowText(buf,127);		newConfigData->actZ=atoi(buf);
	newConfigData->actDirection = m_actDirection.GetCurSel();
	m_runawayX.GetWindowText(buf,127);	newConfigData->runawayX=atoi(buf);
	m_runawayY.GetWindowText(buf,127);	newConfigData->runawayY=atoi(buf);
	m_runawayZ.GetWindowText(buf,127);	newConfigData->runawayZ=atoi(buf);		

	newConfigData->trigger = (
		(m_triggerBattleList.GetCheck()?TRIGGER_BATTLELIST:0)|
		((m_triggerBattleList.GetCheck()&&m_battleBattlelist.GetCheck())?TRIGGER_BATTLELIST_LIST:0)|
		((m_triggerBattleList.GetCheck()&&m_battlePlayer.GetCheck())?TRIGGER_BATTLELIST_PLAYER:0)|
		((m_triggerBattleList.GetCheck()&&m_battleMonster.GetCheck())?TRIGGER_BATTLELIST_MONSTER:0)|
		((m_triggerBattleList.GetCheck()&&m_battleGM.GetCheck())?TRIGGER_BATTLELIST_GM:0)|
		(m_triggerSign.GetCheck()?TRIGGER_SIGN:0)|
		(m_triggerMessage.GetCheck()?TRIGGER_MESSAGE:0)|
		(m_triggerMove.GetCheck()?TRIGGER_MOVE:0)|
		(m_triggerHpLoss.GetCheck()?TRIGGER_HPLOSS:0)|
		(m_triggerHpBelow.GetCheck()?TRIGGER_HPBELOW:0)|
		(m_triggerHpAbove.GetCheck()?TRIGGER_HPABOVE:0)|
		(m_triggerManaBelow.GetCheck()?TRIGGER_MANABELOW:0)|
		(m_triggerManaAbove.GetCheck()?TRIGGER_MANAABOVE:0)|
		(m_triggerSoulPoint.GetCheck()?TRIGGER_SOULPOINT_BELOW:0)|
		(m_triggerSoulPointAbove.GetCheck()?TRIGGER_SOULPOINT_ABOVE:0)|
		(m_triggerBlank.GetCheck()?TRIGGER_BLANK:0)|
		(m_triggerCapacity.GetCheck()?TRIGGER_CAPACITY:0)|
		(m_triggerOutOf.GetCheck()?TRIGGER_OUTOF:0)|		
		((m_triggerOutOf.GetCheck()&&m_outOfFood.GetCheck())?TRIGGER_OUTOF_FOOD:0)|
		((m_triggerOutOf.GetCheck()&&m_outOfCustom.GetCheck())?TRIGGER_OUTOF_CUSTOM:0)|
		((m_triggerOutOf.GetCheck()&&m_outOfSpace.GetCheck())?TRIGGER_OUTOF_SPACE:0)|
		((m_triggerOutOf.GetCheck()&&m_runawayReached.GetCheck())?TRIGGER_RUNAWAY_REACHED:0)
		);

	newConfigData->optionsBattleList = (		
		(m_battleParanoiaM.GetCheck()?BATTLELIST_PARANOIAM:0)
		);
	
	newConfigData->optionsSign = (
		(m_signBattle.GetCheck()?SIGN_BATTLE:0)|
		(m_signPoison.GetCheck()?SIGN_POISON:0)|
		(m_signFire.GetCheck()?SIGN_FIRE:0)|
		(m_signEnergy.GetCheck()?SIGN_ENERGY:0)
		);
	
	newConfigData->optionsMessage = (
		(m_messagePrivate.GetCheck()?MESSAGE_PRIVATE:0)|
		(m_messagePublic.GetCheck()?MESSAGE_PUBLIC:0)|
		(m_messageIgnoreSpells.GetCheck()?MESSAGE_IGNORE_SPELLS:0)
		);
	

	newConfigData->sound = (		
		(m_soundBattleListGm.GetCheck()?TRIGGER_BATTLELIST_GM:0)|
		(m_soundBattleListMonster.GetCheck()?TRIGGER_BATTLELIST_MONSTER:0)|
		(m_soundBattleListPlayer.GetCheck()?TRIGGER_BATTLELIST_PLAYER:0)|
		(m_soundBattleListList.GetCheck()?TRIGGER_BATTLELIST_LIST:0)|
		(m_soundOutOfCustom.GetCheck()?TRIGGER_OUTOF_CUSTOM:0)|
		(m_soundOutOfFood.GetCheck()?TRIGGER_OUTOF_FOOD:0)|
		(m_soundOutOfSpace.GetCheck()?TRIGGER_OUTOF_SPACE:0)|
		(m_soundSign.GetCheck()?TRIGGER_SIGN:0)|
		(m_soundMessage.GetCheck()?TRIGGER_MESSAGE:0)|
		(m_soundHpLoss.GetCheck()?TRIGGER_HPLOSS:0)|
		(m_soundHpBelow.GetCheck()?TRIGGER_HPBELOW:0)|
		(m_soundHpAbove.GetCheck()?TRIGGER_HPABOVE:0)|
		(m_soundManaBelow.GetCheck()?TRIGGER_MANABELOW:0)|
		(m_soundManaAbove.GetCheck()?TRIGGER_MANAABOVE:0)|
		(m_soundMove.GetCheck()?TRIGGER_MOVE:0)|
		(m_soundSoulPoint.GetCheck()?TRIGGER_SOULPOINT_BELOW:0)|
		(m_soundSoulPointAbove.GetCheck()?TRIGGER_SOULPOINT_ABOVE:0)|
		(m_soundBlank.GetCheck()?TRIGGER_BLANK:0)|
		(m_soundCapacity.GetCheck()?TRIGGER_CAPACITY:0)|
		(m_soundRunawayReached.GetCheck()?TRIGGER_RUNAWAY_REACHED:0)
		);

	
	newConfigData->actionSign		= m_actionSign.GetCurSel();
	
	newConfigData->actionMessage	= m_actionMessage.GetCurSel();
	newConfigData->actionHpLoss		= m_actionHpLoss.GetCurSel();
	newConfigData->actionHpBelow	= m_actionHpBelow.GetCurSel();
	newConfigData->actionHpAbove	= m_actionHpAbove.GetCurSel();
	newConfigData->actionManaBelow	= m_actionManaBelow.GetCurSel();
	newConfigData->actionManaAbove	= m_actionManaAbove.GetCurSel();
	newConfigData->actionMove		= m_actionMove.GetCurSel();
	newConfigData->actionSoulPointBelow	= m_actionSoulPoint.GetCurSel();
	newConfigData->actionSoulPointAbove	= m_actionSoulpointAbove.GetCurSel();
	newConfigData->actionBlank		= m_actionBlank.GetCurSel();
	newConfigData->actionCapacity	= m_actionCapacity.GetCurSel();	
	newConfigData->actionRunawayReached	= m_actionRunawayReached.GetCurSel();	
	newConfigData->actionOutOfFood	= m_actionOutOfFood.GetCurSel();	
	newConfigData->actionOutOfSpace	= m_actionOutOfSpace.GetCurSel();	
	newConfigData->actionOutOfCustom= m_actionOutOfCustom.GetCurSel();	
	newConfigData->actionBattleListGm= m_actionBattleListGm.GetCurSel();	
	newConfigData->actionBattleListList= m_actionBattleListList.GetCurSel();	
	newConfigData->actionBattleListMonster= m_actionBattleListMonster.GetCurSel();	
	newConfigData->actionBattleListPlayer= m_actionBattleListPlayer.GetCurSel();	
	newConfigData->allActions=0;
	newConfigData->allActions|=actionPos2ID(newConfigData->actionMessage);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionHpLoss);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionHpBelow);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionHpAbove);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionManaBelow);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionManaAbove);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionMove);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionSoulPointBelow);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionSoulPointAbove);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionBlank);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionCapacity);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionRunawayReached);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionOutOfFood);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionOutOfSpace);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionOutOfCustom);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionBattleListGm);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionBattleListList);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionBattleListMonster);
	newConfigData->allActions|=actionPos2ID(newConfigData->actionBattleListPlayer);


	m_hpBelow.GetWindowText(buf,127);	newConfigData->optionsHpBelow=atoi(buf);
	m_hpAbove.GetWindowText(buf,127);	newConfigData->optionsHpAbove=atoi(buf);
	m_manaBelow.GetWindowText(buf,127);	newConfigData->optionsManaBelow=atoi(buf);
	m_manaAbove.GetWindowText(buf,127);	newConfigData->optionsManaAbove=atoi(buf);
	m_soulPoint.GetWindowText(buf,127);	newConfigData->optionsSoulPointBelow=atoi(buf);
	m_soulPointAbove.GetWindowText(buf,127);	newConfigData->optionsSoulPointAbove=atoi(buf);
	m_blank.GetWindowText(buf,127);		newConfigData->optionsBlank=atoi(buf);
	m_capacity.GetWindowText(buf,127);	newConfigData->optionsCapacity=atoi(buf);
	m_runawayReachedRadius.GetWindowText(buf,127);	newConfigData->optionsRunawayReached=atoi(buf);
	newConfigData->optionsHpBelowUntilRecovery=m_hpBelowUntilRecovery.GetCheck();
	newConfigData->optionsManaBelowUntilRecovery=m_manaBelowUntilRecovery.GetCheck();
	buf[0]='\0';
	m_outOfCustomItem.GetLBText(m_outOfCustomItem.GetCurSel(),buf);
	newConfigData->optionsOutOfCustomItem=itemProxy.getObjectId(buf);

	memcpy(newConfigData->whiteList,memWhiteList,3200);
	newConfigData->status			= status;
	
	

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==1001)
	{
		CMemReaderProxy reader;

		CTibiaCharacter *self = reader.readSelfCharacter();

		char buf[256];

		if (!m_enable.GetCheck())
		{
			sprintf(buf,"%d",self->x);
			m_curX.SetWindowText(buf);
			sprintf(buf,"%d",self->y);
			m_curY.SetWindowText(buf);
			sprintf(buf,"%d",self->z);
			m_curZ.SetWindowText(buf);
			triggerMessage();
		}else if (!m_triggerMessage.GetCheck()){
			triggerMessage();
		}

		sprintf(buf,"Status: %s",status);
		m_status.SetWindowText(buf);

		delete self;
	}
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CComboBox *list[] = {
		&m_actionBattleListMonster,
		&m_actionBattleListGm,
		&m_actionBattleListPlayer,
		&m_actionBattleListList,
		&m_actionSign,
		&m_actionMessage,
		&m_actionMove,
		&m_actionHpLoss,
		&m_actionHpAbove,
		&m_actionHpBelow,
		&m_actionManaAbove,
		&m_actionManaBelow,
		&m_actionSoulPoint,
		&m_actionSoulpointAbove,
		&m_actionBlank,
		&m_actionCapacity,
		&m_actionOutOfFood,
		&m_actionOutOfCustom,
		&m_actionOutOfSpace,
		&m_actionRunawayReached,
		NULL
	};

	CRect lpRect;

	for (int i=0;list[i]!=NULL;i++){
		list[i]->InsertString(ACTION_NONE_POS,"(none)");		
		list[i]->InsertString(ACTION_SUSPEND_POS,"Suspend macro");		
		list[i]->InsertString(ACTION_LOGOUT_POS,"Logout");		
		list[i]->InsertString(ACTION_KILL_POS,"Logout+Kill");		
		list[i]->InsertString(ACTION_SHUTDOWN_POS,"Logout+Shutdown");		
		list[i]->InsertString(ACTION_RUNAWAY_POS,"Runaway");
		list[i]->InsertString(ACTION_RUNAWAY_BACK_POS,"Runaway+back");
		list[i]->InsertString(ACTION_RUNAWAY_CAVEBOOT_HALFSLEEP_POS,"Runaway+half attack");
		list[i]->InsertString(ACTION_RUNAWAY_CAVEBOOT_FULLSLEEP_POS,"Runaway+no attack");
		
		
		//T4: Corrects problem with dropdown list height problem
		list[i]->GetWindowRect( &lpRect );
		lpRect.bottom = lpRect.top + 6 * list[i]->GetItemHeight(-1) + lpRect.Height();
		list[i]->SetWindowPos(NULL, 0, 0, lpRect.Width(), lpRect.Height(), SWP_NOMOVE | SWP_NOZORDER );
	}

	OnAutogoTriggerbattlelist();
	OnAutogoTriggersign();
	OnAutogoTriggermessage();
	OnAutogoTriggermove();
	OnAutogoTriggerhploss();
	OnAutogoTriggerhpbelow();
	OnAutogoTriggersoulpoints();
	OnAutogoTriggerblank();
	OnAutogoTriggercapacity();
	OnAutogoTriggeroutof();
	OnAutogoTriggerhpabove();
	OnAutogoTriggermanabelow();
	OnAutogoTriggermanaabove();
	OnAutogoTriggersoulpointsAbove();
	OnAutogoOutoffood();
	OnAutogoOutofspace();
	OnAutogoOutofcustom();
	OnAutogoRunawayReached();
	OnAutogoBattleplayer();
	OnAutogoBattlemonster();
	OnAutogoBattlegm();
	OnAutogoBattelist();


	m_actDirection.InsertString(0,"(not set)");
	m_actDirection.InsertString(DIR_LEFT,"Left");
	m_actDirection.InsertString(DIR_RIGHT,"Right");
	m_actDirection.InsertString(DIR_UP,"Up");
	m_actDirection.InsertString(DIR_DOWN,"Down");

	CImageList imgList;
	imgList.Create(11,11,ILC_COLOR8|ILC_MASK,4,0);

	CBitmap bitmap1;
	bitmap1.LoadBitmap(IDB_BATTLE);
	imgList.Add(&bitmap1,RGB(0,0,255));

	CBitmap bitmap2;
	bitmap2.LoadBitmap(IDB_POISON);
	imgList.Add(&bitmap2,RGB(0,0,255));

	CBitmap bitmap3;
	bitmap3.LoadBitmap(IDB_FIRE);
	imgList.Add(&bitmap3,RGB(0,0,255));

	CBitmap bitmap4;
	bitmap4.LoadBitmap(IDB_ENERGY);
	imgList.Add(&bitmap4,RGB(255,0,0));

	m_signBattle.SetIcon(imgList.ExtractIcon(0));
	m_signPoison.SetIcon(imgList.ExtractIcon(1));
	m_signFire.SetIcon(imgList.ExtractIcon(2));
	m_signEnergy.SetIcon(imgList.ExtractIcon(3));

	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	int count = itemProxy.getItemsItemsCount();
	for (i=0;i<count;i++)
	{
		int objectId=itemProxy.getObjectId(itemProxy.getItemsItems(i));
		CTibiaTile *tile = reader.getTibiaTile(objectId);
		if (tile)
		{
			m_outOfCustomItem.AddString(itemProxy.getItemsItems(i));			
		}
	}

	memset(status,0,128);

	SetTimer(1001,250,NULL);

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


void CConfigDialog::OnAutogoTorunaway() 
{
	char buf[500];
	m_curX.GetWindowText(buf,499);
	m_runawayX.SetWindowText(buf);
	m_curY.GetWindowText(buf,499);
	m_runawayY.SetWindowText(buf);
	m_curZ.GetWindowText(buf,499);
	m_runawayZ.SetWindowText(buf);
}

void CConfigDialog::OnAutogoTostart() 
{
	char buf[500];
	m_curX.GetWindowText(buf,499);
	m_actX.SetWindowText(buf);
	m_curY.GetWindowText(buf,499);
	m_actY.SetWindowText(buf);
	m_curZ.GetWindowText(buf,499);
	m_actZ.SetWindowText(buf);
}

void CConfigDialog::OnAutogoWhitelist() 
{
	CWhiteList *dialog = new CWhiteList(memWhiteList);
	dialog->DoModal();
	delete dialog;
}

void CConfigDialog::OnAutogoTriggerbattlelist() 
{
	int lVal = (m_triggerBattleList.IsWindowEnabled()?m_triggerBattleList.GetCheck():0);
	
	m_battleParanoiaM.EnableWindow(lVal);
	m_battleWhiteList.EnableWindow(lVal);
	m_battleBattlelist.EnableWindow(lVal);
	m_battleMonster.EnableWindow(lVal);
	m_battlePlayer.EnableWindow(lVal);
	m_battleGM.EnableWindow(lVal);
	
	OnAutogoBattleplayer();
	OnAutogoBattlemonster();
	OnAutogoBattlegm();
	OnAutogoBattelist();
}

void CConfigDialog::OnAutogoTriggersign() 
{
	int lVal = (m_triggerSign.IsWindowEnabled()?m_triggerSign.GetCheck():0);
	m_signBattle.EnableWindow(lVal);
	m_signPoison.EnableWindow(lVal);
	m_signFire.EnableWindow(lVal);
	m_signEnergy.EnableWindow(lVal);
	m_actionSign.EnableWindow(lVal);
	m_soundSign.EnableWindow(lVal);
}

void CConfigDialog::OnAutogoTriggermessage() 
{
	int lVal = (m_triggerMessage.IsWindowEnabled()?m_triggerMessage.GetCheck():0);
	m_messagePrivate.EnableWindow(lVal);
	m_messagePublic.EnableWindow(lVal);
	m_messageIgnoreSpells.EnableWindow(lVal);
	m_actionMessage.EnableWindow(lVal);
	m_soundMessage.EnableWindow(lVal);
}

void CConfigDialog::OnAutogoTriggerhploss() 
{
	int lVal = (m_triggerHpLoss.IsWindowEnabled()?m_triggerHpLoss.GetCheck():0);
	m_soundHpLoss.EnableWindow(lVal);
	m_actionHpLoss.EnableWindow(lVal);
	m_soundHpLoss.EnableWindow(lVal);
}

void CConfigDialog::OnAutogoTriggermove() 
{
	int lVal = (m_triggerMove.IsWindowEnabled()?m_triggerMove.GetCheck():0);
	m_actionMove.EnableWindow(lVal);
	m_soundMove.EnableWindow(lVal);
}

void CConfigDialog::OnAutogoTriggersoulpoints() 
{
	int lVal = (m_triggerSoulPoint.IsWindowEnabled()?m_triggerSoulPoint.GetCheck():0);
	m_soulPoint.EnableWindow(lVal);
	m_actionSoulPoint.EnableWindow(lVal);
	m_soundSoulPoint.EnableWindow(lVal);
}

void CConfigDialog::OnAutogoTriggerblank() 
{
	int lVal = (m_triggerBlank.IsWindowEnabled()?m_triggerBlank.GetCheck():0);
	m_blank.EnableWindow(lVal);
	m_actionBlank.EnableWindow(lVal);
	m_soundBlank.EnableWindow(lVal);
}

void CConfigDialog::OnAutogoTriggercapacity() 
{
	int lVal = (m_triggerCapacity.IsWindowEnabled()?m_triggerCapacity.GetCheck():0);
	m_capacity.EnableWindow(lVal);
	m_actionCapacity.EnableWindow(lVal);
	m_soundCapacity.EnableWindow(lVal);
}

void CConfigDialog::OnAutogoTriggeroutof() 
{	
	int lVal = (m_triggerOutOf.IsWindowEnabled()?m_triggerOutOf.GetCheck():0);

	m_outOfFood.EnableWindow(lVal);
	m_outOfSpace.EnableWindow(lVal);
	m_outOfCustom.EnableWindow(lVal);

	OnAutogoOutoffood();
	OnAutogoOutofspace();
	OnAutogoOutofcustom();
}

void CConfigDialog::OnAutogoTriggerhpbelow() 
{
	int lVal = (m_triggerHpBelow.IsWindowEnabled()?m_triggerHpBelow.GetCheck():0);
	m_hpBelow.EnableWindow(lVal);
	m_actionHpBelow.EnableWindow(lVal);
	m_soundHpBelow.EnableWindow(lVal);
	m_hpBelowUntilRecovery.EnableWindow(lVal);
}


// START

void CConfigDialog::OnAutogoTriggerhpabove() 
{
	int lVal = (m_triggerHpAbove.IsWindowEnabled()?m_triggerHpAbove.GetCheck():0);
	m_hpAbove.EnableWindow(lVal);
	m_actionHpAbove.EnableWindow(lVal);
	m_soundHpAbove.EnableWindow(lVal);
	
}

void CConfigDialog::OnAutogoTriggermanabelow() 
{
	int lVal = (m_triggerManaBelow.IsWindowEnabled()?m_triggerManaBelow.GetCheck():0);
	m_manaBelow.EnableWindow(lVal);
	m_actionManaBelow.EnableWindow(lVal);
	m_soundManaBelow.EnableWindow(lVal);
	m_manaBelowUntilRecovery.EnableWindow(lVal);
	
}

void CConfigDialog::OnAutogoTriggermanaabove() 
{
	int lVal = (m_triggerManaAbove.IsWindowEnabled()?m_triggerManaAbove.GetCheck():0);
	m_manaAbove.EnableWindow(lVal);
	m_actionManaAbove.EnableWindow(lVal);
	m_soundManaAbove.EnableWindow(lVal);
	
}

void CConfigDialog::OnAutogoTriggersoulpointsAbove() 
{
	int lVal = (m_triggerSoulPointAbove.IsWindowEnabled()?m_triggerSoulPointAbove.GetCheck():0);
	m_soulPointAbove.EnableWindow(lVal);
	m_actionSoulpointAbove.EnableWindow(lVal);
	m_soundSoulPointAbove.EnableWindow(lVal);
	
}


void CConfigDialog::OnAutogoOutoffood() 
{
	int lVal = (m_triggerOutOf.IsWindowEnabled()?m_triggerOutOf.GetCheck():0);
	int lVal2 = (m_outOfFood.IsWindowEnabled()?m_outOfFood.GetCheck():0);	
	m_actionOutOfFood.EnableWindow(lVal&&lVal2);
	m_soundOutOfFood.EnableWindow(lVal&&lVal2);
	
}

void CConfigDialog::OnAutogoOutofspace() 
{
	int lVal = (m_triggerOutOf.IsWindowEnabled()?m_triggerOutOf.GetCheck():0);
	int lVal2 = (m_outOfSpace.IsWindowEnabled()?m_outOfSpace.GetCheck():0);	
	m_actionOutOfSpace.EnableWindow(lVal&&lVal2);
	m_soundOutOfSpace.EnableWindow(lVal&&lVal2);
	
}

void CConfigDialog::OnAutogoOutofcustom() 
{
	int lVal = (m_triggerOutOf.IsWindowEnabled()?m_triggerOutOf.GetCheck():0);
	int lVal2 = (m_outOfCustom.IsWindowEnabled()?m_outOfCustom.GetCheck():0);	
	m_actionOutOfCustom.EnableWindow(lVal&&lVal2);
	m_soundOutOfCustom.EnableWindow(lVal&&lVal2);
	m_outOfCustomItem.EnableWindow(lVal&&lVal2);
	
}

void CConfigDialog::OnAutogoRunawayReached() 
{
	int lVal = (m_runawayReached.IsWindowEnabled()?m_runawayReached.GetCheck():0);
	m_runawayReachedRadius.EnableWindow(lVal);
	m_actionRunawayReached.EnableWindow(lVal);
	m_soundRunawayReached.EnableWindow(lVal);
	
}

void CConfigDialog::OnAutogoBattleplayer() 
{
	int lVal = (m_triggerBattleList.IsWindowEnabled()?m_triggerBattleList.GetCheck():0);
	int lVal2 = (m_battlePlayer.IsWindowEnabled()?m_battlePlayer.GetCheck():0);	
	m_actionBattleListPlayer.EnableWindow(lVal&&lVal2);
	m_soundBattleListPlayer.EnableWindow(lVal&&lVal2);			
}

void CConfigDialog::OnAutogoBattlemonster() 
{
	int lVal = (m_triggerBattleList.IsWindowEnabled()?m_triggerBattleList.GetCheck():0);
	int lVal2 = (m_battleMonster.IsWindowEnabled()?m_battleMonster.GetCheck():0);	
	m_actionBattleListMonster.EnableWindow(lVal&&lVal2);
	m_soundBattleListMonster.EnableWindow(lVal&&lVal2);			
	
}

void CConfigDialog::OnAutogoBattlegm() 
{
	int lVal = (m_triggerBattleList.IsWindowEnabled()?m_triggerBattleList.GetCheck():0);
	int lVal2 = (m_battleGM.IsWindowEnabled()?m_battleGM.GetCheck():0);	
	m_actionBattleListGm.EnableWindow(lVal&&lVal2);
	m_soundBattleListGm.EnableWindow(lVal&&lVal2);			
	
}

void CConfigDialog::OnAutogoBattelist() 
{
	int lVal = (m_triggerBattleList.IsWindowEnabled()?m_triggerBattleList.GetCheck():0);
	int lVal2 = (m_battleBattlelist.IsWindowEnabled()?m_battleBattlelist.GetCheck():0);	
	m_actionBattleListList.EnableWindow(lVal&&lVal2);
	m_soundBattleListList.EnableWindow(lVal&&lVal2);			
	
}

int CConfigDialog::actionPos2ID(int pos) {
	switch (pos){
		case ACTION_NONE_POS:		return ACTION_NONE;
		case ACTION_SUSPEND_POS:	return ACTION_SUSPEND;
		case ACTION_LOGOUT_POS:		return ACTION_LOGOUT;
		case ACTION_KILL_POS:		return ACTION_KILL;
		case ACTION_SHUTDOWN_POS:	return ACTION_SHUTDOWN;
		case ACTION_RUNAWAY_POS:	return ACTION_RUNAWAY;
		case ACTION_RUNAWAY_BACK_POS:	return ACTION_RUNAWAY_BACK;
		case ACTION_RUNAWAY_CAVEBOOT_HALFSLEEP_POS: return ACTION_RUNAWAY_CAVEBOOT_HALFSLEEP;
		case ACTION_RUNAWAY_CAVEBOOT_FULLSLEEP_POS: return ACTION_RUNAWAY_CAVEBOOT_FULLSLEEP;
		default:					return 0;
	}

};
