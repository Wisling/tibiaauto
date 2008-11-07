// TibiaItem.h: interface for the CTibiaItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAITEM_H__65F1AF50_58EE_4C63_8E79_E5BC61BD74C9__INCLUDED_)
#define AFX_TIBIAITEM_H__65F1AF50_58EE_4C63_8E79_E5BC61BD74C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ITEMS 200
#define MAX_ITEM_LEN 60

class CTibiaItem  
{
public:
	static int getIndex(int objectId, int type);
	static char * getName(int objectId);
	static void setName(int index, char *name, int type);
	static void addName(char *name, int type);
	static int getObjectId(char *name);
	static int getFoodId(char *name);
	static int getLootItemId(char *name);
	static void setObjectId(int index, int objectId, int type);
	static void addObjectId(int objectId, int type);
	static void addExtraInfo(int extraInfo, int type);
	static void addItem(char *name, int objectId, int extraInfo, int type);
	static int getValueForConst(char *code);
	static void refreshItemLists();
	static void saveItemLists();
	static void setCount(int type, int newCount);
	static void setExtraInfo(int index, int info, int type);
	CTibiaItem();

	int objectId;
	int quantity;
	int pos;

public:
	static int getCorpseIdByCreatureName(char *name);
	static CUIntArray * getItemsFood();
private:
	static CUIntArray *foodList;	
	static int xmlInitialised;
	

public:
	static int itemListsFresh;
	static char itemsItems[MAX_ITEMS][MAX_ITEM_LEN];
	static int itemsItemsId[MAX_ITEMS];
	static int itemsItemsExtra[MAX_ITEMS];
	static int itemsItemsCount;
	static char itemsFood[MAX_ITEMS][MAX_ITEM_LEN];
	static int itemsFoodId[MAX_ITEMS];
	static int itemsFoodTime[MAX_ITEMS];
	static int itemsFoodCount;
	static char itemsCorpses[MAX_ITEMS][MAX_ITEM_LEN];
	static int itemsCorpsesId[MAX_ITEMS];
	static int itemsCorpsesCount;
	static char itemsLooted[MAX_ITEMS][MAX_ITEM_LEN];
	static int itemsLootedId[MAX_ITEMS];
	static int itemsLootedExtra[MAX_ITEMS];
	static int itemsLootedCount;
	static char constsCode[MAX_ITEMS][MAX_ITEM_LEN];
	static int constsValue[MAX_ITEMS];
	static int constsCount;
};

#endif // !defined(AFX_TIBIAITEM_H__65F1AF50_58EE_4C63_8E79_E5BC61BD74C9__INCLUDED_)
