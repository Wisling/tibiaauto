// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "Alarm.h"

using namespace std;

class CConfigData
{
public:
	int options;
	CConfigData();
	virtual ~CConfigData();

	list<Alarm> alarmList;
	int actX,actY,actZ,actDirection;
	int runawayX,runawayY,runawayZ;

	bool triggerMessage;
	bool maintainPos;

	char whiteList[100][32];
	char status[2001];
	char modPriorityStr[2];
	int screenshotType;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
