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


// TibiaContainer.cpp: implementation of the CTibiaContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaContainer.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaContainer::CTibiaContainer()
{
	itemsInside = 0;
	items.RemoveAll();
	flagOnOff = 0;
	objectId  = 0;
	size      = 0;
}

CTibiaContainer::~CTibiaContainer()
{
	int pos;
	int len = items.GetSize();
	for (pos = 0; pos < len; pos++)
	{
		CTibiaItem *item = (CTibiaItem *)items.GetAt(pos);
		delete item;
	}
	;
}

int CTibiaContainer::countItemsOfType(int objectId)
{
	int i;
	int ret = 0;
	for (i = 0; i < itemsInside; i++)
	{
		if (((CTibiaItem *)items.GetAt(i))->objectId == objectId)
		{
			int c = ((CTibiaItem *)items.GetAt(i))->quantity;
			ret += c ? c : 1;
		}
	}
	;
	return ret;
}
