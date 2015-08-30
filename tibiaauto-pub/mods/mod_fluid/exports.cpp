#include "stdafx.h"

#include "mod_fluid.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_fluidApp();
}