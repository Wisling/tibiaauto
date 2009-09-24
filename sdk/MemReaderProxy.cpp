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



// MemReaderProxy.cpp: implementation of the CMemReaderProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemReaderProxy.h"
#include "MemConstData.h"



//////////////////////////////////////////////////////////////////////

HMODULE CMemReaderProxy::dllModule=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemReaderProxy::CMemReaderProxy()
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

		if (!dllModule)
		{
			char buf[256];
			sprintf(buf,"Loading tibiaauto_util.dll failed [1]! (%d)",GetLastError());
			sprintf(buf,"%s",pathBuf);
			MessageBox(0,buf,"ERROR",0);
			exit(0);
		}
	}
}

CMemReaderProxy::~CMemReaderProxy()
{
	
}

CTibiaVIPEntry * CMemReaderProxy::readVIPEntry(int vipNr)
{

	typedef CTibiaVIPEntry * (*Proto_fun)(int);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadVIPEntry");
		if (fun)
		{
			return fun(vipNr);
		}
	} 
	return NULL;
}

CTibiaContainer * CMemReaderProxy::readContainer(int containerNr)
{

	typedef CTibiaContainer * (*Proto_fun)(int containerNr);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadContainer");
		if (fun)
		{
			return fun(containerNr);
		}
	} 
	return NULL;
}

void CMemReaderProxy::setProcessId(int processId)
{
	typedef void (*Proto_fun)(int processId);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"setProcessId");
		if (fun)
		{
			fun(processId);
		}
	} 	
}

void CMemReaderProxy::writeSelfLightPower(int value)
{
	typedef void (*Proto_fun)(int value);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteSelfLightPower");
		if (fun)
		{
			fun(value);
		}
	} 	
}

void CMemReaderProxy::writeSelfLightColor(int value)
{
	typedef void (*Proto_fun)(int value);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteSelfLightColor");
		if (fun)
		{
			fun(value);
		}
	} 	
}

int CMemReaderProxy::readSelfLightPower()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadSelfLightPower");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}

int CMemReaderProxy::readSelfLightColor()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadSelfLightColor");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}


void CMemReaderProxy::cancelAttackCoords()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadCancelAttackCoords");
		if (fun)
		{
			fun();
		}
	} 
}

void CMemReaderProxy::writeGotoCoords(int x,int y,int z)
{
	typedef int (*Proto_fun)(int x, int y, int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteGotoCoords");
		if (fun)
		{
			fun(x,y,z);
		}
	} 
}
int CMemReaderProxy::getLoggedCharNr()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetLoggedCharNr");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;

}
int CMemReaderProxy::getSelfEventFlags()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetSelfEventFlags");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;

}
void CMemReaderProxy::writeVisibleCreatureName(int chNr,char *name)
{
	typedef void (*Proto_fun)(int,char *);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteVisibleCreatureName");
		if (fun)
		{
			fun(chNr,name);
		}
	} 
}
CTibiaItem * CMemReaderProxy::getTradeItemPartner(int nr)
{
	typedef CTibiaItem * (*Proto_fun)(int);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetTradeItemPartner");
		if (fun)
		{
			return fun(nr);
		}
	} 
	return NULL;

}
CTibiaItem * CMemReaderProxy::getTradeItemSelf(int nr)
{
	typedef CTibiaItem * (*Proto_fun)(int nr);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetTradeItemSelf");
		if (fun)
		{
			return fun(nr);
		}
	} 
	return NULL;
}
int CMemReaderProxy::getTradeCountPartner()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetTradeCountPartner");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;

}
int CMemReaderProxy::getTradeCountSelf()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetTradeCountSelf");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;

}
int CMemReaderProxy::getAttackedCreature()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetAttackedCreature");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;

}
char * CMemReaderProxy::GetLoggedChar(int processId)
{
	typedef char * (*Proto_fun)(int);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetLoggedChar");
		if (fun)
		{
			return fun(processId);
		} else {
			::MessageBox(0,"Loading memReadGetLoggedChar failed!","ERROR",0);
			exit(0);
		}
	} else {
		MessageBox(0,"Loading tibiaauto_util.dll failed [2]!","ERROR",0);
		exit(0);
	}
	return NULL;

}
int CMemReaderProxy::readBattleListMax()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadBattleListMax");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;

}
int CMemReaderProxy::readBattleListMin()
{

	typedef int (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadBattleListMin");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;

}
CTibiaCharacter * CMemReaderProxy::readVisibleCreature(int nr)
{
	typedef CTibiaCharacter * (*Proto_fun)(int);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadVisibleCreature");
		if (fun)
		{
			return fun(nr);
		}
	} 
	return NULL;

}
CTibiaItem * CMemReaderProxy::readItem(int locationAddress)
{
	typedef CTibiaItem * (*Proto_fun)(int);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadItem");
		if (fun)
		{
			return fun(locationAddress);
		}
	} 
	return NULL;
}
CTibiaCharacter *CMemReaderProxy::readSelfCharacter()
{
	typedef CTibiaCharacter * (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadSelfCharacter");
		if (fun)
		{
			return fun();
		}
	}
	return NULL;
}
CTibiaCharacter *CMemReaderProxy::getCharacterByTibiaId(int tibiaId)
{
	typedef CTibiaCharacter *(*Proto_fun)(int);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetCharacterByTibiaId");
		if (fun)
		{
			return fun(tibiaId);
		}
	} 
	return NULL;
}


CMemConstData CMemReaderProxy::getMemConstData()
{
	typedef CMemConstData (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getMemConstData");
		if (fun)
		{
			return fun();
		}
	} 
	return CMemConstData();
}

void CMemReaderProxy::setMemIntValue(int address,int value)
{
	typedef void (*Proto_fun)(int address,int value);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadSetMemIntValue");
		if (fun)
		{
			fun(address,value);
		}
	} 
}

int CMemReaderProxy::getMemIntValue(int address)
{
	typedef int (*Proto_fun)(int address);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetMemIntValue");
		if (fun)
		{
			return fun(address);
		}
	} 
	return 0;
}

int CMemReaderProxy::mapGetSelfCellNr()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadMapGetSelfCellNr");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;

}
int CMemReaderProxy::mapGetPointItemsCount(point p,int relToCell/*=-1*/)
{
	typedef int (*Proto_fun)(point p,int relToCell=-1);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadMapGetPointItemsCount");
		if (fun)
		{
			return fun(p, relToCell);
		}
	} 
	return 0;

}
int CMemReaderProxy::mapGetPointItemId(point p, int stackNr,int relToCell/*=-1*/)
{
	typedef int (*Proto_fun)(point p,int stackNr,int relToCell=-1);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadMapGetPointItemId");
		if (fun)
		{
			return fun(p,stackNr, relToCell);
		}
	} 
	return 0;

}

int CMemReaderProxy::mapGetPointItemExtraInfo(point p, int stackNr, int extraPos,int relToCell/*=-1*/)
{
	typedef int (*Proto_fun)(point p,int stackNr, int extraPos,int relToCell=-1);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadMapGetPointItemExtraInfo");
		if (fun)
		{
			return fun(p,stackNr,extraPos, relToCell);
		}
	} 
	return 0;

}

int CMemReaderProxy::mapGetPointStackIndex(point p, int stackNr,int relToCell/*=-1*/)
{
	typedef int (*Proto_fun)(point p,int stackNr,int relToCell=-1);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadMapGetPointStackIndex");
		if (fun)
		{
			return fun(p,stackNr, relToCell);
		}
	} 
	return 0;

}

void  CMemReaderProxy::mapSetPointItemsCount(point p, int count,int relToCell/*=-1*/)
{
	typedef int (*Proto_fun)(point p, int count,int relToCell=-1);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadMapSetPointItemsCount");
		if (fun)
		{
			fun(p,count, relToCell);
		}
	} 

}
void CMemReaderProxy::mapSetPointItemId(point p, int stackNr, int tileId,int relToCell/*=-1*/)
{
	typedef int (*Proto_fun)(point p,int stackNr, int tileId,int relToCell=-1);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadMapSetPointItemId");
		if (fun)
		{
			fun(p,stackNr, tileId, relToCell);
		}
	} 
}


long CMemReaderProxy::getCurrentTm()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetCurrenTm");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}


void CMemReaderProxy::writeEnableRevealCName()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteEnableRevealCName");
		if (fun)
		{
			fun();
		}
	} 
}

void CMemReaderProxy::writeDisableRevealCName()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteDisableRevealCName");
		if (fun)
		{
			fun();
		}
	} 
}

	
int CMemReaderProxy::getProcessId()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getProcessId");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}
int CMemReaderProxy::getKernelMainVersion()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getKernelMainVersion");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}
int CMemReaderProxy::getKernelPatchVersion()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getKernelPatchVersion");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}
void CMemReaderProxy::writeCreatureLightPower(int creatureNr,int value)
{
	typedef int (*Proto_fun)(int creatureNr,int value);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteCreatureLightPower");
		if (fun)
		{
			fun(creatureNr,value);
		}
	} 
}
void CMemReaderProxy::writeCreatureLightColor(int creatureNr,int value)
{	
	typedef int (*Proto_fun)(int creatureNr,int value);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteCreatureLightColor");
		if (fun)
		{
			fun(creatureNr,value);
		}
	} 
}
int CMemReaderProxy::readCreatureLightPower(int creatureNr)
{
	typedef int (*Proto_fun)(int creatureNr);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadCreatureLightPower");
		if (fun)
		{
			return fun(creatureNr);
		}
	} 
	return 0;
}
int CMemReaderProxy::readCreatureLightColor(int creatureNr)
{
	typedef int (*Proto_fun)(int creatureNr);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadCreatureLightColor");
		if (fun)
		{
			return fun(creatureNr);
		}
	} 
	return 0;
}

CTibiaTile *CMemReaderProxy::getTibiaTile(int tileNr)
{
	typedef CTibiaTile * (*Proto_fun)(int tileNr);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getTibiaTile");
		if (fun)
		{
			return fun(tileNr);
		}
	} 
	return NULL;
}

void CMemReaderProxy::setTibiaTile(int tileNr, CTibiaTile *newTile)
{
	typedef void (*Proto_fun)(int tileNr, CTibiaTile *newTile);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"setTibiaTile");
		if (fun)
		{
			fun(tileNr, newTile);
		}
	} 
}

void CMemReaderProxy::cleanupTibiaTiles()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"cleanupTibiaTiles");
		if (fun)
		{
			fun();
		}
	} 
}

CSkin CMemReaderProxy::loadSkin(CString pathBuf) {
	typedef CSkin (__stdcall *Proto_fun)(CString);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"loadSkin");
		if (fun)
		{
			return fun(pathBuf);
		}
	} 
	return skin;
}
CSkin CMemReaderProxy::loadCurrentSkin(CString pathBuf) {
	typedef CSkin (__stdcall *Proto_fun)(CString);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"loadCurrentSkin");
		if (fun)
		{
			return fun(pathBuf);
		}
	} 
	return skin;
}
bool CMemReaderProxy::saveSkin(CString pathBuf, CSkin saveSkin, bool saveSeperate) {
	typedef bool (__stdcall *Proto_fun)(CString, CSkin, bool);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"saveSkin");
		if (fun)
		{
			if (fun(pathBuf, saveSkin, saveSeperate))
				return true;
			else 
				return false;
		}
	}
	return false;
}

void CMemReaderProxy::setRemainingTilesToGo(int val)
{
	typedef void (*Proto_fun)(int);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadSetRemainingTilesToGo");
		if (fun)
		{
			fun(val);
		}
	} 

}

void CMemReaderProxy::setMemRange(DWORD memAddressStart,DWORD memAddressEnd, char *data)
{	
	typedef void (*Proto_fun)(DWORD memAddressStart,DWORD memAddressEnd, char *data);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadSetMemRange");
		if (fun)
		{
			fun(memAddressStart,memAddressEnd,data);
		}
	} 	
}

void CMemReaderProxy::getMemRange(DWORD memAddressStart,DWORD memAddressEnd, char *data)
{
	typedef void (*Proto_fun)(DWORD memAddressStart,DWORD memAddressEnd, char *data);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetMemRange");
		if (fun)
		{
			fun(memAddressStart,memAddressEnd,data);
		}
	} 	
}

char *CMemReaderProxy::getGlobalVariable(char *name)
{
	typedef char * (*Proto_fun)(char *name);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"variableStoreGetVariable");
		if (fun)
		{
			return fun(name);
		}
	} 
	return NULL;
}

void CMemReaderProxy::setGlobalVariable(char *name,char *value)
{
	typedef void (*Proto_fun)(char *name,char *value);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"variableStoreSetVariable");
		if (fun)
		{
			fun(name,value);
		}
	} 	
}

CTibiaMiniMap *CMemReaderProxy::readMiniMap(int nr)
{
	typedef CTibiaMiniMap * (*Proto_fun)(int nr);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadMiniMap");
		if (fun)
		{
			return fun(nr);
		}
	} 
	return NULL;
}

CTibiaMiniMapLabel *CMemReaderProxy::readMiniMapLabel(int mapNr,int pointNr)
{
	typedef CTibiaMiniMapLabel * (*Proto_fun)(int mapNr,int pointNr);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadMiniMapLabel");
		if (fun)
		{
			return fun(mapNr,pointNr);
		}
	} 
	return NULL;
}

CTibiaMiniMapPoint *CMemReaderProxy::readMiniMapPoint(int x, int y, int z)
{
	typedef CTibiaMiniMapPoint * (*Proto_fun)(int x, int y, int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadReadMiniMapPoint");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return NULL;
}

void CMemReaderProxy::writeMiniMapPoint(int x, int y, int z,int col,int spd)
{
	typedef void (*Proto_fun)(int x, int y, int z,int col,int spd);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteMiniMapPoint");
		if (fun)
		{
			fun(x,y,z,col,spd);
		}
	} 
}

void CMemReaderProxy::setMainWindowText(char *text)
{
	typedef char * (*Proto_fun)(char *text);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadSetMainWindowText");
		if (fun)
		{
			fun(text);
		}
	} 
}

void CMemReaderProxy::setMainTrayText(char *text)
{
	typedef char * (*Proto_fun)(char *text);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadSetMainTrayText");
		if (fun)
		{
			fun(text);
		}
	} 
}

int CMemReaderProxy::getPlayerModeAttackPlayers()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetPlayerModeAttackPlayers");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}

int CMemReaderProxy::getPlayerModeAttackType()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetPlayerModeAttackType");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}

int CMemReaderProxy::getPlayerModeFollow()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetPlayerModeFollow");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}

char * CMemReaderProxy::getOpenWindowName()
{
	typedef char *(*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetOpenWindowName");
		if (fun)
		{
			return fun();
		}
	} 
	return "";
}

int CMemReaderProxy::getConnectionState()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetConnectionState");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}

void CMemReaderProxy::setXRayValues(int v1, int v2)
{
	typedef void (*Proto_fun)(int v1, int v2);
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadSetXRayValues");
		if (fun)
		{
			fun(v1,v2);
		}
	}
}

int CMemReaderProxy::getXRayValue1()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetXRayValue1");
		if (fun)
		{
			return fun();
		}
	}
	return 0;
}

int CMemReaderProxy::getXRayValue2()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetXRayValue2");
		if (fun)
		{
			return fun();
		}
	}
	return 0;
}

void CMemReaderProxy::writeCreatureDeltaXY(int creatureNr, int deltaX, int deltaY)
{
	typedef void (*Proto_fun)(int v1, int v2, int v3);
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadWriteCreatureDeltaXY");
		if (fun)
		{
			fun(creatureNr,deltaX,deltaY);
		}
	}
}

void CMemReaderProxy::setAttackedCreature(int tibiaId)
{
	typedef void (*Proto_fun)(int v1);
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadSetAttackedCreature");
		if (fun)
		{
			fun(tibiaId);
		}
	}
}

int CMemReaderProxy::getCreatureDeltaX(int creatureNr)
{
	typedef int (*Proto_fun)(int creatureNr);
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetCreatureDeltaX");
		if (fun)
		{
			return fun(creatureNr);
		}
	}
	return 0;
}

int CMemReaderProxy::getCreatureDeltaY(int creatureNr)
{
	typedef int (*Proto_fun)(int creatureNr);
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"memReadGetCreatureDeltaY");
		if (fun)
		{
			return fun(creatureNr);
		}
	}
	return 0;
}