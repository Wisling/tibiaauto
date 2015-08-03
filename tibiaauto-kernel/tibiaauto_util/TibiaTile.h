#pragma once
#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CTibiaTile
{
public:
	CTibiaTile();
	virtual ~CTibiaTile();
	int ground;
	int goUp;
	int goDown;
	int blocking;
	int speed;
	int requireRope;
	int requireUse;
	int requireShovel;
	int canWalkThrough;
	int isDepot;
	int isContainer;
	int notMoveable;
	int stackable;
	int alwaysOnTop;
	int moreAlwaysOnTop;
	int isTeleporter;
	int isReadable;
	int isFluid;
	int isRune;
	int isUseable;
	int isUseableImmobile;
	int blockPathFind;
	int minimapColor;
};

