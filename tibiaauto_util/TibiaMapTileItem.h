// TibiaMapTileItem.h: interface for the CTibiaMapTileItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAMAPTILEITEM_H__E4092406_EA42_4B13_A38A_50B55EEF0777__INCLUDED_)
#define AFX_TIBIAMAPTILEITEM_H__E4092406_EA42_4B13_A38A_50B55EEF0777__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaMapTileItem
{
public:
	CTibiaMapTileItem();
	int extra;
	int quantity;
	int itemId;
	int val1;
	int val2;
	int val3;
	int val4;
	int val5;
	int val6;

};

class CTibiaMapTileItemAddress
{
public:
	CTibiaMapTileItemAddress();
	CTibiaMapTileItemAddress(int initAddr/*=0*/);
	int extra;
	int quantity;
	int itemId;
	int val1;
	int val2;
	int val3;
	int val4;
	int val5;
	int val6;

};
#endif // !defined(AFX_TIBIAMAPTILEITEM_H__E4092406_EA42_4B13_A38A_50B55EEF0777__INCLUDED_)
