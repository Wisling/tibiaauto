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

int CMemUtil::GetMemBaseAddress(long processId)
{	
	long aaa = _GetModuleImageAddress("E:\\Tibia\\Tibia\\Tibia.exe");
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
