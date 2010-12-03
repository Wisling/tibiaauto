// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_banker.h"
#include "ConfigData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigData::CConfigData() {
	memset(path,0,15);
	targetX = 0;
	targetY = 0;
	targetZ = 0;

	banker.bankerName[0]='\0';
	banker.bankerX = 0;
	banker.bankerY = 0;
	banker.bankerZ = 0;

	minimumGoldToBank = 1000;
	cashOnHand = 0;
	strcpy(modPriorityStr,"2");
	changeGold=0;
	capsLimit=10;
	stopByBanker=0;
}

CConfigData::~CConfigData() {
}
