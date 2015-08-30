#include "stdafx.h"

#include "mod_autogo.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_autogoApp();
}
