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

#define NOGO 0
#define SELLONLY 1
#define BUYONLY 2
#define DOBOTH 3 

#define MAX_BUYSELL_ITEMS 100

CToolSellerState globalSellerState=CToolSellerState_notRunning;
int GUIx = 0,GUIy = 0,GUIz = 0, GUINum = 0;

/////////////////////////////////////////////////////////////////////////////
// CMod_SellerApp

BEGIN_MESSAGE_MAP(CMod_SellerApp, CWinApp)
//{{AFX_MSG_MAP(CMod_SellerApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions

int RandomTimeSellerSay(int length){
	return CModuleUtil::randomFormula(300+min(length*100,1200),200);//ranges from 300 to 1700
}

int RandomTimeSeller(){
	return CModuleUtil::randomFormula(1500,400);
}

int findSeller(CConfigData *, int);
int moveToSeller(CConfigData *,int);
int sellItems(CConfigData *, int);
int buyItems(CConfigData *, int);
int isDepositing();
int spaceAvailable();
int isCavebotOn();
int countAllItemsOfType(int objectId,bool includeSlots=0);
bool shouldGo(CConfigData *);
int individualShouldGo(CConfigData *, int);
bool canGo(CConfigData *config);

// Required to be run more often than the return value is expected to change
int donaAttackingAndLooting(){
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
	CPackSenderProxy sender;

	CConfigData *config = (CConfigData *)lpParam;
	int allAtOnce = 0;
	int buyOrSell = 0;
	int persistentShouldGo=0;
	int lastPathNotFoundTm=0;

	while (!toolThreadShouldStop) {
/*		for (int i=0;i<MAX_SELLERS;i++) {
			char buf[2048];
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
		static int tStart=0;
		Sleep(400);
		tStart=GetTickCount();
		if (buyOrSell == MAX_SELLERS)
			buyOrSell = allAtOnce = persistentShouldGo = 0;

		const char* controller=reader.getGlobalVariable("walking_control");
		if (!persistentShouldGo && shouldGo(config)){
			persistentShouldGo=1;
		}
		if (!persistentShouldGo
			&& config->stopBySeller
			&& (!strcmp(controller,"banker") || !strcmp(controller,"depotwalker"))
			&& canGo(config)){
			persistentShouldGo=1;
		}

		bool control= strcmp(controller,"seller")==0;
		int modpriority=atoi(reader.getGlobalVariable("walking_priority"));
		int wantsControl=0;
		int foundPathToSeller=0;
		// if wants control
		if ((allAtOnce || persistentShouldGo)) {
			//if no path found let other modules work and wait 10 secs before trying again
			if (time(NULL)-lastPathNotFoundTm > 10){
				//if should have control, take it
				if (!control){
					if (atoi(config->modPriorityStr) > modpriority){
						reader.setGlobalVariable("walking_control","seller");
						reader.setGlobalVariable("walking_priority",config->modPriorityStr);
					}else{
						globalSellerState=CToolSellerState_halfSleep;
					}
				}
			} else { // if doesn't want control
				globalSellerState=CToolSellerState_noPathFound;
				//if has control, give it up
				if (control){
					reader.setGlobalVariable("walking_control","");
					reader.setGlobalVariable("walking_priority","0");
				}
			}
		} else { // if doesn't want control
			globalSellerState=CToolSellerState_notRunning;
			//if has control, give it up
			if (control){
				reader.setGlobalVariable("walking_control","");
				reader.setGlobalVariable("walking_priority","0");
			}
		}

		if (donaAttackingAndLooting() && strcmp(reader.getGlobalVariable("walking_control"),"seller")==0) {
			allAtOnce = 1;
			for (int i = buyOrSell; i < MAX_SELLERS; i++) {
				if (individualShouldGo(config, i)){
					if (findSeller(config, i)) {
						globalSellerState=CToolSellerState_walking;
						if (moveToSeller(config,i)) {
							// High Priority Task
							reader.setGlobalVariable("walking_control","seller");
							reader.setGlobalVariable("walking_priority","9");

							globalSellerState=CToolSellerState_talking;
							config->targetX = config->targetY = config->targetZ = 0;
							sellItems(config, i);
							buyItems(config, i);
							buyOrSell++;

							reader.setGlobalVariable("walking_control","seller");
							reader.setGlobalVariable("walking_priority",config->modPriorityStr);
						}
						else break;
					} else {
						lastPathNotFoundTm=time(NULL);
					}
				}
				else buyOrSell++;
			}
		}
	}
	if (strcmp(reader.getGlobalVariable("walking_control"),"seller")==0){
		reader.setGlobalVariable("walking_control","");
		reader.setGlobalVariable("walking_priority","0");
	}
	globalSellerState=CToolSellerState_notRunning;
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
	if (m_configDialog) {
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
		if (m_started) disableControls();
		else enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
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
	char buf[256];
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
			if (m_configData->sellItem[i].tradeItem[j].itemName[0] && m_configData->sellItem[i].tradeItem[j].quantityBuySell < 1) {
				sprintf(buf, "You must sell at least 1 item!!\nPlease correct: Item entry: %s under Seller %d", m_configData->sellItem[i].tradeItem[j].itemName, i);
				if (showAlerts) AfxMessageBox(buf);
				return 0;
			}
			if (m_configData->buyItem[i].tradeItem[j].itemName[0] && m_configData->buyItem[i].tradeItem[j].quantityBuySell < 1) {
				sprintf(buf, "You must buy at least 1 item!!\nPlease correct: Item entry: %s under Seller %d", m_configData->buyItem[i].tradeItem[j].itemName, i);
				if (showAlerts) AfxMessageBox(buf);
				return 0;
			}
			if (m_configData->buyItem[i].tradeItem[j].itemName[0] && m_configData->buyItem[i].tradeItem[j].triggerQuantity < 1) {
				sprintf(buf, "\'Buy When <\" must not be 0 (zero)!!\nPlease correct: Item entry: %s under Seller %d", m_configData->buyItem[i].tradeItem[j].itemName, i);
				if (showAlerts) AfxMessageBox(buf);
				return 0;
			}
			if (m_configData->buyItem[i].tradeItem[j].itemName[0] && m_configData->buyItem[i].tradeItem[j].salePrice < 1) {
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
	if (!strcmp(paramName, "StopBySeller")) m_configData->stopBySeller=atoi(paramValue);
	if (!strcmp(paramName, "ModPriority")) strncpy(m_configData->modPriorityStr,paramValue,2);
	if (!strcmp(paramName, "SellOnCap")) m_configData->sellOnCap = atoi(paramValue);
	if (!strcmp(paramName, "SellOnSpace")) m_configData->sellOnSpace = atoi(paramValue);
	if (!strcmp(paramName, "SellWhen")) m_configData->sellWhen = atoi(paramValue);
// Seller 1
	if (!strcmp(paramName,"Seller1/Name")) sprintf(m_configData->sellerList[0].sellerName, "%s", paramValue);
	if (!strcmp(paramName,"Seller1/Pos")) sscanf(paramValue, "(%d,%d,%d)",&(m_configData->sellerList[0].sellerX),&(m_configData->sellerList[0].sellerY),&(m_configData->sellerList[0].sellerZ));
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
	if (!strcmp(paramName,"Seller2/Pos")) sscanf(paramValue, "(%d,%d,%d)",&(m_configData->sellerList[1].sellerX),&(m_configData->sellerList[1].sellerY),&(m_configData->sellerList[1].sellerZ));
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
	if (!strcmp(paramName,"Seller3/Pos")) sscanf(paramValue, "(%d,%d,%d)",&(m_configData->sellerList[2].sellerX),&(m_configData->sellerList[2].sellerY),&(m_configData->sellerList[2].sellerZ));
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
	if (!strcmp(paramName,"Seller4/Pos")) sscanf(paramValue, "(%d,%d,%d)",&(m_configData->sellerList[3].sellerX),&(m_configData->sellerList[3].sellerY),&(m_configData->sellerList[3].sellerZ));
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
	if (!strcmp(paramName, "StopBySeller")) sprintf(buf, "%d", m_configData->stopBySeller);
	if (!strcmp(paramName, "ModPriority")) sprintf(buf, "%s", m_configData->modPriorityStr);
	if (!strcmp(paramName, "SellOnCap")) sprintf(buf, "%d", m_configData->sellOnCap);
	if (!strcmp(paramName, "SellOnSpace")) sprintf(buf, "%d", m_configData->sellOnSpace);
	if (!strcmp(paramName, "SellWhen")) sprintf(buf,"%d",m_configData->sellWhen);
// Seller 1
	if (!strcmp(paramName,"Seller1/Name")) sprintf(buf,"%s",m_configData->sellerList[0].sellerName);
	if (!strcmp(paramName,"Seller1/Pos")) sprintf(buf,"(%d,%d,%d)",m_configData->sellerList[0].sellerX,m_configData->sellerList[0].sellerY,m_configData->sellerList[0].sellerZ);
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
	if (!strcmp(paramName,"Seller2/Pos")) sprintf(buf,"(%d,%d,%d)",m_configData->sellerList[1].sellerX,m_configData->sellerList[1].sellerY,m_configData->sellerList[1].sellerZ);
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
	if (!strcmp(paramName,"Seller3/Pos")) sprintf(buf,"(%d,%d,%d)",m_configData->sellerList[2].sellerX,m_configData->sellerList[2].sellerY,m_configData->sellerList[2].sellerZ);
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
	if (!strcmp(paramName,"Seller4/Pos")) sprintf(buf,"(%d,%d,%d)",m_configData->sellerList[3].sellerX,m_configData->sellerList[3].sellerY,m_configData->sellerList[3].sellerZ);
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
	case 1: return "Seller1/Pos";
	case 2: return "Seller1/SaleItems/Name";
	case 3: return "Seller1/SaleItems/SaleTriggerQuantity";
	case 4: return "Seller2/Name";
	case 5: return "Seller2/Pos";
	case 6: return "Seller2/SaleItems/Name";
	case 7: return "Seller2/SaleItems/SaleTriggerQuantity";
	case 8: return "Seller3/Name";
	case 9: return "Seller3/Pos";
	case 10: return "Seller3/SaleItems/Name";
	case 11: return "Seller3/SaleItems/SaleTriggerQuantity";
	case 12: return "Seller4/Name";
	case 13: return "Seller4/Pos";
	case 14: return "Seller4/SaleItems/Name";
	case 15: return "Seller4/SaleItems/SaleTriggerQuantity";
	case 16: return "Seller1/BuyItems/Name";
	case 17: return "Seller1/BuyItems/BuyQuantity";
	case 18: return "Seller1/BuyItems/BuyTriggerQuantity";
	case 19: return "Seller1/BuyItems/BuyPrice";
	case 20: return "Seller2/BuyItems/Name";
	case 21: return "Seller2/BuyItems/BuyQuantity";
	case 22: return "Seller2/BuyItems/BuyTriggerQuantity";
	case 23: return "Seller2/BuyItems/BuyPrice";
	case 24: return "Seller3/BuyItems/Name";
	case 25: return "Seller3/BuyItems/BuyQuantity";
	case 26: return "Seller3/BuyItems/BuyTriggerQuantity";
	case 27: return "Seller3/BuyItems/BuyPrice";
	case 28: return "Seller4/BuyItems/Name";
	case 29: return "Seller4/BuyItems/BuyQuantity";
	case 30: return "Seller4/BuyItems/BuyTriggerQuantity";
	case 31: return "Seller4/BuyItems/BuyPrice";
	case 32: return "SellOnCap";
	case 33: return "SellWhen";
	case 34: return "SellOnSpace";
	case 35: return "ModPriority";
	case 36: return "StopBySeller";

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
	if (config->targetX == self->x && config->targetY == self->y && config->targetZ == self->z){
		memset(config->path,0,15);
		delete self;
		return 1;
	}
	GUIx = config->sellerList[traderNum].sellerX;
	GUIy = config->sellerList[traderNum].sellerY;
	GUIz = config->sellerList[traderNum].sellerZ;
	GUINum = traderNum;
	struct point nearestSell = CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->sellerList[traderNum].sellerX, config->sellerList[traderNum].sellerY, config->sellerList[traderNum].sellerZ, 0, config->path,3);
	if (nearestSell.x && nearestSell.y && nearestSell.z) {
		config->targetX = nearestSell.x;
		config->targetY = nearestSell.y;
		config->targetZ = nearestSell.z;
		delete self;
//			AfxMessageBox("Seller Found moving");
		return 1;
	}
	delete self;
//			AfxMessageBox("Seller Not Found");
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

int moveToSeller(CConfigData *config, int traderNum) {
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
				if (mon->tibiaId==0){
					delete mon;
					break;
				}
				int len=strlen(mon->name);
				// since sellerList[traderNum].sellerName may include city, match first part of name
				if (strncmp(config->sellerList[traderNum].sellerName,mon->name,len)==0 && (config->sellerList[traderNum].sellerName[len]==0 || config->sellerList[traderNum].sellerName[len]==' ')){
					for (int tries=0;tries<2;tries++){ // should only need 1 try, but we'd need to start over if we don't make it
						CTibiaCharacter* self = reader.readSelfCharacter();
						delete mon;
						mon=reader.readVisibleCreature(i);

						struct point nearestSell=point(0,0,0);
						int rad=2;
						while (nearestSell.x==0 && rad<=3){//find paths increasingly farther away
							nearestSell= CModuleUtil::findPathOnMap(self->x, self->y, self->z, mon->x, mon->y, mon->z, 0, config->path,rad++);
						}
						if (nearestSell.x && nearestSell.y && nearestSell.z==self->z){
							CModuleUtil::executeWalk(self->x,self->y,self->z,config->path);
							if (CModuleUtil::waitToStandOnSquare(nearestSell.x, nearestSell.y)){
								delete mon;
								mon=reader.readVisibleCreature(i);
								if (!(abs(self->x - mon->x) <=3 && abs(self->y - mon->y) <=3 && self->z == mon->z)){
									delete self;
									continue;
								}
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

int sellItems(CConfigData *config, int traderNum) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaContainer *cont;
	int itemCount;
	int done = -1;

	if (itemProxy.getItemId(config->sellItem[traderNum].tradeItem[0].itemName)==0)
		return 1;

	Sleep (RandomTimeSellerSay(strlen("hi")));
	sender.say("hi");
	Sleep (800);//Give time for NPC window to open
	Sleep (RandomTimeSellerSay(strlen("trade")));
	sender.sayNPC("trade");
	Sleep (RandomTimeSeller());
	for (int j = 0; j < 32; j++) {
		int objectId = itemProxy.getItemId(config->sellItem[traderNum].tradeItem[j].itemName);
		for (int contNr = 0; contNr < memConstData.m_memMaxContainers; contNr++) {
			cont = reader.readContainer(contNr);
			if (cont->flagOnOff) {
				int count = cont->itemsInside;
				for (int slotNr = count - 1; slotNr >= 0; slotNr--) {
					CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(slotNr);
					if (item->objectId == objectId) {
						itemCount = countAllItemsOfType(objectId);
						while (itemCount > 0) {
							CTibiaCharacter *self = reader.readSelfCharacter();
							sender.npcSell(objectId, min(itemCount,MAX_BUYSELL_ITEMS));
							itemCount -= min(itemCount,MAX_BUYSELL_ITEMS);
							Sleep (RandomTimeSeller());
							if (CModuleUtil::waitForCapsChange(self->cap)) {
								if (done!=0) done = 1;
							}
							else
								done=0;
							delete self;
						}
					}
				}
			}
			delete cont;
		}		
	}
	return done;
}

int buyItems(CConfigData *config, int traderNum) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	int itemCount, goldCount;
	int done = -1;

	if (itemProxy.getItemId(config->buyItem[traderNum].tradeItem[0].itemName)==0)
		return 1;

	int objectId;
	//char buf[64];
	Sleep (RandomTimeSellerSay(strlen("hi")));
	sender.say("hi");
	Sleep (800);//Give time for NPC window to open
	Sleep (RandomTimeSellerSay(strlen("trade")));
	sender.sayNPC("trade");
	Sleep (RandomTimeSeller());
	for (int j = 0; j < 32; j++) {
		objectId = itemProxy.getItemId(config->buyItem[traderNum].tradeItem[j].itemName);
		//sprintf(buf, "Item Name: %s", config->buyItem[traderNum].tradeItem[j].itemName);
		//AfxMessageBox(buf);
		if (objectId)
			itemCount = countAllItemsOfType(objectId,true);
		else 
			break;
		goldCount = countAllItemsOfType(itemProxy.getValueForConst("GP"),true);
		goldCount += countAllItemsOfType(itemProxy.getValueForConst("PlatinumCoin"),true) * 100;
		goldCount += countAllItemsOfType(itemProxy.getValueForConst("CrystalCoin"),true) * 10000;
		itemCount = config->buyItem[traderNum].tradeItem[j].quantityBuySell - itemCount;
		itemCount = goldCount / config->buyItem[traderNum].tradeItem[j].salePrice >= itemCount?itemCount:goldCount / config->buyItem[traderNum].tradeItem[j].salePrice;
		//sprintf(buf, "Item: %d\nGold: %d\nCount: %d",objectId, goldCount, itemCount);
		//AfxMessageBox(buf);
		
		while (itemCount > 0) {
			CTibiaCharacter *self = reader.readSelfCharacter();
			sender.npcBuy(objectId, min(itemCount,MAX_BUYSELL_ITEMS), 0, 0);
			itemCount -= min(itemCount,MAX_BUYSELL_ITEMS);
			Sleep (RandomTimeSeller());
			if (CModuleUtil::waitForCapsChange(self->cap)) {
				if (done!=0) done = 1;
			}
			else
				done =0;
			delete self;
		}
	}
	return done==1?1:0;
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

bool shouldGo(CConfigData *config) {
	//char buf[64];
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();


	int count = 0;
	bool should = false;
	for (int i = 0; i < MAX_SELLERS; i++) {
		if (config->sellOnCap && self->cap < config->sellWhen && individualShouldGo(config, i)) 
			{delete self; return true;}
		if (config->sellOnSpace && !spaceAvailable() && individualShouldGo(config, i))
			{delete self; return true;}
		for (int j = 0; j < 32; j++) {
			int objectId = itemProxy.getItemId(config->sellItem[i].tradeItem[j].itemName);
			if (objectId) {
				//sprintf(buf, "%s\nItem count: %d\nTrigger Quantity: %d", config->sellItem[i].tradeItem[j].itemName, countAllItemsOfType(objectId), config->sellItem[i].tradeItem[j].quantityBuySell);
				//AfxMessageBox(buf);
				count = countAllItemsOfType(objectId);
				if (count && count >= config->sellItem[i].tradeItem[j].quantityBuySell)
					should = true;
			}

		}
		for (j = 0; j < 32; j++) {
			int objectId = itemProxy.getItemId(config->buyItem[i].tradeItem[j].itemName);
			if (objectId) {
				//sprintf(buf, "%s\nItem count: %d\nTrigger Quantity: %d", config->sellItem[i].tradeItem[j].itemName, countAllItemsOfType(objectId), config->sellItem[i].tradeItem[j].quantityBuySell);
				//AfxMessageBox(buf);
				count = countAllItemsOfType(itemProxy.getValueForConst("GP"),true);
				count += countAllItemsOfType(itemProxy.getValueForConst("PlatinumCoin"),true) * 100;
				count += countAllItemsOfType(itemProxy.getValueForConst("CrystalCoin"),true) * 10000;
			
			if (countAllItemsOfType(objectId,true) < config->buyItem[i].tradeItem[j].triggerQuantity && count >= config->buyItem[i].tradeItem[j].salePrice)
				should = true;
			}
		}
	}
//	should?AfxMessageBox("Should go"):AfxMessageBox("Should not go");
	delete self;
	return should;
}

bool canGo(CConfigData *config) {
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;

	int count = 0;
	for (int i = 0; i < MAX_SELLERS; i++) {
		//see if there's anything to 
		for (int j = 0; j < 32; j++) {
			int objectId = itemProxy.getItemId(config->sellItem[i].tradeItem[j].itemName);
			if (objectId) {
				//sprintf(buf, "%s\nItem count: %d\nTrigger Quantity: %d", config->sellItem[i].tradeItem[j].itemName, countAllItemsOfType(objectId), config->sellItem[i].tradeItem[j].quantityBuySell);
				//AfxMessageBox(buf);
				count = countAllItemsOfType(objectId);
				if (count) {
					return true;
				}
			}

		}
		for (j = 0; j < 32; j++) {
			int objectId = itemProxy.getItemId(config->buyItem[i].tradeItem[j].itemName);
			if (objectId) {
				//sprintf(buf, "%s\nItem count: %d\nTrigger Quantity: %d", config->sellItem[i].tradeItem[j].itemName, countAllItemsOfType(objectId), config->sellItem[i].tradeItem[j].quantityBuySell);
				//AfxMessageBox(buf);
				count = countAllItemsOfType(itemProxy.getValueForConst("GP"),true);
				count += countAllItemsOfType(itemProxy.getValueForConst("PlatinumCoin"),true) * 100;
				count += countAllItemsOfType(itemProxy.getValueForConst("CrystalCoin"),true) * 10000;
			
			if (countAllItemsOfType(objectId,true) < config->buyItem[i].tradeItem[j].quantityBuySell && count >= config->buyItem[i].tradeItem[j].salePrice)
				return true;
			}
		}
	}
//	should?AfxMessageBox("Should go"):AfxMessageBox("Should not go");
	return false;
}

int individualShouldGo(CConfigData *config, int traderNum) {
	//char buf[64];
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	int ret = NOGO;
	for (int j = 0; j < 32; j++) {
		int objectId = itemProxy.getItemId(config->sellItem[traderNum].tradeItem[j].itemName);
		if (!objectId) break;

		//sprintf(buf, "Seller: %d\nObjectID: %d", traderNum+1, objectId);
		//AfxMessageBox(buf);
		if (objectId && countAllItemsOfType(objectId) > 0)
			ret = SELLONLY;
	}
	int count = -1;
	for (j = 0; j < 32; j++) {
		int objectId = itemProxy.getItemId(config->buyItem[traderNum].tradeItem[j].itemName);
		if (!objectId) break;
		//sprintf(buf, "Seller: %d\nObjectID: %d", traderNum+1, objectId);
		//AfxMessageBox(buf);
		if (count==-1){
			count = countAllItemsOfType(itemProxy.getValueForConst("GP"),true);
			count += countAllItemsOfType(itemProxy.getValueForConst("PlatinumCoin"),true) * 100;
			count += countAllItemsOfType(itemProxy.getValueForConst("CrystalCoin"),true) * 10000;
		}

		if (objectId && countAllItemsOfType(objectId,true) < config->buyItem[traderNum].tradeItem[j].quantityBuySell && count >= config->buyItem[traderNum].tradeItem[j].salePrice) {
			if (ret == SELLONLY)
				ret = DOBOTH;
			else
				ret = BUYONLY;
		}
	}
//	if (ret == NOGO) AfxMessageBox("No business for this seller.");
//	if (ret == BUYONLY) AfxMessageBox("No SELL business for this seller.");
//	if (ret == SELLONLY) AfxMessageBox("No BUY business for this seller.");
//	if (ret == DOBOTH) AfxMessageBox("We have business for this seller.");
	return ret;
}

int spaceAvailable() {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();

	int contNr;
	int hasSpace=0;
	for (contNr = 0; contNr < memConstData.m_memMaxContainers; contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);
		
		if (cont->flagOnOff && cont->itemsInside < cont->size)
			hasSpace = 1;
		delete cont;		
	}
	return hasSpace;
}



void CMod_SellerApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (m_configDialog){
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
