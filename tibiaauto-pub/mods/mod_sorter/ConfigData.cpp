// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_sorter.h"
#include "ConfigData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CConfigData::CConfigData() {
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			sortBags[i].slotNr[j].itemName[0] = 0;
		}
	}
}

CConfigData::~CConfigData() {
}
