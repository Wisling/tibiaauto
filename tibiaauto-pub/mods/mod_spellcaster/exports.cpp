#include "stdafx.h"

#include "mod_spellcaster.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_spellcasterApp();
}