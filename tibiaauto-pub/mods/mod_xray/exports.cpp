#include "stdafx.h"

#include "mod_xray.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_xrayApp();
}