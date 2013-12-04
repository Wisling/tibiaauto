// TibiaTile.h: interface for the CTibiaTile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIATILE_H__39D75BF9_16BF_4794_92F0_27A6875108E8__INCLUDED_)
#define AFX_TIBIATILE_H__39D75BF9_16BF_4794_92F0_27A6875108E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaTile
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

#endif // !defined(AFX_TIBIATILE_H__39D75BF9_16BF_4794_92F0_27A6875108E8__INCLUDED_)
