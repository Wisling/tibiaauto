#include "stdafx.h"

#include "mod_aim.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_aimApp();
}

