// Addresses.cpp: implementation of the Addresses class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_addressfinder.h"
#include "Addresses.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Addresses::Addresses()
{
	flagsAddress                 = 0;
	fistPercentLeftAddress       = 0;
	fistSkillAddress             = 0;
	fishSkillAddress             = 0;
	attackedCreatureAddress      = 0;
	capacityAddress              = 0;
	staminaAddress               = 0;
	soulPointsAddress            = 0;
	manaMaxAddress               = 0;
	manaAddress                  = 0;
	magicLevelPercentLeftAddress = 0;
	magicLevelAddress            = 0;
	levelAddress                 = 0;
	expAddress                   = 0;
	HPMaxAddress                 = 0;
	HPAddress                    = 0;
	selfIDAddress                = 0;
	goToTileAddress              = 0;
	tilesToGoAddress             = 0;
	goZAddress                   = 0;
	goYAddress                   = 0;
	goXAddress                   = 0;
	battleListMaxAddress         = 0;
	battleListMinAddress         = 0;
}

Addresses::~Addresses()
{
}
