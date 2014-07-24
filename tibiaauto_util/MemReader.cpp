// MemReader.cpp: implementation of the CMemReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemReader.h"
#include "PackSender.h"
#include "MemUtil.h"
#include "TibiaItemProxy.h"
#include "TibiaTile.h"
#include "TileReader.h"
#include "SkinLoader.h"
#include "TibiaItem.h"
#include <iostream>
#include <fstream>
using namespace std;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int mod(int i,int m){
	int ans=i%m;
	if (ans<0) ans+=m;
	return ans;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemReader::CMemReader() {
	CMemConstData();
}

CMemReader::~CMemReader() {

}

// Reads circularly linked list of VIP entries stored in Tibia memory
// Input: starting from 0, the index of the entry to be read
// Output: returns valid CTibiaVIPEntry with name == "" if 'nr' is the first empty entry
// returns NULL if 'nr' is out of bounds
// returns complete CTibiaVIPEntry if 'nr'
CTibiaVIPEntry *CMemReader::readVIPEntry(int nr) {
	int vipCount = CMemUtil::GetMemIntValue(m_memAddressVIP+4);
	if (nr < 0 || nr > vipCount){
		return NULL;
	}
	CTibiaVIPEntry *vip = new CTibiaVIPEntry();
	
	if(nr == vipCount){
		vip->name[0]=0;
		return vip;
	}

	DWORD linkedListAddr = dereference(m_memAddressVIP);
	DWORD vipEntryAddr = dereference(linkedListAddr,0);
	//0x0 next entry; 0x4 prev entry; 0x8 E68B6C; 0x10 playerId; 0x14 icon; 0x18 name/pointer to name; 0x28 namelen;
	//0x28 namemaxlen; 0x34 description/desc pointer; 0x44 desclen; 0x48 descmaxlen; 0x50 status; 0x58 loginTm
	for (int iNR=0; iNR!=nr && vipEntryAddr!=linkedListAddr; iNR++){
		vipEntryAddr = dereference(vipEntryAddr,0);
	}
	if (iNR==nr && vipEntryAddr!=linkedListAddr){
		vip->id = CMemUtil::GetMemIntValue(vipEntryAddr+0x10,0);
		vip->icon = CMemUtil::GetMemIntValue(vipEntryAddr+0x14,0) & 0xff;
		vip->notify = CMemUtil::GetMemIntValue(vipEntryAddr+0x14,0)>>8 & 0xff;

		vip->nameLen = CMemUtil::GetMemIntValue(vipEntryAddr+0x28,0);
		if (vip->nameLen >= 16){
			int nameAddr = CMemUtil::GetMemIntValue(vipEntryAddr+0x18,0);
			CMemUtil::GetMemRange(nameAddr,nameAddr+30,vip->name,0);
		}else{
			CMemUtil::GetMemRange(vipEntryAddr+0x18,vipEntryAddr+0x18+vip->nameLen,vip->name,0);
		}
		vip->name[vip->nameLen]=0;

		vip->descrLen = CMemUtil::GetMemIntValue(vipEntryAddr+0x40,0);
		if (vip->descrLen >= 16){
			int descrAddr = CMemUtil::GetMemIntValue(vipEntryAddr+0x30,0);
			CMemUtil::GetMemRange(descrAddr,descrAddr+vip->descrLen,vip->descr,0);
		}else{
			CMemUtil::GetMemRange(vipEntryAddr+0x30,vipEntryAddr+0x30+vip->descrLen,vip->descr,0);
		}
		vip->descr[vip->descrLen]=0;

		vip->status = CMemUtil::GetMemIntValue(vipEntryAddr+0x48,0) & 0xff;
		vip->loginTm = CMemUtil::GetMemIntValue(vipEntryAddr+0x50,0);//64bit
		
		return vip;
	}
	delete vip;
	return NULL;
}

long findContainer(int i, long addrCurr, long addrHead, int depth=0){
	if(depth>0&& 0){
		CPackSender sender;
		char buf[111];
		sprintf(buf,"%d",depth);
		sender.sendTAMessage(buf);
	}
	if(addrCurr!=addrHead && CMemUtil::GetMemIntValue(addrCurr+0x10,0)==i){
		return addrCurr;
	}
	if(depth<5){//binary structure is guaranteed to reach all 16 containers after 4 iterations
		for(int adj=0;adj<12;adj+=4){
			long addrNext = CMemUtil::GetMemIntValue(addrCurr+adj,0);
			if(addrNext != addrHead){
				long ret = findContainer(i, addrNext, addrHead, depth+1);
				if(ret) return ret;
			}
		}
	}
	return NULL;
}

int CMemReader::readOpenContainerCount(){
	return CMemUtil::GetMemIntValue(CMemUtil::GetMemIntValue(m_memAddressFirstContainer)+0x8,0);
}

CTibiaContainer *CMemReader::readContainer(int containerNr) {
	CTibiaContainer *container = new CTibiaContainer();
	//triply linked list
	//container number
	int i;
	
	long addrHead = CMemUtil::GetMemIntValue(CMemUtil::GetMemIntValue(m_memAddressFirstContainer)+4,0);
	long addrIndCont = 0;
	try {
		addrIndCont = findContainer(containerNr,addrHead,addrHead);
	} catch(const char* e) {
		addrIndCont = 0;
	}
	if(addrIndCont){ // return container as is if not found
		long addrCont = CMemUtil::GetMemIntValue(addrIndCont+0x14,0);
		container->flagOnOff=1;
		container->number=CMemUtil::GetMemIntValue(addrCont,0);
		//container->???=CMemUtil::GetMemIntValue(addrCont+4,0); // extraInfo
		//container->???=CMemUtil::GetMemIntValue(addrCont+8,0); // qty
		container->objectId=CMemUtil::GetMemIntValue(addrCont+0xC,0);
		//container->???=CMemUtil::GetMemIntValue(addrCont+0x30,0);
		//container->???=CMemUtil::GetMemIntValue(addrCont+0x34,0);
		//container->???=CMemUtil::GetMemIntValue(addrCont+0x38,0);
		//container->???=CMemUtil::GetMemIntValue(addrCont+0x3C,0);
		container->size=CMemUtil::GetMemIntValue(addrCont+0x40,0);
		container->itemsInside=CMemUtil::GetMemIntValue(addrCont+0x44,0);
		long addrItems = CMemUtil::GetMemIntValue(addrCont+0x4C,0);

		try{//if returns error then addrItems is most likely not a valid address anymore
			if(addrItems){ // if addrItems == NULL then there are no items in the container
				for (i=0;i<container->itemsInside;i++)
				{
					CTibiaItem *item = new CTibiaItem();
					item->objectId = CMemUtil::GetMemIntValue(addrItems+i*m_memLengthItem+8,0);
					item->quantity = CMemUtil::GetMemIntValue(addrItems+i*m_memLengthItem+4,0);
					CTileReader tileReader;
					CTibiaTile *tile=tileReader.getTile(item->objectId);
					if(!tile){
						delete item;
						throw "Error invalid container item.";
					}
					if (!tile->stackable&&item->quantity>1) item->quantity=1;//handles vials and other special uses of "quantity" variable
					item->pos = i;
					container->items.Add(item);
				}
			}else{
				container->itemsInside = 0;
			}
		} catch(const char* e) {
			delete container;
			container = new CTibiaContainer(); //return blank container
			return container;
		}
	}//else: return container as is if it is not found to be open
	return container;
}


CTibiaCharacter *CMemReader::readSelfCharacter() {
	CTibiaCharacter *ch = new CTibiaCharacter();

	ch->hp = CMemUtil::GetMemIntValue(m_memAddressHP)^CMemUtil::GetMemIntValue(m_memAddressXor);
	ch->mana = CMemUtil::GetMemIntValue(m_memAddressMana)^CMemUtil::GetMemIntValue(m_memAddressXor);
	ch->maxHp = CMemUtil::GetMemIntValue(m_memAddressHPMax)^CMemUtil::GetMemIntValue(m_memAddressXor);
	ch->maxMana = CMemUtil::GetMemIntValue(m_memAddressManaMax)^CMemUtil::GetMemIntValue(m_memAddressXor);
	// note: since 8.31 capacity has accuracy to 2 decimal places
	ch->cap = (CMemUtil::GetMemIntValue(m_memAddressCap)^CMemUtil::GetMemIntValue(m_memAddressXor))/100.0;
	ch->stamina = CMemUtil::GetMemIntValue(m_memAddressStamina);
	ch->exp = CMemUtil::GetMemIntValue(m_memAddressExp);
	//ch->exp += (__int64)CMemUtil::GetMemIntValue(m_memAddressExp+4) << 32; //Note Experience became 64 bits since 8.7
	ch->lvl = CMemUtil::GetMemIntValue(m_memAddressLvl);
	ch->mlvl = CMemUtil::GetMemIntValue(m_memAddressMlvl);
	ch->mlvlPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressMlvlPercLeft);
	ch->soulPoints = CMemUtil::GetMemIntValue(m_memAddressSoulPoints);
	switch(CMemUtil::GetMemIntValue(m_memAddressVocation)){
	case 0: strncpy(ch->voc,"n",3); break;
	case 1: strncpy(ch->voc,"k",3); break;
	case 2: strncpy(ch->voc,"p",3); break;
	case 3: strncpy(ch->voc,"s",3); break;
	case 4: strncpy(ch->voc,"d",3); break;
	case 5: strncpy(ch->voc,"ek",3); break;
	case 6: strncpy(ch->voc,"rp",3); break;
	case 7: strncpy(ch->voc,"ms",3); break;
	case 8: strncpy(ch->voc,"ed",3); break;
	}


	ch->skillAxe = CMemUtil::GetMemIntValue(m_memAddressSkillAxe);
	ch->skillClub = CMemUtil::GetMemIntValue(m_memAddressSkillClub);
	ch->skillSword = CMemUtil::GetMemIntValue(m_memAddressSkillSword);
	ch->skillDist = CMemUtil::GetMemIntValue(m_memAddressSkillDist);
	ch->skillFish = CMemUtil::GetMemIntValue(m_memAddressSkillFish);
	ch->skillShield = CMemUtil::GetMemIntValue(m_memAddressSkillShield);
	ch->skillFist = CMemUtil::GetMemIntValue(m_memAddressSkillFist);
	ch->skillAxePercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillAxePercLeft);
	ch->skillClubPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillClubPercLeft);
	ch->skillSwordPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillSwordPercLeft);
	ch->skillDistPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillDistPercLeft);
	ch->skillFishPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillFishPercLeft);
	ch->skillShieldPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillShieldPercLeft);
	ch->skillFistPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillFistPercLeft);
	
	ch->visible=1;

	int loggedCharNr=getLoggedCharNr();
	if (loggedCharNr>=0)
	{
		CTibiaCharacter *monCh = readVisibleCreature(loggedCharNr);
		ch->tibiaId=monCh->tibiaId;
		ch->x=monCh->x;
		ch->y=monCh->y;
		ch->z=monCh->z;
		strcpy(ch->name,monCh->name);
		ch->lookDirection=monCh->lookDirection;
		ch->colorHead=monCh->colorHead;
		ch->colorBody=monCh->colorBody;
		ch->colorLegs=monCh->colorLegs;
		ch->colorFoot=monCh->colorFoot;
		ch->walkSpeed=monCh->walkSpeed;
		ch->skulls=monCh->skulls;
		ch->shields=monCh->shields;
		ch->outfitId=monCh->outfitId;
		ch->mountId=monCh->mountId;
		ch->monsterType=monCh->monsterType;
		ch->hpPercLeft=monCh->hpPercLeft;
		ch->lastAttackTm=monCh->lastAttackTm;
		ch->nr=loggedCharNr;
		ch->warIcon=monCh->warIcon;
		ch->blocking=monCh->blocking;
		ch->moving=monCh->moving;

		delete monCh;
	}

	return ch;
}

CTibiaItem * CMemReader::readItem(int locationAddress)
{
	CTibiaItem *item = new CTibiaItem();

	item->objectId = CMemUtil::GetMemIntValue(locationAddress + 8);
	item->quantity = CMemUtil::GetMemIntValue(locationAddress + 4);
	CTileReader tileReader;
	CTibiaTile *tile=tileReader.getTile(item->objectId);
	if (tile&&!tile->stackable&&item->quantity>1) item->quantity=1;

	if (item->objectId==0)
		item->quantity=0;

	return item;
}

CTibiaCharacter * CMemReader::readVisibleCreature(int nr)
{
	CTibiaCharacter *ch = new CTibiaCharacter();

	int offset = m_memAddressFirstCreature+nr*m_memLengthCreature;
	char memcharinfo[1024];
	memset(memcharinfo,0,1024);
	CMemUtil::GetMemRange(offset,offset+m_memLengthCreature,memcharinfo,1);

	ch->hp=-1;
	ch->mana=-1;
	ch->tibiaId=*((int*)memcharinfo);
	ch->z=*((int*)(memcharinfo+36));
	ch->y=*((int*)(memcharinfo+40));
	ch->x=*((int*)(memcharinfo+44));
	ch->outfitId=*((int*)(memcharinfo+72));

	//ch->vertShift=*((int*)(memcharinfo+48));
	//ch->horizShift=*((int*)(memcharinfo+52));
	ch->lookDirection=*((int*)(memcharinfo+56));
	//ch->timeFinishMove=*((int*)(memcharinfo+60));

	//ch->timeTillCanMoveAgain=*((int*)(memcharinfo+64));
	//ch->totVertIncrement=*((int*)(memcharinfo+68));
	//ch->totHorizIncement=*((int*)(memcharinfo+72));
	//ch->tileSpeed=*((int*)(memcharinfo+76));

	ch->moving=*((int*)(memcharinfo+80));
	//ch->lastMovedLookDirection=*((int*)(memcharinfo+84));
	//ch->???min=*((int*)(memcharinfo+88));
	//ch->???max=*((int*)(memcharinfo+92));

	ch->monsterType=*((int*)(memcharinfo+96));
	ch->colorHead=*((int*)(memcharinfo+100));
	ch->colorBody=*((int*)(memcharinfo+104));
	ch->colorLegs=*((int*)(memcharinfo+108));

	ch->colorFoot=*((int*)(memcharinfo+112));
	//ch->addon=*((int*)(memcharinfo+116));
	ch->mountId=*((int*)(memcharinfo+120));
	//ch->lightPower=*((int*)(memcharinfo+124));

	//ch->lightColour=*((int*)(memcharinfo+128));
	//ch->offenderboxcolour=*((int*)(memcharinfo+132));
	ch->lastAttackTm=*((int*)(memcharinfo+136));
	ch->hpPercLeft=*((int*)(memcharinfo+140));

	ch->walkSpeed=*((int*)(memcharinfo+144));
	ch->visible=*((int*)(memcharinfo+m_offsetCreatureVisible));
	ch->skulls=*((int*)(memcharinfo+152));
	ch->shields=*((int*)(memcharinfo+156));

	ch->warIcon=*((int*)(memcharinfo+160));
	ch->blocking=*((int*)(memcharinfo+148));
	//ch->??=*((int*)(memcharinfo+168));357?
	//ch->??=*((int*)(memcharinfo+172));
	//ch->helpercolour=*((int*)(memcharinfo+176));
	//ch->helpercolour=*((int*)(memcharinfo+180));
	//ch->lightningbolt=*((int*)(memcharinfo+184));
	//ch->lightningbolt=*((int*)(memcharinfo+188));
	//ch->NPCbubble=*((int*)(memcharinfo+192)); //0-none,1-talk,2-quest,3-trade,4-quest trade,


	ch->nr=nr;
	
	CMemUtil::GetMemRange(offset+4,offset+4+31,ch->name);
	
	return ch;
}

int CMemReader::readBattleListMin()
{
	return 0;
}

int CMemReader::readBattleListMax()
{
	return CMemUtil::GetMemIntValue(m_memAddressBattleMax);
}

char * CMemReader::GetLoggedChar(int processId)
{
	char *ret;
	long selfId;
	int i;
	
	CMemUtil::GetMemIntValue(processId,m_memAddressSelfId,&selfId,1);
	for (i=0;i<m_memMaxCreatures;i++)
	{
		long creatureId,visible;
		long offset = m_memAddressFirstCreature+i*m_memLengthCreature;
		CMemUtil::GetMemIntValue(processId,offset+0,&creatureId,1);
		CMemUtil::GetMemIntValue(processId,offset+164,&visible,1);
		if (creatureId == 0) break;
		if (selfId==creatureId&&visible)
		{
			char buf[33];
			buf[32]='\0';
			CMemUtil::GetMemRange(processId,offset+4,offset+4+31,buf,1);
			ret=(char *)malloc(strlen(buf)+1);
			strcpy(ret,buf);
			return ret;
		};
	};
		
	ret=(char*)malloc(strlen("unknown")+1);
	sprintf(ret,"unknown");
	return ret;
}

int CMemReader::getAttackedCreature()
{
	return CMemUtil::GetMemIntValue(m_memAddressAttackedCreature);
}

void CMemReader::setAttackedCreature(int tibiaId)
{
	CMemUtil::SetMemIntValue(m_memAddressAttackedCreature,tibiaId);
}

int CMemReader::getFollowedCreature()
{
	return CMemUtil::GetMemIntValue(m_memAddressFollowedCreature);
}

void CMemReader::setFollowedCreature(int tibiaId)
{
	CMemUtil::SetMemIntValue(m_memAddressFollowedCreature,tibiaId);
}

int CMemReader::getNextPacketCount()
{
	int ret=CMemUtil::GetMemIntValue(m_memAddressPacketCount)+1;
	CMemUtil::SetMemIntValue(m_memAddressPacketCount,ret);
	return ret;
}

CTibiaCharacter *CMemReader::getCharacterByTibiaId(int tibiaId)
{
	CMemReader reader;
	int i;
	for (i=0;i<m_memMaxCreatures;i++)
	{
		int offset = m_memAddressFirstCreature+i*m_memLengthCreature;

		if (CMemUtil::GetMemIntValue(offset)==tibiaId)
			return readVisibleCreature(i);
	}
	return NULL;
}

int CMemReader::getTradeCountSelf()
{
	return CMemUtil::GetMemIntValue(m_memAddressTradeCountSelf);
}

int CMemReader::getTradeCountPartner()
{
	return CMemUtil::GetMemIntValue(m_memAddressTradeCountPartner);
}

CTibiaItem * CMemReader::getTradeItemSelf(int nr)
{
	CTibiaItem *item = new CTibiaItem();
	item->objectId=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemSelf+nr*m_memLengthItem+8);
	item->quantity=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemSelf+nr*m_memLengthItem+4);
	CTileReader tileReader;
	CTibiaTile *tile=tileReader.getTile(item->objectId);
	if (tile&&!tile->stackable&&item->quantity>1) item->quantity=1;
	item->pos=nr;
	return item;
}

CTibiaItem * CMemReader::getTradeItemPartner(int nr)
{
	CTibiaItem *item = new CTibiaItem();
	item->objectId=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemPartner+nr*m_memLengthItem+8);
	item->quantity=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemPartner+nr*m_memLengthItem+4);
	CTileReader tileReader;
	CTibiaTile *tile=tileReader.getTile(item->objectId);
	if (tile&&!tile->stackable&&item->quantity>1) item->quantity=1;
	item->pos=nr;
	return item;
}

void CMemReader::writeVisibleCreatureName(int chNr, char *name)
{
	int offset = m_memAddressFirstCreature+chNr*m_memLengthCreature;
	if (strlen(name)>31)
	{
		AfxMessageBox("writeVisibleCreatureName: too long name");
		return;
	}
	CMemUtil::SetMemRange(offset+4,offset+4+(strlen(name)+1),name);
}

int CMemReader::getSelfEventFlags()
{
	return CMemUtil::GetMemIntValue(m_memAddressSelfFlags);//^CMemUtil::GetMemIntValue(m_memAddressFirstCreature-8);
}

int CMemReader::getLoggedCharNr()
{
	
	int i;
	
	long selfId=CMemUtil::GetMemIntValue(m_memAddressSelfId);
	

	for (i=0;i<m_memMaxCreatures;i++)
	{
		long creatureId,visible;
		long offset = m_memAddressFirstCreature+i*m_memLengthCreature;
		creatureId=CMemUtil::GetMemIntValue(offset+0);
		visible=CMemUtil::GetMemIntValue(offset+m_offsetCreatureVisible);
		if (creatureId == 0) break;
		if (selfId==creatureId&&visible)
		{
			return i;
		};
	};
		
	return -1;
}

void CMemReader::writeGotoCoords(int x, int y, int z)
{
	CMemReader reader;

	int chNr=getLoggedCharNr();
	CMemUtil::SetMemIntValue(m_memAddressGoX,x);
	CMemUtil::SetMemIntValue(m_memAddressGoY,y);
	CMemUtil::SetMemIntValue(m_memAddressGoZ,z);
	//Manage Tibia's memory
	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+80+chNr*m_memLengthCreature, 1);//enable to accept gotocoords, (80=is char moving param)
}

void CMemReader::cancelAttackCoords()
{
	int chNr=getLoggedCharNr();
//	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+80+chNr*m_memLengthCreature,0); will take care of itself
	CMemUtil::SetMemIntValue(m_memAddressGoX,0);
	CMemUtil::SetMemIntValue(m_memAddressGoY,0);
	setRemainingTilesToGo(0);//if left at > 0, no actions can be performed
}

int CMemReader::readCreatureLightPower(int creatureNr)
{
	CMemReader reader;
	return CMemUtil::GetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+124);
}

int CMemReader::readCreatureLightColor(int creatureNr)
{
	CMemReader reader;
	return CMemUtil::GetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+128);
}

void CMemReader::writeCreatureLightPower(int creatureNr,int value)
{
	CMemReader reader;
	CMemUtil::SetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+124,value);
}

void CMemReader::writeCreatureLightColor(int creatureNr,int value)
{
	CMemReader reader;
	CMemUtil::SetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+128,value);
}



int CMemReader::readSelfLightPower()
{
	CMemReader reader;
	int loggedCharNr=reader.getLoggedCharNr();
	return readCreatureLightPower(loggedCharNr);
}

int CMemReader::readSelfLightColor()
{
	CMemReader reader;
	int loggedCharNr=reader.getLoggedCharNr();
	return readCreatureLightColor(loggedCharNr);
}

void CMemReader::writeSelfLightPower(int value)
{
	CMemReader reader;
	int loggedCharNr=reader.getLoggedCharNr();
	writeCreatureLightPower(loggedCharNr,value);
}

void CMemReader::writeSelfLightColor(int value)
{
	CMemReader reader;
	int loggedCharNr=reader.getLoggedCharNr();
	writeCreatureLightColor(loggedCharNr,value);
}

CTibiaMapTile *CMemReader::readMapTile(int tileNr){
	CTibiaMapTile *maptile = new CTibiaMapTile();
	DWORD tileStart = getMapTileStart(tileNr);
	DWORD TESTtileStart = tileStart;
	do{
		CMemUtil::GetMemRange(tileStart,tileStart+m_memLengthMapTile,(char*)maptile,0);//this address comes from Tibia itself and need not be shifted
		if(maptile->count){
			int a=0;
		}
		tileStart=TESTtileStart;
		TESTtileStart = getMapTileStart(tileNr); // Check that reference hasn't changed
	}while(tileStart!=TESTtileStart);//Tibia's map is a very large area of memory. Ensure that pointer is still same afterward in case we read from deallocated space.
	return maptile;
}

int CMemReader::getMapTileStart(int tileNr){
	return  dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile;
}

int CMemReader::mapGetSelfCellNr()
{
	static int prevSelfTileNr=0;
	static int prevSelfTilePos=0;
	int wouldReturn=0;
	CTibiaCharacter *self = readSelfCharacter();
	if (self->z<0 || self->z>15) {
		delete self;
		return 0;
	}
	
	// check self tile cache
	CTibiaMapTile *maptile = readMapTile(prevSelfTileNr);
	int c=maptile->count;
	int tileId=maptile->items[prevSelfTilePos].itemId;
	if (prevSelfTilePos<c&&tileId==99)
	{
		int tileCharId = maptile->items[prevSelfTilePos].quantity;
		if (tileCharId==self->tibiaId)
		{
			delete self;
			delete maptile;
			return prevSelfTileNr;
		}
	}
	delete maptile; maptile = NULL;

	int floorSize=m_memMaxMapTiles/8;
	int tileNrLowest=max(0,min(1768,(self->z<=7)?floorSize*(7-self->z):(floorSize*2)));
	int tileNrHighest=tileNrLowest+floorSize;
/*
	// check using one of 8 memory locations for current cell(this makes the function 100 times faster)
	int tmp=CMemUtil::GetMemIntValue(dereference(m_memAddressMiniMapStart-4));
	for (int lvl=0;lvl<8;lvl++)
	{
		int cell=252*lvl+(tmp+116)%(14*18);
		int pos;
		int tmo=dereference(m_memAddressMapStart)+cell*m_memLengthMapTile;
		int count=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+cell*m_memLengthMapTile);
		for (pos=0;pos<count;pos++)
		{
			int tileId=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+cell*m_memLengthMapTile+pos*12+4);
			if (tileId==99)
			{
				int tileCharId = CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+cell*m_memLengthMapTile+pos*12+4+4);
				if (tileCharId==self->tibiaId)
				{
					delete self;
					prevSelfTileNr=cell;
					prevSelfTilePos=pos;
					return cell;
				}
			}
		}
	}
*/
	//there are 8 stages, if above ground each floor 7 to 0 always has the same stage
	//if underground player is always on stage 2 and only stages 0,1,2,3,4 are relevant
	for (int tileNr=tileNrLowest;tileNr<tileNrHighest;tileNr++)
	{
		CTibiaMapTile *maptile = readMapTile(tileNr);
		int pos;
		int count=maptile->count;
		for (pos=0;pos<count;pos++)
		{
			int tileId=maptile->items[pos].itemId;
			if (tileId==99)
			{
				int tileCharId = maptile->items[pos].quantity;
				if (tileCharId==self->tibiaId)
				{
					delete self;
					delete maptile;
					prevSelfTileNr=tileNr;
					prevSelfTilePos=pos;
					return tileNr;
				}
			}
		}
		delete maptile; maptile = NULL;
	}

	//just in case.  Usually only run when changing levels
	for (tileNr=0;tileNr<m_memMaxMapTiles;tileNr++)
	{
		CTibiaMapTile *maptile = readMapTile(tileNr);
		int pos;
		int count=maptile->count;
		for (pos=0;pos<count;pos++)
		{
			int tileId=maptile->items[pos].itemId;
			if (tileId==99)
			{
				int tileCharId = maptile->items[pos].quantity;
				if (tileCharId==self->tibiaId)
				{
					delete self;
					delete maptile;
					prevSelfTileNr=tileNr;
					prevSelfTilePos=pos;
 					return tileNr;
				}
			}
		}
		delete maptile; maptile = NULL;
	}
	delete self;
	return 0;
}

struct point CMemReader::mapAddPointToCoord(point coord,point p2)
{
	coord.x=mod(coord.x+p2.x,18);
	coord.y=mod(coord.y+p2.y,14);
	coord.z-=p2.z;//in tile array -ve is down, in tibia map point system -ve is up
	return coord;
}
struct point CMemReader::mapDiffCoords(point c1,point c2) // c1-c2
{
	// if diff is 16, it could also actually be -2, -20, 34, ... choose closest
	c1.x=mod(c1.x-c2.x,18);
	c1.y=mod(c1.y-c2.y,14);
	c1.z=c1.z-c2.z;
	if (c1.x>9) c1.x -= 18;
	if (c1.y>7) c1.y -= 14;
	return c1;
}
struct point CMemReader::mapShiftReferencePoint(point p, point oldCoord, point newCoord)
{
	point delta = mapDiffCoords(oldCoord,newCoord);
	p.x = p.x + delta.x;
	p.y = p.y + delta.y;
	p.z = p.z - delta.z;//in tile array -ve is down, in tibia map point system -ve is up
	return p;
}
struct point CMemReader::mapGetCellCoord(int cellNr)
{
	int z=cellNr/(14*18);
	int y=(cellNr-z*14*18)/18;
	int x=(cellNr-z*14*18-y*18);
	return point(x,y,z);
}

int CMemReader::mapGetCoordCell(point c)
{
	return c.z*14*18 + mod(c.y,14)*18 + mod(c.x,18);
}

// A simple check to verify that the x,y bounds are within limits and z is in array
int CMemReader::mapIsPointInTileArray(point p, int relToCell)
{
	point cR = mapGetCellCoord(relToCell);
	//in tile array -ve is down, in tibia map point system -ve is up so z is tricky
	return p.x >= -8 && p.x <= 9 && p.y >= -6 && p.y <= 7 && 7-cR.z >= -p.z && cR.z >= p.z;
}

// Looks at change in position and decides whether the point would still contain valid information
int CMemReader::mapIsPointInScope(point p, int relToCell)
{
	int selfCell = mapGetSelfCellNr();
	point p2 = mapShiftReferencePoint(p,mapGetCellCoord(relToCell),mapGetCellCoord(selfCell));
	if(!mapIsPointInTileArray(p,selfCell)) return 0;
	return 1; // improve on this by detecting when underground and entire tile array is not used
}

int CMemReader::mapGetPointItemsCount(point p,int relToCell/*=-1*/)
{
	if (relToCell == -1) relToCell=mapGetSelfCellNr();
	if (!mapIsPointInTileArray(p,relToCell)) return 0;
	int itemCell = mapGetCoordCell(mapAddPointToCoord(mapGetCellCoord(relToCell),p));
	CTibiaMapTileAddress mt = CTibiaMapTileAddress(getMapTileStart(itemCell));
	int addr = mt.count;
	int count = CMemUtil::GetMemIntValue(addr,0);//this address comes from Tibia itself and need not be shifted
	if(!mapIsPointInScope(p,relToCell)) return 0;
	return count;
}

int CMemReader::mapGetPointItemId(point p, int stackNr,int relToCell/*=-1*/)
{
	if (relToCell == -1) relToCell=mapGetSelfCellNr();
	if (!mapIsPointInTileArray(p,relToCell)) return 0;
	int itemCell = mapGetCoordCell(mapAddPointToCoord(mapGetCellCoord(relToCell),p));
	CTibiaMapTileAddress mt = CTibiaMapTileAddress(getMapTileStart(itemCell));
	int addr = mt.items[stackNr].itemId;
	int itemId = CMemUtil::GetMemIntValue(addr,0);//this address comes from Tibia itself and need not be shifted
	if(!mapIsPointInScope(p,relToCell)) return 0;
	return itemId;
}

void CMemReader::mapSetPointItemId(point p, int stackNr, int tileId,int relToCell/*=-1*/)
{
	if (relToCell == -1) relToCell=mapGetSelfCellNr();
	if (!mapIsPointInTileArray(p,relToCell)) return;
	int itemCell = mapGetCoordCell(mapAddPointToCoord(mapGetCellCoord(relToCell),p));
	CTibiaMapTileAddress mt = CTibiaMapTileAddress(getMapTileStart(itemCell));
	int addr = mt.items[stackNr].itemId;
	CMemUtil::SetMemIntValue(addr,tileId,0);//this address comes from Tibia itself and need not be shifted
}

void CMemReader::mapSetPointItemsCount(point p, int count,int relToCell/*=-1*/)
{
	if (relToCell == -1) relToCell=mapGetSelfCellNr();
	if (!mapIsPointInTileArray(p,relToCell)) return;
	int itemCell = mapGetCoordCell(mapAddPointToCoord(mapGetCellCoord(relToCell),p));
	CTibiaMapTileAddress mt = CTibiaMapTileAddress(getMapTileStart(itemCell));
	int addr = mt.count;
	CMemUtil::SetMemIntValue(addr,count,0);//this address comes from Tibia itself and need not be shifted
}

int CMemReader::dereference(int addr, int addBaseAddr/*=1*/)
{
	static int lastAddrReq=0;
	static int lastAddrResp=0;
	if (0 && lastAddrReq==addr)//Bad idea to return outdated address since Tibia now uses many variable memory addresses.
		return lastAddrResp;
	lastAddrResp=CMemUtil::GetMemIntValue(addr,addBaseAddr);
	lastAddrReq=addr;
	return lastAddrResp;
}

int CMemReader::mapGetPointItemExtraInfo(point p, int stackNr, int extraType, int relToCell/*=-1*/)
{
	if (relToCell == -1) relToCell=mapGetSelfCellNr();
	if (!mapIsPointInTileArray(p,relToCell)) return 0;
	int itemCell = mapGetCoordCell(mapAddPointToCoord(mapGetCellCoord(relToCell),p));
	CTibiaMapTileAddress mt = CTibiaMapTileAddress(getMapTileStart(itemCell));
	int extraInfo=0;
	//these address comes from Tibia itself and need not be shifted
	if (extraType == 0) extraInfo=CMemUtil::GetMemIntValue(mt.items[stackNr].itemId,0);
	if (extraType == 1) extraInfo=CMemUtil::GetMemIntValue(mt.items[stackNr].quantity,0);
	if (extraType == 2) extraInfo=CMemUtil::GetMemIntValue(mt.items[stackNr].extra,0);
	if(!mapIsPointInScope(p,relToCell)) return 0;
	return extraInfo;
}

int CMemReader::mapGetPointStackIndex(point p, int stackNr,int relToCell/*=-1*/)// returns the index of an item in a stack on a tibia tile
{
	if (relToCell == -1) relToCell=mapGetSelfCellNr();
	if (!mapIsPointInTileArray(p,relToCell)) return 0;
	int itemCell = mapGetCoordCell(mapAddPointToCoord(mapGetCellCoord(relToCell),p));
	CTibiaMapTileAddress mt = CTibiaMapTileAddress(getMapTileStart(itemCell));
	int addr = mt.stackind[stackNr];
	int data=CMemUtil::GetMemIntValue(addr,0);//this address comes from Tibia itself and need not be shifted
	if(!mapIsPointInScope(p,relToCell)) return 0;
	return data;
}

long CMemReader::getCurrentTm()
{
	return CMemUtil::GetMemIntValue(m_memAddressCurrentTm);
}

void CMemReader::writeEnableRevealCName()
{
	unsigned char *buf=(unsigned char *)malloc(3);
	// replace jump with own jump
	buf[0]=0xEB;
	buf[1]=0x17;
	CMemUtil::SetMemRange(m_memAddressRevealCName1,m_memAddressRevealCName1+2,(char *)buf);
	

	free(buf);

	// INVISIBLE - 0x419450!!!
}

void CMemReader::writeDisableRevealCName()
{
	unsigned char *buf=(unsigned char *)malloc(2);
	buf[0]=0x75;
	buf[1]=0x0A;
	CMemUtil::SetMemRange(m_memAddressRevealCName1,m_memAddressRevealCName1+2,(char *)buf);
	/*
	buf[0]=0x46;
	CMemUtil::SetMemRange(m_memAddressRevealCName2,m_memAddressRevealCName2+1,(char *)buf);
	// extend reveal horizon
	buf[0]=7;
	CMemUtil::SetMemRange(m_memAddressRevealCName3,m_memAddressRevealCName3+1,(char *)buf);
	buf[0]=5;
	CMemUtil::SetMemRange(m_memAddressRevealCName4,m_memAddressRevealCName4+1,(char *)buf);
	*/
	free(buf);
}

void CMemReader::setRemainingTilesToGo(int val)
{
	CTibiaItemProxy itemProxy;
	int offset = itemProxy.getValueForConst("addrOffset");
	CMemUtil::SetMemIntValue(m_memAddressTilesToGo,val);
	CMemUtil::SetMemIntValue(offset + itemProxy.getValueForConst("addrCurrentTileToGo"),val);
}

CTibiaMiniMap * CMemReader::readMiniMap(int nr)
{
	CTibiaItemProxy itemProxy;
	if (nr<0||nr>9) return NULL;
	CTibiaMiniMap *retMap = new CTibiaMiniMap();

	int offset = itemProxy.getValueForConst("addrMiniMapStart")+itemProxy.getValueForConst("lengthMiniMap")*nr;
	
	retMap->x=CMemUtil::GetMemIntValue(offset+0);
	retMap->y=CMemUtil::GetMemIntValue(offset+4);
	retMap->z=CMemUtil::GetMemIntValue(offset+8);
	retMap->segmentPrev=CMemUtil::GetMemIntValue(offset+12);
	retMap->segmentNext=CMemUtil::GetMemIntValue(offset+16);
	retMap->pointsAttached=CMemUtil::GetMemIntValue(offset+131236);

	return retMap;
}

CTibiaMiniMapLabel * CMemReader::readMiniMapLabel(int mapNr, int pointNr)
{
	CTibiaItemProxy itemProxy;
	if (mapNr<0||mapNr>9) return NULL;
	if (pointNr<0) return NULL;
	CTibiaMiniMapLabel *retPoint = new CTibiaMiniMapLabel();

	int mapOffset = itemProxy.getValueForConst("addrMiniMapStart")+itemProxy.getValueForConst("lengthMiniMap")*mapNr;
	if (pointNr>=CMemUtil::GetMemIntValue(mapOffset+131236))
	{
		// point out of range exception
		delete retPoint;
		return NULL;
	}
	int mapPointAddr = CMemUtil::GetMemIntValue(mapOffset+131232);
	int mapPointOffset = mapPointAddr+pointNr*itemProxy.getValueForConst("lengthMiniMapLabel");
	retPoint->x=CMemUtil::GetMemIntValue(mapPointOffset+0,0);
	retPoint->y=CMemUtil::GetMemIntValue(mapPointOffset+4,0);
	retPoint->type=CMemUtil::GetMemIntValue(mapPointOffset+8,0);
	CMemUtil::GetMemRange(mapPointOffset+12,mapPointOffset+12+100,retPoint->desc,0);

	return retPoint;

}

CTibiaMiniMapPoint * CMemReader::readMiniMapPoint(int x, int y, int z)
{
	CTibiaItemProxy itemProxy;
	CTibiaMiniMapPoint* bogusPoint=new CTibiaMiniMapPoint(x,y,z,0,250);
	for (int nr = 0; nr<10;nr++){
		int mapOffset = itemProxy.getValueForConst("addrMiniMapStart")+itemProxy.getValueForConst("lengthMiniMap")*nr+20;
		CTibiaMiniMap *map = readMiniMap(nr);
		if (map->z==z && map->x==(int)(x/256) && map->y==(int)(y/256)){
			CTibiaMiniMapPoint *miniMapPoint=new CTibiaMiniMapPoint(x,y,z,(unsigned char)CMemUtil::GetMemIntValue(mapOffset+(y-map->y*256)+(x-map->x*256)*256)%256,(unsigned char)CMemUtil::GetMemIntValue(mapOffset+(y-map->y*256)+(x-map->x*256)*256+256*256)%256);
			delete map;
			delete bogusPoint;
			return miniMapPoint;
		}
		delete map;
	}
	char filename[1024+20];
	sprintf(filename,"%s%s%03d%03d%02d.map",getenv("USERPROFILE"),"/Application Data/Tibia/Automap/",(int)(x/256),(int)(y/256),z);
	ifstream f;
	f.open(filename,ifstream::in);
	//AfxMessageBox(filename);
	if (f.good())
	{
		unsigned char colour[65536],speed[65536];
		f.read((char*)colour,65536);
		if (f.bad()){
			f.close();
			sprintf(filename,"Fileread Failed: %s%s%03d%03d%02d.map",getenv("USERPROFILE"),"/Application Data/Tibia/Automap/",(int)(x/256),(int)(y/256),z);
			AfxMessageBox(filename);
			return bogusPoint;
		}//read failed
		f.read((char*)speed,65536);
		if (f.bad()){
			f.close();
			sprintf(filename,"Fileread Failed: %s%s%03d%03d%02d.map",getenv("USERPROFILE"),"/Application Data/Tibia/Automap/",(int)(x/256),(int)(y/256),z);
			AfxMessageBox(filename);
			return bogusPoint;
		}//read failed
		f.close();

		CTibiaMiniMapPoint *miniMapPoint=new CTibiaMiniMapPoint(x,y,z,colour[(x%256)*256+y%256],speed[(x%256)*256+y%256]);
		delete bogusPoint;
		return miniMapPoint;
	}

	//point does not exist on minimaps in memory nor in files
	return bogusPoint;
}

void CMemReader::writeMiniMapPoint(int x,int y,int z,int col,int spd){
	//AfxMessageBox("write started");
	CTibiaItemProxy itemProxy;

	int xMap=(int)(x/256),yMap=(int)(y/256),zMap=z;
	for (int nr = 0; nr<10;nr++){
		int mapOffset = itemProxy.getValueForConst("addrMiniMapStart")+itemProxy.getValueForConst("lengthMiniMap")*nr+20;
		CTibiaMiniMap *map = readMiniMap(nr);

		//char buf[1111];
		//sprintf(buf,"cycle maps:(%d,%d,%d),(%d,%d,%d)",xMap,yMap,zMap,map->x,map->y,map->z);
		//AfxMessageBox(buf);

		if (xMap==map->x && yMap==map->y && zMap==map->z){
			//AfxMessageBox("map found and writing");
			int pointOffset=(x%256)*256+y%256;
			char colour[2]="0";
			char speed[2]="0";
			colour[0]=col;
			speed[0]=spd;
			char colour2[1];
			char speed2[1];

			CMemUtil::SetMemRange(mapOffset+pointOffset,mapOffset+pointOffset+1,(char*)colour,1);
			CMemUtil::GetMemRange(mapOffset+pointOffset,mapOffset+pointOffset+1,(char*)colour2,1);
			CMemUtil::SetMemRange(mapOffset+65536+pointOffset,mapOffset+65536+pointOffset+1,(char*)speed,1);
			CMemUtil::GetMemRange(mapOffset+65536+pointOffset,mapOffset+65536+pointOffset+1,(char*)speed2,1);
			//sprintf(buf,"made:(%d,%d),(%x,%x)",colour2[0],speed2[0],mapOffset+pointOffset,mapOffset+65536+pointOffset);
			//AfxMessageBox(buf);
			delete map;
			break;
		}
		delete map;
	}
	//write to file too to make changes permanent(creates file if none but maphack doesn't create one if not needed/blank map)
	char filename[1024+20];
	sprintf(filename,"%s%s%03d%03d%02d.map",getenv("USERPROFILE"),"/Application Data/Tibia/Automap/",(int)(x/256),(int)(y/256),z);
	fstream f;
	f.open(filename,ifstream::in | ifstream::out);
	if (f.good())
	{
		char colour[1];
		char speed[1];
		colour[0]=col;
		speed[0]=spd;
		f.seekg((x%256)*256+y%256);
		f.write(colour,1);
		f.seekg((x%256)*256+y%256+65536);
		f.write(speed,1);
		f.close();
	} else {
		fstream g;
		g.open(filename,ifstream::out);
		if(g.good()){
			char colour[65536],speed[65536];
			for (int i=0;i<65536;i++){
				colour[i]='\x00';
				speed[i]='\xFA';
				if (i==(x%256)*256+y%256){
					colour[i]=col;
					speed[i]=spd;
				}
			}

			g.write(colour,65536);
			g.write(speed,65536);
			g.close();
		}
	}
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD procId;
	GetWindowThreadProcessId(hwnd,&procId);
	if (procId==CMemUtil::m_globalProcessId)
	{
		CWnd *wnd = new CWnd();
		wnd->Attach(hwnd);
		wnd->SetWindowText((char *)lParam);
		wnd->Detach();
		delete wnd;
		return 0;
	}
	return 1;
}

void CMemReader::setMainWindowText(char *text)
{
	EnumWindows(&EnumWindowsProc,(long)text);
}

void CMemReader::setMainTrayText(char *text)
{
	NOTIFYICONDATA data;
	data.cbSize=sizeof(NOTIFYICONDATA);
	
	data.hWnd=AfxGetMainWnd()->GetSafeHwnd();
	data.uID=1;
	sprintf(data.szTip,"%s",text);
	data.uFlags=NIF_TIP;
	Shell_NotifyIcon(NIM_MODIFY,&data);
}

int CMemReader::getPlayerModeAttackType()
{
	CTibiaItemProxy itemProxy;
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrModeAttackType"));
}

int CMemReader::getPlayerModeFollow()
{
	CTibiaItemProxy itemProxy;
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrModeFollow"));
}

int CMemReader::getPlayerModeAttackPlayers()
{
	CTibiaItemProxy itemProxy;
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrModeAttackPlayers")) & 1;
}

int CMemReader::getPlayerModePVP()
{
	CTibiaItemProxy itemProxy;
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrModePVP")) & 1;
}

char * CMemReader::getOpenWindowName()
{
	CTibiaItemProxy itemProxy;
	int ptr2=0;
	static char nameBuf[128];
	memset(nameBuf,0,128);
	ptr2=CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrCurrentWindow"));
	if (ptr2)
	{
		CMemUtil::GetMemRange(ptr2+84,ptr2+84+128,nameBuf,0);//this address comes from Tibia itself and need not be shifted
	}
	return nameBuf;
}

int CMemReader::getConnectionState()
{
	CTibiaItemProxy itemProxy;
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrConnectionState"));
}

int CMemReader::isLoggedIn()
{
	CTibiaItemProxy itemProxy;
	DWORD addr = itemProxy.getValueForConst("addrConnectionState");
	return CMemUtil::GetMemIntValue(addr)==11;
}

void CMemReader::setXRayValues(int v1, int v2)
{
	CTibiaItemProxy itemProxy;
	CMemUtil::SetMemIntValue(itemProxy.getValueForConst("addrXRay1"),v1);
	CMemUtil::SetMemIntValue(itemProxy.getValueForConst("addrXRay2"),v2);
}

int CMemReader::getXRayValue1()
{
	CTibiaItemProxy itemProxy;
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrXRay1"));
}

int CMemReader::getXRayValue2()
{
	CTibiaItemProxy itemProxy;
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrXRay2"));
}

void CMemReader::writeCreatureDeltaXY(int creatureNr, int deltaX, int deltaY)
{
	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+creatureNr*m_memLengthCreature+52,deltaX);
	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+creatureNr*m_memLengthCreature+48,deltaY);
}

int CMemReader::getCreatureDeltaX(int creatureNr)
{
	return CMemUtil::GetMemIntValue(m_memAddressFirstCreature+creatureNr*m_memLengthCreature+52);
}

int CMemReader::getCreatureDeltaY(int creatureNr)
{
	return CMemUtil::GetMemIntValue(m_memAddressFirstCreature+creatureNr*m_memLengthCreature+48);
}


int CMemReader::itemOnTopIndex(int x,int y,int z/*=0*/)//Now uses Tibia's own indexing system found in memory to determine this
{
	CMemReader reader;
	CPackSender sender;
	CTileReader tileReader;
	int pos;

	int stackCount=reader.mapGetPointItemsCount(point(x,y,z));
	int immoveableItems=0;//count the number of items placed in middle of array but aren't top
	int newCount=stackCount;
	for (pos=0;pos<stackCount;pos++)
	{

		int stackInd=reader.mapGetPointStackIndex(point(x,y,z),pos);
		int tileId = reader.mapGetPointItemId(point(x,y,z),pos);
		CTibiaTile *tile=tileReader.getTile(tileId);
		//decrease the index we want to find by 1 if we found a creature or an overhanging object
		if (tile) newCount-=(tileId==99 || tile->moreAlwaysOnTop==3)?1:0;
		if (stackInd==newCount-1 && tileId!=99 || pos==stackCount-1){
			return pos;
		}
	}
	return -1;
}

int CMemReader::isItemOnTop(int x,int y,int *itemArr,int itemArrSize)
{
	CMemReader reader;
	int topPos=itemOnTopIndex(x,y);
	if (topPos==-1) return 0;

	int tileId = reader.mapGetPointItemId(point(x,y,0),topPos);
	for (int i=0;i<itemArrSize;i++)
	{
		if (tileId==itemArr[i]) return tileId;
	}
	return 0;
}

int CMemReader::isItemCovered(int x,int y,int *itemArr,int itemArrSize)
{
	CMemReader reader;

	int topPos=itemOnTopIndex(x,y);
	if (topPos==-1) return 0;

	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	if (topPos>=stackCount) return 0;
	for (int pos=(topPos+1)%stackCount;pos!=topPos;pos=(pos+1)%stackCount)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		for (int i=0;i<itemArrSize;i++)
		{
			if (tileId==itemArr[i]) return tileId;
		}
	}
	return 0;
}

int CMemReader::isItemOnTop(int x,int y,CUIntArray& itemArr)
{
	CMemReader reader;
	int topPos=itemOnTopIndex(x,y);
	if (topPos==-1) return 0;

	int tileId = reader.mapGetPointItemId(point(x,y,0),topPos);
	int size =itemArr.GetSize();
	for (int i=0;i<size;i++)
	{
		if (tileId==itemArr[i]) return tileId;
	}
	return 0;
}

int CMemReader::isItemCovered(int x,int y,CUIntArray& itemArr)
{
	CMemReader reader;

	int topPos=itemOnTopIndex(x,y);
	if (topPos==-1) return 0;

	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	if (topPos>=stackCount) return 0;
	for (int pos=(topPos+1)%stackCount;pos!=topPos;pos=(pos+1)%stackCount)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		int size =itemArr.GetSize();
		for (int i=0;i<size;i++)
		{
			if (tileId==itemArr[i]) return tileId;
		}
	}
	return 0;
}

int CMemReader::isItemOnTop(int x,int y,int itemId)
{
	int itemArr[1]={itemId};
	return isItemOnTop(x,y,itemArr,1);
}

int CMemReader::isItemCovered(int x,int y,int itemId)
{
	int itemArr[1]={itemId};
	return isItemCovered(x,y,itemArr,1);
}

int CMemReader::getItemIndex(int x,int y,int itemId)
{
	CMemReader reader;

	int topPos=itemOnTopIndex(x,y);	
	if (topPos==-1) return -1;

	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));

	for (int pos=topPos;stackCount && pos!=(topPos-1)%stackCount;pos=(pos+1)%stackCount)
	{
		if (itemId == reader.mapGetPointItemId(point(x,y,0),pos)) return pos;
	}
	if (itemId == reader.mapGetPointItemId(point(x,y,0),pos)) return pos;
	return -1;
}

int CMemReader::itemOnTopCode(int x,int y)
{

	CMemReader reader;
	int pos=itemOnTopIndex(x,y);
	if (pos!=-1)
	{
		return reader.mapGetPointItemId(point(x,y,0),pos);
	}
	return 0;
}

int CMemReader::itemSeenOnTopIndex(int x,int y,int z/*=0*/)
{
	CMemReader reader;
	int pos;
	int stackCount=reader.mapGetPointItemsCount(point(x,y,z));
	for (pos=0;pos<stackCount;pos++)
	{
		int stackInd=reader.mapGetPointStackIndex(point(x,y,z),pos);
		if (stackInd==stackCount-1)
			return pos;
	}
	return stackCount-1;
}

int CMemReader::itemSeenOnTopCode(int x,int y)
{
	CMemReader reader;
	int pos=itemSeenOnTopIndex(x,y);
	if (pos!=-1)
	{
		return reader.mapGetPointItemId(point(x,y,0),pos);
	}
	return 0;
}

int CMemReader::itemOnTopQty(int x,int y)
{
	CMemReader reader;
	int pos=itemOnTopIndex(x,y);
	if (pos==-1) return 0;

	int qty=reader.mapGetPointItemExtraInfo(point(x,y,0),itemOnTopIndex(x,y),1);
	return qty?qty:1;
}

int CMemReader::findNextClosedContainer(int afterCont/*=-1*/)
{

	CMemReader reader;
	CPackSender sender;
	CTibiaContainer *container;
	int targetBag;
	for (targetBag=afterCont+1;targetBag<reader.m_memMaxContainers;targetBag++){
		container = reader.readContainer(targetBag);
		if (!container->flagOnOff){
			delete container;
			break;
		}
		delete container;
	}
	
	if (targetBag == reader.m_memMaxContainers){
		targetBag=reader.m_memMaxContainers-1;
		sender.closeContainer(targetBag);

	}
	return targetBag;
}
