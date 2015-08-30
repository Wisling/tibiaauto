#pragma once

#ifdef TIBIAAUTOUTIL_EXPORTS
#define TIBIAAUTOUTIL_API __declspec(dllexport)
#else // ifdef TIBIAAUTOUTIL_EXPORTS
#define TIBIAAUTOUTIL_API __declspec(dllimport)
#endif // ifdef TIBIAAUTOUTIL_EXPORTS

#include "resource.h"           // main symbols

TIBIAAUTOUTIL_API int getKernelMainVersion();
TIBIAAUTOUTIL_API int getKernelPatchVersion();