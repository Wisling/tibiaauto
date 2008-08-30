// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct Item {
	char itemName[64];
	int salePrice;
	int quantityBuySell;
	int triggerQuantity;
};

struct ItemList {
	Item tradeItem[32];
};

struct ValidPosition {
	int sellerX;
	int sellerY;
	int sellerZ;
};
struct Seller {
	ValidPosition position[10];
};

class CConfigData {
public:
	CConfigData();
	virtual ~CConfigData();

	int path[15];
	int targetX;
	int targetY;
	int targetZ;
	Seller sellerList[MAX_SELLERS];
	int quantity;
	int price;
	ItemList sellItem[MAX_SELLERS];
	ItemList buyItem[MAX_SELLERS];
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
