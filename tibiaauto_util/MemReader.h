// MemReader.h: interface for the CMemReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMREADER_H__520F2B59_0B82_4188_9DE0_9362EB4062E7__INCLUDED_)
#define AFX_MEMREADER_H__520F2B59_0B82_4188_9DE0_9362EB4062E7__INCLUDED_

#include "TibiaCharacter.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaContainer.h"
#include "TibiaMapTile.h"
#include "TibiaMapPoint.h"
#include "MemConstData.h"
#include "TibiaMiniMap.h"
#include "TibiaMiniMapPoint.h"
#include "TibiaMiniMapLabel.h"
#include "TibiaVIPEntry.h"

class CMemReader  : public CMemConstData
{
public:	
	CMemReader(); 
	virtual ~CMemReader();		
public:
	int getCreatureDeltaY(int creatureNr);
	int getCreatureDeltaX(int creatureNr);
	
	void writeCreatureDeltaXY(int creatureNr, int deltaX, int deltaY);
	int getXRayValue2();
	int getXRayValue1();
	void setXRayValues(int v1, int v2);
	int getConnectionState();
	int isLoggedIn();
	char * getOpenWindowName();
	int getPlayerModeAttackPlayers();
	int getPlayerModeFollow();
	int getPlayerModeAttackType();
	int getPlayerModePVP();
	void setMainTrayText(char *text);
	void setMainWindowText(char *text);
	CTibiaMiniMapLabel * readMiniMapLabel(int mapNr,int pointNr);
	CTibiaMiniMapPoint * readMiniMapPoint(int x,int y,int z);
	void writeMiniMapPoint(int x,int y,int z,int col,int spd);
	CTibiaMiniMap * readMiniMap(int nr);
	void setRemainingTilesToGo(int val);
	void writeDisableRevealCName();
	void writeEnableRevealCName();
	long getCurrentTm();
	int mapGetPointItemExtraInfo(point p,int stackNr,int extraPos,int relToCell=-1);
	int mapGetPointStackIndex(point p,int stackNr,int relToCell=-1);
	void mapSetPointItemsCount(point p,int count,int relToCell=-1);
	void mapSetPointItemId(point p, int stackNr, int tileId,int relToCell=-1);
	int mapGetPointItemId(struct point p,int stackNr,int relToCell=-1);
	int mapGetPointItemsCount(struct point p,int relToCell=-1);
	void writeSelfLightColor(int value);
	void writeSelfLightPower(int value);
	int readSelfLightColor();
	int readSelfLightPower();
	int readCreatureLightPower(int creatureNr);
	int readCreatureLightColor(int creatureNr);
	void writeCreatureLightPower(int creatureNr,int value);
	void writeCreatureLightColor(int creatureNr,int value);
	void cancelAttackCoords();
	void writeGotoCoords(int x,int y,int z);
	int getLoggedCharNr();
	int getSelfEventFlags();
	void writeVisibleCreatureName(int chNr,char *name);
	CTibiaItem * getTradeItemPartner(int nr);
	CTibiaItem * getTradeItemSelf(int nr);
	int getTradeCountPartner();
	int getTradeCountSelf();
	int getAttackedCreature();
	void setAttackedCreature(int tibiaId);
	int getFollowedCreature();
	void setFollowedCreature(int tibiaId);
	int getNextPacketCount();
	char * GetLoggedChar(int processId);
	int readBattleListMax();
	int readBattleListMin();
	CTibiaCharacter * readVisibleCreature(int nr);
	CTibiaContainer *readContainer(int containerNr);
	CTibiaItem * readItem(int locationAddress);
	CTibiaCharacter *readSelfCharacter();
	CTibiaCharacter *getCharacterByTibiaId(int tibiaId);
	CTibiaVIPEntry *readVIPEntry(int nr);
	int dereference(int addr, int addBaseAddr=1);
	int mapGetSelfCellNr();
	CTibiaMapTile *readMapTile(int tileNr);
	int itemOnTopIndex(int x,int y,int z=0);
	int isItemOnTop(int x,int y,int *itemArr,int itemArrSize);
	int isItemCovered(int x,int y,int *itemArr,int itemArrSize);
	int isItemOnTop(int x,int y,CUIntArray& itemArr);
	int isItemCovered(int x,int y,CUIntArray& itemArr);
	int isItemOnTop(int x,int y,int itemId);
	int isItemCovered(int x,int y,int itemId);
	int getItemIndex(int x,int y,int itemId);
	int itemOnTopCode(int x,int y);
	int itemSeenOnTopIndex(int x,int y, int z=0);
	int itemSeenOnTopCode(int x,int y);
	int itemOnTopQty(int x,int y);
	int findNextClosedContainer(int afterCont=-1);
private:
	struct point mapAddPointToCoord(point coord,point p2);
	struct point mapGetCellCoord(int cellNr);
	int mapGetCoordCell(point c);
	int getMapTileStart(int tileNr);
	struct point mapDiffCoords(point c1,point c2);
	struct point mapShiftReferencePoint(point p, point oldCoord, point newCoord);
	int mapIsPointInTileArray(point p, int relToCell);
	int mapIsPointInScope(point p, int relToCell);
	
};

#endif // !defined(AFX_MEMREADER_H__520F2B59_0B82_4188_9DE0_9362EB4062E7__INCLUDED_)
