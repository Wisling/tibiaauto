#pragma once

#include "tibiaauto_util.h"
#include "TibiaItem.h"

class TIBIAAUTOUTIL_API CTibiaContainer
{
public:
	int countItemsOfType(int objectId);
	CTibiaContainer();
	virtual ~CTibiaContainer();

	int flagOnOff;
	int objectId;
	int size;
	int number;
	int itemsInside;
	CPtrArray items;
};

