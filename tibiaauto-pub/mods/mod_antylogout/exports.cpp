#include "stdafx.h"

#include "mod_antylogout.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_antylogoutApp();
}
