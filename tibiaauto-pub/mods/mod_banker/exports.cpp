#include "stdafx.h"

#include "mod_banker.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_bankerApp();
}
