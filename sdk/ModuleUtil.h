// ModuleUtil.h: interface for the CModuleUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODULEUTIL_H__B8B0187F_3022_4C4E_8E60_C3593CDB21D5__INCLUDED_)
#define AFX_MODULEUTIL_H__B8B0187F_3022_4C4E_8E60_C3593CDB21D5__INCLUDED_

#include "TibiaItem.h"
#include "Queue.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CModuleUtil  
{
public:
	static void waitForItemsInsideChange(int contNr, int origItemsCount);
	static CTibiaItem * lookupItem(int containerNr, CUIntArray *itemsAccepted);
	static CTibiaItem * lookupItem(int containerNr, CUIntArray *itemsAccepted,int qty);
	static void findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int path[15]);
	static void findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int path[15],int useDiagonal);
	static void executeWalk(int path[15]);
	static void findPathOnMapProcessPoint(CQueue *queue,int prevX,int prevY, int prevZ, int newX, int newY, int newZ);
	static void lootItemFromContainer(int conTNr, CUIntArray *acceptedItems);
	static void eatItemFromContainer(int contNr);
	static int waitForOpenContainer(int contNr,int open);
	static void sleepWithStop(int ms,int *stopFlag);
	static char *wc2c(const unsigned short *src);
	static void waitForCreatureDisappear(int x,int y, int tibiaId);
	static int calcLootChecksum(int tm, int killNr, int nameLen, int itemNr, int objectId, int qty, int lootInBags);
	static struct point findNearestDepot(int startX, int startY, int startZ);
	static struct point findNearestDepot(int startX, int startY, int startZ, int useDiagonal);
	static void prepareProhPointList();


	CModuleUtil();
	virtual ~CModuleUtil();

};

#endif // !defined(AFX_MODULEUTIL_H__B8B0187F_3022_4C4E_8E60_C3593CDB21D5__INCLUDED_)
