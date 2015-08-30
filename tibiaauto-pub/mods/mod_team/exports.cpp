#include "stdafx.h"

#include "mod_team.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_teamApp();
}