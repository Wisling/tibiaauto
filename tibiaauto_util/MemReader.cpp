// MemReader.cpp: implementation of the CMemReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemReader.h"
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemReader::CMemReader() {	
	CMemConstData();
}

CMemReader::~CMemReader() {

}

CTibiaVIPEntry *CMemReader::readVIPEntry(int nr) {	
	CTibiaVIPEntry *vip = new CTibiaVIPEntry();
	if (nr<0||nr>=100) return NULL;
	int offset = nr*m_memLengthVIP + m_memAddressVIP;
	vip->id = CMemUtil::GetMemIntValue(offset);
	vip->status = CMemUtil::GetMemIntValue(offset+34)&1;
	vip->icon = CMemUtil::GetMemIntValue(offset+40);
	CMemUtil::GetMemRange(offset+4,offset+4+29,vip->name);
	return vip;
}

CTibiaContainer *CMemReader::readContainer(int containerNr) {	
	int i;
	
	CTibiaContainer *container = new CTibiaContainer();
	long containerOffset=m_memAddressFirstContainer+containerNr*m_memLengthContainer;		
	container->flagOnOff=CMemUtil::GetMemIntValue(containerOffset+0);	
	container->objectId=CMemUtil::GetMemIntValue(containerOffset+4);
	container->size=CMemUtil::GetMemIntValue(containerOffset+48);
	container->number=containerNr;
	container->itemsInside=CMemUtil::GetMemIntValue(containerOffset+56);

	for (i=0;i<container->itemsInside;i++)
	{		
		CTibiaItem *item = new CTibiaItem();		
		item->objectId = CMemUtil::GetMemIntValue(containerOffset+60+i*m_memLengthItem+0);
		item->quantity = CMemUtil::GetMemIntValue(containerOffset+60+i*m_memLengthItem+4);		
		CTileReader tileReader;
		CTibiaTile *tile=tileReader.getTile(item->objectId);
		if (tile&&!tile->stackable&&item->quantity>1) item->quantity=1;
		item->pos = i;
		container->items.Add(item);
	};

	return container;
}
CTibiaCharacter *CMemReader::readSelfCharacter() {	
	CTibiaCharacter *ch = new CTibiaCharacter();

	ch->hp = CMemUtil::GetMemIntValue(m_memAddressHP);
	ch->mana = CMemUtil::GetMemIntValue(m_memAddressMana);	
	ch->maxHp = CMemUtil::GetMemIntValue(m_memAddressHPMax);
	ch->maxMana = CMemUtil::GetMemIntValue(m_memAddressManaMax);
	// note: since 8.31 capacity has accuracy to 2 decimal places
	ch->cap = CMemUtil::GetMemIntValue(m_memAddressCap)/100.0;
	ch->stamina = CMemUtil::GetMemIntValue(m_memAddressStamina);
	ch->exp = CMemUtil::GetMemIntValue(m_memAddressExp);
	ch->lvl = CMemUtil::GetMemIntValue(m_memAddressLvl);
	ch->mlvl = CMemUtil::GetMemIntValue(m_memAddressMlvl);
	ch->mlvlPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressMlvlPercLeft);
	ch->soulPoints = CMemUtil::GetMemIntValue(m_memAddressSoulPoints);

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
		ch->monsterType=monCh->monsterType;
		ch->hpPercLeft=monCh->hpPercLeft;
		ch->lastAttackTm=monCh->lastAttackTm;
		ch->nr=loggedCharNr;
		ch->warIcon=monCh->warIcon;
		ch->blocking=monCh->blocking;

		delete monCh;
	}

	return ch;
}

CTibiaItem * CMemReader::readItem(int locationAddress)
{
	CTibiaItem *item = new CTibiaItem();

	item->objectId = CMemUtil::GetMemIntValue(locationAddress + 0);
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

	ch->hp=-1;
	ch->mana=-1;
	ch->tibiaId=CMemUtil::GetMemIntValue(offset+0);		
	ch->x=CMemUtil::GetMemIntValue(offset+36);
	ch->y=CMemUtil::GetMemIntValue(offset+40);
	ch->z=CMemUtil::GetMemIntValue(offset+44);
	//ch->horizShift=CMemUtil::GetMemIntValue(offset+48);
	//ch->vertShift=CMemUtil::GetMemIntValue(offset+52);
	//ch->timeFinishMove=CMemUtil::GetMemIntValue(offset+56);
	//ch->timeTillCanMoveAgain=CMemUtil::GetMemIntValue(offset+60);
	//ch->totHorizIncement=CMemUtil::GetMemIntValue(offset+64);
	//ch->totVertIncrement=CMemUtil::GetMemIntValue(offset+68);
	ch->outfitId=CMemUtil::GetMemIntValue(offset+72);
	ch->lookDirection=CMemUtil::GetMemIntValue(offset+80);
	ch->monsterType=CMemUtil::GetMemIntValue(offset+96);		
	ch->colorHead=CMemUtil::GetMemIntValue(offset+100);
	ch->colorBody=CMemUtil::GetMemIntValue(offset+104);
	ch->colorLegs=CMemUtil::GetMemIntValue(offset+108);
	ch->colorFoot=CMemUtil::GetMemIntValue(offset+112);
	//ch->addon=CMemUtil::GetMemIntValue(offset+116);
	//ch->sleeptime=CMemUtil::GetMemIntValue(offset+120);
	//ch->boxcolour=CMemUtil::GetMemIntValue(offset+128);
	ch->lastAttackTm=CMemUtil::GetMemIntValue(offset+132);
	ch->hpPercLeft=CMemUtil::GetMemIntValue(offset+136);
	ch->walkSpeed=CMemUtil::GetMemIntValue(offset+140);
	ch->visible=CMemUtil::GetMemIntValue(offset+144);	
	ch->skulls=CMemUtil::GetMemIntValue(offset+148);
	ch->shields=CMemUtil::GetMemIntValue(offset+152);
	ch->warIcon=CMemUtil::GetMemIntValue(offset+160);
	ch->blocking=CMemUtil::GetMemIntValue(offset+164);
	ch->nr=nr;
	
	CMemUtil::GetMemRange(offset+4,offset+4+31,ch->name);
	
	return ch;
}

int CMemReader::readBattleListMin()
{
	return CMemUtil::GetMemIntValue(m_memAddressBattleMin);
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
	
		
	CMemUtil::GetMemIntValue(processId,m_memAddressSelfId,&selfId);

	for (i=0;i<m_memMaxCreatures;i++)
	{
		long creatureId,visible;
		long offset = m_memAddressFirstCreature+i*m_memLengthCreature;
		CMemUtil::GetMemIntValue(processId,offset+0,&creatureId);		
		CMemUtil::GetMemIntValue(processId,offset+140,&visible);
		if (selfId==creatureId&&visible)
		{
			char buf[33];
			buf[32]==0;
			CMemUtil::GetMemRange(processId,offset+4,offset+4+31,buf);
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
	item->objectId=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemSelf+nr*m_memLengthItem);
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
	item->objectId=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemPartner+nr*m_memLengthItem);
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
	return CMemUtil::GetMemIntValue(m_memAddressSelfFlags);
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
		visible=CMemUtil::GetMemIntValue(offset+140);
		if (selfId==creatureId&&visible)
		{
			return i;
		};
	};
		
	return -1;
}

void CMemReader::writeGotoCoords(int x, int y, int z)
{
	int chNr=getLoggedCharNr();
	CMemUtil::SetMemIntValue(m_memAddressGoX,x);
	CMemUtil::SetMemIntValue(m_memAddressGoY,y);
	CMemUtil::SetMemIntValue(m_memAddressGoZ,z);
	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+76+chNr*m_memLengthCreature,1);//enable to accept gotocoords, (76=is char moving param)
}	

void CMemReader::cancelAttackCoords()
{
	int chNr=getLoggedCharNr();
//	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+76+chNr*m_memLengthCreature,0); will take care of itself
	CMemUtil::SetMemIntValue(m_memAddressGoX,0);
	CMemUtil::SetMemIntValue(m_memAddressGoY,0);
	CMemUtil::SetMemIntValue(m_memAddressGoZ,0);
	setRemainingTilesToGo(0);//if left at > 0, no actions can be performed
}

int CMemReader::readCreatureLightPower(int creatureNr)
{	
	CMemReader reader;
	return CMemUtil::GetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+120);
}

int CMemReader::readCreatureLightColor(int creatureNr)
{	
	CMemReader reader;
	return CMemUtil::GetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+120+4);
}

void CMemReader::writeCreatureLightPower(int creatureNr,int value)
{	
	CMemReader reader;
	CMemUtil::SetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+120,value);		
}

void CMemReader::writeCreatureLightColor(int creatureNr,int value)
{
	CMemReader reader;
	CMemUtil::SetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+120+4,value);		
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



int CMemReader::mapGetSelfCellNr()
{				
	static int prevSelfTileNr=0;
	static int prevSelfTilePos=0;
	int wouldReturn=0;
	CTibiaCharacter *self = readSelfCharacter();
	if (self->z<0 || self->z>20) {
		delete self; 
		return 0;
	}
	
	// check self tile cache
	int c=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+prevSelfTileNr*m_memLengthMapTile);
	int tileId=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+prevSelfTileNr*m_memLengthMapTile+prevSelfTilePos*12+4);
	if (prevSelfTilePos<c&&tileId==99)
	{				
		int tileCharId = CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+prevSelfTileNr*m_memLengthMapTile+prevSelfTilePos*12+4+4);
		if (tileCharId==self->tibiaId)
		{
			delete self;
			return prevSelfTileNr;
			
		} 
	}	
	;

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
		int pos;
		int tmo=dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile;
		int count=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile);		
		for (pos=0;pos<count;pos++)
		{
			int tileId=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile+pos*12+4);
			if (tileId==99)
			{								
				int tileCharId = CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile+pos*12+4+4);
				if (tileCharId==self->tibiaId)
				{					
					delete self;
					prevSelfTileNr=tileNr;
					prevSelfTilePos=pos;
					return tileNr;
				}						
			}			
		}					
	}

	//just in case.  Usually only run when changing levels
	for (tileNr=0;tileNr<m_memMaxMapTiles;tileNr++)
	{
		int pos;
		int count=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile);		
		for (pos=0;pos<count;pos++)
		{
			int tileId=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile+pos*12+4);
			if (tileId==99)
			{								
				int tileCharId = CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile+pos*12+4+4);
				if (tileCharId==self->tibiaId)
				{					
					delete self;
					prevSelfTileNr=tileNr;
					prevSelfTilePos=pos;
 					return tileNr;
				}						
			}			
		}					
	}
	delete self;
	return 0;
}

struct point CMemReader::mapConvertCellToPoint(int cellNr)
{
	int z=cellNr/(14*18);
	int y=(cellNr-z*14*18)/18;
	int x=(cellNr-z*14*18-y*18);

	return point(x,y,z);
}

struct point CMemReader::mapConvertPointToRelPoint(point p)
{	
	return point((p.x<=9)?p.x:(p.x-18),(p.y<=7)?p.y:(p.y-14),p.z);
}

struct point CMemReader::mapConvertRelPointToPoint(point p)
{
	return point((p.x>=0)?p.x:(p.x+18),(p.y>=0)?p.y:(p.y+14),p.z);
}

int CMemReader::mapConvertPointToCell(point p)
{
	return p.z*14*18+p.y*18+p.x;
}

int CMemReader::mapGetPointItemsCount(point p,int relToCell/*=-1*/)
{		
	CTibiaCharacter *self = readSelfCharacter();
	int selfCell=(relToCell==-1?mapGetSelfCellNr():relToCell);
	struct point selfPoint = mapConvertCellToPoint(selfCell);

	//out of xy-range OR above ground and out of z-range OR underground and out of z-range
	if (p.x<-8||p.x>9||p.y<-6||p.y>7||(p.z>7-self->z||p.z<-self->z)&&self->z<=7||(p.z>min(2,15-self->z)||p.z<-2)&&self->z>7)
	{
		delete self;
		return 0;//perceived # of items on square should be 0
		p.x=p.y=p.z=0;
	}
	delete self;
	selfPoint.x+=p.x;
	selfPoint.y+=p.y;
	selfPoint.z-=p.z;//in tile array 0 is lowest
	if (selfPoint.x<0) selfPoint.x+=18;
	if (selfPoint.x>=18) selfPoint.x-=18;
	if (selfPoint.y<0) selfPoint.y+=14;
	if (selfPoint.y>=14) selfPoint.y-=14;
	if (selfPoint.z<0 || selfPoint.z>7) return 0;//should never return here

	int itemCell = mapConvertPointToCell(selfPoint);
	
	int count=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile);	
	return count;
}

int CMemReader::mapGetPointItemId(point p, int stackNr,int relToCell/*=-1*/)
{
	return mapGetPointItemExtraInfo(p,stackNr,0,relToCell);
}

void CMemReader::mapSetPointItemId(point p, int stackNr, int tileId,int relToCell/*=-1*/)
{
	CTibiaCharacter *self = readSelfCharacter();
	int selfCell=(relToCell==-1?mapGetSelfCellNr():relToCell);
	struct point selfPoint = mapConvertCellToPoint(selfCell);

	//out of xy-range OR above ground and out of z-range OR underground and out of z-range
	if (p.x<-8||p.x>9||p.y<-6||p.y>7||(p.z>7-self->z||p.z<-self->z)&&self->z<=7||(p.z>min(2,15-self->z)||p.z<-2)&&self->z>7)
	{
		delete self;
		return;
		p.x=p.y=p.z=0;
	}
	delete self;
	selfPoint.x+=p.x;
	selfPoint.y+=p.y;
	selfPoint.z-=p.z;//in tile array 0 is lowest
	if (selfPoint.x<0) selfPoint.x+=18;
	if (selfPoint.x>=18) selfPoint.x-=18;
	if (selfPoint.y<0) selfPoint.y+=14;
	if (selfPoint.y>=14) selfPoint.y-=14;
	if (selfPoint.z<0 || selfPoint.z>7) return;//should never return here

	int itemCell = mapConvertPointToCell(selfPoint);
	
	CMemUtil::SetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile+4+stackNr*12,tileId);
}

void CMemReader::mapSetPointItemsCount(point p, int count,int relToCell/*=-1*/)
{
	CTibiaCharacter *self = readSelfCharacter();
	int selfCell=(relToCell==-1?mapGetSelfCellNr():relToCell);
	struct point selfPoint = mapConvertCellToPoint(selfCell);

	//out of xy-range OR above ground and out of z-range OR underground and out of z-range
	if (p.x<-8||p.x>9||p.y<-6||p.y>7||(p.z>7-self->z||p.z<-self->z)&&self->z<=7||(p.z>min(2,15-self->z)||p.z<-2)&&self->z>7)
	{
		delete self;
		return;
		p.x=p.y=p.z=0;
	}
	delete self;
	selfPoint.x+=p.x;
	selfPoint.y+=p.y;
	selfPoint.z-=p.z;//in tile array 0 is lowest
	if (selfPoint.x<0) selfPoint.x+=18;
	if (selfPoint.x>=18) selfPoint.x-=18;
	if (selfPoint.y<0) selfPoint.y+=14;
	if (selfPoint.y>=14) selfPoint.y-=14;
	if (selfPoint.z<0 || selfPoint.z>7) return;//should never return here

	int itemCell = mapConvertPointToCell(selfPoint);
	
	CMemUtil::SetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile,count);
}

int CMemReader::dereference(int addr)
{
	static int lastAddrReq=0;
	static int lastAddrResp=0;
	if (lastAddrReq==addr)
		return lastAddrResp;	
	lastAddrResp=CMemUtil::GetMemIntValue(addr);	
	lastAddrReq=addr;		
	return lastAddrResp;
}

int CMemReader::mapGetPointItemExtraInfo(point p, int stackNr, int extraPos,int relToCell/*=-1*/)
{
	CTibiaCharacter *self = readSelfCharacter();
	int selfCell=(relToCell==-1?mapGetSelfCellNr():relToCell);
	struct point selfPoint = mapConvertCellToPoint(selfCell);

	//out of xy-range OR above ground and out of z-range OR underground and out of z-range
	if (p.x<-8||p.x>9||p.y<-6||p.y>7||(p.z>7-self->z||p.z<-self->z)&&self->z<=7||(p.z>min(2,15-self->z)||p.z<-2)&&self->z>7)
	{
		p.x=p.y=p.z=0;
	}
	delete self;
	selfPoint.x+=p.x;
	selfPoint.y+=p.y;
	selfPoint.z-=p.z;//in tile array 0 is lowest
	if (selfPoint.x<0) selfPoint.x+=18;
	if (selfPoint.x>=18) selfPoint.x-=18;
	if (selfPoint.y<0) selfPoint.y+=14;
	if (selfPoint.y>=14) selfPoint.y-=14;
	if (selfPoint.z<0 || selfPoint.z>7) return 0;//should never return here

	int itemCell = mapConvertPointToCell(selfPoint);
	
	int data=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile+4+stackNr*12+extraPos*4);
	return data;
}

int CMemReader::mapGetPointStackIndex(point p, int stackNr,int relToCell/*=-1*/)// returns the index of an item in a stack on a tibia tile
{
	CTibiaCharacter *self = readSelfCharacter();
	int selfCell=(relToCell==-1?mapGetSelfCellNr():relToCell);
	struct point selfPoint = mapConvertCellToPoint(selfCell);

	//out of xy-range OR above ground and out of z-range OR underground and out of z-range
	if (p.x<-8||p.x>9||p.y<-6||p.y>7||(p.z>7-self->z||p.z<-self->z)&&self->z<=7||(p.z>min(2,15-self->z)||p.z<-2)&&self->z>7)
	{
		p.x=p.y=p.z=0;
	}
	delete self;
	selfPoint.x+=p.x;
	selfPoint.y+=p.y;
	selfPoint.z-=p.z;//in tile array 0 is lowest
	if (selfPoint.x<0) selfPoint.x+=18;
	if (selfPoint.x>=18) selfPoint.x-=18;
	if (selfPoint.y<0) selfPoint.y+=14;
	if (selfPoint.y>=14) selfPoint.y-=14;
	if (selfPoint.z<0 || selfPoint.z>7) return 0;//should never return here

	int itemCell = mapConvertPointToCell(selfPoint);
	
	int data=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile+4+10*12+stackNr*4);
	return data;
}

long CMemReader::getCurrentTm()
{
	return CMemUtil::GetMemIntValue(m_memAddressCurrentTm);
}

void CMemReader::writeEnableRevealCName()
{
	unsigned char *buf=(unsigned char *)malloc(3);
	buf[0]=0x90;
	buf[1]=0x90;
	// replace jumps	

	// disable additionall 'z' check
	buf[0]=0x90;
	buf[1]=0x90;
	CMemUtil::SetMemRange(m_memAddressRevealCName1,m_memAddressRevealCName1+2,(char *)buf);	
	

	free(buf);

	// INVISIBLE - 0x419450!!!
}

void CMemReader::writeDisableRevealCName()
{
	unsigned char *buf=(unsigned char *)malloc(2);	
	buf[0]=0x75;
	buf[1]=0x40;	
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
	retPoint->x=CMemUtil::GetMemIntValue(mapPointOffset+0);
	retPoint->y=CMemUtil::GetMemIntValue(mapPointOffset+4);	
	retPoint->type=CMemUtil::GetMemIntValue(mapPointOffset+8);
	CMemUtil::GetMemRange(mapPointOffset+12,mapPointOffset+12+100,retPoint->desc);	

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
	int m_processId=CMemUtil::getGlobalProcessId();
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

			CMemUtil::SetMemRange(m_processId,mapOffset+pointOffset,mapOffset+pointOffset+1,(char*)colour);
			CMemUtil::GetMemRange(m_processId,mapOffset+pointOffset,mapOffset+pointOffset+1,(char*)colour2);
			CMemUtil::SetMemRange(m_processId,mapOffset+65536+pointOffset,mapOffset+65536+pointOffset+1,(char*)speed);
			CMemUtil::GetMemRange(m_processId,mapOffset+65536+pointOffset,mapOffset+65536+pointOffset+1,(char*)speed2);
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
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrModeAttackPlayers"));
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
		CMemUtil::GetMemRange(ptr2+80,ptr2+80+128,nameBuf);		
	}	
	return nameBuf;
}

int CMemReader::getConnectionState()
{
	CTibiaItemProxy itemProxy;
	return CMemUtil::GetMemIntValue(itemProxy.getValueForConst("addrConnectionState"));
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
	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+creatureNr*m_memLengthCreature+48,deltaX);
	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+creatureNr*m_memLengthCreature+52,deltaY);
}

int CMemReader::getCreatureDeltaX(int creatureNr)
{
	return CMemUtil::GetMemIntValue(m_memAddressFirstCreature+creatureNr*m_memLengthCreature+48);
}

int CMemReader::getCreatureDeltaY(int creatureNr)
{
	return CMemUtil::GetMemIntValue(m_memAddressFirstCreature+creatureNr*m_memLengthCreature+52);
}

