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
int withdrawGold(CConfigData *config);
int isDepositing();
int isCavebotOn();
int countAllItemsOfType(int);
int shouldBank(CConfigData *);

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam ) {		
	int alreadySleeping = 0;
	int bankerInvoked = 0;
	char *var;
	CMemReaderProxy reader;
	CConfigData *config = (CConfigData *)lpParam;
	while (!toolThreadShouldStop) {
		Sleep(200);
		int attackedCreature = reader.getAttackedCreature();
		var=reader.getGlobalVariable("caveboot_halfsleep");
		if (var==NULL||strcmp(var,"true")) 
			alreadySleeping = 0; 
		else 
			alreadySleeping = 1;
		if (attackedCreature || alreadySleeping && !bankerInvoked) continue;
		
		//if (isDepositing() || !isCavebotOn() || shouldBank(config) && !attackedCreature) {
		if (shouldBank(config) && !attackedCreature) {
			if (findBanker(config)) {
				reader.setGlobalVariable("caveboot_halfsleep", "true");
				bankerInvoked = 1;
				if (moveToBanker(config)) {
					//AfxMessageBox("Yup, found the banker!");
					if (depositGold()) {
						if (config->cashOnHand && withdrawGold(config)) {							
							reader.setGlobalVariable("caveboot_halfsleep","false");
							bankerInvoked = 0;
						}
						getBalance();
					}
				}
			}
		}
		else {
			reader.setGlobalVariable("caveboot_halfsleep","false");
			bankerInvoked = 0;
		}
	}
	reader.setGlobalVariable("caveboot_halfsleep","false");
	bankerInvoked = 0;
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
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

void CMod_bankerApp::configToControls() {
	if (m_configDialog)		
		m_configDialog->configToControls(m_configData);
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
	if (!strcmp(paramName, "DepositTrigger")) m_configData->minimumGoldToBank = atoi(paramValue);
	if (!strcmp(paramName, "CashOnHand")) m_configData->cashOnHand = atoi(paramValue);

}

char *CMod_bankerApp::saveConfigParam(char *paramName) {
	static char buf[1024];
	buf[0]='\0';
	if (!strcmp(paramName, "BankerName")) sprintf(buf,"%s",m_configData->banker.bankerName);
	if (!strcmp(paramName, "DepositTrigger")) sprintf(buf,"%d",m_configData->minimumGoldToBank);
	if (!strcmp(paramName, "CashOnHand")) sprintf(buf,"%d",m_configData->cashOnHand);

	return buf;
}

char *CMod_bankerApp::getConfigParamName(int nr) {
	switch (nr) {
	case 0: return "BankerName";
	case 1: return "DepositTrigger";
	case 2: return "CashOnHand";
	default:
		return NULL;
	}
}

int findBanker(CConfigData *config) {
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();
	if (config->targetX == self->x && config->targetY == self->y && config->targetZ == self->z) return 1; 
	for (int x = 0; x < 10; x++) {
		struct point nearestBank = CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->banker.position[x].bankerX, config->banker.position[x].bankerY, config->banker.position[x].bankerZ, 0, config->path);
		if (nearestBank.x && nearestBank.y && nearestBank.z) {
			config->targetX = nearestBank.x;
			config->targetY = nearestBank.y;
			config->targetZ = nearestBank.z;
			delete self;
			return 1;
		}
		else if (x == 9) {
			config->targetX = config->targetY = config->targetZ = 0;
			delete self;
			return 0;
		}
	}
	return -1;
}

int moveToBanker(CConfigData *config) {
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();

	CModuleUtil::executeWalk(self->x,self->y,self->z,config->path);
	self = reader.readSelfCharacter();
	if (self->x == config->targetX && self->y == config->targetY && self->z == config->targetZ) {
		delete self;
		return 1;
	}
	else {
		delete self;
		return 0;
	}
}

void getBalance() {
	CPackSenderProxy sender;
	Sleep (RandomTimeBankerSay(strlen("balance")));
	sender.sayNPC("balance");
}

int depositGold() {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CTibiaContainer *cont;
	int goldId = itemProxy.getValueForConst("GP");
	int platId = itemProxy.getValueForConst("PlatinumCoin");
	int crystalId = itemProxy.getValueForConst("CrystalCoin");
	int foundInBag = 0;
	for (int contNr = 0; contNr < 16; contNr++) {
		cont = reader.readContainer(contNr);
		int count = cont->itemsInside;
		for (int slotNr = count - 1; slotNr >= 0; slotNr--) {
			CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(slotNr);
			if (item->objectId == goldId || item->objectId == platId || item->objectId == crystalId) {
				foundInBag = contNr;
				contNr = 16;
				slotNr = 0;
			}
		}
	}

	Sleep (RandomTimeBankerSay(strlen("hi")));
	sender.say("hi");
	Sleep(500);//Give time for NPC window to open
	Sleep (RandomTimeBankerSay(strlen("deposit all")));
	sender.sayNPC("deposit all");
	Sleep (RandomTimeBankerSay(strlen("yes")));
	sender.sayNPC("yes");
	if (CModuleUtil::waitForItemsInsideChange(foundInBag, cont->itemsInside)) {
		delete cont;
		return 1;
	}
	delete cont;
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
	char *var=reader.getGlobalVariable("cavebot_depositing");
	if (var==NULL||strcmp(var,"true")) {
		delete var;
		return 0;
	}
	else {
		delete var;
		return 1;
	}
}

int countAllItemsOfType(int objectId) {
	CMemReaderProxy reader;
	int contNr;
	int ret=0;
	for (contNr = 0; contNr < 16; contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);
		
		if (cont->flagOnOff)
			ret+=cont->countItemsOfType(objectId);
		delete cont;
	}
	return ret;
}

int shouldBank(CConfigData *config) {
	CTibiaItemProxy itemProxy;
	int objectId = itemProxy.getValueForConst("GP");
	int count = countAllItemsOfType(objectId);
	objectId = itemProxy.getValueForConst("PlatinumCoin");
	count += countAllItemsOfType(objectId) * 100;
	objectId = itemProxy.getValueForConst("CrystalCoin");
	count += countAllItemsOfType(objectId) * 10000;
	if (count >= config->minimumGoldToBank)
		return 1;
	else 
		return 0;
}

