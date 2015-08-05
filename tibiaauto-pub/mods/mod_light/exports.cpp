#include "stdafx.h"

#include "mod_light.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new Cmod_lightApp();
}
