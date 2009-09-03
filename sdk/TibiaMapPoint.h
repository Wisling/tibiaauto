#ifndef _TIBIA_MAP_POINT_H
#define _TIBIA_MAP_POINT_H


struct point
{
public:
	int x;
	int y;
	int z;
	point();
	point(int x,int y,int z);
	bool operator()(const point p1, const point p2) const;
};


#endif