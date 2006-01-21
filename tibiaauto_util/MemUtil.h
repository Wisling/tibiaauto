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
	static int SetMemRange(int processId,DWORD memAddressStart,DWORD memAddressEnd,char *data);
	static int SetMemRange(DWORD memAddressStart,DWORD memAddressEnd,char *data);
	static long int GetMemIntValue(DWORD memAddress);
	static int GetMemIntValue(long processId,DWORD memAddress,long int *value);
	static int GetMemIntValue(long processId, DWORD memAddressStart, DWORD memAddressEnd, long int *result);
	static int SetMemIntValue(long processId, DWORD memAddress, long int value);
	static int SetMemIntValue(DWORD memAddress, long int value);
	static int SetMemByteValue(long processId, DWORD memAddress, char value);
	static BOOL AdjustPrivileges();
	static void GetMemRange(DWORD memAddressStart,DWORD memAddressEnd,char *ret);
	static int GetMemRange(long processId, DWORD memAddressStart, DWORD memAddressEnd, char *result);
	
	CMemUtil();
	virtual ~CMemUtil();


	static void setGlobalProcessId(int procId) {m_globalProcessId=procId;};

	static long m_globalProcessId;
private:
	static HANDLE m_prevProcessHandle;
	static long m_prevProcessId;


};

#endif // !defined(AFX_MEMUTIL_H__590B7536_2457_414C_8296_16D5CD2F9AF6__INCLUDED_)
