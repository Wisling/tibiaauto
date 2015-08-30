// IPCPipeBack.cpp: implementation of the CIPCPipeBack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPCPipeBack.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCPipeBack::CIPCPipeBack()
{
	hPipeBack = INVALID_HANDLE_VALUE;
}

CIPCPipeBack::~CIPCPipeBack()
{
}

void CIPCPipeBack::InitialiseIPCback(int partnerProcessId, FILE* debugFile, int COMPLEX)
{
	char buf[1024];
	char lpszPipename[1024];
	sprintf(lpszPipename, "\\\\.\\pipe\\tibiaAutoPipe-back-%d", partnerProcessId);
	if (debugFile && COMPLEX)
		fprintf(debugFile, "[debug] IPC queue is %s\r\n", lpszPipename);


	hPipeBack = CreateNamedPipe(
	        lpszPipename,             // pipe name
	        PIPE_ACCESS_DUPLEX,       // read/write access
	        PIPE_TYPE_MESSAGE |       // message type pipe
	        PIPE_READMODE_MESSAGE |   // message-read mode
	        PIPE_WAIT,                // blocking mode
	        PIPE_UNLIMITED_INSTANCES, // max. instances
	        163840,                  // output buffer size
	        163840,                  // input buffer size
	        1000,                        // client time-out
	        NULL);                    // no security attribute


	if (hPipeBack == INVALID_HANDLE_VALUE)
	{
		if (debugFile && COMPLEX)
		{
			fprintf(debugFile, "[ipcback] Invalid pipe handle: %d\r\n", GetLastError());
			return;
		}
	}


	BOOL fConnected = ConnectNamedPipe(hPipeBack, NULL) ?  true : (GetLastError() == ERROR_PIPE_CONNECTED);


	if (!fConnected)
	{
		sprintf(buf, "client not connected via pipe: %d", GetLastError());
		if (debugFile && COMPLEX)
		{
			fprintf(debugFile, "[ipcback] client not connected via pipe: %d\r\n", GetLastError());
			return;
		}
	}


	if (debugFile && COMPLEX)
	{
		fprintf(debugFile, "[debug] back IPC initialised ok\r\n");
		fflush(debugFile);
	}
}

int CIPCPipeBack::hasInvalidHandle()
{
	return hPipeBack == INVALID_HANDLE_VALUE;
}

void CIPCPipeBack::closePipe()
{
	DisconnectNamedPipe(hPipeBack);
	hPipeBack = INVALID_HANDLE_VALUE;
}

void CIPCPipeBack::send(CIpcMessage& mess)
{
	CIpcMessage::hPipe = hPipeBack;
	mess.send();
}
