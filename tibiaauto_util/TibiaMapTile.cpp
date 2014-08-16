// TibiaMapTile.cpp: implementation of the CTibiaMapTile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "TibiaMapTile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaMapTile::CTibiaMapTile()
{
	count=0;
	memset(stackind,0,sizeof(int)*10);
	memset(items,0,sizeof(CTibiaMapTileItem)*10);
	tileEnd = 0;
}

CTibiaMapTileAddress::CTibiaMapTileAddress()
{
	CTibiaMapTileAddress(0);
}

CTibiaMapTileAddress::CTibiaMapTileAddress(int initAddr=0)
{
	int *dummy=(int*)initAddr;
	int offset=0;
	count=int(&dummy[offset++]);
	int i;
	for (i=0;i<10;i++){
		stackind[i]=int(&dummy[offset++]);
	}
	for (i=0;i<10;i++){
		items[i] = CTibiaMapTileItemAddress(int(&dummy[offset]));
		offset += sizeof(CTibiaMapTileItemAddress)/sizeof(int);
	}
	tileEnd = int(&dummy[offset++]);
}
