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
	void moveObjectBetweenContainers(int objectId, int sourceContNr, int sourcePos, int targetContNr, int targetPos, int qty);

	void useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ);
	void useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method);
	void useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method, int extraInfo);
	void moveObjectFromFloorToContainer(int objectId,int sourceX,int sourceY,int sourceZ,int targetContNr,int targetPos,int quantity);
	void moveObjectFromContainerToFloor(int objectId, int contNr, int pos, int x, int y, int z,int quantity);
	void moveObjectFromFloorToFloor(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ,int quantity);


	void castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId, int method);
	void castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId);
	void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method);
	void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ);
	void sendTAMessage(char *msg);
	void useItemOnFloor(int objectId, int x, int y, int z);
	void useItemInContainer(int objectId, int contNr, int pos);
	void openContainerFromFloor(int objectId,int x,int y,int z,int targetBag);
	void openContainerFromContainer(int objectId, int contNrFrom, int contPosFrom, int targetBag);
	void say(const char *buf);	
	void sayWhisper(const char *buf);
	void sayYell(const char *buf);
	void tell(char *msg, char *playerName);
	void sayOnChan(char *msg, int channelId);
	void logout();
	void stepMulti(int *direction,int size);

	void closeContainer(int contNr);
	void attackMode(int mode,int follow);
	void attack(int tibiaCharId);
	void turnLeft();
	void turnRight();
	void turnUp();
	void turnDown();
	void sendCreatureInfo(char *name, char *info1, char *info2);
	void look(int x,int y, int z,int objectId);
	void ignoreLook(int ignoreLookEnd);
	void sendAutoAimConfig(int active, int onlyCreatures, int aimPlayersFromBattle);
	void sendClearCreatureInfo();
	void enableCName(int enable);




	void sendAttackedCreatureToAutoAim(int attackedCreature);
	
	CPackSenderProxy();
	virtual ~CPackSenderProxy();
private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_PACKSENDERPROXY_H__92FA63F2_EFB1_4955_8438_69A41E490007__INCLUDED_)
