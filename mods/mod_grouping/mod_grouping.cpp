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


// mod_grouping.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_grouping.h"

#include "ConfigData.h"
#include "TibiaContainer.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include "MemConstData.h"
#include <time.h>

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
// CMod_groupingApp

BEGIN_MESSAGE_MAP(CMod_groupingApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_groupingApp)
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
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;

	int groupTime[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//max containers = 16
	int minOpenTime = 5;

	while (!toolThreadShouldStop)
	{			
		Sleep(CModuleUtil::randomFormula(500,200));
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		
		int contNr;
		int movedSomething=0;
		for (contNr=0;contNr<memConstData.m_memMaxContainers && !movedSomething;contNr++)
		{
			CTibiaContainer *cont = reader.readContainer(contNr);
			if (cont->flagOnOff && !groupTime[contNr]) groupTime[contNr]=time(NULL)+minOpenTime;
			else if (!cont->flagOnOff) groupTime[contNr] = 0;

			if (!groupTime[contNr] || time(NULL)<=groupTime[contNr]){
				delete cont;
				continue;
			}
			int itemNrMoved;
			for (itemNrMoved=cont->itemsInside-1;itemNrMoved>=0&&!movedSomething;itemNrMoved--)//Search backwards for a stackable item to move
			{
				CTibiaItem *itemMoved = (CTibiaItem *)cont->items[itemNrMoved];

				int nonGroupable=0;
				if (itemMoved->objectId==itemProxy.getValueForConst("fluid"))
					nonGroupable=1;
				CTibiaTile *tile = reader.getTibiaTile(itemMoved->objectId);
				if (!tile->stackable)
					nonGroupable=1;

				if (itemMoved->quantity&&itemMoved->quantity<100&&!nonGroupable)//If items should be stacked
				{
					int itemNr;
					for (itemNr=0;itemNr<itemNrMoved&&!movedSomething;itemNr++)//Look for match in rest of container
					{
						CTibiaItem *item = (CTibiaItem *)cont->items[itemNr];

						if (itemMoved->objectId==item->objectId&&
							item->quantity&&item->quantity<100)
						{
						/**
						* items match, are groupable, and not in a full group
						**/
							int qtyToMove=0;
							if (itemMoved->quantity+item->quantity<=100)
							{
								qtyToMove=itemMoved->quantity;
							} else {
								qtyToMove=100-item->quantity;
							}
							// do the moving
							sender.moveObjectBetweenContainers(
								item->objectId,0x40+contNr,itemNrMoved,
								0x40+contNr,itemNr,	qtyToMove);
							movedSomething=1;
							
						}
					}
				}
			}
			
			delete cont;
		}
		
		
	}	

	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_groupingApp construction

CMod_groupingApp::CMod_groupingApp()
{	
	m_started=0;	
}

CMod_groupingApp::~CMod_groupingApp()
{
	
	delete m_configData;	
}

char * CMod_groupingApp::getName()
{
	return "Auto grouping";
}


int CMod_groupingApp::isStarted()
{
	return m_started;
}

 
void CMod_groupingApp::start()
{	
	superStart();
	DWORD threadId;
		
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_groupingApp::stop()
{
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	};
	m_started=0;		
} 


char *CMod_groupingApp::getVersion()
{
	return "1.2";
}


