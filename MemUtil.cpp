// MemUtil.cpp: implementation of the CMemUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "MemUtil.h"
#include "psapi.h"
#include "Winternl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HANDLE CMemUtil::m_prevProcessHandle = NULL;
long CMemUtil::m_prevProcessId = -1L;
long CMemUtil::m_globalProcessId=-1L;
long CMemUtil::m_globalBaseAddress=-1L;

CMemUtil::CMemUtil()
{

}

CMemUtil::~CMemUtil()
{

}

BOOL CMemUtil::AdjustPrivileges()
{

	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
	LUID luid;
	

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		AfxMessageBox("ERROR: Unable to open process token");		
		ExitProcess(0);
		return 0;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		AfxMessageBox("ERROR: Unable to lookup debug privilege");
		CloseHandle(hToken);
		ExitProcess(0);
		return 0;
	}

	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges (hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize))
	{
		AfxMessageBox("ERROR: Unable to adjust token privileges");
		CloseHandle(hToken);
		ExitProcess(0);
		return 0;
	}

    CloseHandle(hToken);

	return 1;

}

long _GetModuleImageAddress(const CString& strPath)
{
  HANDLE hFile;
  HANDLE hMap;
  long ulImageBase = 0;

  if ((hFile = ::CreateFile((LPCTSTR)strPath,
      GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
      NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE) {
    return 0;
  }

  if ((hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL))
      == NULL) {
    ::CloseHandle(hFile);
    return 0;
  }

  PBYTE pData;
  
  //
  // Read the 1st 8192 bytes of the PE image
  //
  if ((pData = (PBYTE)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 8192)) == NULL) {
    ::CloseHandle(hMap);
    ::CloseHandle(hFile);
    return 0;
  }

  //
  // Find the PE/PE+ header
  //
  // IMAGE_NT_SIGNATURE (0x00004550) 'PE00'
  // IMAGE_FILE_MACHINE_I386 (0x014c)
  // IMAGE_FILE_MACHINE_AMD64 (0x8664)
  //
  PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pData;
  if (pDos->e_magic == IMAGE_DOS_SIGNATURE) {
#ifdef _WIN64
# define IMAGE_FILE_MACHINE_xxx IMAGE_FILE_MACHINE_AMD64
    PIMAGE_NT_HEADERS64 pHdr = (PIMAGE_NT_HEADERS64)
      (((PBYTE)(pDos)) + pDos->e_lfanew);
#else
# define IMAGE_FILE_MACHINE_xxx IMAGE_FILE_MACHINE_I386
    PIMAGE_NT_HEADERS32 pHdr = (PIMAGE_NT_HEADERS32)
      (((PBYTE)(pDos)) + pDos->e_lfanew);
#endif
    //
    // Sanity: Does the PE header start no further than
    // the start of page 1, and the 'PE' header matches,
    // and the 32bit/64bit header matches?
    //
    if (((PBYTE)pHdr - pData) <= 4096 && pHdr->Signature == IMAGE_NT_SIGNATURE && pHdr->FileHeader.Machine == IMAGE_FILE_MACHINE_xxx) {
      //
      // Snarf the default base load address
      //
      ulImageBase = pHdr->OptionalHeader.ImageBase;
    }
  } else {
  }

  if (!::UnmapViewOfFile(pData)) {
  }
  if (!::CloseHandle(hMap)) {
  }
  ::CloseHandle(hFile);

  return ulImageBase;
}
typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID PebBaseAddress;
    PVOID Reserved2[2];
    PVOID UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION,*PPROCESS_BASIC_INFORMATION;
typedef struct _data {
    ULONG dllBaseAdrr;
	char targetDir[256];
}data;
/*-----------------------------------------------------------------------------------------*/
//The interesting member in our case is PPEB_LDR_DATA LoaderData that
//contains information filled by the loader at startup, and then when
//happens a DLL load/unload.
typedef struct _PEB_LDR_DATA {
    ULONG Length;
    BOOLEAN Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
     
}PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _LSA_UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;

/*-----------------------------------------------------------------------------------------*/
//The PEB_LDR_DATA structure contains three LIST_ENTRY that are part of doubly
//linked lists gathering information on loaded DLL in the current process.
//InLoadOrderModuleList sorts modules in load order, InMemoryOrderModuleList
//in memory order, and InInitializationOrderModuleList keeps track of their
//load order since process start.
 
//These doubly linked list contains pointers to LDR_MODULE inside the parent
//structure for next and previous module.
typedef struct _LDR_MODULE {
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID BaseAddress;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    _LSA_UNICODE_STRING FullDllName;
    _LSA_UNICODE_STRING BaseDllName;
    ULONG Flags;
    SHORT LoadCount;
    SHORT TlsIndex;
    LIST_ENTRY HashTableEntry;
    ULONG TimeDateStamp;
     
}LDR_MODULE, *PLDR_MODULE;

/*-----------------------------------------------------------------------------------------*/
//The role of the PEB is to gather frequently accessed information for a
//process as follows. At address FS:0x30 (or 0x7FFDF000) stands the
//following members of the [PEB].
/* located at 0x7FFDF000 */
typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[1];
    PVOID Reserved3[2];
    PPEB_LDR_DATA Ldr;
    ULONG ProcessParameters;
    BYTE Reserved4[104];
    PVOID Reserved5[52];
    ULONG PostProcessInitRoutine;
    BYTE Reserved6[128];
    PVOID Reserved7[1];
    ULONG SessionId;
     
}PEB, *PPEB;

/* * UnicodeToAnsi converts the Unicode string pszW to an ANSI string
   * and returns the ANSI string through ppszA. Space for the
   * the converted string is allocated by UnicodeToAnsi.
   */
 
HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA){
    ULONG cbAnsi, cCharacters;
    DWORD dwError;
    // If input is null then just return the same.   
    if (pszW == NULL)   
    {
        *ppszA = NULL;
        return NOERROR;
         
    }
    cCharacters = wcslen(pszW)+1;
    // Determine number of bytes to be allocated for ANSI string. An   
    // ANSI string can have at most 2 bytes per character (for Double   
    // Byte Character Strings.)   
    cbAnsi = cCharacters*2;
     
    // Use of the OLE allocator is not required because the resultant   
    // ANSI  string will never be passed to another COM component. You   
    // can use your own allocator.   
     
    *ppszA = (LPSTR) CoTaskMemAlloc(cbAnsi);
    if (NULL == *ppszA)
        return E_OUTOFMEMORY;
    // Convert to ANSI.
    if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA, cbAnsi, NULL, NULL))
    {
        dwError = GetLastError();
        CoTaskMemFree(*ppszA);
        *ppszA = NULL;
        return HRESULT_FROM_WIN32(dwError);
         
    }
    return NOERROR;
     
}

DWORD GetDllBaseAddressViaPeb(char *szDll) {
	DWORD Ret = 0;
	ULONG ldr_addr;
	PPEB_LDR_DATA ldr_data;
	PLDR_MODULE LdMod;
	LPSTR psznameA;
	// The asm code is only for IA-32 architecture   
	__asm mov eax, fs:[0x30]  //get the PEB ADDR   
	__asm add eax, 0xc           
	__asm mov eax, [eax] // get LoaderData ADDR   
	__asm mov ldr_addr, eax   
	 
	__try   
	{
	    ldr_data = (PPEB_LDR_DATA)ldr_addr;
	    LdMod = (PLDR_MODULE)ldr_data->InLoadOrderModuleList.Flink;
	    // all dlls present        
	    while(LdMod->BaseAddress != 0)        
	    {
	        LdMod = (PLDR_MODULE)LdMod->InLoadOrderModuleList.Flink;
	        UnicodeToAnsi(LdMod->BaseDllName.Buffer, &psznameA);
	        //LdMod->FullDllName.Buffer --> path + name              
	        printf("%s 0x%x\n", psznameA, (PULONG)LdMod->BaseAddress);
	        if(!strcmp(psznameA, szDll))              
	        {
	            Ret = (DWORD)LdMod->BaseAddress;
	        }
	         
	    }
	     
	}
	__except(EXCEPTION_EXECUTE_HANDLER)     {
	    return (DWORD)false;
	     
	}
	return Ret;
	 
}

int CMemUtil::GetMemBaseAddress(long processId)
{	
	data dat;
	unsigned long pid=5528;
	char *mydll = "Tibia.exe";
	//char cpid [50];
	//char title [50];
	//sprintf(cpid, "pid: %d", pid);
	//sprintf(title, "dllbaviapeb");
	//MessageBox(0,cpid,title,0);
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, pid);
	dat.dllBaseAdrr = GetDllBaseAddressViaPeb(mydll);
	//dat.dllBaseAdrr = (DWORD)LoadLibrary(mydll);
	char message [50];
	char tilte [50];
	sprintf(message, "ntdll.dll BA 0x%x", dat.dllBaseAdrr);
	sprintf(tilte, "dllbaviapeb");
	MessageBox(0,message,tilte,0);
	GetModuleFileName(GetModuleHandle(NULL), dat.targetDir, 255);
	return 1;
	     
	//long aaa = _GetModuleImageAddress("E:\\Tibia\\Tibia\\Tibia.exe");
	long aaa = _GetModuleImageAddress("C:\\windows\\system32\\calc.exe");
    HANDLE dwHandle;
	MEMORY_BASIC_INFORMATION info;
    unsigned long int this_xp = 0;
            
	if (m_prevProcessId!=processId)
	{
		dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		if (dwHandle==NULL)
		{
			m_prevProcessId=-1;
			return 1;
		}
		m_prevProcessId=processId;
		CloseHandle(m_prevProcessHandle);
		m_prevProcessHandle=dwHandle;
	} else {
		dwHandle=m_prevProcessHandle;
	};
    
	HINSTANCE psapi2 = LoadLibrary("ntdll.dll");
	FARPROC myprocAddr2 = GetProcAddress(HMODULE(psapi2),"NtQueryInformationProcess");
	typedef BOOL (__stdcall *GETSTUFF2)( HANDLE ProcessHandle, int ProcessInformationClass, PVOID ProcessInformation, int ProcessInformationLength, int ReturnLength);
	GETSTUFF2 GetModuleInformation2=GETSTUFF2(myprocAddr2);

	PROCESS_BASIC_INFORMATION info3;
	GetModuleInformation2(dwHandle,0,&info3,20,0);

	HINSTANCE psapi = LoadLibrary("psapi.dll");
	FARPROC myprocAddr = GetProcAddress(HMODULE(psapi),"GetModuleInformation");
	typedef BOOL (__stdcall *GETSTUFF)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb);
	GETSTUFF GetModuleInformation=GETSTUFF(myprocAddr);

	int a = sizeof(MEMORY_BASIC_INFORMATION);
	MODULEINFO info2;
	GetModuleInformation(dwHandle,0,&info2,12);
	VirtualQueryEx(dwHandle, (void*)0x500000, &info, sizeof(MEMORY_BASIC_INFORMATION));
    if (VirtualQueryEx(dwHandle, NULL, &info, 28)) {		
		info.BaseAddress;
		return 0;
    }
    else {	
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		return err;
    }       	
}


int CMemUtil::GetMemIntValue(long processId, DWORD memAddress, long int *value)
{	
    HANDLE dwHandle;
    void *ptr;
    unsigned long int this_xp = 0;
            
	if (m_prevProcessId!=processId)
	{
		dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		if (dwHandle==NULL)
		{
			m_prevProcessId=-1;
			return 1;
		}
		m_prevProcessId=processId;
		m_prevProcessHandle=dwHandle;
	} else {
		dwHandle=m_prevProcessHandle;
	};
    
    ptr = (void *)memAddress;
    if (ReadProcessMemory(dwHandle, ptr, value,   sizeof(long int), NULL)) {		
        return 0;
    }
    else {	
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		return err;
    }       	
}

int CMemUtil::GetMemIntValue(long processId, DWORD memAddressStart, DWORD memAddressEnd, long int *result)
{	
    HANDLE dwHandle;
    void *ptr;
    unsigned long int this_xp = 0;
            
	if (m_prevProcessId!=processId)
	{
		dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		if (dwHandle==NULL)
		{
			m_prevProcessId=-1;
			return 1;
		}
		m_prevProcessId=processId;
		m_prevProcessHandle=dwHandle;
	} else {
		dwHandle=m_prevProcessHandle;
	};
    
    ptr = (void *)memAddressStart;
    if (ReadProcessMemory(dwHandle, ptr, result,   memAddressEnd-memAddressStart, NULL)) {		
        return 0;
    }
    else {	
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		return err;
    }       	
}

int CMemUtil::GetMemRange(long processId, DWORD memAddressStart, DWORD memAddressEnd, char *result)
{	
    HANDLE dwHandle;
    void *ptr;
    unsigned long int this_xp = 0;
            
	if (m_prevProcessId!=processId)
	{
		dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		if (dwHandle==NULL)
		{
			m_prevProcessId=-1;
			return 1;
		}
		m_prevProcessId=processId;
		m_prevProcessHandle=dwHandle;
	} else {
		dwHandle=m_prevProcessHandle;
	};
    
    ptr = (void *)memAddressStart;
    if (ReadProcessMemory(dwHandle, ptr, result,   memAddressEnd-memAddressStart, NULL)) {		
        return 0;
    }
    else {	
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		return err;
    }       	
}

void CMemUtil::GetMemRange(DWORD memAddressStart,DWORD memAddressEnd,char *ret)
{
	GetMemRange(m_globalProcessId,memAddressStart,memAddressEnd,ret);
};

long int CMemUtil::GetMemIntValue(DWORD memAddress)
{
	long int value;			
	int ret=CMemUtil::GetMemIntValue(m_globalProcessId,memAddress,&value);
	if (ret!=0)
	{
		//char buf[128];
		//sprintf(buf,"ERROR: read memory failed; error=%d",ret);		
		//AfxMessageBox(buf);
		ExitProcess(0);
		return 0;
	}
	return value;
};

int CMemUtil::SetMemIntValue(DWORD memAddress, long int value)
{
	return SetMemIntValue(m_globalProcessId,memAddress,value);
}

int CMemUtil::SetMemIntValue(long processId, DWORD memAddress, long int value)
{	
    HANDLE dwHandle;
    void *ptr;
    unsigned long int this_xp = 0;
            
	if (m_prevProcessId!=processId)
	{
		dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		if (dwHandle==NULL)
		{
			m_prevProcessId=-1;
			return GetLastError();
		}
		m_prevProcessId=processId;
		m_prevProcessHandle=dwHandle;
	} else {
		dwHandle=m_prevProcessHandle;
	};
    
    ptr = (void *)memAddress;
    if (WriteProcessMemory(dwHandle, ptr, &value,   sizeof(long int), NULL)) {		
        return 0;
    }
    else {	
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		return err;
    }       	
}

int CMemUtil::SetMemByteValue(long processId, DWORD memAddress, char value)
{	
    HANDLE dwHandle;
    void *ptr;
    unsigned long int this_xp = 0;
            
	if (m_prevProcessId!=processId)
	{
		dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		if (dwHandle==NULL)
		{
			m_prevProcessId=-1;
			return 1;
		}
		m_prevProcessId=processId;
		m_prevProcessHandle=dwHandle;
	} else {
		dwHandle=m_prevProcessHandle;
	};
    
    ptr = (void *)memAddress;
    if (WriteProcessMemory(dwHandle, ptr, &value,   sizeof(char), NULL)) {		
        return 0;
    }
    else {	
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		return err;
    }       	
}

int CMemUtil::SetMemRange(DWORD memAddressStart, DWORD memAddressEnd, char *data)
{
	return SetMemRange(m_globalProcessId,memAddressStart,memAddressEnd,data);
}

int CMemUtil::SetMemRange(int processId, DWORD memAddressStart, DWORD memAddressEnd, char *data)
{
	HANDLE dwHandle;
    void *ptr;
    unsigned long int this_xp = 0;
	DWORD bytesWritten;
	
	if (m_prevProcessId!=processId)
	{
		dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		if (dwHandle==NULL)
		{
			m_prevProcessId=-1;
			return 1;
		}
		m_prevProcessId=processId;
		m_prevProcessHandle=dwHandle;
	} else {
		dwHandle=m_prevProcessHandle;
	};
    
    ptr = (void *)memAddressStart;
    if (WriteProcessMemory(dwHandle, ptr, data,   memAddressEnd-memAddressStart, &bytesWritten)) {
        return 0;
    }
    else {	
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		return err;
    }     
}
