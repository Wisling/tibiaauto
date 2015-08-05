#include "stdafx.h"

#include "mod_fluid.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_fluidApp();
}