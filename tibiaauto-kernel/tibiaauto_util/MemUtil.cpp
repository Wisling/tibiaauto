// MemUtil.cpp: implementation of the CMemUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemUtil.h"
#include "psapi.h"

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
long CMemUtil::m_prevProcessIdBase = -1L;
long CMemUtil::m_prevBaseAddr = NULL;
long CMemUtil::m_globalProcessId=-1L;
long CMemUtil::m_globalBaseAddr = NULL;

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
		PostQuitMessage(0);
		return 0;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		AfxMessageBox("ERROR: Unable to lookup debug privilege");
		CloseHandle(hToken);
		PostQuitMessage(0);
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
		PostQuitMessage(0);
		return 0;
	}

    CloseHandle(hToken);

	return 1;

}

HANDLE CMemUtil::gethandle(long processId)
{
	HANDLE dwHandle;

	if (m_prevProcessId!=processId)
	{
		dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		if (dwHandle==NULL)
		{
			m_prevProcessId=-1;
			return NULL;
		}
		m_prevProcessId=processId;
		m_prevProcessHandle=dwHandle;
	} else {
		dwHandle=m_prevProcessHandle;
	}
	return dwHandle;
}

int CMemUtil::readmemory(int processId, int memAddress, int* result, int size, int addBaseAddress){
	HANDLE dwHandle = gethandle(processId);
	if (dwHandle == NULL){
		return 1;
	}
    void *ptr;
	if(addBaseAddress){
		ptr=(void *)(memAddress-0x400000+GetProcessBaseAddr(processId));
	}else{
		ptr = (void *)memAddress;
	}
	DWORD bytesRead;
    if (ReadProcessMemory(dwHandle, ptr, result, size, &bytesRead)) {
        return 0;
    }
    else {
		if (::GetLastError()==ERROR_INVALID_HANDLE){
			//FILE *f=fopen("C:/out.txt","a+");
			//fprintf(f,"time %d old %d,",time(NULL),dwHandle);
			dwHandle = NULL;
			for(int iter = 1000;iter>0;iter--){
				dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_prevProcessId);
				if(dwHandle){
					char buf[111];
					sprintf(buf,"iter %d",iter);
					if(iter!=1000)
						MessageBox(NULL,buf,"",0);
					break;
				}
				Sleep(10);
			}

			//fprintf(f,"new %d\n",dwHandle);
			//fclose(f);
			m_prevProcessHandle=dwHandle;
			if (ReadProcessMemory(dwHandle, ptr, result, size, NULL)) {
				return 0;
			}
		}
		if (::GetLastError()==ERROR_NOACCESS){
			CloseHandle(dwHandle);
			dwHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_prevProcessId);
			m_prevProcessHandle=dwHandle;
			if (ReadProcessMemory(dwHandle, ptr, result, size, NULL)) {
				return 0;
			}
		}
		if (::GetLastError()==ERROR_PARTIAL_COPY){
			//Possibly Tibia has been killed
			//Test valid address; tareader.getMemIntValue(-1)
			void *ptrTest = (void*)( 0x0410000 + GetProcessBaseAddr(processId));
			int resultTest;
			if (!ReadProcessMemory(dwHandle, ptrTest, &resultTest, 4, NULL)){
				//try getting new handle
				dwHandle = NULL;
				for (int iter = 1000; iter > 0; iter--){
					dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_prevProcessId);
					if (dwHandle){
						char buf[111];
						sprintf(buf, "iter %d", iter);
						if (iter != 1000)
							MessageBox(NULL, buf, "", 0);
						break;
					}
					Sleep(10);
				}
				m_prevProcessHandle = dwHandle;
				if (ReadProcessMemory(dwHandle, ptr, result, size, NULL)) {
					return 0;
				} else{
					DWORD terminatedStatus = 9999;
					if (GetExitCodeProcess(dwHandle, &terminatedStatus)){
						if (terminatedStatus != STILL_ACTIVE){ //If Tibia is no longer active then close TA
							PostQuitMessage(0);
						}
					}
				}
			} else{
				//Tibia is still running but a bad memory address was given
				return 1;
			}


			//fprintf(f,"new %d\n",dwHandle);
			//fclose(f);
		}
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		PostQuitMessage(0);
		return err;
    }
}

int CMemUtil::writememory(int processId, int memAddress, int* value, int size, int addBaseAddress){
	HANDLE dwHandle = gethandle(processId);
	if(dwHandle==NULL)
		return 1;

    void *ptr;
	if(addBaseAddress){
		ptr=(void *)(memAddress-0x400000+GetProcessBaseAddr(processId));
	}else{
		ptr = (void *)memAddress;
	}
	DWORD bytesWritten;
    if (WriteProcessMemory(dwHandle, ptr, value, size, &bytesWritten)) {
        return 0;
    }
    else {
		if (::GetLastError()==ERROR_INVALID_HANDLE){
			//FILE *f=fopen("C:/out.txt","a+");
			//fprintf(f,"time %d old %d,",time(NULL),dwHandle);
			dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_prevProcessId);
			//fprintf(f,"new %d\n",dwHandle);
			//fclose(f);
			m_prevProcessHandle=dwHandle;
			if (WriteProcessMemory(dwHandle, ptr, value, size, NULL)) {
				return 0;
			}
		}
		if (::GetLastError()==ERROR_NOACCESS){
			dwHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_prevProcessId);
			m_prevProcessHandle=dwHandle;
			if (WriteProcessMemory(dwHandle, ptr, value, size, NULL)) {
				return 0;
			}
		}
		DWORD err = ::GetLastError();
		CloseHandle(dwHandle);
		m_prevProcessId=-1;
		return err;
    }
}

int CMemUtil::GetProcessBaseAddr(int processId)
{
	HANDLE dwHandle = gethandle(processId);
	if (processId == m_prevProcessIdBase && m_prevProcessIdBase != -1 && m_prevBaseAddr != NULL){
		return m_prevBaseAddr;
	}else{
		m_prevProcessIdBase = -1;
		m_prevBaseAddr = NULL;
	}

	int ret=0;
	int isNotFromNormalScan = 0;
	if (dwHandle){
		unsigned long moduleCount=0;
		EnumProcessModules(dwHandle,NULL,0,&moduleCount);
		moduleCount = moduleCount/sizeof(HMODULE);

		HMODULE *modules=(HMODULE*)calloc(moduleCount,sizeof(HMODULE));
		char moduleName[64];
		EnumProcessModules(dwHandle, modules, moduleCount*sizeof(HMODULE), &moduleCount);
		for (unsigned long i=0;i<moduleCount;i++){
			GetModuleBaseName(dwHandle,modules[i],moduleName,sizeof(moduleName));
			if(_strcmpi(moduleName,"Tibia.exe")==0){
				MODULEINFO moduleInfo;
				GetModuleInformation(dwHandle, modules[i], &moduleInfo, sizeof(moduleInfo));
				//isNotFromNormalScan=0; // commented to see if Tibia.exe in sometimes not first
				ret = (int)moduleInfo.lpBaseOfDll;
				break;
			}
			if(i==0){ // catches first module in case Tibia.exe does not exist
				MODULEINFO moduleInfo;
				GetModuleInformation(dwHandle, modules[i], &moduleInfo, sizeof(moduleInfo));
				isNotFromNormalScan=1;
				ret = (int)moduleInfo.lpBaseOfDll;
			}
		}
		free(modules); modules = NULL;
	}
	if(isNotFromNormalScan) AfxMessageBox("While finding base address, main module was no first or was not named \"Tibia.exe\".");
	if(ret){
		m_prevBaseAddr = ret;
		m_prevProcessIdBase = processId;
	}
	return ret;
}

int CMemUtil::GetMemIntValue(long processId, DWORD memAddress, long int *value, int addBaseAddress)
{
    return readmemory(processId,memAddress,(int*)value,sizeof(long int),addBaseAddress);
}

int CMemUtil::GetMemRange(long processId, DWORD memAddressStart, DWORD memAddressEnd, char *result, int addBaseAddress)
{
	return readmemory(processId,memAddressStart,(int*)result,memAddressEnd-memAddressStart,addBaseAddress);
}

void CMemUtil::GetMemRange(DWORD memAddressStart,DWORD memAddressEnd,char *ret, int addBaseAddress/*=1*/)
{
	GetMemRange(m_globalProcessId,memAddressStart,memAddressEnd,ret,addBaseAddress);
};

long int CMemUtil::GetMemIntValue(DWORD memAddress, int addBaseAddress/*=1*/)
{
	long int value;
	int ret=CMemUtil::GetMemIntValue(m_globalProcessId,memAddress,&value,addBaseAddress);
	if (ret!=0)
	{
		char buf[128];
		sprintf(buf,"ERROR: read memory failed; error=%d",ret);
		throw "Error reading Tibia memory.";//PostQuitMessage(0);
		return 0;
	}
	return value;
};

int CMemUtil::SetMemIntValue(DWORD memAddress, long int value, int addBaseAddress/*=1*/)
{
	return SetMemIntValue(m_globalProcessId,memAddress,value,addBaseAddress);
}

int CMemUtil::SetMemIntValue(long processId, DWORD memAddress, long int value, int addBaseAddress)
{
	return writememory(processId,memAddress,(int*)&value,sizeof(long int),addBaseAddress);
}

int CMemUtil::SetMemByteValue(long processId, DWORD memAddress, char value, int addBaseAddress)
{
	return writememory(processId,memAddress,(int*)&value,sizeof(char),addBaseAddress);
}

int CMemUtil::SetMemRange(DWORD memAddressStart, DWORD memAddressEnd, char *data, int addBaseAddress/*=1*/)
{
	return SetMemRange(m_globalProcessId,memAddressStart,memAddressEnd,data,addBaseAddress);
}

int CMemUtil::SetMemRange(int processId, DWORD memAddressStart, DWORD memAddressEnd, char *data, int addBaseAddress)
{
	return writememory(processId,memAddressStart,(int*)data,memAddressEnd-memAddressStart,addBaseAddress);
}
