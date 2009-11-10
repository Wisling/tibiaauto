// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

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
	int manaLimit;
	int soulPoints;
	int makeTwo;
	int useArrow;
	int maxUse;
	int premium;
	char spell[1024];
	int useBackpack;
	int randomCast;
	int useSpear;

	int makeNow;

	struct runeSpells
	{
		char words[1024];
		int soulPoints;
		int mana;
	} listSpells[15];

};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
