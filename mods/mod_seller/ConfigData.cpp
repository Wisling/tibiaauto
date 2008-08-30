// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_seller.h"
#include "ConfigData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigData::CConfigData() {
	for (int x = 0; x < 15; x++)
		path[x] = 0;
	targetX = 0;
	targetY = 0;
	targetZ = 0;
	quantity=0;
	price=0;
	for (int i = 0; i < MAX_SELLERS; i++) {
		for (x = 0; x < 10; x++) {
			sellerList[i].position[x].sellerX = 0;
			sellerList[i].position[x].sellerY = 0;
			sellerList[i].position[x].sellerZ = 0;
		}
		for (int j = 0; j < 32; j++) {
			sellItem[i].tradeItem[j].itemName[0] = 0;
			sellItem[i].tradeItem[j].salePrice = 0;
			sellItem[i].tradeItem[j].quantityBuySell = 0;
			sellItem[i].tradeItem[j].triggerQuantity = 0;
			buyItem[i].tradeItem[j].itemName[0] = 0;
			buyItem[i].tradeItem[j].salePrice = 0;
			buyItem[i].tradeItem[j].quantityBuySell = 0;
			buyItem[i].tradeItem[j].triggerQuantity = 0;
		}
	}
}

CConfigData::~CConfigData() {
}
