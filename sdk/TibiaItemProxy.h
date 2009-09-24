#if !defined(AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
#define AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaItemProxy
{
public:
	int getIndex(int objectId, int type);
	char *getName(int objectId);
	void setItemName(int index, char *name);
	void setFoodName(int index, char *name);
	void setLootItemName(int index, char *name);
	int getObjectId(char *name);
	void setItemId(int index, int objectId);
	int getFoodId(char *name);
	void setFoodId(int index, int objectId);
	int getLootItemId(char *name);
	void setLootItemId(int index, int objectId);
	int getValueForConst(char *code);
	void refreshItemLists();
	void saveItemLists();
	void addItemItem(char *name, int objectId);
	void addItemFood(char *name, int objectId, int time);
	void addItemLoot(char *name, int objectId);
	int getCorpseIdByCreatureName(char *name);
	void setExtraInfo(int index, int info, int type);
	int getExtraInfo(int index, int type);
	CUIntArray *getItemsFoodArray();

	char *getItemsItems(int nr);
	int getItemsItemsId(int nr);
	int getItemsItemsCount();
	char *getItemsFood(int nr);
	int getItemsFoodId(int nr);
	int getItemsFoodCount();
	char *getItemsCorpses(int nr);
	int getItemsCorpsesId(int nr);
	int getItemsCorpsesCount();
	char *getItemsLooted(int nr);
	int getItemsLootedId(int nr);
	int getItemsLootedCount();
	void setItemsItemsCount(int newCount);
	void setItemsFoodCount(int newCount);
	void setItemsLootCount(int newCount);
	void cleanup();

	CTibiaItemProxy();
	virtual ~CTibiaItemProxy();
	
private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
