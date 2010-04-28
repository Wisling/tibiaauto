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


// mod_fluid.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_fluid.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include "math.h"
#include <map>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GET 0
#define MAKE 1
#define MAX_DRINK_FAILS 1000

/////////////////////////////////////////////////////////////////////////////
// CMod_fluidApp

BEGIN_MESSAGE_MAP(CMod_fluidApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_fluidApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions

int drinkFails=0;
static map<int,int> setHp;
static map<int,int> setMana;

int RandomVariableHp(int pt,int command,CConfigData *config){
	if (!config->randomCast) return pt;

	CMemReaderProxy reader;
	if (!setHp[pt]) command=MAKE;
	if (command==MAKE){
		// within 10% of number with a min of pt and a max of maxHp
		CTibiaCharacter* self=reader.readSelfCharacter();
		setHp[pt]=CModuleUtil::randomFormula(pt,pt*0.05,max(self->maxHp,pt+1));
		delete self;
	}
	return setHp[pt];
}

int RandomVariableMana(int pt,int command,CConfigData *config){
	if (!config->randomCast) return pt;

	CMemReaderProxy reader;
	if (!setMana[pt]) command=MAKE;
	if (command==MAKE){
		// within 10% of number with a min of pt and a max of maxMana
		CTibiaCharacter* self=reader.readSelfCharacter();
		setMana[pt]=CModuleUtil::randomFormula(pt,pt*0.05,max(self->maxMana,pt+1));
		delete self;
	}
	return setMana[pt];
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

int tryDrinking(int itemId,int itemType,int drink,int hotkey, int hpBelow,int manaBelow)
{
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	int contNr;
	CUIntArray itemArray;
	int drank=0;
	
	itemArray.Add(itemId);
	CTibiaCharacter *self = reader.readSelfCharacter();
	
	if (hotkey)
	{
		sender.useItemOnCreature(itemId,self->tibiaId);
		if (CModuleUtil::waitForHpManaIncrease(self->hp,self->mana)){//most likely using item succeeded
			drank=1;
			drinkFails=0;
		}else{
			drinkFails +=1;
			drinkFails=(drinkFails>MAX_DRINK_FAILS)?1:drinkFails;
		}
	} else {
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
		{
			CTibiaContainer *cont = reader.readContainer(contNr);
			
			if (cont->flagOnOff)
			{
				CTibiaItem *item;
				if (itemType)
				{
					item=CModuleUtil::lookupItem(contNr,&itemArray,itemType);
				} else {
					item=CModuleUtil::lookupItem(contNr,&itemArray);
				}
				
				if (item->objectId)
				{
					if ((self->hp<hpBelow||hpBelow==-1)&&(self->mana<manaBelow||manaBelow==-1)&&drink)
					{
						sender.useItemFromContainerOnCreature(itemId,0x40+contNr,item->pos,self->tibiaId);
						drank=1;
						drinkFails=0;
					}
					delete item;
					break;
				}
				delete item;
			}
			delete cont;
		}
	}
	delete self;
	return drank;
}

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	while (!toolThreadShouldStop)
	{			

		Sleep(100);
		//Send message if at 10 fails
		if((drinkFails+1)%10==0) {
			sender.sendTAMessage("Health or Mana failed to change when using healing item.");
		}
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected

		int drank=0;
		
		int stopSleepTime=reader.getCurrentTm()+CModuleUtil::randomFormula(config->sleep,200,config->sleep);

		CTibiaCharacter *self = reader.readSelfCharacter();

		int hpBelowU=RandomVariableHp(config->hpBelowU,GET,config);
		int hpBelowG=RandomVariableHp(config->hpBelowG,GET,config);
		int hpBelowS=RandomVariableHp(config->hpBelowS,GET,config);
		int hpBelowN=RandomVariableHp(config->hpBelowN,GET,config);
		int hpBelowH=RandomVariableHp(config->hpBelowH,GET,config);
		int hpBelow=RandomVariableHp(config->hpBelow,GET,config);
		int customItem1Below=RandomVariableHp(config->customItem1Below,GET,config);
			
		int manaBelowG=RandomVariableMana(config->manaBelowG,GET,config);
		int manaBelowS=RandomVariableMana(config->manaBelowS,GET,config);
		int manaBelowN=RandomVariableMana(config->manaBelowN,GET,config);
		int manaBelow=RandomVariableMana(config->manaBelow,GET,config);
		int customItem2Below=RandomVariableMana(config->customItem2Below,GET,config);

		// handle  potions
		if (!drank&&(self->hp<self->maxHp&&self->hp<hpBelowU&&config->drinkHpU)&&self->lvl>=130){
			drank|=tryDrinking(itemProxy.getValueForConst("fluidLifeU"),0,config->drinkHpU,config->useHotkey, hpBelowU,-1);
			if (drank) RandomVariableHp(config->hpBelowU,MAKE,config);
		}
		if (!drank&&(self->hp<self->maxHp&&self->hp<hpBelowG&&config->drinkHpG)&&self->lvl>=80){
			drank|=tryDrinking(itemProxy.getValueForConst("fluidLifeG"),0,config->drinkHpG,config->useHotkey, hpBelowG,-1);
			if (drank) RandomVariableHp(config->hpBelowG,MAKE,config);
		}
		if (!drank&&(self->hp<self->maxHp&&self->hp<hpBelowS&&config->drinkHpS)&&self->lvl>=50){
			drank|=tryDrinking(itemProxy.getValueForConst("fluidLifeS"),0,config->drinkHpS,config->useHotkey, hpBelowS,-1);
			if (drank) RandomVariableHp(config->hpBelowS,MAKE,config);
		}
		if (!drank&&(self->hp<self->maxHp&&self->hp<hpBelowN&&config->drinkHpN)){
			drank|=tryDrinking(itemProxy.getValueForConst("fluidLife"),0,config->drinkHpN,config->useHotkey, hpBelowN,-1);			
			if (drank) RandomVariableHp(config->hpBelowN,MAKE,config);
		}
		if (!drank&&(self->hp<self->maxHp&&self->hp<hpBelowH&&config->drinkHpH)){
			drank|=tryDrinking(itemProxy.getValueForConst("fluidLifeH"),0,config->drinkHpH,config->useHotkey, hpBelowH,-1);			
			if (drank) RandomVariableHp(config->hpBelowH,MAKE,config);
		}
		if (!drank&&(self->hp<self->maxHp&&self->hp<hpBelow&&config->drinkHp)){
			drank|=tryDrinking(itemProxy.getValueForConst("fluid"),11,config->drinkHp,config->useHotkey, hpBelow,-1);
			if (drank) RandomVariableHp(config->hpBelow,MAKE,config);
		}

		if (!drank&&(self->hp<self->maxHp&&self->hp<customItem1Below&&config->customItem1Use)){
			drank|=tryDrinking(config->customItem1Item,0,config->customItem1Use,config->useHotkey, customItem1Below,-1);
			if (drank) RandomVariableHp(config->customItem1Below,MAKE,config);
		}


		if (!drank&&(self->mana<self->maxMana&&self->mana<manaBelowG&&config->drinkManaG)&&self->lvl>=80){
			drank|=tryDrinking(itemProxy.getValueForConst("fluidManaG"),0,config->drinkManaG,config->useHotkey, -1, manaBelowG);
			if (drank) RandomVariableMana(config->manaBelowG,MAKE,config);
		}
		if (!drank&&(self->mana<self->maxMana&&self->mana<manaBelowS&&config->drinkManaS)&&self->lvl>=50){
			drank|=tryDrinking(itemProxy.getValueForConst("fluidManaS"),0,config->drinkManaS,config->useHotkey, -1, manaBelowS);
			if (drank) RandomVariableMana(config->manaBelowS,MAKE,config);
		}
		if (!drank&&(self->mana<self->maxMana&&self->mana<manaBelowN&&config->drinkManaN)){
			drank|=tryDrinking(itemProxy.getValueForConst("fluidMana"),0,config->drinkManaN,config->useHotkey, -1, manaBelowN);			
			if (drank) RandomVariableMana(config->manaBelowN,MAKE,config);
		}
		if (!drank&&(self->mana<self->maxMana&&self->mana<manaBelow&&config->drinkMana)){
			drank|=tryDrinking(itemProxy.getValueForConst("fluid"),10,config->drinkMana,config->useHotkey, -1, manaBelow);
			if (drank) RandomVariableMana(config->manaBelow,MAKE,config);
		}

		if (!drank&&(self->mana<self->maxMana&&self->mana<customItem2Below&&config->customItem2Use)){
			drank|=tryDrinking(config->customItem2Item,0,config->customItem2Use,config->useHotkey, -1, customItem2Below);
			if (drank) RandomVariableMana(config->customItem2Below,MAKE,config);
		}
	
		

		if (drank)
		{
			while(reader.getCurrentTm()<stopSleepTime){
				Sleep(50);
			}
		}

		if (config->dropEmpty)
		{
			CUIntArray itemArray;
			
			itemArray.Add(itemProxy.getValueForConst("fluid"));
			itemArray.Add(itemProxy.getValueForConst("fluidEmpty"));
			itemArray.Add(itemProxy.getValueForConst("fluidEmptyS"));
			itemArray.Add(itemProxy.getValueForConst("fluidEmptyG"));
			int contNr;
			for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
			{
				CTibiaContainer *cont = reader.readContainer(contNr);
				
				if (cont->flagOnOff)
				{
					CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemArray,0);
					if (item->objectId)
					{						
						sender.moveObjectFromContainerToFloor(item->objectId,0x40+contNr,item->pos,self->x,self->y,self->z,1);
						Sleep(CModuleUtil::randomFormula(config->sleep,200,0));
						delete item;
						delete cont;
						break;
					}		
					delete item;
				}
				
				delete cont;
			}
		}

		delete self;
				
	}
	setMana.clear();
	setHp.clear();
	toolThreadShouldStop=0;
	drinkFails=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_fluidApp construction

CMod_fluidApp::CMod_fluidApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();
}

CMod_fluidApp::~CMod_fluidApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;
}

char * CMod_fluidApp::getName()
{
	return "Fluid drinker";
}


int CMod_fluidApp::isStarted()
{
	return m_started;
}


void CMod_fluidApp::start()
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

void CMod_fluidApp::stop()
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

void CMod_fluidApp::showConfigDialog()
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
4	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_fluidApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_fluidApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_fluidApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_fluidApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_fluidApp::getVersion()
{
	return "2.0";
}


int CMod_fluidApp::validateConfig(int showAlerts)
{
	if (m_configData->sleep<0)
	{
		if (showAlerts) AfxMessageBox("Sleep after drink must be >=0!");
		return 0;
	}

	if (m_configData->hpBelowH<0)
	{
		if (showAlerts) AfxMessageBox("'hpH below' must be >=0!");
		return 0;
	}

	if (m_configData->hpBelow<0)
	{
		if (showAlerts) AfxMessageBox("'hp below' must be >=0!");
		return 0;
	}

	if (m_configData->hpBelowN<0)
	{
		if (showAlerts) AfxMessageBox("'hpN below' must be >=0!");
		return 0;
	}

	if (m_configData->hpBelowS<0)
	{
		if (showAlerts) AfxMessageBox("'hpS below' must be >=0!");
		return 0;
	}

	if (m_configData->hpBelowG<0)
	{
		if (showAlerts) AfxMessageBox("'hpG below' must be >=0!");
		return 0;
	}

	if (m_configData->hpBelowU<0)
	{
		if (showAlerts) AfxMessageBox("'hpU below' must be >=0!");
		return 0;
	}

	if (m_configData->manaBelow<0)
	{
		if (showAlerts) AfxMessageBox("'mana below' must be >=0!");
		return 0;
	}
	if (m_configData->manaBelowN<0)
	{
		if (showAlerts) AfxMessageBox("'manaN below' must be >=0!");
		return 0;
	}
	if (m_configData->manaBelowS<0)
	{
		if (showAlerts) AfxMessageBox("'manaS below' must be >=0!");
		return 0;
	}
	if (m_configData->manaBelowG<0)
	{
		if (showAlerts) AfxMessageBox("'manaG below' must be >=0!");
		return 0;
	}
	if (m_configData->customItem1Below<0)
	{
		if (showAlerts) AfxMessageBox("'HP below for custom item 1' must be >=0!");
		return 0;
	}
	if (m_configData->customItem2Below<0)
	{
		if (showAlerts) AfxMessageBox("'Mana below for custom item 2' must be >=0!");
		return 0;
	}
	
	return 1;
}

void CMod_fluidApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_fluidApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"drink/hpH")) m_configData->drinkHpH=atoi(paramValue);
	if (!strcmp(paramName,"drink/hp")) m_configData->drinkHp=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpN")) m_configData->drinkHpN=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpS")) m_configData->drinkHpS=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpG")) m_configData->drinkHpG=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpU")) m_configData->drinkHpU=atoi(paramValue);
	if (!strcmp(paramName,"drink/mana")) m_configData->drinkMana=atoi(paramValue);
	if (!strcmp(paramName,"drink/manaN")) m_configData->drinkManaN=atoi(paramValue);
	if (!strcmp(paramName,"drink/manaS")) m_configData->drinkManaS=atoi(paramValue);
	if (!strcmp(paramName,"drink/manaG")) m_configData->drinkManaG=atoi(paramValue);
	if (!strcmp(paramName,"other/dropEmpty")) m_configData->dropEmpty=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpBelowH")) m_configData->hpBelowH=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpBelow")) m_configData->hpBelow=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpBelowN")) m_configData->hpBelowN=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpBelowS")) m_configData->hpBelowS=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpBelowG")) m_configData->hpBelowG=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpBelowU")) m_configData->hpBelowU=atoi(paramValue);
	if (!strcmp(paramName,"drink/manaBelow")) m_configData->manaBelow=atoi(paramValue);
	if (!strcmp(paramName,"drink/manaBelowN")) m_configData->manaBelowN=atoi(paramValue);
	if (!strcmp(paramName,"drink/manaBelowS")) m_configData->manaBelowS=atoi(paramValue);
	if (!strcmp(paramName,"drink/manaBelowG")) m_configData->manaBelowG=atoi(paramValue);
	if (!strcmp(paramName,"custom/item1Below")) m_configData->customItem1Below=atoi(paramValue);
	if (!strcmp(paramName,"custom/item1Item")) m_configData->customItem1Item=atoi(paramValue);
	if (!strcmp(paramName,"custom/item1Use")) m_configData->customItem1Use=atoi(paramValue);
	if (!strcmp(paramName,"custom/item2Below")) m_configData->customItem2Below=atoi(paramValue);
	if (!strcmp(paramName,"custom/item2Item")) m_configData->customItem2Item=atoi(paramValue);
	if (!strcmp(paramName,"custom/item2Use")) m_configData->customItem2Use=atoi(paramValue);
	if (!strcmp(paramName,"other/sleepAfter")) m_configData->sleep=atoi(paramValue);
	if (!strcmp(paramName,"other/randomCast")) m_configData->randomCast=atoi(paramValue);
	if (!strcmp(paramName,"other/useHotkey")) m_configData->useHotkey=atoi(paramValue);
}

char *CMod_fluidApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"drink/hpH")) sprintf(buf,"%d",m_configData->drinkHpH);
	if (!strcmp(paramName,"drink/hp")) sprintf(buf,"%d",m_configData->drinkHp);
	if (!strcmp(paramName,"drink/hpN")) sprintf(buf,"%d",m_configData->drinkHpN);
	if (!strcmp(paramName,"drink/hpS")) sprintf(buf,"%d",m_configData->drinkHpS);
	if (!strcmp(paramName,"drink/hpG")) sprintf(buf,"%d",m_configData->drinkHpG);
	if (!strcmp(paramName,"drink/hpU")) sprintf(buf,"%d",m_configData->drinkHpU);
	if (!strcmp(paramName,"drink/mana")) sprintf(buf,"%d",m_configData->drinkMana);
	if (!strcmp(paramName,"drink/manaN")) sprintf(buf,"%d",m_configData->drinkManaN);
	if (!strcmp(paramName,"drink/manaS")) sprintf(buf,"%d",m_configData->drinkManaS);
	if (!strcmp(paramName,"drink/manaG")) sprintf(buf,"%d",m_configData->drinkManaG);
	if (!strcmp(paramName,"other/dropEmpty")) sprintf(buf,"%d",m_configData->dropEmpty);
	if (!strcmp(paramName,"drink/hpBelowH")) sprintf(buf,"%d",m_configData->hpBelowH);
	if (!strcmp(paramName,"drink/hpBelow")) sprintf(buf,"%d",m_configData->hpBelow);
	if (!strcmp(paramName,"drink/hpBelowN")) sprintf(buf,"%d",m_configData->hpBelowN);
	if (!strcmp(paramName,"drink/hpBelowS")) sprintf(buf,"%d",m_configData->hpBelowS);
	if (!strcmp(paramName,"drink/hpBelowG")) sprintf(buf,"%d",m_configData->hpBelowG);
	if (!strcmp(paramName,"drink/hpBelowU")) sprintf(buf,"%d",m_configData->hpBelowU);
	if (!strcmp(paramName,"drink/manaBelow")) sprintf(buf,"%d",m_configData->manaBelow);
	if (!strcmp(paramName,"drink/manaBelowN")) sprintf(buf,"%d",m_configData->manaBelowN);
	if (!strcmp(paramName,"drink/manaBelowS")) sprintf(buf,"%d",m_configData->manaBelowS);
	if (!strcmp(paramName,"drink/manaBelowG")) sprintf(buf,"%d",m_configData->manaBelowG);
	if (!strcmp(paramName,"other/sleepAfter")) sprintf(buf,"%d",m_configData->sleep);
	if (!strcmp(paramName,"custom/item1Below")) sprintf(buf,"%d",m_configData->customItem1Below);
	if (!strcmp(paramName,"custom/item1Item")) sprintf(buf,"%d",m_configData->customItem1Item);
	if (!strcmp(paramName,"custom/item1Use")) sprintf(buf,"%d",m_configData->customItem1Use);
	if (!strcmp(paramName,"custom/item2Below")) sprintf(buf,"%d",m_configData->customItem2Below);
	if (!strcmp(paramName,"custom/item2Item")) sprintf(buf,"%d",m_configData->customItem2Item);
	if (!strcmp(paramName,"custom/item2Use")) sprintf(buf,"%d",m_configData->customItem2Use);
	if (!strcmp(paramName,"other/randomCast")) sprintf(buf,"%d",m_configData->randomCast);
	if (!strcmp(paramName,"other/useHotkey")) sprintf(buf,"%d",m_configData->useHotkey);

	return buf;
}

char *CMod_fluidApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "drink/hp";
	case 1: return "drink/mana";
	case 2: return "other/dropEmpty";
	case 3: return "drink/hpBelow";
	case 4: return "drink/manaBelow";
	case 5: return "other/sleepAfter";	
	case 6: return "drink/hpN";
	case 7: return "drink/hpS";
	case 8: return "drink/hpG";
	case 9: return "drink/manaN";
	case 10: return "drink/manaS";
	case 11: return "drink/manaG";
	case 12: return "drink/hpBelowN";
	case 13: return "drink/hpBelowS";
	case 14: return "drink/hpBelowG";
	case 15: return "drink/manaBelowN";
	case 16: return "drink/manaBelowS";
	case 17: return "drink/manaBelowG";
	case 18: return "custom/item1Below";
	case 19: return "custom/item1Item";
	case 20: return "custom/item1Use";
	case 21: return "custom/item2Below";
	case 22: return "custom/item2Item";
	case 23: return "custom/item2Use";
	case 24: return "drink/hpH";
	case 25: return "drink/hpU";
	case 26: return "drink/hpBelowH";
	case 27: return "drink/hpBelowU";
	case 28: return "other/randomCast";
	case 29: return "other/useHotkey";
	default:
		return NULL;
	}
}

void CMod_fluidApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	skin.SetButtonSkin(	m_configDialog->m_OK);
	skin.SetButtonSkin(	m_configDialog->m_enable);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (m_configDialog)
		m_configDialog->Invalidate();
}
