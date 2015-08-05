#include "stdafx.h"

#include "mod_aim.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_aimApp();
}

