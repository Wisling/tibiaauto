#include "stdafx.h"

#include "mod_uh.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_uhApp();
}