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
	customSpell=0;
	lifeHp=0;
	lifeSpell[0]=0;
	lifeSpellMana=0;
	exuraSpell=0;
	exuraHp=0;
	exuraSpellMana=0;
	granSpell=0;
	granHp=0;
	granSpellMana=0;
	vitaSpell=0;
	vitaHp=0;
	vitaSpellMana=0;
	paralysisSpell=0;
	poisonSpell=0;
	minPoisonDmg=0;

	summon=0;
	summonLessThan=0;
	summonName[0]=0;
	summonMana=0;
	
	strike=0;
	flam=0;
	frigo=0;
	mort=0;
	tera=0;
	vis=0;
	manaStrike=0;
	defaultStrikeSpell[0]=0;
	strikeSpellHpMin=0;

}

CConfigData::~CConfigData()
{

}
