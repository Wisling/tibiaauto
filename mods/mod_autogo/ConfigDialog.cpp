// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_autogo.h"
#include "ConfigDialog.h"
#include "WhiteList.h"
#include "MemReaderProxy.h"
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
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
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
	DDX_Control(pDX, IDC_AUTOGO_TRIGGEROUTOF, m_triggerOutOf);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERMESSAGE, m_triggerMessage);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERMOVE, m_triggerMove);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERHPLOSS, m_triggerHpLoss);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERCAPACITY, m_triggerCapacity);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERBLANK, m_triggerBlank);
	DDX_Control(pDX, IDC_AUTOGO_TRIGGERBATTLELIST, m_triggerBattleList);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDSOULPOINT, m_soundSoulPoint);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDOUTOF, m_soundOutOf);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDMOVE, m_soundMove);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDMESSAGE, m_soundMessage);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDHPLOSS, m_soundHpLoss);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDCAPACITY, m_soundCapacity);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDBLANK, m_soundBlank);
	DDX_Control(pDX, IDC_AUTOGO_SOUNDBATTLELIST, m_soundBattleList);
	DDX_Control(pDX, IDC_AUTOGO_SIGNPOISON, m_signPoison);
	DDX_Control(pDX, IDC_AUTOGO_SIGNFIRE, m_signFire);
	DDX_Control(pDX, IDC_AUTOGO_SIGNENERGY, m_signEnergy);
	DDX_Control(pDX, IDC_AUTOGO_SIGNBATTLE, m_signBattle);
	DDX_Control(pDX, IDC_AUTOGO_OUTOFWORMS, m_outOfWorms);
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
	DDX_Control(pDX, IDC_AUTOGO_ACTIONOUTOF, m_actionOutOf);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONMOVE, m_actionMove);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONMESSAGE, m_actionMessage);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONHPLOSS, m_actionHpLoss);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONCAPACITY, m_actionCapacity);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONBLANK, m_actionBlank);
	DDX_Control(pDX, IDC_AUTOGO_ACTIONBATTLELIST, m_actionBattleList);
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
	ON_WM_CANCELMODE()
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
	m_soundBattleList.EnableWindow(false);
	m_actionBattleList.EnableWindow(false);
	
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
	m_outOfWorms.EnableWindow(false);
	m_outOfSpace.EnableWindow(false);
	m_actionOutOf.EnableWindow(false);
	m_soundOutOf.EnableWindow(false);
	
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
}

void CConfigDialog::enableControls()
{
	m_triggerBattleList.EnableWindow(true);
	OnAutogoTriggerbattlelist();
	
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
	char buf[128];

	sprintf(buf,"%d",configData->actX);		m_actX.SetWindowText(buf);
	sprintf(buf,"%d",configData->actY);		m_actY.SetWindowText(buf);
	sprintf(buf,"%d",configData->actZ);		m_actZ.SetWindowText(buf);
	m_actDirection.SetCurSel(configData->actDirection);
	sprintf(buf,"%d",configData->runawayX);	m_runawayX.SetWindowText(buf);
	sprintf(buf,"%d",configData->runawayY);	m_runawayY.SetWindowText(buf);
	sprintf(buf,"%d",configData->runawayZ);	m_runawayZ.SetWindowText(buf);	

	m_triggerBattleList.SetCheck((configData->trigger&TRIGGER_BATTLELIST?1:0));
	m_battlePlayer.SetCheck((configData->optionsBattleList&BATTLELIST_PLAYER?1:0));
	m_battleMonster.SetCheck((configData->optionsBattleList&BATTLELIST_MONSTER?1:0));
	m_battleGM.SetCheck((configData->optionsBattleList&BATTLELIST_GM?1:0));
	m_battleBattlelist.SetCheck((configData->optionsBattleList&BATTLELIST_BATTLELIST?1:0));
	m_battleParanoiaM.SetCheck((configData->optionsBattleList&BATTLELIST_PARANOIAM?1:0));
	m_actionBattleList.SetCurSel(configData->actionBattleList);
	m_soundBattleList.SetCheck((configData->sound&TRIGGER_BATTLELIST?1:0));
	
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
	
	m_triggerSoulPoint.SetCheck((configData->trigger&TRIGGER_SOULPOINT?1:0));
	sprintf(buf,"%d",configData->optionsSoulPoint);m_soulPoint.SetWindowText(buf);
	m_actionSoulPoint.SetCurSel(configData->actionSoulPoint);
	m_soundSoulPoint.SetCheck((configData->sound&TRIGGER_SOULPOINT?1:0));
	
	m_triggerBlank.SetCheck((configData->trigger&TRIGGER_BLANK?1:0));
	sprintf(buf,"%d",configData->optionsBlank);m_blank.SetWindowText(buf);
	m_actionBlank.SetCurSel(configData->actionBlank);
	m_soundBlank.SetCheck((configData->sound&TRIGGER_BLANK?1:0));
	
	m_triggerCapacity.SetCheck((configData->trigger&TRIGGER_CAPACITY?1:0));
	sprintf(buf,"%d",configData->optionsCapacity);m_capacity.SetWindowText(buf);
	m_actionCapacity.SetCurSel(configData->actionCapacity);
	m_soundCapacity.SetCheck((configData->sound&TRIGGER_CAPACITY?1:0));
	
	m_triggerOutOf.SetCheck((configData->trigger&TRIGGER_OUTOF?1:0));
	m_outOfFood.SetCheck((configData->optionsOutOf&OUTOF_FOOD?1:0));
	m_outOfWorms.SetCheck((configData->optionsOutOf&OUTOF_WORMS?1:0));
	m_outOfSpace.SetCheck((configData->optionsOutOf&OUTOF_SPACE?1:0));
	m_actionOutOf.SetCurSel(configData->actionOutOf);
	m_soundOutOf.SetCheck((configData->sound&TRIGGER_OUTOF?1:0));

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
	
}

CConfigData * CConfigDialog::controlsToConfig()
{
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
		(m_triggerSign.GetCheck()?TRIGGER_SIGN:0)|
		(m_triggerMessage.GetCheck()?TRIGGER_MESSAGE:0)|
		(m_triggerMove.GetCheck()?TRIGGER_MOVE:0)|
		(m_triggerHpLoss.GetCheck()?TRIGGER_HPLOSS:0)|
		(m_triggerHpBelow.GetCheck()?TRIGGER_HPBELOW:0)|
		(m_triggerSoulPoint.GetCheck()?TRIGGER_SOULPOINT:0)|
		(m_triggerBlank.GetCheck()?TRIGGER_BLANK:0)|
		(m_triggerCapacity.GetCheck()?TRIGGER_CAPACITY:0)|
		(m_triggerOutOf.GetCheck()?TRIGGER_OUTOF:0)
		);

	newConfigData->optionsBattleList = (
		(m_battlePlayer.GetCheck()?BATTLELIST_PLAYER:0)|
		(m_battleMonster.GetCheck()?BATTLELIST_MONSTER:0)|
		(m_battleGM.GetCheck()?BATTLELIST_GM:0)|
		(m_battleBattlelist.GetCheck()?BATTLELIST_BATTLELIST:0)|
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
		(m_messagePublic.GetCheck()?MESSAGE_PUBLIC:0)
		);
	
	newConfigData->optionsOutOf = (
		(m_outOfFood.GetCheck()?OUTOF_FOOD:0)|
		(m_outOfWorms.GetCheck()?OUTOF_WORMS:0)|
		(m_outOfSpace.GetCheck()?OUTOF_SPACE:0)
		);

	newConfigData->sound = (
		(m_soundBattleList.GetCheck()?TRIGGER_BATTLELIST:0)|
		(m_soundSign.GetCheck()?TRIGGER_SIGN:0)|
		(m_soundMessage.GetCheck()?TRIGGER_MESSAGE:0)|
		(m_soundHpLoss.GetCheck()?TRIGGER_HPLOSS:0)|
		(m_soundHpBelow.GetCheck()?TRIGGER_HPBELOW:0)|
		(m_soundMove.GetCheck()?TRIGGER_MOVE:0)|
		(m_soundSoulPoint.GetCheck()?TRIGGER_SOULPOINT:0)|
		(m_soundBlank.GetCheck()?TRIGGER_BLANK:0)|
		(m_soundCapacity.GetCheck()?TRIGGER_CAPACITY:0)|
		(m_soundOutOf.GetCheck()?TRIGGER_OUTOF:0)
		);

	newConfigData->actionBattleList = m_actionBattleList.GetCurSel();
	newConfigData->actionSign		= m_actionSign.GetCurSel();
	newConfigData->actionMessage	= m_actionMessage.GetCurSel();
	newConfigData->actionHpLoss		= m_actionHpLoss.GetCurSel();
	newConfigData->actionHpBelow	= m_actionHpBelow.GetCurSel();
	newConfigData->actionMove		= m_actionMove.GetCurSel();
	newConfigData->actionSoulPoint	= m_actionSoulPoint.GetCurSel();
	newConfigData->actionBlank		= m_actionBlank.GetCurSel();
	newConfigData->actionCapacity	= m_actionCapacity.GetCurSel();
	newConfigData->actionOutOf		= m_actionOutOf.GetCurSel();

	if (newConfigData->trigger&TRIGGER_BATTLELIST)	newConfigData->action|=actionPos2ID(newConfigData->actionBattleList);
	if (newConfigData->trigger&TRIGGER_SIGN)		newConfigData->action|=actionPos2ID(newConfigData->actionSign);
	if (newConfigData->trigger&TRIGGER_MESSAGE)		newConfigData->action|=actionPos2ID(newConfigData->actionMessage);
	if (newConfigData->trigger&TRIGGER_MOVE)		newConfigData->action|=actionPos2ID(newConfigData->actionMove);
	if (newConfigData->trigger&TRIGGER_HPLOSS)		newConfigData->action|=actionPos2ID(newConfigData->actionHpLoss);
	if (newConfigData->trigger&TRIGGER_HPBELOW)		newConfigData->action|=actionPos2ID(newConfigData->actionHpBelow);
	if (newConfigData->trigger&TRIGGER_SOULPOINT)	newConfigData->action|=actionPos2ID(newConfigData->actionSoulPoint);
	if (newConfigData->trigger&TRIGGER_BLANK)		newConfigData->action|=actionPos2ID(newConfigData->actionBlank);
	if (newConfigData->trigger&TRIGGER_CAPACITY)	newConfigData->action|=actionPos2ID(newConfigData->actionCapacity);
	if (newConfigData->trigger&TRIGGER_OUTOF)		newConfigData->action|=actionPos2ID(newConfigData->actionOutOf);

	m_hpBelow.GetWindowText(buf,127);	newConfigData->optionsHpBelow=atoi(buf);
	m_soulPoint.GetWindowText(buf,127);	newConfigData->optionsSoulPoint=atoi(buf);
	m_blank.GetWindowText(buf,127);		newConfigData->optionsBlank=atoi(buf);
	m_capacity.GetWindowText(buf,127);	newConfigData->optionsCapacity=atoi(buf);

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

	CComboBox *list[10] = {&m_actionBattleList,&m_actionSign,&m_actionMessage,&m_actionMove,&m_actionHpLoss,&m_actionHpBelow,
			&m_actionSoulPoint,&m_actionBlank,&m_actionCapacity,&m_actionOutOf};

	CRect lpRect;

	for (int i=0;i<10;i++){
		list[i]->InsertString(ACTION_NONE_POS,"(none)");
		//list[i]->SetItemData(ACTION_NONE_POS,ACTION_NONE);
		list[i]->InsertString(ACTION_SUSPEND_POS,"Suspend macro");
		//list[i]->SetItemData(ACTION_SUSPEND_POS,ACTION_SUSPEND);
		list[i]->InsertString(ACTION_LOGOUT_POS,"Logout");
		//list[i]->SetItemData(ACTION_LOGOUT_POS,ACTION_LOGOUT);
		list[i]->InsertString(ACTION_KILL_POS,"Logout+Kill");
		//list[i]->SetItemData(ACTION_KILL_POS,ACTION_KILL);
		list[i]->InsertString(ACTION_SHUTDOWN_POS,"Logout+Shutdown");
		//list[i]->SetItemData(ACTION_SHUTDOWN_POS,ACTION_SHUTDOWN);
		list[i]->InsertString(ACTION_RUNAWAY_POS,"Go to runaway");
		//list[i]->SetItemData(ACTION_RUNAWAY_POS,ACTION_RUNAWAY);
		
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
	m_battlePlayer.EnableWindow(lVal);
	m_battleMonster.EnableWindow(lVal);
	m_battleGM.EnableWindow(lVal);
	m_battleBattlelist.EnableWindow(lVal);
	m_battleParanoiaM.EnableWindow(lVal);
	m_battleWhiteList.EnableWindow(lVal);
	m_soundBattleList.EnableWindow(lVal);
	m_actionBattleList.EnableWindow(lVal);
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
	m_outOfWorms.EnableWindow(lVal);
	m_outOfSpace.EnableWindow(lVal);
	m_actionOutOf.EnableWindow(lVal);
	m_soundOutOf.EnableWindow(lVal);
}

void CConfigDialog::OnAutogoTriggerhpbelow() 
{
	int lVal = (m_triggerHpBelow.IsWindowEnabled()?m_triggerHpBelow.GetCheck():0);
	m_hpBelow.EnableWindow(lVal);
	m_actionHpBelow.EnableWindow(lVal);
	m_soundHpBelow.EnableWindow(lVal);
}
