/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/


#include "stdafx.h"
#include "IPCBackPipeProxy.h"

//////////////////////////////////////////////////////////////////////

HMODULE CIPCBackPipeProxy::dllModule=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCBackPipeProxy::CIPCBackPipeProxy()
{
	// load module	
	if (dllModule==NULL)	
	{
		dllModule=LoadLibrary("mods\\tibiaauto_util.dll");
	}
}

CIPCBackPipeProxy::~CIPCBackPipeProxy()
{
	
}

int CIPCBackPipeProxy::readFromPipe(struct ipcMessage *mess,int expectedType)
{
	typedef int (*Proto_fun)(struct ipcMessage *mess,int expectedType);
	if (dllModule)
	{			
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"ipcBackPipeReadFromPipe");
		if (fun)
		{
			return fun(mess,expectedType);
		}
	} 
	return 0;
}


void CIPCBackPipeProxy::InitialiseIPC()
{
	typedef void (*Proto_fun)();
	if (dllModule)
	{			
		Proto_fun fun=(Proto_fun)GetProcAddress(dllModule,"ipcBackPipeInitialiseIPC");
		if (fun)
		{
			fun();
		}
	} 
}

