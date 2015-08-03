#pragma once
#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CTibiaMiniMapLabel
{
public:
	CTibiaMiniMapLabel();
	virtual ~CTibiaMiniMapLabel();
	int x;
	int y;
	int type;
	char desc[100];
};
