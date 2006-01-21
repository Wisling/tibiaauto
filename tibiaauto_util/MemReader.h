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
#include "TibiaMapPoint.h"

class CMemReader  
{
public:	
	CMemReader(); 
	virtual ~CMemReader();		
public:
	void writeDisableRevealCName();
	void writeEnableRevealCName();
	long getCurrentTm();
	int mapGetPointItemExtraInfo(point p,int stackNr,int extraPos);
	void mapSetPointItemsCount(point p,int count);
	void mapSetPointItemId(point p, int stackNr, int tileId);
	int mapGetPointItemId(struct point p,int stackNr);
	int mapGetPointItemsCount(struct point p);
	void writeSelfLightColor(int value);
	void writeSelfLightPower(int value);
	int readSelfLightColor();
	int readSelfLightPower();
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
	CTibiaContainer *readContainer(int containerNr);
	CTibiaItem * readItem(int locationAddress);
	CTibiaCharacter *readSelfCharacter();
	CTibiaCharacter *getCharacterByTibiaId(int tibiaId);

	int m_memAddressFirstContainer;
	int m_memAddressFirstCreature;
	int m_memAddressHP;
	int m_memAddressMana;
	int m_memAddressHPMax;
	int m_memAddressManaMax;
	int m_memAddressCap;
	int m_memAddressExp;
	int m_memAddressLvl;
	int m_memAddressMlvl;
	int m_memAddressSoulPoints;
	int m_memAddressLeftHand;
	int m_memAddressRightHand;
	int m_memAddressSlotArrow;	
	int m_memAddressBattleMin;
	int m_memAddressBattleMax;
	int m_memAddressAttackedCreature;	
	int m_memAddressSkillSword;
	int m_memAddressSkillAxe;
	int m_memAddressSkillShield;
	int m_memAddressSkillClub;
	int m_memAddressSkillDist;
	int m_memAddressSkillFish;
	int m_memAddressSkillSwordPercLeft;
	int m_memAddressSkillAxePercLeft;
	int m_memAddressSkillShieldPercLeft;
	int m_memAddressSkillClubPercLeft;
	int m_memAddressSkillDistPercLeft;
	int m_memAddressSkillFishPercLeft;
	int m_memAddressTradeCountSelf;
	int m_memAddressTradeCountPartner;
	int m_memAddressTradeFirstItemSelf;
	int m_memAddressTradeFirstItemPartner;
	int m_memAddressSelfFlags;
	int m_memAddressGoX;
	int m_memAddressGoY;
	int m_memAddressGoZ;
	int m_memAddressSelfId;
	int m_memAddressMapStart;
	int m_memAddressCurrentTm;
	int m_memAddressRevealCName1;
	int m_memAddressRevealCName2;
	


	
	int m_memLengthContainer;
	int m_memLengthItem;
	int m_memLengthCreature;
	int m_memLengthMapTile;


	int m_memMaxContainers;
	int m_memMaxCreatures;
	int m_memMaxMapTiles;
private:
	int dereference(int addr);
	int mapConvertPointToCell(struct point p);
	struct point mapConvertRelPointToPoint(struct point p);
	struct point mapConvertPointToRelPoint(struct point p);
	struct point mapConvertCellToPoint(int cellNr);
	int mapGetSelfCellNr();
	void setMemConsts()
	{
		int BASE = 0x00400000+16920;		             

		// addresses
		m_memAddressFirstContainer      = BASE + 676888;
		m_memAddressFirstCreature       = BASE + 643216;
		m_memAddressHP                  = BASE + 643112;
		m_memAddressMana                = BASE + 643084;
		m_memAddressLeftHand            = BASE + 676828;
		m_memAddressRightHand           = BASE + 676816;
		m_memAddressSlotArrow           = BASE + 676876;		
		m_memAddressBattleMin           = BASE + 643208;
		m_memAddressBattleMax           = BASE + 643204;
		m_memAddressHPMax               = BASE + 643108;
		m_memAddressManaMax             = BASE + 643080;
		m_memAddressCap                 = BASE + 643072;
		m_memAddressExp                 = BASE + 643104;
		m_memAddressLvl                 = BASE + 643100;
		m_memAddressMlvl                = BASE + 643096;
		m_memAddressAttackedCreature    = BASE + 643068; 
		m_memAddressSoulPoints          = BASE + 643076;
		m_memAddressTradeCountSelf      = BASE + 684792;
		m_memAddressTradeCountPartner   = BASE + 686032;
		m_memAddressTradeFirstItemSelf  = BASE + 684796;
		m_memAddressTradeFirstItemPartner=BASE + 686036;
		m_memAddressSelfFlags           = BASE + 643000;
		m_memAddressGoX                 = BASE + 643184;
		m_memAddressGoY                 = BASE + 643180;
		m_memAddressGoZ                 = BASE + 643176;
		m_memAddressSelfId              = BASE + 643116;		
		m_memAddressMapStart            = BASE + 701024;
		m_memAddressCurrentTm           = BASE + 674684;
		
		m_memAddressSkillFish           = BASE + 643056;
		m_memAddressSkillShield         = m_memAddressSkillFish-4;
		m_memAddressSkillDist           = m_memAddressSkillFish-8;
		m_memAddressSkillAxe            = m_memAddressSkillFish-12;
		m_memAddressSkillSword          = m_memAddressSkillFish-16;
		m_memAddressSkillClub           = m_memAddressSkillFish-20;
				
		m_memAddressSkillFishPercLeft   = m_memAddressSkillFish-28;
		m_memAddressSkillShieldPercLeft = m_memAddressSkillFish-32;
		m_memAddressSkillDistPercLeft   = m_memAddressSkillFish-36;
		m_memAddressSkillAxePercLeft    = m_memAddressSkillFish-40;
		m_memAddressSkillSwordPercLeft  = m_memAddressSkillFish-44;
		m_memAddressSkillClubPercLeft   = m_memAddressSkillFish-48;

		m_memAddressRevealCName1        = 0x4489A8;
		m_memAddressRevealCName2        = 0x4489B6;
			

		// lengths
		m_memLengthContainer=492;		
		m_memLengthItem=12;
		m_memLengthCreature=156;
		m_memLengthMapTile=168;

		// max values
		m_memMaxContainers=10;
		m_memMaxCreatures=150;
		m_memMaxMapTiles=2016;
	};
};

#endif // !defined(AFX_MEMREADER_H__520F2B59_0B82_4188_9DE0_9362EB4062E7__INCLUDED_)
