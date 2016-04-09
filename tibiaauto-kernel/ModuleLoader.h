#pragma once

#include <IModuleInterface.h>
#include <map>
#include <string>

using namespace std;

typedef map<string, IModuleInterface*> ModuleMap;
class CModuleLoader
{
public:
	static IModuleInterface* LoadModule(char *moduleName, int pathIsAbsolute);
	static ModuleMap loadedModules;
private:
	CModuleLoader() {};
	virtual ~CModuleLoader() {};
};
