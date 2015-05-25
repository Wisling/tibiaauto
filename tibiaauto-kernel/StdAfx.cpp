// stdafx.cpp : source file that includes just the standard includes
//	tibiaauto.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "time.h"

void ipcMessage::send(HANDLE pipeHandle)
{
	this->tm = time(NULL);
	DWORD cbWritten;
	BOOL fSuccess = WriteFile(
	        pipeHandle,
	        this,
	        sizeof(struct ipcMessage),
	        &cbWritten,
	        NULL);
}
