// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_restack.h"
#include "ConfigData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigData::CConfigData()
{
	ammoType       = 0;
	ammoAt         = 0;
	ammoTo         = 0;
	throwableType  = 0;
	throwableAt    = 0;
	throwableTo    = 0;
	pickupSpears   = 0;
	pickupUL       = 1;
	pickupUC       = 1;
	pickupUR       = 1;
	pickupCL       = 1;
	pickupCC       = 1;
	pickupCR       = 1;
	pickupBL       = 1;
	pickupBC       = 1;
	pickupBR       = 1;
	moveCovering   = 1;
	restackToRight = 0;
	pickupToHand   = 0;
	periodFrom     = periodTo = 0;
	capLimit       = 20;
}

CConfigData::~CConfigData()
{
}
