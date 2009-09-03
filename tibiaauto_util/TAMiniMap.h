// TAMiniMap.h: interface for the CTAMiniMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAMINIMAP_H__0536B572_E292_4473_886A_38C9D6C71128__INCLUDED_)
#define AFX_TAMINIMAP_H__0536B572_E292_4473_886A_38C9D6C71128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaMap.h"
#include "TibiaMapPoint.h"
#include "TibiaMiniMapPoint.h"

struct MiniMapSection{
private:
public:
	unsigned char colour[65536];
	unsigned char speed[65536];
	struct point* mapLoc;
	int timeUsed;
	
	MiniMapSection(int x,int y,int z){
		mapLoc=new point(x,y,z);
		timeUsed=0;
	}

	unsigned char setColourSpeed(char* col,char* spd){
		for (int i=0;i<65536;i++){
			colour[i]=col[i];
			speed[i]=spd[i];
		}

	}

	unsigned char getColour(int x,int y){
		return colour[x*256+y];
	}
	unsigned char getSpeed(int x,int y){
		return speed[x*256+y];
	}

};

class CTAMiniMap
{
public:
	CTAMiniMap();
	virtual ~CTAMiniMap();
	CMap<point *,point *,MiniMapSection *,MiniMapSection *> taMiniMap;

	int isPointInMiniMap(int x,int y,int z);
	CTibiaMiniMapPoint* getMiniMapPoint(int x, int y, int z);
	CUIntArray * findPathOnMiniMap(int x,int y,int z,int x2,int y2,int z2);
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

	MiniMapSection* loadFromMemory(int xMap,int yMap,int zMap);
	MiniMapSection* loadFromFile(int xMap,int yMap,int zMap);
};

#endif // !defined(AFX_TAMINIMAP_H__0536B572_E292_4473_886A_38C9D6C71128__INCLUDED_)
