#pragma once

#include "tibiaauto_util.h"
#include "IpcMessage.h"

class TIBIAAUTOUTIL_API CIPCBackPipe
{
public:
	static int readFromPipe(CIpcMessage *mess, int expectedType);

private:
	CIPCBackPipe() {};
	~CIPCBackPipe() {};
	static HANDLE hPipeBack;
	static int initialised;
	static CIpcMessage *pipeBackCache;
	static int pipeBackCacheSize;
	static int pipeBackCacheCount;
	static void enlargePipeBackCache();
public:
	static void InitialiseIPC();
};

