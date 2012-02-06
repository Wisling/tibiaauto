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


// mod_banker.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_banker.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include "TibiaMapProxy.h"
#include <Tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CToolBankerState globalBankerState=CToolBankerState_notRunning;
int GUIx = 0,GUIy = 0,GUIz = 0;

/////////////////////////////////////////////////////////////////////////////
// CMod_bankerApp

BEGIN_MESSAGE_MAP(CMod_bankerApp, CWinApp)
//{{AFX_MSG_MAP(CMod_bankerApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions

int RandomTimeBankerSay(int length){
	return CModuleUtil::randomFormula(300+min(length*100,1200),200);//ranges from 300 to 1700
}

int findBanker(CConfigData *);
int moveToBanker(CConfigData *);
void getBalance();
int depositGold();
int changeGold();
int withdrawGold(CConfigData *config);
int isDepositing();
int isCavebotOn();
int countAllItemsOfType(int,bool);
int shouldBank(CConfigData *);
int canBank(CConfigData *);

// Required to be run more often than the return value is expected to change
int doneAttackingAndLooting(){
	CMemReaderProxy reader;
	static int lastAttackTm=0;
	int ret = GetTickCount()-lastAttackTm>3*1000 && !reader.getAttackedCreature();
	if (reader.getAttackedCreature()){
		lastAttackTm=GetTickCount();
	}
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam ) {		
	CMemReaderProxy reader;
	CConfigData *config = (CConfigData *)lpParam;

	int persistentShouldGo=0;
	int lastPathNotFoundTm=0;
	while (!toolThreadShouldStop) {
		Sleep(400);
		reader.mapGetPointItemsCount(point(8,-2,1),-1); // debugging line, please remove

		if (!persistentShouldGo && shouldBank(config)){
			persistentShouldGo=1;
		}
		const char* controller=reader.getGlobalVariable("walking_control");
		if (!persistentShouldGo
			&& config->stopByBanker
			&& (!strcmp(controller,"seller") || !strcmp(controller,"depotwalker"))
			&& canBank(config)){
			persistentShouldGo=1;
		}

		bool control= strcmp(reader.getGlobalVariable("walking_control"),"banker")==0;
		int modpriority=atoi(reader.getGlobalVariable("walking_priority"));
		// if wants control
		if (persistentShouldGo) {
			//if no path found let other modules work and wait 10 secs before trying again
			if (time(NULL)-lastPathNotFoundTm > 10){
				//if should have control, take it
				if (!control){
					if (atoi(config->modPriorityStr) > modpriority){
						reader.setGlobalVariable("walking_control","banker");
						reader.setGlobalVariable("walking_priority",config->modPriorityStr);
					} else {
						globalBankerState=CToolBankerState_halfSleep;
					}
				}
			} else {
				globalBankerState=CToolBankerState_noPathFound;
				//if has control, give it up
				if (control){
					reader.setGlobalVariable("walking_control","");
					reader.setGlobalVariable("walking_priority","0");
				}
			}
		} else { // if doesn't want control
			globalBankerState=CToolBankerState_notRunning;
			//if has control, give it up
			if (control){
				reader.setGlobalVariable("walking_control","");
				reader.setGlobalVariable("walking_priority","0");
			}
		}
		if (doneAttackingAndLooting() && strcmp(reader.getGlobalVariable("walking_control"),"banker")==0){
			if (findBanker(config)){
				globalBankerState=CToolBankerState_walking;
				if (moveToBanker(config)) {
					globalBankerState=CToolBankerState_talking;
					//AfxMessageBox("Yup, found the banker!");
					if (config->changeGold){
						changeGold();
					} else {
						if (depositGold()) {
							// High Priority Task
							reader.setGlobalVariable("walking_control","banker");
							reader.setGlobalVariable("walking_priority","9");

							if (config->cashOnHand) {
								withdrawGold(config);
							}
							getBalance();

							reader.setGlobalVariable("walking_control","banker");
							reader.setGlobalVariable("walking_priority",config->modPriorityStr);
						}
					}
					persistentShouldGo=0;
				}
			} else {
				lastPathNotFoundTm=time(NULL);
			}
		}
	}
	if (strcmp(reader.getGlobalVariable("walking_control"),"banker")==0){
		reader.setGlobalVariable("walking_control","");
		reader.setGlobalVariable("walking_priority","0");
	}
	globalBankerState=CToolBankerState_notRunning;
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_bankerApp construction

CMod_bankerApp::CMod_bankerApp() {
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_bankerApp::~CMod_bankerApp() {
	if (m_configDialog)
		delete m_configDialog;
	delete m_configData;	
}

char * CMod_bankerApp::getName() {
	return "Auto banker";
}

int CMod_bankerApp::isStarted() {
	return m_started;
}

void CMod_bankerApp::start() {	
	superStart();
	if (m_configDialog) 	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}
	
	DWORD threadId;
	
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_bankerApp::stop() {
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

void CMod_bankerApp::showConfigDialog() {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	
	if (!m_configDialog) {
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
		if (m_started) disableControls();
		else enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

void CMod_bankerApp::configToControls() {
	if (m_configDialog)		{
		m_configDialog->configToControls(m_configData);
	}
}

void CMod_bankerApp::controlsToConfig() {
	if (m_configDialog)	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_bankerApp::disableControls() {
	if (m_configDialog)
		m_configDialog->disableControls();
}

void CMod_bankerApp::enableControls() {
	if (m_configDialog)
		m_configDialog->enableControls();
}


char *CMod_bankerApp::getVersion() {
	return "2.0";
}

int CMod_bankerApp::validateConfig(int showAlerts) {	
	return 1;
}

void CMod_bankerApp::resetConfig() {
	m_configData = new CConfigData();
}

void CMod_bankerApp::loadConfigParam(char *paramName,char *paramValue) {
	if (!strcmp(paramName, "BankerName")) sprintf(m_configData->banker.bankerName, "%s", paramValue);
	if (!strcmp(paramName, "BankerPos")) sscanf(paramValue,"(%d,%d,%d)",&(m_configData->banker.bankerX),&(m_configData->banker.bankerY),&(m_configData->banker.bankerZ));
	if (!strcmp(paramName, "DepositTrigger")) m_configData->minimumGoldToBank = atoi(paramValue);
	if (!strcmp(paramName, "CashOnHand")) m_configData->cashOnHand = atoi(paramValue);
	if (!strcmp(paramName, "ModPriority")) strcpy(m_configData->modPriorityStr,paramValue);
	if (!strcmp(paramName, "ChangeGold")) m_configData->changeGold = atoi(paramValue);
	if (!strcmp(paramName, "CapsLimit")) m_configData->capsLimit = atoi(paramValue);
	if (!strcmp(paramName, "StopByBanker")) m_configData->stopByBanker = atoi(paramValue);
	if (!strcmp(paramName, "DrawUpTo")) m_configData->drawUpTo = atoi(paramValue);

}

char *CMod_bankerApp::saveConfigParam(char *paramName) {
	static char buf[1024];
	buf[0]='\0';
	if (!strcmp(paramName, "BankerName")) sprintf(buf,"%s",m_configData->banker.bankerName);
	if (!strcmp(paramName, "BankerPos")) sprintf(buf,"(%d,%d,%d)",m_configData->banker.bankerX,m_configData->banker.bankerY,m_configData->banker.bankerZ);
	if (!strcmp(paramName, "DepositTrigger")) sprintf(buf,"%d",m_configData->minimumGoldToBank);
	if (!strcmp(paramName, "CashOnHand")) sprintf(buf,"%d",m_configData->cashOnHand);
	if (!strcmp(paramName, "ModPriority")) strcpy(buf,m_configData->modPriorityStr);
	if (!strcmp(paramName, "ChangeGold")) sprintf(buf,"%d",m_configData->changeGold);
	if (!strcmp(paramName, "CapsLimit")) sprintf(buf,"%d",m_configData->capsLimit);
	if (!strcmp(paramName, "StopByBanker")) sprintf(buf,"%d",m_configData->stopByBanker);
	if (!strcmp(paramName, "DrawUpTo")) sprintf(buf,"%d",m_configData->drawUpTo);

	return buf;
}

char *CMod_bankerApp::getConfigParamName(int nr) {
	switch (nr) {
	case 0: return "BankerName";
	case 1: return "BankerPos";
	case 2: return "DepositTrigger";
	case 3: return "CashOnHand";
	case 4: return "ModPriority";
	case 5: return "ChangeGold";
	case 6: return "CapsLimit";
	case 7: return "StopByBanker";
	case 8: return "DrawUpTo";
	default:
		return NULL;
	}
}

int findBanker(CConfigData *config) {
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();
	if (config->targetX == self->x && config->targetY == self->y && config->targetZ == self->z){
		memset(config->path,0,15);
		delete self; 
		return 1;
	}
	GUIx = config->banker.bankerX;
	GUIy = config->banker.bankerY;
	GUIz = config->banker.bankerZ;
	struct point nearestBank = CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->banker.bankerX, config->banker.bankerY, config->banker.bankerZ, 0, config->path,3);
	if (nearestBank.x && nearestBank.y && nearestBank.z) {
		config->targetX = nearestBank.x;
		config->targetY = nearestBank.y;
		config->targetZ = nearestBank.z;
		delete self;
		return 1;
	}
	delete self;
	return 0;
}

int shouldKeepWalking() {
	static lastAttackTime=0;
	CMemReaderProxy reader;
	if (!reader.getAttackedCreature()){
		const char *var=reader.getGlobalVariable("autolooterTm");
		if (strcmp(var,"")==0){
			if (lastAttackTime<time(NULL)-3)
				return 1;
			else
				return 0;
		}
	}
	lastAttackTime=time(NULL);
	return 0;
}

int moveToBanker(CConfigData *config) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();

	static int positionFound=0;
	if (shouldKeepWalking()){
		//Find a location close enough to NPC
		if (!positionFound){
			CTibiaCharacter* self = reader.readSelfCharacter();
			CModuleUtil::executeWalk(self->x,self->y,self->z,config->path);
			if (self->x == config->targetX && self->y == config->targetY && self->z == config->targetZ) {
				positionFound=1;
			}
			delete self;
		} else { //Approach NPC after finding them
			for (int i=0;i<memConstData.m_memMaxCreatures;i++){
				CTibiaCharacter* mon=reader.readVisibleCreature(i);
				// since banker.bankerName may include city, match first part of name
				if (mon->tibiaId==0){
					delete mon;
					break;
				}
				int len=strlen(mon->name);
				if (strncmp(config->banker.bankerName,mon->name,len)==0 && (config->banker.bankerName[len]==0 || config->banker.bankerName[len]==' ')){
					for (int tries=0;tries<2;tries++){ // should only need 1 try, but we'd need to start over if we don't make it
						CTibiaCharacter* self = reader.readSelfCharacter();
						delete mon;
						mon=reader.readVisibleCreature(i);

						struct point nearestBank=point(0,0,0);
						int rad=2;
						while (nearestBank.x==0 && rad<=3){//find paths increasingly farther away
							nearestBank= CModuleUtil::findPathOnMap(self->x, self->y, self->z, mon->x, mon->y, mon->z, 0, config->path,rad++);
						}
						if (nearestBank.x && nearestBank.y && nearestBank.z==self->z){
							CModuleUtil::executeWalk(self->x,self->y,self->z,config->path);
							if (CModuleUtil::waitToStandOnSquare(nearestBank.x, nearestBank.y)){
								delete mon;
								delete self;
								return 1;
							}
						}
						delete self;
					}
				}
				delete mon;
			}
			positionFound=0;
		}
	}
	return 0;
}

void getBalance() {
	CPackSenderProxy sender;
	Sleep (RandomTimeBankerSay(strlen("balance")));
	sender.sayNPC("balance");
}

int depositGold() {
	CMemReaderProxy reader;
	CPackSenderProxy sender;

	CTibiaCharacter* self=reader.readSelfCharacter();
	float origcaps=self->cap;
	delete self;

	Sleep (RandomTimeBankerSay(strlen("hi")));
	sender.say("hi");
	Sleep(500);//Give time for NPC window to open
	Sleep (RandomTimeBankerSay(strlen("deposit all")));
	sender.sayNPC("deposit all");
	Sleep (RandomTimeBankerSay(strlen("yes")));
	sender.sayNPC("yes");
	if (CModuleUtil::waitForCapsChange(origcaps)) {
		return 1;
	}
	return 0;
}

int withdrawGold(CConfigData *config) {
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaCharacter *self = reader.readSelfCharacter();
	char withdrawBuf[32];

	sprintf(withdrawBuf, "withdraw %d", config->cashOnHand);
	Sleep (RandomTimeBankerSay(strlen(withdrawBuf)));
	sender.sayNPC(withdrawBuf);
	Sleep (RandomTimeBankerSay(strlen("yes")));
	sender.sayNPC("yes");

	if (CModuleUtil::waitForCapsChange(self->cap)) {
		delete self;
		return 1;
	}
	delete self;	
	return 0;
}

int changeGold() {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;

	CTibiaCharacter* self=reader.readSelfCharacter();
	float origcaps=self->cap;
	delete self;
	int retval=0;

	int goldId = itemProxy.getValueForConst("GP");
	int goldCount = countAllItemsOfType(goldId,true);

	char buf[128];
	Sleep (RandomTimeBankerSay(strlen("hi")));
	sender.say("hi");
	Sleep(500);//Give time for NPC window to open
	if (goldCount>=100){
		Sleep (RandomTimeBankerSay(strlen("change gold")));
		sender.sayNPC("change gold");
		sprintf(buf,"%d",goldCount/100);
		Sleep (RandomTimeBankerSay(strlen(buf)));
		sender.sayNPC(buf);
		Sleep (RandomTimeBankerSay(strlen("yes")));
		sender.sayNPC("yes");
	}

	if (CModuleUtil::waitForCapsChange(origcaps)) retval=1;

	int platId = itemProxy.getValueForConst("PlatinumCoin");
	int platCount = countAllItemsOfType(platId,true);
	if (platCount>=100){
		Sleep (RandomTimeBankerSay(strlen("change platinum")));
		sender.sayNPC("change platinum");
		Sleep (RandomTimeBankerSay(strlen("crystal")));
		sender.sayNPC("crystal");
		sprintf(buf,"%d",platCount/100);
		Sleep (RandomTimeBankerSay(strlen(buf)));
		sender.sayNPC(buf);
		Sleep (RandomTimeBankerSay(strlen("yes")));
		sender.sayNPC("yes");
	}

	if (CModuleUtil::waitForCapsChange(origcaps)) retval=1;

	return retval;
}

int isCavebotOn() {
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
	if (hSnap) {
		MODULEENTRY32 lpModule;
		lpModule.dwSize = sizeof(MODULEENTRY32);
		
		Module32First(hSnap,&lpModule);
		do {
			if (strcmpi(lpModule.szModule,"mod_cavebot.dll") == 0){
				FARPROC isStarted;
				isStarted = GetProcAddress(lpModule.hModule,"isStarted");
				if (isStarted) {
					if (isStarted())
						return 1;
					else
						return 0;
				}
			}
		}while (Module32Next(hSnap,&lpModule));		
		CloseHandle(hSnap);
	}
	return -1;
}

int isDepositing() {
	CMemReaderProxy reader;
	const char *var=reader.getGlobalVariable("cavebot_depositing");
	return strcmp(var,"true")==0;
}

int countAllItemsOfType(int objectId,bool includeSlots) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	int contNr;
	int ret=0;
	for (contNr = 0; contNr < memConstData.m_memMaxContainers; contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);
		
		if (cont->flagOnOff)
			ret+=cont->countItemsOfType(objectId);
		delete cont;
	}
	if (includeSlots){
		CMemConstData memConstData = reader.getMemConstData();
		for (int slotNr = 0; slotNr < 10; slotNr++) { // Loops through all 10 inventory slots(backwards)
			CTibiaItem *item = reader.readItem(memConstData.m_memAddressSlotArrow+slotNr*memConstData.m_memLengthItem);
			if (item->objectId==objectId)
				ret += item->quantity?item->quantity:1;
			delete item;
		}
	}
	return ret;
}

int shouldBank(CConfigData *config) {
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();

	CTibiaCharacter* self = reader.readSelfCharacter();
	int belowCaps=self->cap < config->capsLimit;
	delete self;

	
	int goldId = itemProxy.getValueForConst("GP");
	int platId = itemProxy.getValueForConst("PlatinumCoin");
	int crystalId = itemProxy.getValueForConst("CrystalCoin");
	int goldCount = countAllItemsOfType(goldId,true);
	int platCount = countAllItemsOfType(platId,true);
	int crystalCount = countAllItemsOfType(crystalId,true);

	int totalCash=goldCount+platCount*100+crystalCount*10000;
	int canChange=goldCount>=100 || platCount>=100;

	if (belowCaps){
		if (!config->changeGold && totalCash > config->cashOnHand || config->changeGold && canChange)
			return 1;
	}
	if (!config->changeGold){
		if (totalCash >= config->minimumGoldToBank){
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}

int canBank(CConfigData *config){
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();

	CTibiaCharacter* self = reader.readSelfCharacter();
	delete self;

	
	int goldId = itemProxy.getValueForConst("GP");
	int platId = itemProxy.getValueForConst("PlatinumCoin");
	int crystalId = itemProxy.getValueForConst("CrystalCoin");
	int goldCount = countAllItemsOfType(goldId,true);
	int platCount = countAllItemsOfType(platId,true);
	int crystalCount = countAllItemsOfType(crystalId,true);

	int totalCash=goldCount+platCount*100+crystalCount*10000;
	int canChange=goldCount>=100 || platCount>=100;

	if (!config->changeGold && totalCash > config->cashOnHand || config->changeGold && canChange || totalCash < config->cashOnHand && config->drawUpTo)
		return 1;
	return 0;
}
void CMod_bankerApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	skin.SetButtonSkin(	m_configDialog->m_OK);
	skin.SetButtonSkin(	m_configDialog->m_enable);

	if (m_configDialog)
		m_configDialog->Invalidate();
}

