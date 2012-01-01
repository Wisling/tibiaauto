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
}
