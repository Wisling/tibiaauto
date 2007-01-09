// CreaturesReader.cpp: implementation of the CCreaturesReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "CreaturesReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

struct dbKey 
{		
	int x;
	int y;
	int z;
	char creatureName[33];	
	dbKey(int x,int y,int z,char *creatureName)
	{
		this->x=x;
		this->y=y;
		this->z=z;
		memset(this->creatureName,0x00,33);
		strcpy(this->creatureName,creatureName);
	}
};

struct dbData
{
	int tm;
	int tibiaId;
	int x;
	int y;
	int z;	
	int r;	
	char creatureName[33];
	dbData(int tm,int tibiaId,int x,int y,int z,char *creatureName,int r)
	{
		this->tm=tm;
		this->tibiaId=tibiaId;
		this->x=x;
		this->y=y;
		this->z=z;
		memset(this->creatureName,0x00,33);
		strcpy(this->creatureName,creatureName);
		this->r=r;		
	}
};

#pragma pack(pop)   /* restore original alignment from stack */


Db *CCreaturesReader::db=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
  
CCreaturesReader::CCreaturesReader()
{
	// open database (but for the first time only)
	// the database is ro and stays open forever
	if (db==NULL)
	{
		char installPath[1024];
		unsigned long installPathLen=1023;
		installPath[0]='\0';
		HKEY hkey=NULL;
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
		{
			RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
			RegCloseKey(hkey);
		}
		if (!strlen(installPath))
		{
			AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
			exit(1);
		}
		db=new Db(NULL,0);
		u_int32_t oFlags = DB_RDONLY; 
		char path[1024];
		sprintf(path,"%s\\ta_creatures.db",installPath);
		db->open(NULL,path,NULL,DB_BTREE,oFlags,0);
	}
}

CCreaturesReader::~CCreaturesReader()
{
	
}

int CCreaturesReader::findCreatureStatForLocationCount(int x, int y, int z)
{
	Dbc *cursorp;
	db->cursor(NULL, &cursorp, 0);
	dbKey dbK(x,y,z,"");
	dbData dbD(0,0,0,0,0,"",0);

	Dbt key(&dbK, sizeof(dbKey));
    Dbt data(&dbD, sizeof(dbData));
	int c=0;
	int flag=DB_SET_RANGE;
	while (!cursorp->get(&key, &data, flag))
	{		
		
		memcpy(&dbD,data.get_data(),sizeof(dbData));		
		
		if (dbD.x==x&&dbD.y==y&&dbD.z==z)
		{
			c++;
		} else {
			break;
		}
		flag=DB_NEXT;

	}

	cursorp->close();
	return c;
}

char * CCreaturesReader::findCreatureStatForLocationName(int x, int y, int z, int pos)
{
	Dbc *cursorp;
	db->cursor(NULL, &cursorp, 0);
	dbKey dbK(x,y,z,"");
	dbData dbD(0,0,0,0,0,"",0);

	Dbt key(&dbK, sizeof(dbKey));
    Dbt data(&dbD, sizeof(dbData));
	int c=0;
	int flag=DB_SET_RANGE;
	while (!cursorp->get(&key, &data, flag))
	{		
		memcpy(&dbD,data.get_data(),sizeof(dbData));		

		if (dbD.x==x&&dbD.y==y&&dbD.z==z)
		{
			if (c==pos)
			{
				char *ret=(char *)malloc(64);
				strcpy(ret,dbD.creatureName);
				cursorp->close();
				return ret;
			}
			if (c>pos)
			{
				cursorp->close();
				return NULL;
			}
			flag=DB_NEXT;
			c++;
			
		}
	}
	return NULL;	
}

int CCreaturesReader::findCreatureStatForLocationTibiaId(int x, int y, int z, int pos)
{
	Dbc *cursorp;
	db->cursor(NULL, &cursorp, 0);
	dbKey dbK(x,y,z,"");
	dbData dbD(0,0,0,0,0,"",0);

	Dbt key(&dbK, sizeof(dbKey));
    Dbt data(&dbD, sizeof(dbData));
	int c=0;
	int flag=DB_SET_RANGE;
	while (!cursorp->get(&key, &data, flag))
	{		
		memcpy(&dbD,data.get_data(),sizeof(dbData));		

		if (dbD.x==x&&dbD.y==y&&dbD.z==z)
		{
			if (c==pos)
			{
				int ret=dbD.tibiaId;				
				cursorp->close();
				return ret;
			}
			if (c>pos)
			{
				cursorp->close();
				return NULL;
			}
			flag=DB_NEXT;
			c++;
		}
	}
	return 0;	
}



char ** CCreaturesReader::findCreatureStatInArea(int x, int y, int z, int rangeXY, int rangeZ)
{	
	int retSize=8;
	char **ret = (char **)malloc(sizeof(char *)*retSize);
	
	int i;
	for (i=0;i<retSize;i++)
	{
		ret[i]=NULL;
	}	
	
	Dbc *cursorp;
	db->cursor(NULL, &cursorp, 0);
	dbKey dbK(x-rangeXY,y-rangeXY,z-rangeZ,"");
	dbData dbD(0,0,0,0,0,"",0);

	Dbt key(&dbK, sizeof(dbKey));
    Dbt data(&dbD, sizeof(dbData));	
	int flag=DB_SET_RANGE;	
	int xx=0;
	while (!cursorp->get(&key, &data, flag))
	{		
		memcpy(&dbD,data.get_data(),sizeof(dbData));		

		if (dbD.x>=x-rangeXY&&dbD.x<=x+rangeXY&&
			dbD.y>=y-rangeXY&&dbD.y<=y+rangeXY&&
			dbD.z>=z-rangeZ&&dbD.z<=z+rangeZ)			
		{
			// point in range - add it
			ret=addCreatureToList(ret,dbD.creatureName,&retSize);			
												
		}

		if (dbD.x>x+rangeXY)
		{
			// finished looking for data			
			break;
		}
		flag=DB_NEXT;
	}
	cursorp->close();
	return ret;
}

char ** CCreaturesReader::addCreatureToList(char **list, char *name,int *size)
{
	int pos=0;
	for (pos=0;pos<*size&&list[pos];pos++)
	{
		if (!strcmp(list[pos],name))
		{
			// already in the list; return;
			return list;
		}
	}
	// resize check
	if (pos>*size-3) {
		int i;		
		(*size)=(*size)*2;
		list=(char **)realloc(list,sizeof(char *)*(*size));
		for (i=pos;i<*size;i++)
		{
			list[i]=NULL;
		}
	}
	// new entry
	list[pos]=(char *)malloc(64);
	strcpy(list[pos],name);
	return list;
}
