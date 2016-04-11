#include "StdAfx.h"
#include "IModuleInterface.h"

char * IModuleInterface::getName()
{
	return "not implemented";
}

int IModuleInterface::isStarted()
{
	return 0;
}

void IModuleInterface::start()
{

}

void IModuleInterface::stop()
{

}

void IModuleInterface::getNewSkin(CSkin newSkin)
{

}

void IModuleInterface::showConfigDialog()
{

}

void IModuleInterface::configToControls()
{

}

void IModuleInterface::controlsToConfig()
{

}

void IModuleInterface::disableControls()
{

}

void IModuleInterface::enableControls()
{

}

char * IModuleInterface::getVersion()
{
	return "-";
}

int IModuleInterface::validateConfig(int showAlerts)
{
	return 1;
}

void IModuleInterface::resetConfig()
{

}

void IModuleInterface::loadConfigParam(const char *paramName, char *paramValue)
{

}

char * IModuleInterface::saveConfigParam(const char *paramName)
{
	return "";
}


static const char *configParamNames[] =
{
	NULL,
};

const char ** IModuleInterface::getConfigParamNames()
{
	return configParamNames;
}

int IModuleInterface::isMultiParam(const char *paramName)
{
	return 0;
}

void IModuleInterface::resetMultiParamAccess(const char *paramName)
{

}

void IModuleInterface::activate(int kernelVersion)
{
	if (kernelVersion == 30)
		active = 12345;
}

void IModuleInterface::init()
{
	active = 0;
}

void IModuleInterface::superStart()
{
	if (active != 12345)
	{
		AfxMessageBox("Some module comes from an incorrect version of TA! Terminating.");
		PostQuitMessage(0);
	}
}

const char* IModuleInterface::getModuleName()
{
	return moduleName.c_str();
}

void IModuleInterface::setModuleName(const char* moduleName)
{
	this->moduleName = moduleName;
}
