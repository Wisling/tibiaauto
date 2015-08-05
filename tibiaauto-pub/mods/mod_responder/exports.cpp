#include "stdafx.h"

#include "mod_responder.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_responderApp();
}