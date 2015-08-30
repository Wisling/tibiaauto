#pragma once

#include "tibiaauto_util.h"
#include <cstdint>

class TIBIAAUTOUTIL_API CPackSender
{
public:
	static void stopAll();
	static void enableCName(int enable);
	static void ignoreLook(time_t end);
	static void look(int x, int y, int z, int objectId);
	static void sendCreatureInfo(char *name, char *info1, char *info2);
	static void moveObjectFromFloorToFloor(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ, int quantity);
	static void moveObjectFromFloorToContainer(int objectId, int sourceX, int sourceY, int sourceZ, int targetContNr, int targetPos, int quantity);
	static void moveObjectBetweenContainers(int objectId, int sourceContNr, int sourcePos, int targetContNr, int targetPos, int qty);
	static void moveObjectFromContainerToFloor(int objectId, int contNr, int pos, int x, int y, int z, int quantity);
	static void sendAttackedCreatureToAutoAim(int attackedCreature);
	static void turnDown();
	static void turnUp();
	static void turnRight();
	static void turnLeft();
	static void tell(char *msg, char *playerName);
	static void sayOnChan(char *msg, int channelId1, int channelId2);
	static void sayNPC(char *msg);
	static void useItemOnFloor(int objectId, int x, int y, int z);
	static int openAutoContainerFromContainer(int objectId, int contNrFrom, int contPosFrom);
	static void openContainerFromContainer(int objectId, int contNrFrom, int contPosFrom, int targetBag);
	static void sendTAMessage(char *msg);
	static void revealFish(int enable);
	static void attackMode(int attack, int follow, int attlock, int PVPMode);
	static void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method);
	static void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ);
	static void castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId);
	static void castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId, int method);
	static void closeContainer(int contNr);
	static void attack(int tibiaCharId);
	static void follow(int tibiaCharId);
	static void stepMulti(uint8_t* direction, int size);
	static void stepRight();
	static void stepLeft();
	static void stepDown();
	static void stepUp();
	static void stepDownRight();
	static void stepUpRight();
	static void stepDownLeft();
	static void stepUpLeft();
	static void logout();
	static void useItemInContainer(int objectId, int contNr, int pos);
	static int openAutoContainerFromFloor(int objectId, int x, int y, int z);
	static void openContainerFromFloor(int objectId, int x, int y, int z, int targetBag);
	static void say(const char *buf);
	static void sayWhisper(const char *buf);
	static void sayYell(const char *buf);
	static void useWithObjectFromFloorOnFloor(int sourceObjectId, int sourceX, int sourceY, int sourceZ, int targetObjectId, int targetX, int targetY, int targetZ, int method = 2);
	static void useWithObjectFromFloorInContainer(int sourceObjectId, int sourceX, int sourceY, int sourceZ, int targetObjectId, int targetContNr, int targetPos, int method = 2);
	static void useWithObjectFromContainerInContainer(int sourceObjectId, int sourceContNr, int sourcePos, int targetObjectId, int targetContNr, int targetPos, int method = 2);
	static void useWithObjectFromContainerOnFloor(int sourceObjectId, int sourceContNr, int sourcePos, int targetObjectId, int targetX, int targetY, int targetZ, int method = 2);
	static void sendAutoAimConfig(int active, int onlyCreatures, int aimPlayersFromBattle);
	static void sendClearCreatureInfo();
	static void sendMount();
	static void sendDismount();
	static void sendDirectPacket(const char* buf, int len);

	static void useItem(int objectId);
	static void useWithObjectOnFloor(int sourceObjectId, int targetObjectId, int targetX, int targetY, int targetZ, int method = 2);
	static void useWithObjectInContainer(int sourceObjectId, int targetObjectId, int contNr, int itemPos, int method = 2);
	static void useWithObjectSend(int sourceObjectId, int sourceX, int sourceY_Cont, int sourceZ_Pos, int targetObjectId, int targetX, int targetY_Cont, int targetZ_Pos, int method = 2);

	static void useItemOnCreatureSend(int objectId, int x, int y_Cont, int z_Pos, int creatureId);
	static void useItemOnCreature(int objectId, int creatureId);
	static void useItemFromContainerOnCreature(int objectId, int contNr, int itemPos, int creatureId);
	static void useItemFromFloorOnCreature(int objectId, int x, int y, int z, int creatureId);

	static void npcBuy(int objectId, int qty, int ignoreCap, int withBackpack);
	static void npcSell(int objectId, int qty);
	static void CPackSender::printText(CPoint pos, int red, int green, int blue, char* message);
	static void CPackSender::registerInpacketRegex(int handle, char* regExp, int regLen);
	static void CPackSender::unregisterInpacketRegex(int handle);
private:
	static void sendPacket(char *buf);
	static void sendPacket(char *buf, int method);
	CPackSender() {};
	virtual ~CPackSender() {};
};

