#pragma once
#include "tibiaauto_util.h"

#include "TibiaMapTileItem.h"

class TIBIAAUTOUTIL_API CTibiaMapTile
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
	CTibiaMapTileAddress(int initAddr /*=0*/);

	int count;
	int stackind[10];
	CTibiaMapTileItemAddress items[10];
	int tileEnd;
};

