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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMod_fluidApp

BEGIN_MESSAGE_MAP(CMod_fluidApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_fluidApp)
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
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;

	while (!toolThreadShouldStop)
	{			
		Sleep(100);	
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		
		
		CTibiaCharacter *self = reader.readSelfCharacter();
		
		if ((self->hp<config->hpBelow&&config->drinkHp)||config->hotkeyHp)
		{
			int contNr;
			CUIntArray itemArray;
			
			itemArray.Add(itemProxy.getValueForConst("fluid"));
			
			for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
			{
				CTibiaContainer *cont = reader.readContainer(contNr);
				
				if (cont->flagOnOff)
				{
					CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemArray,11);
					if (item)
					{
						if (config->hotkeyHp)
						{
							sender.castRuneAgainstHuman(0x40+contNr,item->pos,itemProxy.getValueForConst("fluid"),self->x,self->y,self->z,107);
						}
						if (self->hp<config->hpBelow&&config->drinkHp)
						{
							sender.castRuneAgainstHuman(0x40+contNr,item->pos,itemProxy.getValueForConst("fluid"),self->x,self->y,self->z);
							Sleep(config->sleep);
						}
						
						delete item;
						break;
					};
				}
				
				delete cont;
			}
		}
			
		if ((self->mana<config->manaBelow&&config->drinkMana)||config->hotkeyMana)
		{
			int contNr;
			CUIntArray itemArray;
			
			itemArray.Add(itemProxy.getValueForConst("fluid"));
			
			for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
			{
				CTibiaContainer *cont = reader.readContainer(contNr);
				
				if (cont->flagOnOff)
				{
					CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemArray,10);
					if (item)
					{
						if (self->mana<config->manaBelow&&config->drinkMana)
						{
							sender.castRuneAgainstHuman(0x40+contNr,item->pos,itemProxy.getValueForConst("fluid"),self->x,self->y,self->z);
							Sleep(config->sleep);
						}
						if (config->hotkeyMana)
						{
							sender.castRuneAgainstHuman(0x40+contNr,item->pos,itemProxy.getValueForConst("fluid"),self->x,self->y,self->z,106);
						}
						
						delete item;
						break;
					};
				}
				
				delete cont;
			}
		}

		if (config->dropEmpty)
		{
			CUIntArray itemArray;
			
			itemArray.Add(itemProxy.getValueForConst("fluid"));
			int contNr;
			for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
			{
				CTibiaContainer *cont = reader.readContainer(contNr);
				
				if (cont->flagOnOff)
				{
					CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemArray,0);
					if (item)
					{						
						sender.moveObjectFromContainerToFloor(itemProxy.getValueForConst("fluid"),0x40+contNr,item->pos,self->x,self->y,self->z,1);
						Sleep(config->sleep);
						delete item;
						break;
					}											
				}
				
				delete cont;
			}
		}

		delete self;
				
	}
	toolThreadShouldStop=0;
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
	}
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
	return "1.1";
}


int CMod_fluidApp::validateConfig(int showAlerts)
{
	if (m_configData->sleep<0)
	{
		if (showAlerts) AfxMessageBox("Sleep after drink must be >=0!");
		return 0;
	}

	if (m_configData->hpBelow<0)
	{
		if (showAlerts) AfxMessageBox("'hp below' must be >=0!");
		return 0;
	}

	if (m_configData->manaBelow<0)
	{
		if (showAlerts) AfxMessageBox("'mana below' must be >=0!");
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
	if (!strcmp(paramName,"drink/hp")) m_configData->drinkHp=atoi(paramValue);
	if (!strcmp(paramName,"drink/mana")) m_configData->drinkMana=atoi(paramValue);
	if (!strcmp(paramName,"other/dropEmpty")) m_configData->dropEmpty=atoi(paramValue);
	if (!strcmp(paramName,"hotkey/hp")) m_configData->hotkeyHp=atoi(paramValue);
	if (!strcmp(paramName,"hotkey/mana")) m_configData->hotkeyMana=atoi(paramValue);
	if (!strcmp(paramName,"drink/hpBelow")) m_configData->hpBelow=atoi(paramValue);
	if (!strcmp(paramName,"drink/manaBelow")) m_configData->manaBelow=atoi(paramValue);
	if (!strcmp(paramName,"other/sleepAfter")) m_configData->sleep=atoi(paramValue);
}

char *CMod_fluidApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"drink/hp")) sprintf(buf,"%d",m_configData->drinkHp);
	if (!strcmp(paramName,"drink/mana")) sprintf(buf,"%d",m_configData->drinkMana);
	if (!strcmp(paramName,"other/dropEmpty")) sprintf(buf,"%d",m_configData->dropEmpty);
	if (!strcmp(paramName,"hotkey/hp")) sprintf(buf,"%d",m_configData->hotkeyHp);
	if (!strcmp(paramName,"hotkey/mana")) sprintf(buf,"%d",m_configData->hotkeyMana);
	if (!strcmp(paramName,"drink/hpBelow")) sprintf(buf,"%d",m_configData->hpBelow);
	if (!strcmp(paramName,"drink/manaBelow")) sprintf(buf,"%d",m_configData->manaBelow);
	if (!strcmp(paramName,"other/sleepAfter")) sprintf(buf,"%d",m_configData->sleep);

	return buf;
}

char *CMod_fluidApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "drink/hp";
	case 1: return "drink/mana";
	case 2: return "other/dropEmpty";
	case 3: return "hotkey/hp";
	case 4: return "hotkey/mana";
	case 5: return "drink/hpBelow";
	case 6: return "drink/manaBelow";
	case 7: return "other/sleepAfter";	
	default:
		return NULL;
	}
}