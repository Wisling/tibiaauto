#include "stdafx.h"

#include "mod_fisher.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_fisherApp();
}