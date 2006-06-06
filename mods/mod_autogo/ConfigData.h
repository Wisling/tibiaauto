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
	int optionsHpBelow;
	int optionsSoulPoint;
	int optionsBlank;
	int optionsCapacity;
	int optionsOutOf;

	int action;
	int actionBattleList;
	int actionSign;
	int actionMessage;
	int actionMove;
	int actionHpLoss;
	int actionHpBelow;
	int actionSoulPoint;
	int actionBlank;
	int actionCapacity;
	int actionOutOf;

	int sound;

	char whiteList[100][32];
	char* status;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
