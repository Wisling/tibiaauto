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



// PackSenderProxy.cpp: implementation of the CPackSenderProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PackSenderProxy.h"


//////////////////////////////////////////////////////////////////////

HMODULE CPackSenderProxy::dllModule=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPackSenderProxy::CPackSenderProxy()
{
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

CPackSenderProxy::~CPackSenderProxy()
{

}

void CPackSenderProxy::setPipeHandle(HANDLE hPipe)
{
	typedef void (*Proto_fun)(HANDLE hPipe);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"setPipeHandle");
		if (fun)
		{
			fun(hPipe);
		}
	} 	
}

void CPackSenderProxy::useWithObjectFromFloorOnFloor(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectFromFloorOnFloor");
		if (fun)
		{
			fun(sourceObjectId, sourceX, sourceY, sourceZ, targetObjectId, targetX, targetY, targetZ, method);
		}
	} 	
}
void CPackSenderProxy::useWithObjectFromFloorInContainer(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetContNr,int targetPos, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetContNr,int targetPos, int method=2);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectFromFloorInContainer");
		if (fun)
		{
			fun(sourceObjectId, sourceX, sourceY, sourceZ, targetObjectId, targetContNr, targetPos,  method);
		}
	} 	
}
void CPackSenderProxy::useWithObjectFromContainerInContainer(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetContNr,int targetPos, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetContNr,int targetPos, int method=2);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectFromContainerInContainer");
		if (fun)
		{
			fun(sourceObjectId, sourceContNr, sourcePos, targetObjectId, targetContNr, targetPos, method);
		}
	} 	
}
void CPackSenderProxy::useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectFromContainerOnFloor");
		if (fun)
		{
			fun(sourceObjectId, sourceContNr, sourcePos, targetObjectId, targetX, targetY, targetZ,  method);
		}
	} 	
}
void CPackSenderProxy::useWithObjectOnFloor(int sourceObjectId,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectOnFloor");
		if (fun)
		{
			fun(sourceObjectId,targetObjectId,targetX,targetY,targetZ, method);
		}
	} 	
}
void CPackSenderProxy::useWithObjectInContainer(int sourceObjectId,int targetObjectId,int contNr,int itemPos, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int targetObjectId,int contNr,int itemPos, int method=2);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectInContainer");
		if (fun)
		{
			fun(sourceObjectId,targetObjectId,contNr,itemPos, method);
		}
	} 	
}


void CPackSenderProxy::moveObjectBetweenContainers(int objectId, int sourceContNr, int sourcePos, int targetContNr, int targetPos, int qty)
{
	typedef void (*Proto_fun)(int objectId, int sourceContNr, int sourcePos, int targetContNr, int targetPos, int qty);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderMoveObjectBetweenContainers");
		if (fun)
		{
			fun(objectId,sourceContNr,sourcePos,targetContNr,targetPos,qty);
		}
	} 	
}

void CPackSenderProxy::moveObjectFromFloorToContainer(int objectId,int sourceX,int sourceY,int sourceZ,int targetContNr,int targetPos,int quantity)
{
	typedef void (*Proto_fun)(int objectId,int sourceX,int sourceY,int sourceZ,int targetContNr,int targetPos,int quantity);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderMoveObjectFromFloorToContainer");
		if (fun)
		{
			fun(objectId,sourceX,sourceY,sourceZ,targetContNr,targetPos,quantity);
		}
	} 
}

void CPackSenderProxy::moveObjectFromContainerToFloor(int objectId, int contNr, int pos, int x, int y, int z,int quantity)
{
	typedef void (*Proto_fun)(int objectId, int contNr, int pos, int x, int y, int z,int quantity);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderMoveObjectFromContainerToFloor");
		if (fun)
		{
			fun(objectId, contNr, pos, x, y, z,quantity);
		}
	} 

}

void CPackSenderProxy::moveObjectFromFloorToFloor(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ,int quantity)
{
	typedef void (*Proto_fun)(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ,int quantity);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderMoveObjectFromFloorToFloor");
		if (fun)
		{
			fun(objectId, srcX, srcY, srcZ, destX, destY, destZ,quantity);
		}
	} 

}

void CPackSenderProxy::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId, int method)
{
	typedef void (*Proto_fun)(int contNr, int itemPos, int runeObjectId, int creatureId, int method);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderCastRuneAgainstCreature");
		if (fun)
		{
			fun(contNr, itemPos, runeObjectId, creatureId, method);
		}
	} 

}
void CPackSenderProxy::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId)
{
	castRuneAgainstCreature(contNr,itemPos,runeObjectId,creatureId,2);
}
void CPackSenderProxy::castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method)
{
	typedef void (*Proto_fun)(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderCastRuneAgainstHuman");
		if (fun)
		{
			fun(contNr, itemPos, runeObjectId, targetX, targetY, targetZ, method);
		}
	} 

}
void CPackSenderProxy::castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ)
{
	castRuneAgainstHuman(contNr,itemPos,runeObjectId,targetX,targetY,targetZ,2);
}
void CPackSenderProxy::useItemOnCreature(int objectId, int creatureId)
{
	typedef void (*Proto_fun)(int objectId, int creatureId);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemOnCreature");
		if (fun)
		{
			fun(objectId, creatureId);
		}
	} 

}
void CPackSenderProxy::useItemFromContainerOnCreature(int objectId, int contNr, int itemPos, int creatureId)
{
	typedef void (*Proto_fun)(int objectId, int contNr, int itemPos, int creatureId);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemFromContainerOnCreature");
		if (fun)
		{
			fun(objectId, contNr, itemPos, creatureId);
		}
	} 

}

void CPackSenderProxy::useItemFromFloorOnCreature(int objectId, int x, int y, int z, int creatureId)
{
	typedef void (*Proto_fun)(int objectId, int x, int y, int z, int creatureId);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemFromFloorOnCreature");
		if (fun)
		{
			fun(objectId, x, y, z, creatureId);
		}
	} 

}

void CPackSenderProxy::sendTAMessage(char *msg)
{
	typedef void (*Proto_fun)(char *msg);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendTAMessage");
		if (fun)
		{
			fun(msg);
		}
	} 

}

void CPackSenderProxy::useItem(int objectId)
{
	typedef void (*Proto_fun)(int objectId);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItem");
		if (fun)
		{
			fun(objectId);
		}
	} 

}
void CPackSenderProxy::useItemOnFloor(int objectId, int x, int y, int z)
{
	typedef void (*Proto_fun)(int objectId, int x, int y, int z);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemOnFloor");
		if (fun)
		{
			fun(objectId, x, y, z);
		}
	} 

}
void CPackSenderProxy::useItemInContainer(int objectId, int contNr, int pos)
{
	typedef void (*Proto_fun)(int objectId, int contNr, int pos);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemInContainer");
		if (fun)
		{
			fun(objectId, contNr, pos);
		}
	} 

}
int CPackSenderProxy::openAutoContainerFromFloor(int objectId,int x,int y,int z)
{
	typedef int (*Proto_fun)(int objectId,int x,int y,int z);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderOpenAutoContainerFromFloor");
		if (fun)
		{
			return fun(objectId,x,y,z);
		}
	} 
	return -1;
}
void CPackSenderProxy::openContainerFromFloor(int objectId,int x,int y,int z, int targetBag)
{
	typedef void (*Proto_fun)(int objectId,int x,int y,int z,int targetBag);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderOpenContainerFromFloor");
		if (fun)
		{
			fun(objectId,x,y,z,targetBag);
		}
	} 
	
}
int CPackSenderProxy::openAutoContainerFromContainer(int objectId, int contNrFrom, int contPosFrom)
{
	typedef int (*Proto_fun)(int objectId, int contNrFrom, int contPosFrom);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderOpenAutoContainerFromContainer");
		if (fun)
		{
			return fun(objectId, contNrFrom, contPosFrom);
		}
	} 
	return -1;
}
void CPackSenderProxy::openContainerFromContainer(int objectId, int contNrFrom, int contPosFrom, int targetBag)
{
	typedef void (*Proto_fun)(int objectId, int contNrFrom, int contPosFrom, int targetBag);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderOpenContainerFromContainer");
		if (fun)
		{
			fun(objectId, contNrFrom, contPosFrom, targetBag);
		}
	} 

}

void CPackSenderProxy::sendAttackedCreatureToAutoAim(int attackedCreature)
{
	typedef void (*Proto_fun)(int attackedCreature);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendAttackedCreatureToAutoAim");
		if (fun)
		{
			fun(attackedCreature);
		}
	} 
}

void CPackSenderProxy::say(const char *buf)
{
	typedef void (*Proto_fun)(const char *buf);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSay");
		if (fun)
		{
			fun(buf);
		}
	} 
}

void CPackSenderProxy::sayWhisper(const char *buf)
{
	typedef void (*Proto_fun)(const char *buf);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSayWhisper");
		if (fun)
		{
			fun(buf);
		}
	} 
}
void CPackSenderProxy::sayYell(const char *buf)
{
	typedef void (*Proto_fun)(const char *buf);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSayYell");
		if (fun)
		{
			fun(buf);
		}
	} 
}
void CPackSenderProxy::sayNPC(const char *buf)
{
	typedef void (*Proto_fun)(const char *buf);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSayNPC");
		if (fun)
		{
			fun(buf);
		}
	} 
}
void CPackSenderProxy::tell(char *msg, char *playerName)
{
	typedef void (*Proto_fun)(char *msg, char *playerName);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTell");
		if (fun)
		{
			fun(msg,playerName);
		}
	} 
}

void CPackSenderProxy::sayOnChan(char *msg, int channelId1,int channelId2)
{
	typedef void (*Proto_fun)(char *msg, int channelId1,int channelId2);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSayOnChan");
		if (fun)
		{
			fun(msg,channelId1,channelId2);
		}
	} 
}

void CPackSenderProxy::npcBuy(int objectId, int qty, int ignoreCap, int withBackpack)
{
	typedef void (*Proto_fun)(int objectId, int qty, int ignoreCap, int withBackpack);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderNpcBuy");
		if (fun)
		{
			fun(objectId,qty, ignoreCap, withBackpack);
		}
	} 
}

void CPackSenderProxy::npcSell(int objectId, int qty)
{
	typedef void (*Proto_fun)(int objectId, int qty);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderNpcSell");
		if (fun)
		{
			fun(objectId,qty);
		}
	} 
}

void CPackSenderProxy::logout()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderLogout");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stepMulti(int *direction,int size)
{
	typedef void (*Proto_fun)(int *direction,int size);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepMulti");
		if (fun)
		{
			fun(direction,size);
		}
	} 
}


void CPackSenderProxy::closeContainer(int contNr)
{
	typedef void (*Proto_fun)(int contNr);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderCloseContainer");
		if (fun)
		{
			fun(contNr);
		}
	} 
}
void CPackSenderProxy::attackMode(int attack,int follow,int attLock)
{
	typedef void (*Proto_fun)(int attack,int follow,int attLock);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderAttackMode");
		if (fun)
		{
			fun(attack,follow,attLock);
		}
	} 
}
void CPackSenderProxy::attack(int tibiaCharId)
{
	typedef void (*Proto_fun)(int tibiaCharId);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderAttack");
		if (fun)
		{
			fun(tibiaCharId);
		}
	} 
}
void CPackSenderProxy::follow(int tibiaCharId)
{
	typedef void (*Proto_fun)(int tibiaCharId);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderFollow");
		if (fun)
		{
			fun(tibiaCharId);
		}
	} 
}

void CPackSenderProxy::turnLeft()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTurnLeft");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::turnRight()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTurnRight");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::turnUp()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTurnUp");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::turnDown()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTurnDown");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stopAll()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStopAll");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::sendCreatureInfo(char *name, char *info1, char *info2)
{
	typedef void (*Proto_fun)(char *name, char *info1, char *info2);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendCreatureInfo");
		if (fun)
		{
			fun(name,info1,info2);
		}
	} 
}

void CPackSenderProxy::printText(CPoint pos, int red, int green, int blue, const char* text)
{
	typedef void (*Proto_fun)(CPoint pos, int red, int green, int blue, const char* text);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderPrintText");
		if (fun)
			fun(pos, red, green, blue, text);
		else
			AfxMessageBox("printText() not loaded!!");
	} 
}

void CPackSenderProxy::look(int x,int y, int z,int objectId)
{
	typedef void (*Proto_fun)(int x,int y, int z,int objectId);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderLook");
		if (fun)
		{
			fun(x,y,z,objectId);
		}
	} 
}

void CPackSenderProxy::ignoreLook(int end)
{
	typedef void (*Proto_fun)(int end);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderIgnoreLook");
		if (fun)
		{
			fun(end);
		}
	} 
}

void CPackSenderProxy::sendAutoAimConfig(int active, int onlyCreatures, int aimPlayersFromBattle)
{
	typedef void (*Proto_fun)(int active, int onlyCreatures, int aimPlayersFromBattle);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendAutoAimConfig");
		if (fun)
		{
			fun(active, onlyCreatures, aimPlayersFromBattle);
		}
	} 

}
void CPackSenderProxy::sendClearCreatureInfo()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendClearCreatureInfo");
		if (fun)
		{
			fun();
		}
	} 

}

void CPackSenderProxy::enableCName(int enable)
{
	typedef void (*Proto_fun)(int enable);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderEnableCName");
		if (fun)
		{
			fun(enable);
		}
	} 
}

void CPackSenderProxy::stepLeft()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepLeft");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stepRight()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepRight");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stepUp()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepUp");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stepDown()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepDown");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stepUpRight()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepUpRight");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stepUpLeft()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepUpLeft");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stepDownRight()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepDownRight");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::stepDownLeft()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepDownLeft");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::sendMount()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendMount");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::sendDismount()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendDismount");
		if (fun)
		{
			fun();
		}
	} 
}

void CPackSenderProxy::sendDirectPacket(const char* buf,int len)
{
	typedef void (*Proto_fun)(const char* buf,int len);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendDirectPacket");
		if (fun)
		{
			fun(buf,len);
		}
	} 
}