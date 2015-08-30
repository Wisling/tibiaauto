#include "stdafx.h"

#include "mod_seller.h"

IModuleInterface* initModuleInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CMod_SellerApp();
}