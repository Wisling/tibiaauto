#include "stdafx.h"

#include "mod_eater.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_eaterApp();
}