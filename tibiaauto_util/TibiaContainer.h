// TibiaContainer.h: interface for the CTibiaContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIACONTAINER_H__F994C191_09E2_497B_B20A_FF311B86858B__INCLUDED_)
#define AFX_TIBIACONTAINER_H__F994C191_09E2_497B_B20A_FF311B86858B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaItem.h"

class CTibiaContainer
{
public:
	int countItemsOfType(int objectId);
	CTibiaContainer();
	virtual ~CTibiaContainer();

	int flagOnOff;
	int objectId;
	int size;
	int number;
	int itemsInside;
	CPtrArray items;
};

#endif // !defined(AFX_TIBIACONTAINER_H__F994C191_09E2_497B_B20A_FF311B86858B__INCLUDED_)
