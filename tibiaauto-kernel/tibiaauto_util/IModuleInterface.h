#pragma once

#include "tibiaauto_util.h"
#include "Skin.h"

class TIBIAAUTOUTIL_API IModuleInterface
{
public:
	virtual ~IModuleInterface() { }
	virtual char * getName();
	virtual int isStarted();
	virtual void start();
	virtual void stop();
	virtual void getNewSkin(CSkin newSkin);
	virtual void showConfigDialog();
	virtual void configToControls();
	virtual void controlsToConfig();
	virtual void disableControls();
	virtual void enableControls();
	virtual char *getVersion();
	virtual int validateConfig(int showAlerts);
	virtual void resetConfig();
	virtual void loadConfigParam(char *paramName, char *paramValue);
	virtual char *saveConfigParam(char *paramName);
	virtual char *getConfigParamName(int nr);
	virtual int isMultiParam(char *paramName);
	virtual void resetMultiParamAccess(char *paramName);
	virtual void activate(int kernelVersion);
	virtual void init();
	virtual void superStart();
	const char* getModuleName();
	void setModuleName(const char* moduleName);
protected:
	std::string moduleName;
	int active;
};
