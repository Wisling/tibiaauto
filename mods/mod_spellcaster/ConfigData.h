#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConfigData  
{
public:
	CConfigData();
	virtual ~CConfigData();

	int mana;
	int manaMana;
	char manaSpell[128];

	int life;
	int customSpell;
	int lifeHp;
	char lifeSpell[128];
	int lifeSpellMana;
	int exuraSpell;
	int exuraHp;
	int exuraSpellMana;
	int granSpell;
	int granHp;
	int granSpellMana;
	int vitaSpell;
	int vitaHp;
	int vitaSpellMana;
	int sioSpell;
	int sioHp;
	int sioSpellMana;
	int poisonSpell;
	int paralysisSpell;
	int minPoisonDmg;

	char healList[100][32];

	int summon;
	int summonLessThan;
	char summonName[128];
	int summonMana;

	int strike;
	int flam;
	int frigo;
	int mort;
	int tera;
	int vis;
	int con;
	int san;
	int hur;
	int manaStrike;
	char defaultStrikeSpell[128];
	int strikeSpellHpMin;

	int aoe;
	int aoeAffect;
	int exori;
	int exoriGran;
	int exoriMas;
	int exevoMasSan;
	int exevoFlamHur;
	int exevoFrigoHur;
	int exevoTeraHur;
	int exevoVisHur;
	int exevoVisLux;
	int exevoGranVisLux;
	int exevoGranMasVis;
	int exevoGranMasFlam;
	int exevoGranMasTera;
	int exevoGranMasFrigo;

	int disableWarning;
	int randomCast;//new

};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
