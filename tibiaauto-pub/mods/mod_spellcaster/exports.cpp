#include "stdafx.h"

#include "mod_spellcaster.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_spellcasterApp();
}