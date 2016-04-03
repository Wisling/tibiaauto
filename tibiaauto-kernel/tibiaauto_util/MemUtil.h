#pragma once

#include "tibiaauto_util.h"

#include <map>
using namespace std;

#define MEMORY_CACHE_VALID_TIME 2 // Milliseconds
#define MEMORY_CACHE_ENTRY_SIZE 32 // Bytes, must be a power of 2
struct MemoryCacheEntry
{
	LONGLONG expirationTime;
	char value[MEMORY_CACHE_ENTRY_SIZE];
};
typedef map<DWORD, MemoryCacheEntry> ReadMemoryCache;

class TIBIAAUTOUTIL_API CMemUtil
{
private:
	CMemUtil();
	virtual ~CMemUtil() {};
	CMemUtil(CMemUtil const&);
public:
	static CMemUtil& getMemUtil()
	{
		static CMemUtil singleton;
		return singleton;
	}

	int SetMemRange(int processId, DWORD memAddressStart, DWORD memAddressEnd, char *data, bool addBaseAddress);
	int SetMemRange(DWORD memAddressStart, DWORD memAddressEnd, char *data, bool addBaseAddress = true);
	long int GetMemIntValue(DWORD memAddress, bool addBaseAddress = true);
	int GetMemIntValue(long processId, DWORD memAddress, long int *value, bool addBaseAddress);
	int SetMemIntValue(long processId, DWORD memAddress, long int value, bool addBaseAddress);
	int SetMemIntValue(DWORD memAddress, long int value, bool addBaseAddress = true);
	int SetMemByteValue(long processId, DWORD memAddress, char value, bool addBaseAddress);
	BOOL AdjustPrivileges();
	void GetMemRange(DWORD memAddressStart, DWORD memAddressEnd, char *ret, bool addBaseAddress = true);
	int GetMemRange(long processId, DWORD memAddressStart, DWORD memAddressEnd, char *result, bool addBaseAddress);
	int GetProcessBaseAddr(int processId);

	void setGlobalProcessId(int procId)
	{
		m_globalProcessId = procId;
		m_globalBaseAddr  = CMemUtil::getMemUtil().GetProcessBaseAddr(procId);
	};
	int getGlobalProcessId()
	{
		return m_globalProcessId;
	};
	int getGlobalBaseAddr()
	{
		return m_globalBaseAddr;
	};

private:
	ReadMemoryCache m_memoryCache;
	LONGLONG m_cacheTimeoutTicks;
	HANDLE m_prevProcessHandle;
	long m_prevProcessId;
	long m_prevProcessIdBase;
	long m_prevBaseAddr;
	long m_globalBaseAddr;
	long m_globalProcessId;
	HANDLE gethandle(long processId);
	int readmemory(DWORD processId, DWORD memAddress, char* result, DWORD size, bool addBaseAddress, bool useCache = 1);
	int writememory(DWORD processId, DWORD memAddress, int* value, DWORD size, bool addBaseAddress);
};

