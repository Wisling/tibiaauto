#include <stdafx.h>
#include <cstdio>

static HHOOK hhookKeyb;
//keyboard hook functions
int count = 0;

typedef void (*Proto_callback)(int value);
volatile Proto_callback hookCallbackFun = NULL;
HINSTANCE hinstDLL = NULL;

void setCallbackFun()
{
	hinstDLL = LoadLibrary((LPCTSTR) "tibiaautoinject2.dll");
	if (hinstDLL == NULL)
		return;
	hookCallbackFun = (Proto_callback)GetProcAddress(hinstDLL, "hookCallback");
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hhookKeyb, nCode,
		                      wParam, lParam);

	if ((lParam & 0x80000000) == 0)
		return CallNextHookEx(hhookKeyb, nCode,
		                      wParam, lParam);
	if (nCode == HC_ACTION)
	{
		if (wParam == VK_PRIOR || wParam == VK_NEXT || wParam == VK_PAUSE)  // Page Up, Page Down, Pause/Break
		{
			if (!hookCallbackFun)
				setCallbackFun();
			if (hookCallbackFun)
				hookCallbackFun((int)wParam);
		}
	}

	return CallNextHookEx(hhookKeyb, nCode,
	                      wParam, lParam);
}
