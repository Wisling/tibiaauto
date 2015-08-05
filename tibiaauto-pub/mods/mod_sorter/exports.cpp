#include "stdafx.h"

#include "mod_sorter.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_sorterApp();
}