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
int RandomVariableMana(int pt,int command,CConfigData *config){
	if (!config->randomCast) return *(int*)pt;

	CMemReaderProxy reader;
	static map<int,int> setMana;
	if (!setMana[pt]) command=MAKE;
	if (command==MAKE){
		// within 10% of number with a min of pt and a max of maxMana
		setMana[pt]=CModuleUtil::randomFormula((int)(*(int*)pt),(int)((*(int*)pt)*0.1),(int)(*(int*)pt),reader.readSelfCharacter()->maxMana+1);
	}
	return setMana[pt];
}

/**
 * Return >=0: ok; <0: error
 */
int moveBlankRuneToHand(CConfigData *config,int handAddress,int locId)
{
	int pos;
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	int blankRuneContNr=-1;
	int blankRuneContPos=-1;

	// step 1. find blank and put into hand
	// scan safe containers
	for (pos=0;pos<10;pos++)
	{
		int itemNr;
		
		CTibiaContainer *container = reader.readContainer(pos);
		if (container->flagOnOff)
		{
			for (itemNr=0;itemNr<container->itemsInside;itemNr++)
			{
				CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);
				if (item->objectId==itemProxy.getValueForConst("runeBlank"))
				{
					// blank rune found!
					blankRuneContNr=container->number;
					blankRuneContPos=item->pos;
					itemNr=1000;
					break;
				};
			};				
		}
		
		delete container;
	};
	
		
	// send the blank rune to a hand
	sender.moveObjectBetweenContainers(itemProxy.getValueForConst("runeBlank"),0x40+blankRuneContNr,blankRuneContPos,locId,0,1);
	Sleep(CModuleUtil::randomFormula(500,200));
	
	// wait for the blank rune to appear in the a hand
	int runeInHand=0;
	int iters=0;
	while (!runeInHand)
	{
		CTibiaItem *item = reader.readItem(handAddress);		
		if (item->objectId!=itemProxy.getValueForConst("runeBlank")&&item->objectId!=0)
			runeInHand=2;
		if (item->objectId==itemProxy.getValueForConst("runeBlank"))
			runeInHand=1;
		Sleep(50);
		iters++;
		// if rune is not in hand after 10 seconds then something went wrong
		if (iters>10000/50)
			runeInHand=2;
		delete item;
	};
	// when runeInHand==2 then we hit some big problem, so we cancel 
	// the operation
	if (runeInHand==2)
		return -1;
	return blankRuneContNr;
}

void moveRuneBankToContainer(int handAddress,int locId,int targetContNr)
{
	CMemReaderProxy reader;
	CPackSenderProxy sender;	
	CTibiaItemProxy itemProxy;

	// wait for something except blank rune to appear in the hand
	int runeInHand=0;
	int iters=0;
	int itemBackToContainerLoop=0;
	// because this is important step - repeat it many times if failed
	for (itemBackToContainerLoop=0;itemBackToContainerLoop<5;itemBackToContainerLoop++)
	{
		while (!runeInHand&&iters++<5000/50)
		{
			CTibiaItem *item = reader.readItem(handAddress);
			if (item->objectId!=itemProxy.getValueForConst("runeBlank")||item->objectId==0)
			{
				runeInHand=1;										
			}
			Sleep(50);
			iters++;					
		};
		
		// send the blank rune to the blank rune's container 
		CTibiaItem *item = reader.readItem(handAddress);			
		sender.moveObjectBetweenContainers(item->objectId,locId,0,0x40+targetContNr,0,1);
		Sleep(CModuleUtil::randomFormula(500,200));
		delete item;
		runeInHand=0;
		iters=0;
		
		while (!runeInHand&&iters++<5000/50)
		{
			
			CTibiaItem *item = reader.readItem(handAddress);			
			if (item->objectId==0)
			{
				runeInHand=1;
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

	for (contNr=0;contNr<10;contNr++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);

		if (cont->flagOnOff&&cont->itemsInside<cont->size)
		{
			sender.moveObjectBetweenContainers(item->objectId,0x06,0,0x40+contNr,0,item->quantity?item->quantity:1);
			Sleep(CModuleUtil::randomFormula(500,200));
			delete cont;
			delete item;
			return;
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

	for (contNr=0;contNr<10;contNr++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);

		if (cont->flagOnOff&&cont->itemsInside<cont->size)
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
	if (config->useArrow||config->useBackpack)
	{
		CTibiaItem *arrowItem = reader.readItem(memConstData.m_memAddressLeftHand);
		leftHandObjectId=arrowItem->objectId;
		delete arrowItem;
	}

	while (!toolThreadShouldStop)
	{			
		CTibiaCharacter *myself = reader.readSelfCharacter();
		
		Sleep(900);
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
				
		
		CTibiaItem *handItem = reader.readItem(memConstData.m_memAddressLeftHand);
		if (handItem->objectId&&handItem->objectId!=leftHandObjectId)
		{
		/**
		* Cleanup left hand. There can be only: leftHandObjectId - other
		* items we move to the first container
			*/
			moveLeftHandToContainer();
			delete handItem;
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
				continue;
			}				
			delete arrowItem;
			
		}		
		delete handItem;
		
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
				blanksCount+=container->countItemsOfType(itemProxy.getValueForConst("runeBlank"));					
			};
			delete container;
		}

		int manaLimit = RandomVariableMana(config->makeNow?((int)&config->mana):((int)&config->manaLimit),GET,config);
		

		if (myself->mana>=manaLimit){
			RandomVariableMana(config->makeNow?((int)&config->mana):((int)&config->manaLimit),MAKE,config);
			bMakeRune = 1;
			//config->makeNow = 0;
		}else if(myself->mana < config->mana){
			bMakeRune = 0;
		}

		if (bMakeRune && myself->soulPoints>=config->soulPoints && blanksCount)
		{		
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
					// we must skip container 7,8,9 due to cave bot!
					for (saveContNr=0;saveContNr<7;saveContNr++)
					{
						CTibiaContainer *saveCont=reader.readContainer(saveContNr);
						if (saveCont->flagOnOff&&saveCont->itemsInside<saveCont->size)
						{
							// some open container with some space inside found
							sender.moveObjectBetweenContainers(handItem->objectId,0x06,0,0x40+saveContNr,saveCont->size-1,handItem->quantity?handItem->quantity:1);
							CModuleUtil::waitForItemsInsideChange(saveContNr,saveCont->itemsInside);

						}
						delete saveCont;
					}
					sender.moveObjectBetweenContainers(handItem->objectId,0x06,0,0x0a,0,handItem->quantity?handItem->quantity:1);
					Sleep(CModuleUtil::randomFormula(500,200));
					delete handItem;				
					continue;	
				}
				delete handItem;
			}
			
			int useRightHand=0;
			
			
			// step 0. check if left hand is empty - if not - we check 'arrow' area									
			
			CTibiaItem *item = reader.readItem(memConstData.m_memAddressLeftHand);
			if (item->objectId)
			{
				// left hand is busy - we can't make any runes :(
				delete item;
				continue;
			}
			
			// step 0.a - check right hand
			if (config->makeTwo)
			{
				CTibiaItem *rightHandItem = reader.readItem(memConstData.m_memAddressRightHand);
				if (rightHandItem->objectId==0)
				{
					useRightHand=1;
				}
				delete rightHandItem;
			} 
									
			int blankContNrLeft=moveBlankRuneToHand(config,memConstData.m_memAddressLeftHand,0x06);			
			
			int blankContNrRight=-1;
			if (blankContNrLeft<0)
				continue;
			
			if (useRightHand)
			{
				blankContNrRight=moveBlankRuneToHand(config,memConstData.m_memAddressRightHand,0x05);
				if (blankContNrRight<0)
					continue;
			}
			
			
			
			// step 2. cast spell
			sender.say((LPCTSTR)config->spell);

			if(!config->maxUse){
				bMakeRune=0;
			}
			
			
			// step 3. put rune into container
			moveRuneBankToContainer(memConstData.m_memAddressLeftHand,0x06,blankContNrLeft);
			if (useRightHand)
			{
				moveRuneBankToContainer(memConstData.m_memAddressRightHand,0x05,blankContNrRight);												
			}
			
		} else {
		/** 
		* If we are here, then we don't have to make runes, but we should
		* rather make sure that (in case we use arrow slot for item saving
		* correct item is in hand.
			*/
			bMakeRune = 0;
			if (config->useArrow)
			{
				CTibiaItem *handItem = reader.readItem(memConstData.m_memAddressLeftHand);							
				if (!handItem->objectId||handItem->objectId!=leftHandObjectId)
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
					for (saveContNr=0;saveContNr<7;saveContNr++)
					{
						CUIntArray itemsAccepted;
						itemsAccepted.Add(leftHandObjectId);
						CTibiaItem *savedItem = CModuleUtil::lookupItem(saveContNr,&itemsAccepted);
						if (savedItem)
						{
							sender.moveObjectBetweenContainers(leftHandObjectId,0x40+saveContNr,savedItem->pos,0x06,0,savedItem->quantity?savedItem->quantity:1);
							Sleep(CModuleUtil::randomFormula(1000,300));
							delete savedItem;
							saveContNr=999;
						}																		
					}
					
					
				}
				delete handItem;
			}
		}
		
		delete myself;		

	}
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
	if (!strcmp(paramName,"makeTwo")) m_configData->makeTwo=atoi(paramValue);
	if (!strcmp(paramName,"useArrow")) m_configData->useArrow=atoi(paramValue);
	if (!strcmp(paramName,"useBackpack")) m_configData->useBackpack=atoi(paramValue);
	if (!strcmp(paramName,"premium")) m_configData->premium=atoi(paramValue);
	if (!strcmp(paramName,"maxUse")) m_configData->maxUse=atoi(paramValue);
	if (!strcmp(paramName,"randomCast")) m_configData->randomCast=atoi(paramValue);

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
	if (!strcmp(paramName,"makeTwo")) sprintf(buf,"%d",m_configData->makeTwo);
	if (!strcmp(paramName,"useArrow")) sprintf(buf,"%d",m_configData->useArrow);
	if (!strcmp(paramName,"useBackpack")) sprintf(buf,"%d",m_configData->useBackpack);
	if (!strcmp(paramName,"premium")) sprintf(buf,"%d",m_configData->premium);
	if (!strcmp(paramName,"maxUse")) sprintf(buf,"%d",m_configData->maxUse);
	if (!strcmp(paramName,"randomCast")) sprintf(buf,"%d",m_configData->randomCast);

	if (!strcmp(paramName,"spells/spell")&&m_configData->listSpells[m_currentSpellNr].words[0] != '0')
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
	case 4: return "makeTwo";
	case 5: return "useArrow";
	case 6: return "premium";
	case 7: return "maxUse";
	case 8: return "spells/spell";
	case 9: return "useBackpack";
	case 10: return "randomCast";
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
