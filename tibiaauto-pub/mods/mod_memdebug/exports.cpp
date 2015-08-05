#include "stdafx.h"

#include "mod_memdebug.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_memdebugApp();
}