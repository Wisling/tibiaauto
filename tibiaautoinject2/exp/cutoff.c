
   
//#define INCLUDE_PROXY_CLASSES
//#include "NewIDirectDrawSurface4.cpp"
//#include "NewIDirectDraw4.cpp"
//#include "NewIDirectDraw.cpp"

 /*
extern "C" 
{

	typedef HRESULT (WINAPI *RealDirectDrawCreate)(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);


	HRESULT WINAPI DirectDrawCreate(
		GUID FAR *lpGUID, 
		LPDIRECTDRAW FAR *lplpDD, 
		IUnknown FAR *pUnkOuter)
	{


		RealDirectDrawCreate pDirectDrawCreate;

		HMODULE hddraw=NULL;
		char szFileName[512];
		UINT ret;
			
		ret = GetSystemDirectory(szFileName, 256);
		if(ret <= 0)
		{
			return FALSE;
		}
		
		strcat(szFileName, "\\ddraw.dll");
		hddraw = LoadLibrary(szFileName);
		if(hddraw == NULL)
		{
			return FALSE;
		}
		
		pDirectDrawCreate = (RealDirectDrawCreate)GetProcAddress(hddraw, "DirectDrawCreate");
		
		if(pDirectDrawCreate == NULL)
		{
			return FALSE;
		} 
		IDirectDraw *realIDirectDraw;
		pDirectDrawCreate(lpGUID,&realIDirectDraw,pUnkOuter);		
		*lplpDD = new NewIDirectDraw(realIDirectDraw,"IDirectDraw");		
		
		return FALSE;
		
	}
} // extern "C"

*/

extern "C" 
{

	typedef PVOID (WINAPI *RealAllocMemEx)(DWORD dwSize, HANDLE hProcess);
	typedef BOOL (WINAPI *RealFreeMemEx)(PVOID  pMem, HANDLE hProcess);
	typedef BOOL (WINAPI *RealHookAPI)(LPCSTR pszModule,LPCSTR pszFuncName,PVOID  pCallbackFunc,PVOID  *pNextHook,DWORD  dwFlags);
	typedef HANDLE (WINAPI *RealCreateRemoteThreadEx)(HANDLE hProcess,LPSECURITY_ATTRIBUTES  lpThreadAttributes,DWORD dwStackSize,LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter,DWORD dwCreationFlags,LPDWORD lpThreadId);
	
	HANDLE WINAPI CreateRemoteThreadEx(
		HANDLE                 hProcess,
		LPSECURITY_ATTRIBUTES  lpThreadAttributes,
		DWORD                  dwStackSize,
		LPTHREAD_START_ROUTINE lpStartAddress,
		LPVOID                 lpParameter,
		DWORD                  dwCreationFlags,
		LPDWORD                lpThreadId
		)
	{
		MessageBox(NULL,"CreateRemoteThreadEx","",0);
		RealCreateRemoteThreadEx fun;
		fun = (RealCreateRemoteThreadEx)GetProcAddress(LoadLibrary("madCHookOrig.dll"), "CreateRemoteThreadEx");		
		return fun(hProcess,lpThreadAttributes,dwStackSize,lpStartAddress,lpParameter,dwCreationFlags,lpThreadId);				
	}
	
	
	BOOL WINAPI HookAPI(
		LPCSTR pszModule,
		LPCSTR pszFuncName,
		PVOID  pCallbackFunc,
		PVOID  *pNextHook,  
		DWORD  dwFlags  
		)
	{
		char b[1024];
		sprintf(b,"HookApi: %s/%s",pszModule,pszFuncName);
		MessageBox(NULL,b,"",0);
		RealHookAPI fun;
		fun = (RealHookAPI)GetProcAddress(LoadLibrary("madCHookOrig.dll"), "HookAPI");		
		return fun(pszModule,pszFuncName,pCallbackFunc,pNextHook,dwFlags);
	}
	
		

	PVOID WINAPI AllocMemEx(
		DWORD  dwSize,  
		HANDLE hProcess  
		)
	{
		MessageBox(NULL,"AllocMemEx","",0);
		RealAllocMemEx fun;
		fun = (RealAllocMemEx)GetProcAddress(LoadLibrary("madCHookOrig.dll"), "AllocMemEx");		
		return fun(dwSize,hProcess);				
	}
	BOOL WINAPI FreeMemEx(
		PVOID  pMem,  
		HANDLE hProcess
		
		)
	{
		MessageBox(NULL,"FreeMemEx","",0);
		RealFreeMemEx fun;
		fun = (RealFreeMemEx)GetProcAddress(LoadLibrary("madCHookOrig.dll"), "FreeMemEx");		
		return fun(pMem,hProcess);				
	}

	BOOL WINAPI HookCode(
		PVOID  pCode,
		PVOID  pCallbackFunc,
		PVOID  *pNextHook,
		DWORD  dwFlags
		)
	{
		MessageBox(NULL,"HookCode","",0);
		return 0;
	}
	
	
} // extern "C"
