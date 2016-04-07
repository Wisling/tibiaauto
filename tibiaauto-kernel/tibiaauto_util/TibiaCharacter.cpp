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
	memset(name, 0x00, 32);
}

CTibiaCharacter::~CTibiaCharacter()
{
}
