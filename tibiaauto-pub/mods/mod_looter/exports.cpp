#include "stdafx.h"

#include "mod_looter.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_looterApp();
}