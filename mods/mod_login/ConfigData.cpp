// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_login.h"
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
	manaBelow=0;
	hpBelow=0;
	drinkMana=0;
	drinkHp=0;
	sleep=1000;
	hotkeyHp=0;
	hotkeyMana=0;
	dropEmpty=1;
}

CConfigData::~CConfigData()
{

}
