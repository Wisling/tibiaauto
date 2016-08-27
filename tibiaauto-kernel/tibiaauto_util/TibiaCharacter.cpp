// TibiaCharacter.cpp: implementation of the CTibiaCharacter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaCharacter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaCharacter::CTibiaCharacter()
{
	initialized = false;
	visible = 0;
	name[0] = 0x00;
	voc[0] = 0x00;
	hp = -1;
	mana = -1;
}

CTibiaCharacter::~CTibiaCharacter()
{
}
