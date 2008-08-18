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


// mod_seller.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_seller.h"

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
// CMod_SellerApp

BEGIN_MESSAGE_MAP(CMod_SellerApp, CWinApp)
//{{AFX_MSG_MAP(CMod_SellerApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
bool globalShouldGo = false;
HANDLE toolThreadHandle;

int findSeller(CConfigData *, int);
int moveToSeller(CConfigData *);
int sellItems(CConfigData *, int);
int isDepositing();
int isCavebotOn();
int countAllItemsOfType(int);
bool shouldGo(CConfigData *);
int individualShouldGo(CConfigData *, int);

DWORD WINAPI toolThreadProc( LPVOID lpParam ) {		
	CMemReaderProxy reader;
	CConfigData *config = (CConfigData *)lpParam;
	while (!toolThreadShouldStop) {
		Sleep(200);
		int attackedCreature = reader.getAttackedCreature();
		if (!globalShouldGo)
			globalShouldGo = shouldGo(config);
		if (globalShouldGo && !attackedCreature) {
			for (int i = 0; i < MAX_SELLERS; i++) {
				if (individualShouldGo(config, i)) {
					if (findSeller(config, i)) {
						reader.setGlobalVariable("caveboot_halfsleep","true");
						if (moveToSeller(config)) {
							if (sellItems(config, i))
								reader.setGlobalVariable("caveboot_halfsleep","false");
						}
					}
				}
			}
		}
		else
			globalShouldGo = false;
			reader.setGlobalVariable("caveboot_halfsleep","false");
	}
	reader.setGlobalVariable("caveboot_halfsleep","false");
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_SellerApp construction

CMod_SellerApp::CMod_SellerApp() {
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_SellerApp::~CMod_SellerApp() {
	if (m_configDialog)
		delete m_configDialog;
	delete m_configData;	
}

char * CMod_SellerApp::getName() {
	return "Auto seller";
}

int CMod_SellerApp::isStarted() {
	return m_started;
}

void CMod_SellerApp::start() {	
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

void CMod_SellerApp::stop() {
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

void CMod_SellerApp::showConfigDialog() {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	
	if (!m_configDialog) {
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

void CMod_SellerApp::configToControls() {
	if (m_configDialog)		
		m_configDialog->configToControls(m_configData);
}

void CMod_SellerApp::controlsToConfig() {
	if (m_configDialog)	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_SellerApp::disableControls() {
	if (m_configDialog)
		m_configDialog->disableControls();
}

void CMod_SellerApp::enableControls() {
	if (m_configDialog)
		m_configDialog->enableControls();
}


char *CMod_SellerApp::getVersion() {
	return "1.0";
}

int CMod_SellerApp::validateConfig(int showAlerts) {	
	return 1;
}

void CMod_SellerApp::resetConfig() {
	m_configData = new CConfigData();
}

void CMod_SellerApp::loadConfigParam(char *paramName,char *paramValue) {
}

char *CMod_SellerApp::saveConfigParam(char *paramName) {
	static char buf[1024];
	buf[0]=0;
	
	return buf;
}

char *CMod_SellerApp::getConfigParamName(int nr) {
	switch (nr) {
	default:
		return NULL;
	}
}

int findSeller(CConfigData *config, int traderNum) {
	//char buf[64];
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();
	if (config->targetX == self->x && config->targetY == self->y && config->targetZ == self->z) return 1; 
	for (int x = 0; x < 10; x++) {
		struct point nearestSell = CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->sellerList[traderNum].position[x].sellerX, config->sellerList[traderNum].position[x].sellerY, config->sellerList[traderNum].position[x].sellerZ, 0, config->path);
		if (nearestSell.x && nearestSell.y && nearestSell.z) {
			config->targetX = nearestSell.x;
			config->targetY = nearestSell.y;
			config->targetZ = nearestSell.z;
			delete self;
			return 1;
		}
		else if (x == 9) {
			config->targetX = config->targetY = config->targetZ = 0;
			//sprintf(buf, "Target Location: %d, %d, %d", config->targetX, config->targetY, config->targetZ);
			//AfxMessageBox(buf);
			delete self;
			return 0;
		}
	}
	return -1;
}

int moveToSeller(CConfigData *config) {
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

int sellItems(CConfigData *config, int traderNum) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CTibiaContainer *cont;
	int itemCount;
	int done = 0;
	for (int j = 0; j < 32; j++) {
		int objectId = itemProxy.getObjectId(config->sellItem[traderNum].tradeItem[j].itemName);
		for (int contNr = 0; contNr < 16; contNr++) {
			cont = reader.readContainer(contNr);
			int count = cont->itemsInside;
			for (int slotNr = count - 1; slotNr >= 0; slotNr--) {
				CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(slotNr);
				if (item->objectId == objectId) {
					itemCount = countAllItemsOfType(objectId);
					sender.say("Hi");
					Sleep (500);
					sender.sayNPC("trade");
					Sleep (500);
					//char buf[64];
					//sprintf(buf, "ObjectID: %d\nQuantity: 1", objectId);
					//AfxMessageBox(buf);
					sender.npcSell(objectId, itemCount);
					Sleep (500);
					sender.sayNPC("Yes");
					if (CModuleUtil::waitForItemsInsideChange(contNr, cont->itemsInside)) {
						delete cont;
						done = 1;
					}
					delete cont;	
					done = 0;
				}
			}
		}		
	}
	return done;
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
	for (contNr = 0; contNr < 8; contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);
		
		if (cont->flagOnOff)
			ret+=cont->countItemsOfType(objectId);
		delete cont;
	}
	return ret;
}

bool shouldGo(CConfigData *config) {
	//char buf[64];
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	int count = 0;
	for (int i = 0; i < MAX_SELLERS; i++) {
		for (int j = 0; j < 32; j++) {
			int objectId = itemProxy.getObjectId(config->sellItem[i].tradeItem[j].itemName);
			//sprintf(buf, "%s\nItem count: %d\nTrigger Quantity: %d", config->sellItem[i].tradeItem[j].itemName, countAllItemsOfType(objectId), config->sellItem[i].tradeItem[j].quantityBuySell);
			//AfxMessageBox(buf);
			count = countAllItemsOfType(objectId);
			if (count && count >= config->sellItem[i].tradeItem[j].quantityBuySell)
				return true;
			else 
				return false;
		}
	}
	return false;
}

int individualShouldGo(CConfigData *config, int traderNum) {
	//char buf[64];
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	for (int j = 0; j < 32; j++) {
		int objectId = itemProxy.getObjectId(config->sellItem[traderNum].tradeItem[j].itemName);
		//sprintf(buf, "Seller: %d\nObjectID: %d", traderNum+1, objectId);
		//AfxMessageBox(buf);
		if (countAllItemsOfType(objectId) >= 1)
			return true;
		else 
			return false;
	}
	return 0;
}

