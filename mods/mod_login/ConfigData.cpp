// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_login.h"
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
	accountNumber[0]=password[0]='\0';
	charPos=1;
	openMain=1;
	openCont1=openCont2=openCont3=openCont4=0;
	openCont5=openCont6=openCont7=openCont8=0;
	loginDelay=600;
	autopass=1;
}

CConfigData::~CConfigData()
{

}
