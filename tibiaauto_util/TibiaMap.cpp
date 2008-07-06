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

template<> 
UINT AFXAPI HashKey<point*> (point* key)
{	
	return key->x*191+key->y*257+key->z*317;
}


typedef point* LPpoint;
template<>
BOOL AFXAPI CompareElements<LPpoint, LPpoint> 
     (const LPpoint *v1, const LPpoint *v2)
{	
	
	if (!v1||!v2) return false;
	point *v1d=*v1;
	point *v2d=*v2;
	if (!v1d||!v2d) return false;	
	if (v1d&&v2d&&v1d->x==v2d->x&&v1d->y==v2d->y&&v1d->z==v2d->z) return true;
	return false;    	
}

CTibiaMap::CTibiaMap()
{		
	prohCount=0;
	prohSize=500;
	prohList=(struct point *)malloc(sizeof(struct point)*prohSize);
	tibiaMap2.InitHashTable(7919*17+1);	
}

CTibiaMap::~CTibiaMap()
{
	free(prohList);	
}



int CTibiaMap::isPointAvailable(int x, int y,int z)
{		
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;	
	if (tibiaMap2.Lookup(&p,pd))
	{
		if (!pd->available) return false;	
	

		// do the prohpoint checkf
		int prohNr;
		for (prohNr=0;prohNr<prohCount;prohNr++)
		{
			if (prohList[prohNr].x==x&&prohList[prohNr].y==y&&prohList[prohNr].z==z)
				return 0;
		}	
		return 1;
	}
	return 0;
}

void CTibiaMap::setPointAsAvailable(int x, int y,int z)
{	
	if (!isPointAvailableNoProh(x,y,z))
	{
		struct point *p=new point(x,y,z);
		struct pointData *pd=new pointData();
		pd->available=1;	
		tibiaMap2.SetAt(p,pd);		
	}
}

void CTibiaMap::clear()
{	
	pointCacheSize=-1;	
	tibiaMap2.RemoveAll();	
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
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		pd->prevX=prevX;
		pd->prevY=prevY;
		pd->prevZ=prevZ;
	}			
}

typedef CMap<point *,point *,pointData *,pointData *> CMyMap;

void CTibiaMap::clearPrevPoint()
{		
	POSITION pos = tibiaMap2.GetStartPosition();	
	while (pos != NULL)
	{
		point *p=NULL;
		pointData *pd=NULL;
		tibiaMap2.GetNextAssoc( pos, p, pd );
		pd->prevX=0;
		pd->prevY=0;
		pd->prevZ=0;
	}		
}



int CTibiaMap::getPrevPointX(int x, int y, int z)
{	
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		return pd->prevX;		
	}
	return 0;
}

int CTibiaMap::getPrevPointY(int x, int y, int z)
{
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		return pd->prevY;
	}
	return 0;
}

int CTibiaMap::getPrevPointZ(int x, int y, int z)
{
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		return pd->prevZ;
	}
	return 0;
}

void CTibiaMap::saveToDisk(FILE *f)
{

}

void CTibiaMap::loadFromDisk(FILE *f)
{
	
}

void CTibiaMap::setPointUpDown(int x, int y, int z, int updown)
{	
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		pd->updown=updown;		
	}
}

int CTibiaMap::getPointUpDown(int x, int y, int z)
{	
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		if (isPointAvailable(x,y,z)) return pd->updown;		
	}
	return 0;
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
	struct point p=point(x,y,z);
	tibiaMap2.RemoveKey(&p);	
}

int CTibiaMap::size()
{
	return tibiaMap2.GetCount();
}


struct point CTibiaMap::getPointByNr(int nr)
{		

	if (tibiaMap2.GetCount()!=pointCacheSize)
	{
		delete []pointCache;
		pointCache=NULL;
		pointCacheSize=-1;
	}
	
	
	if (pointCache==NULL)
	{
		// refresh cache
		int pointCachePos=0;
		pointCache=new point[tibiaMap2.GetCount()];
		pointCacheSize=tibiaMap2.GetCount();
		
		POSITION pos = tibiaMap2.GetStartPosition();	
		while (pos != NULL)
		{
			point *p=NULL;
			pointData *pd=NULL;
			tibiaMap2.GetNextAssoc( pos, p, pd );
			pointCache[pointCachePos++]=point(p->x,p->y,p->z);
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
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		return pd->available;
	}
	return 0;	
}

int CTibiaMap::getPointUpDownNoProh(int x, int y, int z)
{
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		if (isPointAvailableNoProh(x,y,z)) return pd->updown;
	}
	return 0;	
}
