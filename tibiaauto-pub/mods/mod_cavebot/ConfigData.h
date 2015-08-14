// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaMapPoint.h"
#define MAX_WAYPOINTCOUNT 1000
#define MAX_IGNORECOUNT 1000
#define MAX_MONSTERLISTCOUNT 1000
#define MAX_DEPOTTRIGGERCOUNT 10000
#define MAX_DROPLISTCOUNT 100

class CConfigData
{
public:
	struct depotTrigger
	{
		char itemName[64];
		int when;
		int remain;
	} depotTrigger[MAX_DEPOTTRIGGERCOUNT];

	CConfigData();
	virtual ~CConfigData();

	char monsterList[MAX_MONSTERLISTCOUNT][64];
	int monsterCount;
	char ignoreList[MAX_IGNORECOUNT][64];
	int ignoreCount;
	int autoFollow;
	int attackAllMonsters;
	int lootFood;
	int lootGp;
	int lootWorms;
	int mode;
	int suspendOnEnemy;
	point waypointList[MAX_WAYPOINTCOUNT];
	int mapUsed;
	int suspendOnNoMove;
	int waypointSelectMode;
	int capacityLimit;
	int lootCustom;
	int lootInBags;
	int lootWhileKill;
	int stickToMonster;
	int eatFromCorpse;
	int attackRange;
	int unreachableAfter;
	int suspendAfterUnreachable;
	int attackOnlyAttacking;
	int standStill;
	int forceAttackAfterAttack;
	int gatherLootStats;
	int debug;

	int weaponTrain;
	int weaponFight;
	int fightWhenSurrounded;
	int fightWhenAlien;
	int trainingMode;
	int bloodHit;
	int trainingActivate;

	int depotDropInsteadOfDeposit;
	int lootFromFloor;
	int attackHpAbove;
	int backattackRunes;

	int shareAlienBackattack;
	int depotCap;
	int dontAttackPlayers;
	int pausingEnable;
	int selectedWaypoint;
	int radius;

	int dropNotLooted;
	char dropList[MAX_DROPLISTCOUNT][64];
	int dropListCount;
	int dropWhenCapacityLimitReached;
	int dropOnlyLooted;

	char modPriorityStr[2];
	char depotModPriorityStr[2];
	int stopByDepot;
	int depositLootedItemList;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
