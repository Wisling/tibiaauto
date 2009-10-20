// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaMapPoint.h"

class CConfigData  
{
public:
	struct depotTrigger
	{
		char itemName[64];
		int when;
		int remain;
	} depotTrigger[100];

	CConfigData();
	virtual ~CConfigData();

	char monsterList[1000][64];
	int monsterCount;
	char ignoreList[1000][64];
	int ignoreCount;
	int autoFollow;
	int lootFood;
	int lootGp;
	int lootWorms;
	int mode;
	int suspendOnEnemy;
	point waypointList[1000];
	int mapUsed;
	int suspendOnNoMove;
	int waypointSelectMode;
	int capacityLimit;
	int lootCustom;
	int lootInBags;
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
	int weaponHand;	
	int fightWhenSurrounded;
	int fightWhenAlien;
	int trainingMode;
	int bloodHit;
	int trainingActivate;

	int depotDropInsteadOfDepositon;
	int dropNotLooted;
	int lootFromFloor;
	int attackHpAbove;
	int backattackRunes;

	int shareAlienBackattack;
	int depotCap;
	int dontAttackPlayers;
	int pausingEnable;
	int selectedWaypoint;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
