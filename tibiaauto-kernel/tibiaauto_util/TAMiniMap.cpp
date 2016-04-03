// TAMiniMap.cpp: implementation of the CTAMiniMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "afxcoll.h"
#include "TAMiniMap.h"
#include "TibiaMap.h"
#include "TibiaMapPoint.h"
#include "TibiaMiniMap.h"
#include "TibiaMiniMapPoint.h"
#include "PackSender.h"
#include "MemReader.h"
#include "MemUtil.h"
#include "time.h"
#include "HashMap.h"
#include <queue>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include "math.h"
#include "ModuleUtil.h"
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template<>
AFX_INLINE UINT AFXAPI HashKey<point*> (point* key)
{
	return key->x * 191 + key->y * 257 + key->z * 317;
}

typedef point* LPpoint;
template<>
AFX_INLINE BOOL AFXAPI CompareElements<LPpoint, LPpoint>
        (const LPpoint *v1, const LPpoint *v2)
{
	if (!v1 || !v2)
		return false;
	point *v1d = *v1;
	point *v2d = *v2;
	if (!v1d || !v2d)
		return false;
	if (v1d && v2d && v1d->x == v2d->x && v1d->y == v2d->y && v1d->z == v2d->z)
		return true;
	return false;
}

CTAMiniMap::CTAMiniMap()
{
	//AfxMessageBox("new minimap?");
	maxMaps = 100;
	taMiniMap.InitHashTable(17);
	mapCount = 0;
}

CTAMiniMap::~CTAMiniMap()
{
}

//returns true if the map containing (x,y,z) is currently loaded
int CTAMiniMap::isPointInMiniMap(int x, int y, int z)
{
	x = (int)(x / 256);
	y = (int)(y / 256);
	z = z;
	struct point p            = point(x, y, z);
	struct MiniMapSection *pd = NULL;
	if (taMiniMap.Lookup(&p, pd))
		return true;
	return false;
}

//Input: xMap,yMap,zMap are the x,y,z coords of the map files, not the point
//returns NULL if the number of allowable maps is already reached or the map is not in Tibia's memory
MiniMapSection* CTAMiniMap::loadFromMemory(int xMap, int yMap, int zMap)
{
	if (mapCount >= maxMaps)
		unloadMiniMaps();
	//AfxMessageBox("load memory");

	for (int nr = 0; nr < 10; nr++)
	{
		int mapOffset = CTibiaItem::getValueForConst("addrMiniMapStart") + CTibiaItem::getValueForConst("lengthMiniMap") * nr + 20;
		CTibiaMiniMap *map = CMemReader::getMemReader().readMiniMap(nr);

		//char buf[1111];
		//sprintf(buf,"cycle maps:(%d,%d,%d),(%d,%d,%d)",xMap,yMap,zMap,map->x,map->y,map->z);
		//AfxMessageBox(buf);

		if (xMap == map->x && yMap == map->y && zMap == map->z)
		{
			//AfxMessageBox("map found");
			MiniMapSection* pdNew = new MiniMapSection(xMap, yMap, zMap);
			pdNew->timeUsed = time(NULL);
			CMemUtil::getMemUtil().GetMemRange(mapOffset, mapOffset + 65536, (char*)pdNew->colour);
			CMemUtil::getMemUtil().GetMemRange(mapOffset + 65536, mapOffset + 65536 + 65536, (char*)pdNew->speed);

			struct point *p           = new point(xMap, yMap, zMap);
			struct MiniMapSection *pd = NULL;
			taMiniMap.Lookup(p, pd);

			//AfxMessageBox("setting pointer data");
			if (pd)
			{
				delete pd;
				delete p;
				pd = pdNew;
			}
			else
			{
				pd = pdNew;
				taMiniMap.SetAt(p, pd);
				mapCount += 1;
			}

			delete map;
			return pd;
		}
		delete map;
	}
	//minimap does not exist in memory
	return NULL;
}

//Input: xMap,yMap,zMap are the x,y,z coords of the map files, not the point
//returns NULL if the number of allowable maps is already reached or the .map file does not exist
MiniMapSection* CTAMiniMap::loadFromFile(int xMap, int yMap, int zMap)
{
	if (mapCount >= maxMaps)
		unloadMiniMaps();
	//AfxMessageBox("load File");
	char filename[1024 + 20];
	sprintf(filename, "%s%s%03d%03d%02d.map", getenv("USERPROFILE"), "/Application Data/Tibia/Automap/", xMap, yMap, zMap);
	ifstream f;
	f.open(filename, ifstream::in);
	//AfxMessageBox(filename);
	if (!f.bad())
	{
		//AfxMessageBox("file found, loading");
		struct MiniMapSection *pdNew = new MiniMapSection(xMap, yMap, zMap);

		pdNew->timeUsed = time(NULL);
		f.read((char*)pdNew->colour, 65536);
		if (f.bad())  //read failed
		{
			f.close();
			delete pdNew;
			sprintf(filename, "Fileread Failed: %s%s%03d%03d%02d.map", getenv("USERPROFILE"), "/Application Data/Tibia/Automap/", xMap, yMap, zMap);
			AfxMessageBox(filename);
			return NULL;
		}
		f.read((char*)pdNew->speed, 65536);
		if (f.bad())  //read failed
		{
			f.close();
			delete pdNew;
			sprintf(filename, "Fileread Failed: %s%s%03d%03d%02d.map", getenv("USERPROFILE"), "/Application Data/Tibia/Automap/", xMap, yMap, zMap);
			AfxMessageBox(filename);
			return NULL;
		}
		f.close();
		//AfxMessageBox("setting pointer data");

		struct point *p           = new point(xMap, yMap, zMap);
		struct MiniMapSection *pd = NULL;
		taMiniMap.Lookup(p, pd);

		//AfxMessageBox("setting pointer data");
		if (pd)
		{
			delete pd;
			delete p;
			pd = pdNew;
		}
		else
		{
			pd = pdNew;
			taMiniMap.SetAt(p, pdNew);
			mapCount += 1;
		}

		return pd;
	}

	//minimap file does not exist
	return NULL;
}

CTibiaMiniMapPoint* CTAMiniMap::getMiniMapPoint(int x, int y, int z)
{
	struct point p     = point((int)(x / 256), (int)(y / 256), z);
	MiniMapSection *pd = NULL;
	//AfxMessageBox("Lookup");
	if (taMiniMap.Lookup(&p, pd))
	{
		CTibiaMiniMapPoint *miniMapPoint = new CTibiaMiniMapPoint(x, y, z, pd->getColour(x % 256, y % 256), pd->getSpeed(x % 256, y % 256));
		return miniMapPoint;
	}
	else
	{
		if ((pd = loadFromMemory((int)(x / 256), (int)(y / 256), z)) != NULL)
		{
			CTibiaMiniMapPoint *miniMapPoint = new CTibiaMiniMapPoint(x, y, z, pd->getColour(x % 256, y % 256), pd->getSpeed(x % 256, y % 256));
			return miniMapPoint;
		}
		else if ((pd = loadFromFile((int)(x / 256), (int)(y / 256), z)) != NULL)
		{
			CTibiaMiniMapPoint *miniMapPoint = new CTibiaMiniMapPoint(x, y, z, pd->getColour(x % 256, y % 256), pd->getSpeed(x % 256, y % 256));
			return miniMapPoint;
		}
	}
	return new CTibiaMiniMapPoint(x, y, z, 0, 250);
}

void CTAMiniMap::setMiniMapPoint(int x, int y, int z, int col, int spd)
{
	int xMap = (int)(x / 256), yMap = (int)(y / 256), zMap = z;
	for (int nr = 0; nr < 10; nr++)
	{
		int mapOffset = CTibiaItem::getValueForConst("addrMiniMapStart") + CTibiaItem::getValueForConst("lengthMiniMap") * nr + 20;
		CTibiaMiniMap *map = CMemReader::getMemReader().readMiniMap(nr);

		if (xMap == map->x && yMap == map->y && zMap == map->z)
		{
			int pointOffset = (x % 256) * 256 + y % 256;
			char colour     = col;
			char speed      = spd;

			CMemUtil::getMemUtil().SetMemRange(mapOffset + pointOffset, mapOffset + pointOffset + 1, (char*)colour);
			CMemUtil::getMemUtil().SetMemRange(mapOffset + 65536 + pointOffset, mapOffset + 65536 + pointOffset + 1, (char*)speed);
		}
		delete map;
	}
}

void CTAMiniMap::unloadMiniMaps()
{
	mapCount = 0;
	POSITION pos = taMiniMap.GetStartPosition();
	point *p;
	MiniMapSection *pd;
	while (pos != NULL)
	{
		taMiniMap.GetNextAssoc(pos, p, pd);
		delete p;
		delete pd;
	}
	taMiniMap.RemoveAll();
}

class PathFinderNode
{
public:
	int x;
	int y;
	int z;
	int px;
	int py;
	int pz;
	int g;
	int h;
	int f;
	PathFinderNode()
	{
	}

	PathFinderNode(int x1, int y1, int z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}

	PathFinderNode(int x1, int y1, int z1, int px1, int py1, int pz1, int g1, int h1, int f1)
	{
		x  = x1;
		y  = y1;
		z  = z1;
		px = px1;
		py = py1;
		pz = pz1;
		g  = g1;
		h  = h1;
		f  = f1;
	}

	void copy(PathFinderNode c)
	{
		x  = c.x;
		y  = c.y;
		z  = c.z;
		px = c.px;
		py = c.py;
		pz = c.pz;
		g  = c.g;
		h  = c.h;
		f  = c.f;
	}

	PathFinderNode copy()
	{
		return PathFinderNode(x, y, z, px, py, pz, g, h, f);
	}
};

struct pComp {
	bool operator() (const PathFinderNode lhs, const PathFinderNode rhs)
	{
		return lhs.h > rhs.h;
	}
};

class PQI : public priority_queue<PathFinderNode, vector<PathFinderNode>, pComp> {
public:
	vector<PathFinderNode>* Container()
	{
		return &c;
	}
};

//typedef priority_queue<PathFinderNode *, vector<PathFinderNode *>, pComp> PriorityQueue;
// Get access to the underlying container.
template<>
AFX_INLINE UINT AFXAPI HashKey<PathFinderNode> (PathFinderNode key)
{
	return key.x * 191 + key.y * 257 + key.z * 317;
}

void DebugPrint(const char* s, int a, int b = 0, int c = 0, int d = 0)
{
	char buf[111];
	sprintf(buf, "%s - %d %d %d %d", s, a, b, c, d);
	AfxMessageBox(buf);
}

typedef PathFinderNode LNode;
template<>
AFX_INLINE BOOL AFXAPI CompareElements<LNode, LNode>
        (const LNode* v1d, const LNode* v2d)
{
	if (v1d && v2d && v1d->x == v2d->x && v1d->y == v2d->y && v1d->z == v2d->z)
		return true;
	return false;
}

bool mStop    = false;
bool mStopped = true;
int mDistance = 0;
int mOpenSize = 0;
void CTAMiniMap::findPathStop()
{
	mStop = true;
}

bool CTAMiniMap::isFindPathStopped()
{
	return mStopped;
}

int CTAMiniMap::getCurrentDistance()
{
	return mDistance;
}

bool CTAMiniMap::isUpPoint(CTibiaMiniMapPoint* lower) //return true;
{
	bool ret = false;
	//check if both are yellow spots
	if (lower->colour == 210)
	{
		CTibiaMiniMapPoint* upper = getMiniMapPoint(lower->x, lower->y, lower->z - 1);
		if (upper->colour == 210)
		{
			ret = true;
		}
		else if (upper->colour == 129)//check if might be able to go down light gray spot
		{
			CTibiaMiniMapPoint* mpTest1 = getMiniMapPoint(upper->x, upper->y - 1, upper->z);
			CTibiaMiniMapPoint* mpTest2 = getMiniMapPoint(upper->x + 1, upper->y + 1, upper->z);
			CTibiaMiniMapPoint* mpTest3 = getMiniMapPoint(upper->x - 1, upper->y + 1, upper->z);
			if (mpTest1->colour != 129 && mpTest2->colour != 129 && mpTest3->colour != 129)//surrounding tiles aren't grey good chance it is hole
				ret = true;
			delete(mpTest1);
			delete(mpTest2);
			delete(mpTest3);
		}
		delete(upper);
		//CTibiaMiniMapPoint* lower2=CMemReader::getMemReader().readMiniMapPoint(lower->x,lower->y,lower->z+1);
		//CTibiaMiniMapPoint* upper2=CMemReader::getMemReader().readMiniMapPoint(lower->x,lower->y,lower->z-2);
	}
	return ret;
}

bool CTAMiniMap::isDownPoint(CTibiaMiniMapPoint* upper) //return true;
{       //DebugPrint("isDown?",upper->x,upper->y,upper->z,upper->colour);
	//DebugPrint("lower",lower->x,lower->y,lower->z,lower->colour);
	bool ret = false;
	//check if both are yellow spots
	if (upper->colour == 210)
	{
		CTibiaMiniMapPoint* lower = getMiniMapPoint(upper->x, upper->y, upper->z + 1);
		if (lower->colour == 210)
			ret = true;
		delete(lower);
		//CTibiaMiniMapPoint* lower2=CMemReader::getMemReader().readMiniMapPoint(lower->x,lower->y,lower->z+1);
		//CTibiaMiniMapPoint* upper2=CMemReader::getMemReader().readMiniMapPoint(lower->x,lower->y,lower->z-2);
	}
	//check if might be able to go down light gray spot
	else if (upper->colour == 129)
	{
		CTibiaMiniMapPoint* mpTest1 = getMiniMapPoint(upper->x, upper->y - 1, upper->z);
		CTibiaMiniMapPoint* mpTest2 = getMiniMapPoint(upper->x + 1, upper->y + 1, upper->z);
		CTibiaMiniMapPoint* mpTest3 = getMiniMapPoint(upper->x - 1, upper->y + 1, upper->z);
		if (mpTest1->colour != 129 && mpTest2->colour != 129 && mpTest3->colour != 129)//surrounding tiles aren't grey good chance it is hole
		{
			CTibiaMiniMapPoint* lower = getMiniMapPoint(upper->x, upper->y, upper->z + 1);
			if (lower->colour == 210)
				ret = true;
			delete(lower);
		}
		delete(mpTest1);
		delete(mpTest2);
		delete(mpTest3);
	}
	//DebugPrint("result",ret);
	return ret;
}

//Algorithm courtesy of http://code.google.com/p/tibiaapi/source/browse/trunk/tibiaapi/Util/AStarPathFinder.cs
//Provides a guaranteed path between two points but stops when shortest(not best) path found
//To improve, do not store g-value(lower=less time to walk path) whenever collisions
//Do not store in priority queue, use queue and process points in the order they come.
//occur ie(calculating point already in mClose) calculate g and pick better one to put into mClose
//when path found, allow algorithm to run for a bit longer(= as many times as the largest anticipated difference between two unjoined, alternative paths)
//this gets around the need to update all g's for
CUIntArray * CTAMiniMap::findPathOnMiniMap(int startX, int startY, int startZ, int endX, int endY, int endZ)
{
	//inits
	CUIntArray *path = new CUIntArray();
	PQI mOpen;
	int direction[10][3] = {{0, 0, 1}, {1, 0, 0}, {1, -1, 0}, {0, -1, 0}, {-1, -1, 0}, {-1, 0, 0}, {-1, 1, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, -1}};
	CMap<PathFinderNode, PathFinderNode, PathFinderNode, PathFinderNode> mClose;
	mClose.InitHashTable(20011);// 42,000 points used for Carlin-Thais

	CTibiaMiniMapPoint* endPt = CMemReader::getMemReader().readMiniMapPoint(endX, endY, endZ);
	int endBlocked            = endPt->speed == 255;
	delete endPt;
	//DebugPrint("endBlocked",endBlocked);
	int mHEstimate   = 1;
	int maxDistBreak = (int)(1.7 * (abs(startX - endX) + abs(startY - endY)) + 50 * abs(startZ - endZ));//will stop if looking farther away than 1.5x original distance
	int minTimeBreak = 5;//always allows this much time before breaking 5 secs ~ radius of 50 sqm
	time_t timeStart = time(NULL);

	bool found = false;
	mStop    = false;
	mStopped = false;

	PathFinderNode parentNode = PathFinderNode();
	parentNode.g  = 0;
	parentNode.h  = 0;
	parentNode.f  = parentNode.g + parentNode.h;
	parentNode.x  = startX;
	parentNode.y  = startY;
	parentNode.z  = startZ;
	parentNode.px = parentNode.x;
	parentNode.py = parentNode.y;
	parentNode.pz = parentNode.z;
	mOpen.push(parentNode);

	//ofstream fout;
	//fout.open("C:/path.txt",ofstream::out);
	while (!mOpen.empty() && !mStop && !(time(NULL) - timeStart > minTimeBreak && mDistance > maxDistBreak))
	{
		//DebugPrint("mOpen-Top",mOpen.size(),mOpen.top()->x,mOpen.top()->y,mOpen.top()->z);
		//DebugPrint("dist,G",mOpen.top()->h,mOpen.top()->g);
		parentNode = mOpen.top();
		mOpen.pop();
		mDistance = parentNode.h;
		mOpenSize = mOpen.size();
		if (mDistance && mDistance < 30)
			maxDistBreak = 60;//readjust when close in case end is unreachable
		//fout << parentNode.x-endX << " " << parentNode.y-endY << " " << parentNode.z - endZ << "\n";


		if (max(abs(parentNode.x - endX), abs(parentNode.y - endY)) <= endBlocked && parentNode.z == endZ)//can be 1 away if end is blocked
		{
			PathFinderNode addNode = parentNode;
			mClose.SetAt(addNode, addNode);

			found = true;
			break;
		}

		//Lets calculate each successors
		CTibiaMiniMapPoint* mpPar = getMiniMapPoint(parentNode.x, parentNode.y, parentNode.z);
		for (size_t i = 0; i < 10; i++)//0=down 1-8=surroundings 9=up
		{
			PathFinderNode newNode = PathFinderNode();
			newNode.x = parentNode.x + direction[i][0];
			newNode.y = parentNode.y + direction[i][1];
			newNode.z = parentNode.z + direction[i][2];

			bool isAvailable = false;

			//ensures it doesn't load an uneeded map into memory
			CTibiaMiniMapPoint* mp = getMiniMapPoint(newNode.x, newNode.y, newNode.z);
			if (i == 0 || i == 9)
			{
				if (i == 0 && isDownPoint(mpPar) || i == 9 && isUpPoint(mpPar))
					isAvailable = true;
			}
			else
			{
				if (mp->speed != 255 && mp->colour != 0 || mp->colour == 210 /*isUpPoint(mp) || isDownPoint(mp)*/)
					isAvailable = true;
			}

			if (!isAvailable)
			{
				delete mp;
				continue;
			}
			int newG = parentNode.g + mp->speed * (i % 2 == 0 && i > 0 ? 3 : 1);//lower speed value = faster do x3 for diagonals
			delete mp;

			newNode.px = parentNode.x;
			newNode.py = parentNode.y;
			newNode.pz = parentNode.z;
			newNode.g  = newG;
			newNode.h  = mHEstimate * (abs(newNode.x - endX) + abs(newNode.y - endY) + abs(newNode.z - endZ));// + 50*abs(newNode.z - endZ)
			newNode.f  = newNode.g + newNode.h;
			//DebugPrint("newNode",newNode.x,newNode.y,newNode.z);

			vector<PathFinderNode>* mOpenIter = mOpen.Container();
			size_t j;
			for (j = 0; j < mOpenIter->size(); j++)
			{
				if (newNode.x == mOpenIter->at(j).x && newNode.y == mOpenIter->at(j).y && newNode.z == mOpenIter->at(j).z)
					break;
			}
			if (j < mOpenIter->size())
			{
				if (newG < mOpenIter->at(j).g)
					mOpenIter->at(j).copy(newNode);
				continue;
			}
			mOpenIter = NULL;

			PathFinderNode tmpNode;
			if (mClose.Lookup(newNode, tmpNode))
			{
				if (newG < tmpNode.g)
					mClose.SetAt(newNode, newNode);           //this node is better, replace old one and take the hit on keeping accurate g-values
				continue;
				if (newG >= tmpNode.g)
					continue;
			}
			mOpen.push(newNode);
			//DebugPrint("newNode's parent",newNode->px,newNode->py,newNode->pz);
			//DebugPrint("mOpen-add",mOpen.size(),direction[i][0],direction[i][1],direction[i][2]);
		}
		delete(mpPar);
		mClose.SetAt(parentNode, parentNode);
	}
	mStopped = true;
	//fout.close();
	if (found)
	{
		PathFinderNode currNode = parentNode;
		//parentNode=NULL;
		PathFinderNode tmpNode = PathFinderNode(currNode.px, currNode.py, currNode.pz);

/*
                POSITION m = mClose.GetStartPosition();
   AfxMessageBox("loop");
                while (m){
                        PathFinderNode*	k1=NULL;
                        PathFinderNode*	v1=NULL;
                        mClose.GetNextAssoc(m,k1,v1);
                        DebugPrint("currNode",k1->x,k1->y,k1->z);
                        DebugPrint("currNode's parent",v1->px,v1->py,v1->pz);
                }

 */
		mClose.Lookup(tmpNode, parentNode);
		while (!(currNode.x == parentNode.x && currNode.y == parentNode.y && currNode.z == parentNode.z))
		{
			int dir;
			int dx = currNode.x - parentNode.x;
			int dy = currNode.y - parentNode.y;
			int dz = currNode.z - parentNode.z;
			for (dir = 0; dir < 10; dir++)
			{
				if (direction[dir][0] == dx && direction[dir][1] == dy && direction[dir][2] == dz)
					break;
			}
			if (dir == 9)
				dir = STEP_UPSTAIRS;
			else if (dir == 0)
				dir = STEP_DOWNSTAIRS;
			//DebugPrint("dir",dir,dx,dy,dz);
			path->Add(dir);
			currNode = parentNode;
			tmpNode  = PathFinderNode(currNode.px, currNode.py, currNode.pz);
			mClose.Lookup(tmpNode, parentNode);
		}
		int endi = path->GetSize() - 1;
		int begi = 0;
		while (begi < endi && endi > 0)
		{
			int tmp = path->GetAt(endi);
			path->SetAt(endi, path->GetAt(begi));
			path->SetAt(begi, tmp);
			endi--;
			begi++;
		}
	}
	//DebugPrint("mOpen, mClose sizes, open MiniMaps",mOpen.size(),mClose.GetCount(),taMiniMap.GetCount());
	while (!mOpen.empty())
	{
		mOpen.pop();
	}
	mClose.RemoveAll();
	return path;
}
