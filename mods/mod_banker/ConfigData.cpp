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
	for (int x = 0; x < 15; x++)
		path[x] = 0;
	targetX = 0;
	targetY = 0;
	targetZ = 0;
	for (int y = 0; y < 10; y++) {
		banker.position[y].bankerX = 0;
		banker.position[y].bankerY = 0;
		banker.position[y].bankerZ = 0;
	}
	minimumGoldToBank = 1000;
	cashOnHand = 0;
}

CConfigData::~CConfigData() {
}
