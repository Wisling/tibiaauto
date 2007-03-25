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


// mod_eater.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_eater.h"

#include "ConfigData.h"
#include "TibiaContainer.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"
#include "MemConstData.h"
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
// CMod_eaterApp

BEGIN_MESSAGE_MAP(CMod_eaterApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_eaterApp)
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
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaItemProxy itemProxy;
	CConfigData *config = (CConfigData *)lpParam;
	
	while (!toolThreadShouldStop)
	{			
		int pos;

		
		CModuleUtil::sleepWithStop(10000,&toolThreadShouldStop);
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		if (toolThreadShouldStop) continue;
				

		CTibiaItem *foodItem;	
		int foodContainer;

		foodItem=NULL;
		foodContainer=-1;
					
		
			for (pos=0;pos<10&&foodItem==NULL;pos++)		
			{
				foodItem = CModuleUtil::lookupItem(pos,itemProxy.getItemsFoodArray());
				foodContainer = pos;

			}

		
		if (foodItem!=NULL)
		{
			sender.useItemInContainer(foodItem->objectId,0x40+foodContainer,foodItem->pos);
			delete foodItem;
			foodItem=NULL;		
		}
		
		
	}	

	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_eaterApp construction

CMod_eaterApp::CMod_eaterApp()
{	
	m_started=0;	
}

CMod_eaterApp::~CMod_eaterApp()
{
	
	delete m_configData;	
}

char * CMod_eaterApp::getName()
{
	return "Food eater";
}


int CMod_eaterApp::isStarted()
{
	return m_started;
}

 
void CMod_eaterApp::start()
{	
	superStart();
	DWORD threadId;
		
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_eaterApp::stop()
{
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	};
	m_started=0;		
} 


char *CMod_eaterApp::getVersion()
{
	return "1.0";
}


