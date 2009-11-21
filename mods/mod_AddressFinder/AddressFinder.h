// AddressFinder.h: interface for the AddressFinder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADDRESSFINDER_H__14AB440F_93F5_4FD2_83C1_29FEFE428238__INCLUDED_)
#define AFX_ADDRESSFINDER_H__14AB440F_93F5_4FD2_83C1_29FEFE428238__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Addresses.h"
#include <List>

using namespace std;

class AddressFinder  
{
public:
	long getFlagsAddress();
	bool getFlagsComp();
	long getExpAddress();
	long doComparison();
	bool getTibiaFileChunk(int);
	bool getExpComp();
	AddressFinder();
	virtual ~AddressFinder();
	Addresses addresses;

private:
	
	list <byte> TibiaFile;
	list <byte> Comparison;
	list <int>	Mask;
	list<byte>::iterator FileIterator;
	list<byte>::iterator ComparisonIterator;
	list<int>::iterator MaskIterator;
};

#endif // !defined(AFX_ADDRESSFINDER_H__14AB440F_93F5_4FD2_83C1_29FEFE428238__INCLUDED_)
