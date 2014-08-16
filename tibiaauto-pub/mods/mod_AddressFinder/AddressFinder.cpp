// AddressFinder.cpp: implementation of the AddressFinder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_addressfinder.h"
#include "AddressFinder.h"
#include "MemReaderProxy.h"
#include <List>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AddressFinder::AddressFinder()
{
	
}

AddressFinder::~AddressFinder()
{
	
}

bool AddressFinder::getExpComp()
{
	bool result = false;
	Comparison.clear();
	Mask.clear();
	
	Comparison.push_back(0xa1);
	Mask.push_back(1);
	Comparison.push_back(0x00);
	Mask.push_back(0);
	Comparison.push_back(0x00);
	Mask.push_back(0);
	Comparison.push_back(0x00);
	Mask.push_back(0);
	Comparison.push_back(0x00);
	Mask.push_back(0);
	Comparison.push_back(0x40);
	Mask.push_back(1);
	Comparison.push_back(0x83);
	Mask.push_back(1);
	Comparison.push_back(0xf8);
	Mask.push_back(1);
	Comparison.push_back(0x01);
	Mask.push_back(1);
	Comparison.push_back(0x7d);
	Mask.push_back(1);
	Comparison.push_back(0x07);
	Mask.push_back(1);
	Comparison.push_back(0xb8);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0x7f);
	Mask.push_back(1);
	Comparison.push_back(0xeb);
	Mask.push_back(1);
	Comparison.push_back(0x2e);
	Mask.push_back(1);
	Comparison.push_back(0x3d);
	Mask.push_back(1);
	Comparison.push_back(0xf4);
	Mask.push_back(1);
	Comparison.push_back(0x01);
	Mask.push_back(1);
	Comparison.push_back(0x00);
	Mask.push_back(1);
	Comparison.push_back(0x00);
	Mask.push_back(1);
	Comparison.push_back(0x7e);
	Mask.push_back(1);
	Comparison.push_back(0x07);
	Mask.push_back(1);
	Comparison.push_back(0xb8);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0x7f);
	Mask.push_back(1);
	Comparison.push_back(0xeb);
	Mask.push_back(1);
	Comparison.push_back(0x20);
	Mask.push_back(1);
	Comparison.push_back(0x8d);
	Mask.push_back(1);
	Comparison.push_back(0x48);
	Mask.push_back(1);
	Comparison.push_back(0xfa);
	Mask.push_back(1);
	Comparison.push_back(0x0f);
	Mask.push_back(1);
	Comparison.push_back(0xaf);
	Mask.push_back(1);
	Comparison.push_back(0xc8);
	Mask.push_back(1);
	Comparison.push_back(0x83);
	Mask.push_back(1);
	Comparison.push_back(0xc1);
	Mask.push_back(1);
	Comparison.push_back(0x11);
	Mask.push_back(1);
	Comparison.push_back(0x0f);
	Mask.push_back(1);
	Comparison.push_back(0xaf);
	Mask.push_back(1);
	Comparison.push_back(0xc8);
	Mask.push_back(1);
	Comparison.push_back(0x83);
	Mask.push_back(1);
	Comparison.push_back(0xe9);
	Mask.push_back(1);
	Comparison.push_back(0x0c);
	Mask.push_back(1);
	Comparison.push_back(0xb8);
	Mask.push_back(1);
	Comparison.push_back(0xab);
	Mask.push_back(1);
	Comparison.push_back(0xaa);
	Mask.push_back(1);
	Comparison.push_back(0xaa);
	Mask.push_back(1);
	Comparison.push_back(0x2a);
	Mask.push_back(1);
	Comparison.push_back(0xf7);
	Mask.push_back(1);
	Comparison.push_back(0xe9);
	Mask.push_back(1);
	Comparison.push_back(0x8b);
	Mask.push_back(1);
	Comparison.push_back(0xc2);
	Mask.push_back(1);
	Comparison.push_back(0xc1);
	Mask.push_back(1);
	Comparison.push_back(0xe8);
	Mask.push_back(1);
	Comparison.push_back(0x1f);
	Mask.push_back(1);
	Comparison.push_back(0x03);
	Mask.push_back(1);
	Comparison.push_back(0xc2);
	Mask.push_back(1);
	Comparison.push_back(0x6b);
	Mask.push_back(1);
	Comparison.push_back(0xc0);
	Mask.push_back(1);
	Comparison.push_back(0x64);
	Mask.push_back(1);
	Comparison.push_back(0x56);
	Mask.push_back(1);
	Comparison.push_back(0x2b);
	Mask.push_back(1);
	Comparison.push_back(0x05);
	Mask.push_back(1);
	
	result = true;
	return result;
}

bool AddressFinder::getTibiaFileChunk(int size)
{
	bool result = false;
	CMemReaderProxy reader;
	
	for (int loop = 0; loop < size; loop++) {
		TibiaFile.push_back(reader.getMemIntValue(0x401000 + loop));
	}
	
	if (TibiaFile.size() == size)
		result = true;
	
	return result;
}

long AddressFinder::doComparison()
{
	long foundAddress;
	int index = 0x401000 + TibiaFile.size();
	CMemReaderProxy reader;
	while(index < 0x5b0ffe) {
		FileIterator = TibiaFile.begin();
		ComparisonIterator = Comparison.begin();
		MaskIterator = Mask.begin();
		int countSuccess = 0;
		while (FileIterator != TibiaFile.end() && ComparisonIterator != Comparison.end() && MaskIterator != Mask.end() && *MaskIterator * *FileIterator == *MaskIterator * *ComparisonIterator){
			countSuccess++;
			FileIterator++;
			ComparisonIterator++;
			MaskIterator++;
		}
		if (countSuccess == Comparison.size()) {
			foundAddress = index + 1;
			break;
		}
		else {

			TibiaFile.pop_front();
			TibiaFile.push_back(reader.getMemIntValue(++index));
		}
	}
	return foundAddress;
}

long AddressFinder::getExpAddress()
{
	long expAddress = 0;
	
	getExpComp();
	getTibiaFileChunk(Comparison.size());
	expAddress = doComparison();
	
	return expAddress;
}

bool AddressFinder::getFlagsComp()
{
	bool result = false;
	Comparison.clear();
	Mask.clear();
	
	Comparison.push_back(0x55);
	Mask.push_back(1);
	Comparison.push_back(0x8b);
	Mask.push_back(1);
	Comparison.push_back(0xec);
	Mask.push_back(1);
	Comparison.push_back(0x6a);
	Mask.push_back(1);

	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0x68);
	Mask.push_back(1);
	Comparison.push_back(0x5d);
	Mask.push_back(0);
	Comparison.push_back(0x9a);
	Mask.push_back(0);
	Comparison.push_back(0x58);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x64);
	Mask.push_back(1);
	Comparison.push_back(0xa1);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x50);
	Mask.push_back(1);
	Comparison.push_back(0x64);
	Mask.push_back(1);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x25);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x81);
	Mask.push_back(1);
	Comparison.push_back(0xec);
	Mask.push_back(1);
	Comparison.push_back(0x98);
	Mask.push_back(1);
	Comparison.push_back(0x1);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x53);
	Mask.push_back(1);
	Comparison.push_back(0x56);
	Mask.push_back(1);
	Comparison.push_back(0x57);
	Mask.push_back(1);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x65);
	Mask.push_back(1);
	Comparison.push_back(0xf0);
	Mask.push_back(1);
	Comparison.push_back(0x33);
	Mask.push_back(1);
	Comparison.push_back(0xf6);
	Mask.push_back(1);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x75);
	Mask.push_back(1);
	Comparison.push_back(0xfc);
	Mask.push_back(1);
	Comparison.push_back(0xb9);
	Mask.push_back(1);
	Comparison.push_back(0xa);
	Mask.push_back(0);
	Comparison.push_back(0x28);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x33);
	Mask.push_back(1);
	Comparison.push_back(0xc0);
	Mask.push_back(1);
	Comparison.push_back(0xbf);
	Mask.push_back(1);
	Comparison.push_back(0xf0);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0xf3);
	Mask.push_back(1);
	Comparison.push_back(0xab);
	Mask.push_back(1);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x45);
	Mask.push_back(1);
	Comparison.push_back(0xec);
	Mask.push_back(1);
	Comparison.push_back(0x3d);
	Mask.push_back(1);
	Comparison.push_back(0xfa);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x7d);
	Mask.push_back(1);
	Comparison.push_back(0x11);
	Mask.push_back(1);
	Comparison.push_back(0x8b);
	Mask.push_back(1);
	Comparison.push_back(0xc8);
	Mask.push_back(1);
	Comparison.push_back(0x69);
	Mask.push_back(1);
	Comparison.push_back(0xc9);
	Mask.push_back(1);
	Comparison.push_back(0xa4);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x0);
	Mask.push_back(1);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0xb1);
	Mask.push_back(1);
	Comparison.push_back(0xf0);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x40);
	Mask.push_back(1);
	Comparison.push_back(0xeb);
	Mask.push_back(1);
	Comparison.push_back(0xe5);
	Mask.push_back(0);
	Comparison.push_back(0x83);
	Mask.push_back(1);
	Comparison.push_back(0xc8);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0xa3);
	Mask.push_back(1);
	Comparison.push_back(0xec);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0xa3);
	Mask.push_back(1);
	Comparison.push_back(0xe8);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0xe0);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0xdc);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0xe8);
	Mask.push_back(1);
	Comparison.push_back(0xb2);
	Mask.push_back(0);
	Comparison.push_back(0x9e);
	Mask.push_back(0);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x90);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x8c);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x88);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x84);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0xb9);
	Mask.push_back(1);
	Comparison.push_back(0x30);
	Mask.push_back(0);
	Comparison.push_back(0xdb);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0xe8);
	Mask.push_back(1);
	Comparison.push_back(0x7e);
	Mask.push_back(0);
	Comparison.push_back(0x9d);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0xff);
	Mask.push_back(1);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x80);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x78);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x70);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x6c);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x7c);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x74);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x60);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x5c);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	Comparison.push_back(0x58);
	Mask.push_back(0);
	Comparison.push_back(0x3e);
	Mask.push_back(0);
	Comparison.push_back(0x63);
	Mask.push_back(0);
	Comparison.push_back(0x0);
	Mask.push_back(0);
	Comparison.push_back(0x89);
	Mask.push_back(1);
	Comparison.push_back(0x35);
	Mask.push_back(1);
	
	result = true;
	return result;
}

long AddressFinder::getFlagsAddress()
{
	long flagsAddress = 0;
	
	getFlagsComp();
	getTibiaFileChunk(Comparison.size());
	flagsAddress = doComparison();
	
	return flagsAddress;
}
