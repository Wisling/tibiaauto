/*
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


// IModuleInterface.cpp: implementation of the IModuleInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IModuleInterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Exports
//////////////////////////////////////////////////////////////////////


extern "C" char * EXPORT getName()
{
	return module.getName();
}

extern "C" void EXPORT start()
{
	module.start();
}

extern "C" void EXPORT stop()
{
	module.stop();
}

extern "C" int EXPORT isStarted()
{
	return module.isStarted();
}

extern "C" void EXPORT showConfigDialog()
{
	module.showConfigDialog();
}

extern "C"  void EXPORT configToControls()
{
	module.configToControls();
}

extern "C"  void EXPORT controlsToConfig()
{
	module.controlsToConfig();
}

extern "C"  void EXPORT disableControls()
{
	module.disableControls();
}

extern "C"  void EXPORT enableControls()
{
	module.enableControls();
}

extern "C"  char* EXPORT getVersion()
{
	return module.getVersion();
}

extern "C"  int EXPORT validateConfig(int showAlerts)
{
	return module.validateConfig(showAlerts);
}

extern "C"  void EXPORT resetConfig()
{
	module.resetConfig();
}

extern "C"  void EXPORT loadConfigParam(char *paramName, char *paramValue)
{
	module.loadConfigParam(paramName, paramValue);
}

extern "C"  char * EXPORT saveConfigParam(char *paramName)
{
	return module.saveConfigParam(paramName);
}


extern "C"  char * EXPORT getConfigParamName(int nr)
{
	return module.getConfigParamName(nr);
}

extern "C" void EXPORT resetMultiParamAccess(char *paramName)
{
	module.resetMultiParamAccess(paramName);
}

extern "C" int EXPORT isMultiParam(char *paramName)
{
	return module.isMultiParam(paramName);
}

extern "C" void EXPORT init()
{
	module.init();
}

extern "C" void EXPORT activate(int kernelVersion)
{
	module.activate(kernelVersion);
}

extern "C" void EXPORT getNewSkin(CSkin newSkin)
{
	module.getNewSkin(newSkin);
}
