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


// mod_restack.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_restack.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "MemConstData.h"
#include "ModuleUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMod_restackApp

BEGIN_MESSAGE_MAP(CMod_restackApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_restackApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


#include "commons.h"



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

		CTibiaCharacter *self = reader.readSelfCharacter();
				
		CTibiaItem *item;
		CUIntArray itemsAccepted;		

		// check ammo to restack
		int ammoItemId=0;
		switch (config->ammoType)
		{
		case 0:ammoItemId=0;break; 
		case 1:ammoItemId=itemProxy.getValueForConst("arrow");break;
		case 2:ammoItemId=itemProxy.getValueForConst("bolt");break;
		default: ammoItemId=0;break;
		}
		item=reader.readItem(memConstData.m_memAddressSlotArrow);

		if (ammoItemId&&(item->objectId==0||item->objectId==ammoItemId))
		{
			if (item->quantity<=config->ammoAt)
			{
				int contNr;
				int qtyToRestack=config->ammoTo-item->quantity;
				itemsAccepted.RemoveAll();
				itemsAccepted.Add(ammoItemId);
				for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
				{
					CTibiaContainer *cont = reader.readContainer(contNr);
					if (cont->flagOnOff)
					{
						CTibiaItem *itemAccepted=CModuleUtil::lookupItem(contNr,&itemsAccepted);
						if (itemAccepted)
						{
							if (itemAccepted->quantity<qtyToRestack)
								qtyToRestack=itemAccepted->quantity;
							sender.moveObjectBetweenContainers(ammoItemId,0x40+contNr,itemAccepted->pos,0xa,0,qtyToRestack);
							delete itemAccepted;
							delete cont;
							break;
						}
					}
					delete cont;
				}
			}
		}
		delete item;

		// check throwable to restack
		int throwableItemId=0;
		switch (config->throwableType)
		{
		case 0:throwableItemId=0;break; 
		case 1:throwableItemId=itemProxy.getValueForConst("spear");break;		
		case 2:throwableItemId=itemProxy.getValueForConst("smallstone");break;		
		default: throwableItemId=0;break;
		}
		if (!config->restackToRight)
		{
			item=reader.readItem(memConstData.m_memAddressLeftHand);
		} else {
			item=reader.readItem(memConstData.m_memAddressRightHand);
		}

		if (throwableItemId&&(item->objectId==0||item->objectId==throwableItemId))
		{
			if (item->quantity<=config->throwableAt)
			{
				int contNr;
				int qtyToRestack=config->throwableTo-item->quantity;
				itemsAccepted.RemoveAll();
				itemsAccepted.Add(throwableItemId);
				for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
				{
					CTibiaContainer *cont = reader.readContainer(contNr);
					if (cont->flagOnOff)
					{
						CTibiaItem *itemAccepted=CModuleUtil::lookupItem(contNr,&itemsAccepted);					
						if (itemAccepted)
						{
							if (itemAccepted->quantity<qtyToRestack)
								qtyToRestack=itemAccepted->quantity;
							if (!config->restackToRight)
							{
								sender.moveObjectBetweenContainers(throwableItemId,0x40+contNr,itemAccepted->pos,0x6,0,qtyToRestack);
							} else {
								sender.moveObjectBetweenContainers(throwableItemId,0x40+contNr,itemAccepted->pos,0x5,0,qtyToRestack);
							}
							delete itemAccepted;
							delete cont;
							break;
						}						
					}
					delete cont;
				}
			}
		}
		delete item;

		// pickup spears
		if (config->pickupSpears)
		{
			int contNr;						
			for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
			{
				CTibiaContainer *cont = reader.readContainer(contNr);
				if (cont->flagOnOff&&cont->itemsInside<cont->size)
				{					
					int offsetX=0;
					int offsetY=0;
					if (config->pickupUL&&isItemOnTop(-1,-1,throwableItemId)) offsetX=-1,offsetY=-1;
					if (config->pickupUC&&isItemOnTop(0,-1,throwableItemId)) offsetX=0,offsetY=-1;
					if (config->pickupUR&&isItemOnTop(1,-1,throwableItemId)) offsetX=1,offsetY=-1;
					if (config->pickupCL&&isItemOnTop(-1,0,throwableItemId)) offsetX=-1,offsetY=0;
					if (config->pickupCR&&isItemOnTop(1,0,throwableItemId)) offsetX=1,offsetY=0;
					if (config->pickupBL&&isItemOnTop(-1,1,throwableItemId)) offsetX=-1,offsetY=1;
					if (config->pickupBC&&isItemOnTop(0,1,throwableItemId)) offsetX=0,offsetY=1;
					if (config->pickupBR&&isItemOnTop(1,1,throwableItemId)) offsetX=1,offsetY=1;

					if (offsetX||offsetY)
					{
						sender.moveObjectFromFloorToContainer(throwableItemId,self->x+offsetX,self->y+offsetY,self->z,0x40+contNr,0,1);
						Sleep(500);
					}					
					delete cont;
					break;
				}
				delete cont;
			}
		}

		// if throwable is covered by other items - try to take it out
		if (config->moveCovering)
		{
			int offsetX=0,offsetY=0;

			if (config->pickupUL&&isItemCovered(-1,-1,throwableItemId)) offsetX=-1,offsetY=-1;
			if (config->pickupUC&&isItemCovered(0,-1,throwableItemId)) offsetX=0,offsetY=-1;
			if (config->pickupUR&&isItemCovered(1,-1,throwableItemId)) offsetX=1,offsetY=-1;
			if (config->pickupCL&&isItemCovered(-1,0,throwableItemId)) offsetX=-1,offsetY=0;
			if (config->pickupCR&&isItemCovered(1,0,throwableItemId)) offsetX=1,offsetY=0;
			if (config->pickupBL&&isItemCovered(-1,1,throwableItemId)) offsetX=-1,offsetY=1;
			if (config->pickupBC&&isItemCovered(0,1,throwableItemId)) offsetX=0,offsetY=1;
			if (config->pickupBR&&isItemCovered(1,1,throwableItemId)) offsetX=1,offsetY=1;			

			if (offsetX||offsetY)
			{
				int objectId=itemOnTopCode(offsetX,offsetY);
				int qty=itemOnTopQty(offsetX,offsetY);
				sender.moveObjectFromFloorToFloor(objectId,self->x+offsetX,self->y+offsetY,self->z,self->x,self->y,self->z,qty);
				Sleep(500);
			}
		}
		delete self;
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_restackApp construction

CMod_restackApp::CMod_restackApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_restackApp::~CMod_restackApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_restackApp::getName()
{
	return "Ammo restacker";
}


int CMod_restackApp::isStarted()
{
	return m_started;
}


void CMod_restackApp::start()
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

void CMod_restackApp::stop()
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

void CMod_restackApp::showConfigDialog()
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


void CMod_restackApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_restackApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_restackApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_restackApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_restackApp::getVersion()
{
	return "2.2";
}


int CMod_restackApp::validateConfig(int showAlerts)
{
	if (m_configData->ammoAt<0)
	{
		if (showAlerts) AfxMessageBox("Ammo at must be >=0!");
		return 0;
	}
	if (m_configData->ammoTo<0)
	{
		if (showAlerts) AfxMessageBox("Ammo to must be >=0!");
		return 0;
	}
	if (m_configData->ammoTo<m_configData->ammoAt)
	{
		if (showAlerts) AfxMessageBox("Ammo to must be >= ammo at!");
		return 0;
	}
	if (m_configData->throwableAt<0)
	{
		if (showAlerts) AfxMessageBox("throwable at must be >=0!");
		return 0;
	}
	if (m_configData->throwableTo<0)
	{
		if (showAlerts) AfxMessageBox("throwable to must be >=0!");
		return 0;
	}
	if (m_configData->throwableTo<m_configData->throwableAt)
	{
		if (showAlerts) AfxMessageBox("throwable to must be >= throwable at!");
		return 0;
	}
	return 1;
}

void CMod_restackApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_restackApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"ammo/type")) m_configData->ammoType=atoi(paramValue);
	if (!strcmp(paramName,"ammo/at")) m_configData->ammoAt=atoi(paramValue);
	if (!strcmp(paramName,"ammo/to")) m_configData->ammoTo=atoi(paramValue);
	if (!strcmp(paramName,"throwable/type")) m_configData->throwableType=atoi(paramValue);
	if (!strcmp(paramName,"throwable/at")) m_configData->throwableAt=atoi(paramValue);
	if (!strcmp(paramName,"throwable/to")) m_configData->throwableTo=atoi(paramValue);
	if (!strcmp(paramName,"pickup/spears")) m_configData->pickupSpears=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/UL")) m_configData->pickupUL=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/UC")) m_configData->pickupUC=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/UR")) m_configData->pickupUR=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/CL")) m_configData->pickupCL=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/CR")) m_configData->pickupCR=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/BL")) m_configData->pickupBL=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/BC")) m_configData->pickupBC=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/BR")) m_configData->pickupBR=atoi(paramValue);
	if (!strcmp(paramName,"throwable/moveCovering")) m_configData->moveCovering=atoi(paramValue);
	if (!strcmp(paramName,"ammo/restackToRight")) m_configData->restackToRight=atoi(paramValue);
}

char *CMod_restackApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"ammo/type")) sprintf(buf,"%d",m_configData->ammoType);
	if (!strcmp(paramName,"ammo/at")) sprintf(buf,"%d",m_configData->ammoAt);
	if (!strcmp(paramName,"ammo/to")) sprintf(buf,"%d",m_configData->ammoTo);
	if (!strcmp(paramName,"throwable/type")) sprintf(buf,"%d",m_configData->throwableType);
	if (!strcmp(paramName,"throwable/at")) sprintf(buf,"%d",m_configData->throwableAt);
	if (!strcmp(paramName,"throwable/to")) sprintf(buf,"%d",m_configData->throwableTo);
	if (!strcmp(paramName,"pickup/spears")) sprintf(buf,"%d",m_configData->pickupSpears);
	if (!strcmp(paramName,"pickup/place/UL")) sprintf(buf,"%d",m_configData->pickupUL);
	if (!strcmp(paramName,"pickup/place/UC")) sprintf(buf,"%d",m_configData->pickupUC);
	if (!strcmp(paramName,"pickup/place/UR")) sprintf(buf,"%d",m_configData->pickupUR);
	if (!strcmp(paramName,"pickup/place/CL")) sprintf(buf,"%d",m_configData->pickupCL);
	if (!strcmp(paramName,"pickup/place/CR")) sprintf(buf,"%d",m_configData->pickupCR);
	if (!strcmp(paramName,"pickup/place/BL")) sprintf(buf,"%d",m_configData->pickupBL);
	if (!strcmp(paramName,"pickup/place/BC")) sprintf(buf,"%d",m_configData->pickupBC);
	if (!strcmp(paramName,"pickup/place/BR")) sprintf(buf,"%d",m_configData->pickupBR);
	if (!strcmp(paramName,"throwable/moveCovering")) sprintf(buf,"%d",m_configData->moveCovering);
	if (!strcmp(paramName,"ammo/restackToRight")) sprintf(buf,"%d",m_configData->restackToRight);
	return buf;
}

char *CMod_restackApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "ammo/type";
	case 1: return "ammo/at";
	case 2: return "ammo/to";
	case 3: return "throwable/type";
	case 4: return "throwable/at";
	case 5: return "throwable/to";
	case 6: return "pickup/spears";
	case 7: return "pickup/place/UL";
	case 8: return "pickup/place/UC";
	case 9: return "pickup/place/UR";
	case 10: return "pickup/place/CL";
	case 11: return "pickup/place/CR";
	case 12: return "pickup/place/BL";
	case 13: return "pickup/place/BC";
	case 14: return "pickup/place/BR";	
	case 15: return "throwable/moveCovering";
	case 16: return "restackToRight";
	default:
		return NULL;
	}
}