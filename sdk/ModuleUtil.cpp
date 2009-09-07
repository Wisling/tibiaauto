
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
#include "TibiaMiniMapPoint.h"
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

int CModuleUtil::randomFormula(int average, int halfrange){
	halfrange = abs(halfrange)+1;
	return (average-(halfrange-1))+(rand()%halfrange)+(rand()%halfrange);
}

int CModuleUtil::randomFormula(int average, int halfrange, int cutoff){
	//if cutoff==average a value >=average is returned
	//cutoff is included if is it below or = avg, and included if it is above
	//99.9% chance of exit within 10 iterations
	int sample = randomFormula(average,halfrange);
	while ((sample<cutoff) == (cutoff<=average)){
		sample = randomFormula(average,halfrange);
	}
	return sample;
}

int CModuleUtil::randomFormula(int average, int halfrange, int minR, int maxR){
	if (minR > average || maxR < average) return average;
	halfrange = min(halfrange,max(average-minR,maxR-average));
	if (maxR-average<average-minR)
		return randomFormula(average, halfrange, maxR);
	else
		return randomFormula(average, halfrange, minR);
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


int CModuleUtil::waitForHpManaIncrease(int oldHp,int oldMana){//max about 0.45s
	CMemReaderProxy reader;
	int t;
	for (t=0;t<15;t++)
	{
		CTibiaCharacter *self = reader.readSelfCharacter();
		if (self->hp-oldHp>10 || self->mana-oldMana>10)
		{
			delete self;
			return 1;
		}
		delete self;
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForCapsChange(float origCaps)//takes about a max of .6 secs for NPC purchases and up to .9 for dropping an object
{
	CMemReaderProxy reader;
	int t;
	for (t=0;t<30;t++)
	{
		CTibiaCharacter *self = reader.readSelfCharacter();
		if (self->cap!=origCaps)
		{
			delete self;
			return 1;
		}
		delete self;
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForItemsInsideChange(int contNr, int origItemsCount)//takes about a max of .6 secs
{
	CMemReaderProxy reader;
	int t;
	for (t=0;t<30;t++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);
		if (cont->itemsInside!=origItemsCount)
		{
			delete cont;
			return 1;
		}
		delete cont;
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForItemsInsideChange(int contNr, int origItemSlot, int origItemQuantity, int origItemsCount)//takes about a max of .5 secs
{
	CMemReaderProxy reader;
	int t;
	for (t=0;t<30;t++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);
		CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(origItemSlot);
		if (cont->itemsInside!=origItemsCount || item->quantity != origItemQuantity)
		{
			delete cont;
			return 1;
		}
		delete cont;
		Sleep(50);
	}
	return 0;
}

void CModuleUtil::waitForItemChange(int locationAddress, int origItemId)//takes about a max of .6 secs
{
	CMemReaderProxy reader;
	int t;
	for (t=0;t<30;t++)
	{
		CTibiaItem *item = reader.readItem(locationAddress);
		if (item->objectId!=origItemId)
		{
			delete item;
			return;
		}
		if (item)
			delete item;
		Sleep(50);
	}
	return;
}

void CModuleUtil::findPathOnMapProcessPoint(CQueue *queue,int prevX,int prevY, int prevZ, int newX, int newY, int newZ)
{
	CTibiaMapProxy tibiaMap;

	int loopsOnItself=newX==tibiaMap.getPrevPointX(prevX,newY,prevZ)&&newY==tibiaMap.getPrevPointY(prevX,newY,prevZ)&&newZ==tibiaMap.getPrevPointZ(prevX,newY,prevZ);
	int upDownToUpDown = tibiaMap.getPointUpDownNoProh(prevX,prevY,prevZ) && tibiaMap.getPointUpDownNoProh(newX,newY,newZ);

	if (tibiaMap.isPointAvailable(newX,newY,newZ) && !loopsOnItself && !upDownToUpDown)
	{
		//Diagonal fix: Only add possible next point to queue if on different floor or is not a diagonal move with a non-diagonal possibility
		int diagonalMove = (newX-prevX)&&(newY-prevY)&&(newZ==prevZ);
		int otherAvailableX = tibiaMap.getPrevPointZ(newX,prevY,prevZ)==newZ;
		int otherAvailableY = tibiaMap.getPrevPointZ(prevX,newY,prevZ)==newZ;
		//if (prevZ != newZ || !(diagonalMove && (otherAvailableX || otherAvailableY)) || tibiaMap.getPointUpDownNoProh(newX,newY,newZ)){
			if (tibiaMap.getPrevPointX(newX,newY,newZ)==0) queue->add(newX,newY,newZ);// prevents overflow
			tibiaMap.setBestPrevPoint(newX,newY,newZ,prevX,prevY,prevZ);
		//}
	}
}



void inline mapDebug(char *s)
{	
#ifndef MAPDEBUG
	//return;
#endif
	FILE *f=fopen("C:/tibiaauto-debug-map.txt","a+");
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
	char buf[512];
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
			}
		}
	}
	return retPoint;
}



void CModuleUtil::findPathAllDirection(CQueue *queue,int x,int y,int z)
{
		int randDir = rand()%4;
		//First piority points(sides)
		// if we stand on open hole or stairs we can only change z coordinate
		for (int i=randDir;i<randDir+4;i++){
			switch(i%4){
			case 0:	findPathOnMapProcessPoint(queue,x,y,z,x+1,y,z); break;
			case 1:	findPathOnMapProcessPoint(queue,x,y,z,x-1,y,z); break;
			case 2:	findPathOnMapProcessPoint(queue,x,y,z,x,y+1,z); break;
			case 3:	findPathOnMapProcessPoint(queue,x,y,z,x,y-1,z); break;
			}
		}

		//Second piority points(diagonals)
		findPathOnMapProcessPoint(queue,x,y,z,x+1,y+1,z);
		findPathOnMapProcessPoint(queue,x,y,z,x-1,y+1,z);
		findPathOnMapProcessPoint(queue,x,y,z,x+1,y-1,z);
		findPathOnMapProcessPoint(queue,x,y,z,x-1,y-1,z);
}

#define MAX_PATH_LEN 10000
point pathTab[MAX_PATH_LEN];
int pathTabLen;
int lastEndX=-1,lastEndY=-1,lastEndZ=-1;

struct point CModuleUtil::findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int endSpecialLocation, int path[15], int useDiagonal)
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

	int closerEndX=endX,closerEndY=endY,closerEndZ=endZ;
	// If still going to same place, clear only points on screen and keep rest
	if (lastEndX==endX&&lastEndY==endY&&lastEndZ==endZ)
	{
		tibiaMap.clearLocalPrevPoint(startX, startY, startZ,10);
		p = point(endX,endY,endZ);
		point pCloser;
		int stepsSoFar=0;
		while (!(p.x==0&&p.y==0&&p.z==0))
		{				

			pCloser.x=p.x;
			pCloser.y=p.y;
			pCloser.z=p.z;

			p.x=tibiaMap.getPrevPointX(pCloser.x,pCloser.y,pCloser.z);
			p.y=tibiaMap.getPrevPointY(pCloser.x,pCloser.y,pCloser.z);
			p.z=tibiaMap.getPrevPointZ(pCloser.x,pCloser.y,pCloser.z);

			stepsSoFar++;

			if (stepsSoFar>MAX_PATH_LEN-5)
			{
				tibiaMap.clearPrevPoint();
				break;
			}
		};
		closerEndX=pCloser.x;
		closerEndY=pCloser.y;
		closerEndZ=pCloser.z;
#ifdef MAPDEBUG
			char buf[128];
			sprintf(buf,"Goint to %d,%d,%d instead of %d,%d,%d diff %d",closerEndX,closerEndY,closerEndZ,endX,endY,endZ,abs(closerEndX-endX)+abs(closerEndY-endY));
			mapDebug(buf);
#endif
		
	}
	else
		tibiaMap.clearPrevPoint();


	tibiaMap.setPrevPoint(startX,startY,startZ,startX,startY,startZ);

#ifdef MAPDEBUG	
	char buf[512];
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
		if (currentPoint.x==closerEndX&&currentPoint.y==closerEndY&&currentPoint.z==closerEndZ) gotToEndPoint=1;
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
			gotToEndPoint++;
		}

		if (gotToEndPoint || gotToEndPoint>50 || gotToEndPoint && !queue->size())
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
				//mapDebug(buf);
#endif

				pathPos++;
				
				if (pathPos>MAX_PATH_LEN-5)
				{
					delete queue;
					return point(0,0,0);
				}
			};
			pathTab[pathPos].x=startX;
			pathTab[pathPos].y=startY;
			pathTab[pathPos].z=startZ;
			pathPos++;

#ifdef MAPDEBUG
			char buf2[256];
			buf2[0]=0;
			for (int ind=pathPos-1;ind>pathPos-11&&ind>=0;ind--){
				sprintf(buf2,"%s(%d,%d,%d)->",buf2,pathTab[ind].x,pathTab[ind].y,pathTab[ind].z);
			}
			mapDebug(buf2);
#endif
					
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
				path[pos]=0;
				if (curX+1==nextX&&curY==nextY) path[pos]=1; // right
				if (curX-1==nextX&&curY==nextY) path[pos]=5; // left
				if (curX==nextX&&curY+1==nextY) path[pos]=7; // down
				if (curX==nextX&&curY-1==nextY) path[pos]=3; // up

				if (curX+1==nextX&&curY+1==nextY) path[pos]=8; // down-right
				if (curX-1==nextX&&curY+1==nextY) path[pos]=6; // down-left
				if (curX+1==nextX&&curY-1==nextY) path[pos]=2; // up-right
				if (curX-1==nextX&&curY-1==nextY) path[pos]=4; // up-left

				if (curZ-1==nextZ)
				{
					// up
					if (pos==0)
					{		
					
						if (curX==nextX&&curY==nextY)
						{
							path[pos]=0xD0;
						} 
						else 
						{
							path[pos]+=0xE0;
						}
					}
					else{
						path[pos]=0;//undoes direction assignment
					}
					break;
				}
				if (curZ+1==nextZ)
				{
					// down
					if (pos==0)
					{
						if (curX==nextX&&curY==nextY)
						{
							path[pos]=0xD1;
						}
						else 
						{
							path[pos]+=0xF0;
						}
					}
					else{
						path[pos]=0;//undoes direction assignment
					}
					break;
				}
				if (abs(curX-nextX)>1||abs(curY-nextY)>1||path[pos]==0&&abs(curZ-nextZ)>0)
				{
					if (pos==0)
					{
						path[pos]=0xD2;
					}
					else
					{
						path[pos]=0;//undoes direction assignment
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

			delete queue;
			return point(endX,endY,endZ);
		}
		
		
		/**
		 * For each direction - check whether the next point is available on the map
		 * and whether it has not been visited before.
		 */		

		/**
		 * 101 - open hole
		 * 102 - closed hole--
		 * 103 - crate--
		 * 201 - rope--
		 * 202 - magic rope
		 * 203 - ladder--
		 * 204 - stairs
		 * 301 - depot
		 * 302 - teleporter
		 * 303 - permanent block
		 */
		
		int forcedLevelChange=0; // if set to 1 then going north, south, east, west is forbidden

#ifdef MAPDEBUG
		char buf[128];
		sprintf(buf,"current = %d,%d,%d",x,y,z);
		//mapDebug(buf);
#endif
		

		int currentUpDown=tibiaMap.getPointUpDown(x,y,z);

		// we directly go up using rope, magic rope, ladder, stairs
		if (currentUpDown>=200&&currentUpDown<300&&tibiaMap.isPointAvailableNoProh(x,y,z-1))
		{						
			tibiaMap.setBestPrevPoint(x,y,z-1,x,y,z);
			findPathAllDirection(queue,x,y,z-1);
#ifdef MAPDEBUG
			mapDebug("go up");
#endif
		}

		// we go directly down using open hole, closed hole, crate
		if (currentUpDown>=100&&currentUpDown<200&&tibiaMap.isPointAvailableNoProh(x,y,z+1))
		{
			tibiaMap.setBestPrevPoint(x,y,z+1,x,y,z);
			findPathAllDirection(queue,x,y,z+1);
#ifdef MAPDEBUG
			mapDebug("go down");
#endif
		}

		// we go directly down using open hole, closed hole, crate
		if (currentUpDown==302)
		{
			point dest=tibiaMap.getDestPoint(x,y,z);
			tibiaMap.setBestPrevPoint(dest.x,dest.y,dest.z,x,y,z);
			findPathAllDirection(queue,dest.x,dest.y,dest.z);
#ifdef MAPDEBUG
			mapDebug("go teleport");
#endif
		}

		//prevents the adding of squares around current square as possible next points except when standing on it(303 goes nowhere)
		if ((currentUpDown==101||currentUpDown==204||currentUpDown==302||currentUpDown==303) && !(startX==x && startY==y && startZ==z))
		{
			forcedLevelChange=1;
		}


		if (!forcedLevelChange){
		
			findPathAllDirection(queue,x,y,z);


			int xShift=-2,yShift=-2;
			while (!(xShift==1 && yShift==1)){//simply goes in a 3x3 square 
				if (xShift==-2 && yShift==-2) {xShift=-1;yShift=-1;}
				else xShift++;
				if (xShift>1) {xShift=-1;yShift++;}
				if (xShift==0 && yShift==0) xShift++;
				
				int updown=tibiaMap.getPointUpDownNoProh(x+xShift,y+yShift,z);
			
				// special going up if ladder busy
				if ((updown==201||updown==203)&&tibiaMap.isPointAvailableNoProh(x+xShift,y+yShift,z-1))
				{
					tibiaMap.setBestPrevPoint(x+xShift,y+yShift,z-1,x,y,z);
					findPathAllDirection(queue,x+xShift,y+yShift,z-1);
				}
				// special going down if grate or hole busy
				if ((updown==102||updown==103)&&tibiaMap.isPointAvailableNoProh(x+xShift,y+yShift,z+1))
				{
					tibiaMap.setBestPrevPoint(x+xShift,y+yShift,z+1,x,y,z);
					findPathAllDirection(queue,x+xShift,y+yShift,z+1);
				}
			}
		}
	}
	delete queue;
	return point(0,0,0);
}

int CModuleUtil::waitForOpenContainer(int contNr, int open)//max about 0.7s
{
	CMemReaderProxy reader;
	int t;
	for (t=0;t<30;t++)
	{
		CTibiaContainer *cont = reader.readContainer(contNr);
		if (cont->flagOnOff==open)
		{
			delete cont;
			return 1;
		}
		delete cont;
		Sleep(50);
	}
	return 0;
}


int CModuleUtil::loopItemFromSpecifiedContainer(int containerNr,CUIntArray *acceptedItems, int containerCarrying)
{

	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaContainer *cont = reader.readContainer(containerNr);
	CTibiaContainer *contCarrying = reader.readContainer(containerCarrying);
	int looted = 0;
	int numberItemsLooted=0;
	if (cont->flagOnOff)
	{
		int itemNr;
		for (itemNr=cont->itemsInside-1;itemNr>=0;itemNr--)
		{			
			CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(itemNr);
			int i;
			for (i=0;i<acceptedItems->GetSize();i++)
			{
				if ((int)item->objectId==(int)acceptedItems->GetAt(i))
				{
					// item needs to be looted
					int targetPos=contCarrying->size;
					int moved = item->quantity?item->quantity:1;
					CTibiaTile *tile = reader.getTibiaTile(item->objectId);
					if (tile->stackable)
					{						
						// if item is stackable then try to find a suitable stack for it
						for (int stackedItemPos=0;stackedItemPos<contCarrying->itemsInside;stackedItemPos++)
						{
							CTibiaItem *stackedItem=(CTibiaItem *)contCarrying->items.GetAt(stackedItemPos);
							if (stackedItem->objectId==item->objectId&&stackedItem->quantity<100)
							{
								// we have found a suitable stack! :)
								targetPos=stackedItemPos+looted;//since contCarrying is never updated add looted
								//Akilez: Stackable item overflow goes back into container at the same position
								if ((stackedItem->quantity + item->quantity) > 100){
									itemNr++;//Akilez: recheck this space in the container for lootables
								}
								else {
									looted--;//number of items in container will stay the same, undo increment
								}

								moved = min(item->quantity,100-stackedItem->quantity);
								stackedItem->quantity += moved;
								item->quantity -= moved;
								break;
							}
						}	
					}
					sender.moveObjectBetweenContainers(item->objectId,0x40+containerNr,item->pos,0x40+containerCarrying,targetPos,moved);
					Sleep(CModuleUtil::randomFormula(300,150));
					looted++;//needed for positioning of next loot item
					numberItemsLooted++;// return velue
					break;
				}
			}
		}
	}	

	delete cont;
	delete contCarrying;
	return numberItemsLooted;//Akilez: return value now reflects items looted
}

void CModuleUtil::lootItemFromContainer(int contNr, CUIntArray *acceptedItems,int ignoreCont1/*=-1*/,int ignoreCont2/*=-1*/)
{	
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	
	// find first free container
	int qtyLooted=0;
	for (int openCont=0;openCont<memConstData.m_memMaxContainers;openCont++)
	{	
		if (openCont==contNr || openCont==ignoreCont1 || openCont==ignoreCont2) continue;

		CTibiaContainer *targetCont = reader.readContainer(openCont);
		if (targetCont->flagOnOff)
		{			
			qtyLooted += (int)(CModuleUtil::loopItemFromSpecifiedContainer(contNr,acceptedItems,openCont));//Akilez: loot items and get # of items returned
			if (targetCont->itemsInside<targetCont->size) 
			{
				delete targetCont;
				break;
			}
		}
		
		
		delete targetCont;
	}
	//char buf[128];
	//sprintf(buf,"Looted=%d",qtyLooted);
	//AfxMessageBox(buf);

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
		Sleep(CModuleUtil::randomFormula(400,100));
		delete item;
	}
}

int CModuleUtil::waitToApproachSquare(int x, int y)// depends on speed of character (1.4 seconds for lvl 1 doing diagonal)
{
	CMemReaderProxy reader;

	int spaceCount=15;
	//wait until square reached and return if more than 15 steps
	while(spaceCount-->0){
		CTibiaCharacter *self = reader.readSelfCharacter();
		static int newX = self->x;
		static int newY = self->y;
		if (max(abs(self->x-x),abs(self->y-y))<=1){
			delete self;
			break;
		}
		delete self;

		//wait until square changes and return if took too long
		int iterCount=28;
		while (iterCount-->0) {
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (self->x!=newX || self->y!=newY){
				newX=self->x;
				newY=self->y;
				delete self;
				break;
			}
			delete self;
			Sleep(50);
		}
		if (iterCount==-1){
			//AfxMessageBox("return 0,(1)");
			return 0;
		}
	}
	if (spaceCount==-1){
		//AfxMessageBox("return 0,(2)");
		return 0;
	}
	//AfxMessageBox("return 1");
	return 1;
}

int CModuleUtil::waitToStandOnSquare(int x, int y)// depends on speed of character (1.4 seconds for lvl 1 doing diagonal)// see tileDelay
{
	CMemReaderProxy reader;

	int spaceCount=15;
	//wait until square reached and return if more than 15 steps
	while(spaceCount-->0){
		CTibiaCharacter *self = reader.readSelfCharacter();
		static int newX = self->x;
		static int newY = self->y;
		if (max(abs(self->x-x),abs(self->y-y))==0){
			delete self;
			break;
		}
		delete self;

		//wait until square changes and return if took too long
		int iterCount=28;
		while (iterCount-->0) {
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (self->x!=newX || self->y!=newY){
				newX=self->x;
				newY=self->y;
				delete self;
				break;
			}
			delete self;
			Sleep(50);
		}
		if (iterCount==-1){
			//AfxMessageBox("return 0,(1)");
			return 0;
		}
	}
	if (spaceCount==-1){
		//AfxMessageBox("return 0,(2)");
		return 0;
	}
	//AfxMessageBox("return 1");
	return 1;
}

int CModuleUtil::waitForCreatureDisappear(int creatureNr)//ranges from near instantaneous to 1.0 secs
{
	CMemReaderProxy reader;
	int iterCount=25;
	while (iterCount-->0) {
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);
		if (!ch->visible)
		{
			delete ch;
			return 1;
		} 
		delete ch;
		Sleep(50);
	}
	return 0;  
}

int CModuleUtil::waitForCreatureDisappear(int x,int y, int tibiaId)
{
	int xReturn,yReturn;
	return waitForCreatureDisappear(x,y,tibiaId,xReturn,yReturn);
}

int CModuleUtil::waitForCreatureDisappear(int x,int y, int tibiaId, int &xReturn, int &yReturn) {
	CMemReaderProxy reader;
	int iterCount=20;
	while (iterCount-->0) {
		int found=0;
		int pos;
		for (int xOffset = -1; xOffset < 1; xOffset++) {
			for (int yOffset = -1; yOffset < 1; yOffset++) {
				int stackCount=reader.mapGetPointItemsCount(point(x + xOffset,y +yOffset,0));
				for (pos=0;pos<stackCount;pos++) {
					int tileId = reader.mapGetPointItemId(point(x + xOffset,y +yOffset,0),pos);
					int extraData = reader.mapGetPointItemExtraInfo(point(x + xOffset,y +yOffset,0),pos,1);
					if (tileId==99&&extraData==tibiaId) {
						found=1;
						xReturn = xOffset;
						yReturn = yOffset;
					}
				}
			}
		}
		if (!found)
			return 1;
		Sleep(100);
	}
	return 0;
}

int CModuleUtil::calcLootChecksum(int tm, int killNr, int nameLen, int itemNr, int objectId, int qty, int lootInBags,int creatX,int creatY,int creatZ)
{
	return tm*3+killNr*17+nameLen*3+itemNr*15+objectId*19+qty*13+lootInBags*71+creatX*23+creatY*31+creatZ*59;
	//Old checksum return tm*3+killNr*17+nameLen*3+itemNr*15+objectId*19+qty*13+lootInBags*71;
}

void CModuleUtil::executeWalk(int startX, int startY, int startZ,int path[15])
{
	static int lastExecuteWalkTm=0; // indicates when last a "real" walk was executed
	static int lastStartChangeTm=0; // indicates when start point was changed for the last time
	static int movedDiagonally=0; // will make delay to resend walk 3x longer
	static int lastEndX=0,lastEndY=0,lastEndZ=0; 
	static int lastStartX=0,lastStartY=0,lastStartZ=0;
	CPackSenderProxy sender;
	CTibiaMapProxy tibiaMap;
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();

	int currentTm=reader.getCurrentTm();
	int pathSize;
	for (pathSize=0;pathSize<15&&path[pathSize];pathSize++){}
#ifdef MAPDEBUG
	char buf[512];
	sprintf(buf,"pathsize=%d assumed=(%d,%d,%d) now=(%d,%d,%d) delta=(%d,%d,%d)=%d",pathSize,startX,startY,startZ,self->x,self->y,self->z,self->x-startX,self->y-startY,self->z-startZ,abs(self->x-startX)+abs(self->y-startY)+abs(self->z-startZ));	
	mapDebug(buf);
#endif

	//Start walking after changing floors
	if (lastStartZ!=startZ){
		lastStartX=startX;
		lastStartY=startY;
		lastStartZ=startZ;
		lastStartChangeTm=999;
	}else if (lastStartX!=startX||lastStartY!=startY||lastStartZ!=startZ) 
	{
		movedDiagonally=(lastStartX!=startX) && (lastStartY!=startY);
		lastStartX=startX;
		lastStartY=startY;
		lastStartZ=startZ;
		lastStartChangeTm=currentTm;
	}
#ifdef MAPDEBUG
	buf[0]=0;
	for (int tracePath=0;tracePath<pathSize;tracePath++)
	{
		sprintf(buf,"%s(%X) ",buf,path[tracePath]);
	}
	mapDebug(buf);
#endif
	if (pathSize==1&&path[0]>=0xD0)// || pathSize==2&&path[1]>=0xE0)
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
								sender.useWithObjectFromContainerOnFloor(itemProxy.getValueForConst("rope"),0x40+contNr,item->pos,itemProxy.getValueForConst("ropespot"),self->x,self->y,self->z);
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
							Sleep(CModuleUtil::randomFormula(1100,300));
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
										sender.useWithObjectFromContainerOnFloor(itemProxy.getValueForConst("shovel"),0x40+contNr,shovel->pos,tileId,self->x,self->y,self->z);
										break;
									}
								}
							}
							delete self2;
							Sleep(CModuleUtil::randomFormula(900,300));
							switch (mode)
							{
							case 2: path[0]=5;break;
							case 1: path[0]=1;break;
							case 4: path[0]=3;break;
							case 3: path[0]=7;break;						
							}
							sender.stepMulti(path,1);
							Sleep(CModuleUtil::randomFormula(1100,300));
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
		case 0xD2:
			{
				// teleporter it will have walked onto it
				break;
			} // case 0xD2
		case 0xE1:
		case 0xE2:
		case 0xE3:
		case 0xE4:
		case 0xE5:
		case 0xE6:
		case 0xE7:
		case 0xE8:
			{
				// go up - but one square away
				int modX = self->x;
				int modY = self->y;
				if (path[0]==0xE1) modX++;
				if (path[0]==0xE2) {modX++;modY--;}
				if (path[0]==0xE3) modY--;
				if (path[0]==0xE4) {modX--;modY--;}
				if (path[0]==0xE5) modX--;
				if (path[0]==0xE6) {modX--;modY++;}
				if (path[0]==0xE7) modY++;
				if (path[0]==0xE8) {modX++;modY++;}
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
								sender.useWithObjectFromContainerOnFloor(itemProxy.getValueForConst("rope"),0x40+contNr,item->pos,itemProxy.getValueForConst("ropespot"),modX,modY,self->z);
								delete item;
								break;
							}
						}
					}
					break;
				case 203:					
					{
						//buf[111];
						//sprintf(buf,"self %d,%d, mod %d %d",self->x,self->y,modX,modY);
						//sender.sendTAMessage(buf);
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
		case 0xF5:
		case 0xF6:
		case 0xF7:
		case 0xF8:
			{
				// go down - but one square away
				int modX = self->x;
				int modY = self->y;
				if (path[0]==0xF1) modX++;
				if (path[0]==0xF2) {modX++;modY--;}
				if (path[0]==0xF3) modY--;
				if (path[0]==0xF4) {modX--;modY--;}
				if (path[0]==0xF5) modX--;
				if (path[0]==0xF6) {modX--;modY++;}
				if (path[0]==0xF7) modY++;
				if (path[0]==0xF8) {modX++;modY++;}

				switch (tibiaMap.getPointUpDownNoProh(modX,modY,self->z))
				{
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
						}									
						if (shovel)
						{
							// shovel found, so proceed with opening hole
							int count=reader.mapGetPointItemsCount(point(modX-self->x,modY-self->y,0));
							
							int blocked=0;
							int updown=0;
							int i;
							for (i=0;i<count;i++)
							{						
								int tileId = reader.mapGetPointItemId(point(modX-self->x,modY-self->y,0),i);
								//char buf[128];
								//sprintf(buf,"[%d/%d] me (%d,%d,%d) opening (%d,%d,%d) tile %d",i,count,self2->x,self2->y,self2->z,self->x,self->y,self->z,tileId);
								//::MessageBox(0,buf,buf,0);
								if (tileId!=99)
								{
									CTibiaTile *tile = reader.getTibiaTile(tileId);
									
									if (tile->requireShovel)
									{										
										sender.useWithObjectFromContainerOnFloor(itemProxy.getValueForConst("shovel"),0x40+contNr,shovel->pos,tileId,modX,modY,self->z);
										break;
									}
								}
							}
							Sleep(CModuleUtil::randomFormula(900,300));

							int mode = path[0]&0xF;
							int path[3];
							path[1]=0;
							int size=0;
							if (modX-self->x&&modY-self->y){
								if (tibiaMap.isPointAvailable(modX,self->y,self->z)) {
									switch (mode)
									{
								case 2: path[0]=1;path[1]=3;break;
								case 8: path[0]=1;path[1]=7;break;
								case 4: path[0]=5;path[1]=3;break;
								case 6: path[0]=5;path[1]=7;break;						
									}
								}
								else if (modY&&tibiaMap.isPointAvailable(self->x,modY,self->z)){
									switch (mode)
									{
								case 2: path[0]=7;path[1]=5;break;
								case 8: path[0]=3;path[1]=5;break;
								case 4: path[0]=7;path[1]=1;break;
								case 6: path[0]=3;path[1]=1;break;					
									}
								}
								path[2]=0;
								size=2;
							}
							if (path[1]==0) { path[0]=mode; size=1; }

							sender.stepMulti(path,size);
							Sleep(CModuleUtil::randomFormula(1100,300));
							delete shovel;
						}
					}
					break;
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

		//wait for change of level
		int iterCount=60;
		while (iterCount-->0)
		{
			CTibiaCharacter* selfTmp=reader.readSelfCharacter();
			if (selfTmp->z!=self->z)
				break;
			Sleep(50);
			delete selfTmp;
		}
	} else {
		/**
		 * Walk if :
		 * 1. we found a way AND
		 * 2. last planned end point equals to the start point now (means: we reach "final" point) OR
		 * 3. last startX change was > 2.5s ago (means: we are stuck) OR
		 * 4. last walk was over 15s ago (means: walk restart, lag, etc.)
		 */
		
		int lastEndEqStart=(lastEndX==startX&&lastEndY==startY&&lastEndZ==startZ);
		CTibiaMiniMapPoint* mp=reader.readMiniMapPoint(self->x+1,self->y,self->z);//take sample from 1 square away
		int tileSpeed=(mp->speed==255)?200:mp->speed;
		delete mp;
		//&&reader.getMemIntValue(itemProxy.getValueForConst("addrTilesToGo"))==0
		//time to walk 1 sqm is inverse to the speed, double speed== half the time
		int maxTileDelay=(int)(tileSpeed*(movedDiagonally?3.5:1.5)*1000/self->walkSpeed);
		bool stoppedWalking=currentTm-lastStartChangeTm>=maxTileDelay;
		if (pathSize>0&&(lastEndEqStart||stoppedWalking||currentTm-lastExecuteWalkTm>15000))
		{
			// 'normal' stepping limited to 10 steps
			int i;
			if (pathSize>10) pathSize=10;
#ifdef MAPDEBUG
			sprintf(buf,"Send walk to server. size=%d,lastEndEqStart=%d,lastStartChangeTm_diff=%d,lastExecuteWalkTm=%d,tileDelay=%d,stoppedWalking=%d",pathSize,lastEndEqStart,currentTm-lastStartChangeTm,currentTm-lastExecuteWalkTm,maxTileDelay,stoppedWalking);
			mapDebug(buf);
#endif
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

			if (stoppedWalking)
			{
				// adjust lastStartChangeTm to not flood tibia server with walks
				lastStartChangeTm=currentTm+1000;
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

int CModuleUtil::findNextClosedContainer(int afterCont/*=-1*/)
{

	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaContainer *container;
	CMemConstData memConstData = reader.getMemConstData();
	int targetBag;
	for (targetBag=afterCont+1;targetBag<memConstData.m_memMaxContainers;targetBag++){
		container = reader.readContainer(targetBag);
		if (!container->flagOnOff) break;
	}
	
	if (targetBag == memConstData.m_memMaxContainers){
		targetBag=memConstData.m_memMaxContainers-1;
		sender.closeContainer(targetBag);
		waitForOpenContainer(targetBag,false);
	}
	return targetBag;
}

void CModuleUtil::masterDebug(const char* fname, const char* buf1,const char* buf2,const char* buf3,const char* buf4,const char* buf5,const char* buf6){
	//replace old file
	if (strlen(buf1)==0){
		FILE *f=fopen(fname,"w");
		if (f)
		{
			fclose(f);
		}
		return;
	}
	char dateStr [15];
	char timeStr [15];
	_strdate( dateStr);
	_strtime( timeStr );
	FILE *f=fopen(fname,"a+");
	if (f)
	{
		fprintf(f,"%s\t%s\tTibiaAuto\t%s\t%s\t%s\t%s\t%s\t%s\n",dateStr,timeStr,buf1,buf2,buf3,buf4,buf5,buf6);
		fclose(f);
		//delete f;
	}
}
