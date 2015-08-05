#include "stdafx.h"

#include "mod_monstershow.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_monstershowApp();
}