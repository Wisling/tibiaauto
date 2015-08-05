#include "stdafx.h"

#include "mod_playerinfo.h"
/*
extern "C" void* EXPORT WINAPI GetPlayerInfo()
{
	return module.GetPlayerInfo();
}*/

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_playerinfoApp();
}