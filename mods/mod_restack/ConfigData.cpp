// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_restack.h"
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
	ammoType=0;
	ammoAt=0;
	ammoTo=0;
	throwableType=0;
	throwableAt=0;
	throwableTo=0;
	pickupSpears=0;
	pickupUL=0;
	pickupUC=0;
	pickupUR=0;	
	pickupCL=0;
	pickupCR=0;
	pickupBL=0;
	pickupBC=0;
	pickupBR=0;
	moveCovering=1;
	restackToRight=0;
	pickupToHand=0;
}

CConfigData::~CConfigData()
{

}
