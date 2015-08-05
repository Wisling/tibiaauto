#include "stdafx.h"

#include "mod_grouping.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_groupingApp();
}
