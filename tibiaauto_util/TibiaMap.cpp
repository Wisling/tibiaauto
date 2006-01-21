// TibiaMap.cpp: implementation of the CTibiaMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaMap.h"
#include "TibiaMapPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaMap::CTibiaMap()
{	
	mapSize=16;
	mapCount=0;	
	map=(struct point *)malloc(sizeof(struct point)*mapSize);
	prevMap=(struct point *)malloc(sizeof(struct point)*mapSize);

	prohCount=0;
	prohSize=500;
	prohList=(struct point *)malloc(sizeof(struct point)*prohSize);
}

CTibiaMap::~CTibiaMap()
{

}



int CTibiaMap::isPointAvailable(int x, int y,int z)
{
	int i;
	for (i=0;i<mapCount;i++)
	{
		if (map[i].x==x&&map[i].y==y&&map[i].z==z&&map[i].available)
		{
			int prohNr;
			for (prohNr=0;prohNr<prohCount;prohNr++)
			{
				if (prohList[prohNr].x==x&&prohList[prohNr].y==y&&prohList[prohNr].z==z)
					return 0;
			}
			return 1;
		}
	}
	return 0;
}

void CTibiaMap::setPointAsAvailable(int x, int y,int z)
{
	if (!isPointAvailable(x,y,z))
	{
		if (mapCount+2>=mapSize)
			enlarge();
		map[mapCount].x=x;
		map[mapCount].y=y;
		map[mapCount].z=z;
		map[mapCount].updown=0;
		map[mapCount].available=1;
		prevMap[mapCount].x=0;
		prevMap[mapCount].y=0;
		prevMap[mapCount].z=0;
		prevMap[mapCount].updown=0;
		mapCount++;
	};
}

void CTibiaMap::clear()
{
	mapCount=0;
}

void CTibiaMap::enlarge()
{	
	mapSize*=2;
	map=(struct point *)realloc(map,sizeof(struct point)*mapSize);
	prevMap=(struct point *)realloc(prevMap,sizeof(struct point)*mapSize);
}

struct point CTibiaMap::getRandomPoint()
{
	if (!mapCount)
	{
		struct point p;
		p.x=p.y=0;
		return p;
	}	
	int pNr = rand()%mapCount;
	return map[pNr];	
}

void CTibiaMap::setPrevPoint(int x, int y, int z,int prevX, int prevY,int prevZ)
{
	int i;
	for (i=0;i<mapCount;i++)
	{ 
		if (map[i].x==x&&map[i].y==y&&map[i].z==z)
		{
			prevMap[i].x=prevX;
			prevMap[i].y=prevY;
			prevMap[i].z=prevZ;
			return;
		}
	}	
}

void CTibiaMap::clearPrevPoint()
{
	int i;
	for (i=0;i<mapCount;i++)
	{
		prevMap[i].x=prevMap[i].y=0;
	}
}



int CTibiaMap::getPrevPointX(int x, int y, int z)
{	
	int i;
	for (i=0;i<mapCount;i++)
	{
		if (map[i].x==x&&map[i].y==y&&map[i].z==z)
		{
			return prevMap[i].x;						
		}
	}	
	return 0;
}

int CTibiaMap::getPrevPointY(int x, int y, int z)
{
		int i;
	for (i=0;i<mapCount;i++)
	{
		if (map[i].x==x&&map[i].y==y&&map[i].z==z)
		{
			return prevMap[i].y;						
		}
	}	
	return 0;
}

int CTibiaMap::getPrevPointZ(int x, int y, int z)
{
	int i;
	for (i=0;i<mapCount;i++)
	{
		if (map[i].x==x&&map[i].y==y&&map[i].z==z)
		{
			return prevMap[i].z;						
		}
	}	
	return 0;
}

void CTibiaMap::saveToDisk(FILE *f)
{
	int i;
	fwrite(&mapSize,sizeof(int),1,f);
	fwrite(&mapCount,sizeof(int),1,f);
	for (i=0;i<mapSize;i++)
	{
		fwrite(&map[i].x,sizeof(int),1,f);
		fwrite(&map[i].y,sizeof(int),1,f);
		fwrite(&map[i].z,sizeof(int),1,f);
		fwrite(&map[i].updown,sizeof(int),1,f);
		fwrite(&prevMap[i].x,sizeof(int),1,f);
		fwrite(&prevMap[i].y,sizeof(int),1,f);
		fwrite(&prevMap[i].z,sizeof(int),1,f);
		fwrite(&prevMap[i].updown,sizeof(int),1,f);
	}
}

void CTibiaMap::loadFromDisk(FILE *f)
{
	int i;
	fread(&mapSize,sizeof(int),1,f);
	fread(&mapCount,sizeof(int),1,f);

	map=(struct point *)realloc(map,sizeof(struct point)*mapSize);
	prevMap=(struct point *)realloc(prevMap,sizeof(struct point)*mapSize);

	for (i=0;i<mapSize;i++)
	{
		fread(&map[i].x,sizeof(int),1,f);
		fread(&map[i].y,sizeof(int),1,f);
		fread(&map[i].z,sizeof(int),1,f);
		fread(&map[i].updown,sizeof(int),1,f);
		fread(&prevMap[i].x,sizeof(int),1,f);
		fread(&prevMap[i].y,sizeof(int),1,f);
		fread(&prevMap[i].z,sizeof(int),1,f);
		fread(&prevMap[i].updown,sizeof(int),1,f);
	}	
}

void CTibiaMap::setPointUpDown(int x, int y, int z, int updown)
{
	int i;
	for (i=0;i<mapCount;i++)
	{
		if (map[i].x==x&&map[i].y==y&&map[i].z==z)
		{
			map[i].updown=updown;
		}
	}		
}

int CTibiaMap::getPointUpDown(int x, int y, int z)
{
	int i;
	for (i=0;i<mapCount;i++)
	{
		if (map[i].x==x&&map[i].y==y&&map[i].z==z)
		{
			return map[i].updown;
		}
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
	int i;
	for (i=0;i<mapCount;i++)
	{
		if (map[i].x==x&&map[i].y==y&&map[i].z==z)
		{
			map[i].available=0;
		}
	}
}

int CTibiaMap::size()
{
	// for purpose of the external 'size', return is 'count' (no of elements)
	return mapCount;
}

struct point CTibiaMap::getPointByNr(int nr)
{
	return map[nr];
}
