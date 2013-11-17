// ModuleProxy.cpp: implementation of the CModuleProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "ModuleProxy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Static definitions
//////////////////////////////////////////////////////////////////////

CModuleProxy ** CModuleProxy::allModules=NULL;
int CModuleProxy::allModulesCount=0;	
int CModuleProxy::allModulesSize=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModuleProxy::CModuleProxy(char *moduleName,int pathIsAbsolute)
{
	strcpy(this->moduleName,moduleName);

	// load module
	char path[128];
	if (!pathIsAbsolute)
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
			AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
			exit(1);
		}
		
		sprintf(path,"%s\\mods\\%s.dll",installPath,moduleName);	
	} else {
		strcpy(path,moduleName);
	}
	dllModule=LoadLibrary(path);

	// put the module onto our static list
	if (allModulesCount==allModulesSize)
	{
		allModulesSize=allModulesSize*2+3;
		allModules=(CModuleProxy **)realloc(allModules,sizeof(CModuleProxy *)*allModulesSize);
	}
	allModules[allModulesCount++]=this;	
	init();
	// 30 is the current kernel version
	activate(30);
}

CModuleProxy::~CModuleProxy()
{
}

//////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////

char * CModuleProxy::getName()
{
	typedef char *(*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getName");
		if (fun)
		{
			return fun();
		} else {
			return "not implemented";
		}
	} else {
		return "not loaded";
	}
}

int CModuleProxy::isStarted()
{	
	typedef int(*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"isStarted");
		if (fun)
		{
			return fun();
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

void CModuleProxy::start()
{
	if (!isStarted())
	{
		typedef void (*Proto_fun)();
		if (dllModule)
		{
			Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"start");
			if (fun)
			{
				fun();
			}
		}
	}
}

void CModuleProxy::stop()
{
	if (isStarted())
	{
		typedef void (*Proto_fun)();
		if (dllModule)
		{
			Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"stop");
			if (fun)
			{
				fun();
			}
		}
	}
}

void CModuleProxy::getNewSkin(CSkin newSkin)
{
	typedef void ( *Proto_fun)(CSkin);
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getNewSkin");
		if (fun)
		{
			fun(newSkin);
		}
	}
}

void CModuleProxy::showConfigDialog()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"showConfigDialog");
		if (fun)
		{
			fun();
		}
	}
}

void CModuleProxy::configToControls()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"configToControls");	
		if (fun)
		{
			fun();
		}
	}
}

void CModuleProxy::controlsToConfig()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"controlsToConfig");
		if (fun)
		{
			fun();
		}
	}
}
void CModuleProxy::disableControls()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"disableControls");
		if (fun)
		{
			fun();
		}
	}
}

void CModuleProxy::enableControls()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"enableControls");
		if (fun)
		{
			fun();
		}
	}
}

char *CModuleProxy::getVersion()
{
	typedef char *(*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getVersion");
		if (fun)
		{
			return fun();
		} else {
			return "not implemented";
		}
	} else {
		return "not loaded";
	}
}

int CModuleProxy::isLoaded()
{
	return (dllModule!=NULL);
}

char * CModuleProxy::getModuleName()
{
	return moduleName;
}

int CModuleProxy::validateConfig(int showAlerts)
{
	typedef int(*Proto_fun)(int);
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"validateConfig");
		if (fun)
		{
			return fun(showAlerts);
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}


void CModuleProxy::resetConfig()
{
	typedef void(*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"resetConfig");
		if (fun)
		{
			fun();
		}
	}
}


void CModuleProxy::loadConfigParam(char *paramName,char *paramValue)
{
	typedef void(*Proto_fun)(char *,char *);
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"loadConfigParam");
		if (fun)
		{
			fun(paramName,paramValue);
		}
	}
}

char *CModuleProxy::saveConfigParam(char *paramName)
{
	typedef char *(*Proto_fun)(char *);
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"saveConfigParam");
		if (fun)
		{
			return fun(paramName);
		} else {
			return "not implemented";
		}
	} else {
		return "not loaded";
	}
}

char *CModuleProxy::getConfigParamName(int nr)
{
	typedef char *(*Proto_fun)(int);
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"getConfigParamName");
		if (fun)
		{
			return fun(nr);
		} else {
			return "not implemented";
		}
	} else {
		return "not loaded";
	}
}

int CModuleProxy::isMultiParam(char *paramName)
{
	typedef int (*Proto_fun)(char *);
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"isMultiParam");
		if (fun)
		{
			return fun(paramName);
		}
	}
	return 0;
}

void CModuleProxy::resetMultiParamAccess(char *paramName)
{
	typedef void(*Proto_fun)(char *);
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"resetMultiParamAccess");
		if (fun)
		{
			fun(paramName);
		}
	}	
}

void CModuleProxy::init()
{
	typedef void(*Proto_fun)();
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"init");
		if (fun)
		{
			fun();
		}
	}
}

void CModuleProxy::activate(int kernelVersion)
{
	typedef void(*Proto_fun)(int);
	if (dllModule)
	{
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"activate");
		if (fun)
		{
			fun(kernelVersion);
		}
	}
}
