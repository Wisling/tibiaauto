// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_runemaker.h"
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
	manaLimit=500;
	soulPoints=100;
	useArrow=0;
	makeTwo=0;
	maxUse=0;
	spell[0]=0;
	useBackpack=0;
	randomCast=1;

	makeNow=0;

	char spellWords[9][32]={"adori mas frigo","adevo mas hur","adori mas flam","adori gran mort","adura vita","adevo grav tera","adana ani","adevo mas pox","adevo grav Vita"};
	int spellSP[9]={3,4,3,5,3,5,3,2,5};
	int spellMana[9]={530,570,530,985,400,750,1400,520,600};
	{
		for (int i=0; i<15 ; i++) {
			sprintf(listSpells[i].words,"%d",0);
			listSpells[i].soulPoints = 100;
			listSpells[i].mana = 0;
		}
	}
	for (int i=0; i<9 ; i++) {
		sprintf(listSpells[i].words,"%s",spellWords[i]);
		listSpells[i].soulPoints = spellSP[i];
		listSpells[i].mana = spellMana[i];
	}
}

CConfigData::~CConfigData()
{

}
