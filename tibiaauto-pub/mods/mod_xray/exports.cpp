#include "stdafx.h"

#include "mod_xray.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_xrayApp();
}