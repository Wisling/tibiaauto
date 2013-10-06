// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_uh.h"
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
	m_sleepAfter=2000;
	m_uhBorderline=0;
	m_runetype=0;
	m_fallback=1;
	m_hotkeySelf=0;
	m_grpBorderline=0;
	m_grpFallback=1;
	m_grpRunetype=0;
	m_grpMemberCount=0;	
}

CConfigData::~CConfigData()
{

}
