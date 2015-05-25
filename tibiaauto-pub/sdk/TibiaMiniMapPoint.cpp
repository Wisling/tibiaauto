// TibiaMiniMapPoint.cpp: implementation of the CTibiaMiniMapPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaMiniMapPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaMiniMapPoint::CTibiaMiniMapPoint()
{
	x = y = z = colour = speed = 0;
}

CTibiaMiniMapPoint::CTibiaMiniMapPoint(int x, int y, int z, int colour, int speed)
{
	this->x      = x;
	this->y      = y;
	this->z      = z;
	this->colour = colour;
	this->speed  = speed;
}

CTibiaMiniMapPoint::~CTibiaMiniMapPoint()
{
}
