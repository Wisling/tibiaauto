#include "stdafx.h"

#include "mod_login.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_loginApp();
}