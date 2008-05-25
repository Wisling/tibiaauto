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


// mod_fisher.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_fisher.h"

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
// CMod_fisherApp

BEGIN_MESSAGE_MAP(CMod_fisherApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_fisherApp)
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
		Sleep(500);	
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
	
		CTibiaCharacter *self = reader.readSelfCharacter();

		if (config->moveFromHandToCont)
		{
			// left hand
			CTibiaItem *item = reader.readItem(memConstData.m_memAddressLeftHand);
			if (item->objectId==itemProxy.getValueForConst("fish"))
			{
				// fish in left hand - move to first open container
				int contNr;
				for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
				{
					CTibiaContainer *cont = reader.readContainer(contNr);

					if (cont->flagOnOff&&cont->itemsInside<cont->size)
					{
						sender.moveObjectBetweenContainers(
							item->objectId,0x06,0,0x40+contNr,0,item->quantity?item->quantity:1);
						contNr=memConstData.m_memMaxContainers;
					}
					delete cont;
				}
			}
			delete item;

			// right hand
			item = reader.readItem(memConstData.m_memAddressRightHand);
			if (item->objectId==itemProxy.getValueForConst("fish"))
			{
				// fish in right hand - move to first open container
				int contNr;
				for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
				{
					CTibiaContainer *cont = reader.readContainer(contNr);

					if (cont->flagOnOff&&cont->itemsInside<cont->size)
					{
						sender.moveObjectBetweenContainers(
							item->objectId,0x05,0,0x40+contNr,0,item->quantity?item->quantity:1);
						contNr=memConstData.m_memMaxContainers;
					}
					delete cont;
				}
			}
			delete item;
		}
		
		

		// refresh self to have correct cap
		delete self;
		self = reader.readSelfCharacter();
		// if cap check enabled, 
		if (self->cap<config->fishOnlyWhenCap)
		{			
			delete self;
			continue;			
		}

		//New only fish when worms available
		if (config->fishOnlyWhenWorms)
		{
			CUIntArray itemsAccepted;
			itemsAccepted.Add(itemProxy.getValueForConst("worms"));
			for (int contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
			{
				CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemsAccepted);
				if (!item)
				{
					break;
				}
				delete item;
			}
			if (!item)
			{
				delete item;
				delete self;
				continue;
			}
			
		}


		// now find "random" water field with a fish
		int offsetX,offsetY=0;
		int randLoopCount=20;
		while (randLoopCount--)
		{
			offsetX=rand()%15-7;
			offsetY=rand()%11-5;
			int tileId = reader.mapGetPointItemId(point(offsetX,offsetY,0),0);
			if (tileId>=itemProxy.getValueForConst("waterWithFishStart")&&tileId<=itemProxy.getValueForConst("waterWithFishEnd"))
			{
				break;
			}
		}
		if (randLoopCount>0)
		{							
			int fishingRodCont=0xa;
			int fishingRodPos=0;
			
			CUIntArray itemsAccepted;
			int contNr;
			itemsAccepted.Add(itemProxy.getValueForConst("fishingRod"));
			for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
			{
				CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemsAccepted);
				if (item)
				{
					fishingRodCont=0x40+contNr;
					fishingRodPos=item->pos;
					delete item;
					break;
				}
				delete item;
			}												

			int tileId = reader.mapGetPointItemId(point(offsetX,offsetY,0),0);			
			sender.useWithObjectFromContainerOnFloor(
				itemProxy.getValueForConst("fishingRod"),fishingRodCont,fishingRodPos,tileId,self->x+offsetX,self->y+offsetY,self->z);
		}

		

		delete self;

		
				
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_fisherApp construction

CMod_fisherApp::CMod_fisherApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_fisherApp::~CMod_fisherApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_fisherApp::getName()
{
	return "Auto fisher";
}


int CMod_fisherApp::isStarted()
{
	return m_started;
}


void CMod_fisherApp::start()
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

void CMod_fisherApp::stop()
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

void CMod_fisherApp::showConfigDialog()
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


void CMod_fisherApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_fisherApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_fisherApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_fisherApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_fisherApp::getVersion()
{
	return "2.3";
}


int CMod_fisherApp::validateConfig(int showAlerts)
{	
	if (m_configData->fishOnlyWhenCap<0)
	{
		if (showAlerts) AfxMessageBox("Minimum fishing capacity must be >=0!");
		return 0;
	}

	return 1;
}

void CMod_fisherApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_fisherApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"other/fishOnlyWhenCap")) m_configData->fishOnlyWhenCap=atoi(paramValue);
	if (!strcmp(paramName,"move/fromHandToCont")) m_configData->moveFromHandToCont=atoi(paramValue);		
	if (!strcmp(paramName,"other/fishOnlyWhenWorms")) m_configData->fishOnlyWhenWorms=atoi(paramValue);		
}

char *CMod_fisherApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"other/fishOnlyWhenCap")) sprintf(buf,"%d",m_configData->fishOnlyWhenCap);
	if (!strcmp(paramName,"move/fromHandToCont")) sprintf(buf,"%d",m_configData->moveFromHandToCont);	
	if (!strcmp(paramName,"other/f	ishOnlyWhenWorms")) sprintf(buf,"%d",m_configData->fishOnlyWhenWorms);
	

	return buf;
}

char *CMod_fisherApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "other/fishOnlyWhenCap"; // old: fishCap
	case 1: return "move/fromHandToCont";	
	case 2: return "other/fishOnlyWhenWorms";
	default:
		return NULL;
	}
}
