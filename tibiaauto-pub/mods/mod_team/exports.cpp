#include "stdafx.h"

#include "mod_team.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_teamApp();
}