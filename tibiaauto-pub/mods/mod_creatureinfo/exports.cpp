#include "stdafx.h"

#include "mod_creatureinfo.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_creatureinfoApp();
}