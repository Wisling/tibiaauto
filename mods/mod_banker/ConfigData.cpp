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
	for (x = 0; x < 10; x++) {
		bankerX[x] = 0;
		bankerY[x] = 0;
		bankerZ[x] = 0;
	}
	minimumGoldToBank = 1000;
}

CConfigData::~CConfigData() {
}
