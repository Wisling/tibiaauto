// ModuleProxy.h: interface for the CModuleProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODULEPROXY_H__466E0961_7FC2_412F_968C_048EC6D2A8C0__INCLUDED_)
#define AFX_MODULEPROXY_H__466E0961_7FC2_412F_968C_048EC6D2A8C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IModuleInterface.h>

class CModuleProxy : public IModuleInterface
{
public:
	char * getModuleName();
	CModuleProxy(char *moduleName, int pathIsAbsolute);
	virtual ~CModuleProxy();

	char * getName();
	void getNewSkin(CSkin);
	int isStarted();
	void start();
	void stop();
	void showConfigDialog();
	void configToControls();
	void controlsToConfig();
	void disableControls();
	void enableControls();	
	char *getVersion();
	void resetConfig();
	void loadConfigParam(char *paramName,char *paramValue);
	char *saveConfigParam(char *paramName);
	char *getConfigParamName(int nr);
	int isMultiParam(char *paramName);
	void resetMultiParamAccess(char *paramName);

 
	int isLoaded();

public:
	void activate(int kernelVersion);
	void init();
	int validateConfig(int showAlerts);
	static CModuleProxy ** allModules;
	static int allModulesCount;	

	char moduleName[16384];
private:	
	HMODULE dllModule;
	static int allModulesSize;
	
};

#endif // !defined(AFX_MODULEPROXY_H__466E0961_7FC2_412F_968C_048EC6D2A8C0__INCLUDED_)
