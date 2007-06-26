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


// TibiaMapProxy.cpp: implementation of the CTibiaMapProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaMapProxy.h"
#include "TibiaMapPoint.h"


//////////////////////////////////////////////////////////////////////

HMODULE CTibiaMapProxy::dllModule=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaMapProxy::CTibiaMapProxy()
{
	// load module	
	if (dllModule==NULL)
	{
		char installPath[1024];
		unsigned long installPathLen=1023;
		installPath[0]='\0';
		HKEY hkey=NULL;
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
		{
			RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
			RegCloseKey(hkey);
		}
		if (!strlen(installPath))
		{
			::MessageBox(0,"ERROR! Unable to read TA install directory! Please reinstall!","ERROR",0);
			exit(1);
		}

		char pathBuf[2048];
		sprintf(pathBuf,"%s\\mods\\tibiaauto_util.dll",installPath);

		dllModule=LoadLibrary(pathBuf);
	}
}

CTibiaMapProxy::~CTibiaMapProxy()
{
	
}



void CTibiaMapProxy::removePointAvailable(int x,int y,int z)
{
	typedef void (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapRemovePointAvailable");
		if (fun)
		{
			fun(x,y,z);
		}
	} 
}
void CTibiaMapProxy::prohPointClear()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapProhPointClear");
		if (fun)
		{
			fun();
		}
	} 
}
void CTibiaMapProxy::prohPointAdd(int x, int y, int z)
{
	typedef void (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapProhPointAdd");
		if (fun)
		{
			fun(x,y,z);
		}
	} 
}
int CTibiaMapProxy::getPointUpDown(int x,int y,int z)
{
	typedef int (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetPointUpDown");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}
int CTibiaMapProxy::getPointUpDownNoProh(int x,int y,int z)
{
	typedef int (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetPointUpDownNoProh");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}
void CTibiaMapProxy::setPointUpDown(int x,int y,int z,int updown)
{
	typedef void (*Proto_fun)(int x,int y,int z, int updown);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapSetPointUpDown");
		if (fun)
		{
			fun(x,y,z,updown);
		}
	} 
}

int CTibiaMapProxy::getPrevPointZ(int x,int y, int z)
{
	typedef int (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetPrevPointZ");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}
int CTibiaMapProxy::getPrevPointY(int x,int y, int z)
{
	typedef int (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetPrevPointY");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}
int CTibiaMapProxy::getPrevPointX(int x,int y, int z)
{
	typedef int (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetPrevPointX");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}
void CTibiaMapProxy::clearPrevPoint()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapClearPrevPoint");
		if (fun)
		{
			fun();
		}
	} 
}
void CTibiaMapProxy::setPrevPoint(int x,int y,int z,int prevX, int prevY, int prevZ)
{
	typedef void (*Proto_fun)(int x,int y,int z, int prevX, int prevY, int prevZ);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapSetPrevPoint");
		if (fun)
		{
			fun(x,y,z,prevX,prevY,prevZ);
		}
	} 
}
struct point CTibiaMapProxy::getRandomPoint()
{
	typedef struct point (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetRandomPoint");
		if (fun)
		{
			return fun();
		}
	} 
	return struct point();
}
void CTibiaMapProxy::clear()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapClear");
		if (fun)
		{
			fun();
		}
	} 
}
void CTibiaMapProxy::setPointAsAvailable(int x,int y,int z)
{
	typedef void (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapSetPointAsAvailable");
		if (fun)
		{
			fun(x,y,z);
		}
	} 
}
int CTibiaMapProxy::isPointAvailable(int x,int y,int z)
{
	typedef int (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapIsPointAvailable");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}

int CTibiaMapProxy::isPointAvailableNoProh(int x,int y,int z)
{
	typedef int (*Proto_fun)(int x,int y,int z);
	if (dllModule)
	{		
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapIsPointAvailableNoProh");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}


int CTibiaMapProxy::size()
{
	typedef int (*Proto_fun)();
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapSize");
		if (fun)
		{
			return fun();
		}
	} 
	return 0;
}
struct point CTibiaMapProxy::getPointByNr(int nr)
{
	typedef struct point (*Proto_fun)(int nr);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetPointByNr");
		if (fun)
		{
			return fun(nr);
		}
	} 
	return struct point();
}
