// MemConstData.cpp: implementation of the CMemConstData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "MemConstData.h"
#include "TibiaItem.h"

#ifdef _DEBUG
#undef THIS_FILE
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

	m_memAddressFunTibiaPrintText = offset + CTibiaItem::getValueForConst("addrFunTibiaPrintText");
	m_memAddressFunTibiaPlayerNameText = offset + CTibiaItem::getValueForConst("addrFunTibiaPlayerNameText");
	m_memAddressFunTibiaInfoMiddleScreen = offset + CTibiaItem::getValueForConst("addrFunTibiaInfoMiddleScreen");
	m_memAddressFunTibiaIsCreatureVisible = offset + CTibiaItem::getValueForConst("addrFunTibiaIsCreatureVisible");
	m_memAddressFunTibiaEncrypt = offset + CTibiaItem::getValueForConst("addrFunTibiaEncrypt");
	m_memAddressFunTibiaDecrypt = offset + CTibiaItem::getValueForConst("addrFunTibiaDecrypt");
	m_memAddressFunTibiaShouldParseRecv = offset + CTibiaItem::getValueForConst("addrFunTibiaShouldParseRecv");
	m_memAddressFunTibiaInfoMessageBox = offset + CTibiaItem::getValueForConst("addrFunTibiaInfoMessageBox");
	m_memAddressArrayPtrRecvStream = offset + CTibiaItem::getValueForConst("addrArrayPtrRecvStream");
	m_memAddressCallDrawRect = offset + CTibiaItem::getValueForConst("addrCallDrawRect");
	m_memAddressCallDrawBlackRect = offset + CTibiaItem::getValueForConst("addrCallDrawBlackRect");
	m_memAddressCallPrintText01 = offset + CTibiaItem::getValueForConst("addrCallPrintText01");
	m_memAddressCallPrintText02 = offset + CTibiaItem::getValueForConst("addrCallPrintText02");
	m_memAddressCallPrintText03 = offset + CTibiaItem::getValueForConst("addrCallPrintText03");
	m_memAddressCallPrintText04 = offset + CTibiaItem::getValueForConst("addrCallPrintText04");
	m_memAddressCallPrintText05 = offset + CTibiaItem::getValueForConst("addrCallPrintText05");
	m_memAddressCallPlayerNameText01 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText01");
	m_memAddressCallPlayerNameText02 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText02");
	m_memAddressCallPlayerNameText03 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText03");
	m_memAddressCallPlayerNameText04 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText04");
	m_memAddressCallPlayerNameText05 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText05");
	m_memAddressCallPlayerNameText06 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText06");
	m_memAddressCallPlayerNameText07 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText07");
	m_memAddressCallPlayerNameText08 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText08");
	m_memAddressCallPlayerNameText09 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText09");
	m_memAddressCallPlayerNameText10 = offset + CTibiaItem::getValueForConst("addrCallPlayerNameText10");
	m_memAddressCallInfoMiddleScreen01 = offset + CTibiaItem::getValueForConst("addrCallInfoMiddleScreen01");
	m_memAddressCallInfoMiddleScreen02 = offset + CTibiaItem::getValueForConst("addrCallInfoMiddleScreen02");
	m_memAddressCallInfoMiddleScreen03 = offset + CTibiaItem::getValueForConst("addrCallInfoMiddleScreen03");
	m_memAddressCallInfoMiddleScreen04 = offset + CTibiaItem::getValueForConst("addrCallInfoMiddleScreen04");
	m_memAddressCallInfoMessageBox01 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox01");
	m_memAddressCallInfoMessageBox02 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox02");
	m_memAddressCallInfoMessageBox03 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox03");
	m_memAddressCallInfoMessageBox04 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox04");
	m_memAddressCallInfoMessageBox05 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox05");
	m_memAddressCallInfoMessageBox06 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox06");
	m_memAddressCallInfoMessageBox07 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox07");
	m_memAddressCallInfoMessageBox08 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox08");
	m_memAddressCallInfoMessageBox09 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox09");
	m_memAddressCallInfoMessageBox10 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox10");
	m_memAddressCallInfoMessageBox11 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox11");
	m_memAddressCallInfoMessageBox12 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox12");
	m_memAddressCallInfoMessageBox13 = offset + CTibiaItem::getValueForConst("addrCallInfoMessageBox13");
	m_memAddressCallEncrypt01 = offset + CTibiaItem::getValueForConst("addrCallEncrypt01");
	m_memAddressCallDecrypt01 = offset + CTibiaItem::getValueForConst("addrCallDecrypt01");
	m_memAddressCallShouldParseRecv01 = offset + CTibiaItem::getValueForConst("addrCallShouldParseRecv01");

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
