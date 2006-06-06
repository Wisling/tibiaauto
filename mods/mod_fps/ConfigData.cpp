// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_fps.h"
#include "ConfigData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigData::CConfigData()
{
	activeVal	= 40.0;
	inactive	= 0;
	inactiveVal = 10.0;
	minimized	= 0;
	minimizedVal= 1.0;
}

CConfigData::~CConfigData()
{

}
