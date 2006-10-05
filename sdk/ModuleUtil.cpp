
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



// ModuleUtil.cpp: implementation of the CModuleUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ModuleUtil.h"

#include "MemReaderProxy.h"
#include "TibiaContainer.h"
#include "TibiaItem.h"
#include "Queue.h"
#include "TibiaMapProxy.h"
#include "TibiaItemProxy.h"
#include "PackSenderProxy.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModuleUtil::CModuleUtil()
{

}

CModuleUtil::~CModuleUtil()
{

}

CTibiaItem * CModuleUtil::lookupItem(int containerNr, CUIntArray *itemsAccepted)
{
	CMemReaderProxy reader;
	int foodContNr=-1;	
	 
	int itemNr;
	
	CTibiaContainer *container = reader.readContainer(containerNr);
	if (container->flagOnOff&&container->itemsInside>0)
	{		
        for (itemNr=container->itemsInside-1;itemNr>=0;itemNr--)		
		{			
			CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);
			
			int pos;
			for (pos=0;pos<itemsAccepted->GetSize();pos++)
			{
				if ((int)item->objectId==(int)itemsAccepted->GetAt(pos))
				{
					// item found!
					CTibiaItem *retItem = new CTibiaItem();				
					retItem->pos=itemNr;
					retItem->objectId=item->objectId;
					retItem->quantity=item->quantity;				
					
					delete container;
					return retItem;
				}
			};
		};				
	}
	
	delete container;
	return NULL;

}

CTibiaItem * CModuleUtil::lookupItem(int containerNr, CUIntArray *itemsAccepted,int qty)
{	
	CMemReaderProxy reader;
	int foodContNr=-1;	
	 
	int itemNr;
	
	CTibiaContainer *container = reader.readContainer(containerNr);
	if (container->flagOnOff&&container->itemsInside>0)
	{				
		for (itemNr=container->itemsInside-1;itemNr>=0;itemNr--)		
		{			
			CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);
			
			int pos;
			for (pos=0;pos<itemsAccepted->GetSize();pos++)
			{
				if ((int)item->objectId==(int)itemsAccepted->GetAt(pos)&&item->quantity==qty)
				{
					// item found!
					CTibiaItem *retItem = new CTibiaItem();				
					retItem->pos=itemNr;
					retItem->objectId=item->objectId;
					retItem->quantity=item->quantity;				
					
					delete container;
					return retItem;
				}
			};
		};	
	}			
	
	delete container;
	return NULL;
}


void CModuleUtil::waitForItemsInsideChange(int contNr, int origItemsCount)
{
	CMemReaderProxy reader;
	int t;
	for (t=0;t<20;t++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);
		if (cont->itemsInside!=origItemsCount)
		{
			delete cont;
			return;
		}
		delete cont;
		Sleep(100);
	}
	return;
}

void CModuleUtil::findPathOnMapProcessPoint(CQueue *queue,int prevX,int prevY, int prevZ, int newX, int newY, int newZ)
{
	CTibiaMapProxy tibiaMap;
	if (tibiaMap.isPointAvailable(newX,newY,newZ)&&tibiaMap.getPrevPointX(newX,newY,newZ)==0)
	{
		int updown=tibiaMap.getPointUpDown(newX,newY,newZ);				
		queue->add(newX,newY,newZ);
		tibiaMap.setPrevPoint(newX,newY,newZ,prevX,prevY,prevZ);				
	}
}

struct point CModuleUtil::findNearestDepot(int startX, int startY, int startZ)
{
	return findNearestDepot(startX,startY,startZ,0);
}

struct point CModuleUtil::findNearestDepot(int startX, int startY, int startZ, int useDiagonal)
{
	CPackSenderProxy sender;
	prepareProhPointList();

	CTibiaMapProxy tibiaMap;
	CQueue *queue = new CQueue();
	point p;
	p.x=startX;
	p.y=startY;
	p.z=startZ;
	queue->add(p);
	tibiaMap.clearPrevPoint();


	while (queue->size())
	{
		point currentPoint = queue->getFirst();
		int x=currentPoint.x;
		int y=currentPoint.y;
		int z=currentPoint.z;
		
		/**
		 * Check whether we are standing next to a depot (updown==301).
		 */
		if (tibiaMap.getPointUpDown(x+1,y,z)==301||
			tibiaMap.getPointUpDown(x-1,y,z)==301||
			tibiaMap.getPointUpDown(x,y+1,z)==301||
			tibiaMap.getPointUpDown(x,y-1,z)==301)
		{			


			// yes, we are standing next to a depot
			delete queue;			
			return point(x,y,z);
		}

		/**
		* 101 - open hole
		* 102 - closed hole
		* 103 - crate
		* 201 - rope
		* 202 - magic rope
		* 203 - ladder
		* 204 - stairs
		*/

		/**
		 * For each direction - check whether the next point is available on the map
		 * and whether it has not been visited before.
		 */				
		int updownMode=0;		
		
		
		if (tibiaMap.isPointAvailable(x,y,z+1)&&tibiaMap.getPrevPointX(x,y,z+1)==0&&
			tibiaMap.getPointUpDown(x,y,z+1)>=200&&tibiaMap.getPointUpDown(x,y,z+1)<300)
		{
			// up
			queue->add(x,y,z+1);
			tibiaMap.setPrevPoint(x,y,z+1,x,y,z);	
		}		
		if (tibiaMap.isPointAvailable(x,y,z-1)&&tibiaMap.getPrevPointX(x,y,z-1)==0&&
			tibiaMap.getPointUpDown(x,y,z-1)>=100&&tibiaMap.getPointUpDown(x,y,z-1)<200)
		{
			// down: crate, closed hole, open hole + rope, open hole + magic rope, open hole + ladder
			queue->add(x,y,z-1);
			tibiaMap.setPrevPoint(x,y,z-1,x,y,z);
		}	
		if (tibiaMap.getPointUpDown(x,y,z)==101||tibiaMap.getPointUpDown(x,y,z)==204)
		{
			if (tibiaMap.getPrevPointZ(x,y,z)==z)
			{
				updownMode=1;
			}
		}		
		if (!updownMode)
		{
			// if we stand on open hole or stairs we can only change z coordinate
			findPathOnMapProcessPoint(queue,x,y,z,x+1,y,z);
			findPathOnMapProcessPoint(queue,x,y,z,x-1,y,z);
			findPathOnMapProcessPoint(queue,x,y,z,x,y+1,z);
			findPathOnMapProcessPoint(queue,x,y,z,x,y-1,z);
			
			if (useDiagonal)
			{
				findPathOnMapProcessPoint(queue,x,y,z,x+1,y+1,z);
				findPathOnMapProcessPoint(queue,x,y,z,x-1,y+1,z);
				findPathOnMapProcessPoint(queue,x,y,z,x+1,y-1,z);
				findPathOnMapProcessPoint(queue,x,y,z,x-1,y-1,z);
			}
		}
	}
	
	delete queue;
	if (!useDiagonal)
	{
		return findNearestDepot(startX,startY,startZ,1);
	} else {
		return point(0,0,0);
	}
}

void CModuleUtil::findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int path[15])
{	
	findPathOnMap(startX,startY,startZ,endX,endY,endZ,path,0);
}

void CModuleUtil::findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int path[15],int useDiagonal)
{	
	prepareProhPointList();

	CTibiaMapProxy tibiaMap;
	CQueue *queue = new CQueue();

	point p;
	p.x=endX;
	p.y=endY;
	p.z=endZ;

	queue->add(p);

	tibiaMap.clearPrevPoint();
	path[0]=0;
	memset(path,0x00,sizeof(int)*15);
		
	while (queue->size())
	{	
		point currentPoint = queue->getFirst();
		//sprintf(buf,"queue size = %d; cur=(%d,%d,%d)",queue->size(),currentPoint.x,currentPoint.y,currentPoint.z);
		//AfxMessageBox(buf);
		if (currentPoint.x==startX&&currentPoint.y==startY&&currentPoint.z==startZ)
		{						
			//sprintf(buf,"find path (%d,%d,%d)->(%d,%d,%d)",startX,startY,startZ,endX,endY,endZ);
			//AfxMessageBox(buf);
			//AfxMessageBox("got it");
			int pos=0;
			while (!(currentPoint.x==endX&&currentPoint.y==endY&&currentPoint.z==endZ)&&pos<10)
			{
				int nextX = tibiaMap.getPrevPointX(currentPoint.x,currentPoint.y,currentPoint.z);
				int nextY = tibiaMap.getPrevPointY(currentPoint.x,currentPoint.y,currentPoint.z);
				int nextZ = tibiaMap.getPrevPointZ(currentPoint.x,currentPoint.y,currentPoint.z);
				if (currentPoint.x+1==nextX&&currentPoint.y==nextY&&currentPoint.z==nextZ) path[pos++]=1; // right
				if (currentPoint.x-1==nextX&&currentPoint.y==nextY&&currentPoint.z==nextZ) path[pos++]=5; // left
				if (currentPoint.x==nextX&&currentPoint.y+1==nextY&&currentPoint.z==nextZ) path[pos++]=7; // down
				if (currentPoint.x==nextX&&currentPoint.y-1==nextY&&currentPoint.z==nextZ) path[pos++]=3; // up

				if (currentPoint.x+1==nextX&&currentPoint.y+1==nextY&&currentPoint.z==nextZ) path[pos++]=8; // down-right
				if (currentPoint.x-1==nextX&&currentPoint.y+1==nextY&&currentPoint.z==nextZ) path[pos++]=6; // down-left
				if (currentPoint.x+1==nextX&&currentPoint.y-1==nextY&&currentPoint.z==nextZ) path[pos++]=2; // up-right
				if (currentPoint.x-1==nextX&&currentPoint.y-1==nextY&&currentPoint.z==nextZ) path[pos++]=4; // up-left

				if (currentPoint.z==nextZ+1)
				{
					// go to up point - if we are there return the special 
					// up code; otherwise just walk there
					if (pos==0)
					{
						path[pos++]=0xD0;
					}					
					return;
				}
				if (currentPoint.z==nextZ-1)
				{
					// go to down point - if we are there return the special 
					// down code; otherwise just walk there
					if (pos==0)
					{
						path[pos++]=0xD1;
					} 					
					return;
				}
				currentPoint.x=nextX;
				currentPoint.y=nextY;
				currentPoint.z=nextZ;
			};				
			
			return;
		}
		
		/**
		 * For each direction - check whether the next point is available on the map
		 * and whether it has not been visited before.
		 */		
		int x=currentPoint.x;
		int y=currentPoint.y;
		int z=currentPoint.z;
		int updownMode=0;		



		/**
		 * 101 - open hole
		 * 102 - closed hole
		 * 103 - crate
		 * 201 - rope
		 * 202 - magic rope
		 * 203 - ladder
		 * 204 - stairs
		 */

		             
		if (tibiaMap.isPointAvailable(x,y,z+1)&&tibiaMap.getPrevPointX(x,y,z+1)==0&&
			tibiaMap.getPointUpDown(x,y,z+1)>=200&&tibiaMap.getPointUpDown(x,y,z+1)<300)
		{
			// up
			queue->add(x,y,z+1);
			tibiaMap.setPrevPoint(x,y,z+1,x,y,z);			
		}		
		if (tibiaMap.isPointAvailable(x,y,z-1)&&tibiaMap.getPrevPointX(x,y,z-1)==0&&
			tibiaMap.getPointUpDown(x,y,z-1)>=100&&tibiaMap.getPointUpDown(x,y,z-1)<200)
		{
			// down: crate, closed hole, open hole + rope, open hole + magic rope, open hole + ladder
			queue->add(x,y,z-1);
			tibiaMap.setPrevPoint(x,y,z-1,x,y,z);			
		}	
		
		
		
		if (tibiaMap.getPointUpDown(x,y,z)==101||tibiaMap.getPointUpDown(x,y,z)==204)
		{
			if (tibiaMap.getPrevPointZ(x,y,z)==z)
			{
				updownMode=1;
			}
		}		
		if (!updownMode)
		{
			// if we stand on open hole or stairs we can only change z coordinate
			findPathOnMapProcessPoint(queue,x,y,z,x+1,y,z);
			findPathOnMapProcessPoint(queue,x,y,z,x-1,y,z);
			findPathOnMapProcessPoint(queue,x,y,z,x,y+1,z);
			findPathOnMapProcessPoint(queue,x,y,z,x,y-1,z);
			
			if (useDiagonal)
			{
				findPathOnMapProcessPoint(queue,x,y,z,x+1,y+1,z);
				findPathOnMapProcessPoint(queue,x,y,z,x-1,y+1,z);
				findPathOnMapProcessPoint(queue,x,y,z,x+1,y-1,z);
				findPathOnMapProcessPoint(queue,x,y,z,x-1,y-1,z);
			}
		}
	}

	delete queue;	

	if (!useDiagonal)
		findPathOnMap(startX,startY,startZ,endX,endY,endZ,path,1);

}


int CModuleUtil::waitForOpenContainer(int contNr, int open)
{
	CMemReaderProxy reader;
	int t;
	for (t=0;t<20;t++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);
		if (cont->flagOnOff==open)
		{
			delete cont;
			return 1;
		}
		delete cont;
		Sleep(100);
	}
	return 0;
}

void CModuleUtil::lootItemFromContainer(int contNr, CUIntArray *acceptedItems)
{	
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItem *item = CModuleUtil::lookupItem(contNr,acceptedItems);
	CTibiaCharacter *self = reader.readSelfCharacter();

	if (item)
	{
		// find first free container
		int openCont;
		for (openCont=0;openCont<contNr;openCont++)
		{
			CTibiaContainer *targetCont = reader.readContainer(openCont);
			if (targetCont->flagOnOff&&targetCont->itemsInside<targetCont->size)
			{					
				//sender.sendTAMessage("[debug] target container found");
				// good, I can move to openCont
				sender.moveObjectBetweenContainers(item->objectId,0x40+contNr,item->pos,0x40+openCont,0,item->quantity?item->quantity:1);
				
				// wait for sync
				int t3=0;
				for (t3=0;t3<20;t3++)
				{
					CTibiaCharacter *selfNew = reader.readSelfCharacter();
					
					if (selfNew->cap!=self->cap)
					{
						// sync on, stop scanning containers
						delete selfNew;
						openCont=10;
						break;
					}
					
					delete selfNew;
					Sleep(100);
				}									
			}
			
			
			delete targetCont;						
		};
		delete item;
	}
	delete self;
}

void CModuleUtil::sleepWithStop(int ms,int *stopFlag)
{
	while (ms>50&&!(*stopFlag))
	{
		Sleep(50);		
		ms-=50;
	}
	if (*stopFlag) return;
	Sleep(ms);
}

void CModuleUtil::eatItemFromContainer(int contNr)
{	
	CTibiaItemProxy itemProxy;
	CUIntArray acceptedItems;
	
	int p;
	for (p=0;p<itemProxy.getItemsFoodArray()->GetSize();p++)
	{
		acceptedItems.Add(itemProxy.getItemsFoodArray()->GetAt(p));
	}
	
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaItem *item = CModuleUtil::lookupItem(contNr,&acceptedItems);

	if (item)
	{
		sender.useItemInContainer(item->objectId,0x40+contNr,item->pos);
		delete item;
	}
}

void CModuleUtil::waitForCreatureDisappear(int x,int y, int tibiaId)
{
	CMemReaderProxy reader;
	int iterCount=20;
	while (iterCount-->0)
	{
		int found=0;
		int pos;
		int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
		for (pos=0;pos<stackCount;pos++)
		{
			int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
			int extraData = reader.mapGetPointItemExtraInfo(point(x,y,0),pos,1);
			if (tileId==99&&extraData==tibiaId) 
				found=1;
		}
		if (!found)
			return;
		Sleep(100);
	}
}

int CModuleUtil::calcLootChecksum(int tm, int killNr, int nameLen, int itemNr, int objectId, int qty, int lootInBags)
{
	return tm*3+killNr*17+nameLen*3+itemNr*15+objectId*19+qty*13+lootInBags*71;
}

void CModuleUtil::executeWalk(int path[15])
{
	CPackSenderProxy sender;
	CTibiaMapProxy tibiaMap;
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	int pathSize;
	for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){}										
	if (pathSize==1&&(path[0]==0xD0||path[0]==0xD1))
	{
		if (path[0]==0xD0)
								{
			// // go up (201-299 slots)
			switch (tibiaMap.getPointUpDown(self->x,self->y,self->z))
			{		
			case 201:
				// rope
				{
					CUIntArray itemsAccepted;
					int contNr;
					itemsAccepted.Add(itemProxy.getValueForConst("rope"));
					for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
					{
						CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemsAccepted);
						if (item)
						{
							sender.useWithObjectFromContainerOnFloor(itemProxy.getValueForConst("rope"),0x40+contNr,item->pos,itemProxy.getValueForConst("ropespot"),self->x,self->y,self->z,2,1);
							delete item;
							break;
						}
					}
				}
				break;
			case 202:
				// go up using 'magic rope' [for pacc only]
				sender.say("exani tera");
				break;								
			case 203:
				sender.useItemOnFloor(itemProxy.getValueForConst("ladder"),self->x,self->y,self->z);
				// ladder
				break;
			case 204:
				// stairs [do nothing]
				break;
			}		
		} else {
			// go down (101-199 slots)
			switch (tibiaMap.getPointUpDown(self->x,self->y,self->z))
			{		
			case 101:
				// open hole [do nothing]
				break;
			case 102:
				// closed hole - use shovel to open
				{					
					CTibiaItem *shovel = NULL;
					CUIntArray itemsAccepted;
					int contNr;
					itemsAccepted.Add(itemProxy.getValueForConst("shovel"));
					for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
					{
						CTibiaItem *item = CModuleUtil::lookupItem(contNr,&itemsAccepted);
						if (item)
						{
							shovel=item;							
							break;
						}
						delete item;
					}									
					if (shovel)
					{												
						// shovel found, so proceed with opening hole
						int path[2];												
						int mode=0;
						if (tibiaMap.isPointAvailable(self->x-1,self->y,self->z)) mode=1;
						if (tibiaMap.isPointAvailable(self->x+1,self->y,self->z)) mode=2;
						if (tibiaMap.isPointAvailable(self->x,self->y-1,self->z)) mode=3;
						if (tibiaMap.isPointAvailable(self->x,self->y+1,self->z)) mode=4;
						path[1]=0;						
						switch (mode)
						{
						case 1: path[0]=5;break;
						case 2: path[0]=1;break;
						case 3: path[0]=3;break;
						case 4: path[0]=7;break;						
						}												
						sender.stepMulti(path,1);
						Sleep(1000);
						CTibiaCharacter *self2 = reader.readSelfCharacter();						
						int count=reader.mapGetPointItemsCount(point(self->x-self2->x,self->y-self2->y,0));
						
						int blocked=0;
						int updown=0;
						int i;
						for (i=0;i<count;i++)
						{						
							int tileId = reader.mapGetPointItemId(point(self->x-self2->x,self->y-self2->y,0),i);
							//char buf[128];
							//sprintf(buf,"[%d/%d] me (%d,%d,%d) opening (%d,%d,%d) tile %d",i,count,self2->x,self2->y,self2->z,self->x,self->y,self->z,tileId);
							//::MessageBox(0,buf,buf,0);
							if (tileId!=99)
							{
								CTibiaTile *tile = reader.getTibiaTile(tileId);								
								
								if (tile->requireShovel)
								{										
									sender.useWithObjectFromContainerOnFloor(itemProxy.getValueForConst("shovel"),0x40+contNr,shovel->pos,tileId,self->x,self->y,self->z,2,0);
									break;
								}
							}
						}
						delete self2;
						Sleep(1000);
						switch (mode)
						{
						case 2: path[0]=5;break;
						case 1: path[0]=1;break;
						case 4: path[0]=3;break;
						case 3: path[0]=7;break;						
						}
						sender.stepMulti(path,1);
						Sleep(1000);
						delete shovel;
					}
				}
				break;
			case 103:
				// crate - use to go down
				sender.useItemOnFloor(itemProxy.getValueForConst("crate"),self->x,self->y,self->z);
				break;
			}
			
		}
		// after going up wait 3 seconds to have sync with server
		Sleep(3000);
	} else {
		// 'normal' stepping
		sender.stepMulti(path,pathSize);												
	}
	delete self;
}

void CModuleUtil::prepareProhPointList()
{
	CTibiaMapProxy tibiaMap;	
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	// prepare prohPoint list
	tibiaMap.prohPointClear();
	int creatureNr;
	for (creatureNr=0;creatureNr<memConstData.m_memMaxCreatures;creatureNr++)
	{
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);
		if (ch->visible&&(ch->x!=self->x||ch->y!=self->y||ch->z!=self->z))
		{
			tibiaMap.prohPointAdd(ch->x,ch->y,ch->z);
		}
		delete ch;
	}
	delete self;
}