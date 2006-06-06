// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_spellcaster.h"
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
	mana=0;
	manaMana=0;
	manaSpell[0]=0;
	life=0;
	lifeHp=0;
	lifeSpell[0]=0;
	lifeSpellMana;
	summon=0;
	summonLessThan=0;
	summonName[0]=0;
	summonMana=0;
}

CConfigData::~CConfigData()
{

}
