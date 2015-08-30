#include "stdafx.h"

#include "mod_maphack.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_maphackApp();
}