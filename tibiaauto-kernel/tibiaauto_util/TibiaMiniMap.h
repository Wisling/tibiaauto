#pragma once
#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CTibiaMiniMap
{
public:
	CTibiaMiniMap();
	virtual ~CTibiaMiniMap();
	int pointsAttached;
	int segmentPrev;
	int segmentNext;
	int x;
	int y;
	int z;
	// map data is available but is skipped by purpose (too big)
};

