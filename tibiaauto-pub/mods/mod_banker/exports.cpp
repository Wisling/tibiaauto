#include "stdafx.h"

#include "mod_banker.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_bankerApp();
}
