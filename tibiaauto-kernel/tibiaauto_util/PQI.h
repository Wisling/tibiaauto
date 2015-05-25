// PQI.h: interface for the PQI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PQI_H__D521BBEB_1AB6_4737_8ECD_BB910B8DAD37__INCLUDED_)
#define AFX_PQI_H__D521BBEB_1AB6_4737_8ECD_BB910B8DAD37__INCLUDED_

#if _MSC_VER > 1000
#pragma warning( disable : 4786 503 )
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <queue>
#include <vector>
using namespace std;

class pointNode
{
public:
	int x;
	int y;
	int z;
	int px;
	int py;
	int pz;
	int dist;
	pointNode(){
	}
	pointNode(int x1, int y1, int z1, int px1, int py1, int pz1) {
		x  = x1;
		y  = y1;
		z  = z1;
		px = px1;
		py = py1;
		pz = pz1;
	}
	pointNode(int x1, int y1, int z1, int px1, int py1, int pz1, int dist1) {
		x    = x1;
		y    = y1;
		z    = z1;
		px   = px1;
		py   = py1;
		pz   = pz1;
		dist = dist1;
	}
	void copy(pointNode c){
		x    = c.x;
		y    = c.y;
		z    = c.z;
		px   = c.px;
		py   = c.py;
		pz   = c.pz;
		dist = c.dist;
	}
	pointNode copy(){
		return pointNode(x, y, z, px, py, pz, dist);
	}
};

struct pComp {
	bool operator() (const pointNode lhs, const pointNode rhs) {
		return lhs.dist > rhs.dist;
	}
};

template<>
AFX_INLINE UINT AFXAPI HashKey<pointNode> (pointNode key)
{
	return key.x * 191 + key.y * 257 + key.z * 317;
}

typedef pointNode LNode;
template<>
AFX_INLINE BOOL AFXAPI CompareElements<LNode, LNode>
        (const LNode* v1d, const LNode* v2d)
{
	if (v1d && v2d && v1d->x == v2d->x && v1d->y == v2d->y && v1d->z == v2d->z && v1d->px == v2d->px && v1d->py == v2d->py && v1d->pz == v2d->pz)
		return true;
	return false;
}

class PQI : public priority_queue<pointNode, vector<pointNode>, pComp> {
public:
	vector<pointNode>* Container() {
		return &c;
	}
};

#endif // !defined(AFX_PQI_H__D521BBEB_1AB6_4737_8ECD_BB910B8DAD37__INCLUDED_)
