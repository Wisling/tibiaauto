#include "stdafx.h"

#include "mod_addressfinder.h"

IModuleInterface* initModuleInstance()
{
	CMod_addressfinderApp* instance = new CMod_addressfinderApp();
	return instance;
}
