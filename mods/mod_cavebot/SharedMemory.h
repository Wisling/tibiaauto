/********************************************************************************************
* MOD-NAME      : SharedMemory.h
* LONG-NAME     : 
*
* AUTHOR        : Martin Ziacek
* DEPARTMENT    : 
* TELEPHONE     : 
* CREATION-DATE : 09/05/2001 18:00:00
* SP-NO         : 
* FUNCTION      : 
* 
*********************************************************************************************/
#ifndef __MEMMAPFILE_H__
#define __MEMMAPFILE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SharedMemory.h : header file
//

#define DEF_SHARED_SIZE		128
#define VAR_NAME_LENGTH		40

//structure creates header for a variable
typedef struct _tagValueHeader {
	WCHAR wszValueName[VAR_NAME_LENGTH];		//name of the variable
	DWORD dwLength;								//variable size
} ValueHeader;

class CSharedMemory;

typedef void (*PINITMEMORY)(CSharedMemory *);

/////////////////////////////////////////////////////////////////////////////
// format of the shared memory
//DWORD		...		variables' count
//DWORD		...		memory size (what was passed to constructor, it means whole memory size
//ValueHeader[0]
//value 0
//ValueHeader[1]
//value 1
// .....
//ValueHeader[n]
//value n

/////////////////////////////////////////////////////////////////////////////
// CSharedMemory class
class CSharedMemory : public CObject
{
public:
	DECLARE_DYNAMIC(CSharedMemory)

	CSharedMemory(const TCHAR *szName, DWORD dwSize = DEF_SHARED_SIZE, PINITMEMORY InitMemoryProcedure = NULL,LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);
	~CSharedMemory();

	CString GetMemName(void);

	BOOL SetSdMem(SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor);
	BOOL GetSdMem(SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR pSecurityDescriptor, DWORD nLength, LPDWORD lpnLengthNeeded);
	BOOL SetSdSem(SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor);
	BOOL GetSdSem(SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR pSecurityDescriptor, DWORD nLength, LPDWORD lpnLengthNeeded);
	BOOL SetSa(LPSECURITY_ATTRIBUTES lpsaAttributes);

	BOOL AddValue(const TCHAR *szName, DWORD size, void *pDefaultData = NULL);
	BOOL AddDwordValue(const TCHAR *szName, DWORD dwDefault = 0);
	BOOL DeleteValue(const TCHAR *szName);

	BOOL ExistValue(const TCHAR *szName);
	BOOL IsCreated(void);

	DWORD GetVariablesCount(void);
	BOOL GetValueInfo(DWORD dwIndex, ValueHeader *pVarInfo);

	BOOL SetValue(const TCHAR *szName, void *bData, DWORD dwLength);
	BOOL GetValue(const TCHAR *szName, void *bData, LPDWORD dwLength);

	BOOL SetDwordValue(const TCHAR *szName, DWORD dwVal);
	DWORD GetDwordValue(const TCHAR *szName, DWORD dwDefVal = -1);

	void *GetPointer(const TCHAR *szName);

	DWORD WaitForMemChange(DWORD dwMilliseconds = INFINITE);
	DWORD WaitForValueChange(const TCHAR *szName, DWORD dwMilliseconds = INFINITE);
	DWORD WaitForMultipleValuesChanges(CStringArray &str, BOOL bWaitForAll = TRUE, DWORD dwMilliseconds = INFINITE);

	BOOL InterlockedIncrement(const TCHAR *szName, LPLONG plNewVal = NULL);
	BOOL InterlockedDecrement(const TCHAR *szName, LPLONG plNewVal = NULL);
	BOOL InterlockedExchange(const TCHAR *szTargetName, LONG lNewVal, LPLONG plPrevValue = NULL);
	BOOL InterlockedTestExchange(const TCHAR *szTargetName, LONG lOldValue, LONG lNewValue, LPLONG plPrevValue = NULL);
	BOOL InterlockedCompareExchange(const TCHAR *szTargetName, LONG lExchange, LONG lComperand, LPLONG plIntiVal = NULL);
	BOOL InterlockedExchangeAdd(const TCHAR *szTargetName, LONG lIncrement, LPLONG plIntiVal = NULL);

	BOOL Write(BYTE *pbData, DWORD dwLength, DWORD dwOffset = 0);
	BOOL Read(BYTE *pbData, DWORD dwLength, DWORD dwOffset = 0);

#ifdef _DEBUG
	virtual void AssertValid(void);
	virtual void Dump(CDumpContext & dc);
#endif

	DWORD GetLastError(void);
	DWORD GetMemSize(void);
	BOOL AmIFirst(void);

protected:
	void SetLastError(DWORD dwErrCode);
	BOOL CreateSa(void);
	BOOL FindValue(const WCHAR *wszName,LPBYTE *pData, ValueHeader **pTmp);
	WCHAR *AllocateUnicodeStr(const char *szStr);

	CString m_CsName;			//name of shared memory

	DWORD m_dwLastError;		//last error issued from system by method of class

	CSemaphore *m_pSync;		//synchronization of shared memory - across processes
	CSingleLock *m_pLock;		//is made by these two objects

	BOOL m_bFirst;				//TRUE, if this is first reference to sm

	BOOL m_bCreated;			//TRUE, if shared memory was created

	DWORD m_dwMemSize;			//size of allocated shared memory

	LPBYTE m_lpView;			//pointer to the begining of shared memory
	HANDLE m_hFileMap;			//handle of shared memory (mapped file)

	BOOL m_bSecPres;			//TRUE, if OS support security

	UCHAR m_SdEvent[SECURITY_DESCRIPTOR_MIN_LENGTH];
	SECURITY_ATTRIBUTES m_SecAtr;
	SID *m_pLogSid;
	SID *m_pSysSid;
	ACL *m_pNewAcl;
	SID *m_pEvrSid;
};

/////////////////////////////////////////////////////////////////////////////

#endif //__MEMMAPFILE_H__
