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


#include "stdafx.h"
#include "TibiaItemProxy.h"

class CTibiaTree;

//////////////////////////////////////////////////////////////////////

HMODULE CTibiaItemProxy::dllModule = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaItemProxy::CTibiaItemProxy()
{
	// load module
	if (dllModule == NULL)
	{
		char installPath[1024];
		unsigned long installPathLen = 1023;
		installPath[0] = '\0';
		HKEY hkey = NULL;
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_READ, &hkey))
		{
			RegQueryValueEx(hkey, TEXT("Install_Dir"), NULL, NULL, (unsigned char *)installPath, &installPathLen );
			RegCloseKey(hkey);
		}
		if (!strlen(installPath))
		{
			::MessageBox(0, "ERROR! Unable to read TA install directory! Please reinstall!", "ERROR", 0);
			PostQuitMessage(-1);
			return;
		}

		char pathBuf[2048];
		sprintf(pathBuf, "%s\\mods\\tibiaauto_util.dll", installPath);

		dllModule = LoadLibrary(pathBuf);
	}
}

CTibiaItemProxy::~CTibiaItemProxy()
{
}
void CTibiaItemProxy::fillTypedItemIdArray(int ind) {
	typedef void (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemfillTypedItemIdArray");
		if (fun)
			fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
}

char * CTibiaItemProxy::getItemName(int objectId)
{
	typedef char * (*Proto_fun)(int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemName");
		if (fun)
			return fun(objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return "not implemented";
}

char * CTibiaItemProxy::getTypedItemName(int objectId)
{
	typedef char * (*Proto_fun)(int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetTypedItemName");
		if (fun)
			return fun(objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return "not implemented";
}

int CTibiaItemProxy::getItemId(char* name)
{
	typedef int (*Proto_fun)(char* name);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemId");
		if (fun)
			return fun(name);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getTypedItemId(char* name)
{
	typedef int (*Proto_fun)(char* name);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetTypedItemId");
		if (fun)
			return fun(name);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getItemIndex(int objectId)
{
	typedef int (*Proto_fun)(int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemIndex");
		if (fun)
			return fun(objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getTypedItemIndex(int objectId)
{
	typedef int (*Proto_fun)(int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetTypedItemIndex");
		if (fun)
			return fun(objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getFoodIndex(int objectId)
{
	typedef int (*Proto_fun)(int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetFoodIndex");
		if (fun)
			return fun(objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}
int CTibiaItemProxy::getLootItemIndex(int objectId)
{
	typedef int (*Proto_fun)(int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetLootItemIndex");
		if (fun)
			return fun(objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getItemIdAtIndex(int ind)
{
	typedef int (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemIdAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getItemTypeAtIndex(int ind)
{
	typedef int (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemTypeAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getTypedItemIdAtIndex(int ind)
{
	typedef int (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetTypedItemIdAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getFoodIdAtIndex(int ind)
{
	typedef int (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetFoodIdAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}
int CTibiaItemProxy::getLootItemIdAtIndex(int ind)
{
	typedef int (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetLootItemIdAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

char* CTibiaItemProxy::getItemNameAtIndex(int ind)
{
	typedef char* (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemNameAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return "not implemented";
}

char* CTibiaItemProxy::getTypedItemNameAtIndex(int ind)
{
	typedef char* (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetTypedItemNameAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return "not implemented";
}

char* CTibiaItemProxy::getFoodNameAtIndex(int ind)
{
	typedef char* (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetFoodNameAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return "not implemented";
}
char* CTibiaItemProxy::getLootItemNameAtIndex(int ind)
{
	typedef char* (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetLootItemNameAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return "not implemented";
}

int CTibiaItemProxy::getFoodTimeAtIndex(int ind)
{
	typedef int (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetFoodTimeAtIndex");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

CUIntArray* CTibiaItemProxy::getItemIdArrayPtr()
{
	typedef CUIntArray* (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemIdArrayPtr");
		if (fun)
			return fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
	return NULL;
}

CUIntArray* CTibiaItemProxy::getTypedItemIdArrayPtr(int ind)
{
	typedef CUIntArray* (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetTypedItemIdArrayPtr");
		if (fun)
			return fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return NULL;
}

CUIntArray* CTibiaItemProxy::getFoodIdArrayPtr()
{
	typedef CUIntArray* (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetFoodIdArrayPtr");
		if (fun)
			return fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
	return NULL;
}
CUIntArray* CTibiaItemProxy::getLootItemIdArrayPtr()
{
	typedef CUIntArray* (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetLootItemIdArrayPtr");
		if (fun)
			return fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
	return NULL;
}
void CTibiaItemProxy::setItemAsLooted(int objectId) {
	typedef void (*Proto_fun)(int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemSetItemAsLooted");
		if (fun)
			fun(objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
}
void CTibiaItemProxy::clearLootItems() {
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemClearLootItems");
		if (fun)
			fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
}

void CTibiaItemProxy::addItem(char *name, int objectId) {
	typedef void (*Proto_fun)(char *name, int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemAddItem");
		if (fun)
			fun(name, objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
}

void CTibiaItemProxy::addTypedItem(char *name, int objectId, int type) {
	typedef void (*Proto_fun)(char *name, int objectId, int type);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemAddTypedItem");
		if (fun)
			fun(name, objectId, type);
		else
			AfxMessageBox("Error: function not loaded");
	}
}

void CTibiaItemProxy::addFood(char *name, int objectId, int time) {
	typedef void (*Proto_fun)(char *name, int objectId, int time);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemAddFood");
		if (fun)
			fun(name, objectId, time);
		else
			AfxMessageBox("Error: function not loaded");
	}
}
void CTibiaItemProxy::addLootItem(char *name, int objectId) {
	typedef void (*Proto_fun)(char *name, int objectId);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemAddLootItem");
		if (fun)
			fun(name, objectId);
		else
			AfxMessageBox("Error: function not loaded");
	}
}

void CTibiaItemProxy::removeItem(int ind) {
	typedef void (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemRemoveItem");
		if (fun)
			fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
}

void CTibiaItemProxy::removeTypedItem(int ind) {
	typedef void (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemRemoveTypedItem");
		if (fun)
			fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
}

void CTibiaItemProxy::removeFood(int ind) {
	typedef void (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemRemoveFood");
		if (fun)
			fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
}
void CTibiaItemProxy::removeLootItem(int ind) {
	typedef void (*Proto_fun)(int ind);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemRemoveLootItem");
		if (fun)
			fun(ind);
		else
			AfxMessageBox("Error: function not loaded");
	}
}
void CTibiaItemProxy::clearFoodList() {
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemClearFoodList");
		if (fun)
			fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
}
CTibiaTree * CTibiaItemProxy::getItemsTree()
{
	typedef CTibiaTree * (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemsTree");
		if (fun)
			return fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
	return NULL;
}

void CTibiaItemProxy::setItemsTree(CTibiaTree* tibiaTree)
{
	typedef void (*Proto_fun)(CTibiaTree* tibiaTree);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemSetItemsTree");
		if (fun)
			fun(tibiaTree);
		else
			AfxMessageBox("Error: function not loaded");
	}
}

int CTibiaItemProxy::getItemCount()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetItemCount");
		if (fun)
			return fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getTypedItemCount()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetTypedItemCount");
		if (fun)
			return fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getFoodCount()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetFoodCount");
		if (fun)
			return fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}
int CTibiaItemProxy::getLootItemCount()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetLootItemCount");
		if (fun)
			return fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

int CTibiaItemProxy::getValueForConst(char *code)
{
	typedef int (*Proto_fun)(char *code);
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemGetValueForConst");
		if (fun)
			return fun(code);
		else
			AfxMessageBox("Error: function not loaded");
	}
	return 0;
}

void CTibiaItemProxy::refreshItemLists()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemRefreshItemLists");
		if (fun)
			fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
}
void CTibiaItemProxy::saveItemLists() {
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemSaveItemLists");
		if (fun)
			fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
}

void CTibiaItemProxy::cleanup() {
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllModule, "tibiaItemCleanup");
		if (fun)
			fun();
		else
			AfxMessageBox("Error: function not loaded");
	}
}

/*Deprecated Function Start

   int CTibiaItemProxy::getIndex(int objectId, int type) {
        switch (type){
        case 1:
                return getItemIndex(objectId);
        case 2:
                return getFoodIndex(objectId);
        case 3:
                return getLootItemIndex(objectId);
        }
        return -1;
   }
   //Deprecated Functions Section
   char * CTibiaItemProxy::getName(int objectId)
   {
        return getItemName(objectId);
   }
   void CTibiaItemProxy::setItemName(int index, char *name) {
        addItem(name,getItemIdAtIndex(index));
        removeItem(index);
   }
   void CTibiaItemProxy::setExtraInfo(int index, int info, int type) {
        if (type==2){
                char* name=getFoodNameAtIndex(index);
                char* name2=(char*)malloc(strlen(name)+1);
                removeFood(index);
                addFood(name2,getFoodIdAtIndex(index),info);
                free(name2);
        }
   }
   //Deprecated Functions Section
   void CTibiaItemProxy::setFoodName(int index, char *name) {
        addFood(name,getFoodIdAtIndex(index),getFoodTimeAtIndex(index));
        removeFood(index);
   }
   void CTibiaItemProxy::setLootItemName(int index, char *name) {
        addItem(name,getItemIdAtIndex(index));
        removeItem(index);
   }
   //Deprecated Functions Section
   void CTibiaItemProxy::addItemItem(char *name, int objectId) {
        addItem(name,objectId);
   }
   void CTibiaItemProxy::addItemFood(char *name, int objectId, int time) {
        addFood(name,objectId,time);
   }
   //Deprecated Functions Section
   void CTibiaItemProxy::addItemLoot(char *name, int objectId) {
        addLootItem(name,objectId);
   }
   int CTibiaItemProxy::getCorpseIdByCreatureName(char *name)
   {
        return 0;
   }
   int CTibiaItemProxy::getExtraInfo(int index, int type) {
        if (type==2){
                return getFoodTimeAtIndex(index);
        }
        return 0;
   }
   //Deprecated Functions Section
   CUIntArray * CTibiaItemProxy::getItemsFoodArray()
   {
        return getFoodIdArrayPtr();
   }

   char *CTibiaItemProxy::getItemsItems(int nr)
   {
        return getItemNameAtIndex(nr);
   }
   //Deprecated Functions Section
   int CTibiaItemProxy::getItemsItemsId(int nr)
   {
        return getItemIdAtIndex(nr);
   }
   int CTibiaItemProxy::getItemsItemsCount()
   {
        return getItemCount();
   }
   void CTibiaItemProxy::setItemsItemsCount(int newCount) {
   }
   void CTibiaItemProxy::setItemsFoodCount(int newCount) {
   }
   //Deprecated Functions Section
   void CTibiaItemProxy::setItemsLootCount(int newCount) {
   }
   char *CTibiaItemProxy::getItemsFood(int nr)
   {
        return getFoodNameAtIndex(nr);
   }
   int CTibiaItemProxy::getItemsFoodId(int nr)
   {
        return getFoodIdAtIndex(nr);
   }
   //Deprecated Functions Section
   int CTibiaItemProxy::getItemsFoodCount()
   {
        return getFoodCount();
   }
   char *CTibiaItemProxy::getItemsCorpses(int nr)
   {
        return "not implemented";
   }
   int CTibiaItemProxy::getItemsCorpsesId(int nr)
   {
        return 0;
   }
   int CTibiaItemProxy::getItemsCorpsesCount()
   {
        return 0;
   }
   char *CTibiaItemProxy::getItemsLooted(int nr)
   {
        return getLootItemNameAtIndex(nr);
   }

   int CTibiaItemProxy::getItemsLootedId(int nr)
   {
        return getLootItemIdAtIndex(nr);
   }

   int CTibiaItemProxy::getItemsLootedCount()
   {
        return getLootItemCount();
   }
   //Deprecated Functions Section
   int CTibiaItemProxy::getObjectId(char *name)
   {
        return getItemId(name);
   }
   int CTibiaItemProxy::getFoodId(char *name)
   {
        int size=getFoodCount();
        for (int i=0;i<size;i++){
                if(!strcmp(getFoodNameAtIndex(i),name)) return getFoodIdAtIndex(i);
        }
        return 0;
   }
   int CTibiaItemProxy::getLootItemId(char *name)
   {
        int size=getLootItemCount();
        for (int i=0;i<size;i++){
                if(!strcmp(getLootItemNameAtIndex(i),name)) return getLootItemIdAtIndex(i);
        }
        return 0;
   }
   //Deprecated Functions Section
   void CTibiaItemProxy::setItemId(int index, int objectId) {
        char* name=getItemNameAtIndex(index);
        char* name2=(char*)malloc(strlen(name)+1);
        strcpy(name2,name);
        removeItem(index);
        addItem(name2,objectId);
        free(name2);
   }
   void CTibiaItemProxy::setFoodId(int index, int objectId) {
        char* name=getFoodNameAtIndex(index);
        char* name2=(char*)malloc(strlen(name)+1);
        strcpy(name2,name);
        removeFood(index);
        addFood(name2,objectId,getFoodTimeAtIndex(index));
        free(name2);
   }
   void CTibiaItemProxy::setLootItemId(int index, int objectId) {
        char* name=getLootItemNameAtIndex(index);
        char* name2=(char*)malloc(strlen(name)+1);
        strcpy(name2,name);
        removeLootItem(index);
        addLootItem(name2,objectId);
        free(name2);
   }
   Deprecated Function End
   int CTibiaItemProxy::getIndex(int objectId, int type) {
        typedef int (*Proto_fun)(int objectId, int type);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetIndex");
                if (fun) {
                        return fun(objectId, type);
                }
        }
        return -1;
   }

   char * CTibiaItemProxy::getName(int objectId)
   {
        typedef char * (*Proto_fun)(int objectId);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetName");
                if (fun)
                {
                        return fun(objectId);
                }
        }
        return "not implemented";
   }
   void CTibiaItemProxy::setItemName(int index, char *name) {
        typedef void (*Proto_fun)(int index, char *name);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetItemName");
                if (fun) {
                        fun(index, name);
                }
        }
   }
   void CTibiaItemProxy::setExtraInfo(int index, int info, int type) {
        typedef void (*Proto_fun)(int index, int info, int type);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetExtraInfo");
                if (fun) {
                        fun(index, info, type);
                }
        }
   }
   void CTibiaItemProxy::setFoodName(int index, char *name) {
        typedef void (*Proto_fun)(int index, char *name);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetFoodName");
                if (fun) {
                        fun(index, name);
                }
        }
   }
   void CTibiaItemProxy::setLootItemName(int index, char *name) {
        typedef void (*Proto_fun)(int index, char *name);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetLootName");
                if (fun) {
                        fun(index, name);
                }
        }
   }

   void CTibiaItemProxy::addItemItem(char *name, int objectId) {
        typedef void (*Proto_fun)(char *name, int objectId);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemAddItem");
                if (fun) {
                        fun(name, objectId);
                }
        }
   }
   void CTibiaItemProxy::addItemFood(char *name, int objectId, int time) {
        typedef void (*Proto_fun)(char *name, int objectId, int time);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemAddFood");
                if (fun) {
                        fun(name, objectId, time);
                }
        }
   }
   void CTibiaItemProxy::addItemLoot(char *name, int objectId) {
        typedef void (*Proto_fun)(char *name, int objectId);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemAddLoot");
                if (fun) {
                        fun(name, objectId);
                }
        }
   }
   int CTibiaItemProxy::getCorpseIdByCreatureName(char *name)
   {
        typedef int (*Proto_fun)(char *name);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetCorpseIdByCreatureName");
                if (fun)
                {
                        return fun(name);
                }
        }
        return 0;

   }
   int CTibiaItemProxy::getExtraInfo(int index, int type) {
        typedef int (*Proto_fun)(int index, int type);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetExtraInfo");
                if (fun) {
                        return fun(index, type);
                }
        }
        return 0;

   }
   CUIntArray * CTibiaItemProxy::getItemsFoodArray()
   {
        typedef CUIntArray * (*Proto_fun)();
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsFoodArray");
                if (fun)
                {
                        return fun();
                }
        }
        return NULL;
   }

   char *CTibiaItemProxy::getItemsItems(int nr)
   {
        typedef char * (*Proto_fun)(int nr);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsItems");
                if (fun)
                {
                        return fun(nr);
                }
        }
        return "not implemented";

   }
   int CTibiaItemProxy::getItemsItemsId(int nr)
   {
        typedef int (*Proto_fun)(int nr);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsItemsId");
                if (fun)
                {
                        return fun(nr);
                }
        }
        return 0;
   }
   int CTibiaItemProxy::getItemsItemsCount()
   {
        typedef int (*Proto_fun)();
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsItemsCount");
                if (fun)
                {
                        return fun();
                }
        }
        return 0;
   }
   void CTibiaItemProxy::setItemsItemsCount(int newCount) {
        typedef void (*Proto_fun)(int newCount);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetItemsItemsCount");
                if (fun) {
                        fun(newCount);
                }
        }
   }
   void CTibiaItemProxy::setItemsFoodCount(int newCount) {
        typedef void (*Proto_fun)(int newCount);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetItemsFoodCount");
                if (fun) {
                        fun(newCount);
                }
        }
   }
   void CTibiaItemProxy::setItemsLootCount(int newCount) {
        typedef void (*Proto_fun)(int newCount);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetItemsLootCount");
                if (fun) {
                        fun(newCount);
                }
        }
   }
   char *CTibiaItemProxy::getItemsFood(int nr)
   {
        typedef char * (*Proto_fun)(int nr);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsFood");
                if (fun)
                {
                        return fun(nr);
                }
        }
        return "not implemented";

   }
   int CTibiaItemProxy::getItemsFoodId(int nr)
   {
        typedef int (*Proto_fun)(int nr);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsFoodId");
                if (fun)
                {
                        return fun(nr);
                }
        }
        return 0;
   }
   int CTibiaItemProxy::getItemsFoodCount()
   {
        typedef int (*Proto_fun)();
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsFoodCount");
                if (fun)
                {
                        return fun();
                }
        }
        return 0;
   }
   char *CTibiaItemProxy::getItemsCorpses(int nr)
   {
        typedef char * (*Proto_fun)(int nr);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsCorpses");
                if (fun)
                {
                        return fun(nr);
                }
        }
        return "not implemented";

   }
   int CTibiaItemProxy::getItemsCorpsesId(int nr)
   {
        typedef int (*Proto_fun)(int nr);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsCorpsesId");
                if (fun)
                {
                        return fun(nr);
                }
        }
        return 0;
   }
   int CTibiaItemProxy::getItemsCorpsesCount()
   {
        typedef int (*Proto_fun)();
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsCorpsesCount");
                if (fun)
                {
                        return fun();
                }
        }
        return 0;
   }
   char *CTibiaItemProxy::getItemsLooted(int nr)
   {
        typedef char * (*Proto_fun)(int nr);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsLooted");
                if (fun)
                {
                        return fun(nr);
                }
        }
        return "not implemented";

   }

   int CTibiaItemProxy::getItemsLootedId(int nr)
   {
        typedef int (*Proto_fun)(int nr);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsLootedId");
                if (fun)
                {
                        return fun(nr);
                }
        }
        return 0;
   }

   int CTibiaItemProxy::getItemsLootedCount()
   {
        typedef int (*Proto_fun)();
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsLootedCount");
                if (fun)
                {
                        return fun();
                }
        }
        return 0;
   }

   int CTibiaItemProxy::getObjectId(char *name)
   {
        typedef int (*Proto_fun)(char *name);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetObjectId");
                if (fun)
                {
                        return fun(name);
                }
        }
        return 0;
   }
   int CTibiaItemProxy::getFoodId(char *name)
   {
        typedef int (*Proto_fun)(char *name);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetFoodId");
                if (fun)
                {
                        return fun(name);
                }
        }
        return 0;
   }
   int CTibiaItemProxy::getLootItemId(char *name)
   {
        typedef int (*Proto_fun)(char *name);
        if (dllModule)
        {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetLootItemId");
                if (fun)
                {
                        return fun(name);
                }
        }
        return 0;
   }
   void CTibiaItemProxy::setItemId(int index, int objectId) {
        typedef void (*Proto_fun)(int index, int objectId);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetItemObjectId");
                if (fun) {
                        fun(index, objectId);
                }
        }
   }
   void CTibiaItemProxy::setFoodId(int index, int objectId) {
        typedef void (*Proto_fun)(int index, int objectId);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetFoodObjectId");
                if (fun) {
                        fun(index, objectId);
                }
        }
   }
   void CTibiaItemProxy::setLootItemId(int index, int objectId) {
        typedef void (*Proto_fun)(int index, int objectId);
        if (dllModule) {
                static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSetLootObjectId");
                if (fun) {
                        fun(index, objectId);
                }
        }
   }

 */
