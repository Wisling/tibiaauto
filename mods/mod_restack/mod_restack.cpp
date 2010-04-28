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
#include "TibiaTile.h"

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

int getNewPeriod(CConfigData *config)
{
	return ((rand()%(config->periodTo-config->periodFrom+1))+config->periodFrom);	
}

void pickupItemFromFloor(int itemId,int x,int y,int z,int contNr,int slotNr,int qty){
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaCharacter *self = reader.readSelfCharacter();
	sender.moveObjectFromFloorToContainer(itemId,x,y,z,contNr,slotNr,qty);
	while (!CModuleUtil::waitForCapsChange(self->cap) && qty>0) { 
		Sleep(CModuleUtil::randomFormula(100,50));// +1.5 sec wait for caps change
		qty=qty/2;
		sender.moveObjectFromFloorToContainer(itemId,x,y,z,contNr,slotNr,qty);
	}
	Sleep(CModuleUtil::randomFormula(100,100));
}

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	int periodRemaining=getNewPeriod(config);



	while (!toolThreadShouldStop)
	{					
		Sleep(200);
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected

		CTibiaCharacter *self = reader.readSelfCharacter();
		CTibiaItem *item;
		CUIntArray itemsAccepted;

		if (periodRemaining)
		{			
			periodRemaining--;
		} else {
			periodRemaining=getNewPeriod(config);
		}

		// check ammo to restack
		int ammoItemId=config->ammoType;
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
						if (itemAccepted->objectId)
						{
							if (itemAccepted->quantity<qtyToRestack)
								qtyToRestack=itemAccepted->quantity;
							sender.moveObjectBetweenContainers(ammoItemId,0x40+contNr,itemAccepted->pos,0xa,0,qtyToRestack);
							Sleep(CModuleUtil::randomFormula(500,200));
							delete itemAccepted;
							delete cont;
							break;
						}
						delete itemAccepted;
					}
					delete cont;
				}
			}
		}
		delete item;

		// check throwable to restack
		int throwableItemId=config->throwableType;		
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
						if (itemAccepted->objectId)
						{
							if (itemAccepted->quantity<qtyToRestack)
								qtyToRestack=itemAccepted->quantity;
							if (!config->restackToRight)
							{
								sender.moveObjectBetweenContainers(throwableItemId,0x40+contNr,itemAccepted->pos,0x6,0,qtyToRestack);
							} else {
								sender.moveObjectBetweenContainers(throwableItemId,0x40+contNr,itemAccepted->pos,0x5,0,qtyToRestack);
							}
							Sleep(CModuleUtil::randomFormula(400,150));
							delete itemAccepted;
							delete cont;
							break;
						}	
						delete itemAccepted;
					}
					delete cont;
				}
			}
		}
		delete item;
				
		// pickup throwable (spears, small stones)
		if ((config->pickupSpears||config->pickupToHand)&&!periodRemaining&&self->cap>config->capLimit)
		{
			int offsetX=-2;
			int offsetY=-2;
			if (config->pickupUL&&isItemOnTop(-1,-1,throwableItemId)) offsetX=-1,offsetY=-1;
			if (config->pickupUC&&isItemOnTop(0,-1,throwableItemId)) offsetX=0,offsetY=-1;
			if (config->pickupUR&&isItemOnTop(1,-1,throwableItemId)) offsetX=1,offsetY=-1;
			if (config->pickupCL&&isItemOnTop(-1,0,throwableItemId)) offsetX=-1,offsetY=0;
			if (config->pickupCC&&isItemOnTop(0,0,throwableItemId)) offsetX=0,offsetY=0;
			if (config->pickupCR&&isItemOnTop(1,0,throwableItemId)) offsetX=1,offsetY=0;
			if (config->pickupBL&&isItemOnTop(-1,1,throwableItemId)) offsetX=-1,offsetY=1;
			if (config->pickupBC&&isItemOnTop(0,1,throwableItemId)) offsetX=0,offsetY=1;
			if (config->pickupBR&&isItemOnTop(1,1,throwableItemId)) offsetX=1,offsetY=1;


			if ((offsetX!=-2||offsetY!=-2)&&config->pickupSpears)
			{
				int contNr;						
				for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
				{
					CTibiaContainer *cont = reader.readContainer(contNr);
					if (cont->flagOnOff&&cont->itemsInside<cont->size)
					{	
						pickupItemFromFloor(throwableItemId,self->x+offsetX,self->y+offsetY,self->z,0x40+contNr,cont->size-1,itemOnTopQty(offsetX,offsetY));
						// reset offsetXY to avoid pickup up same item to hand
						offsetX=offsetY=-2;

						delete cont;
						break;
					} // if
					delete cont;
				} // for
			} // if (offsetX||offsetY)

			if (config->pickupToHand)
			{				
				CTibiaItem *itemLeftHand=reader.readItem(memConstData.m_memAddressLeftHand);
				CTibiaItem *itemRightHand=reader.readItem(memConstData.m_memAddressRightHand);
				if ((itemLeftHand->objectId==throwableItemId||(itemLeftHand->objectId==0&&itemRightHand->objectId!=throwableItemId))&&(offsetX!=-2||offsetY!=-2))
				{
					CTibiaTile *itemHandTile = reader.getTibiaTile(itemLeftHand->objectId);
					if (itemLeftHand->objectId==0||itemHandTile->stackable&&itemLeftHand->quantity<100)
					{
						// move to left hand
						pickupItemFromFloor(throwableItemId,self->x+offsetX,self->y+offsetY,self->z,0x6,0,itemOnTopQty(offsetX,offsetY));
						offsetX=offsetY=-2;
					}
				}
				if ((itemRightHand->objectId==throwableItemId||(itemRightHand->objectId==0&&itemLeftHand->objectId!=throwableItemId))&&(offsetX!=-2||offsetY!=-2))
				{
					CTibiaTile *itemHandTile = reader.getTibiaTile(itemRightHand->objectId);
					if (itemRightHand->objectId==0||itemHandTile->stackable&&itemLeftHand->quantity<100)
					{
						// move to right hand
						pickupItemFromFloor(throwableItemId,self->x+offsetX,self->y+offsetY,self->z,0x5,0,itemOnTopQty(offsetX,offsetY));
						offsetX=offsetY=-2;
					}
				}	
				delete itemLeftHand;
				delete itemRightHand;

				
			}
		} // if ((config->pickupSpears||config->pickupToHand)&&!periodRemaining)

		// if throwable is covered by other items - try to take it out
		if (config->moveCovering&&!periodRemaining)
		{
			int offsetX=-2,offsetY=-2;

			if (config->pickupUL&&isItemCovered(-1,-1,throwableItemId)) offsetX=-1,offsetY=-1;
			if (config->pickupUC&&isItemCovered(0,-1,throwableItemId)) offsetX=0,offsetY=-1;
			if (config->pickupUR&&isItemCovered(1,-1,throwableItemId)) offsetX=1,offsetY=-1;
			if (config->pickupCL&&isItemCovered(-1,0,throwableItemId)) offsetX=-1,offsetY=0;
			if (config->pickupCC&&isItemCovered(0,0,throwableItemId)) offsetX=0,offsetY=0;
			if (config->pickupCR&&isItemCovered(1,0,throwableItemId)) offsetX=1,offsetY=0;
			if (config->pickupBL&&isItemCovered(-1,1,throwableItemId)) offsetX=-1,offsetY=1;
			if (config->pickupBC&&isItemCovered(0,1,throwableItemId)) offsetX=0,offsetY=1;
			if (config->pickupBR&&isItemCovered(1,1,throwableItemId)) offsetX=1,offsetY=1;			

			if (offsetX!=-2||offsetY!=-2)
			{
				int objectId=itemOnTopCode(offsetX,offsetY);
				int qty=itemOnTopQty(offsetX,offsetY);
				if (offsetX||offsetY)
				{
					sender.moveObjectFromFloorToFloor(objectId,self->x+offsetX,self->y+offsetY,self->z,self->x,self->y,self->z,qty);
				} else {
					// special handling of moving covered items under you
					int moveToX=-2,moveToY=-2;
					if (config->pickupUL) moveToX=-1,moveToY=-1;
					if (config->pickupUC) moveToX=0,moveToY=-1;
					if (config->pickupUR) moveToX=1,moveToY=-1;
					if (config->pickupCL) moveToX=-1,moveToY=0;
					if (config->pickupCR) moveToX=1,moveToY=0;
					if (config->pickupBL) moveToX=-1,moveToY=1;
					if (config->pickupBC) moveToX=0,moveToY=1;
					if (config->pickupBR) moveToX=1,moveToY=1;
					if (moveToX!=-2||moveToY!=-2)
					{
						sender.moveObjectFromFloorToFloor(objectId,self->x,self->y,self->z,self->x+moveToX,self->y+moveToY,self->z,qty);
					}
				}
				Sleep(CModuleUtil::randomFormula(400,200));
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
		if (m_started) disableControls();
		else enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
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
	return "2.6";
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
	if (m_configData->periodTo<m_configData->periodFrom)
	{
		if (showAlerts) AfxMessageBox("period from must be <= period to!");
		return 0;
	}
	if (m_configData->capLimit<0)
	{
		if (showAlerts) AfxMessageBox("capacity limit must be >= 0!");
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
	if (!strcmp(paramName,"pickup/place/CC")) m_configData->pickupCC=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/CR")) m_configData->pickupCR=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/BL")) m_configData->pickupBL=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/BC")) m_configData->pickupBC=atoi(paramValue);
	if (!strcmp(paramName,"pickup/place/BR")) m_configData->pickupBR=atoi(paramValue);
	if (!strcmp(paramName,"throwable/moveCovering")) m_configData->moveCovering=atoi(paramValue);
	if (!strcmp(paramName,"ammo/restackToRight")) m_configData->restackToRight=atoi(paramValue);
	if (!strcmp(paramName,"pickup/toHand")) m_configData->pickupToHand=atoi(paramValue);
	if (!strcmp(paramName,"pickup/periodFrom")) m_configData->periodFrom=atoi(paramValue);
	if (!strcmp(paramName,"pickup/periodTo")) m_configData->periodTo=atoi(paramValue);
	if (!strcmp(paramName,"pickup/capLimit")) m_configData->capLimit=atoi(paramValue);

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
	if (!strcmp(paramName,"pickup/place/CC")) sprintf(buf,"%d",m_configData->pickupCC);
	if (!strcmp(paramName,"pickup/place/CR")) sprintf(buf,"%d",m_configData->pickupCR);
	if (!strcmp(paramName,"pickup/place/BL")) sprintf(buf,"%d",m_configData->pickupBL);
	if (!strcmp(paramName,"pickup/place/BC")) sprintf(buf,"%d",m_configData->pickupBC);
	if (!strcmp(paramName,"pickup/place/BR")) sprintf(buf,"%d",m_configData->pickupBR);
	if (!strcmp(paramName,"throwable/moveCovering")) sprintf(buf,"%d",m_configData->moveCovering);
	if (!strcmp(paramName,"ammo/restackToRight")) sprintf(buf,"%d",m_configData->restackToRight);
	if (!strcmp(paramName,"pickup/toHand")) sprintf(buf,"%d",m_configData->pickupToHand);
	if (!strcmp(paramName,"pickup/periodFrom")) sprintf(buf,"%d",m_configData->periodFrom);
	if (!strcmp(paramName,"pickup/periodTo")) sprintf(buf,"%d",m_configData->periodTo);
	if (!strcmp(paramName,"pickup/capLimit")) sprintf(buf,"%d",m_configData->capLimit);
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
	case 16: return "ammo/restackToRight";
	case 17: return "pickup/toHand";
	case 18: return "pickup/place/CC";
	case 19: return "pickup/periodFrom";
	case 20: return "pickup/periodTo";
	case 21: return "pickup/capLimit";
	default:
		return NULL;
	}
}

void CMod_restackApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	skin.SetButtonSkin(	m_configDialog->m_enable);		
	skin.SetButtonSkin(	m_configDialog->m_OK);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (m_configDialog)
		m_configDialog->Invalidate();
}
