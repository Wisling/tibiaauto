#include "stdafx.h"

#include "mod_memdebug.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_memdebugApp();
}