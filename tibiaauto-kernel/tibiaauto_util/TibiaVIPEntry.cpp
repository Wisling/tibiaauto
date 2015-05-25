// TibiaVIPEntry.cpp: implementation of the CTibiaVIPEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaVIPEntry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaVIPEntry::CTibiaVIPEntry()
{
	memset(name, 0x00, 64);
}

CTibiaVIPEntry::~CTibiaVIPEntry()
{
}
