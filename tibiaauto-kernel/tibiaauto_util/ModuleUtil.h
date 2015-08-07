#pragma once

#include "tibiaauto_util.h"
#include <cstdint>
class CTibiaItem;
class CTibiaList;
template <class T> class CTibiaQueue;

enum StepDirsEnum
{
	STEP_NULL = 0,
	STEP_EAST = 1,
	STEP_NORTHEAST = 2,
	STEP_NORTH = 3,
	STEP_NORTHWEST = 4,
	STEP_WEST = 5,
	STEP_SOUTHWEST = 6,
	STEP_SOUTH = 7,
	STEP_SOUTHEAST = 8,
	STEP_UPSTAIRS = 0x10,
	STEP_DOWNSTAIRS = 0x20,
	STEP_TELEPORT = 0x40
};

class TIBIAAUTOUTIL_API CModuleUtil
{
public:
	static int randomFormula(int average, int halfrange);
	static int randomFormula(int average, int halfrange, int cutoff);
	static int randomFormula(int average, int halfrange, int minR, int maxR);
	static int waitForHpManaIncrease(int oldHp, int oldMana);
	static int waitForManaDecrease(int oldMana);
	static int waitForCapsChange(float origCaps);
	static int waitForItemChange(int locationAddress, int origItemId);
	static int waitForItemChange(int contNr, int slotNr, int origItemId, int quantity);
	static int waitForItemsInsideChange(int contNr, int origItemsCount);
	static int waitForItemsInsideChange(int contNr, int origItemSlot, int origItemQuantity, int origItemsCount);
	static CTibiaItem * lookupItem(int containerNr, CUIntArray *itemsAccepted);
	static CTibiaItem * lookupItem(int containerNr, CUIntArray *itemsAccepted, int qty);
	static int findFreeSpace(int &x, int &y, int z, int r = 1);
	static struct point findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int endSpecialLocation, uint8_t path[15], int radius = 1);
	static void executeWalk(int startX, int startY, int startZ, uint8_t path[15]);
	static void lootItemFromContainer(int conTNr, CUIntArray *acceptedItems, int ignoreCont1 = -1, int ignoreCont2 = -1);
	static int lootItemFromSpecifiedContainer(int containerNr, CUIntArray *acceptedItems, int containerCarrying);
	static int lootItemsToSpecifiedContainers(int containerNr, CUIntArray *acceptedItems, int carriedContainers);
	static void eatItemFromContainer(int contNr);
	static int waitForOpenContainer(int contNr, int open);
	static void sleepWithStop(int ms, int *stopFlag);
	static int waitForCreatureDisappear(int x, int y, int tibiaId);
	static int waitForCreatureDisappear(int creatureNr);
	static int waitToApproachSquare(int x, int y);
	static int waitToStandOnSquare(int x, int y);
	static int waitForCreatureDisappear(int x, int y, int tibiaId, int &, int &);
	static int calcLootChecksum(time_t tm, int killNr, int nameLen, int itemNr, int objectId, int qty, int lootInBags, int creatX, int creatY, int creatZ);
	static void prepareProhPointList();
	static void findPathAllDirection(CTibiaQueue<point> &pointsToAdd, int x, int y, int z);
	static int findNextClosedContainer(int afterCont = -1);
	static void masterDebug(const char* fname, const char* buf1 = "", const char* buf2 = "", const char* buf3 = "", const char* buf4 = "", const char* buf5 = "", const char* buf6 = "");
	static void getInstallPath(char path[2048]);
	static void setTASetting(const char* name, int value);
	static int getTASetting(const char* name);
	static struct point GetPathTab(int);
	static int GetPathTabCount(void);
	static int RegexMatch(char *string, char *regex);
private:
	static void findPathOnMapProcessPoint(CTibiaQueue<point> &pointsToAdd, int prevX, int prevY, int prevZ, int newX, int newY, int newZ);


	CModuleUtil();
	virtual ~CModuleUtil();
};

