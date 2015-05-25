// CreaturesReaderProxy.cpp: implementation of the CCreaturesReaderProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CreaturesReaderProxy.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCreaturesReaderProxy::CCreaturesReaderProxy()
{
	char installPath[1024];
	unsigned long installPathLen = 1023;
	installPath[0] = '\0';
	HKEY hkey = NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_READ, &hkey))
	{
		RegQueryValueEx(hkey, TEXT("Install_Dir"), NULL, NULL, (unsigned char *)installPath, &installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath))
	{
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		PostQuitMessage(-1);
		return;
	}

	char pathBuf[2048];
	sprintf(pathBuf, "%s\\mods\\tibiaauto_util.dll", installPath);

	dllUtil = LoadLibrary(pathBuf);
}

CCreaturesReaderProxy::~CCreaturesReaderProxy()
{
}


int CCreaturesReaderProxy::findCreatureStatForLocationTibiaId(int x, int y, int z, int pos)
{
	if (dllUtil)
	{
		typedef int (*Proto_fun)(int x, int y, int z, int pos);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllUtil, "crFindCreatureStatForLocationTibiaId");
		if (fun)
			return fun(x, y, z, pos);
		else
			return 0;
	}
	else
	{
		return 0;
	}
}

char *CCreaturesReaderProxy::findCreatureStatForLocationName(int x, int y, int z, int pos)
{
	if (dllUtil)
	{
		typedef char *(*Proto_fun)(int x, int y, int z, int pos);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllUtil, "crFindCreatureStatForLocationName");
		if (fun)
			return fun(x, y, z, pos);
		else
			return NULL;
	}
	else
	{
		return NULL;
	}
}

int CCreaturesReaderProxy::findCreatureStatForLocationCount(int x, int y, int z)
{
	if (dllUtil)
	{
		typedef int (*Proto_fun)(int x, int y, int z);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllUtil, "crFindCreatureStatForLocationCount");
		if (fun)
			return fun(x, y, z);
		else
			return 0;
	}
	else
	{
		return 0;
	}
}

char ** CCreaturesReaderProxy::findCreatureStatInArea(int x, int y, int z, int rangeXY, int rangeZ)
{
	if (dllUtil)
	{
		typedef char **(*Proto_fun)(int x, int y, int z, int rangeXY, int rangeZ);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllUtil, "crFindCreatureStatInArea");
		if (fun)
			return fun(x, y, z, rangeXY, rangeZ);
		else
			return NULL;
	}
	else
	{
		return NULL;
	}
}
