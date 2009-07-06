/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/

// mod_cavebot.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_cavebot.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"
//#include "TileReader.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"
#include "TibiaItemProxy.h"
#include "TibiaMapProxy.h"
#include "commons.h"
#include "SendStats.h"
#include "TibiaTile.h"
#include <time.h>
#include "SharedMemory.h"
#include "IPCBackPipeProxy.h"
#include <Tlhelp32.h>

#include <iostream>
#include <fstream>
#include <time.h>
#include <sstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMod_cavebotApp

BEGIN_MESSAGE_MAP(CMod_cavebotApp, CWinApp)
//{{AFX_MSG_MAP(CMod_cavebotApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
// DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

// NOTE: those delete trampolines are used to ensure 'secure' delete operation 
//       and force revealing any memory issues faster

void deleteAndNull(CTibiaContainer *&ptr)
{
	delete ptr;
	ptr=NULL;
}

void deleteAndNull(CTibiaCharacter *&ptr)
{
	delete ptr;
	ptr=NULL;
}


void deleteAndNull(CConfigDialog *&ptr)
{
	delete ptr;
	ptr=NULL;
}


void deleteAndNull(CConfigData *&ptr)
{
	delete ptr;
	ptr=NULL;
}

void deleteAndNull(CTibiaItem *&ptr)
{
	delete ptr;
	ptr=NULL;
}


CToolAutoAttackStateAttack globalAutoAttackStateAttack=CToolAutoAttackStateAttack_notRunning;
CToolAutoAttackStateLoot globalAutoAttackStateLoot=CToolAutoAttackStateLoot_notRunning;
CToolAutoAttackStateWalker globalAutoAttackStateWalker=CToolAutoAttackStateWalker_notRunning;
CToolAutoAttackStateDepot globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notRunning;
CToolAutoAttackStateTraining globalAutoAttackStateTraining=CToolAutoAttackStateTraining_notRunning;

int targetX=0,targetY=0,targetZ=0;
int depotX=0,depotY=0,depotZ=0;
int currentPosTM=0;
int creatureAttackDist=0;
int attackSuspendedUntil=0;
int firstCreatureAttackTM=0;
int currentWaypointNr=0;

int lastTAMessageTm=0;
int taMessageDelay=4;

CTibiaMapProxy tibiaMap;

/**
* Register cavebot debug.
*/
void registerDebug(const char* buf1,const char* buf2="",const char* buf3="",const char* buf4="",const char* buf5="",const char* buf6=""){
	CModuleUtil::masterDebug("tibiaauto-debug-cavebot.txt",buf1,buf2,buf3,buf4,buf5,buf6);
}

std::string intstr(DWORD value){
	std::stringstream ss;
	ss << value;
	return ss.str();
}

/**
* Dump full info about a creature (for debuging purposes)
*/
void dumpCreatureInfo(char *info,int tibiaId) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	
	int i;
	int nr=-1;
	for (i=0;i<memConstData.m_memMaxCreatures;i++) {
		CTibiaCharacter *ch = reader.readVisibleCreature(i);
		
		if (ch->tibiaId==tibiaId) {
			nr=i;
			deleteAndNull(ch);
			break;
		}
		
		deleteAndNull(ch);
	}
	
	int offset = memConstData.m_memAddressFirstCreature+nr*memConstData.m_memLengthCreature;
	
	registerDebug(info);
	
	if (nr!=-1) {
		for (i=0;i<memConstData.m_memLengthCreature;i+=4) {
			char buf[128];
			int val=reader.getMemIntValue(offset+i);
			sprintf(buf,"offset %d -> %d",i,val);
			registerDebug(buf);
		}
	}
	else
		registerDebug("Unable to find creature");
	registerDebug("end of creature dump");
}

/*
* Checks whether we should go back to depot or not.
*/
int depotCheckShouldGo(CConfigData *config) {
	int ret=0;
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	if (self->cap<config->depotCap&&strlen(config->depotTrigger[0].itemName)) {
		// capacity limit check only when some depot items defined
		ret++;
	}
	
	int i;
	for (i=0;i<100&&strlen(config->depotTrigger[i].itemName);i++) {
		int objectId = itemProxy.getObjectId(config->depotTrigger[i].itemName);
		int contNr;
		int totalQty=0;
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++) {
			CTibiaContainer *cont = reader.readContainer(contNr);
			
			if (cont->flagOnOff)
				totalQty+=cont->countItemsOfType(objectId);
			
			deleteAndNull(cont);
		}
		// check whether we should deposit something
		if (config->depotTrigger[i].when>config->depotTrigger[i].remain&&
			totalQty>=config->depotTrigger[i].when) ret++;
		// check whether we should restack something
		if (config->depotTrigger[i].when<config->depotTrigger[i].remain&&
			totalQty<=config->depotTrigger[i].when) ret++;
	}
	deleteAndNull(self);
	return ret;
}

/**
* Do checks concerning 'go to depot' functionality.
*/
void depotCheck(CConfigData *config) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	if (config->debug) registerDebug("Depot check");
	if (globalAutoAttackStateDepot==CToolAutoAttackStateDepot_notRunning) {
		CTibiaCharacter *self=reader.readSelfCharacter();
		if (depotCheckShouldGo(config)) {
			if (config->debug) registerDebug("We should go to a depot");
			/**
			* Ok then - we should do the depositing.
			*/
			
			int path[15];
			struct point nearestDepot = CModuleUtil::findPathOnMap(self->x,self->y,self->z,0,0,0,301,path);
			depotX=nearestDepot.x;
			depotY=nearestDepot.y;
			depotZ=nearestDepot.z;
			if (depotX&&depotY&&depotZ) {
				// depot found - go to it
				globalAutoAttackStateDepot=CToolAutoAttackStateDepot_walking;
				// reset goto target
				targetX=depotX;
				targetY=depotY;
				targetZ=depotZ;
				if (config->debug) registerDebug("Depot found - going to it (x,y,z)=",intstr(targetX).c_str(),intstr(targetY).c_str(),intstr(targetZ).c_str());
			}
			else {
				// depot not found - argh
				globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notFound;
				depotX=depotY=depotZ=0;
				if (config->debug) registerDebug("Depot not found - depot state: not found");
			}
			
			deleteAndNull(self);
			return;
		}
		deleteAndNull(self);
	}
	if (globalAutoAttackStateDepot==CToolAutoAttackStateDepot_walking) {
		CTibiaMapProxy tibiaMap;
		if (config->debug) registerDebug("Walking to a depot section");
		CTibiaCharacter *self = reader.readSelfCharacter();
		// check whether the depot is reachable
		int path[15];
		if (config->debug) registerDebug("findPathOnMap: depot walker");
		CModuleUtil::findPathOnMap(self->x,self->y,self->z,depotX,depotY,depotZ,0,path);
		if (!path[0]||!tibiaMap.isPointAvailable(depotX,depotY,depotZ)) {
			if (config->debug) registerDebug("Path to the current depot not found: must find a new one");
			// path to the current depot not found, must find a new depot
			struct point nearestDepot = CModuleUtil::findPathOnMap(self->x,self->y,self->z,0,0,0,301,path);
			depotX=nearestDepot.x;
			depotY=nearestDepot.y;
			depotZ=nearestDepot.z;
			if (depotX&&depotY&&depotZ) {
				if (config->debug) registerDebug("New depot found");
				// depot found - go to it
				globalAutoAttackStateDepot=CToolAutoAttackStateDepot_walking;
				// reset goto target
				targetX=depotX;
				targetY=depotY;
				targetZ=depotZ;
			}
			else {
				if (config->debug) registerDebug("No new depot found");
				// depot not found - argh
				globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notFound;
				depotX=depotY=depotZ=0;
			}
		}
		else {
			if (config->debug) registerDebug("Currently selected depot is reachable");
		}
		
		deleteAndNull(self);
	}
	if (globalAutoAttackStateDepot==CToolAutoAttackStateDepot_notFound) {
		// not depot found - but try finding another one
		CTibiaCharacter *self = reader.readSelfCharacter();
		int path[15];
		struct point nearestDepot = CModuleUtil::findPathOnMap(self->x,self->y,self->z,0,0,0,301,path);
		depotX=nearestDepot.x;
		depotY=nearestDepot.y;
		depotZ=nearestDepot.z;
		if (depotX&&depotY&&depotZ) {
			// depot found - go to it
			globalAutoAttackStateDepot=CToolAutoAttackStateDepot_walking;
			// reset goto target
			targetX=depotX;
			targetY=depotY;
			targetZ=depotZ;
			if (config->debug) registerDebug("Depot found - going to it (x,y,z)=",intstr(targetX).c_str(),intstr(targetY).c_str(),intstr(targetZ).c_str());
		}
		else {
			// depot not found - argh
			globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notFound;
			depotX=depotY=depotZ=0;
			if (config->debug) registerDebug("Depot not found - depot state: not found (again)");
		}
		
		deleteAndNull(self);
	}
}

int depotDepositOpenChest(int x,int y,int z) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaCharacter *self = reader.readSelfCharacter();
	int depotContNr=-1;
	int count=reader.mapGetPointItemsCount(point(x-self->x,y-self->y,0));
	int pos=0;
	if (count>10) count=10;
	
	int lastNonDepotObject=0;
	for (pos=0;pos<count;pos++) {
		int tileId=reader.mapGetPointItemId(point(x-self->x,y-self->y,0),pos);
		if (!reader.getTibiaTile(tileId)->ground&&
			!reader.getTibiaTile(tileId)->blocking&&
			!reader.getTibiaTile(tileId)->isDepot&&
			!lastNonDepotObject) {
			// not depot, not ground, not blocking => some junk on the depot
			lastNonDepotObject=tileId;
		}
		if (reader.getTibiaTile(tileId)->isDepot) {
			if (lastNonDepotObject) {
				// remove junk laying over the depot chest
				sender.moveObjectFromFloorToFloor(lastNonDepotObject,x,y,z,self->x,self->y,self->z,100);
				Sleep(CModuleUtil::randomFormula(400,200));
			}

			// this is the depot chest so open it
			depotContNr= CModuleUtil::findNextClosedContainer();
			sender.openContainerFromFloor(tileId,x,y,z,depotContNr);
			CModuleUtil::waitForOpenContainer(depotContNr,true);
			CTibiaContainer *cont = reader.readContainer(depotContNr);
			int isOpen=cont->flagOnOff;
			deleteAndNull(cont);
			deleteAndNull(self);
			return 1;
		}
	}
	deleteAndNull(self);
	return -1;
}

/**
* Move the item to some container in the chest.
*/
void depotDepositMoveToChest(int objectId, int sourceContNr, int sourcePos, int qty, int depotContNr,int depotContNr2) {
	if (globalAutoAttackStateDepot==CToolAutoAttackStateDepot_noSpace)
		return;
	
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaContainer *depotChest = reader.readContainer(depotContNr);

	if (!depotChest->flagOnOff) {
		// shouldn't happen...
		deleteAndNull(depotChest);
		return;
	}
	
	/**
	* If container number 8 is open than we can try to deposit to it (to avoid 
	* massive close/open of containers.
	*/
	CTibiaContainer *cont8 = reader.readContainer(depotContNr2);
	if (cont8->flagOnOff&&cont8->itemsInside<cont8->size) {
		// found container in which we can do deposit!
		sender.moveObjectBetweenContainers(objectId,0x40+sourceContNr,sourcePos,0x40+depotContNr2,cont8->size,qty);
		CModuleUtil::waitForItemsInsideChange(depotContNr2,cont8->itemsInside);
		Sleep(CModuleUtil::randomFormula(300,100));
		deleteAndNull(cont8);
		deleteAndNull(depotChest);
		return;
	}
	deleteAndNull(cont8);
	
	/**
	* Scan through all containers in the depot chest to look for an empty one
	*/
	int itemNr;
	int count=depotChest->itemsInside;
	int targetPosDirect=-1;
	CTibiaTile *tile = reader.getTibiaTile(objectId);
	for (itemNr=0;itemNr<count;itemNr++) {
		CTibiaItem *item = (CTibiaItem *)depotChest->items.GetAt(itemNr);
		//look for place directly in depot if needed
		if (tile->stackable)
		{		
			if (item->objectId==objectId&&
				(item->quantity+qty<100||depotChest->itemsInside<depotChest->size))
			{
				targetPosDirect=itemNr;
				break;
			}
		}

		if (reader.getTibiaTile(item->objectId)->isContainer) {
			// that object is a container, so open it
			sender.closeContainer(depotContNr2);
			CModuleUtil::waitForOpenContainer(depotContNr2,false);
			sender.openContainerFromContainer(item->objectId,0x40+depotContNr,itemNr,depotContNr2);
			CModuleUtil::waitForOpenContainer(depotContNr2,true);
			CTibiaContainer *newContainer = reader.readContainer(depotContNr2);
			if (newContainer->flagOnOff) {
				int targetPos=-1;
				if (tile->stackable)
				{						
					// if item is stackable then try to find a suitable stack for it
					for (int stackedItemPos=0;stackedItemPos<newContainer->itemsInside;stackedItemPos++)
					{
						CTibiaItem *stackedItem=(CTibiaItem *)newContainer->items.GetAt(stackedItemPos);
						if (stackedItem->objectId==objectId&&
							(stackedItem->quantity+qty<100||newContainer->itemsInside<newContainer->size))
						{
							targetPos=stackedItemPos;
							break;
						}
					}
				}
				if(targetPos==-1 && newContainer->itemsInside<newContainer->size){
					targetPos=newContainer->size-1;
				}
				if (targetPos!=-1){
					// found container in which we can do deposit!
					sender.moveObjectBetweenContainers(objectId,0x40+sourceContNr,sourcePos,0x40+depotContNr2,targetPos,qty);
					CModuleUtil::waitForItemsInsideChange(depotContNr2,newContainer->itemsInside);
					Sleep(CModuleUtil::randomFormula(300,100));
					deleteAndNull(newContainer);
					deleteAndNull(depotChest);
					return;
				}
			}
			deleteAndNull(newContainer);
		}
	}
	if (targetPosDirect==-1&&depotChest->itemsInside<depotChest->size){
		targetPosDirect=depotChest->size-1;
	}
	if (targetPosDirect!=-1){
		sender.moveObjectBetweenContainers(objectId,0x40+sourceContNr,sourcePos,0x40+depotContNr,targetPosDirect,qty);
		CModuleUtil::waitForItemsInsideChange(depotContNr,depotChest->itemsInside);
		Sleep(CModuleUtil::randomFormula(300,100));
		deleteAndNull(depotChest);
		return;
	}

	
	deleteAndNull(depotChest);
	
	// if we are here this means that the depot is full
	globalAutoAttackStateDepot=CToolAutoAttackStateDepot_noSpace;
}

// take items from depot chest and move to my backpacks (contnr,pos)
int depotDepositTakeFromChest(int objectId,int contNr,int pos,int qtyToPickup,int depotContNr,int depotContNr2) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaContainer *depotChest = reader.readContainer(depotContNr);
	int itemNr;
	
	if (!depotChest->flagOnOff) {
		// shouldn't happen...
		deleteAndNull(depotChest);
		return 0;
	}
	
	/**
	* If container number 8 is open than we can try to take our item from it (to avoid 
	* massive close/open of containers.
	*/
	CTibiaContainer *cont8 = reader.readContainer(depotContNr2);
	if (cont8->flagOnOff) {
		for (itemNr=0;itemNr<cont8->itemsInside;itemNr++) {
			CTibiaItem *item=((CTibiaItem *)cont8->items.GetAt(itemNr));
			if (item->objectId==objectId) {
				// that's this item - pick it up
				if (qtyToPickup>item->quantity) qtyToPickup=item->quantity;
				sender.moveObjectBetweenContainers(objectId,0x40+depotContNr2,itemNr,0x40+contNr,pos,qtyToPickup);
				CModuleUtil::waitForItemsInsideChange(depotContNr2,cont8->itemsInside);
				Sleep(CModuleUtil::randomFormula(300,100));
				deleteAndNull(cont8);
				deleteAndNull(depotChest);
				return qtyToPickup;
			}
		}
	}
	deleteAndNull(cont8);
	
	/**
	* Scan through all containers in the depot chest to look for some matching one
	*/
	int count=depotChest->itemsInside;
	for (itemNr=0;itemNr<count;itemNr++) {
		CTibiaItem *item = (CTibiaItem *)depotChest->items.GetAt(itemNr);
		if (reader.getTibiaTile(item->objectId)->isContainer) {
			// that object is a container, so open it
			sender.closeContainer(depotContNr2);
			CModuleUtil::waitForOpenContainer(depotContNr2,false);
			sender.openContainerFromContainer(item->objectId,0x40+depotContNr,itemNr,depotContNr2);
			CModuleUtil::waitForOpenContainer(depotContNr2,true);
			CTibiaContainer *newContainer = reader.readContainer(depotContNr2);
			if (newContainer->flagOnOff) {
				for (itemNr=0;itemNr<newContainer->itemsInside;itemNr++) {
					CTibiaItem *item=((CTibiaItem *)newContainer->items.GetAt(itemNr));
					if (item->objectId==objectId) {
						// that's this item - pick it up
						if (qtyToPickup>item->quantity) qtyToPickup=item->quantity;
						sender.moveObjectBetweenContainers(objectId,0x40+depotContNr2,itemNr,0x40+contNr,pos,qtyToPickup);
						CModuleUtil::waitForItemsInsideChange(depotContNr2,newContainer->itemsInside);
						Sleep(CModuleUtil::randomFormula(300,100));
						deleteAndNull(newContainer);
						deleteAndNull(depotChest);
						
						return qtyToPickup;
					}
				}
			}
			deleteAndNull(newContainer);
		}
	}
	
	deleteAndNull(depotChest);
	
	// our item to move not found;
	return 0;
}

int countAllItemsOfType(int objectId) {
	CMemReaderProxy reader;
	int contNr;
	int ret=0;
	for (contNr=0;contNr<8;contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);
		
		if (cont->flagOnOff)
			ret+=cont->countItemsOfType(objectId);
		
		deleteAndNull(cont);
	}
	return ret;
}

/**
* We are nearby a depot, so do the depositing.
*/
void depotDeposit(CConfigData *config) {
	int i;
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	int depotContNr=-1;
	int depotContNr2=-1;
	/*
	* Now we must do:
	* 1. locate depot chest (optional)
	* 2. open depot chest and find place for item (optional)
	* 3. deposit items
	*/
	
	if (!config->depotDropInsteadOfDepositon) {
		globalAutoAttackStateDepot=CToolAutoAttackStateDepot_opening;
		for (int x=-1;x<=1;x++){
			for (int y=-1;y<=1;y++){
				if ((x!=0)!=(y!=0)){
					depotContNr=depotDepositOpenChest(self->x+x,self->y+y,self->z);
					if (depotContNr!=-1){ x=999;y=999;} // exit loop
				}
			}
		}
		if (depotContNr==-1) {
			// can't open depot chest ?!?!?
			// so cancel depositing and proceed to walking
			deleteAndNull(self);
			globalAutoAttackStateDepot=CToolAutoAttackStateDepot_walking;
			return;
		}
	}
	
	// do the depositing
	globalAutoAttackStateDepot=CToolAutoAttackStateDepot_depositing;
	depotContNr2 = CModuleUtil::findNextClosedContainer(depotContNr);
	
	for (i=0;i<100&&strlen(config->depotTrigger[i].itemName);i++) {
		int objectToMove = itemProxy.getObjectId(config->depotTrigger[i].itemName);
		int contNr;
		int totalQty=countAllItemsOfType(objectToMove);
		
		if (config->depotTrigger[i].when>config->depotTrigger[i].remain) {
			// deposit to depot
			
			int qtyToMove=totalQty-config->depotTrigger[i].remain;
			for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++) {
				if (contNr==depotContNr) continue;
				CTibiaContainer *cont = reader.readContainer(contNr);
				
				if (cont->flagOnOff) {
					int itemNr;
					for (itemNr=cont->itemsInside-1;itemNr>=0;itemNr--) {
						CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(itemNr);
						if (item->objectId==objectToMove) {
							int itemQty = item->quantity?item->quantity:1;
							if (totalQty-itemQty<config->depotTrigger[i].remain) {
								itemQty=totalQty-config->depotTrigger[i].remain;
							}
							if (itemQty>0) {
								if (!config->depotDropInsteadOfDepositon) {
									// move to the depot chest
									depotDepositMoveToChest(objectToMove,contNr,item->pos,itemQty,depotContNr,depotContNr2);
								}
								else {
									// move onto the floor
									CTibiaCharacter *selftmp = reader.readSelfCharacter();
									sender.moveObjectFromContainerToFloor(objectToMove,0x40+contNr,item->pos,self->x,self->y,self->z,itemQty);
									CModuleUtil::waitForCapsChange(selftmp->cap);
									Sleep(CModuleUtil::randomFormula(300,100));
									deleteAndNull(selftmp);
								}
							}
							totalQty-=itemQty;
						}
					}
				}
				deleteAndNull(cont);
			}
		}
		if (config->depotTrigger[i].when<config->depotTrigger[i].remain&&!config->depotDropInsteadOfDepositon) {
			// pickup from depot
			int qtyToPickup=config->depotTrigger[i].remain-countAllItemsOfType(objectToMove);
			int movesInItemation=1;
			while (qtyToPickup>0&&movesInItemation>0) {
				movesInItemation=0;
				for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++) {
					if (contNr==depotContNr) continue;

					CTibiaContainer *cont = reader.readContainer(contNr);
					if (cont->flagOnOff&&cont->itemsInside<cont->size) {
						int movedQty=depotDepositTakeFromChest(objectToMove,contNr,cont->size-1,qtyToPickup,depotContNr,depotContNr2);
						if (movedQty) movesInItemation++;
					}
					deleteAndNull(cont);
				}
				qtyToPickup=config->depotTrigger[i].remain-countAllItemsOfType(objectToMove);
			} // while picking up 
		} // if pickup from depot
	}

	
	// all is finished :) - we can go back to the hunting area
	sender.closeContainer(depotContNr2);
	CModuleUtil::waitForOpenContainer(depotContNr2,false);
	Sleep(CModuleUtil::randomFormula(300,100));
	sender.closeContainer(depotContNr);
	CModuleUtil::waitForOpenContainer(depotContNr,false);

	globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notRunning;
	depotX=depotY=depotZ=0;
	deleteAndNull(self);

}

/**
* Make sure that some item of type objectId is in the location (e.g. hand).
*/
int ensureItemInPlace(int outputDebug,int location, int locationAddress, int objectId) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaItem *itemSlot = reader.readItem(locationAddress);

	int hasNeededItem=0;
	if (itemSlot->objectId==objectId) {
		// everything's ok - no changes
		deleteAndNull(itemSlot);
		return 1;
	}
	globalAutoAttackStateTraining=CToolAutoAttackStateTraining_switchingWeapon;
	for (int i=0;i<2 && itemSlot->objectId!=objectId;i++)
	{
		if (outputDebug) registerDebug("Next moving item in ensureItemInPlace");
		
		// Search for the item that should occupy the slot
		CUIntArray itemsAccepted;
		int contNr;
		itemsAccepted.Add(objectId);
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++) {
			CTibiaItem *itemWear = CModuleUtil::lookupItem(contNr,&itemsAccepted);
			if (itemWear) {
				CTibiaContainer *cont = reader.readContainer(contNr);
				if (cont->flagOnOff) {
					if (cont->itemsInside>=cont->size && itemSlot->objectId!=0) {
						// container with desired item is full and we need a place to put the item occupying the slot
						int hasSpace=0;
						for (int contNrSpace=0;contNrSpace<memConstData.m_memMaxContainers;contNrSpace++) {
							CTibiaContainer *contSpace = reader.readContainer(contNrSpace);
							if (contSpace->flagOnOff&&contSpace->itemsInside<contSpace->size) {
								// container has some free places
								sender.moveObjectBetweenContainers(itemSlot->objectId,location,0,0x40+contNrSpace,contSpace->size-1,itemSlot->quantity?itemSlot->quantity:1);
								CModuleUtil::waitForItemsInsideChange(contNrSpace,contSpace->itemsInside);
								Sleep(CModuleUtil::randomFormula(300,100));
								hasSpace=1;
								contNrSpace=10000; // stop the loop
							}
							deleteAndNull(contSpace);
						}
						if (!hasSpace) {
							// means: we have no place in any container
							if (time(NULL)-lastTAMessageTm>taMessageDelay){
								lastTAMessageTm=time(NULL);
								sender.sendTAMessage("I've an invalid weapon in hand, and I can't move it anywhere!");
							}
							deleteAndNull(itemSlot);
							deleteAndNull(itemWear);
							deleteAndNull(cont);
							return 0;
						}
					}
					//Assert: we can simply switch the item to wear and the item in the slot OR the slot is empty
					sender.moveObjectBetweenContainers(itemWear->objectId,0x40+contNr,itemWear->pos,location,0,itemWear->quantity?itemWear->quantity:1);
					CModuleUtil::waitForItemChange(locationAddress,itemSlot->objectId);
					Sleep(CModuleUtil::randomFormula(300,100));
					contNr=10000; // stop the loop
					hasNeededItem=1;
				}
				deleteAndNull(cont);
				deleteAndNull(itemWear);
			}
			
		}
		if (!hasNeededItem) {
			//means: we cannot find the needed item
			if (time(NULL)-lastTAMessageTm>taMessageDelay){
				lastTAMessageTm=time(NULL);
				sender.sendTAMessage("I can't find the training/fight weapon in any container!");
			}
			deleteAndNull(itemSlot);
			return 0;
		}
		CModuleUtil::waitForItemChange(locationAddress,itemSlot->objectId);
		deleteAndNull(itemSlot);
		itemSlot = reader.readItem(locationAddress);
	}
	deleteAndNull(itemSlot); 
	return 1;
}

/**
* Check the training related things, like: 
* 1. Check whether we should be in fight or training mode (and switch weapons).
* 2. Check whether we should be full attack/def/dont touch (if blood control is active).
* 3. Switch weapon if needed.
*/
void trainingCheck(CConfigData *config, int currentlyAttackedCreatureNr, int alienFound, int attackingCreatures, int lastAttackedCreatureBloodHit,int *attackMode) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	int weaponHandAddress = config->weaponHand ? memConstData.m_memAddressRightHand : memConstData.m_memAddressLeftHand;
	int weaponHand = config->weaponHand ? 5 : 6;
	
	if (config->trainingActivate) {
		char buf[128];
		sprintf(buf,"Training check: attackedCr=%d, alien=%d, #attacking=%d, lastHpDrop=%d, attackMode=%d",currentlyAttackedCreatureNr,alienFound,attackingCreatures,time(NULL)-lastAttackedCreatureBloodHit,*attackMode);
		if (config->debug) registerDebug(buf);

		if (config->fightWhenAlien&&!alienFound&&!attackingCreatures&&currentlyAttackedCreatureNr==-1){
			//if (config->debug) registerDebug("Training: non-training mode.");
			if (!ensureItemInPlace(config->debug,weaponHand,weaponHandAddress,config->weaponFight)){
				if (config->debug) registerDebug("Training: Failed to equip fight weapon for non-training mode.");
				ensureItemInPlace(config->debug,weaponHand,weaponHandAddress,config->weaponTrain);
			}
			globalAutoAttackStateTraining=CToolAutoAttackStateTraining_notRunning;
			return;
		}
			
		int canTrain=1;
		if (config->fightWhenAlien&&alienFound) canTrain=0;
		if (config->fightWhenSurrounded&&attackingCreatures>2) canTrain=0;

		if (canTrain) {
			//if (config->debug) registerDebug("Training: Training mode.");
			if (!ensureItemInPlace(config->debug,weaponHand,weaponHandAddress,config->weaponTrain)){
				registerDebug("Training: Failed to switch to training weapon.");
				ensureItemInPlace(config->debug,weaponHand,weaponHandAddress,config->weaponFight);
			}
			if (config->trainingMode) *attackMode=config->trainingMode;//0-blank, 1-full atk, 3-full def

			globalAutoAttackStateTraining=CToolAutoAttackStateTraining_training;
			if (config->bloodHit) {
				sprintf(buf,"Training: inside blood hit control (%d-%d=%d)",time(NULL),lastAttackedCreatureBloodHit,time(NULL)-lastAttackedCreatureBloodHit);
				if (config->debug) registerDebug(buf);
				if (time(NULL)-lastAttackedCreatureBloodHit<20) {
					// if 30s has passed since last blood hit, then we do 'normal' attack mode
					// otherwise we do full defence
					*attackMode=2;
					globalAutoAttackStateTraining=CToolAutoAttackStateTraining_trainingFullDef;
				}
			}
		}
		else {
			//if (config->debug) registerDebug("Training: Fight mode.");
			if (!ensureItemInPlace(config->debug,weaponHand,weaponHandAddress,config->weaponFight)){
				if (config->debug) registerDebug("Training: Failed to equip fight for fight mode.");
				ensureItemInPlace(config->debug,weaponHand,weaponHandAddress,config->weaponTrain);
			}
			globalAutoAttackStateTraining=CToolAutoAttackStateTraining_fighting;
		}
	}
}

void dropAllItemsFromContainer(int contNr, int x, int y, int z) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaContainer *dropCont = reader.readContainer(contNr);
	if (dropCont->flagOnOff) {
		int itemNr;
		for (itemNr=dropCont->itemsInside-1;itemNr>=0;itemNr--) {
			CTibiaItem *lootItem = (CTibiaItem *)dropCont->items.GetAt(itemNr);
			sender.moveObjectFromContainerToFloor(lootItem->objectId,0x40+contNr,lootItem->pos,x,y,z,lootItem->quantity?lootItem->quantity:1);
			CModuleUtil::waitForItemsInsideChange(contNr,itemNr+1);
			Sleep(CModuleUtil::randomFormula(500,200));
		}
	}
	deleteAndNull(dropCont);
}


/////////////////////////////////////////////////////////////////////////////
void droppedLootCheck(CConfigData *config, int *lootedArr,int lootedArrSize) {
	char buf[128];
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CTibiaMapProxy tibiaMap;	
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();

	if (config->lootFromFloor&&self->cap>config->capacityLimit) {
		
		if (config->debug) registerDebug("Entering 'loot from floor' area");
		
		int freeContNr=-1;
		int freeContPos=-1;
		
		// lookup a free place in any container
		int contNr;
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++) {
			CTibiaContainer *cont = reader.readContainer(contNr);
			if (cont->flagOnOff&&cont->itemsInside<cont->size) {
				freeContNr=contNr;
				freeContPos=cont->size-1;
				deleteAndNull(cont);
				break;
			}
			deleteAndNull(cont);
		}
		
		if (freeContNr==-1) {
			if (config->debug) registerDebug("Loot from floor: no free container found");
			// no free place in container found - exit
			deleteAndNull(self);
			return;
		}else{
			sprintf(buf,"Container Found: Cont %d Slot%d)",freeContNr,freeContPos);
			if (config->debug) registerDebug(buf);
		}
		
		int foundLootedObjectId=0;
		int x=0,y=0;
		int xSwitch=0;
		int ySwitch=0;
		while (x!=8 && y !=8){
			//manage x and y coords for spiraling
			if (xSwitch==0 && ySwitch==0){xSwitch=1;}
			else if (x==y && x>=0 && xSwitch==1 && ySwitch==0) {x++; xSwitch=0;ySwitch=-1;}
			else if (!xSwitch && !(x==y && x>=0) && abs(x)==abs(y)){xSwitch=ySwitch;ySwitch=0;x+=xSwitch;y+=ySwitch;}
			else if (!ySwitch && abs(x)==abs(y)){ySwitch=-xSwitch;xSwitch=0;x+=xSwitch;y+=ySwitch;}
			else {x+=xSwitch;y+=ySwitch;}
			
			if (abs(x)>7 || abs(y)>5) continue;

			int f1=isItemCovered(x,y,lootedArr,lootedArrSize);
			int f2=isItemOnTop(x,y,lootedArr,lootedArrSize);
			if (f1) foundLootedObjectId=f1;
			if (f2) foundLootedObjectId=f2;
			if(!foundLootedObjectId) continue;

			int shouldStandOn=reader.mapGetPointItemsCount(point(x,y,0))>=10;

			//Walk To Square
			if (abs(x) > 1 || abs(y) > 1 || shouldStandOn&&!(x==0&&y==0)) {
				int path[15];
				int pathSize=0;
				memset(path,0x00,sizeof(int)*15);
				if (config->debug){
					sprintf(buf, "findPathOnMap: loot from floor (%d, %d) %x", x, y,foundLootedObjectId);
					registerDebug(buf);
				}
				CTibiaCharacter *self2 = reader.readSelfCharacter();
				CModuleUtil::findPathOnMap(self2->x, self2->y, self2->z, self->x+x, self->y+y, self->z, 0,path);
				for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){}

				//continue if farther than 10 spaces or path does not include enough info to get there(meaning floor change)
				if (pathSize>=10 || pathSize<x+y-3) {deleteAndNull(self2); continue;}

				sprintf(buf,"Loot from floor: item (%d,%d,%d)",x,y,pathSize);
				if (config->debug) registerDebug(buf);
				sprintf(buf, "Path[0]: %d\tpathSize: %d", path[0], pathSize);
				if (config->debug) registerDebug(buf);

				if (pathSize)  {
					//sender.stopAll();
					//sprintf(buf, "Walking attempt: %d\nPath Size: %d", walkItem, pathSize);
					//AfxMessageBox(buf);
					if (shouldStandOn)
						path[--pathSize]=0;
					CModuleUtil::executeWalk(self2->x,self2->y,self2->z,path);
					// wait for reaching final point
					CModuleUtil::waitToApproachSquare(self->x+x,self->y+y);
				}
				else {
					sprintf(buf,"Loot from floor: aiks, no path found (%d,%d,%d)->(%d,%d,%d)!",self2->x,self2->y,self2->z,self->x+x,self->y+y,self->z);
					if (config->debug) registerDebug(buf);
					deleteAndNull(self2);
					continue;
				}
				deleteAndNull(self2);
			}
			CTibiaCharacter *self2 = reader.readSelfCharacter();
			int itemX=self->x+x-self2->x;
			int itemY=self->y+y-self2->y;

			sprintf(buf,"Loot from floor: found looted object id=0x%x at %d,%d",foundLootedObjectId,itemX,itemY);
			if (config->debug) registerDebug(buf);
			
			//If failed to get within 1 square, let cavebot take over again
			if(!(abs(itemX)<=1 && abs(itemY)<=1 && self2->z==self->z)) break;

			//Uncover Item
			if(foundLootedObjectId==f1&&foundLootedObjectId!=f2)
			{
				int offsetX,offsetY;
				int foundSpace=0;
				// Need to find free square to dump items
				for (offsetX=-1;offsetX<=1;offsetX++) {
					for (offsetY=-1;offsetY<=1;offsetY++) {
						// double loop break!
						if (self2->x+offsetX!=self->x+x&&self2->y+offsetY!=self->y+y&&tibiaMap.isPointAvailable(self2->x+offsetX,self2->y+offsetY,self2->z)&&reader.mapGetPointItemsCount(point(offsetX,offsetY,0))<9) 
						{
							// force loop break;
							foundSpace=1;
							goto exitLoop;
						}
					}
				}
				if (!(self2->x==self->x+x && self2->y==self->y+y)){
					offsetX=0;
					offsetY=0;
					foundSpace=1;
				}
exitLoop:
				sprintf(buf,"Loot from floor: using (%d,%d) as dropout offset/item=%d",offsetX,offsetY,foundLootedObjectId);
				if (config->debug) registerDebug(buf);
				// do it only when drop place is found
				if (foundSpace){
					int uncoverItem=reader.mapGetPointItemsCount(point(itemX,itemY,0))-itemOnTopIndex(itemX,itemY);
					while (uncoverItem-->0&&isItemCovered(itemX,itemY,foundLootedObjectId)&&!isItemOnTop(0,0,foundLootedObjectId)) {
						if (config->debug) registerDebug("Loot from floor: uncovering item");
						int objectId=itemOnTopCode(itemX,itemY);
						int qty=itemOnTopQty(itemX,itemY);
						sender.moveObjectFromFloorToFloor(objectId,self->x+x,self->y+y,self->z,self2->x+offsetX,self2->y+offsetY,self2->z,qty);
						Sleep(CModuleUtil::randomFormula(400,200));
					}
				}
				else {
					if (config->debug) registerDebug("Loot from floor: bad luck - no place to throw junk.");
					deleteAndNull(self2);
					continue;
				}
			}

			//Take item
			if (isItemOnTop(itemX,itemY,foundLootedObjectId)) {
				if (config->debug) registerDebug("Loot from floor: picking up item");
				int qty=itemOnTopQty(itemX,itemY);
				sender.moveObjectFromFloorToContainer(foundLootedObjectId,self->x+x,self->y+y,self->z,0x40+freeContNr,freeContPos,qty);
				Sleep(CModuleUtil::randomFormula(400,200));
				CModuleUtil::waitForItemsInsideChange(freeContNr,freeContPos);
			}
			else {
				if (config->debug) registerDebug("Loot from floor: tough luck - item is not on top. Please wait and try again.");
			}
			//If gotten this far, let cavebot takeover for a bit
			deleteAndNull(self2);
			break;
		}
	} // if (config->lootFromFloor&&self->cap>config->capacityLimit) {
	deleteAndNull(self);


/*
//anilez	if (config->lootFromFloor&&self->cap>config->capacityLimit) {
		if (config->debug) registerDebug("Entering 'loot from floor' area");
		
		int freeContNr=-1;
		int freeContPos=-1;
		int freeContInside=-1;
		int x,y;
		int bestX=0,bestY=0,bestPath=1000;
		
		// lookup a free place in any container
		int contNr;
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++) {
			CTibiaContainer *cont = reader.readContainer(contNr);
			if (cont->flagOnOff&&cont->itemsInside<cont->size) {
				freeContNr=contNr;
				freeContPos=cont->size-1;
				freeContInside=cont->itemsInside;
				deleteAndNull(cont);
				break;
			}
			deleteAndNull(cont);
		}
		
		if (freeContNr==-1) {
			if (config->debug) registerDebug("Loot from floor: no free container found");
			// no free place in container found - exit
			deleteAndNull(self);
			return;
		}
		
		int foundLootedObjectId=0;
		
		for (x=-7;x<=7;x++) {
			for (y=-5;y<=5;y++) {
				int f1=isItemCovered(x,y,lootedArr,lootedArrSize);
				int f2=isItemOnTop(x,y,lootedArr,lootedArrSize);
				if (f1) foundLootedObjectId=f1;
				if (f2) foundLootedObjectId=f2;
				
				if (f1||f2) {
					if (f2 && abs(x) <= 1 && abs(y) <= 1) {
						sprintf(buf, "Loot from floor: picking up item (adjacent/same square) [0x%x]", f2);
						if (config->debug) registerDebug(buf);
						int qty=itemOnTopQty(x, y);
						sender.moveObjectFromFloorToContainer(foundLootedObjectId, self->x + x, self->y + y, self->z, 0x40+freeContNr, freeContPos,qty);
						CModuleUtil::waitForItemsInsideChange(freeContNr, freeContInside);
						//y--; // Recheck the square.
					}
					else {
						// there is the lootedItem (on top or covered)
						int path[15];
						int pathSize=0;
						memset(path,0x00,sizeof(int)*15);
						if (config->debug){
							sprintf(buf, "findPathOnMap: loot from floor (%d, %d)", x, y);
							registerDebug(buf);
						}
						CModuleUtil::findPathOnMap(self->x, self->y, self->z, self->x+x, self->y+y, self->z, 0,path);
						for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){}
						
						// if item is on our place then this is the best situation
						if (x==0&&y==0) pathSize=0;
						
						if (path[0] && bestPath>=pathSize) {
							
							// there is a path to the point AND the path
							// is the closest (the best)
							bestX=x;
							bestY=y;
							bestPath=pathSize;
							sprintf(buf,"Loot from floor: item (%d,%d,%d)",x,y,pathSize);
							if (config->debug) registerDebug(buf);
						}
						if (x == 0 && y == 0) {
							bestX=x;
							bestY=y;
							bestPath=pathSize;
						}
						sprintf(buf, "Path[0]: %d\tpathSize: %d", path[0], pathSize);
						if (config->debug) registerDebug(buf);
					} //end else
					sprintf(buf, "Covered: %s\tOn Top: %s\tCode: 0x%x", f1?"Yes":"No", f2?"Yes":"No", foundLootedObjectId);
					if (config->debug) registerDebug(buf);
				}
			} // for y
		} // for x
		if (foundLootedObjectId) {
			sprintf(buf,"Loot from floor: found looted object id=0x%x",foundLootedObjectId);
			if (config->debug) registerDebug(buf);
		}
		else
			

		//lootTile = reader.getTibiaTile(itemOnTopCode(bestX, bestY));
		//if (lootTile->notMoveable) {
		//	sprintf(buf,"Loot from floor: Loot object %d currently not lootable (Loot Tile: %d)",foundLootedObjectId, lootTile);
		//	if (config->debug) registerDebug(buf);
		//	return;
		//}
		
		if (config->debug) registerDebug("Loot from floor: before anything to loot check");
		
		if (bestPath<1000) {
			if (config->debug) registerDebug("Loot from floor: found something to loot -> walking");
			// anything to loot found
			
			CTibiaCharacter *self2 = reader.readSelfCharacter();
			
			if (self2->x==self->x+bestX&&self2->y==self->y+bestY&&self2->z==self->z) {
				// no need to walk as we are on position already
			}
			
			int path[15];
			int pathSize=0;
			memset(path,0x00,sizeof(int)*15);
			if (config->debug) registerDebug("findPathOnMap: loot from floor 2");
			CModuleUtil::findPathOnMap(self2->x, self2->y, self2->z, self->x+bestX, self->y+bestY, self->z, 0,path);
			for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){}
			
			sprintf(buf,"Loot from floor: path size=%d between (%d,%d,%d) and (%d,%d,%d)",pathSize,self2->x,self2->y,self2->z,self->x+bestX,self->y+bestY,self->z);
			if (config->debug) registerDebug(buf);
			if (pathSize)  {
				//sender.stopAll();
				//sprintf(buf, "Walking attempt: %d\nPath Size: %d", walkItem, pathSize);
				//AfxMessageBox(buf);
				CModuleUtil::executeWalk(self2->x,self2->y,self2->z,path);
				// wait for reaching final point
				CModuleUtil::waitToApproachSquare(self->x+bestX,self->y+bestY);
			}
			else {
				sprintf(buf,"Loot from floor: aiks, no path found (%d,%d,%d)->(%d,%d,%d)!",self2->x,self2->y,self2->z,self->x+bestX,self->y+bestY,self->z);
				if (config->debug) registerDebug(buf);
			}
			sprintf(buf,"Loot from floor: location me(%d,%d,%d)->item(%d,%d,%d)",self2->x,self2->y,self2->z,self->x+bestX,self->y+bestY,self->z);
			if (config->debug) registerDebug(buf);
			
			deleteAndNull(self2);
			self2 = reader.readSelfCharacter();
			if (self->x+bestX==self2->x&&self->y+bestY==self2->y&&self->z==self2->z) {
				if (config->debug) registerDebug("Loot from floor: standing over item");
				// we are standing over the item
				
				int offsetX,offsetY;
				// means - it's covered, so we need to "dig" to it
				for (offsetX=-1;offsetX<=1;offsetX++) {
					for (offsetY=-1;offsetY<=1;offsetY++) {
						// double loop break!
						if ((offsetX||offsetY)&&tibiaMap.isPointAvailable(self2->x+offsetX,self2->y+offsetY,self2->z)) 
						{
							// force loop break;
							goto exitLoop;	
						}
					}
				}
exitLoop:
				sprintf(buf,"Loot from floor: using (%d,%d) as dropout offset/item=%d",offsetX,offsetY,foundLootedObjectId);
				if (config->debug) registerDebug(buf);
				if (offsetX!=2&&offsetY!=2) {
					int uncoverItem=0;
					// do it only when drop place if found
					while (uncoverItem++<10&&isItemCovered(0,0,foundLootedObjectId)&&!isItemOnTop(0,0,foundLootedObjectId)) {
						if (config->debug) registerDebug("Loot from floor: uncovering item");
						int objectId=itemOnTopCode(0,0);
						int qty=itemOnTopQty(0,0);
						sender.moveObjectFromFloorToFloor(objectId,self2->x,self2->y,self2->z,self2->x+offsetX,self2->y+offsetY,self2->z,qty);
						Sleep(CModuleUtil::randomFormula(400,200));
					}
					if (isItemOnTop(0,0,foundLootedObjectId)) {
						if (config->debug) registerDebug("Loot from floor: picking up item");
						int qty=itemOnTopQty(0,0);
						sender.moveObjectFromFloorToContainer(foundLootedObjectId,self2->x,self2->y,self->z,0x40+freeContNr,freeContPos,qty);
						CModuleUtil::waitForItemsInsideChange(freeContNr,freeContInside);
						Sleep(CModuleUtil::randomFormula(400,200));

					}
					else {
						if (config->debug) registerDebug("Loot from floor: bad luck - item is not on top");
					}
				}
			}
			
			deleteAndNull(self2);
		}
	} // if (config->lootFromFloor&&self->cap>config->capacityLimit) {
	deleteAndNull(self);
	*/
}

void fireRunesAgainstCreature(CConfigData *config,int creatureId) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	
	int contNr,realContNr;
	CUIntArray acceptedItems;
	CTibiaItem *rune=NULL;
	
	if (config->debug) registerDebug("Firing runes at alien enemy");
	
	acceptedItems.RemoveAll();
	acceptedItems.Add(itemProxy.getValueForConst("runeSD"));
	for (contNr=0;contNr<memConstData.m_memMaxContainers&&!rune;contNr++)
		rune = CModuleUtil::lookupItem(contNr,&acceptedItems),realContNr=contNr;
	
	acceptedItems.RemoveAll();
	acceptedItems.Add(itemProxy.getValueForConst("runeExplo"));
	for (contNr=0;contNr<memConstData.m_memMaxContainers&&!rune;contNr++)
		rune = CModuleUtil::lookupItem(contNr,&acceptedItems),realContNr=contNr;
	
	acceptedItems.RemoveAll();
	acceptedItems.Add(itemProxy.getValueForConst("runeHMM"));
	for (contNr=0;contNr<memConstData.m_memMaxContainers&&!rune;contNr++)
		rune = CModuleUtil::lookupItem(contNr,&acceptedItems),realContNr=contNr;
	
	acceptedItems.RemoveAll();
	acceptedItems.Add(itemProxy.getValueForConst("runeGFB"));
	for (contNr=0;contNr<memConstData.m_memMaxContainers&&!rune;contNr++)
		rune = CModuleUtil::lookupItem(contNr,&acceptedItems),realContNr=contNr;
	
	if (rune) {
		if (config->debug) registerDebug("Rune to fire found!");
		sender.castRuneAgainstCreature(0x40+realContNr,rune->pos,rune->objectId,creatureId,2);
	}
} // fireRunesAgainstCreature()

/////////////////////////////////////////////////////////////////////////////
// shared memory initalisation
void shMemInit(CSharedMemory *pMem) {
	int pid=GetCurrentProcessId();
	char varName[128];
	sprintf(varName,"mod_cavebot_%d",pid);
	// this means shmem initialisation failed
	if (!pMem||!pMem->IsCreated()) return;
	pMem->AddDwordValue(varName,0);
}

// return monster priority (order on the monster list)
// monsters out of the list have priority (0)
// highest priority have higher numbers
int getAttackPriority(CConfigData *config,char *monsterName) {
	char buf[64];
	int monstListNr;
	int i,len;
	strcpy(buf,monsterName);
	// first filter out [] created by creature info
	for (i=0,len=strlen(buf);i<len;i++) {
		if (buf[i]=='[')  {
			buf[i]='\0';
			break;
		}
	}
	for (monstListNr=0;monstListNr<config->monsterCount;monstListNr++) {
		if (!strcmpi(config->monsterList[monstListNr],buf)) {
			return config->monsterCount-monstListNr;
		}
	}
	
	return 0;
}

int isInHalfSleep() {
	CMemReaderProxy reader;
	char *var=reader.getGlobalVariable("caveboot_halfsleep");
	if (var==NULL||strcmp(var,"true")) return 0; else return 1;
}

int isInFullSleep() {
	CMemReaderProxy reader;
	char *var=reader.getGlobalVariable("caveboot_fullsleep");
	if (var==NULL||strcmp(var,"true")) return 0; else return 1;
}

//returns 1 if needed to resend attack info
//returns 0 if same creature already being attacked
int AttackCreature(CConfigData *config,int id){
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaCharacter *self = reader.readSelfCharacter();
	CTibiaCharacter *attackedCh = reader.getCharacterByTibiaId(id);
	int old =reader.getAttackedCreature();
	if (attackedCh&&reader.getAttackedCreature()!=id){
		reader.cancelAttackCoords();
		Sleep(200);
		reader.setRemainingTilesToGo(0);
		reader.writeCreatureDeltaXY(self->nr,0,0);
		sender.attack(id);
		reader.setAttackedCreature(id);
		if (config->debug&&id){
			char buf[128];
			sprintf(buf,"used to be=%d, set to=%d, now attacking=%d,hp=%d",old,id,reader.getAttackedCreature(),attackedCh->hpPercLeft);
			registerDebug(buf);
		}
		deleteAndNull(self);
		deleteAndNull(attackedCh);
		return 1;
	}
	deleteAndNull(self);
	deleteAndNull(attackedCh);
	return 0;
}
//returns the smallest radius of a square centred at x1,y1 and covers x2,y2
//same equation as finding distance for rods or wands
int maxDist(int x1,int y1,int x2,int y2){ return max(abs(x2-x1),abs(y2-y1));}

//returns the smallest number of steps needed to stand beside x2,y2 starting from x1,y1
//same distance ideally travelled by "Auto Follow"
int taxiDist(int x1,int	y1,int x2,int y2){ return abs(x2-x1)+abs(y2-y1)-2+(x1==x2)+(y1==y2);}

// will eventually use the minimap to determine if path is ok
int canGetToPoint(int targetX,int targetY,int targetZ){
	/*
	CMemReaderProxy reader;
	int path[15];
	CTibiaCharacter *self = reader.readSelfCharacter();
	CModuleUtil::findPathOnMap(self->x,self->y,self->z,targetX,targetY,targetZ,0,path);
	deleteAndNull(self);
	CPackSenderProxy sender;
	char buf[111];
	sprintf(buf,"%d,%d,%d,%d",path&&path[0]&&!path[8],path,path[0],path[8]);
	sender.sendTAMessage(buf);
	return path&&path[0]&&!path[14];*/
	return 1;
}

void SendAttackMode(int attack,int follow){
	CPackSenderProxy sender;
	CMemReaderProxy reader;
	static int curAttack = reader.getPlayerModeAttackType();
	static int curFollow = reader.getPlayerModeFollow();
	static int buttonAttack = reader.getPlayerModeAttackType();
	static int buttonFollow = reader.getPlayerModeFollow();

	if (buttonAttack!=reader.getPlayerModeAttackType()) curAttack=reader.getPlayerModeAttackType();
	if (buttonFollow!=reader.getPlayerModeFollow()) curFollow=reader.getPlayerModeFollow();

	if (attack == curAttack && follow == curFollow)
		return;
	else if (attack != curAttack && follow != curFollow){
		sender.attackMode(attack,curFollow);
		Sleep(CModuleUtil::randomFormula(700,500));
		sender.attackMode(curAttack,follow);
	} else {
		sender.attackMode(attack,follow);
	}
	//char buf[111];
	//sprintf(buf,"Attack Set to: %d,%d",attack,follow);
	//sender.sendTAMessage(buf);
	curAttack = attack;
	curFollow = follow;
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;


DWORD WINAPI toolThreadProc( LPVOID lpParam ) {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	int pid=GetCurrentProcessId();
	CSharedMemory sh_mem("TibiaAuto",1024,shMemInit);
	char varName[128];
	sprintf(varName,"mod_cavebot_%d",pid);
	int iter=0;
	
	int attackConsiderPeriod=20000;
	int currentlyAttackedCreatureNr=-1;

	int lastStandingX=0,lastStandingY=0,lastStandingZ=0;
	int walkerStandingEndTm=0;
	targetX=targetY=targetZ=0;

	int lastAttackedCreatureBloodHit=0;
	int reachedAttackedCreature=0;//true if 1 sqm away or hp dropped while attacking
	int shareAlienBackattack=0;
	int alienCreatureForTrainerFound=0;
	
	int pauseInvoked=0;

	int modRuns=0;//for performing actions once every 10 iterations
	// reset globals
	targetX=targetY=targetZ=0;
	depotX=depotY=depotZ=0;
	creatureAttackDist=0;
	attackSuspendedUntil=0;
	firstCreatureAttackTM=0;
	currentPosTM=time(NULL);
	
	int waypointsCount=0;
	int i;
	for (i=0;i<100;i++) {
		if (config->waypointList[i].x) {
			waypointsCount++; 
		}
		else {
			break;
		}
	}
	if(config->selectedWaypoint!=-1){
		currentWaypointNr=config->selectedWaypoint;
	} else {
		currentWaypointNr = min(0,waypointsCount-1);
	}

	// remove the debug file
	if (config->debug) CModuleUtil::masterDebug("tibiaauto-debug-cavebot.txt");
	
	shareAlienBackattack=config->shareAlienBackattack;
	if (!sh_mem.IsCreated()) {
		if (config->debug) {
			registerDebug("ERROR: shmem initialisation failed");
		}
		shareAlienBackattack=0;
	}
	
	Creature creatureList[250];

	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;
	
	



	
	
	while (!toolThreadShouldStop) {
		Sleep(250);	
		
		if (reader.getConnectionState()!=8||!config->pausingEnable)
		{
			// flush IPC communication if not logged
			while (backPipe.readFromPipe(&mess,1009)) {};
			while (backPipe.readFromPipe(&mess,2002)) {};
		}

		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected

		if (config->pausingEnable){
			if (backPipe.readFromPipe(&mess,1009)||backPipe.readFromPipe(&mess,2002)){
				int msgLen;
				char msgBuf[512];		
				memset(msgBuf,0,512);		
				memcpy(&msgLen,mess.payload,sizeof(int));
				memcpy(msgBuf,mess.payload+4,msgLen);		
				
				if (!strncmp("%ta pause",msgBuf,9))
				{				
					pauseInvoked=!pauseInvoked;
					if (pauseInvoked) sender.sendTAMessage("Paused Cavebot");
					else sender.sendTAMessage("Cavebot Unpaused");
					if (pauseInvoked){
						sender.attack(0);
						reader.setAttackedCreature(0);
						reader.cancelAttackCoords();
						// restore attack mode
						SendAttackMode(reader.getPlayerModeAttackType(),reader.getPlayerModeFollow());

						//reader.setRemainingTilesToGo(0);
						if (config->debug) registerDebug("Paused cavebot");
					}
				}
			}
			if (pauseInvoked) continue;
		}

		if (config->debug)  {
			char buf[256];
			sprintf(buf,"Next loop. States: %d, %d, %d, %d, paused=%d, sleep=%d/%d",globalAutoAttackStateAttack,globalAutoAttackStateLoot,globalAutoAttackStateWalker,globalAutoAttackStateDepot,pauseInvoked,isInHalfSleep(),isInFullSleep());
			registerDebug(buf);
		}
		
		if (isInHalfSleep()) {
			globalAutoAttackStateWalker=CToolAutoAttackStateWalker_halfSleep;
		}
		if (isInFullSleep()) {
			globalAutoAttackStateWalker=CToolAutoAttackStateWalker_fullSleep;
		}
		
		// if in a full sleep mode then just do nothing
		if (isInFullSleep()) continue;
		
		modRuns++;//for performing actions once every 10 iterations
		
		CTibiaCharacter *self = reader.readSelfCharacter();

		/**
		* Check whether we should go to a depot
		*/
		depotCheck(config);
		
		/**
		* Check whether we should collected dropped loot.
		* Conditions:
		* 1. we are not attacking anything
		* 2. we are not in a half sleep (no walking) mode
		* 3. there is something to loot
		*/
		int droppedLootArray[128];
		int droppedLootArrayCount=0;
		int moving = reader.getMemIntValue(memConstData.m_memAddressTilesToGo);
		if (currentlyAttackedCreatureNr==-1&&!isInHalfSleep() && !moving) {
			if (config->lootFood) {
				int p;
				for (p=0;p<itemProxy.getItemsFoodArray()->GetSize();p++) {
					droppedLootArray[droppedLootArrayCount++]=itemProxy.getItemsFoodArray()->GetAt(p);
					if (droppedLootArrayCount>=127) droppedLootArrayCount=127;
				}
			}
			if (config->lootGp) {
				droppedLootArray[droppedLootArrayCount++]=itemProxy.getValueForConst("GP");
				if (droppedLootArrayCount>=127) droppedLootArrayCount=127;
			}
			if (config->lootWorms) {
				droppedLootArray[droppedLootArrayCount++]=itemProxy.getValueForConst("worms");
				if (droppedLootArrayCount>=127) droppedLootArrayCount=127;
			}
			if (config->lootCustom) {
				int i;
				for (i=0;i<itemProxy.getItemsLootedCount();i++) {
					droppedLootArray[droppedLootArrayCount++]=itemProxy.getItemsLootedId(i);
					if (droppedLootArrayCount>=127) droppedLootArrayCount=127;
				}
			}
			if (droppedLootArrayCount) {
				droppedLootCheck(config,droppedLootArray,droppedLootArrayCount);
			}
		}
	
		

		/*wis notes:
		going to depot ignore creatures that aren't attacking
		training changes weapon and mode also
		*/

		if (globalAutoAttackStateAttack==CToolAutoAttackStateAttack_attackSuspended) {
			if (time(NULL)>=attackSuspendedUntil) {
				globalAutoAttackStateAttack=CToolAutoAttackStateAttack_notRunning;
				if (config->debug) registerDebug("Suspend attack period end");
			}
		}
		if (globalAutoAttackStateAttack!=CToolAutoAttackStateAttack_attackSuspended) {
			/*****
			Start attack process
			******/

			CTibiaCharacter *attackedCh = reader.readVisibleCreature(currentlyAttackedCreatureNr);
			
			/**
			* Check if currently attacked creature is alive still
			* If it is dead inform the looter it can loot
			*/
			/*if (currentlyAttackedCreatureNr!=-1) {
				if (config->debug) registerDebug("Checking whether attacked creature is alive");
				// now let's see whether creature is still alive or not
				if (attackedCh&&attackedCh->tibiaId==creatureList[currentlyAttackedCreatureNr].tibiaId) {
					if (!attackedCh->hpPercLeft&&attackedCh->z==self->z) {
						currentlyAttackedCreatureNr=-1;
						if (config->debug) registerDebug("Attacked creature is dead");
						char buf[30];
						sprintf(buf,"%d,%d",time(NULL)+5,attackedCh->tibiaId);
						if((self->cap>config->capacityLimit&&(config->lootFood||config->lootGp||config->lootWorms||config->lootCustom))||config->eatFromCorpse)
							reader.setGlobalVariable("autolooterTm",buf);
					}
				}
			}
			*/
			// keep track of how long standing in same place for info to user
			if (lastStandingX!=self->x||lastStandingY!=self->y||lastStandingZ!=self->z){
				lastStandingX=self->x;
				lastStandingY=self->y;
				lastStandingZ=self->z;
				currentPosTM=time(NULL);
			}
			if (currentlyAttackedCreatureNr!=-1&&(self->cap>config->capacityLimit||config->eatFromCorpse)&&(config->lootFood||config->lootGp||config->lootWorms||config->lootCustom||config->eatFromCorpse)) {
				// now let's see whether creature is still alive or not
				if (attackedCh) {
					if (!attackedCh->hpPercLeft&&
						abs(attackedCh->x-self->x)+abs(attackedCh->y-self->y)<=10&&
						attackedCh->z==self->z) {

						if (config->debug) registerDebug("Looter: Creature is dead.");
						FILE *lootStatsFile = NULL;
						// time,rand,creature,name,pos,objectId,count,bagopen,checksum
						int killNr=rand();

						int lootContNr[2];
						lootContNr[0] = CModuleUtil::findNextClosedContainer();
						if (config->lootInBags)
							lootContNr[1] = CModuleUtil::findNextClosedContainer(lootContNr[0]);

						if (config->gatherLootStats) {
							lootStatsFile=fopen("tibiaauto-stats-loot.txt","a+");
						}
						
						// first make sure all needed containers are closed
						
						currentlyAttackedCreatureNr=-1;
						// attacked creature dead, near me, same floor
						globalAutoAttackStateLoot=CToolAutoAttackStateLoot_opening;

						CModuleUtil::waitForCreatureDisappear(attackedCh->nr);
						deleteAndNull(self);
						self = reader.readSelfCharacter();
						int corpseId = itemOnTopCode(attackedCh->x-self->x,attackedCh->y-self->y);
						if (1){
							// Open corpse to container, wait to get to corpse on ground and wait for open
							sender.openContainerFromFloor(corpseId,attackedCh->x,attackedCh->y,attackedCh->z,lootContNr[0]);

							if(!CModuleUtil::waitToApproachSquare(attackedCh->x,attackedCh->y)){
								//(waitToApproachSquare returns false) => (corpse >1 sqm away and not reached yet), so try again
								sender.openContainerFromFloor(corpseId,attackedCh->x,attackedCh->y,attackedCh->z,lootContNr[0]);
								CModuleUtil::waitToApproachSquare(attackedCh->x,attackedCh->y);
							}

							//Normal looting if autolooter not enabled
							if (CModuleUtil::waitForOpenContainer(lootContNr[0],true)) {
								//sender.sendTAMessage("[debug] opened container");
								if (config->debug) registerDebug("Looter: Opening dead creature corpse (first container)");
								if (config->lootInBags) {
									if (config->debug) registerDebug("Looter: Opening bag (second container)");
									CTibiaContainer *cont = reader.readContainer(lootContNr[0]);
									int itemNr;
									for (itemNr=0;itemNr<cont->itemsInside;itemNr++) {
										CTibiaItem *insideItem = (CTibiaItem *)cont->items.GetAt(itemNr);
										if (insideItem->objectId==itemProxy.getValueForConst("bagbrown")) {
											globalAutoAttackStateLoot=CToolAutoAttackStateLoot_openingBag;
											sender.openContainerFromContainer(insideItem->objectId,0x40+lootContNr[0],insideItem->pos,lootContNr[1]);
											if (!CModuleUtil::waitForOpenContainer(lootContNr[1],true))
												if (config->debug) registerDebug("Failed opening bag");
											break;
										}
									}
									deleteAndNull(cont);
								}
								if (lootStatsFile) {
									int checksum;
									int tm=time(NULL);
									for (int contNrInd=0; contNrInd <1+config->lootInBags;contNrInd++){// get stats from first, then last if lootInBags
										int contNr=lootContNr[contNrInd];
										//sender.sendTAMessage("[debug] stats from conts");
										CTibiaContainer *lootCont = reader.readContainer(contNr);
										if(lootCont->flagOnOff){
											if (config->debug) registerDebug("Recording Stats for Container Number",intstr(contNr).c_str());
											int itemNr;
											for (itemNr=0;itemNr<lootCont->itemsInside;itemNr++) {
												int i,len;
												char statChName[128];
												for (i=0,strcpy(statChName,attackedCh->name),len=strlen(statChName);i<len;i++) {
													if (statChName[i]=='[')
														statChName[i]='\0';
												}
												
												CTibiaItem *lootItem = (CTibiaItem *)lootCont->items.GetAt(itemNr);
												checksum = CModuleUtil::calcLootChecksum(tm,killNr,strlen(statChName),100+itemNr,lootItem->objectId,(lootItem->quantity?lootItem->quantity:1),config->lootInBags&&contNr==lootContNr[1],attackedCh->x,attackedCh->y,attackedCh->z);
												if (checksum<0) checksum*=-1;
												fprintf(lootStatsFile,"%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d\n",tm,killNr,statChName,100+itemNr,lootItem->objectId,lootItem->quantity?lootItem->quantity:1,config->lootInBags&&contNr==lootContNr[1],attackedCh->x,attackedCh->y,attackedCh->z,checksum);
											}
										}
										deleteAndNull(lootCont);
									}
									//free(lootCont);
									if (config->debug) registerDebug("Container data registered.");
								}
								
								CUIntArray acceptedItems;
								if (config->lootGp)
									acceptedItems.Add(itemProxy.getValueForConst("GP"));
									acceptedItems.Add(itemProxy.getValueForConst("PlatinumCoin"));
									acceptedItems.Add(itemProxy.getValueForConst("CrystalCoin"));
								if (config->lootWorms)
									acceptedItems.Add(itemProxy.getValueForConst("worms"));
								if (config->lootCustom) {
									int i;
									for (i=0;i<itemProxy.getItemsLootedCount();i++)
										acceptedItems.Add(itemProxy.getItemsLootedId(i));
								}
								if (config->lootFood) {
									int p;
									for (p=0;p<itemProxy.getItemsFoodArray()->GetSize();p++)
										acceptedItems.Add(itemProxy.getItemsFoodArray()->GetAt(p));
								}
								
								int lootTakeItem;
								for (int contNrInd=0; contNrInd <1+config->lootInBags;contNrInd++){// loot from first, then last if lootInBags
									int contNr=lootContNr[contNrInd];

									if (contNrInd==0) globalAutoAttackStateLoot=CToolAutoAttackStateLoot_moveing;
									else if (contNrInd==1) globalAutoAttackStateLoot=CToolAutoAttackStateLoot_moveingBag;
									CTibiaContainer *lootCont = reader.readContainer(contNr);
									//sender.sendTAMessage("[debug] looting");
									if(lootCont->flagOnOff){
										if (config->debug) registerDebug("Looting from container number",intstr(contNrInd).c_str());
										for (lootTakeItem=0;lootTakeItem<1;lootTakeItem++) {
											if (self->cap>config->capacityLimit) {
												CModuleUtil::lootItemFromContainer(contNr,&acceptedItems);
											}
											if (config->eatFromCorpse) {
												CModuleUtil::eatItemFromContainer(contNr);
											}
										}
										if (config->dropNotLooted) {
											dropAllItemsFromContainer(contNr,attackedCh->x,attackedCh->y,attackedCh->z);
										}
										
										globalAutoAttackStateLoot=CToolAutoAttackStateLoot_closingBag;
										//sender.sendTAMessage("[debug] closing bag");
										sender.closeContainer(contNr);
										CModuleUtil::waitForOpenContainer(contNr,false);
									}
									deleteAndNull(lootCont);
								}
								globalAutoAttackStateLoot=CToolAutoAttackStateLoot_notRunning;
								if (config->debug) registerDebug("End of looting");
							} // if waitForContainer(9)
							else{
								char buf[128];
								sprintf(buf,"Failed to open Item ID:%d",corpseId);
								if (config->debug) registerDebug(buf);
							}
							if (lootStatsFile)  {
								fclose(lootStatsFile);
							}
						}
						globalAutoAttackStateLoot=CToolAutoAttackStateLoot_notRunning;
						deleteAndNull(attackedCh);
						currentlyAttackedCreatureNr=-1;
						CTibiaCharacter *attackedCh = reader.readVisibleCreature(currentlyAttackedCreatureNr);
					}
					else {// if creatureIsNotDead
						char debugBuf[256];
						sprintf(debugBuf,"Attacked creature info: x dist=%d y dist=%d z dist=%d id=%d visible=%d hp=%d",abs(self->x-attackedCh->x),abs(self->y-attackedCh->y),abs(self->z-attackedCh->z),attackedCh->tibiaId,attackedCh->visible,attackedCh->hpPercLeft);
						if (config->debug) registerDebug(debugBuf);
					}
				} // if attackedCh
			}

			//only time currentlyAttackedCreatureNr can = -1 is if looter has run(fixes skipping monster)
			if (currentlyAttackedCreatureNr!=-1&&!creatureList[currentlyAttackedCreatureNr].isOnscreen)
				currentlyAttackedCreatureNr=-1;

			// if client thinks we are not attacking anything, then
			// send "attack new target" to it and increment failed attack for creatureNr by 1
			if (reader.getAttackedCreature()==0&&currentlyAttackedCreatureNr!=-1&&(!attackedCh||attackedCh->hpPercLeft)) {
				char buf[128];
				sprintf(buf,"Resetting attacked creature to %d,%d,%d [1]",currentlyAttackedCreatureNr,reader.getAttackedCreature(),attackedCh->hpPercLeft);
				if (config->debug) registerDebug(buf);

				creatureList[currentlyAttackedCreatureNr].failedAttacks++;
				AttackCreature(config,attackedCh->tibiaId);
				char debugBuf[256];
				sprintf(debugBuf,"Attacked creature info: x dist=%d y dist=%d z dist=%d id=%d visible=%d hp=%d",abs(self->x-attackedCh->x),abs(self->y-attackedCh->y),abs(self->z-attackedCh->z),attackedCh->tibiaId,attackedCh->visible,attackedCh->hpPercLeft);
				//AfxMessageBox(debugBuf);

			}
			else if (reader.getAttackedCreature()&&attackedCh&&reader.getAttackedCreature()!=attackedCh->tibiaId){
				//wis: keep player-chosen creature as attackee
				currentlyAttackedCreatureNr=reader.getCharacterByTibiaId(reader.getAttackedCreature())->nr;
				char buf[128];
				sprintf(buf,"Setting attacked creature to %d [1]",currentlyAttackedCreatureNr);
				if (config->debug) registerDebug(buf);
			}
			//if autofollow on, still not <=1 from creature and standing for > 2s refresh attack and add fail attack
			//wait .5 secs between failed attacks
			else if (currentlyAttackedCreatureNr!=-1&&config->autoFollow&&creatureList[currentlyAttackedCreatureNr].distance(self->x,self->y)>1&&time(NULL)-currentPosTM>(2.5+.5*creatureList[currentlyAttackedCreatureNr].failedAttacks)){
				if (creatureList[currentlyAttackedCreatureNr].tibiaId){
					creatureList[currentlyAttackedCreatureNr].failedAttacks++;
					sender.attack(creatureList[currentlyAttackedCreatureNr].tibiaId);}
			}

			deleteAndNull(attackedCh);

			// refresh information
			int currentTm=reader.getCurrentTm();
			int playersOnScreen=0;
			int monstersSurrounding=0;
			alienCreatureForTrainerFound=0;

			int crNr;
			for (crNr=0;crNr<memConstData.m_memMaxCreatures;crNr++) {
				CTibiaCharacter *ch=reader.readVisibleCreature(crNr);
				if (ch->visible==0 || abs(self->x-ch->x)>7 || abs(self->y-ch->y)>5 || ch->z!=self->z) { creatureList[crNr].isOnscreen=0;}
				else {
					/**
					* creature ID for creature number has changed,
					* so new creature is occupying the slot already
					*/
					if (creatureList[crNr].tibiaId!=ch->tibiaId&&currentlyAttackedCreatureNr!=crNr) {
						int keepAttackTm =creatureList[crNr].lastAttackTm;
						creatureList[crNr]= Creature();
						creatureList[crNr].lastAttackTm= keepAttackTm;
						creatureList[crNr].tibiaId=ch->tibiaId;

						int monstListNr;
						//Get name and take care of monsters numbered by TA's creature info
						int i,len;
						char statChName[128];
						for (i=0,strcpy(statChName,ch->name),len=strlen(statChName);i<len;i++) {
							if (statChName[i]=='[')
								statChName[i]='\0';
						}
						strcpy(creatureList[crNr].name,statChName);

						// scan creature list to find its priority
						for (monstListNr=0;monstListNr<config->monsterCount;monstListNr++) {
							if (!strcmpi(config->monsterList[monstListNr],creatureList[crNr].name))
								creatureList[crNr].listPriority=config->monsterCount-monstListNr;//1 is lowest, 0 means not in attack list
						}

						// scan ignore list
						for (monstListNr=0;monstListNr<config->ignoreCount;monstListNr++) {
							if (!strcmpi(config->ignoreList[monstListNr],creatureList[crNr].name)) {
								if (config->debug&&creatureList[crNr].isIgnoredUntil-time(NULL)<9999) registerDebug("Creature found on ignore list");
								creatureList[crNr].isIgnoredUntil=1555555555;//ignore forever
							}
						}

						//ignore self forever
						if (ch->tibiaId==self->tibiaId)creatureList[crNr].isIgnoredUntil=1555555555 ;//ignore forever
					}
					else if (crNr==currentlyAttackedCreatureNr){
						//if creature we are attacking has lost health, record time of bloodhit
						if (ch->hpPercLeft<creatureList[crNr].hpPercLeft)
							lastAttackedCreatureBloodHit=time(NULL);
						//determine if creature reached to ignore creature if not reached in time
						if(!reachedAttackedCreature && (time(NULL)-lastAttackedCreatureBloodHit<1 || creatureList[crNr].distance(self->x,self->y)<=1))
							reachedAttackedCreature=1;
						//if attacked creature has not been reached after given time, ignore creature
						if (!reachedAttackedCreature && time(NULL)-firstCreatureAttackTM>config->unreachableAfter){
							creatureList[crNr].isIgnoredUntil=time(NULL)+config->suspendAfterUnreachable;
						}
					}
					// Attack time has changed. We interpret this by saying that ch->tibiaId is attacking us
					// even if ID changed, since 'lastAttackTm' is NOT updated when Tibia reuses slot.
					if (creatureList[crNr].lastAttackTm!=ch->lastAttackTm && currentTm-ch->lastAttackTm<attackConsiderPeriod) {
						creatureList[crNr].isAttacking=1;
						char buf[1024];
						sprintf(buf,"lastAttackTm change for %d: was %d/%d is %d/%d",creatureList[crNr].tibiaId,creatureList[crNr].lastAttackTm,ch->tibiaId,ch->lastAttackTm);
						if (config->debug) registerDebug(buf);
					}
					creatureList[crNr].lastAttackTm= ch->lastAttackTm;
					creatureList[crNr].x=ch->x;
					creatureList[crNr].y=ch->y;
					creatureList[crNr].z=ch->z;
					creatureList[crNr].isOnscreen=1;
					creatureList[crNr].isDead=(ch->hpPercLeft==0);
					creatureList[crNr].hpPercLeft=ch->hpPercLeft;
					creatureList[crNr].isIgnoredUntil=(creatureList[crNr].isIgnoredUntil<time(NULL))?0:creatureList[crNr].isIgnoredUntil;
					//outfit type&& head color==0 <=> creature is invisible
					creatureList[crNr].isInvisible = (ch->monsterType==0&&ch->colorHead==0);

					//ignore creatures with >=3 failed attacks for 30 secs
					if (creatureList[crNr].failedAttacks>=3){
						creatureList[crNr].isIgnoredUntil=time(NULL)+30;
						creatureList[crNr].failedAttacks=0;
					} //else if creature attacking us from 1 away but ignored for <5 mins, unignore
					else if (creatureList[crNr].isIgnoredUntil-time(NULL)<9999&&!creatureList[crNr].isInvisible&&(creatureList[crNr].distance(self->x,self->y)<=1&&currentlyAttackedCreatureNr!=-1)){
						creatureList[crNr].isIgnoredUntil=0;
					}
					
					//update environmental variables
					if (creatureList[crNr].isAttacking && taxiDist(self->x,self->y,creatureList[crNr].x,creatureList[crNr].y)<=1) monstersSurrounding++;
					if (crNr!=self->nr && creatureList[crNr].tibiaId<0x40000000 && creatureList[crNr].isOnscreen) playersOnScreen++;
					if (crNr!=self->nr && !creatureList[crNr].listPriority && creatureList[crNr].isOnscreen) alienCreatureForTrainerFound=1;

					// if sharing alien backattack is active, then we may attack
					// creatures which are not directly attacking us
					if (!creatureList[crNr].isAttacking&&shareAlienBackattack) {
						int count=sh_mem.GetVariablesCount();
						int pos;
						for (pos=0;pos<count;pos++) {
							ValueHeader valueHeader;
							sh_mem.GetValueInfo(pos,&valueHeader);
							if (strcmp(varName,(char *)valueHeader.wszValueName)) {
								int attackTibiaId=0;
								unsigned long int len=4;
								// avoid looping with out own attack target
								sh_mem.GetValue((char *)valueHeader.wszValueName,&attackTibiaId,&len);

								if (attackTibiaId==ch->tibiaId) {
									if (config->debug) registerDebug("Setting isAttacking because of shm info");
									creatureList[crNr].isAttacking=1;
								}
							}
						}
					}
				}// if visible
				deleteAndNull(ch);
				if (config->debug && modRuns%10==0 && creatureList[crNr].isOnscreen) {
					char buf[128];
					sprintf(buf,"%s, nr=%d, isatta=%d, isonscreen=%d, atktm=%d ID=%d ignore=%d,x=%d,y=%d,z=%d",crNr!=self->nr?creatureList[crNr].name:"****",crNr,creatureList[crNr].isAttacking,creatureList[crNr].isOnscreen,creatureList[crNr].lastAttackTm,creatureList[crNr].tibiaId,creatureList[crNr].isIgnoredUntil?creatureList[crNr].isIgnoredUntil-time(NULL):0,creatureList[crNr].x,creatureList[crNr].y,creatureList[crNr].z);
					registerDebug(buf);
				}

			}//end for
			
			//Determine best creature to attack on-screen
			int bestCreatureNr=-1;
			for (crNr=0;crNr<memConstData.m_memMaxCreatures;crNr++) {
				//If cannot attack, don't
				if (!creatureList[crNr].isOnscreen || creatureList[crNr].isInvisible || creatureList[crNr].isDead || crNr==self->nr) continue;
				//If shouldn't attack, don't
				if (creatureList[crNr].hpPercLeft < config->attackHpAbove) {
					if (config->debug && modRuns%10==0) registerDebug("Quit Case:hp above value");
					continue;}
				if (config->dontAttackPlayers && creatureList[crNr].tibiaId<0x40000000) {
					if (config->debug && modRuns%10==0) registerDebug("Quit Case:No attack player");
					continue;}
				if (config->attackOnlyAttacking && !creatureList[crNr].isAttacking){ 
					if (config->debug && modRuns%10==0) registerDebug("Quit Case:only attack attacking"); 
					continue;}
				if ((!creatureList[crNr].isAttacking || creatureList[crNr].hpPercLeft==100) && maxDist(self->x,self->y,creatureList[crNr].x,creatureList[crNr].y)>config->attackRange){	
					if (config->debug && modRuns%10==0) registerDebug("Quit Case:out of range");
					continue;}
				if (creatureList[crNr].isIgnoredUntil){
					if (config->debug && modRuns%10==0) registerDebug("Quit Case:is ignored");
					continue;}
				if (config->suspendOnEnemy && playersOnScreen && !creatureList[crNr].isAttacking){
					if (config->debug) registerDebug("Quit Case:Suspend on enemy");
					continue;}//only attack creatures attacking me if suspendOnEnemy and player on screen
				if (!(creatureList[crNr].listPriority || config->forceAttackAfterAttack && creatureList[crNr].isAttacking)){
					if (config->debug && modRuns%10==0) registerDebug("Quit Case:Options do not allow attack");
					continue;}

				//if shareBackattackAlien, add possible attack option to list if not on priority list
				if (!creatureList[crNr].listPriority && shareAlienBackattack){
					// if we are backattacking and shareing backattack
					// with other instances is active - then register it
					// in the shm
					sh_mem.SetValue(varName,&creatureList[crNr].tibiaId,4);
				}

				//if bestCreatureNr is not picked yet switch to first reasonable option
				if(bestCreatureNr==-1) bestCreatureNr=crNr;
				int isCloser = taxiDist(self->x,self->y,creatureList[crNr].x,creatureList[crNr].y)<taxiDist(self->x,self->y,creatureList[bestCreatureNr].x,creatureList[bestCreatureNr].y);
				int isFarther = taxiDist(self->x,self->y,creatureList[crNr].x,creatureList[crNr].y)>taxiDist(self->x,self->y,creatureList[bestCreatureNr].x,creatureList[bestCreatureNr].y);
				if(isCloser){
					if (creatureList[bestCreatureNr].isAttacking &&
						(creatureList[bestCreatureNr].listPriority>creatureList[crNr].listPriority ||
						creatureList[bestCreatureNr].listPriority==0 && creatureList[crNr].listPriority!=0)&&
						canGetToPoint(creatureList[bestCreatureNr].x,creatureList[bestCreatureNr].y,creatureList[bestCreatureNr].z)){
						//distance creature has higher priority and can be reached
					} else{
						if (config->debug) registerDebug("Attacker: Better because closer.");
						bestCreatureNr=crNr;}
				}else if (!isFarther){
					//If force attack prioritize creature not on list with lowest health
					if ((config->forceAttackAfterAttack && creatureList[crNr].listPriority==0 && !isFarther && creatureList[crNr].isAttacking) && 
						(creatureList[crNr].hpPercLeft<creatureList[bestCreatureNr].hpPercLeft || creatureList[bestCreatureNr].listPriority!=0)){
						if (config->debug) registerDebug("Attacker: Better because alien creature.");
						bestCreatureNr=crNr;}
					//If attack only if attacked prioritize creature not on list or creatures attacking us with highest priority
					if ((!config->attackOnlyAttacking || creatureList[crNr].isAttacking) && creatureList[crNr].listPriority!=0  &&
						(creatureList[crNr].listPriority>creatureList[bestCreatureNr].listPriority && creatureList[bestCreatureNr].listPriority!=0)){
						if (config->debug) registerDebug("Attacker: Better because higher priority");
						bestCreatureNr=crNr;}
				} else if (isFarther){
					if (creatureList[crNr].isAttacking &&
						(creatureList[crNr].listPriority>creatureList[bestCreatureNr].listPriority ||
						creatureList[crNr].listPriority==0 && creatureList[bestCreatureNr].listPriority!=0)&&
						canGetToPoint(creatureList[crNr].x,creatureList[crNr].y,creatureList[crNr].z)) {
						if (config->debug) registerDebug("Attacker: Better because attacking and higher priority.");
						bestCreatureNr=crNr;
					}	
				}

/*
				if (monstersSurrounding<=2){
					if (!isFarther&&creatureList[crNr].listPriority>=creatureList[bestCreatureNr].listPriority){
						if (config->debug) registerDebug("Better:Closer");
						bestCreatureNr=crNr;
					}
				}else{
					//If force attack prioritize creature not on list with lowest health
					if ((config->forceAttackAfterAttack && creatureList[crNr].listPriority==0 && !isFarther && creatureList[crNr].isAttacking) && 
						(creatureList[crNr].hpPercLeft<creatureList[bestCreatureNr].hpPercLeft || creatureList[bestCreatureNr].listPriority!=0)){
						if (config->debug) registerDebug("Better:alien creature");
						bestCreatureNr=crNr;}
					//If attack only if attacked prioritize creature not on list or creatures attacking us with highest priority
					if ((!config->attackOnlyAttacking || creatureList[crNr].isAttacking) && creatureList[crNr].listPriority!=0  &&
						(creatureList[crNr].listPriority>creatureList[bestCreatureNr].listPriority && creatureList[bestCreatureNr].listPriority!=0)){
						if (config->debug) registerDebug("Better:Higher Priority");
						bestCreatureNr=crNr;}
				}
*/
				char buf[256];
				sprintf(buf,"Nr=%d, name=%s\t isAttacking=%d, Prio=%d, fail#=%d, hp%%=%d, Id=%d, Onscreen=%d, Invis=%d, Dead=%d, lastAttackTm=%d, IgnoredUntil=%d, shouldAttack=%d, dist=%d",crNr,creatureList[crNr].name,creatureList[crNr].isAttacking,creatureList[crNr].listPriority,creatureList[crNr].failedAttacks,creatureList[crNr].hpPercLeft,creatureList[crNr].tibiaId,creatureList[crNr].isOnscreen,creatureList[crNr].isInvisible,creatureList[crNr].isDead,creatureList[crNr].lastAttackTm,creatureList[crNr].isIgnoredUntil,crNr==bestCreatureNr,taxiDist(self->x,self->y,creatureList[crNr].x,creatureList[crNr].y));
				if (config->debug) registerDebug(buf);
			}
			
			//Determine whether currently attacked creature is better than bestCreature
			if (bestCreatureNr!=-1){
				//If cannot attack currentlyAttackedCreature, switch to bestCreature
				if (currentlyAttackedCreatureNr==-1
					||!creatureList[currentlyAttackedCreatureNr].isOnscreen
					|| creatureList[currentlyAttackedCreatureNr].isInvisible
					|| creatureList[currentlyAttackedCreatureNr].isDead
					|| creatureList[currentlyAttackedCreatureNr].hpPercLeft<config->attackHpAbove
					|| config->dontAttackPlayers && creatureList[currentlyAttackedCreatureNr].tibiaId<0x40000000
					|| config->attackOnlyAttacking && !creatureList[currentlyAttackedCreatureNr].isAttacking
					|| maxDist(self->x,self->y,creatureList[currentlyAttackedCreatureNr].x,creatureList[currentlyAttackedCreatureNr].y)>config->attackRange && creatureList[currentlyAttackedCreatureNr].hpPercLeft>15 // only switch when the monstr is out of range if they don't seem to be inconveniently 'running in low health'
					|| creatureList[currentlyAttackedCreatureNr].isIgnoredUntil
					|| config->suspendOnEnemy && playersOnScreen && creatureList[currentlyAttackedCreatureNr].isAttacking==0
					||!(creatureList[currentlyAttackedCreatureNr].listPriority || config->forceAttackAfterAttack && creatureList[currentlyAttackedCreatureNr].isAttacking)){
					currentlyAttackedCreatureNr=bestCreatureNr;
				}
				//if no harm in switching and monster is not already in low health and running, switch
				else if ((!reachedAttackedCreature || !config->stickToMonster)
                     &&(!config->autoFollow || creatureList[currentlyAttackedCreatureNr].hpPercLeft>15)
                     &&creatureList[bestCreatureNr].listPriority!=creatureList[currentlyAttackedCreatureNr].listPriority){
					currentlyAttackedCreatureNr=bestCreatureNr;
					if (config->debug)  {
						char buf[256];
						sprintf(buf,"Switching to Nr=%d name=%s point=%d,%d,%d id=%d",bestCreatureNr,creatureList[bestCreatureNr].name,creatureList[bestCreatureNr].x,creatureList[bestCreatureNr].y,creatureList[bestCreatureNr].z,creatureList[bestCreatureNr].tibiaId);
						registerDebug(buf);
					}
				}
			}
			else if(creatureList[currentlyAttackedCreatureNr].hpPercLeft!=0) currentlyAttackedCreatureNr=-1;

			if (config->debug) registerDebug("Entering attack execution area");

			/**
			* Do the training control things.
			*/
			int attackMode=config->mode+1;
			trainingCheck(config,currentlyAttackedCreatureNr,alienCreatureForTrainerFound,monstersSurrounding,lastAttackedCreatureBloodHit,&attackMode);
			SendAttackMode(attackMode,config->autoFollow);
 
			//perform server visible tasks if we have something to attack
			if (currentlyAttackedCreatureNr!=-1&&creatureList[currentlyAttackedCreatureNr].hpPercLeft){
				if (AttackCreature(config,creatureList[currentlyAttackedCreatureNr].tibiaId)){
					firstCreatureAttackTM=time(NULL);
					reachedAttackedCreature=0;
				}

				// if we are backattacking alien then maybe fire runes at them
				// note: fire runes only if my own hp is over 50%
				// to avoid uh-exhaust
				if (!creatureList[currentlyAttackedCreatureNr].listPriority&&config->backattackRunes&&self->hp>self->maxHp/2) {
					if (config->debug) registerDebug("Firing SD at enemy");
					fireRunesAgainstCreature(config,creatureList[currentlyAttackedCreatureNr].tibiaId);
				}
				globalAutoAttackStateWalker=CToolAutoAttackStateWalker_notRunning;
				globalAutoAttackStateAttack=CToolAutoAttackStateAttack_attackingCreature;
				if (config->debug)  {
					char buf[256];
					sprintf(buf,"Attacking Nr=%d name=%s point=%d,%d,%d id=%d ignore=%d",bestCreatureNr,creatureList[bestCreatureNr].name,creatureList[bestCreatureNr].x,creatureList[bestCreatureNr].y,creatureList[bestCreatureNr].z,creatureList[bestCreatureNr].tibiaId,creatureList[bestCreatureNr].isIgnoredUntil);
					registerDebug(buf);
				}
			}else{
				AttackCreature(config,0);
				globalAutoAttackStateAttack=CToolAutoAttackStateAttack_notRunning;
				if (config->debug) registerDebug("No attack target found");
			}
			/*****
			End attack process
			******/
		}

		/*****
		Start Walking Process
		******/

		moving = reader.getMemIntValue(memConstData.m_memAddressTilesToGo);
		if (currentlyAttackedCreatureNr==-1 && !moving){//wis:make sure doesn;t start while looting last monster
			if (self->x==depotX&&self->y==depotY&&self->z==depotZ) {
				if (config->debug) registerDebug("Depot reached");
				// depot waypoint reached!
				// then do the depot stuff
				depotDeposit(config);
			}
			else if (abs(targetX-self->x)<=1&&abs(targetY-self->y)<=1&&targetZ==self->z) {
				if (config->debug) registerDebug("Waypoint reached");
				// normal waypoint reached
				targetX=targetY=targetZ=0;
				walkerStandingEndTm=time(NULL)+config->standStill;
			}

			//if Standing time not ended yet
			if (time(NULL)<walkerStandingEndTm) {
				globalAutoAttackStateWalker=CToolAutoAttackStateWalker_standing;
				if (config->debug) registerDebug("Standing");				
				deleteAndNull(self);
				continue;
			}

			// no target found - go somewhere
			if (!targetX&&!targetY&&!targetZ) {

				if (config->debug) registerDebug("Getting location for walker.");

				if (depotX&&depotY&&depotZ) {
					// if we are walking to a depot - then just go there
					targetX=depotX;
					targetY=depotY;
					targetZ=depotZ;
					if (config->debug) registerDebug("Continuing to a depot");
				}
				//if waypoint recently reached OR no path found for 4 secs OR stuck for 10 secs find new waypoint
				else if (walkerStandingEndTm||globalAutoAttackStateWalker==CToolAutoAttackStateWalker_noPathFound&&time(NULL)-currentPosTM>4||time(NULL)-currentPosTM>10) {
					walkerStandingEndTm=0;
					// find a new goto target from waypoint list
					if (!waypointsCount) {
						globalAutoAttackStateWalker=CToolAutoAttackStateWalker_noWaypoints;
						deleteAndNull(self);
						continue;
					}
					int nextWaypoint = 0;
					switch (config->waypointSelectMode) {
					case 0: // random
						//also exludes previous waypoint from being selected again
						if (waypointsCount!=1){
							nextWaypoint=waypointsCount?(rand()%(waypointsCount-1)):0;
							nextWaypoint+= (int)(nextWaypoint>=currentWaypointNr);
						}
						currentWaypointNr=nextWaypoint;
						break;
					case 1: // round-robin
						currentWaypointNr++;
						if (currentWaypointNr==waypointsCount)
							currentWaypointNr=0;
						break;
					}
				}//else:continue to previous waypoint

				targetX=config->waypointList[currentWaypointNr].x;
				targetY=config->waypointList[currentWaypointNr].y;
				targetZ=config->waypointList[currentWaypointNr].z;
				char buf[128];
				sprintf(buf,"Walking to waypoint (%d,%d,%d)",targetX,targetY,targetZ);
				if (config->debug) registerDebug(buf);
			}
			if (targetX&&targetY&&!isInHalfSleep()) {
				int mapUsed=config->mapUsed;
				if (targetZ!=self->z&&config->mapUsed==2)
					config->mapUsed=0;
				switch (mapUsed) {
					// Tibia Auto map chosen
				case 0: {
					char buf[128];
					int path[15];					
					
					deleteAndNull(self);
					self = reader.readSelfCharacter();
					
					// proceed with path searching
					sprintf(buf,"findPathOnMap: standard walk (%d,%d,%d)->(%d,%d,%d)",self->x,self->y,self->z,targetX,targetY,targetZ);
					if (config->debug) registerDebug(buf);

					int ticksStart = GetTickCount();
					CModuleUtil::findPathOnMap(self->x,self->y,self->z,targetX,targetY,targetZ,0,path);
					int ticksEnd = GetTickCount();

					sprintf(buf,"timing: findPathOnMap() = %dms",ticksEnd-ticksStart);
					if (config->debug) registerDebug(buf);

					int pathSize;
					for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){}
					if (pathSize||self->x==targetX&&targetY==self->y&&self->z==targetZ) {							
						CModuleUtil::executeWalk(self->x,self->y,self->z,path);
						globalAutoAttackStateWalker=CToolAutoAttackStateWalker_ok;
						if (config->debug) registerDebug("Walking: execute walk");
					}
					else {
						char buf[128];
						sprintf(buf,"Walking: no path found to (%d,%d,%d)",targetX,targetY,targetZ);
						if (config->debug) registerDebug(buf);

						// if no path found - then we forget current target
						targetX=targetY=targetZ=0;
						globalAutoAttackStateWalker=CToolAutoAttackStateWalker_noPathFound;
					}
						} // case 0
					break;
					// Tibia client map chosen
				case 1: {
					if (self->z==targetZ) {
						globalAutoAttackStateWalker=CToolAutoAttackStateWalker_ok;
						reader.writeGotoCoords(targetX,targetY,targetZ);
					}
					else {
						if (config->debug) registerDebug("Can't change levels when using tibia client map :(");
						globalAutoAttackStateWalker=CToolAutoAttackStateWalker_noPathFound;
					}
						} // case 1
					break;
				}
			}
		} // if (no currentlyAttackedCreatureNr)

		deleteAndNull(self);

	} // while
	

	if (shareAlienBackattack) {
		int zero=0;
		sh_mem.SetValue(varName,&zero,4);
	}
	// cancel attacks
	//sender.stopAll();
	sender.attack(0);
	reader.setAttackedCreature(0);
	reader.cancelAttackCoords();
	CTibiaCharacter *self = reader.readSelfCharacter();
	reader.writeCreatureDeltaXY(self->nr,0,0);
	deleteAndNull(self);
	// restore attack mode
	SendAttackMode(reader.getPlayerModeAttackType(),reader.getPlayerModeFollow());

	//reader.setRemainingTilesToGo(0);
	if (config->debug) registerDebug("Exiting cavebot");
	globalAutoAttackStateAttack=CToolAutoAttackStateAttack_notRunning;
	globalAutoAttackStateLoot=CToolAutoAttackStateLoot_notRunning;
	globalAutoAttackStateWalker=CToolAutoAttackStateWalker_notRunning;
	globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notRunning;
	globalAutoAttackStateTraining=CToolAutoAttackStateTraining_notRunning;
	toolThreadShouldStop=0;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMod_cavebotApp construction

CMod_cavebotApp::CMod_cavebotApp() {
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();
}

CMod_cavebotApp::~CMod_cavebotApp() {
	if (m_configDialog) {
		deleteAndNull(m_configDialog);
	}
	deleteAndNull(m_configData);
}

char * CMod_cavebotApp::getName() {
	return "Cave bot";
}

int CMod_cavebotApp::isStarted() {
	return m_started;
}

void CMod_cavebotApp::start() {
	superStart();
	if (m_configDialog) {
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}
	
	if (m_configData->gatherLootStats) {
		FILE *f=fopen("tibiaauto-stats-loot.txt","r");
		
		if (f)
		{
			fseek(f,0,SEEK_END);
			
			int flen=ftell(f);
			fclose(f);
			if (flen>1024*100) {
				CSendStats info;
				info.DoModal();
			}
		}
	}
	
	DWORD threadId;
	
	toolThreadShouldStop=0;
	toolThreadHandle = ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);
	m_started=1;
}

void CMod_cavebotApp::stop() {
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	}
	m_started=0;
	
	if (m_configDialog) {
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
} 

void CMod_cavebotApp::showConfigDialog() {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!m_configDialog) {
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

void CMod_cavebotApp::configToControls() {
	if (m_configDialog) {
		m_configDialog->configToControls(m_configData);
	}
}

void CMod_cavebotApp::controlsToConfig() {
	if (m_configDialog) {
		deleteAndNull(m_configData);
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_cavebotApp::disableControls() {
	if (m_configDialog) {
		m_configDialog->disableControls();
	}
}

void CMod_cavebotApp::enableControls() {
	if (m_configDialog) {
		m_configDialog->enableControls();
	}
}

char *CMod_cavebotApp::getVersion() {
	return "2.14";
}

int CMod_cavebotApp::validateConfig(int showAlerts) {
	if (m_configData->capacityLimit<0) {
		if (showAlerts) AfxMessageBox("Capacity limit must be >=0!");
		return 0;
	}
	if (m_configData->attackRange<0||m_configData->attackRange>8) {
		if (showAlerts) AfxMessageBox("Attack range must be between 0 and 8!");
		return 0;
	}
	if (m_configData->suspendAfterUnreachable<0) {
		if (showAlerts) AfxMessageBox("'suspend after unreachable' must be >=0!");
		return 0;
	}
	if (m_configData->unreachableAfter<0) {
		if (showAlerts) AfxMessageBox("'unreachable after' must be >= 0!");
		return 0;
	}
	if (m_configData->standStill<0) {
		if (showAlerts) AfxMessageBox("Stand still must be >=0!");
		return 0;
	}
	if (m_configData->attackHpAbove<0||m_configData->attackHpAbove>100) {
		if (showAlerts) AfxMessageBox("Attack hp above % must be between 0 and 100!");
		return 0;
	}
	if (m_configData->depotCap<0) {
		if (showAlerts) AfxMessageBox("Depot capacity must be >= 0!");
		return 0;
	}
	
	return 1;
}

void CMod_cavebotApp::resetConfig() {
	m_configData = new CConfigData();
}

void CMod_cavebotApp::loadConfigParam(char *paramName,char *paramValue) {
	if (!strcmp(paramName,"attack/follow")) m_configData->autoFollow=atoi(paramValue);
	if (!strcmp(paramName,"attack/mode")) m_configData->mode=atoi(paramValue);
	if (!strcmp(paramName,"attack/suspendOnEnemey")) m_configData->suspendOnEnemy=atoi(paramValue);
	if (!strcmp(paramName,"attack/suspendOnNoMove")) m_configData->suspendOnNoMove=atoi(paramValue);
	if (!strcmp(paramName,"attack/range")) m_configData->attackRange=atoi(paramValue);
	if (!strcmp(paramName,"attack/stick")) m_configData->stickToMonster=atoi(paramValue);
	if (!strcmp(paramName,"attack/unreachableAfter")) m_configData->unreachableAfter=atoi(paramValue);
	if (!strcmp(paramName,"attack/suspendAfterUnreachable")) m_configData->suspendAfterUnreachable=atoi(paramValue);
	if (!strcmp(paramName,"attack/attackOnlyAttacking")) m_configData->attackOnlyAttacking=atoi(paramValue);
	if (!strcmp(paramName,"attack/forceAttackAfterAttack")) m_configData->forceAttackAfterAttack=atoi(paramValue);
	if (!strcmp(paramName,"attack/hpAbove")) m_configData->attackHpAbove=atoi(paramValue);
	if (!strcmp(paramName,"attack/backattackRunes")) m_configData->backattackRunes=atoi(paramValue);
	if (!strcmp(paramName,"attack/shareAlienBackattack")) m_configData->shareAlienBackattack=atoi(paramValue);
	
	if (!strcmp(paramName,"loot/item/gp")) m_configData->lootGp=atoi(paramValue);
	if (!strcmp(paramName,"loot/item/custom")) m_configData->lootCustom=atoi(paramValue);
	if (!strcmp(paramName,"loot/item/worm")) m_configData->lootWorms=atoi(paramValue);
	if (!strcmp(paramName,"loot/item/food")) m_configData->lootFood=atoi(paramValue);
	if (!strcmp(paramName,"loot/other/inbag")) m_configData->lootInBags=atoi(paramValue);
	if (!strcmp(paramName,"loot/other/capacityLimit")) m_configData->capacityLimit=atoi(paramValue);
	if (!strcmp(paramName,"loot/other/eatFromCorpse")) m_configData->eatFromCorpse=atoi(paramValue);
	if (!strcmp(paramName,"loot/other/dropNotLooted")) m_configData->dropNotLooted=atoi(paramValue);
	if (!strcmp(paramName,"loot/other/lootFromFloor")) m_configData->lootFromFloor=atoi(paramValue);
	
	if (!strcmp(paramName,"training/weaponTrain")) m_configData->weaponTrain=atoi(paramValue);
	if (!strcmp(paramName,"training/weaponFight")) m_configData->weaponFight=atoi(paramValue);
	if (!strcmp(paramName,"training/weaponHand")) m_configData->weaponHand=atoi(paramValue);
	if (!strcmp(paramName,"training/trainingMode")) m_configData->trainingMode=atoi(paramValue);
	if (!strcmp(paramName,"training/fightWhenSurrounded")) m_configData->fightWhenSurrounded=atoi(paramValue);
	if (!strcmp(paramName,"training/fightWhenAlien")) m_configData->fightWhenAlien=atoi(paramValue);
	if (!strcmp(paramName,"training/bloodHit")) m_configData->bloodHit=atoi(paramValue);
	if (!strcmp(paramName,"training/activate")) m_configData->trainingActivate=atoi(paramValue);
	if (!strcmp(paramName,"depot/depotDropInsteadOfDepositon")) m_configData->depotDropInsteadOfDepositon=atoi(paramValue);
	if (!strcmp(paramName,"depot/depotCap")) m_configData->depotCap=atoi(paramValue);
	
	if (!strcmp(paramName,"general/debug")) m_configData->debug=atoi(paramValue);
	
	// stats should be always gathered
	//if (!strcmp(paramName,"loot/stats/gather")) m_configData->gatherLootStats=atoi(paramValue);
	
	if (!strcmp(paramName,"walker/other/selectMode")) m_configData->waypointSelectMode=atoi(paramValue);
	if (!strcmp(paramName,"walker/other/mapUsed")) m_configData->mapUsed=atoi(paramValue);
	if (!strcmp(paramName,"walker/other/standAfterWaypointReached")) m_configData->standStill=atoi(paramValue);
	
	if (!strcmp(paramName,"walker/waypoint")) {
		if (m_currentWaypointNr==0) {
			int i;
			for (i=0;i<1000;i++) {
				m_configData->waypointList[i].x=0;
			}
		}
		sscanf(paramValue,"%d,%d,%d",
			&m_configData->waypointList[m_currentWaypointNr].x,
			&m_configData->waypointList[m_currentWaypointNr].y,
			&m_configData->waypointList[m_currentWaypointNr].z);
		m_currentWaypointNr++;
	}
	if (!strcmp(paramName,"attack/monster")) {
		if (m_currentMonsterNr==0) {
			m_configData->monsterCount=0;
		}
		strcpy(m_configData->monsterList[m_currentMonsterNr],paramValue);
		m_configData->monsterCount++;
		m_currentMonsterNr++;
	}
	if (!strcmp(paramName,"attack/ignore")) {
		if (m_currentIgnoreNr==0) {
			m_configData->ignoreCount=0;
		}
		strcpy(m_configData->ignoreList[m_currentIgnoreNr],paramValue);
		m_configData->ignoreCount++;
		m_currentIgnoreNr++;
	}
	if (!strcmp(paramName,"depot/entry")) {
		if (m_currentDepotEntryNr==0) {
			int i;
			for (i=0;i<100;i++)
				m_configData->depotTrigger[i].itemName[0]='\0';
		}
		sscanf(paramValue,"%d,%d,%[^#]",&m_configData->depotTrigger[m_currentDepotEntryNr].when,&m_configData->depotTrigger[m_currentDepotEntryNr].remain,m_configData->depotTrigger[m_currentDepotEntryNr].itemName);
		m_currentDepotEntryNr++;
	}
}

char *CMod_cavebotApp::saveConfigParam(char *paramName) {
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"attack/follow")) sprintf(buf,"%d",m_configData->autoFollow);
	if (!strcmp(paramName,"attack/mode")) sprintf(buf,"%d",m_configData->mode);
	if (!strcmp(paramName,"attack/suspendOnEnemey")) sprintf(buf,"%d",m_configData->suspendOnEnemy);
	if (!strcmp(paramName,"attack/suspendOnNoMove")) sprintf(buf,"%d",m_configData->suspendOnNoMove);
	if (!strcmp(paramName,"attack/range")) sprintf(buf,"%d",m_configData->attackRange);
	if (!strcmp(paramName,"attack/stick")) sprintf(buf,"%d",m_configData->stickToMonster);
	if (!strcmp(paramName,"attack/unreachableAfter")) sprintf(buf,"%d",m_configData->unreachableAfter);
	if (!strcmp(paramName,"attack/suspendAfterUnreachable")) sprintf(buf,"%d",m_configData->suspendAfterUnreachable);
	if (!strcmp(paramName,"attack/attackOnlyAttacking")) sprintf(buf,"%d",m_configData->attackOnlyAttacking);
	if (!strcmp(paramName,"attack/forceAttackAfterAttack")) sprintf(buf,"%d",m_configData->forceAttackAfterAttack);
	if (!strcmp(paramName,"attack/hpAbove")) sprintf(buf,"%d",m_configData->attackHpAbove);
	if (!strcmp(paramName,"attack/backattackRunes")) sprintf(buf,"%d",m_configData->backattackRunes);
	if (!strcmp(paramName,"attack/shareAlienBackattack")) sprintf(buf,"%d",m_configData->shareAlienBackattack);
	
	if (!strcmp(paramName,"loot/item/gp")) sprintf(buf,"%d",m_configData->lootGp);
	if (!strcmp(paramName,"loot/item/custom")) sprintf(buf,"%d",m_configData->lootCustom);
	if (!strcmp(paramName,"loot/item/worm")) sprintf(buf,"%d",m_configData->lootWorms);
	if (!strcmp(paramName,"loot/item/food")) sprintf(buf,"%d",m_configData->lootFood);
	if (!strcmp(paramName,"loot/other/inbag")) sprintf(buf,"%d",m_configData->lootInBags);
	if (!strcmp(paramName,"loot/other/capacityLimit")) sprintf(buf,"%d",m_configData->capacityLimit);
	if (!strcmp(paramName,"loot/other/eatFromCorpse")) sprintf(buf,"%d",m_configData->eatFromCorpse);
	if (!strcmp(paramName,"loot/stats/gather")) sprintf(buf,"%d",m_configData->gatherLootStats);
	if (!strcmp(paramName,"loot/other/dropNotLooted")) sprintf(buf,"%d",m_configData->dropNotLooted);
	if (!strcmp(paramName,"loot/other/lootFromFloor")) sprintf(buf,"%d",m_configData->lootFromFloor);
	
	if (!strcmp(paramName,"walker/other/selectMode")) sprintf(buf,"%d",m_configData->waypointSelectMode);
	if (!strcmp(paramName,"walker/other/mapUsed")) sprintf(buf,"%d",m_configData->mapUsed);
	if (!strcmp(paramName,"walker/other/standAfterWaypointReached")) sprintf(buf,"%d",m_configData->standStill);
	
	if (!strcmp(paramName,"general/debug")) sprintf(buf,"%d",m_configData->debug);
	
	if (!strcmp(paramName,"attack/monster")&&m_currentMonsterNr<m_configData->monsterCount) {
		strcpy(buf,m_configData->monsterList[m_currentMonsterNr]);
		m_currentMonsterNr++;
	}
	if (!strcmp(paramName,"attack/ignore")&&m_currentIgnoreNr<m_configData->ignoreCount) {
		strcpy(buf,m_configData->ignoreList[m_currentIgnoreNr]);
		m_currentIgnoreNr++;
	}
	if (!strcmp(paramName,"walker/waypoint")&&m_configData->waypointList[m_currentWaypointNr].x) {
		sprintf(buf,"%d,%d,%d",m_configData->waypointList[m_currentWaypointNr].x,m_configData->waypointList[m_currentWaypointNr].y,m_configData->waypointList[m_currentWaypointNr].z);
		m_currentWaypointNr++;
	}
	if (!strcmp(paramName,"depot/entry")&&strlen(m_configData->depotTrigger[m_currentDepotEntryNr].itemName)&&m_currentDepotEntryNr<100) {
		sprintf(buf,"%d,%d,%s",m_configData->depotTrigger[m_currentDepotEntryNr].when,m_configData->depotTrigger[m_currentDepotEntryNr].remain,m_configData->depotTrigger[m_currentDepotEntryNr].itemName);
		m_currentDepotEntryNr++;
	}
	
	if (!strcmp(paramName,"training/weaponTrain")) sprintf(buf,"%d",m_configData->weaponTrain);
	if (!strcmp(paramName,"training/weaponFight")) sprintf(buf,"%d",m_configData->weaponFight);
	if (!strcmp(paramName,"training/weaponHand")) sprintf(buf,"%d",m_configData->weaponHand);
	if (!strcmp(paramName,"training/trainingMode")) sprintf(buf,"%d",m_configData->trainingMode);
	if (!strcmp(paramName,"training/fightWhenSurrounded")) sprintf(buf,"%d",m_configData->fightWhenSurrounded);
	if (!strcmp(paramName,"training/fightWhenAlien")) sprintf(buf,"%d",m_configData->fightWhenAlien);
	if (!strcmp(paramName,"training/bloodHit")) sprintf(buf,"%d",m_configData->bloodHit);
	if (!strcmp(paramName,"training/activate")) sprintf(buf,"%d",m_configData->trainingActivate);
	if (!strcmp(paramName,"depot/depotDropInsteadOfDepositon")) sprintf(buf,"%d",m_configData->depotDropInsteadOfDepositon);
	if (!strcmp(paramName,"depot/depotCap")) sprintf(buf,"%d",m_configData->depotCap);
	
	return buf;
}

char *CMod_cavebotApp::getConfigParamName(int nr) {
	switch (nr) {
	case 0: return "attack/follow";
	case 1: return "attack/mode";
	case 2: return "attack/suspendOnEnemey";
	case 3: return "attack/monster";
	case 4: return "loot/item/gp";
	case 5: return "loot/item/custom";
	case 6: return "loot/item/worm";
	case 7: return "loot/item/food";
	case 8: return "loot/other/inbag";
	case 9: return "loot/other/capacityLimit";
	case 10: return "walker/waypoint";
	case 11: return "walker/other/selectMode";
	case 12: return "walker/other/mapUsed";
	case 13: return "loot/stats/gather";
	case 14: return "attack/suspendOnNoMove";
	case 15: return "attack/range";
	case 16: return "loot/other/eatFromCorpse";
	case 17: return "attack/stick";
	case 18: return "attack/unreachableAfter";
	case 19: return "attack/suspendAfterUnreachable";
	case 20: return "attack/attackOnlyAttacking";
	case 21: return "attack/forceAttackAfterAttack";
	case 22: return "walker/other/standAfterWaypointReached";
	case 23: return "depot/entry";
	case 24: return "general/debug";
	case 25: return "training/weaponTrain";
	case 26: return "training/weaponFight";
	case 27: return "training/fightWhenSurrounded";
	case 28: return "training/fightWhenAlien";
	case 29: return "training/bloodHit";
	case 30: return "training/activate";
	case 31: return "depot/depotDropInsteadOfDepositon";
	case 32: return "loot/other/dropNotLooted";
	case 33: return "loot/other/lootFromFloor";
	case 34: return "attack/hpAbove";
	case 35: return "attack/ignore";
	case 36: return "attack/backattackRunes";
	case 37: return "attack/shareAlienBackattack";
	case 38: return "depot/depotCap";
	case 39: return "training/weaponHand";
	case 40: return "training/trainingMode";
		
	default:
		return NULL;
	}
}

int CMod_cavebotApp::isMultiParam(char *paramName) {
	if (!strcmp(paramName,"walker/waypoint")) return 1;
	if (!strcmp(paramName,"attack/monster")) return 1;
	if (!strcmp(paramName,"depot/entry")) return 1;
	if (!strcmp(paramName,"attack/ignore")) return 1;
	return 0;
}

void CMod_cavebotApp::resetMultiParamAccess(char *paramName) {
	if (!strcmp(paramName,"walker/waypoint")) m_currentWaypointNr=0;
	if (!strcmp(paramName,"attack/monster")) m_currentMonsterNr=0;
	if (!strcmp(paramName,"depot/entry")) m_currentDepotEntryNr=0;
	if (!strcmp(paramName,"attack/ignore")) m_currentIgnoreNr=0;
}
