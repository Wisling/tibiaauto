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

int RandomTimeEaterWait(int digestTime){
	return CModuleUtil::randomFormula(digestTime,(int)(digestTime*.1),1000000);
}

int RandomTimeEaterAmount(){//returns how much food to eat
	int ans =1;
	while (rand()%100<60) ans++;
	return ans;
}

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
	int digestTime = -1;
	int digestAmount = 0;

	while (!toolThreadShouldStop)
	{			
		int pos;
		if (digestTime==-1) digestTime=0;
		else {
    		CModuleUtil::sleepWithStop(RandomTimeEaterWait(digestTime ? digestTime * 1000 : 12000), &toolThreadShouldStop);
        }
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		if (toolThreadShouldStop) continue;

		digestTime=0;
		
		for (int i=RandomTimeEaterAmount();i>0;i--){
			CTibiaItem *foodItem;
			CTibiaCharacter *self = reader.readSelfCharacter();
			int foodContainer;

			foodItem=NULL;
			foodContainer=-1;
						
			
			CUIntArray *foods=itemProxy.getItemsFoodArray();
			for (pos=0;pos<memConstData.m_memMaxContainers&&foodItem==NULL;pos++)		
			{
				foodItem = CModuleUtil::lookupItem(pos,foods);
				foodContainer = pos;
			}
			//taken care of. delete foods;

			
			if (foodItem!=NULL)
			{
				sender.useItemInContainer(foodItem->objectId,0x40+foodContainer,foodItem->pos);
				
				CTibiaCharacter* self2=reader.readSelfCharacter();
				if (CModuleUtil::waitForCapsChange(self2->cap))
					digestTime += itemProxy.getExtraInfo(itemProxy.getIndex(foodItem->objectId, 2), 2);
				delete self2;
				char buf[111];
				if (i!=1)
					Sleep(CModuleUtil::randomFormula(400,100));
				delete foodItem;
				foodItem = NULL;
			}
			delete self;
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


