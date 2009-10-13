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

int RandomEaterWaitTime(int digestTime){
	/***************************
	rand()							:: Random number between 0 and RAND_MAX (?32767?)
	digestTime * .2					:: 20% of digestTime
	rand() % (int)(digestTime * .2)	:: Pseudo-random number between 0 and 20% of digestTime
	digestTime * .9					:: 90% of digestTime...
									   When added to a random number from 0 to 20% of digestTime, 
									   this creates a uniform random number from 90 to 110% (+/- 10%)
	***************************/
	return rand() % (int)(digestTime * .2) + (int)(digestTime * .9);//randomFormula creates a non-uniform distribution weighted towards an average
}

int RandomAmountEater(){//returns how much food to eat
	return rand() % 7 + 1;			//Eat only 1-7 pieces of food at a time
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{			
	srand(time(NULL));				//Seed the random number generation
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaItemProxy itemProxy;
	CConfigData *config = (CConfigData *)lpParam;
	int digestTime = -1;
									//Superfluous referance to "digestAmount" removed

	while (!toolThreadShouldStop)
	{			
		int pos;
		if (digestTime==-1) digestTime=0;
		else {
			CModuleUtil::sleepWithStop(RandomEaterWaitTime(digestTime ? digestTime * 1000 : 12000), &toolThreadShouldStop);
		}
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		if (toolThreadShouldStop) continue;
		
		digestTime=0;
		
		for (int i=RandomAmountEater();i>0;i--){
									//Superfluous referance to "self" removed
			CTibiaItem *foodItem;	//In keeping with the paradigm of declaring and deleting dynamic items in the same scope
			int foodContainer;

			foodItem=NULL;
			foodContainer=-1;
						
			CUIntArray *foods=itemProxy.getItemsFoodArray();
			for (pos=0;pos<memConstData.m_memMaxContainers;pos++)		
			{
				CTibiaItem *tempFoodItem;
				tempFoodItem = NULL;

									//We haven't found a food item yet! 
				if (foodItem==NULL) {
					foodItem = CModuleUtil::lookupItem(pos,foods);
					foodContainer = pos;
				}
									//Are there any other things to eat?
				else 
					tempFoodItem = CModuleUtil::lookupItem(pos,foods);
									//Free up space in BPs by eating items with smaller quantities until they are gone
									//Incomplete algorithim!!!! (only find smaller quantity items if they exist in another BP)
				if (tempFoodItem != NULL && tempFoodItem->quantity < foodItem->quantity) {
					foodItem = CModuleUtil::lookupItem(pos,foods);
					foodContainer = pos;
				}

				delete tempFoodItem;
				tempFoodItem = NULL;
			}
			
			if (foodItem!=NULL)
			{
									//Previously used "self" variable was unneeded, changed variable name for clarity
				CTibiaCharacter* self=reader.readSelfCharacter();
									//Eat only if the character has less than full health or mana and only if not in a protection zone
				int flags = reader.getSelfEventFlags();
				if (!(flags & 0x4000) && self->mana < self->maxMana || self->hp < self->maxHp)
					sender.useItemInContainer(foodItem->objectId,0x40+foodContainer,foodItem->pos);
				
				if (CModuleUtil::waitForCapsChange(self->cap))
					digestTime += itemProxy.getExtraInfo(itemProxy.getIndex(foodItem->objectId, 2), 2);
				delete self; 
				self = NULL;
									//Superfluous referance to buf removed.
				if (i!=1)
					Sleep(CModuleUtil::randomFormula(400,100));
			}
			delete foodItem;		//In keeping with the paradigm of declaring and deleting dynamic items in the same scope
			foodItem = NULL;
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
	return "2.0";
}


