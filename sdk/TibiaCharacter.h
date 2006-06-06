// TibiaCharacter.h: interface for the CTibiaCharacter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIACHARACTER_H__AFC25758_9E59_4EA5_92A9_83E2907E04DD__INCLUDED_)
#define AFX_TIBIACHARACTER_H__AFC25758_9E59_4EA5_92A9_83E2907E04DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaCharacter  
{
public:
	CTibiaCharacter();
	virtual ~CTibiaCharacter();
	
	int hp;
	int mana;
	int x;
	int y;
	int z;
	int tibiaId;
	int visible;
	int outfitId;
	int monsterType;
	int hpPercLeft;
	int cap;
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

};

#endif // !defined(AFX_TIBIACHARACTER_H__AFC25758_9E59_4EA5_92A9_83E2907E04DD__INCLUDED_)
