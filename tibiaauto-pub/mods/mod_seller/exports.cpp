#include "stdafx.h"

#include "mod_seller.h"

IModuleInterface* initModuleInstance()
{
	return new CMod_SellerApp();
}