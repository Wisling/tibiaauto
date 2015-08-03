#pragma once
#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CTibiaMapTileItem
{
public:
	CTibiaMapTileItem();
	int extra;
	int quantity;
	int itemId;
	int val1;
	int val2;
	int val3;
	int val4;
	int val5;
};

class CTibiaMapTileItemAddress
{
public:
	CTibiaMapTileItemAddress();
	CTibiaMapTileItemAddress(int initAddr /*=0*/);
	int extra;
	int quantity;
	int itemId;
	int val1;
	int val2;
	int val3;
	int val4;
	int val5;
};

