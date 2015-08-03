#pragma once

#include "tibiaauto_util.h"

class TIBIAAUTOUTIL_API CIPCBackPipe
{
public:
	int readFromPipe(struct ipcMessage *mess, int expectedType);
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

