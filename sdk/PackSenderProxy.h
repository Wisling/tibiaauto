// PackSenderProxy.h: interface for the CPackSenderProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKSENDERPROXY_H__92FA63F2_EFB1_4955_8438_69A41E490007__INCLUDED_)
#define AFX_PACKSENDERPROXY_H__92FA63F2_EFB1_4955_8438_69A41E490007__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPackSenderProxy  
{
public:
	void setPipeHandle(HANDLE hPipe);

	void useWithObjectFromFloorOnFloor(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	void useWithObjectFromFloorInContainer(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetContNr,int targetPos, int method=2);
	void useWithObjectFromContainerInContainer(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetContNr,int targetPos, int method=2);
	void useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	void moveObjectFromFloorToFloor(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ,int quantity);
	void moveObjectFromFloorToContainer(int objectId,int sourceX,int sourceY,int sourceZ,int targetContNr,int targetPos,int quantity);
	void moveObjectBetweenContainers(int objectId,int sourceContNr,int sourcePos,int targetContNr,int targetPos, int qty);
	void moveObjectFromContainerToFloor(int objectId, int contNr, int pos, int x,int y, int z, int quantity);


	void castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId, int method);
	void castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId);
	void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method);
	void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ);
	void sendTAMessage(char *msg);
	void useItemOnFloor(int objectId, int x, int y, int z);
	void useItemInContainer(int objectId, int contNr, int pos);
	int openAutoContainerFromFloor(int objectId,int x,int y,int z);
	void openContainerFromFloor(int objectId,int x,int y,int z, int targetBag);
	int openAutoContainerFromContainer(int objectId, int contNrFrom, int contPosFrom);
	void openContainerFromContainer(int objectId, int contNrFrom, int contPosFrom, int targetBag);
	void say(const char *buf);
	void sayWhisper(const char *buf);
	void sayYell(const char *buf);
	void sayNPC(const char *msg);
	void tell(char *msg, char *playerName);
	void sayOnChan(char *msg, int channelId1,int channelId2);
	void logout();
	void stepMulti(int *direction,int size);
	void npcSell(int objectId,int qty);
	void npcBuy(int objectId,int qty, int ignoreCap, int withBackpack);

	void useItem(int objectId);
	void useWithObjectOnFloor(int sourceObjectId,int targetObjectId,int targetX,int targetY,int targetZ, int method=2);
	void useWithObjectInContainer(int sourceObjectId,int targetObjectId,int contNr,int itemPos, int method=2);

	void useItemOnCreature(int objectId, int creatureId);
	void useItemFromContainerOnCreature(int objectId, int contNr, int itemPos, int creatureId);
	void useItemFromFloorOnCreature(int objectId, int x, int y, int z, int creatureId);

	void closeContainer(int contNr);
	void attackMode(int attack,int follow,int attLock);
	void attack(int tibiaCharId);
	void follow(int tibiaCharId);
	void turnLeft();
	void turnRight();
	void turnUp();
	void turnDown();
	void stopAll();
	void sendCreatureInfo(char *name, char *info1, char *info2);
	void look(int x,int y, int z,int objectId);
	void ignoreLook(int ignoreLookEnd);
	void sendAutoAimConfig(int active, int onlyCreatures, int aimPlayersFromBattle);
	void sendClearCreatureInfo();
	void enableCName(int enable);
	void stepLeft();
	void stepRight();
	void stepUp();
	void stepDown();
	void stepUpRight();
	void stepDownRight();
	void stepUpLeft();
	void stepDownLeft();
	void sendMount();
	void sendDismount();
	void sendDirectPacket(const char* buf,int len);

	void printText(CPoint pos, int red, int green, int blue, const char* text);
	void registerInpacketRegex(int handle, char* regExp, int regLen);
	void unregisterInpacketRegex(int handle);


	void sendAttackedCreatureToAutoAim(int attackedCreature);
	
	CPackSenderProxy();
	virtual ~CPackSenderProxy();
private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_PACKSENDERPROXY_H__92FA63F2_EFB1_4955_8438_69A41E490007__INCLUDED_)
