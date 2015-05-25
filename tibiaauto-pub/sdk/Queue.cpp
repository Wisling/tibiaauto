/*
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


// Queue.cpp: implementation of the CQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Queue.h"
#include "TibiaMapPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueue::CQueue()
{
	tabSize = 128;
	count   = 0;
	tab     = (struct point *)malloc(sizeof(struct point) * tabSize);
	start   = 0;
	end     = 0;
}

CQueue::~CQueue()
{
	free(tab);
}

void CQueue::add(struct point p)
{
	if (count + 3 >= tabSize)
		enlarge();
	tab[start++] = p;
	if (start == tabSize)
		start = 0;
	count++;
}

int CQueue::size()
{
	return count;
}

struct point CQueue::getFirst()
{
	point ret = tab[end++];
	if (end == tabSize)
		end = 0;
	count--;
	return ret;
}

void CQueue::enlarge()
{
	tabSize *= 2;
	tab      = (struct point *)realloc(tab, sizeof(struct point) * tabSize);
	if (start < end)
	{
		int i;
		for (i = 0; i < start; i++)
			tab[tabSize / 2 + i] = tab[i];
		start += tabSize / 2;
	}
}

void CQueue::add(int x, int y, int z)
{
	point p;
	p.x = x;
	p.y = y;
	p.z = z;
	add(p);
}
