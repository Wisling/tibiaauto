#include "stdafx.h"
#include "IpcMessage.h"

HANDLE CIpcMessage::hPipe = INVALID_HANDLE_VALUE;

CIpcMessage::CIpcMessage()
{
	messageType = 0;
	memset(payload, 0, 1024);
	tm = 0;
}

void CIpcMessage::send()
{
	this->tm = time(NULL);
	DWORD cbWritten;
	BOOL fSuccess = WriteFile(
		hPipe,
		this,
		sizeof(CIpcMessage),
		&cbWritten,
		NULL);
}


