#include "stdafx.h"

#include "mod_itemconfig.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_itemconfigApp();
}
