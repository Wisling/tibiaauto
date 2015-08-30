#pragma once

#include "tibiaauto_util.h"
#include "Skin.h"

TIBIAAUTOUTIL_API bool saveSkin(CString, CSkin, bool);
TIBIAAUTOUTIL_API CSkin loadCurrentSkin(CString);
TIBIAAUTOUTIL_API CSkin loadSkin(CString);
