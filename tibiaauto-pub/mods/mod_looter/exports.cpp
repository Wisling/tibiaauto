#include "stdafx.h"

#include "mod_looter.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_looterApp();
}