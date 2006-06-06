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
	optionsSoulPoint = 1;
	optionsBlank = 1;
	optionsCapacity = 1;
	optionsOutOf = 0;

	action = 0;
	actionBattleList = 0;
	actionSign = 0;
	actionMessage = 0;
	actionMove = 0;
	actionHpLoss = 0;
	actionHpBelow = 0;
	actionSoulPoint = 0;
	actionBlank = 0;
	actionCapacity = 0;
	actionOutOf = 0;

	sound = 0;

	memset(whiteList,0,3200);

	status			= 0;
}

CConfigData::~CConfigData()
{

}
