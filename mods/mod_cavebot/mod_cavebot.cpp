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


// mod_cavebot.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_cavebot.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

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
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CToolAutoAttackStateAttack globalAutoAttackStateAttack=CToolAutoAttackStateAttack_notRunning;
CToolAutoAttackStateLoot globalAutoAttackStateLoot=CToolAutoAttackStateLoot_notRunning;
CToolAutoAttackStateWalker globalAutoAttackStateWalker=CToolAutoAttackStateWalker_notRunning;
CToolAutoAttackStateDepot globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notRunning;
CToolAutoAttackStateTraining globalAutoAttackStateTraining=CToolAutoAttackStateTraining_notRunning;

int lastAttackTmCreatureId[150];
int lastAttackTmTm[150];

int targetX=0,targetY=0,targetZ=0;
int depotX=0,depotY=0,depotZ=0;
int firstCreatureAttackTM=0;
int currentPosTM=0;
int creatureAttackDist=0;
int unreachableSecondsLeft=0;

CTibiaMapProxy tibiaMap;

/**
 * Register cavebot debug.
 */
void registerDebug(char *s)
{
	FILE *f=fopen("tibiaauto-debug-cavebot.txt","a+");
	if (f)
	{
		char dateStr [15];
		char timeStr [15];
		_strdate( dateStr);
		_strtime( timeStr );
		fprintf(f,"%s %s: %s\n",dateStr,timeStr,s);		
		fclose(f);
	}
}

/**
 * Dump full info about a creature (for debuging purposes)
 */
void dumpCreatureInfo(char *info,int tibiaId)
{
	CMemReaderProxy reader;		
	CMemConstData memConstData = reader.getMemConstData();
	
	int i;
	int nr=-1;
	for (i=0;i<memConstData.m_memMaxCreatures;i++)
	{
		CTibiaCharacter *ch = reader.readVisibleCreature(i);

		if (ch->tibiaId==tibiaId)
		{
			nr=i;
			delete ch;
			break;
		}

		delete ch;
	}

	int offset = memConstData.m_memAddressFirstCreature+nr*memConstData.m_memLengthCreature;

	registerDebug(info);

	if (nr!=-1)
	{
		for (i=0;i<memConstData.m_memLengthCreature;i+=4)
		{
			char buf[128];
			int val=reader.getMemIntValue(offset+i);
			sprintf(buf,"offset %d -> %d",i,val);
			registerDebug(buf);
		}
	} else {
		registerDebug("Unable to find creature");
	}
	registerDebug("end of creature dump");
}

/**
 * Checks whethre we should go back to depot or not.
 */
int depotCheckShouldGo(CConfigData *config)
{
	int ret=0;
	CMemReaderProxy reader;	
	CTibiaItemProxy itemProxy;	
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	if (self->cap<=config->depotCap) ret++;
	
	int i;
	for (i=0;i<100&&strlen(config->depotTrigger[i].itemName);i++)
	{
		int objectId = itemProxy.getObjectId(config->depotTrigger[i].itemName);
		int contNr;
		int totalQty=0;
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
		{
			CTibiaContainer *cont = reader.readContainer(contNr);
			
			if (cont->flagOnOff)				
				totalQty+=cont->countItemsOfType(objectId);
			
			delete cont;
		}
		// check whether we should deposit something
		if (config->depotTrigger[i].when>config->depotTrigger[i].remain&&
			totalQty>=config->depotTrigger[i].when) ret++;
		// check whether we should restack something
		if (config->depotTrigger[i].when<config->depotTrigger[i].remain&&
			totalQty<=config->depotTrigger[i].when) ret++;
	}
	delete self;
	return ret;
}

/**
 * Do checks concerning 'go to depot' functionality.
 */
void depotCheck(CConfigData *config)
{	
	CMemReaderProxy reader;	
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;	
	CMemConstData memConstData = reader.getMemConstData();
	if (config->debug) registerDebug("Depot check");
	if (globalAutoAttackStateDepot==CToolAutoAttackStateDepot_notRunning)
	{
		CTibiaCharacter *self=reader.readSelfCharacter();
		if (depotCheckShouldGo(config))
		{
			if (config->debug) registerDebug("We should go to a depot");
			/**
			* Ok then - we should do the depositing.
			*/
			
			int path[15];
			struct point nearestDepot = CModuleUtil::findPathOnMap(self->x,self->y,self->z,0,0,0,301,path);
			depotX=nearestDepot.x;
			depotY=nearestDepot.y;
			depotZ=nearestDepot.z;
			if (depotX&&depotY&&depotZ)
			{
				// depot found - go to it
				globalAutoAttackStateDepot=CToolAutoAttackStateDepot_walking;
				// reset goto target
				targetX=depotX;
				targetY=depotY;
				targetZ=depotZ;
				if (config->debug) registerDebug("Depot found - going to it");
			} else {
				// depot not found - argh
				globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notFound;
				depotX=depotY=depotZ=0;
				if (config->debug) registerDebug("Depot not found - depot state: not found");
			}
			
			delete self;
			return;
		}	
	
	}
	if (globalAutoAttackStateDepot==CToolAutoAttackStateDepot_walking)
	{
		CTibiaMapProxy tibiaMap;
		if (config->debug) registerDebug("Walking to a depot section");
		CTibiaCharacter *self = reader.readSelfCharacter();
		// check whether the depot is reachable
		int path[15];
		if (config->debug) registerDebug("findPathOnMap: depot walker");
		CModuleUtil::findPathOnMap(self->x,self->y,self->z,depotX,depotY,depotZ,0,path);
		if (!path[0]||!tibiaMap.isPointAvailable(depotX,depotY,depotZ))
		{
			if (config->debug) registerDebug("Path to the current depot not found: must find a new one");
			// path to the current depot not found, must find a new depot
			struct point nearestDepot = CModuleUtil::findPathOnMap(self->x,self->y,self->z,0,0,0,301,path);
			depotX=nearestDepot.x;
			depotY=nearestDepot.y;
			depotZ=nearestDepot.z;
			if (depotX&&depotY&&depotZ)
			{
				if (config->debug) registerDebug("New depot found");
				// depot found - go to it
				globalAutoAttackStateDepot=CToolAutoAttackStateDepot_walking;
				// reset goto target
				targetX=depotX;
				targetY=depotY;
				targetZ=depotZ;
			} else {
				if (config->debug) registerDebug("No new depot found");
				// depot not found - argh
				globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notFound;
				depotX=depotY=depotZ=0;
			}
		} else {
			if (config->debug) registerDebug("Currently selected depot is reachable");
		}

		delete self;
	}
	if (globalAutoAttackStateDepot==CToolAutoAttackStateDepot_notFound)
	{
		// not depot found - but try finding another one
		CTibiaCharacter *self = reader.readSelfCharacter();
		int path[15];
		struct point nearestDepot = CModuleUtil::findPathOnMap(self->x,self->y,self->z,0,0,0,301,path);
		depotX=nearestDepot.x;
		depotY=nearestDepot.y;
		depotZ=nearestDepot.z;
		if (depotX&&depotY&&depotZ)
		{
			// depot found - go to it
			globalAutoAttackStateDepot=CToolAutoAttackStateDepot_walking;
			// reset goto target
			targetX=depotX;
			targetY=depotY;
			targetZ=depotZ;
			if (config->debug) registerDebug("Depot found - going to it");
		} else {
			// depot not found - argh
			globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notFound;
			depotX=depotY=depotZ=0;
			if (config->debug) registerDebug("Depot not found - depot state: not found (again)");
		}
		
		delete self;
	}
}

int depotDepositOpenChest(int x,int y,int z)
{
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaCharacter *self = reader.readSelfCharacter();
	int count=reader.mapGetPointItemsCount(point(x-self->x,y-self->y,0));
	int pos=0;
	if (count>10) count=10;
				
	int lastNonDepotObject=0;
	for (pos=0;pos<count;pos++)
	{			
		int tileId=reader.mapGetPointItemId(point(x-self->x,y-self->y,0),pos);
		if (!reader.getTibiaTile(tileId)->ground&&
			!reader.getTibiaTile(tileId)->blocking&&
			!reader.getTibiaTile(tileId)->isDepot&&
			!lastNonDepotObject)
		{
			// not depot, not ground, not blocking => some junk on the depot			
			lastNonDepotObject=tileId;
			
		}
		if (reader.getTibiaTile(tileId)->isDepot)
		{				
			if (lastNonDepotObject)
			{
				// remove junk laying over the depot chest
				sender.moveObjectFromFloorToFloor(lastNonDepotObject,x,y,z,self->x,self->y,self->z,100);
				Sleep(200);
			}
			// this is the depot chest so open it			
			sender.openContainerFromFloor(tileId,x,y,z,9);
			CModuleUtil::waitForOpenContainer(9,1);
			CTibiaContainer *cont = reader.readContainer(9);
			int isOpen=cont->flagOnOff;
			delete cont;
			delete self;
			return isOpen;
		}
	}		
	delete self;
	return 0;
}

/**
 * Move the item to some container in the chest.
 */
void depotDepositMoveToChest(int objectId, int sourceContNr, int sourcePos, int qty)
{
	if (globalAutoAttackStateDepot==CToolAutoAttackStateDepot_noSpace)
		return;
	
	
	CMemReaderProxy reader;	
	CPackSenderProxy sender;
	CTibiaContainer *depotChest = reader.readContainer(9);
	
	if (!depotChest->flagOnOff)
	{
		// shouldn't happen...
		delete depotChest;
		return;
	}

	/**
	 * If container number 8 is open than we can try to deposit to it (to avoid 
	 * massive close/open of containers.
	 */
	CTibiaContainer *cont8 = reader.readContainer(8);
	if (cont8->flagOnOff&&cont8->itemsInside<cont8->size)
	{
		// found container in which we can do deposit!
		sender.moveObjectBetweenContainers(objectId,0x40+sourceContNr,sourcePos,0x40+8,cont8->size,qty);
		CModuleUtil::waitForItemsInsideChange(8,cont8->itemsInside);
		delete cont8;
		delete depotChest;
		return;
	}
	delete cont8;

	/**
	 * Scan through all containers in the depot chest to look for an empty one
	 */
	int itemNr;
	int count=depotChest->itemsInside;
	for (itemNr=0;itemNr<count;itemNr++)
	{
		CTibiaItem *item = (CTibiaItem *)depotChest->items.GetAt(itemNr);				
		if (reader.getTibiaTile(item->objectId)->isContainer)
		{
			// that object is a container, so open it
			sender.closeContainer(8);
			CModuleUtil::waitForOpenContainer(8,0);
			sender.openContainerFromContainer(item->objectId,0x40+9,itemNr,8);
			CModuleUtil::waitForOpenContainer(8,1);
			CTibiaContainer *newContainer = reader.readContainer(8);
			if (newContainer->flagOnOff&&newContainer->itemsInside<newContainer->size)
			{
				// found container in which we can do deposit!
				sender.moveObjectBetweenContainers(objectId,0x40+sourceContNr,sourcePos,0x40+8,newContainer->size-1,qty);
				CModuleUtil::waitForItemsInsideChange(8,newContainer->itemsInside);
				delete newContainer;
				delete depotChest;
				return;
			}
			delete newContainer;			
		}
	}

	delete depotChest;

	// if we are here this means that the depot is full
	globalAutoAttackStateDepot=CToolAutoAttackStateDepot_noSpace;
}

// take items from depot chest and move to my backpacks (contnr,pos)
int depotDepositTakeFromChest(int objectId,int contNr,int pos,int qtyToPickup)
{			
	CMemReaderProxy reader;	
	CPackSenderProxy sender;
	CTibiaContainer *depotChest = reader.readContainer(9);
	int itemNr;
	
	if (!depotChest->flagOnOff)
	{
		// shouldn't happen...
		delete depotChest;
		return 0;
	}

	/**
	 * If container number 8 is open than we can try to take our item from it (to avoid 
	 * massive close/open of containers.
	 */
	CTibiaContainer *cont8 = reader.readContainer(8);
	if (cont8->flagOnOff)
	{
		for (itemNr=0;itemNr<cont8->itemsInside;itemNr++)
		{
			CTibiaItem *item=((CTibiaItem *)cont8->items.GetAt(itemNr));
			if (item->objectId==objectId)
			{
				// that's this item - pick it up
				if (qtyToPickup>item->quantity) qtyToPickup=item->quantity;
				sender.moveObjectBetweenContainers(objectId,0x40+8,itemNr,0x40+contNr,pos,qtyToPickup);
				CModuleUtil::waitForItemsInsideChange(contNr,pos);

				delete cont8;
				delete depotChest;
				return qtyToPickup;

			}
		}				
	}
	delete cont8;

	/**
	 * Scan through all containers in the depot chest to look for some matching one
	 */	
	int count=depotChest->itemsInside;
	for (itemNr=0;itemNr<count;itemNr++)
	{
		CTibiaItem *item = (CTibiaItem *)depotChest->items.GetAt(itemNr);				
		if (reader.getTibiaTile(item->objectId)->isContainer)
		{
			// that object is a container, so open it
			sender.closeContainer(8);
			CModuleUtil::waitForOpenContainer(8,0);
			sender.openContainerFromContainer(item->objectId,0x40+9,itemNr,8);
			CModuleUtil::waitForOpenContainer(8,1);
			CTibiaContainer *newContainer = reader.readContainer(8);
			if (newContainer->flagOnOff)
			{
				for (itemNr=0;itemNr<newContainer->itemsInside;itemNr++)
				{
					CTibiaItem *item=((CTibiaItem *)newContainer->items.GetAt(itemNr));
					if (item->objectId==objectId)
					{
						// that's this item - pick it up
						if (qtyToPickup>item->quantity) qtyToPickup=item->quantity;
						sender.moveObjectBetweenContainers(objectId,0x40+8,itemNr,0x40+contNr,pos,qtyToPickup);
						CModuleUtil::waitForItemsInsideChange(contNr,pos);
						
						delete newContainer;
						delete depotChest;
						
						return qtyToPickup;
					}
				}		
				
			}
			delete newContainer;			
		}
	}

	delete depotChest;

	// our item to move not found;
	return 0;	
}

int countAllItemsOfType(int objectId)
{
	CMemReaderProxy reader;
	int contNr;
	int ret=0;
	for (contNr=0;contNr<8;contNr++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);
		
		if (cont->flagOnOff)
			ret+=cont->countItemsOfType(objectId);
		
		delete cont;
	}
	return ret;
}

/**
 * We are nearby a depot, so do the depositing.
 */
void depotDeposit(CConfigData *config)
{
	int i;
	CMemReaderProxy reader;	
	CPackSenderProxy sender;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	/*
	 * Now we must do:
	 * 1. locate depot chest (optional)
	 * 2. open depot chest and find place for item (optional)
	 * 3. deposit items
	 */
	
	if (!config->depotDropInsteadOfDepositon)
	{
		globalAutoAttackStateDepot=CToolAutoAttackStateDepot_opening;
		int depotChestOpen=0;
		depotChestOpen|=depotDepositOpenChest(self->x-1,self->y,self->z);
		depotChestOpen|=depotDepositOpenChest(self->x+1,self->y,self->z);
		depotChestOpen|=depotDepositOpenChest(self->x,self->y-1,self->z);
		depotChestOpen|=depotDepositOpenChest(self->x,self->y+1,self->z);
		if (!depotChestOpen)
		{
			// can't open depot chest ?!?!?
			// so cancel depositing and proceed to walking
			delete self;
			globalAutoAttackStateDepot=CToolAutoAttackStateDepot_walking;
			return;
		}
	}

	// do the depositing
	globalAutoAttackStateDepot=CToolAutoAttackStateDepot_depositing;

	sender.closeContainer(8);
	CModuleUtil::waitForOpenContainer(8,0);
	
	for (i=0;i<100&&strlen(config->depotTrigger[i].itemName);i++)
	{
		int objectToMove = itemProxy.getObjectId(config->depotTrigger[i].itemName);
		int contNr;
		int totalQty=countAllItemsOfType(objectToMove);
		
		if (config->depotTrigger[i].when>config->depotTrigger[i].remain)
		{
			// deposit to depot
		
			
			int qtyToMove=totalQty-config->depotTrigger[i].remain;
			for (contNr=0;contNr<8;contNr++)
			{
				CTibiaContainer *cont = reader.readContainer(contNr);
				
				if (cont->flagOnOff)
				{
					int itemNr;
					for (itemNr=cont->itemsInside-1;itemNr>=0;itemNr--)				
					{
						CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(itemNr);
						if (item->objectId==objectToMove)
						{
							int itemQty = item->quantity?item->quantity:1;
							if (totalQty-itemQty<config->depotTrigger[i].remain) 
							{
								itemQty=totalQty-config->depotTrigger[i].remain;
							}
							if (itemQty>0)
							{		
								if (!config->depotDropInsteadOfDepositon)
								{
									// move to the depot chest
									depotDepositMoveToChest(objectToMove,contNr,item->pos,itemQty);
								} else {
									// move onto the floor									
									sender.moveObjectFromContainerToFloor(objectToMove,0x40+contNr,item->pos,self->x,self->y,self->z,itemQty);
								}
							}
							totalQty-=itemQty;
						}
					}
				}
				
				delete cont;
			}
		}
		if (config->depotTrigger[i].when<config->depotTrigger[i].remain&&!config->depotDropInsteadOfDepositon)
		{
			// pickup from depot
			int qtyToPickup=config->depotTrigger[i].remain-countAllItemsOfType(objectToMove);
			int movesInIteration=1;
			while (qtyToPickup>0&&movesInIteration>0)
			{			
				movesInIteration=0;
				for (contNr=0;contNr<8;contNr++)
				{
					CTibiaContainer *cont = reader.readContainer(contNr);
					if (cont->flagOnOff&&cont->itemsInside<cont->size)
					{
						int movedQty=depotDepositTakeFromChest(objectToMove,contNr,cont->size-1,qtyToPickup);						
						if (movedQty) movesInIteration++;
						
					}
					delete cont;
				}
				qtyToPickup=config->depotTrigger[i].remain-countAllItemsOfType(objectToMove);
			} // while picking up 			
		} // if pickup from depot
	}
		
	
	// all is finished :) - we can go back to the hunting area
	globalAutoAttackStateDepot=CToolAutoAttackStateDepot_notRunning;
	depotX=depotY=depotZ=0;	
	delete self;
}

/**
 * Make sure that some item of type objectId is in the location (e.g. hand).
 */
void ensureItemInPlace(int outputDebug,int location, int locationAddress, int objectId)
{	
	CMemReaderProxy reader;
	CPackSenderProxy sender;	
	CMemConstData memConstData = reader.getMemConstData();	
	CTibiaItem *item = reader.readItem(locationAddress);
	int canProceed=0;
	if (item->objectId==objectId)
	{
		// everything's ok - no changes
		delete item;
		return;
	}
	globalAutoAttackStateTraining=CToolAutoAttackStateTraining_switchingWeapon;
	while (item->objectId!=0)
	{
		if (outputDebug) registerDebug("Next emptying ensureItemInPlace");
		int contNr;
		// location not empty so we must put away the item from it
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
		{
			CTibiaContainer *cont = reader.readContainer(contNr);
			if (cont->flagOnOff&&cont->itemsInside<cont->size)
			{
				// container has some free places
				sender.moveObjectBetweenContainers(item->objectId,location,0,0x40+contNr,cont->size-1,item->quantity?item->quantity:1);
				CModuleUtil::waitForItemsInsideChange(contNr,cont->itemsInside);
				contNr=10000; // stop the loop
				canProceed=1;
			}

			delete cont;
		}
		delete item;
		if (!canProceed)
		{
			// means: we have no place in any container
			sender.sendTAMessage("I've an invalid weapon in hand, and I can't move it anywhere!");
			return;
		}
		item = reader.readItem(locationAddress);
	}

	canProceed=0;
	
	while (item->objectId!=objectId)
	{
		if (outputDebug) registerDebug("Next moving item in ensureItemInPlace");
		
		// now the location is free, so we just need to find the good item and put it there	
		CUIntArray itemsAccepted;
		int contNr;
		itemsAccepted.Add(objectId);
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
		{
			CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemsAccepted);
			if (item)
			{
				CTibiaContainer *cont = reader.readContainer(contNr);
				if (cont->flagOnOff)
				{
					sender.moveObjectBetweenContainers(item->objectId,0x40+contNr,item->pos,location,0,item->quantity?item->quantity:1);
					CModuleUtil::waitForItemsInsideChange(contNr,cont->itemsInside);
					contNr=10000; // stop the loot
					canProceed=1;
				}
				delete cont;
			}			
		}
		delete item;
		if (!canProceed)
		{
			// means: we have no place in any container
			sender.sendTAMessage("I can't find the training/fight weapon in any container!");
			return;
		}
		item = reader.readItem(locationAddress);
	}
	delete item;
}

/**
 * Check the training related things, like: 
 * 1. Check whether we should be in fight or training mode (and switch weapons).
 * 2. Check whether we should be full attack/def/dont touch (if blood control is active).
 * 3. Switch weapon if needed.
 */
void trainingCheck(CConfigData *config, int alienFound, int attackingCreatures, int lastAttackedCreatureHpDrop,int *attackMode)
{
	CMemReaderProxy reader;
	CPackSenderProxy sender;	
	CMemConstData memConstData = reader.getMemConstData();	
	
	if (config->trainingActivate)
	{		
		char buf[128];
		int canTrain=1;

		sprintf(buf,"Training check: alien=%d, attacking=%d, lastHpDrop=%d, attackMode=%d",alienFound,attackingCreatures,time(NULL)-lastAttackedCreatureHpDrop,*attackMode);
		if (config->debug) registerDebug(buf);
		if (config->fightWhenAlien&&alienFound) canTrain=0;
		if (config->fightWhenSurrounded&&attackingCreatures>2) canTrain=0;
		if (canTrain)
		{
			if (config->debug) registerDebug("Training: training mode");
			ensureItemInPlace(config->debug,6,memConstData.m_memAddressLeftHand,config->weaponTrain);
			globalAutoAttackStateTraining=CToolAutoAttackStateTraining_training;
			if (config->bloodHit)
			{
				sprintf(buf,"Training: inside blood hit control (%d-%d=%d)",time(NULL),lastAttackedCreatureHpDrop,time(NULL)-lastAttackedCreatureHpDrop);
				if (config->debug) registerDebug(buf);
				if (time(NULL)-lastAttackedCreatureHpDrop<20)
				{					
					// if 30s has passed since last blood hit, then we do 'normal' attack mode
					// otherwise we do full defence
					*attackMode=2;
					globalAutoAttackStateTraining=CToolAutoAttackStateTraining_trainingFullDef;
				}
			}			
		} else {
			if (config->debug) registerDebug("Training: fight mode");
			ensureItemInPlace(config->debug,6,memConstData.m_memAddressLeftHand,config->weaponFight);
			globalAutoAttackStateTraining=CToolAutoAttackStateTraining_fighting;
		}

		
		
	}
}

void dropAllItemsFromContainer(int contNr, int x, int y, int z)
{	
	CMemReaderProxy reader;
	CPackSenderProxy sender;	
	CTibiaContainer *dropCont = reader.readContainer(contNr);
	if (dropCont->flagOnOff)
	{				
		int itemNr;
		for (itemNr=dropCont->itemsInside-1;itemNr>=0;itemNr--)
		{			
			CTibiaItem *lootItem = (CTibiaItem *)dropCont->items.GetAt(itemNr);	
			sender.moveObjectFromContainerToFloor(lootItem->objectId,0x40+contNr,lootItem->pos,x,y,z,lootItem->quantity?lootItem->quantity:1);
			CModuleUtil::waitForItemsInsideChange(contNr,itemNr+1);
		}	
	}
	
	delete dropCont;
}

/////////////////////////////////////////////////////////////////////////////
void droppedLootCheck(CConfigData *config, int lootedItemId)
{		
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
		int freeContInside=-1;
		int x,y;	
		int bestX=0,bestY=0,bestPath=1000;
		
		// lookup a free place in any container
		int contNr;						
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
		{
			CTibiaContainer *cont = reader.readContainer(contNr);
			if (cont->flagOnOff&&cont->itemsInside<cont->size)
			{	
				freeContNr=contNr;
				freeContPos=cont->size-1;
				freeContInside=cont->itemsInside;
				delete cont;
				break;
			}
			delete cont;
		}

		if (freeContNr==-1)
		{
			if (config->debug) registerDebug("Loot from floor: no free container found");
			// no free place in container found - exit
			return;
		}


		for (x=-7;x<=7;x++)
		{
			for (y=-5;y<=5;y++)
			{				
				if (isItemCovered(x,y,lootedItemId)||isItemOnTop(x,y,lootedItemId)) 
				{
					// there is the lootedItem (on top or covered)
					int path[15];
					int pathSize=0;
					memset(path,0x00,sizeof(int)*15);					
					if (config->debug) registerDebug("findPathOnMap: loot from floor");
					CModuleUtil::findPathOnMap(self->x, self->y, self->z, self->x+x, self->y+y, self->z, 0,path);
					for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){};					

					// if item is on our place then this is the best situation
					if (x==0&&y==0) pathSize=0;

					if (bestPath>pathSize)
					{
						// there is a path to the point AND the path
						// is the closest (the best)
						bestX=x;
						bestY=y;
						bestPath=pathSize;
						sprintf(buf,"Floor loot: item (%d,%d,%d)",x,y,pathSize);
						if (config->debug) registerDebug(buf);						
					}					
				}
			}
		}

		if (config->debug) registerDebug("Loot from floor: before anything to loot check");

		if (bestPath<1000)
		{
			if (config->debug) registerDebug("Loot from floor: found something to loot -> walking");
			// anything to loot found							
			
			int walkIter;				
			CTibiaCharacter *self2 = reader.readSelfCharacter();				
			
			for (walkIter=0;walkIter<5||(self->x+x==self2->x&&self->y+y==self2->y&&self->z==self2->z);walkIter++)
			{
				delete self2;
				self2 = reader.readSelfCharacter();				
				if (self2->x==self->x+bestX&&self2->y==self->y&&self2->z==self->z)
				{
					// no need to walk as we are on position already
					break;
				}

				int path[15];
				int pathSize=0;
				memset(path,0x00,sizeof(int)*15);					
				if (config->debug) registerDebug("findPathOnMap: loot from floor 2");
				CModuleUtil::findPathOnMap(self2->x, self2->y, self2->z, self->x+bestX, self->y+bestY, self->z, 0,path);
				for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){};					
				
				sprintf(buf,"Loot from floor: path size=%d between (%d,%d,%d) and (%d,%d,%d)",pathSize,self2->x,self2->y,self2->z,self->x+bestX,self->y+bestY,self->z);
				if (config->debug) registerDebug(buf);
				if (pathSize) 
				{
					int i;
					CModuleUtil::executeWalk(self2->x,self2->y,self2->z,path);	
					// wait up to 2000ms for reaching final point
					for (i=0;i<20;i++)
					{
						CTibiaCharacter *self3 = reader.readSelfCharacter();
						if (self->x+bestX==self3->x&&self->y+bestY==self3->y&&self->z==self3->z)
						{
							// this means: we reached final point
							break;
						}
						delete self3;
						Sleep(100);
					}
				} else {
					sprintf(buf,"Loot from floor: aiks, no path found (%d,%d,%d)->(%d,%d,%d)!",self2->x,self2->y,self2->z,self->x+bestX,self->y+bestY,self->z);
					if (config->debug) registerDebug(buf);
					break;
				}
			}
			sprintf(buf,"Loot from floor: location me(%d,%d,%d)->item(%d,%d,%d)",self2->x,self2->y,self2->z,self->x+bestX,self->y+bestY,self->z);
			if (config->debug) registerDebug(buf);

			if (self->x+bestX==self2->x&&self->y+bestY==self2->y&&self->z==self2->z)
			{
				if (config->debug) registerDebug("Loot from floor: standing over item");
				// we are standing over the item
				
				int offsetX,offsetY;
				// means - it's covered, so we need to "dig" to it
				for (offsetX=-1;offsetX<=1;offsetX++)
				{
					for (offsetY=-1;offsetY<=1;offsetY++)
					{
						// double loop break!
						if ((offsetX||offsetY)&&tibiaMap.isPointAvailable(self2->x+offsetX,self2->y+offsetY,self2->z)) goto extBreak;
					}					
				}
extBreak:
				sprintf(buf,"Loot from floor: using (%d,%d) as dropout offset",offsetX,offsetY);
				if (config->debug) registerDebug(buf);
				if (offsetX!=2&&offsetY!=2)
				{
					int uncoverIter=0;
					// do it only when drop place if found
					while (uncoverIter++<10&&isItemCovered(0,0,lootedItemId)&&!isItemOnTop(0,0,lootedItemId))
					{
						if (config->debug) registerDebug("Loot from floor: uncovering item");
						int objectId=itemOnTopCode(0,0);
						int qty=itemOnTopQty(0,0);
						sender.moveObjectFromFloorToFloor(objectId,self2->x,self2->y,self2->z,self2->x+offsetX,self2->y+offsetY,self2->z,qty);
						Sleep(250);
					}
					if (isItemOnTop(0,0,lootedItemId))
					{
						if (config->debug) registerDebug("Loot from floor: picking up item");
						int qty=itemOnTopQty(0,0);
						sender.moveObjectFromFloorToContainer(lootedItemId,self2->x,self2->y,self->z,0x40+freeContNr,freeContPos,qty);
						CModuleUtil::waitForItemsInsideChange(freeContNr,freeContInside);
					}
					
				}
			}

			delete self2;
			
			
		}					
	} // if (config->lootFromFloor&&self->cap>config->capacityLimit) {
	delete self;
}

void fireRunesAgainstCreature(CConfigData *config,int creatureId)
{
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

	if (rune)
	{
		if (config->debug) registerDebug("Rune to fire found!");
		sender.castRuneAgainstCreature(0x40+realContNr,rune->pos,rune->objectId,creatureId);
	}
	
} // fireRunesAgainstCreature()


/////////////////////////////////////////////////////////////////////////////
// shared memory initalisation
void shMemInit(CSharedMemory *pMem)
{
	int pid=GetCurrentProcessId();
	char varName[128];
	sprintf(varName,"mod_cavebot_%d",pid);
	// this means shmem initialisation failed
	if (!pMem||!pMem->IsCreated()) return;
	pMem->AddDwordValue(varName,0);
}

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
	int pid=GetCurrentProcessId();
	CSharedMemory sh_mem("TibiaAuto",1024,shMemInit);
	char varName[128];
	sprintf(varName,"mod_cavebot_%d",pid);	

	int attackConsiderPeriod=20000;
	int currentlyAttackedCreature=0;	
	int lastWaypointNr=0;
	int lastStandingX=0,lastStandingY=0,lastStandingZ=0;
	int walkerStandingEndTm=0;
	unreachableSecondsLeft=0;
	targetX=targetY=targetZ=0;
	int lastAttackedCreatureHp=0;
	int lastAttackedCreatureHpDrop=0;	
	int shareAlienBackattack=0;
	

	// reset globals
	targetX=targetY=targetZ=0;
	depotX=depotY=depotZ=0;
	firstCreatureAttackTM=0;
	currentPosTM=0;
	creatureAttackDist=0;
	unreachableSecondsLeft=0;


	int waypointsCount=0;
	int i;
	for (i=0;i<100;i++)
	{
		if (config->waypointList[i].x)
		{
			waypointsCount++; 
		} else {
			break;
		}
	}
	firstCreatureAttackTM=0;
	currentPosTM=0;
	for (i=0;i<150;i++)
	{
		lastAttackTmCreatureId[i]=lastAttackTmTm[i]=0;
	}

	if (config->debug) 
	{
		// remove the debug file
		FILE *f=fopen("tibiaauto-debug-cavebot.txt","w");
		if (f)
			fclose(f);
		registerDebug("Cavebot started");
	}	

	shareAlienBackattack=config->shareAlienBackattack;
	if (!sh_mem.IsCreated()) 
	{
		if (config->debug)
		{
			registerDebug("ERROR: shmem initialisation failed");
		}
		shareAlienBackattack=0;
	}

	
	

	while (!toolThreadShouldStop)
	{						
		Sleep(1000);
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		if (config->debug) 
		{
			char buf[256];
			sprintf(buf,"Next loop. States: %d, %d, %d, %d",globalAutoAttackStateAttack,globalAutoAttackStateLoot,globalAutoAttackStateWalker,globalAutoAttackStateDepot);
			registerDebug(buf);
		}					
		
		CTibiaCharacter *self = reader.readSelfCharacter();		
		
		// if client thinks we are attacking something different, then
		// send "attack new target" to it
		if (reader.getAttackedCreature()!=currentlyAttackedCreature)
		{
			sender.attack(currentlyAttackedCreature);
		}


		/**
		 * Check whether we should go to a depot
		 */
		depotCheck(config);

		/**
		 * Check whether we should collected dropped loot.
		 */
		if (!currentlyAttackedCreature)
		{
			if (config->lootFood)
			{
				int p;
				for (p=0;p<itemProxy.getItemsFoodArray()->GetSize();p++)
				{
					droppedLootCheck(config,itemProxy.getItemsFoodArray()->GetAt(p));					
				}
			}
			if (config->lootGp)
			{
				droppedLootCheck(config,itemProxy.getValueForConst("GP"));
			}
			if (config->lootWorms)
			{
				droppedLootCheck(config,itemProxy.getValueForConst("worms"));
			}
			if (config->lootCustom)
			{
				int i;
				for (i=0;i<itemProxy.getItemsLootedCount();i++)
				{															
					droppedLootCheck(config,itemProxy.getItemsLootedId(i));
				}
			}			
		}

		int attackingCreatureCount=0;
		int targetFound=0;		
		int creatureDistList[100];
		int creatureBackAttackList[100];
		int alienCreatureFound=0;
		int alienCreatureForTrainingFound=0;		
		memset(creatureDistList,0x00,sizeof(int)*100);
		memset(creatureBackAttackList,0x00,sizeof(int)*100);

		if (globalAutoAttackStateAttack==CToolAutoAttackStateAttack_monsterUnreachable)
		{
			unreachableSecondsLeft--;
			if (unreachableSecondsLeft<=0)
			{
				globalAutoAttackStateAttack=CToolAutoAttackStateAttack_notRunning;
				unreachableSecondsLeft=0;
				// reset found target
				targetFound=0;
				if (config->debug) registerDebug("Monster unreachable period end");
			}
		}

		/** 
		 * If we are not attacking anyone and we are in the training mode,
		 * then make sure that we have the 'fight' in hand
		 */
		if (config->trainingActivate&&!currentlyAttackedCreature)
		{
			if (config->debug) registerDebug("Training: non training mode");
			ensureItemInPlace(config->debug,6,memConstData.m_memAddressLeftHand,config->weaponFight);
			globalAutoAttackStateTraining=CToolAutoAttackStateTraining_notRunning;
		}
		
		/**
		* Check if currently attacked creature is alive still
		* If it is dead we could try to get some loot from its body		 
		*/
		
		
		//if (currentlyAttackedCreature&&(self->cap>config->capacityLimit||config->eatFromCorpse)&&(config->lootFood||config->lootGp||config->lootWorms||config->lootCustom||config->eatFromCorpse))
		if (currentlyAttackedCreature)
		{						
			if (config->debug) registerDebug("Checking whether attacked creature is alive");
			// now let's see whether creature is still alive or not
			CTibiaCharacter *attackedCh = reader.getCharacterByTibiaId(currentlyAttackedCreature);
			if (attackedCh)
			{				
				if (!attackedCh->hpPercLeft&&
					abs(attackedCh->x-self->x)+abs(attackedCh->y-self->y)<4&&
					attackedCh->z==self->z)
				{
					if (config->debug) registerDebug("Attacked creature is dead");
					FILE *lootStatsFile = NULL;
					// time,rand,creature,name,pos,objectId,count,bagopen,checksum
					int killNr=rand();
					if (config->gatherLootStats)
					{
						lootStatsFile=fopen("tibiaauto-stats-loot.txt","a+");						
					}
					
					

					currentlyAttackedCreature=0;
					// attacked creature dead, near me, same floor
					globalAutoAttackStateLoot=CToolAutoAttackStateLoot_opening;
					// open container 9 and wait for sync									
					CModuleUtil::waitForCreatureDisappear(attackedCh->x-self->x,attackedCh->y-self->y,attackedCh->tibiaId);
					int corpseId = itemOnTopCode(attackedCh->x-self->x,attackedCh->y-self->y);					
					sender.openContainerFromFloor(corpseId,attackedCh->x,attackedCh->y,attackedCh->z,9);										
					
					
					if (CModuleUtil::waitForOpenContainer(9,true))
					{			
						if (config->debug) registerDebug("Open dead creature corpse (container 9)");
						int tm=time(NULL);
						int checksum;
						if (lootStatsFile)
						{		
							int i,len;
							char statChName[128];
							for (i=0,strcpy(statChName,attackedCh->name),len=strlen(statChName);i<len;i++)
							{
								if (statChName[i]=='[')
									statChName[i]='\0';
							}
							checksum = CModuleUtil::calcLootChecksum(tm,killNr,strlen(statChName),-1,corpseId,0,config->lootInBags);
							fprintf(lootStatsFile,"%d,%d,'%s',%d,%d,%d,%d,%d\n",tm,killNr,statChName,-1,corpseId,0,config->lootInBags,checksum);
							fflush(lootStatsFile);

							CTibiaContainer *lootCont = reader.readContainer(9);
							int itemNr;
							for (itemNr=0;itemNr<lootCont->itemsInside;itemNr++)
							{												
								CTibiaItem *lootItem = (CTibiaItem *)lootCont->items.GetAt(itemNr);
																
								checksum = CModuleUtil::calcLootChecksum(tm,killNr,strlen(statChName),itemNr,lootItem->objectId,(lootItem->quantity?lootItem->quantity:1),config->lootInBags);
								if (checksum<0) checksum*=-1;
								fprintf(lootStatsFile,"%d,%d,'%s',%d,%d,%d,%d,%d\n",tm,killNr,statChName,itemNr,lootItem->objectId,lootItem->quantity?lootItem->quantity:1,config->lootInBags,checksum);
								fflush(lootStatsFile);
							}
							free(lootCont);
						}
						if (config->debug) registerDebug("Container 9 data registered.");

						CUIntArray acceptedItems;
						if (config->lootFood)
						{
							int p;
							for (p=0;p<itemProxy.getItemsFoodArray()->GetSize();p++)
							{
								acceptedItems.Add(itemProxy.getItemsFoodArray()->GetAt(p));
							}
						}
						if (config->lootGp)
						{
							acceptedItems.Add(itemProxy.getValueForConst("GP"));
						}
						if (config->lootWorms)
						{
							acceptedItems.Add(itemProxy.getValueForConst("worms"));
						}
						if (config->lootCustom)
						{
							int i;
							for (i=0;i<itemProxy.getItemsLootedCount();i++)
							{					
								
								acceptedItems.Add(itemProxy.getItemsLootedId(i));
							}
						}
						

						int lootTakeIter;
						
						globalAutoAttackStateLoot=CToolAutoAttackStateLoot_moveing;
						for (lootTakeIter=0;lootTakeIter<5;lootTakeIter++)
						{				
							if (self->cap>config->capacityLimit)
							{
								CModuleUtil::lootItemFromContainer(9,&acceptedItems);
							}
							if (config->eatFromCorpse)
							{
								CModuleUtil::eatItemFromContainer(9);
								Sleep(250);
							}
						}
						if (config->lootInBags)
						{
							if (config->debug) registerDebug("Looting in bags");
							CTibiaContainer *cont = reader.readContainer(9);
							int itemNr;
							for (itemNr=0;itemNr<cont->itemsInside;itemNr++)
							{
								CTibiaItem *insideItem = (CTibiaItem *)cont->items.GetAt(itemNr);
								if (insideItem->objectId==itemProxy.getValueForConst("bagbrown"))
								{
									globalAutoAttackStateLoot=CToolAutoAttackStateLoot_openingBag;
									sender.openContainerFromContainer(insideItem->objectId,0x40+9,insideItem->pos,8);
									//sender.sendTAMessage("[debug] Opening bag");
									if (CModuleUtil::waitForOpenContainer(8,true))
									{
										if (lootStatsFile)
										{
											CTibiaContainer *lootCont = reader.readContainer(8);
											int itemNr;
											for (itemNr=0;itemNr<lootCont->itemsInside;itemNr++)
											{								
												int i,len;
												char statChName[128];
												for (i=0,strcpy(statChName,attackedCh->name),len=strlen(statChName);i<len;i++)
												{
													if (statChName[i]=='[')
														statChName[i]='\0';
												}

												CTibiaItem *lootItem = (CTibiaItem *)lootCont->items.GetAt(itemNr);												
												checksum = CModuleUtil::calcLootChecksum(tm,killNr,strlen(statChName),100+itemNr,lootItem->objectId,(lootItem->quantity?lootItem->quantity:1),config->lootInBags);
												if (checksum<0) checksum*=-1;
												fprintf(lootStatsFile,"%d,%d,'%s',%d,%d,%d,%d,%d\n",tm,killNr,statChName,100+itemNr,lootItem->objectId,lootItem->quantity?lootItem->quantity:1,config->lootInBags,checksum);
											}
											free(lootCont);
										}
										//sender.sendTAMessage("[debug] bag opened");
										globalAutoAttackStateLoot=CToolAutoAttackStateLoot_moveingBag;
										for (lootTakeIter=0;lootTakeIter<5;lootTakeIter++)
										{									
											//sender.sendTAMessage("[debug] loot iteration");
											if (self->cap>config->capacityLimit)
											{
												CModuleUtil::lootItemFromContainer(8,&acceptedItems);
											}
											if (config->eatFromCorpse)
											{
												CModuleUtil::eatItemFromContainer(8);												
											}											
										}
										
										if (config->dropNotLooted)
										{
											dropAllItemsFromContainer(8,attackedCh->x,attackedCh->y,attackedCh->z);											
										}
			
										globalAutoAttackStateLoot=CToolAutoAttackStateLoot_closingBag;
										//sender.sendTAMessage("[debug] closing bag");
										sender.closeContainer(8);
										CModuleUtil::waitForOpenContainer(8,false);
									}
								}
							}
							delete cont;
						} // if (lootInBags)

						if (config->dropNotLooted)
						{
							dropAllItemsFromContainer(9,attackedCh->x,attackedCh->y,attackedCh->z);											
						}
						
						
						globalAutoAttackStateLoot=CToolAutoAttackStateLoot_closing;
						// close container 9 and wait for sync
						
						sender.closeContainer(9);
						CModuleUtil::waitForOpenContainer(9,false);					
						globalAutoAttackStateLoot=CToolAutoAttackStateLoot_notRunning;
						if (config->debug) registerDebug("End of looting");
					} // if waitForContainer(9)						
					if (lootStatsFile) 
					{
						fclose(lootStatsFile);
					}
					globalAutoAttackStateLoot=CToolAutoAttackStateLoot_notRunning;
				} else {// if creatureIsDead
					if (config->debug) registerDebug("The creature is still alive");
					char debugBuf[256];
					sprintf(debugBuf,"Attacked creature info: x dist=%d y dist=%d z dist=%d id=%d visible=%d hp=%d",abs(self->x-attackedCh->x),abs(self->y-attackedCh->y),abs(self->z-attackedCh->z),attackedCh->tibiaId,attackedCh->visible,attackedCh->hpPercLeft);
					if (config->debug) registerDebug(debugBuf);
				}
				
				delete attackedCh;
			} // if attackedCh
		}
				
		int creatureAttackNewDist=100;
		int currentTm=reader.getCurrentTm();
		int backAttack=0; // if set, then this is 'back attack' - means ignore 'alien creatures, etc.'
		int backAttackAlien=0; // if set, then this is 'back attack from alien'
		alienCreatureForTrainingFound=0;
		
		for (int crNr=0;crNr<memConstData.m_memMaxCreatures;crNr++)
		{			
			CTibiaCharacter *ch=reader.readVisibleCreature(crNr);			

			if (ch->visible)
			{
				// refresh "last attacked tm" information
				if (lastAttackTmTm[crNr]!=ch->lastAttackTm)
				{
					/**
					 * One condition is satisfied:
					 * 1. Attack time has changed. We interpret this
					 *    by saying that ch->tibiaId is attacking us.
					 *    Note that this may not be truth if a creature
					 *    consumes same slot in less than < 2 seconds.
					 *    This shouldn't happen, but if it does...
					 */
					lastAttackTmCreatureId[crNr]=ch->tibiaId;
					lastAttackTmTm[crNr]=ch->lastAttackTm;
				}

				int monstListNr;

				// scan ignore list
				int creatureFoundOnIgnoreList=0;
				int i,len;
				char statChName[128];
				for (i=0,strcpy(statChName,ch->name),len=strlen(statChName);i<len;i++)
				{
					if (statChName[i]=='[')
						statChName[i]='\0';
				}
				for (monstListNr=0;monstListNr<config->ignoreCount;monstListNr++)
				{		
					if (!strcmpi(config->ignoreList[monstListNr],statChName))
					{
						if (config->debug) registerDebug("Creature found on ignore list");
						creatureFoundOnIgnoreList=1;
					}
				}
				
				// if monster is on ignore list then never attack it
				// and never backattack it - just ignore it
				// this option is usefull for training to not kill the trainer monsters
				if (!creatureFoundOnIgnoreList)
				{
					
					// scan creature list to find a target				
					int creatureFoundOnList=0;			
					for (monstListNr=0;monstListNr<config->monsterCount;monstListNr++)
					{									
						if (!strcmpi(config->monsterList[monstListNr],statChName))
							creatureFoundOnList=1;
					}				
					
					// if we are walking to a depot, we ignore creatures on list
					// but back attack still is active
					if (depotX&&depotY&&depotZ)
						creatureFoundOnList=0;
					
					// protection "lastAttackTmCreatureId[crNr]==ch->tibiaId" is added
					// because last attack tm is NOT refreshed after monster
					// slot reusage! (tibia client bug)
					int isMonsterAttackingMe=(currentTm-attackConsiderPeriod<ch->lastAttackTm&&ch->lastAttackTm&&lastAttackTmCreatureId[crNr]==ch->tibiaId);					
					if (config->debug)
					{
						char buf[128];
						sprintf(buf,"isMonsterAttackingMe=%d currentTm=%d attackConsiderPeriod=%d currentTm-cons=%d lastAttackTm=%d",isMonsterAttackingMe, currentTm, attackConsiderPeriod, currentTm-attackConsiderPeriod, ch->lastAttackTm);
						registerDebug(buf);
					}
					if (!isMonsterAttackingMe&&shareAlienBackattack)
					{
						// if sharing alien backattack is active, then we may attack
						// creatures which are not directly attacking us
						int count=sh_mem.GetVariablesCount();
						int pos;
						for (pos=0;pos<count;pos++)
						{
							ValueHeader valueHeader;
							sh_mem.GetValueInfo(pos,&valueHeader);
							if (strcmp(varName,(char *)valueHeader.wszValueName))
							{
								int attackTibiaId=0;
								unsigned long int len=4;
								// avoid looping with out own attack target
								sh_mem.GetValue((char *)valueHeader.wszValueName,&attackTibiaId,&len);
								if (attackTibiaId==ch->tibiaId)
								{
									if (config->debug) registerDebug("Setting isMonsterAttackingMe because of shm info");
									isMonsterAttackingMe=1;
								}
							}
						}
					}
					int visibleMonster=(ch->z==self->z&&ch->tibiaId!=self->tibiaId&&ch->tibiaId);
					
					// set backAttack
					int backAttackThisMonster=0;
					if (isMonsterAttackingMe&&config->forceAttackAfterAttack&&visibleMonster) backAttack=1,backAttackThisMonster=1;
					// set backAttackAlien
					if (backAttackThisMonster&&!creatureFoundOnList) backAttackAlien=1;
					
					int shouldAttack=1;					
					// don't attack creatures outside of the list except when backAttacking
					if (!creatureFoundOnList&&!backAttackThisMonster) shouldAttack=0;
					// don't attack creatures with '0' id				
					// don't attack myself				
					// don't attack creatures on different floors
					if (!visibleMonster) shouldAttack=0;									
					// don't attack creatures not attacking me
					if (config->attackOnlyAttacking&&!isMonsterAttackingMe) shouldAttack=0;					
					
					int xDist = abs(self->x-ch->x);
					int yDist = abs(self->y-ch->y);
					int maxDist = xDist;
					if (yDist>maxDist) maxDist=yDist;
					// don't attack creatures which are out of range
					if (maxDist>config->attackRange) shouldAttack=0;
					// don't attack monsters on list when their hp is below hpAbove%
					if (creatureFoundOnList&&ch->hpPercLeft<config->attackHpAbove) shouldAttack=0;
					
					if (!creatureFoundOnList&&self->tibiaId!=ch->tibiaId&&self->z==ch->z)
					{
						if (config->debug) registerDebug("Alien creature for training found");
						alienCreatureForTrainingFound=1;
					}									
					
					if (shouldAttack&&(!creatureDistList[maxDist]||backAttackThisMonster))
					{		
						char buf[128];
						sprintf(buf,"should attack creature %s/%d",ch->name,backAttackThisMonster);
						if (config->debug) registerDebug(buf);
						
						creatureDistList[maxDist]=ch->tibiaId;
						// this means we are backattacking alien
						creatureBackAttackList[maxDist]=(backAttackThisMonster&&!creatureFoundOnList);
						creatureFoundOnList=1;
						// put down new dist of currently attacked
						// creature for further use
						if (currentlyAttackedCreature==ch->tibiaId)
							creatureAttackNewDist=maxDist;		
						
					} else {	
						// I'm here means: don't have to attack OR dist slot already busy
						if (!shouldAttack)
						{
							if (!creatureFoundOnList&&self->tibiaId!=ch->tibiaId&&self->z==ch->z)
							{
								char buf[128];
								sprintf(buf,"Alien creature found: %s",ch->name);
								if (config->debug) registerDebug(buf);
								alienCreatureFound=1;
							}
						}
					}
					
					// increment 'amount of creatures attacking me' counter
					if (isMonsterAttackingMe) attackingCreatureCount++;
				} // creature is NOT on the ignore list
			}
			delete ch;
		};		
		if (!backAttackAlien&&shareAlienBackattack)
		{
			int zero=0;
			if (config->debug) registerDebug("Resetting back attack shmem info");
			sh_mem.SetValue(varName,&zero,4);
		}

		/**
		  * If alien creature found in the hunting area (e.g. player), then
		  * do not attack our creature, cause it can anger other players
		  */
		char debugBuf[128];
		sprintf(debugBuf,"Before main attack if: alien=%d, suspend=%d, globalAttackState=%d, backAttack=%d, backAttackAlien=%d",alienCreatureFound,config->suspendOnEnemy,globalAutoAttackStateAttack,backAttack,backAttackAlien);
		if (config->debug) registerDebug(debugBuf);		

		if (backAttack||((!alienCreatureFound||!config->suspendOnEnemy)&&globalAutoAttackStateAttack!=CToolAutoAttackStateAttack_monsterUnreachable))
		{
			if (config->debug) registerDebug("Entering attack pre-execution area");
			int dist=0;
			if (config->stickToMonster)
			{
				if (creatureAttackNewDist<=config->attackRange)
				{
					if (config->debug) registerDebug("Entered stick to monster block");
					/**
					 * This means: our creature is still not far from us
					 * and we should stick to it. So let's do it and skip
					 * all the code for the new target selection by putting 
					 * the creature on the first place (as it were just near us).
					 */
					targetFound=1;
					//dist=100;
					//creatureAttackDist=creatureAttackNewDist;
					if (currentlyAttackedCreature)
					{
						if (config->debug) registerDebug("Will attack in the stick to monster mode");
						creatureDistList[0]=currentlyAttackedCreature;						
						creatureBackAttackList[0]=backAttackAlien;
					}
				}
			}

			if (config->debug) registerDebug("Before dist loop");
			for (;dist<40;dist++)
			{				
				/**
				 * creatureDistList[dist] means: there is a creature to attack 'dist' tiles away
				 * creatureBackAttackList[dist]==backAttack means: if we are backattacking then we attack only backattacked creatures
				 */
				if (creatureDistList[dist]&&backAttackAlien==creatureBackAttackList[dist])
				{
					// if we are backattacking and shareing backattack
					// with other instances is active - then register it
					// in the shm
					if (backAttackAlien&&shareAlienBackattack)
					{
						sh_mem.SetValue(varName,&creatureDistList[dist],4);
					}
					// if we are backattacking alien then maybe fire runes at them
					// note: fire runes only if my own hp is over 50%
					// to avoid uh-exhaust
					if (backAttackAlien&&config->backattackRunes&&self->hp>self->maxHp/2)
					{
						if (config->debug) registerDebug("Firing runes at enemy");
						fireRunesAgainstCreature(config,creatureDistList[dist]);
					}
					if (config->debug) registerDebug("Entering attack execution area");
					/**
					* Do the training control things.
					*/
					int attackMode=config->mode;					
					trainingCheck(config,alienCreatureForTrainingFound,attackingCreatureCount,lastAttackedCreatureHpDrop,&attackMode);
					// proceed with attacking
					reader.cancelAttackCoords();
					/**
					* Reset remaining tiles to go
					* for the client (to avoid interferences).
					*/
					reader.setRemainingTilesToGo(0);
					sender.attackMode(attackMode+1,config->autoFollow);

					if (config->debug)
					{
						dumpCreatureInfo("Creature to attack",creatureDistList[dist]);
					}
					sender.attack(creatureDistList[dist]);										
					targetFound=1;
					// decrement lastWaypPointNr for round-robin waypoint chooser
					if (targetX||targetY||targetZ)
					{
						// but only when there was some target chosen
						lastWaypointNr--;
						if (lastWaypointNr==-1)
							lastWaypointNr=waypointsCount-1;
					}
					// reset goto target
					targetX=targetY=targetZ=0;
					globalAutoAttackStateWalker=CToolAutoAttackStateWalker_notRunning;
					
					CTibiaCharacter *enemyCh = reader.getCharacterByTibiaId(creatureDistList[dist]);
					if (enemyCh)
					{
						creatureAttackDist = abs(self->x-enemyCh->x);
						if (abs(self->y-enemyCh->y)>creatureAttackDist)
							creatureAttackDist=abs(self->y-enemyCh->y);
						
						// set id of currently attacked creature
						if (currentlyAttackedCreature!=creatureDistList[dist])
						{
							if (config->debug) registerDebug("Attacking new creature");
							// start attacking new creature
							currentlyAttackedCreature=creatureDistList[dist];
							firstCreatureAttackTM=time(NULL);	
							lastAttackedCreatureHpDrop=time(NULL)-30;
							lastAttackedCreatureHp=enemyCh->hpPercLeft;						
						} else {
							if (config->debug) registerDebug("Continously attacking creature");
							// continously attacking creature												
							if (config->suspendOnNoMove&&time(NULL)-firstCreatureAttackTM>=config->unreachableAfter)
							{							
								if (creatureAttackDist>config->attackRange)
								{
									if (config->debug) registerDebug("Attacked monster is unreachable");
									// reset targetFound to 0 (we don't want to attack an unreachable monster)
									targetFound=0;
									globalAutoAttackStateAttack=CToolAutoAttackStateAttack_monsterUnreachable;
									unreachableSecondsLeft=config->suspendAfterUnreachable;
								}												
							}
							// for the blood hit control
							if (enemyCh->hpPercLeft!=lastAttackedCreatureHp)
							{
								lastAttackedCreatureHpDrop=time(NULL);
								lastAttackedCreatureHp=enemyCh->hpPercLeft;
							}						
						}
						delete enemyCh;
					}
					if (targetFound)
					{
						globalAutoAttackStateAttack=CToolAutoAttackStateAttack_attackingCreature;;
						if (config->debug) registerDebug("Attack state: attacking");
					}
					break;
				}
			} // for (dist)
			if (!targetFound&&globalAutoAttackStateAttack!=CToolAutoAttackStateAttack_monsterUnreachable)
			{				
				globalAutoAttackStateAttack=CToolAutoAttackStateAttack_notRunning;
				sender.attack(0);
				currentlyAttackedCreature=0;
				if (config->debug) registerDebug("No attack target found");
			}
		} else {			
			// send 'cancel attack'
			if (config->debug) registerDebug("Entered cancel attack");
			sender.attack(0);
			currentlyAttackedCreature=0;
			if (globalAutoAttackStateAttack==CToolAutoAttackStateAttack_monsterUnreachable)
			{
				// creature not reachable
				if (config->debug) registerDebug("Cancel attack: not reachable");
			} else {
				// alien found
				globalAutoAttackStateAttack=CToolAutoAttackStateAttack_attackingAlienFound;				
				if (config->debug) registerDebug("Cancel attack: alien found");
			}
		}		
		if (creatureAttackDist==1)
		{
			// reset "current Pos TM" - attacked creature is next to us
			currentPosTM=time(NULL);
		}
		if (lastStandingX==self->x&&lastStandingY==self->y&&lastStandingZ==self->z)
		{			
			if (config->suspendOnNoMove&&time(NULL)-currentPosTM>=config->unreachableAfter&&creatureAttackDist>1&&globalAutoAttackStateAttack==CToolAutoAttackStateAttack_attackingCreature)
			{
				// standing in the same place when attacking too long
				// and monster is > 1 cell from us
				// reset targetFound to 0 (we don't want to attack an unreachable monster)								
				globalAutoAttackStateAttack=CToolAutoAttackStateAttack_monsterUnreachable;
				unreachableSecondsLeft=config->suspendAfterUnreachable;
				targetFound=0;
				if (config->debug) registerDebug("Entering unreachable mode");
			}
		} else {
			lastStandingX=self->x;
			lastStandingY=self->y;
			lastStandingZ=self->z;
			currentPosTM=time(NULL);
		}
	
				
		if (targetX==self->x&&targetY==self->y&&targetZ==self->z)
		{		
			if (self->x==depotX&&self->y==depotY&&self->z==depotZ)
			{		
				if (config->debug) registerDebug("Depot reached");
				// depot waypoint reached!
				// then do the depot stuff
				depotDeposit(config);
			} else {		
				if (config->debug) registerDebug("Waypoint reached");
				// normal waypoint reached
				targetX=targetY=targetZ=0;
				walkerStandingEndTm=time(NULL)+config->standStill;
			}
		}
		if (!targetFound)
		{
			// check whether we should not stand in place?
			if (time(NULL)<walkerStandingEndTm)
			{
				globalAutoAttackStateWalker=CToolAutoAttackStateWalker_standing;
				if (config->debug) registerDebug("Standing");
				continue;
			}

			// no target to attack found - go somewhere else						
			if (!targetX&&!targetY&&!targetZ)
			{
				if (config->debug) registerDebug("Looking for a new waypoint");
				if (depotX&&depotY&&depotZ)
				{
					// if we are walking to a depot - then just go there
					targetX=depotX;
					targetY=depotY;
					targetZ=depotZ;
					if (config->debug) registerDebug("Walking to a depot");
				} else {
					
					// find a new goto target from waypoint list			
					if (!waypointsCount)
					{	
						globalAutoAttackStateWalker=CToolAutoAttackStateWalker_noWaypoints;
						continue;
					}
					int newWaypoint = 0;
					switch (config->waypointSelectMode)
					{
					case 0: // random
						newWaypoint=waypointsCount?(rand()%waypointsCount):0;
						break;
					case 1: // round-robin
						newWaypoint=lastWaypointNr++;
						if (lastWaypointNr==waypointsCount)
							lastWaypointNr=0;
						break;
					}
					
					targetX=config->waypointList[newWaypoint].x;
					targetY=config->waypointList[newWaypoint].y;
					targetZ=config->waypointList[newWaypoint].z;
					char buf[128];
					sprintf(buf,"Walking to a new waypoint (%d,%d,%d)",targetX,targetY,targetZ);
					if (config->debug) registerDebug(buf);
				}
				
			};
			if (targetX&&targetY)
			{	
				int mapUsed=config->mapUsed;
				if (targetZ!=self->z&&config->mapUsed==2)
					config->mapUsed=0;
				switch (mapUsed)
				{
				case 0: // Tibia Auto map chosen
					{
						char buf[128];
						int path[15];
						delete self;
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
						if (pathSize)
						{
							CModuleUtil::executeWalk(self->x,self->y,self->z,path);														
							globalAutoAttackStateWalker=CToolAutoAttackStateWalker_ok;
							if (config->debug) registerDebug("Walking: execute walk");
						} else {							
							// if no path found - then we forget current target
							targetX=targetY=targetZ=0;
							globalAutoAttackStateWalker=CToolAutoAttackStateWalker_noPathFound;							
							if (config->debug) registerDebug("Walking: no path found");
						}
					} // case 0
					break;
				case 1: // Tibia client map chosen
					{									
						if (self->z==targetZ)
						{
							globalAutoAttackStateWalker=CToolAutoAttackStateWalker_ok;
							reader.writeGotoCoords(targetX,targetY,targetZ);
						} else {
							if (config->debug) registerDebug("Can't change levels when using tibia client map :(");
							globalAutoAttackStateWalker=CToolAutoAttackStateWalker_noPathFound;
						}
					} // case 1
					break;
				}
			}
		} // if (!targetFound)

		delete self;
		if (config->debug) registerDebug("End cavebot loop");
				
	}
	if (shareAlienBackattack)
	{
		int zero=0;		
		sh_mem.SetValue(varName,&zero,4);
	}
	// cancel attacks
	sender.attack(0);
	// restore attack mode
	sender.attackMode(reader.getPlayerModeAttackType(),reader.getPlayerModeFollow());
	
	reader.setRemainingTilesToGo(0);
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

CMod_cavebotApp::CMod_cavebotApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_cavebotApp::~CMod_cavebotApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_cavebotApp::getName()
{
	return "Cave bot";
}


int CMod_cavebotApp::isStarted()
{
	return m_started;
}


void CMod_cavebotApp::start()
{	
	superStart();
	if (m_configDialog)
	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

	if (m_configData->gatherLootStats)
	{
		FILE *f=fopen("tibiaauto-stats-loot.txt","r");
		
		if (f)
			
		{
			fseek(f,0,SEEK_END);
			
			int flen=ftell(f);
			fclose(f);		
			if (flen>1024*100)
			{
				CSendStats info;
				info.DoModal();				
			}		
		}
	}

	DWORD threadId;
		
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_cavebotApp::stop()
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

void CMod_cavebotApp::showConfigDialog()
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


void CMod_cavebotApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_cavebotApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_cavebotApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_cavebotApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_cavebotApp::getVersion()
{
	return "2.14";
}


int CMod_cavebotApp::validateConfig(int showAlerts)
{
	if (m_configData->capacityLimit<0)
	{
		if (showAlerts) AfxMessageBox("Capacity limit must be >=0!");
		return 0;
	}
	if (m_configData->attackRange<0||m_configData->attackRange>8)
	{
		if (showAlerts) AfxMessageBox("Attack range must be between 0 and 8!");
		return 0;
	}
	if (m_configData->suspendAfterUnreachable<0)
	{
		if (showAlerts) AfxMessageBox("'suspend after unreachable' must be >=0!");
		return 0;
	}
	if (m_configData->unreachableAfter<0)
	{
		if (showAlerts) AfxMessageBox("'unreachable after' must be >= 0!");
		return 0;
	}
	if (m_configData->standStill<0)
	{
		if (showAlerts) AfxMessageBox("Stand still must be >=0!");
		return 0;
	}
	if (m_configData->attackHpAbove<0||m_configData->attackHpAbove>100)
	{
		if (showAlerts) AfxMessageBox("Attack hp above % must be between 0 and 100!");
		return 0;
	}
	if (m_configData->depotCap<0) 
	{
		if (showAlerts) AfxMessageBox("Depot capacity must be >= 0!");
		return 0;
	}


	
	return 1;
}

void CMod_cavebotApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_cavebotApp::loadConfigParam(char *paramName,char *paramValue)
{	
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

	if (!strcmp(paramName,"walker/waypoint")) 
	{
		if (m_currentWaypointNr==0)
		{
			int i;
			for (i=0;i<1000;i++)
			{
				m_configData->waypointList[i].x=0;
			}
		}
		sscanf(paramValue,"%d,%d,%d",
			&m_configData->waypointList[m_currentWaypointNr].x,
			&m_configData->waypointList[m_currentWaypointNr].y,
			&m_configData->waypointList[m_currentWaypointNr].z);
		m_currentWaypointNr++;	
		
	}
	if (!strcmp(paramName,"attack/monster")) 
	{
		if (m_currentMonsterNr==0)
		{
			m_configData->monsterCount=0;
		}
		strcpy(m_configData->monsterList[m_currentMonsterNr],paramValue);		
		m_configData->monsterCount++;
		m_currentMonsterNr++;
	}
	if (!strcmp(paramName,"attack/ignore")) 
	{
		if (m_currentIgnoreNr==0)
		{
			m_configData->ignoreCount=0;
		}
		strcpy(m_configData->ignoreList[m_currentIgnoreNr],paramValue);		
		m_configData->ignoreCount++;
		m_currentIgnoreNr++;
	}
	if (!strcmp(paramName,"depot/entry"))
	{
		if (m_currentDepotEntryNr==0)
		{
			int i;
			for (i=0;i<100;i++)
				m_configData->depotTrigger[i].itemName[0]='\0';						
		}
		sscanf(paramValue,"%d,%d,%[^#]",&m_configData->depotTrigger[m_currentDepotEntryNr].when,&m_configData->depotTrigger[m_currentDepotEntryNr].remain,m_configData->depotTrigger[m_currentDepotEntryNr].itemName);
		m_currentDepotEntryNr++;
	}
}

char *CMod_cavebotApp::saveConfigParam(char *paramName)
{
	
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

	if (!strcmp(paramName,"attack/monster")&&m_currentMonsterNr<m_configData->monsterCount) 
	{

		strcpy(buf,m_configData->monsterList[m_currentMonsterNr]);
		m_currentMonsterNr++;	
	}
	if (!strcmp(paramName,"attack/ignore")&&m_currentIgnoreNr<m_configData->ignoreCount) 
	{

		strcpy(buf,m_configData->ignoreList[m_currentIgnoreNr]);
		m_currentIgnoreNr++;	
	}
	if (!strcmp(paramName,"walker/waypoint")&&m_configData->waypointList[m_currentWaypointNr].x) 
	{

		sprintf(buf,"%d,%d,%d",m_configData->waypointList[m_currentWaypointNr].x,m_configData->waypointList[m_currentWaypointNr].y,m_configData->waypointList[m_currentWaypointNr].z);
		m_currentWaypointNr++;
	}
	if (!strcmp(paramName,"depot/entry")&&strlen(m_configData->depotTrigger[m_currentDepotEntryNr].itemName)&&m_currentDepotEntryNr<100)
	{
		sprintf(buf,"%d,%d,%s",m_configData->depotTrigger[m_currentDepotEntryNr].when,m_configData->depotTrigger[m_currentDepotEntryNr].remain,m_configData->depotTrigger[m_currentDepotEntryNr].itemName);
		m_currentDepotEntryNr++;
	}

	if (!strcmp(paramName,"training/weaponTrain")) sprintf(buf,"%d",m_configData->weaponTrain);
	if (!strcmp(paramName,"training/weaponFight")) sprintf(buf,"%d",m_configData->weaponFight);
	if (!strcmp(paramName,"training/fightWhenSurrounded")) sprintf(buf,"%d",m_configData->fightWhenSurrounded);
	if (!strcmp(paramName,"training/fightWhenAlien")) sprintf(buf,"%d",m_configData->fightWhenAlien);
	if (!strcmp(paramName,"training/bloodHit")) sprintf(buf,"%d",m_configData->bloodHit);
	if (!strcmp(paramName,"training/activate")) sprintf(buf,"%d",m_configData->trainingActivate);
	if (!strcmp(paramName,"depot/depotDropInsteadOfDepositon")) sprintf(buf,"%d",m_configData->depotDropInsteadOfDepositon);
	if (!strcmp(paramName,"depot/depotCap")) sprintf(buf,"%d",m_configData->depotCap);
	
	
	return buf;

}

char *CMod_cavebotApp::getConfigParamName(int nr)
{
	switch (nr)
	{
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
	

	default:
		return NULL;
	}
}

int CMod_cavebotApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName,"walker/waypoint")) return 1;
	if (!strcmp(paramName,"attack/monster")) return 1;
	if (!strcmp(paramName,"depot/entry")) return 1;
	if (!strcmp(paramName,"attack/ignore")) return 1;
	return 0;
}

void CMod_cavebotApp::resetMultiParamAccess(char *paramName)
{
	if (!strcmp(paramName,"walker/waypoint")) m_currentWaypointNr=0;
	if (!strcmp(paramName,"attack/monster")) m_currentMonsterNr=0;
	if (!strcmp(paramName,"depot/entry")) m_currentDepotEntryNr=0;
	if (!strcmp(paramName,"attack/ignore")) m_currentIgnoreNr=0;
}
