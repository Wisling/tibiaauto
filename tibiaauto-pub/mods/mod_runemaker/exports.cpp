#include "stdafx.h"

#include "mod_runemaker.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_runemakerApp();
}