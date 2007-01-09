// CreaturesProxy.h: interface for the CCreaturesProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CreaturesProxy_H__168C6EFF_1AFF_4A36_B1D7_2271E77AB0EC__INCLUDED_)
#define AFX_CreaturesProxy_H__168C6EFF_1AFF_4A36_B1D7_2271E77AB0EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCreaturesReaderProxy  
{
public:
	int findCreatureStatForLocationTibiaId(int x,int y,int z,int pos);
	char * findCreatureStatForLocationName(int x,int y,int z,int pos);
	int findCreatureStatForLocationCount(int x,int y,int z);
	char ** findCreatureStatInArea(int x, int y, int z, int rangeXY, int rangeZ);
	CCreaturesReaderProxy();
	virtual ~CCreaturesReaderProxy();

private:
	HMODULE dllUtil;
};

#endif // !defined(AFX_CreaturesProxy_H__168C6EFF_1AFF_4A36_B1D7_2271E77AB0EC__INCLUDED_)
