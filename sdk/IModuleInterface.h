// IModuleInterface.h: interface for the IModuleInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMODULEINTERFACE_H__B3A3B5DF_77D6_4BA1_B763_A1B01B3DC640__INCLUDED_)
#define AFX_IMODULEINTERFACE_H__B3A3B5DF_77D6_4BA1_B763_A1B01B3DC640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Skin.h"

class IModuleInterface
{
public:
	virtual char * getName()
	{
		return "not implemented";
	}
	virtual int isStarted()
	{
		return 0;
	}
	virtual void start()
	{
	}
	virtual void stop()
	{
	}
	virtual void getNewSkin(CSkin newSkin)
	{
	}
	virtual void showConfigDialog()
	{
	}
	virtual void configToControls()
	{
	}
	virtual void controlsToConfig()
	{
	}
	virtual void disableControls()
	{
	}
	virtual void enableControls()
	{
	}
	virtual char *getVersion()
	{
		return "-";
	}
	virtual int validateConfig(int showAlerts)
	{
		return 1;
	}
	virtual void resetConfig()
	{
	}
	virtual void loadConfigParam(char *paramName,char *paramValue)
	{
	}
	virtual char *saveConfigParam(char *paramName)
	{
		return "";
	}
	virtual char *getConfigParamName(int nr)
	{
		return NULL;
	}
	
	virtual int isMultiParam(char *paramName)
	{
		return 0;
	}

	virtual void resetMultiParamAccess(char *paramName)
	{
	}
	virtual void activate(int kernelVersion)
	{
		if (kernelVersion==30)
		{
			active=12345;
		}
	}
	virtual void init()
	{
		active=0;
	}
	virtual void superStart()
	{
		if (active!=12345)
		{
			AfxMessageBox("Some module comes from an incorrect version of TA! Terminating.");
			ExitProcess(0);
		}
	}
protected:
	int active;
};

#endif // !defined(AFX_IMODULEINTERFACE_H__B3A3B5DF_77D6_4BA1_B763_A1B01B3DC640__INCLUDED_)
