// TibiaItem.h: interface for the CTibiaItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAITEM_H__65F1AF50_58EE_4C63_8E79_E5BC61BD74C9__INCLUDED_)
#define AFX_TIBIAITEM_H__65F1AF50_58EE_4C63_8E79_E5BC61BD74C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ITEMS 200
#define MAX_ITEM_LEN 60

class CTibiaItem
{
public:
	static char * getName(int objectId);
	static void refreshItemLists();
	CTibiaItem();

	int objectId;
	int quantity;
	int pos;

	
};

#endif // !defined(AFX_TIBIAITEM_H__65F1AF50_58EE_4C63_8E79_E5BC61BD74C9__INCLUDED_)
