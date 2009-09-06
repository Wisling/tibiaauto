// MemReaderProxy.h: interface for the CMemReaderProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMREADERPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
#define AFX_MEMREADERPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_

#include "TibiaContainer.h"
#include "TibiaCharacter.h"
#include "TibiaItem.h"
#include "MemConstData.h"
#include "TibiaMapPoint.h"
#include "TibiaTile.h"
#include "TibiaMiniMap.h"
#include "TibiaMiniMapLabel.h"
#include "TibiaMiniMapPoint.h"
#include "TibiaVIPEntry.h"
#include "Skin.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMemReaderProxy  
{
public:
	void setProcessId(int processId);
	CTibiaContainer * readContainer(int containerNr);
	void writeSelfLightPower(int value);
	void writeSelfLightColor(int value);
	int readSelfLightPower();
	int readSelfLightColor();
 
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
	char * GetLoggedChar(int processId);
	int readBattleListMax();
	int readBattleListMin();
	CTibiaCharacter * readVisibleCreature(int nr);
	CTibiaItem * readItem(int locationAddress);
	CTibiaCharacter *readSelfCharacter();
	CTibiaCharacter *getCharacterByTibiaId(int tibiaId);
	CMemConstData getMemConstData();
	int mapGetSelfCellNr();
	int mapGetPointItemsCount(point p,int relToCell=-1);
	int mapGetPointItemId(point p, int stackNr,int relToCell=-1);
	void mapSetPointItemsCount(point p,int count,int relToCell=-1);
	void mapSetPointItemId(point p, int stackNr, int tileId,int relToCell=-1);
	int mapGetPointItemExtraInfo(point p, int stackNr,int extraPos,int relToCell=-1);
	int mapGetPointStackIndex(point p, int stackNr,int relToCell=-1);
	long getCurrentTm();
	void setRemainingTilesToGo(int val);
	void setMemRange(DWORD memAddressStart,DWORD memAddressEnd, char *data);
	void getMemRange(DWORD memAddressStart,DWORD memAddressEnd, char *data);
	void setXRayValues(int v1,int v2);
	int getXRayValue1();
	int getXRayValue2();


	void setMemIntValue(int address,int value);
	int getMemIntValue(int address);
	void writeEnableRevealCName();
	void writeDisableRevealCName();
	
	int getProcessId();
	int getKernelMainVersion();
	int getKernelPatchVersion();
	void writeCreatureLightPower(int creatureNr,int value);
	void writeCreatureLightColor(int creatureNr,int value);
	int readCreatureLightPower(int creatureNr);
	int readCreatureLightColor(int creatureNr);

	CTibiaTile *getTibiaTile(int tileNr);
	void setTibiaTile(int tileNr, CTibiaTile *newTile);

	CSkin loadSkin(CString);
	CSkin loadCurrentSkin(CString);
	bool saveSkin(CString, CSkin, bool);

	// sets a global variable of 'name' to 'value'
	void setGlobalVariable(char *name,char *value);
	// returns pointer to a global variable value string buffer
	// note: if other module changes value of the variable in the meantime, the change
	//       is visible to all modules immediatelly
	char *getGlobalVariable(char *name);

	CTibiaMiniMap *readMiniMap(int nr);
	CTibiaMiniMapLabel *readMiniMapLabel(int mapNr,int pointNr);
	CTibiaMiniMapPoint *readMiniMapPoint(int x, int y, int z);
	void writeMiniMapPoint(int x, int y, int z,int col,int spd);

	void setMainWindowText(char *text);
	void setMainTrayText(char *text);

	int getPlayerModeAttackPlayers();
	int getPlayerModeFollow();
	int getPlayerModeAttackType();

	char *getOpenWindowName();
	int getConnectionState();

	void writeCreatureDeltaXY(int creatureNr, int deltaX, int deltaY);
	int getCreatureDeltaX(int creatureNr);
	int getCreatureDeltaY(int creatureNr);

	void setAttackedCreature(int tibiaId);	
	CTibiaVIPEntry *readVIPEntry(int vipNr);
	CMemReaderProxy();
	virtual ~CMemReaderProxy();

	
private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_MEMREADERPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
