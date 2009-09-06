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


// TAMiniMapProxy.cpp: implementation of the CTAMiniMapProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TAMiniMapProxy.h"
#include "TibiaMapPoint.h"


//////////////////////////////////////////////////////////////////////

HMODULE CTAMiniMapProxy::dllModule=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTAMiniMapProxy::CTAMiniMapProxy()
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

CTAMiniMapProxy::~CTAMiniMapProxy()
{
	
}

int CTAMiniMapProxy::isPointInMiniMap(int x,int y,int z)
{
	typedef int (*Proto_fun)(int x, int y, int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapIsPointInMiniMap");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}
CTibiaMiniMapPoint * CTAMiniMapProxy::getMiniMapPoint(int x,int y,int z)
{
	typedef CTibiaMiniMapPoint * (*Proto_fun)(int x, int y, int z);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetMiniMapPoint");
		if (fun)
		{
			return fun(x,y,z);
		}
	} 
	return 0;
}
void CTAMiniMapProxy::setMiniMapPoint(int x,int y,int z,int col,int spd)
{
	typedef void(*Proto_fun)(int x,int y,int z,int col,int spd);
	if (dllModule)
	{			
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapSetMiniMapPoint");
		if (fun)
		{
			fun(x,y,z,col,spd);
		}
	} 
}
CUIntArray * CTAMiniMapProxy::findPathOnMiniMap(int x,int y,int z,int x2,int y2,int z2)
{
	typedef CUIntArray*(*Proto_fun)(int x,int y,int z,int x2,int y2,int z2);
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapFindPathOnMiniMap");
		if (fun)
		{
			return fun(x, y, z, x2, y2, z2);
		}
	} 
	return NULL;
}

void CTAMiniMapProxy::findPathStop()
{
	typedef void(*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapFindPathStop");
		if (fun)
		{
			fun();
		}
	} 
}
bool CTAMiniMapProxy::isFindPathStopped()
{
	typedef bool(*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapIsFindPathStopped");
		if (fun)
		{
			return fun();
		}
	} 
	return NULL;
}
int CTAMiniMapProxy::getCurrentDistance()
{
	typedef int(*Proto_fun)();
	if (dllModule)
	{
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"tibiaMapGetCurrentDistance");
		if (fun)
		{
			return fun();
		}
	} 
	return NULL;
}