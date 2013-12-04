// IPCBackPipe.h: interface for the CIPCBackPipe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCBACKPIPE_H__CB9257F9_5D0D_4DC8_A04F_6B9AF26445BE__INCLUDED_)
#define AFX_IPCBACKPIPE_H__CB9257F9_5D0D_4DC8_A04F_6B9AF26445BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCBackPipe
{
public:
	int readFromPipe(struct ipcMessage *mess,int expectedType);
	CIPCBackPipe();
	virtual ~CIPCBackPipe();

private:
	static HANDLE hPipeBack;
	static int initialised;
	static struct ipcMessage *pipeBackCache;
	static int pipeBackCacheSize;
	static int pipeBackCacheCount;
	static void enlargePipeBackCache();
public:
	void InitialiseIPC();
};

#endif // !defined(AFX_IPCBACKPIPE_H__CB9257F9_5D0D_4DC8_A04F_6B9AF26445BE__INCLUDED_)
