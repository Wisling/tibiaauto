#include "stdafx.h"

#include "mod_creatureinfo.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_creatureinfoApp();
}