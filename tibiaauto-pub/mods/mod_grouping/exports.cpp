#include "stdafx.h"

#include "mod_grouping.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_groupingApp();
}
