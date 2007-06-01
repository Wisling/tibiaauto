// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConfigData  
{
public:
	CConfigData();
	virtual ~CConfigData();

	int actX,actY,actZ,actDirection;	
	int runawayX,runawayY,runawayZ;

	int trigger;

	int optionsBattleList;
	int optionsSign;
	int optionsMessage;
	int optionsHpAbove;
	int optionsHpBelow;
	int optionsManaAbove;
	int optionsManaBelow;
	int optionsSoulPointBelow;
	int optionsSoulPointAbove;
	int optionsBlank;
	int optionsCapacity;
	int optionsOutOfCustomItem;
	int optionsRunawayReached;
	int optionsHpBelowUntilRecovery;
	int optionsManaBelowUntilRecovery;

	//int action;
	int actionBattleListGm;
	int actionBattleListList;
	int actionBattleListPlayer;
	int actionBattleListMonster;
	int actionSign;
	int actionMessage;
	int actionMove;
	int actionHpLoss;
	int actionHpAbove;
	int actionHpBelow;
	int actionManaAbove;
	int actionManaBelow;
	int actionSoulPointBelow;
	int actionSoulPointAbove;
	int actionBlank;
	int actionCapacity;
	int actionOutOfFood;
	int actionOutOfCustom;
	int actionOutOfSpace;
	int actionRunawayReached;

	int sound;
	int allActions;

	char whiteList[100][32];
	char* status;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
