// ModuleLoader.cpp: implementation of the CModuleLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "ModuleLoader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Static definitions
//////////////////////////////////////////////////////////////////////

IModuleInterface ** CModuleLoader::allModules = NULL;
int CModuleLoader::allModulesCount        = 0;
int CModuleLoader::allModulesSize         = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IModuleInterface* CModuleLoader::LoadModule(char *moduleName, int pathIsAbsolute)
{
	// load module
	char path[128];
	if (!pathIsAbsolute)
	{
		char installPath[1024];
		unsigned long installPathLen = 1023;
		installPath[0] = '\0';
		HKEY hkey = NULL;
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_READ, &hkey))
		{
			RegQueryValueEx(hkey, TEXT("Install_Dir"), NULL, NULL, (unsigned char *)installPath, &installPathLen);
			RegCloseKey(hkey);
		}
		if (!strlen(installPath))
		{
			AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
			PostQuitMessage(-1);
			return NULL;
		}

		sprintf(path, "%s\\mods\\%s.dll", installPath, moduleName);
	}
	else
	{
		strcpy(path, moduleName);
	}
	HMODULE dllModule = LoadLibrary(path);
	if (!dllModule)
	{
		char errorMsg[128];
		sprintf(errorMsg, "ERROR! Unable to load module %s.", moduleName);
		AfxMessageBox(errorMsg);
		return NULL;		
	}

	typedef IModuleInterface *(*FactoryFunProto)();
	FactoryFunProto factoryFun = (FactoryFunProto)GetProcAddress(dllModule, "initModuleInstance");
	if (!dllModule)
	{
		char errorMsg[128];
		sprintf(errorMsg, "ERROR! Unable to resolve initModuleInstance entry in %s.", moduleName);
		AfxMessageBox(errorMsg);
		return NULL;
	}
	IModuleInterface* ret = factoryFun();

	// put the module onto our static list
	if (allModulesCount == allModulesSize)
	{
		allModulesSize = allModulesSize * 2 + 3;
		allModules = (IModuleInterface **)realloc(allModules, sizeof(IModuleInterface *) * allModulesSize);
	}
	allModules[allModulesCount++] = ret;
	ret->setModuleName(moduleName);
	ret->init();
	// 30 is the current kernel version
	ret->activate(30);
	return ret;
}
