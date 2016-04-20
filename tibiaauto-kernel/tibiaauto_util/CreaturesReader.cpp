// CreaturesReader.cpp: implementation of the CCreaturesReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "CreaturesReader.h"
#include "InstallPath.h"

#include "creatures.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

struct dbKey
{
private:
	unsigned short int x;
	unsigned short int y;
	unsigned char z;
	unsigned short int creatureNr;
public:
	dbKey(unsigned short int x, unsigned short int y, unsigned char z, unsigned short int creatureNr)
	{
		this->x          = ((x & 0xff) << 8) + (x >> 8);
		this->y          = ((y & 0xff) << 8) + (y >> 8);
		this->z          = z;
		this->creatureNr = creatureNr;
	}

	unsigned short int getX()
	{
		return ((x & 0xff) << 8) + (x >> 8);
	}

	unsigned short int getY()
	{
		return ((y & 0xff) << 8) + (y >> 8);
	}

	unsigned char getZ()
	{
		return z;
	}

	unsigned short int getCreatureNr()
	{
		return creatureNr;
	}
};

#pragma pack(pop)   /* restore original alignment from stack */


Db *CCreaturesReader::db = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCreaturesReader::CCreaturesReader()
{
	// open database (but for the first time only)
	// the database is ro and stays open forever
	if (db == NULL)
	{
		db = new Db(NULL, 0);
		u_int32_t oFlags = DB_RDONLY;
		char path[1024];
		sprintf(path, "%s\\ta_creatures_c.db", CInstallPath::getInstallPath().c_str());
		db->open(NULL, path, NULL, DB_BTREE, oFlags, 0);
	}
}

CCreaturesReader::~CCreaturesReader()
{
}

int CCreaturesReader::findCreatureStatForLocationCount(int x, int y, int z)
{
	Dbc *cursorp;
	db->cursor(NULL, &cursorp, 0);
	dbKey dbK(x, y, z, 0);

	Dbt key(&dbK, sizeof(dbKey));
	Dbt data(NULL, 0);
	int c    = 0;
	int flag = DB_SET_RANGE;
	while (!cursorp->get(&key, &data, flag))
	{
		memcpy(&dbK, key.get_data(), sizeof(dbKey));

		if (dbK.getX() == x && dbK.getY() == y && dbK.getZ() == z)
			c++;
		else
			break;
		flag = DB_NEXT;
	}

	cursorp->close();
	return c;
}

char * CCreaturesReader::findCreatureStatForLocationName(int x, int y, int z, int pos)
{
	Dbc *cursorp;
	db->cursor(NULL, &cursorp, 0);
	dbKey dbK(x, y, z, 0);

	Dbt key(&dbK, sizeof(dbKey));
	Dbt data(NULL, 0);
	int c    = 0;
	int flag = DB_SET_RANGE;
	while (!cursorp->get(&key, &data, flag))
	{
		memcpy(&dbK, key.get_data(), sizeof(dbKey));

		if (dbK.getX() == x && dbK.getY() == y && dbK.getZ() == z)
		{
			if (c == pos)
			{
				char *ret = (char *)malloc(64);
				sprintf(ret, "%s", dbCreatureTab[dbK.getCreatureNr()]);

				cursorp->close();
				return ret;
			}
			if (c > pos)
			{
				cursorp->close();
				return NULL;
			}
			flag = DB_NEXT;
			c++;
		}
	}
	return NULL;
}

int CCreaturesReader::findCreatureStatForLocationTibiaId(int x, int y, int z, int pos)
{
	Dbc *cursorp;
	db->cursor(NULL, &cursorp, 0);
	dbKey dbK(x, y, z, 0);

	Dbt key(&dbK, sizeof(dbKey));
	Dbt data(NULL, 0);
	int c    = 0;
	int flag = DB_SET_RANGE;
	while (!cursorp->get(&key, &data, flag))
	{
		memcpy(&dbK, key.get_data(), sizeof(dbKey));

		if (dbK.getX() == x && dbK.getY() == y && dbK.getZ() == z)
		{
			if (c == pos)
			{
				cursorp->close();
				return 1;
			}
			if (c > pos)
			{
				cursorp->close();
				return NULL;
			}
			flag = DB_NEXT;
			c++;
		}
	}
	return 0;
}

char ** CCreaturesReader::findCreatureStatInArea(int x, int y, int z, int rangeXY, int rangeZ)
{
	int retSize = 8;
	char **ret  = (char **)malloc(sizeof(char *) * retSize);

	int i;
	for (i = 0; i < retSize; i++)
	{
		ret[i] = NULL;
	}

	Dbc *cursorp;
	db->cursor(NULL, &cursorp, 0);
	dbKey dbK(x - rangeXY, y - rangeXY, z - rangeZ, 0);

	Dbt key(&dbK, sizeof(dbKey));
	Dbt data(&dbK, sizeof(dbKey));
	int flag = DB_SET_RANGE;

	while (!cursorp->get(&key, &data, flag))
	{
		memcpy(&dbK, key.get_data(), sizeof(dbKey));


		if (dbK.getX() >= x - rangeXY && dbK.getX() <= x + rangeXY &&
		    dbK.getY() >= y - rangeXY && dbK.getY() <= y + rangeXY &&
		    dbK.getZ() >= z - rangeZ && dbK.getZ() <= z + rangeZ)
			// point in range - add it
			ret = addCreatureToList(ret, dbCreatureTab[dbK.getCreatureNr()], &retSize);


		if (dbK.getX() > x + rangeXY)
			// finished looking for data
			break;
		flag = DB_NEXT;
	}

	cursorp->close();
	return ret;
}

char ** CCreaturesReader::addCreatureToList(char **list, char *name, int *size)
{
	int pos = 0;
	for (pos = 0; pos < *size && list[pos]; pos++)
	{
		if (!strcmp(list[pos], name))
			// already in the list; return;
			return list;
	}
	// resize check
	if (pos > *size - 3)
	{
		int i;
		(*size) = (*size) * 2;
		list    = (char **)realloc(list, sizeof(char *) * (*size));
		for (i = pos; i < *size; i++)
		{
			list[i] = NULL;
		}
	}
	// new entry
	list[pos] = (char *)malloc(64);
	strcpy(list[pos], name);
	return list;
}
