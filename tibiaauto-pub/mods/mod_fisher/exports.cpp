#include "stdafx.h"

#include "mod_fisher.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_fisherApp();
}