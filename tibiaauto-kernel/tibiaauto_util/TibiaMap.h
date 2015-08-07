#pragma once
#include "tibiaauto_util.h"

#include "TibiaMapPoint.h"

using namespace std;

enum MapPointType
{
	MAP_POINT_TYPE_SELF = -2,
	MAP_POINT_TYPE_CLEAR = -1,
	MAP_POINT_TYPE_AVAILABLE = 0,
	MAP_POINT_TYPE_OPEN_HOLE = 101,
	MAP_POINT_TYPE_CLOSED_HOLE = 102,
	MAP_POINT_TYPE_CRATE = 103,
	MAP_POINT_TYPE_ROPE = 201,
	MAP_POINT_TYPE_MAGICROPE = 202,
	MAP_POINT_TYPE_LADDER = 203,
	MAP_POINT_TYPE_STAIRS = 204,
	MAP_POINT_TYPE_DEPOT = 301,
	MAP_POINT_TYPE_TELEPORT = 302,
	MAP_POINT_TYPE_BLOCK = 303,
	MAP_POINT_TYPE_USABLE_TELEPORT = 304,
};

struct pointData
{
public:
	int available;
	MapPointType type;
	int prevX;
	int prevY;
	int prevZ;
	int speed;
	int dist;
	point destPoint;
	int locked;

	pointData()
	{
		clear();
	}

	void clear()
	{
		available = 0;
		type      = MAP_POINT_TYPE_AVAILABLE;
		prevX     = 0;
		prevY     = 0;
		prevZ     = 0;
		dist      = 0;
		speed     = 130;//130 default( is >255/2 and <70*2)
		destPoint = point(0, 0, 0);
		locked    = 0;
	}
};

class TIBIAAUTOUTIL_API CTibiaMap
{
private:
	CTibiaMap();
	virtual ~CTibiaMap();
	CTibiaMap(CTibiaMap const&);
	void operator=(CTibiaMap const&);
public:
	static CTibiaMap& getTibiaMap()
	{
		static CTibiaMap singleton;
		return singleton;
	}
	MapPointType getPointTypeNoProh(int x, int y, int z);
	int isPointAvailableNoProh(int x, int y, int z);
	struct point getPointByNr(int nr);
	int size();
	void removePointAvailable(int x, int y, int z);
	void prohPointClear();
	void prohPointAdd(int x, int y, int z);
	MapPointType getPointType(int x, int y, int z);
	void setPointType(int x, int y, int z, MapPointType type);
	void loadFromDisk(FILE *f);
	void saveToDisk(FILE *f);
	int getPrevPointZ(int x, int y, int z);
	int getPrevPointY(int x, int y, int z);
	int getPrevPointX(int x, int y, int z);
	void clearPrevPoint();
	void clearDistance();
	void clearLocalPrevPoint(int x, int y, int z, int radius);
	void setPrevPoint(int x, int y, int z, int prevX, int prevY, int prevZ);
	void setBestPrevPoint(int x, int y, int z, int prevX, int prevY, int prevZ);
	struct point getRandomPoint();
	void clear();
	void setPointAsAvailable(int x, int y, int z);
	int isPointAvailable(int x, int y, int z);
	point getDestPoint(int x, int y, int z);
	void setDestPoint(int x, int y, int z, int destX, int destY, int destZ);
	int getPointSpeed(int x, int y, int z);
	void setPointSpeed(int x, int y, int z, int speed);
	int getPointDistance(int x, int y, int z);
	void setPointDistance(int x, int y, int z, int dist);
	int calcDistance(int x, int y, int z, int prevX, int prevY, int prevZ);
	void setPointLocked(int x, int y, int z, int locked);
	int isPointLocked(int x, int y, int z);

private:
	void enlarge();
	int prohCount;
	int prohSize;
	struct point *prohList;
	int CTibiaMap::intPoint(point p);
	int isBetterPrevPoint (int x, int y, int z, int prevX, int prevY, int prevZ);
	int isPointLonger(int x, int y, int z, int prevX, int prevY, int prevZ);

	//map<point, pointData, point> tibiaMap;
	CMap<point *, point *, pointData *, pointData *> tibiaMap2;
};

