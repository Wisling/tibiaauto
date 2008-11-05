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

//////////////////////////////////////////////////////////////////////

HMODULE CTibiaItemProxy::dllModule=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaItemProxy::CTibiaItemProxy()
{
	// load module	
	if (dllModule==NULL)	
	{
		char installPath[1024];
		unsigned long installPathLen=1023;
		installPath[0]='\0';
		HKEY hkey=NULL;
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
		{
			RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
			RegCloseKey(hkey);
		}
		if (!strlen(installPath))
		{
			::MessageBox(0,"ERROR! Unable to read TA install directory! Please reinstall!","ERROR",0);
			exit(1);
		}

		char pathBuf[2048];
		sprintf(pathBuf,"%s\\mods\\tibiaauto_util.dll",installPath);

		dllModule=LoadLibrary(pathBuf);
	}
}

CTibiaItemProxy::~CTibiaItemProxy()
{
	
}



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
void CTibiaItemProxy::refreshItemLists()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemRefreshItemLists");
		if (fun)
		{
			fun();
		}
	} 

}
void CTibiaItemProxy::saveItemLists() {
	typedef void (*Proto_fun)();
	if (dllModule) {			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemSaveItemLists");
		if (fun) {
			fun();
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
void CTibiaItemProxy::addItemFood(char *name, int objectId) {
	typedef void (*Proto_fun)(char *name, int objectId);
	if (dllModule) {			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemAddFood");
		if (fun) {
			fun(name, objectId);
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

int CTibiaItemProxy::getValueForConst(char *code)
{
	typedef int (*Proto_fun)(char *code);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetValueForConst");
		if (fun)
		{
			return fun(code);
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
