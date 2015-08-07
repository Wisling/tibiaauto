#include "stdafx.h"

#include "mod_playerinfo.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_playerinfoApp();
}