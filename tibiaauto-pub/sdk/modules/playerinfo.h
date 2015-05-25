struct skillstruct {
	unsigned long start;
	unsigned long current;
	unsigned long lvl;
	unsigned long percLeft;
	float percPerHour;
};

struct spellstruct {
	time_t start;
	time_t remaining;
	long lasts;
	long warning;
};

struct mod_playerinfo {
	time_t timeStart;
	time_t timeCurrent;
	unsigned short hp;
	unsigned short maxHp;
	unsigned short mana;
	unsigned short maxMana;
	unsigned short soulPoints;
	unsigned short maxSoulPoints;
	float capacity;
	unsigned short maxCapacity;
	unsigned short stamina;
	unsigned long expStart;
	unsigned long expCurrent;
	unsigned long expLvl;
	unsigned long expPerHour;
	unsigned long expLeft;
	char voc[3];
	skillstruct skill[9];
	spellstruct spell[4];
};

#define SKILL_LEVEL                     0
#define SKILL_MAGICLEVEL        1
#define SKILL_FIST                      2
#define SKILL_CLUB                      3
#define SKILL_SWORD                     4
#define SKILL_AXE                       5
#define SKILL_DIST                      6
#define SKILL_SHIELD            7
#define SKILL_FISH                      8

#define SPELL_INVISIBLE         0
#define SPELL_HASTE                     1
#define SPELL_STRONGHASTE       2
#define SPELL_SHIELD            3

typedef void* (WINAPI * GPIPROC)(void);
