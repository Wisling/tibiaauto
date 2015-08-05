#include "stdafx.h"

#include "mod_antylogout.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_antylogoutApp();
}
