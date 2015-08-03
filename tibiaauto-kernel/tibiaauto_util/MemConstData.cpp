// MemConstData.cpp: implementation of the CMemConstData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "MemConstData.h"

#ifdef _DEBUG
#undef THIS_FILE
#include "TibiaItem.h"
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemConstData::CMemConstData()
{
	int offset = CTibiaItem::getValueForConst("addrOffset");

	m_memAddressPacketCount = offset + CTibiaItem::getValueForConst("addrPacketCount");

	// lengths
	m_memLengthContainer = CTibiaItem::getValueForConst("lengthContainer");
	m_memLengthItem      = CTibiaItem::getValueForConst("lengthItem");
	m_memLengthCreature  = CTibiaItem::getValueForConst("lengthCreature");
	m_memLengthMapTile   = CTibiaItem::getValueForConst("lengthMapTile");
	m_memLengthVIP       = CTibiaItem::getValueForConst("lengthVIP");

	// addresses
	m_memAddressXor                   = offset + CTibiaItem::getValueForConst("addrXor");
	m_memAddressVIP                   = offset + CTibiaItem::getValueForConst("addrVIP");
	m_memAddressFirstContainer        = offset + CTibiaItem::getValueForConst("addrFirstContainer");
	m_memAddressFirstCreature         = offset + CTibiaItem::getValueForConst("addrFirstCreature");
	m_memAddressHP                    = offset + CTibiaItem::getValueForConst("addrHP");
	m_memAddressMana                  = offset + CTibiaItem::getValueForConst("addrMana");
	m_memAddressSlotArrow             = offset + CTibiaItem::getValueForConst("addrSlotArrow"); //arrow, ring, boots, legs, left hand, right hand, armor, backpack, necklace, helmet
	m_memAddressLeftHand              = offset + m_memAddressSlotArrow + m_memLengthItem * 4;
	m_memAddressRightHand             = offset + m_memAddressSlotArrow + m_memLengthItem * 5;
	m_memAddressBackpack              = offset + m_memAddressSlotArrow + m_memLengthItem * 7;
	m_memAddressSelfPosX              = offset + CTibiaItem::getValueForConst("addrSelfPosX");
	m_memAddressSelfPosY              = offset + CTibiaItem::getValueForConst("addrSelfPosY");
	m_memAddressSelfPosZ              = offset + CTibiaItem::getValueForConst("addrSelfPosZ");
	m_memAddressBattleMin             = offset + CTibiaItem::getValueForConst("addrBattleMin");
	m_memAddressBattleMax             = offset + CTibiaItem::getValueForConst("addrBattleMax");
	m_memAddressHPMax                 = offset + CTibiaItem::getValueForConst("addrHPMax");
	m_memAddressManaMax               = offset + CTibiaItem::getValueForConst("addrManaMax");
	m_memAddressCap                   = offset + CTibiaItem::getValueForConst("addrCap");
	m_memAddressStamina               = offset + CTibiaItem::getValueForConst("addrStamina");
	m_memAddressExp                   = offset + CTibiaItem::getValueForConst("addrExp");
	m_memAddressLvl                   = offset + CTibiaItem::getValueForConst("addrLvl");
	m_memAddressMlvl                  = offset + CTibiaItem::getValueForConst("addrMlvl");
	m_memAddressMlvlPercLeft          = offset + CTibiaItem::getValueForConst("addrMlvlPercLeft");
	m_memAddressAttackedCreature      = offset + CTibiaItem::getValueForConst("addrAttackedCreature");
	m_memAddressFollowedCreature      = offset + CTibiaItem::getValueForConst("addrFollowedCreature");
	m_memAddressSoulPoints            = offset + CTibiaItem::getValueForConst("addrSoulPoints");
	m_memAddressTradeCountSelf        = offset + CTibiaItem::getValueForConst("addrTradeCountSelf");
	m_memAddressTradeCountPartner     = offset + CTibiaItem::getValueForConst("addrTradeCountPartner");
	m_memAddressTradeFirstItemSelf    = offset + CTibiaItem::getValueForConst("addrTradeFirstItemSelf");
	m_memAddressTradeFirstItemPartner = offset + CTibiaItem::getValueForConst("addrTradeFirstItemPartner");
	m_memAddressSelfFlags             = offset + CTibiaItem::getValueForConst("addrSelfFlags");
	m_memAddressGoX                   = offset + CTibiaItem::getValueForConst("addrGoX");
	m_memAddressGoY                   = offset + CTibiaItem::getValueForConst("addrGoY");
	m_memAddressGoZ                   = offset + CTibiaItem::getValueForConst("addrGoZ");
	m_memAddressVocation              = offset + CTibiaItem::getValueForConst("addrVocation");

	m_memAddressSkillFish   = offset + CTibiaItem::getValueForConst("addrSkillFish");
	m_memAddressSkillShield = m_memAddressSkillFish - 4;
	m_memAddressSkillDist   = m_memAddressSkillFish - 8;
	m_memAddressSkillAxe    = m_memAddressSkillFish - 12;
	m_memAddressSkillSword  = m_memAddressSkillFish - 16;
	m_memAddressSkillClub   = m_memAddressSkillFish - 20;
	m_memAddressSkillFist   = offset + CTibiaItem::getValueForConst("addrSkillFist");

	m_memAddressSkillFistPercLeft   = offset + CTibiaItem::getValueForConst("addrSkillFistPercLeft");
	m_memAddressSkillClubPercLeft   = m_memAddressSkillFistPercLeft + 4;
	m_memAddressSkillSwordPercLeft  = m_memAddressSkillFistPercLeft + 8;
	m_memAddressSkillAxePercLeft    = m_memAddressSkillFistPercLeft + 12;
	m_memAddressSkillDistPercLeft   = m_memAddressSkillFistPercLeft + 16;
	m_memAddressSkillShieldPercLeft = m_memAddressSkillFistPercLeft + 20;
	m_memAddressSkillFishPercLeft   = m_memAddressSkillFistPercLeft + 24;

	m_memAddressSelfId       = offset + CTibiaItem::getValueForConst("addrSelfId");
	m_memAddressMapStart     = offset + CTibiaItem::getValueForConst("addrMapStart");
	m_memAddressMiniMapStart = offset + CTibiaItem::getValueForConst("addrMiniMapStart");
	m_memAddressCurrentTm    = offset + CTibiaItem::getValueForConst("addrCurrentTm");

	m_memAddressCurrentTileToGo = offset + CTibiaItem::getValueForConst("addrCurrentTileToGo");
	m_memAddressTilesToGo       = offset + CTibiaItem::getValueForConst("addrTilesToGo");
	m_memAddressPathToGo        = offset + CTibiaItem::getValueForConst("addrPathToGo");

	// functions

	m_memAddressRevealCName1 = offset + CTibiaItem::getValueForConst("addrFunRevealCName1");
	m_memAddressRevealCName2 = offset + CTibiaItem::getValueForConst("addrFunRevealCName2");
	m_memAddressRevealCName3 = offset + CTibiaItem::getValueForConst("addrFunRevealCName3");
	m_memAddressRevealCName4 = offset + CTibiaItem::getValueForConst("addrFunRevealCName4");


	// max values
	m_memMaxContainers = CTibiaItem::getValueForConst("maxContainers");
	m_memMaxCreatures  = CTibiaItem::getValueForConst("maxCreatures");
	m_memMaxMapTiles   = CTibiaItem::getValueForConst("maxMapTiles");

	m_offsetCreatureVisible = 164;
}
