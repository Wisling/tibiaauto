// TibiaMapTileItem.cpp: implementation of the CTibiaMapTileItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "TibiaMapTileItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaMapTileItem::CTibiaMapTileItem()
{
	extra = 0;
	quantity = 0;
	itemId = 0;
	val1 = 0;
	val2 = 0;
	val3 = 0;
	val4 = 0;
	val5 = 0;
	val6 = 0;
}


CTibiaMapTileItemAddress::CTibiaMapTileItemAddress()
{
	CTibiaMapTileItemAddress(0);
}
CTibiaMapTileItemAddress::CTibiaMapTileItemAddress(int initAddr=0)
{
	extra = initAddr;
	quantity = initAddr+4;
	itemId = initAddr+8;
	val1 = initAddr+12;
	val2 = initAddr+16;
	val3 = initAddr+20;
	val4 = initAddr+24;
	val5 = initAddr+28;
	val6 = initAddr+32;
}
