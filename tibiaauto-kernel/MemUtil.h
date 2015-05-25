// MemUtil.h: interface for the CMemUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMUTIL_H__590B7536_2457_414C_8296_16D5CD2F9AF6__INCLUDED_)
#define AFX_MEMUTIL_H__590B7536_2457_414C_8296_16D5CD2F9AF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMemUtil
{
public:
	static int SetMemRange(int processId, DWORD memAddressStart, DWORD memAddressEnd, char *data, int addBaseAddress);
	static int SetMemRange(DWORD memAddressStart, DWORD memAddressEnd, char *data, int addBaseAddress = 1);
	static long int GetMemIntValue(DWORD memAddress, int addBaseAddress = 1);
	static int GetMemIntValue(long processId, DWORD memAddress, long int *value, int addBaseAddress);
	static int SetMemIntValue(long processId, DWORD memAddress, long int value, int addBaseAddress);
	static int SetMemIntValue(DWORD memAddress, long int value, int addBaseAddress = 1);
	static int SetMemByteValue(long processId, DWORD memAddress, char value, int addBaseAddress);
	static BOOL AdjustPrivileges();
	static void GetMemRange(DWORD memAddressStart, DWORD memAddressEnd, char *ret, int addBaseAddress = 1);
	static int GetMemRange(long processId, DWORD memAddressStart, DWORD memAddressEnd, char *result, int addBaseAddress);
	static int GetProcessBaseAddr(int processId);
	CMemUtil();
	virtual ~CMemUtil();


	static void setGlobalProcessId(int procId)
	{
		m_globalProcessId = procId;
		m_globalBaseAddr  = CMemUtil::GetProcessBaseAddr(procId);
	};
	static int getGlobalProcessId()
	{
		return m_globalProcessId;
	};
	static int getGlobalBaseAddr()
	{
		return m_globalBaseAddr;
	};

	static long m_globalProcessId;
	static long m_globalBaseAddr;
private:
	static HANDLE m_prevProcessHandle;
	static long m_prevProcessId;
	static long m_prevProcessIdBase;
	static long m_prevBaseAddr;
	static HANDLE gethandle(long processId);
	static int readmemory(int processId, int memAddress, int* result, int size, int addBaseAddress);
	static int writememory(int processId, int memAddress, int* value, int size, int addBaseAddress);
};

#endif // !defined(AFX_MEMUTIL_H__590B7536_2457_414C_8296_16D5CD2F9AF6__INCLUDED_)
