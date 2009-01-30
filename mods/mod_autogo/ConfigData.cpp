// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_autogo.h"
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
	actX = actY = actZ = actDirection = 0;
	runawayX = runawayY = runawayZ = 0;

	trigger = 0;

	optionsBattleList = 0;
	optionsSign = 0;
	optionsMessage = 0;
	optionsHpBelow = 50;
	optionsHpAbove = 5000;
	optionsManaBelow = 50;
	optionsManaAbove = 5000;
	optionsSoulPointBelow = 1;
	optionsSoulPointAbove = 100;
	optionsBlank = 1;
	optionsCapacity = 1;
	optionsOutOfCustomItem = 0;
	optionsRunawayReached = 3;
	optionsHpBelowUntilRecovery=0;
	optionsManaBelowUntilRecovery=0;
	
	actionBattleListGm = 0;
	actionBattleListList = 0;
	actionBattleListPlayer = 0;
	actionBattleListMonster = 0;
	actionSign = 0;
	actionMessage = 0;
	actionMove = 0;
	actionHpLoss = 0;
	actionHpBelow = 0;
	actionHpAbove = 0;
	actionManaBelow = 0;
	actionManaAbove = 0;
	actionSoulPointBelow = 0;
	actionSoulPointAbove = 0;
	actionBlank = 0;
	actionCapacity = 0;
	actionOutOfFood = 0;
	actionOutOfSpace = 0;
	actionOutOfCustom = 0;
	actionRunawayReached = 0;
	allActions=0;

	sound = 0;


	memset(whiteList,0,3200);
	mkBlack=0;



	status			= 0;
}

CConfigData::~CConfigData()
{

}
