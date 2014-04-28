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
	memset(path,0,15);
	targetX = 0;
	targetY = 0;
	targetZ = 0;
	for (int i = 0; i < MAX_SELLERS; i++) {
		sellerList[i].sellerName[0] = 0;
		sellerList[i].sellerX = 0;
		sellerList[i].sellerY = 0;
		sellerList[i].sellerZ = 0;
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
	sellOnCap = 1;
	sellOnSpace = 1;
	sellWhen = 50;
	strcpy(modPriorityStr,"2");
	stopBySeller=0;
	suggestBanker=1;
}

CConfigData::~CConfigData() {
}
