#include "stdafx.h"

#include "mod_uh.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_uhApp();
}