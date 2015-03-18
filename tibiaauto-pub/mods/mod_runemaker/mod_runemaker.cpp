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


static map<int*,int> setMana;
int RandomVariableMana(int &pt,int command,CConfigData *config){
	CMemReaderProxy reader;

	CTibiaCharacter* self=reader.readSelfCharacter();
	int val=pt<0?max(self->maxMana+pt,self->maxMana/10):pt;
	if (!config->randomCast){
		delete self;
		return val;
	}
	if (!setMana[&pt]) command=MAKE;
	if (command==MAKE){
		CPackSenderProxy sender;
		// within 10% of number with a cutoff at maxMana
		setMana[&pt] = CModuleUtil::randomFormula(val, (int)(val * 0.1), val, max(self->maxMana, val + 1));
		char buf[111];
		sprintf(buf,"%d",setMana[&pt]);
		sender.sendTAMessage(buf);
	}
	delete self;
	return setMana[&pt];
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

	while (!toolThreadShouldStop)
	{
		Sleep(900);
		if (!reader.isLoggedIn()) continue; // do not proceed if not connected
		CTibiaCharacter *myself = reader.readSelfCharacter();

		int blanksCount=0;
		int openContNr=0;
		int openContMax=reader.readOpenContainerCount();
		for (int i=0;i<memConstData.m_memMaxContainers && openContNr<openContMax;i++)
		{
			CTibiaContainer *container = reader.readContainer(i);
			if (container->flagOnOff)
			{
				openContNr++;
				if(config->useSpear){
					blanksCount+=container->countItemsOfType(itemProxy.getValueForConst("spear"));
				} else {
					blanksCount+=container->countItemsOfType(itemProxy.getValueForConst("runeBlank"));
				}
			};
			delete container;
		}
		if(myself->soulPoints>=config->soulPoints){
			for(;blanksCount>0;blanksCount--){
				int manaLimit = RandomVariableMana(config->makeNow?config->mana:(config->manaLimit>config->mana)?config->manaLimit:config->mana,GET,config);
				if (myself->mana>=manaLimit){
					RandomVariableMana(config->makeNow?config->mana:(config->manaLimit>config->mana)?config->manaLimit:config->mana,MAKE,config);
					// cast spell
					sender.say((LPCTSTR)config->spell);
					CModuleUtil::waitForManaDecrease(myself->mana);
					if(!config->maxUse){
						break; //even if we have the mana and blank runes/spears do not continue casting
					}
				}
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
		m_configDialog->DestroyWindow();
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
	
	return 1;
}

void CMod_runemakerApp::resetConfig()
{
	if(m_configData){
		delete m_configData;
		m_configData = NULL;
	}
	m_configData = new CConfigData();
}

void CMod_runemakerApp::loadConfigParam(char *paramName,char *paramValue)
{

	if (!strcmp(paramName,"spell")) strcpy(m_configData->spell,paramValue);
	if (!strcmp(paramName,"mana")) m_configData->mana=atoi(paramValue);
	if (!strcmp(paramName,"manaLimit")) m_configData->manaLimit=atoi(paramValue);
	if (!strcmp(paramName,"soulPoints")) m_configData->soulPoints=atoi(paramValue);
	if (!strcmp(paramName,"premium")) m_configData->premium=atoi(paramValue);
	if (!strcmp(paramName,"maxUse")) m_configData->maxUse=atoi(paramValue);
	if (!strcmp(paramName,"randomCast")) m_configData->randomCast=atoi(paramValue);
	if (!strcmp(paramName,"useSpear")) m_configData->useSpear=atoi(paramValue);

	if (!strcmp(paramName,"spells/spell"))
	{
		if (m_currentSpellNr==0)
		{
			int i;
			for (i=0;i<100;i++)
			{
				m_configData->listSpells[i].words[0]='0';
			}
		}
		sscanf(paramValue,"%d,%d,%[a-zA-Z0-9+\"\'\\/ ,-]",
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
	if (!strcmp(paramName,"premium")) sprintf(buf,"%d",m_configData->premium);
	if (!strcmp(paramName,"maxUse")) sprintf(buf,"%d",m_configData->maxUse);
	if (!strcmp(paramName,"randomCast")) sprintf(buf,"%d",m_configData->randomCast);
	if (!strcmp(paramName,"useSpear")) sprintf(buf,"%d",m_configData->useSpear);

	if (!strcmp(paramName,"spells/spell")&&m_configData->listSpells[m_currentSpellNr].words[0] != '0'&&m_currentSpellNr<100)
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
	case 4: return "premium";
	case 5: return "maxUse";
	case 6: return "spells/spell";
	case 7: return "randomCast";
	case 8: return "useSpear";
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

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog){
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
