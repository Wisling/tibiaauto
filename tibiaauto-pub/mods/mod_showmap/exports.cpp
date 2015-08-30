#include "stdafx.h"

#include "mod_showmap.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_showmapApp();
}