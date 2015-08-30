#include "stdafx.h"

#include "mod_cavebot.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_cavebotApp();
}