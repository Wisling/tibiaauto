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
		dllModule=LoadLibrary("mods\\tibiaauto_util.dll");
	}
}

CTibiaItemProxy::~CTibiaItemProxy()
{
	
}



char * CTibiaItemProxy::getName(int objectId)
{
	typedef char * (*Proto_fun)(int objectId);
	if (dllModule)
	{			
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetName");
		if (fun)
		{
			return fun(objectId);
		}
	} 
	return "not implemented";
}
void CTibiaItemProxy::refreshItemLists()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemRefreshItemLists");
		if (fun)
		{
			fun();
		}
	} 

}
int CTibiaItemProxy::getCorpseIdByCreatureName(char *name)
{
	typedef int (*Proto_fun)(char *name);
	if (dllModule)
	{			
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetCorpseIdByCreatureName");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsFoodArray");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsItems");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsItemsId");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsItemsCount");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}
char *CTibiaItemProxy::getItemsFood(int nr)
{
	typedef char * (*Proto_fun)(int nr);
	if (dllModule)
	{			
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsFood");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsFoodId");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsFoodCount");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsCorpses");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsCorpsesId");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsCorpsesCount");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsLooted");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsLootedId");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetItemsLootedCount");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetValueForConst");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaItemGetObjectId");
		if (fun)
		{
			return fun(name);
		}
	} 
	return 0;
}
