#include "stdafx.h"

#include "mod_trademon.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_trademonApp();
}