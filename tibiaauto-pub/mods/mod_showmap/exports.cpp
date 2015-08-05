#include "stdafx.h"

#include "mod_showmap.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_showmapApp();
}