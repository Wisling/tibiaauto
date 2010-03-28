// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_looter.h"
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
	m_lootInDepot=0;
	m_lootWorms=0;
	m_lootGp=0;
	m_lootFood=0;
	m_eatFromCorpse=0;
	m_lootCustom=0;
	m_autoOpen=0;
	m_mode10=2;
	m_mode9=2;
	m_mode8=2;
	m_mode7=2;
	m_mode6=2;
	m_mode5=2;
	m_mode4=2;
	m_mode3=2;
	m_mode2=2;
	m_mode1=1;
}

CConfigData::~CConfigData()
{

}
