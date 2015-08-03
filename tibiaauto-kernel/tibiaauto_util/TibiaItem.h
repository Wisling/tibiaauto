#pragma once
#include "tibiaauto_util.h"

class CTibiaList;
class CTibiaTree;

class TIBIAAUTOUTIL_API CTibiaItem
{
public:
	static void cleanup();
	static char * getItemName(int objectId);
	static int getItemId(char *name);
	static int getItemIndex(int objectId);
	static int getFoodIndex(int objectId);
	static int getLootItemIndex(int objectId);
	static int getItemIdAtIndex(int ind);
	static int getFoodIdAtIndex(int ind);
	static int getLootItemIdAtIndex(int ind);
	static char * getItemNameAtIndex(int ind);
	static char * getFoodNameAtIndex(int ind);
	static char * getLootItemNameAtIndex(int ind);
	static int getFoodTimeAtIndex(int ind);
	static CUIntArray* getItemIdArrayPtr();
	static CUIntArray* getFoodIdArrayPtr();
	static CUIntArray* getLootItemIdArrayPtr();
	static CUIntArray* getTypedItemIdArrayPtr(int ind);
	static void setItemAsLooted(int objectId);
	static void clearLootItems();
	//static void setFoodNameAtIndex(int ind, int objectId);
	//static void setFoodTimeAtIndex(int ind, int objectId);
	//static void setFoodIdAtIndex(int ind, char *name);
	static int getTypedItemCount();
	static int getTypedItemId(char *name);
	static int getTypedItemIndex(int objectId);
	static char* getTypedItemNameAtIndex(int ind);
	static int getTypedItemIdAtIndex(int ind);
	static char * getTypedItemName(int objectId);
	static void fillTypedItemIdArray(int ind);
	static void addTypedItem(char *name, int objectId, int type);
	static int getItemTypeAtIndex(int ind);
	static void removeTypedItem(int ind);

	static void addItem(char *name, int objectId);
	static void addFood(char *name, int objectId, int extraInfo);
	static void addLootItem(char *name, int objectId);
	static void removeItem(int ind);
	static void removeFood(int ind);
	static void removeLootItem(int ind);
	static void clearFoodList();
	static int getItemCount();
	static int getFoodCount();
	static int getLootItemCount();
	static int getValueForConst(char *code);
	static CTibiaTree * getItemsTree();
	static void setItemsTree(CTibiaTree * tibiaTree);
	static void refreshItemLists();
	static void saveItemLists();

	CTibiaItem();

	int objectId;
	int quantity;
	int pos;

private:
	static CTibiaTree* itemTree;
	static CTibiaList foodList;
	static CTibiaList lootList;
	static CTibiaList itemList;
	static CTibiaList typedList;
	static CTibiaList constCodeList;
	static int xmlInitialised;


public:
	static int itemListsFresh;
};

