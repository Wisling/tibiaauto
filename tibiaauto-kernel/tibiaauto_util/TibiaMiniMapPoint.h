#pragma once
#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CTibiaMiniMapPoint
{
public:
	CTibiaMiniMapPoint();
	CTibiaMiniMapPoint(int x, int y, int z, int colour, int speed);
	virtual ~CTibiaMiniMapPoint();
	int x;
	int y;
	int z;
	int colour;
	int speed;
};
