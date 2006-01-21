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
	static char * getName(int objectId);
	static void refreshItemLists();
	CTibiaItem();

	int objectId;
	int quantity;
	int pos;

public:
	static int getCorpseIdByCreatureName(char *name);
	static CUIntArray * getItemsFood();
	static int m_itemTypeBlankRune;
	static int m_itemTypeFish;
	static int m_itemTypeFishingRod;
	static int m_itemTypeWaterWithFish;
	static int m_itemTypeWaterNoFish;
	static int m_itemTypeRuneUH;
	static int m_itemTypeRuneIH;
	static int m_itemTypeHam;
	static int m_itemTypeMeat;
	static int m_itemTypeGP;
	static int m_itemTypeRuneHmm;
	static int m_itemTypeRuneExplo;
	static int m_itemTypeRuneGfb;
	static int m_itemTypeRuneSd;
	static int m_itemTypeFluid;
	static int m_itemTypeWorms;
	static int m_itemTypeArrow;
	static int m_itemTypeBolt;
	static int m_itemTypeSpear;
	static int m_itemTypeBagBrown;
	static int m_itemTypeRope;
	static int m_itemTypeLadder;
	static int m_itemTypeRopespot;
	static int m_itemTypeSmallStone;
	static int m_itemTypeWaterWithFishStart;
	static int m_itemTypeWaterWithFishEnd;
private:
	static CUIntArray *foodList;	
	static int xmlInitialised;
	

public:
	static int itemListsFresh;
	static char itemsItems[MAX_ITEMS][MAX_ITEM_LEN];
	static int itemsItemsId[MAX_ITEMS];
	static int itemsItemsCount;
	static char itemsFood[MAX_ITEMS][MAX_ITEM_LEN];
	static int itemsFoodId[MAX_ITEMS];
	static int itemsFoodCount;
	static char itemsCorpses[MAX_ITEMS][MAX_ITEM_LEN];
	static int itemsCorpsesId[MAX_ITEMS];
	static int itemsCorpsesCount;
	static char itemsLooted[MAX_ITEMS][MAX_ITEM_LEN];
	static int itemsLootedId[MAX_ITEMS];
	static int itemsLootedCount;
};

#endif // !defined(AFX_TIBIAITEM_H__65F1AF50_58EE_4C63_8E79_E5BC61BD74C9__INCLUDED_)
