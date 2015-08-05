#include "stdafx.h"

#include "mod_runemaker.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_runemakerApp();
}