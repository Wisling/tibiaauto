// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_fluid.h"
#include "ConfigData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigData::CConfigData()
{
	manaBelow        = 0;
	manaBelowN       = 0;
	manaBelowS       = 0;
	manaBelowG       = 0;
	hpBelowH         = 0;
	hpBelow          = 0;
	hpBelowN         = 0;
	hpBelowS         = 0;
	hpBelowG         = 0;
	hpBelowU         = 0;
	drinkMana        = 0;
	drinkManaN       = 0;
	drinkManaS       = 0;
	drinkManaG       = 0;
	drinkHpH         = 0;
	drinkHp          = 0;
	drinkHpN         = 0;
	drinkHpS         = 0;
	drinkHpG         = 0;
	drinkHpU         = 0;
	sleep            = 1000;
	dropEmpty        = 1;
	customItem1Below = 0;
	customItem1Item  = 0;
	customItem1Use   = 0;
	customItem2Below = 0;
	customItem2Item  = 0;
	customItem2Use   = 0;
	randomCast       = 1;
	useHotkey        = 1;
}

CConfigData::~CConfigData()
{
}
