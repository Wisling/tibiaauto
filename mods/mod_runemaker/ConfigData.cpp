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

	for (int i=0; i<15 ; i++) {
		sprintf(listSpells[i].words,"%d",0);
		listSpells[i].soulPoints = 100;
		listSpells[i].mana = 0;
	}
}

CConfigData::~CConfigData()
{

}
