// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_LINE_LEN		256

class CConfigData  
{
public:
	CConfigData();
	virtual ~CConfigData();

	int player;
	char player1[MAX_LINE_LEN];
	char player2[MAX_LINE_LEN];
	int monster;
	char monster1[MAX_LINE_LEN];
	char monster2[MAX_LINE_LEN];
	int uniqueMonsterNames;
	int self;
	char self1[MAX_LINE_LEN];
	char self2[MAX_LINE_LEN];
	char selfTray[MAX_LINE_LEN];
	char selfWindow[MAX_LINE_LEN];
	int allFloorInfo;
	int addRequest;
	int collectStats;
	//int showCreaturesInArea;
	int rangeXY;
	int rangeZ;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
