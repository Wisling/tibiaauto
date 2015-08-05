#include "stdafx.h"

#include "mod_fps.h"

IModuleInterface* initModuleInstance()
{
	return new Cmod_fpsApp();
}