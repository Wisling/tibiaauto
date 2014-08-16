// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_trademon.h"
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
	message[0]=0;
	tradeTalk = 0;
	channel = 1;
	channelInterval = 123;
	yell = 0;
	yellInterval = 33;
	say = 0;
	sayInterval = 2;
	tradeMon = 1;
}

CConfigData::~CConfigData()
{

}
