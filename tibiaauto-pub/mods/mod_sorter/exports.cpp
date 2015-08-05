#include "stdafx.h"

#include "mod_sorter.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_sorterApp();
}