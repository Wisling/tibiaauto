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

#include "stdafx.h"
#include "TibiaMapPoint.h"

point::point()
{
	x = 0;
	y = 0;
	z = 0;
};

point::point(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

bool point::operator()(const point p1, const point p2) const
{
	if (p1.x > p2.x)
		return true;
	if (p1.x < p2.x)
		return false;
	if (p1.x == p2.x)
	{
		if (p1.y > p2.y)
			return true;
		if (p1.y < p2.y)
			return false;
		if (p1.y == p2.y)
		{
			if (p1.z > p2.z)
				return true;
			if (p1.z < p2.z)
				return false;
			if (p1.z == p2.z)
				return false;
		}
	}
	// never reached
	return false;
}
