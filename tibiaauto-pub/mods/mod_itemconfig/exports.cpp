#include "stdafx.h"

#include "mod_itemconfig.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_itemconfigApp();
}
