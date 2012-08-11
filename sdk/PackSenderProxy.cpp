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
<<<<<<< HEAD
	{
=======
	{			
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"setPipeHandle");
		if (fun)
		{
			fun(hPipe);
		}
<<<<<<< HEAD
	}
=======
	} 	
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::useWithObjectFromFloorOnFloor(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectFromFloorOnFloor");
		if (fun)
		{
			fun(sourceObjectId, sourceX, sourceY, sourceZ, targetObjectId, targetX, targetY, targetZ, method);
		}
<<<<<<< HEAD
	}
=======
	} 	
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::useWithObjectFromFloorInContainer(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetContNr,int targetPos, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetContNr,int targetPos, int method=2);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectFromFloorInContainer");
		if (fun)
		{
			fun(sourceObjectId, sourceX, sourceY, sourceZ, targetObjectId, targetContNr, targetPos,  method);
		}
<<<<<<< HEAD
	}
=======
	} 	
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::useWithObjectFromContainerInContainer(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetContNr,int targetPos, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetContNr,int targetPos, int method=2);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectFromContainerInContainer");
		if (fun)
		{
			fun(sourceObjectId, sourceContNr, sourcePos, targetObjectId, targetContNr, targetPos, method);
		}
<<<<<<< HEAD
	}
=======
	} 	
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectFromContainerOnFloor");
		if (fun)
		{
			fun(sourceObjectId, sourceContNr, sourcePos, targetObjectId, targetX, targetY, targetZ,  method);
		}
<<<<<<< HEAD
	}
=======
	} 	
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::useWithObjectOnFloor(int sourceObjectId,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectOnFloor");
		if (fun)
		{
			fun(sourceObjectId,targetObjectId,targetX,targetY,targetZ, method);
		}
<<<<<<< HEAD
	}
=======
	} 	
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::useWithObjectInContainer(int sourceObjectId,int targetObjectId,int contNr,int itemPos, int method/*=2*/)
{
	typedef void (*Proto_fun)(int sourceObjectId,int targetObjectId,int contNr,int itemPos, int method=2);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseWithObjectInContainer");
		if (fun)
		{
			fun(sourceObjectId,targetObjectId,contNr,itemPos, method);
		}
<<<<<<< HEAD
	}
=======
	} 	
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}


void CPackSenderProxy::moveObjectBetweenContainers(int objectId, int sourceContNr, int sourcePos, int targetContNr, int targetPos, int qty)
{
	typedef void (*Proto_fun)(int objectId, int sourceContNr, int sourcePos, int targetContNr, int targetPos, int qty);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{			
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderMoveObjectBetweenContainers");
		if (fun)
		{
			fun(objectId,sourceContNr,sourcePos,targetContNr,targetPos,qty);
		}
<<<<<<< HEAD
	}
=======
	} 	
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::moveObjectFromFloorToContainer(int objectId,int sourceX,int sourceY,int sourceZ,int targetContNr,int targetPos,int quantity)
{
	typedef void (*Proto_fun)(int objectId,int sourceX,int sourceY,int sourceZ,int targetContNr,int targetPos,int quantity);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderMoveObjectFromFloorToContainer");
		if (fun)
		{
			fun(objectId,sourceX,sourceY,sourceZ,targetContNr,targetPos,quantity);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::moveObjectFromContainerToFloor(int objectId, int contNr, int pos, int x, int y, int z,int quantity)
{
	typedef void (*Proto_fun)(int objectId, int contNr, int pos, int x, int y, int z,int quantity);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderMoveObjectFromContainerToFloor");
		if (fun)
		{
			fun(objectId, contNr, pos, x, y, z,quantity);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}

void CPackSenderProxy::moveObjectFromFloorToFloor(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ,int quantity)
{
	typedef void (*Proto_fun)(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ,int quantity);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderMoveObjectFromFloorToFloor");
		if (fun)
		{
			fun(objectId, srcX, srcY, srcZ, destX, destY, destZ,quantity);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}

void CPackSenderProxy::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId, int method)
{
	typedef void (*Proto_fun)(int contNr, int itemPos, int runeObjectId, int creatureId, int method);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderCastRuneAgainstCreature");
		if (fun)
		{
			fun(contNr, itemPos, runeObjectId, creatureId, method);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}
void CPackSenderProxy::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId)
{
	castRuneAgainstCreature(contNr,itemPos,runeObjectId,creatureId,2);
}
void CPackSenderProxy::castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method)
{
	typedef void (*Proto_fun)(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderCastRuneAgainstHuman");
		if (fun)
		{
			fun(contNr, itemPos, runeObjectId, targetX, targetY, targetZ, method);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}
void CPackSenderProxy::castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ)
{
	castRuneAgainstHuman(contNr,itemPos,runeObjectId,targetX,targetY,targetZ,2);
}
void CPackSenderProxy::useItemOnCreature(int objectId, int creatureId)
{
	typedef void (*Proto_fun)(int objectId, int creatureId);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemOnCreature");
		if (fun)
		{
			fun(objectId, creatureId);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}
void CPackSenderProxy::useItemFromContainerOnCreature(int objectId, int contNr, int itemPos, int creatureId)
{
	typedef void (*Proto_fun)(int objectId, int contNr, int itemPos, int creatureId);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemFromContainerOnCreature");
		if (fun)
		{
			fun(objectId, contNr, itemPos, creatureId);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}

void CPackSenderProxy::useItemFromFloorOnCreature(int objectId, int x, int y, int z, int creatureId)
{
	typedef void (*Proto_fun)(int objectId, int x, int y, int z, int creatureId);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemFromFloorOnCreature");
		if (fun)
		{
			fun(objectId, x, y, z, creatureId);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}

void CPackSenderProxy::sendTAMessage(char *msg)
{
	typedef void (*Proto_fun)(char *msg);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendTAMessage");
		if (fun)
		{
			fun(msg);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}

void CPackSenderProxy::useItem(int objectId)
{
	typedef void (*Proto_fun)(int objectId);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItem");
		if (fun)
		{
			fun(objectId);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}
void CPackSenderProxy::useItemOnFloor(int objectId, int x, int y, int z)
{
	typedef void (*Proto_fun)(int objectId, int x, int y, int z);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemOnFloor");
		if (fun)
		{
			fun(objectId, x, y, z);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}
void CPackSenderProxy::useItemInContainer(int objectId, int contNr, int pos)
{
	typedef void (*Proto_fun)(int objectId, int contNr, int pos);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUseItemInContainer");
		if (fun)
		{
			fun(objectId, contNr, pos);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}
int CPackSenderProxy::openAutoContainerFromFloor(int objectId,int x,int y,int z)
{
	typedef int (*Proto_fun)(int objectId,int x,int y,int z);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderOpenAutoContainerFromFloor");
		if (fun)
		{
			return fun(objectId,x,y,z);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
	return -1;
}
void CPackSenderProxy::openContainerFromFloor(int objectId,int x,int y,int z, int targetBag)
{
	typedef void (*Proto_fun)(int objectId,int x,int y,int z,int targetBag);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderOpenContainerFromFloor");
		if (fun)
		{
			fun(objectId,x,y,z,targetBag);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
	
}
int CPackSenderProxy::openAutoContainerFromContainer(int objectId, int contNrFrom, int contPosFrom)
{
	typedef int (*Proto_fun)(int objectId, int contNrFrom, int contPosFrom);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderOpenAutoContainerFromContainer");
		if (fun)
		{
			return fun(objectId, contNrFrom, contPosFrom);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
	return -1;
}
void CPackSenderProxy::openContainerFromContainer(int objectId, int contNrFrom, int contPosFrom, int targetBag)
{
	typedef void (*Proto_fun)(int objectId, int contNrFrom, int contPosFrom, int targetBag);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderOpenContainerFromContainer");
		if (fun)
		{
			fun(objectId, contNrFrom, contPosFrom, targetBag);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}

void CPackSenderProxy::sendAttackedCreatureToAutoAim(int attackedCreature)
{
	typedef void (*Proto_fun)(int attackedCreature);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendAttackedCreatureToAutoAim");
		if (fun)
		{
			fun(attackedCreature);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::say(const char *buf)
{
	typedef void (*Proto_fun)(const char *buf);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSay");
		if (fun)
		{
			fun(buf);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::sayWhisper(const char *buf)
{
	typedef void (*Proto_fun)(const char *buf);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSayWhisper");
		if (fun)
		{
			fun(buf);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::sayYell(const char *buf)
{
	typedef void (*Proto_fun)(const char *buf);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSayYell");
		if (fun)
		{
			fun(buf);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::sayNPC(const char *buf)
{
	typedef void (*Proto_fun)(const char *buf);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSayNPC");
		if (fun)
		{
			fun(buf);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::tell(char *msg, char *playerName)
{
	typedef void (*Proto_fun)(char *msg, char *playerName);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTell");
		if (fun)
		{
			fun(msg,playerName);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::sayOnChan(char *msg, int channelId1,int channelId2)
{
	typedef void (*Proto_fun)(char *msg, int channelId1,int channelId2);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSayOnChan");
		if (fun)
		{
			fun(msg,channelId1,channelId2);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::npcBuy(int objectId, int qty, int ignoreCap, int withBackpack)
{
	typedef void (*Proto_fun)(int objectId, int qty, int ignoreCap, int withBackpack);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderNpcBuy");
		if (fun)
		{
			fun(objectId,qty, ignoreCap, withBackpack);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::npcSell(int objectId, int qty)
{
	typedef void (*Proto_fun)(int objectId, int qty);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderNpcSell");
		if (fun)
		{
			fun(objectId,qty);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::logout()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderLogout");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepMulti(int *direction,int size)
{
	typedef void (*Proto_fun)(int *direction,int size);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepMulti");
		if (fun)
		{
			fun(direction,size);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}


void CPackSenderProxy::closeContainer(int contNr)
{
	typedef void (*Proto_fun)(int contNr);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderCloseContainer");
		if (fun)
		{
			fun(contNr);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::attackMode(int attack,int follow,int attLock)
{
	typedef void (*Proto_fun)(int attack,int follow,int attLock);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderAttackMode");
		if (fun)
		{
			fun(attack,follow,attLock);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::attack(int tibiaCharId)
{
	typedef void (*Proto_fun)(int tibiaCharId);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderAttack");
		if (fun)
		{
			fun(tibiaCharId);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}
void CPackSenderProxy::follow(int tibiaCharId)
{
	typedef void (*Proto_fun)(int tibiaCharId);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderFollow");
		if (fun)
		{
			fun(tibiaCharId);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::turnLeft()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTurnLeft");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::turnRight()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTurnRight");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::turnUp()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTurnUp");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::turnDown()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderTurnDown");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stopAll()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStopAll");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::sendCreatureInfo(char *name, char *info1, char *info2)
{
	typedef void (*Proto_fun)(char *name, char *info1, char *info2);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendCreatureInfo");
		if (fun)
		{
			fun(name,info1,info2);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::printText(CPoint pos, int red, int green, int blue, const char* text)
{
	typedef void (*Proto_fun)(CPoint pos, int red, int green, int blue, const char* text);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderPrintText");
		if (fun)
			fun(pos, red, green, blue, text);
		else
			AfxMessageBox("printText() not loaded!!");
<<<<<<< HEAD
	}
}

void CPackSenderProxy::registerInpacketRegex(int handle, char* regExp, int regLen)
{
	typedef void (*Proto_fun)(int handle, char* regExp, int regLen);
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderRegisterInpacketRegex");
		if (fun)
			fun(handle, regExp, regLen);
		else
			AfxMessageBox("registerInpacketRegex() not loaded!!");
	}
}

void CPackSenderProxy::unregisterInpacketRegex(int handle)
{
	typedef void (*Proto_fun)(int handle);
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderUnregisterInpacketRegex");
		if (fun)
			fun(handle);
		else
			AfxMessageBox("unregisterInpacketRegex() not loaded!!");
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::look(int x,int y, int z,int objectId)
{
	typedef void (*Proto_fun)(int x,int y, int z,int objectId);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderLook");
		if (fun)
		{
			fun(x,y,z,objectId);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::ignoreLook(int end)
{
	typedef void (*Proto_fun)(int end);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderIgnoreLook");
		if (fun)
		{
			fun(end);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::sendAutoAimConfig(int active, int onlyCreatures, int aimPlayersFromBattle)
{
	typedef void (*Proto_fun)(int active, int onlyCreatures, int aimPlayersFromBattle);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendAutoAimConfig");
		if (fun)
		{
			fun(active, onlyCreatures, aimPlayersFromBattle);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}
void CPackSenderProxy::sendClearCreatureInfo()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendClearCreatureInfo");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1

}

void CPackSenderProxy::enableCName(int enable)
{
	typedef void (*Proto_fun)(int enable);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderEnableCName");
		if (fun)
		{
			fun(enable);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepLeft()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepLeft");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepRight()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepRight");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepUp()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepUp");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepDown()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepDown");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepUpRight()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepUpRight");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepUpLeft()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepUpLeft");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepDownRight()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepDownRight");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::stepDownLeft()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderStepDownLeft");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::sendMount()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendMount");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::sendDismount()
{
	typedef void (*Proto_fun)();
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendDismount");
		if (fun)
		{
			fun();
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}

void CPackSenderProxy::sendDirectPacket(const char* buf,int len)
{
	typedef void (*Proto_fun)(const char* buf,int len);
	if (dllModule)
<<<<<<< HEAD
	{
=======
	{		
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"packSenderSendDirectPacket");
		if (fun)
		{
			fun(buf,len);
		}
<<<<<<< HEAD
	}
=======
	} 
>>>>>>> 18ec6100f7f6d8cadad6ce302029d3c3591c9cf1
}