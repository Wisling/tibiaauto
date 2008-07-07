// PackSender.h: interface for the CPackSender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKSENDER_H__A8C435EC_72BC_417E_9C2E_AF2A3F40133F__INCLUDED_)
#define AFX_PACKSENDER_H__A8C435EC_72BC_417E_9C2E_AF2A3F40133F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPackSender  
{
public:		
	void stopAll();
	void enableCName(int enable);
	void ignoreLook(int end);
	void look(int x,int y,int z,int objectId);
	void sendCreatureInfo(char *name,char *info1,char *info2);
	void moveObjectFromFloorToFloor(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ,int quantity);
	void sendAttackedCreatureToAutoAim(int attackedCreature);
	void turnDown();
	void turnUp();
	void turnRight();
	void turnLeft();
	void tell(char *msg, char *playerName);
	void sayOnChan(char *msg, int channelId1,int channelId2);
	void sayNPC(char *msg);
	void useItemOnFloor(int objectId, int x, int y, int z);
	void moveObjectFromContainerToFloor(int objectId, int contNr, int pos, int x,int y, int z, int quantity);
	void openContainerFromContainer(int objectId, int contNrFrom, int contPosFrom, int targetBag);
	void moveObjectFromFloorToContainer(int objectId,int sourceX,int sourceY,int sourceZ,int targetContNr,int targetPos,int quantity);
	void sendTAMessage(char *msg);
	void revealFish(int enable);
	void attackMode(int mode,int follow);
	void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method);
	void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ);
	void castRuneAgainstCreature(int contNr,int itemPos,int runeObjectId,int creatureId);
	void CPackSender::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId, int method);
	void closeContainer(int contNr);
	void attack(int tibiaCharId);
	void stepMulti(int *direction,int size);
	void stepRight();
	void stepLeft();
	void stepDown();
	void stepUp();
	void logout();
	void useItemInContainer(int objectId,int contNr,int pos);
	void openContainerFromFloor(int objectId,int x,int y,int z,int targetBag);
	void say(const char *buf);	
	void sayWhisper(const char *buf);	
	void sayYell(const char *buf);	
	void moveObjectBetweenContainers(int objectId,int sourceContNr,int sourcePos,int targetContNr,int targetPos, int qty);
	void useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ);
	void useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method);
	void useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method, int extrainfo);
	void sendAutoAimConfig(int active, int onlyCreatures, int aimPlayersFromBattle);
	void sendClearCreatureInfo();
	void npcBuy(int objectId,int qty);
	void npcSell(int objectId,int qty);
	CPackSender();
	virtual ~CPackSender();
private:
	void sendPacket(char *buf);
	void sendPacket(char *buf,int method);
};

#endif // !defined(AFX_PACKSENDER_H__A8C435EC_72BC_417E_9C2E_AF2A3F40133F__INCLUDED_)
