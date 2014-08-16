// TibiaTile.cpp: implementation of the CTibiaTile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "TibiaTile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaTile::CTibiaTile()
{
	goUp=0;
	goDown=0;
	blocking=0;
	speed=0;
	requireRope=0;
	requireUse=0;
	requireShovel=0;
	canWalkThrough=0;
	isDepot=0;
	isContainer=0;
	notMoveable=0;
	stackable=0;
	alwaysOnTop=0;
	moreAlwaysOnTop=0;

	ground=0;
	isTeleporter=5000;//useful for finding errors. This should never become user visible
	isReadable=0;
	isFluid=0;
	isRune=0;
	isUseable=0;
	isUseableImmobile=0;
	blockPathFind=0;
	minimapColor=0;
}

CTibiaTile::~CTibiaTile()
{

}
