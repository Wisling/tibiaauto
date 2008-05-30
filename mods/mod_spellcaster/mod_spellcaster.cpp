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


// mod_spellcaster.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_spellcaster.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"
#include <fstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMod_spellcasterApp

BEGIN_MESSAGE_MAP(CMod_spellcasterApp, CWinApp)
//{{AFX_MSG_MAP(CMod_spellcasterApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;
monster monstersInfo[500];

int initalizeCreatures();
int isInitializedCreatures();
int getcurrentMonsterNumberFromName(char *);
int aoeShouldFire(CConfigData *);

int OnList(char whiteList[100][32],char name[]){
	int i=0;
	while (IsCharAlphaNumeric(whiteList[i][0])){
		if (!strcmpi(whiteList[i],name)){
			return 1;
		}
		i++;
	}

	return 0;
}

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	int currentMonsterNumber = 0;
	char text[128] = {0};
	int best = 0;
	
	if (isInitializedCreatures() == 0)
		initalizeCreatures();
	
	while (!toolThreadShouldStop)
	{			
		Sleep(200);	
		CTibiaCharacter *self = reader.readSelfCharacter();
		int attackedCreature = reader.getAttackedCreature();
		int flags = reader.getSelfEventFlags();
		
		//T4: First try to heal/also uses paralysis cure here
		if (config->life && (self->hp<=config->lifeHp || self->hp<=config->vitaHp || self->hp<=config->granHp || self->hp<=config->exuraHp || (config->paralysisSpell && flags & 32 == 32))) {
			// Akilez:	Give 1st priority to custom spells!
			if (config->customSpell && self->hp<=config->lifeHp && self->mana >= config->lifeSpellMana){
				sender.sayWhisper(config->lifeSpell);
				Sleep(700);
				self = reader.readSelfCharacter();
			}
			else if(config->vitaSpell && self->hp<=config->vitaHp && self->mana >= config->vitaSpellMana){
				sender.sayWhisper("exura vita");
				Sleep(700);
			}
			else if(config->granSpell && self->hp<=config->granHp && self->mana >= config->granSpellMana){
				sender.sayWhisper("exura gran");
				Sleep(700);
			}
			else if((config->exuraSpell && self->hp<=config->exuraHp && self->mana >= config->exuraSpellMana) || (config->paralysisSpell && flags & 32 == 32)) {
				sender.sayWhisper("exura");
				Sleep(700);
			}
			else {
				sender.sendTAMessage("WARNING!!! Not enough mana to Heal!!!");
				Sleep(700);
			}
		}	
		else if(config->poisonSpell && flags & 1 == 1) {
			for (int i = 0; i<6; i++) {
				for (int j = 0; j<10; j++) {
					char word = reader.getMemIntValue(0x76D928+i*10+j);//Wis: NEEDS TO BE UPDATED WITH EVERY NEW TIBIA VERSION, or assigned to a constant in items.xml 
					if (word % 0x100 == 0) text[i*10+j] = ' '; 
					text[i*10+j] = word%0x100;
				}				
			}
			if ((strstr(text,"hitpoints.") != 0) || (strstr(text,"hitpoint.") != 0)) {
				char pointLossText[10] = {0};
				for (int i = 0; i<10;i++) {
					if (!isdigit(text[strlen("You lose ")+i])) break;
					pointLossText[i] = text[strlen("You lose ")+i];
				}
				if (atoi(pointLossText) >= config->minPoisonDmg && atoi(pointLossText) != 5) sender.sayWhisper("exana pox");
			}
		}
		else if (config->sioSpell && self->mana >= config->sioSpellMana) {
			int chNr;
			for (chNr=0;chNr<memConstData.m_memMaxCreatures;chNr++) {
				CTibiaCharacter *ch = reader.readVisibleCreature(chNr);
				if (OnList(config->healList,ch->name) && ch->hp <= config->sioHp && ch->visible == 1) {
					char buf[256];
					sprintf(buf,"exura sio \"%s\"",ch->name);
					sender.sendTAMessage(buf);
					Sleep(700);	
				}
			}
		}
		if (config->aoe) {
			int spell = aoeShouldFire(config);
			if (spell) {
				switch (spell) {
				case 1:
					if (self->mana > 115) 
						sender.sayWhisper("exori");
					else if (self->mana > 340)
						sender.sayWhisper("exori gran");
					break;
				case 2:
					if (self->mana > 160) 
						sender.sayWhisper("exori mas");
					break;
				case 3:
					if (self->mana > 180) 
						sender.sayWhisper("exevo mas san");
					break;
				case 4:
					if (self->mana > 25) 
						sender.sayWhisper("exevo flam hur");
					break;
				case 5:
					if (self->mana > 25) 
						sender.sayWhisper("exevo frigo hur");
					break;
				case 6:
					if (self->mana > 210) 
						sender.sayWhisper("exevo tera hur");
					break;
				case 7:
					if (self->mana > 170) 
						sender.sayWhisper("exevo vis hur");
					break;
				case 8:
					if (self->mana > 40) 
						sender.sayWhisper("exevo vis lux");
					break;
				case 9:
					if (self->mana > 110) 
						sender.sayWhisper("exevo gran vis lux");
					break;
				case 10:
					if (self->mana > 650) 
						sender.sayWhisper("exevo gran mas vis");
					break;
				case 11:
					if (self->mana > 1200) 
						sender.sayWhisper("exevo gran mas flam");
					break;
				case 12:
					if (self->mana > 770) 
						sender.sayWhisper("exevo gran mas tera");
					break;
				case 13:
					if (self->mana > 1200) 
						sender.sayWhisper("exevo gran mas frigo");
					break;
				default:
					break;
				}
				Sleep(700);
			}
		}
		//Akilez: Use mana for strike spells
		else if(config->strike && self->mana>=config->manaStrike && attackedCreature){
			attackedCreature = reader.getAttackedCreature();
			//T4: If any creature is attacked
			if (attackedCreature) {			
				//T4: Get attacked creature stucture
				CTibiaCharacter *ch = reader.getCharacterByTibiaId(attackedCreature);
				currentMonsterNumber = getcurrentMonsterNumberFromName(ch->name);
				if (ch->name && ch->hpPercLeft && currentMonsterNumber > -1) {
					if ((monstersInfo[currentMonsterNumber].hp * ch->hpPercLeft * .01 > config->strikeSpellHpMin) || (currentMonsterNumber == -1))
					{
						//T4: cords
						int chX=ch->x;
						int chY=ch->y;
						int chZ=ch->z;
						
						int xDist = abs(self->x-chX);
						int yDist = abs(self->y-chY);
						int maxDist = xDist;
						if (yDist>maxDist) maxDist=yDist;
						int test = config->flam+config->frigo+config->mort+config->tera+config->vis;
						if (maxDist <= 3 && test) {
							int check = 0;
							if (monstersInfo[currentMonsterNumber].weakFire && config->flam) {
								best = 1;
								check = monstersInfo[currentMonsterNumber].weakFire;
							}
							if (monstersInfo[currentMonsterNumber].weakIce && monstersInfo[currentMonsterNumber].weakIce > check && config->frigo) {
								best = 2;
								check = monstersInfo[currentMonsterNumber].weakIce;
							}
							if (monstersInfo[currentMonsterNumber].weakEarth && monstersInfo[currentMonsterNumber].weakEarth > check && config->tera) {
								best = 3;
								check = monstersInfo[currentMonsterNumber].weakEarth;
							}
							if (monstersInfo[currentMonsterNumber].weakEnergy && monstersInfo[currentMonsterNumber].weakEnergy > check && config->vis) {
								best = 4;
								check = monstersInfo[currentMonsterNumber].weakEnergy;
							}
							if (monstersInfo[currentMonsterNumber].weakDeath && monstersInfo[currentMonsterNumber].weakDeath > check && config->mort) {
								best = 5;
								check = monstersInfo[currentMonsterNumber].weakDeath;
							}
							
							switch (best) {
							case 1:
								sender.sayWhisper("exori flam");
								break;
							case 2:
								sender.sayWhisper("exori frigo");
								break;
							case 3:
								sender.sayWhisper("exori tera");
								break;
							case 4:
								sender.sayWhisper("exori vis");
								break;
							case 5:
								sender.sayWhisper("exori mort");
								break;
							default:
								break;
							}
							Sleep(700);
							best = 0;
						}
						else if (maxDist <= 4 && config->san && monstersInfo[currentMonsterNumber].weakHoly) {							
							sender.sayWhisper("exori san");
							Sleep(700);
						}
						else if (maxDist <= 5 && config->hur && monstersInfo[currentMonsterNumber].weakPhysical) {
							sender.sayWhisper("exori hur");
							Sleep(700);
						}
						else if (config->con && monstersInfo[currentMonsterNumber].weakPhysical) {
							sender.sayWhisper("exori con");
							Sleep(700);
						}
						else if (config->defaultStrikeSpell) {
							sender.sayWhisper(config->defaultStrikeSpell);
							Sleep(700);
						}
						else {
							sender.sendTAMessage("WARNING!!! No appropriate strike spell configured!");
						}
					}
				}
				delete ch;
				attackedCreature = 0;
			}			
		}	
		//T4: Use mana in other purpose otherwise
		else if(config->mana && self->mana>=config->manaMana){
			sender.sayWhisper(config->manaSpell);
			Sleep(700);
		}	
		else if (config->summon) {
			// now try to summon creatures
			int chNr;
			int summonCount=0;
			for (chNr=0;chNr<memConstData.m_memMaxCreatures;chNr++) {
				CTibiaCharacter *ch = reader.readVisibleCreature(chNr);
				if (ch->z==self->z&&ch->visible&&!strcmpi(_strlwr(config->summonName),_strlwr(ch->name)))
					summonCount++;
				delete ch;
			}
			if (summonCount<config->summonLessThan && self->mana>=config->summonMana) {				
				// we should summon something
				char buf[256];
				sprintf(buf,"utevo res \"%s\"",config->summonName);
				sender.sayWhisper(buf);
				Sleep(700);
			}
		}
		delete self;
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_spellcasterApp construction

CMod_spellcasterApp::CMod_spellcasterApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_spellcasterApp::~CMod_spellcasterApp()
{
	if (m_configDialog)	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_spellcasterApp::getName() {
	return "Spell Caster";
}


int CMod_spellcasterApp::isStarted() {
	return m_started;
}


void CMod_spellcasterApp::start() {	
	if (m_configDialog)	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}
	
	DWORD threadId;
	
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_spellcasterApp::stop() {
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	};
	m_started=0;
	
	if (m_configDialog)	{
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
} 

void CMod_spellcasterApp::showConfigDialog() {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	
	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_spellcasterApp::configToControls() {
	if (m_configDialog) {
		m_configDialog->configToControls(m_configData);
	}
}

void CMod_spellcasterApp::controlsToConfig() {
	if (m_configDialog) {
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_spellcasterApp::disableControls() {
	if (m_configDialog) {
		m_configDialog->disableControls();
	}
}

void CMod_spellcasterApp::enableControls() {
	if (m_configDialog) {
		m_configDialog->enableControls();
	}
}

char *CMod_spellcasterApp::getVersion() {
	return "2.6";
}

int CMod_spellcasterApp::validateConfig(int showAlerts) {
	if (m_configData->mana) {
		if (!strlen(m_configData->manaSpell)) {
			if (showAlerts) AfxMessageBox("Some mana spell to cast must be defined!");
			return 0;
		}
		if (m_configData->manaMana<0) {
			if (showAlerts) AfxMessageBox("'mana above' must be >=0!");
			return 0;
		}
	}
	if (m_configData->life) {
		int test = m_configData->customSpell+m_configData->vitaSpell+m_configData->granSpell+m_configData->exuraSpell+m_configData->paralysisSpell+m_configData->poisonSpell;
		if (!test) {
			AfxMessageBox("At least one healing spell must be defined!");
			return 0;
		}
		if (m_configData->customSpell) {
			if (!strlen(m_configData->lifeSpell)) {
				if (showAlerts) AfxMessageBox("Some life spell to cast must be defined!");
				return 0;
			}
			if (m_configData->lifeHp<0)	{
				if (showAlerts) AfxMessageBox("'Cast when life below' must be >=0!");
				return 0;
			}
			if (m_configData->lifeSpellMana<1) {
				if (showAlerts) AfxMessageBox("'Spell mana' must be >=1!");
				return 0;
			}			
		}
		if (m_configData->vitaSpell) {
			if (m_configData->vitaHp<0)
			{
				if (showAlerts) AfxMessageBox("'Cast when life below' must be >=0!");
				return 0;
			}
			if (m_configData->vitaSpellMana<160) {
				if (showAlerts) AfxMessageBox("'Spell mana' must be >=160!");
				return 0;
			}
		}
		if (m_configData->sioSpell) {
			if (m_configData->sioHp<0)
			{
				if (showAlerts) AfxMessageBox("'Cast when life below' must be >=0!");
				return 0;
			}
			if (m_configData->sioSpellMana<160) {
				if (showAlerts) AfxMessageBox("'Spell mana' must be >=160!");
				return 0;
			}
		}
		if (m_configData->granSpell) {
			if (m_configData->granHp<0) {
				if (showAlerts) AfxMessageBox("'Cast when life below' must be >=0!");
				return 0;
			}
			if (m_configData->granSpellMana<70) {
				if (showAlerts) AfxMessageBox("'Spell mana' must be >=70!");
				return 0;
			}
		}
		if (m_configData->exuraSpell) {
			if (m_configData->exuraHp<0) {
				if (showAlerts) AfxMessageBox("'Cast when life below' must be >=20!");
				return 0;
			}
			if (m_configData->exuraSpellMana<20) {
				if (showAlerts) AfxMessageBox("'Spell mana' must be >=1!");
				return 0;
			}
		}
		if (m_configData->poisonSpell) {
			if (m_configData->minPoisonDmg<1) {
				if (showAlerts) AfxMessageBox("'Cure poison when damage greater than' must be >=1!");
				return 0;
			}
		}
	}
	if (m_configData->summon) {
		if (m_configData->summonLessThan<0||m_configData->summonLessThan>2) {
			if (showAlerts) AfxMessageBox("'summon less than' must be between 0 and 2!");
			return 0;
		}
		if (!strlen(m_configData->summonName)) {
			if (showAlerts) AfxMessageBox("Summon's name must be not empty!");
			return 0;
		}
		if (m_configData->summonMana<0) {
			if (showAlerts) AfxMessageBox("'summon mana' must be >=0!");
			return 0;
		}
	}
	if (m_configData->strike) {
		if (m_configData->strikeSpellHpMin<0) {
			if (showAlerts) AfxMessageBox("'Target's HP above' must be >=0!");
			return 0;
		}
		if (m_configData->manaStrike<20) {
			if (showAlerts) AfxMessageBox("'Strike mana above' must be >=20!");
			return 0;
		}
		int test = m_configData->flam+m_configData->frigo+m_configData->mort+m_configData->tera+m_configData->vis+m_configData->hur+m_configData->con+m_configData->san;
		if (!test) {
			if (showAlerts) AfxMessageBox("You must choose one strike spell!!");
			return 0;
		}
	}
	if (m_configData->aoe) {
		int test = m_configData->exori + m_configData->exoriGran + m_configData->exoriMas + m_configData->exevoFlamHur + m_configData->exevoFrigoHur + m_configData->exevoTeraHur + m_configData->exevoVisHur + m_configData->exevoVisLux + m_configData->exevoGranVisLux + m_configData->exevoGranMasFlam + m_configData->exevoGranMasFrigo + m_configData->exevoGranMasTera + m_configData->exevoGranMasVis;
		if (!test) {
			if (showAlerts) AfxMessageBox("You must choose one Area of Effect spell!!");
			return 0;
		}
	}
	return 1;
}

void CMod_spellcasterApp::resetConfig() {
	m_configData = new CConfigData();
}

void CMod_spellcasterApp::loadConfigParam(char *paramName,char *paramValue) {
	if (!strcmp(paramName,"mana")) m_configData->mana=atoi(paramValue);
	if (!strcmp(paramName,"manaMana")) m_configData->manaMana=atoi(paramValue);
	if (!strcmp(paramName,"manaSpell")) lstrcpyn(m_configData->manaSpell,paramValue,128);
	
	if (!strcmp(paramName,"life")) m_configData->life=atoi(paramValue);
	if (!strcmp(paramName,"customSpell")) m_configData->customSpell=atoi(paramValue);
	if (!strcmp(paramName,"lifeHp")) m_configData->lifeHp=atoi(paramValue);
	if (!strcmp(paramName,"lifeSpell")) lstrcpyn(m_configData->lifeSpell,paramValue,128);
	if (!strcmp(paramName,"lifeSpellMana")) m_configData->lifeSpellMana=atoi(paramValue);
	if (!strcmp(paramName,"exuraSpell")) m_configData->exuraSpell=atoi(paramValue);
	if (!strcmp(paramName,"exuraHp")) m_configData->exuraHp=atoi(paramValue);
	if (!strcmp(paramName,"exuraSpellMana")) m_configData->exuraSpellMana=atoi(paramValue);
	if (!strcmp(paramName,"granSpell")) m_configData->granSpell=atoi(paramValue);
	if (!strcmp(paramName,"granHp")) m_configData->granHp=atoi(paramValue);
	if (!strcmp(paramName,"granSpellMana")) m_configData->granSpellMana=atoi(paramValue);
	if (!strcmp(paramName,"vitaSpell")) m_configData->vitaSpell=atoi(paramValue);
	if (!strcmp(paramName,"vitaHp")) m_configData->vitaHp=atoi(paramValue);
	if (!strcmp(paramName,"vitaSpellMana")) m_configData->vitaSpellMana=atoi(paramValue);
	if (!strcmp(paramName,"sioSpell")) m_configData->sioSpell=atoi(paramValue);
	if (!strcmp(paramName,"sioHp")) m_configData->sioHp=atoi(paramValue);
	if (!strcmp(paramName,"sioSpellMana")) m_configData->sioSpellMana=atoi(paramValue);
	if (!strcmp(paramName,"poisonSpell")) m_configData->poisonSpell=atoi(paramValue);
	if (!strcmp(paramName,"paralysisSpell")) m_configData->paralysisSpell=atoi(paramValue);
	if (!strcmp(paramName,"minPoisonDmg")) m_configData->minPoisonDmg=atoi(paramValue);
	
	if (!strcmp(paramName,"summon")) m_configData->summon=atoi(paramValue);
	if (!strcmp(paramName,"summonLessThan")) m_configData->summonLessThan=atoi(paramValue);
	if (!strcmp(paramName,"summonMana")) m_configData->summonMana=atoi(paramValue);
	if (!strcmp(paramName,"summonName")) lstrcpyn(m_configData->summonName,paramValue,128);
	
	if (!strcmp(paramName,"strike")) m_configData->strike=atoi(paramValue);
	if (!strcmp(paramName,"ExoriFlam")) m_configData->flam=atoi(paramValue);
	if (!strcmp(paramName,"ExoriFrigo")) m_configData->frigo=atoi(paramValue);
	if (!strcmp(paramName,"ExoriMort")) m_configData->mort=atoi(paramValue);
	if (!strcmp(paramName,"ExoriTera")) m_configData->tera=atoi(paramValue);
	if (!strcmp(paramName,"ExoriVis")) m_configData->vis=atoi(paramValue);
	if (!strcmp(paramName,"ExoriCon")) m_configData->con=atoi(paramValue);
	if (!strcmp(paramName,"ExoriSan")) m_configData->san=atoi(paramValue);
	if (!strcmp(paramName,"ExoriHur")) m_configData->hur=atoi(paramValue);
	if (!strcmp(paramName,"manaStrike")) m_configData->manaStrike=atoi(paramValue);
	if (!strcmp(paramName,"defaultStrikeSpell")) lstrcpyn(m_configData->defaultStrikeSpell,paramValue,128);
	if (!strcmp(paramName,"strikeSpellHpMin")) m_configData->strikeSpellHpMin=atoi(paramValue);
	
	if (!strcmp(paramName,"aoe")) m_configData->aoe=atoi(paramValue);
	if (!strcmp(paramName,"Exori")) m_configData->exori=atoi(paramValue);
	if (!strcmp(paramName,"ExoriGran")) m_configData->exoriGran=atoi(paramValue);
	if (!strcmp(paramName,"ExoriMas")) m_configData->exoriMas=atoi(paramValue);
	if (!strcmp(paramName,"ExoriMasSan")) m_configData->exevoMasSan=atoi(paramValue);
	if (!strcmp(paramName,"ExevoFlamHur")) m_configData->exevoFlamHur=atoi(paramValue);
	if (!strcmp(paramName,"ExevoFrigoHur")) m_configData->exevoFrigoHur=atoi(paramValue);
	if (!strcmp(paramName,"ExevoTeraHur")) m_configData->exevoTeraHur=atoi(paramValue);
	if (!strcmp(paramName,"ExevoVisHur")) m_configData->exevoVisHur=atoi(paramValue);
	if (!strcmp(paramName,"ExevoVisLux")) m_configData->exevoVisLux=atoi(paramValue);
	if (!strcmp(paramName,"ExevoGranVisLux")) m_configData->exevoGranVisLux=atoi(paramValue);
	if (!strcmp(paramName,"ExevoGranMasVis")) m_configData->exevoGranMasVis=atoi(paramValue);
	if (!strcmp(paramName,"ExevoGranMasFlam")) m_configData->exevoGranMasFlam=atoi(paramValue);
	if (!strcmp(paramName,"ExevoGranMasTera")) m_configData->exevoGranMasTera=atoi(paramValue);
	if (!strcmp(paramName,"ExevoGranMasFrigo")) m_configData->exevoGranMasFrigo=atoi(paramValue);
	if (!strcmp(paramName,"whiteList")){
		if (currentPos>99)
			return;
		lstrcpyn(m_configData->healList[currentPos++],paramValue,32);
	}
}

char *CMod_spellcasterApp::saveConfigParam(char *paramName) {
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"mana")) sprintf(buf,"%d",m_configData->mana);
	if (!strcmp(paramName,"manaMana")) sprintf(buf,"%d",m_configData->manaMana);
	if (!strcmp(paramName,"manaSpell")) sprintf(buf,"%s",m_configData->manaSpell);
	
	if (!strcmp(paramName,"life")) sprintf(buf,"%d",m_configData->life);
	if (!strcmp(paramName,"customSpell")) sprintf(buf,"%d",m_configData->customSpell);
	if (!strcmp(paramName,"lifeHp")) sprintf(buf,"%d",m_configData->lifeHp);
	if (!strcmp(paramName,"lifeSpell")) sprintf(buf,"%s",m_configData->lifeSpell);
	if (!strcmp(paramName,"lifeSpellMana")) sprintf(buf,"%d",m_configData->lifeSpellMana);
	if (!strcmp(paramName,"exuraSpell")) sprintf(buf,"%d",m_configData->exuraSpell);
	if (!strcmp(paramName,"exuraHp")) sprintf(buf,"%d",m_configData->exuraHp);
	if (!strcmp(paramName,"exuraSpellMana")) sprintf(buf,"%d",m_configData->exuraSpellMana);
	if (!strcmp(paramName,"granSpell")) sprintf(buf,"%d",m_configData->granSpell);
	if (!strcmp(paramName,"granHp")) sprintf(buf,"%d",m_configData->granHp);
	if (!strcmp(paramName,"granSpellMana")) sprintf(buf,"%d",m_configData->granSpellMana);
	if (!strcmp(paramName,"vitaSpell")) sprintf(buf,"%d",m_configData->vitaSpell);
	if (!strcmp(paramName,"vitaHp")) sprintf(buf,"%d",m_configData->vitaHp);
	if (!strcmp(paramName,"vitaSpellMana")) sprintf(buf,"%d",m_configData->vitaSpellMana);
	if (!strcmp(paramName,"sioSpell")) sprintf(buf,"%d",m_configData->sioSpell);
	if (!strcmp(paramName,"sioHp")) sprintf(buf,"%d",m_configData->sioHp);
	if (!strcmp(paramName,"sioSpellMana")) sprintf(buf,"%d",m_configData->sioSpellMana);
	if (!strcmp(paramName,"poisonSpell")) sprintf(buf,"%d",m_configData->poisonSpell);
	if (!strcmp(paramName,"paralysisSpell")) sprintf(buf,"%d",m_configData->paralysisSpell);
	if (!strcmp(paramName,"minPoisonDmg")) sprintf(buf,"%d",m_configData->minPoisonDmg);
	
	if (!strcmp(paramName,"summon")) sprintf(buf,"%d",m_configData->summon);
	if (!strcmp(paramName,"summonLessThan")) sprintf(buf,"%d",m_configData->summonLessThan);
	if (!strcmp(paramName,"summonMana")) sprintf(buf,"%d",m_configData->summonMana);
	if (!strcmp(paramName,"summonName")) strcpy(buf,m_configData->summonName);
	
	if (!strcmp(paramName,"strike")) sprintf(buf,"%d",m_configData->strike);
	if (!strcmp(paramName,"ExoriFlam")) sprintf(buf,"%d",m_configData->flam);
	if (!strcmp(paramName,"ExoriFrigo")) sprintf(buf,"%d",m_configData->frigo);
	if (!strcmp(paramName,"ExoriMort")) sprintf(buf,"%d",m_configData->mort);
	if (!strcmp(paramName,"ExoriTera")) sprintf(buf,"%d",m_configData->tera);
	if (!strcmp(paramName,"ExoriVis")) sprintf(buf,"%d",m_configData->vis);
	if (!strcmp(paramName,"ExoriCon")) sprintf(buf,"%d",m_configData->con);
	if (!strcmp(paramName,"ExoriSan")) sprintf(buf,"%d",m_configData->san);
	if (!strcmp(paramName,"ExoriHur")) sprintf(buf,"%d",m_configData->hur);
	if (!strcmp(paramName,"manaStrike")) sprintf(buf,"%d",m_configData->manaStrike);
	if (!strcmp(paramName,"defaultStrikeSpell")) sprintf(buf,"%s",m_configData->defaultStrikeSpell);
	if (!strcmp(paramName,"strikeSpellHpMin")) sprintf(buf,"%d",m_configData->strikeSpellHpMin);
	
	if (!strcmp(paramName,"aoe")) sprintf(buf,"%d",m_configData->aoe);
	if (!strcmp(paramName,"Exori")) sprintf(buf,"%d",m_configData->exori);
	if (!strcmp(paramName,"ExoriGran")) sprintf(buf,"%d",m_configData->exoriGran);
	if (!strcmp(paramName,"ExoriMas")) sprintf(buf,"%d",m_configData->exoriMas);
	if (!strcmp(paramName,"ExoriMasSan")) sprintf(buf,"%d",m_configData->exevoMasSan);
	if (!strcmp(paramName,"ExevoFlamHur")) sprintf(buf,"%d",m_configData->exevoFlamHur);
	if (!strcmp(paramName,"ExevoFrigoHur")) sprintf(buf,"%d",m_configData->exevoFrigoHur);
	if (!strcmp(paramName,"ExevoTeraHur")) sprintf(buf,"%d",m_configData->exevoTeraHur);
	if (!strcmp(paramName,"ExevoVisHur")) sprintf(buf,"%d",m_configData->exevoVisHur);
	if (!strcmp(paramName,"ExevoVisLux")) sprintf(buf,"%d",m_configData->exevoVisLux);
	if (!strcmp(paramName,"ExevoGranVisLux")) sprintf(buf,"%d",m_configData->exevoGranVisLux);
	if (!strcmp(paramName,"ExevoGranMasVis")) sprintf(buf,"%d",m_configData->exevoGranMasVis);
	if (!strcmp(paramName,"ExevoGranMasFlam")) sprintf(buf,"%d",m_configData->exevoGranMasFlam);
	if (!strcmp(paramName,"ExevoGranMasTera")) sprintf(buf,"%d",m_configData->exevoGranMasTera);
	if (!strcmp(paramName,"ExevoGranMasFrigo")) sprintf(buf,"%d",m_configData->exevoGranMasFrigo);
	if (!strcmp(paramName,"healList")){		
		if (currentPos<100){				
			if (IsCharAlphaNumeric(m_configData->healList[currentPos][0])){				
				lstrcpyn(buf,m_configData->healList[currentPos++],32);
			}
		}		
	}
	
	return buf;
}

char *CMod_spellcasterApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "mana";
	case 1: return "manaMana";
	case 2: return "manaSpell";
	case 3: return "life";
	case 4: return "lifeHp";
	case 5: return "lifeSpell";
	case 6: return "lifeSpellMana";
	case 7: return "summon";
	case 8: return "summonLessThan";
	case 9: return "summonName";
	case 10: return "strike";
	case 11: return "ExoriFlam";
	case 12: return "ExoriFrigo";
	case 13: return "ExoriMort";
	case 14: return "ExoriTera";
	case 15: return "ExoriVis";
	case 16: return "manaStrike";
	case 17: return "customSpell";
	case 18: return "exuraSpell";
	case 19: return "exuraHp";
	case 20: return "exuraSpellMana";
	case 21: return "granSpell";
	case 22: return "granHp";
	case 23: return "granSpellMana";
	case 24: return "vitaSpell";
	case 25: return "vitaHp";
	case 26: return "vitaSpellMana";
	case 27: return "defaultStrikeSpell";
	case 28: return "summonMana";
	case 29: return "strikeSpellHpMin";
	case 30: return "poisonSpell";
	case 31: return "paralysisSpell";
	case 32: return "minPoisonDmg";
	case 33: return "ExoriCon";
	case 34: return "ExoriSan";
	case 35: return "ExoriHur";
	case 36: return "aoe";
	case 37: return "Exori";
	case 38: return "ExoriGran";
	case 39: return "ExoriMas";
	case 40: return "ExoriMasSan";
	case 41: return "ExevoFlamHur";
	case 42: return "ExevoFrigoHur";
	case 43: return "ExevoTeraHur";
	case 44: return "ExevoVisHur";
	case 45: return "ExevoVisLux";
	case 46: return "ExevoGranVisLux";
	case 47: return "ExevoGranMasVis";
	case 48: return "ExevoGranMasFlam";
	case 49: return "ExevoGranMasTera";
	case 50: return "ExevoGranMasFrigo";
	case 51: return "sioSpell";
	case 52: return "sioHp";
	case 53: return "sioSpellMana";
	case 54: return "healList";
	default:
		return NULL;
	}
}
int CMod_spellcasterApp::isMultiParam(char *paramName) {
	if (!strcmp(paramName,"healList")) return 1;
	return 0;
}

void CMod_spellcasterApp::resetMultiParamAccess(char *paramName) {
	if (!strcmp(paramName,"healList")) currentPos=0;
}

int initalizeCreatures() {
	char installPath[1024];
	unsigned long installPathLen=1023;
	installPath[0]='\0';
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
	{
		RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath))
	{
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}
	
	char pathBuf[2048];    
	
	sprintf(pathBuf,"%s\\mods\\tibiaauto-creatureWeakness.csv",installPath);
	
	ifstream creatureFile (pathBuf, ios::in);
	if (!creatureFile.is_open()) {	AfxMessageBox("File tibiaauto-creatureWeakness.csv Not found!"); creatureFile.close(); return 0;}
	char buf[128] = {0};
	int crNum = 0;
	
	while (!creatureFile.eof()) {
		creatureFile.getline(monstersInfo[crNum].name, 128, ',');
		//AfxMessageBox(monstersInfo[crNum].name);
		creatureFile.getline(buf, 128, ',');
		if (strcmpi(buf, "Earth")==0) monstersInfo[crNum].weakEarth = 5;
		else if (strcmpi(buf, "Strong")==0) monstersInfo[crNum].weakEarth = 1;
		else if (strcmpi(buf, "Immune")==0) monstersInfo[crNum].weakEarth = 0;
		else  monstersInfo[crNum].weakEarth = 2;
		//AfxMessageBox(buf);
		creatureFile.getline(buf, 128, ',');
		if (strcmpi(buf, "Fire")==0) monstersInfo[crNum].weakFire = 5;
		else if (strcmpi(buf, "Strong")==0) monstersInfo[crNum].weakFire = 1;
		else if (strcmpi(buf, "Immune")==0) monstersInfo[crNum].weakFire = 0;
		else  monstersInfo[crNum].weakFire = 2;
		//AfxMessageBox(buf);
		creatureFile.getline(buf, 128, ',');
		if (strcmpi(buf, "Energy")==0) monstersInfo[crNum].weakEnergy = 5;
		else if (strcmpi(buf, "Strong")==0) monstersInfo[crNum].weakEnergy = 1;
		else if (strcmpi(buf, "Immune")==0) monstersInfo[crNum].weakEnergy = 0;
		else  monstersInfo[crNum].weakEnergy = 2;
		//AfxMessageBox(buf);
		creatureFile.getline(buf, 128, ',');
		if (strcmpi(buf, "Ice")==0) monstersInfo[crNum].weakIce = 5;
		else if (strcmpi(buf, "Strong")==0) monstersInfo[crNum].weakIce = 1;
		else if (strcmpi(buf, "Immune")==0) monstersInfo[crNum].weakIce = 0;
		else  monstersInfo[crNum].weakIce = 2;
		//AfxMessageBox(buf);
		creatureFile.getline(buf, 128, ',');
		if (strcmpi(buf, "Death")==0) monstersInfo[crNum].weakDeath = 5;
		else if (strcmpi(buf, "Strong")==0) monstersInfo[crNum].weakDeath = 1;
		else if (strcmpi(buf, "Immune")==0) monstersInfo[crNum].weakDeath = 0;
		else  monstersInfo[crNum].weakDeath = 2;
		//AfxMessageBox(buf);
		creatureFile.getline(buf, 128, ',');
		if (strcmpi(buf, "Holy")==0) monstersInfo[crNum].weakHoly = 5;
		else if (strcmpi(buf, "Strong")==0) monstersInfo[crNum].weakHoly = 1;
		else if (strcmpi(buf, "Immune")==0) monstersInfo[crNum].weakHoly = 0;
		else  monstersInfo[crNum].weakHoly = 2;
		//AfxMessageBox(buf);
		creatureFile.getline(buf, 128, ',');
		if (strcmpi(buf, "Physical")==0) monstersInfo[crNum].weakPhysical = 5;
		else if (strcmpi(buf, "Strong")==0) monstersInfo[crNum].weakPhysical = 1;
		else if (strcmpi(buf, "Immune")==0) monstersInfo[crNum].weakPhysical = 0;
		else  monstersInfo[crNum].weakPhysical = 2;
		//AfxMessageBox(buf);
		creatureFile.getline(buf, 128, '\n');
		monstersInfo[crNum++].hp = atoi(buf);
		//AfxMessageBox(monstersInfo[max].name);
	}
	creatureFile.close();
	
	return 1;
}

int isInitializedCreatures() {
	if (strcmpi(monstersInfo[0].name, "Initialized") == 0) return 1;
	return 0;
}

int getcurrentMonsterNumberFromName(char *match) {
	int foundNone = -1, max = 0;
	while (true) {
		if (strcmpi(monstersInfo[max].name, match)==0) {			
			return max;
		}
		if (max++ == 250) return foundNone;;		
	}
	
}

int aoeShouldFire(CConfigData *config) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	int exoriCount, exoriMasCount, exevoMasSanCount, egmTeraCount, egmFlamCount, egmFrigoCount, egmVisCount = 0;
	int exevoTeraHurCount, exevoVisHurCount, exevoGranVisLuxCount, exevoVisLuxCount, exevoFlamHurCount, exevoFrigoHurCount = 0;
	int deltaX, deltaY = 0;
	int chNr, returnSpell, faceDir = 0;
	for (chNr=0;chNr<memConstData.m_memMaxCreatures;chNr++) {
		CTibiaCharacter *ch = reader.readVisibleCreature(chNr);
		int monster = getcurrentMonsterNumberFromName(ch->name);
		if (strcmpi(_strlwr(self->name),_strlwr(ch->name)) != 0 && self->z == ch->z && ch->visible == 1) {
			deltaX = ch->x - self->x;
			deltaY = ch->y - self->y;
			if (deltaX >= 0) {
				if (deltaY > 0) {
					if (deltaX > deltaY)
						faceDir = 4;
					else 
						faceDir = 0;
				}
				else if (deltaY <= 0) {
					if (deltaX > abs(deltaY))
						faceDir = 4;
					else 
						faceDir = 2;
				}
			}
			else {
				if (deltaY > 0) {
					if (abs(deltaX) > deltaY)
						faceDir = 1;
					else 
						faceDir = 0;
				}
				else if (deltaY <= 0) {
					if (abs(deltaX) > abs(deltaY))
						faceDir = 1;
					else 
						faceDir = 2;
				}
			}
			deltaX = abs(deltaX);
			deltaY = abs(deltaY);
			if (deltaX > deltaY) {
				deltaX = deltaY;
				deltaY = abs(ch->x - self->x);
			}
			if (monster > -1) {
				if (deltaX == 0) {
					if (deltaY <= 8 && monstersInfo[monster].weakEnergy)
						exevoGranVisLuxCount++;
					else if (deltaY <= 6) {
						if (monstersInfo[monster].weakEarth)
							egmTeraCount++;
						if (monstersInfo[monster].weakEnergy) {
							exevoVisLuxCount++;
							egmVisCount++;
						}  // end if
					}  // end if
					else if (deltaY <= 5) {
						if (monstersInfo[monster].weakEnergy)
							exevoVisHurCount++;
						if (monstersInfo[monster].weakEarth)
							exevoTeraHurCount++;
						if (monstersInfo[monster].weakFire)
							egmFlamCount++;						
						if (monstersInfo[monster].weakIce)
							egmFrigoCount++;						
					}  // end if
					else if (deltaY <= 3) {
						if (monstersInfo[monster].weakHoly)
							exevoMasSanCount++;
						if (monstersInfo[monster].weakPhysical)
							exoriMasCount++;
						if (monstersInfo[monster].weakFire && deltaY > 0)
							exevoFlamHurCount++;						
						if (monstersInfo[monster].weakIce && deltaY > 0)
							exevoFrigoHurCount++;						
					}  // end if
					else if (deltaY <= 1 && monstersInfo[monster].weakPhysical)
						exoriCount++;
				}  // end if
				else if (deltaX == 1) {
					if (deltaY <= 5) {
						if (monstersInfo[monster].weakEarth) {
							if (deltaY > 2)
								exevoTeraHurCount++;
							egmTeraCount++;
						}  // end if
						if (monstersInfo[monster].weakEnergy) {
							if (deltaY > 2) 
								exevoVisHurCount++;
							egmVisCount++;
						}  // end if
					}  // end if
					else if (deltaY <= 4) {
						if (monstersInfo[monster].weakFire)
							egmFlamCount++;						
						if (monstersInfo[monster].weakIce)
							egmFrigoCount++;						
					}  // end if
					else if (deltaY <= 3) {
						if (monstersInfo[monster].weakHoly)
							exevoMasSanCount++;
						if (monstersInfo[monster].weakPhysical)
							exoriMasCount++;
						if (monstersInfo[monster].weakFire && deltaY > 1)
							exevoFlamHurCount++;						
						if (monstersInfo[monster].weakIce && deltaY > 1)
							exevoFrigoHurCount++;						
					}  // end if
					else if (deltaY <= 1 && monstersInfo[monster].weakPhysical)
						exoriCount++;
				}  // end if
				else if (deltaX == 2) {
					if (deltaY <= 5 && monstersInfo[monster].weakEarth)
						egmTeraCount++;
					else if (deltaY <= 4) {
						if (monstersInfo[monster].weakEnergy)
							egmVisCount++;
						if (monstersInfo[monster].weakFire)
							egmFlamCount++;						
					}  // end if
					else if (deltaY <= 3) {
						if (monstersInfo[monster].weakFire && deltaY > 2)
							exevoFlamHurCount++;						
						if (monstersInfo[monster].weakIce) {
							if (deltaY > 2)
								exevoFrigoHurCount++;						
							egmFrigoCount++;
						}  // end if
					}  // end if
					else if (deltaY <= 2) {
						if (monstersInfo[monster].weakHoly)
							exevoMasSanCount++;
						if (monstersInfo[monster].weakPhysical)
							exoriMasCount++;
					}  // end if
				}  // end if
				else if (deltaX == 3) {
					if (deltaY <= 4 && monstersInfo[monster].weakEarth)
						egmTeraCount++;
					else if (deltaY <= 3) {
						if (monstersInfo[monster].weakEnergy)
							egmVisCount++;
						if (monstersInfo[monster].weakFire)
							egmFlamCount++;
					}  // end if
					else if (deltaY <= 2 && monstersInfo[monster].weakIce)
						egmFrigoCount++;
					else if (deltaY <= 1) {
						if (monstersInfo[monster].weakHoly)
							exevoMasSanCount++;
						if (monstersInfo[monster].weakPhysical)
							exoriMasCount++;
					}  // end if
				}  // end if
				delete ch;
			} // end if			
		} // end if
	} // end for
	
	if (config->exori || config->exoriGran && exoriCount > 2)
		returnSpell = 1;
	if (config->exoriMas && exoriMasCount > 2 && exoriMasCount > exoriCount)
		returnSpell = 2;
	if (config->exevoMasSan && exevoMasSanCount > 2 && exevoMasSanCount > exoriMasCount)	
		returnSpell = 3;
	if (config->exevoFlamHur && exevoFlamHurCount > 2 && exevoMasSanCount > exevoFlamHurCount)
		returnSpell = 4;
	if (config->exevoFrigoHur && exevoFrigoHurCount > 2 && exevoFrigoHurCount > exevoFlamHurCount)
		returnSpell = 5;
	if (config->exevoTeraHur && exevoTeraHurCount > 2 && exevoTeraHurCount > exevoFrigoHurCount)
		returnSpell = 6;
	if (config->exevoVisHur && exevoVisHurCount > 2 && exevoVisHurCount > exevoTeraHurCount)
		returnSpell = 7;
	if (config->exevoVisLux && exevoVisLuxCount > 2 && exevoVisLuxCount > exevoVisHurCount)	
		returnSpell = 8;
	if (config->exevoGranVisLux && exevoGranVisLuxCount > 2 && exevoGranVisLuxCount > exevoVisLuxCount)
		returnSpell = 9;
	if (config->exevoGranMasVis && egmVisCount > 2 && egmVisCount > exevoGranVisLuxCount)	
		returnSpell = 10;
	if (config->exevoGranMasFlam && egmFlamCount > 2 && egmFlamCount > egmVisCount)
		returnSpell = 11;
	if (config->exevoGranMasTera && egmTeraCount > 2 && egmTeraCount > egmFlamCount)
		returnSpell = 12;
	if (config->exevoGranMasFrigo && egmFrigoCount > 2 && egmFrigoCount > egmTeraCount)
		returnSpell = 13;
			
	return returnSpell;
}
