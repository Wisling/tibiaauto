// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include <iostream>
#include "mod_cavebot.h"
#include "ConfigDialog.h"
#include "LoadWaypointsInfo.h"
#include "DropLootDialog.h"

#include <TibiaItem.h>
#include <MemReader.h>
#include <VariableStore.h>
#include <TAMiniMap.h>
#include <TibiaMap.h>
#include <TibiaMiniMap.h>
#include <TibiaMiniMapLabel.h>
#include <TibiaMiniMapPoint.h>
#include <ModuleUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG


extern CToolAutoAttackStateAttack globalAutoAttackStateAttack;
extern CToolAutoAttackStateLoot globalAutoAttackStateLoot;
extern CToolAutoAttackStateWalker globalAutoAttackStateWalker;
extern CToolAutoAttackStateDepot globalAutoAttackStateDepot;
extern CToolAutoAttackStateTraining globalAutoAttackStateTraining;
extern int actualTargetX, actualTargetY, actualTargetZ;
extern int depotX, depotY, depotZ;
extern time_t currentPosTM;
extern int creatureAttackDist;
extern time_t firstCreatureAttackTM;
extern int currentWaypointNr;
extern int pathfindDistance;
extern time_t walkerStandingEndTm;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

void PathfindThread(LPVOID lpParam);

CConfigDialog::CConfigDialog(CMod_cavebotApp *app, CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app                          = app;
	cavebotFindpathStartedWaypoint = 0;

	for (int i = 0; i < MAX_DROPLISTCOUNT; i++)
	{
		virDropList[i][0] = '\0';
	}
	virDropListCount                = 0;
	virDropWhenCapacityLimitReached = 0;
	virDropOnlyLooted               = 0;
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_FRAME_WAYPOINT_WALKER, m_WaypointWalker);
	DDX_Control(pDX, IDC_FRAME_TRAINING, m_Training);
	DDX_Control(pDX, IDC_FRAME_MONSTER_ATTACKING, m_MonsterAttacking);
	DDX_Control(pDX, IDC_FRAME_DEPOT_WALKER, m_DepotWalker);
	DDX_Control(pDX, IDC_FRAME_DEBUGGING, m_Debugging);
	DDX_Control(pDX, IDC_FRAME_CORPSE_LOOTING, m_CorpseLooting);
	DDX_Control(pDX, IDC_DEPOT_CAP, m_depotCap);
	DDX_Control(pDX, IDC_SHARE_ALIEN_BACKATTACK, m_shareAlienBackattack);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_IGNORE, m_ignore);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_IGNORELIST, m_ignoreList);
	DDX_Control(pDX, IDC_BACKATTACK_RUNES, m_backattackRunes);
	DDX_Control(pDX, IDC_ATTACK_HP_ABOVE, m_attackHpAbove);
	DDX_Control(pDX, IDC_LOOTFROMFLOOR, m_lootFromFloor);
	DDX_Control(pDX, IDC_DROPNOTLOOTED, m_dropNotLooted);
	DDX_Control(pDX, IDC_DEPOT_DROP_INSTEAD_OF_DEPOSIT, m_depotDropInsteadOfDeposit);
	DDX_Control(pDX, IDC_TRAINING_WEAPON_TRAIN, m_weaponTrain);
	DDX_Control(pDX, IDC_TRAINING_WEAPON_FIGHT, m_weaponFight);
	DDX_Control(pDX, IDC_TRAINING_STATE, m_trainingState);
	DDX_Control(pDX, IDC_TRAINING_FIGHT_WHEN_SURROUNDED, m_fightWhenSurrounded);
	DDX_Control(pDX, IDC_TRAINING_FIGHT_WHEN_ALIEN, m_fightWhenAlien);
	DDX_Control(pDX, IDC_TRAINING_ATTACK_MODE, m_trainingMode);
	DDX_Control(pDX, IDC_TRAINING_BLOOD_HIT, m_bloodHit);
	DDX_Control(pDX, IDC_TRAINING_ACTIVATE, m_activate);
	DDX_Control(pDX, IDC_DEBUG, m_debug);
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
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_ALL_MONSTERS, m_attackAllMonsters);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_MONSTERLIST, m_monsterList);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_MONSTER, m_monster);
	DDX_Control(pDX, IDC_DONT_ATTACK_PLAYERS, m_dontAttackPlayers);
	DDX_Control(pDX, IDC_PAUSING_ENABLE, m_pausingEnable);
	DDX_Control(pDX, IDC_AUTORESEARCH, m_autoResearch);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_DEPOT_ENTRYREMOVE, m_depotEntryRemove);
	DDX_Control(pDX, IDC_DEPOT_ENTRYADD, m_depotEntryAdd);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_REMOVE_WAYPOINT, m_RemoveWaypoint);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_REMOVE_MONSTER, m_RemoveMonster);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_REMOVE_IGNORE, m_RemoveIgnore);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_ADD_WAYPOINT, m_AddWaypoint);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_ADD_MONSTER, m_AddMonster);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_ADD_IGNORE, m_AddIgnore);
	DDX_Control(pDX, IDC_LOAD_FROM_MINIMAP, m_LoadMinimap);
	DDX_Control(pDX, IDC_MONSTER_ATTACK_UP, m_MonsterDown);
	DDX_Control(pDX, IDC_MONSTER_ATTACK_DOWN, m_MonsterUp);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_RADIUS, m_radius);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_ADD_DELAY, m_AddDelay);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_DELAY, m_delay);
	DDX_Control(pDX, IDC_LOOTWHILEKILL, m_lootWhileKill);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_DROPLOOTLIST, m_dropLootList);
	DDX_Control(pDX, IDC_DEPOT_MODPRIORITY, m_depotModPriority);
	DDX_Control(pDX, IDC_DEPOT_STOPBYDEPOT, m_stopByDepot);
	DDX_Control(pDX, IDC_DEPOT_DEPOSIT_LOOTED_ITEM_LIST, m_depositLootedItemList);
	DDX_Control(pDX, IDC_DEPOT_AUTOADD_ITEM_LIST, m_autoaddItemList);
	DDX_Control(pDX, IDC_DEPOT_CLEAR_ENTRYLIST, m_clearEntryList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_ENABLE, OnEnable)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_REMOVE_MONSTER, OnToolAutoattackRemoveMonster)
ON_BN_DOUBLECLICKED(IDC_TOOL_AUTOATTACK_REMOVE_MONSTER, OnToolAutoattackRemoveMonster)
ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_ADD_MONSTER, OnToolAutoattackAddMonster)
ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_REMOVE_WAYPOINT, OnToolAutoattackRemoveWaypoint)
ON_BN_DOUBLECLICKED(IDC_TOOL_AUTOATTACK_REMOVE_WAYPOINT, OnToolAutoattackRemoveWaypoint)
ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_ADD_WAYPOINT, OnToolAutoattackAddWaypoint)
ON_BN_DOUBLECLICKED(IDC_TOOL_AUTOATTACK_ADD_WAYPOINT, OnToolAutoattackAddWaypoint)
ON_BN_CLICKED(IDC_DEPOT_ENTRYADD, OnDepotEntryadd)
ON_BN_DOUBLECLICKED(IDC_DEPOT_ENTRYADD, OnDepotEntryadd)
ON_BN_CLICKED(IDC_DEPOT_ENTRYREMOVE, OnDepotEntryremove)
ON_BN_DOUBLECLICKED(IDC_DEPOT_ENTRYREMOVE, OnDepotEntryremove)
ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_ADD_IGNORE, OnToolAutoattackAddIgnore)
ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_REMOVE_IGNORE, OnToolAutoattackRemoveIgnore)
ON_BN_DOUBLECLICKED(IDC_TOOL_AUTOATTACK_REMOVE_IGNORE, OnToolAutoattackRemoveIgnore)
ON_BN_CLICKED(IDC_LOAD_FROM_MINIMAP, OnLoadFromMinimap)
ON_BN_CLICKED(IDC_MONSTER_ATTACK_UP, OnMonsterAttackUp)
ON_BN_DOUBLECLICKED(IDC_MONSTER_ATTACK_UP, OnMonsterAttackUp)
ON_BN_CLICKED(IDC_MONSTER_ATTACK_DOWN, OnMonsterAttackDown)
ON_BN_DOUBLECLICKED(IDC_MONSTER_ATTACK_DOWN, OnMonsterAttackDown)
ON_BN_CLICKED(IDC_AUTORESEARCH, OnAutoResearch)
ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_ADD_DELAY, OnAddDelay)
ON_BN_DOUBLECLICKED(IDC_TOOL_AUTOATTACK_ADD_DELAY, OnAddDelay)
ON_BN_CLICKED(IDC_TOOL_AUTOATTACK_DROPLOOTLIST, OnDropLootList)
ON_BN_CLICKED(IDC_DROPNOTLOOTED, OnDropLoot)
ON_WM_ERASEBKGND()
ON_WM_DRAWITEM()
ON_WM_CTLCOLOR()
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_DEPOT_AUTOADD_ITEM_LIST, &CConfigDialog::OnAutoaddItemList)
ON_BN_CLICKED(IDC_DEPOT_CLEAR_ENTRYLIST, &CConfigDialog::OnDepotClearEntrylist)
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
	}
	else
	{
		m_app->stop();
	}
}

void CConfigDialog::disableControls()
{
	m_lootCustom.EnableWindow(false);
	m_lootWhileKill.EnableWindow(false);
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
	m_attackAllMonsters.EnableWindow(false);
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
	m_trainingMode.EnableWindow(false);
	m_bloodHit.EnableWindow(false);
	m_activate.EnableWindow(false);
	m_depotDropInsteadOfDeposit.EnableWindow(false);
	m_dropNotLooted.EnableWindow(false);
	m_lootFromFloor.EnableWindow(false);
	m_attackHpAbove.EnableWindow(false);
	m_ignoreList.EnableWindow(false);
	m_ignore.EnableWindow(false);
	m_backattackRunes.EnableWindow(false);
	m_shareAlienBackattack.EnableWindow(false);
	m_depotCap.EnableWindow(false);
	m_dontAttackPlayers.EnableWindow(false);
	m_pausingEnable.EnableWindow(false);
	m_curX.EnableWindow(false);
	m_curY.EnableWindow(false);
	m_curZ.EnableWindow(false);
	m_radius.EnableWindow(false);
	m_delay.EnableWindow(false);
	m_dropLootList.EnableWindow(false);
	m_dropNotLooted.EnableWindow(false);
	m_depotModPriority.EnableWindow(false);
	m_stopByDepot.EnableWindow(false);
	m_depositLootedItemList.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_lootCustom.EnableWindow(true);
	m_lootWhileKill.EnableWindow(true);
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
	m_attackAllMonsters.EnableWindow(true);
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
	m_trainingMode.EnableWindow(true);
	m_bloodHit.EnableWindow(true);
	m_activate.EnableWindow(true);
	m_depotDropInsteadOfDeposit.EnableWindow(true);
	m_dropNotLooted.EnableWindow(true);
	m_lootFromFloor.EnableWindow(true);
	m_attackHpAbove.EnableWindow(true);
	m_ignoreList.EnableWindow(true);
	m_ignore.EnableWindow(true);
	m_backattackRunes.EnableWindow(true);
	m_shareAlienBackattack.EnableWindow(true);
	m_depotCap.EnableWindow(true);
	m_dontAttackPlayers.EnableWindow(true);
	m_pausingEnable.EnableWindow(true);
	m_curX.EnableWindow(true);
	m_curY.EnableWindow(true);
	m_curZ.EnableWindow(true);
	m_radius.EnableWindow(true);
	m_delay.EnableWindow(true);
	m_dropLootList.EnableWindow(true);
	m_dropNotLooted.EnableWindow(true);
	m_depotModPriority.EnableWindow(true);
	m_stopByDepot.EnableWindow(true);
	m_depositLootedItemList.EnableWindow(true);

	OnDropLoot();
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	
	int i;
	char buf[128];


	sprintf(buf, "%d", configData->unreachableAfter);
	m_unreachableAfter.SetWindowText(buf);
	sprintf(buf, "%d", configData->suspendAfterUnreachable);
	m_suspendAfterUnreachable.SetWindowText(buf);
	sprintf(buf, "%d", configData->standStill);
	m_standStill.SetWindowText(buf);
	sprintf(buf, "%d", configData->attackHpAbove);
	m_attackHpAbove.SetWindowText(buf);
	m_attackOnlyAttacking.SetCheck(configData->attackOnlyAttacking);
	m_forceAttackAfterAttack.SetCheck(configData->forceAttackAfterAttack);
	m_stickToMonster.SetCheck(configData->stickToMonster);
	m_eatFromCorpse.SetCheck(configData->eatFromCorpse);
	sprintf(buf, "%d", configData->attackRange);
	m_attackRange.SetWindowText(buf);
	m_autoFollow.SetCheck(configData->autoFollow);
	m_attackAllMonsters.SetCheck(configData->attackAllMonsters);
	m_lootFood.SetCheck(configData->lootFood);
	m_lootGp.SetCheck(configData->lootGp);
	m_lootWorms.SetCheck(configData->lootWorms);
	m_mode.SetCurSel(configData->mode);
	m_suspendOnEnemy.SetCheck(configData->suspendOnEnemy);
	m_nomoveSuspended.SetCheck(configData->suspendOnNoMove);
	m_waypointSelMode.SetCurSel(configData->waypointSelectMode);
	m_mapUsed.SetCurSel(configData->mapUsed);
	m_lootCustom.SetCheck(configData->lootCustom);
	m_lootWhileKill.SetCheck(configData->lootWhileKill);
	m_lootinBags.SetCheck(configData->lootInBags);
	m_gatherLootStats.SetCheck(configData->gatherLootStats);
	m_debug.SetCheck(configData->debug);
	sprintf(buf, "%d", configData->capacityLimit);
	m_lootCapLimit.SetWindowText(buf);
	sprintf(buf, "%d", configData->depotCap);
	m_depotCap.SetWindowText(buf);
	while (m_waypointList.GetCount())
		m_waypointList.DeleteString(0);
	for (i = 0; i < MAX_WAYPOINTCOUNT; i++)
	{
		//y==-1 and z==-1 means waypoint is a delay
		if (configData->waypointList[i].y == -1 && configData->waypointList[i].z == -1)
		{
			int delay = configData->waypointList[i].x;
			sprintf(buf, "Delay %d sec", delay);
			m_waypointList.AddString(buf);
		}
		else if (configData->waypointList[i].x != 0 && configData->waypointList[i].y != 0)
		{
			sprintf(buf, "(%d,%d,%d)", configData->waypointList[i].x, configData->waypointList[i].y, configData->waypointList[i].z);
			m_waypointList.AddString(buf);
		}
	}
	while (m_monsterList.GetCount())
		m_monsterList.DeleteString(0);
	for (i = 0; i < configData->monsterCount && i < MAX_MONSTERLISTCOUNT; i++)
		m_monsterList.AddString(configData->monsterList[i]);

	while (m_ignoreList.GetCount())
		m_ignoreList.DeleteString(0);
	for (i = 0; i < configData->ignoreCount && i < MAX_IGNORECOUNT; i++)
		m_ignoreList.AddString(configData->ignoreList[i]);

	while (m_depotEntryList.GetCount())
		m_depotEntryList.DeleteString(0);
	reloadDepotItems();
	reloadTrainingItems();
	for (i = 0; i < MAX_DEPOTTRIGGERCOUNT; i++)
	{
		if (strlen(configData->depotTrigger[i].itemName))
		{
			sprintf(buf, "%s %d->%d", configData->depotTrigger[i].itemName, configData->depotTrigger[i].when, configData->depotTrigger[i].remain);
			m_depotEntryList.AddString(buf);
			m_depotItemList.DeleteString(m_depotItemList.FindStringExact(-1, configData->depotTrigger[i].itemName));
		}
	}
	m_fightWhenSurrounded.SetCheck(configData->fightWhenSurrounded);
	m_fightWhenAlien.SetCheck(configData->fightWhenAlien);
	m_trainingMode.SetCurSel(configData->trainingMode);
	m_bloodHit.SetCheck(configData->bloodHit);
	m_activate.SetCheck(configData->trainingActivate);
	m_weaponTrain.SetCurSel(m_weaponTrain.FindStringExact(-1, CTibiaItem::getItemName(configData->weaponTrain)));
	if (m_weaponTrain.GetCurSel() == -1)
		m_weaponTrain.SetCurSel(0);
	m_weaponFight.SetCurSel(m_weaponFight.FindStringExact(-1, CTibiaItem::getItemName(configData->weaponFight)));
	if (m_weaponFight.GetCurSel() == -1)
		m_weaponFight.SetCurSel(0);
	m_depotDropInsteadOfDeposit.SetCheck(configData->depotDropInsteadOfDeposit);
	m_dropNotLooted.SetCheck(configData->dropNotLooted);
	m_lootFromFloor.SetCheck(configData->lootFromFloor);
	m_backattackRunes.SetCheck(configData->backattackRunes);
	m_shareAlienBackattack.SetCheck(configData->shareAlienBackattack);
	m_dontAttackPlayers.SetCheck(configData->dontAttackPlayers);
	m_pausingEnable.SetCheck(configData->pausingEnable);
	sprintf(buf, "%d", configData->radius);
	m_radius.SetWindowText(buf);

	memcpy(virDropList, configData->dropList, 64 * MAX_DROPLISTCOUNT);
	virDropListCount                = configData->dropListCount;
	virDropWhenCapacityLimitReached = configData->dropWhenCapacityLimitReached;
	virDropOnlyLooted               = configData->dropOnlyLooted;
	m_depotModPriority.SetCurSel(atoi(configData->depotModPriorityStr) - 1);
	m_stopByDepot.SetCheck(configData->stopByDepot);
	m_depositLootedItemList.SetCheck(configData->depositLootedItemList);

	OnDropLoot();
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	m_standStill.GetWindowText(buf, 127);
	newConfigData->standStill = atoi(buf);
	m_suspendAfterUnreachable.GetWindowText(buf, 127);
	newConfigData->suspendAfterUnreachable = atoi(buf);
	m_unreachableAfter.GetWindowText(buf, 127);
	newConfigData->unreachableAfter = atoi(buf);
	m_attackHpAbove.GetWindowText(buf, 127);
	newConfigData->attackHpAbove          = atoi(buf);
	newConfigData->attackOnlyAttacking    = m_attackOnlyAttacking.GetCheck();
	newConfigData->forceAttackAfterAttack = m_forceAttackAfterAttack.GetCheck();
	newConfigData->stickToMonster         = m_stickToMonster.GetCheck();
	newConfigData->eatFromCorpse          = m_eatFromCorpse.GetCheck();
	m_attackRange.GetWindowText(buf, 127);
	newConfigData->attackRange       = atoi(buf);
	newConfigData->autoFollow        = m_autoFollow.GetCheck();
	newConfigData->attackAllMonsters = m_attackAllMonsters.GetCheck();
	newConfigData->lootFood          = m_lootFood.GetCheck();
	newConfigData->lootGp            = m_lootGp.GetCheck();
	newConfigData->lootWorms         = m_lootWorms.GetCheck();
	newConfigData->mode              = m_mode.GetCurSel();
	newConfigData->suspendOnEnemy    = m_suspendOnEnemy.GetCheck();
	newConfigData->gatherLootStats   = m_gatherLootStats.GetCheck();
	newConfigData->debug             = m_debug.GetCheck();
	int i;
	newConfigData->monsterCount = m_monsterList.GetCount();
	for (i = 0; i < MAX_MONSTERLISTCOUNT; i++)
		newConfigData->monsterList[i][0] = 0;
	for (i = 0; i < m_monsterList.GetCount(); i++)
	{
		if (i < MAX_MONSTERLISTCOUNT)
		{
			int j;
			m_monsterList.GetText(i, newConfigData->monsterList[i]);
			int len = strlen(newConfigData->monsterList[i]);
			for (j = 0; j < len; j++)
				newConfigData->monsterList[i][j] = tolower(newConfigData->monsterList[i][j]);
		}
		else
		{
			m_monsterList.DeleteString(i);
		}
	};
	newConfigData->ignoreCount = m_ignoreList.GetCount();
	for (i = 0; i < MAX_IGNORECOUNT; i++)
		newConfigData->ignoreList[i][0] = 0;
	for (i = 0; i < m_ignoreList.GetCount(); i++)
	{
		if (i < MAX_IGNORECOUNT)
		{
			int j;
			m_ignoreList.GetText(i, newConfigData->ignoreList[i]);
			int len = strlen(newConfigData->ignoreList[i]);
			for (j = 0; j < len; j++)
				newConfigData->ignoreList[i][j] = tolower(newConfigData->ignoreList[i][j]);
		}
		else
		{
			m_ignoreList.DeleteString(i);
		}
	};
	for (i = 0; i < MAX_WAYPOINTCOUNT; i++)
	{
		newConfigData->waypointList[i].x = 0;
		newConfigData->waypointList[i].y = 0;
		newConfigData->waypointList[i].z = 0;
	}
	for (i = 0; i < m_waypointList.GetCount(); i++)
	{
		if (i < MAX_WAYPOINTCOUNT){
			m_waypointList.GetText(i, buf);
			int curX = 0, curY = 0, curZ = 0;
			sscanf(buf, "(%d,%d,%d)", &curX, &curY, &curZ);

			if (curX == 0 && curY == 0 && curZ == 0)
			{
				int delay = 0;
				sscanf(buf, "Delay %d sec", &delay);
				newConfigData->waypointList[i].x = delay;
				newConfigData->waypointList[i].y = -1;
				newConfigData->waypointList[i].z = -1;
			}
			else
			{
				newConfigData->waypointList[i].x = curX;
				newConfigData->waypointList[i].y = curY;
				newConfigData->waypointList[i].z = curZ;
			}
		}
		else
		{
			m_waypointList.DeleteString(i);
		}
	};
	newConfigData->suspendOnNoMove    = m_nomoveSuspended.GetCheck();
	newConfigData->waypointSelectMode = m_waypointSelMode.GetCurSel();
	newConfigData->mapUsed            = m_mapUsed.GetCurSel();
	m_lootCapLimit.GetWindowText(buf, 127);
	newConfigData->capacityLimit = atoi(buf);
	m_depotCap.GetWindowText(buf, 127);
	newConfigData->depotCap      = atoi(buf);
	newConfigData->lootCustom    = m_lootCustom.GetCheck();
	newConfigData->lootWhileKill = m_lootWhileKill.GetCheck();
	newConfigData->lootInBags    = m_lootinBags.GetCheck();

	for (i = 0; i < MAX_DEPOTTRIGGERCOUNT; i++)
		newConfigData->depotTrigger[i].itemName[0] = '\0';
	for (i = 0; i < m_depotEntryList.GetCount(); i++)
	{
		if (i < MAX_DEPOTTRIGGERCOUNT)
		{
			char paramString[128];
			m_depotEntryList.GetText(i, newConfigData->depotTrigger[i].itemName);
			// from depotEntryList we got 'Apple Pie 128->64'
			// and we convert it to Apple Pie + 128->64
			int j;
			for (j = strlen(newConfigData->depotTrigger[i].itemName) - 1; j > 0; j--)
			{
				if (newConfigData->depotTrigger[i].itemName[j] == ' ')
				{
					memcpy(paramString, newConfigData->depotTrigger[i].itemName + j + 1, strlen(newConfigData->depotTrigger[i].itemName) - j);
					newConfigData->depotTrigger[i].itemName[j] = '\0';
					break;
				}
			}
			sscanf(paramString, "%d->%d", &newConfigData->depotTrigger[i].when, &newConfigData->depotTrigger[i].remain);
		}
		else
		{
			m_depotEntryList.DeleteString(i);
		}
	}

	newConfigData->fightWhenSurrounded       = m_fightWhenSurrounded.GetCheck();
	newConfigData->fightWhenAlien            = m_fightWhenAlien.GetCheck();
	newConfigData->trainingMode              = m_trainingMode.GetCurSel();
	newConfigData->bloodHit                  = m_bloodHit.GetCheck();
	newConfigData->trainingActivate          = m_activate.GetCheck();
	newConfigData->weaponTrain               = m_weaponTrain.GetItemData(m_weaponTrain.GetCurSel());
	newConfigData->weaponFight               = m_weaponFight.GetItemData(m_weaponFight.GetCurSel());
	newConfigData->depotDropInsteadOfDeposit = m_depotDropInsteadOfDeposit.GetCheck();
	newConfigData->dropNotLooted             = m_dropNotLooted.GetCheck();
	newConfigData->lootFromFloor             = m_lootFromFloor.GetCheck();
	newConfigData->backattackRunes           = m_backattackRunes.GetCheck();
	newConfigData->shareAlienBackattack      = m_shareAlienBackattack.GetCheck();
	newConfigData->dontAttackPlayers         = m_dontAttackPlayers.GetCheck();
	newConfigData->pausingEnable             = m_pausingEnable.GetCheck();
	newConfigData->selectedWaypoint          = m_waypointList.GetCurSel();
	m_radius.GetWindowText(buf, 127);
	newConfigData->radius = atoi(buf);

	memcpy(newConfigData->dropList, virDropList, 64 * MAX_DROPLISTCOUNT);
	newConfigData->dropListCount                = virDropListCount;
	newConfigData->dropWhenCapacityLimitReached = virDropWhenCapacityLimitReached;
	newConfigData->dropOnlyLooted               = virDropOnlyLooted;
	sprintf(newConfigData->depotModPriorityStr, "%d", m_depotModPriority.GetCurSel() + 1);
	newConfigData->stopByDepot           = m_stopByDepot.GetCheck();
	newConfigData->depositLootedItemList = m_depositLootedItemList.GetCheck();

	return newConfigData;
}

HANDLE hThread = 0;

static int lastX = 0, lastY = 0, lastZ = 0;

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1001)
	{
		KillTimer(1001);
		CMemReader& reader = CMemReader::getMemReader();

		CTibiaCharacter self;
		reader.readSelfCharacter(&self);

		char buf[256];
		if (lastX != self.x || lastY != self.y || lastZ != self.z)
		{
			sprintf(buf, "%d", self.x);
			m_curX.SetWindowText(buf);
			sprintf(buf, "%d", self.y);
			m_curY.SetWindowText(buf);
			sprintf(buf, "%d", self.z);
			m_curZ.SetWindowText(buf);
			lastX = self.x;
			lastY = self.y;
			lastZ = self.z;
		}


		switch (globalAutoAttackStateAttack)
		{
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
			time_t t1 = time(NULL) - firstCreatureAttackTM;
			time_t t2 = time(NULL) - currentPosTM;
			sprintf(buf, "State: attacking [since %lld, distance %d; standing since %llds]", t1, creatureAttackDist, t2);
			m_stateAttack.SetWindowText(buf);
			break;
		}
		default:
			m_stateAttack.SetWindowText("State: unknown");
		}

		switch (globalAutoAttackStateLoot)
		{
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
		default:
			m_stateLoot.SetWindowText("State: unknown");
		}


		switch (globalAutoAttackStateWalker)
		{
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
			if (globalAutoAttackStateDepot != CToolAutoAttackStateDepot_walking)
			{
				int tilesAway = abs(self.x - actualTargetX) + abs(self.y - actualTargetY) + abs(self.z - actualTargetZ);
				sprintf(buf, "State: walking to (%d,%d,%d) %d tiles away", actualTargetX, actualTargetY, actualTargetZ, tilesAway);
				m_stateWalker.SetWindowText(buf);
				if (currentWaypointNr != m_waypointList.GetCurSel())
					m_waypointList.SetCurSel(currentWaypointNr);
			}
			else
			{
				sprintf(buf, "State: not running");
				m_stateWalker.SetWindowText(buf);
			}
			break;
		}
		case CToolAutoAttackStateWalker_noPathFound:
			m_stateWalker.SetWindowText("State: no path found");
			break;
		case CToolAutoAttackStateWalker_standing:
			if (walkerStandingEndTm - time(NULL) < 0)
				break;
			sprintf(buf, "State: standing for %d more seconds", (int)(walkerStandingEndTm ? walkerStandingEndTm - time(NULL) : 0));
			m_stateWalker.SetWindowText(buf);
			if (currentWaypointNr != m_waypointList.GetCurSel())
				m_waypointList.SetCurSel(currentWaypointNr);
			break;
		case CToolAutoAttackStateWalker_halfSleep:
			sprintf(buf, "State: half module sleep by %s:%s", CVariableStore::getVariable("walking_control"), CVariableStore::getVariable("walking_priority"));
			m_stateWalker.SetWindowText(buf);
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
		case CToolAutoAttackStateDepot_macroPause:
			m_stateDepot.SetWindowText("State: hotkey paused");
			break;
		case CToolAutoAttackStateDepot_notFound:
			m_stateDepot.SetWindowText("State: depot not found");
			break;
		case CToolAutoAttackStateDepot_noSpace:
			m_stateDepot.SetWindowText("State: no space in depot chest");
			break;
		case CToolAutoAttackStateDepot_walking:
		{
			int tilesAway = abs(self.x - depotX) + abs(self.y - depotY) + abs(self.z - depotZ);
			sprintf(buf, "State: walking to depot (%d,%d,%d) %d tiles away", depotX, depotY, depotZ, tilesAway);
			m_stateDepot.SetWindowText(buf);
			break;
		}
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
		case CToolAutoAttackStateTraining_trainingLessDef:
			m_trainingState.SetWindowText("State: training and attempting bloodhit");
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

		CTAMiniMap& taMiniMap = CTAMiniMap::getTAMiniMap();
		if (!taMiniMap.isFindPathStopped())
		{
			char buf[128];
			sprintf(buf, "Auto Research Map Distance to %d:%d", cavebotFindpathStartedWaypoint, taMiniMap.getCurrentDistance());
			m_autoResearch.SetWindowText(buf);
		}
		else
		{
			char buf[128];
			sprintf(buf, "Auto Research Map");
			m_autoResearch.SetWindowText(buf);
		}

		DWORD exitCode          = 0;
		static int failedOnce   = 0;
		static time_t startTime = 0;
		if (hThread)
			GetExitCodeThread(hThread, &exitCode);
		if (exitCode == 1)//errors finding path
		{
			failedOnce = 1;
			hThread    = 0;
			char buf[512];
			sprintf(buf, "Failed to find path to waypoint %d. Either the travelling distance is much larger than distance between the points or it may be blocked via the minimap.", cavebotFindpathStartedWaypoint);
			SetTimer(1001, 100, NULL);
			CDialog::OnTimer(nIDEvent);
			AfxMessageBox(buf);
			return;
		}

		if (m_autoResearch.GetCheck() && (!hThread || exitCode != STILL_ACTIVE))// STILL_ACTIVE = 0x00000103 = 259
		{
			if (!pathfindPoints.empty())
			{
				if (!cavebotFindpathStartedWaypoint)
					startTime = time(NULL);
				cavebotFindpathStartedWaypoint += 1;
				PathfindParams* myData = pathfindPoints.front();
				pathfindPoints.pop();
				DWORD dwThread;
				hThread = CreateThread(
				        NULL,              // default security attributes
				        0,                 // use default stack size
				        (LPTHREAD_START_ROUTINE)PathfindThread,        // thread function
				        (LPVOID)myData,             // argument to thread function
				        0,                 // use default creation flags
				        &dwThread);   // returns the thread identifier
			}
			else
			{
				m_autoResearch.SetCheck(0);
				if (!failedOnce)
				{
					SetTimer(1001, 100, NULL);
					CDialog::OnTimer(nIDEvent);
					char buf[512];
					sprintf(buf, "Successfully completed researching map in %d seconds.\nRemember to enable extended map reseach to changing levels and avoid new obstacles.", (int)(time(NULL) - startTime + 1));
					AfxMessageBox(buf);
					return;
				}
			}
		}
		else if (!m_autoResearch.GetCheck() && cavebotFindpathStartedWaypoint)
		{
			hThread                        = 0;
			startTime                      = 0;
			failedOnce                     = 0;
			cavebotFindpathStartedWaypoint = 0;
			if (!taMiniMap.isFindPathStopped())
				taMiniMap.findPathStop();
			while (!pathfindPoints.empty())
			{
				PathfindParams*& myData = pathfindPoints.front();
				delete myData;
				pathfindPoints.pop();
			}
		}



		SetTimer(1001, 100, NULL);
	};

	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_autoResearch);
	skin.SetButtonSkin(m_enable);
	skin.SetButtonSkin(m_depotEntryRemove);
	skin.SetButtonSkin(m_depotEntryAdd);
	skin.SetButtonSkin(m_OK);
	skin.SetButtonSkin(m_RemoveWaypoint);
	skin.SetButtonSkin(m_RemoveMonster);
	skin.SetButtonSkin(m_RemoveIgnore);
	skin.SetButtonSkin(m_AddWaypoint);
	skin.SetButtonSkin(m_AddMonster);
	skin.SetButtonSkin(m_AddIgnore);
	skin.SetButtonSkin(m_LoadMinimap);
	skin.SetButtonSkin(m_MonsterDown);
	skin.SetButtonSkin(m_MonsterUp);
	skin.SetButtonSkin(m_AddDelay);
	skin.SetButtonSkin(m_dropLootList);
	skin.SetButtonSkin(m_autoaddItemList);
	skin.SetButtonSkin(m_clearEntryList);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	// initialise comboboxes, etc.
	reloadDepotItems();
	reloadTrainingItems();


	// set timers
	SetTimer(1001, 100, NULL);

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
	int sel = m_waypointList.GetCurSel();
	/* Removed to avoid confusion
	   int x,y,z;
	   char buf[128];
	   m_waypointList.GetText(sel,buf);
	   if (sscanf(buf,"(%d,%d,%d)",&x,&y,&z)==3){
	        sprintf(buf,"%d",x);m_curX.SetWindowText(buf);
	        sprintf(buf,"%d",y);m_curY.SetWindowText(buf);
	        sprintf(buf,"%d",z);m_curZ.SetWindowText(buf);
	   }
	 */
	if (sel == -1)
		return;
	m_waypointList.DeleteString(sel);
	m_waypointList.SetCurSel(min(sel, m_waypointList.GetCount() - 1));
}

void CConfigDialog::OnToolAutoattackAddWaypoint()
{
	if (!(m_waypointList.GetCount() < MAX_WAYPOINTCOUNT)){
		char buf[256];
		sprintf(buf, "You cannot add more than %d waypoints.", MAX_WAYPOINTCOUNT);
		AfxMessageBox(buf);
		return;
	}
	char buf[256];
	int curX, curY, curZ;
	m_curX.GetWindowText(buf, 255);
	curX = atoi(buf);
	m_curY.GetWindowText(buf, 255);
	curY = atoi(buf);
	m_curZ.GetWindowText(buf, 255);
	curZ = atoi(buf);
	sprintf(buf, "(%d,%d,%d)", curX, curY, curZ);
	if (m_waypointList.GetCount() != 1 && m_waypointList.GetCurSel() == 0)
	{
		m_waypointList.InsertString(0, buf);
		m_waypointList.SetCurSel(0);
	}
	else
	{
		m_waypointList.InsertString(m_waypointList.GetCurSel() + 1, buf);
		m_waypointList.SetCurSel(m_waypointList.GetCurSel() + 1);
	}
}

void CConfigDialog::OnAddDelay()
{
	if (!(m_waypointList.GetCount() < MAX_WAYPOINTCOUNT)){
		char buf[256];
		sprintf(buf, "You cannot add more than %d waypoints.", MAX_WAYPOINTCOUNT);
		AfxMessageBox(buf);
		return;
	}
	char buf[256];
	m_delay.GetWindowText(buf, 255);
	int tm = atoi(buf);
	if (tm < 0)
	{
		AfxMessageBox("Delay time must be >=0!");
		m_delay.SetWindowText("");
		return;
	}
	sprintf(buf, "Delay %d sec", tm);
	if (m_waypointList.GetCount() != 1 && m_waypointList.GetCurSel() == 0)
	{
		m_waypointList.InsertString(0, buf);
		m_waypointList.SetCurSel(0);
	}
	else
	{
		m_waypointList.InsertString(m_waypointList.GetCurSel() + 1, buf);
		m_waypointList.SetCurSel(m_waypointList.GetCurSel() + 1);
	}
}

void CConfigDialog::OnToolAutoattackRemoveMonster()
{
	int sel = m_monsterList.GetCurSel();
	if (sel == -1)
		return;
	char currentMonster[128];
	m_monsterList.GetText(sel, currentMonster);
	m_monsterList.DeleteString(sel);
	m_monster.SetWindowText(currentMonster);
	m_monsterList.SetCurSel(min(max(sel - 1, 0), m_monsterList.GetCount() - 1));
}

void CConfigDialog::OnToolAutoattackAddMonster()
{
	char buf[128];
	m_monster.GetWindowText(buf, 128);
	if (strlen(buf)){
		if (!(m_monsterList.GetCount() < MAX_MONSTERLISTCOUNT)){
			char buf[256];
			sprintf(buf, "You cannot add more than %d monsters to the cavebot attack list.", MAX_MONSTERLISTCOUNT);
			AfxMessageBox(buf);
			return;
		}
		m_monsterList.SetCurSel(m_monsterList.AddString(buf));
		m_monster.SetWindowText("");
	}
}

void CConfigDialog::OnDepotEntryadd()
{
	if (!(m_depotEntryList.GetCount() < MAX_DEPOTTRIGGERCOUNT)){
		char buf[256];
		sprintf(buf, "You cannot add more than %d depot triggers.", MAX_DEPOTTRIGGERCOUNT);
		AfxMessageBox(buf);
		return;
	}

	char buf[128];
	int depotWhen = 0;
	int depotRemain = 0;
	int sel = m_depotItemList.GetCurSel();
	char itemName[128];

	m_depotWhen.GetWindowText(buf, 127);
	depotWhen = atoi(buf);
	m_depotRemain.GetWindowText(buf, 127);
	depotRemain = atoi(buf);
	m_depotItemList.GetLBText(sel, itemName);

	if (sel == -1)
		return;
	if (depotWhen < 0)
		return;
	if (depotRemain < 0)
		return;
	if (depotWhen == depotRemain)
		return;

	sprintf(buf, "%s %d->%d", itemName, depotWhen, depotRemain);
	m_depotEntryList.SetCurSel(m_depotEntryList.AddString(buf));
	m_depotItemList.DeleteString(m_depotItemList.FindStringExact(-1, itemName));
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
	int sel = m_depotEntryList.GetCurSel();
	if (sel == -1)
		return;
	m_depotEntryList.GetText(sel, itemName);
	// from depotEntryList we got 'Apple Pie 128->64'
	// and we convert it to Apple Pie + 128->64
	int i;
	for (i = strlen(itemName) - 1; i > 0; i--)
	{
		if (itemName[i] == ' ')
		{
			memcpy(paramString, itemName + i + 1, strlen(itemName) - i);
			itemName[i] = '\0';
			break;
		}
	}
	sscanf(paramString, "%d->%d", &depotWhen, &depotRemain);
	m_depotItemList.AddString(itemName);
	m_depotItemList.SetCurSel(m_depotItemList.FindStringExact(-1, itemName));
	sprintf(buf, "%d", depotWhen);
	m_depotWhen.SetWindowText(buf);
	sprintf(buf, "%d", depotRemain);
	m_depotRemain.SetWindowText(buf);
	m_depotEntryList.DeleteString(sel);
	m_depotEntryList.SetCurSel(min(sel, m_depotEntryList.GetCount() - 1));
}

void CConfigDialog::reloadDepotItems()
{
	

	while (m_depotItemList.GetCount() > 0)
		m_depotItemList.DeleteString(0);

	// load items for depot item combo
	int count = CTibiaItem::getItemCount();
	for (int i = 0; i < count; i++)
	{
		m_depotItemList.AddString(CTibiaItem::getItemNameAtIndex(i));
	}
	m_depotItemList.SetCurSel(0);
}

void CConfigDialog::reloadTrainingItems()
{
	

	while (m_weaponTrain.GetCount() > 0)
		m_weaponTrain.DeleteString(0);
	while (m_weaponFight.GetCount() > 0)
		m_weaponFight.DeleteString(0);

	// load items for depot item combo
	int count = CTibiaItem::getItemCount();
	int i;
	for (i = 0; i < count; i++)
	{
		m_weaponTrain.AddString(CTibiaItem::getItemNameAtIndex(i));
		m_weaponFight.AddString(CTibiaItem::getItemNameAtIndex(i));
		int idx = m_weaponTrain.FindString(-1, CTibiaItem::getItemNameAtIndex(i));
		m_weaponTrain.SetItemData(idx, CTibiaItem::getItemIdAtIndex(i));
		m_weaponFight.SetItemData(idx, CTibiaItem::getItemIdAtIndex(i));
	}
	m_weaponTrain.SetCurSel(0);
	m_weaponFight.SetCurSel(0);
}

void CConfigDialog::OnToolAutoattackAddIgnore()
{
	char buf[128];
	m_ignore.GetWindowText(buf, 128);
	if (strlen(buf)){
		if (!(m_ignoreList.GetCount() < MAX_IGNORECOUNT)){
			char buf[256];
			sprintf(buf, "You cannot add more than %d ignored monsters to the cavebot list.", MAX_IGNORECOUNT);
			AfxMessageBox(buf);
			return;
		}
		m_ignoreList.AddString(buf);
		m_ignore.SetWindowText("");
		m_ignoreList.SetCurSel(m_ignoreList.GetCount() - 1);
	}
}

void CConfigDialog::OnToolAutoattackRemoveIgnore()
{
	int sel = m_ignoreList.GetCurSel();
	if (sel == -1)
		return;
	char currentignore[128];
	m_ignoreList.GetText(sel, currentignore);
	m_ignoreList.DeleteString(sel);
	m_ignore.SetWindowText(currentignore);
	m_ignoreList.SetCurSel(min(sel, m_ignoreList.GetCount() - 1));
}

/* Compare two points for qsort */
int pointTabCompare(const void *arg1, const void *arg2)
{
	CTibiaMiniMapLabel *point1 = *(CTibiaMiniMapLabel **)arg1;
	CTibiaMiniMapLabel *point2 = *(CTibiaMiniMapLabel **)arg2;
	if (point1 == NULL)
		return 1;
	if (point2 == NULL)
		return -1;
	return strcmp(point1->desc, point2->desc);
}

void CConfigDialog::OnLoadFromMinimap()
{
	char prefixOut[512];
	CLoadWaypointsInfo *dialog = new CLoadWaypointsInfo(prefixOut);
	int ret                    = dialog->DoModal();
	delete dialog;

	if (ret == IDCANCEL)
		return;

	CTibiaMiniMapLabel **pointTab = (CTibiaMiniMapLabel **)malloc(sizeof(CTibiaMiniMapLabel *) * 500);
	int i;
	for (i = 0; i < 500; i++)
		pointTab[i] = NULL;

	int totalPointCount = 0;
	int addedPointCount = 0;
	int mapNr, pointCount, pointNr;
	CMemReader& reader = CMemReader::getMemReader();
	if (ret == IDYES)
		m_waypointList.ResetContent();
	else
		totalPointCount = m_waypointList.GetCount();
	for (mapNr = 0; mapNr < 10; mapNr++)
	{
		CTibiaMiniMap *map = reader.readMiniMap(mapNr);
		if (map->x != -1 || map->y != 0 || map->z != 0)
		{
			// this is a really defined map
			pointCount = map->pointsAttached;
			for (pointNr = 0; pointNr < pointCount; pointNr++)
			{
				CTibiaMiniMapLabel *point = reader.readMiniMapLabel(mapNr, pointNr);

				// point->x==0 means the point is deleted
				int len = strlen(prefixOut);
				if (point->x && !strncmp(point->desc, prefixOut, len) && totalPointCount < 500)
				{
					// hack: store on point->type 'z' coord
					point->type                 = map->z;
					pointTab[totalPointCount++] = point;
					addedPointCount++;
					// when point is added for further processing
					// it will be deleteed later on
				}
				else
				{
					// free memory for point only when it is not added
					// for further processing
					delete point;
				}
			}
		}

		delete map;
	}
	if (totalPointCount == 500)
		AfxMessageBox("Warning: maximum (500) supported waypoints amount exceeded!");
	// sort all points to be added lexicographically
	qsort(pointTab, 500, sizeof(CTibiaMiniMapLabel *), pointTabCompare);
	for (i = 0; i < 500; i++)
	{
		if (pointTab[i])
		{
			if (i < MAX_WAYPOINTCOUNT){
				// this is a point to be added
				char buf[256];
				int curX = pointTab[i]->x;
				int curY = pointTab[i]->y;
				int curZ = pointTab[i]->type; // hack only: store on type 'z' coord on map
				sprintf(buf, "(%d,%d,%d)", curX, curY, curZ);
				m_waypointList.AddString(buf);
			}
			addedPointCount++;
			delete pointTab[i];
		}
	}

	if (addedPointCount == 0)
		AfxMessageBox("No points added. Hint: did you specify a valid prefix to find in the point descriptions?");
}

void CConfigDialog::OnMonsterAttackUp()
{
	char monsterName[256];
	int curSel = m_monsterList.GetCurSel();
	if (curSel <= 0)
		return;
	m_monsterList.GetText(curSel, monsterName);
	m_monsterList.DeleteString(curSel);
	m_monsterList.InsertString(curSel - 1, monsterName);
	m_monsterList.SetCurSel(curSel - 1);
}

void CConfigDialog::OnMonsterAttackDown()
{
	char monsterName[256];
	int curSel = m_monsterList.GetCurSel();
	if (curSel == -1)
		return;
	if (curSel == m_monsterList.GetCount() - 1)
		return;
	m_monsterList.GetText(curSel, monsterName);
	m_monsterList.DeleteString(curSel);
	m_monsterList.InsertString(curSel + 1, monsterName);
	m_monsterList.SetCurSel(curSel + 1);
}

void PathfindThread(LPVOID lpParam)
{
	CTibiaMap& tibiaMap = CTibiaMap::getTibiaMap();
	PathfindParams* myData = (PathfindParams*)lpParam;
	int startX             = myData->x;
	int startY             = myData->y;
	int startZ             = myData->z;
	int endX               = myData->x2;
	int endY               = myData->y2;
	int endZ               = myData->z2;
	int direction[10][3]   = {{0, 0, 1}, {1, 0, 0}, {1, -1, 0}, {0, -1, 0}, {-1, -1, 0}, {-1, 0, 0}, {-1, 1, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, -1}};

	CUIntArray *path = CTAMiniMap::getTAMiniMap().findPathOnMiniMap(myData->x, myData->y, myData->z, myData->x2, myData->y2, myData->z2);

	int i;
	int radius = 2;//0 means 1x1, 2 means 5x5
	int x, y;
	int sz = path->GetSize();
	/*
	   char buf[111111];
	   sprintf(buf,"%d - ",sz);
	   for (i=0;i<sz;i++){

	        sprintf(buf+strlen(buf),"%d",a->GetAt(i));
	   }
	   AfxMessageBox(buf);
	 */
	//map all squares around currrent point
	for (x = startX - radius; x <= startX + radius; x++)
	{
		for (y = startY - radius; y <= startY + radius; y++)
		{
			if (!tibiaMap.isPointAvailableNoProh(x, y, startZ))
			{
				CTibiaMiniMapPoint * mp = CTAMiniMap::getTAMiniMap().getMiniMapPoint(x, y, startZ);
				if (mp->speed != 255 && mp->colour != 0)
				{
					tibiaMap.setPointAsAvailable(x, y, startZ);//is not blocking nor unexplored
					tibiaMap.setPointSpeed(x, y, startZ, mp->speed);
				}
				delete mp;
			}
		}
	}
	//Add data to TA's Cavebotting Map
	for (i = 0; i < sz; i++)
	{
		int dir = path->GetAt(i);
		if (dir == STEP_UPSTAIRS)
			dir = 9;
		else if (dir == STEP_DOWNSTAIRS)
			dir = 0;
		startX += direction[dir][0];
		startY += direction[dir][1];
		startZ += direction[dir][2];
		if (dir == 0 || dir == 9)//floor change
		{       //map all new squares around currrent point
			for (x = startX - radius; x <= startX + radius; x++)
			{
				for (y = startY - radius; y <= startY + radius; y++)
				{
					if (!tibiaMap.isPointAvailableNoProh(x, y, startZ))
					{
						CTibiaMiniMapPoint * mp = CTAMiniMap::getTAMiniMap().getMiniMapPoint(x, y, startZ);
						if (mp->speed != 255 && mp->colour != 0)
						{
							tibiaMap.setPointAsAvailable(x, y, startZ);//is not blocking nor unexplored
							tibiaMap.setPointSpeed(x, y, startZ, mp->speed);
						}
						delete mp;
					}
				}
			}
			MapPointType setPrev = (dir == 0) ? MAP_POINT_TYPE_OPEN_HOLE : MAP_POINT_TYPE_STAIRS;//hole if went down; stairs if went up
			MapPointType setCur = (dir == 9) ? MAP_POINT_TYPE_OPEN_HOLE : MAP_POINT_TYPE_STAIRS;//hole if went up; stairs if went down
			//add level change point
			tibiaMap.setPointAsAvailable(startX, startY, startZ - direction[dir][2]);
			tibiaMap.setPointAsAvailable(startX, startY, startZ);
			tibiaMap.setPointType(startX, startY, startZ - direction[dir][2], setPrev);
			tibiaMap.setPointType(startX, startY, startZ, setCur);
			tibiaMap.setPointSpeed(startX, startY, startZ - direction[dir][2], 130);//130 default( is >255/2 and <70*2)
			tibiaMap.setPointSpeed(startX, startY, startZ, 130);
		}
		else
		{
			y = startY + radius * direction[dir][1];
			//explore one row of map in direction of travel
			for (x = startX - radius; x <= startX + radius && direction[dir][1]; x++)
			{
				if (!tibiaMap.isPointAvailableNoProh(x, y, startZ))
				{
					CTibiaMiniMapPoint* mp = CTAMiniMap::getTAMiniMap().getMiniMapPoint(x, y, startZ);
					if (mp->speed != 255 && mp->colour != 0)
					{
						tibiaMap.setPointAsAvailable(x, y, startZ);//is not blocking nor unexplored
						tibiaMap.setPointSpeed(x, y, startZ, mp->speed);
					}
					delete mp;
				}
			}

			x = startX + radius * direction[dir][0];
			//explore one column of map in direction of travel
			for (y = startY - radius; y <= startY + radius && direction[dir][0]; y++)
			{
				if (!tibiaMap.isPointAvailableNoProh(x, y, startZ))
				{
					CTibiaMiniMapPoint * mp = CTAMiniMap::getTAMiniMap().getMiniMapPoint(x, y, startZ);
					if (mp->speed != 255 && mp->colour != 0)
					{
						tibiaMap.setPointAsAvailable(x, y, startZ);//is not blocking nor unexplored
						tibiaMap.setPointSpeed(x, y, startZ, mp->speed);
					}
					delete mp;
				}
			}
		}
	}
	delete myData;
	delete path;
	if (sz == 0)
		ExitThread(1);   //1=did not find path
	else
		ExitThread(NULL);
}

void CConfigDialog::OnAutoResearch()
{
	CMemReader& reader = CMemReader::getMemReader();
	CTibiaCharacter ch;
	reader.readSelfCharacter(& ch);
	int curX  = ch.x, curY = ch.y, curZ = ch.z;
	int nextX = 0, nextY = 0, nextZ = 0;
	if (!reader.isLoggedIn())
	{
		curX  = 32333;
		curY  = 31782;
		curZ  = 7;
		nextX = 32333;
		nextY = 32222;
		nextZ = 6;
		PathfindParams* myData = new PathfindParams(curX, curY, curZ, nextX, nextY, nextZ);
		pathfindPoints.push(myData);
	}

	char buf[128];
	if (!m_waypointList.GetCount())
		return;
	for (int i = 0; i < m_waypointList.GetCount() + (m_waypointList.GetCount() > 2); i++)//goes around waypoints in a circle+1 path from current point(no circle if only 2 points)
	{
		m_waypointList.GetText(i % m_waypointList.GetCount(), buf);
		sscanf(buf, "(%d,%d,%d)", &nextX, &nextY, &nextZ);
		if (nextX == curX && nextY == curY && nextZ == curZ)
			continue;                                   // is a delay waypoint
		PathfindParams* myData = new PathfindParams(curX, curY, curZ, nextX, nextY, nextZ);
		if (!(curX == nextX && nextY == curY && curZ == nextZ))
			pathfindPoints.push(myData);
		curX = nextX;
		curY = nextY;
		curZ = nextZ;
	}
}

void CConfigDialog::OnDropLootList()
{
	CDropLootDialog *dialog = new CDropLootDialog(virDropList, virDropListCount, virDropWhenCapacityLimitReached, virDropOnlyLooted);
	dialog->DoModal();
	delete dialog;
}

void CConfigDialog::OnDropLoot()
{
	m_dropLootList.EnableWindow(m_dropNotLooted.GetCheck());
}


void CConfigDialog::OnAutoaddItemList()
{
	m_depositLootedItemList.SetCheck(false);
	m_depotEntryList.SetRedraw(false);
	char buf[128];
	for (int i = 0; i < CTibiaItem::getLootItemCount(); i++)
	{
		char* itemName = CTibiaItem::getLootItemNameAtIndex(i);
		int index = m_depotItemList.FindStringExact(-1, itemName);
		if (index != -1)
		{
			sprintf(buf, "%s %d->%d", itemName, 999999, 0);
			m_depotEntryList.SetCurSel(m_depotEntryList.AddString(buf));
			m_depotItemList.DeleteString(index);
			m_depotItemList.SetCurSel(0);
		}
	}
	m_depotEntryList.SetRedraw(true);
}


void CConfigDialog::OnDepotClearEntrylist()
{
	char itemName[128];
	char paramString[128];
	for (int ind = 0; ind < m_depotEntryList.GetCount(); ind++)
	{
		m_depotEntryList.GetText(ind, paramString);
		int i;
		for (i = strlen(paramString) - 1; i > 0; i--)
		{
			if (paramString[i] == ' ')
			{
				memcpy(itemName, paramString, i);
				itemName[i] = '\0';
				break;
			}
		}
		m_depotItemList.AddString(itemName);
	}
	m_depotEntryList.ResetContent();
	m_depotEntryList.SetCurSel(-1);
}
