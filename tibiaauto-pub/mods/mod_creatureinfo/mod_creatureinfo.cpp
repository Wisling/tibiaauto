/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/


// mod_creatureinfo.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_creatureinfo.h"
#include <math.h>

#include "ConfigData.h"
#include "TibiaContainer.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"
#include "MemConstData.h"
#include "IPCBackPipeProxy.h"
#include "regex.h"
#include "RegexpProxy.h"
#include "Util.h"
#include <map>
#include "CreaturesReaderProxy.h"
#include "SendStats.h"

#include "modules\playerinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_STRING_LEN 65536

using namespace std;


class creatureData
{
public:
	int tm;
	int tibiaId;
	creatureData()
	{
		clear();
	}
	creatureData(int tibiaId)
	{
		clear();
		this->tibiaId=tibiaId;
	}
	void clear()
	{
		tm=time(NULL);
		tibiaId=0;
	}
	creatureData& operator=(const creatureData& p1)
	{
		tm = p1.tm;
		tibiaId = p1.tibiaId;
		return *this;
	}
};

class creatureKey
{
private:
	int tibiaId;
public:
	creatureKey()
	{
		tibiaId=0;
	};
	
	creatureKey(int tibiaId)
	{
		this->tibiaId=tibiaId;
	}
	
	bool operator()(const creatureKey p1, const creatureKey p2) const
	{
		return p1.tibiaId-p2.tibiaId != 0;
	}
};

int Monster_GetHp(char *name);
int Monster_GetExp(char *name);

/////////////////////////////////////////////////////////////////////////////
// CMod_creatureinfoApp

BEGIN_MESSAGE_MAP(CMod_creatureinfoApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_creatureinfoApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int playersCount;	//T4: First is reserved for self
player playersInfo[MAX_PLAYERS];
int monstersCount;
monster monstersInfo[MAX_MONSTERS];
mod_playerinfo *playerInfo;

int Player_Vocation2VocID(char* resVoc){
	int voc = -1;
	if (!strcmp(resVoc,"no vocation"))			voc=VOCID_NONE;
	else if (!strcmp(resVoc,"knight"))			voc=VOCID_KNIGHT;
	else if (!strcmp(resVoc,"elite knight"))	voc=VOCID_KNIGHT|VOCID_PREMIUM;
	else if (!strcmp(resVoc,"paladin"))			voc=VOCID_PALLADIN;
	else if (!strcmp(resVoc,"royal paladin"))	voc=VOCID_PALLADIN|VOCID_PREMIUM;
	else if (!strcmp(resVoc,"druid"))			voc=VOCID_DRUID;
	else if (!strcmp(resVoc,"elder druid"))		voc=VOCID_DRUID|VOCID_PREMIUM;
	else if (!strcmp(resVoc,"sorcerer"))		voc=VOCID_SORCERER;
	else if (!strcmp(resVoc,"master sorcerer"))	voc=VOCID_SORCERER|VOCID_PREMIUM;
	
	return voc;
}

int Player_VocID2Vocation(int vocId, char* vocation, int lShort){
	switch (vocId){
	case VOCID_NONE:					strcpy(vocation,(lShort?"NV":"No vocation"));break;
	case VOCID_KNIGHT:					strcpy(vocation,(lShort?"K":"Knight"));break;
	case VOCID_KNIGHT|VOCID_PREMIUM:	strcpy(vocation,(lShort?"EK":"Elite knight"));break;
	case VOCID_PALLADIN:				strcpy(vocation,(lShort?"P":"Paladin"));break;
	case VOCID_PALLADIN|VOCID_PREMIUM:	strcpy(vocation,(lShort?"RP":"Royal paladin"));break;
	case VOCID_DRUID:					strcpy(vocation,(lShort?"D":"Druid"));break;
	case VOCID_DRUID|VOCID_PREMIUM:		strcpy(vocation,(lShort?"ED":"Elder druid"));break;
	case VOCID_SORCERER:				strcpy(vocation,(lShort?"S":"Sorcerer"));break;
	case VOCID_SORCERER|VOCID_PREMIUM:	strcpy(vocation,(lShort?"MS":"Master sorcerer"));break;
	default: return 0;
	}
	return 1;
}

void Expression_PreParse(char *expression, expressionInfo *expInfo){
	char *current = expression;
	char *tagStart = 0;
	char *tagEnd   = 0;
	char *negStart = 0;
	char *mask = expInfo->mask;

	expInfo->tagCount = 0;

	while (*current){
		if (*current == '{'){
			//Are we inside tag ?
			if (!tagStart){
				negStart = current;
			}

		}else if (*current == '<'){
			tagStart = current;
			//Is there any prefix
			if (negStart){
				//T4: Copy negotietive prefix
				lstrcpyn(expInfo->tag[expInfo->tagCount].prefix,negStart+1,min(current-negStart,39));
			}

		}else if(*current == '>'){
			tagEnd	= current;
			lstrcpyn(expInfo->tag[expInfo->tagCount].name,tagStart+1,min(tagEnd-tagStart,39));
			*mask++ = '%';
			*mask++ = 's';
			//If there where prefix we must check also for suffix
			if (!negStart){
				expInfo->tagCount++;
				tagStart = 0;
				tagEnd	 = 0;
			}

		}else if(*current == '}'){
			if (negStart && tagStart && tagEnd){
				//T4: Copy negotietive suffix
				lstrcpyn(expInfo->tag[expInfo->tagCount].suffix,tagEnd+1,min(current-tagEnd,39));
				expInfo->tagCount++;
				tagStart = 0;
				tagEnd	 = 0;
				negStart = 0;
			}

		}else if(!tagStart && !negStart){
			//All other chars stay
			*mask++ = *current;

		}
		current++;
	}
}

void Expression_Tags_Internals(char* tagName, char* svalue, CTibiaCharacter *ch){
	if(!strcmpi(tagName,"tibiaid")){
		sprintf(svalue,"%d",ch->tibiaId);

	}else if(!strcmpi(tagName,"x")){
		sprintf(svalue,"%d",ch->x);

	}else if(!strcmpi(tagName,"y")){
		sprintf(svalue,"%d",ch->y);

	}else if(!strcmpi(tagName,"z")){
		sprintf(svalue,"%d",ch->z);

	}else if(!strcmpi(tagName,"outfitid")){
		sprintf(svalue,"%d",ch->outfitId);

	}else if(!strcmpi(tagName,"mountid")){
		sprintf(svalue,"%d",ch->mountId);

	}else if(!strcmpi(tagName,"moving")){
		sprintf(svalue,"%d",ch->moving);

	}else if(!strcmpi(tagName,"monstertype")){
		sprintf(svalue,"%d",ch->monsterType);

	}else if(!strcmpi(tagName,"hppercleft")){
		sprintf(svalue,"%d",ch->hpPercLeft);

	}else if(!strcmpi(tagName,"lookdir")){
		sprintf(svalue,"%d",ch->lookDirection);

	}else if(!strcmpi(tagName,"colorhead")){
		sprintf(svalue,"%d",ch->colorHead);

	}else if(!strcmpi(tagName,"colorbody")){
		sprintf(svalue,"%d",ch->colorBody);

	}else if(!strcmpi(tagName,"colorlegs")){
		sprintf(svalue,"%d",ch->colorLegs);

	}else if(!strcmpi(tagName,"colorfoot")){
		sprintf(svalue,"%d",ch->colorFoot);

	}else if(!strcmpi(tagName,"walkspeed")){
		sprintf(svalue,"%d",ch->walkSpeed);

	}else if(!strcmpi(tagName,"skulls")){
		sprintf(svalue,"%d",ch->skulls);

	}else if(!strcmpi(tagName,"shields")){
		sprintf(svalue,"%d",ch->shields);

	}else if(!strcmpi(tagName,"waricon")){
		sprintf(svalue,"%d",ch->warIcon);

	}else if(!strcmpi(tagName,"blocking")){
		sprintf(svalue,"%d",ch->blocking);

	}
}

void Creature_TypeToText(int type, char* svalue){
	if (type == TYPE_PLAYER || type == TYPE_SELF){
		lstrcpy(svalue,"Player");

	}else if (type == TYPE_MONSTER){
		lstrcpy(svalue,"Monster");

	}else if (type == TYPE_NPC){
		lstrcpy(svalue,"NPC");

	}else{
		lstrcpy(svalue,"Unknown");

	}
}

void Expression_Tags_All(char* tagName, char* svalue, creature *data, CTibiaCharacter *ch){
	if (!strcmpi(tagName,"hp")){
		if (data && data->type == TYPE_SELF){
			sprintf(svalue,"%d",ch->hp);
		}else if (data && data->maxHp){
			sprintf(svalue,"%d",MulDiv(ch->hpPercLeft,data->maxHp,100));
		}else{
			sprintf(svalue,"%d%%",ch->hpPercLeft);
		}

	}else if(!strcmpi(tagName,"maxhp")){
		if (data && data->maxHp){
			sprintf(svalue,"%d",data->maxHp);
		}else{
			lstrcpy(svalue,"");
		}
	}else if(!strcmpi(tagName,"name")){
		if (data){
			sprintf(svalue,"%s",data->name);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (data && !strcmpi(tagName,"type")){
		Creature_TypeToText(data->type,svalue);
	}else if (!strcmpi(tagName,"relz")){
		CMemReaderProxy reader;
		CTibiaCharacter *self = reader.readSelfCharacter();
		int relz = self->z - ch->z;
		if (relz!=0){
			if (relz > 0){
				sprintf(svalue,"+%d",relz);
			}else{
				sprintf(svalue,"%d",relz);
			}
		}else{
			lstrcpy(svalue,"");
		}
		delete self;
	}
}

void Expression_Tags_Player(char* tagName, char* svalue, player* data, CTibiaCharacter *ch){
	if (!strcmpi(tagName,"guildname")){
		lstrcpy(svalue,data->guildName);

	}else if (!strcmpi(tagName,"guildrank")){
		lstrcpy(svalue,data->guildRank);

	}else if (!strcmpi(tagName,"guilddescription")){
		lstrcpy(svalue,data->guildDescription);

	}else if (!strcmpi(tagName,"level")){
		sprintf(svalue,"%d",data->level);

	}else if (!strcmpi(tagName,"voc")){
		Player_VocID2Vocation(data->vocId,svalue,1);

	}else if (!strcmpi(tagName,"vocation")){
		Player_VocID2Vocation(data->vocId,svalue,0);
	}else if (!strcmpi(tagName,"speedmodif")){
		int base = 218 + 2*(data->level);
		int modif = ch->walkSpeed - base;

		int haste = floorf(float(base)*0.3-24);
		if (haste%2){
			haste--;
		}
		int shaste = floorf(float(base)*0.7-56);
		if (shaste%2){
			shaste--;
		}
		
		if (modif == 0){
			lstrcpy(svalue,"");
		}else if (modif == 40){
			lstrcpy(svalue,"BOH");
		}else if(modif == 60){
			lstrcpy(svalue,"TimeRing");
		}else if(modif == haste){
			lstrcpy(svalue,"Haste");
		}else if(modif == 100){
			lstrcpy(svalue,"BOH+TimeRing");
		}else if(modif == haste+40){
			lstrcpy(svalue,"BOH+Haste");
		}else if(modif == haste+60){
			lstrcpy(svalue,"Haste+TimeRing");
		}else if(modif == haste+100){
			lstrcpy(svalue,"BOH+Haste+TimeRing");
		}else if(modif == shaste){
			lstrcpy(svalue,"StrongHaste");
		}else if(modif == shaste+40){
			lstrcpy(svalue,"BOH+StrongHaste");
		}else if(modif == shaste+60){
			lstrcpy(svalue,"StrongHaste+TimeRing");
		}else if(modif == shaste+100){
			lstrcpy(svalue,"BOH+StrongHaste+TimeRing");
		}else{
			lstrcpy(svalue,"Unknown modif");
		}
	}
}

void Expression_Tags_Self(char* tagName, char* svalue,CConfigData *config){
	//T4: Char info
	if (!strcmpi(tagName,"mana")){
		sprintf(svalue,"%d",playerInfo->mana);

	}else if (!strcmpi(tagName,"maxmana")){
		sprintf(svalue,"%d",playerInfo->maxMana);

	}else if (!strcmpi(tagName,"voc")){
		sprintf(svalue,"%s",playerInfo->voc);
		for(int i=strlen(svalue)-1;i>=0;i--) svalue[i] = toupper(svalue[i]);
	}else if (!strcmpi(tagName,"capacity")){
		sprintf(svalue,"%d.%02d",(int)playerInfo->capacity,(int)((playerInfo->capacity-(int)playerInfo->capacity)*100));

	}else if (!strcmpi(tagName,"stamina")){
		sprintf(svalue,"%d:%02d",(int)(playerInfo->stamina/60),(int)(playerInfo->stamina%60));

	}else if (!strcmpi(tagName,"maxcapacity")){
		sprintf(svalue,"%d",playerInfo->maxCapacity);

	}else if (!strcmpi(tagName,"soulpoints")){
		sprintf(svalue,"%d",playerInfo->soulPoints);

	//T4: Char stats
	}else if (!strcmpi(tagName,"exp")){
		sprintf(svalue,"%d",playerInfo->expCurrent);

	}else if (!strcmpi(tagName,"expleft")){
		sprintf(svalue,"%d",playerInfo->expLeft);

	}else if (!strcmpi(tagName,"expperh")){
		if (playerInfo->expPerHour){
			sprintf(svalue,"%d",playerInfo->expPerHour);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"level")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_LEVEL].lvl);

	}else if (!strcmpi(tagName,"level%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_LEVEL].percLeft);

	}else if (!strcmpi(tagName,"levelperh")){
		if (playerInfo->skill[SKILL_LEVEL].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_LEVEL].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}
	}else if (!strcmpi(tagName,"levelupin")){
		if (playerInfo->skill[SKILL_LEVEL].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_LEVEL].percLeft)/float(playerInfo->skill[SKILL_LEVEL].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	}else if (!strcmpi(tagName,"magiclevel")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_MAGICLEVEL].lvl);

	}else if (!strcmpi(tagName,"magiclevel%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_MAGICLEVEL].percLeft);

	}else if (!strcmpi(tagName,"magiclevelperh")){
		if (playerInfo->skill[SKILL_MAGICLEVEL].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_MAGICLEVEL].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}
	}else if (!strcmpi(tagName,"magiclevelupin")){
		if (playerInfo->skill[SKILL_MAGICLEVEL].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_MAGICLEVEL].percLeft)/float(playerInfo->skill[SKILL_MAGICLEVEL].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	//T4: Skills stats
	}else if (!strcmpi(tagName,"skillsword")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_SWORD].lvl);

	}else if (!strcmpi(tagName,"skillaxe")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_AXE].lvl);

	}else if (!strcmpi(tagName,"skillclub")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_CLUB].lvl);

	}else if (!strcmpi(tagName,"skillshield")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_SHIELD].lvl);

	}else if (!strcmpi(tagName,"skilldistance")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_DIST].lvl);

	}else if (!strcmpi(tagName,"skillfishing")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_FISH].lvl);

	}else if (!strcmpi(tagName,"skillfist")){
		sprintf(svalue,"%d",playerInfo->skill[SKILL_FIST].lvl);

	}else if (!strcmpi(tagName,"skillsword%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_SWORD].percLeft);

	}else if (!strcmpi(tagName,"skillswordperh")){
		if (playerInfo->skill[SKILL_SWORD].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_SWORD].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"skillswordupin")){
		if (playerInfo->skill[SKILL_SWORD].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_SWORD].percLeft)/float(playerInfo->skill[SKILL_SWORD].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	}else if (!strcmpi(tagName,"skillaxe%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_AXE].percLeft);

	}else if (!strcmpi(tagName,"skillaxeperh")){
		if (playerInfo->skill[SKILL_AXE].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_AXE].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"skillaxeupin")){
		if (playerInfo->skill[SKILL_AXE].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_AXE].percLeft)/float(playerInfo->skill[SKILL_AXE].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	}else if (!strcmpi(tagName,"skillclub%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_CLUB].percLeft);

	}else if (!strcmpi(tagName,"skillclubperh")){
		if (playerInfo->skill[SKILL_CLUB].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_CLUB].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"skillclubupin")){
		if (playerInfo->skill[SKILL_CLUB].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_CLUB].percLeft)/float(playerInfo->skill[SKILL_CLUB].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	}else if (!strcmpi(tagName,"skillshield%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_SHIELD].percLeft);

	}else if (!strcmpi(tagName,"skillshieldperh")){
		if (playerInfo->skill[SKILL_SHIELD].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_SHIELD].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"skillshieldupin")){
		if (playerInfo->skill[SKILL_SHIELD].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_SHIELD].percLeft)/float(playerInfo->skill[SKILL_SHIELD].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	}else if (!strcmpi(tagName,"skilldistance%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_DIST].percLeft);

	}else if (!strcmpi(tagName,"skilldistanceperh")){
		if (playerInfo->skill[SKILL_DIST].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_DIST].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"skilldistanceupin")){
		if (playerInfo->skill[SKILL_DIST].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_DIST].percLeft)/float(playerInfo->skill[SKILL_DIST].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	}else if (!strcmpi(tagName,"skillfishing%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_FISH].percLeft);

	}else if (!strcmpi(tagName,"skillfishingperh")){
		if (playerInfo->skill[SKILL_FISH].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_FISH].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"skillfishingupin")){
		if (playerInfo->skill[SKILL_FISH].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_FISH].percLeft)/float(playerInfo->skill[SKILL_FISH].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	}else if (!strcmpi(tagName,"skillfist%")){
		sprintf(svalue,"%d%%",playerInfo->skill[SKILL_FIST].percLeft);

	}else if (!strcmpi(tagName,"skillfistperh")){
		if (playerInfo->skill[SKILL_FIST].percPerHour > 0.001){
			sprintf(svalue,"%.02f%%/h",playerInfo->skill[SKILL_FIST].percPerHour);
		}else{
			lstrcpy(svalue,"");
		}
	}else if (!strcmpi(tagName,"skillfistupin")){
		if (playerInfo->skill[SKILL_FIST].percPerHour > 0.001){
			float fHour = float(float(playerInfo->skill[SKILL_FIST].percLeft)/float(playerInfo->skill[SKILL_FIST].percPerHour));
			unsigned long hour = long(floor(fHour));
			unsigned long min = long(60*(fHour-hour));

			sprintf(svalue,"%dh%dmin",hour,min);
		}else{
			sprintf(svalue,"");

		}

	//T4: Spell Timers
	}else if (!strcmpi(tagName,"timehaste")){
		if (playerInfo->spell[SPELL_HASTE].remaining > 0){
			sprintf(svalue,"%d",playerInfo->spell[SPELL_HASTE].remaining);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"timestronghaste")){
		if (playerInfo->spell[SPELL_STRONGHASTE].remaining > 0){
			sprintf(svalue,"%d",playerInfo->spell[SPELL_STRONGHASTE].remaining);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"timeinvisible")){
		if (playerInfo->spell[SPELL_INVISIBLE].remaining > 0){
			sprintf(svalue,"%d",playerInfo->spell[SPELL_INVISIBLE].remaining);
		}else{
			lstrcpy(svalue,"");
		}

	}else if (!strcmpi(tagName,"timeshield")){
		if (playerInfo->spell[SPELL_SHIELD].remaining > 0){
			sprintf(svalue,"%d",playerInfo->spell[SPELL_SHIELD].remaining);
		}else{
			lstrcpy(svalue,"");
		}
	//V: creature statisticsx
	} else if (!strcmpi(tagName,"crstatMostHp")) {
		
		CCreaturesReaderProxy cReader;
		CMemReaderProxy reader;
		CTibiaCharacter *self = reader.readSelfCharacter();
				
		char crStatCreature[128];
		int crStatMaxValue=0;
		sprintf(crStatCreature,"<none>");
		
		
		char **crList = cReader.findCreatureStatInArea(self->x,self->y,self->z,config->rangeXY,config->rangeZ);
		
		int pos;
		for (pos=0;crList[pos];pos++)
		{
		
			int foundValue=Monster_GetHp(crList[pos]);
			if (foundValue>crStatMaxValue)
			{
				crStatMaxValue=foundValue;
				strcpy(crStatCreature,crList[pos]);
			}
		
			free(crList[pos]);
		
		}
		
		
		free(crList);
		
		lstrcpy(svalue,crStatCreature);
		
		
		delete self;
		
	} else if (!strcmpi(tagName,"crstatMostExp")) {
		
		CCreaturesReaderProxy cReader;
		CMemReaderProxy reader;
		CTibiaCharacter *self = reader.readSelfCharacter();
				
		//int x,y;
		char crStatCreature[128];
		int crStatMaxValue=0;
		sprintf(crStatCreature,"<none>");
		
		
		char **crList = cReader.findCreatureStatInArea(self->x,self->y,self->z,config->rangeXY,config->rangeZ);
		
		int pos;
		for (pos=0;crList[pos];pos++)
		{
		
			int foundValue=Monster_GetExp(crList[pos]);
			if (foundValue>crStatMaxValue)
			{
				crStatMaxValue=foundValue;
				strcpy(crStatCreature,crList[pos]);
			}
		
			free(crList[pos]);
		
		}
		
		
		free(crList);
		
		lstrcpy(svalue,crStatCreature);
		
		
		delete self;
		
	}
	
}

void Expression_Tags_Monster(char* tagName, char* svalue, monster* data){
	if (!strcmpi(tagName,"exp")){
		if (data->exp>0){
			sprintf(svalue,"%d",data->exp);
		}else{
			strcpy(svalue,"");
		}
	}else if (!strcmpi(tagName,"maxHp")){
		if (data->maxHp>0){
			sprintf(svalue,"%d",data->maxHp);
		}else{
			strcpy(svalue,"");
		}
	}else if (!strcmpi(tagName,"weakness") || !strcmpi(tagName,"strength") || !strcmpi(tagName,"immunity") || !strcmpi(tagName,"noneffect")){
		int amt;
		if (!strcmpi(tagName,"weakness")){
			amt = 5;
		}else if (!strcmpi(tagName,"strength")){
			amt = 1;
		}else if (!strcmpi(tagName,"immunity")){
			amt = 0;
		}else if (!strcmpi(tagName,"noneffect")){
			amt = 2;
		}
		if (data->physical==amt)
			sprintf(svalue,"Ph,");
		if (data->fire==amt)
			sprintf(svalue,"Fr,");
		if (data->ice==amt)
			sprintf(svalue,"Ic,");
		if (data->earth==amt)
			sprintf(svalue,"Ea,");
		if (data->energy==amt)
			sprintf(svalue,"En,");
		if (data->holy==amt)
			sprintf(svalue,"Hl,");
		if (data->death==amt)
			sprintf(svalue,"Dt,");
		svalue[strlen(svalue)-1] = '\0';
	}else if (!strcmpi(tagName,"description")){
		if (lstrlen(data->description)){
			sprintf(svalue,"%s",data->description);
		}else{
			strcpy(svalue,"");
		}

	}
}



void Expression_GatherData(expressionInfo *expInfo, creature *data,CTibiaCharacter *ch,CConfigData *config){
	char svalue[128];
	int i;

	//T4: Here are made all transforamtion tag <-> value

	for (i=0;i<expInfo->tagCount;i++){
		svalue[0]=0;
		Expression_Tags_All(expInfo->tag[i].name,svalue,data,ch);
		Expression_Tags_Internals(expInfo->tag[i].name,svalue,ch);
		if (data){
			if (data->type == TYPE_PLAYER || data->type == TYPE_SELF){
				Expression_Tags_Player(expInfo->tag[i].name,svalue,(player*)data,ch);
				if (data->type == TYPE_SELF){
					Expression_Tags_Self(expInfo->tag[i].name,svalue,config);
				}
			}else if (data->type == TYPE_MONSTER || data->type == TYPE_NPC){
				Expression_Tags_Monster(expInfo->tag[i].name,svalue,(monster*)data);
			}
		}
		if (svalue[0]){
			sprintf(expInfo->tag[i].value,"%s%s%s",expInfo->tag[i].prefix,svalue,expInfo->tag[i].suffix);
		}else{
			lstrcpy(expInfo->tag[i].value,"");
		}
	}
}

void Expression_Exec(expressionInfo *expInfo,char *out){
	sprintf(out,expInfo->mask,expInfo->tag[0].value,expInfo->tag[1].value,expInfo->tag[2].value,expInfo->tag[3].value,expInfo->tag[4].value,expInfo->tag[5].value,expInfo->tag[6].value,expInfo->tag[7].value,expInfo->tag[8].value,expInfo->tag[9].value);
}

int Player_CalcHp(int vocId, int lvl){
	if (vocId&VOCID_KNIGHT)	//T4: Kinght
		return 65+lvl*15;
	if (vocId&VOCID_PALLADIN) //T4: Palladin
		return 105+lvl*10;
	//T4: Druid,Sorcerer or no vacation
	return 145+lvl*5;
}

void CreatureList_Init(){
	
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);

	char pathBuf[2048];
	
	
	
    XMLPlatformUtils::Initialize();
    XercesDOMParser *parser = new XercesDOMParser();
    try{
        int itemNr,rootNr;

        monstersCount=0;
               
		sprintf(pathBuf,"%s\\mods\\tibiaauto-creatures.xml",installPath);
        parser->parse(pathBuf);
       
        DOMNode  *doc = parser->getDocument();
        for (rootNr=0;rootNr<doc->getChildNodes()->getLength();rootNr++)
        {
            DOMNode *root = doc->getChildNodes()->item(rootNr);
               
            if (wcscmp(root->getNodeName(),L"creatures-definitions"))
                continue;
            for (itemNr=0;itemNr<root->getChildNodes()->getLength();itemNr++)
            {
                int attrNr;
                DOMNode *item = root->getChildNodes()->item(itemNr);
                if (wcscmp(item->getNodeName(),L"creature"))
                    continue;
               
                int creatureHp=0;
				int creatureExp=0;
				int creatureType=0;
				int creaturePhysical=2;
				int creatureEarth=2;
				int creatureFire=2;
				int creatureEnergy=2;
				int creatureIce=2;
				int creatureHoly=2;
				int creatureDeath=2;
				int creatureSeeInvisible=0;
				int creatureParalysisImmune=0;
                char *objectName=NULL;
				char *objectDescription=NULL;
               
                for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
                {
                    DOMNode *attrNode = item->getAttributes()->item(attrNr);
                    if (!wcscmp(attrNode->getNodeName(),L"name")){
						objectName=CUtil::wc2c(attrNode->getNodeValue());

					}else if (!wcscmp(attrNode->getNodeName(),L"hp")){

						char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureHp = atoi(idTmp);
                        free(idTmp);

                    }else if (!wcscmp(attrNode->getNodeName(),L"exp")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureExp = atoi(idTmp);
                        free(idTmp);

                    }else if (!wcscmp(attrNode->getNodeName(),L"type")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureType = atoi(idTmp);
                        free(idTmp);
					}else if (!wcscmp(attrNode->getNodeName(),L"physical")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creaturePhysical = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"earth")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureEarth = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"fire")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureFire = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"energy")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureEnergy = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"ice")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureIce = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"holy")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureHoly = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"death")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureDeath = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"see_invisible")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureSeeInvisible = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"paralysis_immune")){
                        char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
                        creatureParalysisImmune = atoi(idTmp);
                        free(idTmp);
                    }else if (!wcscmp(attrNode->getNodeName(),L"description")){
						objectDescription=CUtil::wc2c(attrNode->getNodeValue());

					}
                }
                if (!objectName||!strlen(objectName)){
					if (objectDescription)
						free(objectDescription);
					if (objectName)
						free(objectName);
                    continue;
				}
   
				lstrcpyn(monstersInfo[monstersCount].name,objectName,31);
				monstersInfo[monstersCount].type		= (creatureType==TYPE_UNKNOWN?TYPE_MONSTER:creatureType);
                monstersInfo[monstersCount].maxHp		= creatureHp;
				monstersInfo[monstersCount].exp			= creatureExp;
				monstersInfo[monstersCount].physical	= creaturePhysical;
				monstersInfo[monstersCount].earth		= creatureEarth;
				monstersInfo[monstersCount].fire		= creatureFire;
				monstersInfo[monstersCount].energy		= creatureEnergy;
				monstersInfo[monstersCount].ice			= creatureIce;
				monstersInfo[monstersCount].holy		= creatureHoly;
				monstersInfo[monstersCount].death		= creatureDeath;
				monstersInfo[monstersCount].seeInvisible	= creatureSeeInvisible;
				monstersInfo[monstersCount].paralysisImmune	= creatureParalysisImmune;
				if (objectDescription){
					lstrcpyn(monstersInfo[monstersCount].description,objectDescription,MAX_LINE_LEN);
				}
                monstersCount++;

				free(objectName);
            }
        }
    } catch (...)
    {
        AfxMessageBox("Unable to load creatures definitions!");
    }

    delete parser;
}

int Monster_GetHp(char *name){
	for(int i=0;i<monstersCount;i++){
		if(!strcmpi(monstersInfo[i].name,name)){
			return monstersInfo[i].maxHp;
		}
	}
	return 0;
}

int Monster_GetExp(char *name){
	for(int i=0;i<monstersCount;i++){
		if(!strcmpi(monstersInfo[i].name,name)){
			return monstersInfo[i].exp;
		}
	}
	return 0;
}

void Player_Register(char *name,int vocId,int lvl,char *guildName,char *guildRank,char *guildDescription){
	int i;
	for (i=0;i<playersCount;i++){
		if (!strcmpi(playersInfo[i].name,name)){
			//T4: Player found in known list - so just refresh
			playersInfo[i].vocId = vocId;
			playersInfo[i].level = lvl;
			playersInfo[i].maxHp = Player_CalcHp(vocId,lvl);
			lstrcpyn(playersInfo[i].guildName,guildName,31);
			lstrcpyn(playersInfo[i].guildRank,guildRank,31);
			lstrcpyn(playersInfo[i].guildDescription,guildDescription,31);
			return;
		}
	}
	//T4: Player not found - so add it
	if (playersCount<MAX_PLAYERS){
		lstrcpyn(playersInfo[playersCount].name,name,32);
		playersInfo[playersCount].type	= TYPE_PLAYER;
		playersInfo[playersCount].maxHp = Player_CalcHp(vocId,lvl);
		playersInfo[playersCount].vocId = vocId;
		playersInfo[playersCount].level = lvl;
		lstrcpyn(playersInfo[playersCount].guildName,guildName,31);
		lstrcpyn(playersInfo[playersCount].guildRank,guildRank,31);
		lstrcpyn(playersInfo[playersCount].guildDescription,guildDescription,31);
		playersCount++;
	}
}

void Monster_Register(char *name,int type,int maxHp, int exp, int physical, int earth, int fire, int energy, int ice, int holy, int death, int seeInvisible, int paralysisImmune, char* description){
	//T4 This is called monster but lot of other objects info will be added here
	int i;
	for (i=0;i<monstersCount;i++){
		if (!strcmpi(monstersInfo[i].name,name)){
			//T4: Monster found - do nothing
			return;
		}
	}
	//T4: Monster not found - this should happen always
	if (monstersCount<MAX_MONSTERS){
		lstrcpyn(monstersInfo[monstersCount].name,name,32);
		monstersInfo[monstersCount].type		= type;
		monstersInfo[monstersCount].maxHp		= maxHp;
		monstersInfo[monstersCount].exp			= exp;
		monstersInfo[monstersCount].physical	= physical;
		monstersInfo[monstersCount].earth		= earth;
		monstersInfo[monstersCount].fire		= fire;
		monstersInfo[monstersCount].energy		= energy;
		monstersInfo[monstersCount].ice			= ice;
		monstersInfo[monstersCount].holy		= holy;
		monstersInfo[monstersCount].death		= death;
		monstersInfo[monstersCount].seeInvisible	= seeInvisible;
		monstersInfo[monstersCount].paralysisImmune	= paralysisImmune;
		lstrcpyn(monstersInfo[monstersCount].description,(description?description:""),MAX_LINE_LEN);
		monstersCount++;
	}
}

void ReadPipeInfo(){
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CRegexpProxy regexpProxy;
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;

	while (backPipe.readFromPipe(&mess,1002)){
		int len;
		char msgBuf[16384];
				
		memset(msgBuf,0,16384);
		memcpy(&len,mess.payload,sizeof(int));
		memcpy(msgBuf,mess.payload+4,len);
		
		// parse 'see' info'
		regex_t preg;
		regmatch_t pmatch[20];

		//You see Test (Level 24). He has no vocation.
		//You see Test (Level 30). He is a master sorcerer. He is Member of Guild.
		//You see Test (Level 37). He is a master sorcerer. He is Member of Guild (Description).
		//You see yourself. You are a sorcerer. You are Member of the Guild (Description).
		if (!regexpProxy.regcomp(&preg,"You see ([a-z' -]+)..Level ([0-9]+).. (he|she) (is an?|has) (knight|paladin|druid|sorcerer|elder druid|elite knight|master sorcerer|royal paladin|no vocation). *(s?he is ([a-z' -]+) of the ([a-z' -]+).(.([a-z '-]+)..)?)?",REG_EXTENDED|REG_ICASE)){
			if (!regexpProxy.regexec(&preg,msgBuf,20,pmatch,0)){
				//T4: This is a player
				char resNick[128];
				char resLvl[128];
				char resVoc[128];
				char resGuildName[128];
				char resGuildRank[128];
				char resGuildDescription[128];
				resNick[0]=resLvl[0]=resVoc[0]=resGuildName[0]=resGuildRank[0]=resGuildDescription[0]=0;

				lstrcpyn(resLvl,msgBuf+pmatch[2].rm_so,min(127,pmatch[2].rm_eo-pmatch[2].rm_so+1));
				lstrcpyn(resNick,msgBuf+pmatch[1].rm_so,min(127,pmatch[1].rm_eo-pmatch[1].rm_so+1));
				lstrcpyn(resVoc,msgBuf+pmatch[5].rm_so,min(127,pmatch[5].rm_eo-pmatch[5].rm_so+1));
				
				if (pmatch[8].rm_so>0){
					//T4: This player is a member of guild
					lstrcpyn(resGuildRank,msgBuf+pmatch[7].rm_so,min(127,pmatch[7].rm_eo-pmatch[7].rm_so+1));
					lstrcpyn(resGuildName,msgBuf+pmatch[8].rm_so,min(127,pmatch[8].rm_eo-pmatch[8].rm_so+1));
					if (pmatch[10].rm_so>0){
						//T4: Also have description
						lstrcpyn(resGuildDescription,msgBuf+pmatch[10].rm_so,min(127,pmatch[10].rm_eo-pmatch[10].rm_so+1));
					}
				}
								
				int voc=Player_Vocation2VocID(resVoc);
				int lvl=atoi(resLvl);
				
				Player_Register(resNick,voc,lvl,resGuildName,resGuildRank,resGuildDescription);

				//sprintf(resNick,"Registered Players: %d",playersCount);
				//sender.sendTAMessage(resNick);

			}else{
				regex_t preg3;
				if (!regexpProxy.regcomp(&preg3,"You see yourself. You (are an?|have) (knight|paladin|druid|sorcerer|elder druid|elite knight|master sorcerer|royal paladin|no vocation).*( You are ([a-z' -]+) of the ([a-z' -]+).(.([a-z '-]+)..)?)?",REG_EXTENDED|REG_ICASE)){
					if (!regexpProxy.regexec(&preg3,msgBuf,20,pmatch,0)){
						//T4: This is self
						char resVoc[128];
						char resGuildName[128];
						char resGuildRank[128];
						char resGuildDescription[128];
						resVoc[0]=resGuildName[0]=resGuildRank[0]=resGuildDescription[0]=0;

						lstrcpyn(resVoc,msgBuf+pmatch[2].rm_so,min(127,pmatch[2].rm_eo-pmatch[2].rm_so+1));
						
						if (pmatch[3].rm_so>0){
							//T4: This player is a member of guild
							lstrcpyn(resGuildRank,msgBuf+pmatch[4].rm_so,min(127,pmatch[4].rm_eo-pmatch[4].rm_so+1));
							lstrcpyn(resGuildName,msgBuf+pmatch[5].rm_so,min(127,pmatch[5].rm_eo-pmatch[5].rm_so+1));
							if (pmatch[6].rm_so>0){
								//T4: Also have description
								lstrcpyn(resGuildDescription,msgBuf+pmatch[7].rm_so,min(127,pmatch[7].rm_eo-pmatch[7].rm_so+1));
							}
						}

						CTibiaCharacter *self = reader.readSelfCharacter();
						CTibiaCharacter *selfName = reader.readVisibleCreature(reader.getLoggedCharNr());

						lstrcpyn(playersInfo[0].name,selfName->name,32);
						playersInfo[0].type	= TYPE_SELF;
						playersInfo[0].maxHp = self->maxHp;
						playersInfo[0].vocId = Player_Vocation2VocID(resVoc);
						playersInfo[0].level = self->lvl;
						lstrcpyn(playersInfo[0].guildName,resGuildName,31);
						lstrcpyn(playersInfo[0].guildRank,resGuildRank,31);
						lstrcpyn(playersInfo[0].guildDescription,resGuildDescription,31);

						delete self;
						delete selfName;
					}else{
						// T4: No match, this is non-player info text
						regex_t preg2;
						if (!regexpProxy.regcomp(&preg2,"You see (an? |)([a-z '-,.]+).$",REG_EXTENDED|REG_ICASE)){
							if (!regexpProxy.regexec(&preg2,msgBuf,20,pmatch,0)){
								char resNick[128];
								lstrcpyn(resNick,msgBuf+pmatch[2].rm_so,min(127,pmatch[2].rm_eo-pmatch[2].rm_so+1));
								if (pmatch[1].rm_eo!=pmatch[1].rm_so){
									Monster_Register
										(resNick,TYPE_MONSTER,0,0,2,2,2,2,2,2,2,0,0,"Auto Added");
								}else{
									Monster_Register(resNick,TYPE_NPC,0,0,2,2,2,2,2,2,2,0,0,"Auto Added");
								}
								//sprintf(resNick,"Registered Monsters: %d",monstersCount);
								//sender.sendTAMessage(resNick);
							}
							regexpProxy.regfree(&preg2);
						}
					}
					regexpProxy.regfree(&preg3);
				}
			}
			regexpProxy.regfree(&preg);
		}
		
	}
}

int PlayerInfo_Init(){
	GPIPROC GetPlayerInfo;
	HMODULE hPlayerInfo;

	hPlayerInfo = GetModuleHandle("mod_playerinfo.dll");
	if (hPlayerInfo){
		GetPlayerInfo = (GPIPROC)GetProcAddress(hPlayerInfo,"GetPlayerInfo");
		if (GetPlayerInfo){
			playerInfo = (mod_playerinfo*)GetPlayerInfo();
			return (!IsBadReadPtr(playerInfo,sizeof(mod_playerinfo)));
		}
	}
	return 0;
}

void Monster_GetName(char *sCreature,char *out){
	int i;
	//T4: Monster name in BattleList have brakets at the end we have to remove them
	for (i=0;i<32;i++){
		if (sCreature[i]==0||sCreature[i]=='['){
			break;
		}
		out[i] = sCreature[i];
	}
	if (i<32){
		out[i] = 0;
	}
}
void Monster_SetNameNumber(char *sCreature,int nr, char *out){
	char buffer[32];
	lstrcpyn(buffer,sCreature,26); //T4: 26, cus 5 chars are neeede for number and 1 char for null terminator
	sprintf(out,"%s[%02d]",buffer,nr);
	out[32]=0;
}

int RandomTimeCreatureInfo(CTibiaCharacter *self,CTibiaCharacter *ch){
	int dist = self->x-ch->x + self->y-ch->y;
	return CModuleUtil::randomFormula(300,(7-dist)*30);//ranges from 220-510
}

map <int,int> lookCount;
int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	//T4: Expression structures
	expressionInfo playerLine1;
	expressionInfo playerLine2;
	expressionInfo monsterLine1;
	expressionInfo monsterLine2;
	expressionInfo selfLine1;
	expressionInfo selfLine2;
	expressionInfo selfLineWindow;
	expressionInfo selfLineTray;

	//T4: Init expression parsing, split expression in to text and tags
	if (config->player){
		Expression_PreParse(config->player1,&playerLine1);
		Expression_PreParse(config->player2,&playerLine2);
	}
	if (config->monster){
		Expression_PreParse(config->monster1,&monsterLine1);
		Expression_PreParse(config->monster2,&monsterLine2);
	}
	if (config->self){
		Expression_PreParse(config->self1,&selfLine1);
		Expression_PreParse(config->self2,&selfLine2);
		Expression_PreParse(config->selfWindow,&selfLineWindow);
		Expression_PreParse(config->selfTray,&selfLineTray);

		//T4: Init PlayerInfo structure
		if (!PlayerInfo_Init()){
			AfxMessageBox("Problem with PlayerInfo module");
			toolThreadShouldStop = 0;
			return 0;
		}
	}

	// map with creatures seen
	map<creatureKey,creatureData,creatureKey> crMap;
	
	
	
	while (!toolThreadShouldStop)
	{
		Sleep(500);

		//T4: Parse info messages
		ReadPipeInfo();
	
		CTibiaCharacter *self = reader.readSelfCharacter();

		time_t curTime=time(NULL);

		int chNr;
		for (chNr=0;chNr<memConstData.m_memMaxCreatures;chNr++){
			//T4: Check all chars in BattleList
			CTibiaCharacter *ch = reader.readVisibleCreature(chNr);
			if (ch->tibiaId == 0){
				delete ch;
				break;
			}
			if (config->collectStats)
			{
				// collect statistics, but for monsters only

				                                   				       				
				if (ch->visible && ch->tibiaId > 0x40000000)
				{
					// see if we ever before have seen this creature
					// if not we refresh data about it every 30 seconds
					creatureData crData = crMap[creatureKey(ch->tibiaId)];
					if (!crData.tibiaId || (crData.tibiaId&&curTime-crData.tm>30))
					{
						char installPath[1024];
						CModuleUtil::getInstallPath(installPath);
						char pathBuf[2048];
						sprintf(pathBuf,"%s\\tibiaauto-stats-creatures.txt",installPath);
						FILE *f = fopen(pathBuf,"a+");
						if (f)
						{
							char statChName[128];
							int i,len;
							for (i=0,strcpy(statChName,ch->name),len=strlen(statChName);i<len;i++)
							{
								if (statChName[i]=='[')
									statChName[i]='\0';
							}
							
							time_t tm = time(NULL);
							int r = rand();
							int checksum=r*15+tm+ch->tibiaId*3+ch->x*5+ch->y*7+ch->z*11+strlen(statChName)*13+ch->walkSpeed*17;
							//int checksum=r*15+tm+ch->tibiaId*3+ch->x*5+ch->y*7+ch->z*11+strlen(statChName)*13;
							fprintf(f,"%d,%d,%d,%d,%d,'%s',%d,%d,%d\n",tm,ch->tibiaId,ch->x,ch->y,ch->z,statChName,ch->walkSpeed,r,checksum);
							fclose(f);
														
						}
						crMap[creatureKey(ch->tibiaId)].tibiaId=ch->tibiaId;
						crMap[creatureKey(ch->tibiaId)].tm=time(NULL);
					}
				}
			}

			if (ch->visible && abs(self->x-ch->x)<=7 && abs(self->y-ch->y)<=5 && (config->allFloorInfo?1:ch->z==self->z)){
				char line1[MAX_LINE_LEN];
				char line2[MAX_LINE_LEN];
				char lineWindow[MAX_LINE_LEN];
				char lineTray[MAX_LINE_LEN];

				int found=0;
				//T4: Apply custom text only to creatures on same z
				if (ch->tibiaId==self->tibiaId){
					//T4: Self
					if (!strcmpi(playersInfo[0].name,ch->name)){
						//T4: Sending info msg is needed to know guild info
						found=1;
					}else{
						playersInfo[0].type = TYPE_SELF;
					}
					if (config->self){
						Expression_GatherData(&selfLine1,(creature*)&playersInfo[0],self,config);
						Expression_GatherData(&selfLine2,(creature*)&playersInfo[0],self,config);
						Expression_GatherData(&selfLineWindow,(creature*)&playersInfo[0],self,config);
						Expression_GatherData(&selfLineTray,(creature*)&playersInfo[0],self,config);

						Expression_Exec(&selfLine1,line1);
						Expression_Exec(&selfLine2,line2);
						Expression_Exec(&selfLineWindow,lineWindow);
						Expression_Exec(&selfLineTray,lineTray);


						sender.sendCreatureInfo(ch->name,line1,line2);
						reader.setMainWindowText(lineWindow);
						reader.setMainTrayText(lineTray);
					}else{
						found=1;
					}
				}else{
					//T4: We found a visible monster or player
					int i;
					  
					if (ch->tibiaId < 0x40000000){
						if (config->player){
							//T4: This is a player
							for (i=1;i<playersCount;i++){
								if (!strcmpi(playersInfo[i].name,ch->name)){
									//T4: Player found in Known list
									found=1;
									break;
								}
							}
							if (found){
								Expression_GatherData(&playerLine1,(creature*)&playersInfo[i],ch,config);
								Expression_GatherData(&playerLine2,(creature*)&playersInfo[i],ch,config);
							}else{
								Expression_GatherData(&playerLine1,NULL,ch,config);
								Expression_GatherData(&playerLine2,NULL,ch,config);
							}

							//strcpy(line1,config->player1);
							//strcpy(line2,config->player2);
							Expression_Exec(&playerLine1,line1);
							Expression_Exec(&playerLine2,line2);

							sender.sendCreatureInfo(ch->name,line1,line2);
						}else{
							found=1;
						}
					}else{
						if (config->monster){
							//T4: This is a monster/npc
							char name[40];
							//T4: Extract name from BattleList string;
							Monster_GetName(ch->name,name);
							for (i=0;i<monstersCount;i++){
								if (!strcmpi(monstersInfo[i].name,name)){
									//T4: Monster found in Known list
									if (config->uniqueMonsterNames){
										//T4: We have to add number to name because we need unique names
										Monster_SetNameNumber(name,ch->nr,name);
										reader.writeVisibleCreatureName(ch->nr,name);
									}

									found=1;
									break;
								}
							}
							if (found){
								Expression_GatherData(&monsterLine1,(creature*)&monstersInfo[i],ch,config);
								Expression_GatherData(&monsterLine2,(creature*)&monstersInfo[i],ch,config);
							}else{
								Expression_GatherData(&monsterLine1,NULL,ch,config);
								Expression_GatherData(&monsterLine2,NULL,ch,config);
							}

							Expression_Exec(&monsterLine1,line1);
							Expression_Exec(&monsterLine2,line2);

							sender.sendCreatureInfo(name,line1,line2);
						}else{
							found=1;
						}
					}
				}
				if (!found && lookCount[ch->tibiaId]<7) {
					// creature not yet known - send "look" command
					sender.ignoreLook(time(NULL)+2);
					sender.look(ch->x,ch->y,ch->z,99);
					lookCount[ch->tibiaId]=lookCount[ch->tibiaId]+1;
					if (config->addRequest && ch->walkSpeed >= 350){
						if (ch->lookDirection==0){
							sender.look(ch->x,ch->y-1,ch->z,99);

						}else if(ch->lookDirection==1){
							sender.look(ch->x+1,ch->y,ch->z,99);

						}else if(ch->lookDirection==2){
							sender.look(ch->x,ch->y+1,ch->z,99);

						}else if(ch->lookDirection==3){
							sender.look(ch->x-1,ch->y,ch->z,99);
						}
					}
					Sleep(RandomTimeCreatureInfo(self,ch));
				}
			}
			delete ch;
		}
		delete self;
	}
	CTibiaCharacter *self = reader.readSelfCharacter();
	reader.setMainWindowText("Tibia");
	reader.setMainTrayText("<Running Tibia Auto>"); // back to the default setting
	delete self;
	//T4: Tool has been disabled, so clean all mess
	sender.sendClearCreatureInfo();
	if (config->uniqueMonsterNames){
		int chNr;
		for (chNr=0;chNr<memConstData.m_memMaxCreatures;chNr++){
			char name[40];
			CTibiaCharacter *ch = reader.readVisibleCreature(chNr);
			if (ch->tibiaId == 0){
				delete ch;
				break;
			}
			Monster_GetName(ch->name,name);
			reader.writeVisibleCreatureName(ch->nr,name);

			delete ch;
		}
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_creatureinfoApp construction

CMod_creatureinfoApp::CMod_creatureinfoApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();
}

CMod_creatureinfoApp::~CMod_creatureinfoApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
	delete m_configData;
}

char * CMod_creatureinfoApp::getName()
{
	return "Creature Info";
}


int CMod_creatureinfoApp::isStarted()
{
	return m_started;
}

 
void CMod_creatureinfoApp::start()
{
	superStart();
	if (m_configDialog)
	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

	DWORD threadId;

	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf,"%s\\tibiaauto-stats-creatures.txt",installPath);
	FILE *f=fopen(pathBuf,"r");
		
	if (f)
	
	{
		fseek(f,0,SEEK_END);

		int flen=ftell(f);
		fclose(f);
		if (flen>1024*500 && m_configDialog)
		{
			CSendStats info;
			info.DoModal();
		}
	}

	//T4: Reset counters
	playersCount	= 1;

	CreatureList_Init();
		
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);
	m_started=1;
}

void CMod_creatureinfoApp::stop()
{
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	};
	m_started=0;
	
	if (m_configDialog)
	{
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
}


char *CMod_creatureinfoApp::getVersion()
{
	return "2.1";
}

void CMod_creatureinfoApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
		if (m_started) disableControls();
		else enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_creatureinfoApp::configToControls()
{
	if (m_configDialog)
	{
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_creatureinfoApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_creatureinfoApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_creatureinfoApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


int CMod_creatureinfoApp::validateConfig(int showAlerts)
{
	if (m_configData->rangeXY<0||m_configData->rangeXY>100)
	{
		if (showAlerts) AfxMessageBox("RangeXY must be beween 0 and 100!");
		return 0;
	}
	if (m_configData->rangeZ<0||m_configData->rangeZ>15)
	{
		if (showAlerts) AfxMessageBox("RangeZ must be beween 0 and Z!");
		return 0;
	}
	return 1;
}

void CMod_creatureinfoApp::resetConfig()
{
	if(m_configData){
		delete m_configData;
		m_configData = NULL;
	}
	m_configData = new CConfigData();
}

void CMod_creatureinfoApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"player/active"))		m_configData->player = atoi(paramValue);
	if (!strcmp(paramName,"player/first"))	lstrcpyn(m_configData->player1,paramValue,MAX_LINE_LEN);
	if (!strcmp(paramName,"player/second"))	lstrcpyn(m_configData->player2,paramValue,MAX_LINE_LEN);
	if (!strcmp(paramName,"monster/active"))		m_configData->monster = atoi(paramValue);
	if (!strcmp(paramName,"monster/first"))	lstrcpyn(m_configData->monster1,paramValue,MAX_LINE_LEN);
	if (!strcmp(paramName,"monster/second"))lstrcpyn(m_configData->monster2,paramValue,MAX_LINE_LEN);
	if (!strcmp(paramName,"monster/uniqueMonsterNames"))	m_configData->uniqueMonsterNames = atoi(paramValue);
	if (!strcmp(paramName,"self/active"))		m_configData->self = atoi(paramValue);
	if (!strcmp(paramName,"self/first"))	lstrcpyn(m_configData->self1,paramValue,MAX_LINE_LEN);
	if (!strcmp(paramName,"self/second"))	lstrcpyn(m_configData->self2,paramValue,MAX_LINE_LEN);
	if (!strcmp(paramName,"self/window"))	lstrcpyn(m_configData->selfWindow,paramValue,MAX_LINE_LEN);
	if (!strcmp(paramName,"self/tray"))	lstrcpyn(m_configData->selfTray,paramValue,MAX_LINE_LEN);
	//LOCKED
	//if (!strcmp(paramName,"allFloorInfo"))		m_configData->allFloorInfo = atoi(paramValue);
	if (!strcmp(paramName,"addRequest"))		m_configData->addRequest = atoi(paramValue);
	if (!strcmp(paramName,"area/collectStats"))		m_configData->collectStats = 1;
	if (!strcmp(paramName,"area/rangeXY"))		m_configData->rangeXY = atoi(paramValue);
	if (!strcmp(paramName,"area/rangeZ"))		m_configData->rangeZ = atoi(paramValue);


}

char *CMod_creatureinfoApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"player/active"))		sprintf(buf,"%d",m_configData->player);
	if (!strcmp(paramName,"player/first"))	lstrcpyn(buf,m_configData->player1,MAX_LINE_LEN);
	if (!strcmp(paramName,"player/second"))	lstrcpyn(buf,m_configData->player2,MAX_LINE_LEN);
	if (!strcmp(paramName,"monster/active"))		sprintf(buf,"%d",m_configData->monster);
	if (!strcmp(paramName,"monster/first"))	lstrcpyn(buf,m_configData->monster1,MAX_LINE_LEN);
	if (!strcmp(paramName,"monster/second"))lstrcpyn(buf,m_configData->monster2,MAX_LINE_LEN);
	if (!strcmp(paramName,"monster/uniqueMonsterNames"))	sprintf(buf,"%d",m_configData->uniqueMonsterNames);
	if (!strcmp(paramName,"self/active"))		sprintf(buf,"%d",m_configData->self);
	if (!strcmp(paramName,"self/first"))	lstrcpyn(buf,m_configData->self1,MAX_LINE_LEN);
	if (!strcmp(paramName,"self/second"))	lstrcpyn(buf,m_configData->self2,MAX_LINE_LEN);
	if (!strcmp(paramName,"self/window"))	lstrcpyn(buf,m_configData->selfWindow,MAX_LINE_LEN);
	if (!strcmp(paramName,"self/tray"))	lstrcpyn(buf,m_configData->selfTray,MAX_LINE_LEN);
	if (!strcmp(paramName,"allFloorInfo"))	sprintf(buf,"%d",m_configData->allFloorInfo);
	if (!strcmp(paramName,"addRequest"))	sprintf(buf,"%d",m_configData->addRequest);
	if (!strcmp(paramName,"area/collectStats")) sprintf(buf,"%d",m_configData->collectStats);
	if (!strcmp(paramName,"area/rangeXY")) sprintf(buf,"%d",m_configData->rangeXY);
	if (!strcmp(paramName,"area/rangeZ")) sprintf(buf,"%d",m_configData->rangeZ);

	return buf;
}

char *CMod_creatureinfoApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "player/active";
	case 1: return "player/first";
	case 2: return "player/second";
	case 3: return "monster/active";
	case 4: return "monster/first";
	case 5: return "monster/second";
	case 6: return "monster/uniqueMonsterNames";
	case 7: return "self/active";
	case 8: return "self/first";
	case 9: return "self/second";
	case 10: return "allFloorInfo";
	case 11: return "addRequest";
	case 12: return "area/collectStats";
	case 13: return "area/showCreaturesInArea";
	case 14: return "self/window";
	case 15: return "self/tray";
	case 16: return "area/rangeXY";
	case 17: return "area/rangeZ";
	default:
		return NULL;
	}
}

void CMod_creatureinfoApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog){
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}