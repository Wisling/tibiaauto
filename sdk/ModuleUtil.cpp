
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

//#define MAPDEBUG

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
		queue->add(newX,newY,newZ);
		tibiaMap.setPrevPoint(newX,newY,newZ,prevX,prevY,prevZ);				
	}
}



void inline mapDebug(char *s)
{	
#ifndef MAPDEBUG
	return;
#endif
	FILE *f=fopen("tibiaauto-debug-map.txt","a+");
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


void inline testDebug(char *s)
{	
	FILE *f=fopen("tibiaauto-debug-test.txt","a+");
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



struct point CModuleUtil::findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int endSpecialLocation,int path[15])
{		
	struct point retPoint(0,0,0);

	path[0]=0;
	memset(path,0x00,sizeof(int)*15);
#ifdef MAPDEBUG
	char buf[128];
	sprintf(buf,"findPathOnMap(startX=%d,startY=%d,startZ=%d,endX=%d,endY=%d,endZ=%d,endSpecialLocation=%d",startX,startY,startZ,endX,endY,endZ,endSpecialLocation);
	mapDebug(buf);
#endif

	// means: we are already on the target location
	if (startX==endX&&startY==endY&&startZ==endZ) return retPoint;

	int x,y;
	prepareProhPointList();
	CTibiaMapProxy tibiaMap;

	// try to go to the point normally
	retPoint=findPathOnMap(startX,startY,startZ,endX,endY,endZ,endSpecialLocation,path,0);
	if (path[0]) return retPoint;

	// try to go to the point using also diagonals
	retPoint=findPathOnMap(startX,startY,startZ,endX,endY,endZ,endSpecialLocation,path,1);
	if (path[0]) return retPoint;

	// try to go to some point nearby
	for (x=-1;x<=1;x++)
	{
		for (y=-1;y<=1;y++)
		{	
			if (x||y)
			{
#ifdef MAPDEBUG
				sprintf(buf,"trampoline (%d,%d,%d)->(%d,%d,%d)",startX,startY,startZ,endX+x,endY+y,endZ);
				mapDebug(buf);
#endif
				retPoint=findPathOnMap(startX,startY,startZ,endX+x,endY+y,endZ,endSpecialLocation,path,0);
				if (path[0]) return retPoint;	
				retPoint=findPathOnMap(startX,startY,startZ,endX+x,endY+y,endZ,endSpecialLocation,path,1);
				if (path[0]) return retPoint;		
			}
		}
	}
	return retPoint;
}



void CModuleUtil::findPathAllDirection(CQueue *queue,int x,int y,int z,int updownMode,int useDiagonal)
{

		// if we stand on open hole or stairs we can only change z coordinate
		findPathOnMapProcessPoint(queue,x,y,z,x+1,y,z+updownMode);
		findPathOnMapProcessPoint(queue,x,y,z,x-1,y,z+updownMode);
		findPathOnMapProcessPoint(queue,x,y,z,x,y+1,z+updownMode);
		findPathOnMapProcessPoint(queue,x,y,z,x,y-1,z+updownMode);
			
		if (useDiagonal)
		{
			findPathOnMapProcessPoint(queue,x,y,z,x+1,y+1,z+updownMode);
			findPathOnMapProcessPoint(queue,x,y,z,x-1,y+1,z+updownMode);
			findPathOnMapProcessPoint(queue,x,y,z,x+1,y-1,z+updownMode);
			findPathOnMapProcessPoint(queue,x,y,z,x-1,y-1,z+updownMode);
		}
}

#define MAX_PATH_LEN 10000
point pathTab[MAX_PATH_LEN];
int pathTabLen;
int lastEndX=-1,lastEndY=-1,lastEndZ=-1;

struct point CModuleUtil::findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int endSpecialLocation, int path[15],int useDiagonal)
{	
	CPackSenderProxy sender;
	CTibiaMapProxy tibiaMap;
		

	path[0]=0;
	memset(path,0x00,sizeof(int)*15);

	// unable to reach point which is not available (e.g. other creature standing on it)
	if (!endSpecialLocation&&!tibiaMap.isPointAvailable(endX,endY,endZ)) return point(0,0,0);
	
	
	CQueue *queue = new CQueue();

	point p;
	p.x=startX;
	p.y=startY;
	p.z=startZ;
	

	queue->add(p);

	tibiaMap.clearPrevPoint();		
	tibiaMap.setPrevPoint(startX,startY,startZ,startX,startY,startZ);

#ifdef MAPDEBUG	
	char buf[128];	
	sprintf(buf,"============== (%d,%d,%d)->(%d,%d,%d) =========",startX,startY,startZ,endX,endY,endZ);
	mapDebug(buf);
#endif
	


	while (queue->size())
	{	
		point currentPoint = queue->getFirst();		

		int x=currentPoint.x;
		int y=currentPoint.y;
		int z=currentPoint.z;

#ifdef MAPDEBUG
		sprintf(buf,"queue: [%3d] (%d,%d,%d)",queue->size(),currentPoint.x,currentPoint.y,currentPoint.z);
		mapDebug(buf);
#endif

		int gotToEndPoint=0;
		// final point reached
		if (currentPoint.x==endX&&currentPoint.y==endY&&currentPoint.z==endZ) gotToEndPoint=1;
		// standing by the depot
		if (endSpecialLocation&&
			(tibiaMap.getPointUpDown(x+1,y,z)==endSpecialLocation||
			tibiaMap.getPointUpDown(x-1,y,z)==endSpecialLocation||
			tibiaMap.getPointUpDown(x,y+1,z)==endSpecialLocation||
			tibiaMap.getPointUpDown(x,y-1,z)==endSpecialLocation))
		{
			// endSpecialLocation found - where we stand is our "end point"
			gotToEndPoint=1;
			endX=x;
			endY=y;
			endZ=z;
		}
	


		if (gotToEndPoint)
		{				
			int pathPos=0;
			while (currentPoint.x!=startX||currentPoint.y!=startY||currentPoint.z!=startZ)
			{				
				p.x=currentPoint.x;
				p.y=currentPoint.y;
				p.z=currentPoint.z;
				pathTab[pathPos].x=p.x;
				pathTab[pathPos].y=p.y;
				pathTab[pathPos].z=p.z;

				currentPoint.x=tibiaMap.getPrevPointX(p.x,p.y,p.z);
				currentPoint.y=tibiaMap.getPrevPointY(p.x,p.y,p.z);
				currentPoint.z=tibiaMap.getPrevPointZ(p.x,p.y,p.z);
#ifdef MAPDEBUG
				sprintf(buf,"cur=(%d,%d,%d) new=(%d,%d,%d)",p.x,p.y,p.z,currentPoint.x,currentPoint.y,currentPoint.z);
				mapDebug(buf);
#endif

				pathPos++;

				if (pathPos>MAX_PATH_LEN-5) return point(0,0,0);
			};
			pathTab[pathPos].x=startX;
			pathTab[pathPos].y=startY;
			pathTab[pathPos].z=startZ;
			pathPos++;
					
#ifdef MAPDEBUG
			sprintf(buf,"111 pathPos=%d",pathPos);
			mapDebug(buf);
#endif
			int pos=0;
			for (pos=0;pos<10&&pos<pathPos-1;pos++)			
			{
				int curX = pathTab[pathPos-pos-1].x;
				int curY = pathTab[pathPos-pos-1].y;
				int curZ = pathTab[pathPos-pos-1].z;
				int nextX = pathTab[pathPos-pos-2].x;
				int nextY = pathTab[pathPos-pos-2].y;
				int nextZ = pathTab[pathPos-pos-2].z;
#ifdef MAPDEBUG
				sprintf(buf,"X cur=(%d,%d,%d) next=(%d,%d,%d)",curX,curY,curZ,nextX,nextY,nextZ);
				mapDebug(buf);
#endif
				if (curX+1==nextX&&curY==nextY&&curZ==nextZ) path[pos]=1; // right
				if (curX-1==nextX&&curY==nextY&&curZ==nextZ) path[pos]=5; // left
				if (curX==nextX&&curY+1==nextY&&curZ==nextZ) path[pos]=7; // down
				if (curX==nextX&&curY-1==nextY&&curZ==nextZ) path[pos]=3; // up

				if (curX+1==nextX&&curY+1==nextY&&curZ==nextZ) path[pos]=8; // down-right
				if (curX-1==nextX&&curY+1==nextY&&curZ==nextZ) path[pos]=6; // down-left
				if (curX+1==nextX&&curY-1==nextY&&curZ==nextZ) path[pos]=2; // up-right
				if (curX-1==nextX&&curY-1==nextY&&curZ==nextZ) path[pos]=4; // up-left

				if (curZ-1==nextZ)
				{
					// up
					if (pos==0)
					{		
						if (curX==nextX+1)
						{
							path[pos]=0xE1;
						}
						if (curX==nextX-1)
						{
							path[pos]=0xE2;
						}
						if (curY==nextY+1)
						{
							path[pos]=0xE3;
						}
						if (curY==nextY-1)
						{
							path[pos]=0xE4;
						}
						
						if (curX==nextX&&curY==nextY)
						{
							path[pos]=0xD0;
						}
					}
					break;
				}
				if (curZ+1==nextZ)
				{
					// down
					if (pos==0)
					{
						if (curX==nextX+1)
						{
							path[pos]=0xF1;
						}
						if (curX==nextX-1)
						{
							path[pos]=0xF2;
						}
						if (curY==nextY+1)
						{
							path[pos]=0xF3;
						}
						if (curY==nextY-1)
						{
							path[pos]=0xF4;
						}
						
						if (curX==nextX&&curY==nextY)
						{
							path[pos]=0xD1;
						}
					}
					break;
				}
			}
			
#ifdef MAPDEBUG
			sprintf(buf,"XXX pos=%d pathPos=%d ",pos,pathPos);
			int i;
			for (i=0;i<pos;i++)
			{
				char buf2[128];				
				sprintf(buf2," (%d)",path[i]);
				if (strlen(buf2)<100)
				{
					strcat(buf,buf2);
				}
			}
			mapDebug(buf);
#endif

			lastEndX=endX;
			lastEndY=endY;
			lastEndZ=endZ;
			pathTabLen=pathPos;
					
			return point(endX,endY,endZ);
		}
		
		
		/**
		 * For each direction - check whether the next point is available on the map
		 * and whether it has not been visited before.
		 */		

		/**
		 * 101 - open hole
		 * 102 - closed hole
		 * 103 - crate
		 * 201 - rope
		 * 202 - magic rope
		 * 203 - ladder
		 * 204 - stairs
		 */
		
		int forcedLevelChange=0; // if set to 1 then going north, south, east, west is forbidden

		#ifdef MAPDEBUG
		char buf[128];
		sprintf(buf,"current = %d,%d,%d",x,y,z);
		mapDebug(buf);
#endif
		

		int currentUpDown=tibiaMap.getPointUpDown(x,y,z);

		// we directly go up using rope, magic rope, ladder, stairs
		if (currentUpDown>=200&&currentUpDown<300&&tibiaMap.isPointAvailableNoProh(x,y,z-1)&&tibiaMap.getPrevPointX(x,y,z-1)==0)
		{						
			queue->add(x,y,z-1);
			tibiaMap.setPrevPoint(x,y,z-1,x,y,z);
			findPathAllDirection(queue,x,y,z-1,0,useDiagonal);
#ifdef MAPDEBUG
			mapDebug("go up");
#endif
		}

		// we go directly down using open hole, closed hole, crate
		if (currentUpDown>=100&&currentUpDown<200&&tibiaMap.isPointAvailableNoProh(x,y,z+1)&&tibiaMap.getPrevPointX(x,y,z+1)==0)
		{			
			queue->add(x,y,z+1);
			tibiaMap.setPrevPoint(x,y,z+1,x,y,z);
			findPathAllDirection(queue,x,y,z+1,0,useDiagonal);
#ifdef MAPDEBUG
			mapDebug("go down");
#endif
		}

		if (currentUpDown==101||currentUpDown==204)
		{
			forcedLevelChange=1;
		}

		if (!forcedLevelChange)
		{
			findPathAllDirection(queue,x,y,z,0,useDiagonal);
		}
	
		
		 int updownPZ=tibiaMap.getPointUpDownNoProh(x+1,y,z);
		 int updownZP=tibiaMap.getPointUpDownNoProh(x,y+1,z);
		 int updownMZ=tibiaMap.getPointUpDownNoProh(x-1,y,z);
		 int updownZM=tibiaMap.getPointUpDownNoProh(x,y-1,z);
	
		// special going up if lader busy
		if ((updownPZ==201||updownPZ==203)&&tibiaMap.isPointAvailableNoProh(x+1,y,z-1)&&tibiaMap.getPrevPointX(x+1,y,z-1)==0)
		{
			queue->add(x+1,y,z-1);
			tibiaMap.setPrevPoint(x+1,y,z-1,x,y,z);
			findPathAllDirection(queue,x+1,y,z,-1,useDiagonal);
		}

		if ((updownMZ==201||updownMZ==203)&&tibiaMap.isPointAvailableNoProh(x-1,y,z-1)&&tibiaMap.getPrevPointX(x-1,y,z-1)==0)
		{
			queue->add(x-1,y,z-1);
			tibiaMap.setPrevPoint(x-1,y,z-1,x,y,z);
			findPathAllDirection(queue,x-1,y,z,-1,useDiagonal);
		}

		if ((updownZP==201||updownZP==203)&&tibiaMap.isPointAvailableNoProh(x,y+1,z-1)&&tibiaMap.getPrevPointX(x,y+1,z-1)==0)
		{
			queue->add(x,y+1,z-1);
			tibiaMap.setPrevPoint(x,y+1,z-1,x,y,z);
			findPathAllDirection(queue,x,y+1,z,-1,useDiagonal);
		}

		if ((updownZM==201||updownZM==203)&&tibiaMap.isPointAvailableNoProh(x,y-1,z-1)&&tibiaMap.getPrevPointX(x,y-1,z-1)==0)
		{
			queue->add(x,y-1,z-1);
			tibiaMap.setPrevPoint(x,y-1,z-1,x,y,z);
			findPathAllDirection(queue,x,y-1,z,-1,useDiagonal);
		}

		// special going down if crate busy
		if ((updownPZ==103)&&tibiaMap.isPointAvailableNoProh(x+1,y,z+1)&&tibiaMap.getPrevPointX(x+1,y,z+1)==0)
		{
			queue->add(x+1,y,z+1);
			tibiaMap.setPrevPoint(x+1,y,z+1,x,y,z);
			findPathAllDirection(queue,x+1,y,z,1,useDiagonal);
		}

		if ((updownMZ==103)&&tibiaMap.isPointAvailableNoProh(x-1,y,z+1)&&tibiaMap.getPrevPointX(x-1,y,z+1)==0)
		{
			queue->add(x-1,y,z+1);
			tibiaMap.setPrevPoint(x-1,y,z+1,x,y,z);
			findPathAllDirection(queue,x-1,y,z,1,useDiagonal);
		}

		if ((updownZP==103)&&tibiaMap.isPointAvailableNoProh(x,y+1,z+1)&&tibiaMap.getPrevPointX(x,y+1,z+1)==0)
		{
			queue->add(x,y+1,z+1);
			tibiaMap.setPrevPoint(x,y+1,z+1,x,y,z);
			findPathAllDirection(queue,x,y+1,z,1,useDiagonal);
		}

		if ((updownZM==103)&&tibiaMap.isPointAvailableNoProh(x,y-1,z+1)&&tibiaMap.getPrevPointX(x,y-1,z+1)==0)
		{
			queue->add(x,y-1,z+1);
			tibiaMap.setPrevPoint(x,y-1,z+1,x,y,z);
			findPathAllDirection(queue,x,y-1,z,1,useDiagonal);
		}
		
	
	}

	delete queue;		
	return point(0,0,0);
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
		CTibiaTile *tile = reader.getTibiaTile(item->objectId);

		CTibiaContainer *lootedCont = reader.readContainer(contNr);
		// find first free container
		int openCont;
		for (openCont=0;openCont<8;openCont++)
		{
			
			CTibiaContainer *targetCont = reader.readContainer(openCont);			
			if (targetCont->flagOnOff)
			{					
				int itemNr;
				// if item is stackable try to attach it to an existing stack
				if (tile->stackable)
				{
					for (itemNr=0;itemNr<targetCont->itemsInside;itemNr++)
					{
						CTibiaItem *itemInside = (CTibiaItem *)targetCont->items.GetAt(itemNr);
						if (itemInside->objectId==item->objectId&&itemInside->quantity+item->quantity<=100)
						{
							sender.moveObjectBetweenContainers(item->objectId,0x40+contNr,item->pos,0x40+openCont,itemNr,item->quantity?item->quantity:1);

							waitForItemsInsideChange(contNr,lootedCont->itemsInside);

							// jump out of all loops as looting is done
							openCont=1000;	
							itemNr=1000;
						}
					}
				}

				if (openCont!=1000&&targetCont->itemsInside<targetCont->size)
				{
					//sender.sendTAMessage("[debug] target container found");
					// good, I can move to openCont
					sender.moveObjectBetweenContainers(item->objectId,0x40+contNr,item->pos,0x40+openCont,targetCont->size-1,item->quantity?item->quantity:1);
					
					waitForItemsInsideChange(contNr,lootedCont->itemsInside);
					
					// jump out of all loops as looting is done
					openCont=1000;					
				}
			}
			
			
			delete targetCont;						
		};
		delete lootedCont;
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

void CModuleUtil::executeWalk(int startX, int startY, int startZ,int path[15])
{
	static unsigned int lastExecuteWalkTm=0; // indicates when last a "real" walk was executed
	static unsigned int lastStartChangeTm=0; // indicates when start point was changed for the last time
	static int lastEndX=0,lastEndY=0,lastEndZ=0; 
	static int lastStartX=0,lastStartY=0,lastStartZ=0; 
	int currentTm=time(NULL);
	CPackSenderProxy sender;
	CTibiaMapProxy tibiaMap;
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	int pathSize;
	for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){}										
#ifdef MAPDEBUG
	char buf[128];
	sprintf(buf,"pathsize=%d assumed=(%d,%d,%d) now=(%d,%d,%d) delta=(%d,%d,%d)=%d",pathSize,startX,startY,startZ,self->x,self->y,self->z,self->x-startX,self->y-startY,self->z-startZ,abs(self->x-startX)+abs(self->y-startY)+abs(self->z-startZ));	
	mapDebug(buf);
#endif
	
	if (lastStartX!=startX||lastStartY!=startY||lastStartZ!=startZ) 
	{
		lastStartX=startX;
		lastStartY=startY;
		lastStartZ=startZ;
		lastStartChangeTm=currentTm;
	}

	if (pathSize==1&&path[0]>=0xD0)
	{
		lastEndX=lastEndY=lastEndZ=0;
		lastExecuteWalkTm=currentTm;
		switch (path[0])
		{
		case 0xD0:		
			{
				// // go up standard (201-299 slots)
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
					{
						// ladder
						// fix 02.02.2008: ladder code needs to be evaluated on the fly					
						int count=reader.mapGetPointItemsCount(point(0,0,0));
						int ladderCode=0;
						int i;
						
						for (i=0;i<count;i++)
						{						
							int tileId = reader.mapGetPointItemId(point(0,0,0),i);
							CTibiaTile *tileData = reader.getTibiaTile(tileId);
							if (tileId!=99&&tileData->goUp&&tileData->requireUse)
							{
								ladderCode=tileId;
								break;
							}
						} // for i
						if (ladderCode)
						{
							sender.useItemOnFloor(ladderCode,self->x,self->y,self->z);
						}
					}
					break;
				case 204:
					// stairs [do nothing]
					break;
				}		
				break;
			} // case 0xD0
		case 0xD1:
			{
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
				} // switch updown
			} // case 0xD1	
		case 0xE1:
		case 0xE2:
		case 0xE3:
		case 0xE4:
			{
				// go up - but one left/right/up/down
				int modX = self->x;
				int modY = self->y;
				if (path[0]==0xE1) modX--;
				if (path[0]==0xE2) modX++;
				if (path[0]==0xE3) modY--;
				if (path[0]==0xE4) modY++;
				switch (tibiaMap.getPointUpDownNoProh(modX,modY,self->z))
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
								sender.useWithObjectFromContainerOnFloor(itemProxy.getValueForConst("rope"),0x40+contNr,item->pos,itemProxy.getValueForConst("ropespot"),modX,modY,self->z,2,1);
								delete item;
								break;
							}
						}
					}
					break;									
				case 203:					
					{
						// ladder
						// fix 02.02.2008: ladder code needs to be evaluated on the fly					
						int count=reader.mapGetPointItemsCount(point(modX-self->x,modY-self->y,0));
						int ladderCode=0;
						int i;
						
						for (i=0;i<count;i++)
						{						
							int tileId = reader.mapGetPointItemId(point(modX-self->x,modY-self->y,0),i);
							CTibiaTile *tileData = reader.getTibiaTile(tileId);
							if (tileId!=99&&tileData->goUp&&tileData->requireUse)
							{
								ladderCode=tileId;
								break;
							}
						} // for i
						if (ladderCode)
						{
							sender.useItemOnFloor(ladderCode,modX,modY,self->z);
						}
					}

					// ladder
					break;				
				}		
				break;
			} // case 0xE1-E4
		case 0xF1:
		case 0xF2:
		case 0xF3:
		case 0xF4:
			{
				// go down - but one left/right/up/down
				int modX = self->x;
				int modY = self->y;
				if (path[0]==0xF1) modX--;
				if (path[0]==0xF2) modX++;
				if (path[0]==0xF3) modY--;
				if (path[0]==0xF4) modY++;

				switch (tibiaMap.getPointUpDownNoProh(modX,modY,self->z))
				{		
				case 103:
					// crate
					{
						sender.useItemOnFloor(itemProxy.getValueForConst("crate"),modX,modY,self->z);
					}
					break;													
				}		
				break;
			} // case 0xF1-F4
			
		} // switc path[0]
		// after going up wait 3 seconds to have sync with server
		Sleep(3000);
	} else {
		/**
		 * Walk if :
		 * 1. we found a way AND
		 * 2. last planned end point equals to the start point now (means: we reach "final" point) OR
		 * 3. last startX change was > 3s ago (means: we are stuck) OR
		 * 4. last walk was over 15s ago (means: walk restart, lag, etc.)
		 */
		
		int lastEndEqStart=(lastEndX==startX&&lastEndY==startY&&lastEndZ==startZ);
		if (pathSize>0&&(lastEndEqStart||currentTm-lastStartChangeTm>=2||currentTm-lastExecuteWalkTm>15))
		{
			// 'normal' stepping limited to 10 steps
			int i;
			if (pathSize>10) pathSize=10;

			sender.stepMulti(path,pathSize);												

			lastEndX=startX;
			lastEndY=startY;
			lastEndZ=startZ;
			lastExecuteWalkTm=currentTm;
			for (i=0;i<pathSize;i++)
			{
				switch (path[i])
				{
				case 1: lastEndX++;break;
				case 5: lastEndX--;break;
				case 7: lastEndY++;break;
				case 3: lastEndY--;break;
				case 8: lastEndY++;lastEndX++;break;
				case 6: lastEndY++;lastEndX--;break;
				case 2: lastEndY--;lastEndX++;break;
				case 4: lastEndY--;lastEndX--;break;
				}
			}
			//sprintf(buf,"walk: (%d,%d,%d)->(%d,%d,%d) [%d,%d,%d] %d,%d p=%d",startX,startY,startZ,lastEndX,lastEndY,lastEndZ,lastStartX,lastStartY,lastStartZ,currentTm-lastStartChangeTm,currentTm-lastExecuteWalkTm,pathSize);
			//testDebug(buf);

			if (currentTm-lastStartChangeTm>=2) 
			{
				// adjust lastStartChangeTm to not flood tibia server with walks
				lastStartChangeTm=currentTm-1;;
			}
		}
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