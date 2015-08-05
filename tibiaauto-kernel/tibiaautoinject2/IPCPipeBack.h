#pragma once

#include <IpcBackPipe.h>
#include "time.h"
//#include <fstream>

class CIPCPipeBack
{
public:
	HANDLE hPipeBack;

public:
	CIPCPipeBack();
	virtual ~CIPCPipeBack();
	void InitialiseIPCback(int partnerProcessId, FILE* debugFile, int COMPLEX);
	int hasInvalidHandle();
	void closePipe();
	void send(CIpcMessage& mess);
};
