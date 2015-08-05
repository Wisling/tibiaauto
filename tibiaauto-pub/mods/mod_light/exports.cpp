#include "stdafx.h"

#include "mod_light.h"

IModuleInterface* initModuleInstance()
{
	return new Cmod_lightApp();
}
