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


// mod_spellcaster.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_spellcaster.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMod_spellcasterApp

BEGIN_MESSAGE_MAP(CMod_spellcasterApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_spellcasterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;

	while (!toolThreadShouldStop)
	{			
		Sleep(200);	
		
		CTibiaCharacter *self = reader.readSelfCharacter();
		
		//T4: First try to heal
		if (config->life && self->hp<=config->lifeHp){
			if (self->mana >= config->lifeSpellMana){
				sender.say(config->lifeSpell);
			}else{
				sender.sendTAMessage("WARNING!!!: Not enought mana to heal");
				Sleep(200);
			}
			Sleep(600);

		//T4: Use mana in other pupouse otherwise
		}else if(config->mana && self->mana>=config->manaMana){
			sender.say(config->manaSpell);
			Sleep(600);

		}else{
			// now try to summon creatures
			int chNr;
			int summonCount=0;
			for (chNr=0;chNr<memConstData.m_memMaxCreatures;chNr++)
			{
				CTibiaCharacter *ch = reader.readVisibleCreature(chNr);

				if (ch->z==self->z&&ch->visible&&!strcmpi(config->summonName,ch->name))
					summonCount++;

				delete ch;
			}
			if (config->summon && summonCount<config->summonLessThan && self->mana>=config->summonMana)
			{				
				// we should summon something
				char buf[256];
				sprintf(buf,"utevo res %s",config->summonName);
				sender.say(buf);
				Sleep(600);
			}
		}
		

		delete self;
				
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_spellcasterApp construction

CMod_spellcasterApp::CMod_spellcasterApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_spellcasterApp::~CMod_spellcasterApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_spellcasterApp::getName()
{
	return "Spell caster";
}


int CMod_spellcasterApp::isStarted()
{
	return m_started;
}


void CMod_spellcasterApp::start()
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

void CMod_spellcasterApp::stop()
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

void CMod_spellcasterApp::showConfigDialog()
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


void CMod_spellcasterApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_spellcasterApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_spellcasterApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_spellcasterApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_spellcasterApp::getVersion()
{
	return "1.2";
}


int CMod_spellcasterApp::validateConfig(int showAlerts)
{
	if (m_configData->mana){
		if (!strlen(m_configData->manaSpell))
		{
			if (showAlerts) AfxMessageBox("Some mana spell to cast must be defined!");
			return 0;
		}
		if (m_configData->manaMana<0)
		{
			if (showAlerts) AfxMessageBox("'mana above' must be >=0!");
			return 0;
		}
	}

	if (m_configData->life){
		if (!strlen(m_configData->lifeSpell))
		{
			if (showAlerts) AfxMessageBox("Some life spell to cast must be defined!");
			return 0;
		}

		if (m_configData->lifeHp<0)
		{
			if (showAlerts) AfxMessageBox("'Cast when life below' must be >=0!");
			return 0;
		}
		if (m_configData->lifeSpellMana<1)
		{
			if (showAlerts) AfxMessageBox("'Spell mana' must be >=1!");
			return 0;
		}
	}


	if (m_configData->summon){
		if (m_configData->summonLessThan<0||m_configData->summonLessThan>2)
		{
			if (showAlerts) AfxMessageBox("'summon less than' must be between 0 and 2!");
			return 0;
		}
		if (!strlen(m_configData->summonName))
		{
			if (showAlerts) AfxMessageBox("Summon's name must be not empty!");
			return 0;
		}
		if (m_configData->summonMana<0)
		{
			if (showAlerts) AfxMessageBox("'summon mana' must be >=0!");
			return 0;
		}
	}
	
	return 1;
}

void CMod_spellcasterApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_spellcasterApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"mana")) m_configData->mana=atoi(paramValue);
	if (!strcmp(paramName,"manaMana")) m_configData->manaMana=atoi(paramValue);
	if (!strcmp(paramName,"manaSpell")) lstrcpyn(m_configData->manaSpell,paramValue,128);

	if (!strcmp(paramName,"life")) m_configData->life=atoi(paramValue);
	if (!strcmp(paramName,"lifeHp")) m_configData->lifeHp=atoi(paramValue);
	if (!strcmp(paramName,"lifeSpell")) lstrcpyn(m_configData->lifeSpell,paramValue,128);
	if (!strcmp(paramName,"lifeSpellMana")) m_configData->lifeSpellMana=atoi(paramValue);

	if (!strcmp(paramName,"summon")) m_configData->summon=atoi(paramValue);
	if (!strcmp(paramName,"summonLessThan")) m_configData->summonLessThan=atoi(paramValue);
	if (!strcmp(paramName,"summonMana")) m_configData->summonMana=atoi(paramValue);
	if (!strcmp(paramName,"summonName")) lstrcpyn(m_configData->summonName,paramValue,128);
}

char *CMod_spellcasterApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"mana")) sprintf(buf,"%d",m_configData->mana);
	if (!strcmp(paramName,"manaMana")) sprintf(buf,"%d",m_configData->manaMana);
	if (!strcmp(paramName,"manaSpell")) sprintf(buf,"%s",m_configData->manaSpell);

	if (!strcmp(paramName,"life")) sprintf(buf,"%d",m_configData->life);
	if (!strcmp(paramName,"lifeHp")) sprintf(buf,"%d",m_configData->lifeHp);
	if (!strcmp(paramName,"lifeSpell")) sprintf(buf,"%s",m_configData->lifeSpell);
	if (!strcmp(paramName,"lifeSpellMana")) sprintf(buf,"%d",m_configData->lifeSpellMana);

	if (!strcmp(paramName,"summon")) sprintf(buf,"%d",m_configData->summon);
	if (!strcmp(paramName,"summonLessThan")) sprintf(buf,"%d",m_configData->summonLessThan);
	if (!strcmp(paramName,"summonMana")) sprintf(buf,"%d",m_configData->summonMana);
	if (!strcmp(paramName,"summonName")) strcpy(buf,m_configData->summonName);

	return buf;
}

char *CMod_spellcasterApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "mana";
	case 1: return "manaMana";
	case 2: return "manaSpell";
	case 3: return "life";
	case 4: return "lifeHp";
	case 5: return "lifeSpell";
	case 6: return "lifeSpellMana";
	case 7: return "summon";
	case 8: return "summonLessThan";
	case 9: return "summonName";
	case 10: return "summonMana";
	default:
		return NULL;
	}
}