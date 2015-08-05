#include "stdafx.h"

#include "mod_monstershow.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_monstershowApp();
}