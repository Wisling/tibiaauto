// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_cavebot.h"
#include "ConfigData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigData::CConfigData()
{	
	int i;
	for (i=0;i<1000;i++)
	{
		waypointList[i].x=0;
		monsterList[i][0]=0;
		ignoreList[i][0]=0;
	}
	for (i=0;i<100;i++)
	{
		depotTrigger[i].itemName[0]=0;
	}

	monsterCount=0;
	ignoreCount=0;
	autoFollow=1;
	lootFood=0;
	lootGp=0;
	lootWorms=0;
	mode=0;
	suspendOnEnemy=1;	
	suspendOnNoMove=1;
	waypointSelectMode=0;
	mapUsed=0;
	capacityLimit=50;
	lootCustom=0;
	lootInBags=1;
	attackRange=5;
	stickToMonster=0;
	eatFromCorpse=0;
	unreachableAfter=30;
	suspendAfterUnreachable=15;
	attackOnlyAttacking=1;
	standStill=2;
	forceAttackAfterAttack=1;
	gatherLootStats=1;
	debug=1;
	weaponTrain=0;
	weaponFight=0;
	fightWhenSurrounded=1;
	fightWhenAlien=1;
	bloodHit=1;
	trainingActivate=0;
	dropNotLooted=0;
	lootFromFloor=0;
	attackHpAbove=0;
	backattackRunes=0;
	shareAlienBackattack=0;

}

CConfigData::~CConfigData()
{

}
