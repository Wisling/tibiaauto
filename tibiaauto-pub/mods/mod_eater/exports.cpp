#include "stdafx.h"

#include "mod_eater.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_eaterApp();
}