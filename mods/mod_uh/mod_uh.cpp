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


// mod_uh.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_uh.h"

#include "ConfigDialog.h"
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

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CMod_uhApp

BEGIN_MESSAGE_MAP(CMod_uhApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_uhApp)
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
	CConfigData *config = (CConfigData *)lpParam;
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CUIntArray  acceptedItems;
	int uhFallbackNeeded=0;
	int grpUhFallbackNeeded=0;
	int i;	

	while (!toolThreadShouldStop)
	{			
		Sleep(200);		
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
			
		CTibiaCharacter *self = reader.readSelfCharacter();

		if (self->hp<=config->m_uhBorderline||config->m_hotkeySelf)
		{	
			int uhContainer;
			
			acceptedItems.RemoveAll();
			if (!uhFallbackNeeded)
			{
				switch (config->m_runetype)
				{
				case 0: acceptedItems.Add(itemProxy.getValueForConst("runeUH"));break;
				case 1: acceptedItems.Add(itemProxy.getValueForConst("runeIH"));break;
				}				
			} else {
				acceptedItems.Add(itemProxy.getValueForConst("runeUH"));
				acceptedItems.Add(itemProxy.getValueForConst("runeIH"));
				uhFallbackNeeded=0;
			}
			
			CTibiaItem *uhItem=new CTibiaItem();
			uhContainer=-1;
			
			int contNr;
			for (contNr=0;contNr<memConstData.m_memMaxContainers&&!uhItem->objectId;contNr++)
			{
				CTibiaContainer *cont = reader.readContainer(contNr);

				if (cont->flagOnOff)
				{		
					delete uhItem;
					uhItem = CModuleUtil::lookupItem(contNr,&acceptedItems);
					uhContainer = contNr;
				};

				delete cont;
			}

										
			
			if (uhItem->objectId)
			{
				reader.setGlobalVariable("UH_needed","true");
				if (self->hp<=config->m_uhBorderline)
				{
					sender.useWithObjectFromContainerOnFloor(
						uhItem->objectId,0x40+uhContainer,uhItem->pos,0x63,
						self->x,self->y,self->z);
					Sleep(config->m_sleepAfter);
				}
				if (config->m_hotkeySelf)
				{
					sender.useWithObjectFromContainerOnFloor(
						uhItem->objectId,0x40+uhContainer,uhItem->pos,0x63,
						self->x,self->y,self->z,105);				
				}
			} else {
				if (config->m_fallback)
				{
					uhFallbackNeeded=1;
				}
			}			
			delete uhItem;
		}
		else 
			reader.setGlobalVariable("UH_needed","false");

		
		// grp heal

		int crNr;
		for (crNr=0;crNr<memConstData.m_memMaxCreatures;crNr++)
		{						
			CTibiaCharacter *ch = reader.readVisibleCreature(crNr);

			if (ch->visible)
			{
				char chName[128];
				memset(chName,0,128);
				memcpy(chName,ch->name,strlen(ch->name));												

				int chToHeal=0;
				

				for (i=0;i<config->m_grpMemberCount;i++)
				{
					if (!strcmpi(config->m_grpMemberList[i],chName))
					{
						
						chToHeal=1;
					}										
				}				
				if (chToHeal&&ch->hpPercLeft<config->m_grpBorderline&&self->z==ch->z)
				{
					CTibiaItem *uhItem=new CTibiaItem();
					int uhContainer=-1;

					acceptedItems.RemoveAll();
					if (!grpUhFallbackNeeded)
					{
						switch (config->m_grpRunetype)
						{
						case 0: acceptedItems.Add(itemProxy.getValueForConst("runeUH"));break;
						case 1: acceptedItems.Add(itemProxy.getValueForConst("runeIH"));break;
						}
					} else {
						acceptedItems.Add(itemProxy.getValueForConst("runeUH"));
						acceptedItems.Add(itemProxy.getValueForConst("runeIH"));
						grpUhFallbackNeeded=0;
					}
					
					int contNr;
					for (contNr=0;contNr<memConstData.m_memMaxContainers&&!uhItem->objectId;contNr++)
					{
						CTibiaContainer *cont = reader.readContainer(contNr);

						if (cont->flagOnOff)
						{
							delete uhItem;
							uhItem = CModuleUtil::lookupItem(contNr,&acceptedItems);
							uhContainer = contNr;
						};
						
						delete cont;
					}										
					
					if (uhItem->objectId)
					{						
						sender.useWithObjectFromContainerOnFloor(
							uhItem->objectId,0x40+uhContainer,uhItem->pos,0x63,
							ch->x,ch->y,ch->z);					
						Sleep(config->m_sleepAfter);						

					} else {
						if (config->m_grpFallback)
							grpUhFallbackNeeded=1;
					}
					delete uhItem;
				}
			}

			delete ch;
		}						

		delete self;		
				
	}
	reader.setGlobalVariable("UH_needed","false");
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_uhApp construction

CMod_uhApp::CMod_uhApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_uhApp::~CMod_uhApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_uhApp::getName()
{
	return "Auto UH";
}


int CMod_uhApp::isStarted()
{
	return m_started;
}


void CMod_uhApp::start()
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

void CMod_uhApp::stop()
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

void CMod_uhApp::showConfigDialog()
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


void CMod_uhApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_uhApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_uhApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_uhApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_uhApp::getVersion()
{
	return "1.2";
}


int CMod_uhApp::validateConfig(int showAlerts)
{
	if (m_configData)
	{
		if (m_configData->m_uhBorderline<0) 
		{			
			if (showAlerts) AfxMessageBox("UH borderline for player must be >= 0!");
			return 0;
		}
		if (m_configData->m_grpBorderline<0) 
		{
			if (showAlerts) AfxMessageBox("UH borderline for group must be >= 0!");
			return 0;
		}
		if (m_configData->m_grpBorderline>100) 
		{
			if (showAlerts) AfxMessageBox("UH borderline for group must be <= 100!");
			return 0;
		}
		if (m_configData->m_sleepAfter<0) 
		{
			if (showAlerts) AfxMessageBox("Sleep after case must be >=0!");
			return 0;
		}
	}
	return 1;	
}

void CMod_uhApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_uhApp::loadConfigParam(char *paramName,char *paramValue)
{	
	if (!strcmp(paramName,"self/fallback")) m_configData->m_fallback=atoi(paramValue);
	if (!strcmp(paramName,"self/hotkey")) m_configData->m_hotkeySelf=atoi(paramValue);
	if (!strcmp(paramName,"self/runetype")) m_configData->m_runetype=atoi(paramValue);
	if (!strcmp(paramName,"self/borderline")) m_configData->m_uhBorderline=atoi(paramValue);
	if (!strcmp(paramName,"grp/fallback")) m_configData->m_grpFallback=atoi(paramValue);
	if (!strcmp(paramName,"grp/borderline")) m_configData->m_grpBorderline=atoi(paramValue);
	if (!strcmp(paramName,"grp/runetype")) m_configData->m_grpRunetype=atoi(paramValue);	
	if (!strcmp(paramName,"other/sleepAfter")) m_configData->m_sleepAfter=atoi(paramValue);	
	if (!strcmp(paramName,"grp/member")) 
	{
		if (currentMemberPos>900)
			return;
		if (!currentMemberPos) m_configData->m_grpMemberCount=0;

		strcpy(m_configData->m_grpMemberList[currentMemberPos++],paramValue);
		
		m_configData->m_grpMemberCount=currentMemberPos;		
	}
}

char *CMod_uhApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"self/fallback")) sprintf(buf,"%d",m_configData->m_fallback);
	if (!strcmp(paramName,"self/hotkey")) sprintf(buf,"%d",m_configData->m_hotkeySelf);
	if (!strcmp(paramName,"self/runetype")) sprintf(buf,"%d",m_configData->m_runetype);
	if (!strcmp(paramName,"self/borderline")) sprintf(buf,"%d",m_configData->m_uhBorderline);
	if (!strcmp(paramName,"grp/fallback")) sprintf(buf,"%d",m_configData->m_grpFallback);
	if (!strcmp(paramName,"grp/borderline")) sprintf(buf,"%d",m_configData->m_grpBorderline);
	if (!strcmp(paramName,"grp/runetype")) sprintf(buf,"%d",m_configData->m_grpRunetype);	
	if (!strcmp(paramName,"other/sleepAfter")) sprintf(buf,"%d",m_configData->m_sleepAfter);
	if (!strcmp(paramName,"grp/member")) 
	{		
		
		if (currentMemberPos<m_configData->m_grpMemberCount)
		{					
			sprintf(buf,"%s",m_configData->m_grpMemberList[currentMemberPos++]);			
		}		
	}

	return buf;
}

char *CMod_uhApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "self/fallback";
	case 1: return "self/hotkey";
	case 2: return "self/runetype";
	case 3: return "other/sleepAfter";
	case 4: return "self/borderline";
	case 5: return "grp/borderline";
	case 6: return "grp/fallback";
	case 7: return "grp/runetype";
	case 8: return "grp/member";	
	default:
		return NULL;
	}
}

int CMod_uhApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName,"grp/member")) return 1;
	return 0;
}

void CMod_uhApp::resetMultiParamAccess(char *paramName)
{
	if (!strcmp(paramName,"grp/member")) currentMemberPos=0;
}

void CMod_uhApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	skin.SetButtonSkin(	m_configDialog->m_memberRemove);
	skin.SetButtonSkin(	m_configDialog->m_memberAdd);
	skin.SetButtonSkin(	m_configDialog->m_OK);
	skin.SetButtonSkin(	m_configDialog->m_enable);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (m_configDialog)
		m_configDialog->Invalidate();
}
