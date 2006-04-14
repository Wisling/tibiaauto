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
	}
};
 
class CTibiaMap  
{
public:
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
	void setPrevPoint(int x,int y,int z,int prevX, int prevY, int prevZ);
	struct point getRandomPoint();
	void clear();
	void setPointAsAvailable(int x,int y,int z);
	int isPointAvailable(int x,int y,int z);	
	CTibiaMap();
	virtual ~CTibiaMap();
	
private:
	void enlarge();
	int prohCount;	
	int prohSize;
	struct point *prohList;
	
	map<point, pointData, point> tibiaMap;		
};

#endif // !defined(AFX_TIBIAMAP_H__D128DE5B_B472_4E61_B805_B3BF714E9099__INCLUDED_)
