// TibiaContainer.cpp: implementation of the CTibiaContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaContainer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTibiaContainer::CTibiaContainer()
{
	itemsInside = 0;
	items.RemoveAll();
	flagOnOff = 0;
	objectId  = 0;
	size      = 0;
}

CTibiaContainer::~CTibiaContainer()
{
	int pos;
	int len = items.GetSize();
	for (pos = 0; pos < len; pos++)
	{
		CTibiaItem *item = (CTibiaItem *)items.GetAt(pos);
		delete item;
	}
	;
}

int CTibiaContainer::countItemsOfType(int objectId)
{
	int i;
	int ret = 0;
	for (i = 0; i < itemsInside; i++)
	{
		if (((CTibiaItem *)items.GetAt(i))->objectId == objectId)
		{
			int c = ((CTibiaItem *)items.GetAt(i))->quantity;
			ret += c ? c : 1;
		}
	}
	;
	return ret;
}
