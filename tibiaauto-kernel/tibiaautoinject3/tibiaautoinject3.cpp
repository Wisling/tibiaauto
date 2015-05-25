#include <stdafx.h>
#include <cstdio>

static HHOOK hhookKeyb;
//keyboard hook functions
int count = 0;

typedef void (*Proto_callback)(int value);
volatile Proto_callback hookCallbackFun = NULL;
void setCallbackFun()
{
	int size = 16;

	HANDLE hMapFile;
	LPCTSTR pBuf;
	char mapFileBuf[1024];

	wsprintf(mapFileBuf, "Global\\tibiaauto-mapfile-%d", ::GetCurrentProcessId());

	hMapFile = OpenFileMapping(
	        FILE_MAP_ALL_ACCESS,          // read/write access
	        FALSE,                        // do not inherit the name
	        mapFileBuf);                      // name of mapping object

	if(hMapFile == NULL)
		return;

	pBuf = (LPTSTR) MapViewOfFile(hMapFile, // handle to map object
	                              FILE_MAP_ALL_ACCESS,  // read/write permission
	                              0, 0, size);
	if(pBuf == NULL)
		return;

	CopyMemory((PVOID)&hookCallbackFun, (PVOID)pBuf, sizeof(void *));

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode < 0)
		return CallNextHookEx(hhookKeyb, nCode,
		                      wParam, lParam);

	if((lParam & 0x80000000) == 0)
		return CallNextHookEx(hhookKeyb, nCode,
		                      wParam, lParam);
	if(nCode == HC_ACTION)
	{
		if(wParam == VK_PRIOR)  // Page Up
		{
			if(!hookCallbackFun)
				setCallbackFun();
			else
				hookCallbackFun(VK_PRIOR);
		}
		if(wParam == VK_NEXT)  // Page Down
		{
			if(!hookCallbackFun)
				setCallbackFun();
			else
				hookCallbackFun(VK_NEXT);
		}
		if(wParam == VK_PAUSE)  // Pause/Break
		{
			if(!hookCallbackFun)
				setCallbackFun();
			else
				hookCallbackFun(VK_PAUSE);
		}
	}

	return CallNextHookEx(hhookKeyb, nCode,
	                      wParam, lParam);
}
