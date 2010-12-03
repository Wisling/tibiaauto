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
	options = OPTIONS_FLASHONALARM;
	actX = actY = actZ = actDirection = 0;
	runawayX = runawayY = runawayZ = 0;

	triggerMessage = 0;
	maintainStart = 0;
	status[0] = 0;
	memset(whiteList,0,3200);
	strcpy(modPriorityStr,"2");
}

CConfigData::~CConfigData()
{

}
