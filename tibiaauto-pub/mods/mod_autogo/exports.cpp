#include "stdafx.h"

#include "mod_autogo.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_autogoApp();
}
