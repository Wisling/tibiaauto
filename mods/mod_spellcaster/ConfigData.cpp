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
	exuraSpellMana=20;
	granSpell=0;
	granHp=0;
	granSpellMana=70;
	vitaSpell=0;
	vitaHp=0;
	vitaSpellMana=160;
	sioSpell=0;
	sioHp=0;
	sioSpellMana=140;
	paralysisSpell=0;
	poisonSpell=0;
	minPoisonDmg=0;

	memset(healList,0,3200);


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
	con=0;
	san=0;
	hur=0;
	manaStrike=20;
	defaultStrikeSpell[0]=0;
	strikeSpellHpMin=0;

	aoe=0;
	aoeEffect=0;
	exori=0;
	exoriGran=0;
	exoriMas=0;
	exevoMasSan=0;
	exevoFlamHur=0;
	exevoFrigoHur=0;
	exevoTeraHur=0;
	exevoVisHur=0;
	exevoVisLux=0;
	exevoGranVisLux=0;
	exevoGranMasVis=0;
	exevoGranMasFlam=0;
	exevoGranMasTera=0;
	exevoGranMasFrigo=0;
}

CConfigData::~CConfigData()
{

}
