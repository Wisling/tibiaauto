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
		dllAlice=LoadLibrary("mods\\alice.dll");
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
		Proto_fun fun=(Proto_fun)GetProcAddress(dllAlice,"kernelRespond");
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
