// TibiaMapTile.h: interface for the CTibiaMapTile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAMAPTILE_H__AB1816E6_41B9_4651_ABD6_631D17C385BD__INCLUDED_)
#define AFX_TIBIAMAPTILE_H__AB1816E6_41B9_4651_ABD6_631D17C385BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaMapTileItem.h"

class CTibiaMapTile  
{
public:
	CTibiaMapTile();

	int count;
	int stackind[10];
	CTibiaMapTileItem items[10];
	int tileEnd;

};

class CTibiaMapTileAddress
{
public:
	CTibiaMapTileAddress();
	CTibiaMapTileAddress(int initAddr/*=0*/);

	int count;
	int stackind[10];
	CTibiaMapTileItemAddress items[10];
	int tileEnd;

};

#endif // !defined(AFX_TIBIAMAPTILE_H__AB1816E6_41B9_4651_ABD6_631D17C385BD__INCLUDED_)
