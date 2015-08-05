#include "stdafx.h"

#include "mod_playerinfo.h"
/*
extern "C" void* EXPORT WINAPI GetPlayerInfo()
{
	return module.GetPlayerInfo();
}*/

IModuleInterface* initModuleInstance()
{
	return new CMod_playerinfoApp();
}