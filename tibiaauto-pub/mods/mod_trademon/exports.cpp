#include "stdafx.h"

#include "mod_trademon.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_trademonApp();
}