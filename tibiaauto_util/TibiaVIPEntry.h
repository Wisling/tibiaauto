// TibiaVIPEntry.h: interface for the CTibiaVIPEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAVIPENTRY_H__F994C191_09E2_497B_B20A_FF311B86858B__INCLUDED_)
#define AFX_TIBIAVIPENTRY_H__F994C191_09E2_497B_B20A_FF311B86858B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaVIPEntry
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

#endif
