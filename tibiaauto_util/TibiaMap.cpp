// TibiaMap.cpp: implementation of the CTibiaMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaMap.h"
#include "TibiaMapPoint.h"
#include "HashMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


using namespace std;

//////////////////////////////////////////////////////////////////////

static point *pointCache=NULL;
static int pointCacheSize=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CTibiaMap::CTibiaMap()
{		
	prohCount=0;
	prohSize=500;
	prohList=(struct point *)malloc(sizeof(struct point)*prohSize);
}

CTibiaMap::~CTibiaMap()
{

}



int CTibiaMap::isPointAvailable(int x, int y,int z)
{			
	if (!tibiaMap[point(x,y,z)].available)
		return 0;

	// do the prohpoint check
	int prohNr;
	for (prohNr=0;prohNr<prohCount;prohNr++)
	{
		if (prohList[prohNr].x==x&&prohList[prohNr].y==y&&prohList[prohNr].z==z)
			return 0;
	}	
	return 1;
}

void CTibiaMap::setPointAsAvailable(int x, int y,int z)
{
	tibiaMap[point(x,y,z)].available=1;	
}

void CTibiaMap::clear()
{
	pointCacheSize=-1;
	tibiaMap.clear();
}

void CTibiaMap::enlarge()
{	
	
}

struct point CTibiaMap::getRandomPoint()
{
	int s=size();
	if (!s) s=1;
	int nr = rand()%s;
	return getPointByNr(nr);	
}

void CTibiaMap::setPrevPoint(int x, int y, int z,int prevX, int prevY,int prevZ)
{
	tibiaMap[point(x,y,z)].prevX=prevX;
	tibiaMap[point(x,y,z)].prevY=prevY;
	tibiaMap[point(x,y,z)].prevZ=prevZ;		
}

void CTibiaMap::clearPrevPoint()
{	
	std::map<point, pointData, point>::iterator itr;
	for (itr = tibiaMap.begin(); itr != tibiaMap.end(); itr++)
	{		
		itr->second.prevX=0;
		itr->second.prevY=0;
		itr->second.prevZ=0;
	}		
}



int CTibiaMap::getPrevPointX(int x, int y, int z)
{	
	return tibiaMap[point(x,y,z)].prevX;
}

int CTibiaMap::getPrevPointY(int x, int y, int z)
{
	return tibiaMap[point(x,y,z)].prevY;	
}

int CTibiaMap::getPrevPointZ(int x, int y, int z)
{
	return tibiaMap[point(x,y,z)].prevZ;	
}

void CTibiaMap::saveToDisk(FILE *f)
{

}

void CTibiaMap::loadFromDisk(FILE *f)
{
	
}

void CTibiaMap::setPointUpDown(int x, int y, int z, int updown)
{	
	tibiaMap[point(x,y,z)].updown=updown;
}

int CTibiaMap::getPointUpDown(int x, int y, int z)
{	
	if (isPointAvailable(x,y,z))
	{
		return tibiaMap[point(x,y,z)].updown;	
	} else {
		return 0;
	}
}

void CTibiaMap::prohPointAdd(int x, int y, int z)
{
	if (prohCount>prohSize-5)
		return;
	prohList[prohCount].x=x;
	prohList[prohCount].y=y;
	prohList[prohCount].z=z;
	prohCount++;
}

void CTibiaMap::prohPointClear()
{
	prohCount=0;
}

void CTibiaMap::removePointAvailable(int x, int y, int z)
{	
	tibiaMap[point(x,y,z)].available=0;
}

int CTibiaMap::size()
{
	return tibiaMap.size();
}


struct point CTibiaMap::getPointByNr(int nr)
{		

	if (tibiaMap.size()!=pointCacheSize)
	{
		delete []pointCache;
		pointCache=0;
	}

	
	
	if (pointCache==NULL)
	{
		// refresh cache
		pointCache=new point[tibiaMap.size()];
		pointCacheSize=tibiaMap.size();
		std::map<point, pointData, point>::iterator itr;
		int pos=0;		
		for (itr=tibiaMap.begin();itr != tibiaMap.end(); itr++)
		{
			pointCache[pos++]=itr->first;
		}
	}
	if (nr<pointCacheSize)
	{
		return pointCache[nr];
	} else {
		// this is patological
		return point(0,0,0);
	}
}


int CTibiaMap::isPointAvailableNoProh(int x, int y, int z)
{
	return tibiaMap[point(x,y,z)].available;
}

int CTibiaMap::getPointUpDownNoProh(int x, int y, int z)
{
	if (isPointAvailableNoProh(x,y,z))
	{
		return tibiaMap[point(x,y,z)].updown;	
	} else {
		return 0;
	}
}
