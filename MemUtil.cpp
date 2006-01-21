// MemUtil.cpp: implementation of the CMemUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "MemUtil.h"

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
