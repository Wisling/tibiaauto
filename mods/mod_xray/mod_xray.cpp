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


// mod_xray.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_xray.h"
#include <Tlhelp32.h>

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include "IPCBackPipeProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMod_xrayApp

BEGIN_MESSAGE_MAP(CMod_xrayApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_xrayApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

int lastX=0;
int lastY=0;
int lastZ=0;


DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;	
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;
	
	

	while (!toolThreadShouldStop)
	{			
		int offset=0;
		Sleep(100);	
				
			
		unsigned long bRead=0;
		
		if (reader.getConnectionState()!=8)
		{
			// flush IPC communication if not logged
			while (backPipe.readFromPipe(&mess,1008)) {};
			while (backPipe.readFromPipe(&mess,2001)) {};
		}
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
	
		CTibiaCharacter *self = reader.readSelfCharacter();
	
		if (config->autoreset&&(self->x!=lastX||self->y!=lastY||self->z!=lastZ))
		{
			reader.setXRayValues(7,2);
			lastX=self->x;
			lastY=self->y;
			lastZ=self->z;
		}

		
		// message can go via 'say command' or via hooks
		if (backPipe.readFromPipe(&mess,1008)||(config->extrahotkeys&&backPipe.readFromPipe(&mess,2001))){

			int msgLen;		
			char msgBuf[16384];		
			
			memset(msgBuf,0,16384);		
			memcpy(&msgLen,mess.payload,sizeof(int));		
			memcpy(msgBuf,mess.payload+4,msgLen);		
			
			if (!strncmp("%ta lu",msgBuf,6))
			{				
				offset=1;
			}
			if (!strncmp("%ta ld",msgBuf,6))
			{				
				offset=-1;
			}
			if (!strncmp("%ta lr",msgBuf,6))
			{
				reader.setXRayValues(7,2);
				continue;
			}

			if (offset)
			{
				
				// code segment hack,
				// groundlevel is for ground and upper levels,
				// original value 7
				int groundlevel = reader.getXRayValue1();
				// undergroundlevel for underground
				// original value = 2
				int undergroundlevel = reader.getXRayValue2();

				groundlevel += offset;
				undergroundlevel += offset;

				// character at ground or higher
				// ground = 7. 6->5->4 and so are higher levels
				// 8 -> 9 -> 10 and so are lower levels.

				if(self->z <= 7)
				{					
					// Can only go down as many times as you are away from the 7th-1 floor
					// prevents possible crash when looking at ground floor and you go down 1 floor
					groundlevel = max(groundlevel,7-((self->z==7?7:6)-self->z));
					// Can only go up as many times as you are away from the 0th floor
					// prevents possible crash when looking at top floor and you go up 1 floor
					groundlevel = min(groundlevel,7-((self->z==0?0:1)-self->z));
					reader.setXRayValues(groundlevel,2);
				}

				if(self->z >7)
				{
					// Can only go down as many times as you are away from the 15th floor, but not below 0
					undergroundlevel = max(undergroundlevel,max(0,2-(15-self->z)));
					// Can only go up to 7 only up to 4 is useful though
					undergroundlevel = min(undergroundlevel,7);
					reader.setXRayValues(7,undergroundlevel);					
				}

			}

		}
				
		
		
		delete self;
		
		
	
	}


	if (reader.getConnectionState()==8)
	{
		// change xray only when connected fully
		reader.setXRayValues(7,2);
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_xrayApp construction

CMod_xrayApp::CMod_xrayApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_xrayApp::~CMod_xrayApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_xrayApp::getName()
{
	return "XRay";
}


int CMod_xrayApp::isStarted()
{
	if (!m_started)
	{
		// if not started then regularry consume 1008 and 2001 messages from the queue
		CIPCBackPipeProxy backPipe;
		struct ipcMessage mess;	

		backPipe.readFromPipe(&mess,1008);
		backPipe.readFromPipe(&mess,2001);
	}
	return m_started;
}


void CMod_xrayApp::start()
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

void CMod_xrayApp::stop()
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

void CMod_xrayApp::showConfigDialog()
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


void CMod_xrayApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_xrayApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_xrayApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_xrayApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_xrayApp::getVersion()
{
	return "1.0";
}


int CMod_xrayApp::validateConfig(int showAlerts)
{		

	return 1;
}

void CMod_xrayApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_xrayApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"other/autoreset")) m_configData->autoreset=atoi(paramValue);
	if (!strcmp(paramName,"other/extrahotkeys")) m_configData->extrahotkeys=atoi(paramValue);		
}

char *CMod_xrayApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"other/autoreset")) sprintf(buf,"%d",m_configData->autoreset);
	if (!strcmp(paramName,"other/extrahotkeys")) sprintf(buf,"%d",m_configData->extrahotkeys);	
	

	return buf;
}

char *CMod_xrayApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "other/autoreset"; 
	case 1: return "other/extrahotkeys";	
	default:
		return NULL;
	}
}
