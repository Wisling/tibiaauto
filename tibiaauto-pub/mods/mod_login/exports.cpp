#include "stdafx.h"

#include "mod_login.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_loginApp();
}