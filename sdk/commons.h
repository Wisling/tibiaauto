/////////////////////////////////////////////////////////////////////////////
#include "PackSenderProxy.h"
#include "MemReaderProxy.h"
#include "ModuleUtil.h"

int itemOnTopIndex(int x,int y,int z)//Now uses Tibia's own indexing system found in memory to determine this
{
	CMemReaderProxy reader;
	int pos;

	int stackCount=reader.mapGetPointItemsCount(point(x,y,z));
	int immoveableItems=0;//count the number of items Tibia is using for decorations and made immovable
	for (pos=0;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,z),pos);
		CTibiaTile *tile=reader.getTibiaTile(tileId);
		if (tileId!=99 && tile->notMoveable && !tile->ground && !tile->alwaysOnTop && !tile->isContainer)
			immoveableItems++;
	}
	int newCount=stackCount;
	for (pos=0;pos<stackCount;pos++)
	{
		int stackInd=reader.mapGetPointStackIndex(point(x,y,z),pos);
		int tileId = reader.mapGetPointItemId(point(x,y,z),pos);
		CTibiaTile *tile=reader.getTibiaTile(tileId);
		//If a movable tile is found then pretend as if the immoveableItems are not in the stack(they are at the end)
		//If a movable tile is never found, then keep things the way they are
		//Edit: check if it is a container, since recently killed creatures are immovable(10 second rule)
		if (immoveableItems && (tileId==99 || !tile->notMoveable || tile->isContainer)) {
			stackCount-=immoveableItems;
			newCount-=immoveableItems;
			immoveableItems=0;
		}
		//decrease the index we want to find by 1 if we found a creature or an overhanging object
		newCount-=(tileId==99 || tile->moreAlwaysOnTop==3)?1:0;

		//If we are at the index we wanted to get to or are at top of stack, return
		if (stackInd==newCount-1 && tileId!=99 || pos==stackCount-1)
			return pos;
	}
	return -1;
}

int itemOnTopIndex(int x,int y)
{
	return itemOnTopIndex(x,y,0);
}

int isItemOnTop(int x,int y,int *itemArr,int itemArrSize)
{
	CMemReaderProxy reader;
	int topPos=itemOnTopIndex(x,y);
	if (topPos==-1) return 0;

	int tileId = reader.mapGetPointItemId(point(x,y,0),topPos);
	for (int i=0;i<itemArrSize;i++)
	{
		if (tileId==itemArr[i]) return tileId;
	}		
	return 0;
}

int isItemCovered(int x,int y,int *itemArr,int itemArrSize)
{
	CMemReaderProxy reader;

	int topPos=itemOnTopIndex(x,y);
	if (topPos==-1) return 0;

	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	if (topPos>=stackCount) return 0;
	for (int pos=(topPos+1)%stackCount;pos!=topPos;pos=(pos+1)%stackCount)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		for (int i=0;i<itemArrSize;i++)
		{
			if (tileId==itemArr[i]) return tileId;
		}		
	}
	return 0;
}

int isItemOnTop(int x,int y,int itemId)
{
	int itemArr[1]={itemId};
	return isItemOnTop(x,y,itemArr,1);
}

int isItemCovered(int x,int y,int itemId)
{
	int itemArr[1]={itemId};
	return isItemCovered(x,y,itemArr,1);
}

int getItemIndex(int x,int y,int itemInd)
{
	CMemReaderProxy reader;

	int topPos=itemOnTopIndex(x,y);
	if (topPos==-1) return 0;

	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (int pos=0;pos<=stackCount;pos++)
	{
		if (itemInd == reader.mapGetPointItemId(point(x,y,0),pos))		return pos;
	}
	return -1;
}

int itemOnTopCode(int x,int y)
{

	CMemReaderProxy reader;
	int pos=itemOnTopIndex(x,y);
	if (pos!=-1)
	{
		return reader.mapGetPointItemId(point(x,y,0),pos);
	}
	return 0;
}

int itemSeenOnTopIndex(int x,int y, int z=0)
{
	CMemReaderProxy reader;
	int pos;	
	int stackCount=reader.mapGetPointItemsCount(point(x,y,z));
	for (pos=0;pos<stackCount;pos++)
	{
		int stackInd=reader.mapGetPointStackIndex(point(x,y,z),pos);
		if (stackInd==stackCount-1)
			return pos;
	}
	return stackCount-1;
}

int itemSeenOnTopCode(int x,int y)
{
	CMemReaderProxy reader;
	int pos=itemSeenOnTopIndex(x,y);
	if (pos!=-1)
	{
		return reader.mapGetPointItemId(point(x,y,0),pos);
	}
	return 0;
}

int itemOnTopQty(int x,int y)
{
	CMemReaderProxy reader;
	int pos=itemOnTopIndex(x,y);
	if (pos==-1) return 0;

	int qty=reader.mapGetPointItemExtraInfo(point(x,y,0),pos,1);
	return qty?qty:1;
}

int findNextClosedContainer(int afterCont=-1)
{

	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTibiaContainer *container;
	CMemConstData memConstData = reader.getMemConstData();
	int targetBag;
	for (targetBag=afterCont+1;targetBag<memConstData.m_memMaxContainers;targetBag++){
		container = reader.readContainer(targetBag);
		if (!container->flagOnOff){
			delete container;
			break;
		}
		delete container;
	}
	
	if (targetBag == memConstData.m_memMaxContainers){
		targetBag=memConstData.m_memMaxContainers-1;
		sender.closeContainer(targetBag);
		CModuleUtil::waitForOpenContainer(targetBag,true);

	}
	return targetBag;
}
