#include "stdafx.h"

#include "mod_restack.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_restackApp();
}