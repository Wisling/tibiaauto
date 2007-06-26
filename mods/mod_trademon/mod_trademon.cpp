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


// mod_trademon.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_trademon.h"

#include "ConfigData.h"
#include "TibiaContainer.h"

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
// CMod_trademonApp

BEGIN_MESSAGE_MAP(CMod_trademonApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_trademonApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{			
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CConfigData *config = (CConfigData *)lpParam;

	int offeredItems=0;

	int channelTime=0;
	int yellTime=0;
	int sayTime=0;	

	while (!toolThreadShouldStop){			

		Sleep(300);	
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		
		if (config->tradeTalk){
			if (config->channel){
				if (channelTime+config->channelInterval < time(NULL)){
					channelTime = time(NULL);
					sender.sayOnChan(config->message,5);	//T4: 5 - Trade
				}
			}
			if (config->yell){
				if (yellTime+config->yellInterval < time(NULL)){
					yellTime = time(NULL);
					sender.sayYell(config->message);
				}
			}
			if (config->say){
				if (sayTime+config->sayInterval < time(NULL)){
					sayTime = time(NULL);
					sender.say(config->message);
				}
			}
		}
		if (config->tradeMon){
			if (offeredItems!=reader.getTradeCountPartner())
			{
				offeredItems=reader.getTradeCountPartner();
				// if offered items count=0 then continue (means - no more trade open)
				if (!offeredItems)
					continue;

				char *strbuf=(char *)malloc(65536);
				memset(strbuf,0,65536);
				int *itemsCount=(int *)malloc(sizeof(int)*65536);			
				int i;

				memset(itemsCount,0,65536*sizeof(int));
							
				
				for (i=0;i<offeredItems;i++)
				{
					CTibiaItem *item = reader.getTradeItemPartner(i);				
					itemsCount[item->objectId]+=item->quantity?item->quantity:1;				
					delete item;
				}
				sprintf(strbuf,"[trade] ");			
				for (i=0;i<65536;i++)
				{
					if (itemsCount[i])
					{
						char buf[500];
						sprintf(buf,"%s [%d]",itemProxy.getName(i),itemsCount[i]);
						if (i)
							strcat(strbuf,", ");
						strcat(strbuf,buf);
						strbuf[1000]=0;
					}				
				}			
				sender.sendTAMessage(strbuf);			
				free(strbuf);
				free(itemsCount);
			}
		}		
	}
	
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_trademonApp construction

CMod_trademonApp::CMod_trademonApp()
{	
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();		
}

CMod_trademonApp::~CMod_trademonApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}	
	delete m_configData;	
}

char * CMod_trademonApp::getName()
{
	return "Trade tool";
}


int CMod_trademonApp::isStarted()
{
	return m_started;
}

 
void CMod_trademonApp::start()
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

void CMod_trademonApp::stop()
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

void CMod_trademonApp::showConfigDialog()
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


void CMod_trademonApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_trademonApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_trademonApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_trademonApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}

char *CMod_trademonApp::getVersion()
{
	return "2.0";
}

int CMod_trademonApp::validateConfig(int showAlerts)
{
	if (m_configData)
	{
		if (m_configData->tradeTalk)
		{
			if (strlen(m_configData->message)==0){
				if (showAlerts) AfxMessageBox("You have to specify a message text");
				return 0;
			}
			if (!(m_configData->channel || m_configData->yell || m_configData->say)){
				if (showAlerts) AfxMessageBox("You have to select at least one way of showing your message");
				return 0;
			}
			if (m_configData->channel && m_configData->channelInterval<1) 
			{			
				if (showAlerts) AfxMessageBox("Channel interval must be >= 1!");
				return 0;
			}
			if (m_configData->yell && m_configData->yellInterval<1) 
			{
				if (showAlerts) AfxMessageBox("UH borderline for group must be >= 0!");
				return 0;
			}
			if (m_configData->say && m_configData->sayInterval<1) 
			{
				if (showAlerts) AfxMessageBox("UH borderline for group must be <= 100!");
				return 0;
			}
		}
	}
	return 1;	
}

void CMod_trademonApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_trademonApp::loadConfigParam(char *paramName,char *paramValue)
{	
	if (!strcmp(paramName,"tradeTalker")) m_configData->tradeTalk=atoi(paramValue);
	if (!strcmp(paramName,"message")) lstrcpyn(m_configData->message,paramValue,255);
	if (!strcmp(paramName,"channel")) m_configData->channel=atoi(paramValue);
	if (!strcmp(paramName,"channelInterval")) m_configData->channelInterval=atoi(paramValue);
	if (!strcmp(paramName,"yell")) m_configData->yell=atoi(paramValue);
	if (!strcmp(paramName,"yellInterval")) m_configData->yellInterval=atoi(paramValue);
	if (!strcmp(paramName,"say")) m_configData->say=atoi(paramValue);
	if (!strcmp(paramName,"sayInterval")) m_configData->sayInterval=atoi(paramValue);
	if (!strcmp(paramName,"tradeMon")) m_configData->tradeMon=atoi(paramValue);
}

char *CMod_trademonApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"tradeTalker")) sprintf(buf,"%d",m_configData->tradeTalk);
	if (!strcmp(paramName,"message")) sprintf(buf,"%s",m_configData->message);
	if (!strcmp(paramName,"channel")) sprintf(buf,"%d",m_configData->channel);
	if (!strcmp(paramName,"channelInterval")) sprintf(buf,"%d",m_configData->channelInterval);
	if (!strcmp(paramName,"yell")) sprintf(buf,"%d",m_configData->yell);
	if (!strcmp(paramName,"yellInterval")) sprintf(buf,"%d",m_configData->yellInterval);
	if (!strcmp(paramName,"say")) sprintf(buf,"%d",m_configData->say);
	if (!strcmp(paramName,"sayInterval")) sprintf(buf,"%d",m_configData->sayInterval);
	if (!strcmp(paramName,"tradeMon")) sprintf(buf,"%d",m_configData->tradeMon);

	return buf;
}

char *CMod_trademonApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "tradeTalker";
	case 1: return "message";
	case 2: return "channel";
	case 3: return "channelInterval";
	case 4: return "yell";
	case 5: return "yellInterval";
	case 6: return "say";
	case 7: return "sayInterval";
	case 8: return "tradeMon";	
	default:
		return NULL;
	}
}
