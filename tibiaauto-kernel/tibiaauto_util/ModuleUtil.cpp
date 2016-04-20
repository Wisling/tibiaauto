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

#pragma warning( disable : 4786 )
#include "stdafx.h"
#include "ModuleUtil.h"

#include "TibiaContainer.h"
#include "TibiaItem.h"
#include "TibiaMiniMapPoint.h"
#include "TibiaStructures.h"
#pragma warning( disable : 4786 )
#include "MemReader.h"
#include "TibiaMap.h"
#include "PackSender.h"
#include "TibiaTile.h"
#include "TileReader.h"
#include <MemUtil.h>
#include "regex.h"


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

//Always returns a valid item.  objectId==0 if fails
CTibiaItem * CModuleUtil::lookupItem(int containerNr, CUIntArray *itemsAccepted)
{
	int foodContNr = -1;

	CTibiaItem *retItem = new CTibiaItem();
	int itemNr;

	CTibiaContainer *container = CMemReader::getMemReader().readContainer(containerNr);
	if (container->flagOnOff && container->itemsInside > 0)
	{
		for (itemNr = container->itemsInside - 1; itemNr >= 0; itemNr--)
		{
			CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);

			int pos;
			for (pos = 0; pos < itemsAccepted->GetSize(); pos++)
			{
				if ((int)item->objectId == (int)itemsAccepted->GetAt(pos))
				{
					// item found!
					retItem->pos      = itemNr;
					retItem->objectId = item->objectId;
					retItem->quantity = item->quantity;

					delete container;
					return retItem;
				}
			}
		}
	}

	delete container;
	retItem->objectId = 0;
	return retItem;
}

int CModuleUtil::randomFormula(int average, int halfrange) // average-|halfrange| <= ans <= average+|halfrange|
{
	srand(GetTickCount() * (average + 47328) * (halfrange + 23939));
	halfrange = abs(halfrange) + 1;
	return (average - (halfrange - 1)) + (rand() % halfrange) + (rand() % halfrange);
}

int CModuleUtil::randomFormula(int average, int halfrange, int cutoff)
{
	//if cutoff>average returns [average-halfrange,cutoff-1] else [cutoff,average+halfrange]
	//cutoff always included
	//95% chance of exit within 3 iterations
	int sample = randomFormula(average, halfrange);
	int iter   = 0;
	while ((sample >= cutoff) == (cutoff > average))
	{
		if (iter >= 2 || average == cutoff && sample < cutoff)
		{
			sample = average + (average - sample); // put sample on opposite side of average
			break;
		}
		sample = randomFormula(average, halfrange);
		iter++;
	}
	return sample;
}

int CModuleUtil::randomFormula(int average, int halfrange, int minR, int maxR)
{
	//returns between max and min inclusive
	if (minR > average || maxR < average)
		return average;
	halfrange = min(halfrange, max(average - minR, maxR - average));
	if (maxR - average < average - minR)
		return randomFormula(average, halfrange, maxR + 1);
	else
		return randomFormula(average, halfrange, minR);
}

CTibiaItem * CModuleUtil::lookupItem(int containerNr, CUIntArray *itemsAccepted, int fluidType)
{
	int foodContNr = -1;

	int itemNr;
	CTibiaItem *retItem = new CTibiaItem();

	CTibiaContainer *container = CMemReader::getMemReader().readContainer(containerNr);
	if (container->flagOnOff && container->itemsInside > 0)
	{
		for (itemNr = container->itemsInside - 1; itemNr >= 0; itemNr--)
		{
			CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);

			int pos;
			for (pos = 0; pos < itemsAccepted->GetSize(); pos++)
			{
				if ((int)item->objectId == (int)itemsAccepted->GetAt(pos) && item->quantity == fluidType)
				{
					// item found!

					retItem->pos      = itemNr;
					retItem->objectId = item->objectId;
					retItem->quantity = item->quantity;

					delete container;
					return retItem;
				}
			}
		}
	}

	delete container;
	return retItem;
}

int CModuleUtil::waitForHpManaIncrease(int oldHp, int oldMana) //max about 0.45s
{
	int t;
	for (t = 0; t < 15; t++)
	{
		CTibiaCharacter self;
		CMemReader::getMemReader().readSelfCharacter(&self);
		if (self.hp - oldHp > 30 || self.mana - oldMana > 30)
		{
			return 1;
		}
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForManaDecrease(int oldMana) //max about 0.45s
{
	int t;
	for (t = 0; t < 15; t++)
	{
		CTibiaCharacter self;
		CMemReader::getMemReader().readSelfCharacter(&self);
		if (self.mana - oldMana < 10)
		{
			return 1;
		}
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForCapsChange(float origCaps)//takes about a max of .6 secs for NPC purchases and up to .9 for dropping an object
{
	int t;
	for (t = 0; t < 30; t++)
	{
		CTibiaCharacter self;
		CMemReader::getMemReader().readSelfCharacter(&self);
		if (self.cap != origCaps)
		{
			return 1;
		}
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForItemsInsideChange(int contNr, int origItemsCount)//takes about a max of .6 secs
{
	int t;
	for (t = 0; t < 30; t++)
	{
		CTibiaContainer *cont = CMemReader::getMemReader().readContainer(contNr);
		if (cont->itemsInside != origItemsCount)
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
	int t;
	for (t = 0; t < 30; t++)
	{
		CTibiaContainer *cont = CMemReader::getMemReader().readContainer(contNr);
		if (cont->itemsInside != origItemsCount)//ensures cont->items.GetAt array has at least origItemSlot items
		{
			delete cont;
			return 1;
		}
		CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(origItemSlot);
		if (item->quantity != origItemQuantity)
		{
			delete cont;
			return 1;
		}
		delete cont;
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForItemChange(int locationAddress, int origItemId)//takes about a max of .6 secs
{
	int t;
	for (t = 0; t < 30; t++)
	{
		CTibiaItem *item = CMemReader::getMemReader().readItem(locationAddress);
		if (item->objectId != origItemId)
		{
			delete item;
			return 1;
		}
		delete item;
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForItemChange(int contNr, int slotNr, int origItemId, int quantity)//takes about a max of .5 secs
{
	int t;
	for (t = 0; t < 30; t++)
	{
		CTibiaContainer *cont = CMemReader::getMemReader().readContainer(contNr);
		if (cont->itemsInside <= slotNr)//ensures cont->items.GetAt array has at least origItemSlot items
		{
			delete cont;
			return 1;
		}
		CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(slotNr);
		if (item->objectId != origItemId || item->quantity != quantity)
		{
			delete cont;
			return 1;
		}
		delete cont;
		Sleep(50);
	}
	return 0;
}

//Input: x,y absolute position around which to search
//Output: returns 0 if unsucessful, otherwise returns 1 and changes x and y to location
int CModuleUtil::findFreeSpace(int &x, int &y, int z, int r /* =1 */)
{
	int count    = 0;
	int totcount = (1 + 2 * r) * (1 + 2 * r);
	for (int offsetX = rand() % (1 + 2 * r); count < totcount; offsetX = (offsetX + 1) % (1 + 2 * r))
	{
		for (int offsetY = rand() % (1 + 2 * r); count < totcount; offsetY = (offsetY + 1) % (1 + 2 * r))
		{
			count++;
			// double loop break!
			if (CTibiaMap::getTibiaMap().isPointAvailableNoProh(offsetX - r + x, offsetY - r + y, z))
			{
				x += offsetX - r;
				y += offsetY - r;
				return 1;
			}
		}
	}
	return 0;
}

void inline mapDebug(char *s)
{
#ifndef MAPDEBUG
	return;
#endif // ifndef MAPDEBUG
	FILE *f = fopen("C:/tibiaauto-debug-map.txt", "a+");
	if (f)
	{
		char dateStr [15];
		char timeStr [15];
		_strdate(dateStr);
		_strtime(timeStr);
		fprintf(f, "%s %s: %s\n", dateStr, timeStr, s);
		fclose(f);
	}
}

void inline testDebug(char *s)
{
	char installPath[2048];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf, "%s\\tibiaauto-debug-test.txt", installPath);
	FILE *f = fopen(pathBuf, "a+");
	if (f)
	{
		char dateStr [15];
		char timeStr [15];
		_strdate(dateStr);
		_strtime(timeStr);
		fprintf(f, "%s %s: %s\n", dateStr, timeStr, s);
		fclose(f);
	}
}

void CModuleUtil::findPathOnMapProcessPoint(CTibiaQueue<point> &pointsToAdd, int prevX, int prevY, int prevZ, int newX, int newY, int newZ)
{
	if (CTibiaMap::getTibiaMap().isPointAvailable(newX, newY, newZ))
		pointsToAdd.Add(point(newX, newY, newZ));
}

void CModuleUtil::findPathAllDirection(CTibiaQueue<point> &pointsToAdd, int x, int y, int z)
{
	int randDir = rand() % 4;
	//First piority points(sides)
	for (int i = randDir; i < randDir + 4; i++)
	{
		switch (i % 4)
		{
		case 0:
			findPathOnMapProcessPoint(pointsToAdd, x, y, z, x + 1, y, z);
			break;
		case 1:
			findPathOnMapProcessPoint(pointsToAdd, x, y, z, x - 1, y, z);
			break;
		case 2:
			findPathOnMapProcessPoint(pointsToAdd, x, y, z, x, y + 1, z);
			break;
		case 3:
			findPathOnMapProcessPoint(pointsToAdd, x, y, z, x, y - 1, z);
			break;
		}
	}

	//Second piority points(diagonals)
	findPathOnMapProcessPoint(pointsToAdd, x, y, z, x + 1, y + 1, z);
	findPathOnMapProcessPoint(pointsToAdd, x, y, z, x - 1, y + 1, z);
	findPathOnMapProcessPoint(pointsToAdd, x, y, z, x + 1, y - 1, z);
	findPathOnMapProcessPoint(pointsToAdd, x, y, z, x - 1, y - 1, z);
}

#define MAX_PATH_LEN 10000
point pathTab[MAX_PATH_LEN];
int pathTabLen;

int lastDestX      = -1, lastDestY = -1, lastDestZ = -1, lastSpecLoc = -1;
point lastEndPoint = point(-1, -1, -1);
void resetPathfinderData()
{
	lastDestX    = -1;
	lastDestY    = -1;
	lastDestZ    = -1;
	lastEndPoint = point(-1, -1, -1);
	lastSpecLoc  = -1;
}

struct point CModuleUtil::GetPathTab(int index)
{
	return pathTab[index];
}

int CModuleUtil::GetPathTabCount()
{
	int index = 0;
	int count = 0;
	while (pathTab[index++].x != 0 && pathTab[index].y != 0 && pathTab[index].z != 0)
		count++;
	return count;
}

int CModuleUtil::RegexMatch(char *string, char *regex)
{
	regex_t preg;
	if (!regcomp(&preg, regex, REG_EXTENDED | REG_ICASE | REG_NOSUB))
	{
		if (!regexec(&preg, string, 0, NULL, NULL))
		{
			// match found
			regfree(&preg);
			return 1;
		}
		else
		{
			// no match found
			regfree(&preg);
			return 0;
		}
	}
	else
	{
		// error during compilation of regex
		return 0;
	}
}

class pointNode
{
public:
	int x;
	int y;
	int z;
	int px;
	int py;
	int pz;
	int g;
	int h;
	int f;
	pointNode()
	{
	}

	pointNode(int x1, int y1, int z1, int px1, int py1, int pz1)
	{
		x  = x1;
		y  = y1;
		z  = z1;
		px = px1;
		py = py1;
		pz = pz1;
		g  = 0;
		h  = 0;
		f  = 0;
	}

	pointNode(int x1, int y1, int z1, int px1, int py1, int pz1, int g1, int h1, int f1)
	{
		x  = x1;
		y  = y1;
		z  = z1;
		px = px1;
		py = py1;
		pz = pz1;
		g  = g1;
		h  = h1;
		f  = f1;
	}

	void copy(pointNode c)
	{
		x  = c.x;
		y  = c.y;
		z  = c.z;
		px = c.px;
		py = c.py;
		pz = c.pz;
		g  = c.g;
		h  = c.h;
		f  = c.f;
	}

	pointNode copy()
	{
		return pointNode(x, y, z, px, py, pz, g, h, f);
	}

	char* toString()
	{
		char* ret = (char*)malloc(100);
		sprintf(ret, "(%d,%d,%d)", x, y, z);
		return ret;
	}

	bool equals(pointNode* a)
	{
		return a->x == x && a->y == y && a->z == z;
	}
};

int calcHeur(int startX, int startY, int startZ, int endX, int endY, int endZ)
{
	if (endX == 0 && endY == 0 && endZ == 0)
		return 0;
	return (abs(startX - endX) + abs(startY - endY) + abs(startZ - endZ)) * 100;
}

struct point CModuleUtil::findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int endSpecialLocation, uint8_t path[15], int radius /*= 1*/)
{
#ifdef MAPDEBUG
	char bufD[2500];
#endif // ifdef MAPDEBUG

	memset(path, 0x00, sizeof(path[0]) * 15);
	int closerEnd       = 0;
	CTibiaMap &tibiaMap = CTibiaMap::getTibiaMap();

	if (endSpecialLocation || !radius)
		radius = 0;
	// else radius=(rand()%max(1,radius))+1; random radius taken care of by module


#ifdef MAPDEBUG
	sprintf(bufD, "SSS-findPathOnMap(startX=%d,startY=%d,startZ=%d,endX=%d,endY=%d,endZ=%d,endSpecialLocation=%d,radius=%d", startX, startY, startZ, endX, endY, endZ, endSpecialLocation, radius);
	mapDebug(bufD);
#endif // ifdef MAPDEBUG

	// means: we are already on the target location
	if (max(abs(startX - endX), abs(startY - endY)) <= radius && startZ == endZ)
		return point(startX, startY, startZ);

	prepareProhPointList();

	// unable to reach point which is not available (e.g. other creature standing on it)
	//if (!endSpecialLocation&&!tibiaMap.isPointAvailable(endX,endY,endZ)) return point(0,0,0);
	if (endSpecialLocation)
	{
		if (endSpecialLocation == lastSpecLoc && lastDestX != -1)// go to same location if found a path to last location
		{
			CPackSender::sendTAMessage("Recalculating path to previously found depot.");
			endX = lastDestX;
			endY = lastDestY;
			endZ = lastDestZ;
		}
		else
		{
			endX = 0;
			endY = 0;
			endZ = 0;
		}
	}

	int pathFindX = endX, pathFindY = endY, pathFindZ = endZ;
	tibiaMap.clearDistance();

	if (lastDestX == endX && lastDestY == endY && lastDestZ == endZ || lastSpecLoc == endSpecialLocation)
	{
		tibiaMap.clearLocalPrevPoint(startX, startY, startZ, 10);
		pathFindX = lastEndPoint.x, pathFindY = lastEndPoint.y, pathFindZ = lastEndPoint.z;
		point prevP(0, 0, 0);
		int pathSize = 0;
		while (true)
		{
			prevP.x = tibiaMap.getPrevPointX(pathFindX, pathFindY, pathFindZ);
			if (prevP.x == 0)
			{
				radius    = 0;
				closerEnd = 1;
				break;
			}
			prevP.y   = tibiaMap.getPrevPointY(pathFindX, pathFindY, pathFindZ);
			prevP.z   = tibiaMap.getPrevPointZ(pathFindX, pathFindY, pathFindZ);
			pathFindX = prevP.x, pathFindY = prevP.y, pathFindZ = prevP.z;
			pathSize++;
			if (pathSize > MAX_PATH_LEN - 20)
			{
				pathFindX = endX, pathFindY = endY, pathFindZ = endZ;
				tibiaMap.clearPrevPoint();
				break;
			}
		}
	}
	else
	{
		tibiaMap.clearPrevPoint();
	}

	CTibiaPriorityQueue pQueue;
	pQueue.Add(1, (DWORD) new pointNode(startX, startY, startZ, startX, startY, startZ, 1, 0, 0));

	tibiaMap.setPrevPoint(startX, startY, startZ, startX, startY, startZ);
	tibiaMap.setPointDistance(startX, startY, startZ, 1);

	int gotToEndPoint = 0;
	point endPoint;
	int pqcountiter = 0;
	while (pQueue.GetCount() && gotToEndPoint != 1)
	{
		pqcountiter++;
		pointNode* currentPoint = (pointNode*)pQueue.Remove();

		int x  = currentPoint->x;
		int y  = currentPoint->y;
		int z  = currentPoint->z;
		int px = currentPoint->px;
		int py = currentPoint->py;
		int pz = currentPoint->pz;
		int g  = currentPoint->g;
		int h  = currentPoint->h;
		int f  = currentPoint->f;
		delete currentPoint;


#ifdef MAPDEBUG
		sprintf(bufD, "Processing Point: (%d,%d,%d) (%d,%d,%d) %d ", x, y, z, px, py, pz, g);
		mapDebug(bufD);
		pointNode* peeked = (pointNode*)pQueue.Peek();
		if (peeked)
		{
			sprintf(bufD, "Next Will be: (%d,%d,%d) (%d,%d,%d) %d ", peeked->x, peeked->y, peeked->z, peeked->px, peeked->py, peeked->pz, peeked->g);
			mapDebug(bufD);
		}
		else
		{
			mapDebug("Next Will be: 0");
		}
#endif // ifdef MAPDEBUG
		if (!closerEnd)
		{
			// Check if reached closerPoint, within radius of endPoint or 1 square away from an endSpecialLocation tile
			if (max(abs(x - pathFindX), abs(y - pathFindY)) <= radius && z == pathFindZ)
			{
				gotToEndPoint = 1;//max(pQueueIter->size(),200);// found a path, but keep looking to find shortest path
				endPoint.x    = x;
				endPoint.y    = y;
				endPoint.z    = z;
			}
			// standing by the depot
			if (endSpecialLocation &&
			    (tibiaMap.getPointType(x + 1, y, z) == endSpecialLocation ||
			     tibiaMap.getPointType(x - 1, y, z) == endSpecialLocation ||
			     tibiaMap.getPointType(x, y + 1, z) == endSpecialLocation ||
			     tibiaMap.getPointType(x, y - 1, z) == endSpecialLocation))
			{
				// endSpecialLocation found - where we stand is our "end point"
				gotToEndPoint = 1;//max(pQueueIter->size(),200);
				endPoint.x    = x;
				endPoint.y    = y;
				endPoint.z    = z;
			}
		}
		else if (x == pathFindX && y == pathFindY && z == pathFindZ)
		{
			gotToEndPoint = 1;
			endPoint      = lastEndPoint;
		}

		if (gotToEndPoint)
		{
			break;
			//tibiaMap.setPrevPoint(endPoint.x,endPoint.y,endPoint.z,px,py,pz);
			//tibiaMap.setPointDistance(endPoint.x,endPoint.y,endPoint.z,dist);
		}

		/**
		 * For each direction - check whether the next point is available on the map
		 * and whether it has not been visited before.
		 */

		/**
		 * 101 - open hole
		 * 102 - closed hole--
		 * 103 - grate--
		 * 201 - rope--
		 * 202 - magic rope
		 * 203 - ladder--
		 * 204 - stairs
		 * 301 - depot
		 * 302 - teleporter
		 * 303 - permanent block
		 */

		int forcedLevelChange = 0, usedLevelChange = 0; // if set to 1 then going north, south, east, west is forbidden
		CTibiaQueue<point> pointsToAdd;

		MapPointType currentPointType = tibiaMap.getPointType(x, y, z);

		// we directly go up using rope, magic rope, ladder, stairs
		if (currentPointType >= MAP_POINT_TYPE_ROPE && currentPointType < MAP_POINT_TYPE_DEPOT)
		{
			if (tibiaMap.isPointAvailableNoProh(x, y, z - 1))
			{
				pointsToAdd.Add(point(x, y, z - 1));
#ifdef MAPDEBUG
				mapDebug("go up");
#endif // ifdef MAPDEBUG
			}
			else    //go up oblong stairs
			{
				for (int xl = -1; xl <= 1; xl++)
				{
					for (int yl = -1; yl <= 1; yl++)
					{
						if (xl ^ yl)
						{
							MapPointType ptType = tibiaMap.getPointType(x + xl, y + yl, z - 1);
							if (ptType >= MAP_POINT_TYPE_OPEN_HOLE && ptType < MAP_POINT_TYPE_ROPE)
							{
								pointsToAdd.Add(point(x + xl, y + yl, z - 1));
								xl = yl = 9999;
#ifdef MAPDEBUG
								mapDebug("go up oblong stairs");
#endif // ifdef MAPDEBUG
							}
						}
					}
				}
			}
		}

		// we go directly down using open hole, closed hole, grate
		if (currentPointType >= MAP_POINT_TYPE_OPEN_HOLE && currentPointType < MAP_POINT_TYPE_ROPE)
		{
			if (tibiaMap.isPointAvailableNoProh(x, y, z + 1))
			{
				pointsToAdd.Add(point(x, y, z + 1));
#ifdef MAPDEBUG
				mapDebug("go down");
#endif // ifdef MAPDEBUG
			}
			else
			{
				for (int xl = -1; xl <= 1; xl++)
				{
					for (int yl = -1; yl <= 1; yl++)
					{
						if (xl ^ yl)
						{
							MapPointType ptType = tibiaMap.getPointType(x + xl, y + yl, z + 1);
							if (ptType >= MAP_POINT_TYPE_ROPE && ptType < MAP_POINT_TYPE_DEPOT)
							{
								pointsToAdd.Add(point(x + xl, y + yl, z + 1));
								xl = yl = 9999;
#ifdef MAPDEBUG
								mapDebug("go down oblong stairs");
#endif // ifdef MAPDEBUG
							}
						}
					}
				}
			}
		}


		// we go directly to teleporter position
		if (currentPointType == MAP_POINT_TYPE_TELEPORT || currentPointType == MAP_POINT_TYPE_USABLE_TELEPORT)
		{
			point dest = tibiaMap.getDestPoint(x, y, z);
			pointsToAdd.Add(point(dest.x, dest.y, dest.z));
#ifdef MAPDEBUG
			mapDebug("go teleport");
#endif // ifdef MAPDEBUG
		}

		//prevents the adding of squares around current square as possible next points except when standing on it(MAP_POINT_TYPE_BLOCK goes nowhere)
		if ((currentPointType == MAP_POINT_TYPE_OPEN_HOLE ||
		     currentPointType == MAP_POINT_TYPE_STAIRS ||
		     currentPointType == MAP_POINT_TYPE_TELEPORT ||
		     currentPointType == MAP_POINT_TYPE_BLOCK) && !(startX == x && startY == y && startZ == z))
			forcedLevelChange = 1;

		if (abs(x - px) > 1 || abs(y - py) > 1 || z != pz)
			usedLevelChange = 1;

		if (!forcedLevelChange || usedLevelChange)
		{
			findPathAllDirection(pointsToAdd, x, y, z);

			int xShift = -2, yShift = -2;
			while (!(xShift == 1 && yShift == 1))//simply goes in a 3x3 square
			{
				if (xShift == -2 && yShift == -2)
				{
					xShift = -1;
					yShift = -1;
				}
				else
				{
					xShift++;
				}
				if (xShift > 1)
				{
					xShift = -1;
					yShift++;
				}
				if (xShift == 0 && yShift == 0)
					xShift++;                    //skip middle

				MapPointType pointType = tibiaMap.getPointTypeNoProh(x + xShift, y + yShift, z);

				// special going up if ladder busy
				if ((pointType == MAP_POINT_TYPE_ROPE || pointType == MAP_POINT_TYPE_LADDER) && tibiaMap.isPointAvailableNoProh(x + xShift, y + yShift, z - 1))
					pointsToAdd.Add(point(x + xShift, y + yShift, z - 1));
				// special going down if grate or hole busy
				if ((pointType == MAP_POINT_TYPE_CLOSED_HOLE || pointType == MAP_POINT_TYPE_CRATE) && tibiaMap.isPointAvailableNoProh(x + xShift, y + yShift, z + 1))
					pointsToAdd.Add(point(x + xShift, y + yShift, z + 1));
			}
		}
		while (pointsToAdd.GetCount())
		{
			point addPoint = pointsToAdd.Remove();
			int newDist    = g + tibiaMap.calcDistance(x, y, z, addPoint.x, addPoint.y, addPoint.z);
			int oldDist    = tibiaMap.getPointDistance(addPoint.x, addPoint.y, addPoint.z);
			if (oldDist == 0 || newDist < oldDist)
			{
				tibiaMap.setPrevPoint(addPoint.x, addPoint.y, addPoint.z, x, y, z);
				tibiaMap.setPointDistance(addPoint.x, addPoint.y, addPoint.z, newDist);
				pointNode* pn = new pointNode(addPoint.x, addPoint.y, addPoint.z, x, y, z, newDist, 0, 0);
#ifdef MAPDEBUG
				if (pn->x == -858993460)
				{
					char buf2[1111];
					sprintf(buf2, "%d\n", pQueue.GetCount());
					mapDebug(buf2);
					int a = 0;
				}
#endif // ifdef MAPDEBUG
				pQueue.Add(pn->g, (DWORD)pn);
			}
		}
	}
	pqcountiter++;
#ifdef MAPDEBUG
	char buf[1111];
	sprintf(buf, "Ended loop count=%d gotToEndPoint=%d", pQueue.GetCount(), gotToEndPoint);
	mapDebug(buf);
#endif // ifdef MAPDEBUG

	//sprintf(buf,"%d",pqcountiter);
	//AfxMessageBox(buf);
	if (gotToEndPoint)
	{
		int pathPos     = 0;
		point currPoint = endPoint;
		point tmpPoint;

		while (currPoint.x != startX || currPoint.y != startY || currPoint.z != startZ)
		{
			tmpPoint.x         = currPoint.x;
			tmpPoint.y         = currPoint.y;
			tmpPoint.z         = currPoint.z;
			pathTab[pathPos].x = tmpPoint.x;
			pathTab[pathPos].y = tmpPoint.y;
			pathTab[pathPos].z = tmpPoint.z;

			currPoint.x = tibiaMap.getPrevPointX(tmpPoint.x, tmpPoint.y, tmpPoint.z);
			currPoint.y = tibiaMap.getPrevPointY(tmpPoint.x, tmpPoint.y, tmpPoint.z);
			currPoint.z = tibiaMap.getPrevPointZ(tmpPoint.x, tmpPoint.y, tmpPoint.z);
#ifdef MAPDEBUG
			sprintf(buf, "cur=(%d,%d,%d) new=(%d,%d,%d)", tmpPoint.x, tmpPoint.y, tmpPoint.z, currPoint.x, currPoint.y, currPoint.z);
			//mapDebug(buf);
#endif // ifdef MAPDEBUG

			pathPos++;

			if (pathPos > MAX_PATH_LEN - 5)
			{
				while (pQueue.GetCount())
				{
					pointNode* pn = (pointNode*)pQueue.Remove();
					delete pn;
				}
				return point(0, 0, 0);
			}
		}
		;
		pathTab[pathPos].x = startX;
		pathTab[pathPos].y = startY;
		pathTab[pathPos].z = startZ;
		pathPos++;
		//This is past our start point, zero out these value to ensure proper functioning of the getPathTabCount function
		pathTab[pathPos].x = 0;
		pathTab[pathPos].y = 0;
		pathTab[pathPos].z = 0;

#ifdef MAPDEBUG
		char buf2[256000];
		buf2[0] = 0;
		for (int ind = pathPos - 1; ind >= 0; ind--)
		{
			sprintf(buf2, "%s(%d,%d,%d)->", buf2, pathTab[ind].x, pathTab[ind].y, pathTab[ind].z);
		}
		mapDebug(buf2);
#endif // ifdef MAPDEBUG

#ifdef MAPDEBUG
		sprintf(buf, "111 pathPos=%d", pathPos);
		mapDebug(buf);
#endif // ifdef MAPDEBUG
		int pos = 0;
		for (pos = 0; pos < 10 && pos < pathPos - 1; pos++)
		{
			int curX  = pathTab[pathPos - pos - 1].x;
			int curY  = pathTab[pathPos - pos - 1].y;
			int curZ  = pathTab[pathPos - pos - 1].z;
			int nextX = pathTab[pathPos - pos - 2].x;
			int nextY = pathTab[pathPos - pos - 2].y;
			int nextZ = pathTab[pathPos - pos - 2].z;
#ifdef MAPDEBUG
			sprintf(buf, "X cur=(%d,%d,%d) next=(%d,%d,%d)", curX, curY, curZ, nextX, nextY, nextZ);
			mapDebug(buf);
#endif // ifdef MAPDEBUG
			path[pos] = STEP_NULL;
			if (curX + 1 == nextX && curY == nextY)
				path[pos] = STEP_EAST;
			else if (curX - 1 == nextX && curY == nextY)
				path[pos] = STEP_WEST;
			else if (curX == nextX && curY + 1 == nextY)
				path[pos] = STEP_SOUTH;
			else if (curX == nextX && curY - 1 == nextY)
				path[pos] = STEP_NORTH;
			else if (curX + 1 == nextX && curY + 1 == nextY)
				path[pos] = STEP_SOUTHEAST;
			else if (curX - 1 == nextX && curY + 1 == nextY)
				path[pos] = STEP_SOUTHWEST;
			else if (curX + 1 == nextX && curY - 1 == nextY)
				path[pos] = STEP_NORTHEAST;
			else if (curX - 1 == nextX && curY - 1 == nextY)
				path[pos] = STEP_NORTHWEST;

			if (curZ - 1 == nextZ)
			{
				// up
				if (pos == 0)
				{
					if (curX == nextX && curY == nextY)
						path[pos] = 0xD0;
					else
						path[pos] += 0xE0;
				}
				else
				{
					path[pos] = STEP_NULL; //undoes direction assignment - finishing the path
				}
				break;
			}
			else if (curZ + 1 == nextZ)
			{
				// down
				if (pos == 0)
				{
					if (curX == nextX && curY == nextY)
						path[pos] = 0xD1;
					else
						path[pos] += 0xF0;
				}
				else
				{
					path[pos] = STEP_NULL; //undoes direction assignment - finishing the path
				}
				break;
			}
			if (abs(curX - nextX) > 1 || abs(curY - nextY) > 1 || path[pos] == 0 && abs(curZ - nextZ) > 0)
			{
				if (pos == 0)
					path[pos] = 0xD2;
				else
					path[pos] = 0;//undoes direction assignment
				break;
			}
		}

#ifdef MAPDEBUG
		sprintf(buf, "XXX pos=%d pathPos=%d ", pos, pathPos);
		int i;
		for (i = 0; i < pos; i++)
		{
			char buf2[128];
			sprintf(buf2, " (%d)", path[i]);
			if (strlen(buf2) < 100)
				strcat(buf, buf2);
		}
		mapDebug(buf);
#endif // ifdef MAPDEBUG

		//store param passed to findPathOnMap
		lastDestX    = endX;
		lastDestY    = endY;
		lastDestZ    = endZ;
		lastEndPoint = endPoint;//keep actual location we found a path to

		pathTabLen = pathPos;
#ifdef MAPDEBUG
		sprintf(buf, "RETURN (%d,%d,%d)", endPoint.x, endPoint.y, endPoint.z);
		mapDebug(buf);
#endif // ifdef MAPDEBUG

		while (pQueue.GetCount())
		{
			pointNode* pn = (pointNode*)pQueue.Remove();
			delete pn;
		}

		return endPoint;
	}

	//reset stored information since no path was found
	resetPathfinderData();

	while (pQueue.GetCount())
	{
		pointNode* pn = (pointNode*)pQueue.Remove();
		delete pn;
	}
	return point(0, 0, 0);
}

int CModuleUtil::waitForOpenContainer(int contNr, int open)//max about 0.7s
{
	int t;
	for (t = 0; t < 30; t++)
	{
		CTibiaContainer *cont = CMemReader::getMemReader().readContainer(contNr);
		if (cont->flagOnOff == open)
		{
			delete cont;
			return 1;
		}
		delete cont;
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::lootItemFromSpecifiedContainer(int containerNr, CUIntArray *acceptedItems, int containerCarrying)
{
	CTibiaContainer *cont         = CMemReader::getMemReader().readContainer(containerNr);
	CTibiaContainer *contCarrying = CMemReader::getMemReader().readContainer(containerCarrying);
	int looted                    = 0;
	int numberItemsLooted         = 0;
	if (cont->flagOnOff)
	{
		int itemNr;
		for (itemNr = cont->itemsInside - 1; itemNr >= 0; itemNr--)
		{
			CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(itemNr);
			int i;
			for (i = 0; i < acceptedItems->GetSize(); i++)
			{
				int newItemsMin = CTibiaItem::getValueForConst("lootItemsAboveId");
				if ((int)item->objectId == (int)acceptedItems->GetAt(i) || (int)item->objectId >= newItemsMin)
				{
					// item needs to be looted
					int targetPos      = contCarrying->size - 1;
					int moved          = item->quantity ? item->quantity : 1;
					CTibiaTile *tile   = CTileReader::getTileReader().getTile(item->objectId);
					int stackedItemPos = -1;
					if (tile->stackable)
					{
						// if item is stackable then try to find a suitable stack for it
						for (stackedItemPos = 0; stackedItemPos < contCarrying->itemsInside; stackedItemPos++)
						{
							CTibiaItem *stackedItem = (CTibiaItem *)contCarrying->items.GetAt(stackedItemPos);
							if (stackedItem->objectId == item->objectId && stackedItem->quantity < 100)
							{
								// we have found a suitable stack! :)
								targetPos = stackedItemPos + looted;//since contCarrying is never updated add looted
								//Akilez: Stackable item overflow goes back into container at the same position
								if ((stackedItem->quantity + item->quantity) > 100)
									itemNr++;//Akilez: recheck this space in the container for lootables
								else
									looted--;//number of items in container will stay the same, undo increment

								moved                  = min(item->quantity, 100 - stackedItem->quantity);
								stackedItem->quantity += moved;
								item->quantity        -= moved;
								break;
							}
						}
					}
					// If not full or we found a spot inside the container for a stackable item move item
					if (contCarrying->itemsInside != contCarrying->size || stackedItemPos != -1 && stackedItemPos < contCarrying->itemsInside)
					{
						CPackSender::moveObjectBetweenContainers(item->objectId, 0x40 + containerNr, item->pos, 0x40 + containerCarrying, targetPos, moved);
						Sleep(CModuleUtil::randomFormula(300, 150));
						//char buf[111];
						//sprintf(buf,"id %d,cont %d,pos %d,tocont %d,topos %d,qty %d",item->objectId,0x40+containerNr,item->pos,0x40+containerCarrying,targetPos,moved);
						//CPackSender::sendTAMessage(buf);
						looted++;//needed for positioning of next loot item
						numberItemsLooted++;// return velue
					}
					break;
				}
			}
		}
	}

	delete cont;
	delete contCarrying;
	return numberItemsLooted;//Akilez: return value now reflects items looted
}

int CModuleUtil::lootItemsToSpecifiedContainers(int containerNr, CUIntArray *acceptedItems, int carriedContainers)
{
	CMemConstData* memConstData = &CMemReader::getMemReader();

	CTibiaContainer *cont = CMemReader::getMemReader().readContainer(containerNr);
	int looted            = 0;
	int numberItemsLooted = 0;
	if (cont->flagOnOff)
	{
		int itemNr;
		for (itemNr = cont->itemsInside - 1; itemNr >= 0; itemNr--)
		{
			CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(itemNr);
			int i;
			for (i = 0; i < acceptedItems->GetSize(); i++)
			{
				if ((int)item->objectId == (int)acceptedItems->GetAt(i))
				{
					// item needs to be looted
					for (int contNr = 0; contNr < memConstData->m_memMaxContainers; contNr++)
					{
						if (!(carriedContainers & (contNr + 1)))
							continue;
						CTibiaContainer *contCarrying = CMemReader::getMemReader().readContainer(contNr);
						if (contCarrying->flagOnOff == 0)
						{
							delete contCarrying;
							continue;
						}

						int targetPos      = contCarrying->size - 1;
						int moved          = item->quantity ? item->quantity : 1;
						CTibiaTile *tile   = CTileReader::getTileReader().getTile(item->objectId);
						int stackedItemPos = -1;
						if (tile->stackable)
						{
							// if item is stackable then try to find a suitable stack for it
							for (stackedItemPos = 0; stackedItemPos < contCarrying->itemsInside; stackedItemPos++)
							{
								CTibiaItem *stackedItem = (CTibiaItem *)contCarrying->items.GetAt(stackedItemPos);
								if (stackedItem->objectId == item->objectId && stackedItem->quantity < 100)
								{
									// we have found a suitable stack! :)
									targetPos = stackedItemPos + looted;//since contCarrying is never updated add looted
									//Akilez: Stackable item overflow goes back into container at the same position
									if ((stackedItem->quantity + item->quantity) > 100)
										itemNr++;//Akilez: recheck this space in the container for lootables
									else
										looted--;//number of items in container will stay the same, undo increment
									if (contCarrying->itemsInside != contCarrying->size)
										moved = item->quantity;
									else
										moved = min(item->quantity, 100 - stackedItem->quantity);
									stackedItem->quantity += moved;
									item->quantity        -= moved;
									break;
								}
							}
						}
						// If not full or we found a spot inside the container for a stackable item move item
						if (contCarrying->itemsInside != contCarrying->size || stackedItemPos != -1 && stackedItemPos < contCarrying->itemsInside)
						{
							CPackSender::moveObjectBetweenContainers(item->objectId, 0x40 + containerNr, item->pos, 0x40 + contNr, targetPos, moved);
							Sleep(CModuleUtil::randomFormula(300, 150));
							//char buf[111];
							//sprintf(buf,"id %d,cont %d,pos %d,tocont %d,topos %d,qty %d",item->objectId,0x40+containerNr,item->pos,0x40+containerCarrying,targetPos,moved);
							//CPackSender::sendTAMessage(buf);
							looted++;//needed for positioning of next loot item
							numberItemsLooted++;// return velue
							goto gotoNextItem;
						}
						delete contCarrying;
					}
				}
			}
gotoNextItem:;
		}
	}
	delete cont;
	return numberItemsLooted;//Akilez: return value now reflects items looted
}

void CModuleUtil::lootItemFromContainer(int contNr, CUIntArray *acceptedItems, int ignoreCont1 /*=-1*/, int ignoreCont2 /*=-1*/)
{
	CMemConstData* memConstData = &CMemReader::getMemReader();

	// find first free container
	int qtyLooted   = 0;
	int openContNr  = 0;
	int openContMax = CMemReader::getMemReader().readOpenContainerCount();
	for (int openCont = 0; openCont < memConstData->m_memMaxContainers && openContNr < openContMax; openCont++)
	{
		if (openCont == contNr || openCont == ignoreCont1 || openCont == ignoreCont2)
			continue;

		CTibiaContainer *targetCont = CMemReader::getMemReader().readContainer(openCont);
		if (targetCont->flagOnOff)
		{
			openContNr++;
			CTibiaContainer *sourceCont = CMemReader::getMemReader().readContainer(contNr);
			qtyLooted = (int)(CModuleUtil::lootItemFromSpecifiedContainer(contNr, acceptedItems, openCont));//Akilez: loot items and get # of items returned
			if (qtyLooted)
				waitForItemsInsideChange(contNr, sourceCont->itemsInside);
			delete sourceCont;

			if (targetCont->itemsInside < targetCont->size)
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

void CModuleUtil::sleepWithStop(int ms, int *stopFlag)
{
	while (ms > 50 && !(*stopFlag))
	{
		Sleep(50);
		ms -= 50;
	}
	if (*stopFlag)
		return;
	Sleep(ms);
}

void CModuleUtil::eatItemFromContainer(int contNr)
{
	CUIntArray* acceptedItems = CTibiaItem::getFoodIdArrayPtr();
	CTibiaItem* item          = lookupItem(contNr, acceptedItems);
	if (item->objectId)
	{
		for (int i = 0; i < 3 && i < (item->quantity ? item->quantity : 1); i++)
		{
			CPackSender::useItemInContainer(item->objectId, 0x40 + contNr, item->pos);
			Sleep(CModuleUtil::randomFormula(400, 100));
		}
	}
	delete item;
}

int CModuleUtil::waitToApproachSquare(int x, int y)// depends on speed of character (1.4 seconds for lvl 1 doing diagonal)
{
	int spaceCount = 15;
	//wait until square reached and return if more than 15 steps
	while (spaceCount-- > 0)
	{
		CTibiaCharacter self;
		CMemReader::getMemReader().readSelfCharacter(&self);
		static int newX       = self.x;
		static int newY       = self.y;
		if (max(abs(self.x - x), abs(self.y - y)) <= 1)
		{
			break;
		}

		//wait until square changes and return if took too long
		int iterCount = 28;
		while (iterCount-- > 0)
		{
			CTibiaCharacter self;
			CMemReader::getMemReader().readSelfCharacter(&self);
			if (self.moving == 0)
			{
				iterCount = 0;
			}
			else if (self.x != newX || self.y != newY)
			{
				newX = self.x;
				newY = self.y;
				break;
			}
			Sleep(50);
		}
		if (iterCount == -1)
			//AfxMessageBox("return 0,(1)");
			return 0;
	}
	if (spaceCount == -1)
		//AfxMessageBox("return 0,(2)");
		return 0;
	//AfxMessageBox("return 1");
	return 1;
}

int CModuleUtil::waitToStandOnSquare(int x, int y)// depends on speed of character (1.4 seconds for lvl 1 doing diagonal)// see tileDelay
{
	int spaceCount = 15;
	//wait until square reached and return if more than 15 steps
	while (spaceCount-- > 0)
	{
		CTibiaCharacter self;
		CMemReader::getMemReader().readSelfCharacter(&self);
		static int newX       = self.x;
		static int newY       = self.y;
		if (max(abs(self.x - x), abs(self.y - y)) == 0)
		{
			break;
		}

		//wait until square changes and return if took too long
		int iterCount = 28;
		while (iterCount-- > 0)
		{
			CTibiaCharacter self;
			CMemReader::getMemReader().readSelfCharacter(&self);
			if (self.moving == 0)
			{
				iterCount = 0;
			}
			else if (self.x != newX || self.y != newY)
			{
				newX = self.x;
				newY = self.y;
				break;
			}
			Sleep(50);
		}
		if (iterCount == -1)
			//AfxMessageBox("return 0,(1)");
			return 0;
	}
	if (spaceCount == -1)
		//AfxMessageBox("return 0,(2)");
		return 0;
	//AfxMessageBox("return 1");
	return 1;
}

int CModuleUtil::waitForCreatureDisappear(int creatureNr)//ranges from near instantaneous to 1.0 secs
{
	int iterCount = 25;
	while (iterCount-- > 0)
	{
		CTibiaCharacter ch;
		CMemReader::getMemReader().readVisibleCreature(&ch, creatureNr);
		if (!ch.visible)
		{
			return 1;
		}
		Sleep(50);
	}
	return 0;
}

int CModuleUtil::waitForCreatureDisappear(int x, int y, int tibiaId)
{
	int xReturn, yReturn;
	return waitForCreatureDisappear(x, y, tibiaId, xReturn, yReturn);
}

int CModuleUtil::waitForCreatureDisappear(int x, int y, int tibiaId, int &xReturn, int &yReturn)
{
	int iterCount = 20;
	while (iterCount-- > 0)
	{
		int found = 0;
		int pos;
		for (int xOffset = -1; xOffset < 1; xOffset++)
		{
			for (int yOffset = -1; yOffset < 1; yOffset++)
			{
				int stackCount = CMemReader::getMemReader().mapGetPointItemsCount(point(x + xOffset, y + yOffset, 0));
				for (pos = 0; pos < stackCount; pos++)
				{
					int tileId    = CMemReader::getMemReader().mapGetPointItemId(point(x + xOffset, y + yOffset, 0), pos);
					int extraData = CMemReader::getMemReader().mapGetPointItemExtraInfo(point(x + xOffset, y + yOffset, 0), pos, 1);
					if (tileId == 99 && extraData == tibiaId)
					{
						found   = 1;
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

int CModuleUtil::calcLootChecksum(time_t tm, int killNr, int nameLen, int itemNr, int objectId, int qty, int lootInBags, int creatX, int creatY, int creatZ)
{
	return (int)tm * 3 + killNr * 17 + nameLen * 3 + itemNr * 15 + objectId * 19 + qty * 13 + lootInBags * 71 + creatX * 23 + creatY * 31 + creatZ * 59;
	//Old checksum return tm*3+killNr*17+nameLen*3+itemNr*15+objectId*19+qty*13+lootInBags*71;
}

// In order to 'walk through' other players, this function must be called twice within the walkthrough cutoff time.
// Todo: Implement 'walking through' other players in this function
void CModuleUtil::executeWalk(int startX, int startY, int startZ, uint8_t path[15])
{
	static int lastExecuteWalkTm = 0; // indicates when last a "real" walk was executed
	static int lastStartChangeTm = 0; // indicates when start point was changed for the last time
	//static int movedDiagonally=0; // will make delay to resend walk 3x longer
	static int lastEndX         = 0, lastEndY = 0, lastEndZ = 0;
	static int lastStartX       = 0, lastStartY = 0, lastStartZ = 0;
	CMemConstData* memConstData = &CMemReader::getMemReader();
	CTibiaCharacter self;
	CMemReader::getMemReader().readSelfCharacter(&self);
	CTibiaMap& tibiaMap         = CTibiaMap::getTibiaMap();

	int currentTm = CMemReader::getMemReader().getCurrentTm();
	int pathSize;
	for (pathSize = 0; pathSize < 15 && path[pathSize]; pathSize++)
	{
	}
#ifdef MAPDEBUG
	char buf[512];
	sprintf(buf, "pathsize=%d assumed=(%d,%d,%d) now=(%d,%d,%d) delta=(%d,%d,%d)=%d", pathSize, startX, startY, startZ, self.x, self.y, self.z, self.x - startX, self.y - startY, self.z - startZ, abs(self.x - startX) + abs(self.y - startY) + abs(self.z - startZ));
	mapDebug(buf);
#endif // ifdef MAPDEBUG

	//Start walking after changing floors
	if (lastStartZ != startZ)
	{
		lastStartX        = startX;
		lastStartY        = startY;
		lastStartZ        = startZ;
		lastStartChangeTm = 0;
	}
	else if (lastStartX != startX || lastStartY != startY || lastStartZ != startZ)
	{
		//movedDiagonally=(lastStartX!=startX) && (lastStartY!=startY);
		lastStartX = startX;
		lastStartY = startY;
		lastStartZ = startZ;
		//lastStartChangeTm=currentTm;
	}
#ifdef MAPDEBUG
	buf[0] = 0;
	for (int tracePath = 0; tracePath < pathSize; tracePath++)
	{
		sprintf(buf, "%s(%X) ", buf, path[tracePath]);
	}
	mapDebug(buf);
#endif // ifdef MAPDEBUG
	if (pathSize == 1 && path[0] >= 0xD0)// || pathSize==2&&path[1]>=0xE0)
	{
		lastEndX          = lastEndY = lastEndZ = 0;
		lastExecuteWalkTm = currentTm;
		switch (path[0])
		{
		case 0xD0:
		{
			// // go up standard (201-299 slots)
			switch (tibiaMap.getPointType(self.x, self.y, self.z))
			{
			case MAP_POINT_TYPE_ROPE:
			{
				CUIntArray itemsAccepted;
				int contNr;
				itemsAccepted.Add(CTibiaItem::getValueForConst("rope"));
				for (contNr = 0; contNr < memConstData->m_memMaxContainers; contNr++)
				{
					CTibiaItem *item = CModuleUtil::lookupItem(contNr, &itemsAccepted);
					if (item->objectId)
					{
						int ropeId = CMemReader::getMemReader().mapGetPointItemId(point(0, 0, 0), 0);
						CPackSender::useWithObjectFromContainerOnFloor(CTibiaItem::getValueForConst("rope"), 0x40 + contNr, item->pos, ropeId, self.x, self.y, self.z);
						delete item;
						break;
					}
					delete item;
				}
				break;
			}
			case MAP_POINT_TYPE_MAGICROPE:
				// go up using 'magic rope' [for pacc only]
				CPackSender::say("exani tera");
				break;
			case MAP_POINT_TYPE_LADDER:
			{
				// ladder
				// fix 02.02.2008: ladder code needs to be evaluated on the fly
				int count      = CMemReader::getMemReader().mapGetPointItemsCount(point(0, 0, 0));
				int ladderCode = 0;
				int i;

				for (i = 0; i < count; i++)
				{
					int tileId           = CMemReader::getMemReader().mapGetPointItemId(point(0, 0, 0), i);
					CTibiaTile *tileData = CTileReader::getTileReader().getTile(tileId);
					if (tileId != 99 && tileData->goUp && tileData->requireUse)
					{
						ladderCode = tileId;
						break;
					}
				}                 // for i
				if (ladderCode)
					CPackSender::useItemOnFloor(ladderCode, self.x, self.y, self.z);
				break;
			}
			case MAP_POINT_TYPE_STAIRS:
				// do nothing
				break;
			}
			break;
		}         // case 0xD0
		case 0xD1:
		{
			// go down (101-199 slots)
			switch (tibiaMap.getPointType(self.x, self.y, self.z))
			{
			case MAP_POINT_TYPE_OPEN_HOLE:
				// do nothing
				break;
			case MAP_POINT_TYPE_CLOSED_HOLE:
			{
				CTibiaItem *shovel = NULL;
				CUIntArray itemsAccepted;
				int contNr;
				itemsAccepted.Add(CTibiaItem::getValueForConst("shovel"));
				for (contNr = 0; contNr < memConstData->m_memMaxContainers; contNr++)
				{
					CTibiaItem *item = CModuleUtil::lookupItem(contNr, &itemsAccepted);
					if (item->objectId)
					{
						shovel = item;
						break;
					}
					delete item;
				}
				if (shovel)
				{
					// shovel found, so proceed with opening hole
					int mode = 0;
					if (tibiaMap.isPointAvailable(self.x - 1, self.y, self.z))
						mode = 1;
					if (tibiaMap.isPointAvailable(self.x + 1, self.y, self.z))
						mode = 2;
					if (tibiaMap.isPointAvailable(self.x, self.y - 1, self.z))
						mode = 3;
					if (tibiaMap.isPointAvailable(self.x, self.y + 1, self.z))
						mode = 4;
					switch (mode)
					{
					case 1:
						CPackSender::stepLeft();
						break;
					case 2:
						CPackSender::stepRight();
						break;
					case 3:
						CPackSender::stepUp();
						break;
					case 4:
						CPackSender::stepDown();
						break;
					}
					Sleep(CModuleUtil::randomFormula(1100, 300));
					CTibiaCharacter self2;
					CMemReader::getMemReader().readSelfCharacter(&self2);
					int count              = CMemReader::getMemReader().mapGetPointItemsCount(point(self.x - self2.x, self.y - self2.y, 0));
					int holeId             = 0;
					int i;
					for (i = 0; i < count; i++)
					{
						int tileId = CMemReader::getMemReader().mapGetPointItemId(point(self.x - self2.x, self.y - self2.y, 0), i);
						//char buf[128];
						//sprintf(buf,"[%d/%d] me (%d,%d,%d) opening (%d,%d,%d) tile %d",i,count,self2.x,self2.y,self2.z,self.x,self.y,self.z,tileId);
						//::MessageBox(0,buf,buf,0);
						if (tileId != 99)
						{
							CTibiaTile *tile = CTileReader::getTileReader().getTile(tileId);

							if (tile->requireShovel)
								holeId = tileId;
							//remove an item if it is on top of hole
							if (!tile->notMoveable)
							{
								int qty = CMemReader::getMemReader().mapGetPointItemExtraInfo(point(self.x - self2.x, self.y - self2.y, 0), i, 1);
								CPackSender::moveObjectFromFloorToFloor(tileId, self.x, self.y, self.z, self2.x, self2.y, self2.z, qty ? qty : 1);
								Sleep(CModuleUtil::randomFormula(400, 200));
								i--;
								count--;
							}
						}
					}
					//did not find tile, open top tile (hopefully this is a hole)
					if (!holeId && count)
						holeId = CMemReader::getMemReader().itemOnTopCode(self.x - self2.x, self.y - self2.y);

					if (holeId)
					{
						CPackSender::useWithObjectFromContainerOnFloor(CTibiaItem::getValueForConst("shovel"), 0x40 + contNr, shovel->pos, holeId, self.x, self.y, self.z);
						Sleep(CModuleUtil::randomFormula(900, 300));

						switch (mode)
						{
						case 2:
							CPackSender::stepLeft();
							break;
						case 1:
							CPackSender::stepRight();
							break;
						case 4:
							CPackSender::stepUp();
							break;
						case 3:
							CPackSender::stepDown();
							break;
						}
						Sleep(CModuleUtil::randomFormula(1100, 300));
					}
					delete shovel;
				}
				break;
			}
			case MAP_POINT_TYPE_CRATE:
			{
				//Find a free spot to untrash, default under self if we're not covering the crate
				int freeX = 0, freeY = 0;
				for (int x = -1; x <= 1; x++)
				{
					for (int y = -1; y <= 1; y++)
					{
						if (x || y)
						{
							if (tibiaMap.isPointAvailableNoProh(self.x + x, self.y + y, self.z))
							{
								freeX = x;
								freeY = y;
								x     = 999;  //exit both loops
								y     = 999;
							}
						}
					}
				}
				int count   = CMemReader::getMemReader().mapGetPointItemsCount(point(0, 0, 0));
				int grateId = 0;
				int i;
				for (i = 0; i < count; i++)
				{
					int tileId = CMemReader::getMemReader().mapGetPointItemId(point(0, 0, 0), i);
					//char buf[128];
					//sprintf(buf,"[%d/%d] me (%d,%d,%d) opening (%d,%d,%d) tile %d",i,count,self2.x,self2.y,self2.z,self.x,self.y,self.z,tileId);
					//::MessageBox(0,buf,buf,0);
					if (tileId != 99)
					{
						CTibiaTile *tile = CTileReader::getTileReader().getTile(tileId);

						if (tile->requireUse)
							grateId = tileId;
						//remove an item if it is on top of grate
						if (!tile->notMoveable)
						{
							int qty = CMemReader::getMemReader().mapGetPointItemExtraInfo(point(0, 0, 0), i, 1);
							CPackSender::moveObjectFromFloorToFloor(tileId, self.x, self.y, self.z, self.x + freeX, self.y + freeY, self.z, qty ? qty : 1);
							Sleep(CModuleUtil::randomFormula(400, 200));
							i--;
							count--;
						}
					}
				}
				//did not find tile, use top tile (hopefully this is a grate)
				if (!grateId && count > 1)
					grateId = CMemReader::getMemReader().itemOnTopCode(0, 0);

				if (grateId)
					CPackSender::useItemOnFloor(grateId, self.x, self.y, self.z);
				break;
			}
			}         // switch pointType
		}         // case 0xD1
		case 0xD2:
		{
			// teleporter it will have walked onto it
			break;
		}         // case 0xD2
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
			int modX = self.x;
			int modY = self.y;
			if (path[0] == 0xE1)
				modX++;
			if (path[0] == 0xE2)
			{
				modX++;
				modY--;
			}
			if (path[0] == 0xE3)
				modY--;
			if (path[0] == 0xE4)
			{
				modX--;
				modY--;
			}
			if (path[0] == 0xE5)
				modX--;
			if (path[0] == 0xE6)
			{
				modX--;
				modY++;
			}
			if (path[0] == 0xE7)
				modY++;
			if (path[0] == 0xE8)
			{
				modX++;
				modY++;
			}
			switch (tibiaMap.getPointTypeNoProh(modX, modY, self.z))
			{
			case 201:
				// rope
			{
				CUIntArray itemsAccepted;
				int contNr;
				itemsAccepted.Add(CTibiaItem::getValueForConst("rope"));
				for (contNr = 0; contNr < memConstData->m_memMaxContainers; contNr++)
				{
					CTibiaItem *item = CModuleUtil::lookupItem(contNr, &itemsAccepted);
					if (item->objectId)
					{
						int ropeId = CMemReader::getMemReader().mapGetPointItemId(point(modX - self.x, modY - self.y, 0), 0);
						CPackSender::useWithObjectFromContainerOnFloor(CTibiaItem::getValueForConst("rope"), 0x40 + contNr, item->pos, ropeId, modX, modY, self.z);
						delete item;
						break;
					}
					delete item;
				}
				break;
			}
			case 203:
			{
				//buf[111];
				//sprintf(buf,"self %d,%d, mod %d %d",self.x,self.y,modX,modY);
				//CPackSender::sendTAMessage(buf);
				// ladder
				// fix 02.02.2008: ladder code needs to be evaluated on the fly
				int count      = CMemReader::getMemReader().mapGetPointItemsCount(point(modX - self.x, modY - self.y, 0));
				int ladderCode = 0;
				int i;

				for (i = 0; i < count; i++)
				{
					int tileId           = CMemReader::getMemReader().mapGetPointItemId(point(modX - self.x, modY - self.y, 0), i);
					CTibiaTile *tileData = CTileReader::getTileReader().getTile(tileId);
					if (tileId != 99 && tileData->goUp && tileData->requireUse)
					{
						ladderCode = tileId;
						break;
					}
				}                 // for i
				if (ladderCode)
					CPackSender::useItemOnFloor(ladderCode, modX, modY, self.z);
				break;

				// ladder
			}
			}
			break;
		}         // case 0xE1-E4
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
			int modX = self.x;
			int modY = self.y;
			if (path[0] == 0xF1)
				modX++;
			if (path[0] == 0xF2)
			{
				modX++;
				modY--;
			}
			if (path[0] == 0xF3)
				modY--;
			if (path[0] == 0xF4)
			{
				modX--;
				modY--;
			}
			if (path[0] == 0xF5)
				modX--;
			if (path[0] == 0xF6)
			{
				modX--;
				modY++;
			}
			if (path[0] == 0xF7)
				modY++;
			if (path[0] == 0xF8)
			{
				modX++;
				modY++;
			}

			switch (tibiaMap.getPointTypeNoProh(modX, modY, self.z))
			{
			case MAP_POINT_TYPE_CLOSED_HOLE:
				// closed hole - use shovel to open
			{
				CTibiaItem *shovel = NULL;
				CUIntArray itemsAccepted;
				int contNr;
				itemsAccepted.Add(CTibiaItem::getValueForConst("shovel"));
				for (contNr = 0; contNr < memConstData->m_memMaxContainers; contNr++)
				{
					CTibiaItem *item = CModuleUtil::lookupItem(contNr, &itemsAccepted);
					if (item->objectId)
					{
						shovel = item;
						break;
					}
					delete item;
				}
				if (shovel)
				{
					// shovel found, so proceed with opening hole
					int count = CMemReader::getMemReader().mapGetPointItemsCount(point(modX - self.x, modY - self.y, 0));

					int holeId     = 0;
					int movePlayer = 0;
					int i;
					for (i = 0; i < count; i++)
					{
						int tileId = CMemReader::getMemReader().mapGetPointItemId(point(modX - self.x, modY - self.y, 0), i);
						//char buf[128];
						//sprintf(buf,"[%d/%d] me (%d,%d,%d) opening (%d,%d,%d) tile %d",i,count,self2.x,self2.y,self2.z,self.x,self.y,self.z,tileId);
						//::MessageBox(0,buf,buf,0);
						if (tileId != 99)
						{
							CTibiaTile *tile = CTileReader::getTileReader().getTile(tileId);

							if (tile->requireShovel)
								holeId = tileId;
							//remove an item if it is on top of hole
							if (!tile->notMoveable)
							{
								int qty = CMemReader::getMemReader().mapGetPointItemExtraInfo(point(modX - self.x, modY - self.y, 0), i, 1);
								CPackSender::moveObjectFromFloorToFloor(tileId, modX, modY, self.z, self.x, self.y, self.z, qty ? qty : 1);
								Sleep(CModuleUtil::randomFormula(400, 200));
								i--;
								count--;
							}
						}
						else
						{
							movePlayer = 1;
						}
					}
					if (movePlayer)
					{
						int freeX = 0, freeY = 0;
						for (int x = -1; x <= 1; x++)
						{
							for (int y = -1; y <= 1; y++)
							{
								if (x || y)
								{
									if (tibiaMap.isPointAvailable(modX + x, modY + y, self.z))
									{
										freeX = x;
										freeY = y;
										x     = 999;          //exit both loops
										y     = 999;
									}
								}
							}
						}
						CPackSender::moveObjectFromFloorToFloor(99, modX, modY, self.z, modX + freeX, modY + freeY, self.z, 1);
						Sleep(CModuleUtil::randomFormula(1500, 300));
					}
					//did not find tile, open last tile in list(hopefully this is a hole)
					if (!holeId && count)
						holeId = CMemReader::getMemReader().itemOnTopCode(modX - self.x, modY - self.y);

					if (holeId)
					{
						CPackSender::useWithObjectFromContainerOnFloor(CTibiaItem::getValueForConst("shovel"), 0x40 + contNr, shovel->pos, holeId, modX, modY, self.z);
						Sleep(CModuleUtil::randomFormula(900, 300));

						int mode = path[0] & 0xF;
						uint8_t path[3];
						path[1] = STEP_NULL;
						int size = 0;
						if (modX - self.x && modY - self.y)
						{
							if (tibiaMap.isPointAvailable(modX, self.y, self.z))
							{
								switch (mode)
								{
								case 2:
									path[0] = STEP_EAST;
									path[1] = STEP_NORTH;
									break;
								case 8:
									path[0] = STEP_EAST;
									path[1] = STEP_SOUTH;
									break;
								case 4:
									path[0] = STEP_WEST;
									path[1] = STEP_NORTH;
									break;
								case 6:
									path[0] = STEP_WEST;
									path[1] = STEP_SOUTH;
									break;
								}
							}
							else if (modY && tibiaMap.isPointAvailable(self.x, modY, self.z))
							{
								switch (mode)
								{
								case 2:
									path[0] = STEP_SOUTH;
									path[1] = STEP_WEST;
									break;
								case 8:
									path[0] = STEP_SOUTH;
									path[1] = STEP_EAST;
									break;
								case 4:
									path[0] = STEP_NORTH;
									path[1] = STEP_WEST;
									break;
								case 6:
									path[0] = STEP_NORTH;
									path[1] = STEP_EAST;
									break;
								}
							}
							path[2] = STEP_NULL;
							size    = 2;
						}
						if (path[1] == STEP_NULL)
						{
							path[0] = mode;
							size    = 1;
						}

						CPackSender::stepMulti(path, size);
						Sleep(CModuleUtil::randomFormula(1100, 300));
					}
					delete shovel;
				}
				break;
			}
			case MAP_POINT_TYPE_CRATE:
			{
				int count = CMemReader::getMemReader().mapGetPointItemsCount(point(modX - self.x, modY - self.y, 0));

				int grateId = 0;
				int i;
				for (i = 0; i < count; i++)
				{
					int tileId = CMemReader::getMemReader().mapGetPointItemId(point(modX - self.x, modY - self.y, 0), i);
					//char buf[128];
					//sprintf(buf,"[%d/%d] me (%d,%d,%d) opening (%d,%d,%d) tile %d",i,count,self2.x,self2.y,self2.z,self.x,self.y,self.z,tileId);
					//::MessageBox(0,buf,buf,0);
					if (tileId != 99)
					{
						CTibiaTile *tile = CTileReader::getTileReader().getTile(tileId);

						if (tile->requireUse)
							grateId = tileId;
						//remove an item if it is on top of grate
						if (!tile->notMoveable)
						{
							int qty = CMemReader::getMemReader().mapGetPointItemExtraInfo(point(modX - self.x, modY - self.y, 0), i, 1);
							CPackSender::moveObjectFromFloorToFloor(tileId, modX, modY, self.z, self.x, self.y, self.z, qty ? qty : 1);
							Sleep(CModuleUtil::randomFormula(400, 200));
							i--;
							count--;
						}
					}
				}
				if (!grateId && count)
					grateId = CMemReader::getMemReader().itemOnTopCode(modX - self.x, modY - self.y);
				if (grateId)
					CPackSender::useItemOnFloor(grateId, modX, modY, self.z);
				break;
			}
			}
			break;
		}         // case 0xF1-F4
		} // switch path[0]

		//wait for change of level
		int iterCount = 60;
		while (iterCount-- > 0)
		{
			CTibiaCharacter selfTmp;
			CMemReader::getMemReader().readSelfCharacter(&selfTmp);
			if (selfTmp.z != self.z)
			{
				break;
			}
			Sleep(50);
		}
	}
	else
	{
		/**
		 * Walk if :
		 * 1. we found a way AND
		 * 2. last planned end point equals to the start point now (means: we reach "final" point) OR
		 * 3. last startX change was > 2.5s ago (means: we are stuck) OR
		 * 4. last walk was over 15s ago (means: walk restart, lag, etc.)
		 */

		int lastEndEqStart = (lastEndX == startX && lastEndY == startY && lastEndZ == startZ);

		//CMemReader::getMemReader().getMemIntValue(CTibiaItem::getValueForConst("addrTilesToGo"))==0
		//time to walk 1 sqm is inverse to the speed, double speed== half the time
		//int maxTileDelay=(int)(tileSpeed*(movedDiagonally?3:1)*1000/self.walkSpeed)+600;
		int pathLen         = CMemUtil::getMemUtil().GetMemIntValue(CTibiaItem::getValueForConst("addrTilesToGo"));
		int pathInd         = CMemUtil::getMemUtil().GetMemIntValue(CTibiaItem::getValueForConst("addrCurrentTileToGo"));
		bool stoppedWalking = self.moving == 0 && (pathLen == pathInd || pathLen == 0);

		//int pathStartAddr=CMemReader::getMemReader().m_memAddressPathToGo;
		//int tmpX=lastEndX,tmpY=lastEndY;

		//while (tmpX!=self.x && tmpY!=self.y){
		//		case 1: tmpX--;break;
		//		case 5: tmpX++;break;
		//		case 7: tmpY--;break;
		//			case 3: tmpY++;break;
		//			case 8: tmpY--;tmpX--;break;
		//			case 6: tmpY--;tmpX++;break;
		//			case 2: tmpY++;tmpX--;break;
		//			case 4: tmpY++;tmpX++;break;
		//	}

		if (pathSize > 0 && (lastEndEqStart || stoppedWalking || currentTm - lastExecuteWalkTm > 15000))
		{
			// 'normal' stepping limited to 10 steps
			if (pathSize > 10)
				pathSize = 10;
#ifdef MAPDEBUG
			sprintf(buf, "Send walk to server. size=%d,lastEndEqStart=%d,lastStartChangeTm_diff=%d,lastExecuteWalkTm=%d,stoppedWalking=%d", pathSize, lastEndEqStart, currentTm - lastStartChangeTm, currentTm - lastExecuteWalkTm, stoppedWalking);
			mapDebug(buf);
#endif // ifdef MAPDEBUG
			int x1 = startX;
			int y1 = startY;

			lastEndX          = startX;
			lastEndY          = startY;
			lastEndZ          = startZ;
			lastExecuteWalkTm = currentTm;
			int offset = 0;

			for (int i = 0; i < pathSize; i++)
			{
				//Finds the position (offset) we need to start from in case we moved since the path was found
				if (self.x != x1 || self.y != y1)
				{
					offset++;
					switch (path[i])
					{
					case STEP_EAST:
						x1++;
						break;
					case STEP_WEST:
						x1--;
						break;
					case STEP_SOUTH:
						y1++;
						break;
					case STEP_NORTH:
						y1--;
						break;
					case STEP_SOUTHEAST:
						y1++;
						x1++;
						break;
					case STEP_SOUTHWEST:
						y1++;
						x1--;
						break;
					case STEP_NORTHEAST:
						y1--;
						x1++;
						break;
					case STEP_NORTHWEST:
						y1--;
						x1--;
						break;
					}
				}

				//Finds endX and endY, the endpoint of multisteps normally sent by the Tibia client
				int prevlastEndX = lastEndX, prevlastEndY = lastEndY;
				switch (path[i])
				{
				case STEP_EAST:
					lastEndX++;
					break;
				case STEP_WEST:
					lastEndX--;
					break;
				case STEP_SOUTH:
					lastEndY++;
					break;
				case STEP_NORTH:
					lastEndY--;
					break;
				case STEP_SOUTHEAST:
					lastEndY++;
					lastEndX++;
					break;
				case STEP_SOUTHWEST:
					lastEndY++;
					lastEndX--;
					break;
				case STEP_NORTHEAST:
					lastEndY--;
					lastEndX++;
					break;
				case STEP_NORTHWEST:
					lastEndY--;
					lastEndX--;
					break;
				}
				if (abs(x1 - lastEndX) > 8 || abs(y1 - lastEndY) > 6)
				{
					lastEndX = prevlastEndX;
					lastEndY = prevlastEndY;
					pathSize = i;
					break;
				}
			}
			if (offset >= pathSize || startZ != self.z)
				return;                                  //failed to find our position on the proposed path

			CPackSender::stepMulti(path + offset * sizeof(path[0]), pathSize - offset);

			//sprintf(buf,"walk: (%d,%d,%d)->(%d,%d,%d) [%d,%d,%d] %d,%d p=%d",startX,startY,startZ,lastEndX,lastEndY,lastEndZ,lastStartX,lastStartY,lastStartZ,currentTm-lastStartChangeTm,currentTm-lastExecuteWalkTm,pathSize);
			//testDebug(buf);

			if (stoppedWalking)
				// adjust lastStartChangeTm to not flood tibia server with walks
				lastStartChangeTm = currentTm + 1000;
		}
	}
}

void CModuleUtil::prepareProhPointList()
{
	CMemReader& reader = CMemReader::getMemReader();
	CTibiaCharacter self;
	reader.readSelfCharacter(&self);
	// prepare prohPoint list
	CTibiaMap::getTibiaMap().prohPointClear();
	int creatureNr;
	for (creatureNr = 0; creatureNr < reader.m_memMaxCreatures; creatureNr++)
	{
		CTibiaCharacter ch;
		reader.readVisibleCreature(&ch, creatureNr);
		if (ch.tibiaId == 0)
		{
			break;
		}
		if (ch.visible && (ch.x != self.x || ch.y != self.y || ch.z != self.z))
		{
			if (ch.blocking)
			{
				CTibiaMap::getTibiaMap().prohPointAdd(ch.x, ch.y, ch.z);
			}
			else
			{
				point checkpoint(ch.x - self.x, ch.y - self.y, ch.z - self.z);
				int stackCount = CMemReader::getMemReader().mapGetPointItemsCount(checkpoint);
				for (int pos = 0; pos != stackCount; pos++)
				{
					int itemId = reader.mapGetPointItemId(checkpoint, pos);

					if (10145 == itemId ||
					    10146 == itemId /*depot tile*/ ||
					    1647 == itemId ||
					    1649 == itemId ||
					    1665 == itemId ||
					    1667 == itemId ||
					    1679 == itemId ||
					    1681 == itemId ||
					    1688 == itemId ||
					    1697 == itemId ||
					    5103 == itemId ||
					    5121 == itemId ||
					    6897 == itemId ||
					    7039 == itemId ||
					    9557 == itemId ||
					    9864 == itemId /*open level doors*/)
						CTibiaMap::getTibiaMap().prohPointAdd(ch.x, ch.y, ch.z);
				}
			}
		}
	}
}

int CModuleUtil::findNextClosedContainer(int afterCont /*=-1*/)
{
	CTibiaContainer *container;
	CMemReader& reader = CMemReader::getMemReader();
	int targetBag;
	for (targetBag = afterCont + 1; targetBag < reader.m_memMaxContainers; targetBag++)
	{
		container = reader.readContainer(targetBag);
		if (!container->flagOnOff)
		{
			delete container;
			break;
		}
		delete container;
	}

	if (targetBag == reader.m_memMaxContainers)
	{
		targetBag = reader.m_memMaxContainers - 1;
		CPackSender::closeContainer(targetBag);
		waitForOpenContainer(targetBag, false);
	}
	return targetBag;
}

void CModuleUtil::getInstallPath(char* path)
{
	strcpy(path, CInstallPath::getInstallPath().c_str());
}

int CModuleUtil::getTASetting(const char* name)
{
	HKEY hkey              = NULL;
	int value              = 0;
	unsigned long valueLen = 4;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_READ, &hkey))
	{
		RegQueryValueEx(hkey, TEXT(name), NULL, NULL, (unsigned char *)&value, &valueLen);
		RegCloseKey(hkey);
	}
	return value;
}

void CModuleUtil::setTASetting(const char* name, int value)
{
	HKEY hkey              = NULL;
	unsigned long valueLen = 4;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_SET_VALUE, &hkey))
	{
		RegSetValueEx(hkey, TEXT(name), NULL, REG_DWORD, (unsigned char *)&value, valueLen);
		RegCloseKey(hkey);
	}
}

void CModuleUtil::masterDebug(const char* fname, const char* buf1, const char* buf2, const char* buf3, const char* buf4, const char* buf5, const char* buf6)
{
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf, "%s\\%s", installPath, fname);

	//replace old file
	if (strlen(buf1) == 0)
	{
		FILE *f = fopen(pathBuf, "w");
		if (f)
			fclose(f);
		return;
	}
	char dateStr [15];
	char timeStr [15];
	_strdate(dateStr);
	_strtime(timeStr);
	FILE *f = fopen(pathBuf, "a+");
	if (f)
	{
		fprintf(f, "%s\t%s\tTibiaAuto\t%s\t%s\t%s\t%s\t%s\t%s\n", dateStr, timeStr, buf1, buf2, buf3, buf4, buf5, buf6);
		fclose(f);
	}
}
