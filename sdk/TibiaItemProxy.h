#if !defined(AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
#define AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaTree;
class CTibiaList;

class CTibiaItemProxy
{
public:
	void cleanup();
	char * getItemName(int objectId);
	int getItemId(char *name);
	int getItemIndex(int objectId);
	int getFoodIndex(int objectId);
	int getLootItemIndex(int objectId);
	int getItemIdAtIndex(int ind);
	int getFoodIdAtIndex(int ind);
	int getLootItemIdAtIndex(int ind);
	char * getItemNameAtIndex(int ind);
	char * getFoodNameAtIndex(int ind);
	char * getLootItemNameAtIndex(int ind);
	int getFoodTimeAtIndex(int ind);
	CUIntArray* getItemIdArrayPtr();
	CUIntArray* getFoodIdArrayPtr();
	CUIntArray* getLootItemIdArrayPtr();
	void setItemAsLooted(int objectId);
	void clearLootItems();

	void addItem(char *name, int objectId);
	void addFood(char *name, int objectId, int extraInfo);
	void addLootItem(char *name, int objectId);
	void removeItem(int ind);
	void removeFood(int ind);
	void removeLootItem(int ind);
	void clearFoodList();
	int getItemCount();
	int getFoodCount();
	int getLootItemCount();
	int getValueForConst(char *code);
	CTibiaTree * getItemsTree();
	void setItemsTree(CTibiaTree * tibiaTree);
	void refreshItemLists();
	void saveItemLists();

	CTibiaItemProxy();
	virtual ~CTibiaItemProxy();
	
	/*Deprecated Function Start*/
	int getIndex(int objectId, int type);
	char *getName(int objectId);
	void setItemName(int index, char *name);
	//Deprecated Functions Section
	void setFoodName(int index, char *name);
	void setLootItemName(int index, char *name);
	int getObjectId(char *name);
	void setItemId(int index, int objectId);
	int getFoodId(char *name);
	//Deprecated Functions Section
	void setFoodId(int index, int objectId);
	int getLootItemId(char *name);
	void setLootItemId(int index, int objectId);
	void addItemItem(char *name, int objectId);
	//Deprecated Functions Section
	void addItemFood(char *name, int objectId, int time);
	void addItemLoot(char *name, int objectId);
	int getCorpseIdByCreatureName(char *name);
	void setExtraInfo(int index, int info, int type);
	int getExtraInfo(int index, int type);
	//Deprecated Functions Section
	CUIntArray *getItemsFoodArray();

	char *getItemsItems(int nr);
	int getItemsItemsId(int nr);
	int getItemsItemsCount();
	char *getItemsFood(int nr);
	int getItemsFoodId(int nr);
	int getItemsFoodCount();
	char *getItemsCorpses(int nr);
	//Deprecated Functions Section
	int getItemsCorpsesId(int nr);
	int getItemsCorpsesCount();
	char *getItemsLooted(int nr);
	int getItemsLootedId(int nr);
	int getItemsLootedCount();
	//Deprecated Functions Section
	void setItemsItemsCount(int newCount);
	void setItemsFoodCount(int newCount);
	void setItemsLootCount(int newCount);
	/*Deprecated Function End*/

private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
