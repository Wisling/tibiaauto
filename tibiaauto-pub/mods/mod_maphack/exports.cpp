#include "stdafx.h"

#include "mod_maphack.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_maphackApp();
}