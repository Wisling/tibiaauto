#include "stdafx.h"

#include "mod_playerinfo.h"

CMod_playerinfoApp module;

#include <IModuleInterface.cpp>

extern "C" void* EXPORT WINAPI GetPlayerInfo()
{
	return module.GetPlayerInfo();
}
