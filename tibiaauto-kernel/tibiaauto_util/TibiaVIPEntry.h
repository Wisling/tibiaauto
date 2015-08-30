#pragma once
#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CTibiaVIPEntry
{
public:
	CTibiaVIPEntry();
	virtual ~CTibiaVIPEntry();

	int id;
	int nameLen;
	char name[64];//maxLen=29 without null
	int descrLen;
	char descr[200];//maxLen=128 without null
	unsigned int loginTm;
	int status;
	int icon;
	int notify;
};
