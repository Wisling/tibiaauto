#pragma once

#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CTibiaCharacter
{
public:
	CTibiaCharacter();
	virtual ~CTibiaCharacter();

	bool initialized;
	int hp;
	int mana;
	int x;
	int y;
	int z;
	unsigned int tibiaId;
	int visible;
	int outfitId;
	int mountId;
	int moving;
	int monsterType;
	int hpPercLeft;
	float cap;
	int stamina;
	int maxHp;
	int maxMana;
	int exp;
	int lvl;
	int mlvl;
	int mlvlPercLeft;
	int soulPoints;
	int skillSword;
	int skillAxe;
	int skillClub;
	int skillShield;
	int skillDist;
	int skillFish;
	int skillFist;
	int skillSwordPercLeft;
	int skillAxePercLeft;
	int skillClubPercLeft;
	int skillShieldPercLeft;
	int skillDistPercLeft;
	int skillFishPercLeft;
	int skillFistPercLeft;
	char name[64];
	int nr;
	int lastAttackTm;
	int lookDirection;
	int colorHead;
	int colorBody;
	int colorLegs;
	int colorFoot;
	int walkSpeed;
	int skulls;
	int shields;
	int warIcon;
	int blocking;
	char voc[3];
};

