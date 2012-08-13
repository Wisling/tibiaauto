// MemConstData.cpp: implementation of the CMemConstData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "MemConstData.h"
#include "TibiaItemProxy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemConstData::CMemConstData()
{
	CTibiaItemProxy itemProxy;	
	
	int offset = itemProxy.getValueForConst("addrOffset");

	m_memAddressPacketCount			= offset + itemProxy.getValueForConst("addrPacketCount");

	// addresses	
	m_memAddressXor					= offset + itemProxy.getValueForConst("addrXor");
	m_memAddressVIP					= offset + itemProxy.getValueForConst("addrVIP");	
	m_memAddressFirstContainer      = offset + itemProxy.getValueForConst("addrFirstContainer");
	m_memAddressFirstCreature       = offset + itemProxy.getValueForConst("addrFirstCreature");
	m_memAddressHP                  = offset + itemProxy.getValueForConst("addrHP");
	m_memAddressMana                = offset + itemProxy.getValueForConst("addrMana");
	m_memAddressLeftHand            = offset + itemProxy.getValueForConst("addrLeftHand");
	m_memAddressRightHand           = offset + itemProxy.getValueForConst("addrRightHand");
	m_memAddressSlotArrow           = offset + itemProxy.getValueForConst("addrSlotArrow");
	m_memAddressSelfPosX            = offset + itemProxy.getValueForConst("addrSelfPosX");
	m_memAddressSelfPosY            = offset + itemProxy.getValueForConst("addrSelfPosY");
	m_memAddressSelfPosZ            = offset + itemProxy.getValueForConst("addrSelfPosZ");
	m_memAddressBattleMin           = offset + itemProxy.getValueForConst("addrBattleMin");
	m_memAddressBattleMax           = offset + itemProxy.getValueForConst("addrBattleMax");
	m_memAddressHPMax               = offset + itemProxy.getValueForConst("addrHPMax");
	m_memAddressManaMax             = offset + itemProxy.getValueForConst("addrManaMax");
	m_memAddressCap                 = offset + itemProxy.getValueForConst("addrCap");
	m_memAddressStamina             = offset + itemProxy.getValueForConst("addrStamina");
	m_memAddressExp                 = offset + itemProxy.getValueForConst("addrExp");
	m_memAddressLvl                 = offset + itemProxy.getValueForConst("addrLvl");
	m_memAddressMlvl                = offset + itemProxy.getValueForConst("addrMlvl");
	m_memAddressMlvlPercLeft        = offset + itemProxy.getValueForConst("addrMlvlPercLeft");
	m_memAddressAttackedCreature    = offset + itemProxy.getValueForConst("addrAttackedCreature"); 
	m_memAddressFollowedCreature    = offset + itemProxy.getValueForConst("addrFollowedCreature"); 
	m_memAddressSoulPoints          = offset + itemProxy.getValueForConst("addrSoulPoints");
	m_memAddressTradeCountSelf      = offset + itemProxy.getValueForConst("addrTradeCountSelf");
	m_memAddressTradeCountPartner   = offset + itemProxy.getValueForConst("addrTradeCountPartner");
	m_memAddressTradeFirstItemSelf  = offset + itemProxy.getValueForConst("addrTradeFirstItemSelf");
	m_memAddressTradeFirstItemPartner=offset + itemProxy.getValueForConst("addrTradeFirstItemPartner");
	m_memAddressSelfFlags           = offset + itemProxy.getValueForConst("addrSelfFlags");
	m_memAddressGoX                 = offset + itemProxy.getValueForConst("addrGoX");
	m_memAddressGoY                 = offset + itemProxy.getValueForConst("addrGoY");
	m_memAddressGoZ                 = offset + itemProxy.getValueForConst("addrGoZ");
	
	m_memAddressSkillFish           = offset + itemProxy.getValueForConst("addrSkillFish");
	m_memAddressSkillShield         = m_memAddressSkillFish-4;
	m_memAddressSkillDist           = m_memAddressSkillFish-8;
	m_memAddressSkillAxe            = m_memAddressSkillFish-12;
	m_memAddressSkillSword          = m_memAddressSkillFish-16;
	m_memAddressSkillClub           = m_memAddressSkillFish-20;
	m_memAddressSkillFist			= offset + itemProxy.getValueForConst("addrSkillFist");
				
	m_memAddressSkillFistPercLeft   = offset + itemProxy.getValueForConst("addrSkillFistPercLeft");
	m_memAddressSkillClubPercLeft   = m_memAddressSkillFistPercLeft+4;
	m_memAddressSkillSwordPercLeft  = m_memAddressSkillFistPercLeft+8;
	m_memAddressSkillAxePercLeft    = m_memAddressSkillFistPercLeft+12;
	m_memAddressSkillDistPercLeft   = m_memAddressSkillFistPercLeft+16;
	m_memAddressSkillShieldPercLeft = m_memAddressSkillFistPercLeft+20;
	m_memAddressSkillFishPercLeft   = m_memAddressSkillFistPercLeft+24;
	
	m_memAddressSelfId              = offset + itemProxy.getValueForConst("addrSelfId");
	m_memAddressMapStart            = offset + itemProxy.getValueForConst("addrMapStart");
	m_memAddressMiniMapStart            = offset + itemProxy.getValueForConst("addrMiniMapStart");
	m_memAddressCurrentTm           = offset + itemProxy.getValueForConst("addrCurrentTm");

	m_memAddressCurrentTileToGo		= offset + itemProxy.getValueForConst("addrCurrentTileToGo");
	m_memAddressTilesToGo		= offset + itemProxy.getValueForConst("addrTilesToGo");
	m_memAddressPathToGo		= offset + itemProxy.getValueForConst("addrPathToGo");

	// functions

	m_memAddressRevealCName1        = offset + itemProxy.getValueForConst("addrFunRevealCName1");
	m_memAddressRevealCName2        = offset + itemProxy.getValueForConst("addrFunRevealCName2");
	m_memAddressRevealCName3        = offset + itemProxy.getValueForConst("addrFunRevealCName3");
	m_memAddressRevealCName4        = offset + itemProxy.getValueForConst("addrFunRevealCName4");
	
	
	// lengths
	m_memLengthContainer=itemProxy.getValueForConst("lengthContainer");
	m_memLengthItem=itemProxy.getValueForConst("lengthItem");
	m_memLengthCreature=itemProxy.getValueForConst("lengthCreature");
	m_memLengthMapTile=itemProxy.getValueForConst("lengthMapTile");
	m_memLengthVIP=itemProxy.getValueForConst("lengthVIP");
	
	// max values
	m_memMaxContainers=itemProxy.getValueForConst("maxContainers");
	m_memMaxCreatures=itemProxy.getValueForConst("maxCreatures");
	m_memMaxMapTiles=itemProxy.getValueForConst("maxMapTiles");
}
