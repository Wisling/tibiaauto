// TibiaMap.cpp: implementation of the CTibiaMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaMap.h"
#include "TibiaMapPoint.h"

#include <iostream>
#include <fstream>

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

AFX_INLINE UINT AFXAPI HashKey<point*> (point* key)
{	
	return key->x*191+key->y*257+key->z*317;
}


typedef point* LPpoint;
AFX_INLINE BOOL AFXAPI CompareElements<LPpoint, LPpoint> 
     (const LPpoint *v1, const LPpoint *v2)
{	
	if (!v1||!v2) return false;
	point *v1d=*v1;
	point *v2d=*v2;
	if (!v1d||!v2d) return false;	
	if (v1d&&v2d&&v1d->x==v2d->x&&v1d->y==v2d->y&&v1d->z==v2d->z) return true;
	return false;    	
}

typedef CMap<point *,point *,pointData *,pointData *> CMyMap;

CTibiaMap::CTibiaMap()
{		
	prohCount=0;
	prohSize=500;
	prohList=(struct point *)malloc(sizeof(struct point)*prohSize);

	//		ofstream myfile;
	//		myfile.open ("C:/example.txt",ios::out);
	//
	//		myfile <<"\n";
	//		myfile.close();
	//mar/03/08 pointData size=36, point size=12
	tibiaMap2.InitHashTable(11213);//56115 map points in a mapping of travel between all cities 
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
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		pd->available=1;
		pd->updown=0;
	} 
	else 
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
	POSITION pos=tibiaMap2.GetStartPosition();
	point *p;
	pointData *pd;
	while(pos!=NULL){
		tibiaMap2.GetNextAssoc(pos,p,pd);
		delete p;
		delete pd;
	}
	tibiaMap2.RemoveAll();
}

void CTibiaMap::enlarge()
{	
	
}

int CTibiaMap::isPointLonger(int x,int y,int z,int prevX, int prevY, int prevZ){
	return x!=prevX && y!=prevY || z!=prevZ;
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
//	ofstream myfile;
//	myfile.open ("C:/example.txt",ios::app);
//	myfile <<"NormSETTING---"<<"("<<x<<","<<y<<","<<z<<") to "<<"("<<prevX<<","<<prevY<<","<<prevZ<<")"<<"\n";
//	myfile.close();

	struct point p=point(x,y,z);
	struct pointData *pd=NULL;

	if (tibiaMap2.Lookup(&p,pd))
	{
		pd->prevX=prevX;
		pd->prevY=prevY;
		pd->prevZ=prevZ;
		pd->dist=getPointDistance(prevX,prevY,prevZ)+calcDistance(x,y,z,prevX,prevY,prevZ);
	}
}

void CTibiaMap::setBestPrevPoint(int x, int y, int z, int prevX, int prevY, int prevZ)
{
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;

	if (tibiaMap2.Lookup(&p,pd))
	{
		if (pd->prevX==0) {
			setPrevPoint(x, y, z, prevX, prevY, prevZ);
//			ofstream myfile;
//			myfile.open ("C:/example.txt",ios::app);
//			myfile <<"skip"<<pd->prevX<<"\n";
//			myfile.close();
		}
		else if(!(x==pd->prevX&&y==pd->prevY&&z==pd->prevZ)){

			//return;
//			ofstream myfile;
//			myfile.open ("C:/example.txt",ios::app);
//			myfile <<"BEFORE---"<<"("<<p.x<<","<<p.y<<","<<p.z<<") to "<<"("<<pd->prevX<<","<<pd->prevY<<","<<pd->prevZ<<")"<<"("<<pPrev.x<<","<<pPrev.y<<","<<pPrev.z<<") to "<<"("<<pdPrev->prevX<<","<<pdPrev->prevY<<","<<pdPrev->prevZ<<")"<<"\n";
//			myfile.close();
			struct point p1=point(pd->prevX, pd->prevY, pd->prevZ);
			struct point p2=point(prevX, prevY, prevZ);
			int newDist=getPointDistance(p2.x,p2.y,p2.z)+calcDistance(x,y,z,p2.x,p2.y,p2.z);
			int oldDist=getPointDistance(p1.x,p1.y,p1.z)+calcDistance(x,y,z,p1.x,p1.y,p1.z);
			//setPointDistance(x,y,z,min(newDist,oldDist));

			//return is new point less distance than old point
			if(newDist<oldDist)
				setPrevPoint(x,y,z,prevX, prevY, prevZ);
		}
	}
}

int CTibiaMap::intPoint(point p) {return p.x*1000000+p.y*10+p.z;}

//returns -1 if loops onto itself, 0 if nothing should change, 1 if should change
int CTibiaMap::isBetterPrevPoint (int x, int y, int z, int prevX, int prevY, int prevZ)
{	
	return 1;
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	tibiaMap2.Lookup(&p,pd);

	//start with original prior point and pending prior point
	struct point p1=point(pd->prevX, pd->prevY, pd->prevZ);
	struct point p2=point(prevX, prevY, prevZ);
	int newDist=getPointDistance(p2.x,p2.y,p2.z)+calcDistance(x,y,z,p2.x,p2.y,p2.z);
	int oldDist=getPointDistance(p1.x,p1.y,p1.z)+calcDistance(x,y,z,p1.x,p1.y,p1.z);
	//setPointDistance(x,y,z,min(newDist,oldDist));

	//return is new point less distance than old point
	return newDist<oldDist;
	/*
	//keep track of which points have been seen
	CMap<int,int,int,int> track;
	int ans=0;

	ofstream myfile;
	myfile.open ("C:/example.txt",ios::app);
	myfile <<"Start\n";
	myfile.flush();

		myfile <<"Starts with ("<<x<<","<<y<<","<<z<<")"<<"\n";
		myfile.flush();

	int isFinished = 0;
	int dist1=getDistance(x,y,z,p1.x,p1.y,p1.z);
	int dist2=getDistance(x,y,z,p2.x,p2.y,p2.z);

	while(!isFinished){
			myfile <<"("<<p1.x<<","<<p1.y<<","<<p1.z<<")"<<dist1<<" to "<<"("<<p2.x<<","<<p2.y<<","<<p2.z<<")"<<dist2<<"\n";
			myfile.flush();
		isFinished=1;
		if(p1.x==x && p1.y==y && p1.z==z || p2.x==x && p2.y==y && p2.z==z){track.RemoveAll(); return 0;}
		struct pointData *pd1=NULL;
		struct pointData *pd2=NULL;
		if (tibiaMap2.Lookup(&p1,pd1))
		{
			// return if p1 point has been added by p2 already
			ans=0;
			track.Lookup(intPoint(p1),ans);
					myfile <<"ans1("<<p1.x<<","<<p1.y<<","<<p1.z<<")="<<ans<<"\n";
					myfile.flush();
			if (ans){
					myfile <<"1"<<dist1<<track[intPoint(p1)]<<"\n";
					myfile.flush();
				track.RemoveAll();

				if (x==32361&&y==31780){
				char buf[111];
				sprintf(buf,"%d,%d,(%d,%d,%d),(%d,%d,%d),(%d,%d,%d)",dist1,ans,x,y,z,pd->prevX, pd->prevY, pd->prevZ,prevX, prevY, prevZ);
				//AfxMessageBox(buf);
				}
				if (dist1<=ans) return 0;
				else return 1;
			}
			// stop if loops infinitely(start point is set to itself)
			else if (p1.x==pd1->prevX && p1.y==pd1->prevY && p1.z==pd1->prevZ){
				track.SetAt(intPoint(p1),dist1);
					myfile <<"added1("<<p1.x<<","<<p1.y<<","<<p1.z<<")\n";
					myfile.flush();
				p1=point(0,0,0);
			}
			// add to list with distance so far
			else{
				track.SetAt(intPoint(p1),dist1);
					myfile <<"added1("<<p1.x<<","<<p1.y<<","<<p1.z<<")\n";
					myfile.flush();
				dist1+=getDistance(p1.x,p1.y,p1.z,pd1->prevX,pd1->prevY,pd1->prevZ);
				isFinished=0;
				p1=point(pd1->prevX,pd1->prevY,pd1->prevZ);
			}
		}
		if (tibiaMap2.Lookup(&p2,pd2))
		{
			ans=0;
			track.Lookup(intPoint(p2),ans);
					myfile <<"ans2("<<p2.x<<","<<p2.y<<","<<p2.z<<")="<<ans<<"\n";
					myfile.flush();
			if (ans){
					myfile <<"2"<<dist2<<track[intPoint(p2)]<<"\n";
					myfile.flush();
				track.RemoveAll();
				if (x==32361&&y==31780){
				char buf[111];
				sprintf(buf,"%d,%d,(%d,%d,%d),(%d,%d,%d),(%d,%d,%d)",ans,dist2,x,y,z,pd->prevX, pd->prevY, pd->prevZ,prevX, prevY, prevZ);
				//AfxMessageBox(buf);
				}

				if (dist2<ans) return 1;
				else return 0;
			}
			else if (p2.x==pd2->prevX && p2.y==pd2->prevY && p2.z==pd2->prevZ){
				track.SetAt(intPoint(p2),dist2);
					myfile <<"added2("<<p2.x<<","<<p2.y<<","<<p2.z<<")\n";
					myfile.flush();
				p2=point(0,0,0);
			}
			else{
				track.SetAt(intPoint(p2),dist2);
					myfile <<"added2("<<p2.x<<","<<p2.y<<","<<p2.z<<")\n";
					myfile.flush();
				dist2+=getDistance(p2.x,p2.y,p2.z,pd2->prevX,pd2->prevY,pd2->prevZ);
				isFinished=0;
				p2=point(pd2->prevX,pd2->prevY,pd2->prevZ);
			}
		}
	}
	AfxMessageBox("impossible!");
	track.RemoveAll();
	return 0;// if reached here then points are not connected(impossible)
	*/
}

int CTibiaMap::getPointSpeed(int x, int y, int z)
{
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd)) return pd->speed;

	return 0;
}


void CTibiaMap::setPointSpeed(int x, int y, int z,int speed)
{
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd)) pd->speed=speed;
}

int CTibiaMap::getPointDistance(int x, int y, int z)
{
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;	
	if (tibiaMap2.Lookup(&p,pd)) return pd->dist;

	return 0;
}

void CTibiaMap::setPointDistance(int x, int y, int z,int dist)
{
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd)) pd->dist=dist;
}

int CTibiaMap::calcDistance(int x, int y, int z, int prevX, int prevY, int prevZ)
{
	int currentUpDown=getPointUpDown(x,y,z);
	int forcedLevelChange=0;
	if (currentUpDown==101||currentUpDown==204||currentUpDown==302||currentUpDown==303)
	{
		forcedLevelChange=1;
	}
	int spd = getPointSpeed(x, y, z);
	if (spd==0){
		char buf[111];
		sprintf(buf,"speed(%d,%d,%d) %d",x, y, z,spd);
		//AfxMessageBox(buf);
	}
	return spd*(isPointLonger(x, y, z, prevX, prevY, prevZ)?3:1)+forcedLevelChange*10000;//return getPointSpeed(x, y, z)*(isPointLonger(x, y, z, prevX, prevY, prevZ)?3:1);
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

void CTibiaMap::clearDistance()
{
	POSITION pos = tibiaMap2.GetStartPosition();	
	while (pos != NULL)
	{
		point *p=NULL;
		pointData *pd=NULL;
		tibiaMap2.GetNextAssoc( pos, p, pd );
		pd->dist=0;
	}
}


void CTibiaMap::clearLocalPrevPoint(int x,int y,int z,int radius)
{
	POSITION pos = tibiaMap2.GetStartPosition();
	while (pos != NULL)
	{
		point *p=NULL;
		pointData *pd=NULL;
		tibiaMap2.GetNextAssoc( pos, p, pd );
		if (abs(x-pd->prevX)<=radius && abs(y-pd->prevY)<=radius && z==pd->prevZ){
			pd->prevX=0;
			pd->prevY=0;
			pd->prevZ=0;
			pd->dist=0;
		}
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
		if (isPointAvailableNoProh(x,y,z)) return pd->updown;
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
		// this is pathological
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

point CTibiaMap::getDestPoint(int x,int y, int z){
	if (isPointAvailableNoProh(x,y,z)){
		struct point p=point(x,y,z);
		struct pointData *pd=NULL;
		if (tibiaMap2.Lookup(&p,pd))
			return pd->destPoint;
	}
	return point(0,0,0);
}

void CTibiaMap::setDestPoint(int x,int y,int z,int destX, int destY, int destZ){
	struct point p=point(x,y,z);
	struct pointData *pd=NULL;
	if (tibiaMap2.Lookup(&p,pd))
	{
		point dest=point(destX,destY,destZ);
		pd->destPoint=dest;
	}
}