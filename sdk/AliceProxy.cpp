// AliceProxy.cpp: implementation of the CAliceProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AliceProxy.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HMODULE CAliceProxy::dllAlice=NULL;

CAliceProxy::CAliceProxy()
{	
	if (dllAlice==NULL)
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

		char pathBuf[2048];
		sprintf(pathBuf,"%s\\mods\\alice.dll",installPath);

		dllAlice=LoadLibrary(pathBuf);
	}
}

CAliceProxy::~CAliceProxy()
{

}

char * CAliceProxy::respond(char *text, char *id)
{
	if (dllAlice)
	{
		typedef char *(*Proto_fun)(char *text, char *id);	
		static Proto_fun fun=(Proto_fun)GetProcAddress(dllAlice,"kernelRespond");
		if (fun)
		{
			return fun(text,id);
		} else {
			return "Alice::respond not found";
		}	
	} else {
		return "Alice not loaded";
	}
}
