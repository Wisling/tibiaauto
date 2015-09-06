#pragma once

#include <IModuleInterface.h>

class CModuleLoader
{
public:
	static IModuleInterface* LoadModule(char *moduleName, int pathIsAbsolute);
	static IModuleInterface ** allModules;
	static int allModulesCount;
private:
	CModuleLoader() {};
	virtual ~CModuleLoader() {};
	static int allModulesSize;
};
