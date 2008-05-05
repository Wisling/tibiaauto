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


// mod_maphack.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_maphack.h"

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
// CMod_maphackApp

BEGIN_MESSAGE_MAP(CMod_maphackApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_maphackApp)
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
	int count=0;

	sender.enableCName(1);
	while (!toolThreadShouldStop)
	{			
		count++;
		Sleep(200);	
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		
		if (config->revealNoFish&&count%5==0)
		{			
			int x,y;
			for (x=-7;x<=7;x++)
			{
				for (y=-5;y<=5;y++)
				{
					int tileId = reader.mapGetPointItemId(point(x,y,0),0);
					if (tileId>=itemProxy.getValueForConst("waterWithFishStart")+12&&tileId<=itemProxy.getValueForConst("waterWithFishEnd")+12)
					{
						reader.mapSetPointItemId(point(x,y,0),0,727);
					}
				}
			}
		}

		if (config->revealCName&&count%5==0)
		{
			reader.writeEnableRevealCName();
		}
		if (config->revealInvisible)
		{
			/*
			int creatureNr=0;
			for (creatureNr=0;creatureNr<memConstData.m_memMaxCreatures;creatureNr++)
			{
				CTibiaCharacter *cr=reader.readVisibleCreature(creatureNr);
				if (cr->monsterType==
				delete cr;
			}
			*/

		}
				
	}
	sender.enableCName(0);
	reader.writeDisableRevealCName();
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_maphackApp construction

CMod_maphackApp::CMod_maphackApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_maphackApp::~CMod_maphackApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_maphackApp::getName()
{
	return "Map hack";
}


int CMod_maphackApp::isStarted()
{
	return m_started;
}


void CMod_maphackApp::start()
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

void CMod_maphackApp::stop()
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

void CMod_maphackApp::showConfigDialog()
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


void CMod_maphackApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_maphackApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_maphackApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_maphackApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_maphackApp::getVersion()
{
	return "1.4";
}


int CMod_maphackApp::validateConfig(int showAlerts)
{
	
	return 1;
}

void CMod_maphackApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_maphackApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"reveal/noFish")) m_configData->revealNoFish=atoi(paramValue);
	if (!strcmp(paramName,"reveal/cName")) m_configData->revealCName=atoi(paramValue);	
	if (!strcmp(paramName,"reveal/invisible")) m_configData->revealInvisible=atoi(paramValue);
}

char *CMod_maphackApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"reveal/noFish")) sprintf(buf,"%d",m_configData->revealNoFish);
	if (!strcmp(paramName,"reveal/cName")) sprintf(buf,"%d",m_configData->revealCName);	
	if (!strcmp(paramName,"reveal/invisible")) sprintf(buf,"%d",m_configData->revealInvisible);

	return buf;
}

char *CMod_maphackApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "reveal/noFish";
	case 1: return "reveal/cName";
	case 2: return "reveal/invisible";
	default:
		return NULL;
	}
}