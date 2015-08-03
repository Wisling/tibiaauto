#pragma once

#include "tibiaauto_util.h"

//ugly hack-around to prevent conflict of always-included by mfc oledb.h with berkeley's db.h
#define DBTYPE ORACLE_DBTYPE
#undef DB_UNKNOWN
#include <berkeleydb/db_cxx.h>
#undef DBTYPE

class TIBIAAUTOUTIL_API CCreaturesReader
{
public:
	char ** findCreatureStatInArea(int x, int y, int z, int rangeXY, int rangeZ);
	int findCreatureStatForLocationTibiaId(int x, int y, int z, int pos);
	char * findCreatureStatForLocationName(int x, int y, int z, int pos);
	int findCreatureStatForLocationCount(int x, int y, int z);
	CCreaturesReader();
	virtual ~CCreaturesReader();
private:
	char ** addCreatureToList(char **list, char *name, int *size);
	static Db *db;
};