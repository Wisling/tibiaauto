// Alarm.cpp: implementation of the Alarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_autogo.h"
#include "Alarm.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include "IPCBackPipeProxy.h"
#include "TibiaContainer.cpp" //What am I missing???  Why do I have to include a .cpp to get this to compile???? Other modules call the .h and work fine!
#include "ModuleUtil.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Alarm::Alarm() {
	alarmDescriptor = "";
	spell = "";
	sound = "";
	logEvents = false;
	maximize = false;
	shutdown = false;
	killTibia = false;
	logout = false;
	attack = false;
	depot = false;
	start = false;
	runaway = true;
	screenshot = 0;
	intTrigger = -1;
	strTrigger = "";
	condition = 0;
	attribute = 0;
	alarmType = 0;

	onScreenAt = 0;
	spellCast = false;
	screenshotsTaken = 0;
	timeLastSS = 0;
	modulesStarted = false;
	modulesSuspended = false;
	eventLogged = false;
	maximized = false;
	flashed = false;
	cavebotForced = false;
}

Alarm::Alarm(int type, int attr, int cond, int trig, bool run, bool sta, bool dep, CString spe, int scr, bool atk, bool log, bool kill, bool shut, bool max, CString audio, bool event, list<CString> beginModules, list<CString> endModules) {
	alarmDescriptor = "";
	startModules = beginModules;
	stopModules = endModules;
	spell = spe;
	sound = audio;
	logEvents = event;
	maximize = max;
	shutdown = shut;
	killTibia = kill;
	logout = log;
	attack = atk;
	depot = dep;
	start = sta;
	runaway = run;
	screenshot = scr;
	intTrigger = trig;
	condition = cond;
	attribute = attr;
	alarmType = type;

	onScreenAt = 0;
	spellCast = false;
	screenshotsTaken = 0;
	timeLastSS = 0;
	modulesStarted = false;
	modulesSuspended = false;
	eventLogged = false;
	maximized = false;
	flashed = false;
	cavebotForced = false;
}

Alarm::~Alarm(){

}

//////////////////////////////////////////////////////////////////////
// Setters
//////////////////////////////////////////////////////////////////////

void Alarm::setType(int type) {
	alarmType = type;
}

void Alarm::setAttribute(int attrib) {
	attribute = attrib;
}

void Alarm::setCondition(int cond) {
	condition = cond;
}

void Alarm::setTrigger(int trig) {
	intTrigger = trig;
}

void Alarm::setTrigger(CString trig) {
	strTrigger = trig;
}
void Alarm::setStartModules(list<CString> beginModules) {
	startModules = beginModules;
}

void Alarm::setStopModules(list<CString> endModules) {
	stopModules = endModules;
}

void Alarm::setGoToRunaway(bool onOff) {
	runaway = onOff;
}

void Alarm::setGoToStart(bool onOff) {
	start = onOff;
}

void Alarm::setGoToDepot(bool onOff) {
	depot = onOff;
}

void Alarm::setCastSpell(CString spellToCast) {
	spell = spellToCast;
}

void Alarm::setTakeScreenshot(int screenshotOption) {
	screenshot = screenshotOption;
}

void Alarm::setAttack(bool onOff) {
	attack = onOff;
}

void Alarm::setLogout(bool onOff) {
	logout = onOff;
}

void Alarm::setKillClient(bool onOff) {
	killTibia = onOff;
}

void Alarm::setShutdownComputer(bool onOff) {
	shutdown = onOff;
}

void Alarm::setMaximizeTibia(bool onOff) {
	maximize = onOff;
}

void Alarm::setAudioAlarm(CString waveToPlay) {
	sound = waveToPlay;
}

void Alarm::setLogEvents(bool onOff) {
	logEvents = onOff;
}

//////////////////////////////////////////////////////////////////////
// Actions/Accessors
//////////////////////////////////////////////////////////////////////

list<CString> Alarm::doStopModules() {
	return stopModules;
}

list<CString> Alarm::doStartModules() {
	return startModules;
}

bool Alarm::doLogout() {
	return logout;
}

bool Alarm::doKillClient() {
	return killTibia;
}

bool Alarm::doShutdownComputer() {
	return shutdown;
}

bool Alarm::doMaximizeClient() {
	return maximize;
}

CString Alarm::doAlarm() {
	return sound;
}

bool Alarm::doLogEvents() {
	return logEvents;
}

int Alarm::doTakeScreenshot() {
	return screenshot;
}

bool Alarm::doGoToRunaway() {
	return runaway;
}

bool Alarm::doGoToStart() {
	return start;
}

bool Alarm::doGoToDepot() {
	return depot;
}

CString Alarm::doCastSpell() {
	return spell;
}

bool Alarm::doAttack() {
	return attack;
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

void Alarm::initializeCharacter() {
	//Initialize the character's starting info 
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();
	lvlInit = self->lvl;
	mlvlInit = self->mlvl;
	skillFishInit = self->skillFish;
	skillFistInit = self->skillFist;
	skillClubInit = self->skillClub;
	skillSwordInit = self->skillSword;
	skillAxeInit = self->skillAxe;
	skillDistInit = self->skillDist;
	skillShieldInit = self->skillShield;
	positionXInit = self->x;
	positionYInit = self->y;
	positionZInit = self->z;
}

bool Alarm::checkAlarm(char whiteList[100][32], int options) {
	bool retval = false;
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();
	switch (alarmType) {
	case SKILL:
		switch (condition) {
		case LEVELUP:
			switch (attribute) {
			case ALL:
				retval |= self->lvl > lvlInit ? true : false;
				retval |= self->mlvl > mlvlInit ? true : false;
				retval |= self->skillFish > skillFishInit ? true : false;
				retval |= self->skillFist > skillFistInit ? true : false;
				retval |= self->skillClub > skillClubInit ? true : false;
				retval |= self->skillSword > skillSwordInit ? true : false;
				retval |= self->skillAxe > skillAxeInit ? true : false;
				retval |= self->skillDist > skillDistInit ? true : false;
				retval |= self->skillShield > skillShieldInit ? true : false;
				break;
			case LEVEL:
				return self->lvl > lvlInit ? true : false;
				break;
			case MAGICLVL:
				return self->mlvl > mlvlInit ? true : false;
				break;
			case FISHING:
				return self->skillFish > skillFishInit ? true : false;
				break;
			case FIST:
				return self->skillFist > skillFistInit ? true : false;
				break;
			case CLUB:
				return self->skillClub > skillClubInit ? true : false;
				break;
			case SWORDSKILL:
				return self->skillSword > skillSwordInit ? true : false;
				break;
			case AXE:
				return self->skillAxe > skillAxeInit ? true : false;
				break;
			case DISTANCE:
				return self->skillDist > skillDistInit ? true : false;
				break;
			case SHIELD:
				return self->skillShield > skillShieldInit ? true : false;
				break;
			}
			break;
		case PERCLVLACHIEVED: 
			switch (attribute) {
			case ALL:
				retval |= 100 - self->mlvlPercLeft >= intTrigger ? true : false;
				retval |= 100 - self->skillFishPercLeft >= intTrigger ? true : false;
				retval |= 100 - self->skillFistPercLeft >= intTrigger ? true : false;
				retval |= 100 - self->skillClubPercLeft >= intTrigger ? true : false;
				retval |= 100 - self->skillSwordPercLeft >= intTrigger ? true : false;
				retval |= 100 - self->skillAxePercLeft >= intTrigger ? true : false;
				retval |= 100 - self->skillDistPercLeft >= intTrigger ? true : false;
				retval |= 100 - self->skillShieldPercLeft >= intTrigger ? true : false;
				break;
			case LEVEL:
//				return ??;
				break;
			case MAGICLVL:
				return 100 - self->mlvlPercLeft >= intTrigger ? true : false;
				break;
			case FISHING:
				return 100 - self->skillFishPercLeft >= intTrigger ? true : false;
				break;
			case FIST:
				return 100 - self->skillFistPercLeft >= intTrigger ? true : false;
				break;
			case CLUB:
				return 100 - self->skillClubPercLeft >= intTrigger ? true : false;
				break;
			case SWORDSKILL:
				return 100 - self->skillSwordPercLeft >= intTrigger ? true : false;
				break;
			case AXE:
				return 100 - self->skillAxePercLeft >= intTrigger ? true : false;
				break;
			case DISTANCE:
				return 100 - self->skillDistPercLeft >= intTrigger ? true : false;
				break;
			case SHIELD:
				return 100 - self->skillShieldPercLeft >= intTrigger ? true : false;
				break;
			}
			break;
		case PERCLVLREMAINING: 
			switch (attribute) {
			case ALL:
				retval |= self->mlvlPercLeft <= intTrigger ? true : false;
				retval |= self->skillFishPercLeft <= intTrigger ? true : false;
				retval |= self->skillFistPercLeft <= intTrigger ? true : false;
				retval |= self->skillClubPercLeft <= intTrigger ? true : false;
				retval |= self->skillSwordPercLeft <= intTrigger ? true : false;
				retval |= self->skillAxePercLeft <= intTrigger ? true : false;
				retval |= self->skillDistPercLeft <= intTrigger ? true : false;
				retval |= self->skillShieldPercLeft <= intTrigger ? true : false;
				break;
			case LEVEL:
//				return ??;
				break;
			case MAGICLVL:
				return self->mlvlPercLeft <= intTrigger ? true : false;
				break;
			case FISHING:
				return self->skillFishPercLeft <= intTrigger ? true : false;
				break;
			case FIST:
				return self->skillFistPercLeft <= intTrigger ? true : false;
				break;
			case CLUB:
				return self->skillClubPercLeft <= intTrigger ? true : false;
				break;
			case SWORDSKILL:
				return self->skillSwordPercLeft <= intTrigger ? true : false;
				break;
			case AXE:
				return self->skillAxePercLeft <= intTrigger ? true : false;
				break;
			case DISTANCE:
				return self->skillDistPercLeft <= intTrigger ? true : false;
				break;
			case SHIELD:
				return self->skillShieldPercLeft <= intTrigger ? true : false;
				break;
			}
			break;
		}
		break;
	case RESOURCE:
		switch (condition) {
		case EQUAL:
			switch(attribute) {
			case HP:
				return self->hp == intTrigger ? true : false;
				break;
			case MP:
				return self->mana == intTrigger ? true : false;
				break;
			case XP:
				return self->exp == intTrigger ? true : false;
				break;
			case SP:
				return self->soulPoints == intTrigger ? true : false;
				break;
			case STAMINA:
				return self->stamina == intTrigger ? true : false;
				break;
			case CAPACITY:
				return self->cap == intTrigger ? true : false;
				break;
			case SPACE:
				return spaceAvailable() == intTrigger ? true : false;
				break;
			}
			break;
		case LESS:
			switch(attribute) {
			case HP:
				return self->hp < intTrigger ? true : false;
				break;
			case MP:
				return self->mana < intTrigger ? true : false;
				break;
			case XP:
				return self->exp < intTrigger ? true : false;
				break;
			case SP:
				return self->soulPoints < intTrigger ? true : false;
				break;
			case STAMINA:
				return self->stamina < intTrigger ? true : false;
				break;
			case CAPACITY:
				return self->cap < intTrigger ? true : false;
				break;
			case SPACE:
				return spaceAvailable() < intTrigger ? true : false;
				break;
			}
			break;
		case MORE:
			switch(attribute) {
			case HP:
				return self->hp > intTrigger ? true : false;
				break;
			case MP:
				return self->mana > intTrigger ? true : false;
				break;
			case XP:
				return self->exp > intTrigger ? true : false;
				break;
			case SP:
				return self->soulPoints > intTrigger ? true : false;
				break;
			case STAMINA:
				return self->stamina > intTrigger ? true : false;
				break;
			case CAPACITY:
				return self->cap > intTrigger ? true : false;
				break;
			case SPACE:
				return spaceAvailable() > intTrigger ? true : false;
				break;
			}
			break;
		}
		break;
	case ITEMS:
		switch (condition) {
		case EQUAL:
			switch(attribute) {
			case 0:
				return countAllFood() == intTrigger ? true : false;
				break;
			default:
				return countAllItemsOfType(attribute) == intTrigger ? true : false;
				break;
			}
			break;
		case LESS:
			switch(attribute) {
			case 0:
				return countAllFood() < intTrigger ? true : false;
				break;
			default:
				return countAllItemsOfType(attribute) < intTrigger ? true : false;
				break;
			}
			break;
		case MORE:
			switch(attribute) {
			case 0:
				return countAllFood() > intTrigger ? true : false;
				break;
			default:
				return countAllItemsOfType(attribute) > intTrigger ? true : false;
				break;
			}
			break;
		}
		break;
	case PROXIMITY:
		switch (condition) {
		case NEARBY:
			switch(attribute) {
			case PLAYER:
				return playerOnScreen(whiteList, options);
				break;
			case MONSTER:
				return monsterOnScreen(whiteList, options);
				break;
			case GM:
				return gmOnScreen(whiteList, options);
				break;
			case BATTLELIST:
				return battleListActive();
				break;
			case BLACKSKULL:
				return skullOnScreen(BLACK_SKULL);
				break;
			case REDSKULL:
				return skullOnScreen(RED_SKULL);
				break;
			case GREENSKULL:
				return skullOnScreen(GREEN_SKULL);
				break;
			case YELLOWSKULL:
				return skullOnScreen(YELLOW_SKULL);
				break;
			case WHITESKULL:
				return skullOnScreen(WHITE_SKULL);
				break;
			}
			break;
		case DISAPPEARS:
			switch(attribute) {
			case PLAYER:
				return !playerOnScreen(whiteList, options);
				break;
			case MONSTER:
				return !monsterOnScreen(whiteList, options);
				break;
			case GM:
				return !gmOnScreen(whiteList, options);
				break;
			case BATTLELIST:
				return !battleListActive();
				break;
			case BLACKSKULL:
				return !skullOnScreen(BLACK_SKULL);
				break;
			case REDSKULL:
				return !skullOnScreen(RED_SKULL);
				break;
			case GREENSKULL:
				return !skullOnScreen(GREEN_SKULL);
				break;
			case YELLOWSKULL:
				return !skullOnScreen(YELLOW_SKULL);
				break;
			case WHITESKULL:
				return !skullOnScreen(WHITE_SKULL);
				break;
			}
			break;
		case ISONSCREENFOR: {
			bool onscreen;
			switch(attribute) {
			case PLAYER:
				onscreen = playerOnScreen(whiteList, options);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			case MONSTER:
				onscreen = monsterOnScreen(whiteList, options);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			case GM:
				onscreen = gmOnScreen(whiteList, options);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			case BATTLELIST:
				onscreen = battleListActive();
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			case BLACKSKULL:
				onscreen = skullOnScreen(BLACK_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			case REDSKULL:
				onscreen = skullOnScreen(RED_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			case GREENSKULL:
				onscreen = skullOnScreen(GREEN_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			case YELLOWSKULL:
				onscreen = skullOnScreen(YELLOW_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			case WHITESKULL:
				onscreen = skullOnScreen(WHITE_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					return true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				return false;
				break;
			}
			}
			break;
		}	
		break;
	case ONLINE:
		switch (condition) {
		case LOGON:
			switch (attribute) {
			case CURRENTPLAYERONLINE:
				return reader.getConnectionState() == 8;
				break;
			default:
				return vipOnline(attribute);
				break;
			}
			break;
		case LOGOFF:
			switch (attribute) {
			case CURRENTPLAYERONLINE:
				return reader.getConnectionState() != 8;
				break;
			default:
				return !vipOnline(attribute);
				break;
			}
			break;
		}
		break;
	case CHARACTER_STATUS:
		return reader.getSelfEventFlags() & (int)pow(2, attribute);
		break;
	case GENERAL:
		struct tibiaMessage *msg = triggerMessage();
		int isSpell = msg && (options & MESSAGE_IGNORE_SPELLS) && isSpellMessage(msg->msg);
		switch (attribute) {
		case ALLMESSAGES:
			retval = !isSpell && msg && (msg->type == 1 || msg->type == 2 || msg->type == 6);
			break;
		case PUBLICMESSAGES:
			retval =  !isSpell && msg && (msg->type == 1 || msg->type == 2);
			break;
		case PRIVATEMESSAGES:
			retval = !isSpell && msg && msg->type == 6;
			break;
		case CHARACTERMOVED:
			retval = positionXInit != self->x || positionYInit != self->y || positionZInit != self->z;
			break;
		}
		if (msg) delete msg;
		break;
	}
	return retval;
}

//////////////////////////////////////////////////////////////////////
// Triggers
//////////////////////////////////////////////////////////////////////

int Alarm::spaceAvailable() {
	CTibiaItemProxy itemProxy;	
	CMemReaderProxy reader;
	int space = 0;
	
	for (int contNr = 0; contNr < itemProxy.getValueForConst("maxContainers"); contNr++) {		
		CTibiaContainer *cont = reader.readContainer(contNr);
		if (cont->flagOnOff)
			space += cont->size - cont->itemsInside;
		delete cont;
	}	
	return space;
}

bool Alarm::playerOnScreen(char whiteList[100][32], int options) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	
	for (int creatureNr = 0; creatureNr < memConstData.m_memMaxCreatures; creatureNr++) {
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);		
		if (ch->visible) {		
			if (ch->tibiaId != self->tibiaId && (!OnList(whiteList,(char*)ch->name) ^ options & MAKE_BLACKLIST)) {				
				if (ch->z == self->z || (options & BATTLELIST_PARANOIAM) || (options & BATTLELIST_ANXIETY && abs(ch->z - self->z) <= 1)) {					
					if (ch->tibiaId < 0x40000000) {
						delete ch;
						delete self;							
						return true;
					}					
				}
			}			
		}
		delete ch;
	}
	delete self;
	return false;
}

bool Alarm::gmOnScreen(char whiteList[100][32],int options) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();

	for (int creatureNr = 0; creatureNr < memConstData.m_memMaxCreatures; creatureNr++){
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);		
		if (ch->visible) {		
			if (ch->tibiaId != self->tibiaId && (!OnList(whiteList, (char*)ch->name) ^ options & MAKE_BLACKLIST)) {				
				if (ch->z == self->z || (options & BATTLELIST_PARANOIAM) || (options & BATTLELIST_ANXIETY && abs(ch->z - self->z) <= 1)) {					
					if (ch->name[0] == 'G' && ch->name[1] == 'M') { // this is GM						
						delete ch;
						delete self;						
						return true;
					}
					if (ch->name[0] == 'C' && ch->name[1] == 'M') { // this is CM						
						delete ch;
						delete self;						
						return true;
					}					
				}
			}		
		}
		delete ch;
	};
	delete self;
	return false;
}

bool Alarm::monsterOnScreen(char whiteList[100][32], int options) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	
	for (int creatureNr = 0; creatureNr < memConstData.m_memMaxCreatures; creatureNr++) {
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);
		if (ch->visible) {
			if (ch->tibiaId != self->tibiaId && (!OnList(whiteList, (char*)ch->name) ^ options & MAKE_BLACKLIST)) {				
				if (ch->z == self->z || (options & BATTLELIST_PARANOIAM) || (options & BATTLELIST_ANXIETY && abs(ch->z - self->z) <= 1)) {					
					if (ch->tibiaId >= 0x40000000) { // this is monster/npc
						delete ch;
						delete self;							
						return true;
					}					
				}
			}			
		}
		delete ch;
	};
	delete self;
	return false;
}

bool Alarm::battleListActive() {
	CMemReaderProxy reader;

	return (reader.readBattleListMax() >= 0 || reader.readBattleListMin() >= 0);	
}

bool Alarm::vipOnline(int iconIndex) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaVIPEntry *vip;

	for (int vipNr = 0; strcmp((vip = reader.readVIPEntry(vipNr))->name, ""); vipNr++) {
		if (vip->icon == iconIndex && vip->status == 1){
			delete vip;
			return true;
		}
		delete vip;
	}
	return false;
}

bool Alarm::skullOnScreen(int options) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
CString buf;	
	for (int creatureNr = 0; creatureNr < memConstData.m_memMaxCreatures; creatureNr++) {
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);		
		if (ch->visible) {
			if (ch->tibiaId != self->tibiaId) {				
				if (ch->z == self->z) {					
					if (ch->skulls == options) {
						delete ch;
						delete self;						
						return true;
					}
				}
			}			
		}
		delete ch;
	}
	delete self;
	return false;
}

//////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////

int Alarm::countAllItemsOfType(int objectId) {
	CTibiaItemProxy itemProxy;	
	CMemReaderProxy reader;
	int itemCount = 0;

	for (int contNr = 0; contNr < itemProxy.getValueForConst("maxContainers"); contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);		
		if (cont->flagOnOff)
			itemCount += cont->countItemsOfType(objectId);
		delete cont;
	}
	return itemCount;
}

int Alarm::countAllFood() {
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	int foodCount = 0;
	
	for (int contNr = 0; contNr < itemProxy.getValueForConst("maxContainers"); contNr++) {
		CTibiaItem *item = CModuleUtil::lookupItem(contNr, itemProxy.getFoodIdArrayPtr());
		if (item)
			foodCount += countAllItemsOfType(item->objectId);
	}	
	return foodCount;
}

bool Alarm::OnList(char whiteList[100][32],char name[]) {
	int i=0;
	while (IsCharAlphaNumeric(whiteList[i][0])) {
		if (!strcmpi(whiteList[i],name))
			return true;
		i++;
	}
	return false;
}

bool Alarm::isSpellMessage(char *msg) {
	const char *spellPre[] = {"ex",	"ad", "ut", "al", NULL};
	const char *spellSuf[] = {"ana", "eta", "evo", "ito", "ori", "ura", "ani", "iva", "amo", NULL};
	char newmsg[128] = {'\0'};

	//get 5 characters from msg discarding all spaces
	int count=0;
	for (int i = 0; count < 5 && msg[i]; i++) {
		if (msg[i] != ' ') {
			newmsg[count] = msg[i];
			count++;
		}
	}
	newmsg[count] = '\0';

	if (strlen(newmsg) != 5) return false;

	//if string starts with prefix and is = prefix+suffix, return 1
	for (int pos = 0; spellPre[pos]; pos++) {
		if (strnicmp(newmsg, spellPre[pos], 2) == 0) {
			for (int pos2 = 0; spellSuf[pos2]; pos2++) {
				char tmp[10];
				tmp[0]=0;
				strcat(tmp,spellPre[pos]);
				strcat(tmp,spellSuf[pos2]);
				tmp[5]='\0';
				if (strnicmp(newmsg, tmp, 5) == 0) {
					return true;
				}
			}
		}
	}
	return false;
}

struct tibiaMessage * Alarm::triggerMessage() {
	CMemReaderProxy reader;
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;

	if (backPipe.readFromPipe(&mess,1003)) {
		int infoType;
		int nickLen;
		int msgLen;
		char nickBuf[16384];
		char msgBuf[16384];
		
		memset(nickBuf,0,16384);
		memset(msgBuf,0,16384);
		memcpy(&infoType,mess.payload,sizeof(int));
		memcpy(&nickLen,mess.payload+4,sizeof(int));
		memcpy(&msgLen,mess.payload+8,sizeof(int));
		memcpy(nickBuf,mess.payload+12,nickLen);
		memcpy(msgBuf,mess.payload+12+nickLen,msgLen);
		
		CTibiaCharacter *self = reader.readSelfCharacter();

		if (strcmpi(nickBuf, self->name) != 0 && strcmpi(nickBuf, "Tibia Auto") != 0) {
			delete self;
			struct tibiaMessage *newMsg = new tibiaMessage();
			newMsg->type = infoType;
			strcpy(newMsg->msg, msgBuf);
			return newMsg;
		}
		delete self;
	}
	return NULL;
}




int Alarm::getAlarmType() {
	return alarmType;
}

int Alarm::getAttribute() {
	return attribute;
}

int Alarm::getCondition() {
	return condition;
}

CString Alarm::getStrTrigger() {
	return strTrigger;
}

int Alarm::getIntTrigger() {
	return intTrigger;
}

int Alarm::getManaCost() {
	return manaCost;
}

void Alarm::setManaCost(int cost) {
	manaCost = cost;
}

CString Alarm::getDescriptor() {
	return alarmDescriptor;
}

void Alarm::setDescriptor(CString descriptor) {
	alarmDescriptor = descriptor;
}
