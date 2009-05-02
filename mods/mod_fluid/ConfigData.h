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

	int manaBelow;
	int manaBelowN;
	int manaBelowS;
	int manaBelowG;
	int hpBelowH;
	int hpBelow;
	int hpBelowN;
	int hpBelowS;
	int hpBelowG;
	int hpBelowU;
	int drinkMana;
	int drinkManaN;
	int drinkManaS;
	int drinkManaG;
	int drinkHpH;
	int drinkHp;
	int drinkHpN;
	int drinkHpS;
	int drinkHpG;
	int drinkHpU;
	int sleep;
	int hotkeyHp;
	int hotkeyMana;
	int dropEmpty;
	int customItem1Below;
	int customItem1Item;
	int customItem1Use;
	int customItem2Below;
	int customItem2Item;
	int customItem2Use;
	int randomCast;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
