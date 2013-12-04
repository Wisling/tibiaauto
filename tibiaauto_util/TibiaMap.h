// TibiaMap.h: interface for the CTibiaMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAMAP_H__D128DE5B_B472_4E61_B805_B3BF714E9099__INCLUDED_)
#define AFX_TIBIAMAP_H__D128DE5B_B472_4E61_B805_B3BF714E9099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "TibiaMapPoint.h"

using namespace std;



struct pointData
{
public:
	int available;
	int updown;
	int prevX;
	int prevY;
	int prevZ;
	int speed;
	int dist;
	point destPoint;

	pointData()
	{
		clear();
	}
	void clear()
	{
		available=0;
		updown=0;
		prevX=0;
		prevY=0;
		prevZ=0;
		dist=0;
		speed=130;//130 default( is >255/2 and <70*2)
		destPoint=point(0,0,0);
	}
};
 
class CTibiaMap
{
public:
	int getPointUpDownNoProh(int x,int y, int z);
	int isPointAvailableNoProh(int x, int y,int z);
	struct point getPointByNr(int nr);
	int size();
	void removePointAvailable(int x,int y,int z);
	void prohPointClear();
	void prohPointAdd(int x, int y, int z);
	int getPointUpDown(int x,int y,int z);
	void setPointUpDown(int x,int y,int z,int updown);
	void loadFromDisk(FILE *f);
	void saveToDisk(FILE *f);
	int getPrevPointZ(int x,int y, int z);
	int getPrevPointY(int x,int y, int z);
	int getPrevPointX(int x,int y, int z);
	void clearPrevPoint();
	void clearDistance();
	void clearLocalPrevPoint(int x,int y,int z,int radius);
	void setPrevPoint(int x,int y,int z,int prevX, int prevY, int prevZ);
	void setBestPrevPoint(int x, int y, int z, int prevX, int prevY, int prevZ);
	struct point getRandomPoint();
	void clear();
	void setPointAsAvailable(int x,int y,int z);
	int isPointAvailable(int x,int y,int z);
	point getDestPoint(int x,int y, int z);
	void setDestPoint(int x,int y,int z,int destX, int destY, int destZ);
	int getPointSpeed(int x,int y, int z);
	void setPointSpeed(int x, int y, int z,int speed);
	int getPointDistance(int x, int y, int z);
	void setPointDistance(int x, int y, int z, int dist);
	int calcDistance(int x, int y, int z, int prevX, int prevY, int prevZ);
	CTibiaMap();
	virtual ~CTibiaMap();
	
private:
	void enlarge();
	int prohCount;
	int prohSize;
	struct point *prohList;
	int CTibiaMap::intPoint(point p);
	int isBetterPrevPoint (int x, int y, int z, int prevX, int prevY, int prevZ);
	int isPointLonger(int x,int y,int z,int prevX, int prevY, int prevZ);

	//map<point, pointData, point> tibiaMap;
	CMap<point *,point *,pointData *,pointData *> tibiaMap2;
};

#endif // !defined(AFX_TIBIAMAP_H__D128DE5B_B472_4E61_B805_B3BF714E9099__INCLUDED_)
