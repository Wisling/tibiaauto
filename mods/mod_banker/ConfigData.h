// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct Banker {
	char bankerName[64];
	int bankerX;
	int bankerY;
	int bankerZ;
};

class CConfigData {
public:
	CConfigData();
	virtual ~CConfigData();

	int path[15];
	int targetX;
	int targetY;
	int targetZ;
	Banker banker;
	int minimumGoldToBank;
	int cashOnHand;
	char modPriorityStr[2];
	int changeGold;
	int capsLimit;
	int stopByBanker;
	int drawUpTo;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
