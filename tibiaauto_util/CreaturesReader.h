// CreaturesReader.h: interface for the CCreaturesReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CREATURESREADER_H__468CF54F_40B5_41C7_9328_D27B7BB20DCA__INCLUDED_)
#define AFX_CREATURESREADER_H__468CF54F_40B5_41C7_9328_D27B7BB20DCA__INCLUDED_

// error here means: download berkley db 
#include "db_cxx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCreaturesReader  
{
public:
	char ** findCreatureStatInArea(int x,int y,int z,int rangeXY,int rangeZ);
	int findCreatureStatForLocationTibiaId(int x,int y,int z,int pos);
	char * findCreatureStatForLocationName(int x,int y,int z,int pos);
	int findCreatureStatForLocationCount(int x,int y,int z);
	CCreaturesReader();
	virtual ~CCreaturesReader();
private:
	char ** addCreatureToList(char **list,char *name,int *size);
	static Db *db;

};

#endif // !defined(AFX_CREATURESREADER_H__468CF54F_40B5_41C7_9328_D27B7BB20DCA__INCLUDED_)
