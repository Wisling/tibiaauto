// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_runemaker.h"
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
	mana       = 0;
	manaLimit  = 500;
	soulPoints = 5;
	maxUse     = 0;
	spell[0]   = 0;
	randomCast = 1;
	useSpear   = 0;

	makeNow = 0;
	const int DEFAULT_SIZE = 10;

	char spellWords[DEFAULT_SIZE][32] = {"exeta con", "adori mas frigo", "adevo mas hur", "adori mas flam", "adori gran mort", "adura vita", "adevo grav tera", "adana ani", "adevo mas pox", "adevo grav Vita"};
	int spellSP[DEFAULT_SIZE]         = {3, 3, 4, 3, 5, 3, 5, 3, 2, 5};
	int spellMana[DEFAULT_SIZE]       = {350, 530, 570, 530, 985, 400, 750, 1400, 520, 600};
	{
		for (int i = 0; i < 100; i++)
		{
			sprintf(listSpells[i].words, "%d", 0);
			listSpells[i].soulPoints = 5;
			listSpells[i].mana       = 0;
		}
	}
	for (int i = 0; i < DEFAULT_SIZE; i++)
	{
		sprintf(listSpells[i].words, "%s", spellWords[i]);
		listSpells[i].soulPoints = spellSP[i];
		listSpells[i].mana       = spellMana[i];
	}
}

CConfigData::~CConfigData()
{
}
