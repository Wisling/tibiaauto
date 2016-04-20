#pragma once

#include "tibiaauto_util.h"
#include "TibiaMapPoint.h"
class CTibiaMap;
class CTibiaMiniMapPoint;

struct MiniMapSection {
private:
public:
	unsigned char colour[65536];
	unsigned char speed[65536];
	struct point* mapLoc;
	time_t timeUsed;

	MiniMapSection(int x, int y, int z)
	{
		mapLoc   = new point(x, y, z);
		timeUsed = 0;
	}

	~MiniMapSection()
	{
		delete mapLoc;
	}

	unsigned char setColourSpeed(char* col, char* spd)
	{
		for (int i = 0; i < 65536; i++)
		{
			colour[i] = col[i];
			speed[i]  = spd[i];
		}
	}

	unsigned char getColour(int x, int y)
	{
		return colour[x * 256 + y];
	}

	unsigned char getSpeed(int x, int y)
	{
		return speed[x * 256 + y];
	}
};

class TIBIAAUTOUTIL_API CTAMiniMap
{
private:
	CTAMiniMap();
	virtual ~CTAMiniMap();
	CTAMiniMap(CTAMiniMap const&);
public:
	static CTAMiniMap& getTAMiniMap()
	{
		static CTAMiniMap singleton;
		return singleton;
	}
	CMap<point *, point *, MiniMapSection *, MiniMapSection *> taMiniMap;

	int isPointInMiniMap(int x, int y, int z);
	CTibiaMiniMapPoint* getMiniMapPoint(int x, int y, int z);
	void setMiniMapPoint(int x, int y, int z, int col, int spd);
	CUIntArray * findPathOnMiniMap(int x, int y, int z, int x2, int y2, int z2);
	void findPathStop();
	bool isFindPathStopped();
	int getCurrentDistance();
	bool isUpPoint(CTibiaMiniMapPoint* lower);
	bool isDownPoint(CTibiaMiniMapPoint* upper);
	void unloadMiniMaps();

	// map data is available but is skipped by purpose (too big)
private:
	int maxMaps;//10=about 1.4 MB
	int mapCount;

	MiniMapSection* loadFromMemory(int xMap, int yMap, int zMap);
	MiniMapSection* loadFromFile(int xMap, int yMap, int zMap);
};
