#pragma once
#define MAX_SELLERS 4
#define MAX_SELLER_ITEMS 300
#include <cstdint>

struct Item {
	char itemName[64];
	int salePrice;
	int quantityBuySell;
	int triggerQuantity;
};

struct ItemList {
	Item tradeItem[MAX_SELLER_ITEMS];
};

struct Seller {
	char sellerName[64];
	int sellerX;
	int sellerY;
	int sellerZ;
};

class CConfigData {
public:
	CConfigData();
	virtual ~CConfigData();

	uint8_t path[15];
	int targetX;
	int targetY;
	int targetZ;
	Seller sellerList[MAX_SELLERS];
	ItemList sellItem[MAX_SELLERS];
	ItemList buyItem[MAX_SELLERS];
	int sellOnCap;
	int sellWhen;
	int sellOnSpace;
	char modPriorityStr[2];
	int stopBySeller;
	int suggestBanker;
};