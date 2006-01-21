// Queue.cpp: implementation of the CQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Queue.h"
#include "TibiaMapPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueue::CQueue()
{
	tabSize=128;
	count=0;
	tab=(struct point *)malloc(sizeof(struct point)*tabSize);
	start=0;
	end=0;
}

CQueue::~CQueue()
{
	free(tab);
}

void CQueue::add(struct point p)
{
	if (count+3>=tabSize)
		enlarge();
	tab[start++]=p;
	if (start==tabSize)
		start=0;
	count++;
}

int CQueue::size()
{
	return count;
}

struct point CQueue::getFirst()
{
	point ret=tab[end++];
	if (end==tabSize)
		end=0;
	count--;
	return ret;
}

void CQueue::enlarge()
{	
	tabSize*=2;
	tab=(struct point *)realloc(tab,sizeof(struct point)*tabSize);
	if (start<end)
	{
		int i;
		for (i=0;i<start;i++)
			tab[tabSize/2+i]=tab[i];
		start+=tabSize/2;
	}
}

void CQueue::add(int x, int y, int z)
{
	point p;
	p.x=x;
	p.y=y;
	p.z=z;
	add(p);
}
