#include "stdafx.h"

#include "mod_cavebot.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_cavebotApp();
}