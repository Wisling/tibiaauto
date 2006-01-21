// MemConstData.cpp: implementation of the CMemConstData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "MemConstData.h"

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
	int BASE = 0x00400000+16920;		             
	
	// addresses
	m_memAddressFirstContainer      = BASE + 676888;
	m_memAddressFirstCreature       = BASE + 643216;
	m_memAddressHP                  = BASE + 643112;
	m_memAddressMana                = BASE + 643084;
	m_memAddressLeftHand            = BASE + 676828;
	m_memAddressRightHand           = BASE + 676816;
	m_memAddressSlotArrow           = BASE + 676876;
	m_memAddressSelfPosX            = BASE + 687392;
	m_memAddressSelfPosY            = BASE + 687388;
	m_memAddressSelfPosZ            = BASE + 687384;
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
	
	
	// lengths
	m_memLengthContainer=492;		
	m_memLengthItem=12;
	m_memLengthCreature=156;
	
	// max values
	m_memMaxContainers=10;
	m_memMaxCreatures=150;
}
