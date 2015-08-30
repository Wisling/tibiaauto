#pragma once

#include "tibiaauto_util.h"
#include "TibiaMap.h"
#include "TibiaMapPoint.h"

class TIBIAAUTOUTIL_API CQueue
{
public:
	void add(int x, int y, int z);
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

