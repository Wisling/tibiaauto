#include "stdafx.h"

#include "mod_responder.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_responderApp();
}