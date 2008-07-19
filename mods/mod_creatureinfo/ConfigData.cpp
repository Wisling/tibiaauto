// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_creatureinfo.h"
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
	player=1;
	lstrcpy(player1,"<voc><level> <hp>{/<maxhp>}");
	lstrcpy(player2,"{<guildrank> of }{<guildname>}{ (<guilddescription>)}");
	monster=1;
	lstrcpy(monster1,"<type> <hp>{/<maxhp>}W(<weakness>)");
	lstrcpy(monster2,"{Exp <exp>}");
	uniqueMonsterNames=1;
	self=1;
	lstrcpy(self1,"<voc><level> Hp:<hp>{/<maxhp>} Mana:<mana>/<maxmana>");
	lstrcpy(self2,"Exp:<exp> Mlvl:<magiclevel>(<magiclevel%>)");
	strcpy(selfWindow,"Tibia");
	strcpy(selfTray,"<name>");
	allFloorInfo=0;
	addRequest=0;
	collectStats=1;
	showCreaturesInArea=0;
	rangeXY=50;
	rangeZ=0;
}

CConfigData::~CConfigData()
{

}
