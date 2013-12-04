// IPCPipeBack.h: interface for the ipcMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCBACKPIPE1_H__054B62A3_8A94_4824_B73B_DD6C35175066__INCLUDED_)
#define AFX_CIPCBACKPIPE1_H__054B62A3_8A94_4824_B73B_DD6C35175066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ipcm.h"

static class CIPCPipeBack
{
public:
	ipcMessage mess;
	HANDLE hPipeBack;

public:
	CIPCPipeBack()
	{
		hPipeBack=INVALID_HANDLE_VALUE
	}
	virtual ~CIPCPipeBack();
	void send(ipcMessage mess)
	{
		mess.tm = time(NULL);
		DWORD cbWritten;
		BOOL fSuccess = WriteFile(
			hPipeBack,
			this,
			sizeof(struct ipcMessage),
			&cbWritten,
			NULL);
	}
	void InitialiseIPCback(pipepath,partnerProcessId)
	{
		char buf[1024];
		char lpszPipename[1024];
		sprintf(lpszPipename,pipepath,partnerProcessId);
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"[debug] IPC queue is %s\r\n",lpszPipename);
		}
		
		
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
			if (debugFile&&COMPLEX)
			{
				fprintf(debugFile,"[ipcback] Invalid pipe handle: %d\r\n",GetLastError());
				return;
			}
		}
		
		
		BOOL fConnected = ConnectNamedPipe(hPipeBack, NULL) ?  true : (GetLastError() == ERROR_PIPE_CONNECTED);
		
		
		if (!fConnected)
		{
			sprintf(buf,"client not connected via pipe: %d",GetLastError());
			if (debugFile&&COMPLEX)
			{
				fprintf(debugFile,"[ipcback] client not connected via pipe: %d\r\n",GetLastError());
				return;
			}
		}
		
		
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"[debug] back IPC initialised ok\r\n");
			fflush(debugFile);
		}
		
	}
};


#endif // !defined(AFX_CIPCBACKPIPE1_H__054B62A3_8A94_4824_B73B_DD6C35175066__INCLUDED_)
