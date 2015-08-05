#include "stdafx.h"

#include "mod_addressfinder.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_addressfinderApp();
}
