// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_cavebot.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "TibiaMiniMap.h"
#include "TibiaMiniMapPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CToolAutoAttackStateAttack globalAutoAttackStateAttack;
extern CToolAutoAttackStateLoot globalAutoAttackStateLoot;
extern CToolAutoAttackStateWalker globalAutoAttackStateWalker;
extern CToolAutoAttackStateDepot globalAutoAttackStateDepot;
extern CToolAutoAttackStateTraining globalAutoAttackStateTraining;
extern int targetX,targetY,targetZ;
extern int depotX,depotY,depotZ;
extern int firstCreatureAttackTM;
extern int currentPosTM;
extern int creatureAttackDist;
extern int pauseAfterUnreachableTm;



/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_cavebotApp *app,CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_DEPOT_CAP, m_depotCap);
	DDX_Control(pDX, IDC_SHARE_ALIEN_BACKATTACK, m_shareAlienBackattack);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_IGNORE, m_ignore);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_IGNORELIST, m_ignoreList);
	DDX_Control(pDX, IDC_BACKATTACK_RUNES, m_backattackRunes);
	DDX_Control(pDX, IDC_ATTACK_HP_ABOVE, m_attackHpAbove);
	DDX_Control(pDX, IDC_LOOTFROMFLOOR, m_lootFromFloor);
	DDX_Control(pDX, IDC_DROPNOTLOOTED, m_dropNotLooted);
	DDX_Control(pDX, IDC_DEPOT_DROP_INSTEAD_OF_DEPOSIT, m_depotDropInsteadOfDepositon);
	DDX_Control(pDX, IDC_TRAINING_WEAPON_TRAIN, m_weaponTrain);
	DDX_Control(pDX, IDC_TRAINING_WEAPON_FIGHT, m_weaponFight);
	DDX_Control(pDX, IDC_TRAINING_STATE, m_trainingState);
	DDX_Control(pDX, IDC_TRAINING_FIGHT_WHEN_SURROUNDED, m_fightWhenSurrounded);
	DDX_Control(pDX, IDC_TRAINING_FIGHT_WHEN_ALIEN, m_fightWhenAlien);
	DDX_Control(pDX, IDC_TRAINING_BLOOD_HIT, m_bloodHit);
	DDX_Control(pDX, IDC_TRAINING_ACTIVATE, m_activate);
	DDX_Control(pDX, IDC_DEBUG, m_debug);
	DDX_Control(pDX, IDC_DEPOT_ENTRYREMOVE, m_depotEntryRemove);
	DDX_Control(pDX, IDC_DEPOT_ENTRYADD, m_depotEntryAdd);
	DDX_Control(pDX, IDC_DEPOT_WHEN, m_depotWhen);
	DDX_Control(pDX, IDC_DEPOT_REMAIN, m_depotRemain);
	DDX_Control(pDX, IDC_DEPOT_ITEMLIST, m_depotItemList);
	DDX_Control(pDX, IDC_DEPOT_ENTRYLIST, m_depotEntryList);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_STATE_DEPOT, m_stateDepot);
	DDX_Control(pDX, IDC_GATHER_LOOT_STATS, m_gatherLootStats);
	DDX_Control(pDX, IDC_UNREACHABLE_AFTER, m_unreachableAfter);
	DDX_Control(pDX, IDC_SUSPEND_AFTER_UNREACHABLE, m_suspendAfterUnreachable);
	DDX_Control(pDX, IDC_ATTACK_ONLY_ATTACKING, m_attackOnlyAttacking);
	DDX_Control(pDX, IDC_STAND_STILL, m_standStill);
	DDX_Control(pDX, IDC_FORCE_ATTACK_AFTER_ATTACK, m_forceAttackAfterAttack);
	DDX_Control(pDX, IDC_STICK_TO_MONSTER, m_stickToMonster);
	DDX_Control(pDX, IDC_EAT_FROM_CORPSE, m_eatFromCorpse);
	DDX_Control(pDX, IDC_ATTACK_RANGE, m_attackRange);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_AUTOLOOTCUSTOM, m_lootCustom);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_WAYPOINTSELMODE, m_waypointSelMode);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_STATE_WALKER, m_stateWalker);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_STATE_LOOT, m_stateLoot);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_STATE_ATTACK, m_stateAttack);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_NOMOVESUSPEND, m_nomoveSuspended);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_MAPUSED, m_mapUsed);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_LOOTINBAGS, m_lootinBags);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_LOOTCAPLIMIT, m_lootCapLimit);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_SUSPENDONENEMY, m_suspendOnEnemy);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_AUTOLOOTWORMS, m_lootWorms);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_MODE, m_mode);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_AUTOLOOTGP, m_lootGp);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_AUTOLOOTFOOD, m_lootFood);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_CURZ, m_curZ);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_CURY, m_curY);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_CURX, m_curX);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_WAYPOINTLIST, m_waypointList);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_AUTOFOLLOW, m_autoFollow);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_MONSTERLIST, m_monsterList);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_MONSTER, m_monster);
	DDX_Control(pDX, IDC_ENSURE_LOOTING, m_ensureLoot);
	DDX_Control(pDX, IDC_CAVEBOT_PAUSE_ALL, m_pauseAll);
	DDX_Control(pDX, IDC_CAVEBOT_PAUSE_WALKER, m_pauseWalker);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_REMOVE_MONSTER, OnToolAutoattackRemoveMonster)
	ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_ADD_MONSTER, OnToolAutoattackAddMonster)
	ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_REMOVE_WAYPOINT, OnToolAutoattackRemoveWaypoint)
	ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_ADD_WAYPOINT, OnToolAutoattackAddWaypoint)
	ON_BN_CLICKED(IDC_DEPOT_ENTRYADD, OnDepotEntryadd)
	ON_BN_CLICKED(IDC_DEPOT_ENTRYREMOVE, OnDepotEntryremove)
	ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_ADD_IGNORE, OnToolAutoattackAddIgnore)
	ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_REMOVE_IGNORE, OnToolAutoattackRemoveIgnore)
	ON_BN_CLICKED(IDC_LOAD_FROM_MINIMAP, OnLoadFromMinimap)
	ON_BN_CLICKED(IDC_MONSTER_ATTACK_UP, OnMonsterAttackUp)
	ON_BN_CLICKED(IDC_MONSTER_ATTACK_DOWN, OnMonsterAttackDown)
	ON_BN_CLICKED(IDC_ENSURE_LOOTING, OnEnsureLoot)
	ON_BN_CLICKED(IDC_CAVEBOT_PAUSE_ALL, OnPauseAll)
	ON_BN_CLICKED(IDC_CAVEBOT_PAUSE_WALKER, OnPauseWalker)
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
	m_lootCustom.EnableWindow(false);
	m_waypointSelMode.EnableWindow(false);	
	m_nomoveSuspended.EnableWindow(false);
	m_mapUsed.EnableWindow(false);
	m_lootinBags.EnableWindow(false);	
	m_lootCapLimit.EnableWindow(false);
	m_suspendOnEnemy.EnableWindow(false);
	m_lootWorms.EnableWindow(false);
	m_mode.EnableWindow(false);
	m_lootGp.EnableWindow(false);
	m_lootFood.EnableWindow(false);
	m_waypointList.EnableWindow(false);
	m_autoFollow.EnableWindow(false);
	m_monsterList.EnableWindow(false);
	m_monster.EnableWindow(false);
	m_stickToMonster.EnableWindow(false);
	m_eatFromCorpse.EnableWindow(false);
	m_attackRange.EnableWindow(false);
	m_unreachableAfter.EnableWindow(false);
	m_suspendAfterUnreachable.EnableWindow(false);
	m_attackOnlyAttacking.EnableWindow(false);
	m_standStill.EnableWindow(false);
	m_forceAttackAfterAttack.EnableWindow(false);
	m_gatherLootStats.EnableWindow(false);
	m_depotEntryAdd.EnableWindow(false);
	m_depotEntryList.EnableWindow(false);
	m_depotEntryRemove.EnableWindow(false);
	m_depotItemList.EnableWindow(false);
	m_depotRemain.EnableWindow(false);
	m_depotWhen.EnableWindow(false);
	m_debug.EnableWindow(false);
	m_weaponTrain.EnableWindow(false);
	m_weaponFight.EnableWindow(false);	
	m_fightWhenSurrounded.EnableWindow(false);
	m_fightWhenAlien.EnableWindow(false);
	m_bloodHit.EnableWindow(false);
	m_activate.EnableWindow(false);
	m_depotDropInsteadOfDepositon.EnableWindow(false);
	m_dropNotLooted.EnableWindow(false);
	m_lootFromFloor.EnableWindow(false);
	m_attackHpAbove.EnableWindow(false);
	m_ignoreList.EnableWindow(false);
	m_ignore.EnableWindow(false);
	m_backattackRunes.EnableWindow(false);
	m_shareAlienBackattack.EnableWindow(false);
	m_depotCap.EnableWindow(false);
	m_ensureLoot.EnableWindow(false);

}

void CConfigDialog::enableControls()
{
	m_lootCustom.EnableWindow(true);
	m_waypointSelMode.EnableWindow(true);	
	m_nomoveSuspended.EnableWindow(true);
	m_mapUsed.EnableWindow(true);
	m_lootinBags.EnableWindow(true);	
	m_lootCapLimit.EnableWindow(true);
	m_suspendOnEnemy.EnableWindow(true);
	m_lootWorms.EnableWindow(true);
	m_mode.EnableWindow(true);
	m_lootGp.EnableWindow(true);
	m_lootFood.EnableWindow(true);
	m_waypointList.EnableWindow(true);
	m_autoFollow.EnableWindow(true);
	m_monsterList.EnableWindow(true);
	m_monster.EnableWindow(true);
	m_stickToMonster.EnableWindow(true);
	m_eatFromCorpse.EnableWindow(true);
	m_attackRange.EnableWindow(true);
	m_unreachableAfter.EnableWindow(true);
	m_suspendAfterUnreachable.EnableWindow(true);
	m_attackOnlyAttacking.EnableWindow(true);
	m_standStill.EnableWindow(true);
	m_forceAttackAfterAttack.EnableWindow(true);	
	m_depotEntryAdd.EnableWindow(true);
	m_depotEntryList.EnableWindow(true);
	m_depotEntryRemove.EnableWindow(true);
	m_depotItemList.EnableWindow(true);
	m_depotRemain.EnableWindow(true);
	m_depotWhen.EnableWindow(true);
	m_debug.EnableWindow(true);
	m_weaponTrain.EnableWindow(true);
	m_weaponFight.EnableWindow(true);	
	m_fightWhenSurrounded.EnableWindow(true);
	m_fightWhenAlien.EnableWindow(true);
	m_bloodHit.EnableWindow(true);
	m_activate.EnableWindow(true);
	m_depotDropInsteadOfDepositon.EnableWindow(true);
	m_dropNotLooted.EnableWindow(true);
	m_lootFromFloor.EnableWindow(true);
	m_attackHpAbove.EnableWindow(true);
	m_ignoreList.EnableWindow(true);
	m_ignore.EnableWindow(true);
	m_backattackRunes.EnableWindow(true);
	m_shareAlienBackattack.EnableWindow(true);
	m_depotCap.EnableWindow(true);
	m_ensureLoot.EnableWindow(true);
}



void CConfigDialog::configToControls(CConfigData *configData)
{
	CTibiaItemProxy itemProxy;
	int i;
	char buf[128];
	

	sprintf(buf,"%d",configData->unreachableAfter);m_unreachableAfter.SetWindowText(buf);
	sprintf(buf,"%d",configData->suspendAfterUnreachable);m_suspendAfterUnreachable.SetWindowText(buf);
	sprintf(buf,"%d",configData->standStill);m_standStill.SetWindowText(buf);
	sprintf(buf,"%d",configData->attackHpAbove);m_attackHpAbove.SetWindowText(buf);
	m_attackOnlyAttacking.SetCheck(configData->attackOnlyAttacking);
	m_forceAttackAfterAttack.SetCheck(configData->forceAttackAfterAttack);
	m_stickToMonster.SetCheck(configData->stickToMonster);
	m_eatFromCorpse.SetCheck(configData->eatFromCorpse);
	sprintf(buf,"%d",configData->attackRange);m_attackRange.SetWindowText(buf);
	m_autoFollow.SetCheck(configData->autoFollow);
	m_lootFood.SetCheck(configData->lootFood);
	m_lootGp.SetCheck(configData->lootGp);
	m_lootWorms.SetCheck(configData->lootWorms);
	m_mode.SetCurSel(configData->mode);
	m_suspendOnEnemy.SetCheck(configData->suspendOnEnemy);
	m_nomoveSuspended.SetCheck(configData->suspendOnNoMove);
	m_waypointSelMode.SetCurSel(configData->waypointSelectMode);
	m_mapUsed.SetCurSel(configData->mapUsed);	
	m_lootCustom.SetCheck(configData->lootCustom);
	m_lootinBags.SetCheck(configData->lootInBags);
	m_gatherLootStats.SetCheck(configData->gatherLootStats);
	m_debug.SetCheck(configData->debug);
	sprintf(buf,"%d",configData->capacityLimit);m_lootCapLimit.SetWindowText(buf);
	sprintf(buf,"%d",configData->depotCap);m_depotCap.SetWindowText(buf);
	while (m_waypointList.GetCount()) m_waypointList.DeleteString(0);
	for (i=0;i<100;i++)
	{
		if (configData->waypointList[i].x&&configData->waypointList[i].y)
		{
			sprintf(buf,"(%d,%d,%d)",configData->waypointList[i].x,configData->waypointList[i].y,configData->waypointList[i].z);
			m_waypointList.AddString(buf);
		};	
	}
	while (m_monsterList.GetCount())	
		m_monsterList.DeleteString(0);		
	for (i=0;i<configData->monsterCount;i++)
		m_monsterList.AddString(configData->monsterList[i]);

	while (m_ignoreList.GetCount())	
		m_ignoreList.DeleteString(0);		
	for (i=0;i<configData->ignoreCount;i++)
		m_ignoreList.AddString(configData->ignoreList[i]);

	while (m_depotEntryList.GetCount()) m_depotEntryList.DeleteString(0);
	reloadDepotItems();
	reloadTrainingItems();
	for (i=0;i<100;i++)
	{
		if (strlen(configData->depotTrigger[i].itemName))
		{
			sprintf(buf,"%s %d->%d",configData->depotTrigger[i].itemName,configData->depotTrigger[i].when,configData->depotTrigger[i].remain);
			m_depotEntryList.AddString(buf);
			m_depotItemList.DeleteString(m_depotItemList.FindString(0,configData->depotTrigger[i].itemName));		
		}
	}
	m_fightWhenSurrounded.SetCheck(configData->fightWhenSurrounded);
	m_fightWhenAlien.SetCheck(configData->fightWhenAlien);
	m_bloodHit.SetCheck(configData->bloodHit);
	m_activate.SetCheck(configData->trainingActivate);
	m_weaponTrain.SetCurSel(m_weaponTrain.FindString(-1,itemProxy.getName(configData->weaponTrain)));
	if (m_weaponTrain.GetCurSel()==-1) m_weaponTrain.SetCurSel(0);
	m_weaponFight.SetCurSel(m_weaponFight.FindString(-1,itemProxy.getName(configData->weaponFight)));	
	if (m_weaponFight.GetCurSel()==-1) m_weaponFight.SetCurSel(0);
	m_depotDropInsteadOfDepositon.SetCheck(configData->depotDropInsteadOfDepositon);
	m_dropNotLooted.SetCheck(configData->dropNotLooted);
	m_lootFromFloor.SetCheck(configData->lootFromFloor);
	m_backattackRunes.SetCheck(configData->backattackRunes);
	m_shareAlienBackattack.SetCheck(configData->shareAlienBackattack);
	m_ensureLoot.SetCheck(configData->ensureLoot);
		

}

CConfigData * CConfigDialog::controlsToConfig()
{
	
	char buf[128];
	CConfigData *newConfigData = new CConfigData();	

	m_standStill.GetWindowText(buf,127);
	newConfigData->standStill=atoi(buf);
	m_suspendAfterUnreachable.GetWindowText(buf,127);
	newConfigData->suspendAfterUnreachable=atoi(buf);
	m_unreachableAfter.GetWindowText(buf,127);
	newConfigData->unreachableAfter=atoi(buf);
	m_attackHpAbove.GetWindowText(buf,127);
	newConfigData->attackHpAbove=atoi(buf);
	newConfigData->attackOnlyAttacking=m_attackOnlyAttacking.GetCheck();
	newConfigData->forceAttackAfterAttack=m_forceAttackAfterAttack.GetCheck();
	newConfigData->stickToMonster=m_stickToMonster.GetCheck();	
	newConfigData->eatFromCorpse=m_eatFromCorpse.GetCheck();
	m_attackRange.GetWindowText(buf,127);
	newConfigData->attackRange=atoi(buf);
	newConfigData->autoFollow=m_autoFollow.GetCheck();	
	newConfigData->lootFood=m_lootFood.GetCheck();
	newConfigData->lootGp=m_lootGp.GetCheck();
	newConfigData->lootWorms=m_lootWorms.GetCheck();
	newConfigData->mode=m_mode.GetCurSel();
	newConfigData->suspendOnEnemy=m_suspendOnEnemy.GetCheck();
	newConfigData->gatherLootStats=m_gatherLootStats.GetCheck();
	newConfigData->debug=m_debug.GetCheck();
	int i;
	newConfigData->monsterCount=m_monsterList.GetCount();
	for (i=0;i<100;i++)
		newConfigData->monsterList[i][0]=0;
	for (i=0;i<m_monsterList.GetCount();i++)
	{			
		int j;
		m_monsterList.GetText(i,newConfigData->monsterList[i]);	
		int len=strlen(newConfigData->monsterList[i]);
		for (j=0;j<len;j++)
			newConfigData->monsterList[i][j]=tolower(newConfigData->monsterList[i][j]);
	};
	newConfigData->ignoreCount=m_ignoreList.GetCount();
	for (i=0;i<100;i++)
		newConfigData->ignoreList[i][0]=0;
	for (i=0;i<m_ignoreList.GetCount();i++)
	{			
		int j;
		m_ignoreList.GetText(i,newConfigData->ignoreList[i]);	
		int len=strlen(newConfigData->ignoreList[i]);
		for (j=0;j<len;j++)
			newConfigData->ignoreList[i][j]=tolower(newConfigData->ignoreList[i][j]);
	};
	for (i=0;i<100;i++)
		newConfigData->waypointList[i].x=newConfigData->waypointList[i].y=0;
	for (i=0;i<m_waypointList.GetCount();i++)
	{			
		m_waypointList.GetText(i,buf);
		int curX=0,curY=0,curZ=0;
		sscanf(buf,"(%d,%d,%d)",&curX,&curY,&curZ);
		newConfigData->waypointList[i].x=curX;
		newConfigData->waypointList[i].y=curY;
		newConfigData->waypointList[i].z=curZ;
	};
	newConfigData->suspendOnNoMove=m_nomoveSuspended.GetCheck();
	newConfigData->waypointSelectMode=m_waypointSelMode.GetCurSel();
	newConfigData->mapUsed=m_mapUsed.GetCurSel();	
	m_lootCapLimit.GetWindowText(buf,127);
	newConfigData->capacityLimit=atoi(buf);	
	m_depotCap.GetWindowText(buf,127);
	newConfigData->depotCap=atoi(buf);	
	newConfigData->lootCustom=m_lootCustom.GetCheck();
	newConfigData->lootInBags=m_lootinBags.GetCheck();	

	for (i=0;i<100;i++)
		newConfigData->depotTrigger[i].itemName[0]='\0';
	for (i=0;i<m_depotEntryList.GetCount();i++)
	{
		char paramString[128];
		m_depotEntryList.GetText(i,newConfigData->depotTrigger[i].itemName);
		// from depotEntryList we got 'Apple Pie 128->64'
		// and we convert it to Apple Pie + 128->64
		int j;
		for (j=strlen(newConfigData->depotTrigger[i].itemName)-1;j>0;j--)
		{
			if (newConfigData->depotTrigger[i].itemName[j]==' ')
			{
				memcpy(paramString,newConfigData->depotTrigger[i].itemName+j+1,strlen(newConfigData->depotTrigger[i].itemName)-j);
				newConfigData->depotTrigger[i].itemName[j]='\0';
				break;
			}
		}
		sscanf(paramString,"%d->%d",&newConfigData->depotTrigger[i].when,&newConfigData->depotTrigger[i].remain);
	}

	newConfigData->fightWhenSurrounded=m_fightWhenSurrounded.GetCheck();
	newConfigData->fightWhenAlien=m_fightWhenAlien.GetCheck();
	newConfigData->bloodHit=m_bloodHit.GetCheck();
	newConfigData->trainingActivate=m_activate.GetCheck();
	newConfigData->weaponTrain=m_weaponTrain.GetItemData(m_weaponTrain.GetCurSel());
	newConfigData->weaponFight=m_weaponFight.GetItemData(m_weaponFight.GetCurSel());
	newConfigData->depotDropInsteadOfDepositon=m_depotDropInsteadOfDepositon.GetCheck();
	newConfigData->dropNotLooted=m_dropNotLooted.GetCheck();
	newConfigData->lootFromFloor=m_lootFromFloor.GetCheck();
	newConfigData->backattackRunes=m_backattackRunes.GetCheck();
	newConfigData->shareAlienBackattack=m_shareAlienBackattack.GetCheck();
	newConfigData->ensureLoot=m_ensureLoot.GetCheck();



	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==1001)
	{
		KillTimer(1001);
		CMemReaderProxy reader;

		CTibiaCharacter *self = reader.readSelfCharacter();

		char buf[256];
		sprintf(buf,"%d",self->x);
		m_curX.SetWindowText(buf);
		sprintf(buf,"%d",self->y);
		m_curY.SetWindowText(buf);
		sprintf(buf,"%d",self->z);
		m_curZ.SetWindowText(buf);
		

		switch (globalAutoAttackStateAttack) {

		case CToolAutoAttackStateAttack_notRunning:			
			m_stateAttack.SetWindowText("State: not running");
			break;
		case CToolAutoAttackStateAttack_macroPause:
			m_stateAttack.SetWindowText("State: macro pause");
			break;
		case CToolAutoAttackStateAttack_attackingAlienFound:
			m_stateAttack.SetWindowText("State: alien creature found");
			break;
		case CToolAutoAttackStateAttack_attackingCreature:
			{
				int t1=time(NULL)-firstCreatureAttackTM;
				int t2=time(NULL)-currentPosTM;
				sprintf(buf,"State: attacking [since %ds; distance %d; standing since %ds]",t1,creatureAttackDist,t2);
				m_stateAttack.SetWindowText(buf);
			}
			break;
		case CToolAutoAttackStateAttack_monsterUnreachable:
			sprintf(buf,"State: monster unreachable [yet %ds]",pauseAfterUnreachableTm);
			m_stateAttack.SetWindowText(buf);
			break;
		default:
			m_stateAttack.SetWindowText("State: unknown");
		}
		
		switch (globalAutoAttackStateLoot) {				
		case CToolAutoAttackStateLoot_notRunning:
			m_stateLoot.SetWindowText("State: not running");
			break;
		case CToolAutoAttackStateLoot_macroPause:
			m_stateLoot.SetWindowText("State: macro pause");
			break;
		case CToolAutoAttackStateLoot_opening:
			m_stateLoot.SetWindowText("State: opening container");
			break;
		case CToolAutoAttackStateLoot_moveing:
			m_stateLoot.SetWindowText("State: moving item");
			break;
		case CToolAutoAttackStateLoot_closing:
			m_stateLoot.SetWindowText("State: closing container");
			break;
		case CToolAutoAttackStateLoot_openingBag:
			m_stateLoot.SetWindowText("State: opening inner bag");
			break;
		case CToolAutoAttackStateLoot_moveingBag:
			m_stateLoot.SetWindowText("State: moving item from inner bag");
			break;
		case CToolAutoAttackStateLoot_closingBag:
			m_stateLoot.SetWindowText("State: closing inner bag");
			break;
		default:
			m_stateLoot.SetWindowText("State: unknown");
		}


		switch (globalAutoAttackStateWalker) {			
		
		case CToolAutoAttackStateWalker_notRunning:
			m_stateWalker.SetWindowText("State: not running");
			break;			
		case CToolAutoAttackStateWalker_macroPause:
			m_stateWalker.SetWindowText("State: not running");
			break;
		case CToolAutoAttackStateWalker_noWaypoints:
			m_stateWalker.SetWindowText("State: no waypoints");
			break;
		case CToolAutoAttackStateWalker_ok:
			{
				int tilesAway=abs(self->x-targetX)+abs(self->y-targetY)+abs(self->z-targetZ);
				sprintf(buf,"State: walking to (%d,%d,%d) %d tiles away",targetX,targetY,targetZ,tilesAway);
				m_stateWalker.SetWindowText(buf);
			}
			break;
		case CToolAutoAttackStateWalker_noPathFound:
			m_stateWalker.SetWindowText("State: no path found");
			break;
		case CToolAutoAttackStateWalker_standing:
			m_stateWalker.SetWindowText("State: standing");
			break;
		case CToolAutoAttackStateWalker_halfSleep:
			m_stateWalker.SetWindowText("State: half module sleep");
			break;
		case CToolAutoAttackStateWalker_fullSleep:
			m_stateWalker.SetWindowText("State: full module sleep");
			break;
		default:
			m_stateWalker.SetWindowText("State: unknown");
		}

		switch (globalAutoAttackStateDepot)
		{			
			case CToolAutoAttackStateDepot_notRunning:
				m_stateDepot.SetWindowText("State: not running");
				break;
			case CToolAutoAttackStateDepot_notFound:
				m_stateDepot.SetWindowText("State: depot not found");
				break;
			case CToolAutoAttackStateDepot_noSpace:
				m_stateDepot.SetWindowText("State: no space in depot chest");
				break;
			case CToolAutoAttackStateDepot_walking:
				{
					int tilesAway=abs(self->x-depotX)+abs(self->y-depotY)+abs(self->z-depotZ);					
					sprintf(buf,"State: walking to depot (%d,%d,%d) %d tiles away",depotX,depotY,depotZ,tilesAway);
					m_stateDepot.SetWindowText(buf);
				}
				break;
			case CToolAutoAttackStateDepot_opening:
				m_stateDepot.SetWindowText("State: opening depot");
				break;
			case CToolAutoAttackStateDepot_depositing:
				m_stateDepot.SetWindowText("State: depositing");
				break;
			default:
				m_stateDepot.SetWindowText("State: unknown");
		}

		switch (globalAutoAttackStateTraining)
		{
		case CToolAutoAttackStateTraining_notRunning:
			m_trainingState.SetWindowText("State: not running");
			break;
		case CToolAutoAttackStateTraining_training:
			m_trainingState.SetWindowText("State: training");
			break;
		case CToolAutoAttackStateTraining_trainingFullDef:
			m_trainingState.SetWindowText("State: training in full defence");
			break;
		case CToolAutoAttackStateTraining_fighting:
			m_trainingState.SetWindowText("State: fighting");
			break;
		case CToolAutoAttackStateTraining_switchingWeapon:
			m_trainingState.SetWindowText("State: switching weapon");
			break;
		default:
			m_trainingState.SetWindowText("State: unknown");
		}

		delete self;
		
		
		SetTimer(1001,100,NULL);
	};
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// initialise comboboxes, etc.
	reloadDepotItems();
	reloadTrainingItems();
	

	// set timers
	SetTimer(1001,100,NULL);	
	
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




void CConfigDialog::OnToolAutoattackRemoveWaypoint() 
{
	int sel=m_waypointList.GetCurSel();
	if (sel==-1)
		return;
	char currentMonster[128];
	m_waypointList.GetText(sel,currentMonster);
	m_waypointList.DeleteString(sel);	
}

void CConfigDialog::OnEnsureLoot() 
{
	if(m_ensureLoot.GetCheck())
		AfxMessageBox("WARNING!! May cause a slower execution in cavebot!");
	else
		AfxMessageBox("WARNING!! May cause a some creatures to be NOT looted!");
}

void CConfigDialog::OnToolAutoattackAddWaypoint() 
{
	// max 500 waypoints may be defined
	if (m_waypointList.GetCount()>=499)
		return;
	char buf[256];
	int curX,curY,curZ;
	m_curX.GetWindowText(buf,255);
	curX=atoi(buf);
	m_curY.GetWindowText(buf,255);
	curY=atoi(buf);
	m_curZ.GetWindowText(buf,255);
	curZ=atoi(buf);
	sprintf(buf,"(%d,%d,%d)",curX,curY,curZ);
	m_waypointList.AddString(buf);
}

void CConfigDialog::OnToolAutoattackRemoveMonster() 
{
	int sel=m_monsterList.GetCurSel();
	if (sel==-1)
		return;
	char currentMonster[128];
	m_monsterList.GetText(sel,currentMonster);
	m_monsterList.DeleteString(sel);
	m_monster.SetWindowText(currentMonster);
}

void CConfigDialog::OnToolAutoattackAddMonster() 
{
	char buf[128];
	m_monster.GetWindowText(buf,128);
	if (strlen(buf)&&m_monsterList.GetCount()<32)
	{
		m_monsterList.AddString(buf);
	}
	m_monster.SetWindowText("");
}

void CConfigDialog::OnDepotEntryadd() 
{
	char buf[128];
	int depotWhen=0;
	int depotRemain=0;
	int sel=m_depotItemList.GetCurSel();
	char itemName[128];
	
	m_depotWhen.GetWindowText(buf,127);
	depotWhen=atoi(buf);
	m_depotRemain.GetWindowText(buf,127);
	depotRemain=atoi(buf);	
	m_depotItemList.GetLBText(sel,itemName);

	if (sel==-1) return;
	if (depotWhen<0) return;
	if (depotRemain<0) return;
	if (depotWhen==depotRemain) return;	
		
	sprintf(buf,"%s %d->%d",itemName,depotWhen,depotRemain);
	m_depotEntryList.AddString(buf);
	m_depotItemList.DeleteString(m_depotItemList.FindString(0,itemName));
	m_depotItemList.SetCurSel(0);
	m_depotWhen.SetWindowText("");
	m_depotRemain.SetWindowText("");
}

void CConfigDialog::OnDepotEntryremove() 
{
	char buf[128];
	char itemName[128];
	char paramString[128];
	int depotWhen;
	int depotRemain;
	int sel=m_depotEntryList.GetCurSel();
	if (sel==-1)
		return;	
	m_depotEntryList.GetText(sel,itemName);
	// from depotEntryList we got 'Apple Pie 128->64'
	// and we convert it to Apple Pie + 128->64
	int i;
	for (i=strlen(itemName)-1;i>0;i--)
	{
		if (itemName[i]==' ')
		{
			memcpy(paramString,itemName+i+1,strlen(itemName)-i);
			itemName[i]='\0';
			break;
		}
	}
	sscanf(paramString,"%d->%d",&depotWhen,&depotRemain);
	m_depotItemList.AddString(itemName);
	m_depotItemList.SetCurSel(m_depotItemList.FindString(0,itemName));
	sprintf(buf,"%d",depotWhen);
	m_depotWhen.SetWindowText(buf);
	sprintf(buf,"%d",depotRemain);
	m_depotRemain.SetWindowText(buf);
	m_depotEntryList.DeleteString(sel);	
}

void CConfigDialog::reloadDepotItems()
{
	CTibiaItemProxy itemProxy;

	while (m_depotItemList.GetCount()>0) m_depotItemList.DeleteString(0);

	// load items for depot item combo
	int count = itemProxy.getItemsItemsCount();
	int i;
	for (i=0;i<count;i++)
	{
		m_depotItemList.AddString(itemProxy.getItemsItems(i));
	}
	m_depotItemList.SetCurSel(0);
}

void CConfigDialog::reloadTrainingItems()
{
	CTibiaItemProxy itemProxy;

	while (m_weaponTrain.GetCount()>0) m_weaponTrain.DeleteString(0);
	while (m_weaponFight.GetCount()>0) m_weaponFight.DeleteString(0);

	// load items for depot item combo
	int count = itemProxy.getItemsItemsCount();
	int i;
	for (i=0;i<count;i++)
	{
		m_weaponTrain.AddString(itemProxy.getItemsItems(i));
		m_weaponFight.AddString(itemProxy.getItemsItems(i));
		int idx = m_weaponTrain.FindString(-1,itemProxy.getItemsItems(i));
		m_weaponTrain.SetItemData(idx,itemProxy.getItemsItemsId(i));
		m_weaponFight.SetItemData(idx,itemProxy.getItemsItemsId(i));
	}
	m_weaponTrain.SetCurSel(0);
	m_weaponFight.SetCurSel(0);
}

void CConfigDialog::OnToolAutoattackAddIgnore() 
{
	char buf[128];
	m_ignore.GetWindowText(buf,128);
	if (strlen(buf)&&m_ignoreList.GetCount()<32)
	{
		m_ignoreList.AddString(buf);
	}
	m_ignore.SetWindowText("");	
}

void CConfigDialog::OnToolAutoattackRemoveIgnore() 
{
	int sel=m_ignoreList.GetCurSel();
	if (sel==-1)
		return;
	char currentignore[128];
	m_ignoreList.GetText(sel,currentignore);
	m_ignoreList.DeleteString(sel);
	m_ignore.SetWindowText(currentignore);	
}

/* Compare two points for qsort */
int pointTabCompare( const void *arg1, const void *arg2 )
{	
   CTibiaMiniMapPoint *point1=*(CTibiaMiniMapPoint **)arg1;
   CTibiaMiniMapPoint *point2=*(CTibiaMiniMapPoint **)arg2;   
   if (point1==NULL) return 1;
   if (point2==NULL) return -1;
   return strcmp(point1->desc,point2->desc);
}

void CConfigDialog::OnLoadFromMinimap() 
{
	int ret=AfxMessageBox("Do you want to clear all waypoints before loading from minimap?",MB_YESNOCANCEL);
	if (ret==IDCANCEL) return;


	CTibiaMiniMapPoint **pointTab = (CTibiaMiniMapPoint **)malloc(sizeof(CTibiaMiniMapPoint *)*500);
	int i;
	for (i=0;i<500;i++) pointTab[i]=NULL;

	int totalPointCount=0;
	int addedPointCount=0;
	int mapNr,pointCount,pointNr;
	CMemReaderProxy reader;
	if (ret==IDYES)
	{
		m_waypointList.ResetContent();
	} else {
		totalPointCount=m_waypointList.GetCount();
	}
	for (mapNr=0;mapNr<10;mapNr++)
	{
		CTibiaMiniMap *map = reader.readMiniMap(mapNr);
		if (map->x!=-1||map->y!=0||map->z!=0)
		{
			// this is a really defined map
			pointCount=map->pointsAttached;
			for (pointNr=0;pointNr<pointCount;pointNr++)
			{
				CTibiaMiniMapPoint *point = reader.readMiniMapPoint(mapNr,pointNr);

				// point->x==0 means the point is deleted
				if (point->x&&!strncmp(point->desc,"$ta",3)&&totalPointCount<500)
				{
					// hack: store on point->type 'z' coord
					point->type=map->z;
					pointTab[totalPointCount++]=point;
					addedPointCount++;
					// when point is added for further processing
					// it will be delete'ed lateron
				} else {
					// free memory for point only when it is not added
					// for further processing
					delete point;
				}
			}
		}

		delete map;
	}	
	if (totalPointCount==500)
	{
		AfxMessageBox("Warning: maximum (500) supported waypoints amount exceeded!");
	}
	// sort all points to be added lexycographically
	qsort(pointTab,500,sizeof(CTibiaMiniMapPoint *),pointTabCompare);
	for (i=0;i<500;i++) 
	{
		if (pointTab[i]) 
		{
			// this is a point to be added
			char buf[256];
			int curX=pointTab[i]->x;
			int curY=pointTab[i]->y;
			int curZ=pointTab[i]->type; // hack only: store on type 'z' coord on map
			sprintf(buf,"(%d,%d,%d)",curX,curY,curZ);
			m_waypointList.AddString(buf);
			addedPointCount++;
			delete pointTab[i];
		}
	}

	if (addedPointCount==0)
	{
		AfxMessageBox("No points added. Hint: did you use $ta prefix for the points description?");
	}
}

void CConfigDialog::OnMonsterAttackUp() 
{
	char monsterName[256];
	int curSel = m_monsterList.GetCurSel();
	if (curSel<=0) return;
	m_monsterList.GetText(curSel,monsterName);
	m_monsterList.DeleteString(curSel);
	m_monsterList.InsertString(curSel-1,monsterName);
	m_monsterList.SetCurSel(curSel-1);
}

void CConfigDialog::OnMonsterAttackDown() 
{
	char monsterName[256];
	int curSel = m_monsterList.GetCurSel();
	if (curSel==-1) return;
	if (curSel==m_monsterList.GetCount()-1) return;
	m_monsterList.GetText(curSel,monsterName);
	m_monsterList.DeleteString(curSel);
	m_monsterList.InsertString(curSel+1,monsterName);
	m_monsterList.SetCurSel(curSel+1);
}

void CConfigDialog::OnPauseAll() {
	CPackSenderProxy sender;
	CMemReaderProxy reader;
	if (m_pauseAll.GetCheck()) {
		m_app->enableControls();
		reader.setGlobalVariable("caveboot_fullsleep","true");
		sender.stopAll();
	}
	else { 
		m_app->controlsToConfig();
		if (m_app->validateConfig(1)) {			
			reader.setGlobalVariable("caveboot_fullsleep","false");
			m_app->disableControls();
		}
		else 
			m_pauseWalker.SetCheck(1);
	}
}

void CConfigDialog::OnPauseWalker() {
	CPackSenderProxy sender;
	CMemReaderProxy reader;
	if (m_pauseWalker.GetCheck()) {
		m_app->enableControls();
		reader.setGlobalVariable("caveboot_halfsleep","true");
		sender.stopAll();
	}
	else {
		m_app->controlsToConfig();
		if (m_app->validateConfig(1)) {			
			reader.setGlobalVariable("caveboot_halfsleep","false");
			m_app->disableControls();
		}
		else 
			m_pauseWalker.SetCheck(1);
	}
}
