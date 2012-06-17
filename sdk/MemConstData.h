// MemConstData.h: interface for the CMemConstData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMCONSTDATA_H__90A06B3F_905E_46D1_8243_6ED758E0F845__INCLUDED_)
#define AFX_MEMCONSTDATA_H__90A06B3F_905E_46D1_8243_6ED758E0F845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMemConstData  
{
public:
	CMemConstData();
	
	int m_memAddressXor;
	int m_memAddressPacketCount;
	int m_memAddressVIP;
	int m_memAddressFirstContainer;
	int m_memAddressFirstCreature;
	int m_memAddressHP;
	int m_memAddressMana;
	int m_memAddressHPMax;
	int m_memAddressManaMax;
	int m_memAddressCap;
	int m_memAddressStamina;
	int m_memAddressExp;
	int m_memAddressLvl;
	int m_memAddressMlvl;
	int m_memAddressMlvlPercLeft;
	int m_memAddressSoulPoints;
	int m_memAddressLeftHand;
	int m_memAddressRightHand;
	int m_memAddressSlotArrow;
	int m_memAddressSelfPosX;
	int m_memAddressSelfPosY;
	int m_memAddressSelfPosZ;
	int m_memAddressBattleMin;
	int m_memAddressBattleMax;
	int m_memAddressAttackedCreature;	
	int m_memAddressFollowedCreature;	
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
	int m_memAddressRevealCName1;
	int m_memAddressRevealCName2;
	int m_memAddressRevealCName3;
	int m_memAddressRevealCName4;
	int m_memAddressSelfId;
	int m_memAddressMapStart;
	int m_memAddressMiniMapStart;
	int m_memAddressCurrentTm;
	int m_memAddressCurrentTileToGo;
	int m_memAddressTilesToGo;
	int m_memAddressPathToGo;
	int m_memAddressSkillFist;
	int m_memAddressSkillFistPercLeft;


	
	int m_memLengthContainer;
	int m_memLengthItem;
	int m_memLengthCreature;
	int m_memLengthMapTile;
	int m_memLengthVIP;


	int m_memMaxContainers;
	int m_memMaxCreatures;
	int m_memMaxMapTiles;
};

#endif // !defined(AFX_MEMCONSTDATA_H__90A06B3F_905E_46D1_8243_6ED758E0F845__INCLUDED_)
