// ModuleLoader.cpp: implementation of the CModuleLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "ModuleLoader.h"
#include "InstallPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Static definitions
//////////////////////////////////////////////////////////////////////

ModuleMap CModuleLoader::loadedModules;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IModuleInterface* CModuleLoader::LoadModule(char *moduleName, int pathIsAbsolute)
{
	// load module
	char path[128];
	if (!pathIsAbsolute)
	{
		sprintf(path, "%s\\mods\\%s.dll", CInstallPath::getInstallPath().c_str(), moduleName);
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

	// put the module onto our list
	loadedModules[moduleName] = ret;
	ret->setModuleName(moduleName);
	ret->init();
	// 30 is the current kernel version
	ret->activate(30);
	return ret;
}
