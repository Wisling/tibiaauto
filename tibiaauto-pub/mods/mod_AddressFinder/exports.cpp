#include "stdafx.h"

#include "mod_addressfinder.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_addressfinderApp();
}
