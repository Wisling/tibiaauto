#include "stdafx.h"

#include "mod_fps.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new Cmod_fpsApp();
}