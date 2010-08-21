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


// mod_runemaker.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_runemaker.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include <map>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GET 0
#define MAKE 1

/////////////////////////////////////////////////////////////////////////////
// CMod_runemakerApp

BEGIN_MESSAGE_MAP(CMod_runemakerApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_runemakerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		// DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
const int numCont=16;//"maxContainers"
int openContTime[numCont];
const int minSecondsOpen=20;


static map<int,int> setMana;
int RandomVariableMana(int pt,int command,CConfigData *config){
	if (!config->randomCast) return pt;

	CMemReaderProxy reader;
	if (!setMana[pt]) command=MAKE;
	if (command==MAKE){
		// within 10% of number with a min of pt and a max of maxMana
		CTibiaCharacter* self=reader.readSelfCharacter();
		setMana[pt]=CModuleUtil::randomFormula(pt,pt * 0.1,pt,max(self->maxMana,pt+1));
		delete self;
	}
	return pt;
}

/**
 * Return >=0: ok; <0: error
 */
int moveSpearToHand(CConfigData *config,int handAddress,int locId)
{
	int contNr;
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	int spearContNr=-1;
	int spearContPos=-1;
	int spearId=-1;

	// step 1. find spear and put into hand
	// scan safe containers
	for (contNr=0;contNr<numCont;contNr++)
	{
		int itemNr;
		
		CTibiaContainer *container = reader.readContainer(contNr);
		if (container->flagOnOff && openContTime[contNr] && time(NULL)-openContTime[contNr]>minSecondsOpen)
		{
			for (itemNr=0;itemNr<container->itemsInside;itemNr++)
			{
				CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);
				if (item->objectId==itemProxy.getValueForConst("spear"))
				{
					// spear found!
					spearContNr=container->number;
					spearContPos=item->pos;
					spearId=item->objectId;
					contNr=9999;//exit both loops
					break;
				};
			};
		}
		delete container;
	};
	
		
	// send the blank rune to a hand
	sender.moveObjectBetweenContainers(spearId,0x40+spearContNr,spearContPos,locId,0,1);
	Sleep(CModuleUtil::randomFormula(500,200));
	
	// wait for the spear to appear in the a hand
	int spearInHand=0;
	int iters=0;
	while (!spearInHand)
	{
		CTibiaItem *item = reader.readItem(handAddress);		
		if (item->objectId!=0&&item->objectId!=itemProxy.getValueForConst("spear"))
			spearInHand=2;//item in hand but not spear
		if (item->objectId==itemProxy.getValueForConst("spear"))
			spearInHand=1;
		Sleep(50);
		iters++;
		// if rune is not in hand after 5 seconds then something went wrong
		if (iters>5000/50)
			spearInHand=2;
		delete item;
	};
	// when spearInHand==2 then we hit some big problem, so we cancel 
	// the operation
	if (spearInHand==2)
		return -1;
	return spearContNr;
}

void moveSpearBackToContainer(int handAddress,int locId,int targetContNr,CConfigData *config)
{
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;

	// wait for something except blank rune to appear in the hand
	int objectMoved=0;
	int iters=0;
	int itemBackToContainerLoop=0;
	// because this is important step - repeat it many times if failed
	for (itemBackToContainerLoop=0;itemBackToContainerLoop<5;itemBackToContainerLoop++)
	{
		while (!objectMoved&&iters++<5000/50)
		{
			CTibiaItem *item = reader.readItem(handAddress);
			if (item->objectId==0 || item->objectId!=itemProxy.getValueForConst("spear"))
			{
				objectMoved=1;
			}
			Sleep(50);
			iters++;
			delete item;
		};
		
		// send the spear to the spear's container 
		CTibiaItem *item = reader.readItem(handAddress);
		sender.moveObjectBetweenContainers(item->objectId,locId,0,0x40+targetContNr,0,1);
		Sleep(CModuleUtil::randomFormula(500,200));
		delete item;
		objectMoved=0;
		iters=0;
		
		while (!objectMoved&&iters++<5000/50)
		{
			
			CTibiaItem *item = reader.readItem(handAddress);			
			if (item->objectId==0)
			{
				objectMoved=1;
				itemBackToContainerLoop=100;
			}
			Sleep(50);					
			delete item;
		}
	}	
}

void moveLeftHandToContainer()
{
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaItem *item = reader.readItem(memConstData.m_memAddressLeftHand);
	int contNr;

	for (contNr=0;contNr<numCont;contNr++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);

		if (time(NULL)-openContTime[contNr]>minSecondsOpen && openContTime[contNr]){
			if (cont->flagOnOff && cont->itemsInside<cont->size)
			{
				sender.moveObjectBetweenContainers(item->objectId,0x06,0,0x40+contNr,0,item->quantity?item->quantity:1);
				Sleep(CModuleUtil::randomFormula(500,200));
				delete cont;
				delete item;
				return;
			}
		}

		delete cont;
	}
	delete item;
}

void moveArrowItemToContainer()
{
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaItem *item = reader.readItem(memConstData.m_memAddressSlotArrow);	
	int contNr;

	for (contNr=0;contNr<numCont;contNr++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);

		if (cont->flagOnOff&&cont->itemsInside<cont->size&& openContTime[contNr] && time(NULL)-openContTime[contNr]>minSecondsOpen)
		{
			sender.moveObjectBetweenContainers(item->objectId,0x0a,0,0x40+contNr,0,item->quantity?item->quantity:1);
			Sleep(CModuleUtil::randomFormula(500,200));
			delete cont;
			delete item;
			return;
		}

		delete cont;
	}
	delete item;
}


/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	int bMakeRune=0;


	int leftHandObjectId=0;
	if (config->useSpear && (config->useArrow||config->useBackpack))
	{
		CTibiaItem *handItem = reader.readItem(memConstData.m_memAddressLeftHand);
		leftHandObjectId=handItem->objectId;
		delete handItem;
	}

	//Containers open on start are okay to put stuff in
	for (int contNr=0;contNr<numCont;contNr++){
		CTibiaContainer *cont = reader.readContainer(contNr);
		if (cont->flagOnOff) {
			openContTime[contNr]=time(NULL)-minSecondsOpen-1;
		} else {
			openContTime[contNr]=0;
		}
	}
	while (!toolThreadShouldStop)
	{			
		Sleep(900);
		//Keep track of when containers are opened. Later that container will only be
		//used if time(NULL)-openContTime[i]>minSecondsOpen
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		CTibiaCharacter *myself = reader.readSelfCharacter();
				
		
		if (config->useSpear){
			CTibiaItem *handItem = reader.readItem(memConstData.m_memAddressLeftHand);
			for (int contNr=0;contNr<numCont;contNr++){
				CTibiaContainer *cont = reader.readContainer(contNr);
				if (cont->flagOnOff)
				{
					if (!openContTime[contNr]) openContTime[contNr]=time(NULL);
				} else {
					openContTime[contNr]=0;
				}
			}
			
			if (handItem->objectId&&handItem->objectId!=leftHandObjectId)
			{
			/**
			* Cleanup left hand. There can be only: leftHandObjectId - other
			* items we move to the first container
			*/
				moveLeftHandToContainer();
				delete handItem;
				delete myself;
				continue;
			}
			
			if (config->useArrow)
			{
				CTibiaItem *arrowItem = reader.readItem(memConstData.m_memAddressSlotArrow);
				
				if (arrowItem->objectId&&(arrowItem->objectId!=leftHandObjectId||arrowItem->objectId==handItem->objectId))
				{
					// cleanup arrow item
					moveArrowItemToContainer();
					delete handItem;
					delete arrowItem;
					delete myself;
					continue;
				}				
				delete arrowItem;
				
			}		
			delete handItem;
		}
		
		/**
		 * So now the situation is: left hand clear or The Item in it, 		
		 * 
		 * So now - count blanks - if 0 we can't do anything :(.
		*/
		
		int blanksCount=0;
		for (int i=0;i<memConstData.m_memMaxContainers;i++)
		{
			CTibiaContainer *container = reader.readContainer(i);
			if (container->flagOnOff)
			{
				if(config->useSpear){
					blanksCount+=container->countItemsOfType(itemProxy.getValueForConst("spear"));
				} else {
					blanksCount+=container->countItemsOfType(itemProxy.getValueForConst("runeBlank"));					
				}
			};
			delete container;
		}

		int manaLimit = RandomVariableMana(config->makeNow?config->mana:config->manaLimit,GET,config);
		

		if (myself->mana>=manaLimit){
			RandomVariableMana(config->makeNow?config->mana:config->manaLimit,MAKE,config);
			bMakeRune = 1;
		}else if(myself->mana < config->mana){
			bMakeRune = 0;
		}

		if (bMakeRune && myself->soulPoints>=config->soulPoints && blanksCount)
		{		
			if (config->useSpear){
				if (config->useArrow)
				{
					// make sure that left hand is saved to an arrow slot
					CTibiaItem *handItem = reader.readItem(memConstData.m_memAddressLeftHand);
					if (handItem->objectId)
					{
						/**
						 * So now the situation is: we have empty arrow, and The Item
						 * in left hand. So move it.
						 */
						sender.moveObjectBetweenContainers(handItem->objectId,0x06,0,0x0a,0,handItem->quantity?handItem->quantity:1);
						Sleep(CModuleUtil::randomFormula(500,200));
						delete handItem;
						delete myself;
						continue;
					}
					delete handItem;
				}
				if (config->useBackpack)
				{
					// make sure the left hand is save to some backpack
					CTibiaItem *handItem = reader.readItem(memConstData.m_memAddressLeftHand);
					if (handItem->objectId)
					{					
						int saveContNr;
						for (saveContNr=0;saveContNr<numCont;saveContNr++)
						{
							if (openContTime[saveContNr] && time(NULL)-openContTime[saveContNr]>minSecondsOpen){
								CTibiaContainer *saveCont=reader.readContainer(saveContNr);
								if (saveCont->flagOnOff&&saveCont->itemsInside<saveCont->size)
								{
									// some open container with some space inside found
									sender.moveObjectBetweenContainers(handItem->objectId,0x06,0,0x40+saveContNr,saveCont->size-1,handItem->quantity?handItem->quantity:1);
									CModuleUtil::waitForItemsInsideChange(saveContNr,saveCont->itemsInside);
									Sleep(CModuleUtil::randomFormula(500,200));
									delete saveCont;
									break;

								}
								delete saveCont;
							}
						}
						delete handItem;
						delete myself;
						continue;
					}
					delete handItem;
				}
			
			
				// step 0. check if left hand is empty - if not - we check 'arrow' area									
				
				CTibiaItem *item = reader.readItem(memConstData.m_memAddressLeftHand);
				if (item->objectId)
				{
					// left hand is busy - we can't make any runes :(
					delete item;
					delete myself;
					continue;
				}
				delete item;
				
										
				int spearContNrLeft=moveSpearToHand(config,memConstData.m_memAddressLeftHand,0x06);			
				
				if (spearContNrLeft<0){
					delete myself;
					continue;
				}
				
			}
			// step 2. cast spell
			sender.say((LPCTSTR)config->spell);

			if(!config->maxUse){
				bMakeRune=0;
			}
			
			if (config->useSpear){
				// step 3. put spear into container
				moveLeftHandToContainer();
			}
			
		} else if(config->useSpear) {
		/** 
		* If we are here, then we don't have to make spears, but we should
		* rather make sure that (in case we use arrow slot for item saving
		* correct item is in hand.
			*/
			bMakeRune = 0;
			if (config->useArrow)
			{
				CTibiaItem *handItem = reader.readItem(memConstData.m_memAddressLeftHand);						
				if (handItem->objectId==0)
				{					
					CTibiaItem *arrowItem = reader.readItem(memConstData.m_memAddressSlotArrow);
					if (arrowItem->objectId==leftHandObjectId)
					{
						// The Item is in arrow slot and left hand is empty
						sender.moveObjectBetweenContainers(arrowItem->objectId,0x0a,0,0x06,0,arrowItem->quantity?arrowItem->quantity:1);
						Sleep(CModuleUtil::randomFormula(500,200));
					} else {
						// AAAAAAAAAAAA: we lost The Item !!!
						// AAAAAAAAAAAA
						sender.sendTAMessage("WARNING: I cannot find The Item in neither left hand nor arrow slot!");
					}
					delete arrowItem;
				}
				
				delete handItem;
			}
			if (config->useBackpack)
			{
				// if we are using 'save to backpack' feature, we need to lookup the item
				// in some backpack and move it to hand
				CTibiaItem *handItem = reader.readItem(memConstData.m_memAddressLeftHand);							
				if (!handItem->objectId||handItem->objectId!=leftHandObjectId)
				{					
					int saveContNr;
					// we must skip container 7,8,9 due to cave bot!
					for (saveContNr=0;saveContNr<numCont;saveContNr++)
					{
						CUIntArray itemsAccepted;
						itemsAccepted.Add(leftHandObjectId);
						CTibiaItem *savedItem = CModuleUtil::lookupItem(saveContNr,&itemsAccepted);
						if (savedItem->objectId)
						{
							sender.moveObjectBetweenContainers(leftHandObjectId,0x40+saveContNr,savedItem->pos,0x06,0,savedItem->quantity?savedItem->quantity:1);
							Sleep(CModuleUtil::randomFormula(1000,300));
							delete savedItem;
							break;
						}		
						delete savedItem;
					}
				}
				delete handItem;
			}
		}
		delete myself;		
	}
	setMana.clear();
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_runemakerApp construction

CMod_runemakerApp::CMod_runemakerApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_runemakerApp::~CMod_runemakerApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_runemakerApp::getName()
{
	return "Rune maker";
}


int CMod_runemakerApp::isStarted()
{
	return m_started;
}


void CMod_runemakerApp::start()
{	
	superStart();
	if (m_configDialog)
	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

	DWORD threadId;
		
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_runemakerApp::stop()
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

void CMod_runemakerApp::showConfigDialog()
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


void CMod_runemakerApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_runemakerApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_runemakerApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_runemakerApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}

void CMod_runemakerApp::makeNow(int enable)
{

	m_configData->makeNow = enable;

}

char *CMod_runemakerApp::getVersion()
{
	return "1.4";
}


int CMod_runemakerApp::validateConfig(int showAlerts)
{
	if (m_configData->soulPoints<0)
	{
		if (showAlerts) AfxMessageBox("Soul points must be >=0!");
		return 0;
	}
	if (strlen(m_configData->spell)==0)
	{
		if (showAlerts) AfxMessageBox("Please enter rune spell!");
		return 0;
	}
	if (m_configData->useBackpack&&m_configData->useArrow)
	{
		if (showAlerts) AfxMessageBox("At most one of 'save backpack' and 'save to arrow slot' can be set at once!");
		return 0;
	}
	
	return 1;
}

void CMod_runemakerApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_runemakerApp::loadConfigParam(char *paramName,char *paramValue)
{

	if (!strcmp(paramName,"spell")) strcpy(m_configData->spell,paramValue);
	if (!strcmp(paramName,"mana")) m_configData->mana=atoi(paramValue);
	if (!strcmp(paramName,"manaLimit")) m_configData->manaLimit=atoi(paramValue);
	if (!strcmp(paramName,"soulPoints")) m_configData->soulPoints=atoi(paramValue);
	if (!strcmp(paramName,"useArrow")) m_configData->useArrow=atoi(paramValue);
	if (!strcmp(paramName,"useBackpack")) m_configData->useBackpack=atoi(paramValue);
	if (!strcmp(paramName,"premium")) m_configData->premium=atoi(paramValue);
	if (!strcmp(paramName,"maxUse")) m_configData->maxUse=atoi(paramValue);
	if (!strcmp(paramName,"randomCast")) m_configData->randomCast=atoi(paramValue);
	if (!strcmp(paramName,"useSpear")) m_configData->useSpear=atoi(paramValue);

	if (!strcmp(paramName,"spells/spell")) 
	{
		if (m_currentSpellNr==0)
		{
			int i;
			for (i=0;i<15;i++)
			{
				m_configData->listSpells[i].words[0]='0';
			}
		}
		sscanf(paramValue,"%d,%d,%[a-zA-Z0-9+\"\'\\/ \,\-]",
			&m_configData->listSpells[m_currentSpellNr].mana,
			&m_configData->listSpells[m_currentSpellNr].soulPoints,
			&m_configData->listSpells[m_currentSpellNr].words);
		m_currentSpellNr++;	
		
	}
}	

char *CMod_runemakerApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"spell")) sprintf(buf,"%s",m_configData->spell);
	if (!strcmp(paramName,"mana")) sprintf(buf,"%d",m_configData->mana);
	if (!strcmp(paramName,"manaLimit")) sprintf(buf,"%d",m_configData->manaLimit);
	if (!strcmp(paramName,"soulPoints")) sprintf(buf,"%d",m_configData->soulPoints);
	if (!strcmp(paramName,"useArrow")) sprintf(buf,"%d",m_configData->useArrow);
	if (!strcmp(paramName,"useBackpack")) sprintf(buf,"%d",m_configData->useBackpack);
	if (!strcmp(paramName,"premium")) sprintf(buf,"%d",m_configData->premium);
	if (!strcmp(paramName,"maxUse")) sprintf(buf,"%d",m_configData->maxUse);
	if (!strcmp(paramName,"randomCast")) sprintf(buf,"%d",m_configData->randomCast);
	if (!strcmp(paramName,"useSpear")) sprintf(buf,"%d",m_configData->useSpear);

	if (!strcmp(paramName,"spells/spell")&&m_configData->listSpells[m_currentSpellNr].words[0] != '0'&&m_currentSpellNr<15)
	{
		sprintf(buf,"%d,%d,%s",m_configData->listSpells[m_currentSpellNr].mana,m_configData->listSpells[m_currentSpellNr].soulPoints,m_configData->listSpells[m_currentSpellNr].words);
		m_currentSpellNr++;
	}

	return buf;
}

char *CMod_runemakerApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "spell";
	case 1: return "mana";
	case 2: return "manaLimit";
	case 3: return "soulPoints";
	case 4: return "useArrow";
	case 5: return "premium";
	case 6: return "maxUse";
	case 7: return "spells/spell";
	case 8: return "useBackpack";
	case 9: return "randomCast";
	case 10: return "useSpear";
	default:
		return NULL;
	}
}

int CMod_runemakerApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName,"spells/spell")) return 1;
	return 0;
}

void CMod_runemakerApp::resetMultiParamAccess(char *paramName)
{
	if (!strcmp(paramName,"spells/spell")) m_currentSpellNr=0;
}

void CMod_runemakerApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	skin.SetButtonSkin( m_configDialog->m_addSpell);
	skin.SetButtonSkin( m_configDialog->m_deleteSpell);
	skin.SetButtonSkin( m_configDialog->m_loadSpell);
	skin.SetButtonSkin( m_configDialog->m_makeNow);
	skin.SetButtonSkin(	m_configDialog->m_OK);
	skin.SetButtonSkin(	m_configDialog->m_enable);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (m_configDialog)
		m_configDialog->Invalidate();
}
