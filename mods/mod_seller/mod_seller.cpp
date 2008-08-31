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
HANDLE toolThreadHandle;

int findSeller(CConfigData *, int);
int moveToSeller(CConfigData *);
int sellItems(CConfigData *, int);
int buyItems(CConfigData *, int);
int isDepositing();
int noSpace();
int isCavebotOn();
int countAllItemsOfType(int);
bool shouldGo(CConfigData *);
int individualShouldGo(CConfigData *, int);

DWORD WINAPI toolThreadProc( LPVOID lpParam ) {		
	CMemReaderProxy reader;
	CConfigData *config = (CConfigData *)lpParam;
	int allAtOnce = 0;
	int buyOrSell = 0;
	char *var;
	int alreadySleeping = 0;
	int sellerInvoked = 0;
	while (!toolThreadShouldStop) {
/*		for (int i=0;i<MAX_SELLERS;i++) {
			char buf[2048];
			sprintf(buf, "Seller: %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d\nWaypoint: %d %d %d", i, 
				config->sellerList[i].position[0].sellerX, config->sellerList[i].position[0].sellerY, config->sellerList[i].position[0].sellerZ,
				config->sellerList[i].position[1].sellerX, config->sellerList[i].position[1].sellerY, config->sellerList[i].position[1].sellerZ,
				config->sellerList[i].position[2].sellerX, config->sellerList[i].position[2].sellerY, config->sellerList[i].position[2].sellerZ,
				config->sellerList[i].position[3].sellerX, config->sellerList[i].position[3].sellerY, config->sellerList[i].position[3].sellerZ,
				config->sellerList[i].position[4].sellerX, config->sellerList[i].position[4].sellerY, config->sellerList[i].position[4].sellerZ,
				config->sellerList[i].position[5].sellerX, config->sellerList[i].position[5].sellerY, config->sellerList[i].position[5].sellerZ,
				config->sellerList[i].position[6].sellerX, config->sellerList[i].position[6].sellerY, config->sellerList[i].position[6].sellerZ,
				config->sellerList[i].position[7].sellerX, config->sellerList[i].position[7].sellerY, config->sellerList[i].position[7].sellerZ,
				config->sellerList[i].position[8].sellerX, config->sellerList[i].position[8].sellerY, config->sellerList[i].position[8].sellerZ,
				config->sellerList[i].position[9].sellerX, config->sellerList[i].position[8].sellerY, config->sellerList[i].position[9].sellerZ);
			AfxMessageBox(buf);
			sprintf(buf, "Seller: %d\nTrigger: %s %d %d\nTrigger: %s %d %d\nTrigger: %s %d %d\nTrigger: %s %d %d\nTrigger: %s %d %d\nTrigger: %s %d %d\nTrigger: %s %d %d\nTrigger: %s %d %d\nTrigger: %s %d %d\nTrigger: %s %d %d", i, 
				config->sellItem[i].tradeItem[0].itemName, config->sellItem[i].tradeItem[0].quantityBuySell, config->sellItem[i].tradeItem[0].salePrice,
				config->sellItem[i].tradeItem[1].itemName, config->sellItem[i].tradeItem[1].quantityBuySell, config->sellItem[i].tradeItem[1].salePrice,
				config->sellItem[i].tradeItem[2].itemName, config->sellItem[i].tradeItem[2].quantityBuySell, config->sellItem[i].tradeItem[2].salePrice,
				config->sellItem[i].tradeItem[3].itemName, config->sellItem[i].tradeItem[3].quantityBuySell, config->sellItem[i].tradeItem[3].salePrice,
				config->sellItem[i].tradeItem[4].itemName, config->sellItem[i].tradeItem[4].quantityBuySell, config->sellItem[i].tradeItem[4].salePrice,
				config->sellItem[i].tradeItem[5].itemName, config->sellItem[i].tradeItem[5].quantityBuySell, config->sellItem[i].tradeItem[5].salePrice,
				config->sellItem[i].tradeItem[6].itemName, config->sellItem[i].tradeItem[6].quantityBuySell, config->sellItem[i].tradeItem[6].salePrice,
				config->sellItem[i].tradeItem[7].itemName, config->sellItem[i].tradeItem[7].quantityBuySell, config->sellItem[i].tradeItem[7].salePrice,
				config->sellItem[i].tradeItem[8].itemName, config->sellItem[i].tradeItem[8].quantityBuySell, config->sellItem[i].tradeItem[8].salePrice,
				config->sellItem[i].tradeItem[9].itemName, config->sellItem[i].tradeItem[9].quantityBuySell, config->sellItem[i].tradeItem[9].salePrice);
			AfxMessageBox(buf);
		}

			sprintf(buf, "Buyer: %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\nTrigger: %s %d %d %d\n", i, 
				config->buyItem[i].tradeItem[0].itemName, config->buyItem[i].tradeItem[0].quantityBuySell, config->buyItem[i].tradeItem[0].salePrice, config->buyItem[i].tradeItem[0].triggerQuantity,
				config->buyItem[i].tradeItem[1].itemName, config->buyItem[i].tradeItem[1].quantityBuySell, config->buyItem[i].tradeItem[1].salePrice, config->buyItem[i].tradeItem[1].triggerQuantity,
				config->buyItem[i].tradeItem[2].itemName, config->buyItem[i].tradeItem[2].quantityBuySell, config->buyItem[i].tradeItem[2].salePrice, config->buyItem[i].tradeItem[2].triggerQuantity,
				config->buyItem[i].tradeItem[3].itemName, config->buyItem[i].tradeItem[3].quantityBuySell, config->buyItem[i].tradeItem[3].salePrice, config->buyItem[i].tradeItem[3].triggerQuantity,
				config->buyItem[i].tradeItem[4].itemName, config->buyItem[i].tradeItem[4].quantityBuySell, config->buyItem[i].tradeItem[4].salePrice, config->buyItem[i].tradeItem[4].triggerQuantity,
				config->buyItem[i].tradeItem[5].itemName, config->buyItem[i].tradeItem[5].quantityBuySell, config->buyItem[i].tradeItem[5].salePrice, config->buyItem[i].tradeItem[5].triggerQuantity,
				config->buyItem[i].tradeItem[6].itemName, config->buyItem[i].tradeItem[6].quantityBuySell, config->buyItem[i].tradeItem[6].salePrice, config->buyItem[i].tradeItem[6].triggerQuantity,
				config->buyItem[i].tradeItem[7].itemName, config->buyItem[i].tradeItem[7].quantityBuySell, config->buyItem[i].tradeItem[7].salePrice, config->buyItem[i].tradeItem[7].triggerQuantity,
				config->buyItem[i].tradeItem[8].itemName, config->buyItem[i].tradeItem[8].quantityBuySell, config->buyItem[i].tradeItem[8].salePrice, config->buyItem[i].tradeItem[8].triggerQuantity,
				config->buyItem[i].tradeItem[9].itemName, config->buyItem[i].tradeItem[9].quantityBuySell, config->buyItem[i].tradeItem[9].salePrice, config->buyItem[i].tradeItem[9].triggerQuantity);
			AfxMessageBox(buf);
		}
*/
		Sleep(200);
		var=reader.getGlobalVariable("caveboot_halfsleep");
		if (var==NULL||strcmp(var,"true")) 
			alreadySleeping = 0; 
		else 
			alreadySleeping = 1;
		if (alreadySleeping && !sellerInvoked) continue;

		int attackedCreature = reader.getAttackedCreature();
		if (allAtOnce || shouldGo(config) && !attackedCreature) {
			allAtOnce = 1;
			for (int i = 0; i < MAX_SELLERS; i++) {
				buyOrSell = individualShouldGo(config, i); 
				if (buyOrSell && findSeller(config, i)) {
					reader.setGlobalVariable("caveboot_halfsleep","true");
					sellerInvoked = 1;
					if (moveToSeller(config)) {
						config->targetX = config->targetY = config->targetZ = 0; 
						if (buyOrSell == 1) {
							if (sellItems(config, i)) {
								reader.setGlobalVariable("caveboot_halfsleep","false");
								sellerInvoked = 0;
							}
						}
						if(buyOrSell == 2) {
							if (buyItems(config, i)) {
								reader.setGlobalVariable("caveboot_halfsleep","false");
								sellerInvoked = 0;
							}
						}
					}
				if (findSeller(config, i)) break;	
				}
				else if (!buyOrSell) allAtOnce = 0;
			}
		}
		else {
			reader.setGlobalVariable("caveboot_halfsleep","false");
			sellerInvoked = 0;
		}
	}
	reader.setGlobalVariable("caveboot_halfsleep","false");
	sellerInvoked = 0;
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
	return "1.1";
}

int CMod_SellerApp::validateConfig(int showAlerts) {
	char buf[128];
	if (m_configData->sellOnCap && m_configData->sellWhen < 1) {
		if (showAlerts) AfxMessageBox("\"Sell when capacity is less than:\" amount must not be 0(zero)!!");
		return 0;
	}
	for (int i = 0; i < MAX_SELLERS; i++) {
		for (int j = 0; j < 32; j++) {
			if (m_configData->buyItem[i].tradeItem[j].quantityBuySell < m_configData->buyItem[i].tradeItem[j].triggerQuantity) {
				sprintf(buf, "Trigger Quantity must be < Quantity to buy up to:\nPlease correct: Item entry: %s under Seller %d", m_configData->buyItem[i].tradeItem[j].itemName, i);
				if (showAlerts) AfxMessageBox(buf);
				return 0;
			}
			if (m_configData->sellItem[i].tradeItem[j].quantityBuySell < 1) {
				sprintf(buf, "You must sell at least 1 item!!\nPlease correct: Item entry: %s under Seller %d", m_configData->sellItem[i].tradeItem[j].itemName, i);
				if (showAlerts) AfxMessageBox(buf);
				return 0;
			}
			if (m_configData->buyItem[i].tradeItem[j].quantityBuySell < 1) {
				sprintf(buf, "You must buy at least 1 item!!\nPlease correct: Item entry: %s under Seller %d", m_configData->buyItem[i].tradeItem[j].itemName, i);
				if (showAlerts) AfxMessageBox(buf);
				return 0;
			}
			if (m_configData->buyItem[i].tradeItem[j].triggerQuantity < 1) {
				sprintf(buf, "\'Buy When <\" must not be 0 (zero)!!\nPlease correct: Item entry: %s under Seller %d", m_configData->buyItem[i].tradeItem[j].itemName, i);
				if (showAlerts) AfxMessageBox(buf);
				return 0;
			}
			if (m_configData->buyItem[i].tradeItem[j].salePrice < 1) {
				sprintf(buf, "\"Sale Price\" must not be 0 (zero)!!\nPlease correct: Item entry: %s under Seller %d", m_configData->buyItem[i].tradeItem[j].itemName, i);
				if (showAlerts) AfxMessageBox(buf);
				return 0;
			}
		}
	}


	return 1;
}

void CMod_SellerApp::resetConfig() {
	m_configData = new CConfigData();
}

void CMod_SellerApp::loadConfigParam(char *paramName,char *paramValue) {
	if (!strcmp(paramName, "SellOnCap")) m_configData->sellOnCap = atoi(paramValue);
	if (!strcmp(paramName, "SellOnSpace")) m_configData->sellOnSpace = atoi(paramValue);
	if (!strcmp(paramName, "SellWhen")) m_configData->sellWhen = atoi(paramValue);
// Seller 1
	if (!strcmp(paramName,"Seller1/Name")) sprintf(m_configData->sellerList[0].sellerName, "%s", paramValue);
	if (!strcmp(paramName,"Seller1/SaleItems/Name")) {
		if (currentPos>32) return;
		sprintf(m_configData->sellItem[0].tradeItem[currentPos++].itemName, "%s", paramValue);
	}
	if (!strcmp(paramName,"Seller1/SaleItems/SaleTriggerQuantity")) {
		if (currentPos>32) return;
		m_configData->sellItem[0].tradeItem[currentPos++].quantityBuySell = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller1/BuyItems/Name")) {
		if (currentPos>32) return;
		sprintf(m_configData->buyItem[0].tradeItem[currentPos++].itemName, "%s", paramValue);
	}
	if (!strcmp(paramName,"Seller1/BuyItems/BuyQuantity")) {
		if (currentPos>32) return;
		m_configData->buyItem[0].tradeItem[currentPos++].quantityBuySell = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller1/BuyItems/BuyTriggerQuantity")) {
		if (currentPos>32) return;
		m_configData->buyItem[0].tradeItem[currentPos++].triggerQuantity = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller1/BuyItems/BuyPrice")){
		if (currentPos>32) return;
		m_configData->buyItem[0].tradeItem[currentPos++].salePrice = atoi(paramValue);
	}
// Seller 2
	if (!strcmp(paramName,"Seller2/Name")) sprintf(m_configData->sellerList[1].sellerName, "%s", paramValue);
	if (!strcmp(paramName,"Seller2/SaleItems/Name")) {
		if (currentPos>32) return;
		sprintf(m_configData->sellItem[1].tradeItem[currentPos++].itemName, "%s", paramValue);
	}
	if (!strcmp(paramName,"Seller2/BuyItems/Name")) {
		if (currentPos>32) return;
		sprintf(m_configData->buyItem[1].tradeItem[currentPos++].itemName, "%s", paramValue);
	}
	if (!strcmp(paramName,"Seller2/BuyItems/BuyQuantity")) {
		if (currentPos>32) return;
		m_configData->buyItem[1].tradeItem[currentPos++].quantityBuySell = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller2/BuyItems/BuyTriggerQuantity")) {
		if (currentPos>32) return;
		m_configData->buyItem[1].tradeItem[currentPos++].triggerQuantity = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller2/BuyItems/BuyPrice")) {
		if (currentPos>32) return;
		m_configData->buyItem[1].tradeItem[currentPos++].salePrice = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller2/SaleItems/SaleTriggerQuantity")) {
		if (currentPos>32) return;
		m_configData->sellItem[1].tradeItem[currentPos++].quantityBuySell = atoi(paramValue);
	}
// Seller 3
	if (!strcmp(paramName,"Seller3/Name")) sprintf(m_configData->sellerList[2].sellerName, "%s", paramValue);
	if (!strcmp(paramName,"Seller3/SaleItems/Name")) {
		if (currentPos>32) return;
		sprintf(m_configData->sellItem[2].tradeItem[currentPos++].itemName, "%s", paramValue);
	}
	if (!strcmp(paramName,"Seller3/SaleItems/SaleTriggerQuantity")){
		if (currentPos>32) return;
		m_configData->sellItem[2].tradeItem[currentPos++].quantityBuySell = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller3/BuyItems/Name")){
		if (currentPos>32) return;
		sprintf(m_configData->buyItem[2].tradeItem[currentPos++].itemName, "%s", paramValue);
	}
	if (!strcmp(paramName,"Seller3/BuyItems/BuyQuantity")){
		if (currentPos>32) return;
		m_configData->buyItem[2].tradeItem[currentPos++].quantityBuySell = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller3/BuyItems/BuyTriggerQuantity")){
		if (currentPos>32) return;
		m_configData->buyItem[2].tradeItem[currentPos++].triggerQuantity = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller3/BuyItems/BuyPrice")){
		if (currentPos>32) return;
		m_configData->buyItem[2].tradeItem[currentPos++].salePrice = atoi(paramValue);
	}
// Seller 4
	if (!strcmp(paramName,"Seller4/Name")) sprintf(m_configData->sellerList[3].sellerName, "%s", paramValue);
	if (!strcmp(paramName,"Seller4/SaleItems/Name")){
		if (currentPos>32) return;
		sprintf(m_configData->sellItem[3].tradeItem[currentPos++].itemName, "%s", paramValue);
	}
	if (!strcmp(paramName,"Seller4/SaleItems/SaleTriggerQuantity")){
		if (currentPos>32) return;
		m_configData->sellItem[3].tradeItem[currentPos++].quantityBuySell = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller4/BuyItems/Name")){
		if (currentPos>32) return;
		sprintf(m_configData->buyItem[3].tradeItem[currentPos++].itemName, "%s", paramValue);
	}
	if (!strcmp(paramName,"Seller4/BuyItems/BuyQuantity")){
		if (currentPos>32) return;
		m_configData->buyItem[3].tradeItem[currentPos++].quantityBuySell = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller4/BuyItems/BuyTriggerQuantity")){
		if (currentPos>32) return;
		m_configData->buyItem[3].tradeItem[currentPos++].triggerQuantity = atoi(paramValue);
	}
	if (!strcmp(paramName,"Seller4/BuyItems/BuyPrice")){
		if (currentPos>32) return;
		m_configData->buyItem[3].tradeItem[currentPos++].salePrice = atoi(paramValue);
	}
}

char *CMod_SellerApp::saveConfigParam(char *paramName) {
	static char buf[1024];
	buf[0]='\0';
	if (!strcmp(paramName, "SellOnCap")) sprintf(buf, "%d", m_configData->sellOnCap);
	if (!strcmp(paramName, "SellOnSpace")) sprintf(buf, "%d", m_configData->sellOnSpace);
	if (!strcmp(paramName, "SellWhen")) sprintf(buf,"%s",m_configData->sellWhen);
// Seller 1
	if (!strcmp(paramName,"Seller1/Name")) sprintf(buf,"%s",m_configData->sellerList[0].sellerName);
	if (!strcmp(paramName,"Seller1/SaleItems/Name")) {		
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->sellItem[0].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%s", m_configData->sellItem[0].tradeItem[currentPos++].itemName);
		}
	}
	if (!strcmp(paramName,"Seller1/SaleItems/SaleTriggerQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->sellItem[0].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->sellItem[0].tradeItem[currentPos++].quantityBuySell);
		}
	}
	if (!strcmp(paramName,"Seller1/BuyItems/Name")) {		
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[0].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%s", m_configData->buyItem[0].tradeItem[currentPos++].itemName);
		}
	}
	if (!strcmp(paramName,"Seller1/BuyItems/BuyQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[0].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[0].tradeItem[currentPos++].quantityBuySell);
		}
	}
	if (!strcmp(paramName,"Seller1/BuyItems/BuyTriggerQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[0].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[0].tradeItem[currentPos++].triggerQuantity);
		}
	}
	if (!strcmp(paramName,"Seller1/BuyItems/BuyPrice")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[0].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[0].tradeItem[currentPos++].salePrice);
		}
	}
// Seller 2
	if (!strcmp(paramName,"Seller2/Name")) sprintf(buf,"%s",m_configData->sellerList[1].sellerName);
	if (!strcmp(paramName,"Seller2/SaleItems/Name")) {		
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->sellItem[1].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%s", m_configData->sellItem[1].tradeItem[currentPos++].itemName);
		}
	}
	if (!strcmp(paramName,"Seller2/SaleItems/SaleTriggerQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->sellItem[1].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->sellItem[1].tradeItem[currentPos++].quantityBuySell);
		}
	}
	if (!strcmp(paramName,"Seller2/BuyItems/Name")) {		
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[1].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%s", m_configData->buyItem[1].tradeItem[currentPos++].itemName);
		}
	}
	if (!strcmp(paramName,"Seller2/BuyItems/BuyQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[1].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[1].tradeItem[currentPos++].quantityBuySell);
		}
	}
	if (!strcmp(paramName,"Seller2/BuyItems/BuyTriggerQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[1].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[1].tradeItem[currentPos++].triggerQuantity);
		}
	}
	if (!strcmp(paramName,"Seller2/BuyItems/BuyPrice")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[1].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[1].tradeItem[currentPos++].salePrice);
		}
	}
// Seller 3
	if (!strcmp(paramName,"Seller3/Name")) sprintf(buf,"%s",m_configData->sellerList[2].sellerName);
	if (!strcmp(paramName,"Seller3/SaleItems/Name")) {		
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->sellItem[2].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%s", m_configData->sellItem[2].tradeItem[currentPos++].itemName);
		}
	}
	if (!strcmp(paramName,"Seller3/SaleItems/SaleTriggerQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->sellItem[2].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->sellItem[2].tradeItem[currentPos++].quantityBuySell);
		}
	}
	if (!strcmp(paramName,"Seller3/BuyItems/Name")) {		
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[2].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%s", m_configData->buyItem[2].tradeItem[currentPos++].itemName);
		}
	}
	if (!strcmp(paramName,"Seller3/BuyItems/BuyQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[2].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[2].tradeItem[currentPos++].quantityBuySell);
		}
	}
	if (!strcmp(paramName,"Seller3/BuyItems/BuyTriggerQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[2].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[2].tradeItem[currentPos++].triggerQuantity);
		}
	}
	if (!strcmp(paramName,"Seller3/BuyItems/BuyPrice")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[2].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[2].tradeItem[currentPos++].salePrice);
		}
	}
// Seller 4
	if (!strcmp(paramName,"Seller4/Name")) sprintf(buf,"%s",m_configData->sellerList[3].sellerName);
	if (!strcmp(paramName,"Seller4/SaleItems/Name")) {		
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->sellItem[3].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%s", m_configData->sellItem[3].tradeItem[currentPos++].itemName);
		}
	}
	if (!strcmp(paramName,"Seller4/SaleItems/SaleTriggerQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->sellItem[3].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->sellItem[3].tradeItem[currentPos++].quantityBuySell);
		}
	}
	if (!strcmp(paramName,"Seller4/BuyItems/Name")) {		
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[3].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%s", m_configData->buyItem[3].tradeItem[currentPos++].itemName);
		}
	}
	if (!strcmp(paramName,"Seller4/BuyItems/BuyQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[3].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[3].tradeItem[currentPos++].quantityBuySell);
		}
	}
	if (!strcmp(paramName,"Seller4/BuyItems/BuyTriggerQuantity")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[3].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[3].tradeItem[currentPos++].triggerQuantity);
		}
	}
	if (!strcmp(paramName,"Seller4/BuyItems/BuyPrice")) {
		if (currentPos<32) {
			if (IsCharAlphaNumeric(m_configData->buyItem[3].tradeItem[currentPos].itemName[0]))
				sprintf(buf, "%d", m_configData->buyItem[3].tradeItem[currentPos++].salePrice);
		}
	}

	return buf;
}

char *CMod_SellerApp::getConfigParamName(int nr) {
	switch (nr) {
	case 0: return "Seller1/Name";
	case 1: return "Seller1/SaleItems/Name";
	case 2: return "Seller1/SaleItems/SaleTriggerQuantity";
	case 3: return "Seller2/Name";
	case 4: return "Seller2/SaleItems/Name";
	case 5: return "Seller2/SaleItems/SaleTriggerQuantity";
	case 6: return "Seller3/Name";
	case 7: return "Seller3/SaleItems/Name";
	case 8: return "Seller3/SaleItems/SaleTriggerQuantity";
	case 9: return "Seller4/Name";
	case 10: return "Seller4/SaleItems/Name";
	case 11: return "Seller4/SaleItems/SaleTriggerQuantity";
	case 12: return "Seller1/BuyItems/Name";
	case 13: return "Seller1/BuyItems/BuyQuantity";
	case 14: return "Seller1/BuyItems/BuyTriggerQuantity";
	case 15: return "Seller1/BuyItems/BuyPrice";
	case 16: return "Seller2/BuyItems/Name";
	case 17: return "Seller2/BuyItems/BuyQuantity";
	case 18: return "Seller2/BuyItems/BuyTriggerQuantity";
	case 19: return "Seller2/BuyItems/BuyPrice";
	case 20: return "Seller3/BuyItems/Name";
	case 21: return "Seller3/BuyItems/BuyQuantity";
	case 22: return "Seller3/BuyItems/BuyTriggerQuantity";
	case 23: return "Seller3/BuyItems/BuyPrice";
	case 24: return "Seller4/BuyItems/Name";
	case 25: return "Seller4/BuyItems/BuyQuantity";
	case 26: return "Seller4/BuyItems/BuyTriggerQuantity";
	case 27: return "Seller4/BuyItems/BuyPrice";
	case 28: return "SellOnCap";
	case 29: return "SellWhen";
	case 30: return "SellOnSpace";

	default:
		return NULL;
	}
}

int CMod_SellerApp::isMultiParam(char *paramName) {
	if (!strcmp(paramName,"Seller1/SaleItems/Name")) return 1;
	if (!strcmp(paramName,"Seller1/SaleItems/SaleTriggerQuantity")) return 1;
	if (!strcmp(paramName,"Seller2/SaleItems/Name")) return 1;
	if (!strcmp(paramName,"Seller2/SaleItems/SaleTriggerQuantity")) return 1;
	if (!strcmp(paramName,"Seller3/SaleItems/Name")) return 1;
	if (!strcmp(paramName,"Seller3/SaleItems/SaleTriggerQuantity")) return 1;
	if (!strcmp(paramName,"Seller4/SaleItems/Name")) return 1;
	if (!strcmp(paramName,"Seller4/SaleItems/SaleTriggerQuantity")) return 1;
	if (!strcmp(paramName,"Seller1/BuyItems/Name")) return 1;
	if (!strcmp(paramName,"Seller1/BuyItems/BuyQuantity")) return 1;
	if (!strcmp(paramName,"Seller1/BuyItems/BuyTriggerQuantity")) return 1;
	if (!strcmp(paramName,"Seller1/BuyItems/BuyPrice")) return 1;
	if (!strcmp(paramName,"Seller2/BuyItems/Name")) return 1;
	if (!strcmp(paramName,"Seller2/BuyItems/BuyQuantity")) return 1;
	if (!strcmp(paramName,"Seller2/BuyItems/BuyTriggerQuantity")) return 1;
	if (!strcmp(paramName,"Seller2/BuyItems/BuyPrice")) return 1;
	if (!strcmp(paramName,"Seller3/BuyItems/Name")) return 1;
	if (!strcmp(paramName,"Seller3/BuyItems/BuyQuantity")) return 1;
	if (!strcmp(paramName,"Seller3/BuyItems/BuyTriggerQuantity")) return 1;
	if (!strcmp(paramName,"Seller3/BuyItems/BuyPrice")) return 1;
	if (!strcmp(paramName,"Seller4/BuyItems/Name")) return 1;
	if (!strcmp(paramName,"Seller4/BuyItems/BuyQuantity")) return 1;
	if (!strcmp(paramName,"Seller4/BuyItems/BuyTriggerQuantity")) return 1;
	if (!strcmp(paramName,"Seller4/BuyItems/BuyPrice")) return 1;
	return 0;
}

void CMod_SellerApp::resetMultiParamAccess(char *paramName) {
	if (!strcmp(paramName,"Seller1/SaleItems/Name")) currentPos=0;
	if (!strcmp(paramName,"Seller1/SaleItems/SaleTriggerQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller2/SaleItems/Name")) currentPos=0;
	if (!strcmp(paramName,"Seller2/SaleItems/SaleTriggerQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller3/SaleItems/Name")) currentPos=0;
	if (!strcmp(paramName,"Seller3/SaleItems/SaleTriggerQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller4/SaleItems/Name")) currentPos=0;
	if (!strcmp(paramName,"Seller4/SaleItems/SaleTriggerQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller1/BuyItems/Name")) currentPos=0;
	if (!strcmp(paramName,"Seller1/BuyItems/BuyQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller1/BuyItems/BuyTriggerQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller1/BuyItems/BuyPrice")) currentPos=0;
	if (!strcmp(paramName,"Seller2/BuyItems/Name")) currentPos=0;
	if (!strcmp(paramName,"Seller2/BuyItems/BuyQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller2/BuyItems/BuyTriggerQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller2/BuyItems/BuyPrice")) currentPos=0;
	if (!strcmp(paramName,"Seller3/BuyItems/Name")) currentPos=0;
	if (!strcmp(paramName,"Seller3/BuyItems/BuyQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller3/BuyItems/BuyTriggerQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller3/BuyItems/BuyPrice")) currentPos=0;
	if (!strcmp(paramName,"Seller4/BuyItems/Name")) currentPos=0;
	if (!strcmp(paramName,"Seller4/BuyItems/BuyQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller4/BuyItems/BuyTriggerQuantity")) currentPos=0;
	if (!strcmp(paramName,"Seller4/BuyItems/BuyPrice")) currentPos=0;
}

int findSeller(CConfigData *config, int traderNum) {
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
	sender.say("Hi");
	Sleep (500);
	sender.sayNPC("trade");
	Sleep (500);
	for (int j = 0; j < 32; j++) {
		int objectId = itemProxy.getObjectId(config->sellItem[traderNum].tradeItem[j].itemName);
		for (int contNr = 0; contNr < 16; contNr++) {
			cont = reader.readContainer(contNr);
			if (cont->flagOnOff) {
				int count = cont->itemsInside;
				for (int slotNr = count - 1; slotNr >= 0; slotNr--) {
					CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(slotNr);
					if (item->objectId == objectId) {
						itemCount = countAllItemsOfType(objectId);
						sender.npcSell(objectId, itemCount);
						Sleep(1000);
						if (CModuleUtil::waitForItemsInsideChange(contNr, cont->itemsInside)) {
							done = 1;
							break;
						}
						done = 0;
					}
				}
			}
		}		
	}
	delete cont;	
	return done;
}

int buyItems(CConfigData *config, int traderNum) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	int itemCount, goldCount;
	int done = 0;
	int objectId;
	//char buf[64];
	sender.say("Hi");
	Sleep (500);
	sender.sayNPC("trade");
	Sleep (500);
	for (int j = 0; j < 32; j++) {
			objectId = itemProxy.getObjectId(config->buyItem[traderNum].tradeItem[j].itemName);
			//sprintf(buf, "Item Name: %s", config->buyItem[traderNum].tradeItem[j].itemName);
			//AfxMessageBox(buf);
			if (objectId)
				itemCount = countAllItemsOfType(objectId);
			else 
				break;			;
			goldCount = countAllItemsOfType(itemProxy.getValueForConst("GP"));
			itemCount = config->buyItem[traderNum].tradeItem[j].quantityBuySell - itemCount;
			itemCount = goldCount / config->buyItem[traderNum].tradeItem[j].salePrice >= itemCount?itemCount:goldCount / config->buyItem[traderNum].tradeItem[j].salePrice;
			//sprintf(buf, "Item: %d\nGold: %d\nCount: %d",objectId, goldCount, itemCount);
			//AfxMessageBox(buf);
			if (itemCount > 0) {
				sender.npcBuy(objectId, itemCount);
				Sleep(1000);
				done = 1;
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
	CTibiaCharacter *self;
	self = reader.readSelfCharacter();


	int count = 0;
	bool should = false;
	for (int i = 0; i < MAX_SELLERS; i++) {
		if (config->sellOnCap && self->cap < config->sellWhen && individualShouldGo(config, i)) 
			return true;
		if (config->sellOnSpace && noSpace() && individualShouldGo(config, i))
			return true;
		for (int j = 0; j < 32; j++) {
			int objectId = itemProxy.getObjectId(config->sellItem[i].tradeItem[j].itemName);
			//sprintf(buf, "%s\nItem count: %d\nTrigger Quantity: %d", config->sellItem[i].tradeItem[j].itemName, countAllItemsOfType(objectId), config->sellItem[i].tradeItem[j].quantityBuySell);
			//AfxMessageBox(buf);
			count = countAllItemsOfType(objectId);
			if (count && count >= config->sellItem[i].tradeItem[j].quantityBuySell)
				should = true;
		}
		for (j = 0; j < 32; j++) {
			int objectId = itemProxy.getObjectId(config->buyItem[i].tradeItem[j].itemName);
			//sprintf(buf, "%s\nItem count: %d\nTrigger Quantity: %d", config->sellItem[i].tradeItem[j].itemName, countAllItemsOfType(objectId), config->sellItem[i].tradeItem[j].quantityBuySell);
			//AfxMessageBox(buf);
			count = countAllItemsOfType(objectId);
			if (count < config->buyItem[i].tradeItem[j].triggerQuantity && countAllItemsOfType(itemProxy.getValueForConst("GP")) >= config->buyItem[i].tradeItem[j].salePrice)
				should = true;
		}
	}
	return should;
}

int individualShouldGo(CConfigData *config, int traderNum) {
	//char buf[64];
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	for (int j = 0; j < 32; j++) {
		int objectId = itemProxy.getObjectId(config->sellItem[traderNum].tradeItem[j].itemName);
		//sprintf(buf, "Seller: %d\nObjectID: %d", traderNum+1, objectId);
		//AfxMessageBox(buf);
		if (objectId && countAllItemsOfType(objectId) >= 1)
			return 1;
	}
	for (j = 0; j < 32; j++) {
		int objectId = itemProxy.getObjectId(config->buyItem[traderNum].tradeItem[j].itemName);
		//sprintf(buf, "Seller: %d\nObjectID: %d", traderNum+1, objectId);
		//AfxMessageBox(buf);
		if (objectId && countAllItemsOfType(objectId) < config->buyItem[traderNum].tradeItem[j].quantityBuySell && countAllItemsOfType(itemProxy.getValueForConst("GP")) >= config->buyItem[traderNum].tradeItem[j].salePrice)
			return 2;
	}
	return 0;
}

int noSpace() {
	CMemReaderProxy reader;
	int contNr;
	int hasSpace=0;
	for (contNr = 0; contNr < 16; contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);
		
		if (cont->flagOnOff && cont->itemsInside < cont->size) {
			delete cont;
			return 0;
		}
		delete cont;		
	}
	return 1;
}


