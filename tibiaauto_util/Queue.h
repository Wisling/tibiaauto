// Queue.h: interface for the CQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__D521BBEB_1AB6_4737_8ECD_BB910B8DAD37__INCLUDED_)
#define AFX_QUEUE_H__D521BBEB_1AB6_4737_8ECD_BB910B8DAD37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaMap.h"
#include "TibiaMapPoint.h"

class CQueue
{
public:
	void add(int x,int y,int z);
	struct point getFirst();
	int size();
	void add(struct point p);
	CQueue();
	virtual ~CQueue();

private:
	void enlarge();
	int count;
	int tabSize;
	int start;
	int end;
	struct point *tab;

};

#endif // !defined(AFX_QUEUE_H__D521BBEB_1AB6_4737_8ECD_BB910B8DAD37__INCLUDED_)
