#pragma once

#include <IModuleInterface.h>
#include <map>
#include <string>

using namespace std;

typedef map<string, IModuleInterface*> ModuleMap;
class TIBIAAUTOUTIL_API CModuleLoader
{
public:
	static IModuleInterface* LoadModule(const char *moduleName, int pathIsAbsolute);
	static bool IsCavebotOn();
	static IModuleInterface* GetLoadedModule(const char* moduleName);
	static ModuleMap loadedModules;
private:
	CModuleLoader() {};
	virtual ~CModuleLoader() {};
};
