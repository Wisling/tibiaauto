#pragma once
#include "tibiaauto_util.h"

struct point
{
public:
	int x;
	int y;
	int z;
	point();
	point(int x, int y, int z);
	bool operator()(const point p1, const point p2) const;
};

