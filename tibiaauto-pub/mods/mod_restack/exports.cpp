#include "stdafx.h"

#include "mod_restack.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_restackApp();
}