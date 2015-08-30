// tibiaauto_util.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "tibiaauto_utilfun.h"

#include "MemReader.h"
#include "PackSender.h"
#include "MemConstData.h"
#include "TibiaMapPoint.h"
#include "TibiaMiniMapPoint.h"
#include "TibiaMap.h"
#include "TAMiniMap.h"
#include "TibiaItem.h"
#include "IPCBackPipe.h"
#include "TibiaTile.h"
#include "tileReader.h"
#include "CreaturesReader.h"
#include "VariableStore.h"
#include "../md5class.h"


#include "MemUtil.h"
#include "TibiaContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = "C:/temp/testfdkls.txt";
#endif // ifdef _DEBUG

extern HANDLE hPipe;

void setProcessId(int processId)
{
	CMemUtil::setGlobalProcessId(processId);
}

int getProcessId()
{
	return CMemUtil::getGlobalProcessId();
}

void getBaseAddr()
{
	CMemUtil::getGlobalBaseAddr();
}

CMemConstData getMemConstData()
{
	return CMemConstData();
}

void memReadSetMemIntValue(int address, int value, int addBaseAddr = 1)
{
	CMemUtil::SetMemIntValue(address, value, addBaseAddr);
}

int memReadGetMemIntValue(int address, int addBaseAddr = 1)
{
	return CMemUtil::GetMemIntValue(address, addBaseAddr);
}

void memReadSetMemRange(DWORD memAddressStart, DWORD memAddressEnd, char *data, int addBaseAddr = 1)
{
	CMemUtil::SetMemRange(memAddressStart, memAddressEnd, data, addBaseAddr);
}

void memReadGetMemRange(DWORD memAddressStart, DWORD memAddressEnd, char *data, int addBaseAddr = 1)
{
	CMemUtil::GetMemRange(memAddressStart, memAddressEnd, data, addBaseAddr);
}

void tibiaItemfillTypedItemIdArray(int ind)
{
	CTibiaItem::fillTypedItemIdArray(ind);
}

char * tibiaItemGetItemName(int objectId)
{
	return CTibiaItem::getItemName(objectId);
}

char * tibiaItemGetTypedItemName(int objectId)
{
	return CTibiaItem::getTypedItemName(objectId);
}

int tibiaItemGetItemId(char *name)
{
	return CTibiaItem::getItemId(name);
}

int tibiaItemGetTypedItemId(char *name)
{
	return CTibiaItem::getTypedItemId(name);
}

int tibiaItemGetItemIndex(int objectId)
{
	return CTibiaItem::getItemIndex(objectId);
}

int tibiaItemGetTypedItemIndex(int objectId)
{
	return CTibiaItem::getTypedItemIndex(objectId);
}

int tibiaItemGetFoodIndex(int objectId)
{
	return CTibiaItem::getFoodIndex(objectId);
}

int tibiaItemGetLootItemIndex(int objectId)
{
	return CTibiaItem::getLootItemIndex(objectId);
}

int tibiaItemGetItemIdAtIndex(int ind)
{
	return CTibiaItem::getItemIdAtIndex(ind);
}

int tibiaItemGetItemTypeAtIndex(int ind)
{
	return CTibiaItem::getItemTypeAtIndex(ind);
}

int tibiaItemGetTypedItemIdAtIndex(int ind)
{
	return CTibiaItem::getTypedItemIdAtIndex(ind);
}

int tibiaItemGetFoodIdAtIndex(int ind)
{
	return CTibiaItem::getFoodIdAtIndex(ind);
}

int tibiaItemGetLootItemIdAtIndex(int ind)
{
	return CTibiaItem::getLootItemIdAtIndex(ind);
}

char* tibiaItemGetItemNameAtIndex(int ind)
{
	return CTibiaItem::getItemNameAtIndex(ind);
}

char* tibiaItemGetTypedItemNameAtIndex(int ind)
{
	return CTibiaItem::getTypedItemNameAtIndex(ind);
}

char* tibiaItemGetFoodNameAtIndex(int ind)
{
	return CTibiaItem::getFoodNameAtIndex(ind);
}

char* tibiaItemGetLootItemNameAtIndex(int ind)
{
	return CTibiaItem::getLootItemNameAtIndex(ind);
}

int tibiaItemGetFoodTimeAtIndex(int ind)
{
	return CTibiaItem::getFoodTimeAtIndex(ind);
}

CUIntArray* tibiaItemGetItemIdArrayPtr()
{
	return CTibiaItem::getItemIdArrayPtr();
}

CUIntArray* tibiaItemGetTypedItemIdArrayPtr(int ind)
{
	return CTibiaItem::getTypedItemIdArrayPtr(ind);
}

CUIntArray* tibiaItemGetFoodIdArrayPtr()
{
	return CTibiaItem::getFoodIdArrayPtr();
}

CUIntArray* tibiaItemGetLootItemIdArrayPtr()
{
	return CTibiaItem::getLootItemIdArrayPtr();
}

void tibiaItemSetItemAsLooted(int objectId)
{
	CTibiaItem::setItemAsLooted(objectId);
}

void tibiaItemClearLootItems()
{
	CTibiaItem::clearLootItems();
}

void tibiaItemAddItem(char *name, int objectId)
{
	CTibiaItem::addItem(name, objectId);
}

void tibiaItemAddTypedItem(char *name, int objectId, int type)
{
	CTibiaItem::addTypedItem(name, objectId, type);
}

void tibiaItemAddFood(char *name, int objectId, int extraInfo)
{
	CTibiaItem::addFood(name, objectId, extraInfo);
}

void tibiaItemAddLootItem(char *name, int objectId)
{
	CTibiaItem::addLootItem(name, objectId);
}

void tibiaItemRemoveItem(int ind)
{
	CTibiaItem::removeItem(ind);
}

void tibiaItemRemoveTypedItem(int ind)
{
	CTibiaItem::removeTypedItem(ind);
}

void tibiaItemRemoveFood(int ind)
{
	CTibiaItem::removeFood(ind);
}

void tibiaItemRemoveLootItem(int ind)
{
	CTibiaItem::removeLootItem(ind);
}

void tibiaItemClearFoodList()
{
	CTibiaItem::clearFoodList();
}

int tibiaItemGetItemCount()
{
	return CTibiaItem::getItemCount();
}

int tibiaItemGetTypedItemCount()
{
	return CTibiaItem::getTypedItemCount();
}

int tibiaItemGetFoodCount()
{
	return CTibiaItem::getFoodCount();
}

int tibiaItemGetLootItemCount()
{
	return CTibiaItem::getLootItemCount();
}

int tibiaItemGetValueForConst(char *code)
{
	return CTibiaItem::getValueForConst(code);
}

CTibiaTree * tibiaItemGetItemsTree()
{
	return CTibiaItem::getItemsTree();
}

void tibiaItemSetItemsTree(CTibiaTree * tibiaTree)
{
	CTibiaItem::setItemsTree(tibiaTree);
}

void tibiaItemRefreshItemLists()
{
	CTibiaItem::itemListsFresh = 0;
	CTibiaItem::refreshItemLists();
}

void tibiaItemSaveItemLists()
{
	CTibiaItem::saveItemLists();
}

void tibiaItemCleanup()
{
	CTibiaItem::cleanup();
}

int getKernelMainVersion()
{
	return 3;
}

int getKernelPatchVersion()
{
	return 0;
}

void variableStoreSetVariable(char *name, char *value)
{
	CVariableStore::setVariable(name, value);
}

const char *variableStoreGetVariable(char *name)
{
	return CVariableStore::getVariable(name);
}

int crFindCreatureStatForLocationTibiaId(int x, int y, int z, int pos)
{
	CCreaturesReader cReader;
	return cReader.findCreatureStatForLocationTibiaId(x, y, z, pos);
}

int crFindCreatureStatForLocationCount(int x, int y, int z)
{
	CCreaturesReader cReader;
	return cReader.findCreatureStatForLocationCount(x, y, z);
}

char *crFindCreatureStatForLocationName(int x, int y, int z, int pos)
{
	CCreaturesReader cReader;
	return cReader.findCreatureStatForLocationName(x, y, z, pos);
}

char ** crFindCreatureStatInArea(int x, int y, int z, int rangeXY, int rangeZ)
{
	CCreaturesReader cReader;
	return cReader.findCreatureStatInArea(x, y, z, rangeXY, rangeZ);
}

//////////////////////////////////////////
// Blow lie older removed functions that make use of their newer replacement functions

/* Deprecated function START*/
int tibiaItemGetIndex(int objectId, int type)
{
	switch (type)
	{
	case 1:
		return tibiaItemGetItemIndex(objectId);
	case 2:
		return tibiaItemGetFoodIndex(objectId);
	case 3:
		return tibiaItemGetLootItemIndex(objectId);
	}
	return -1;
}

char * tibiaItemGetName(int objectId)
{
	return tibiaItemGetItemName(objectId);
}

int tibiaItemGetObjectId(char *name)
{
	return tibiaItemGetItemId(name);
}

int tibiaItemGetFoodId(char *name)
{
	int size = tibiaItemGetFoodCount();
	for (int i = 0; i < size; i++)
	{
		if (!strcmp(tibiaItemGetFoodNameAtIndex(i), name))
			return tibiaItemGetFoodIdAtIndex(i);
	}
	return 0;
}

//Deprecated Functions Section
int tibiaItemGetLootItemId(char *name)
{
	int size = tibiaItemGetLootItemCount();
	for (int i = 0; i < size; i++)
	{
		if (!strcmp(tibiaItemGetLootItemNameAtIndex(i), name))
			return tibiaItemGetLootItemIdAtIndex(i);
	}
	return 0;
}

void tibiaItemSetItemName(int index, char *name)
{
	tibiaItemAddItem(name, tibiaItemGetItemIdAtIndex(index));
	tibiaItemRemoveItem(index);
}

int tibiaItemGetExtraInfo(int index, int type)
{
	if (type == 2)
		return tibiaItemGetFoodTimeAtIndex(index);
	return 0;
}

void tibiaItemAddLoot (char *name, int objectId)
{
	tibiaItemAddLootItem(name, objectId);
}

//Deprecated Functions Section
void tibiaItemSetFoodName(int index, char *name)
{
	tibiaItemAddFood(name, tibiaItemGetFoodIdAtIndex(index), tibiaItemGetFoodTimeAtIndex(index));
	tibiaItemRemoveFood(index);
}

void tibiaItemSetLootName(int index, char *name)
{
	tibiaItemAddLootItem(name, tibiaItemGetLootItemIdAtIndex(index));
	tibiaItemRemoveLootItem(index);
}

void tibiaItemSetExtraInfo(int index, int info, int type)
{
	if (type == 2)
	{
		char* name  = tibiaItemGetFoodNameAtIndex(index);
		char* name2 = (char*)malloc(strlen(name) + 1);
		tibiaItemRemoveFood(index);
		tibiaItemAddFood(name2, tibiaItemGetFoodIdAtIndex(index), info);
		free(name2);
	}
}

void tibiaItemSetItemObjectId(int index, int objectId)
{
	char* name  = tibiaItemGetItemNameAtIndex(index);
	char* name2 = (char*)malloc(strlen(name) + 1);
	strcpy(name2, name);
	tibiaItemRemoveItem(index);
	tibiaItemAddItem(name2, objectId);
	free(name2);
}

//Deprecated Functions Section
void tibiaItemSetFoodObjectId(int index, int objectId)
{
	char* name  = tibiaItemGetFoodNameAtIndex(index);
	char* name2 = (char*)malloc(strlen(name) + 1);
	strcpy(name2, name);
	tibiaItemRemoveFood(index);
	tibiaItemAddFood(name2, objectId, tibiaItemGetFoodTimeAtIndex(index));
	free(name2);
}

void tibiaItemSetLootObjectId(int index, int objectId)
{
	char* name  = tibiaItemGetLootItemNameAtIndex(index);
	char* name2 = (char*)malloc(strlen(name) + 1);
	strcpy(name2, name);
	tibiaItemRemoveLootItem(index);
	tibiaItemAddLootItem(name2, objectId);
	free(name2);
}

int tibiaItemGetCorpseIdByCreatureName(char *name)
{
	return 0;
}

CUIntArray * tibiaItemGetItemsFoodArray()
{
	return tibiaItemGetFoodIdArrayPtr();
}

char *tibiaItemGetItemsItems(int nr)
{
	return tibiaItemGetItemNameAtIndex(nr);
}

int tibiaItemGetItemsItemsId(int nr)
{
	return tibiaItemGetItemIdAtIndex(nr);
}

//Deprecated Functions Section
int tibiaItemGetItemsItemsCount()
{
	return tibiaItemGetItemCount();
}

void tibiaItemSetItemsItemsCount(int newCount)
{
}

void tibiaItemSetItemsFoodCount(int newCount)
{
}

void tibiaItemSetItemsLootCount(int newCount)
{
}

char *tibiaItemGetItemsFood(int nr)
{
	return tibiaItemGetFoodNameAtIndex(nr);
}

int tibiaItemGetItemsFoodId(int nr)
{
	return tibiaItemGetFoodIdAtIndex(nr);
}

int tibiaItemGetItemsFoodCount()
{
	return tibiaItemGetFoodCount();
}

char *tibiaItemGetItemsCorpses(int nr)
{
	return "not implemented";
}

//Deprecated Functions Section
int tibiaItemGetItemsCorpsesId(int nr)
{
	return 0;
}

int tibiaItemGetItemsCorpsesCount()
{
	return 0;
}

char *tibiaItemGetItemsLooted(int nr)
{
	return tibiaItemGetLootItemNameAtIndex(nr);
}

int tibiaItemGetItemsLootedId(int nr)
{
	return tibiaItemGetLootItemIdAtIndex(nr);
}

int tibiaItemGetItemsLootedCount()
{
	return tibiaItemGetLootItemCount();
}

/* Deprecated Function END */
