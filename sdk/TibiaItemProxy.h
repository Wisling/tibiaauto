#if !defined(AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
#define AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaItemProxy
{
public:
	char * getName(int objectId);
	int getObjectId(char *name);
	int getValueForConst(char *code);
	void refreshItemLists();
	int getCorpseIdByCreatureName(char *name);
	CUIntArray * getItemsFoodArray();

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


	CTibiaItemProxy();
	virtual ~CTibiaItemProxy();
	
private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_TIBIAITEMPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
